//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPathManagerTests.cpp
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


#include "ZETest/ZETest.h"
#include "ZEFile/ZEPathManager.h"
#include "ZEFile/ZEFile.h"
#include "ZEDS/ZEString.h"
#include <stdio.h>
#include "ZEError.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define WINAPI__stdcall
#include "ZEPathUtils.h"


ZE_TEST(ZEPathManager)
{
	ZE_TEST_ITEM("void ZEPathManager::CustomizePaths(const ZEString* CompanyName, const ZEString* ApplicationName, const ZEString* ResourceDirectoryName)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();
		const ZEString CompanyName;
		const ZEString ApplicationName;
		const ZEString ResourceDirectoryName;

		ZEPathManager::CustomizePaths(&CompanyName, &ApplicationName, &ResourceDirectoryName);
		const ZEString WorkingDrive = ZEPathManager::GetWorkingDrive();
		const ZEString WorkingDirectory = ZEPathManager::GetWorkingDirectory();

		ZEPathManager::SetUserDataPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "AppData" + Seperator + "Local" + Seperator + "Zinek" + Seperator + "Engine");
		ZEPathManager::SetResourcesPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources");
		ZEPathManager::SetSystemDataPath("C:\\ProgramData\\Zinek\\Engine");
		ZEPathManager::SetSavedGamesPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "Saved Games" + Seperator + "Zinek" + Seperator + "Engine");
 		ZEPathManager::SetApplicationResourcesPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources" + Seperator + "Engine");
	}

	ZE_TEST_ITEM("bool ZEPathManager::GetEnablePathRestriction()")
	{
		bool EnabledPathRestriction = ZEPathManager::GetEnablePathRestriction();
		ZE_TEST_CHECK_ERROR(!EnabledPathRestriction);
	}

	ZE_TEST_ITEM("void ZEPathManager::SetEnablePathRestriction(bool Enable)")
	{
		ZEPathManager::SetEnablePathRestriction(true);
		ZE_TEST_CHECK_ERROR(ZEPathManager::GetEnablePathRestriction());

		ZEPathManager::SetEnablePathRestriction(false);
		ZE_TEST_CHECK_ERROR(!ZEPathManager::GetEnablePathRestriction());
	}

	ZE_TEST_ITEM("const ZEString& ZEPathManager::GetWorkingDrive()")
	{
 		const ZEString WorkingDrive = ZEPathManager::GetWorkingDrive();
 		ZE_TEST_CHECK_STRING_EQUAL(WorkingDrive, "C:");
	}

	ZE_TEST_ITEM("void ZEPathManager::SetUserDataPath(const ZEString& Name)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEPathManager::SetUserDataPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "AppData" + Seperator + "Local" + Seperator + "Zinek" + Seperator + "Engine", false));

		const ZEString UserDataPath = ZEPathManager::GetUserDataPath();
		ZE_TEST_CHECK_STRING_EQUAL(UserDataPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "AppData" + Seperator + "Local" + Seperator + "Zinek" + Seperator + "Engine", false));
	}

	ZE_TEST_ITEM("const ZEString&	ZEPathManager::GetUserDataPath()")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		const ZEString UserDataPath = ZEPathManager::GetUserDataPath();
		ZE_TEST_CHECK_STRING_EQUAL(UserDataPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "AppData" + Seperator + "Local" + Seperator + "Zinek" + Seperator + "Engine", false));
	}

	ZE_TEST_ITEM("void ZEPathManager::SetResourcesPath(const ZEString& Name)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEPathManager::SetResourcesPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources", false));

		const ZEString ResourcesPath = ZEPathManager::GetResourcesPath();
		ZE_TEST_CHECK_STRING_EQUAL(ResourcesPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources", false));
	}

	ZE_TEST_ITEM("const ZEString&	ZEPathManager::GetResourcesPath()")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		const ZEString ResourcesPath = ZEPathManager::GetResourcesPath();
		ZE_TEST_CHECK_STRING_EQUAL(ResourcesPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources", false));
	}

	ZE_TEST_ITEM("void ZEPathManager::SetSystemDataPath(const ZEString& Name)")
	{
		ZEPathManager::SetSystemDataPath("C:\\ProgramData\\Zinek\\Engine");

		const ZEString SystemDataPath = ZEPathManager::GetSystemDataPath();
		ZE_TEST_CHECK_STRING_EQUAL(SystemDataPath, "C:\\ProgramData\\Zinek\\Engine");
	}

	ZE_TEST_ITEM("const ZEString&	ZEPathManager::GetSystemDataPath()")
	{
		const ZEString SystemDataPath = ZEPathManager::GetSystemDataPath();
		ZE_TEST_CHECK_STRING_EQUAL(SystemDataPath, "C:\\ProgramData\\Zinek\\Engine");
	}

	ZE_TEST_ITEM("void ZEPathManager::SetSavedGamesPath(const ZEString& Name)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEPathManager::SetSavedGamesPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "Saved Games" + Seperator + "Zinek" + Seperator + "Engine", false));

		const ZEString SavedGamesPath = ZEPathManager::GetSavedGamesPath();
		ZE_TEST_CHECK_STRING_EQUAL(SavedGamesPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "Saved Games" + Seperator + "Zinek" + Seperator + "Engine", false));
	}

	ZE_TEST_ITEM("const ZEString&	ZEPathManager::GetSavedGamesPath()")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		const ZEString SavedGamesPath = ZEPathManager::GetSavedGamesPath();
		ZE_TEST_CHECK_STRING_EQUAL(SavedGamesPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "Saved Games" + Seperator + "Zinek" + Seperator + "Engine", false));
	}

	ZE_TEST_ITEM("const ZEString& ZEPathManager::GetWorkingDirectory()")
	{
		const ZEString WorkingDirectory = ZEPathManager::GetWorkingDirectory();
		ZE_TEST_CHECK_STRING_EQUAL(WorkingDirectory, ZEPathManager::GetWorkingDirectory());
	}

	ZE_TEST_ITEM("void ZEPathManager::SetApplicationResourcesPath(const ZEString& Name)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEPathManager::SetApplicationResourcesPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources" + Seperator + "Engine", false));

		const ZEString ApplicationResourcesPath = ZEPathManager::GetApplicationResourcesPath();
		ZE_TEST_CHECK_STRING_EQUAL(ApplicationResourcesPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources" + Seperator + "Engine", false));
	}

	ZE_TEST_ITEM("const ZEString&	ZEPathManager::GetApplicationResourcesPath()")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		const ZEString ApplicationResourcesPath = ZEPathManager::GetApplicationResourcesPath();
		ZE_TEST_CHECK_STRING_EQUAL(ApplicationResourcesPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources" + Seperator + "Engine", false));
	}

	ZE_TEST_ITEM("ZEKnownPath ZEPathManager::GetKnownPath(const ZEString& AbsolutePath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();
		ZEKnownPath KnownPath;

		ZE_TEST_CASE("none")
		{
			KnownPath = ZEPathManager::GetKnownPath("Known.txt");
			ZE_TEST_CHECK_EQUAL(KnownPath, ZE_KP_NONE);
		}

		ZE_TEST_CASE("application resources")
		{
			KnownPath = ZEPathManager::GetKnownPath(ZEPathManager::GetApplicationResourcesPath());
			ZE_TEST_CHECK_EQUAL(KnownPath, ZE_KP_APP_RESOURCES);

			KnownPath = ZEPathManager::GetKnownPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources" + Seperator + "Engine", false));
			ZE_TEST_CHECK_EQUAL(KnownPath, ZE_KP_APP_RESOURCES);

			KnownPath = ZEPathManager::GetKnownPath(ZEPathManager::GetApplicationResourcesPath() + Seperator + "Test");
			ZE_TEST_CHECK_EQUAL(KnownPath, ZE_KP_APP_RESOURCES);
		}

		ZE_TEST_CASE("resources")
		{
			KnownPath = ZEPathManager::GetKnownPath(ZEPathManager::GetResourcesPath());
			ZE_TEST_CHECK_EQUAL(KnownPath, ZE_KP_RESOURCES);

			KnownPath = ZEPathManager::GetKnownPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources", false));
			ZE_TEST_CHECK_EQUAL(KnownPath, ZE_KP_RESOURCES);

			KnownPath = ZEPathManager::GetKnownPath(ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
			ZE_TEST_CHECK_EQUAL(KnownPath, ZE_KP_RESOURCES);
		}

		ZE_TEST_CASE("user data")
		{
			KnownPath = ZEPathManager::GetKnownPath(ZEPathManager::GetUserDataPath());
			ZE_TEST_CHECK_EQUAL(KnownPath, ZE_KP_USER_DATA);

			KnownPath = ZEPathManager::GetKnownPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "AppData" + Seperator + "Local" + Seperator + "Zinek" + Seperator + "Engine", false));
			ZE_TEST_CHECK_EQUAL(KnownPath, ZE_KP_USER_DATA);
		}

		ZE_TEST_CASE("system data")
		{
			KnownPath = ZEPathManager::GetKnownPath(ZEPathManager::GetSystemDataPath());
			ZE_TEST_CHECK_EQUAL(KnownPath, ZE_KP_SYSTEM_DATA);

			KnownPath = ZEPathManager::GetKnownPath("C:\\ProgramData\\Zinek\\Engine");
			ZE_TEST_CHECK_EQUAL(KnownPath, ZE_KP_SYSTEM_DATA);
		}

		ZE_TEST_CASE("saved games")
		{
			KnownPath = ZEPathManager::GetKnownPath(ZEPathManager::GetSavedGamesPath());
			ZE_TEST_CHECK_EQUAL(KnownPath, ZE_KP_SAVED_GAMES);

			KnownPath = ZEPathManager::GetKnownPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "Saved Games" + Seperator + "Zinek" + Seperator + "Engine", false));
			ZE_TEST_CHECK_EQUAL(KnownPath, ZE_KP_SAVED_GAMES);
		}

		ZE_TEST_CASE("working directory")
		{
			KnownPath = ZEPathManager::GetKnownPath(ZEPathManager::GetWorkingDirectory());
			ZE_TEST_CHECK_EQUAL(KnownPath, ZE_KP_WORKING_DIRECTORY);

			KnownPath = ZEPathManager::GetKnownPath(ZEPathManager::GetWorkingDirectory() + Seperator + "Test");
			ZE_TEST_CHECK_EQUAL(KnownPath, ZE_KP_WORKING_DIRECTORY);
		}
	}

	ZE_TEST_ITEM("const ZEString& ZEPathManager::GetKnownPath(const ZEKnownPath KnownPath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZE_TEST_CASE("none")
		{
			const ZEString KnownPath = ZEPathManager::GetKnownPath(ZE_KP_NONE);
		}

		ZE_TEST_CASE("working directory")
		{
			const ZEString KnownPath = ZEPathManager::GetKnownPath(ZE_KP_WORKING_DIRECTORY);
			ZE_TEST_CHECK_STRING_EQUAL(KnownPath, ZEPathManager::GetWorkingDirectory());
		}

		ZE_TEST_CASE("resources")
		{
			const ZEString KnownPath = ZEPathManager::GetKnownPath(ZE_KP_RESOURCES);
			ZE_TEST_CHECK_STRING_EQUAL(KnownPath, ZEPathManager::GetResourcesPath());
			ZE_TEST_CHECK_STRING_EQUAL(KnownPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources", false));
		}

		ZE_TEST_CASE("application resources")
		{
			const ZEString KnownPath = ZEPathManager::GetKnownPath(ZE_KP_APP_RESOURCES);
			ZE_TEST_CHECK_STRING_EQUAL(KnownPath, ZEPathManager::GetApplicationResourcesPath());
			ZE_TEST_CHECK_STRING_EQUAL(KnownPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources" + Seperator + "Engine", false));
		}

		ZE_TEST_CASE("user data")
		{
			const ZEString KnownPath = ZEPathManager::GetKnownPath(ZE_KP_USER_DATA);
			ZE_TEST_CHECK_STRING_EQUAL(KnownPath, ZEPathManager::GetUserDataPath());
			ZE_TEST_CHECK_STRING_EQUAL(KnownPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "AppData" + Seperator + "Local" + Seperator + "Zinek" + Seperator + "Engine", false));
		}

		ZE_TEST_CASE("system data")
		{
			const ZEString KnownPath = ZEPathManager::GetKnownPath(ZE_KP_SYSTEM_DATA);
			ZE_TEST_CHECK_STRING_EQUAL(KnownPath, ZEPathManager::GetSystemDataPath());
			ZE_TEST_CHECK_STRING_EQUAL(KnownPath, "C:\\ProgramData\\Zinek\\Engine");
		}

		ZE_TEST_CASE("saved games")
		{
			const ZEString KnownPath = ZEPathManager::GetKnownPath(ZE_KP_SAVED_GAMES);
			ZE_TEST_CHECK_STRING_EQUAL(KnownPath, ZEPathManager::GetSavedGamesPath());
			ZE_TEST_CHECK_STRING_EQUAL(KnownPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "Saved Games" + Seperator + "Zinek" + Seperator + "Engine", false));
		}
	}

	ZE_TEST_ITEM("ZEString ZEPathManager::GetFinalPath(const ZEString& Path, ZEKnownPath* Root)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();
		ZEKnownPath Root;
		bool Enabled;

		ZE_TEST_CASE("with a relative path when path restriction is false, and the known path is null")
		{
			ZEPathManager::SetEnablePathRestriction(false);
			const ZEString FinalPath = ZEPathManager::GetFinalPath("DirName.txt", NULL);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, "DirName.txt");
		}

		ZE_TEST_CASE("with a relative path when path restriction is true")
		{
			ZEPathManager::SetEnablePathRestriction(true);
			const ZEString FinalPath = ZEPathManager::GetFinalPath("DirName.txt", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetWorkingDirectory() + Seperator + "DirName.txt");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_WORKING_DIRECTORY);
		}

		ZE_TEST_CASE("with a relative path when path restriction is false");
		{
			ZEPathManager::SetEnablePathRestriction(false);
			const ZEString FinalPath = ZEPathManager::GetFinalPath("DirName.txt", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, "DirName.txt");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_NONE);
		}

		ZE_TEST_CASE("with an absolute path when path restriction is true")
		{
			ZEPathManager::SetEnablePathRestriction(true);
			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetWorkingDirectory() + Seperator + "Test", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetWorkingDirectory() + Seperator + "Test");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_WORKING_DIRECTORY);
		}

		ZE_TEST_CASE("with an absolute path when path restriction is false")
		{
			ZEPathManager::SetEnablePathRestriction(false);
			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetWorkingDirectory() + Seperator + "Test", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetWorkingDirectory() + Seperator + "Test");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_NONE);
		}
			
		ZE_TEST_CASE("set EnabledPathRestriction false, then call function with a relative path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("TestResources"+ Seperator + "resources" +Seperator + "DirName.txt", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath , "TestResources"+ Seperator + "resources" +Seperator + "DirName.txt");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_NONE);
		}

		ZE_TEST_CASE("set EnabledPathRestriction true, then call function with a relative path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("TestResources"+ Seperator + "resources" +Seperator + "DirName.txt", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath ,ZEPathManager::GetWorkingDirectory() + Seperator + "TestResources"+ Seperator + "resources" +Seperator + "DirName.txt");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_WORKING_DIRECTORY);
		}

		ZE_TEST_CASE("set EnabledPathRestriction false, then call function with a relative path with a symbol of resources path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("#" + Seperator + "DirName.txt", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources" + Seperator + "DirName.txt", false));
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_NONE);
		}

		ZE_TEST_CASE("set EnabledPathRestriction true, then call function with a relative path with a symbol of resources path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("#" +Seperator +"DirName.txt", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_RESOURCES);
		}

		ZE_TEST_CASE("call function with resources path and a file at the end of path")
		{
			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetResourcesPath() + Seperator + "Test.txt", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetResourcesPath() + Seperator + "Test.txt");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_RESOURCES);
		}

		ZE_TEST_CASE("call function with a relative path with a symbol of application resources path")
		{
			const ZEString FinalPath = ZEPathManager::GetFinalPath("~", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetApplicationResourcesPath());
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_APP_RESOURCES);
		}

		ZE_TEST_CASE("set EnabledPathRestriction false, then call function with a relative path with a symbol of application resources path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("~" + Seperator + "Folder", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetApplicationResourcesPath() + Seperator + "Folder");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_NONE);
		}

		ZE_TEST_CASE("set EnabledPathRestriction true, then call function with a relative path with a symbol of application resources path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("~" + Seperator + "Folder", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetApplicationResourcesPath() + Seperator + "Folder");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_APP_RESOURCES);
		}

		ZE_TEST_CASE("set EnabledPathRestriction false, then call function with application resources path and a folder at the end of the path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetApplicationResourcesPath() + Seperator + "Folder", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetApplicationResourcesPath() + Seperator + "Folder");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_NONE);
		}

		ZE_TEST_CASE("set EnabledPathRestriction true, then call function with application resources path and a folder at the end of the path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetApplicationResourcesPath() + Seperator + "Folder", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetApplicationResourcesPath() + Seperator + "Folder");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_APP_RESOURCES);
		}

		ZE_TEST_CASE("call function with a symbol of user data path when path restriction is enabled")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("$", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetUserDataPath());
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_USER_DATA);
		}

		ZE_TEST_CASE("call function with a symbol of user data path when path restriction is not enabled")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("$", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetUserDataPath());
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_NONE);
		}

		ZE_TEST_CASE("set EnabledPathRestriction true, then call function with a relative path with a symbol of user data path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("$" + Seperator + "Folder", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetUserDataPath() + Seperator + "Folder");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_USER_DATA);
		}

		ZE_TEST_CASE("set EnabledPathRestriction false, then call function with a relative path with a symbol of user data path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("$" + Seperator + "Folder", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetUserDataPath() + Seperator + "Folder");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_NONE);
		}

		ZE_TEST_CASE("set EnabledPathRestriction true, then call function with user data path and a folder at the end of the path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetUserDataPath() + Seperator + "Folder", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetUserDataPath() + Seperator + "Folder");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_USER_DATA);
		}

		ZE_TEST_CASE("set EnabledPathRestriction false, then call function with user data path and a folder at the end of the path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetUserDataPath() + Seperator + "Folder", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetUserDataPath() + Seperator + "Folder");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_NONE);
		}

		ZE_TEST_CASE("call function with a symbol of system data path when path restriction is enabled")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("&", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetSystemDataPath());
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_SYSTEM_DATA);
		}

		ZE_TEST_CASE("call function with a symbol of system data path when path restriction is not enabled")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("&", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetSystemDataPath());
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_NONE);
		}

		ZE_TEST_CASE("set EnabledPathRestriction true, then call function with a relative path with a symbol of system data path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("&" + Seperator + "Folder", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetSystemDataPath() + Seperator + "Folder");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_SYSTEM_DATA);
		}

		ZE_TEST_CASE("set EnabledPathRestriction false, then call function with a relative path with a symbol of system data path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("&" + Seperator + "Folder", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetSystemDataPath() + Seperator + "Folder");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_NONE);
		}

		ZE_TEST_CASE("set EnabledPathRestriction true, then call function with system data path and a folder at the end of the path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetSystemDataPath() + Seperator + "Folder", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetSystemDataPath() + Seperator + "Folder");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_SYSTEM_DATA);
		}

		ZE_TEST_CASE("set EnabledPathRestriction false, then call function with system data path and a folder at the end of the path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetSystemDataPath() + Seperator + "Folder", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetSystemDataPath() + Seperator + "Folder");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_NONE);
		}

		ZE_TEST_CASE("call function with a symbol of saved games path when path restriction is enabled")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("|", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetSavedGamesPath());
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_SAVED_GAMES);
		}

		ZE_TEST_CASE("call function with a symbol of saved games path when path restriction is not enabled")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("|", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetSavedGamesPath());
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_NONE);
		}

		ZE_TEST_CASE("set EnabledPathRestriction true, then call function with a relative path with a symbol of saved games path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("|" + Seperator + "Folder", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetSavedGamesPath() + Seperator + "Folder");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_SAVED_GAMES);
		}

		ZE_TEST_CASE("set EnabledPathRestriction false, then call function with a relative path with a symbol of saved games path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("|" + Seperator + "Folder", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetSavedGamesPath() + Seperator + "Folder");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_NONE);
		}

		ZE_TEST_CASE("set EnabledPathRestriction true, then call function with saved games path and a folder at the end of the path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetSavedGamesPath() + Seperator + "Folder", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetSavedGamesPath() + Seperator + "Folder");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_SAVED_GAMES);
		}

		ZE_TEST_CASE("set EnabledPathRestriction false, then call function with saved games path and a folder at the end of the path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetSavedGamesPath() + Seperator + "Folder", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetSavedGamesPath() + Seperator + "Folder");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_NONE);
		}

		ZE_TEST_CASE("set EnabledPathRestriction true, then call function with a relative path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("TestResources" + Seperator + "Saved Games" + Seperator + "Zinek" + Seperator + "Engine", &Root);
			ZE_TEST_CHECK_STRING_EQUAL(FinalPath, ZEPathManager::GetWorkingDirectory() + Seperator + "TestResources" + Seperator + "Saved Games" + Seperator + "Zinek" + Seperator + "Engine");
			ZE_TEST_CHECK_EQUAL(Root, ZE_KP_WORKING_DIRECTORY);
		}
	}
}
