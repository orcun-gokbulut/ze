//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEWindowsInputMouseDevice.cpp
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

#include "ZEWindowsInputMouseDevice.h"
#include "ZEInput\ZEInputDefinitions.h"
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

class ZEWMIKSH : public ZESystemMessageHandler
{
	public:	
		ZEWindowsInputMouseDevice* Device;
		virtual bool Callback(MSG* Message);
		ZEWMIKSH(ZEWindowsInputMouseDevice* Device);
};

bool ZEWMIKSH::Callback(MSG* Message)
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
			
			if (Input.header.dwType != RIM_TYPEMOUSE && Input.header.hDevice != Device->DeviceHandle)
				return false;

			Device->AxisState[ZE_IMA_HORIZANTAL_AXIS] += Input.data.mouse.lLastX;
			Device->AxisState[ZE_IMA_VERTICAL_AXIS] += Input.data.mouse.lLastY;

			Device->ButtonState[0] |= (Input.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN);
			Device->ButtonState[1] |= (Input.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN);
			Device->ButtonState[2] |= (Input.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN);
			Device->ButtonState[3] |= (Input.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN);
			Device->ButtonState[4] |= (Input.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN);
			
			if (Input.data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
				Device->AxisState[2] += *((SHORT*)&Input.data.mouse.usButtonData);

			return true;
		}

		default:
			return false;
	}
}

ZEWMIKSH::ZEWMIKSH(ZEWindowsInputMouseDevice* Device)
{
	this->Device = Device;
}


ZEWindowsInputMouseDevice::ZEWindowsInputMouseDevice()
{
	DeviceName = "Mouse";
	MessageHandler = new ZEWMIKSH(this);
}

ZEWindowsInputMouseDevice::~ZEWindowsInputMouseDevice()
{
	delete MessageHandler;
}

const ZEString& ZEWindowsInputMouseDevice::GetDeviceName()
{
	return DeviceName;
}

ZEDWORD ZEWindowsInputMouseDevice::GetButtonCount()
{
	return 5;
}

ZEDWORD ZEWindowsInputMouseDevice::GetAxisCount()
{
	return 3;
}

bool ZEWindowsInputMouseDevice::Initialize()
{	

	ZESystemMessageManager::GetInstance()->RegisterMessageHandler(MessageHandler);

	memset(&ButtonState, 0, sizeof(ButtonState));

	return ZEInputDevice::Initialize();
}

void ZEWindowsInputMouseDevice::Deinitialize()
{
	ZESystemMessageManager::GetInstance()->UnregisterMessageHandler(MessageHandler);

	return ZEInputDevice::Deinitialize();
}

void ZEWindowsInputMouseDevice::ProcessInputs()
{   
	AxisStateOld[0] = AxisState[0];
	AxisStateOld[1] = AxisState[1];
	AxisStateOld[2] = AxisState[2];

	AxisState[0] = 0;
	AxisState[1] = 0;
	AxisState[2] = 0;

	memcpy(ButtonStateOld, ButtonState, sizeof(ButtonState));
	memset(&ButtonState, 0, sizeof(ButtonState));
}

bool ZEWindowsInputMouseDevice::ProcessInputBinding(ZEInputBinding* InputBinding, ZEInputAction* InputAction)
{
	if (InputBinding->Event.Device == this)
	{
		if (InputBinding->Event.Type == ZE_IT_BUTTON)
		{
			int ButtonIndex = InputBinding->Event.Index;

			if (ButtonIndex > 5)
				return false;

			if ((InputBinding->Event.ButtonState == ZE_IBS_PRESSED && ButtonStateOld[ButtonIndex] == false && ButtonState[ButtonIndex] == true) ||
				(InputBinding->Event.ButtonState == ZE_IBS_PRESSING && ButtonState[ButtonIndex] == true))
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
		else if (InputBinding->Event.Type == ZE_IT_AXIS)
		{
			int AxisIndex = InputBinding->Event.Index;

			if (AxisIndex > 2)
				return false;

			if (InputBinding->Event.AxisSign == ZE_IAS_POSITIVE && AxisState[AxisIndex] > 0)
			{
				InputAction->Id = InputBinding->ActionId;
				InputAction->AxisValue = AxisState[AxisIndex];
				InputAction->From = InputBinding;
				return true;
			}
			else if (InputBinding->Event.AxisSign == ZE_IAS_NEGATIVE && AxisState[AxisIndex] < 0)
			{
				InputAction->Id = InputBinding->ActionId;
				InputAction->AxisValue = -AxisState[InputBinding->Event.Index];
				InputAction->From = InputBinding;
				return true;
			}
			else if (InputBinding->Event.AxisSign == ZE_IAS_ALL)
			{
				InputAction->Id = InputBinding->ActionId;
				InputAction->AxisValue = AxisState[InputBinding->Event.Index];
				InputAction->From = InputBinding;
				return true;
			}
		}
	}

	return false;
}
