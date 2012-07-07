//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPathManager_Windows.cpp
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

#include <winerror.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <winbase.h>
#include "ZEDS\ZEArray.h"

bool ZEPathManager::Initialized = false;
bool ZEPathManager::EnablePathRestriction = false;

ZEString ZEPathManager::UserDataPath = "";
ZEString ZEPathManager::ResourcesPath = "";
ZEString ZEPathManager::SystemDataPath = "";
ZEString ZEPathManager::SavedGamesPath = "";
ZEString ZEPathManager::WorkingDirectory = "";
ZEString ZEPathManager::ApplicationResourcesPath = "";

ZEString ZEPathManager::CompanyName = "Zinek";
ZEString ZEPathManager::ApplicationName = "Engine";
ZEString ZEPathManager::ResourceDirName = "Resources";

static const ZEString EmptyPath = "";
static const ZEString CurDirPath = ".";
static const ZEString UpperDirPath = "..";
static const ZEString PathSeperator = "\\";

// Helper functions to get OS version Info
static bool GetOSVersion(ZEUInt& VerMajor, ZEUInt& VerMinor, ZEUInt& Build)
{
    BOOL Result;
    OSVERSIONINFO OSInfo;

    ZESize StructSize = sizeof(OSVERSIONINFO);
    memset((void*)&OSInfo, 0, StructSize);
    OSInfo.dwOSVersionInfoSize = (DWORD)StructSize;

    SetLastError(ERROR_SUCCESS);
    Result = GetVersionEx(&OSInfo);
    if ( !Result )
    {
        ZEString ErrorString;
        ZEFileUtils::GetErrorString(ErrorString, GetLastError());
        zeError("Cannot get OS version.\nError: %s", ErrorString.ToCString());
        return false;
    }

    Build = OSInfo.dwBuildNumber;
    VerMajor = OSInfo.dwMajorVersion;
    VerMinor = OSInfo.dwMinorVersion;

    return true;
}



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

    ZEUInt OSVerMajor, OSVerMinor, OSBuild;
    GetOSVersion(OSVerMajor, OSVerMinor, OSBuild);

    HRESULT Result;
    DWORD Lenght;
    PWSTR Path;
    TCHAR NPath[MAX_PATH+1];

    Lenght = GetCurrentDirectory(MAX_PATH, NPath);
    if ( Lenght == 0 )
    {
        ZEString ErrorString;
        ZEFileUtils::GetErrorString(ErrorString, GetLastError());
        zeError("Cannot get current directory.\nError: %s", ErrorString.ToCString());
        return;
    }

	WorkingDirectory = NPath;

    // Get PathToAppResources
    ApplicationResourcesPath = NPath;
    ApplicationResourcesPath += PathSeperator;
	ApplicationResourcesPath += ResourceDirName;
	ApplicationResourcesPath += PathSeperator;
    ApplicationResourcesPath += ApplicationName;

    // Get PathToResources
    ResourcesPath = NPath;
    ResourcesPath += PathSeperator;
	ResourcesPath += ResourceDirName;

    // Server2008/Vista and later
    if (OSVerMajor >= 6 && OSVerMinor >= 0)
    {
        // Get PathToSystemData
        Result = SHGetKnownFolderPath(FOLDERID_ProgramData, KF_FLAG_CREATE, NULL, &Path);
        if ( Result != S_OK )
        {
            ZEString ErrorString;
            ZEFileUtils::GetErrorString(ErrorString, GetLastError());
            zeError("Cannot get common app data directory path.\nError: %s", ErrorString.ToCString());
            return;
        }

        SystemDataPath = ZEString::FromWString(Path);
		SystemDataPath += PathSeperator;
		SystemDataPath += CompanyName;
		SystemDataPath += PathSeperator;
        SystemDataPath += ApplicationName;
        CoTaskMemFree((void*)Path);

        // Get PathToUserData
        Result = SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, NULL, &Path);
        if ( Result != S_OK )
        {
            ZEString ErrorString;
            ZEFileUtils::GetErrorString(ErrorString, GetLastError());
            zeError("Cannot get local app data directory path.\nError: %s", ErrorString.ToCString());
            return;
        }

        UserDataPath = ZEString::FromWString(Path);
		UserDataPath += PathSeperator;
		UserDataPath += CompanyName;
		UserDataPath += PathSeperator;
        UserDataPath += ApplicationName;
        CoTaskMemFree((void*)Path);

        // Get PathToSavedGames
        Result = SHGetKnownFolderPath(FOLDERID_SavedGames, KF_FLAG_CREATE, NULL, &Path);
        if ( Result != S_OK )
        {
            ZEString ErrorString;
            ZEFileUtils::GetErrorString(ErrorString, GetLastError());
            zeError("Cannot get local app data directory path.\nError: %s", ErrorString.ToCString());
            return;
        }

        SavedGamesPath = ZEString::FromWString(Path);
		SavedGamesPath += PathSeperator;
		SavedGamesPath += CompanyName;
		SavedGamesPath += PathSeperator;
        SavedGamesPath += ApplicationName;
        CoTaskMemFree((void*)Path);

    }
    // Server2003/XP and previous
    else if (OSVerMajor <= 5 && OSVerMinor <= 2)
    {
        // Get PathToSystemData
        Result = SHGetFolderPath(NULL, CSIDL_FLAG_CREATE | CSIDL_COMMON_APPDATA, NULL, 0, NPath);
        if ( Result != S_OK )
        {
            ZEString ErrorString;
            ZEFileUtils::GetErrorString(ErrorString, GetLastError());
            zeError("Cannot get common app data directory path.\nError: %s", ErrorString.ToCString());
            return;
        }

        SystemDataPath = NPath;
		SystemDataPath += PathSeperator;
		SystemDataPath += CompanyName;
		SystemDataPath += PathSeperator;
        SystemDataPath += ApplicationName;

        // Get PathToUserData
        Result = SHGetFolderPath(NULL, CSIDL_FLAG_CREATE | CSIDL_LOCAL_APPDATA, NULL, 0, NPath);
        if ( Result != S_OK )
        {
            ZEString ErrorString;
            ZEFileUtils::GetErrorString(ErrorString, GetLastError());
            zeError("Cannot get local app data directory path.\nError: %s", ErrorString.ToCString());
            return;
        }

        UserDataPath = NPath;
		UserDataPath += PathSeperator;
		UserDataPath += CompanyName;
		UserDataPath += PathSeperator;
        UserDataPath += ApplicationName;

        // Get PathToSavedGames
        Result = SHGetFolderPath(NULL, CSIDL_FLAG_CREATE | CSIDL_MYDOCUMENTS, NULL, 0, NPath);
        if ( Result != S_OK )
        {
            ZEString ErrorString;
            ZEFileUtils::GetErrorString(ErrorString, GetLastError());
            zeError("Cannot get local app data directory path.\nError: %s", ErrorString.ToCString());
            return;
        }

        SavedGamesPath = NPath;
		SavedGamesPath += PathSeperator;
		SavedGamesPath += CompanyName;
		SavedGamesPath += PathSeperator;
        SavedGamesPath += ApplicationName;
    }

    Initialized = true;
}

bool ZEPathManager::GetInitialized()
{
    return Initialized;
}

void ZEPathManager::SetCompanyName(const ZEString& Name)
{
	CompanyName = Name;

	InitializePaths();
}

const ZEString& ZEPathManager::GetCompanyName()
{
	return CompanyName;
}

void ZEPathManager::SetApplicationName(const ZEString& Name)
{
	ApplicationName = Name;

	InitializePaths();
}

const ZEString& ZEPathManager::GetApplicationName()
{
	return ApplicationName;
}

void ZEPathManager::SetResourceDirName(const ZEString& Name)
{
	ResourceDirName;

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

	return ApplicationResourcesPath;
}

const ZEString& ZEPathManager::GetPathSeperator()
{
	return PathSeperator;
}

const ZEString& ZEPathManager::GetKnownPath(ZEKnownPath KnownPath)
{
    InitializePaths();

    switch (KnownPath)
    {
        case ZE_KP_NONE:
			if (!EnablePathRestriction)
				return EmptyPath;
			// Fall Through!!!
		case ZE_KP_WORKING_DIRECTORY:
			return WorkingDirectory;
			break;
        case ZE_KP_RESOURCES:
            return ResourcesPath;
            break;
        case ZE_KP_APP_RESOURCES:
            return ApplicationResourcesPath;
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
            return EmptyPath;
            break;
    }
}

ZEKnownPath ZEPathManager::GetKnownPath(ZEString* RelativePart, const ZEString& SymbolicPath)
{
	char* Token;
	char* Context;
	ZEString Temp = SymbolicPath;
	bool SymbolFound = false;
	ZEKnownPath Root;
	const char* Search = "\\/";
	
	if (RelativePart != NULL)
		RelativePart->IsEmpty();

	Token = strtok_s((char*)Temp.ToCString(), Search, &Context);
	while (Token != NULL)
	{
		if (!SymbolFound)
		{
			// Check token for known symbols
			if (strlen(Token) == 1)
			{
				switch (*Token)
				{
					// Need to tokenize here to skip symbol
					case ZE_PATH_SYMBOL_USER_DATA:
						Token = strtok_s(NULL, Search, &Context);
						Root = ZE_KP_USER_DATA;
						SymbolFound = true;
						break;
					case ZE_PATH_SYMBOL_SYSTEM_DATA:
						Token = strtok_s(NULL, Search, &Context);
						Root = ZE_KP_SYSTEM_DATA;
						SymbolFound = true;
						break;
					case ZE_PATH_SYMBOL_SAVED_GAMES:
						Token = strtok_s(NULL, Search, &Context);
						Root = ZE_KP_SAVED_GAMES;
						SymbolFound = true;
						break;
					case ZE_PATH_SYMBOL_APP_RESOURCES:
						Token = strtok_s(NULL, Search, &Context);
						Root = ZE_KP_APP_RESOURCES;
						SymbolFound = true;
						break;
					case ZE_PATH_SYMBOL_RESOURCES:
						Token = strtok_s(NULL, Search, &Context);
						Root = ZE_KP_RESOURCES;
						SymbolFound = true;
						break;
				}
			}
		}
		
		if (SymbolFound)
		{
			if (RelativePart != NULL)
			{
				// Add to output
				*RelativePart += PathSeperator;
				*RelativePart += Token;
			}
			else
			{
				break;
			}
		}
		
		Token = strtok_s(NULL, Search, &Context);
	}

	if (!SymbolFound)
	{
		if (EnablePathRestriction)
		{
			*RelativePart = "";
		}
		else
		{
			*RelativePart = SymbolicPath;
		}

		return ZE_KP_NONE;
	}
	else
	{
		return Root;
	}
}

bool ZEPathManager::IsAbsolutePath(const ZEString& Path)
{
	ZESize Length = Path.GetLength();
	ZESize Offset = 0;
	
	// Skip the separator if it is at the beginning
	if (Path[0] == PathSeperator[0])
		Offset = 1;
	
	// Search for ":"
	ZESize Index = 0;
	while ((Index + Offset < Length) && (Path[Index + Offset] != PathSeperator[0]))
	{
		if (Path[Index + Offset] == ':')
			return true;

		Index++;
	}

	// ":" not found
	return false;
}

bool ZEPathManager::IsRelativePath(const ZEString& Path)
{
	if(EnablePathRestriction)
	{
		return GetRelativePath(Path).IsEmpty() ? false : true;
	}
	
	return IsAbsolutePath(Path) ? false : true;
}


ZEString ZEPathManager::GetRelativePath(const ZEString& FullPath)
{	
	ZESize WorkDirLen;
	ZESize ResDirLen;
	ZESize AppResDirLen;
	ZESize SysDirLen;
	ZESize UsrDirLen;
	ZESize SaveDirLen;

	ZESize SrcDirLen = FullPath.GetLength();
	const char* SrcDir = FullPath.ToCString();
	
	if ((SrcDirLen >= (WorkDirLen = WorkingDirectory.GetLength())) &&
		(strncmp(SrcDir, WorkingDirectory.ToCString(), WorkDirLen) == 0))
	{
		return FullPath.Right(WorkDirLen);
	}
	else if ((SrcDirLen >= (ResDirLen = ResourcesPath.GetLength())) &&
			(strncmp(SrcDir, ResourcesPath.ToCString(), ResDirLen) == 0))
	{
		return FullPath.Right(ResDirLen);
	}
	else if ((SrcDirLen >= (AppResDirLen = ApplicationResourcesPath.GetLength())) &&
			(strncmp(SrcDir, ApplicationResourcesPath.ToCString(), AppResDirLen) == 0))
	{
		return FullPath.Right(AppResDirLen);
	}
	else if ((SrcDirLen >= (SysDirLen = SystemDataPath.GetLength())) &&
			(strncmp(SrcDir, SystemDataPath.ToCString(), SysDirLen) == 0))
	{
		return FullPath.Right(SysDirLen);
	}
	else if ((SrcDirLen >= (UsrDirLen = UserDataPath.GetLength())) &&
			(strncmp(SrcDir, UserDataPath.ToCString(), UsrDirLen) == 0))
	{
		return FullPath.Right(UsrDirLen);	
	}
	else if ((SrcDirLen >= (SaveDirLen = SavedGamesPath.GetLength())) &&
			(strncmp(SrcDir, SavedGamesPath.ToCString(), SaveDirLen) == 0))
	{
		return FullPath.Right(SaveDirLen);
	}
	else
	{
		return "";
	}
}

ZEString ZEPathManager::GetAbsolutePath(const ZEString& RelativePath)
{
	zeCriticalError("Not implemented");
}

bool ZEPathManager::PathBoundaryCheck(const ZEString& RootPath, const ZEString& Path)
{
    char* Token;
    char* NextToken;
    ZEInt Depth;
    ZEString Temp;
    const char* Search = PathSeperator.ToCString();

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
        Token = strtok_s((char*)Temp.ToCString(), Search, &NextToken);
        while (Token != NULL)
        {
            if (strncmp(Token, "..", 2) == 0)
                Depth --;
            else
                Depth ++;

            Token = strtok_s(NULL, Search, &NextToken);
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
        if (Temp[I] == '/')
            Temp[I] = '\\';


    return Temp;
}

ZEString ZEPathManager::SimplifyPath(const ZEString& Path)
{
	ZEInt Depth = 0;
	ZEString Output;
	char* Context = NULL;
	char* NextToken = NULL;
	char* CurrentToken = NULL;
	ZEString TempSource = Path;
	const char* Search = GetPathSeperator();
	char* Source = (char*)TempSource.ToCString();
	
	struct ZEToken
	{
		char*	Token;
		ZESize	Lenth;

		bool IsDot()
		{
			return (Lenth == 1) && (strncmp(Token, ".", Lenth) == 0);
		}
		bool IsDotDot()
		{
			return (Lenth == 2) && (strncmp(Token, "..", Lenth) == 0);
		}
	};

	ZESmartArray<ZEToken> Tokens;

	// Find all tokens and put them in an array
	CurrentToken = strtok_s(Source, Search, &Context);
	while (CurrentToken != NULL)
	{
		ZEToken Token = {CurrentToken, strlen(CurrentToken)};
		Tokens.Add(Token);

		CurrentToken = strtok_s(NULL, Search, &Context);
	}
	
	ZESize Length = Tokens[0].Lenth;

	// Process tokens
	ZESize Index = 0;
	ZEUInt UsefulTokens = 0;
	while ((Index < Tokens.GetCount()) && (Tokens[Index].Token != NULL))
	{
		if (Tokens[Index].IsDot())
		{
			// Omit "."
			Tokens.DeleteAt(Index);
			continue;
		}
		else if (Tokens[Index].IsDotDot())
		{
			// Omit ".." if there is useful tokens
			if (UsefulTokens > 0)
			{
				// Drop the last useful token
				Tokens.DeleteAt(--Index);
				Tokens.DeleteAt(Index);
				UsefulTokens--;
			}
			else
			{
				Depth--;
				Index++;
				// leave ".."
			}
		}
		else
		{
			UsefulTokens++;
			Depth++;
			Index++;
		}
	}

	Index = 0;
	while ((Index < Tokens.GetCount()) && (Tokens[Index].Token != NULL))
	{
		Output += Tokens[Index].Token;
		Output += PathSeperator;
		Index++;
	}

	// Remove last separator
	Output.Remove(Output.GetLength()-1, 1);

	return Output;
}

// Checks format, constructs final path and makes a boundary check
ZEString ZEPathManager::GetFinalPath(const ZEString& Path, ZEKnownPath* Root)
{
    ZEString FinalPath;
	ZEString AbsolutePath;
	ZEString RelativePath;
	ZEKnownPath Temp;

    // Check/Correct format of the given string
    // FinalPath = PathFormatCheck(Path);
    Temp = GetKnownPath(&RelativePath, Path);
    AbsolutePath = GetKnownPath(Temp) + RelativePath;
	FinalPath = SimplifyPath(AbsolutePath);


	if (Root != NULL)
		*Root = Temp;

    return FinalPath;
}
