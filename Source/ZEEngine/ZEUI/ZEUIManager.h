//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIManager.h
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
#ifndef	__ZE_UI_MANAGER_H__
#define __ZE_UI_MANAGER_H__

#include "ZEMeta/ZEObject.h"
#include "ZEDS/ZEArray.h"
#include "ZEUIRenderer.h"
#include "ZEUIEvents.h"
#include "ZEInput/ZEInputMap.h"
#include "ZEFontResource.h"

enum ZEUIInputKey
{
	ZE_UI_IKB_ESCAPE,
	ZE_UI_IKB_1,
	ZE_UI_IKB_2,
	ZE_UI_IKB_3,
	ZE_UI_IKB_4,
	ZE_UI_IKB_5,
	ZE_UI_IKB_6,
	ZE_UI_IKB_7,
	ZE_UI_IKB_8,
	ZE_UI_IKB_9,
	ZE_UI_IKB_0,
	ZE_UI_IKB_MINUS,
	ZE_UI_IKB_EQUALS,
	ZE_UI_IKB_TAB,
	ZE_UI_IKB_Q,
	ZE_UI_IKB_W,
	ZE_UI_IKB_E,
	ZE_UI_IKB_R,
	ZE_UI_IKB_T,
	ZE_UI_IKB_Y,
	ZE_UI_IKB_U,
	ZE_UI_IKB_I,
	ZE_UI_IKB_O,
	ZE_UI_IKB_P,
	ZE_UI_IKB_LBRACKET,
	ZE_UI_IKB_RBRACKET,
	ZE_UI_IKB_RETURN,
	ZE_UI_IKB_LCONTROL,
	ZE_UI_IKB_A,
	ZE_UI_IKB_S,
	ZE_UI_IKB_D,
	ZE_UI_IKB_F,
	ZE_UI_IKB_G,
	ZE_UI_IKB_H,
	ZE_UI_IKB_J,
	ZE_UI_IKB_K,
	ZE_UI_IKB_L,
	ZE_UI_IKB_SEMICOLON	,
	ZE_UI_IKB_APOSTROPHE,	
	ZE_UI_IKB_LSHIFT,	
	ZE_UI_IKB_BACKSLASH,
	ZE_UI_IKB_Z,
	ZE_UI_IKB_X,
	ZE_UI_IKB_C,
	ZE_UI_IKB_V,
	ZE_UI_IKB_B,
	ZE_UI_IKB_N,
	ZE_UI_IKB_M,
	ZE_UI_IKB_COMMA,
	ZE_UI_IKB_PERIOD,	
	ZE_UI_IKB_SLASH,
	ZE_UI_IKB_RSHIFT,	
	ZE_UI_IKB_MULTIPLY,	
	ZE_UI_IKB_SPACE	,
	ZE_UI_IKB_CAPITAL,	
	ZE_UI_IKB_F1,	
	ZE_UI_IKB_F2,	
	ZE_UI_IKB_F3,	
	ZE_UI_IKB_F4,	
	ZE_UI_IKB_F5,	
	ZE_UI_IKB_F6,	
	ZE_UI_IKB_F7,	
	ZE_UI_IKB_F8,	
	ZE_UI_IKB_F9,	
	ZE_UI_IKB_F10,	
	ZE_UI_IKB_NUMLOCK,	
	ZE_UI_IKB_SCROLL,	
	ZE_UI_IKB_NUMPAD7,	
	ZE_UI_IKB_NUMPAD8,	
	ZE_UI_IKB_NUMPAD9,	
	ZE_UI_IKB_SUBTRACT,	
	ZE_UI_IKB_NUMPAD4,	
	ZE_UI_IKB_NUMPAD5,	
	ZE_UI_IKB_NUMPAD6,	
	ZE_UI_IKB_ADD,	
	ZE_UI_IKB_NUMPAD1,	
	ZE_UI_IKB_NUMPAD2,	
	ZE_UI_IKB_NUMPAD3,	
	ZE_UI_IKB_NUMPAD0,	
	ZE_UI_IKB_DECIMAL,	
	ZE_UI_IKB_OEM_102,	
	ZE_UI_IKB_F11,	
	ZE_UI_IKB_F12,	
	ZE_UI_IKB_F13,	
	ZE_UI_IKB_F14,	
	ZE_UI_IKB_F15,	
	ZE_UI_IKB_KANA,	
	ZE_UI_IKB_ABNT_C1,	
	ZE_UI_IKB_CONVERT,	
	ZE_UI_IKB_NOCONVERT,
	ZE_UI_IKB_YEN,	
	ZE_UI_IKB_ABNT_C2,	
	ZE_UI_IKB_NUMPADEQUALS,	
	ZE_UI_IKB_PREVTRACK,
	ZE_UI_IKB_AT,	
	ZE_UI_IKB_COLON,
	ZE_UI_IKB_UNDERLINE,
	ZE_UI_IKB_KANJI,
	ZE_UI_IKB_STOP,	
	ZE_UI_IKB_AX,	
	ZE_UI_IKB_UNLABELED,
	ZE_UI_IKB_NEXTTRACK,
	ZE_UI_IKB_NUMPADENTER,	
	ZE_UI_IKB_RCONTROL,	
	ZE_UI_IKB_MUTE,	
	ZE_UI_IKB_CALCULATOR,	
	ZE_UI_IKB_NUMPADCOMMA,	
	ZE_UI_IKB_DIVIDE,	
	ZE_UI_IKB_SYSRQ,
	ZE_UI_IKB_RMENU,
	ZE_UI_IKB_PAUSE,
	ZE_UI_IKB_HOME,	
	ZE_UI_IKB_UP,	
	ZE_UI_IKB_PRIOR,
	ZE_UI_IKB_LEFT,	
	ZE_UI_IKB_RIGHT,
	ZE_UI_IKB_END,	
	ZE_UI_IKB_DOWN,	
	ZE_UI_IKB_NEXT,	
	ZE_UI_IKB_INSERT,	
	ZE_UI_IKB_DELETE,
	ZE_UI_IKB_BACKSPACE,
};

enum ZEUIMouseKey;

ZE_META_FORWARD_DECLARE(ZEUIControl, "ZEUIControl.h")
ZE_META_FORWARD_DECLARE(ZEUICursorControl, "ZEUICursorControl.h")

class ZEUIManager : public ZEObject
{
	ZE_OBJECT

	private:
		
		ZEInputMap					InputMap;

		ZEArray<ZEUIControl*>		Controls;
		ZEUIRenderer*				UIRenderer;
		ZEUICursorControl*			Cursor;
		
		ZEUIControl*				LastHoveredControl;
		ZEUIControl*				LastPressedControl;
		ZEUIControl*				LastFocusedControl;

		ZEUIMouseKey				PreviousPressedButton;

		ZEVector2					OldMousePosition;
		bool						MouseMoveEventFlag;

		ZEUIControl*				FindEventReciever(ZEUIControl* ParentControl);

		static ZEVector4			DefaultBackgroundColor;
		static ZEVector4			DefaultForegroundColor;
		//static ZEFontResource*		DefaultFontResource;

									ZEUIManager();
									~ZEUIManager();

	public:
	
		void						SetActiveCursor(ZEUICursorControl* Cursor);

		void						AddControl(ZEUIControl* Control);
		void						RemoveControl(ZEUIControl* Control);
		ZEArray<ZEUIControl*>&		GetControls();

		bool						Initialize();
		void						Deinitialize();
		
		void						ProcessEvents();
		void						Render(ZERNRenderer* Render);
		void						Tick(float ElapsedTime);

		void						Destroy();

		static ZEUIManager*			CreateInstance();

		static const ZEVector4&		GetDefaultBackgroundColor();
		static const ZEVector4&		GetDefaultForegroundColor();
		static ZEFontResource*		GetDefaultFontResource();
};

#endif




