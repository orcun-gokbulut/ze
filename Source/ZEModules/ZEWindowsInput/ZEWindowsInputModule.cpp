//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEWindowsInputModule.cpp
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

#include "ZEError.h"
#include "ZEWindowsInputModule.h"
#include "ZEWindowsInputMouseDevice.h"
#include "ZEWindowsInputKeyboardDevice.h"
#include "ZECore\ZESystemMessageManager.h"

#define WINDIWS_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>

ZE_EXTENSION_DESCRIPTION(ZEWindowsInputModule, ZEInputDeviceModule, NULL)

bool ZEWindowsInputSystemMessageHandler::Callback(MSG* Message)
{
	if (Module->RawInputCount >= ZE_MAX_RAW_INPUT_COUNT)
		return false;

	UINT InputSize = sizeof(RAWINPUT);
	if (GetRawInputData((HRAWINPUT)Message->lParam, RID_INPUT, &Module->RawInputs[Module->RawInputCount], &InputSize, sizeof(RAWINPUTHEADER)) == (UINT)-1)
		return false;

	Module->RawInputCount++;

	return true;
}

bool ZEWindowsInputModule::InitializeSelf()
{	
	zeLog("Initializing Windows Input.");

	if (!ZEInputDeviceModule::InitializeSelf())
		return false;

	RAWINPUTDEVICE Rid[2];    
	// Mouse
	Rid[0].usUsagePage = 0x01; 
	Rid[0].usUsage = 0x02; 
	Rid[0].dwFlags = 0;
	Rid[0].hwndTarget = NULL;

	// Keyboard
	Rid[1].usUsagePage = 0x01; 
	Rid[1].usUsage = 0x06; 
	Rid[1].dwFlags = 0;
	Rid[1].hwndTarget = NULL;

	if (RegisterRawInputDevices(Rid, 2, sizeof(RAWINPUTDEVICE)) == FALSE) 
	{
		zeError("Can not register input devices.");
		return false;
	}

	MouseDevice = new ZEWindowsInputMouseDevice();
	MouseDevice->Initialize();
	RegisterDevice(MouseDevice);
	
	KeyboardDevice = new ZEWindowsInputKeyboardDevice();
	KeyboardDevice->Initialize();
	RegisterDevice(KeyboardDevice);

	MessageHandler.Module = this;
	MessageHandler.Register();

	return true;
}

bool ZEWindowsInputModule::DeinitializeSelf()
{
	MessageHandler.Unregister();

	return ZEInputDeviceModule::DeinitializeSelf();
}

void ZEWindowsInputModule::Process()
{
	KeyboardDevice->State.Advance();
	MouseDevice->State.Advance();
	for (ZESize I = 0; I < MouseDevice->State.Axises.CurrentValues.GetCount(); I++)
		MouseDevice->State.Axises.CurrentValues[I] = 0.0f;

	for (ZESize I = 0; I < RawInputCount; I++)
	{
		if (RawInputs[I].header.dwType== RIM_TYPEMOUSE)
			MouseDevice->Process(RawInputs[I]);
		else if (RawInputs[I].header.dwType== RIM_TYPEKEYBOARD)
			KeyboardDevice->Process(RawInputs[I]);
	}

	RawInputCount = 0;
}

ZEWindowsInputModule::ZEWindowsInputModule()
{
	RawInputCount = 0;
}
