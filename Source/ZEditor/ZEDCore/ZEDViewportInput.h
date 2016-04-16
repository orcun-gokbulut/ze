//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDViewportInput.h
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

#pragma once

#include "ZETypes.h"
#include "ZEDS/ZEString.h"

enum ZEDMouseButton
{
	ZED_MB_NONE						= 0x00000000,
	ZED_MB_LEFT						= 0x00000001,
	ZED_MB_RIGHT					= 0x00000002,
	ZED_MB_MIDDLE					= 0x00000004,
	ZED_MB_BACK						= 0x00000010,
	ZED_MB_FORWARD					= 0x00000020,
	ZED_MB_EXTRA_0					= 0x00000100,
	ZED_MB_EXTRA_2					= 0x00000200,
	ZED_MB_EXTRA_3					= 0x00000400,
	ZED_MB_EXTRA_4					= 0x00000800,
	ZED_MB_EXTRA_5					= 0x00001000,
	ZED_MB_EXTRA_6					= 0x00002000,
	ZED_MB_EXTRA_7					= 0x00004000,
	ZED_MB_EXTRA_8					= 0x00008000,
	ZED_MB_EXTRA_9					= 0x00010000,
	ZED_MB_EXTRA_10					= 0x00020000,
	ZED_MB_EXTRA_11					= 0x00040000,
};

enum ZEDKeyboardKeyModifier
{
	ZED_KKM_CTRL					= 0x00010000,
	ZED_KKM_ALT						= 0x00020000,
	ZED_KKM_SHIFT					= 0x00040000,
	ZED_KKM_WINDOWS					= 0x00080000,
	ZED_KKM_KEYPAD					= 0x00100000,
	ZED_KKM_CAPS_LOCK				= 0x01000000,
	ZED_KKM_NUM_LOCK				= 0x02000000,
	ZED_KKM_SCROLL_LOCK				= 0x04000000,
};

enum ZEDKeyboardKey
{
	ZED_KK_ESCAPE					= 0x01,
	ZED_KK_1						= 0x02,
	ZED_KK_2						= 0x03,
	ZED_KK_3						= 0x04,
	ZED_KK_4						= 0x05,
	ZED_KK_5						= 0x06,
	ZED_KK_6						= 0x07,
	ZED_KK_7						= 0x08,
	ZED_KK_8						= 0x09,
	ZED_KK_9						= 0x0A,
	ZED_KK_0						= 0x0B,
	ZED_KK_MINUS					= 0x0C,
	ZED_KK_EQUALS					= 0x0D,
	ZED_KK_BACK						= 0x0E, 
	ZED_KK_TAB						= 0x0F,
	ZED_KK_Q						= 0x10,
	ZED_KK_W						= 0x11,
	ZED_KK_E						= 0x12,
	ZED_KK_R						= 0x13,
	ZED_KK_T						= 0x14,
	ZED_KK_Y						= 0x15,
	ZED_KK_U						= 0x16,
	ZED_KK_I						= 0x17,
	ZED_KK_O						= 0x18,
	ZED_KK_P						= 0x19,
	ZED_KK_LBRACKET					= 0x1A,
	ZED_KK_RBRACKET					= 0x1B,
	ZED_KK_RETURN					= 0x1C,
	ZED_KK_LCONTROL					= 0x1D,
	ZED_KK_A						= 0x1E,
	ZED_KK_S						= 0x1F,
	ZED_KK_D						= 0x20,
	ZED_KK_F						= 0x21,
	ZED_KK_G						= 0x22,
	ZED_KK_H						= 0x23,
	ZED_KK_J						= 0x24,
	ZED_KK_K						= 0x25,
	ZED_KK_L						= 0x26,
	ZED_KK_SEMICOLON				= 0x27,
	ZED_KK_APOSTROPHE				= 0x28,
	ZED_KK_GRAVE					= 0x29,
	ZED_KK_LSHIFT					= 0x2A,
	ZED_KK_BACKSLASH				= 0x2B,
	ZED_KK_Z						= 0x2C,
	ZED_KK_X						= 0x2D,
	ZED_KK_C						= 0x2E,
	ZED_KK_V						= 0x2F,
	ZED_KK_B						= 0x30,
	ZED_KK_N						= 0x31,
	ZED_KK_M						= 0x32,
	ZED_KK_COMMA					= 0x33,
	ZED_KK_PERIOD					= 0x34,
	ZED_KK_SLASH					= 0x35,
	ZED_KK_RSHIFT					= 0x36,
	ZED_KK_MULTIPLY					= 0x37,
	ZED_KK_LMENU					= 0x38, 
	ZED_KK_SPACE					= 0x39,
	ZED_KK_CAPITAL					= 0x3A,
	ZED_KK_F1						= 0x3B,
	ZED_KK_F2						= 0x3C,
	ZED_KK_F3						= 0x3D,
	ZED_KK_F4						= 0x3E,
	ZED_KK_F5						= 0x3F,
	ZED_KK_F6						= 0x40,
	ZED_KK_F7						= 0x41,
	ZED_KK_F8						= 0x42,
	ZED_KK_F9						= 0x43,
	ZED_KK_F10						= 0x44,
	ZED_KK_NUMLOCK					= 0x45,
	ZED_KK_SCROLL					= 0x46,
	ZED_KK_NUMPAD7					= 0x47,
	ZED_KK_NUMPAD8					= 0x48,
	ZED_KK_NUMPAD9					= 0x49,
	ZED_KK_SUBTRACT					= 0x4A,
	ZED_KK_NUMPAD4					= 0x4B,
	ZED_KK_NUMPAD5					= 0x4C,
	ZED_KK_NUMPAD6					= 0x4D,
	ZED_KK_ADD						= 0x4E,
	ZED_KK_NUMPAD1					= 0x4F,
	ZED_KK_NUMPAD2					= 0x50,
	ZED_KK_NUMPAD3					= 0x51,
	ZED_KK_NUMPAD0					= 0x52,
	ZED_KK_DECIMAL					= 0x53, 
	ZED_KK_OEM_102					= 0x56,
	ZED_KK_F11						= 0x57,
	ZED_KK_F12						= 0x58,
	ZED_KK_F13						= 0x64,
	ZED_KK_F14						= 0x65,
	ZED_KK_F15						= 0x66,
	ZED_KK_KANA						= 0x70,
	ZED_KK_ABNT_C1					= 0x73,
	ZED_KK_CONVERT					= 0x79, 
	ZED_KK_NOCONVERT				= 0x7B,
	ZED_KK_YEN						= 0x7D,
	ZED_KK_ABNT_C2					= 0x7E,
	ZED_KK_NUMPADEQUALS				= 0x8D,
	ZED_KK_PREVTRACK				= 0x90,
	ZED_KK_AT						= 0x91,
	ZED_KK_COLON					= 0x92,
	ZED_KK_UNDERLINE				= 0x93,
	ZED_KK_KANJI					= 0x94,
	ZED_KK_STOP						= 0x95,
	ZED_KK_AX						= 0x96, 
	ZED_KK_UNLABELED				= 0x97,
	ZED_KK_NEXTTRACK				= 0x99, 
	ZED_KK_NUMPADENTER				= 0x9C,
	ZED_KK_RCONTROL					= 0x9D,
	ZED_KK_MUTE						= 0xA0,
	ZED_KK_CALCULATOR				= 0xA1,
	ZED_KK_PLAYPAUSE				= 0xA2,
	ZED_KK_MEDIASTOP				= 0xA4,
	ZED_KK_VOLUMEDOWN				= 0xAE,
	ZED_KK_VOLUMEUP					= 0xB0,
	ZED_KK_WEBHOME					= 0xB2,
	ZED_KK_NUMPADCOMMA				= 0xB3,
	ZED_KK_DIVIDE					= 0xB5,
	ZED_KK_SYSRQ					= 0xB7,
	ZED_KK_RMENU					= 0xB8,
	ZED_KK_PAUSE					= 0xC5, 
	ZED_KK_HOME						= 0xC7,
	ZED_KK_UP						= 0xC8, 
	ZED_KK_PRIOR					= 0xC9,
	ZED_KK_LEFT						= 0xCB, 
	ZED_KK_RIGHT					= 0xCD, 
	ZED_KK_END						= 0xCF,
	ZED_KK_DOWN						= 0xD0,
	ZED_KK_NEXT						= 0xD1,
	ZED_KK_INSERT					= 0xD2,
	ZED_KK_DELETE					= 0xD3,
	ZED_KK_LWIN						= 0xDB,
	ZED_KK_RWIN						= 0xDC, 
	ZED_KK_APPS						= 0xDD,
	ZED_KK_POWER					= 0xDE,
	ZED_KK_SLEEP					= 0xDF,
	ZED_KK_WAKE						= 0xE3,
	ZED_KK_WEBSEARCH				= 0xE5,
	ZED_KK_WEBFAVORITES				= 0xE6,
	ZED_KK_WEBREFRESH				= 0xE7,
	ZED_KK_WEBSTOP					= 0xE8,
	ZED_KK_WEBFORWARD				= 0xE9,
	ZED_KK_WEBBACK					= 0xEA, 
	ZED_KK_MYCOMPUTER				= 0xEB,
	ZED_KK_MAIL						= 0xEC, 
	ZED_KK_MEDIASELECT				= 0xED,
	ZED_KK_BACKSPACE				= ZED_KK_BACK,
	ZED_KK_NUMPADSTAR				= ZED_KK_MULTIPLY,
	ZED_KK_LALT						= ZED_KK_LMENU,
	ZED_KK_CAPSLOCK					= ZED_KK_CAPITAL,
	ZED_KK_NUMPADMINUS				= ZED_KK_SUBTRACT,
	ZED_KK_NUMPADPLUS				= ZED_KK_ADD,
	ZED_KK_NUMPADPERIOD				= ZED_KK_DECIMAL,
	ZED_KK_NUMPADSLASH				= ZED_KK_DIVIDE,
	ZED_KK_RALT						= ZED_KK_RMENU,
	ZED_KK_UPARROW					= ZED_KK_UP,
	ZED_KK_PGUP						= ZED_KK_PRIOR,
	ZED_KK_LEFTARROW				= ZED_KK_LEFT,
	ZED_KK_RIGHTARROW				= ZED_KK_RIGHT,
	ZED_KK_DOWNARROW				= ZED_KK_DOWN,
	ZED_KK_PGDN						= ZED_KK_NEXT
};

enum ZEDViewportInputEventType
{
	ZED_ET_BUTTON_PRESSED,
	ZED_ET_BUTTON_RELEASED,
	ZED_ET_MOUSE_MOVED,
	ZED_ET_MOUSE_DOUBLE_CLICKED
};

class ZEDViewportMouseEvent
{
	public:
		ZEDViewportInputEventType			Type;
		ZEInt								PositionX;
		ZEInt								PositionY;
		ZEInt								ScreenPositionX;
		ZEInt								ScreenPositionY;
		ZEDMouseButton						Button;
		ZEDMouseButton						OtherButtons;
};

class ZEDViewportKeyboardEvent
{
	public:
		ZEDViewportInputEventType			Type;
		ZEDKeyboardKey						Key;
		ZEUInt								ScanCode;
		ZEUInt								VirtualKey;
		bool								AutoRepeat;
		ZEString							Text;
};
