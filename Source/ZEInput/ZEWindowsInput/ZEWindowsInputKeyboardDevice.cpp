//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEWindowsInputKeyboardDevice.cpp
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

#include "ZEWindowsInputKeyboardDevice.h"

#include "ZECore/ZECore.h"
#include "ZECore/ZEConsole.h"
#include "ZEError.h"

#include "ZEInput/ZEInputEvent.h"
#include "ZEInput/ZEInputMapBinding.h"
#include "ZEInput/ZEInputAction.h"

#include "ZECore/ZESystemMessageHandler.h"
#include "ZECore/ZESystemMessageManager.h"

#include "ZEInput/ZEInputDefinitions.h"

#define WINDIWS_LEAN_AND_MEAN
#include <windows.h>

class ZEWMIKSMH : public ZESystemMessageHandler
{
	public:	
		ZEWindowsInputKeyboardDevice* Device;
		virtual bool Callback(MSG* Message);
		ZEWMIKSMH(ZEWindowsInputKeyboardDevice* Device);
};

bool ZEWMIKSMH::Callback(MSG* Message)
{
	switch(Message->message)
	{
		case WM_INPUT:
		{			
			RAWINPUT Input;
			UINT InputSize = sizeof(RAWINPUT);
			if (GetRawInputData((HRAWINPUT)Message->lParam, RID_INPUT, &Input, &InputSize, sizeof(RAWINPUTHEADER)) == (UINT)-1)
			{
				zeError("WindowsInput", "Can not read raw input data.");
				return false;
			}

			if (Input.header.dwType != RIM_TYPEKEYBOARD && Input.header.hDevice != Device->DeviceHandle)
				return false;

			if ((Input.data.keyboard.Flags & 0x01) == RI_KEY_MAKE)
				Device->ButtonState[Input.data.keyboard.MakeCode] = true;
			else if ((Input.data.keyboard.Flags & 0x01) == RI_KEY_BREAK)
				Device->ButtonState[Input.data.keyboard.MakeCode] = false;

			return true;
		}

		default:
			return false;
	}
}

ZEWMIKSMH::ZEWMIKSMH(ZEWindowsInputKeyboardDevice* Device)
{
	this->Device = Device;
}

ZEWindowsInputKeyboardDevice::ZEWindowsInputKeyboardDevice()
{
	MessageHandler = new ZEWMIKSMH(this);
}

ZEWindowsInputKeyboardDevice::~ZEWindowsInputKeyboardDevice()
{
	delete MessageHandler;
}


const ZEString& ZEWindowsInputKeyboardDevice::GetDeviceName()
{
	return DeviceName;
}

ZEDWORD ZEWindowsInputKeyboardDevice::GetButtonCount()
{
	return 256;
}

void ZEWindowsInputKeyboardDevice::UnAcquire()
{
	memset(&ButtonState, 0, sizeof(ButtonState));
}

/*#define DEFAULT(Num) {Num, #Num, ZE_IT_BUTTON}
ZEInputDescription* ZEWindowsInputKeyboardDevice::GetInputDescriptions()
{
	ZEInputDescription Inputs[] =
	{
		{ZE_IKB_ESCAPE,			"ESC",				ZE_IT_BUTTON},
		{ZE_IKB_1,				"1",				ZE_IT_BUTTON},
		{ZE_IKB_2,				"2",				ZE_IT_BUTTON},
		{ZE_IKB_3,				"3",				ZE_IT_BUTTON},
		{ZE_IKB_4,				"4",				ZE_IT_BUTTON},
		{ZE_IKB_5,				"5",				ZE_IT_BUTTON},
		{ZE_IKB_6,				"6",				ZE_IT_BUTTON},
		{ZE_IKB_7,				"7",				ZE_IT_BUTTON},
		{ZE_IKB_8,				"8",				ZE_IT_BUTTON},
		{ZE_IKB_9,				"9",				ZE_IT_BUTTON},
		{ZE_IKB_0,				"0",				ZE_IT_BUTTON},
		{ZE_IKB_MINUS,			"-",				ZE_IT_BUTTON},
		{ZE_IKB_EQUALS,			"=",				ZE_IT_BUTTON},
		{ZE_IKB_BACK,			"Back",				ZE_IT_BUTTON},
		{ZE_IKB_TAB,			"Tab",				ZE_IT_BUTTON},
		{ZE_IKB_Q,				"q",				ZE_IT_BUTTON},
		{ZE_IKB_W,				"w",				ZE_IT_BUTTON},
		{ZE_IKB_E,				"e",				ZE_IT_BUTTON},
		{ZE_IKB_R,				"r",				ZE_IT_BUTTON},
		{ZE_IKB_T,				"t",				ZE_IT_BUTTON},
		{ZE_IKB_Y,				"y",				ZE_IT_BUTTON},
		{ZE_IKB_U,				"u",				ZE_IT_BUTTON},
		{ZE_IKB_I,				"i",				ZE_IT_BUTTON},
		{ZE_IKB_O,				"o",				ZE_IT_BUTTON},
		{ZE_IKB_P,				"p",				ZE_IT_BUTTON},
		{ZE_IKB_LBRACKET,		"<",				ZE_IT_BUTTON},
		{ZE_IKB_RBRACKET,		">",				ZE_IT_BUTTON},
		{ZE_IKB_RETURN,			"Enter",			ZE_IT_BUTTON},
		{ZE_IKB_LCONTROL,		"L-Ctrl",			ZE_IT_BUTTON},
		{ZE_IKB_A,				"a",				ZE_IT_BUTTON},
		{ZE_IKB_S,				"s",				ZE_IT_BUTTON},
		{ZE_IKB_D,				"d",				ZE_IT_BUTTON},
		{ZE_IKB_F,				"f",				ZE_IT_BUTTON},
		{ZE_IKB_G,				"g",				ZE_IT_BUTTON},
		{ZE_IKB_H,				"h",				ZE_IT_BUTTON},
		{ZE_IKB_J,				"j",				ZE_IT_BUTTON},
		{ZE_IKB_K,				"k",				ZE_IT_BUTTON},
		{ZE_IKB_L,				"l",				ZE_IT_BUTTON},
		{ZE_IKB_SEMICOLON,		":",				ZE_IT_BUTTON},
		{ZE_IKB_APOSTROPHE,		"1",				ZE_IT_BUTTON},
		{ZE_IKB_GRAVE,			"1",				ZE_IT_BUTTON},
		{ZE_IKB_LSHIFT,			"L-Shift",			ZE_IT_BUTTON},
		{ZE_IKB_BACKSLASH,		"\\",				ZE_IT_BUTTON},
		{ZE_IKB_Z,				"z",				ZE_IT_BUTTON},
		{ZE_IKB_X,				"x",				ZE_IT_BUTTON},
		{ZE_IKB_C,				"c",				ZE_IT_BUTTON},
		{ZE_IKB_V,				"v",				ZE_IT_BUTTON},
		{ZE_IKB_B,				"b",				ZE_IT_BUTTON},
		{ZE_IKB_N,				"n",				ZE_IT_BUTTON},
		{ZE_IKB_M,				"m",				ZE_IT_BUTTON},
		{ZE_IKB_COMMA,			",",				ZE_IT_BUTTON},
		{ZE_IKB_PERIOD,			".",				ZE_IT_BUTTON},
		{ZE_IKB_SLASH,			"/",				ZE_IT_BUTTON},
		{ZE_IKB_RSHIFT,			"R-Shift",			ZE_IT_BUTTON},
		{ZE_IKB_MULTIPLY,		"*",				ZE_IT_BUTTON},
		{ZE_IKB_LMENU,			"1",				ZE_IT_BUTTON},
		{ZE_IKB_SPACE,			"Space",			ZE_IT_BUTTON},
		{ZE_IKB_CAPITAL,		"Caps",				ZE_IT_BUTTON},
		{ZE_IKB_F1,				"F1",				ZE_IT_BUTTON},
		{ZE_IKB_F2,				"F2",				ZE_IT_BUTTON},
		{ZE_IKB_F3,				"F3",				ZE_IT_BUTTON},
		{ZE_IKB_F4,				"F4",				ZE_IT_BUTTON},
		{ZE_IKB_F5,				"F5",				ZE_IT_BUTTON},
		{ZE_IKB_F6,				"F6",				ZE_IT_BUTTON},
		{ZE_IKB_F7,				"F7",				ZE_IT_BUTTON},
		{ZE_IKB_F8,				"F8",				ZE_IT_BUTTON},
		{ZE_IKB_F9,				"F9",				ZE_IT_BUTTON},
		{ZE_IKB_F10,			"F10",				ZE_IT_BUTTON},
		{ZE_IKB_NUMLOCK,		"Num",				ZE_IT_BUTTON},
		{ZE_IKB_SCROLL,			"Scroll",			ZE_IT_BUTTON},
		{ZE_IKB_NUMPAD7,		"Num 7",			ZE_IT_BUTTON},
		{ZE_IKB_NUMPAD8,		"Num 8",			ZE_IT_BUTTON},
		{ZE_IKB_NUMPAD9,		"Num 9",			ZE_IT_BUTTON},
		{ZE_IKB_SUBTRACT,		"Num -",			ZE_IT_BUTTON},
		{ZE_IKB_NUMPAD4,		"Num 4",			ZE_IT_BUTTON},
		{ZE_IKB_NUMPAD5,		"Num 5",			ZE_IT_BUTTON},
		{ZE_IKB_NUMPAD6,		"Num 6",			ZE_IT_BUTTON},
		{ZE_IKB_ADD,			"Num +",			ZE_IT_BUTTON},
		{ZE_IKB_NUMPAD1,		"Num 1",			ZE_IT_BUTTON},
		{ZE_IKB_NUMPAD2,		"Num 2",			ZE_IT_BUTTON},
		{ZE_IKB_NUMPAD3,		"Num 3",			ZE_IT_BUTTON},
		{ZE_IKB_NUMPAD0,		"Num 0",			ZE_IT_BUTTON},
		{ZE_IKB_DECIMAL,		"Decimal",			ZE_IT_BUTTON},
		{ZE_IKB_OEM_102,		"0x54",				ZE_IT_BUTTON},
		DEFAULT(0x55),
		DEFAULT(0x56),
		{ZE_IKB_F11,			"F11",				ZE_IT_BUTTON},
		{ZE_IKB_F12,			"F12",				ZE_IT_BUTTON},
		DEFAULT(0x58),
		DEFAULT(0x59),
		DEFAULT(0x50),
		DEFAULT(0x5A),
		DEFAULT(0x5B),
		DEFAULT(0x5C),
		DEFAULT(0x5D),
		DEFAULT(0x5E),
		DEFAULT(0x5F),
		DEFAULT(0x60),
		DEFAULT(0x61),
		DEFAULT(0x62),
		DEFAULT(0x63),
		{ZE_IKB_F13,			"F13",				ZE_IT_BUTTON},
		{ZE_IKB_F14,			"F14",				ZE_IT_BUTTON},
		{ZE_IKB_F15,			"F15",				ZE_IT_BUTTON},
		DEFAULT(0x67),
		DEFAULT(0x68),
		DEFAULT(0x69),
		DEFAULT(0x6A),
		DEFAULT(0x6B),
		DEFAULT(0x6C),
		DEFAULT(0x6D),
		DEFAULT(0x6E),
		DEFAULT(0x6F),
		{ZE_IKB_KANA,			"0x70",				ZE_IT_BUTTON},
		DEFAULT(0x71),
		DEFAULT(0x72),
		{ZE_IKB_ABNT_C1,		"0x73",				ZE_IT_BUTTON},
		DEFAULT(0x74),
		DEFAULT(0x75),
		DEFAULT(0x76),
		DEFAULT(0x77),
		DEFAULT(0x78),
		{ZE_IKB_CONVERT,		"0x79",				ZE_IT_BUTTON},
		DEFAULT(0x7A),
		{ZE_IKB_NOCONVERT,		"0x7B",				ZE_IT_BUTTON},
		DEFAULT(0x7C),
		{ZE_IKB_YEN,			"0x7D",				ZE_IT_BUTTON},
		{ZE_IKB_ABNT_C2,		"0x7E",				ZE_IT_BUTTON},
		DEFAULT(0x7F),
		DEFAULT(0x80),
		DEFAULT(0x81),
		DEFAULT(0x82),
		DEFAULT(0x83),
		DEFAULT(0x84),
		DEFAULT(0x85),
		DEFAULT(0x86),
		DEFAULT(0x87),
		DEFAULT(0x88),
		DEFAULT(0x89),
		DEFAULT(0x8A),
		DEFAULT(0x8B),
		DEFAULT(0x8C),
		{ZE_IKB_NUMPADEQUALS,	"0x8D",				ZE_IT_BUTTON},
		DEFAULT(0x8E),
		DEFAULT(0x8F),
		{ZE_IKB_PREVTRACK,		"0x90",				ZE_IT_BUTTON},
		{ZE_IKB_AT,				"@",				ZE_IT_BUTTON},
		{ZE_IKB_COLON,			"|",				ZE_IT_BUTTON},
		{ZE_IKB_UNDERLINE,		"_",				ZE_IT_BUTTON},
		{ZE_IKB_KANJI,			"0x94",				ZE_IT_BUTTON},
		{ZE_IKB_STOP,			"0x95",				ZE_IT_BUTTON},
		{ZE_IKB_AX,				"0x96",				ZE_IT_BUTTON},
		{ZE_IKB_UNLABELED,		"0x97",				ZE_IT_BUTTON},
		{ZE_IKB_NEXTTRACK,		"0x99",				ZE_IT_BUTTON},
		{ZE_IKB_NUMPADENTER,	"Num Enter",		ZE_IT_BUTTON},
		{ZE_IKB_RCONTROL,		"R-Control",		ZE_IT_BUTTON},
		{ZE_IKB_MUTE,			"Mute",				ZE_IT_BUTTON},
		{ZE_IKB_CALCULATOR,		"Calc",				ZE_IT_BUTTON},
		{ZE_IKB_PLAYPAUSE,		"Play/Pause",		ZE_IT_BUTTON},
		{ZE_IKB_MEDIASTOP,		"Stop",				ZE_IT_BUTTON},
		{ZE_IKB_VOLUMEDOWN,		"Vol Down",			ZE_IT_BUTTON},
		{ZE_IKB_VOLUMEUP,		"Vol Down",			ZE_IT_BUTTON},
		{ZE_IKB_WEBHOME,		"Home",				ZE_IT_BUTTON},
		{ZE_IKB_NUMPADCOMMA,	"Num ,",			ZE_IT_BUTTON},
		{ZE_IKB_DIVIDE,			"Num Div",			ZE_IT_BUTTON},
		{ZE_IKB_SYSRQ,			"SysRQ",			ZE_IT_BUTTON},
		{ZE_IKB_RMENU,			"Right Menu",		ZE_IT_BUTTON},
		{ZE_IKB_PAUSE,			"Pause",			ZE_IT_BUTTON},
		{ZE_IKB_HOME,			"Home",				ZE_IT_BUTTON},
		{ZE_IKB_UP,				"Up",				ZE_IT_BUTTON},
		{ZE_IKB_PRIOR,			"Prior",			ZE_IT_BUTTON},
		{ZE_IKB_LEFT,			"Left",				ZE_IT_BUTTON}, 
		{ZE_IKB_RIGHT,			"Right",			ZE_IT_BUTTON}, 
		{ZE_IKB_END,			"End",				ZE_IT_BUTTON},
		{ZE_IKB_DOWN,			"Down",				ZE_IT_BUTTON},
		{ZE_IKB_NEXT,			"Next",				ZE_IT_BUTTON},
		{ZE_IKB_INSERT,			"Insert",			ZE_IT_BUTTON},
		{ZE_IKB_DELETE,			"Delete",			ZE_IT_BUTTON},
		{ZE_IKB_LWIN,			"Left Win",			ZE_IT_BUTTON},
		{ZE_IKB_RWIN,			"Right Win",		ZE_IT_BUTTON},
		{ZE_IKB_APPS,			"Apps",				ZE_IT_BUTTON},
		{ZE_IKB_POWER,			"Power",			ZE_IT_BUTTON},
		{ZE_IKB_SLEEP,			"Sleep",			ZE_IT_BUTTON},
		{ZE_IKB_WAKE,			"Wake",				ZE_IT_BUTTON},
		{ZE_IKB_WEBSEARCH,		"WebSerach",		ZE_IT_BUTTON},
		{ZE_IKB_WEBFAVORITES,	"Web Fav",			ZE_IT_BUTTON},
		{ZE_IKB_WEBREFRESH,		"Web Refresh",		ZE_IT_BUTTON},
		{ZE_IKB_WEBSTOP,		"Web Stop",			ZE_IT_BUTTON},
		{ZE_IKB_WEBFORWARD,		"Web Forward",		ZE_IT_BUTTON},
		{ZE_IKB_WEBBACK,		"Web Back",			ZE_IT_BUTTON},
		{ZE_IKB_MYCOMPUTER,		"My Computer",		ZE_IT_BUTTON},
		{ZE_IKB_MAIL,			"Mail",				ZE_IT_BUTTON},
		{ZE_IKB_MEDIASELECT,	"Media Select",		ZE_IT_BUTTON},
	};

	return Inputs;
}*/

bool ZEWindowsInputKeyboardDevice::Initialize()
{	

	ZESystemMessageManager::GetInstance()->RegisterMessageHandler(MessageHandler);

	memset(&ButtonState, 0, sizeof(ButtonState));

	return ZEInputDevice::Initialize();
}

void ZEWindowsInputKeyboardDevice::Deinitialize()
{
	ZESystemMessageManager::GetInstance()->UnregisterMessageHandler(MessageHandler);

	return ZEInputDevice::Deinitialize();
}

void ZEWindowsInputKeyboardDevice::ProcessInputs()
{   
	memcpy(ButtonStateOld, ButtonState, sizeof(ButtonState));
}

bool ZEWindowsInputKeyboardDevice::ProcessInputBinding(ZEInputBinding* InputBinding, ZEInputAction* InputAction)
{
	if (InputBinding->Event.Device == this)
	{
		if (InputBinding->Event.Type == ZE_IT_BUTTON)
		{
			int ButtonIndex = InputBinding->Event.Index;

			if (ButtonIndex > 255)
				return false;

			if (InputBinding->Event.ButtonState == ZE_IBS_PRESSING && ButtonState[ButtonIndex] == true)
			{
				InputAction->Id = InputBinding->ActionId;
				InputAction->ButtonState = ZE_IBS_PRESSED;
				InputAction->From = InputBinding;
				return true;
			}
			else if (InputBinding->Event.ButtonState == ZE_IBS_PRESSED && ButtonStateOld[ButtonIndex] == false && ButtonState[ButtonIndex] == true)
			{
				InputAction->Id = InputBinding->ActionId;
				InputAction->ButtonState = ZE_IBS_PRESSED;
				InputAction->From = InputBinding;
				return true;
			}
			else if (InputBinding->Event.ButtonState == ZE_IBS_RELEASED && ButtonStateOld[ButtonIndex] == true && ButtonState[ButtonIndex] == false)
			{
				InputAction->Id = InputBinding->ActionId;
				InputAction->ButtonState = ZE_IBS_RELEASED;
				InputAction->From = InputBinding;
				return true;
			}
		}
	}

	return false;
}
