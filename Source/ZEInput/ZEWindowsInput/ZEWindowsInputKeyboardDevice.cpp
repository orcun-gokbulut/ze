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
#include "ZECore/ZEError.h"

#include "ZEInput/ZEInputEvent.h"
#include "ZEInput/ZEInputMapBinding.h"
#include "ZEInput/ZEInputAction.h"

#include "ZECore/ZESystemMessageHandler.h"
#include "ZECore/ZESystemMessageManager.h"

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
				return;
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

unsigned int ZEWindowsInputKeyboardDevice::GetDeviceId()
{
	return 1;
}

const char* ZEWindowsInputKeyboardDevice::GetDeviceName()
{
	return "Keyboard";
}

unsigned int ZEWindowsInputKeyboardDevice::GetDeviceIndex()
{
	return 1;

}

const ZEArray<ZEInputDescription>& ZEWindowsInputKeyboardDevice::GetInputDescriptions()
{
	ZEArray<ZEInputDescription> Descriptions;
	return Descriptions;
}

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
	memset(&ButtonState, 0, sizeof(ButtonState));
}

bool ZEWindowsInputKeyboardDevice::ProcessInputBinding(ZEInputBinding* InputBinding, ZEInputAction* InputAction)
{
	if (InputBinding->Event.DeviceType == ZE_IDT_KEYBOARD)
	{
		if (InputBinding->Event.InputType == ZE_IT_BUTTON)
		{
			int ButtonIndex = InputBinding->Event.ButtonId;

			if (ButtonIndex > 255)
				return false;

			if ((InputBinding->Event.ButtonState == ZE_IBS_PRESSED && ButtonStateOld[ButtonIndex] == false && ButtonState[ButtonIndex] == true) ||
				(InputBinding->Event.ButtonState == ZE_IBS_ALL && ButtonState[ButtonIndex] == true))
			{
				InputAction->Id = InputBinding->ActionId;
				InputAction->ButtonState = ZE_IBS_PRESSED;
				InputAction->From = InputBinding;
				return true;
			}
			else if (InputBinding->Event.ButtonState == ZE_IBS_RELEASED && ButtonStateOld[ButtonIndex] == false && ButtonState[ButtonIndex] == false)
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
