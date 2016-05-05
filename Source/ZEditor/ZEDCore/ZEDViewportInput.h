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
#include "ZEMath/ZEVector.h"

enum ZEDInputMouseButton
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

enum ZEDInputKeyModifier
{
	ZED_KKM_NONE					= 0x00000000,
	ZED_KKM_CTRL					= 0x00010000,
	ZED_KKM_ALT						= 0x00020000,
	ZED_KKM_SHIFT					= 0x00040000,
	ZED_KKM_WINDOWS					= 0x00080000,
};

enum ZEDInputKeyboardKey
{
	ZED_IKK_KEY_ESCAPE				= 0x01000000,	 
	ZED_IKK_KEY_TAB					= 0x01000001,	 
	ZED_IKK_KEY_BACKTAB				= 0x01000002,	 
	ZED_IKK_KEY_BACKSPACE			= 0x01000003,	 
	ZED_IKK_KEY_RETURN				= 0x01000004,	 
	ZED_IKK_KEY_ENTER				= 0x01000005,
	ZED_IKK_KEY_INSERT				= 0x01000006,	 
	ZED_IKK_KEY_DELETE				= 0x01000007,	 
	ZED_IKK_KEY_BREAK				= 0x01000008,	
	ZED_IKK_KEY_PRINT_SCREEN		= 0x01000009,	 
	ZED_IKK_KEY_SYSREQ				= 0x0100000A,	 
	ZED_IKK_KEY_CLEAR				= 0x0100000B,	 
	ZED_IKK_KEY_HOME				= 0x01000010,	 
	ZED_IKK_KEY_END					= 0x01000011,	 
	ZED_IKK_KEY_LEFT				= 0x01000012,	 
	ZED_IKK_KEY_UP					= 0x01000013,	 
	ZED_IKK_KEY_RIGHT				= 0x01000014,	 
	ZED_IKK_KEY_DOWN				= 0x01000015,	 
	ZED_IKK_KEY_PAGEUP				= 0x01000016,	 
	ZED_IKK_KEY_PAGEDOWN			= 0x01000017,	 
	ZED_IKK_KEY_SHIFT				= 0x01000020,	 
	ZED_IKK_KEY_CONTROL				= 0x01000021,
	ZED_IKK_KEY_META				= 0x01000022,
	ZED_IKK_KEY_ALT					= 0x01000023,	 
	ZED_IKK_KEY_ALTGR				= 0x01001103,
	ZED_IKK_KEY_CAPSLOCK			= 0x01000024,	 
	ZED_IKK_KEY_NUMLOCK				= 0x01000025,	 
	ZED_IKK_KEY_SCROLLLOCK			= 0x01000026,	 
	ZED_IKK_KEY_F1					= 0x01000030,	 
	ZED_IKK_KEY_F2					= 0x01000031,	 
	ZED_IKK_KEY_F3					= 0x01000032,	 
	ZED_IKK_KEY_F4					= 0x01000033,	 
	ZED_IKK_KEY_F5					= 0x01000034,	 
	ZED_IKK_KEY_F6					= 0x01000035,	 
	ZED_IKK_KEY_F7					= 0x01000036,	 
	ZED_IKK_KEY_F8					= 0x01000037,	 
	ZED_IKK_KEY_F9					= 0x01000038,	 
	ZED_IKK_KEY_F10					= 0x01000039,	 
	ZED_IKK_KEY_F11					= 0x0100003A,	 
	ZED_IKK_KEY_F12					= 0x0100003B,	 
	ZED_IKK_KEY_SUPER_L				= 0x01000053,	 
	ZED_IKK_KEY_SUPER_R				= 0x01000054,	 
	ZED_IKK_KEY_MENU				= 0x01000055,	 
	ZED_IKK_KEY_HYPER_L				= 0x01000056,	 
	ZED_IKK_KEY_HYPER_R				= 0x01000057,	 
	ZED_IKK_KEY_HELP				= 0x01000058,	 
	ZED_IKK_KEY_DIRECTION_L			= 0x01000059,	 
	ZED_IKK_KEY_DIRECTION_R			= 0x01000060,	 
	ZED_IKK_KEY_SPACE				= 0x00000020,	 
	ZED_IKK_KEY_EXCLAM				= 0x00000021,	 
	ZED_IKK_KEY_QUOTEDBL			= 0x00000022,	 
	ZED_IKK_KEY_NUMBERSIGN			= 0x00000023,	 
	ZED_IKK_KEY_DOLLAR				= 0x00000024,	 
	ZED_IKK_KEY_PERCENT				= 0x00000025,	 
	ZED_IKK_KEY_AMPERSAND			= 0x00000026,	 
	ZED_IKK_KEY_APOSTROPHE			= 0x00000027,	 
	ZED_IKK_KEY_PAREN_LEFT			= 0x00000028,	 
	ZED_IKK_KEY_PAREN_RIGHT			= 0x00000029,	 
	ZED_IKK_KEY_ASTERISK			= 0x0000002A,	 
	ZED_IKK_KEY_PLUS				= 0x0000002B,	 
	ZED_IKK_KEY_COMMA				= 0x0000002C,	 
	ZED_IKK_KEY_MINUS				= 0x0000002D,	 
	ZED_IKK_KEY_PERIOD				= 0x0000002E,	 
	ZED_IKK_KEY_SLASH				= 0x0000002F,	 
	ZED_IKK_KEY_0					= 0x00000030,	 
	ZED_IKK_KEY_1					= 0x00000031,	 
	ZED_IKK_KEY_2					= 0x00000032,	 
	ZED_IKK_KEY_3					= 0x00000033,	 
	ZED_IKK_KEY_4					= 0x00000034,	 
	ZED_IKK_KEY_5					= 0x00000035,	 
	ZED_IKK_KEY_6					= 0x00000036,	 
	ZED_IKK_KEY_7					= 0x00000037,	 
	ZED_IKK_KEY_8					= 0x00000038,	 
	ZED_IKK_KEY_9					= 0x00000039,	 
	ZED_IKK_KEY_COLON				= 0x0000003A,	 
	ZED_IKK_KEY_SEMICOLON			= 0x0000003B,	 
	ZED_IKK_KEY_LESS				= 0x0000003C,	 
	ZED_IKK_KEY_EQUAL				= 0x0000003D,	 
	ZED_IKK_KEY_GREATER				= 0x0000003E,	 
	ZED_IKK_KEY_QUESTION			= 0x0000003F,	 
	ZED_IKK_KEY_AT					= 0x00000040,	 
	ZED_IKK_KEY_A					= 0x00000041,	 
	ZED_IKK_KEY_B					= 0x00000042,	 
	ZED_IKK_KEY_C					= 0x00000043,	 
	ZED_IKK_KEY_D					= 0x00000044,	 
	ZED_IKK_KEY_E					= 0x00000045,	 
	ZED_IKK_KEY_F					= 0x00000046,	 
	ZED_IKK_KEY_G					= 0x00000047,	 
	ZED_IKK_KEY_H					= 0x00000048,	 
	ZED_IKK_KEY_I					= 0x00000049,	 
	ZED_IKK_KEY_J					= 0x0000004A,	 
	ZED_IKK_KEY_K					= 0x0000004B,	 
	ZED_IKK_KEY_L					= 0x0000004C,	 
	ZED_IKK_KEY_M					= 0x0000004D,	 
	ZED_IKK_KEY_N					= 0x0000004E,	 
	ZED_IKK_KEY_O					= 0x0000004F,	 
	ZED_IKK_KEY_P					= 0x00000050,	 
	ZED_IKK_KEY_Q					= 0x00000051,	 
	ZED_IKK_KEY_R					= 0x00000052,	 
	ZED_IKK_KEY_S					= 0x00000053,	 
	ZED_IKK_KEY_T					= 0x00000054,	 
	ZED_IKK_KEY_U					= 0x00000055,	 
	ZED_IKK_KEY_V					= 0x00000056,	 
	ZED_IKK_KEY_W					= 0x00000057,	 
	ZED_IKK_KEY_X					= 0x00000058,	 
	ZED_IKK_KEY_Y					= 0x00000059,	 
	ZED_IKK_KEY_Z					= 0x0000005A,	 
	ZED_IKK_KEY_BRACKET_LEFT		= 0X0000005B,	 
	ZED_IKK_KEY_BACKSLASH			= 0X0000005C,	 
	ZED_IKK_KEY_BRACKET_RIGHT		= 0X0000005D,	 
	ZED_IKK_KEY_ASCII_CIRCUM		= 0X0000005E,	 
	ZED_IKK_KEY_UNDERSCORE			= 0X0000005F,	 
	ZED_IKK_KEY_QUOTE_LEFT			= 0X00000060,	 
	ZED_IKK_KEY_BRACE_LEFT			= 0X0000007B,	 
	ZED_IKK_KEY_BAR					= 0X0000007C,	 
	ZED_IKK_KEY_BRACE_RIGHT			= 0X0000007D,	 
	ZED_IKK_KEY_ASCII_TILDE			= 0X0000007E,	 
	ZED_IKK_KEY_NOBREAKSPACE		= 0X000000A0,	 
	ZED_IKK_KEY_EXCLAM_DOWN			= 0X000000A1,	 
	ZED_IKK_KEY_CENT				= 0X000000A2,	 
	ZED_IKK_KEY_STERLING			= 0X000000A3,	 
	ZED_IKK_KEY_CURRENCY			= 0X000000A4,	 
	ZED_IKK_KEY_YEN					= 0X000000A5,	 
	ZED_IKK_KEY_BROKEN_BAR			= 0X000000A6,	 
	ZED_IKK_KEY_SECTION				= 0X000000A7	 
};

enum ZEDInputEventType
{
	ZED_ET_NONE = 0,
	ZED_ET_BUTTON_PRESSED,
	ZED_ET_BUTTON_PRESSING,
	ZED_ET_BUTTON_RELEASED,
	ZED_ET_MOUSE_MOVED,
	ZED_ET_MOUSE_DOUBLE_CLICKED
};

class ZEDViewport;

class ZEDViewportMouseEvent
{
	friend class ZEDViewport;
	private:
		ZEDViewport*				Viewport;
		ZEDInputEventType			Type;
		ZEVector2					Delta;
		ZEVector2					Position;
		ZEUInt						Modifiers;
		ZEDInputMouseButton			Button;

	public:
		ZEDViewport*				GetViewport() const;
		ZEDInputEventType			GetType() const;
		const ZEVector2&			GetDelta() const;
		const ZEVector2&			GetPosition() const;
		ZEUInt						GetModifiers() const;
		ZEDInputMouseButton			GetButton() const;

									ZEDViewportMouseEvent();
};

class ZEDViewportKeyboardEvent
{
	friend class ZEDViewport;
	private:
		ZEDViewport*				Viewport;
		ZEDInputEventType			Type;
		ZEDInputKeyboardKey			Key;
		ZEUInt						Modifiers;
		ZEUInt						VirtualKey;
		ZEString					Text;

	public:
		ZEDViewport*				GetViewport() const;
		ZEDInputEventType			GetType() const;
		ZEDInputKeyboardKey			GetKey() const;
		ZEUInt						GetModifiers() const;
		ZEUInt						GetVirtualKey() const;
		const ZEString&				GetText() const;


									ZEDViewportKeyboardEvent();
};
