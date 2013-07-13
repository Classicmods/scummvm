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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "base/plugins.h"
#include "engines/advancedDetector.h"

#include "mortevielle/mortevielle.h"
#include "mortevielle/detection_tables.h"
#include "mortevielle/saveload.h"

namespace Mortevielle {
uint32 MortevielleEngine::getGameFlags() const { return _gameDescription->flags; }

Common::Language MortevielleEngine::getLanguage() const { return _gameDescription->language; }

}

static const PlainGameDescriptor MortevielleGame[] = {
	{"mortevielle", "Mortville Manor"},
	{0, 0}
};

class MortevielleMetaEngine : public AdvancedMetaEngine {
public:
	MortevielleMetaEngine() : AdvancedMetaEngine(Mortevielle::MortevielleGameDescriptions, sizeof(ADGameDescription),
		MortevielleGame) {
		_md5Bytes = 512;
		_singleid = "mortevielle";
	}
	virtual const char *getName() const {
		return "Mortevielle";
	}

	virtual const char *getOriginalCopyright() const {
		return "Mortville Manor (C) 1987-89 Lankhor";
	}

	virtual bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const;
	virtual bool hasFeature(MetaEngineFeature f) const;
	virtual int getMaximumSaveSlot() const;
	virtual SaveStateList listSaves(const char *target) const;
	virtual SaveStateDescriptor querySaveMetaInfos(const char *target, int slot) const;
};

bool MortevielleMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	if (desc) {
		*engine = new Mortevielle::MortevielleEngine(syst, desc);
	}
	return desc != 0;
}

bool MortevielleMetaEngine::hasFeature(MetaEngineFeature f) const {
	switch (f) {
	case kSupportsListSaves:
	case kSupportsDeleteSave:
	case kSupportsLoadingDuringStartup:
	case kSavesSupportMetaInfo:
	case kSavesSupportThumbnail:
	case kSavesSupportCreationDate:
		return true;
	default:
		return false;
	}
}

int MortevielleMetaEngine::getMaximumSaveSlot() const { return 99; }

SaveStateList MortevielleMetaEngine::listSaves(const char *target) const {
	return Mortevielle::SavegameManager::listSaves(target);
}

SaveStateDescriptor MortevielleMetaEngine::querySaveMetaInfos(const char *target, int slot) const {
	Common::String filename = Mortevielle::MortevielleEngine::generateSaveFilename(target, slot);
	return Mortevielle::SavegameManager::querySaveMetaInfos(filename);
}


#if PLUGIN_ENABLED_DYNAMIC(MORTEVIELLE)
	REGISTER_PLUGIN_DYNAMIC(MORTEVIELLE, PLUGIN_TYPE_ENGINE, MortevielleMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(MORTEVIELLE, PLUGIN_TYPE_ENGINE, MortevielleMetaEngine);
#endif
