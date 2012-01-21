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
#include "ZEInput/ZEInputDeviceExtension.h"
#include "ZEInput/ZEInputDevice.h"
#include "ZECore/ZEExtensionManager.h"
#include "ZEWindowsInputMouseDevice.h"
#include "ZEWindowsInputKeyboardDevice.h"

#include "ZECore/ZECore.h"
#include "ZECore/ZEConsole.h"
#include "ZEError.h"

#define WINDIWS_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>

ZE_MODULE_DESCRIPTION(ZEWindowsInputModule, ZEInputModule, NULL)

const ZEArray<ZEInputDevice*>& ZEWindowsInputModule::GetInputDevices()
{
	return Devices;
}

bool ZEWindowsInputModule::Initialize()
{	
	zeLog("Initializing Windows Input.");

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

	UINT NumberOfDevices;
	ZEArray<RAWINPUTDEVICELIST> DeviceList;

	if (GetRawInputDeviceList(NULL, &NumberOfDevices, sizeof(RAWINPUTDEVICELIST)) != 0) 
	{ 
		zeError("Can not load input device list.");
		return false;
	}

	DeviceList.SetCount(NumberOfDevices);
	if (GetRawInputDeviceList(DeviceList.GetCArray(), &NumberOfDevices, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1)
	{
		zeError("Can not load input device list.");
		return false;
	}

	ZEInt MouseIndex = 0;
	ZEInt KeyboardIndex = 0;
	for (ZESize I = 0; I < DeviceList.GetCount(); I++)
	{
		if (DeviceList[I].dwType == RIM_TYPEMOUSE)
		{
			ZEWindowsInputMouseDevice* Device = new ZEWindowsInputMouseDevice();
			Device->DeviceName = "Mouse";
			Device->DeviceHandle = DeviceList[I].hDevice;

			UINT Size = sizeof(RID_DEVICE_INFO);
			GetRawInputDeviceInfo(Device->DeviceHandle, RIDI_DEVICEINFO, &Device->DeviceInfo, &Size);

			Devices.Add(Device);

			MouseIndex++;
		}
		else if (DeviceList[I].dwType == RIM_TYPEKEYBOARD)
		{
			ZEWindowsInputKeyboardDevice* Device = new ZEWindowsInputKeyboardDevice();

			Device->DeviceName = "Keyboard";
			Device->DeviceHandle = DeviceList[I].hDevice;

			UINT Size = sizeof(RID_DEVICE_INFO);
			GetRawInputDeviceInfo(Device->DeviceHandle, RIDI_DEVICEINFO, &Device->DeviceInfo, &Size);

			Devices.Add(Device);

			KeyboardIndex++;
		}
	}

	ZEArray<ZEExtensionDescription*> ExtensionDescriptions = ZEExtensionManager::GetInstance()->GetExtensionDescriptions(ZEInputDeviceExtension::Description());

	for (ZESize I = 0; I < ExtensionDescriptions.GetCount(); I++)
	{
		ZEInputDeviceExtension* Extension = (ZEInputDeviceExtension*)ExtensionDescriptions[I]->CreateInstance();

		ZEArray<ZEInputDevice*> ExtensionDevices = Extension->GetDevices();
		for (ZESize N = 0; N < ExtensionDevices.GetCount(); N++)
			Devices.Add(ExtensionDevices[N]);
	}

	for (ZESize I = 0; I < Devices.GetCount(); I++)
		if (!Devices[I]->Initialize())
		{
			zeError("Can not initialize input device. Name : \"\"", Devices[I]->GetDeviceName());
		}

	return ZEInputModule::Initialize();
}

void ZEWindowsInputModule::Deinitialize()
{
	for (ZESize I = 0; I < Devices.GetCount(); I++)
		Devices[I]->Destroy();
	Devices.Clear();

	ZEInputModule::Deinitialize();
}

void ZEWindowsInputModule::ProcessInputs()
{   
	for (ZESize I = 0; I < Devices.GetCount(); I++)
		Devices[I]->ProcessInputs();
}

void ZEWindowsInputModule::ProcessInputMap(ZEInputMap* InputMap)
{
	InputMap->InputActionCount = 0;

	for (ZESize I = 0; I < InputMap->InputBindings.GetCount(); I++)
		for (ZESize N = 0; N < Devices.GetCount(); N++)
			if (Devices[N]->ProcessInputBinding(&InputMap->InputBindings[I], &InputMap->InputActions[InputMap->InputActionCount]))
				InputMap->InputActionCount++;
}
