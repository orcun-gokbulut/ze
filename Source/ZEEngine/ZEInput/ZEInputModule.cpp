//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInputModule.cpp
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

#include "ZEInputModule.h"

#include "ZEDS/ZEArray.h"
#include "ZEMeta/ZEProvider.h"

#include "ZECore/ZECore.h"
#include "ZEInputDevice.h"
#include "ZEInputDeviceModule.h"

ZEInputModule::ZEInputModule()
{
	Acquired = false;
}


ZEInputModule::~ZEInputModule()
{
	Deinitialize();
}

void ZEInputModule::BaseInitialize()
{
}

void ZEInputModule::BaseDeinitialize()
{
}

const ZEArray<ZEInputDeviceModule*>& ZEInputModule::GetDeviceModules()
{
	return DeviceModules;
}

bool ZEInputModule::IsAcquired()
{
	if (!IsInitialized())
		return false;

	return Acquired;
}

void ZEInputModule::Acquire()
{
	if (!IsInitialized())
		return;

	Acquired = true;
	for (ZESize I = 0; I < DeviceModules.GetCount(); I++)
		DeviceModules[I]->Acquire();
}

void ZEInputModule::UnAcquire()
{
	if (!IsInitialized())
		return;

	Acquired = false;
	for (ZESize I = 0; I < DeviceModules.GetCount(); I++)
		DeviceModules[I]->UnAcquire();
}

bool ZEInputModule::InitializeInternal()
{
	if (!ZEModule::InitializeInternal())
		return false;

	zeLog("Initializing Input.");

	zeLog("Loading Input Device modules.");
	ZEArray<ZEClass*> DeviceModuleClasses = ZEProvider::GetInstance()->GetClasses(ZEInputDeviceModule::Class(), true);

	for (ZESize I = 0; I < DeviceModuleClasses.GetCount(); I++)
	{
		ZEInputDeviceModule* DeviceModule = static_cast<ZEInputDeviceModule*>(DeviceModuleClasses[I]->CreateInstance());
		if (DeviceModule == NULL)
		{	
			zeError("Cannot create instance of Input Device Module. Name: \"%s\".", DeviceModuleClasses[I]->GetName());
			continue;
		}

		bool Result = DeviceModule->Initialize();
		if (!Result)
		{
			zeError("Can not initialize input device module.");
			DeviceModule->Destroy();
			continue;
		}

		DeviceModules.Add(DeviceModule);

		const ZEArray<ZEInputDevice*>& Devices = DeviceModule->GetDevices();
		for (ZESize I = 0; I < DeviceModule->GetDevices().GetCount(); I++)
		{
			if (!Devices[I]->Initialize())
				zeError("Cannot initialize input device.");
		}
	}

	return true;
}

bool ZEInputModule::DeinitializeInternal()
{
	for (ZESize I = 0; I < DeviceModules.GetCount(); I++)
		DeviceModules[I]->Destroy();

	DeviceModules.Clear();

	return ZEModule::DeinitializeInternal();
}

void ZEInputModule::Process()
{
	if (!IsInitialized())
		return;

	Acquired = false;
	for (ZESize I = 0; I < DeviceModules.GetCount(); I++)
		DeviceModules[I]->Process();
}

ZEInputModule* ZEInputModule::GetInstance()
{
	return ZECore::GetInstance()->GetInputModule();
}

ZEInputModule* ZEInputModule::CreateInstance()
{
	return new ZEInputModule();
}
