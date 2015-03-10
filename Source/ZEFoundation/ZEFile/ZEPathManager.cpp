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

void ZEPathManager::SetEnginePath(const char* Path)
{
	EnginePath = ZEPathInfo::Divide(Path);
	ZEPathInfo::Normalize(EnginePath);
}

void ZEPathManager::SetResourcePath(const char* Path)
{
	ResourcePath = ZEPathInfo::Divide(Path);
	ZEPathInfo::Normalize(ResourcePath);
}

void ZEPathManager::SetStoragePath(const char* Path)
{
	StoragePath = ZEPathInfo::Divide(Path);
	ZEPathInfo::Normalize(StoragePath);
}

void ZEPathManager::SetUserStoragePath(const char* Path)
{
	UserStoragePath = ZEPathInfo::Divide(Path);
	ZEPathInfo::Normalize(UserStoragePath);
}

void ZEPathManager::SetAccessControl(bool Enable)
{
	AccessControl = Enable;
}

bool ZEPathManager::GetAccessControl()
{
	return AccessControl;
}

ZEString ZEPathManager::GetEnginePath()
{
	return ZEPathInfo::Construct(EnginePath);
}

ZEString ZEPathManager::GetResourcePath()
{
	return ZEPathInfo::Construct(ResourcePath);
}

ZEString ZEPathManager::GetStoragePath()
{
	return ZEPathInfo::Construct(StoragePath);
}

ZEString ZEPathManager::GetUserStoragePath()
{
	return ZEPathInfo::Construct(UserStoragePath);
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
	RealPath.Path = Path;

	if (Path == NULL || Path[0] == '\0')
		return RealPath;

	ZEArray<ZEString> PathElements = ZEPathInfo::Divide(Path);
	ZEPathInfo::Normalize(PathElements);
	
	if (PathElements.GetSize() == 0)
		return RealPath;

	if (PathElements[0] == "#Internal:" || PathElements[0] == "#I:")
	{
		RealPath.Access = ZE_PA_READ;
		RealPath.Root = ZE_PR_INTERNAL;
		PathElements.Remove(0);
		RealPath.Path = ZEPathInfo::Construct(PathElements);
	}
	else if (PathElements[0] == "#Resource:" || PathElements[0] == "#R:")
	{
		RealPath.Access = AccessControl ? ZE_PA_READ : ZE_PA_READ_WRITE;
		RealPath.Root = ZE_PR_RESOURCE;
		PathElements.Remove(0);
		PathElements.MassInsert(0, ResourcePath.GetCArray(), ResourcePath.GetSize());
		RealPath.Path = ZEPathInfo::Construct(PathElements);
	}
	else if (PathElements[0] == "#Storage:" || PathElements[0] == "#S:")
	{
		RealPath.Access = ZE_PA_READ_WRITE;
		RealPath.Root = ZE_PR_STORAGE;
		PathElements.Remove(0);
		PathElements.MassInsert(0, StoragePath.GetCArray(), StoragePath.GetSize());
		RealPath.Path = ZEPathInfo::Construct(PathElements);
	}
	else if (PathElements[0] == "#UserStorage:" || PathElements[0] == "#US:")
	{
		RealPath.Access = ZE_PA_READ_WRITE;
		RealPath.Root = ZE_PR_USER_STORAGE;
		PathElements.Remove(0);
		PathElements.MassInsert(0, UserStoragePath.GetCArray(), UserStoragePath.GetSize());
		RealPath.Path = ZEPathInfo::Construct(PathElements);
	}
	else if (ZEPathInfo::CheckParent(ResourcePath, PathElements))
	{
		RealPath.Access = AccessControl ? ZE_PA_READ : ZE_PA_READ_WRITE;
		RealPath.Root = ZE_PR_RESOURCE;
		RealPath.Path = ZEPathInfo::Construct(PathElements);
	}
	else if (ZEPathInfo::CheckParent(StoragePath, PathElements))
	{
		RealPath.Access = ZE_PA_READ_WRITE;
		RealPath.Root = ZE_PR_STORAGE;
		RealPath.Path = ZEPathInfo::Construct(PathElements);
	}
	else if (ZEPathInfo::CheckParent(UserStoragePath, PathElements))
	{
		RealPath.Access = ZE_PA_READ_WRITE;
		RealPath.Root = ZE_PR_USER_STORAGE;
		RealPath.Path = ZEPathInfo::Construct(PathElements);
	}

	return RealPath;
}

ZEPathRoot ZEPathManager::GetRoot(const char* Path)
{
	if (Path == NULL || Path[0] == '\0')
		return ZE_PR_UNKOWN;

	ZEArray<ZEString> PathElements = ZEPathInfo::Divide(Path);
	ZEPathInfo::Normalize(PathElements);
	if (PathElements.GetSize() == 0)
		return ZEPathRoot();

	if (PathElements[0] == "#Internal:" || PathElements[0] == "#I:")
		return ZE_PR_INTERNAL;
	else if (PathElements[0] == "#Resource:" || PathElements[0] == "#R:")
		return ZE_PR_RESOURCE;
	else if (PathElements[0] == "#Storage:" || PathElements[0] == "#S:")
		return ZE_PR_STORAGE;
	else if (PathElements[0] == "#UserStorage:" || PathElements[0] == "#US:")
		return ZE_PR_USER_STORAGE;
	else if (ZEPathInfo::CheckParent(PathElements, ResourcePath))
		return ZE_PR_RESOURCE;
	else if (ZEPathInfo::CheckParent(PathElements, StoragePath))
		return ZE_PR_STORAGE;
	else if (ZEPathInfo::CheckParent(PathElements, UserStoragePath))
		return ZE_PR_USER_STORAGE;

	return ZE_PR_UNKOWN;
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
