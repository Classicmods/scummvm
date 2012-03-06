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

/*
 * This file is based on WME Lite.
 * http://dead-code.org/redir.php?target=wmelite
 * Copyright (c) 2011 Jan Nedoma
 */

#include "dcgf.h"
#include "BFont.h"
#include "BFontBitmap.h"
#include "BParser.h"
#include "BFileManager.h"
#include "BFontTT.h"
#include "BGame.h"

namespace WinterMute {

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_PERSISTENT(CBFont, false)

//////////////////////////////////////////////////////////////////////
CBFont::CBFont(CBGame *inGame): CBObject(inGame) {

}


//////////////////////////////////////////////////////////////////////
CBFont::~CBFont() {
}


//////////////////////////////////////////////////////////////////////
void CBFont::DrawText(byte  *text, int x, int y, int width, TTextAlign align, int max_height, int MaxLenght) {
}


//////////////////////////////////////////////////////////////////////
int CBFont::GetTextHeight(byte  *text, int width) {
	return 0;
}


//////////////////////////////////////////////////////////////////////
int CBFont::GetTextWidth(byte  *text, int MaxLenght) {
	return 0;
}

/*
//////////////////////////////////////////////////////////////////////
HRESULT CBFont::LoadFile(char * Filename)
{
    BYTE* Buffer = Game->m_FileManager->ReadWholeFile(Filename);
    if(Buffer==NULL){
        Game->LOG(0, "CBFont::LoadFile failed for file '%s'", Filename);
        return E_FAIL;
    }

    HRESULT ret;

    m_Filename = new char [strlen(Filename)+1];
    strcpy(m_Filename, Filename);

    if(FAILED(ret = LoadBuffer(Buffer))) Game->LOG(0, "Error parsing FONT file '%s'", Filename);

    delete [] Buffer;

    return ret;
}


TOKEN_DEF_START
  TOKEN_DEF (FONT)
TOKEN_DEF_END
//////////////////////////////////////////////////////////////////////
HRESULT CBFont::LoadBuffer(byte  * Buffer)
{
    TOKEN_TABLE_START(commands)
        TOKEN_TABLE (FONT)
    TOKEN_TABLE_END

    char* params;
    int cmd;
    CBParser parser(Game);

    if(parser.GetCommand ((char**)&Buffer, commands, (char**)&params)!=TOKEN_FONT){
        Game->LOG(0, "'FONT' keyword expected.");
        return E_FAIL;
    }
    Buffer = (byte *)params;

    while ((cmd = parser.GetCommand ((char**)&Buffer, commands, (char**)&params)) > 0)
    {
        switch (cmd)
        {
            case TOKEN_IMAGE:
                surface_file = (char*)params;
            break;

            case TOKEN_TRANSPARENT:
                parser.ScanStr(params, "%d,%d,%d", &r, &g, &b);
                custom_trans = true;
            break;
        }


    }
    if (cmd == PARSERR_TOKENNOTFOUND){
        Game->LOG(0, "Syntax error in FONT definition");
        return E_FAIL;
    }

    return S_OK;
}
*/

//////////////////////////////////////////////////////////////////////////
int CBFont::GetLetterHeight() {
	return 0;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFont::Persist(CBPersistMgr *PersistMgr) {

	CBObject::Persist(PersistMgr);
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
CBFont *CBFont::CreateFromFile(CBGame *Game, char *Filename) {
	if (IsTrueType(Game, Filename)) {
		CBFontTT *Font = new CBFontTT(Game);
		if (Font) {
			if (FAILED(Font->LoadFile(Filename))) {
				delete Font;
				return NULL;
			}
		}
		return Font;
	} else {
		CBFontBitmap *Font = new CBFontBitmap(Game);
		if (Font) {
			if (FAILED(Font->LoadFile(Filename))) {
				delete Font;
				return NULL;
			}
		}
		return Font;
	}
}


TOKEN_DEF_START
TOKEN_DEF(FONT)
TOKEN_DEF(TTFONT)
TOKEN_DEF_END
//////////////////////////////////////////////////////////////////////////
bool CBFont::IsTrueType(CBGame *Game, char *Filename) {
	TOKEN_TABLE_START(commands)
	TOKEN_TABLE(FONT)
	TOKEN_TABLE(TTFONT)
	TOKEN_TABLE_END


	byte *Buffer = Game->m_FileManager->ReadWholeFile(Filename);
	if (Buffer == NULL) return false;

	byte *WorkBuffer = Buffer;

	char *params;
	CBParser parser(Game);

	bool Ret = false;
	if (parser.GetCommand((char **)&WorkBuffer, commands, (char **)&params) == TOKEN_TTFONT)
		Ret = true;

	delete [] Buffer;
	return Ret;
}

} // end of namespace WinterMute
