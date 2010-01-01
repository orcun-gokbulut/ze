//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ModuleManager.cpp
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

#include "ModuleManager.h"
#include "Core/Core.h"

ZEOptionSection ZEModuleManager::ModuleManagerOptions;

bool ZEModuleManager::CheckModule(ZEModuleDescription* ModuleDesc)
{
	ZEModuleDescription* Desc = GetModuleDescription(ModuleDesc->GetName());
	if (Desc != NULL)
	{
		zeError("Module Manager", 
			"Can not load module. Module is allready loaded. (Module Name : \"%s\", Module Version = %d.%d)", 
			ModuleDesc->GetName(), ModuleDesc->GetMajorVersion(), ModuleDesc->GetMinorVersion());
		return false;
	}

	if (!ModuleDesc->CheckCompatible())
	{
		zeError("Moudle Manager",
			"Module compatible check failed. Module is not compatible with this system.  (Module Name : \"%s\", Module Version : %d.%d)",
			ModuleDesc->GetName(), ModuleDesc->GetMajorVersion(), ModuleDesc->GetMinorVersion());
		return false;
	}

/*#ifndef ZEDEBUG_ENABLED
	if ((ModuleDesc->GetAttributes() & ZE_MA_DEBUG))
	{
		zeError("Module Manager", 
			"Can not load module. Module is a debug module. Core must be in debug mode to load debug modules. (Module Name : \"%s\", Module Version : %d.%d)",
			ModuleDesc->GetName(), ModuleDesc->GetMajorVersion(), ModuleDesc->GetMinorVersion());
		return false;
	}
#endif*/

	if (ModuleDesc->GetRequiredZinekEngineVersion() != ZEVERSION_MAJORNUMBER)
	{
		zeError("Module Manager", 
			"Can not load module. Required Zinek Engine version does no match. This module requires Zinek Engine version %d.0. (Module Name : \"%s\", Module Version : %d.%d)",
			ModuleDesc->GetRequiredZinekEngineVersion(), ModuleDesc->GetName(), ModuleDesc->GetMajorVersion(), ModuleDesc->GetMinorVersion());
		return false;
	}
	return true;
}

size_t ZEModuleManager::GetModuleCount()
{
	return ModuleList.GetCount();
}

ZEModuleDescription* ZEModuleManager::GetModuleDescription(size_t Index)
{
	return ModuleList[Index];
}

ZEModuleDescription* ZEModuleManager::GetModuleDescription(const char* Name)
{
	for (size_t I = 0; I < ModuleList.GetCount(); I++)
		if (_stricmp(ModuleList[I]->GetName(), Name) == 0)
			return ModuleList[I];

	return NULL;
}

ZEModuleDescription* ZEModuleManager::GetModuleDescription(ZEModuleType Type)
{
	char* ModuleTypeName;
	switch(Type)
	{
		case ZE_MT_GRAPHICS:
			ModuleTypeName = "GraphicsModule";
			break;
		case ZE_MT_INPUT:
			ModuleTypeName = "InputModule";
			break;
		case ZE_MT_SOUND:
			ModuleTypeName = "SoundModule";
			break;
		case ZE_MT_PHYSICS:
			ModuleTypeName = "PhysicsModule";
			break;
		case ZE_MT_NETWORK:
			ModuleTypeName = "NetworkModule";
			break;
		default:
			return NULL;
	}

	return GetModuleDescription(ModuleManagerOptions.GetOption(ModuleTypeName)->GetValue());
}

ZEModule* ZEModuleManager::CreateModule(size_t Index)
{
	ZEModuleDescription* ModuleDesc = GetModuleDescription(Index);

	if (ModuleDesc == NULL)
	{
		zeError("Module Manager", 
			"Can not find module. (Module Index : %d)",
			Index);

		return NULL;
	}

	ZEModule* Module = ModuleDesc->CreateModuleInstance();
	if (Module == NULL)
	{
		zeError("Module Manager", 
			"Can not create module. (Module Name : \"%s\", Module Version : %d.%d)",
			ModuleDesc->GetName(), ModuleDesc->GetMajorVersion(), ModuleDesc->GetMinorVersion());

		return NULL;
	}

	return Module;
}

ZEModule* ZEModuleManager::CreateModule(const char* Name)
{
	ZEModuleDescription* ModuleDesc = GetModuleDescription(Name);
	if (ModuleDesc == NULL)
	{
		zeError("Module Manager", 
			"Can not find module. (Module Name : \"%s\")",
			Name);

		return NULL;
	}

	ZEModule* Module = ModuleDesc->CreateModuleInstance();
	if (Module == NULL)
	{
		zeError("Module Manager", 
			"Can not create module. (Module Name : \"%s\", Module Version : %d.%d)",
			ModuleDesc->GetName(), ModuleDesc->GetMajorVersion(), ModuleDesc->GetMinorVersion());

		return NULL;
	}

	return Module;
}


ZEModule* ZEModuleManager::CreateModule(ZEModuleType ModuleType)
{
	ZEModuleDescription* ModuleDesc = GetModuleDescription(ModuleType);
	if (ModuleDesc == NULL)
	{
		zeError("Module Manager", 
			"Can not find module. Please check your options.ini.");

		return NULL;
	}

	ZEModule* Module = ModuleDesc->CreateModuleInstance();
	if (Module == NULL)
	{
		zeError("Module Manager", 
			"Can not create module. (Module Name : \"%s\", Module Version : %d.%d)",
			ModuleDesc->GetName(), ModuleDesc->GetMajorVersion(), ModuleDesc->GetMinorVersion());

		return NULL;
	}

	return Module;
}

bool ZEModuleManager::LoadInternalModule(ZEModuleDescription* ModuleDesc)
{
	zeLog("Loading Internal Module \"%s\". Version : %d.%d, Type : UNKNOWN, Attributes : %c%c%c%c%c%c, Option Section : %s\r\n",
		ModuleDesc->GetName(),
		ModuleDesc->GetMajorVersion(),
		ModuleDesc->GetMinorVersion(),
		((ModuleDesc->GetAttributes() & ZE_MA_INTERNAL)		!= 0 ? 'I' : ' '),
		((ModuleDesc->GetAttributes() & ZE_MA_EXTERNAL)		!= 0 ? 'E' : ' '),
		((ModuleDesc->GetAttributes() & ZE_MA_SAFE)			!= 0 ? 'S' : ' '),
		((ModuleDesc->GetAttributes() & ZE_MA_DEBUG)		!= 0 ? 'D' : ' '),
		((ModuleDesc->GetAttributes() & ZE_MA_COMPATIBLE)	!= 0 ? 'C' : ' '),
		((ModuleDesc->GetAttributes() & ZE_MA_VALIDATED)	!= 0 ? 'V' : ' '),
		(ModuleDesc->GetOptions() == NULL ? "(NONE)" : ModuleDesc->GetOptions()->GetName())
		);

	if (!CheckModule(ModuleDesc))
	{
		zeError("Module Manager",
			"Can not load module. Module check failed. (Module Name : \"%s\", Module Version : %d.%d)",
			ModuleDesc->GetName(), ModuleDesc->GetMajorVersion(), ModuleDesc->GetMinorVersion());
		return false;
	}
	if (ModuleDesc->GetOptions() != NULL)
	{
		bool Result;
		Result = zeOptions->RegisterSection(ModuleDesc->GetOptions());
		if (!Result)
		{
			zeError("Module Manager", "Can not register module's option section.  (Module Name : \"%s\", Module Version : %d.%d)", ModuleDesc->GetName(), ModuleDesc->GetMajorVersion(), ModuleDesc->GetMinorVersion());
		}
	}

	ModuleList.Add(ModuleDesc);
	return true;
}

bool ZEModuleManager::LoadExternalModule(const char* FileName)
{
	return false;
}

void ZEModuleManager::SeekAndLoadExternalModules(const char* Directory)
{
}

void ZEModuleManager::UnloadModule(ZEModuleDescription* ModuleDesc)
{
	ModuleList.DeleteValue(ModuleDesc);
}

#include "Graphics/Direct3D9/Direct3D9ModuleDescription.h"
#include "Sound/DirectSoundModule.h"
#include "Input/DirectInput/DirectInputModuleDescription.h"
#include "Input/DummyInput/DummyInputModuleDescription.h"
#include "Input/VirtualInput/VirtualInputModuleDescription.h"

ZEModuleManager::ZEModuleManager()
{
	LoadInternalModule(new ZEDirect3D9ModuleDescription());
	LoadInternalModule(new ZEDirectSoundModuleDescription());
	LoadInternalModule(new ZEDummyInputModuleDescription());
	LoadInternalModule(new ZEVirtualInputModuleDescription());
	LoadInternalModule(new ZEDirectInputModuleDescription());
		
	ModuleManagerOptions.SetName("ModuleManager");
	ModuleManagerOptions.AddOption(new ZEOption("GraphicsModule", "Direct3D9", ZEOPTIONATTRIBUTE_NORMAL));
	ModuleManagerOptions.AddOption(new ZEOption("InputModule", "DirectInput", ZEOPTIONATTRIBUTE_NORMAL));
	ModuleManagerOptions.AddOption(new ZEOption("SoundModule", "DirectSound", ZEOPTIONATTRIBUTE_NORMAL));
	ModuleManagerOptions.AddOption(new ZEOption("NetworkModule", "WinNetwork", ZEOPTIONATTRIBUTE_NORMAL));
	ModuleManagerOptions.AddOption(new ZEOption("PhysicsModule", "Ageia", ZEOPTIONATTRIBUTE_NORMAL));
	ModuleManagerOptions.AddOption(new ZEOption("GameModule", "TestGame", ZEOPTIONATTRIBUTE_NORMAL));
	zeOptions->RegisterSection(&ModuleManagerOptions);
}
ZEModuleManager::~ZEModuleManager()
{
	for (size_t I = 0; I < ModuleList.GetCount(); I++)
		delete ModuleList[I];
}
