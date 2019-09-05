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

#ifndef TITANIC_PET_SOUND_H
#define TITANIC_PET_SOUND_H

#include "titanic/pet_control/pet_glyphs.h"
#include "titanic/pet_control/pet_gfx_element.h"
#include "titanic/gfx/text_control.h"
#include "titanic/pet_control/pet_slider.h"

namespace Titanic {

class CPetRealLife;

class CPetSound : public CPetGlyph {
	enum SliderType {
		MASTER_SLIDER = 0, MUSIC_SLIDER = 1, PARROT_SLIDER = 2, SPEECH_SLIDER = 3
	};
private:
	CPetGfxElement _element;
	CPetSoundSlider _masterVolume;
	CPetSoundSlider _musicVolume;
	CPetSoundSlider _parrotVolume;
	CPetSoundSlider _speechVolume;
	CTextControl _textMasterVolume;
	CTextControl _textMusicVolume;
	CTextControl _textParrotVolume;
	CTextControl _textSpeechVolume;
	CPetSlider *_draggingSlider;
	SliderType _draggingSliderNum;
private:
	/**
	 * Called when a slider has changed
	 */
	void sliderChanged(double offset, SliderType sliderNum);
public:
	CPetSound();

	/**
	 * Setup the glyph
	 */
	virtual bool setup(CPetControl *petControl, CPetGlyphs *owner);

	/**
	 * Reset the glyph
	 */
	virtual bool reset();

	/**
	 * Handles any secondary drawing of the glyph
	 */
	virtual void draw2(CScreenManager *screenManager);

	/**
	 * Called for mouse button down messages
	 */
	virtual bool MouseButtonDownMsg(const Point &pt);

	/**
	 * Called when mouse drag starts
	 */
	virtual bool MouseDragStartMsg(CMouseDragStartMsg *msg);

	/**
	 * Called during mouse drags
	 */
	virtual bool MouseDragMoveMsg(CMouseDragMoveMsg *msg);

	/**
	 * Called when mouse drag ends
	 */
	virtual bool MouseDragEndMsg(CMouseDragEndMsg *msg);

	/**
	 * Handles mouse button up messages
	 */
	virtual bool MouseButtonUpMsg(const Point &pt);

	/**
	 * Highlight any currently highlighted element
	 */
	virtual void highlightCurrent(const Point &pt) {
		setSliders();
	}

	/**
	 * Returns the tooltip text for when the glyph is selected
	 */
	virtual void getTooltip(CTextControl *text);

	/**
	 * Sets the positions of the volume sliders
	 */
	void setSliders();
};

} // End of namespace Titanic

#endif /* TITANIC_PET_SOUND_H */