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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "common/scummsys.h"
#include "mads/game.h"
#include "mads/mads.h"
#include "mads/resources.h"
#include "mads/screen.h"
#include "mads/nebular/menu_nebular.h"

namespace MADS {

namespace Nebular {

#define NEBULAR_MENUSCREEN 990
#define MADS_MENU_Y ((MADS_SCREEN_HEIGHT - MADS_SCENE_HEIGHT) / 2)
#define MADS_MENU_ANIM_DELAY 70

MenuView::MenuView(MADSEngine *vm) : FullScreenDialog(vm) {
	_breakFlag = false;
	_redrawFlag = true;
	_palFlag = false;
}

void MenuView::show() {
	Scene &scene = _vm->_game->_scene;	
	EventsManager &events = *_vm->_events;
	display();

	events.setEventTarget(this);
	events.hideCursor();

	while (!_breakFlag && !_vm->shouldQuit()) {
		if (_redrawFlag) {
			scene.drawElements(_vm->_game->_fx, _vm->_game->_fx);

			_vm->_screen.copyRectToScreen(Common::Rect(0, 0, 320, 200));
			_redrawFlag = false;
		}

		_vm->_events->waitForNextFrame();
		_vm->_game->_fx = kTransitionNone;
		doFrame();
	}

	events.setEventTarget(nullptr);
}

void MenuView::display() {
	_vm->_palette->resetGamePalette(4, 8);

	FullScreenDialog::display();
}

/*------------------------------------------------------------------------*/

MainMenu::MainMenu(MADSEngine *vm): MenuView(vm) {
	Common::fill(&_menuItems[0], &_menuItems[7], (SpriteAsset *)nullptr);
	Common::fill(&_menuItemIndexes[0], &_menuItemIndexes[7], -1);
	_delayTimeout = 0;
	_menuItemIndex = -1;
	_frameIndex = 0;
	_skipFlag = false;
	_highlightedIndex = -1;
	_selectedIndex = -1;
	_buttonDown = false;
}

MainMenu::~MainMenu() {
}

void MainMenu::display() {
	MenuView::display();
	Scene &scene = _vm->_game->_scene;
	ScreenObjects &screenObjects = _vm->_game->_screenObjects;
	screenObjects.clear();

	// Load each of the menu item assets and add to the scene sprites list
	for (int i = 0; i < 7; ++i) {
		Common::String spritesName = Resources::formatName(NEBULAR_MENUSCREEN,
			'A', i + 1, EXT_SS, "");
		_menuItems[i] = new SpriteAsset(_vm, spritesName, 0);
		_menuItemIndexes[i] = scene._sprites.add(_menuItems[i]);

		// Register the menu item area in the screen objects
		MSprite *frame0 = _menuItems[i]->getFrame(0);
		Common::Point pt(frame0->_offset.x - (frame0->w / 2),
			frame0->_offset.y - frame0->h + _vm->_screen._offset.y);
		screenObjects.add(
			Common::Rect(pt.x, pt.y, pt.x + frame0->w, pt.y + frame0->h),
			LAYER_GUI, CAT_COMMAND, i);
	}

	// Set the cursor for when it's shown
	_vm->_events->setCursor(CURSOR_ARROW);
}

void MainMenu::doFrame() {
	// Delay between animation frames on the menu
	uint32 currTime = g_system->getMillis();
	if (currTime < _delayTimeout)
		return;
	_delayTimeout = currTime + MADS_MENU_ANIM_DELAY;

	// If an item has already been selected, handle rotating out the other menu items
	if (_selectedIndex != -1) {
		if (_frameIndex == _menuItems[0]->getCount()) {
			handleAction((MADSGameAction)_selectedIndex);
		} else {
			for (_menuItemIndex = 0; _menuItemIndex < 6; ++_menuItemIndex) {
				if (_menuItemIndex != _selectedIndex) {
					addSpriteSlot();
				}
			}

			// Move the menu items to the next frame
			++_frameIndex;
		}
		return;
	}

	// If we've alerady reached the end of the menuitem animation, exit immediately
	if (_menuItemIndex == 6)
		return;

	// If the user has chosen to skip the animation, show the full menu immediately
	if (_skipFlag && _menuItemIndex >= 0) {
		// Quickly loop through all the menu items to display each's final frame		
		for (; _menuItemIndex < 6; ++_menuItemIndex) {
			// Draw the final frame of the menuitem
			_frameIndex = 0;
			addSpriteSlot();
		}

		_vm->_events->showCursor();
	} else {
		if ((_menuItemIndex == -1) || (_frameIndex == 0)) {
			if (++_menuItemIndex == 6) {
				// Reached end of display animation
				_vm->_events->showCursor();
				return;
			}

			_frameIndex = _menuItems[_menuItemIndex]->getCount() - 1;
		} else {
			--_frameIndex;
		}

		// Move to the next menuitem frame
		addSpriteSlot();
	}
}

void MainMenu::addSpriteSlot() {
	Scene &scene = _vm->_game->_scene;
	SpriteSlots &spriteSlots = scene._spriteSlots;
	
	int seqIndex = (_menuItemIndex < 6) ? _menuItemIndex : _frameIndex;
	spriteSlots.deleteTimer(seqIndex);

	SpriteAsset *menuItem = _menuItems[_menuItemIndex];
	MSprite *spr = menuItem->getFrame(_frameIndex);

	SpriteSlot &slot = spriteSlots[spriteSlots.add()];
	slot._flags = IMG_UPDATE;
	slot._seqIndex = seqIndex;
	slot._spritesIndex = _menuItemIndexes[_menuItemIndex];
	slot._frameNumber = _frameIndex + 1;
	slot._position = spr->_offset;
	slot._depth = 1;
	slot._scale = 100;

	_redrawFlag = true;
}

bool MainMenu::onEvent(Common::Event &event) {
	Scene &scene = _vm->_game->_scene;
	if (_selectedIndex != -1)
		return false;

	// Handle keypresses - these can be done at any time, even when the menu items are being drawn
	if (event.type == Common::EVENT_KEYDOWN) {
		switch (event.kbd.keycode) {
		case Common::KEYCODE_ESCAPE:
		case Common::KEYCODE_F6:
			handleAction(EXIT);
			break;

		case Common::KEYCODE_F1:
			handleAction(START_GAME);
			break;

		case Common::KEYCODE_F2:
			handleAction(RESUME_GAME);
			break;

		case Common::KEYCODE_F3:
			handleAction(SHOW_INTRO);
			break;

		case Common::KEYCODE_F4:
			handleAction(CREDITS);
			break;

		case Common::KEYCODE_F5:
			handleAction(QUOTES);
			break;

		case Common::KEYCODE_s: {
			// Goodness knows why, but Rex has a key to restart the menuitem animations
			// Restart the animation
			_menuItemIndex = -1;
			for (int i = 0; i < 6; ++i)
				scene._spriteSlots.deleteTimer(i);

			_skipFlag = false;
			_vm->_events->hideCursor();
			break;
		}

		default:
			// Any other key skips the menu animation
			_skipFlag = true;
			return false;
		}

		return true;
	}

	switch (event.type) {
	case Common::EVENT_LBUTTONDOWN:
		if (_vm->_events->isCursorVisible()) {
			_buttonDown = true;
			int menuIndex = getHighlightedItem(event.mouse);

			if (menuIndex != _highlightedIndex) {
				scene._spriteSlots.deleteTimer(menuIndex);

				_highlightedIndex = menuIndex;
				if (_highlightedIndex != -1) {
					_frameIndex = _highlightedIndex;
					addSpriteSlot();
				}
			}
		} else {
			// Skip the menu animation
			_skipFlag = true;
		}
		return true;

	case Common::EVENT_MOUSEMOVE: 
		if (_buttonDown) {
			int menuIndex = getHighlightedItem(event.mouse);
			if (menuIndex != _highlightedIndex) {
				if (_highlightedIndex != -1) {
					// Revert to the unselected menu item
					unhighlightItem();
				}

				if (menuIndex != -1) {
					// Highlight new item
					_highlightedIndex = menuIndex;
					_frameIndex = _highlightedIndex;
					addSpriteSlot();
				}
			}
		}
		break;

	case Common::EVENT_LBUTTONUP:
		_buttonDown = false;
		if (_highlightedIndex != -1) {
			_selectedIndex = _highlightedIndex;
			unhighlightItem();
			_frameIndex = 0;
		}

		return true;

	default:
		break;
	}
	
	return false;
}

int MainMenu::getHighlightedItem(const Common::Point &pt) {
	return _vm->_game->_screenObjects.scan(pt, LAYER_GUI) - 1;
}

void MainMenu::unhighlightItem() {
	// Revert to the unselected menu item
	_vm->_game->_scene._spriteSlots.deleteTimer(_highlightedIndex);
	_menuItemIndex = _highlightedIndex;
	_frameIndex = 0;
	addSpriteSlot();

	_menuItemIndex = 6;
	_highlightedIndex = -1;
}

void MainMenu::handleAction(MADSGameAction action) {
	_vm->_events->hideCursor();
	_breakFlag = true;

	switch (action) {
	case START_GAME:
		// Show the difficulty dialog
		_vm->_dialogs->_pendingDialog = DIALOG_DIFFICULTY;
		break;

	case RESUME_GAME:
		// The original resumed the most recently saved game. Instead, 
		// just show the load game scren
		_vm->_dialogs->_pendingDialog = DIALOG_RESTORE;
		return;

	case SHOW_INTRO:
		AnimationView::execute(_vm, "@rexopen");
		break;

	case CREDITS:
		TextView::execute(_vm, "credits");
		return;

	case QUOTES:
		TextView::execute(_vm, "quotes");
		return;

	case EXIT:
		_vm->_dialogs->_pendingDialog = DIALOG_ADVERT;
		break;
	default:
		break;
	}
}

/*------------------------------------------------------------------------*/

AdvertView::AdvertView(MADSEngine *vm): EventTarget(), _vm(vm) {
	_breakFlag = false;
}

void AdvertView::show() {
	bool altAdvert = _vm->getRandomNumber(1000) >= 500;
	int screenId = altAdvert ? 995 : 996;
	uint32 expiryTime = g_system->getMillis() + 10 * 1000;

	_vm->_palette->resetGamePalette(4, 8);
	
	// Load the advert background onto the screen
	SceneInfo *sceneInfo = SceneInfo::init(_vm);
	sceneInfo->load(screenId, 0, Common::String(), 0, _vm->_game->_scene._depthSurface,
		_vm->_screen);
	_vm->_screen.copyRectToScreen(_vm->_screen.getBounds());
	delete sceneInfo;

	EventsManager &events = *_vm->_events;
	events.setEventTarget(this);
	events.hideCursor();

	while (!_breakFlag && !_vm->shouldQuit()) {
		_vm->_events->waitForNextFrame();
		_vm->_game->_fx = kTransitionNone;

		_breakFlag |= g_system->getMillis() >= expiryTime;
	}

	events.setEventTarget(nullptr);
	_vm->quitGame();
}

bool AdvertView::onEvent(Common::Event &event) {
	if (event.type == Common::EVENT_KEYDOWN || event.type == Common::EVENT_LBUTTONDOWN) {
		_breakFlag = true;
		return true;
	}

	return false;
}

/*------------------------------------------------------------------------*/

char TextView::_resourceName[100];
#define TEXTVIEW_LINE_SPACING 2
#define TEXT_ANIMATION_DELAY 100
#define TV_NUM_FADE_STEPS 40
#define TV_FADE_DELAY_MILLI 50

void TextView::execute(MADSEngine *vm, const Common::String &resName) {
	assert(resName.size() < 100);
	strcpy(_resourceName, resName.c_str());
	vm->_dialogs->_pendingDialog = DIALOG_TEXTVIEW;
}

TextView::TextView(MADSEngine *vm) : MenuView(vm),
		_textSurface(MADS_SCREEN_WIDTH, MADS_SCREEN_HEIGHT + _vm->_font->getHeight()) {
	_animating = false;
	_panSpeed = 0;
	Common::fill(&_spareScreens[0], &_spareScreens[10], 0);
	_spareScreen = nullptr;
	_scrollCount = 0;
	_lineY = -1;
	_scrollTimeout = 0;
	_panCountdown = 0;
	_translationX = 0;
}

TextView::~TextView() {
	delete _spareScreen;
}

void TextView::load() {
	if (!_script.open(_resourceName))
		error("Could not open resource %s", _resourceName);

	processLines();
}

void TextView::processLines() {
	if (_script.eos())
		error("Attempted to read past end of response file");

	while (!_script.eos()) {
		_script.readLine(_currentLine, 79);

		// Commented out line, so go loop for another
		if (_currentLine[0] == '#')
			continue;

		// Process the line
		char *cStart = strchr(_currentLine, '[');
		if (cStart) {
			while (cStart) {
				// Loop for possible multiple commands on one line
				char *cEnd = strchr(_currentLine, ']');
				if (!cEnd)
					error("Unterminated command '%s' in response file", _currentLine);

				*cEnd = '\0';
				processCommand();

				// Copy rest of line (if any) to start of buffer
				strcpy(_currentLine, cEnd + 1);

				cStart = strchr(_currentLine, '[');
			}

			if (_currentLine[0]) {
				processText();
				break;
			}

		} else {
			processText();
			break;
		}
	}
}

void TextView::processCommand() {
	Scene &scene = _vm->_game->_scene;
	Common::String scriptLine(_currentLine + 1);
	scriptLine.toUppercase();
	const char *paramP;
	const char *commandStr = scriptLine.c_str();

	if (!strncmp(commandStr, "BACKGROUND", 10)) {
		// Set the background
		paramP = commandStr + 10;
		int screenId = getParameter(&paramP);
		
		SceneInfo *sceneInfo = SceneInfo::init(_vm);
		sceneInfo->load(screenId, 0, Common::String(), 0, scene._depthSurface,
			scene._backgroundSurface);
	
	} else if (!strncmp(commandStr, "GO", 2)) {
		_animating = true;

		// Grab what the final palete will be
		byte destPalette[PALETTE_SIZE];
		_vm->_palette->grabPalette(destPalette, 0, 256);

		// Copy the loaded background, if any, to the view surface
		//int yp = 22;
		//scene._backgroundSurface.copyTo(this, 0, 22);

		// Handle fade-in
		//byte srcPalette[768];
		//Common::fill(&srcPalette[0], &srcPalette[PALETTE_SIZE], 0);
		//_vm->_palette->fadeIn(srcPalette, destPalette, 0, PALETTE_COUNT, 0, 0,
		//	TV_FADE_DELAY_MILLI, TV_NUM_FADE_STEPS);
		_vm->_game->_fx = kTransitionFadeIn;

	} else if (!strncmp(commandStr, "PAN", 3)) {
		// Set panning values
		paramP = commandStr + 3;
		int panX = getParameter(&paramP);
		int panY = getParameter(&paramP);
		int panSpeed = getParameter(&paramP);

		if ((panX != 0) || (panY != 0)) {
			_pan = Common::Point(panX, panY);
			_panSpeed = panSpeed;
		}

	} else if (!strncmp(commandStr, "DRIVER", 6)) {
		// Set the driver to use
		paramP = commandStr + 6;
		int driverNum = getParameter(&paramP);
		_vm->_sound->init(driverNum);

	} else if (!strncmp(commandStr, "SOUND", 5)) {
		// Set sound number
		paramP = commandStr + 5;
		int soundId = getParameter(&paramP);
		_vm->_sound->command(soundId);

	} else if (!strncmp(commandStr, "COLOR", 5) && ((commandStr[5] == '0') || 
			(commandStr[5] == '1'))) {
		// Set the text colors
		int index = commandStr[5] - '0';
		paramP = commandStr + 6;

		byte palEntry[3];
		palEntry[0] = getParameter(&paramP) << 2;
		palEntry[1] = getParameter(&paramP) << 2;
		palEntry[2] = getParameter(&paramP) << 2;
		_vm->_palette->setPalette(&palEntry[0], 5 + index, 1);

	} else if (!strncmp(commandStr, "SPARE", 5)) {
		// Sets a secondary background number that can be later switched in with a PAGE command
		paramP = commandStr + 6;
		int spareIndex = commandStr[5] - '0';
		if ((spareIndex >= 0) && (spareIndex <= 9)) {
			int screenId = getParameter(&paramP);

			_spareScreens[spareIndex] = screenId;
		}

	} else if (!strncmp(commandStr, "PAGE", 4)) {
		// Signals to change to a previous specified secondary background
		paramP = commandStr + 4;
		int spareIndex = getParameter(&paramP);

		// Only allow background switches if one isn't currently in progress
		if (!_spareScreen && (_spareScreens[spareIndex] != 0)) {
			_spareScreen = new MSurface(MADS_SCREEN_WIDTH, MADS_SCREEN_HEIGHT);
			//_spareScreen->loadBackground(_spareScreens[spareIndex], &_bgSpare);

			_translationX = 0;
		}

	} else {
		error("Unknown response command: '%s'", commandStr);
	}
}

int TextView::getParameter(const char **paramP) {
	if ((**paramP != '=') && (**paramP != ','))
		return 0;

	int result = 0;
	++*paramP;
	while ((**paramP >= '0') && (**paramP <= '9')) {
		result = result * 10 + (**paramP - '0');
		++*paramP;
	}

	return result;
}

void TextView::processText() {
	int lineWidth, xStart;

	if (!strcmp(_currentLine, "***")) {
		// Special signifier for end of script
		_scrollCount = _vm->_font->getHeight() * 13;
		_lineY = -1;
		return;
	}

	_lineY = 0;

	// Lines are always centered, except if line contains a '@', in which case the
	// '@' marks the position that must be horizontally centered
	char *centerP = strchr(_currentLine, '@');
	if (centerP) {
		*centerP = '\0';
		xStart = (MADS_SCREEN_WIDTH / 2) - _vm->_font->getWidth(_currentLine);

		// Delete the @ character and shift back the remainder of the string
		char *p = centerP + 1;
		if (*p == ' ') ++p;
		strcpy(centerP, p);

	} else {
		lineWidth = _vm->_font->getWidth(_currentLine);
		xStart = (MADS_SCREEN_WIDTH - lineWidth) / 2;
	}

	// Copy the text line onto the bottom of the textSurface surface, which will allow it
	// to gradually scroll onto the screen
	int yp = _textSurface.h - _vm->_font->getHeight() - TEXTVIEW_LINE_SPACING;
	_textSurface.fillRect(Common::Rect(0, yp, MADS_SCREEN_WIDTH, _textSurface.h), 0);
	_vm->_font->writeString(&_textSurface, _currentLine, Common::Point(xStart, yp));
}

/*------------------------------------------------------------------------*/

char AnimationView::_resourceName[100];

void AnimationView::execute(MADSEngine *vm, const Common::String &resName) {
	assert(resName.size() < 100);
	strcpy(_resourceName, resName.c_str());
	vm->_dialogs->_pendingDialog = DIALOG_ANIMVIEW;
}

AnimationView::AnimationView(MADSEngine *vm) : MenuView(vm) {
	_soundDriverLoaded = false;
}

void AnimationView::load() {
	Common::String resName(_resourceName);
	if (!resName.hasSuffix("."))
		resName += ".res";

	if (!_script.open(resName))
		error("Could not open resource %s", resName.c_str());

	processLines();
}

bool AnimationView::onEvent(Common::Event &event) {
	// Wait for the Escape key or a mouse press
	if (((event.type == Common::EVENT_KEYDOWN) && (event.kbd.keycode == Common::KEYCODE_ESCAPE)) ||
			(event.type == Common::EVENT_RBUTTONUP)) {
		scriptDone();
		return true;
	}

	return false;
}

void AnimationView::doFrame() {
	Scene &scene = _vm->_game->_scene;
	int bgNumber = 0;

	// Only update state if wait period has expired
	if (_previousUpdate > 0) {
		if (g_system->getMillis() - _previousUpdate < 3000) {
			return;
		} else {
			// time for an update
			_previousUpdate = g_system->getMillis();
		}
	} else {
		_previousUpdate = g_system->getMillis();
		return;
	}

	char bgFile[10];
	strncpy(bgFile, _currentFile, 5);
	bgFile[0] = bgFile[2];
	bgFile[1] = bgFile[3];
	bgFile[2] = bgFile[4];
	bgFile[3] = '\0';
	bgNumber = atoi(bgFile);
	sprintf(bgFile, "rm%i.art", bgNumber);

	// Not all scenes have a background. If there is one, refresh it
	if (Common::File::exists(bgFile)) {
		_vm->_palette->resetGamePalette(4, 8);
		SceneInfo *sceneInfo = SceneInfo::init(_vm);
		sceneInfo->load(bgNumber, 0, Common::String(), 0, scene._depthSurface,
			scene._backgroundSurface);
	}

	// Read next line
	processLines();
}

void AnimationView::scriptDone() {
	_breakFlag = true;
	_vm->_dialogs->_pendingDialog = DIALOG_MAIN_MENU;
}

void AnimationView::processLines() {
	if (_script.eos()) {
		// end of script, end animation
		scriptDone();
		return;
	}

	while (!_script.eos()) {
		_script.readLine(_currentLine, 79);

		// Process the line
		char *cStart = strchr(_currentLine, '-');
		if (cStart) {
			while (cStart) {
				// Loop for possible multiple commands on one line
				char *cEnd = strchr(_currentLine, ' ');
				if (!cEnd)
					error("Unterminated command '%s' in response file", _currentLine);

				*cEnd = '\0';
				processCommand();

				// Copy rest of line (if any) to start of buffer
				// Don't use strcpy() here, because if the
				// rest of the line is the longer of the two
				// strings, the memory areas will overlap.
				memmove(_currentLine, cEnd + 1, strlen(cEnd + 1) + 1);

				cStart = strchr(_currentLine, '-');
			}

			if (_currentLine[0]) {
				sprintf(_currentFile, "%s", _currentLine);
				//printf("File: %s\n", _currentLine);
				break;
			}

		} else {
			sprintf(_currentFile, "%s", _currentLine);
			warning("File: %s\n", _currentLine);
			break;
		}
	}
}

void AnimationView::processCommand() {
	Common::String commandLine(_currentLine + 1);
	commandLine.toUppercase();
	const char *commandStr = commandLine.c_str();
	const char *param = commandStr;

	if (!strncmp(commandStr, "X", 1)) {
		//printf("X ");
	} else if (!strncmp(commandStr, "W", 1)) {
		//printf("W ");
	} else if (!strncmp(commandStr, "R", 1)) {
		param = param + 2;
		//printf("R:%s ", param);
	} else if (!strncmp(commandStr, "O", 1)) {
		// Set the transition effect
		param = param + 2;
		_vm->_game->_fx = (ScreenTransition)atoi(param);
	} else {
		error("Unknown response command: '%s'", commandStr);
	}
}

} // End of namespace Nebular

} // End of namespace MADS
