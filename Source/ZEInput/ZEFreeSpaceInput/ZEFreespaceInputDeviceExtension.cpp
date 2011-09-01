//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFreespaceInputDeviceExtension.cpp
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

#include "ZECore/ZEError.h"
#include "ZECore/ZEConsole.h"
#include "ZEFreespaceInputDeviceExtension.h"
#include "ZEFreespaceInputDeviceExtensionDescription.h"

#include <freespace/freespace.h>


ZEExtensionDescription* ZEFreespaceInputDeviceExtension::GetExtensionDescription()
{
	return ZEFreespaceInputDeviceExtension::ExtensionDescription();
}

ZEExtensionDescription* ZEFreespaceInputDeviceExtension::ExtensionDescription()
{
	static ZEFreespaceInputDeviceExtensionDescription Desc;
	return &Desc;
}

ZEArray<ZEInputDevice*> ZEFreespaceInputDeviceExtension::GetDevices()
{
	ZEArray<ZEInputDevice*> DevicePointers;
	DevicePointers.MassAdd(Devices.GetCount());
	
	for (size_t I = 0; I < Devices.GetCount(); I++)
		DevicePointers[I] = &Devices[I];
	
	return DevicePointers;
}

bool ZEFreespaceInputDeviceExtension::Initialize()
{
	zeLog("Freespace Input Module", "Initializing module.");

	int Result;
	Result = freespace_init();
	if (Result != FREESPACE_SUCCESS) 
	{
		zeError("Freespace Input Module", "Can not initialize Freespace. Error Code : %d.", Result);
		return false;
	}

	FreespaceDeviceId DeviceIds[256];
	int NumberOfDevices = 0;
	Result = freespace_getDeviceList(DeviceIds, 256, &NumberOfDevices);
	if (NumberOfDevices == 0) 
	{
		zeWarning("Freespace Input Device Extension", "Can not aquire number of freespace devices. Error Code : %d.", Result);
		return false;
	}

	Devices.SetCount(NumberOfDevices);
	for(size_t I = 0; I < NumberOfDevices; I++)
	{
		Devices[I].Device = DeviceIds[I];
		freespace_getDeviceInfo(I, &Devices[I].DeviceInfo);
	}
	
	zeLog("Freespace Input Device Extension", "Found %d number of devices.", NumberOfDevices);
	
	return true;
}

void ZEFreespaceInputDeviceExtension::Deinitialize()
{
	zeLog("Freespace Input Module", "Deinitializing module.");

	for (size_t I = 0; I < Devices.GetCount(); I++)
		Devices[I].Deinitialize();

	Devices.Clear();

	freespace_exit();

	zeLog("Freespace Input Device Extension", "Module deinitialized.");
}
