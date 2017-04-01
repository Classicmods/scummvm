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
 *
 */


#include "graphics/macgui/macwindowmanager.h"
#include "graphics/macgui/macfontmanager.h"
#include "graphics/macgui/mactextwindow.h"

namespace Graphics {

MacTextWindow::MacTextWindow(MacWindowManager *wm) :
		MacWindow(wm->getNextId(), true, true, true, wm) {
	wm->addWindowInitialized(this);
}

MacTextWindow::~MacTextWindow() {
}

const Font *MacTextWindow::getTextWindowFont() {
	// TODO: make this have an actual effect
	return _wm->_fontMan->getFont(Graphics::MacFont(kMacFontChicago, 8));
}

} // End of namespace Graphics
