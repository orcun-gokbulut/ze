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


ZETestSuite(ZEPathManager)
{
	ZETest("void ZEPathManager::CustomizePaths(const ZEString* CompanyName, const ZEString* ApplicationName, const ZEString* ResourceDirectoryName)")
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

	ZETest("bool ZEPathManager::GetEnablePathRestriction()")
	{
		bool EnabledPathRestriction = ZEPathManager::GetEnablePathRestriction();
		ZETestCheck(!EnabledPathRestriction);
	}

	ZETest("void ZEPathManager::SetEnablePathRestriction(bool Enable)")
	{
		ZEPathManager::SetEnablePathRestriction(true);
		ZETestCheck(ZEPathManager::GetEnablePathRestriction());

		ZEPathManager::SetEnablePathRestriction(false);
		ZETestCheck(!ZEPathManager::GetEnablePathRestriction());
	}

	ZETest("const ZEString& ZEPathManager::GetWorkingDrive()")
	{
 		const ZEString WorkingDrive = ZEPathManager::GetWorkingDrive();
 		ZETestCheckString(WorkingDrive, "C:");
	}

	ZETest("void ZEPathManager::SetUserDataPath(const ZEString& Name)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEPathManager::SetUserDataPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "AppData" + Seperator + "Local" + Seperator + "Zinek" + Seperator + "Engine", false));

		const ZEString UserDataPath = ZEPathManager::GetUserDataPath();
		ZETestCheckString(UserDataPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "AppData" + Seperator + "Local" + Seperator + "Zinek" + Seperator + "Engine", false));
	}

	ZETest("const ZEString&	ZEPathManager::GetUserDataPath()")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		const ZEString UserDataPath = ZEPathManager::GetUserDataPath();
		ZETestCheckString(UserDataPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "AppData" + Seperator + "Local" + Seperator + "Zinek" + Seperator + "Engine", false));
	}

	ZETest("void ZEPathManager::SetResourcesPath(const ZEString& Name)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEPathManager::SetResourcesPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources", false));

		const ZEString ResourcesPath = ZEPathManager::GetResourcesPath();
		ZETestCheckString(ResourcesPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources", false));
	}

	ZETest("const ZEString&	ZEPathManager::GetResourcesPath()")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		const ZEString ResourcesPath = ZEPathManager::GetResourcesPath();
		ZETestCheckString(ResourcesPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources", false));
	}

	ZETest("void ZEPathManager::SetSystemDataPath(const ZEString& Name)")
	{
		ZEPathManager::SetSystemDataPath("C:\\ProgramData\\Zinek\\Engine");

		const ZEString SystemDataPath = ZEPathManager::GetSystemDataPath();
		ZETestCheckString(SystemDataPath, "C:\\ProgramData\\Zinek\\Engine");
	}

	ZETest("const ZEString&	ZEPathManager::GetSystemDataPath()")
	{
		const ZEString SystemDataPath = ZEPathManager::GetSystemDataPath();
		ZETestCheckString(SystemDataPath, "C:\\ProgramData\\Zinek\\Engine");
	}

	ZETest("void ZEPathManager::SetSavedGamesPath(const ZEString& Name)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEPathManager::SetSavedGamesPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "Saved Games" + Seperator + "Zinek" + Seperator + "Engine", false));

		const ZEString SavedGamesPath = ZEPathManager::GetSavedGamesPath();
		ZETestCheckString(SavedGamesPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "Saved Games" + Seperator + "Zinek" + Seperator + "Engine", false));
	}

	ZETest("const ZEString&	ZEPathManager::GetSavedGamesPath()")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		const ZEString SavedGamesPath = ZEPathManager::GetSavedGamesPath();
		ZETestCheckString(SavedGamesPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "Saved Games" + Seperator + "Zinek" + Seperator + "Engine", false));
	}

	ZETest("const ZEString& ZEPathManager::GetWorkingDirectory()")
	{
		const ZEString WorkingDirectory = ZEPathManager::GetWorkingDirectory();
		ZETestCheckString(WorkingDirectory, ZEPathManager::GetWorkingDirectory());
	}

	ZETest("void ZEPathManager::SetApplicationResourcesPath(const ZEString& Name)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEPathManager::SetApplicationResourcesPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources" + Seperator + "Engine", false));

		const ZEString ApplicationResourcesPath = ZEPathManager::GetApplicationResourcesPath();
		ZETestCheckString(ApplicationResourcesPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources" + Seperator + "Engine", false));
	}

	ZETest("const ZEString&	ZEPathManager::GetApplicationResourcesPath()")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		const ZEString ApplicationResourcesPath = ZEPathManager::GetApplicationResourcesPath();
		ZETestCheckString(ApplicationResourcesPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources" + Seperator + "Engine", false));
	}

	ZETest("ZEKnownPath ZEPathManager::GetKnownPath(const ZEString& AbsolutePath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();
		ZEKnownPath KnownPath;

		ZETestCase("none")
		{
			KnownPath = ZEPathManager::GetKnownPath("Known.txt");
			ZETestCheckEqual(KnownPath, ZE_KP_NONE);
		}

		ZETestCase("application resources")
		{
			KnownPath = ZEPathManager::GetKnownPath(ZEPathManager::GetApplicationResourcesPath());
			ZETestCheckEqual(KnownPath, ZE_KP_APP_RESOURCES);

			KnownPath = ZEPathManager::GetKnownPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources" + Seperator + "Engine", false));
			ZETestCheckEqual(KnownPath, ZE_KP_APP_RESOURCES);

			KnownPath = ZEPathManager::GetKnownPath(ZEPathManager::GetApplicationResourcesPath() + Seperator + "Test");
			ZETestCheckEqual(KnownPath, ZE_KP_APP_RESOURCES);
		}

		ZETestCase("resources")
		{
			KnownPath = ZEPathManager::GetKnownPath(ZEPathManager::GetResourcesPath());
			ZETestCheckEqual(KnownPath, ZE_KP_RESOURCES);

			KnownPath = ZEPathManager::GetKnownPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources", false));
			ZETestCheckEqual(KnownPath, ZE_KP_RESOURCES);

			KnownPath = ZEPathManager::GetKnownPath(ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
			ZETestCheckEqual(KnownPath, ZE_KP_RESOURCES);
		}

		ZETestCase("user data")
		{
			KnownPath = ZEPathManager::GetKnownPath(ZEPathManager::GetUserDataPath());
			ZETestCheckEqual(KnownPath, ZE_KP_USER_DATA);

			KnownPath = ZEPathManager::GetKnownPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "AppData" + Seperator + "Local" + Seperator + "Zinek" + Seperator + "Engine", false));
			ZETestCheckEqual(KnownPath, ZE_KP_USER_DATA);
		}

		ZETestCase("system data")
		{
			KnownPath = ZEPathManager::GetKnownPath(ZEPathManager::GetSystemDataPath());
			ZETestCheckEqual(KnownPath, ZE_KP_SYSTEM_DATA);

			KnownPath = ZEPathManager::GetKnownPath("C:\\ProgramData\\Zinek\\Engine");
			ZETestCheckEqual(KnownPath, ZE_KP_SYSTEM_DATA);
		}

		ZETestCase("saved games")
		{
			KnownPath = ZEPathManager::GetKnownPath(ZEPathManager::GetSavedGamesPath());
			ZETestCheckEqual(KnownPath, ZE_KP_SAVED_GAMES);

			KnownPath = ZEPathManager::GetKnownPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "Saved Games" + Seperator + "Zinek" + Seperator + "Engine", false));
			ZETestCheckEqual(KnownPath, ZE_KP_SAVED_GAMES);
		}

		ZETestCase("working directory")
		{
			KnownPath = ZEPathManager::GetKnownPath(ZEPathManager::GetWorkingDirectory());
			ZETestCheckEqual(KnownPath, ZE_KP_WORKING_DIRECTORY);

			KnownPath = ZEPathManager::GetKnownPath(ZEPathManager::GetWorkingDirectory() + Seperator + "Test");
			ZETestCheckEqual(KnownPath, ZE_KP_WORKING_DIRECTORY);
		}
	}

	ZETest("const ZEString& ZEPathManager::GetKnownPath(const ZEKnownPath KnownPath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZETestCase("none")
		{
			const ZEString KnownPath = ZEPathManager::GetKnownPath(ZE_KP_NONE);
		}

		ZETestCase("working directory")
		{
			const ZEString KnownPath = ZEPathManager::GetKnownPath(ZE_KP_WORKING_DIRECTORY);
			ZETestCheckString(KnownPath, ZEPathManager::GetWorkingDirectory());
		}

		ZETestCase("resources")
		{
			const ZEString KnownPath = ZEPathManager::GetKnownPath(ZE_KP_RESOURCES);
			ZETestCheckString(KnownPath, ZEPathManager::GetResourcesPath());
			ZETestCheckString(KnownPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources", false));
		}

		ZETestCase("application resources")
		{
			const ZEString KnownPath = ZEPathManager::GetKnownPath(ZE_KP_APP_RESOURCES);
			ZETestCheckString(KnownPath, ZEPathManager::GetApplicationResourcesPath());
			ZETestCheckString(KnownPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources" + Seperator + "Engine", false));
		}

		ZETestCase("user data")
		{
			const ZEString KnownPath = ZEPathManager::GetKnownPath(ZE_KP_USER_DATA);
			ZETestCheckString(KnownPath, ZEPathManager::GetUserDataPath());
			ZETestCheckString(KnownPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "AppData" + Seperator + "Local" + Seperator + "Zinek" + Seperator + "Engine", false));
		}

		ZETestCase("system data")
		{
			const ZEString KnownPath = ZEPathManager::GetKnownPath(ZE_KP_SYSTEM_DATA);
			ZETestCheckString(KnownPath, ZEPathManager::GetSystemDataPath());
			ZETestCheckString(KnownPath, "C:\\ProgramData\\Zinek\\Engine");
		}

		ZETestCase("saved games")
		{
			const ZEString KnownPath = ZEPathManager::GetKnownPath(ZE_KP_SAVED_GAMES);
			ZETestCheckString(KnownPath, ZEPathManager::GetSavedGamesPath());
			ZETestCheckString(KnownPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "Saved Games" + Seperator + "Zinek" + Seperator + "Engine", false));
		}
	}

	ZETest("ZEString ZEPathManager::GetFinalPath(const ZEString& Path, ZEKnownPath* Root)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();
		ZEKnownPath Root;
		bool Enabled;

		ZETestCase("with a relative path when path restriction is false, and the known path is null")
		{
			ZEPathManager::SetEnablePathRestriction(false);
			const ZEString FinalPath = ZEPathManager::GetFinalPath("DirName.txt", NULL);
			ZETestCheckString(FinalPath, "DirName.txt");
		}

		ZETestCase("with a relative path when path restriction is true")
		{
			ZEPathManager::SetEnablePathRestriction(true);
			const ZEString FinalPath = ZEPathManager::GetFinalPath("DirName.txt", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetWorkingDirectory() + Seperator + "DirName.txt");
			ZETestCheckEqual(Root, ZE_KP_WORKING_DIRECTORY);
		}

		ZETestCase("with a relative path when path restriction is false");
		{
			ZEPathManager::SetEnablePathRestriction(false);
			const ZEString FinalPath = ZEPathManager::GetFinalPath("DirName.txt", &Root);
			ZETestCheckString(FinalPath, "DirName.txt");
			ZETestCheckEqual(Root, ZE_KP_NONE);
		}

		ZETestCase("with an absolute path when path restriction is true")
		{
			ZEPathManager::SetEnablePathRestriction(true);
			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetWorkingDirectory() + Seperator + "Test", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetWorkingDirectory() + Seperator + "Test");
			ZETestCheckEqual(Root, ZE_KP_WORKING_DIRECTORY);
		}

		ZETestCase("with an absolute path when path restriction is false")
		{
			ZEPathManager::SetEnablePathRestriction(false);
			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetWorkingDirectory() + Seperator + "Test", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetWorkingDirectory() + Seperator + "Test");
			ZETestCheckEqual(Root, ZE_KP_NONE);
		}
			
		ZETestCase("set EnabledPathRestriction false, then call function with a relative path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("TestResources"+ Seperator + "resources" +Seperator + "DirName.txt", &Root);
			ZETestCheckString(FinalPath , "TestResources"+ Seperator + "resources" +Seperator + "DirName.txt");
			ZETestCheckEqual(Root, ZE_KP_NONE);
		}

		ZETestCase("set EnabledPathRestriction true, then call function with a relative path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("TestResources"+ Seperator + "resources" +Seperator + "DirName.txt", &Root);
			ZETestCheckString(FinalPath ,ZEPathManager::GetWorkingDirectory() + Seperator + "TestResources"+ Seperator + "resources" +Seperator + "DirName.txt");
			ZETestCheckEqual(Root, ZE_KP_WORKING_DIRECTORY);
		}

		ZETestCase("set EnabledPathRestriction false, then call function with a relative path with a symbol of resources path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("#" + Seperator + "DirName.txt", &Root);
			ZETestCheckString(FinalPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources" + Seperator + "DirName.txt", false));
			ZETestCheckEqual(Root, ZE_KP_NONE);
		}

		ZETestCase("set EnabledPathRestriction true, then call function with a relative path with a symbol of resources path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("#" +Seperator +"DirName.txt", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
			ZETestCheckEqual(Root, ZE_KP_RESOURCES);
		}

		ZETestCase("call function with resources path and a file at the end of path")
		{
			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetResourcesPath() + Seperator + "Test.txt", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetResourcesPath() + Seperator + "Test.txt");
			ZETestCheckEqual(Root, ZE_KP_RESOURCES);
		}

		ZETestCase("call function with a relative path with a symbol of application resources path")
		{
			const ZEString FinalPath = ZEPathManager::GetFinalPath("~", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetApplicationResourcesPath());
			ZETestCheckEqual(Root, ZE_KP_APP_RESOURCES);
		}

		ZETestCase("set EnabledPathRestriction false, then call function with a relative path with a symbol of application resources path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("~" + Seperator + "Folder", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetApplicationResourcesPath() + Seperator + "Folder");
			ZETestCheckEqual(Root, ZE_KP_NONE);
		}

		ZETestCase("set EnabledPathRestriction true, then call function with a relative path with a symbol of application resources path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("~" + Seperator + "Folder", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetApplicationResourcesPath() + Seperator + "Folder");
			ZETestCheckEqual(Root, ZE_KP_APP_RESOURCES);
		}

		ZETestCase("set EnabledPathRestriction false, then call function with application resources path and a folder at the end of the path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetApplicationResourcesPath() + Seperator + "Folder", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetApplicationResourcesPath() + Seperator + "Folder");
			ZETestCheckEqual(Root, ZE_KP_NONE);
		}

		ZETestCase("set EnabledPathRestriction true, then call function with application resources path and a folder at the end of the path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetApplicationResourcesPath() + Seperator + "Folder", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetApplicationResourcesPath() + Seperator + "Folder");
			ZETestCheckEqual(Root, ZE_KP_APP_RESOURCES);
		}

		ZETestCase("call function with a symbol of user data path when path restriction is enabled")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("$", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetUserDataPath());
			ZETestCheckEqual(Root, ZE_KP_USER_DATA);
		}

		ZETestCase("call function with a symbol of user data path when path restriction is not enabled")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("$", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetUserDataPath());
			ZETestCheckEqual(Root, ZE_KP_NONE);
		}

		ZETestCase("set EnabledPathRestriction true, then call function with a relative path with a symbol of user data path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("$" + Seperator + "Folder", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetUserDataPath() + Seperator + "Folder");
			ZETestCheckEqual(Root, ZE_KP_USER_DATA);
		}

		ZETestCase("set EnabledPathRestriction false, then call function with a relative path with a symbol of user data path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("$" + Seperator + "Folder", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetUserDataPath() + Seperator + "Folder");
			ZETestCheckEqual(Root, ZE_KP_NONE);
		}

		ZETestCase("set EnabledPathRestriction true, then call function with user data path and a folder at the end of the path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetUserDataPath() + Seperator + "Folder", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetUserDataPath() + Seperator + "Folder");
			ZETestCheckEqual(Root, ZE_KP_USER_DATA);
		}

		ZETestCase("set EnabledPathRestriction false, then call function with user data path and a folder at the end of the path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetUserDataPath() + Seperator + "Folder", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetUserDataPath() + Seperator + "Folder");
			ZETestCheckEqual(Root, ZE_KP_NONE);
		}

		ZETestCase("call function with a symbol of system data path when path restriction is enabled")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("&", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetSystemDataPath());
			ZETestCheckEqual(Root, ZE_KP_SYSTEM_DATA);
		}

		ZETestCase("call function with a symbol of system data path when path restriction is not enabled")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("&", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetSystemDataPath());
			ZETestCheckEqual(Root, ZE_KP_NONE);
		}

		ZETestCase("set EnabledPathRestriction true, then call function with a relative path with a symbol of system data path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("&" + Seperator + "Folder", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetSystemDataPath() + Seperator + "Folder");
			ZETestCheckEqual(Root, ZE_KP_SYSTEM_DATA);
		}

		ZETestCase("set EnabledPathRestriction false, then call function with a relative path with a symbol of system data path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("&" + Seperator + "Folder", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetSystemDataPath() + Seperator + "Folder");
			ZETestCheckEqual(Root, ZE_KP_NONE);
		}

		ZETestCase("set EnabledPathRestriction true, then call function with system data path and a folder at the end of the path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetSystemDataPath() + Seperator + "Folder", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetSystemDataPath() + Seperator + "Folder");
			ZETestCheckEqual(Root, ZE_KP_SYSTEM_DATA);
		}

		ZETestCase("set EnabledPathRestriction false, then call function with system data path and a folder at the end of the path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetSystemDataPath() + Seperator + "Folder", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetSystemDataPath() + Seperator + "Folder");
			ZETestCheckEqual(Root, ZE_KP_NONE);
		}

		ZETestCase("call function with a symbol of saved games path when path restriction is enabled")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("|", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetSavedGamesPath());
			ZETestCheckEqual(Root, ZE_KP_SAVED_GAMES);
		}

		ZETestCase("call function with a symbol of saved games path when path restriction is not enabled")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("|", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetSavedGamesPath());
			ZETestCheckEqual(Root, ZE_KP_NONE);
		}

		ZETestCase("set EnabledPathRestriction true, then call function with a relative path with a symbol of saved games path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("|" + Seperator + "Folder", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetSavedGamesPath() + Seperator + "Folder");
			ZETestCheckEqual(Root, ZE_KP_SAVED_GAMES);
		}

		ZETestCase("set EnabledPathRestriction false, then call function with a relative path with a symbol of saved games path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("|" + Seperator + "Folder", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetSavedGamesPath() + Seperator + "Folder");
			ZETestCheckEqual(Root, ZE_KP_NONE);
		}

		ZETestCase("set EnabledPathRestriction true, then call function with saved games path and a folder at the end of the path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetSavedGamesPath() + Seperator + "Folder", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetSavedGamesPath() + Seperator + "Folder");
			ZETestCheckEqual(Root, ZE_KP_SAVED_GAMES);
		}

		ZETestCase("set EnabledPathRestriction false, then call function with saved games path and a folder at the end of the path")
		{
			ZEPathManager::SetEnablePathRestriction(false);

			const ZEString FinalPath = ZEPathManager::GetFinalPath(ZEPathManager::GetSavedGamesPath() + Seperator + "Folder", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetSavedGamesPath() + Seperator + "Folder");
			ZETestCheckEqual(Root, ZE_KP_NONE);
		}

		ZETestCase("set EnabledPathRestriction true, then call function with a relative path")
		{
			ZEPathManager::SetEnablePathRestriction(true);

			const ZEString FinalPath = ZEPathManager::GetFinalPath("TestResources" + Seperator + "Saved Games" + Seperator + "Zinek" + Seperator + "Engine", &Root);
			ZETestCheckString(FinalPath, ZEPathManager::GetWorkingDirectory() + Seperator + "TestResources" + Seperator + "Saved Games" + Seperator + "Zinek" + Seperator + "Engine");
			ZETestCheckEqual(Root, ZE_KP_WORKING_DIRECTORY);
		}
	}
}
