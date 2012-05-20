/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "common/coroutines.h"
#include "common/algorithm.h"
#include "common/debug.h"
#include "common/hashmap.h"
#include "common/hash-str.h"
#include "common/system.h"
#include "common/textconsole.h"

namespace Common {

/** Helper null context instance */
CoroContext nullContext = NULL;

DECLARE_SINGLETON(CoroutineScheduler);


#ifdef COROUTINE_DEBUG
namespace {
static int s_coroCount = 0;

typedef Common::HashMap<Common::String, int> CoroHashMap;
static CoroHashMap *s_coroFuncs = 0;

static void changeCoroStats(const char *func, int change) {
	if (!s_coroFuncs)
		s_coroFuncs = new CoroHashMap();

	(*s_coroFuncs)[func] += change;
}

static void displayCoroStats() {
	debug("%d active coros", s_coroCount);

	// Loop over s_coroFuncs and print info about active coros
	if (!s_coroFuncs)
		return;
	for (CoroHashMap::const_iterator it = s_coroFuncs->begin();
		it != s_coroFuncs->end(); ++it) {
		if (it->_value != 0)
			debug("  %3d x %s", it->_value, it->_key.c_str());
	}
}

}
#endif

CoroBaseContext::CoroBaseContext(const char *func)
	: _line(0), _sleep(0), _subctx(0) {
#ifdef COROUTINE_DEBUG
	_funcName = func;
	changeCoroStats(_funcName, +1);
	s_coroCount++;
#endif
}

CoroBaseContext::~CoroBaseContext() {
#ifdef COROUTINE_DEBUG
	s_coroCount--;
	changeCoroStats(_funcName, -1);
	debug("Deleting coro in %s at %p (subctx %p)",
		_funcName, (void *)this, (void *)_subctx);
	displayCoroStats();
#endif
	delete _subctx;
}

//--------------------- Scheduler Class ------------------------

/**
 * Constructor
 */
CoroutineScheduler::CoroutineScheduler() {
	processList = NULL;
	pFreeProcesses = NULL;
	pCurrent = NULL;

#ifdef DEBUG
	// diagnostic process counters
	numProcs = 0;
	maxProcs = 0;
#endif

	pRCfunction = NULL;
	pidCounter = 0;

	active = new PROCESS;
	active->pPrevious = NULL;
	active->pNext = NULL;

	reset();
}

/**
 * Destructor
 */
CoroutineScheduler::~CoroutineScheduler() {
	// Kill all running processes (i.e. free memory allocated for their state).
	PROCESS *pProc = active->pNext;
	while (pProc != NULL) {
		delete pProc->state;
		pProc->state = 0;
		pProc = pProc->pNext;
	}

	free(processList);
	processList = NULL;

	delete active;
	active = 0;

	// Clear the event list
	Common::List<EVENT *>::iterator i;
	for (i = _events.begin(); i != _events.end(); ++i)
		delete (*i);
}

/**
 * Kills all processes and places them on the free list.
 */
void CoroutineScheduler::reset() {

#ifdef DEBUG
	// clear number of process in use
	numProcs = 0;
#endif

	if (processList == NULL) {
		// first time - allocate memory for process list
		processList = (PROCESS *)calloc(CORO_MAX_PROCESSES, sizeof(PROCESS));

		// make sure memory allocated
		if (processList == NULL) {
			error("Cannot allocate memory for process data");
		}

		// fill with garbage
		memset(processList, 'S', CORO_MAX_PROCESSES * sizeof(PROCESS));
	}

	// Kill all running processes (i.e. free memory allocated for their state).
	PROCESS *pProc = active->pNext;
	while (pProc != NULL) {
		delete pProc->state;
		pProc->state = 0;
		Common::fill(&pProc->pidWaiting[0], &pProc->pidWaiting[CORO_MAX_PID_WAITING], 0);
		pProc = pProc->pNext;
	}

	// no active processes
	pCurrent = active->pNext = NULL;

	// place first process on free list
	pFreeProcesses = processList;

	// link all other processes after first
	for (int i = 1; i <= CORO_NUM_PROCESS; i++) {
		processList[i - 1].pNext = (i == CORO_NUM_PROCESS) ? NULL : processList + i;
		processList[i - 1].pPrevious = (i == 1) ? active : processList + (i - 2);
	}
}


#ifdef	DEBUG
/**
 * Shows the maximum number of process used at once.
 */
void CoroutineScheduler::printStats() {
	debug("%i process of %i used", maxProcs, CORO_NUM_PROCESS);
}
#endif

#ifdef DEBUG
/**
 * Checks both the active and free process list to insure all the links are valid,
 * and that no processes have been lost
 */
void CoroutineScheduler::CheckStack() {
	Common::List<PROCESS *> pList;

	// Check both the active and free process lists
	for (int i = 0; i < 2; ++i) {
		PROCESS *p = (i == 0) ? active : pFreeProcesses;

		if (p != NULL) {
			// Make sure the linkages are correct
			while (p->pNext != NULL) {
				assert(p->pNext->pPrevious == p);
				pList.push_back(p);
				p = p->pNext;
			}
			pList.push_back(p);
		}
	}

	// Make sure all processes are accounted for
	for (int idx = 0; idx < CORO_NUM_PROCESS; idx++) {
		bool found = false;
		for (Common::List<PROCESS *>::iterator i = pList.begin(); i != pList.end(); ++i) {
			PROCESS *pTemp = *i;
			if (*i == &processList[idx]) {
				found = true;
				break;
			}
		}

		assert(found);
	}
}
#endif

/**
 * Give all active processes a chance to run
 */
void CoroutineScheduler::schedule() {
	// start dispatching active process list
	PROCESS *pNext;
	PROCESS *pProc = active->pNext;
	while (pProc != NULL) {
		pNext = pProc->pNext;

		if (--pProc->sleepTime <= 0) {
			// process is ready for dispatch, activate it
			pCurrent = pProc;
			pProc->coroAddr(pProc->state, pProc->param);

			if (!pProc->state || pProc->state->_sleep <= 0) {
				// Coroutine finished
				pCurrent = pCurrent->pPrevious;
				killProcess(pProc);
			} else {
				pProc->sleepTime = pProc->state->_sleep;
			}

			// pCurrent may have been changed
			pNext = pCurrent->pNext;
			pCurrent = NULL;
		}

		pProc = pNext;
	}
}

/**
 * Reschedules all the processes to run again this query
 */
void CoroutineScheduler::rescheduleAll() {
	assert(pCurrent);

	// Unlink current process
	pCurrent->pPrevious->pNext = pCurrent->pNext;
	if (pCurrent->pNext)
		pCurrent->pNext->pPrevious = pCurrent->pPrevious;

	// Add process to the start of the active list
	pCurrent->pNext = active->pNext;
	active->pNext->pPrevious = pCurrent;
	active->pNext = pCurrent;
	pCurrent->pPrevious = active;
}

/**
 * If the specified process has already run on this tick, make it run
 * again on the current tick.
 */
void CoroutineScheduler::reschedule(PPROCESS pReSchedProc) {
	// If not currently processing the schedule list, then no action is needed
	if (!pCurrent)
		return;

	if (!pReSchedProc)
		pReSchedProc = pCurrent;

	PPROCESS pEnd;

	// Find the last process in the list.
	// But if the target process is down the list from here, do nothing
	for (pEnd = pCurrent; pEnd->pNext != NULL; pEnd = pEnd->pNext) {
		if (pEnd->pNext == pReSchedProc)
			return;
	}

	assert(pEnd->pNext == NULL);

	// Could be in the middle of a KillProc()!
	// Dying process was last and this process was penultimate
	if (pReSchedProc->pNext == NULL)
		return;

	// If we're moving the current process, move it back by one, so that the next
	// schedule() iteration moves to the now next one
	if (pCurrent == pReSchedProc)
		pCurrent = pCurrent->pPrevious;

	// Unlink the process, and add it at the end
	pReSchedProc->pPrevious->pNext = pReSchedProc->pNext;
	pReSchedProc->pNext->pPrevious = pReSchedProc->pPrevious;
	pEnd->pNext = pReSchedProc;
	pReSchedProc->pPrevious = pEnd;
	pReSchedProc->pNext = NULL;
}

/**
 * Moves the specified process to the end of the dispatch queue
 * allowing it to run again within the current game cycle.
 * @param pGiveProc		Which process
 */
void CoroutineScheduler::giveWay(PPROCESS pReSchedProc) {
	// If not currently processing the schedule list, then no action is needed
	if (!pCurrent)
		return;

	if (!pReSchedProc)
		pReSchedProc = pCurrent;

	// If the process is already at the end of the queue, nothing has to be done
	if (!pReSchedProc->pNext)
		return;

	PPROCESS pEnd;

	// Find the last process in the list.
	for (pEnd = pCurrent; pEnd->pNext != NULL; pEnd = pEnd->pNext)
		;
	assert(pEnd->pNext == NULL);


	// If we're moving the current process, move it back by one, so that the next
	// schedule() iteration moves to the now next one
	if (pCurrent == pReSchedProc)
		pCurrent = pCurrent->pPrevious;

	// Unlink the process, and add it at the end
	pReSchedProc->pPrevious->pNext = pReSchedProc->pNext;
	pReSchedProc->pNext->pPrevious = pReSchedProc->pPrevious;
	pEnd->pNext = pReSchedProc;
	pReSchedProc->pPrevious = pEnd;
	pReSchedProc->pNext = NULL;
}

/**
 * Continously makes a given process wait for another process to finish or event to signal.
 *
 * @param pid			Process/Event identifier
 * @param duration		Duration in milliseconds
 * @param expired		If specified, set to true if delay period expired
 */
void CoroutineScheduler::waitForSingleObject(CORO_PARAM, int pid, uint32 duration, bool *expired) {
	if (!pCurrent)
		error("Called CoroutineScheduler::waitForSingleObject from the main process");

	CORO_BEGIN_CONTEXT;
		uint32 endTime;
		PROCESS *pProcess;
		EVENT *pEvent;
	CORO_END_CONTEXT(_ctx);

	CORO_BEGIN_CODE(_ctx);

	// Signal the process Id this process is now waiting for
	pCurrent->pidWaiting[0] = pid;

	_ctx->endTime = (duration == CORO_INFINITE) ? CORO_INFINITE : g_system->getMillis() + duration;
	if (expired)
		// Presume it will expire
		*expired = true;

	// Outer loop for doing checks until expiry 
	while (g_system->getMillis() < _ctx->endTime) {
		// Check to see if a process or event with the given Id exists
		_ctx->pProcess = getProcess(pid);
		_ctx->pEvent = !_ctx->pProcess ? getEvent(pid) : NULL;

		// If there's no active process or event, presume it's a process that's finished,
		// so the waiting can immediately exit
		if ((_ctx->pProcess == NULL) && (_ctx->pEvent == NULL)) {
			if (expired)
				*expired = false;
			break;
		}

		// If a process was found, don't go into the if statement, and keep waiting. 
		// Likewise if it's an event that's not yet signalled
		if ((_ctx->pEvent != NULL) && _ctx->pEvent->signalled) {
			// Unless the event is flagged for manual reset, reset it now
			if (!_ctx->pEvent->manualReset)
				_ctx->pEvent->signalled = false;

			if (expired)
				*expired = false;
			break;
		}

		// Sleep until the next cycle
		CORO_SLEEP(1);
	}

	// Signal waiting is done
	Common::fill(&pCurrent->pidWaiting[0], &pCurrent->pidWaiting[CORO_MAX_PID_WAITING], 0);

	CORO_END_CODE;
}

/**
 * Continously makes a given process wait for given prcesses to finished or events to be set
 *
 * @param nCount		Number of Id's being passed
 * @param evtList		List of pids to wait for
 * @param bWaitAll		Specifies whether all or any of the processes/events 
 * @param duration		Duration in milliseconds
 * @param expired		Set to true if delay period expired
 */
void CoroutineScheduler::waitForMultipleObjects(CORO_PARAM, int nCount, uint32 *pidList, bool bWaitAll, 
						   uint32 duration, bool *expired) {
	if (!pCurrent)
		error("Called CoroutineScheduler::waitForMultipleEvents from the main process");

	CORO_BEGIN_CONTEXT;
		uint32 endTime;
		bool signalled;
		bool pidSignalled;
		int i;
		PROCESS *pProcess;
		EVENT *pEvent;
	CORO_END_CONTEXT(_ctx);

	CORO_BEGIN_CODE(_ctx);

	// Signal the waiting events
	assert(nCount < CORO_MAX_PID_WAITING);
	Common::copy(pidList, pidList + nCount, pCurrent->pidWaiting);

	_ctx->endTime = (duration == CORO_INFINITE) ? CORO_INFINITE : g_system->getMillis() + duration;
	if (expired)
		// Presume that delay will expire
		*expired = true;

	// Outer loop for doing checks until expiry 
	while (g_system->getMillis() < _ctx->endTime) {
		_ctx->signalled = bWaitAll;

		for (_ctx->i = 0; _ctx->i < nCount; ++_ctx->i) {
			_ctx->pProcess = getProcess(pidList[_ctx->i]);
			_ctx->pEvent = !_ctx->pProcess ? getEvent(pidList[_ctx->i]) : NULL;

			// Determine the signalled state
			_ctx->pidSignalled = (_ctx->pProcess) || !_ctx->pEvent ? false : _ctx->pEvent->signalled;

			if (bWaitAll && _ctx->pidSignalled)
				_ctx->signalled = false;
			else if (!bWaitAll & _ctx->pidSignalled)
				_ctx->signalled = true;
		}

		// At this point, if the signalled variable is set, waiting is finished
		if (_ctx->signalled) {
			// Automatically reset any events not flagged for manual reset
			for (_ctx->i = 0; _ctx->i < nCount; ++_ctx->i) {
				_ctx->pEvent = getEvent(pidList[_ctx->i]);

				if (_ctx->pEvent->manualReset)
					_ctx->pEvent->signalled = false;
			}

			if (expired)
				*expired = false;
			break;
		}

		// Sleep until the next cycle
		CORO_SLEEP(1);
	}

	// Signal waiting is done
	Common::fill(&pCurrent->pidWaiting[0], &pCurrent->pidWaiting[CORO_MAX_PID_WAITING], 0);

	CORO_END_CODE;
}

/**
 * Make the active process sleep for the given duration in milliseconds
 * @param duration					Duration in milliseconds
 * @remarks		This duration won't be precise, since it relies on the frequency the
 * scheduler is called.
 */
void CoroutineScheduler::sleep(CORO_PARAM, uint32 duration) {
	if (!pCurrent)
		error("Called CoroutineScheduler::waitForSingleObject from the main process");

	CORO_BEGIN_CONTEXT;
		uint32 endTime;
		PROCESS *pProcess;
		EVENT *pEvent;
	CORO_END_CONTEXT(_ctx);

	CORO_BEGIN_CODE(_ctx);

	_ctx->endTime = g_system->getMillis() + duration;

	// Outer loop for doing checks until expiry 
	while (g_system->getMillis() < _ctx->endTime) {
		// Sleep until the next cycle
		CORO_SLEEP(1);
	}

	CORO_END_CODE;
}

/**
 * Creates a new process.
 *
 * @param pid	process identifier
 * @param CORO_ADDR	coroutine start address
 * @param pParam	process specific info
 * @param sizeParam	size of process specific info
 */
PROCESS *CoroutineScheduler::createProcess(uint32 pid, CORO_ADDR coroAddr, const void *pParam, int sizeParam) {
	PROCESS *pProc;

	// get a free process
	pProc = pFreeProcesses;

	// trap no free process
	assert(pProc != NULL); // Out of processes

#ifdef DEBUG
	// one more process in use
	if (++numProcs > maxProcs)
		maxProcs = numProcs;
#endif

	// get link to next free process
	pFreeProcesses = pProc->pNext;
	if (pFreeProcesses)
		pFreeProcesses->pPrevious = NULL;

	if (pCurrent != NULL) {
		// place new process before the next active process
		pProc->pNext = pCurrent->pNext;
		if (pProc->pNext)
			pProc->pNext->pPrevious = pProc;

		// make this new process the next active process
		pCurrent->pNext = pProc;
		pProc->pPrevious = pCurrent;

	} else {	// no active processes, place process at head of list
		pProc->pNext = active->pNext;
		pProc->pPrevious = active;

		if (pProc->pNext)
			pProc->pNext->pPrevious = pProc;
		active->pNext = pProc;

	}

	// set coroutine entry point
	pProc->coroAddr = coroAddr;

	// clear coroutine state
	pProc->state = 0;

	// wake process up as soon as possible
	pProc->sleepTime = 1;

	// set new process id
	pProc->pid = pid;

	// set new process specific info
	if (sizeParam) {
		assert(sizeParam > 0 && sizeParam <= CORO_PARAM_SIZE);

		// set new process specific info
		memcpy(pProc->param, pParam, sizeParam);
	}

	// return created process
	return pProc;
}

/**
 * Creates a new process with an auto-incrementing Process Id.
 *
 * @param CORO_ADDR	coroutine start address
 * @param pParam	process specific info
 * @param sizeParam	size of process specific info
 */
uint32 CoroutineScheduler::createProcess(CORO_ADDR coroAddr, const void *pParam, int sizeParam) {
	PROCESS *pProc = createProcess(++pidCounter, coroAddr, pParam, sizeParam);
	return pProc->pid;
}

/**
 * Creates a new process with an auto-incrementing Process Id, and a single pointer parameter.
 *
 * @param CORO_ADDR	coroutine start address
 * @param pParam	process specific info
 * @param sizeParam	size of process specific info
 */
uint32 CoroutineScheduler::createProcess(CORO_ADDR coroAddr, const void *pParam) {
	return createProcess(coroAddr, &pParam, sizeof(void *));
}


/**
 * Kills the specified process.
 *
 * @param pKillProc	which process to kill
 */
void CoroutineScheduler::killProcess(PROCESS *pKillProc) {
	// make sure a valid process pointer
	assert(pKillProc >= processList && pKillProc <= processList + CORO_NUM_PROCESS - 1);

	// can not kill the current process using killProcess !
	assert(pCurrent != pKillProc);

#ifdef DEBUG
	// one less process in use
	--numProcs;
	assert(numProcs >= 0);
#endif

	// Free process' resources
	if (pRCfunction != NULL)
		(pRCfunction)(pKillProc);

	delete pKillProc->state;
	pKillProc->state = 0;

	// Take the process out of the active chain list
	pKillProc->pPrevious->pNext = pKillProc->pNext;
	if (pKillProc->pNext)
		pKillProc->pNext->pPrevious = pKillProc->pPrevious;

	// link first free process after pProc
	pKillProc->pNext = pFreeProcesses;
	if (pFreeProcesses)
		pKillProc->pNext->pPrevious = pKillProc;
	pKillProc->pPrevious = NULL;

	// make pKillProc the first free process
	pFreeProcesses = pKillProc;
}



/**
 * Returns a pointer to the currently running process.
 */
PROCESS *CoroutineScheduler::getCurrentProcess() {
	return pCurrent;
}

/**
 * Returns the process identifier of the specified process.
 *
 * @param pProc	which process
 */
int CoroutineScheduler::getCurrentPID() const {
	PROCESS *pProc = pCurrent;

	// make sure a valid process pointer
	assert(pProc >= processList && pProc <= processList + CORO_NUM_PROCESS - 1);

	// return processes PID
	return pProc->pid;
}

/**
 * Kills any process matching the specified PID. The current
 * process cannot be killed.
 *
 * @param pidKill	process identifier of process to kill
 * @param pidMask	mask to apply to process identifiers before comparison
 * @return The number of processes killed is returned.
 */
int CoroutineScheduler::killMatchingProcess(uint32 pidKill, int pidMask) {
	int numKilled = 0;
	PROCESS *pProc, *pPrev;	// process list pointers

	for (pProc = active->pNext, pPrev = active; pProc != NULL; pPrev = pProc, pProc = pProc->pNext) {
		if ((pProc->pid & (uint32)pidMask) == pidKill) {
			// found a matching process

			// dont kill the current process
			if (pProc != pCurrent) {
				// kill this process
				numKilled++;

				// Free the process' resources
				if (pRCfunction != NULL)
					(pRCfunction)(pProc);

				delete pProc->state;
				pProc->state = 0;

				// make prev point to next to unlink pProc
				pPrev->pNext = pProc->pNext;
				if (pProc->pNext)
					pPrev->pNext->pPrevious = pPrev;

				// link first free process after pProc
				pProc->pNext = pFreeProcesses;
				pProc->pPrevious = NULL;
				pFreeProcesses->pPrevious = pProc;

				// make pProc the first free process
				pFreeProcesses = pProc;

				// set to a process on the active list
				pProc = pPrev;
			}
		}
	}

#ifdef DEBUG
	// adjust process in use
	numProcs -= numKilled;
	assert(numProcs >= 0);
#endif

	// return number of processes killed
	return numKilled;
}

/**
 * Set pointer to a function to be called by killProcess().
 *
 * May be called by a resource allocator, the function supplied is
 * called by killProcess() to allow the resource allocator to free
 * resources allocated to the dying process.
 *
 * @param pFunc	Function to be called by killProcess()
 */
void CoroutineScheduler::setResourceCallback(VFPTRPP pFunc) {
	pRCfunction = pFunc;
}

PROCESS *CoroutineScheduler::getProcess(uint32 pid) {
	PROCESS *pProc = active->pNext;
	while ((pProc != NULL) && (pProc->pid != pid))
		pProc = pProc->pNext;

	return pProc;
}

EVENT *CoroutineScheduler::getEvent(uint32 pid) {
	Common::List<EVENT *>::iterator i;
	for (i = _events.begin(); i != _events.end(); ++i) {
		EVENT *evt = *i;
		if (evt->pid == pid)
			return evt;
	}

	return NULL;
}


/**
 * Creates a new event object
 * @param bManualReset					Events needs to be manually reset. Otherwise, events
 * will be automatically reset after a process waits on the event finishes
 * @param bInitialState					Specifies whether the event is signalled or not initially
 */
uint32 CoroutineScheduler::createEvent(bool bManualReset, bool bInitialState) {
	EVENT *evt = new EVENT();
	evt->pid = ++pidCounter;
	evt->manualReset = bManualReset;
	evt->signalled = bInitialState;

	_events.push_back(evt);
	return evt->pid;
}

/**
 * Destroys the given event
 * @param pidEvent						Event PID
 */
void CoroutineScheduler::closeEvent(uint32 pidEvent) {
	EVENT *evt = getEvent(pidEvent);
	if (evt) {
		_events.remove(evt);
		delete evt;
	}
}

/**
 * Sets the event
 * @param pidEvent						Event PID
 */
void CoroutineScheduler::setEvent(uint32 pidEvent) {
	EVENT *evt = getEvent(pidEvent);
	if (evt)
		evt->signalled = true;
}

/**
 * Resets the event
 * @param pidEvent						Event PID
 */
void CoroutineScheduler::resetEvent(uint32 pidEvent) {
	EVENT *evt = getEvent(pidEvent);
	if (evt)
		evt->signalled = false;
}

/**
 * Temporarily sets a given event to true, and then runs all waiting processes, allowing any
 * processes waiting on the event to be fired. It then immediately resets the event again.
 * @param pidEvent						Event PID
 *
 * @remarks		Should not be run inside of another process
 */
void CoroutineScheduler::pulseEvent(uint32 pidEvent) {
	EVENT *evt = getEvent(pidEvent);
	if (!evt)
		return;
	
	// Set the event as true
	evt->signalled = true;
	
	// start dispatching active process list for any processes that are currently waiting
	PROCESS *pOriginal = pCurrent;
	PROCESS *pNext;
	PROCESS *pProc = active->pNext;
	while (pProc != NULL) {
		pNext = pProc->pNext;

		// Only call processes that are currently waiting (either in waitForSingleObject or
		// waitForMultipleObjects) for the given event Pid
		for (int i = 0; i < CORO_MAX_PID_WAITING; ++i) {
			if (pProc->pidWaiting[i] == pidEvent) {
				// Dispatch the process
				pCurrent = pProc;
				pProc->coroAddr(pProc->state, pProc->param);

				if (!pProc->state || pProc->state->_sleep <= 0) {
					// Coroutine finished
					pCurrent = pCurrent->pPrevious;
					killProcess(pProc);
				} else {
					pProc->sleepTime = pProc->state->_sleep;
				}

				// pCurrent may have been changed
				pNext = pCurrent->pNext;
				pCurrent = NULL;

				break;
			}
		}

		pProc = pNext;
	}

	// Restore the original current process (if one was active)
	pCurrent = pOriginal;

	// Reset the event back to non-signalled
	evt->signalled = false;
}


} // end of namespace Common