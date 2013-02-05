//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIManager.cpp
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

#include "ZEUIManager.h"
#include "ZEUIRenderer.h"
#include "ZEUIControl.h"
#include "ZEError.h"
#include "ZEGraphics/ZEFixedMaterial.h"
#include "ZEInput/ZEInputDefinitions.h"
#include "ZEInput/ZEInputModule.h"
#include "ZEFontResourceBitmap.h"
#include "ZEFontResourceDynamic.h"

ZEVector4 ZEUIManager::DefaultBackgroundColor = ZEVector4(0.2f, 0.2f, 0.2f, 1.0f);
ZEVector4 ZEUIManager::DefaultForegroundColor = ZEVector4(0.0f, 0.8f, 0.0f, 1.0f);
//ZEFontResource* ZEUIManager::DefaultFontResource = ZEFontResourceBitmap::LoadSharedResource("Courier New.zeFont");

const ZEVector4& ZEUIManager::GetDefaultBackgroundColor()
{
	return DefaultBackgroundColor;
}

const ZEVector4&	ZEUIManager::GetDefaultForegroundColor()
{
	return DefaultForegroundColor;
}

ZEFontResource* ZEUIManager::GetDefaultFontResource()
{
	return ZEFontResourceDynamic::LoadSharedResource("TrajanPro-Regular.otf", 16);
}

ZEUIManager::ZEUIManager() 
{
	UIRenderer = NULL;
	OldMousePosition = ZEVector2::Zero;
	LastHoveredControl = NULL;
	LastPressedControl = NULL;
	LastFocusedControl = NULL;
	MouseMoveEventFlag = false;
	Cursor = NULL;
	PreviousPressedButton = ZE_UI_MOUSE_BUTTON_NONE; 

	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_ESCAPE,			ZEInputEvent("Keyboard",ZE_IKB_ESCAPE,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_1,				ZEInputEvent("Keyboard",ZE_IKB_1,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_2,				ZEInputEvent("Keyboard",ZE_IKB_2,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_3,				ZEInputEvent("Keyboard",ZE_IKB_3,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_4,				ZEInputEvent("Keyboard",ZE_IKB_4,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_5,				ZEInputEvent("Keyboard",ZE_IKB_5,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_6,				ZEInputEvent("Keyboard",ZE_IKB_6,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_7,				ZEInputEvent("Keyboard",ZE_IKB_7,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_8,				ZEInputEvent("Keyboard",ZE_IKB_8,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_9,				ZEInputEvent("Keyboard",ZE_IKB_9,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_0,				ZEInputEvent("Keyboard",ZE_IKB_0,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_MINUS,			ZEInputEvent("Keyboard",ZE_IKB_MINUS,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_EQUALS,			ZEInputEvent("Keyboard",ZE_IKB_EQUALS,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_TAB,				ZEInputEvent("Keyboard",ZE_IKB_TAB,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_Q,				ZEInputEvent("Keyboard",ZE_IKB_Q,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_W,				ZEInputEvent("Keyboard",ZE_IKB_W,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_E,				ZEInputEvent("Keyboard",ZE_IKB_E,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_R,				ZEInputEvent("Keyboard",ZE_IKB_R,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_T,				ZEInputEvent("Keyboard",ZE_IKB_T,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_Y,				ZEInputEvent("Keyboard",ZE_IKB_Y,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_U,				ZEInputEvent("Keyboard",ZE_IKB_U,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_I,				ZEInputEvent("Keyboard",ZE_IKB_I,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_O,				ZEInputEvent("Keyboard",ZE_IKB_O,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_P,				ZEInputEvent("Keyboard",ZE_IKB_P,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_LBRACKET,			ZEInputEvent("Keyboard",ZE_IKB_LBRACKET,			ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_RBRACKET,			ZEInputEvent("Keyboard",ZE_IKB_RBRACKET,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_RETURN,			ZEInputEvent("Keyboard",ZE_IKB_RETURN,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_LCONTROL,			ZEInputEvent("Keyboard",ZE_IKB_LCONTROL,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_A,				ZEInputEvent("Keyboard",ZE_IKB_A,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_S,				ZEInputEvent("Keyboard",ZE_IKB_S,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_D,				ZEInputEvent("Keyboard",ZE_IKB_D,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_F,				ZEInputEvent("Keyboard",ZE_IKB_F,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_G,				ZEInputEvent("Keyboard",ZE_IKB_G,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_H,				ZEInputEvent("Keyboard",ZE_IKB_H,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_J,				ZEInputEvent("Keyboard",ZE_IKB_J,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_K,				ZEInputEvent("Keyboard",ZE_IKB_K,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_L,				ZEInputEvent("Keyboard",ZE_IKB_L,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_SEMICOLON, 		ZEInputEvent("Keyboard",ZE_IKB_SEMICOLON, 		 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_APOSTROPHE,		ZEInputEvent("Keyboard",ZE_IKB_APOSTROPHE,		 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_LSHIFT,			ZEInputEvent("Keyboard",ZE_IKB_LSHIFT,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_BACKSLASH, 		ZEInputEvent("Keyboard",ZE_IKB_BACKSLASH, 		 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_Z,				ZEInputEvent("Keyboard",ZE_IKB_Z,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_X,				ZEInputEvent("Keyboard",ZE_IKB_X,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_C,				ZEInputEvent("Keyboard",ZE_IKB_C,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_V,				ZEInputEvent("Keyboard",ZE_IKB_V,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_B,				ZEInputEvent("Keyboard",ZE_IKB_B,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_N,				ZEInputEvent("Keyboard",ZE_IKB_N,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_M,				ZEInputEvent("Keyboard",ZE_IKB_M,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_COMMA,			ZEInputEvent("Keyboard",ZE_IKB_COMMA,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_PERIOD,			ZEInputEvent("Keyboard",ZE_IKB_PERIOD,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_SLASH,			ZEInputEvent("Keyboard",ZE_IKB_SLASH,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_RSHIFT,			ZEInputEvent("Keyboard",ZE_IKB_RSHIFT,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_MULTIPLY,			ZEInputEvent("Keyboard",ZE_IKB_MULTIPLY,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_SPACE	,			ZEInputEvent("Keyboard",ZE_IKB_SPACE	,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_CAPITAL,			ZEInputEvent("Keyboard",ZE_IKB_CAPITAL,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_F1,				ZEInputEvent("Keyboard",ZE_IKB_F1,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_F2,				ZEInputEvent("Keyboard",ZE_IKB_F2,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_F3,				ZEInputEvent("Keyboard",ZE_IKB_F3,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_F4,				ZEInputEvent("Keyboard",ZE_IKB_F4,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_F5,				ZEInputEvent("Keyboard",ZE_IKB_F5,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_F6,				ZEInputEvent("Keyboard",ZE_IKB_F6,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_F7,				ZEInputEvent("Keyboard",ZE_IKB_F7,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_F8,				ZEInputEvent("Keyboard",ZE_IKB_F8,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_F9,				ZEInputEvent("Keyboard",ZE_IKB_F9,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_F10,				ZEInputEvent("Keyboard",ZE_IKB_F10,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_NUMLOCK,			ZEInputEvent("Keyboard",ZE_IKB_NUMLOCK,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_SCROLL,			ZEInputEvent("Keyboard",ZE_IKB_SCROLL,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_NUMPAD7,			ZEInputEvent("Keyboard",ZE_IKB_NUMPAD7,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_NUMPAD8,			ZEInputEvent("Keyboard",ZE_IKB_NUMPAD8,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_NUMPAD9,			ZEInputEvent("Keyboard",ZE_IKB_NUMPAD9,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_SUBTRACT,			ZEInputEvent("Keyboard",ZE_IKB_SUBTRACT,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_NUMPAD4,			ZEInputEvent("Keyboard",ZE_IKB_NUMPAD4,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_NUMPAD5,			ZEInputEvent("Keyboard",ZE_IKB_NUMPAD5,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_NUMPAD6,			ZEInputEvent("Keyboard",ZE_IKB_NUMPAD6,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_ADD,				ZEInputEvent("Keyboard",ZE_IKB_ADD,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_NUMPAD1,			ZEInputEvent("Keyboard",ZE_IKB_NUMPAD1,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_NUMPAD2,			ZEInputEvent("Keyboard",ZE_IKB_NUMPAD2,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_NUMPAD3,			ZEInputEvent("Keyboard",ZE_IKB_NUMPAD3,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_NUMPAD0,			ZEInputEvent("Keyboard",ZE_IKB_NUMPAD0,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_DECIMAL,			ZEInputEvent("Keyboard",ZE_IKB_DECIMAL,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_OEM_102,			ZEInputEvent("Keyboard",ZE_IKB_OEM_102,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_F11,				ZEInputEvent("Keyboard",ZE_IKB_F11,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_F12,				ZEInputEvent("Keyboard",ZE_IKB_F12,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_F13,				ZEInputEvent("Keyboard",ZE_IKB_F13,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_F14,				ZEInputEvent("Keyboard",ZE_IKB_F14,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_F15,				ZEInputEvent("Keyboard",ZE_IKB_F15,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_KANA,				ZEInputEvent("Keyboard",ZE_IKB_KANA,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_ABNT_C1,			ZEInputEvent("Keyboard",ZE_IKB_ABNT_C1,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_CONVERT,			ZEInputEvent("Keyboard",ZE_IKB_CONVERT,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_NOCONVERT, 		ZEInputEvent("Keyboard",ZE_IKB_NOCONVERT, 		 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_YEN,				ZEInputEvent("Keyboard",ZE_IKB_YEN,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_ABNT_C2,			ZEInputEvent("Keyboard",ZE_IKB_ABNT_C2,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_NUMPADEQUALS,		ZEInputEvent("Keyboard",ZE_IKB_NUMPADEQUALS,		 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_PREVTRACK,		ZEInputEvent("Keyboard",ZE_IKB_PREVTRACK,		 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_AT,				ZEInputEvent("Keyboard",ZE_IKB_AT,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_COLON,			ZEInputEvent("Keyboard",ZE_IKB_COLON,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_UNDERLINE,		ZEInputEvent("Keyboard",ZE_IKB_UNDERLINE,		 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_KANJI,			ZEInputEvent("Keyboard",ZE_IKB_KANJI,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_STOP,				ZEInputEvent("Keyboard",ZE_IKB_STOP,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_AX,				ZEInputEvent("Keyboard",ZE_IKB_AX,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_UNLABELED,		ZEInputEvent("Keyboard",ZE_IKB_UNLABELED,		 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_NEXTTRACK,		ZEInputEvent("Keyboard",ZE_IKB_NEXTTRACK,		 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_NUMPADENTER,		ZEInputEvent("Keyboard",ZE_IKB_NUMPADENTER,		 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_RCONTROL,			ZEInputEvent("Keyboard",ZE_IKB_RCONTROL,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_MUTE,				ZEInputEvent("Keyboard",ZE_IKB_MUTE,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_CALCULATOR,		ZEInputEvent("Keyboard",ZE_IKB_CALCULATOR,		 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_NUMPADCOMMA,		ZEInputEvent("Keyboard",ZE_IKB_NUMPADCOMMA,		 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_DIVIDE,			ZEInputEvent("Keyboard",ZE_IKB_DIVIDE,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_SYSRQ,			ZEInputEvent("Keyboard",ZE_IKB_SYSRQ,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_RMENU,			ZEInputEvent("Keyboard",ZE_IKB_RMENU,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_PAUSE,			ZEInputEvent("Keyboard",ZE_IKB_PAUSE,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_HOME,				ZEInputEvent("Keyboard",ZE_IKB_HOME,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_UP,				ZEInputEvent("Keyboard",ZE_IKB_UP,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_PRIOR,			ZEInputEvent("Keyboard",ZE_IKB_PRIOR,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_LEFT,				ZEInputEvent("Keyboard",ZE_IKB_LEFT,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_RIGHT,			ZEInputEvent("Keyboard",ZE_IKB_RIGHT,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_END,				ZEInputEvent("Keyboard",ZE_IKB_END,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_DOWN,				ZEInputEvent("Keyboard",ZE_IKB_DOWN,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_NEXT,				ZEInputEvent("Keyboard",ZE_IKB_NEXT,				 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_INSERT,			ZEInputEvent("Keyboard",ZE_IKB_INSERT,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_DELETE,			ZEInputEvent("Keyboard",ZE_IKB_DELETE,			 ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding( ZE_IKB_BACKSPACE,		ZEInputEvent("Keyboard",ZE_IKB_BACKSPACE,			ZE_IBS_PRESSED)));

}

ZEUIManager::~ZEUIManager() 
{
	if (UIRenderer != NULL)
		UIRenderer->Destroy();
}

void ZEUIManager::SetActiveCursor(ZEUICursorControl* Cursor)
{
	this->Cursor = Cursor;
}

void ZEUIManager::AddControl(ZEUIControl* Control)
{
	zeDebugCheck(Controls.FindIndex(Control) != -1, "UI Control already added to ZEUIManager. (Control Name : %s)", Control->GetName());

	Controls.Add(Control);
}

void ZEUIManager::RemoveControl(ZEUIControl* Control)
{
	Controls.DeleteValue(Control);
}

ZEArray<ZEUIControl*>& ZEUIManager::GetControls()
{
	return Controls;
}

#include "ZEFontResource.h"
#include "ZEUICursorControl.h"
#include "ZEUIButtonControl.h"

bool ZEUIManager::Initialize()
{
	if (UIRenderer == NULL)
		UIRenderer = ZEUIRenderer::CreateInstance();

	UIRenderer->Initialize();

	return true;
}

void ZEUIManager::Deinitialize()
{
	UIRenderer->Destroy();
	UIRenderer = NULL;
}

void ZEUIManager::ProcessEvents()
{
	if (Cursor != NULL)
	{
		ZEVector2 CursorPosition = Cursor->GetPosition();

		/************************************************************************/
		/*                          HOVER ENTER AND LEAVE                       */
		/************************************************************************/
		for (size_t I = 0; I < Controls.GetCount(); I++)
		{
			if (Controls[I] == Cursor)
			{
				continue;
			}

			if (Controls[I]->GetVisiblity() == false)
				continue;

			if (LastHoveredControl != NULL && ZERectangle::IntersectionTest(LastHoveredControl->GetVisibleRectangle(), CursorPosition) == false)
			{
				LastHoveredControl->MouseLeaveEvent(CursorPosition);
				LastHoveredControl = NULL;
			}

			ZEUIControl* Reciever = NULL;

			for (size_t I = 0; I < Controls.GetCount(); I++)
			{
				if (Controls[I]->GetVisiblity() == false)
					continue;

				if (ZERectangle::IntersectionTest(Controls[I]->GetVisibleRectangle(), CursorPosition))
				{

					if(Controls[I] == Cursor)
						continue;

					if (Controls[I]->GetChildControls().GetCount() == 0)
					{
						Reciever = Controls[I];
					}

					else
					{
						Reciever = FindEventReciever(Controls[I]);
					}
				}
			}

			if (Reciever != NULL && Reciever != LastHoveredControl && MouseMoveEventFlag == false && !Reciever->GetHovered())
			{
				Reciever->MouseEnterEvent(CursorPosition);

				if (LastHoveredControl != NULL)
					LastHoveredControl->SetHovered(false);

				LastHoveredControl = Reciever;
			}
		}

		/************************************************************************/
		/*                          HOVER ENTER AND LEAVE                       */
		/************************************************************************/

		/************************************************************************/
		/*        MOUSE PRESS, RELEASE EVENT AND FOCUSGAIN, FOCUSLOST           */
		/************************************************************************/

		if (Cursor->GetCurrentButton() != ZE_UI_MOUSE_BUTTON_NONE && PreviousPressedButton == ZE_UI_MOUSE_BUTTON_NONE && LastHoveredControl != NULL)
		{
			LastHoveredControl->MouseButtonPressed(Cursor->GetCurrentButton(), CursorPosition);
			LastPressedControl = LastHoveredControl;
			PreviousPressedButton = Cursor->GetCurrentButton();

			if (LastFocusedControl != NULL && LastFocusedControl != LastPressedControl && LastPressedControl->GetFocusable())
			{
				LastFocusedControl->FocusLost();
				LastFocusedControl = NULL;
			}

			if (LastPressedControl->GetFocusable() && !LastPressedControl->GetFocused() && LastFocusedControl == NULL)
			{
				LastPressedControl->FocusGained();
				LastFocusedControl = LastPressedControl;
			}
		}

		if (LastPressedControl != NULL && Cursor->GetCurrentButton() == ZE_UI_MOUSE_BUTTON_NONE)
		{
			LastPressedControl->MouseButtonReleased(PreviousPressedButton, CursorPosition);
			MouseMoveEventFlag = false;

			if (ZERectangle::IntersectionTest(LastPressedControl->GetVisibleRectangle(), CursorPosition))
			{
				LastHoveredControl = LastPressedControl;
			}

			LastPressedControl = NULL;
			PreviousPressedButton = ZE_UI_MOUSE_BUTTON_NONE;
		}

		if (LastPressedControl != NULL)
			MouseMoveEventFlag = true;

		if (MouseMoveEventFlag == true && LastPressedControl != NULL)
		{
			if (LastPressedControl != Cursor && OldMousePosition != CursorPosition)
				LastPressedControl->MouseMoveEvent(Cursor->GetCurrentButton(), CursorPosition - OldMousePosition);
		}

		if(LastHoveredControl == NULL && LastFocusedControl != NULL && 
			(Cursor->GetCurrentButton() == ZE_UI_MOUSE_BUTTON_LEFT || 
			Cursor->GetCurrentButton() == ZE_UI_MOUSE_BUTTON_RIGHT || 
			Cursor->GetCurrentButton() == ZE_UI_MOUSE_BUTTON_MIDDLE))
		{
			LastFocusedControl->FocusLost();
			LastFocusedControl = NULL;
		}

		/************************************************************************/
		/*        MOUSE PRESS, RELEASE EVENT AND FOCUSGAIN, FOCUSLOST           */
		/************************************************************************/

		OldMousePosition = CursorPosition;
	}

	/************************************************************************/
	/*                          KEYPRESS EVENTS                             */
	/************************************************************************/

	if (LastFocusedControl != NULL)
	{
		zeInput->ProcessInputMap(&InputMap);

		for (size_t I = 0; I < InputMap.InputActionCount; I++)
		{
			switch (InputMap.InputActions[I].Id)
			{
				case ZE_IKB_Q:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_Q);
					break;

				case ZE_IKB_W:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_W);
					break;

				case ZE_IKB_E:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_E);
					break;

				case ZE_IKB_R:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_R);
					break;

				case ZE_IKB_T:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_T);
					break;

				case ZE_IKB_Y:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_Y);
					break;

				case ZE_IKB_U:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_U);
					break;

				case ZE_IKB_I:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_I);
					break;

				case ZE_IKB_O:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_O);
					break;

				case ZE_IKB_P:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_P);
					break;

				case ZE_IKB_A:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_A);
					break;

				case ZE_IKB_S:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_S);
					break;

				case ZE_IKB_D:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_D);
					break;

				case ZE_IKB_F:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_F);
					break;

				case ZE_IKB_G:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_G);
					break;

				case ZE_IKB_H:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_H);
					break;

				case ZE_IKB_J:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_J);
					break;

				case ZE_IKB_K:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_K);
					break;

				case ZE_IKB_L:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_L);
					break;

				case ZE_IKB_Z:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_Z);
					break;

				case ZE_IKB_X:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_X);
					break;

				case ZE_IKB_C:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_C);
					break;

				case ZE_IKB_V:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_V);
					break;

				case ZE_IKB_B:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_B);
					break;

				case ZE_IKB_N:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_N);
					break;

				case ZE_IKB_M:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_M);
					break;

				case ZE_IKB_SPACE:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_SPACE);
					break;

				case ZE_IKB_BACKSPACE:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_BACKSPACE);
					break;

				case ZE_IKB_LEFT:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_LEFT);
					break;

				case ZE_IKB_RIGHT:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_RIGHT);
					break;

				case ZE_IKB_UP:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_UP);
					break;

				case ZE_IKB_DOWN:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_DOWN);
					break;
				
				case ZE_IKB_RETURN:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_RETURN);
					break;

				case ZE_IKB_TAB:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_TAB);
					break;

				case ZE_IKB_PERIOD:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_PERIOD);
					break;

				case ZE_IKB_1:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_1);
					break;
				case ZE_IKB_2:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_2);
					break;
				case ZE_IKB_3:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_3);
					break;
				case ZE_IKB_4:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_4);
					break;
				case ZE_IKB_5:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_5);
					break;
				case ZE_IKB_6:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_6);
					break;
				case ZE_IKB_7:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_7);
					break;
				case ZE_IKB_8:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_8);
					break;
				case ZE_IKB_9:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_9);
					break;
				case ZE_IKB_0:
					LastFocusedControl->KeyPressed(ZE_UI_IKB_0);
					break;

			}
		}
	}

	/************************************************************************/
	/*                          KEYPRESS EVENTS                             */
	/************************************************************************/
}

ZEUIControl* ZEUIManager::FindEventReciever(ZEUIControl* ParentControl)
{
	for (int I = ParentControl->GetChildControls().GetCount() - 1; I >= 0; I--)
	{
		ZEUIControl* CurrentControl = ParentControl->GetChildControls()[I];
		if (ZERectangle::IntersectionTest(CurrentControl->GetVisibleRectangle(), Cursor->GetPosition()))
		{
			if (CurrentControl->GetChildControls().GetCount() != 0)
			{
				return FindEventReciever(CurrentControl);
			}

			else 
				return CurrentControl;
		}
	}

	return ParentControl;
}

void ZEUIManager::Render(ZERenderer* Renderer)
{
	UIRenderer->Clean();
	for (size_t I = 0; I < Controls.GetCount(); I++)
		if (Controls[I]->GetVisiblity())
			Controls[I]->Draw(UIRenderer);

	UIRenderer->Render(Renderer);
}

void ZEUIManager::Tick(float ElapsedTime)
{
	ProcessEvents();

	for (size_t I = 0; I < Controls.GetCount(); I++)
		if (Controls[I]->GetEnabled())
			Controls[I]->Tick(ElapsedTime);
}

void ZEUIManager::Destroy()
{
	delete this;
}

ZEUIManager* ZEUIManager::CreateInstance()
{
	return new ZEUIManager();
}




