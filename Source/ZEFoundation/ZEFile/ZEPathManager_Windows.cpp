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

void ZEPathManager::CustomizePaths(const ZEString* CompanyName, const ZEString* ApplicationName, const ZEString* ResourceDirectoryName)
{
	const ZEString PathSeperator = ZEPathUtils::GetSeperator();

	HRESULT Result;
	DWORD Lenght;
	PWSTR Path;
	TCHAR NPath[MAX_PATH + 1];

	Lenght = GetCurrentDirectory(MAX_PATH, NPath);
	if (Lenght == 0)
	{
		ZEString ErrorString;
		ZEFileUtils::GetErrorString(ErrorString, GetLastError());
		zeError("Cannot get current directory.\nError: %s", ErrorString.ToCString());
		return;
	}

	char* Token;
	char* Context;
	ZEString Temp = NPath;
	const char* Search = "\\/";

	Token = strtok_s((char*)Temp.ToCString(), Search, &Context);
	if (Token == NULL)
	{
		ZEString ErrorString;
		ZEFileUtils::GetErrorString(ErrorString, GetLastError());
		zeError("Cannot get working drive.\nError: %s", ErrorString.ToCString());
		return;
	}

	WorkingDrive = Token;
	WorkingDirectory = NPath;

	// Create AppResourcesPath
	AppResourcesPath = NPath;
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
	ResourcesPath = NPath;
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
		// Create SystemDataPath
		Result = SHGetKnownFolderPath(FOLDERID_ProgramData, KF_FLAG_CREATE, NULL, &Path);
		if ( Result != S_OK )
		{
			ZEString ErrorString;
			ZEFileUtils::GetErrorString(ErrorString, GetLastError());
			zeError("Cannot get common app data directory path.\nError: %s", ErrorString.ToCString());
			return;
		}
		SystemDataPath = ZEString::FromWCString(Path);
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
		if ( Result != S_OK )
		{
			ZEString ErrorString;
			ZEFileUtils::GetErrorString(ErrorString, GetLastError());
			zeError("Cannot get local app data directory path.\nError: %s", ErrorString.ToCString());
			return;
		}
		UserDataPath = ZEString::FromWCString(Path);
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
		if ( Result != S_OK )
		{
			ZEString ErrorString;
			ZEFileUtils::GetErrorString(ErrorString, GetLastError());
			zeError("Cannot get local app data directory path.\nError: %s", ErrorString.ToCString());
			return;
		}
		SavedGamesPath = ZEString::FromWCString(Path);
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
		// Create SystemDataPath
		Result = SHGetFolderPath(NULL, CSIDL_FLAG_CREATE | CSIDL_COMMON_APPDATA, NULL, 0, NPath);
		if ( Result != S_OK )
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
		Result = SHGetFolderPath(NULL, CSIDL_FLAG_CREATE | CSIDL_LOCAL_APPDATA, NULL, 0, NPath);
		if ( Result != S_OK )
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
		Result = SHGetFolderPath(NULL, CSIDL_FLAG_CREATE | CSIDL_MYDOCUMENTS, NULL, 0, NPath);
		if ( Result != S_OK )
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

ZEKnownPath ZEPathManager::GetKnownPath(const ZEString& AbsolutePath)
{
#define Compare(Str1, Str2, Length)  (strncmp((Str1), (Str2), (Length)) == 0)

	ZESize Length = 0;
	ZEKnownPath Root = ZE_KP_NONE;
	ZESize PathLen = AbsolutePath.GetLength();
	ZESize ResrcLen = ResourcesPath.GetLength();
	ZESize AppResLen = AppResourcesPath.GetLength();
	ZESize WrkDirLen = WorkingDirectory.GetLength();
	ZESize SysDataLen = SystemDataPath.GetLength();
	ZESize UsrDataLen = UserDataPath.GetLength();
	ZESize SvdGameLen = SavedGamesPath.GetLength();

	if (PathLen >= ResrcLen && ResrcLen > Length &&
		Compare(AbsolutePath, ResourcesPath, ResrcLen))
	{
		Length = ResrcLen;
		Root = ZE_KP_RESOURCES;
	}
	if (PathLen >= AppResLen && AppResLen > Length &&
		Compare(AbsolutePath, AppResourcesPath, AppResLen))
	{
		Length = ResrcLen;
		Root = ZE_KP_APP_RESOURCES;
	}
	if (PathLen >= WrkDirLen && WrkDirLen > Length &&
		Compare(AbsolutePath, WorkingDirectory, WrkDirLen))
	{
		Length = WrkDirLen;
		Root = ZE_KP_WORKING_DIRECTORY;
	}
	if (PathLen >= SysDataLen && SysDataLen > Length &&
		Compare(AbsolutePath, SystemDataPath, SysDataLen))
	{
		Length = SysDataLen;
		Root = ZE_KP_SYSTEM_DATA;
	}
	if (PathLen >= UsrDataLen && UsrDataLen > Length &&
		Compare(AbsolutePath, UserDataPath, UsrDataLen))
	{
		Length = UsrDataLen;
		Root = ZE_KP_USER_DATA;
	}
	if (PathLen >= SvdGameLen && SvdGameLen > Length &&
		Compare(AbsolutePath, SavedGamesPath, SvdGameLen))
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

	if (ISSEPERATOR(AbsolutePath[0]))
	{
		Offset += 1;
		RootFound = true;
	}

	char* Token;
	char* Context;
	ZEString Temp = AbsolutePath;
	const char* Search = "\\/";
	char* Source = (char*)Temp.ToCString();

	Token = strtok_s(Source, Search, &Context);
	if (Token != NULL && strlen(Token) == 2 && ISDRIVELETTER(Token[0]) && Token[1] == ':')
	{
		Offset += strlen(Token);
		DriveFound = true;
	}

	if (DriveFound)
	{
		// If there is a drive letter use it
		Drive += Token;
		Drive += ZEPathUtils::GetSeperator();
		RestOfPath = AbsolutePath.Right(AbsolutePath.GetLength() - Offset);
	}
	else
	{
		if (RootFound)
		{
			// If no drive but root is found
			Drive += ZEPathManager::GetWorkingDrive();
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
ZEString ZEPathManager::GetFinalPath(const ZEString& Path, ZEKnownPath* Root)
{
	ZEString RootPath;
	ZEString FinalPath;
	ZEString RelativePartOfPath;
	ZEKnownPath RootSymbol;

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
	        RootSymbol = GetKnownPath(FinalPath);

			// Get root symbol path
	        RootPath = GetKnownPath(RootSymbol);

			// Check for restriction
	        if (!ZEPathUtils::CheckForRestriction(RootPath, FinalPath))
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

	        // Get root path
            RootPath = GetKnownPath(RootSymbol);

            // Construct temp path
            FinalPath = RootPath;
            FinalPath += ZEPathUtils::GetSeperator();
            FinalPath += RelativePartOfPath;

            // We have a drive since we have path restriction, do not stack dot dot
            FinalPath = ZEPathUtils::GetSimplifiedPath(FinalPath, false);

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
			RootSymbol = ZEPathUtils::SearchForSymbol(&RelativePartOfPath, Path);

			RootPath = GetKnownPath(RootSymbol);

			FinalPath = RootPath;
			FinalPath += ZEPathUtils::GetSeperator();
			FinalPath += RelativePartOfPath;

			// No restriction, dot dots can be stacked
			FinalPath = ZEPathUtils::GetSimplifiedPath(FinalPath, true);
			
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
