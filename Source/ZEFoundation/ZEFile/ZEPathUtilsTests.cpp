//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPathUtilsTests.cpp
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
#include "ZEFile/ZEPathUtils.h"
#include "ZEFile/ZEFile.h"
#include "ZEDS/ZEString.h"
#include <stdio.h>
#include "ZEError.h"

ZETestSuite(ZEPathUtils)
{
	ZETest("const ZEString& ZEPathUtils::GetDot()")
	{

	}

	ZETest("const ZEString& ZEPathUtils::GetDotDot()")
	{

	}

	ZETest("const ZEString& ZEPathUtils::GetSeperator()")
	{

	}

	ZETest("const ZEString& ZEPathUtils::GetEmptyPath()")
	{

	}

	ZETest("bool ZEPathUtils::IsRelativePath(const ZEString& Path)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		bool Relative = ZEPathUtils::IsRelativePath("Path.txt");
		ZETestCheck(Relative);

		Relative = ZEPathUtils::IsRelativePath("RunDir\\Path.txt");
		ZETestCheck(Relative);

		Relative = ZEPathUtils::IsRelativePath(ZEPathManager::GetResourcesPath());
		ZETestCheck(!Relative);

		Relative = ZEPathUtils::IsRelativePath(ZEPathManager::GetResourcesPath() + Seperator + "Path.txt");
		ZETestCheck(!Relative);

		Relative = ZEPathUtils::IsRelativePath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + "Path.txt",false));
		ZETestCheck(Relative);

		Relative = ZEPathUtils::IsRelativePath(ZEPathManager::GetWorkingDirectory());
		ZETestCheck(!Relative);
	}

	ZETest("ZEString ZEPathUtils::GetRelativePath(const ZEString& RootPath, const ZEString& AbsolutePath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEString RelativePath = ZEPathUtils::GetRelativePath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false), ZEPathManager::GetResourcesPath());
		ZETestCheckString(RelativePath, Seperator + "TestResources"+ Seperator + "RunDir" + Seperator + "resources");

		RelativePath = ZEPathUtils::GetRelativePath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false), ZEPathManager::GetApplicationResourcesPath());
		ZETestCheckString(RelativePath, Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources" + Seperator + "Engine");

		RelativePath = ZEPathUtils::GetRelativePath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false), ZEPathManager::GetSavedGamesPath());
		ZETestCheckString(RelativePath, Seperator + "TestResources" + Seperator + "Saved Games" + Seperator + "Zinek" + Seperator + "Engine");

		RelativePath = ZEPathUtils::GetRelativePath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false), ZEPathManager::GetUserDataPath());
		ZETestCheckString(RelativePath, Seperator + "TestResources" + Seperator + "AppData" + Seperator + "Local" + Seperator + "Zinek" + Seperator + "Engine");

		RelativePath = ZEPathUtils::GetRelativePath(ZEPathManager::GetResourcesPath(), ZEPathManager::GetResourcesPath());
		//EmptyPath

		RelativePath = ZEPathUtils::GetRelativePath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false), ZEPathManager::GetResourcesPath() + Seperator + "Path.txt");
		ZETestCheckString(RelativePath, Seperator + "TestResources"+ Seperator + "RunDir" + Seperator + "resources" + Seperator + "Path.txt");

		RelativePath = ZEPathUtils::GetRelativePath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false), ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetResourcesPath() + Seperator + ".." + Seperator + "..", false));
		ZETestCheckString(RelativePath, Seperator + "TestResources");

		RelativePath = ZEPathUtils::GetRelativePath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false), ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetSavedGamesPath() + Seperator + ".." + Seperator + "..", false));
		ZETestCheckString(RelativePath, Seperator + "TestResources" + Seperator + "Saved Games");
	}

	ZETest("bool ZEPathUtils::IsAbsolutePath(const ZEString& Path)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		bool Absolute = ZEPathUtils::IsAbsolutePath("Path.txt");
		ZETestCheck(!Absolute);

		Absolute = ZEPathUtils::IsAbsolutePath("Folder");
		ZETestCheck(!Absolute);

		Absolute = ZEPathUtils::IsAbsolutePath(ZEPathManager::GetWorkingDrive() + Seperator + "Folder");
		ZETestCheck(Absolute);

		Absolute = ZEPathUtils::IsAbsolutePath(ZEPathManager::GetWorkingDirectory());
		ZETestCheck(Absolute);

		Absolute = ZEPathUtils::IsAbsolutePath(ZEPathManager::GetWorkingDirectory() + Seperator + "Path.txt");
		ZETestCheck(Absolute);

		Absolute = ZEPathUtils::IsAbsolutePath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + "..", false));
		ZETestCheck(Absolute);

		Absolute = ZEPathUtils::IsAbsolutePath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + "..", false));
		ZETestCheck(!Absolute);
	}

	ZETest("ZEString ZEPathUtils::GetAbsolutePath(const ZEString& RootPath, const ZEString& RelativePath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();
		bool Enabled = ZEPathManager::GetEnablePathRestriction();
		ZETestCheck(Enabled);

		ZEString AbsolutePath = ZEPathUtils::GetAbsolutePath(ZEPathManager::GetWorkingDrive(), Seperator + "Path.txt");
		ZETestCheckString(AbsolutePath, ZEPathManager::GetWorkingDrive() + Seperator + "Path.txt");

		AbsolutePath = ZEPathUtils::GetAbsolutePath(ZEPathManager::GetWorkingDirectory(), Seperator + "Folder");
		ZETestCheckString(AbsolutePath, ZEPathManager::GetWorkingDirectory() + Seperator + "Folder");

		AbsolutePath = ZEPathUtils::GetAbsolutePath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false), Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources");
		ZETestCheckString(AbsolutePath, ZEPathManager::GetResourcesPath());

		AbsolutePath = ZEPathUtils::GetAbsolutePath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false), Seperator + "TestResources" + Seperator + "AppData" + Seperator + "Local" + Seperator + "Zinek" + Seperator + "Engine");
		ZETestCheckString(AbsolutePath, ZEPathManager::GetUserDataPath());

		AbsolutePath = ZEPathUtils::GetAbsolutePath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false), Seperator + "TestResources" + Seperator + "Saved Games" + Seperator + "Zinek" + Seperator + "Engine");
		ZETestCheckString(AbsolutePath, ZEPathManager::GetSavedGamesPath());

		AbsolutePath = ZEPathUtils::GetAbsolutePath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false), Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources" + Seperator + "Engine");
		ZETestCheckString(AbsolutePath, ZEPathManager::GetApplicationResourcesPath());
	}

	ZETest("bool ZEPathUtils::CheckPathContainsRoot(const ZEString& RootPath, const ZEString& AbsolutePath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		bool PathRestriction = ZEPathUtils::CheckPathContainsRoot(ZEPathManager::GetWorkingDrive(), ZEPathManager::GetWorkingDirectory());
		ZETestCheck(PathRestriction);

		PathRestriction = ZEPathUtils::CheckPathContainsRoot(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false), ZEPathManager::GetResourcesPath());
		ZETestCheck(PathRestriction);

		PathRestriction = ZEPathUtils::CheckPathContainsRoot(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false), ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + "Path.txt", false));
		ZETestCheck(!PathRestriction);

		PathRestriction = ZEPathUtils::CheckPathContainsRoot(ZEPathManager::GetApplicationResourcesPath(), ZEPathManager::GetApplicationResourcesPath() + Seperator + "Folder");
		ZETestCheck(PathRestriction);

		PathRestriction = ZEPathUtils::CheckPathContainsRoot(ZEPathManager::GetApplicationResourcesPath(), ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetApplicationResourcesPath() + Seperator + ".." + Seperator + "..", false));
		ZETestCheck(!PathRestriction);
	}

	ZETest("ZEString ZEPathUtils::GetSimplifiedPath(const ZEString& Path, bool StackDotDot)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEString SimplifiedPath = ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory(), true);
		ZETestCheckString(SimplifiedPath, ZEPathManager::GetWorkingDirectory());

		SimplifiedPath = ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources", false);
		ZETestCheckString(SimplifiedPath, ZEPathManager::GetResourcesPath());

		SimplifiedPath = ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "AppData" + Seperator + "Local" + Seperator + "Zinek" + Seperator + "Engine", false);
		ZETestCheckString(SimplifiedPath, ZEPathManager::GetUserDataPath());

		SimplifiedPath = ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetSavedGamesPath() + Seperator + ".." + Seperator + "..", false);
		ZETestCheckString(SimplifiedPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false) + Seperator + "TestResources" + Seperator + "Saved Games");

		ZEKnownPath Root;
		SimplifiedPath = ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetFinalPath("~", &Root) + Seperator + ".." , false);
		ZETestCheckString(SimplifiedPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetApplicationResourcesPath() + Seperator + "..", false));

		SimplifiedPath = ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetResourcesPath() + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + "RunDir", false);
		ZETestCheckString(SimplifiedPath, ZEPathManager::GetWorkingDirectory());

		SimplifiedPath = ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetResourcesPath() + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + ".." + Seperator + ".." , false);
		ZETestCheckString(SimplifiedPath, ZEPathManager::GetWorkingDrive());

		SimplifiedPath = ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "." + Seperator + "Test", false);
		ZETestCheckString(SimplifiedPath, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false) + Seperator + "Test");

		SimplifiedPath = ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "." + Seperator + "Test", true);
		ZETestCheckString(SimplifiedPath, ZEPathManager::GetWorkingDirectory() + Seperator + "Test");
	}

	ZETest("ZEKnownPath ZEPathUtils::SearchForSymbol(ZEString* RelativePart, const ZEString& SymbolicPath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();
		ZEString RelativePart;
		ZEKnownPath KnownPath;

		ZETestCase("symbolic path with resources path symbol")
		{
			KnownPath = ZEPathUtils::SearchForSymbol(&RelativePart, "#");
			ZETestCheckEqual(KnownPath, ZE_KP_RESOURCES);
			//RelativePart EmptyPath
		}

		ZETestCase("symbolic path with application resources path symbol")
		{
			KnownPath = ZEPathUtils::SearchForSymbol(&RelativePart, "~");
			ZETestCheckEqual(KnownPath, ZE_KP_APP_RESOURCES);
		}

		ZETestCase("symbolic path with user data path symbol")
		{
			KnownPath = ZEPathUtils::SearchForSymbol(&RelativePart, "$");
			ZETestCheckEqual(KnownPath, ZE_KP_USER_DATA);
		}

		ZETestCase("symbolic path with system data path symbol")
		{
			KnownPath = ZEPathUtils::SearchForSymbol(&RelativePart, "&");
			ZETestCheckEqual(KnownPath, ZE_KP_SYSTEM_DATA);
		}

		ZETestCase("symbolic path with saved games path symbol")
		{
			KnownPath = ZEPathUtils::SearchForSymbol(&RelativePart, "|");
			ZETestCheckEqual(KnownPath, ZE_KP_SAVED_GAMES);
		}

		ZETestCase("SymbolicPath with # and without a root")
		{
			KnownPath = ZEPathUtils::SearchForSymbol(&RelativePart, "#" + Seperator + "Test.txt");
			ZETestCheckString(RelativePart, Seperator + "Test.txt");
			ZETestCheckEqual(KnownPath, ZE_KP_RESOURCES);
		}

		ZETestCase("symbolic path with resources path with resources path symbol")
		{
			KnownPath = ZEPathUtils::SearchForSymbol(&RelativePart, "#" + Seperator + ZEPathManager::GetResourcesPath());
			ZETestCheckString(RelativePart, Seperator + ZEPathManager::GetResourcesPath());
			ZETestCheckEqual(KnownPath, ZE_KP_RESOURCES);
		}

		ZETestCase("SymbolicPath with ~ and without a root")
		{
			KnownPath = ZEPathUtils::SearchForSymbol(&RelativePart, "~" + Seperator + "Test.txt");
			ZETestCheckString(RelativePart, Seperator + "Test.txt");
			ZETestCheckEqual(KnownPath, ZE_KP_APP_RESOURCES);
		}

		ZETestCase("symbolic path with application resources path with application resources path symbol")
		{
			KnownPath = ZEPathUtils::SearchForSymbol(&RelativePart, "~" + Seperator + ZEPathManager::GetApplicationResourcesPath());
			ZETestCheckString(RelativePart, Seperator + ZEPathManager::GetApplicationResourcesPath());
			ZETestCheckEqual(KnownPath, ZE_KP_APP_RESOURCES);
		}

		ZETestCase("SymbolicPath with $ and without a root")
		{
			KnownPath = ZEPathUtils::SearchForSymbol(&RelativePart, "$" + Seperator + "Test.txt");
			ZETestCheckString(RelativePart, Seperator + "Test.txt");
			ZETestCheckEqual(KnownPath, ZE_KP_USER_DATA);
		}

		ZETestCase("symbolic path with user data path with user data path symbol")
		{
			KnownPath = ZEPathUtils::SearchForSymbol(&RelativePart, "$" + Seperator + ZEPathManager::GetUserDataPath());
			ZETestCheckString(RelativePart, Seperator + ZEPathManager::GetUserDataPath());
			ZETestCheckEqual(KnownPath, ZE_KP_USER_DATA);
		}

		ZETestCase("SymbolicPath with & and without a root")
		{
			KnownPath = ZEPathUtils::SearchForSymbol(&RelativePart, "&" + Seperator + "Test.txt");
			ZETestCheckString(RelativePart, Seperator + "Test.txt");
			ZETestCheckEqual(KnownPath, ZE_KP_SYSTEM_DATA);
		}

		ZETestCase("symbolic path with system data path with system data path symbol")
		{
			KnownPath = ZEPathUtils::SearchForSymbol(&RelativePart, "&" + Seperator + ZEPathManager::GetSystemDataPath());
			ZETestCheckString(RelativePart, Seperator + ZEPathManager::GetSystemDataPath());
			ZETestCheckEqual(KnownPath, ZE_KP_SYSTEM_DATA);
		}

		ZETestCase("SymbolicPath with | and without a root")
		{
			KnownPath = ZEPathUtils::SearchForSymbol(&RelativePart, "|" + Seperator + "Test.txt");
			ZETestCheckString(RelativePart, Seperator + "Test.txt");
			ZETestCheckEqual(KnownPath, ZE_KP_SAVED_GAMES);
		}

		ZETestCase("symbolic path with saved games path with saved games path symbol")
		{
			KnownPath = ZEPathUtils::SearchForSymbol(&RelativePart, "|" + Seperator + ZEPathManager::GetSavedGamesPath());
			ZETestCheckString(RelativePart, Seperator + ZEPathManager::GetSavedGamesPath());
			ZETestCheckEqual(KnownPath, ZE_KP_SAVED_GAMES);
		}

		ZETestCase("symbolic path without a symbol")
		{
			KnownPath = ZEPathUtils::SearchForSymbol(&RelativePart, ZEPathManager::GetWorkingDirectory());
			ZETestCheckString(RelativePart, Seperator + ZEPathManager::GetWorkingDirectory());
			ZETestCheckEqual(KnownPath, ZE_KP_NONE);
		}

		ZETestCase("symbolic path with a symbol of resources path and application resources path")
		{
			KnownPath = ZEPathUtils::SearchForSymbol(&RelativePart, "#" + Seperator + ZEPathManager::GetApplicationResourcesPath());
			ZETestCheckString(RelativePart, Seperator + ZEPathManager::GetApplicationResourcesPath());
			ZETestCheckEqual(KnownPath, ZE_KP_RESOURCES);
		}
	}	
}
