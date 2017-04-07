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

#include "ZEWindowsInputModule.h"

#include "ZEError.h"
#include "ZEWindowsInputMouseDevice.h"
#include "ZEWindowsInputKeyboardDevice.h"
#include "ZECore/ZESystemMessageManager.h"

#include <string.h>
#include <memory.h>
#include <stdio.h>

bool ZEWindowsInputSystemMessageHandler::Callback(MSG* Message)
{
	if (Module->MessageCount >= ZE_MAX_RAW_INPUT_COUNT)
		return false;

	Module->Messages[Module->MessageCount].Message = *Message;
	if (Message->message == WM_INPUT)
	{
		UINT InputSize = sizeof(RAWINPUT);
		if (GetRawInputData((HRAWINPUT)Message->lParam, RID_INPUT, &Module->Messages[Module->MessageCount].RawInput, &InputSize, sizeof(RAWINPUTHEADER)) == (UINT)-1)
			return false;
	}

	Module->MessageCount++;

	return true;
}

ZEWindowsInputModule::ZEWindowsInputModule()
{
	MouseDevice = NULL;
	KeyboardDevice = NULL;
	MessageCount = 0;
}

ZEWindowsInputModule::~ZEWindowsInputModule()
{
	Deinitialize();
}

bool ZEWindowsInputModule::InitializeInternal()
{	
	zeLog("Initializing Windows Input.");

	if (!ZEInputDeviceModule::InitializeInternal())
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

	if (MouseDevice == NULL)
	{
		MouseDevice = new ZEWindowsInputMouseDevice();
		RegisterDevice(MouseDevice);
	}
	
	if (KeyboardDevice == NULL)
	{
		KeyboardDevice = new ZEWindowsInputKeyboardDevice();
		RegisterDevice(KeyboardDevice);
	}

	MessageHandler.Module = this;
	MessageHandler.Register();

	return true;
}

bool ZEWindowsInputModule::DeinitializeInternal()
{
	KeyboardDevice->Deinitialize();
	MouseDevice->Deinitialize();
	MessageHandler.Unregister();

	return ZEInputDeviceModule::DeinitializeInternal();
}

void ZEWindowsInputModule::Process(const ZETimeParameters* Parameters)
{
	MouseDevice->Process(Messages, MessageCount);
	KeyboardDevice->Process(Messages, MessageCount);
	MessageCount = 0;
}

ZEWindowsInputModule* ZEWindowsInputModule::CreateInstance()
{
	return new ZEWindowsInputModule();
}
