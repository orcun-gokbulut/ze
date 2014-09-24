//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPluginManager.cpp
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

#include "ZECore.h"
#include "ZETypes.h"
#include "ZEError.h"
#include "ZEPlugin.h"
#include "ZEConsole.h"
#include "ZEModuleManager.h"
#include "ZEPluginManager.h"
#include "ZEExtensionManager.h"

#include "ZEDS/ZEString.h"
#include "ZEDS/ZEFormat.h"

#include <string.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

ZEPlugin* ZEPluginManager::GetPlugin(const ZEString& Name)
{
	for (ZESize I = 0; I < Plugins.GetCount(); I++)
		if (Plugins[I]->GetName() == Name)
			return Plugins[I];

	return NULL;
}

const ZEArray<ZEPlugin*>& ZEPluginManager::GetPlugins()
{
	return Plugins;
}

bool ZEPluginManager::RegisterPlugin(ZEPlugin* Plugin)
{
	if (Plugin == NULL)
		return false;

	zeLog("Registering plugin. Plugin Name : \"%s\".", 
		Plugin->GetName().ToCString());

	if (Plugins.Exists(Plugin))
	{
		zeError("Pluging already registered. Plugin Name : \"%s\".", 
			Plugin->GetName().ToCString());
		return false;
	}

	/*const ZEArray<ZEExtensionDescription*>& Extensions = Plugin->GetExtensionDescriptions();

	zeLog("Registering plugin's extensions. Plugin Name : \"%s\", Total Extensions : %Iu.", 
		Plugin->GetName().ToCString(), Extensions.GetCount());
	
	for (ZESize I = 0; I < Extensions.GetCount(); I++ )
		ZEExtensionManager::GetInstance()->RegisterExtension(Extensions[I]);

	zeLog("Plugin registered. Plugin Name : \"%s\".", (const char*)Plugin->GetName());*/

	return true;
}

void ZEPluginManager::UnregisterPlugin(ZEPlugin* Plugin)
{
	Plugins.RemoveValue(Plugin);
	/*const ZEArray<ZEExtensionDescription*>& Extensions = Plugin->GetExtensionDescriptions();
	for (ZESize I = 0; I < Extensions.GetCount(); I++ )
		ZEExtensionManager::GetInstance()->UnregisterExtension(Extensions[I]);
	*/
}

typedef ZEPlugin* (*ZEGetPlugingFunction)();

static void LoadExternalPlugin_Load(const ZEString& Path)
{
	SetDllDirectoryW(Path.ToWCString());

	HMODULE Module = LoadLibraryW(Path.ToWCString());
	if (Module == NULL)
		return;
	
	ZEGetPlugingFunction Function = (ZEGetPlugingFunction)GetProcAddress(Module, "GetPlugin");
	ZEPlugin* Plugin = Function();
	if (Plugin == NULL)
	{
		FreeLibrary(Module);
		SetDllDirectoryW(NULL);
		return;
	}

	SetDllDirectoryW(NULL);
}

void ZEPluginManager::LoadExternalPlugins(ZEString& Path, bool Recursive)
{
	WIN32_FIND_DATAW FindData;

	HANDLE Handle = FindFirstFileW(ZEFormat::Format("{0}\\*.zePlugin", Path).ToWCString(), &FindData);

	if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		LoadExternalPlugin_Load(ZEFormat::Format("{0}\\{1}", Path, ZEString(FindData.cFileName)));

	while(FindNextFileW(Handle, &FindData))
	{
		if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			LoadExternalPlugin_Load(ZEFormat::Format("{0}\\{1}", Path, ZEString(FindData.cFileName)));
	}

	if (!Recursive)
		return;

	Handle = FindFirstFileW(ZEFormat::Format("{0}\\*", Path).ToWCString(), &FindData);

	if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
		LoadExternalPlugins(ZEFormat::Format("{0}\\{1}", Path, ZEString(FindData.cFileName)));

	while(FindNextFileW(Handle, &FindData))
	{
		if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			LoadExternalPlugins(ZEFormat::Format("{0}\\{1}", Path, ZEString(FindData.cFileName)));
	}
}

ZEPluginManager::ZEPluginManager()
{

}

ZEPluginManager::~ZEPluginManager()
{

}

ZEPluginManager* ZEPluginManager::GetInstance()
{
	return ZECore::GetInstance()->GetPluginManager();
}
