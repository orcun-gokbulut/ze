//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPathManager.cpp
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

#include "ZEPathManager.h"

#include "ZEError.h"
#include "ZEPathInfo.h"
#include "ZEDS\ZEFormat.h"
#include "ZEFileInfo.h"
#include "ZEPathTokenizer.h"

void ZEPathManager::SetEnginePath(const ZEString& Path)
{
	EnginePath = ZEPathInfo(Path).Normalize();
}

void ZEPathManager::SetResourcePath(const ZEString& Path)
{
	ResourcePath = ZEPathInfo(Path).Normalize();
}

void ZEPathManager::SetStoragePath(const ZEString& Path)
{
	StoragePath = ZEPathInfo(Path).Normalize();
}

void ZEPathManager::SetUserStoragePath(const ZEString& Path)
{
	UserStoragePath = ZEPathInfo(Path).Normalize();
}

void ZEPathManager::SetAccessControl(bool Enable)
{
	AccessControl = Enable;
}

bool ZEPathManager::GetAccessControl()
{
	return AccessControl;
}

const ZEString& ZEPathManager::GetEnginePath()
{
	return EnginePath;
}

const ZEString& ZEPathManager::GetResourcePath()
{
	return ResourcePath;
}

const ZEString& ZEPathManager::GetStoragePath()
{
	return StoragePath;
}

const ZEString& ZEPathManager::GetUserStoragePath()
{
	return UserStoragePath;
}

#ifdef WIN32
#include <windows.h>
#include <shlobj.h>

void ZEPathManager::Initialize()
{
	char ZinekPath[MAX_PATH];
	DWORD a = GetCurrentDirectory(MAX_PATH, ZinekPath);

	char AppData[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, AppData);

	char SystemAppData[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, SystemAppData);

	SetEnginePath(ZinekPath);
	SetResourcePath(ZEFormat::Format("{0}/Resources", ZinekPath));
	SetStoragePath(ZEFormat::Format("{0}/Storage", ZinekPath));
	SetUserStoragePath(ZEFormat::Format("{0}/ZinekEngine", AppData));
}

#endif

void ZEPathManager::Deinitialize()
{

}

ZERealPath ZEPathManager::TranslateToRealPath(const char* Path)
{
	ZERealPath RealPath;
	RealPath.Access = AccessControl ? ZE_PA_NO_ACCESS : ZE_PA_READ_WRITE;
	RealPath.Root = ZE_PR_UNKOWN;

	if (Path == NULL || Path[0] == '\0')
		return RealPath;

	ZEPathTokenizer Tokenizer;
	Tokenizer.Tokenize(Path);
	if (!ZEPathInfo::Normalize(Tokenizer))
		return RealPath;

	if (Tokenizer.GetTokenCount() == 0)
		return RealPath;

	const char* FirstToken = Tokenizer.GetToken(0);

	if (stricmp(FirstToken, "#Internal:") == 0 || stricmp(FirstToken, "#I:") == 0)
	{
		RealPath.Access = ZE_PA_READ;
		RealPath.Root = ZE_PR_INTERNAL;
		Tokenizer.SetToken(0, NULL);
		Tokenizer.Combine();
		RealPath.Path = Tokenizer.GetOutput();
	}
	else if (stricmp(FirstToken, "#Engine:") == 0 || 	stricmp(FirstToken, "#E:") == 0)
	{
		RealPath.Access = AccessControl ? ZE_PA_READ : ZE_PA_READ_WRITE;
		RealPath.Root = ZE_PR_ENGINE;
		Tokenizer.SetToken(0, EnginePath.ToCString());
		Tokenizer.Combine();
		RealPath.Path = Tokenizer.GetOutput();
	}
	else if (stricmp(FirstToken, "#Resource:") == 0 || 	stricmp(FirstToken, "#R:") == 0)
	{
		RealPath.Access = AccessControl ? ZE_PA_READ : ZE_PA_READ_WRITE;
		RealPath.Root = ZE_PR_RESOURCE;
		Tokenizer.SetToken(0, ResourcePath.ToCString());
		Tokenizer.Combine();
		RealPath.Path = Tokenizer.GetOutput();
	}
	else if (stricmp(FirstToken, "#Storage:") == 0 || stricmp(FirstToken, "#S:") == 0)
	{
		RealPath.Access = ZE_PA_READ_WRITE;
		RealPath.Root = ZE_PR_STORAGE;
		Tokenizer.SetToken(0, StoragePath.ToCString());
		Tokenizer.Combine();
		RealPath.Path = Tokenizer.GetOutput();
	}
	else if (stricmp(FirstToken, "#UserStorage:") == 0 || strcmp(FirstToken, "#US:") == 0)
	{
		RealPath.Access = ZE_PA_READ_WRITE;
		RealPath.Root = ZE_PR_USER_STORAGE;
		Tokenizer.SetToken(0, UserStoragePath.ToCString());
		Tokenizer.Combine();
		RealPath.Path = Tokenizer.GetOutput();
	}
	else
	{
		Tokenizer.Combine();
		if (strnicmp(Tokenizer.GetOutput(), ResourcePath.ToCString(), ResourcePath.GetLength()) == 0)
		{
			RealPath.Access = AccessControl ? ZE_PA_READ : ZE_PA_READ_WRITE;
			RealPath.Root = ZE_PR_RESOURCE;
			RealPath.Path = Tokenizer.GetOutput();
		}
		else if (strnicmp(Tokenizer.GetOutput(), EnginePath.ToCString(), EnginePath.GetLength()) == 0)
		{
			RealPath.Access = AccessControl ? ZE_PA_READ : ZE_PA_READ_WRITE;
			RealPath.Root = ZE_PR_ENGINE;
			RealPath.Path = Tokenizer.GetOutput();
		}
		else if (strnicmp(Tokenizer.GetOutput(), StoragePath.ToCString(), StoragePath.GetLength()) == 0)
		{
			RealPath.Access = ZE_PA_READ_WRITE;
			RealPath.Root = ZE_PR_STORAGE;
			RealPath.Path = Tokenizer.GetOutput();
		}
		else if (strnicmp(Tokenizer.GetOutput(), UserStoragePath.ToCString(), UserStoragePath.GetLength()) == 0)
		{
			RealPath.Access = ZE_PA_READ_WRITE;
			RealPath.Root = ZE_PR_USER_STORAGE;
			RealPath.Path = Tokenizer.GetOutput();
		}
		else
			RealPath.Path = Tokenizer.GetOutput();
	}

	return RealPath;
}

ZEPathManager::ZEPathManager()
{
	AccessControl = true;
}

ZEPathManager* ZEPathManager::GetInstance()
{
	static ZEPathManager* PathManager = NULL;
	if (PathManager == NULL)
	{
		PathManager = new ZEPathManager();
		PathManager->Initialize();
	}

	return PathManager;
}
