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

bool ZEPathManager::EnablePathRestriction = false;

ZEString ZEPathManager::WorkingDrive = "";
ZEString ZEPathManager::UserDataPath = "";
ZEString ZEPathManager::ResourcesPath = "";
ZEString ZEPathManager::SystemDataPath = "";
ZEString ZEPathManager::SavedGamesPath = "";
ZEString ZEPathManager::WorkingDirectory = "";
ZEString ZEPathManager::AppResourcesPath = "";


ZEPathManager::ZEPathManager()
{

}

ZEPathManager::~ZEPathManager()
{

}

void ZEPathManager::CustomizePaths(const ZEString* CompanyName, const ZEString* ApplicationName, const ZEString* ResourceDirectoryName)
{
    errno = 0;
    char* WorkDir = getcwd(NULL, 0);
    if (WorkDir == NULL)
    {
        ZEString ErrorString;
        ZEFileUtils::GetErrorString(ErrorString, (ZEInt)errno);
        zeError("Cannot get working directory.\nError: %s", ErrorString.ToCString());
        return;
    }

    const ZEString PathSeperator = ZEPathUtils::GetSeperator();

    WorkingDrive = PathSeperator;

    WorkingDirectory = WorkDir;

    ResourcesPath = WorkingDirectory;
    if (ResourceDirectoryName != NULL)
    {
        ResourcesPath += PathSeperator;
        ResourcesPath += *ResourceDirectoryName;
    }

	AppResourcesPath = ResourcesPath;
	if (ApplicationName != NULL)
    {
        AppResourcesPath += PathSeperator;
        AppResourcesPath += *ApplicationName;
    }

    SystemDataPath = WorkingDirectory;
    SystemDataPath += PathSeperator;
    SystemDataPath += "SystemData";
    if (ApplicationName != NULL)
    {
        SystemDataPath += PathSeperator;
        SystemDataPath += *ApplicationName;
    }

    UserDataPath = WorkingDirectory;
    UserDataPath += PathSeperator;
    UserDataPath += "UserData";
    if (ApplicationName != NULL)
    {
        UserDataPath += PathSeperator;
        UserDataPath += *ApplicationName;
    }

    SavedGamesPath = WorkingDirectory;
    SavedGamesPath += PathSeperator;
    SavedGamesPath += "SavedGames";
    if (ApplicationName != NULL)
    {
        SavedGamesPath += PathSeperator;
        SavedGamesPath += *ApplicationName;
    }
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
    return WorkingDrive;
}

const ZEString& ZEPathManager::GetWorkingDirectory()
{
	return WorkingDirectory;
}

void ZEPathManager::SetUserDataPath(const ZEString& Name)
{
    UserDataPath = Name;
}

const ZEString&	ZEPathManager::GetUserDataPath()
{
    return UserDataPath;
}

void ZEPathManager::SetResourcesPath(const ZEString& Name)
{
    ResourcesPath = Name;
}

const ZEString&	ZEPathManager::GetResourcesPath()
{
	return ResourcesPath;
}

void ZEPathManager::SetSystemDataPath(const ZEString& Name)
{
    SystemDataPath = Name;
}

const ZEString&	ZEPathManager::GetSystemDataPath()
{
	return SystemDataPath;
}

void ZEPathManager::SetSavedGamesPath(const ZEString& Name)
{
    SavedGamesPath = Name;
}

const ZEString&	ZEPathManager::GetSavedGamesPath()
{
	return SavedGamesPath;
}

void ZEPathManager::SetApplicationResourcesPath(const ZEString& Name)
{
    AppResourcesPath = Name;
}

const ZEString&	ZEPathManager::GetApplicationResourcesPath()
{
	return AppResourcesPath;
}

ZEKnownPath ZEPathManager::GetKnownPath(const ZEString& AbsolutePath)
{
#define Compare(Str1, Str2, Length)  (strncmp((Str1), (Str2), (Length)) == 0)

	ZESize Length = 0;
	ZEKnownPath Root = ZE_KP_NONE;
	ZESize PathLen = strlen(AbsolutePath.ToCString());
	ZESize ResrcLen = strlen(ResourcesPath.ToCString());
	ZESize AppResLen = strlen(AppResourcesPath.ToCString());
	ZESize WrkDirLen = strlen(WorkingDirectory.ToCString());
	ZESize SysDataLen = strlen(SystemDataPath.ToCString());
	ZESize UsrDataLen = strlen(UserDataPath.ToCString());
	ZESize SvdGameLen = strlen(SavedGamesPath.ToCString());

	if (PathLen >= ResrcLen && ResrcLen > Length &&
		strncmp(AbsolutePath, ResourcesPath, ResrcLen) == 0)
	{
		Length = ResrcLen;
		Root = ZE_KP_RESOURCES;
	}
	if (PathLen >= AppResLen && AppResLen > Length &&
		strncmp(AbsolutePath, AppResourcesPath, AppResLen) == 0)
	{
		Length = ResrcLen;
		Root = ZE_KP_APP_RESOURCES;
	}
	if (PathLen >= WrkDirLen && WrkDirLen > Length &&
		strncmp(AbsolutePath, WorkingDirectory, WrkDirLen) == 0)
	{
		Length = WrkDirLen;
		Root = ZE_KP_WORKING_DIRECTORY;
	}
	if (PathLen >= SysDataLen && SysDataLen > Length &&
		strncmp(AbsolutePath, SystemDataPath, SysDataLen) == 0)
	{
		Length = SysDataLen;
		Root = ZE_KP_SYSTEM_DATA;
	}
	if (PathLen >= UsrDataLen && UsrDataLen > Length &&
		strncmp(AbsolutePath, UserDataPath, UsrDataLen) == 0)
	{
		Length = UsrDataLen;
		Root = ZE_KP_USER_DATA;
	}
	if (PathLen >= SvdGameLen && SvdGameLen > Length &&
		strncmp(AbsolutePath, SavedGamesPath, SvdGameLen) == 0)
	{
		Length = SvdGameLen;
		Root = ZE_KP_SAVED_GAMES;
	}

	return Root;
}

const ZEString& ZEPathManager::GetKnownPath(const ZEKnownPath KnownPath)
{
	switch (KnownPath)
	{
		case ZE_KP_NONE:
			if (!EnablePathRestriction)
				return ZEPathUtils::GetDot();
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
	ZEString RootPath;
    ZEString FinalPath;
    ZEKnownPath RootSymbol;

    ZEString Drive = ZEPathUtils::GetSeperator();

	if (ZEPathUtils::IsAbsolutePath(Path))
	{
	    if (EnablePathRestriction)
	    {
	        // We have a drive, do not stack dots while simplification
	        FinalPath = Drive + ZEPathUtils::GetSimplifiedPath(Path, false);

			// Try to get root
	        RootSymbol = GetKnownPath(FinalPath);

			// Get root symbol path
	        RootPath = GetKnownPath(RootSymbol);

			// Check for restriction
	        if (!ZEPathUtils::CheckForRestriction(RootPath, FinalPath))
            {
                zeError("Final path is restricted!\nPath: \"%s\".\nRoot: \"%s\".", FinalPath.ToCString(), RootPath.ToCString());
                return "";
            }
	    }
	    else // No restriction
	    {
	       // We have a drive, do not stack dots while simplification
	       FinalPath = Drive + ZEPathUtils::GetSimplifiedPath(Path, false);

           RootSymbol = ZE_KP_NONE;
	    }
	}
	else
	{
        ZEString RelativePartOfPath;

	    if (EnablePathRestriction)
	    {
	        // Construct absolute path
	        RootSymbol = ZEPathUtils::SearchForSymbol(&RelativePartOfPath, Path);

	        // Get root path
            RootPath = GetKnownPath(RootSymbol);

            // Construct temp path
            FinalPath += RootPath;
            FinalPath += ZEPathUtils::GetSeperator();
            FinalPath += RelativePartOfPath;

            // Simplify
            FinalPath = Drive + ZEPathUtils::GetSimplifiedPath(FinalPath, false);

            // Root can change after simplification, check again
            RootSymbol = GetKnownPath(FinalPath);
            RootPath = GetKnownPath(RootSymbol);

            if (!ZEPathUtils::CheckForRestriction(RootPath, FinalPath))
            {
                zeError("Final path is restricted!\nPath: \"%s\".\nRoot: \"%s\".", FinalPath.ToCString(), RootPath.ToCString());
                return "";
            }
	    }
	    else
	    {
	        ZEString TempPath;
	        bool ConvertedToAbsolute = false;

            // Try to construct absolute path
            RootSymbol = ZEPathUtils::SearchForSymbol(&RelativePartOfPath, Path);
            if (RootSymbol != ZE_KP_NONE)
                ConvertedToAbsolute = true;

	        // Get root path
            RootPath = GetKnownPath(RootSymbol);

            TempPath += RootPath;
            TempPath += ZEPathUtils::GetSeperator();
            TempPath += RelativePartOfPath;

            // Simplify
            if (ConvertedToAbsolute)
                FinalPath = Drive;

            FinalPath += ZEPathUtils::GetSimplifiedPath(TempPath, true);

            if (FinalPath.IsEmpty())
				FinalPath = ZEPathUtils::GetDot();

            // No restriction so there is no need for root
            RootSymbol = ZE_KP_NONE;
	    }
	}

	if (Root != NULL)
		*Root = RootSymbol;

	return FinalPath;
}
