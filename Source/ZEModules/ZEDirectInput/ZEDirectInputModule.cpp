//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDirectInputModule.cpp
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

#include "ZECore/ZECore.h"
#include "ZEError.h"
#include "ZEDirectInputModule.h"

BOOL CALLBACK CheckDirectInputDevices(const LPCDIDEVICEINSTANCE DeviceInstance, LPVOID Ref)
{
	if ((DeviceInstance->dwDevType & DI8DEVCLASS_GAMECTRL) != DI8DEVCLASS_GAMECTRL)
		return TRUE;
	
	ZEDirectInputModule* Module = (ZEDirectInputModule*)Ref;

	ZEDirectInputDevice* Device = new ZEDirectInputDevice();
	Device->DirectInputInstance = *DeviceInstance;
	Device->Module = Module;

	if (!Module->RegisterDevice(Device))
		Device->Destroy();

	return TRUE;
}

bool ZEDirectInputModule::InitializeInternal()
{
	if (!ZEInputDeviceModule::InitializeInternal())
		return false;

	HRESULT hr;
	hr = DirectInput8Create((HINSTANCE)ZECore::GetInstance()->GetApplicationInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL);
	if (FAILED(hr))
	{	
		Deinitialize();
		zeError("Can not create direct input.");
		return false;
	}

	hr = DirectInput->EnumDevices(DI8DEVCLASS_ALL, &CheckDirectInputDevices, this, DIEDFL_ATTACHEDONLY);
	if (FAILED(hr))
	{
		Deinitialize();
		zeError("Can not enumurate devices.");
		return false;
	}

	return true;
}

bool ZEDirectInputModule::DeinitializeInternal()
{
	const ZEArray<ZEInputDevice*>& Devices = GetDevices();
	for(ZESize I = 0; I < Devices.GetCount(); I++)
		Devices[I]->Deinitialize();

	if (DirectInput != NULL)
	{
		DirectInput->Release();
		DirectInput = NULL;
	}

	return ZEInputDeviceModule::DeinitializeInternal();
}

ZEDirectInputModule::ZEDirectInputModule()
{
	DirectInput = NULL;
}

ZEDirectInputModule::~ZEDirectInputModule()
{

}

void ZEDirectInputModule::Process()
{
	const ZEArray<ZEInputDevice*> Devices = GetDevices();
	for (ZESize I = 0; I < Devices.GetSize(); I++)
		((ZEDirectInputDevice*)Devices[I])->Process();

}


ZEDirectInputModule* ZEDirectInputModule::CreateInstance()
{
	return new ZEDirectInputModule();
}
