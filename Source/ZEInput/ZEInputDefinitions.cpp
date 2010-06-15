//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - InputDefinitions.cpp
 ------------------------------------------------------------------------------
 Copyright (C) 2008-2021 Yiğit Orçun GÖKBULUT. All rights reserved.

 This file is part of the Zinek Engine  Software. Zinek Engine Software and the
 accompanying  materials are  made available  under the  terms of Zinek  Engine
 Commercial License or the GNU General Public License Version 3.

                      ZINEK ENGINE COMMERCIAL LICENSE
 Licensees  holding  valid  Zinek Engine Commercial  License(s) may  use  Zinek
 Engine  Software in  accordance  with   the  commercial  license  agreement(s)
 (which can only be  issued  by  copyright  owner "Yiğit  Orçun  GÖKBULUT") and
 provided with the Software  or, alternatively,  in  accordance with the  terms
 contained  in  a  written  agreement  between  you  and  copyright  owner. For
 licensing  terms  and conditions  please  contact  with  copyright owner.

                    GNU GENERAL PUBLIC LICENSE VERSION 3
 This program is free software: you can  redistribute it and/or modify it under
 the terms of the GNU General Public  License as published by the Free Software
 Foundation, either  version 3 of  the License, or  (at your option)  any later
 version. This program is  distributed in the hope that  it will be useful, but
 WITHOUT ANY WARRANTY; without even the  implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See  the GNU General Public License for more
 details. You  should have received  a copy of the  GNU General  Public License
 along with this program. If not, see <https://www.gnu.org/licenses/>.

 Copyright Owner Information:
  Name: Yiğit Orçun GÖKBULUT
  Contact: orcun.gokbulut@gmail.com
  Github: https://www.github.com/orcun-gokbulut/ZE
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

#include "InputDefinitions.h"
#include <string.h>

#define ZE_MAX_KEYIDNAME			0x100


const char* KeyNames[ZE_MAX_KEYIDNAME][2] =
{
	{"ESCAPE", "Escape"},					// 0x01
	{"1", "1"},								// 0x02
	{"2", "2"},								// 0x03
	{"3", "3"},								// 0x04
	{"4", "4"},								// 0x05
	{"5", "5"},								// 0x06
	{"6", "6"},								// 0x07
	{"7", "7"},								// 0x08
	{"8", "8"},								// 0x09
	{"9", "9"},								// 0x0A
	{"0", "0"},								// 0x0B
	{"MINUS", "Minus"},						// 0x0C
	{"EQUALS", "Equals"},					// 0x0D
	{"BACK", "Back"},						// 0x0E 
	{"TAB", "Tab"},							// 0x0F
	{"Q", "Q"},								// 0x10
	{"W", "W"},								// 0x11
	{"E", "E"},								// 0x12
	{"R", "R"},								// 0x13
	{"T", "T"},								// 0x14
	{"Y", "Y"},								// 0x15
	{"U", "U"},								// 0x16
	{"I", "I"},								// 0x17
	{"O", "O"},								// 0x18
	{"P", "P"},								// 0x19
	{"LBRACKET", "Left Bracelet"},			// 0x1A
	{"RBRACKET", "Right Bracelet"},			// 0x1B
	{"RETURN", "Return"},					// 0x1C
	{"LCONTROL", "Left Control"},			// 0x1D
	{"A", "A"},								// 0x1E
	{"S", "S"},								// 0x1F
	{"D", "D"},								// 0x20
	{"F", "F"},								// 0x21
	{"G", "G"},								// 0x22
	{"H", "H"},								// 0x23
	{"J", "J"},								// 0x24
	{"K", "K"},								// 0x25
	{"L", "L"},								// 0x26
	{"SEMICOLON", "Semicolon"},				// 0x27
	{"APOSTROPHE", "Apostrophe"},			// 0x28
	{"GRAVE", "Grave"},						// 0x29
	{"LSHIFT", "Left Shift"},				// 0x2A
	{"BACKSLASH", "Backslash"},				// 0x2B
	{"Z", "Z"},								// 0x2C
	{"X", "X"},								// 0x2D
	{"C", "C"},								// 0x2E
	{"V", "V"},								// 0x2F
	{"B", "B"},								// 0x30
	{"N", "N"},								// 0x31
	{"M", "M"},								// 0x32
	{"COMMA", "Comma"},						// 0x33
	{"PERIOD", "Period"},					// 0x34
	{"SLASH", "Slash"},						// 0x35
	{"RSHIFT", "Right Shift"},				// 0x36
	{"MULTIPLY", "Multiply"},				// 0x37
	{"LMENU", "Left Menu"},					// 0x38 
	{"SPACE", "Space"},						// 0x39
	{"CAPITAL", "Capital"},					// 0x3A
	{"F1", "F1"},							// 0x3B
	{"F2", "F2"},							// 0x3C
	{"F3", "F3"},							// 0x3D
	{"F4", "F4"},							// 0x3E
	{"F5", "F5"},							// 0x3F
	{"F6", "F6"},							// 0x40
	{"F7", "F7"},							// 0x41
	{"F8", "F8"},							// 0x42
	{"F9", "F9"},							// 0x43
	{"F10", "F10"},							// 0x44
	{"NUMLOCK", "Number Lock"},				// 0x45
	{"SCROLL", "Scroll"},					// 0x46
	{"NUMPAD7", "Numeric Pad 7"},			// 0x47
	{"NUMPAD8", "Numeric Pad 8"},			// 0x48
	{"NUMPAD9", "Numeric Pad 9"},			// 0x49
	{"SUBTRACT", "-"},						// 0x4A
	{"NUMPAD4", "Numeric Pad 4"},			// 0x4B
	{"NUMPAD5", "Numeric Pad 5"},			// 0x4C
	{"NUMPAD6", "Numeric Pad 6"},			// 0x4D
	{"ADD", "+"},							// 0x4E
	{"NUMPAD1", "Numeric Pad 1"},			// 0x4F
	{"NUMPAD2", "Numeric Pad 2"},			// 0x50
	{"NUMPAD3", "Numeric Pad 3"},			// 0x51
	{"NUMPAD0", "Numeric Pad 0"},			// 0x52
	{"DECIMAL", "Decimal"},					// 0x53 
	{"OEM_102", "Oem 102"},					// 0x56
	{"F11", "F11"},							// 0x57
	{"F12", "F12"},							// 0x58
	{"0x59", "0x59"},						// 0x59
	{"0x5A", "0x5A"},						// 0x5A
	{"0x5B", "0x5B"},						// 0x5B
	{"0x5C", "0x5C"},						// 0x5C
	{"0x5D", "0x5D"},						// 0x5D
	{"0x5E", "0x5E"},						// 0x5E
	{"0x5F", "0x5F"},						// 0x5F
	{"0x60", "0x60"},						// 0x60
	{"0x61", "0x61"},						// 0x61
	{"0x62", "0x62"},						// 0x62
	{"0x63", "0x63"},						// 0x63
	{"F13", "F13"},							// 0x64
	{"F14", "F14"},							// 0x65
	{"F15", "F15"},							// 0x66
	{"0x67", "0x67"},						// 0x67
	{"0x68", "0x68"},						// 0x68
	{"0x69", "0x69"},						// 0x69
	{"0x6A", "0x6A"},						// 0x6A
	{"0x6B", "0x6B"},						// 0x6B
	{"0x6C", "0x6C"},						// 0x6C
	{"0x6D", "0x6D"},						// 0x6D
	{"0x6E", "0x6E"},						// 0x6E
	{"0x6F", "0x6F"},						// 0x6F
	{"KANA", "Kana"},						// 0x70
	{"0x71", "0x71"},						// 0x71
	{"0x72", "0x72"},						// 0x72
	{"ABNT_C1", "Abnt C1"},					// 0x73
	{"0x74", "0x74"},						// 0x74
	{"0x75", "0x75"},						// 0x75
	{"0x76", "0x76"},						// 0x76
	{"0x77", "0x77"},						// 0x77
	{"0x78", "0x78"},						// 0x78
	{"CONVERT", "Convert"},					// 0x79 
	{"0x7A", "0x7A"},						// 0x7A
	{"NOCONVERT", "No Convert"},			// 0x7B
	{"0x7C", "0x7C"},						// 0x7C
	{"YEN", "Yen"},							// 0x7D
	{"ABNT_C2", "Abtn C2"},					// 0x7E
	{"0x7F", "0x7F"},						// 0x7F
	{"0x80", "0x80"},						// 0x80
	{"0x81", "0x81"},						// 0x81
	{"0x82", "0x82"},						// 0x82
	{"0x83", "0x83"},						// 0x83
	{"0x84", "0x84"},						// 0x84 
	{"0x85", "0x85"},						// 0x85
	{"0x86", "0x86"},						// 0x86
	{"0x87", "0x87"},						// 0x87
	{"0x88", "0x88"},						// 0x88
	{"0x89", "0x89"},						// 0x89
	{"0x8A", "0x8A"},						// 0x8A
	{"0x8B", "0x8B"},						// 0x8B
	{"0x8C", "0x8C"},						// 0x8C
	{"NUMPADEQUALS", "Numeric Pad ="},		// 0x8D
	{"0x8E", "0x8E"},						// 0x8E
	{"0x8F", "0x8F"},						// 0x8F
	{"PREVTRACK", "Previous Track"},		// 0x90
	{"AT", "At"},							// 0x91
	{"COLON", "Colon"},						// 0x92
	{"UNDERLINE", "Underline"},				// 0x93
	{"KANJI", "Kanji"},						// 0x94
	{"STOP", "Stop"},						// 0x95
	{"AX", "Ax"},							// 0x96 
	{"UNLABELED", "Unabled"},				// 0x97
	{"NEXTTRACK", "Next Track"},			// 0x99
	{"0x9A", "0x9A"},						// 0x9A
	{"0x9B", "0x9B"},						// 0x9B
	{"NUMPADENTER", "Numeric Pad Return"},	// 0x9C
	{"RCONTROL", "Right Control"},			// 0x9D
	{"0x9E", "0x9E"},						// 0x9E
	{"0x9F", "0x9F"},						// 0x9F
	{"MUTE", "Mute"},						// 0xA0
	{"CALCULATOR", "Calculator"},			// 0xA1
	{"PLAYPAUSE", "Play / Pause"},			// 0xA2
	{"MEDIASTOP", "Media Stop"},			// 0xA4
	{"0xA3", "0xA3"},						// 0xA3
	{"0xA4", "0xA4"},						// 0xA4
	{"0xA5", "0xA5"},						// 0xA5
	{"0xA6", "0xA6"},						// 0xA6
	{"0xA7", "0xA7"},						// 0xA7
	{"0xA8", "0xA8"},						// 0xA8
	{"0xA9", "0xA9"},						// 0xA9
	{"0xAA", "0xAA"},						// 0xAA
	{"0xAB", "0xAB"},						// 0xAB
	{"0xAC", "0xAC"},						// 0xAC
	{"0xAD", "0xAD"},						// 0xAD
	{"0xAE", "0xAE"},						// 0xAE
	{"0xAF", "0xAF"},						// 0xAF
	{"VOLUMEDOWN", "Volume Down"},			// 0xAE
	{"VOLUMEUP", "Volume Up"},				// 0xB0
	{"0xB1", "0xB1"},						// 0xB1
	{"WEBHOME", "Web Home"},				// 0xB2
	{"NUMPADCOMMA", "Numeric Pad Comma"},	// 0xB3
	{"0xB4", "0xB4"},						// 0xB4
	{"DIVIDE", "/"},						// 0xB5
	{"0xB6", "0xB6"},						// 0xB6
	{"SYSRQ", "Sysrq"},						// 0xB7
	{"RMENU", "Right Menu"},				// 0xB8
	{"0xB9","0xB9"},						// 0xB9
	{"0xBA","0xBA"},						// 0xBA
	{"0xBB","0xBB"},						// 0xBB
	{"0xBC","0xBC"},						// 0xBC
	{"0xBD","0xBD"},						// 0xBD
	{"0xBE","0xBE"},						// 0xBE
	{"0xBF","0xBF"},						// 0xBF
	{"0xC0","0xC0"},						// 0xC0
	{"0xC1","0xC1"},						// 0xC1
	{"0xC2","0xC2"},						// 0xC2
	{"0xC3","0xC3"},						// 0xC3
	{"0xC4","0xC4"},						// 0xC4
	{"PAUSE", "Pause"},						// 0xC5 
	{"HOME", "Home"},						// 0xC7
	{"UP", "Up Arrow"},						// 0xC8 
	{"PRIOR", "Prior"},						// 0xC9
	{"0xCA","0xCA"},						// 0xCA
	{"LEFT", "Left Arrow"},					// 0xCB 
	{"RIGHT", "Right Arrow"},				// 0xCD 
	{"0xCE","0xCE"},						// 0xCE
	{"END", "End"},							// 0xCF
	{"DOWN", "Down Arrow"},					// 0xD0
	{"NEXT", "Next"},						// 0xD1
	{"INSERT", "Insert"},					// 0xD2
	{"DELETE", "Delete"},					// 0xD3
	{"0xD4","0xD4"},						// 0xD4
	{"0xD5","0xD5"},						// 0xD5
	{"0xD6","0xD6"},						// 0xD6
	{"0xD7","0xD7"},						// 0xD7
	{"0xD8","0xD8"},						// 0xD8
	{"0xD9","0xD9"},						// 0xD9
	{"0xDA","0xDA"},						// 0xDA
	{"LWIN", "Left Windows"},				// 0xDB
	{"RWIN", "Right Windows"},				// 0xDC 
	{"APPS", "Apps"},						// 0xDD
	{"POWER", "Power"},						// 0xDE
	{"SLEEP", "Sleep"},						// 0xDF
	{"0xE0","0xE0"},						// 0xE0
	{"0xE1","0xE1"},						// 0xE1
	{"0xE2","0xE2"},						// 0xE2
	{"WAKE", "Wake"},						// 0xE3
	{"WEBSEARCH", "Web Search"},			// 0xE5
	{"WEBFAVORITES", "Web Favorites"},		// 0xE6
	{"WEBREFRESH", "Web Refresh"},			// 0xE7
	{"WEBSTOP", "Web Stop"},				// 0xE8
	{"WEBFORWARD", "Web Forvard"},			// 0xE9
	{"WEBBACK", "Web Back"},				// 0xEA 
	{"MYCOMPUTER", "My Computer"},			// 0xEB
	{"MAIL", "Mail"},						// 0xEC 
	{"MEDIASELECT", "Media Select"},		// 0xED
	{"0xEE","0xEE"},						// 0xEE
	{"0xEF","0xEF"},						// 0xEF
	{"0xF0","0xF0"},						// 0xF0
	{"0xF1","0xF1"},						// 0xF1
	{"0xF2","0xF2"},						// 0xF2
	{"0xF3","0xF3"},						// 0xF3
	{"0xF4","0xF4"},						// 0xF4
	{"0xF5","0xF5"},						// 0xF5
	{"0xF6","0xF6"},						// 0xF6
	{"0xF7","0xF7"},						// 0xF7
	{"0xF8","0xF8"},						// 0xF8
	{"0xF9","0xF9"},						// 0xF9
	{"0xFA","0xFA"},						// 0xFA
	{"0xFB","0xFB"},						// 0xFB
	{"0xFC","0xFC"},						// 0xFC
	{"0xFD","0xFD"},						// 0xFD
	{"0xFE","0xFE"},						// 0xFE
	{"0xFF","0xFF"},						// 0xFF
};

unsigned char zeGetKeyboardButtonId(char* KeyShortName)
{
	for (size_t I = 0; I < ZE_MAX_KEYIDNAME; I++)
		if (stricmp(KeyNames[I][0], KeyShortName) == 0)
			return I;
	return 0;
}

const char* zeGetKeyboardButtonName(unsigned char Key)
{
	return KeyNames[Key][1];
}

const char* zeGetKeyboardButtonShortName(unsigned char Key)
{
	return KeyNames[Key][0];
}
