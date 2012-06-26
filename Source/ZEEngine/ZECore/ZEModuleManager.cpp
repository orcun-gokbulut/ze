//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModuleManager.cpp
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
#include "ZEModuleManager.h"
#include "ZEError.h"
#include "ZECore/ZEConsole.h"
#include "ZEOptionManager.h"
#include "ZEModule.h"
#include "ZEDefinitions.h"
#include "ZECompileOptions.h"

ZEOptionSection ZEModuleManager::ModuleManagerOptions;

const ZEArray<ZEExtensionDescription*>& ZEModuleManager::GetModuleDescriptions()
{
	return ModuleList;
}

ZEExtensionDescription* ZEModuleManager::GetModuleDescription(ZESize Index)
{
	return ModuleList[Index];
}

ZEExtensionDescription* ZEModuleManager::GetModuleDescription(const ZEString& Name)
{
	for (ZESize I = 0; I < ModuleList.GetCount(); I++)
		if (ModuleList[I]->GetName() == Name)
			return ModuleList[I];

	return NULL;
}

ZEExtensionDescription* ZEModuleManager::GetModuleDescription(ZEExtensionDescription* BaseModuleDescription)
{
	if (BaseModuleDescription->GetParent() != ZEModule::Description())
		return NULL;

	return GetModuleDescription(ZEString(ModuleManagerOptions.GetOption(BaseModuleDescription->GetName())->GetValue().GetString()));
}

ZEModule* ZEModuleManager::CreateModuleInstance(ZESize Index)
{
	ZEExtensionDescription* ModuleDesc = GetModuleDescription(Index);

	if (ModuleDesc == NULL)
	{
		zeError("Can not find module. Module Index : %Iu", Index);

		return NULL;
	}

	ZEModule* Module = (ZEModule*)ModuleDesc->CreateInstance();
	if (Module == NULL)
	{
		zeError("Can not create module. Module Name : \"%s\"",
			(const char*)ModuleDesc->GetName());

		return NULL;
	}

	return Module;
}

ZEModule* ZEModuleManager::CreateModuleInstance(const ZEString& Name)
{
	ZEExtensionDescription* ModuleDesc = GetModuleDescription(Name);
	if (ModuleDesc == NULL)
	{
		zeError("Can not find module. Module Name : \"%s\"", (const char*)Name);

		return NULL;
	}

	ZEModule* Module = (ZEModule*)ModuleDesc->CreateInstance();
	if (Module == NULL)
	{
		zeError("Can not create module. Module Name : \"%s\"", (const char*)ModuleDesc->GetName());

		return NULL;
	}

	return Module;
}


ZEModule* ZEModuleManager::CreateModuleInstance(ZEExtensionDescription* BaseModuleDescription)
{
	ZEExtensionDescription* ModuleDesc = GetModuleDescription(BaseModuleDescription);
	if (ModuleDesc == NULL)
	{
		zeCriticalError("Can not find \"%s\" module. Please check your options.ini.", (const char*)BaseModuleDescription->GetName());

		return NULL;
	}

	ZEModule* Module = (ZEModule*)ModuleDesc->CreateInstance();
	if (Module == NULL)
	{
		zeCriticalError("Can not create module. Module Name : \"%s\"",
			(const char*)ModuleDesc->GetName());

		return NULL;
	}

	return Module;
}

bool ZEModuleManager::RegisterModule(ZEExtensionDescription* ModuleDesc)
{
	zeLog("Loading Internal Module \"%s\".", (const char*)ModuleDesc->GetName());

	ZEExtensionDescription* Desc = GetModuleDescription(ModuleDesc->GetName());
	if (Desc != NULL)
	{
		zeError("Can not load module. Module is already loaded. Module Name : \"%s\"", 
			(const char*)ModuleDesc->GetName());
		return false;
	}
	
	if (ModuleDesc->GetOptions() != NULL)
	{
		bool Result;
		Result = ZEOptionManager::GetInstance()->RegisterSection(ModuleDesc->GetOptions());
		if (!Result)
		{
			zeError("Can not register module's option section. Module Name : \"%s\"", 
				(const char*)ModuleDesc->GetName());
		}
	}

	ModuleList.Add(ModuleDesc);
	return true;
}

void ZEModuleManager::UnregisterModule(ZEExtensionDescription* ModuleDesc)
{
	ModuleList.DeleteValue(ModuleDesc);
}

#include "ZEModules/ZEDirect3D9/ZED3D9Module.h"
#include "ZEModules/ZEDirectSound/ZEDSModule.h"
#include "ZEModules/ZEDirectInput/ZEDirectInputModule.h"
#include "ZEModules/ZEDummyInput/ZEDummyInputModule.h"
#include "ZEModules/ZEWindowsInput/ZEWindowsInputModule.h"
#include "ZEModules/ZEVirtualInput/ZEVirtualInputModule.h"
#include "ZEModules/ZEOpenAL/ZEALModule.h"
#include "ZEModules/ZEPhysX/ZEPhysXModule.h"

ZEModuleManager::ZEModuleManager()
{
	RegisterModule(ZED3D9Module::Description());
	RegisterModule(ZEDSModule::Description());
	/*RegisterModule(ZEDummyInputModule::Description());
	RegisterModule(ZEVirtualInputModule::Description());
	RegisterModule(ZEDirectInputModule::Description());*/
	RegisterModule(ZEALModule::Description());
	RegisterModule(ZEPhysXModule::Description());
	RegisterModule(ZEWindowsInputModule::Description());

	ModuleManagerOptions.SetName("ModuleManager");
	ModuleManagerOptions.AddOption(new ZEOption("ZEGraphicsModule", "ZED3D9Module", ZE_OA_NORMAL));
	ModuleManagerOptions.AddOption(new ZEOption("ZEInputModule", "ZEWindowsInputModule", ZE_OA_NORMAL));
	ModuleManagerOptions.AddOption(new ZEOption("ZESoundModule", "ZEDSModule", ZE_OA_NORMAL));
	ModuleManagerOptions.AddOption(new ZEOption("ZENetworkModule", "ZEWinNetwork", ZE_OA_NORMAL));
	ModuleManagerOptions.AddOption(new ZEOption("ZEPhysicsModule", "ZEPhysXModule", ZE_OA_NORMAL));
	ModuleManagerOptions.AddOption(new ZEOption("ZEGameModule", "ZETestGame", ZE_OA_NORMAL));
	ZEOptionManager::GetInstance()->RegisterSection(&ModuleManagerOptions);
}

ZEModuleManager::~ZEModuleManager()
{

}

ZEModuleManager* ZEModuleManager::GetInstance()
{
	return ZECore::GetInstance()->GetModuleManager();
}
