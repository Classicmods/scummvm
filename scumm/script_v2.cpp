/* ScummVM - Scumm Interpreter
 * Copyright (C) 2001  Ludvig Strigeus
 * Copyright (C) 2001-2003 The ScummVM project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 *
 */

#include "stdafx.h"
#include "scumm.h"
#include "actor.h"
#include "charset.h"
#include "intern.h"
#include "sound.h"
#include "verbs.h"

#define OPCODE(x)	{ &Scumm_v2::x, #x }

void Scumm_v2::setupOpcodes() {
	static const OpcodeEntryV2 opcodes[256] = {
		/* 00 */
		OPCODE(o5_stopObjectCode),
		OPCODE(o2_putActor),
		OPCODE(o2_startMusic),
		OPCODE(o2_getActorRoom),
		/* 04 */
		OPCODE(o2_isGreaterEqual),
		OPCODE(o2_drawObject),
		OPCODE(o2_getActorElevation),
		OPCODE(o2_setState08),
		/* 08 */
		OPCODE(o2_isNotEqual),
		OPCODE(o2_faceActor),
		OPCODE(o2_assignVarWordDirect),
		OPCODE(o2_setObjY),
		/* 0C */
		OPCODE(o2_resourceRoutines),
		OPCODE(o2_walkActorToActor),
		OPCODE(o2_putActorAtObject),
		OPCODE(o2_ifNotState08),
		/* 10 */
		OPCODE(o2_getObjectOwner),
		OPCODE(o2_animateActor),
		OPCODE(o2_panCameraTo),
		OPCODE(o2_actorSet),
		/* 14 */
		OPCODE(o2_print),
		OPCODE(o2_actorFromPos),
		OPCODE(o2_getRandomNr),
		OPCODE(o2_setState04),
		/* 18 */
		OPCODE(o5_jumpRelative),
		OPCODE(o2_doSentence),
		OPCODE(o2_move),
		OPCODE(o2_setBitVar),
		/* 1C */
		OPCODE(o2_startSound),
		OPCODE(o2_ifClassOfIs),
		OPCODE(o2_walkActorTo),
		OPCODE(o2_ifState02),
		/* 20 */
		OPCODE(o5_stopMusic),
		OPCODE(o2_putActor),
		OPCODE(o2_saveLoadGame),
		OPCODE(o2_getActorY),
		/* 24 */
		OPCODE(o2_loadRoomWithEgo),
		OPCODE(o2_drawObject),
		OPCODE(o2_setVarRange),
		OPCODE(o2_ifNotState04),
		/* 28 */
		OPCODE(o2_equalZero),
		OPCODE(o2_setOwnerOf),
		OPCODE(o2_addDirect),
		OPCODE(o2_delayVariable),
		/* 2C */
		OPCODE(o2_assignVarByte),
		OPCODE(o2_putActorInRoom),
		OPCODE(o5_delay),
		OPCODE(o2_ifNotState04),
		/* 30 */
		OPCODE(o2_matrixOps),
		OPCODE(o2_getBitVar),
		OPCODE(o2_setCameraAt),
		OPCODE(o2_roomOps),
		/* 34 */
		OPCODE(o2_getDist),
		OPCODE(o2_findObject),
		OPCODE(o2_walkActorToObject),
		OPCODE(o2_setState01),
		/* 38 */
		OPCODE(o2_lessOrEqual),
		OPCODE(o2_doSentence),
		OPCODE(o2_subtract),
		OPCODE(o2_waitForActor),
		/* 3C */
		OPCODE(o5_stopSound),
		OPCODE(o2_getActorElevation),
		OPCODE(o2_walkActorTo),
		OPCODE(o2_ifNotState01),
		/* 40 */
		OPCODE(o2_cutscene),
		OPCODE(o2_putActor),
		OPCODE(o2_startScript),
		OPCODE(o2_getActorX),
		/* 44 */
		OPCODE(o2_isLess),
		OPCODE(o2_drawObject),
		OPCODE(o2_increment),
		OPCODE(o2_setState08),
		/* 48 */
		OPCODE(o2_isEqual),
		OPCODE(o2_faceActor),
		OPCODE(o2_chainScript),
		OPCODE(o2_setObjY),
		/* 4C */
		OPCODE(o2_waitForSentence),
		OPCODE(o2_walkActorToActor),
		OPCODE(o2_putActorAtObject),
		OPCODE(o2_ifState08),
		/* 50 */
		OPCODE(o2_pickupObject),
		OPCODE(o2_animateActor),
		OPCODE(o2_actorFollowCamera),
		OPCODE(o2_actorSet),
		/* 54 */
		OPCODE(o2_setObjectName),
		OPCODE(o2_actorFromPos),
		OPCODE(o2_getActorMoving),
		OPCODE(o2_setState02),
		/* 58 */
		OPCODE(beginOverride),
		OPCODE(o2_doSentence),
		OPCODE(o2_add),
		OPCODE(o2_setBitVar),
		/* 5C */
		OPCODE(o5_dummy),
		OPCODE(o2_ifClassOfIs),
		OPCODE(o2_walkActorTo),
		OPCODE(o2_ifNotState02),
		/* 60 */
		OPCODE(o5_cursorCommand),
		OPCODE(o2_putActor),
		OPCODE(o5_stopScript),
		OPCODE(o5_getActorFacing),
		/* 64 */
		OPCODE(o5_loadRoomWithEgo),
		OPCODE(o5_pickupObject),
		OPCODE(o5_getClosestObjActor),
		OPCODE(o2_clearState04),
		/* 68 */
		OPCODE(o5_isScriptRunning),
		OPCODE(o2_setOwnerOf),
		OPCODE(o2_subDirect),
		OPCODE(o5_dummy),
		/* 6C */
		OPCODE(o2_getObjY),
		OPCODE(o2_putActorInRoom),
		OPCODE(o5_dummy),
		OPCODE(o2_ifState04),
		/* 70 */
		OPCODE(o5_lights),
		OPCODE(o5_getActorCostume),
		OPCODE(o5_loadRoom),
		OPCODE(o2_roomOps),
		/* 74 */
		OPCODE(o2_getDist),
		OPCODE(o2_findObject),
		OPCODE(o2_walkActorToObject),
		OPCODE(o2_clearState01),
		/* 78 */
		OPCODE(o2_isGreater),
		OPCODE(o2_doSentence),
		OPCODE(o2_verbOps),
		OPCODE(o5_getActorWalkBox),
		/* 7C */
		OPCODE(o5_isSoundRunning),
		OPCODE(o2_getActorElevation),
		OPCODE(o2_walkActorTo),
		OPCODE(o2_ifNotState01),
		/* 80 */
		OPCODE(o5_breakHere),
		OPCODE(o2_putActor),
		OPCODE(o2_startMusic),
		OPCODE(o2_getActorRoom),
		/* 84 */
		OPCODE(o2_isGreaterEqual),
		OPCODE(o2_drawObject),
		OPCODE(o2_getActorElevation),
		OPCODE(o2_setState08),
		/* 88 */
		OPCODE(o2_isNotEqual),
		OPCODE(o2_faceActor),
		OPCODE(o2_assignVarWordDirect),
		OPCODE(o2_setObjY),
		/* 8C */
		OPCODE(o2_resourceRoutines),
		OPCODE(o2_walkActorToActor),
		OPCODE(o2_putActorAtObject),
		OPCODE(o2_ifNotState08),
		/* 90 */
		OPCODE(o2_getObjectOwner),
		OPCODE(o2_animateActor),
		OPCODE(o2_panCameraTo),
		OPCODE(o2_actorSet),
		/* 94 */
		OPCODE(o2_print),
		OPCODE(o2_actorFromPos),
		OPCODE(o2_getRandomNr),
		OPCODE(o2_setState02),
		/* 98 */
		OPCODE(o2_restart),
		OPCODE(o2_doSentence),
		OPCODE(o2_assignVarWord),
		OPCODE(o2_setBitVar),
		/* 9C */
		OPCODE(o2_startSound),
		OPCODE(o2_ifClassOfIs),
		OPCODE(o2_walkActorTo),
		OPCODE(o2_ifState02),
		/* A0 */
		OPCODE(o5_stopObjectCode),
		OPCODE(o2_putActor),
		OPCODE(o5_getAnimCounter),
		OPCODE(o2_getActorY),
		/* A4 */
		OPCODE(o5_loadRoomWithEgo),
		OPCODE(o5_pickupObject),
		OPCODE(o2_setVarRange),
		OPCODE(o2_setState04),
		/* A8 */
		OPCODE(o2_notEqualZero),
		OPCODE(o2_setOwnerOf),
		OPCODE(o2_addDirect),
		OPCODE(o5_dummy),
		/* AC */
		OPCODE(o5_delay),
		OPCODE(o2_putActorInRoom),
		OPCODE(o2_waitForSentence),
		OPCODE(o2_ifNotState04),
		/* B0 */
		OPCODE(o2_matrixOps),
		OPCODE(o2_getBitVar),
		OPCODE(o2_setCameraAt),
		OPCODE(o2_roomOps),
		/* B4 */
		OPCODE(o2_getDist),
		OPCODE(o2_findObject),
		OPCODE(o2_walkActorToObject),
		OPCODE(o2_setState02),
		/* B8 */
		OPCODE(o2_lessOrEqual),
		OPCODE(o2_doSentence),
		OPCODE(o2_subtract),
		OPCODE(o2_waitForActor),
		/* BC */
		OPCODE(o5_stopSound),
		OPCODE(o2_getActorElevation),
		OPCODE(o2_walkActorTo),
		OPCODE(o2_ifNotState01),
		/* C0 */
		OPCODE(o5_endCutscene),
		OPCODE(o2_putActor),
		OPCODE(o2_startScript),
		OPCODE(o2_getActorX),
		/* C4 */
		OPCODE(o2_isLess),
		OPCODE(o2_drawObject),
		OPCODE(o5_decrement),
		OPCODE(o2_clearState08),
		/* C8 */
		OPCODE(o2_isEqual),
		OPCODE(o2_faceActor),
		OPCODE(o2_chainScript),
		OPCODE(o2_setObjY),
		/* CC */
		OPCODE(o5_pseudoRoom),
		OPCODE(o2_walkActorToActor),
		OPCODE(o2_putActorAtObject),
		OPCODE(o2_ifState08),
		/* D0 */
		OPCODE(o2_pickupObject),
		OPCODE(o2_animateActor),
		OPCODE(o2_actorFollowCamera),
		OPCODE(o2_actorSet),
		/* D4 */
		OPCODE(o2_setObjectName),
		OPCODE(o2_actorFromPos),
		OPCODE(o2_getActorMoving),
		OPCODE(o2_setState02),
		/* D8 */
		OPCODE(o2_printEgo),
		OPCODE(o2_doSentence),
		OPCODE(o2_add),
		OPCODE(o5_divide),
		/* DC */
		OPCODE(o5_dummy),
		OPCODE(o2_ifClassOfIs),
		OPCODE(o2_walkActorTo),
		OPCODE(o2_ifNotState02),
		/* E0 */
		OPCODE(o5_cursorCommand),
		OPCODE(o2_putActor),
		OPCODE(o5_stopScript),
		OPCODE(o5_getActorFacing),
		/* E4 */
		OPCODE(o5_loadRoomWithEgo),
		OPCODE(o5_pickupObject),
		OPCODE(o5_getClosestObjActor),
		OPCODE(o2_clearState04),
		/* E8 */
		OPCODE(o5_isScriptRunning),
		OPCODE(o2_setOwnerOf),
		OPCODE(o2_subDirect),
		OPCODE(o5_dummy),
		/* EC */
		OPCODE(o2_getObjY),
		OPCODE(o2_putActorInRoom),
		OPCODE(o5_dummy),
		OPCODE(o2_ifState04),
		/* F0 */
		OPCODE(o5_lights),
		OPCODE(o5_getActorCostume),
		OPCODE(o5_loadRoom),
		OPCODE(o2_roomOps),
		/* F4 */
		OPCODE(o2_getDist),
		OPCODE(o2_findObject),
		OPCODE(o2_walkActorToObject),
		OPCODE(o2_clearState01),
		/* F8 */
		OPCODE(o2_isGreater),
		OPCODE(o2_doSentence),
		OPCODE(o2_verbOps),
		OPCODE(o5_getActorWalkBox),
		/* FC */
		OPCODE(o5_isSoundRunning),
		OPCODE(o2_getActorElevation),
		OPCODE(o2_walkActorTo),
		OPCODE(o2_ifState01)
	};

	_opcodesV2 = opcodes;
}

void Scumm_v2::executeOpcode(int i) {
	OpcodeProcV2 op = _opcodesV2[i].proc;
	(this->*op) ();
}

const char *Scumm_v2::getOpcodeDesc(int i) {
	return _opcodesV2[i].desc;
}

int Scumm_v2::getVar() {
	int var_id = fetchScriptByte();
	if ((var_id >= 14) && (var_id <= 16))
		return _vars[_vars[var_id]];
	return _vars[var_id];
}

int Scumm_v2::getVarOrDirectByte(byte mask) {
	if (_opcode & mask)
		return getVar();
	return fetchScriptByte();
}

int Scumm_v2::getVarOrDirectWord(byte mask) {
	if (_opcode & mask)
		return getVar();
	return fetchScriptWord();
}

void Scumm_v2::setStateCommon(byte type) {
	int obj = getVarOrDirectWord(0x80);
	putState(obj, getState(obj) | type);
}

void Scumm_v2::clearStateCommon(byte type) {
	int obj = getVarOrDirectWord(0x80);
	putState(obj, getState(obj) & ~type);
}

void Scumm_v2::o2_setState08() {
	int obj = getVarOrDirectWord(0x80);
	putState(obj, getState(obj) | 0x08);
	removeObjectFromRoom(obj);
	clearDrawObjectQueue();
}

void Scumm_v2::o2_clearState08() {
	int obj = getVarOrDirectWord(0x80);
	putState(obj, getState(obj) & 0xF7);
	removeObjectFromRoom(obj);
	clearDrawObjectQueue();
}

void Scumm_v2::o2_setState04() {
	setStateCommon(0x04);
}

void Scumm_v2::o2_clearState04() {
	clearStateCommon(0x04);
}

void Scumm_v2::o2_setState02() {
	setStateCommon(0x02);
}

void Scumm_v2::o2_clearState02() {
	clearStateCommon(0x02);
}

void Scumm_v2::o2_setState01() {
	setStateCommon(0x01);
}

void Scumm_v2::o2_clearState01() {
	clearStateCommon(0x01);
}

void Scumm_v2::getResultPos() {
	_resultVarNumber = fetchScriptByte();
}

void Scumm_v2::getResultPosDirect() {
	_resultVarNumber = _vars[fetchScriptByte()];
}

void Scumm_v2::o2_assignVarWordDirect() {
	getResultPosDirect();
	_vars[_resultVarNumber] = fetchScriptWord();
}

void Scumm_v2::o2_assignVarByteDirect() {
	getResultPosDirect();
	_vars[_resultVarNumber] = fetchScriptByte();
}

void Scumm_v2::o2_assignVarByte() {
	getResultPos();
	_vars[_resultVarNumber] = fetchScriptByte();
}

void Scumm_v2::o2_assignVarWord() {
	getResultPos();
	_vars[_resultVarNumber] = fetchScriptWord();
}

void Scumm_v2::o2_setObjY() {
	int obj = getVarOrDirectWord(0x80);
	int y = fetchScriptByte();

	if (whereIsObject(obj) != WIO_NOT_FOUND) {
		ObjectData *od = &_objs[getObjectIndex(obj)];
		od->walk_y = (y << 5) | (od->walk_y & 0x1F);
	}
}

void Scumm_v2::o2_getObjY() {
	int obj = getVarOrDirectWord(0x80);
	getResultPos();

	if (whereIsObject(obj) != WIO_NOT_FOUND) {
		ObjectData *od = &_objs[getObjectIndex(obj)];
		_vars[_resultVarNumber] = od->walk_y >> 5;
	} else {
		_vars[_resultVarNumber] = 0xFF;
	}
}

void Scumm_v2::o2_setBitVar() {
	byte hi = fetchScriptByte();
	byte lo = fetchScriptByte();
	byte a = getVarOrDirectByte(0x80);

	int bit_var = (hi << 8) + lo + a;
	int bit_offset = bit_var & 0x0f;
  bit_var >>= 4;

	if (getVarOrDirectByte(0x80))
		_vars[bit_var] |= (1 << bit_offset);
	else
		_vars[bit_var] &= ~(1 << bit_offset);
}

void Scumm_v2::o2_getBitVar() {
	getResultPos();
	byte hi = fetchScriptByte();
	byte lo = fetchScriptByte();
	byte a = getVarOrDirectByte(0x80);

	int bit_var = (hi << 8) + lo + a;
	int bit_offset = bit_var & 0x0f;
  bit_var >>= 4;

	_vars[_resultVarNumber] = (_vars[bit_var] & (1 << bit_offset)) ? 1 : 0;
}

void Scumm_v2::ifStateCommon(byte type) {
	int obj = getVarOrDirectWord(0x80);

	if ((getState(obj) & type) == 0)
		o5_jumpRelative();
	else
		ignoreScriptWord();
}

void Scumm_v2::ifNotStateCommon(byte type) {
	int obj = getVarOrDirectWord(0x80);

	if ((getState(obj) & type) != 0)
		o5_jumpRelative();
	else
		ignoreScriptWord();
}

void Scumm_v2::o2_ifState08() {
	ifStateCommon(0x08);
}

void Scumm_v2::o2_ifNotState08() {
	ifNotStateCommon(0x08);
}

void Scumm_v2::o2_ifState04() {
	ifStateCommon(0x04);
}

void Scumm_v2::o2_ifNotState04() {
	ifNotStateCommon(0x04);
}

void Scumm_v2::o2_ifState02() {
	ifStateCommon(0x02);
}

void Scumm_v2::o2_ifNotState02() {
	ifNotStateCommon(0x02);
}

void Scumm_v2::o2_ifState01() {
	ifStateCommon(0x01);
}

void Scumm_v2::o2_ifNotState01() {
	ifNotStateCommon(0x01);
}

void Scumm_v2::o2_addDirect() {
	int a;
	getResultPosDirect();
	a = getVarOrDirectWord(0x80);
	_vars[_resultVarNumber] += a;
}

void Scumm_v2::o2_subDirect() {
	int a;
	getResultPosDirect();
	a = getVarOrDirectWord(0x80);
	_vars[_resultVarNumber] -= a;
}

void Scumm_v2::o2_waitForActor() {
	if (derefActorSafe(getVarOrDirectByte(0x80), "o2_waitForActor")->moving) {
		_scriptPointer -= 2;
		o5_breakHere();
	}
}

void Scumm_v2::o2_actorSet() {
	int act = getVarOrDirectByte(0x80);
	int arg = getVarOrDirectByte(0x40);
	Actor *a = derefActorSafe(act, "actorSet");
	int i;

	if (!a)
		return;

	switch (fetchScriptByte()) {
		case 1: 	// Actor Sound
			a->sound[0] = arg;
			break;

		case 2:		// Actor Set Color
			i = fetchScriptByte();
			a->palette[i] = arg;
			a->needRedraw = true;
			break;

		case 3:		// Actor Name
			loadPtrToResource(rtActorName, a->number, NULL);
			break;

		case 4:		// Actor Costume
			a->setActorCostume(arg);
			break;

		case 5:		// Talk Color
			a->talkColor = arg;
			break;
	}
}

void Scumm_v2::o2_waitForSentence() {
	if (_sentenceNum)
		if (!isScriptInUse(2))
			return;

	_scriptPointer--;
	o5_breakHere();
}

void Scumm_v2::o2_restart() {
}

void Scumm_v2::o2_drawObject() {
	int obj, idx, i;
	ObjectData *od;
	uint16 x, y, w, h;
	int xpos, ypos;

	obj = getVarOrDirectWord(0x80);
	xpos = getVarOrDirectWord(0x40);
	ypos = getVarOrDirectWord(0x20);

	idx = getObjectIndex(obj);
	if (idx == -1)
		return;

	od = &_objs[idx];
	if (xpos != 0xFF) {
		od->walk_x += (xpos << 3) - od->x_pos;
		od->x_pos = xpos << 3;
		od->walk_y += (ypos << 3) - od->y_pos;
		od->y_pos = ypos << 3;
	}
	addObjectToDrawQue(idx);

	x = od->x_pos;
	y = od->y_pos;
	w = od->width;
	h = od->height;

	i = _numLocalObjects;
	do {
		if (_objs[i].obj_nr && _objs[i].x_pos == x && _objs[i].y_pos == y && _objs[i].width == w && _objs[i].height == h)
			putState(_objs[i].obj_nr, getState(_objs[i].obj_nr) & 0xF7);
	} while (--i);

	putState(obj, getState(_objs[obj].obj_nr) | 0x08);
}

void Scumm_v2::o2_resourceRoutines() {
	int resid = getVarOrDirectByte(0x80);
	int opcode = fetchScriptByte();

	if (((opcode & 0x0f) == 0) || ((opcode & 0x0f) == 1)) {
		switch (opcode & 0xf1) {
		case 96:
			lock(rtSound, resid);
			return;
		case 97:
			unlock(rtSound, resid);
			return;
		case 80:
			lock(rtScript, resid);
			return;
		case 81:
			unlock(rtScript, resid);
			return;
		case 32:
			lock(rtCostume, resid);
			return;
		case 33:
			unlock(rtCostume, resid);
			return;
		case 48:
			lock(rtRoom, resid);
			return;
		case 49:
			unlock(rtRoom, resid);
			return;
		default:
			error("o2_resourceRoutines: unknown lock/unlock opcode");
		}
	} else {
		switch (opcode & 0xf1) {
		case 96:
		case 80:
		case 32:
		case 48:
			return;
		case 97:
			ensureResourceLoaded(rtSound, resid);
			return;
		case 81:
			ensureResourceLoaded(rtScript, resid);
			return;
		case 33:
			ensureResourceLoaded(rtCostume, resid);
			return;
		case 49:
			ensureResourceLoaded(rtRoom, resid);
		default:
			error("o2_resourceRoutines: unknown load/nuke opcode");
		}
	}
}

void Scumm_v2::o2_verbOps() {
 int opcode = fetchScriptByte(), slot, state;

 switch (opcode) {
	case 0:		// Delete Verb
		slot = getVarOrDirectByte(0x80);
	break;

	case 0xFF:	// Verb On/Off
		slot = fetchScriptByte();
		state = fetchScriptByte();
	break;

	default: {	// New Verb
		fetchScriptByte(); // X
		fetchScriptByte(); // Y
		getVarOrDirectByte(0x80); // Slot
		fetchScriptByte(); // ?
	}
	break;
 }
}

void Scumm_v2::o2_isEqual() {
	int a = getVar();
	int b = getVarOrDirectWord(0x80);

	if (b == a)
		ignoreScriptWord();
	else
		o5_jumpRelative();

}

void Scumm_v2::o2_isGreater() {
	int16 a = getVar();
	int16 b = getVarOrDirectWord(0x80);

	if (b > a)
		ignoreScriptWord();
	else
		o5_jumpRelative();
}

void Scumm_v2::o2_isGreaterEqual() {
	int16 a = getVar();
	int16 b = getVarOrDirectWord(0x80);

	if (b >= a)
		ignoreScriptWord();
	else
		o5_jumpRelative();
}

void Scumm_v2::o2_isLess() {
	int16 a = getVar();
	int16 b = getVarOrDirectWord(0x80);

	if (b < a)
		ignoreScriptWord();
	else
		o5_jumpRelative();
}

void Scumm_v2::o2_lessOrEqual() {
	int16 a = getVar();
	int16 b = getVarOrDirectWord(0x80);

	if (b <= a)
		ignoreScriptWord();
	else
		o5_jumpRelative();
}

void Scumm_v2::o2_isNotEqual() {
	int16 a = getVar();
	int16 b = getVarOrDirectWord(0x80);

	if (b != a)
		ignoreScriptWord();
	else
		o5_jumpRelative();
}

void Scumm_v2::o2_notEqualZero() {
	int a = getVar();

	if (a != 0)
		ignoreScriptWord();
	else
		o5_jumpRelative();
}

void Scumm_v2::o2_equalZero() {
	int a = getVar();

	if (a == 0)
		ignoreScriptWord();
	else
		o5_jumpRelative();
}

void Scumm_v2::o2_doSentence() {
	int a;
	SentenceTab *st;

	a = getVarOrDirectByte(0x80);
	if (a == 0xFB) {
		_sentenceNum = 0;
		stopScriptNr(2);
		clearClickedStatus();
		return;
	}
	if (a == 0xFC) {
		_sentenceNum--;
		clearClickedStatus();
		return;
	}

	st = &_sentence[_sentenceNum++];

	st->unk5 = a;
	st->unk4 = getVarOrDirectWord(0x40);
	st->unk3 = getVarOrDirectWord(0x20);
	st->freezeCount = 0;
	
	// TODO
	switch(fetchScriptByte()) {
	case 1:
		break;
	case 2:
		break;
	}
}

void Scumm_v2::o2_printEgo() {
	//_actorToPrintStrFor = (unsigned char)_vars[VAR_EGO];
	//_messagePtr = _scriptPointer;

	printf("o2_printEgo(%s)\n", _scriptPointer);
	_scriptPointer += resStrLen(_scriptPointer) + 1;
	//_messagePtr = addMessageToStack(_messagePtr);
	//_scriptPointer = _messagePtr;
}

void Scumm_v2::o2_ifClassOfIs() {
	int act = getVarOrDirectWord(0x80);
	int clsop = getVarOrDirectByte(0x40);

	if (getObjectIndex(act) == -1) {
		o5_jumpRelative();
		return;
	}

	ObjectData *od = &_objs[getObjectIndex(act)];
	byte cls = *(getResourceAddress(rtRoom, _currentRoom) + od->OBCDoffset + 10);
	if ((cls & clsop) != clsop) {
		o5_jumpRelative();
	}
}

void Scumm_v2::o2_walkActorTo() {
	int x, y;
	Actor *a;
	a = derefActorSafe(getVarOrDirectByte(0x80), "o2_walkActorTo");
	x = getVarOrDirectByte(0x40);
	y = getVarOrDirectByte(0x20);
	a->startWalkActor(x, y, -1);
}

void Scumm_v2::o2_putActor() {
	int x, y;
	Actor *a;

	a = derefActorSafe(getVarOrDirectByte(0x80), "o2_putActor");
	if (!a)
		return;
	x = getVarOrDirectByte(0x40);
	y = getVarOrDirectByte(0x20);

	a->putActor(x, y, a->room);
}

void Scumm_v2::o2_startScript() {
	int script = getVarOrDirectByte(0x80);
	runScript(script, 0, 0, 0);
}

void Scumm_v2::o2_panCameraTo() {
	panCameraTo(getVarOrDirectByte(0x80), 0);
}

void Scumm_v2::o2_move() {
	getResultPos();
	_vars[_resultVarNumber] = getVarOrDirectWord(0x80);
}

void Scumm_v2::o2_startMusic() {
	_sound->addSoundToQueue(getVarOrDirectByte(0x80));
}

void Scumm_v2::o2_getActorRoom() {
	int act;
	Actor *a;
	getResultPos();
	act = getVarOrDirectByte(0x80);

	a = derefActorSafe(act, "o2_getActorRoom");
	if (!a) {
		warning("Invalid actor %d in o2_getActorRoom", act);
		return;
	}

	_vars[_resultVarNumber] = a->room;
}

void Scumm_v2::o2_getActorElevation() {
	int act;
	Actor *a;
	getResultPos();
	act = getVarOrDirectByte(0x80);

	a = derefActorSafe(act, "o2_getActorElevation");
	if (!a) {
		warning("Invalid actor %d in o2_getActorElevation", act);
		return;
	}

	_vars[_resultVarNumber] = a->elevation;
}

void Scumm_v2::o2_walkActorToActor() {
	int b, x, y;
	Actor *a, *a2;
	int nr;
	int nr2 = getVarOrDirectByte(0x80);
	a = derefActorSafe(nr2, "o2_walkActorToActor");
	if (!a)
		return;

	if (!a->isInCurrentRoom()) {
		getVarOrDirectByte(0x40);
		fetchScriptByte();
		return;
	}

	nr = getVarOrDirectByte(0x40);
	a2 = derefActorSafe(nr, "o2_walkActorToActor(2)");
	if (!a2)
		return;

	if (!a2->isInCurrentRoom()) {
		fetchScriptByte();
		return;
	}
	b = fetchScriptByte();				/* distance from actor */
	if (b == 0xFF) {
		b = a2->scalex * a->width / 0xFF;
		b = b + b / 2;
	}
	x = a2->x;
	y = a2->y;
	if (x < a->x)
		x += b;
	else
		x -= b;

	a->startWalkActor(x, y, -1);
}

void Scumm_v2::o2_walkActorToObject() {
	int obj;
	Actor *a;

	a = derefActorSafe(getVarOrDirectByte(0x80), "o2_walkActorToObject");
	obj = getVarOrDirectWord(0x40);
	if (whereIsObject(obj) != WIO_NOT_FOUND) {
		int x, y, dir;
		getObjectXYPos(obj, x, y, dir);
		a->startWalkActor(x, y, dir);
	}
}

void Scumm_v2::o2_faceActor() {
	int act, obj;
	Actor *a;
	act = getVarOrDirectByte(0x80);
	obj = getVarOrDirectWord(0x40);

	a = derefActorSafe(act, "o2_faceActor");
	assert(a);

	a->faceToObject(obj);
}

void Scumm_v2::o2_putActorAtObject() {
	int obj, x, y;
	Actor *a;

	a = derefActorSafe(getVarOrDirectByte(0x80), "o2_putActorAtObject");
	obj = getVarOrDirectWord(0x40);
	if (whereIsObject(obj) != WIO_NOT_FOUND)
		getObjectXYPos(obj, x, y);
	else {
		x = 240;
		y = 120;
	}
	a->putActor(x, y, a->room);
}

void Scumm_v2::o2_getObjectOwner() {
	getResultPos();
	_vars[_resultVarNumber] = getOwner(getVarOrDirectWord(0x80));
}

void Scumm_v2::o2_animateActor() {
	int act = getVarOrDirectByte(0x80);
	int anim = getVarOrDirectByte(0x40);

	Actor *a = derefActorSafe(act, "o2_animateActor");
	if (!a)
		return;

	a->animateActor(anim);
}

void Scumm_v2::o2_print() {
	_actorToPrintStrFor = getVarOrDirectByte(0x80);
	decodeParseString();
}

void Scumm_v2::o2_actorFromPos() {
	int x, y;
	getResultPos();
	x = getVarOrDirectByte(0x80);
	y = getVarOrDirectByte(0x40);
	_vars[_resultVarNumber] = getActorFromPos(x, y);
}

void Scumm_v2::o2_getRandomNr() {
	getResultPos();
	_vars[_resultVarNumber] = _rnd.getRandomNumber(getVarOrDirectByte(0x80));
}

void Scumm_v2::o2_isScriptRunning() {
	getResultPos();
	_vars[_resultVarNumber] = isScriptRunning(getVarOrDirectByte(0x80));
}

void Scumm_v2::o2_getVerbEntrypoint() {
	int a, b;
	getResultPos();
	a = getVarOrDirectWord(0x80);
	b = getVarOrDirectWord(0x40);

	_vars[_resultVarNumber] = getVerbEntrypoint(a, b);
}

void Scumm_v2::o2_startSound() {
	_vars[VAR_MUSIC_FLAG] = 0;
	_sound->addSoundToQueue(getVarOrDirectByte(0x80));
}

void Scumm_v2::o2_saveLoadGame() {
	getResultPos();
	byte a = getVarOrDirectByte(0x80);

	error("TODO: o2_saveLoadGame(%d)", a);
	_vars[_resultVarNumber] = 0;
}

void Scumm_v2::o2_getActorX() {
	int a;
	getResultPos();

	a = getVarOrDirectByte(0x80);
	_vars[_resultVarNumber] = getObjX(a);
}

void Scumm_v2::o2_getActorY() {
	int a;
	getResultPos();

	a = getVarOrDirectByte(0x80);
	_vars[_resultVarNumber] = getObjY(a);
}

void Scumm_v2::o2_loadRoomWithEgo() {
	Actor *a;
	int obj, room, x, y;

	obj = getVarOrDirectWord(0x80);
	room = getVarOrDirectByte(0x40);

	a = derefActorSafe(_vars[VAR_EGO], "o2_loadRoomWithEgo");

	a->putActor(0, 0, room);
	_egoPositioned = false;

	x = (int16)fetchScriptByte();
	y = (int16)fetchScriptByte();

	startScene(a->room, a, obj);

	camera._dest.x = camera._cur.x = a->x;
	setCameraAt(a->x, a->y);
	setCameraFollows(a);

	_fullRedraw = 1;

	if (x != -1) {
		a->startWalkActor(x, y, -1);
	}
	runScript(5, 0, 0, 0);
}

void Scumm_v2::o2_setVarRange() {
	int a, b;

	getResultPos();
	a = fetchScriptByte();
	do {
		if (_opcode & 0x80)
			b = fetchScriptWord();
		else
			b = fetchScriptByte();

		_vars[_resultVarNumber++] = b;
	} while (--a);
}

void Scumm_v2::o2_setOwnerOf() {
	int obj, owner;

	obj = getVarOrDirectWord(0x80);
	owner = getVarOrDirectByte(0x40);

	setOwnerOf(obj, owner);
	runHook(0);
}

void Scumm_v2::o2_delayVariable() {
	vm.slot[_currentScript].delay = getVar();
	vm.slot[_currentScript].status = 1;
	o5_breakHere();
}

void Scumm_v2::o2_putActorInRoom() {
	int room;
	Actor *a;

	a = derefActorSafe(getVarOrDirectByte(0x80), "o2_putActorInRoom");
	room = getVarOrDirectByte(0x40);

	if (a == NULL)
		return;

	a->room = room;
	if (!room)
		a->putActor(0, 0, 0);
}

void Scumm_v2::o2_matrixOps() {
	int a, b;

	a = getVarOrDirectByte(0x80);
	b = fetchScriptByte();
	setBoxFlags(a, b);
}

void Scumm_v2::o2_setCameraAt() {
	setCameraAtEx(getVarOrDirectByte(0x80));
}

void Scumm_v2::o2_roomOps() {
	int a = getVarOrDirectByte(0x80);
	int b = getVarOrDirectByte(0x40);

	_opcode = fetchScriptByte();
	switch (_opcode & 0x1F) {
	case 1:											/* room scroll */
		if (a < (_realWidth / 2))
			a = (_realWidth / 2);
		if (b < (_realWidth / 2))
			b = (_realWidth / 2);
		if (a > _scrWidth - (_realWidth / 2))
			a = _scrWidth - (_realWidth / 2);
		if (b > _scrWidth - (_realWidth / 2))
			b = _scrWidth - (_realWidth / 2);
		_vars[VAR_CAMERA_MIN_X] = a;
		_vars[VAR_CAMERA_MAX_X] = b;
		break;
	case 2:											/* room color */
		_shadowPalette[b] = a;
		_fullRedraw = true;
		break;
	}
}

void Scumm_v2::o2_getDist() {
	getResultPos();
	int o1 = getVarOrDirectWord(0x80);
	int o2 = getVarOrDirectWord(0x40);
	_vars[_resultVarNumber] = getObjActToObjActDist(o1, o2);
}

void Scumm_v2::o2_findObject() {
	getResultPos();
	int x = getVarOrDirectWord(0x80);
	int y = getVarOrDirectWord(0x40);
	_vars[_resultVarNumber] = findObject(x, y);
}

void Scumm_v2::o2_subtract() {
	int a;
	getResultPos();
	a = getVarOrDirectWord(0x80);
	_vars[_resultVarNumber] -= a;
}

void Scumm_v2::o2_cutscene() {
	// TODO
}

void Scumm_v2::o2_increment() {
	getResultPos();
	_vars[_resultVarNumber]++;
}

void Scumm_v2::o2_chainScript() {
	int data = getVarOrDirectByte(0x80);
	int cur = _currentScript;

	vm.slot[cur].number = 0;
	vm.slot[cur].status = 0;
	_currentScript = 0xFF;

	runScript(data, 0, 0, 0);
}

void Scumm_v2::o2_pickupObject() {
	int obj = getVarOrDirectWord(0x80);

	if (obj < 1) {
		error("pickupObject received invalid index %d (script %d)", obj, vm.slot[_currentScript].number);
	}

	if (getObjectIndex(obj) == -1)
		return;

	if (whereIsObject(obj) == WIO_INVENTORY)	/* Don't take an */
		return;											/* object twice */

	addObjectToInventory(obj, _roomResource);
	removeObjectFromRoom(obj);
	putOwner(obj, _vars[VAR_EGO]);
	putClass(obj, 32, 1);
	putState(obj, 1);
	clearDrawObjectQueue();
	runHook(1);
}

void Scumm_v2::o2_actorFollowCamera() {
	actorFollowCamera(getVarOrDirectByte(0x80));
}

void Scumm_v2::o2_setObjectName() {
	int obj = getVarOrDirectWord(0x80);
	int size;
	int a;
	int i = 0;
	byte *name = NULL;
	byte work[256];
	
	// Read in new name
	do {
		a = fetchScriptByte();
		work[i++] = a;
	} while (a);
	work[i] = 0;

	if (obj < NUM_ACTORS)
		error("Can't set actor %d name with new-name-of", obj);

	name = getObjOrActorName(obj);

	byte *objptr;
	byte offset = 0;

	objptr = getOBCDFromObject(obj);
	offset = *(objptr + 18);
	size = READ_LE_UINT16(objptr) - offset;

	if (i >= size) {
		warning("New name of object %d too long (old *%s* new *%s*)", obj, name, work);
		i = size - 1;
	}

	memcpy(name, work, i + 1);
	runHook(0);
}

void Scumm_v2::o2_getActorMoving() {
	int act;
	Actor *a;
	getResultPos();
	act = getVarOrDirectByte(0x80);

	a = derefActorSafe(act, "o2_getActorMoving");
	if (!a) {
		warning("Invalid actor %d in o2_getActorMoving", act);
		return;
	}

	_vars[_resultVarNumber] = a->moving;
}

void Scumm_v2::o2_add() {
	int a;
	getResultPos();
	a = getVarOrDirectWord(0x80);
	_vars[_resultVarNumber] += a;
}

