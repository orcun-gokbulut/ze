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
#include "ZEPathUtils.h"
#include "ZEFileUtils.h"
#include "ZEDS\ZEArray.h"
#include "ZEPathManager.h"

#include <winerror.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <winbase.h>

bool ZEPathManager::EnablePathRestriction = false;

ZEString ZEPathManager::WorkingDrive = "";
ZEString ZEPathManager::UserDataPath = "";
ZEString ZEPathManager::ResourcesPath = "";
ZEString ZEPathManager::SystemDataPath = "";
ZEString ZEPathManager::SavedGamesPath = "";
ZEString ZEPathManager::WorkingDirectory = "";
ZEString ZEPathManager::AppResourcesPath = "";

// Helper functions to get OS version Info
static bool GetOSVersion(ZEUInt& VerMajor, ZEUInt& VerMinor, ZEUInt& Build)
{
	BOOL Result;
	OSVERSIONINFOW OSInfo;
	
	ZESize StructSize = sizeof(OSVERSIONINFOW);
	memset((void*)&OSInfo, 0, StructSize);
	OSInfo.dwOSVersionInfoSize = (DWORD)StructSize;
	
	SetLastError(ERROR_SUCCESS);
	Result = GetVersionExW(&OSInfo);
	if (!Result)
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

void ZEPathManager::CustomizePaths(const ZEString* CompanyName, const ZEString* ApplicationName, const ZEString* ResourceDirectoryName)
{
	const ZEString PathSeperator = ZEPathUtils::GetSeperator();

	DWORD Lenght = 0;
	WCHAR NPath[MAX_PATH] = {0};

	Lenght = GetCurrentDirectoryW(MAX_PATH, NPath);
	if (Lenght == 0)
	{
		ZEString ErrorString;
		ZEFileUtils::GetErrorString(ErrorString, GetLastError());
		zeError("Cannot get current directory.\nError: %s", ErrorString.ToCString());
		return;
	}

	// Save working directory
	WorkingDirectory = NPath;

	wchar_t* Token;
	wchar_t* Context;
	const wchar_t* Search = L"\\/";
	
	// Tokenize working directory and get working drive
	Token = wcstok_s(NPath, Search, &Context);
	if (Token == NULL)
	{
		ZEString ErrorString;
		ZEFileUtils::GetErrorString(ErrorString, GetLastError());
		zeError("Cannot get working drive.\nError: %s", ErrorString.ToCString());
		return;
	}

	// Save working drive
	WorkingDrive = Token;
	
	// Create AppResourcesPath
	AppResourcesPath = WorkingDirectory;
	if (ResourceDirectoryName != NULL)
	{
		AppResourcesPath += PathSeperator;
		AppResourcesPath += *ResourceDirectoryName;
	}
	if (ApplicationName != NULL)
	{
		AppResourcesPath += PathSeperator;
		AppResourcesPath += *ApplicationName;
	}

	// Create ResourcesPath
	ResourcesPath = WorkingDirectory;
	if (ResourceDirectoryName != NULL)
	{
		ResourcesPath += PathSeperator;
		ResourcesPath += *ResourceDirectoryName;
	}
	
	ZEUInt OSVerMajor, OSVerMinor, OSBuild;
	GetOSVersion(OSVerMajor, OSVerMinor, OSBuild);

	// Server2008/Vista and later
	if (OSVerMajor >= 6 && OSVerMinor >= 0)
	{
		PWSTR Path = NULL;
		HRESULT Result = 0;

		// Create SystemDataPath
		Result = SHGetKnownFolderPath(FOLDERID_ProgramData, KF_FLAG_CREATE, NULL, &Path);
		if (Result != S_OK)
		{
			ZEString ErrorString;
			ZEFileUtils::GetErrorString(ErrorString, GetLastError());
			zeError("Cannot get common app data directory path.\nError: %s", ErrorString.ToCString());
			return;
		}

		SystemDataPath = Path;
		if (CompanyName != NULL)
		{
			SystemDataPath += PathSeperator;
			SystemDataPath += *CompanyName;
		}
		if (ApplicationName != NULL)
		{
			SystemDataPath += PathSeperator;
			SystemDataPath += *ApplicationName;
		}
		CoTaskMemFree((void*)Path);

		// Create UserDataPath
		Result = SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, NULL, &Path);
		if (Result != S_OK)
		{
			ZEString ErrorString;
			ZEFileUtils::GetErrorString(ErrorString, GetLastError());
			zeError("Cannot get local app data directory path.\nError: %s", ErrorString.ToCString());
			return;
		}

		UserDataPath = Path;
		if (CompanyName != NULL)
		{
			UserDataPath += PathSeperator;
			UserDataPath += *CompanyName;
		}
		if (ApplicationName != NULL)
		{
			UserDataPath += PathSeperator;
			UserDataPath += *ApplicationName;
		}
		CoTaskMemFree((void*)Path);

		// Create SavedGamesPath
		Result = SHGetKnownFolderPath(FOLDERID_SavedGames, KF_FLAG_CREATE, NULL, &Path);
		if (Result != S_OK)
		{
			ZEString ErrorString;
			ZEFileUtils::GetErrorString(ErrorString, GetLastError());
			zeError("Cannot get local app data directory path.\nError: %s", ErrorString.ToCString());
			return;
		}

		SavedGamesPath = Path;
		if (CompanyName != NULL)
		{
			SavedGamesPath += PathSeperator;
			SavedGamesPath += *CompanyName;
		}
		if (ApplicationName != NULL)
		{
			SavedGamesPath += PathSeperator;
			SavedGamesPath += *ApplicationName;
		}
		CoTaskMemFree((void*)Path);

	}
	// Server2003/XP and previous
	else if (OSVerMajor <= 5 && OSVerMinor <= 2)
	{	
		HRESULT Result = 0;
		
		// Create SystemDataPath
		Result = SHGetFolderPathW(NULL, CSIDL_FLAG_CREATE | CSIDL_COMMON_APPDATA, NULL, 0, NPath);
		if (Result != S_OK)
		{
			ZEString ErrorString;
			ZEFileUtils::GetErrorString(ErrorString, GetLastError());
			zeError("Cannot get common app data directory path.\nError: %s", ErrorString.ToCString());
			return;
		}

		SystemDataPath = NPath;
		if (CompanyName != NULL)
		{
			SystemDataPath += PathSeperator;
			SystemDataPath += *CompanyName;
		}
		if (ApplicationName != NULL)
		{
			SystemDataPath += PathSeperator;
			SystemDataPath += *ApplicationName;
		}

		// Create UserDataPath
		Result = SHGetFolderPathW(NULL, CSIDL_FLAG_CREATE | CSIDL_LOCAL_APPDATA, NULL, 0, NPath);
		if (Result != S_OK)
		{
			ZEString ErrorString;
			ZEFileUtils::GetErrorString(ErrorString, GetLastError());
			zeError("Cannot get local app data directory path.\nError: %s", ErrorString.ToCString());
			return;
		}

		UserDataPath = NPath;
		if (CompanyName != NULL)
		{
			UserDataPath += PathSeperator;
			UserDataPath += *CompanyName;
		}
		if (ApplicationName != NULL)
		{
			UserDataPath += PathSeperator;
			UserDataPath += *ApplicationName;
		}

		// Create SavedGamesPath
		Result = SHGetFolderPathW(NULL, CSIDL_FLAG_CREATE | CSIDL_MYDOCUMENTS, NULL, 0, NPath);
		if (Result != S_OK)
		{
			ZEString ErrorString;
			ZEFileUtils::GetErrorString(ErrorString, GetLastError());
			zeError("Cannot get local app data directory path.\nError: %s", ErrorString.ToCString());
			return;
		}

		SavedGamesPath = NPath;
		if (CompanyName != NULL)
		{
			SavedGamesPath += PathSeperator;
			SavedGamesPath += *CompanyName;
		}
		if (ApplicationName != NULL)
		{
			SavedGamesPath += PathSeperator;
			SavedGamesPath += *ApplicationName;
		}
	}
}

const ZEString& ZEPathManager::GetWorkingDrive()
{
	return WorkingDrive;
}

const ZEString& ZEPathManager::GetWorkingDirectory()
{
	return WorkingDirectory;
}

void ZEPathManager::SetEnablePathRestriction(bool Enable)
{
	EnablePathRestriction = Enable;
}

bool ZEPathManager::GetEnablePathRestriction()
{
	return EnablePathRestriction;
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

ZEPathRoot ZEPathManager::GetRootPath(const ZEString& AbsolutePath)
{

	ZESize Length = 0;
	ZEPathRoot Root = ZE_KP_NONE;
	ZESize PathLen = strlen(AbsolutePath.ToCString());
	ZESize ResrcLen = strlen(ResourcesPath.ToCString());
	ZESize AppResLen = strlen(AppResourcesPath.ToCString());
	ZESize WrkDirLen = strlen(WorkingDirectory.ToCString());
	ZESize SysDataLen = strlen(SystemDataPath.ToCString());
	ZESize UsrDataLen = strlen(UserDataPath.ToCString());
	ZESize SvdGameLen = strlen(SavedGamesPath.ToCString());

	if (PathLen >= ResrcLen && ResrcLen > Length &&
		(strncmp(AbsolutePath, ResourcesPath, ResrcLen) == 0))
	{
		Length = ResrcLen;
		Root = ZE_KP_RESOURCES;
	}
	if (PathLen >= AppResLen && AppResLen > Length &&
		(strncmp(AbsolutePath, AppResourcesPath, AppResLen) == 0))
	{
		Length = ResrcLen;
		Root = ZE_KP_APP_RESOURCES;
	}
	if (PathLen >= WrkDirLen && WrkDirLen > Length &&
		(strncmp(AbsolutePath, WorkingDirectory, WrkDirLen) == 0))
	{
		Length = WrkDirLen;
		Root = ZE_KP_WORKING_DIRECTORY;
	}
	if (PathLen >= SysDataLen && SysDataLen > Length &&
		(strncmp(AbsolutePath, SystemDataPath, SysDataLen) == 0))
	{
		Length = SysDataLen;
		Root = ZE_KP_SYSTEM_DATA;
	}
	if (PathLen >= UsrDataLen && UsrDataLen > Length &&
		(strncmp(AbsolutePath, UserDataPath, UsrDataLen) == 0))
	{
		Length = UsrDataLen;
		Root = ZE_KP_USER_DATA;
	}
	if (PathLen >= SvdGameLen && SvdGameLen > Length &&
		(strncmp(AbsolutePath, SavedGamesPath, SvdGameLen) == 0))
	{
		Length = SvdGameLen;
		Root = ZE_KP_SAVED_GAMES;
	}
	
	return Root;
}

const ZEString& ZEPathManager::GetRootPath(const ZEPathRoot KnownPath)
{
	switch (KnownPath)
	{
		case ZE_KP_NONE:
			if (!EnablePathRestriction)
				return ZEPathUtils::GetEmptyPath();
			// Fall Through
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

void SeperateDriveAndPath(ZEString& Drive, ZEString& RestOfPath, const ZEString& AbsolutePath)
{
	ZESize Offset = 0;
	bool RootFound = false;
	bool DriveFound = false;

	if (AbsolutePath.IsEmpty())
	{
		Drive = ZEPathUtils::GetEmptyPath();
		RestOfPath = ZEPathUtils::GetEmptyPath();
	}

	// Create a local copy
	ZEString Temp = AbsolutePath;

	// Does it start with root
	if (AbsolutePath[0] == '\\' || AbsolutePath[0] == '/')
	{
		Offset += 1;
		RootFound = true;
	}

	wchar_t* Token;
	wchar_t* Context;
	wchar_t* Source = (wchar_t*)Temp.ToWCString();
	const wchar_t* Search = L"\\/";
	
	Token = wcstok_s(Source, Search, &Context);
	if (Token != NULL && wcslen(Token) == 2)
	{
		if ((L'A' < Token[0]) && (Token[0] < L'Z') && (Token[1] == L':'))
		{
			Offset += wcslen(Token);
			DriveFound = true;
		}
	}

	if (DriveFound)
	{
		// If there is a drive letter use it
		Drive = Token;
		Drive += ZEPathUtils::GetSeperator();
		RestOfPath = AbsolutePath.Right(AbsolutePath.GetLength() - Offset);
	}
	else
	{
		if (RootFound)
		{
			// If no drive but root is found
			Drive = ZEPathManager::GetWorkingDrive();
			Drive += ZEPathUtils::GetSeperator();
			RestOfPath = AbsolutePath.Right(AbsolutePath.GetLength() - Offset);
		}
		else
		{
			// Not an absolute path
			Drive = ZEPathUtils::GetEmptyPath();
			RestOfPath = AbsolutePath;
		}
	}
}

// Checks format, constructs final path and makes a boundary check
ZEString ZEPathManager::GetRealPath(const ZEString& Path, ZEPathRoot* Root)
{
	ZEString RootPath;
	ZEString FinalPath;
	ZEString RelativePartOfPath;
	ZEPathRoot RootSymbol;

	// Absolute Path
	if (ZEPathUtils::IsAbsolutePath(Path))
	{
		ZEString Drive, RestOfPath;
		SeperateDriveAndPath(Drive, RestOfPath, Path);
		// From now on work with RestOfPath

	    if (EnablePathRestriction)
	    {
			// We have a drive, do not stack dots while simplification
	        FinalPath = Drive + ZEPathUtils::GetSimplifiedPath(RestOfPath, false);
			// Try to get root
	        RootSymbol = GetRootPath(FinalPath);
			// Get root symbol path
	        RootPath = GetRootPath(RootSymbol);
			// Check for restriction
	        if (!ZEPathUtils::CheckPathContainsRoot(RootPath, FinalPath))
            {
                zeError("Final path is restricted!\nPath: \"%s\".\nRoot: \"%s\".", FinalPath, RootPath);
                return "";
            }
	    }
	    else // No restriction
	    {
	       // We have a drive, do not stack dots while simplification
	       FinalPath = Drive + ZEPathUtils::GetSimplifiedPath(RestOfPath, false);
           RootSymbol = ZE_KP_NONE;
	    }
	}
	// Relative path
	else
	{
	    if (EnablePathRestriction)
	    {
	        RootSymbol = ZEPathUtils::SearchForSymbol(&RelativePartOfPath, Path);
            RootPath = GetRootPath(RootSymbol);
            // Construct temp path
            FinalPath = RootPath;
            FinalPath += RelativePartOfPath;
            // We have a drive since we have path restriction, do not stack dot dot
            FinalPath = ZEPathUtils::GetSimplifiedPath(FinalPath, false);
            // Root can change after simplification, check again
            RootSymbol = GetRootPath(FinalPath);
            RootPath = GetRootPath(RootSymbol);

            if (!ZEPathUtils::CheckPathContainsRoot(RootPath, FinalPath))
            {
                zeError("Final path is restricted!\nPath: \"%s\".\nRoot: \"%s\".", FinalPath.ToCString(), RootPath.ToCString());
                return "";
            }
	    }
	    else
	    {
			RootSymbol = ZEPathUtils::SearchForSymbol(&RelativePartOfPath, Path);
			RootPath = GetRootPath(RootSymbol);
			FinalPath = RootPath;
			FinalPath += RelativePartOfPath;
			
			// No restriction, dot dots can be stacked since path can be relative
			FinalPath = ZEPathUtils::GetSimplifiedPath(FinalPath, true);

			// No restriction so there is no need for root
			RootSymbol = ZE_KP_NONE;
	    }
	}

	if (Root != NULL)
		*Root = RootSymbol;

	return FinalPath;
}
