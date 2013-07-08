//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInputDeviceExtension.cpp
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

#include "ZEInputDeviceExtension.h"

ZE_EXTENSION_DESCRIPTION_ABSTRACT(ZEInputDeviceModule, ZEExtension, NULL)

bool ZEInputDeviceModule::RegisterDevice(ZEInputDevice* Device)
{
	for (ZESize I = 0; I < Devices.GetCount(); I++)
	{
		if (Devices[I] == Device)
		{
			zeError("Input device already registered. Device Name : \"%s\".", Device->GetName());
			return false;
		}

		if (Devices[I]->GetName() == Device->GetName())
		{
			zeError("A input device with the same name has been already registered. Device Name : \"%s\".", Device->GetName());
			return false;
		}
	}

	Devices.Add(Device);

	if (GetInitializationState() == ZE_IS_INITIALIZING || GetInitializationState() == ZE_IS_INITIALIZED)
	{
		if (!Device->Initialize())
		{
			zeError("Can not initialize input device.");
			return false;
		}
	}

	return true;
}

void ZEInputDeviceModule::UnregisterDevice(ZEInputDevice* Device)
{
	if (GetInitializationState() == ZE_IS_DEINITIALIZING || GetInitializationState() == ZE_IS_NOT_INITIALIZED)
		Device->Deinitialize();

	Devices.DeleteValue(Device);
}

void ZEInputDeviceModule::DestroyDevices()
{
	for (ZESize I = 0; I < Devices.GetCount(); I++)
		Devices[I]->Destroy();

	Devices.Clear();
}


const ZEArray<ZEInputDevice*>& ZEInputDeviceModule::GetDevices()
{
	return Devices;
}

bool ZEInputDeviceModule::DeinitializeSelf()
{
	for (ZESize I = 0; I < Devices.GetCount(); I++)
		Devices[I]->Deinitialize();

	return ZEExtension::Deinitialize();
}

void ZEInputDeviceModule::Acquire()
{
	for (ZESize I = 0; I < Devices.GetCount(); I++)
		Devices[I]->Acquire();
}

void ZEInputDeviceModule::UnAcquire()
{
	for (ZESize I = 0; I < Devices.GetCount(); I++)
		Devices[I]->UnAcquire();
}

ZEInputDeviceModule::~ZEInputDeviceModule()
{
	DestroyDevices();
}
