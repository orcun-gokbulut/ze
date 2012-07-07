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

#include "errno.h"
#include <string.h>
#include <memory.h>

ZEString ZEPathManager::AppName = "";
ZEString ZEPathManager::UserDataPath = "";
ZEString ZEPathManager::ResourcesPath = "";
ZEString ZEPathManager::SystemDataPath = "";
ZEString ZEPathManager::SavedGamesPath = "";
ZEString ZEPathManager::ApplicationResourcesPath;

bool ZEPathManager::Initialized = false;
bool ZEPathManager::EnablePathRestriction = true;


static const ZEString PathSeperator = "/";

// Helper functions to get ZEFileKnownPaths from string
static ZEFileKnownPaths GetRootByString(const ZEString& Path)
{
    ZEFileKnownPaths RootSymbol;

    switch (Path[0])
    {
        case ZE_FILE_PATH_IDENTIFIER_SYMBOL_USER_DATA:
            RootSymbol = ZE_FKP_USER_DATA;
            break;
        case ZE_FILE_PATH_IDENTIFIER_SYMBOL_SYSTEM_DATA:
            RootSymbol = ZE_FKP_SYSTEM_DATA;
            break;
        case ZE_FILE_PATH_IDENTIFIER_SYMBOL_SAVED_GAMES:
            RootSymbol = ZE_FKP_SAVED_GAMES;
            break;
        case ZE_FILE_PATH_IDENTIFIER_SYMBOL_APP_RESOURCES:
            RootSymbol = ZE_FKP_APP_RESOURCES;
            break;
        case ZE_FILE_PATH_IDENTIFIER_SYMBOL_RESOURCES:
            RootSymbol = ZE_FKP_RESOURCES;
            break;
        default:
            RootSymbol = ZE_FKP_NONE;
            break;
    }

    return RootSymbol;
}

void ZEPathManager::InitializePaths()
{
    if (Initialized)
        return;

    errno = 0;
    char* WorkDir = getcwd(NULL, 0);
    if (WorkDir == NULL)
    {
        ZEString ErrorString;
        ZEFileUtils::GetErrorString(ErrorString, (ZEInt)errno);
        zeError("Cannot get working directory.\nError: %s", ErrorString.ToCString());
        return;
    }

    // Paths should be rearranged
    ResourcesPath = WorkDir;
    ResourcesPath += "/Resources";

    UserDataPath = WorkDir;
    UserDataPath += "/UserData";

    SystemDataPath = WorkDir;
    SystemDataPath += "/SystemData";

    SavedGamesPath = WorkDir;
    SavedGamesPath += "/SavedGames";

    ApplicationResourcesPath = WorkDir;
    ApplicationResourcesPath += "/Resources/" + AppName;

    Initialized = true;
}

bool ZEPathManager::GetInitialized()
{
    return Initialized;
}

void ZEPathManager::SetEnablePathRestriction(bool Enable)
{
    EnablePathRestriction = Enable;
}

bool ZEPathManager::GetEnablePathRestriction()
{
    return EnablePathRestriction;
}

void ZEPathManager::SetApplicationName(const ZEString &Name)
{
    InitializePaths();

    AppName = Name;
}

const ZEString& ZEFileUtils::GetPathSeperator()
{
	return PathSeperator;
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

const ZEString&	ZEPathManager::GetApplicationResourcesPath()
{
    InitializePaths();

    return ApplicationResourcesPath;
}

ZEString ZEPathManager::GetKnownPath(ZEFileKnownPaths KnownPath)
{
    InitializePaths();

    switch (KnownPath)
    {
        case ZE_FKP_NONE:
            if (!EnablePathRestriction)
            {
                return ".";
            }
            // Fall through !
        case ZE_FKP_RESOURCES:
            return ResourcesPath;
            break;
        case ZE_FKP_APP_RESOURCES:
            return ApplicationResourcesPath;
            break;
        case ZE_FKP_USER_DATA:
            return UserDataPath;
            break;
        case ZE_FKP_SYSTEM_DATA:
            return SystemDataPath;
            break;
        case ZE_FKP_SAVED_GAMES:
            return SavedGamesPath;
            break;
        default:
            return "";
            break;
    }

    return "";
}

bool ZEPathManager::PathBoundaryCheck(const ZEString& RootPath, const ZEString& Path)
{
    char* Token;
    char* NextToken;
    ZEInt Depth;
    ZEString Temp;
    const char* Search = "/";

    if (!EnablePathRestriction)
        return true;

    //Check if path is an extension of root
    if (strncmp(RootPath.ToCString(), Path.ToCString(), RootPath.GetLength()) != 0)
        return false;

    Depth = 0;
    // Exclude root path from Path
    Temp = Path.Right(Path.GetLength() - RootPath.GetLength());
    if ( !Temp.IsEmpty() )
    {
        // Check the depth. Path should not go above the root
        Token = NULL;
        NextToken = NULL;
        Token = strtok_r((char*)Temp.ToCString(), Search, &NextToken);
        while (Token != NULL)
        {
            if (strncmp(Token, "..", 2) == 0)
                Depth --;
            else
                Depth ++;

            Token = strtok_r(NULL, Search, &NextToken);
        }
    }

    if (Depth < 0)
        return false;

    return true;
}

ZEString ZEPathManager::PathFormatCheck(const ZEString& Path)
    // Checks the format of the path for slash correctness
{
    ZESize Lenght;
    ZEString Temp;

    Temp = Path;

    // Remove any (back)slash from beginning of path
    if (Temp[0] == '\\' || Temp[0] == '/')
        Temp = Temp.Right(Temp.GetLength()-1);

    Lenght = Temp.GetLength()-1;
    // Remove any (back)slash from end of path
    if (Temp[Lenght] == '\\' || Temp[Lenght] == '/')
        Temp = Temp.Left(Temp.GetLength()-1);

    // Correct (back)slashes
    Lenght = Temp.GetLength();
    for (ZESize I = 0; I < Lenght; ++I)
        if (Temp[I] == '\\')
            Temp[I] = '/';


    return Temp;
}

// Checks format, constructs final path and makes a boundary check
ZEString ZEPathManager::GetFinalPath(const ZEString& Path, ZEFileKnownPaths& Root)
{
    ZESize SymbolCount;
    ZEString FinalPath;

    // Check/Correct format of the given string
    FinalPath = PathFormatCheck(Path);

    Root = GetRootByString(Path);
    if (Root == ZE_FKP_NONE)
    {
        // No Symbol found
        SymbolCount = 0;

        // Set default path
        if (EnablePathRestriction)
            Root = ZE_FKP_RESOURCES;
    }
    else
    {
        SymbolCount = 1;
    }

    // Do not include symbol
    ZEString RootPath = GetKnownPath(Root);
    FinalPath = RootPath + "/" + FinalPath.Right(FinalPath.GetLength() - SymbolCount);

    // Check boundary
    if ( !PathBoundaryCheck(RootPath, FinalPath) )
    {
        zeError("Reaching files above the root is forbidden...");
        return "";
    }

    return FinalPath;
}
