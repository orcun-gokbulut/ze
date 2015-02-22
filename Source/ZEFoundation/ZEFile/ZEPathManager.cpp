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

#include "ZEError.h"
#include "ZEPathManager.h"
#include "ZEDS\ZEString.h"
#include "ZEDS\ZEArray.h"
#include "ZEDS\ZEFormat.h"

static char* DividePath(char* Output, ZESize OutputSize, const char* Path)
{
	char* OutputStart = Output;
	bool Start = false;

	while(*Path != '\0')
	{
		if (OutputSize == 0)	
			return NULL;

		if (*Path == '/' || *Path == '\\')
		{
			Path++;
			break;
		}
		else if (*Path == ' ' && Start)
		{
			Path++;
			continue;	
		}
		else
		{
			*Output = *Path;
			Start = false;
		}

		OutputSize--;
		Output++;
		Path++;
	}

	*Output = '\0';
	ZESSize LastSpaceStart = -1;
	ZESize Index = 0;
	while (*Output != '\0')
	{
		if (*Output == ' ' && LastSpaceStart == -1)
			LastSpaceStart = Index;
		else
			LastSpaceStart = -1;

		Index++;
	}

	if (LastSpaceStart != -1)
		OutputStart[LastSpaceStart] = '\0';

	return OutputStart;
}

static bool GeneratePathArray(ZEArray<ZEString> PathArray, const char* Path)
{
	while(true)
	{
		char PathPart[256];
		char* Path = DividePath(PathPart, 256, Path);

		if (Path == NULL)
			return false;

		if (*Path == '\0')
			break;

		PathArray.Add(PathPart);
	}

}

static bool NormalizePath(ZEArray<ZEString> Path)
{
	for (size_t I = 0; I < Path.GetCount(); I++)
	{
		if (Path[I] == ".")
		{
			Path.Remove(I);
			I--;
		}
		else if (Path[I] == '..')
		{
			if (I = 0)
				return false;
		}
	}
}

void ZEPathManager::SetAccessControl(bool Enable)
{
	AccessControl = Enable;
}

bool ZEPathManager::GetAccessControl()
{
	return AccessControl;
}

#ifdef WIN32
#include <windows.h>
#include <shlobj.h>

void ZEPathManager::Initialize()
{
	HMODULE ModuleHandle = GetModuleHandle(NULL);
	char ZinekExecutivePath[256];
	GetModuleFileName(ModuleHandle, ZinekExecutivePath, 256);

	char ZinekPath[256];
	GetFullPathName(ZinekExecutivePath, 256, ZinekPath, NULL);


	char AppData[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, AppData);

	char SystemAppData[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, SystemAppData);

	EnginePath = ZinekPath;
	ResourcePath = ZEFormat::Format("{0}/Resources", EnginePath);
	StoragePath = ZEFormat::Format("{0}/ZinekEngine", SystemAppData);
	UserStoragePath = ZEFormat::Format("{0}/ZinekEngine", AppData);
}

#endif

void ZEPathManager::Deinitialize()
{

}

ZERealPath ZEPathManager::GetRealPath(const char* Path)
{
	ZEArray<ZEString> PathArray;
	if (!GeneratePathArray(PathArray, Path))
		return ZERealPath();
}

ZERealPath ZEPathManager::GetRoot(ZEPathRoot Root)
{
	ZERealPath RealPath;
	RealPath.Root = Root;
	switch (Root)
	{
		default:
		case ZE_PR_NONE:
			RealPath.Access = ZE_PA_NO_ACCESS;
			RealPath.Path = "";
			break;

		case ZE_PR_RELATIVE:
			RealPath.Access = ZE_PA_UNKNOWN;
			RealPath.Path = "";
			break;

		case ZE_PR_RESOURCE:
			RealPath.Access = ZE_PA_READ;
			RealPath.Path = "";
			break;

		case ZE_PR_STORAGE:
			RealPath.Access = ZE_PA_READ_WRITE;
			RealPath.Path = "";
			break;

		case ZE_PR_USER_STORAGE:
			RealPath.Access = ZE_PA_READ_WRITE;
			RealPath.Path = "";
			break;

		case ZE_PR_INTERNAL:
			RealPath.Access = ZE_PA_READ;
			RealPath.Path = "";
			break;
	}

	return RealPath;
}

ZEPathRoot ZEPathManager::GetRoot(ZEString RootPath)
{
	ZERealPath RealPath;
	if (RootPath.Left(0) == '\\' || RootPath.Right(0) == '/')
	{
		return ZE_PR_NONE;
	}
	else if (RootPath.Right(0) == ":")
	{
		ZEString RootName = RootPath.Left(RootPath.GetLength() - 1);
		if (RootName == "Internal" || RootName == "I")
			return ZE_PR_INTERNAL;
		else if (RootName == "#Resource" || RootName == "#R")
			return ZE_PR_RESOURCE;
		else if (RootName == "#Storage" || RootName == "#S")
			return ZE_PR_STORAGE;
		else if (RootName == "#UserStorage" || RootName == "#US")
			return ZE_PR_USER_STORAGE;
		else
			return ZE_PR_NONE;
	}
	else
	{
		return ZE_PR_RELATIVE;
	}
}

ZEPathManager::ZEPathManager()
{
	AccessControl = true;
}

ZEPathManager* ZEPathManager::GetInstance()
{
	static ZEPathManager* PathManager = NULL;
	if (PathManager != NULL)
	{
		PathManager = new ZEPathManager();
		PathManager->Initialize();
	}

	return PathManager;
}
