//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPathManager_Unix.cpp
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
#include "ZEFileUtils.h"
#include "ZEPathManager.h"
#include "ZEPathUtils.h"

#include "errno.h"
#include <string.h>
#include <memory.h>

bool ZEPathManager::Initialized = false;
bool ZEPathManager::EnablePathRestriction = false;

ZEString ZEPathManager::WorkingDrive = "";

ZEString ZEPathManager::UserDataPath = "";
ZEString ZEPathManager::ResourcesPath = "";
ZEString ZEPathManager::SystemDataPath = "";
ZEString ZEPathManager::SavedGamesPath = "";
ZEString ZEPathManager::WorkingDirectory = "";
ZEString ZEPathManager::AppResourcesPath = "";

ZEString ZEPathManager::CompanyName = "Zinek";
ZEString ZEPathManager::ApplicationName = "Engine";
ZEString ZEPathManager::ResourceDirName = "Resources";


ZEPathManager::ZEPathManager()
{

}

ZEPathManager::~ZEPathManager()
{

}

void ZEPathManager::InitializePaths()
{
    if (Initialized)
        return;

    const ZEString PathSeperator = ZEPathUtils::GetSeperator();

    errno = 0;
    char* WorkDir = getcwd(NULL, 0);
    if (WorkDir == NULL)
    {
        ZEString ErrorString;
        ZEFileUtils::GetErrorString(ErrorString, (ZEInt)errno);
        zeError("Cannot get working directory.\nError: %s", ErrorString.ToCString());
        return;
    }

    WorkingDrive = PathSeperator;

    WorkingDirectory = WorkDir;

    ResourcesPath = WorkingDirectory;
	ResourcesPath += PathSeperator;
	ResourcesPath += ResourceDirName;

	AppResourcesPath = ResourcesPath;
	AppResourcesPath += PathSeperator;
	AppResourcesPath += ApplicationName;

    SystemDataPath = WorkingDirectory;
    SystemDataPath += PathSeperator;
    SystemDataPath += "SystemData";
    SystemDataPath += PathSeperator;
    SystemDataPath += ApplicationName;


    UserDataPath = WorkingDirectory;
    UserDataPath += PathSeperator;
    UserDataPath += "UserData";
    UserDataPath += PathSeperator;
    UserDataPath += ApplicationName;

    SavedGamesPath = WorkingDirectory;
    SavedGamesPath += PathSeperator;
    SavedGamesPath += "SavedGames";
    SavedGamesPath += PathSeperator;
    SavedGamesPath += ApplicationName;

    Initialized = true;
}

bool ZEPathManager::GetInitialized()
{
	return Initialized;
}

void ZEPathManager::SetCompanyName(const ZEString& Name)
{
	CompanyName = Name;
	Initialized = false;

	InitializePaths();
}

const ZEString& ZEPathManager::GetCompanyName()
{
	return CompanyName;
}

void ZEPathManager::SetApplicationName(const ZEString& Name)
{
	ApplicationName = Name;
	Initialized = false;

	InitializePaths();
}

const ZEString& ZEPathManager::GetApplicationName()
{
	return ApplicationName;
}

void ZEPathManager::SetResourceDirName(const ZEString& Name)
{
	ResourceDirName;
	Initialized = false;

	InitializePaths();
}

const ZEString& ZEPathManager::GetResourceDirName()
{
	return ResourceDirName;
}

void ZEPathManager::SetEnablePathRestriction(bool Enable)
{
	EnablePathRestriction = Enable;
}

bool ZEPathManager::GetEnablePathRestriction()
{
	return EnablePathRestriction;
}

const ZEString& ZEPathManager::GetWorkingDrive()
{
	InitializePaths();

	return WorkingDrive;
}

const ZEString&	ZEPathManager::GetUserDataPath()
{
	InitializePaths();

	return UserDataPath;
}

const ZEString&	ZEPathManager::GetResourcesPath()
{
	InitializePaths();

	return ResourcesPath;
}

const ZEString&	ZEPathManager::GetSystemDataPath()
{
	InitializePaths();

	return SystemDataPath;
}

const ZEString&	ZEPathManager::GetSavedGamesPath()
{
	InitializePaths();

	return SavedGamesPath;
}

const ZEString& ZEPathManager::GetWorkingDirectory()
{
	InitializePaths();

	return WorkingDirectory;
}

const ZEString&	ZEPathManager::GetApplicationResourcesPath()
{
	InitializePaths();

	return AppResourcesPath;
}

ZEKnownPath ZEPathManager::GetKnownPath(const ZEString& AbsolutePath)
{
#define Compare(Str1, Str2, Length)  (strncmp((Str1), (Str2), (Length)) == 0)

	InitializePaths();

	ZESize Len = 0;
	ZEKnownPath Root = ZE_KP_NONE;
	ZESize PathLen = AbsolutePath.GetLength();
	ZESize ResrcLen = ResourcesPath.GetLength();
	ZESize AppResLen = AppResourcesPath.GetLength();
	ZESize WrkDirLen = WorkingDirectory.GetLength();
	ZESize SysDataLen = SystemDataPath.GetLength();
	ZESize UsrDataLen = UserDataPath.GetLength();
	ZESize SvdGameLen = SavedGamesPath.GetLength();

	// Comparison should be done with the order
	// staring from longest to shortest path

	if (PathLen >= ResrcLen && ResrcLen > Len &&
		Compare(AbsolutePath, ResourcesPath, ResrcLen))
	{
		Len = ResrcLen;
		Root = ZE_KP_RESOURCES;
	}
	if (PathLen >= AppResLen && AppResLen > Len &&
		Compare(AbsolutePath, AppResourcesPath, AppResLen))
	{
		Len = ResrcLen;
		Root = ZE_KP_APP_RESOURCES;
	}
	if (PathLen >= WrkDirLen && WrkDirLen > Len &&
		Compare(AbsolutePath, WorkingDirectory, WrkDirLen))
	{
		Len = WrkDirLen;
		Root = ZE_KP_WORKING_DIRECTORY;
	}
	if (PathLen >= SysDataLen && SysDataLen > Len &&
		Compare(AbsolutePath, SystemDataPath, SysDataLen))
	{
		Len = SysDataLen;
		Root = ZE_KP_SYSTEM_DATA;
	}
	if (PathLen >= UsrDataLen && UsrDataLen > Len &&
		Compare(AbsolutePath, UserDataPath, UsrDataLen))
	{
		Len = UsrDataLen;
		Root = ZE_KP_USER_DATA;
	}
	if (PathLen >= SvdGameLen && SvdGameLen > Len &&
		Compare(AbsolutePath, SavedGamesPath, SvdGameLen))
	{
		Len = SvdGameLen;
		Root = ZE_KP_SAVED_GAMES;
	}

	return Root;
}

const ZEString& ZEPathManager::GetKnownPath(ZEKnownPath& RedirectedKnownPath, const ZEKnownPath KnownPath)
{
	InitializePaths();

	RedirectedKnownPath = KnownPath;

	switch (KnownPath)
	{
		case ZE_KP_NONE:
			if (!EnablePathRestriction)
				return ZEPathUtils::GetDot();
			RedirectedKnownPath = ZE_KP_WORKING_DIRECTORY;
			// Fall Through!!!
		case ZE_KP_WORKING_DIRECTORY:
			return WorkingDirectory;
			break;
		case ZE_KP_RESOURCES:
			return ResourcesPath;
			break;
		case ZE_KP_APP_RESOURCES:
			return AppResourcesPath;
			break;
		case ZE_KP_USER_DATA:
			return UserDataPath;
			break;
		case ZE_KP_SYSTEM_DATA:
			return SystemDataPath;
			break;
		case ZE_KP_SAVED_GAMES:
			return SavedGamesPath;
			break;
		default:
			return ZEPathUtils::GetEmptyPath();
			break;
	}
}

// Checks format, constructs final path and makes a boundary check
ZEString ZEPathManager::GetFinalPath(const ZEString& Path, ZEKnownPath* Root)
{
	ZEKnownPath RootSymbol;
	ZEString FinalPath;
	ZEString TempPath;
	ZEString RelativePartOfPath;
	ZEString RootPath;

	if (ZEPathUtils::IsAbsolutePath(Path))
	{
	    if (EnablePathRestriction)
	    {
	        FinalPath = ZEPathUtils::GetSimplifiedPath(Path);
	        RootSymbol = GetKnownPath(FinalPath);
	        RootPath = GetKnownPath(RootSymbol, RootSymbol);

	        if (!ZEPathUtils::CheckForRestriction(RootPath, FinalPath))
            {
                zeError("Final path is restricted!\nPath: \"%s\".\nRoot: \"%s\".", FinalPath, RootPath);
                return "";
            }
	    }
	    else // No restriction
	    {
	        FinalPath = ZEPathUtils::GetSimplifiedPath(Path);
            RootSymbol = ZE_KP_NONE;
	    }
	}
	else
	{
	    if (EnablePathRestriction)
	    {
	        // Construct absolute path
	        RootSymbol = ZEPathUtils::SearchForSymbol(&RelativePartOfPath, Path);

	        // Get root path
            RootPath = GetKnownPath(RootSymbol, RootSymbol);

            // Construct temp path
            TempPath = RootPath;
            TempPath += ZEPathUtils::GetSeperator();
            TempPath += RelativePartOfPath;

            // Simplify
            FinalPath = ZEPathUtils::GetSimplifiedPath(TempPath);

            // Root can change after simplification, check again
            RootSymbol = GetKnownPath(FinalPath);
            RootPath = GetKnownPath(RootSymbol, RootSymbol);

            if (!ZEPathUtils::CheckForRestriction(RootPath, FinalPath))
            {
                zeError("Final path is restricted!\nPath: \"%s\".\nRoot: \"%s\".", FinalPath, RootPath);
                return "";
            }
	    }
	    else
	    {
            // Try to contruct absolute path
            RootSymbol = ZEPathUtils::SearchForSymbol(&RelativePartOfPath, Path);

	        // Get root path
            RootPath = GetKnownPath(RootSymbol, RootSymbol);

            // Construct temp path
            TempPath = RootPath;
            TempPath += ZEPathUtils::GetSeperator();
            TempPath += RelativePartOfPath;

            // Simplify
            FinalPath = ZEPathUtils::GetSimplifiedPath(TempPath);

            // No restriction so there is no need for root
            RootSymbol = ZE_KP_NONE;
	    }
	}

	if (Root != NULL)
		*Root = RootSymbol;

	return FinalPath;
}
