//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDInputDefinitions.h
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

#include "ZEDEvent.h"

enum ZEDMouseButton
{
	ZED_VMB_NONE					= 0x00000000,
	ZED_VMB_LEFT					= 0x00000001,
	ZED_VMB_RIGHT					= 0x00000002,
	ZED_VMB_MIDDLE					= 0x00000004,
	ZED_VMB_BACK					= 0x00000010,
	ZED_VMB_FORWARD					= 0x00000020,
	ZED_VMB_EXTRA_0					= 0x00000100,
	ZED_VMB_EXTRA_2					= 0x00000200,
	ZED_VMB_EXTRA_3					= 0x00000400,
	ZED_VMB_EXTRA_4					= 0x00000800,
	ZED_VMB_EXTRA_5					= 0x00001000,
	ZED_VMB_EXTRA_6					= 0x00002000,
	ZED_VMB_EXTRA_7					= 0x00004000,
	ZED_VMB_EXTRA_8					= 0x00008000,
	ZED_VMB_EXTRA_9					= 0x00010000,
	ZED_VMB_EXTRA_10				= 0x00020000,
};

enum ZEDKeyModifier
{
	ZED_VKM_NONE					= 0x00000000,
	ZED_VKM_CTRL					= 0x00010000,
	ZED_VKM_ALT						= 0x00020000,
	ZED_VKM_SHIFT					= 0x00040000,
	ZED_VKM_WINDOWS					= 0x00080000,
};
typedef ZEFlags ZEDKeyModifiers;

enum ZEDKeyboardKey
{
	ZED_VKK_NONE					= 0x00000000,
	ZED_VKK_ESCAPE					= 0x01000000,	 
	ZED_VKK_TAB						= 0x01000001,	 
	ZED_VKK_BACKTAB					= 0x01000002,	 
	ZED_VKK_BACKSPACE				= 0x01000003,	 
	ZED_VKK_RETURN					= 0x01000004,	 
	ZED_VKK_ENTER					= 0x01000005,
	ZED_VKK_INSERT					= 0x01000006,	 
	ZED_VKK_DELETE					= 0x01000007,	 
	ZED_VKK_BREAK					= 0x01000008,	
	ZED_VKK_PRINT_SCREEN			= 0x01000009,	 
	ZED_VKK_SYSREQ					= 0x0100000A,	 
	ZED_VKK_CLEAR					= 0x0100000B,	 
	ZED_VKK_HOME					= 0x01000010,	 
	ZED_VKK_END						= 0x01000011,	 
	ZED_VKK_LEFT					= 0x01000012,	 
	ZED_VKK_UP						= 0x01000013,	 
	ZED_VKK_RIGHT					= 0x01000014,	 
	ZED_VKK_DOWN					= 0x01000015,	 
	ZED_VKK_PAGEUP					= 0x01000016,	 
	ZED_VKK_PAGEDOWN				= 0x01000017,	 
	ZED_VKK_SHIFT					= 0x01000020,	 
	ZED_VKK_CONTROL					= 0x01000021,
	ZED_VKK_META					= 0x01000022,
	ZED_VKK_ALT						= 0x01000023,	 
	ZED_VKK_ALTGR					= 0x01001103,
	ZED_VKK_CAPSLOCK				= 0x01000024,	 
	ZED_VKK_NUMLOCK					= 0x01000025,	 
	ZED_VKK_SCROLLLOCK				= 0x01000026,	 
	ZED_VKK_F1						= 0x01000030,	 
	ZED_VKK_F2						= 0x01000031,	 
	ZED_VKK_F3						= 0x01000032,	 
	ZED_VKK_F4						= 0x01000033,	 
	ZED_VKK_F5						= 0x01000034,	 
	ZED_VKK_F6						= 0x01000035,	 
	ZED_VKK_F7						= 0x01000036,	 
	ZED_VKK_F8						= 0x01000037,	 
	ZED_VKK_F9						= 0x01000038,	 
	ZED_VKK_F10						= 0x01000039,	 
	ZED_VKK_F11						= 0x0100003A,	 
	ZED_VKK_F12						= 0x0100003B,	 
	ZED_VKK_SUPER_L					= 0x01000053,	 
	ZED_VKK_SUPER_R					= 0x01000054,	 
	ZED_VKK_MENU					= 0x01000055,	 
	ZED_VKK_HYPER_L					= 0x01000056,	 
	ZED_VKK_HYPER_R					= 0x01000057,	 
	ZED_VKK_HELP					= 0x01000058,	 
	ZED_VKK_DIRECTION_L				= 0x01000059,	 
	ZED_VKK_DIRECTION_R				= 0x01000060,	 
	ZED_VKK_SPACE					= 0x00000020,	 
	ZED_VKK_EXCLAM					= 0x00000021,	 
	ZED_VKK_QUOTEDBL				= 0x00000022,	 
	ZED_VKK_NUMBERSIGN				= 0x00000023,	 
	ZED_VKK_DOLLAR					= 0x00000024,	 
	ZED_VKK_PERCENT					= 0x00000025,	 
	ZED_VKK_AMPERSAND				= 0x00000026,	 
	ZED_VKK_APOSTROPHE				= 0x00000027,	 
	ZED_VKK_PAREN_LEFT				= 0x00000028,	 
	ZED_VKK_PAREN_RIGHT				= 0x00000029,	 
	ZED_VKK_ASTERISK				= 0x0000002A,	 
	ZED_VKK_PLUS					= 0x0000002B,	 
	ZED_VKK_COMMA					= 0x0000002C,	 
	ZED_VKK_MINUS					= 0x0000002D,	 
	ZED_VKK_PERIOD					= 0x0000002E,	 
	ZED_VKK_SLASH					= 0x0000002F,	 
	ZED_VKK_0						= 0x00000030,	 
	ZED_VKK_1						= 0x00000031,	 
	ZED_VKK_2						= 0x00000032,	 
	ZED_VKK_3						= 0x00000033,	 
	ZED_VKK_4						= 0x00000034,	 
	ZED_VKK_5						= 0x00000035,	 
	ZED_VKK_6						= 0x00000036,	 
	ZED_VKK_7						= 0x00000037,	 
	ZED_VKK_8						= 0x00000038,	 
	ZED_VKK_9						= 0x00000039,	 
	ZED_VKK_COLON					= 0x0000003A,	 
	ZED_VKK_SEMICOLON				= 0x0000003B,	 
	ZED_VKK_LESS					= 0x0000003C,	 
	ZED_VKK_EQUAL					= 0x0000003D,	 
	ZED_VKK_GREATER					= 0x0000003E,	 
	ZED_VKK_QUESTION				= 0x0000003F,	 
	ZED_VKK_AT						= 0x00000040,	 
	ZED_VKK_A						= 0x00000041,	 
	ZED_VKK_B						= 0x00000042,	 
	ZED_VKK_C						= 0x00000043,	 
	ZED_VKK_D						= 0x00000044,	 
	ZED_VKK_E						= 0x00000045,	 
	ZED_VKK_F						= 0x00000046,	 
	ZED_VKK_G						= 0x00000047,	 
	ZED_VKK_H						= 0x00000048,	 
	ZED_VKK_I						= 0x00000049,	 
	ZED_VKK_J						= 0x0000004A,	 
	ZED_VKK_K						= 0x0000004B,	 
	ZED_VKK_L						= 0x0000004C,	 
	ZED_VKK_M						= 0x0000004D,	 
	ZED_VKK_N						= 0x0000004E,	 
	ZED_VKK_O						= 0x0000004F,	 
	ZED_VKK_P						= 0x00000050,	 
	ZED_VKK_Q						= 0x00000051,	 
	ZED_VKK_R						= 0x00000052,	 
	ZED_VKK_S						= 0x00000053,	 
	ZED_VKK_T						= 0x00000054,	 
	ZED_VKK_U						= 0x00000055,	 
	ZED_VKK_V						= 0x00000056,	 
	ZED_VKK_W						= 0x00000057,	 
	ZED_VKK_X						= 0x00000058,	 
	ZED_VKK_Y						= 0x00000059,	 
	ZED_VKK_Z						= 0x0000005A,	 
	ZED_VKK_BRACKET_LEFT			= 0x0000005B,	 
	ZED_VKK_BACKSLASH				= 0x0000005C,	 
	ZED_VKK_BRACKET_RIGHT			= 0x0000005D,	 
	ZED_VKK_ASCII_CIRCUM			= 0x0000005E,	 
	ZED_VKK_UNDERSCORE				= 0x0000005F,	 
	ZED_VKK_QUOTE_LEFT				= 0x00000060,	 
	ZED_VKK_BRACE_LEFT				= 0x0000007B,	 
	ZED_VKK_BAR						= 0x0000007C,	 
	ZED_VKK_BRACE_RIGHT				= 0x0000007D,	 
	ZED_VKK_ASCII_TILDE				= 0x0000007E,	 
	ZED_VKK_NOBREAKSPACE			= 0x000000A0,	 
	ZED_VKK_EXCLAM_DOWN				= 0x000000A1,	 
	ZED_VKK_CENT					= 0x000000A2,	 
	ZED_VKK_STERLING				= 0x000000A3,	 
	ZED_VKK_CURRENCY				= 0x000000A4,	 
	ZED_VKK_YEN						= 0x000000A5,	 
	ZED_VKK_BROKEN_BAR				= 0x000000A6,	 
	ZED_VKK_SECTION					= 0x000000A7	 
};
