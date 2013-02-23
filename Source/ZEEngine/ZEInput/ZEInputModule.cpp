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
#include "ZEInputDevice.h"
#include "ZECore/ZECore.h"
#include "ZEDS/ZEArray.h"
#include "ZEInputDeviceExtension.h"
#include "ZECore/ZEExtensionManager.h"

ZE_MODULE_DESCRIPTION(ZEInputModule, ZEModule, NULL)

ZEInputModule::ZEInputModule()
{
	Acquired = false;
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

bool ZEInputModule::Initialize()
{
	if (IsInitialized())
		return false;

	zeLog("Initializing Input.");

	zeLog("Loading Input Device modules.");
	ZEArray<ZEExtensionDescription*> DeviceModuleDescriptions = ZEExtensionManager::GetInstance()->GetExtensionDescriptions(ZEInputDeviceModule::Description());

	for (ZESize I = 0; I < DeviceModuleDescriptions.GetCount(); I++)
	{
		ZEInputDeviceModule* DeviceModule = (ZEInputDeviceModule*)DeviceModuleDescriptions[I]->CreateInstance();
		bool Result = DeviceModule->Initialize();
		if (!Result)
		{
			zeLog("Can not initialize input device module.");
			continue;
		}

		DeviceModules.Add(DeviceModule);
	}

	return ZEModule::Initialize();
}

void ZEInputModule::Deinitialize()
{
	if (!IsInitialized())
		return;

	for (ZESize I = 0; I < DeviceModules.GetCount(); I++)
		DeviceModules[I]->Destroy();

	DeviceModules.Clear();

	ZEModule::Deinitialize();
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
