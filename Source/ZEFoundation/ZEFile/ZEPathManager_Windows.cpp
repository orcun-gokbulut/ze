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
#include "ZEPathUtils.h"

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

static ZEString FixAbsolutePath(const ZEString& AbsolutePath)
{
	char* Token;
	char* Context;
	bool DriveFound = false;
	ZEString Temp = AbsolutePath;
	const char* Search = "\\/";

	ZEString WorkDrive = ZEPathManager::GetWorkingDrive();
	ZESize Len = WorkDrive.GetLength();
	
	Token = strtok_s((char*)Temp.ToCString(), Search, &Context);
	if (Token != NULL)
	{
		if (strlen(Token) == Len && strncmp(WorkDrive, Token, Len) == 0)
			DriveFound = true;
	}

	if (!DriveFound)
	{
		if (AbsolutePath[0] == '\\' || AbsolutePath[0] == '/')
			return WorkDrive + AbsolutePath;
		else
			return ZEPathUtils::GetSeperator() + AbsolutePath;
	}
	else
	{
		return AbsolutePath;
	}
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

	const ZEString PathSeperator = ZEPathUtils::GetSeperator();

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

	// Get PathToAppResources
	AppResourcesPath = NPath;
	AppResourcesPath += PathSeperator;
	AppResourcesPath += ResourceDirName;
	AppResourcesPath += PathSeperator;
	AppResourcesPath += ApplicationName;
	
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
				return ZEPathUtils::GetEmptyPath();
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
	ZEKnownPath Rt;
	ZEString FinalPath;
	ZEString AbsolutePath;
	ZEString RelativePartOfPath;
	ZEString RootPath;

	if (ZEPathUtils::IsAbsolutePath(Path))
	{
		AbsolutePath = FixAbsolutePath(Path);
		Rt = GetKnownPath(AbsolutePath);
		RootPath = GetKnownPath(Rt);
	}
	else
	{
		Rt = ZEPathUtils::SearchForSymbol(&RelativePartOfPath, Path);
		RootPath = GetKnownPath(Rt, Rt);
		AbsolutePath = RootPath + RelativePartOfPath;
	}
	
	FinalPath = ZEPathUtils::GetSimplifiedPath(AbsolutePath);

	if (EnablePathRestriction)
	{
		if (!ZEPathUtils::CheckForRestriction(RootPath, FinalPath))
		{
			zeError("Path is restricted!\nPath: \"%s\".\nRoot: \"%s\".", Path.ToCString(), RootPath.ToCString());
			return "";
		}
	}
	
	if (Root != NULL)
		*Root = Rt;

	return FinalPath;
}
