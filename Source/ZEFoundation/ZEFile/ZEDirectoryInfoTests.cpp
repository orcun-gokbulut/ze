//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDirectoryInfoTests.cpp
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
#include "ZEFile/ZEDirectoryInfo.h"
#include "ZEFile/ZEFile.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEArray.h"
#include <stdio.h>
#include "ZEError.h"
#include "ZEPathUtils.h"

ZETestSuite(ZEDirectoryInfo)
{
	ZETest("set paths")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();
		const ZEString CompanyName;
		const ZEString ApplicationName;
		const ZEString ResourceDirectoryName;

		ZEPathManager::CustomizePaths(&CompanyName, &ApplicationName, &ResourceDirectoryName);
		const ZEString WorkingDrive = ZEPathManager::GetWorkingDrive();
		const ZEString WorkingDirectory = ZEPathManager::GetWorkingDirectory();

		ZEPathManager::SetUserDataPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "AppData" + Seperator + "Local" + Seperator + "Zinek" + Seperator + "Engine", false));
		ZEPathManager::SetResourcesPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources", false));
		ZEPathManager::SetSystemDataPath("C:\\ProgramData\\Zinek\\Engine");
		ZEPathManager::SetSavedGamesPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "Saved Games" + Seperator + "Zinek" + Seperator + "Engine", false));
		ZEPathManager::SetApplicationResourcesPath(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir" + Seperator + "resources" + Seperator + "Engine", false));
	}

	ZETest("ZEDirectoryInfo::~ZEDirectoryInfo()")
	{

	}

	ZETest("ZEDirectoryInfo::ZEDirectoryInfo()")
	{
		ZEDirectoryInfo Directory;
	}

	ZETest("ZEDirectoryInfo::ZEDirectoryInfo(const ZEString& DirectoryPath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEDirectoryInfo Directory(ZEPathManager::GetResourcesPath() + Seperator + "Directory");
		ZETestCheckString(Directory.GetName(), "Directory");
		ZETestCheckString(Directory.GetPath(), ZEPathManager::GetResourcesPath() + Seperator + "Directory");

		ZETestCase("open a file through the specified path")
		{
			ZEFileTime CreationDate;
			ZEFileTime ModificationDate;
			ZEFile File;
			const ZEString FilePath = ZEPathManager::GetResourcesPath() + Seperator + "DirectoryInfo.txt";
			bool Opened = File.Open(FilePath, ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

			ZEDirectoryInfo DirectoryInfo(FilePath);

			ZETestCheckString(DirectoryInfo.GetName(), "DirectoryInfo.txt");
			ZETestCheckString(DirectoryInfo.GetPath(), FilePath);
			bool Created = DirectoryInfo.GetCreationDate(CreationDate);
			ZETestCheck(Created);
			//CreationDate will be the current date
			bool Modified = DirectoryInfo.GetModificationDate(ModificationDate);
			ZETestCheck(Modified);
			//ModificationDate will be the current date

			File.Close();
			remove(FilePath);
		}
	}

	ZETest("void ZEDirectoryInfo::SetPath(const ZEString& DirectoryPath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEDirectoryInfo Directory("resources\\DirName.txt");
		ZETestCheckString(Directory.GetName(), "DirName.txt");
		ZETestCheckString(Directory.GetPath(), "resources\\DirName.txt");

		Directory.SetPath(ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
		ZETestCheckString(Directory.GetName(), "DirName.txt");
		ZETestCheckString(Directory.GetPath(), ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
	}

	ZETest("const ZEString& ZEDirectoryInfo::GetName() const")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEDirectoryInfo Directory1("DirName.txt");
		const ZEString Name1 = Directory1.GetName();
		ZETestCheckString(Name1, "DirName.txt");

		ZEDirectoryInfo Directory2("resources\\DirName.txt");
		const ZEString Name2 = Directory2.GetName();
		ZETestCheckString(Name2, "DirName.txt");

		ZEDirectoryInfo Directory3(ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
		const ZEString Name3 = Directory3.GetName();
		ZETestCheckString(Name3, "DirName.txt");

		ZEDirectoryInfo Directory4(ZEPathManager::GetResourcesPath());
		const ZEString Name4 = Directory4.GetName();
		ZETestCheckString(Name4, "resources");
	}

	ZETest("const ZEString& ZEDirectoryInfo::GetPath() const")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEDirectoryInfo Directory1("DirName.txt");
		const ZEString Path1 = Directory1.GetPath();
		ZETestCheckString(Path1, "DirName.txt");

		ZEDirectoryInfo Directory2("resources\\DirName.txt");
		const ZEString Path2 = Directory2.GetPath();
		ZETestCheckString(Path2, "resources\\DirName.txt");

		ZEDirectoryInfo Directory3(ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
		const ZEString Path3 = Directory3.GetPath();
		ZETestCheckString(Path3, ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");

		ZEDirectoryInfo Directory4(ZEPathManager::GetResourcesPath());
		const ZEString Path4 = Directory4.GetPath();
		ZETestCheckString(Path4, ZEPathManager::GetResourcesPath());
	}

	ZETest("bool ZEDirectoryInfo::GetCreationDate(ZEFileTime& Time)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEFileTime CreationDate;
		ZEDirectoryInfo Directory(ZEPathManager::GetResourcesPath() + Seperator + "DirectoryInfo.txt");

		bool Created = Directory.GetCreationDate(CreationDate);
		ZETestCheck(!Created);
		//CreationDate 0

		ZEFile File;
		bool Opened = File.Open(ZEPathManager::GetResourcesPath() + Seperator +"DirectoryInfo.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

		Created = Directory.GetCreationDate(CreationDate);
		ZETestCheck(Created);
		//CreationDate = Creation

		File.Close();
		remove(ZEPathManager::GetResourcesPath() + Seperator + "DirectoryInfo.txt");
	}

	ZETest("bool ZEDirectoryInfo::GetModificationDate(ZEFileTime& Time)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEFileTime ModificationDate;
		ZEDirectoryInfo Directory(ZEPathManager::GetResourcesPath() + Seperator + "DirectoryInfo.txt");

		bool Modified = Directory.GetModificationDate(ModificationDate);
		ZETestCheck(!Modified);
		//ModificationDate 0

		ZEFile File;
		bool Opened = File.Open(ZEPathManager::GetResourcesPath() + Seperator + "DirectoryInfo.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

		Modified = Directory.GetModificationDate(ModificationDate);
		ZETestCheck(Modified);
		//ModificationDate = Modification

		File.Close();
		remove(ZEPathManager::GetResourcesPath() + Seperator + "DirectoryInfo.txt");
 	}

	ZETest("ZEArray<ZEFileInfo*>* ZEDirectoryInfo::GetFileList()")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEDirectoryInfo Directory1(ZEPathManager::GetResourcesPath());

		ZEArray<ZEFileInfo*>* FileList = Directory1.GetFileList();
		ZETestCheckEqual(FileList->GetCount(), 1);

		ZEDirectoryInfo Directory2(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "\\TestResources", false));

		FileList = Directory2.GetFileList();
		ZETestCheckEqual(FileList->GetCount(), 0);
	}

	ZETest("ZEArray<ZEDirectoryInfo*>* ZEDirectoryInfo::GetDirectoryList()")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEDirectoryInfo Directory1(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "\\TestResources", false));

		ZEArray<ZEDirectoryInfo*>* DirectoryList = Directory1.GetDirectoryList();
		ZETestCheckEqual(DirectoryList->GetCount(), 5);

		ZEDirectoryInfo Directory2(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir", false));

		DirectoryList = Directory2.GetDirectoryList();
		ZETestCheckEqual(DirectoryList->GetCount(), 3);
	}

	ZETest("bool ZEDirectoryInfo::IsDirectory(const ZEString& DirectoryPath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		bool Directory = ZEDirectoryInfo::IsDirectory(ZEPathManager::GetWorkingDrive());
		ZETestCheck(!Directory);

		Directory = ZEDirectoryInfo::IsDirectory(ZEPathManager::GetWorkingDirectory());
		ZETestCheck(Directory);

		Directory = ZEDirectoryInfo::IsDirectory(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false));
		ZETestCheck(Directory);

		Directory = ZEDirectoryInfo::IsDirectory(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + "DirName.txt", false));
		ZETestCheck(!Directory);

		Directory = ZEDirectoryInfo::IsDirectory(ZEPathManager::GetResourcesPath());
		ZETestCheck(Directory);

		Directory = ZEDirectoryInfo::IsDirectory(ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
		ZETestCheck(!Directory);

		Directory = ZEDirectoryInfo::IsDirectory("DirName.txt");
		ZETestCheck(!Directory);

		Directory = ZEDirectoryInfo::IsDirectory(ZEPathManager::GetWorkingDrive() + Seperator + "Test");
		ZETestCheck(!Directory);

		Directory = ZEDirectoryInfo::IsDirectory(ZEPathManager::GetWorkingDrive() + Seperator + "Test" + Seperator + "DirName.txt");
		ZETestCheck(!Directory);

		Directory = ZEDirectoryInfo::IsDirectory(ZEPathManager::GetSavedGamesPath());
		ZETestCheck(Directory);
	}

	ZETest("ZEString ZEDirectoryInfo::GetDirectoryName(const ZEString& DirectoryPath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEString DirectoryName = ZEDirectoryInfo::GetDirectoryName(ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
		ZETestCheckString(DirectoryName, "DirName.txt");

		DirectoryName = ZEDirectoryInfo::GetDirectoryName(ZEPathManager::GetResourcesPath());
		ZETestCheckString(DirectoryName, "resources");

		DirectoryName = ZEDirectoryInfo::GetDirectoryName(ZEPathManager::GetWorkingDrive());
		ZETestCheckString(DirectoryName, ZEPathManager::GetWorkingDrive());

		DirectoryName = ZEDirectoryInfo::GetDirectoryName(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false));
		ZETestCheckString(DirectoryName, "trunk");

		DirectoryName = ZEDirectoryInfo::GetDirectoryName(ZEPathManager::GetApplicationResourcesPath());
		ZETestCheckString(DirectoryName, "Engine");

		DirectoryName = ZEDirectoryInfo::GetDirectoryName(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetSavedGamesPath() + Seperator + "..", false));
		ZETestCheckString(DirectoryName, "Zinek");

		DirectoryName = ZEDirectoryInfo::GetDirectoryName("DirName.txt");
		ZETestCheckString(DirectoryName, "DirName.txt");
	}

	ZETest("ZEString ZEDirectoryInfo::GetParentDirectory(const ZEString& DirectoryPath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEString ParentDirectory = ZEDirectoryInfo::GetParentDirectory(ZEPathManager::GetWorkingDrive());

		ParentDirectory = ZEDirectoryInfo::GetParentDirectory(ZEPathManager::GetWorkingDirectory());
		ZETestCheckString(ParentDirectory, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false));

		ParentDirectory = ZEDirectoryInfo::GetParentDirectory(ZEPathManager::GetWorkingDirectory() + Seperator + "DirName.txt");
		ZETestCheckString(ParentDirectory, ZEPathManager::GetWorkingDirectory());

		ParentDirectory = ZEDirectoryInfo::GetParentDirectory(ZEPathManager::GetResourcesPath());
		ZETestCheckString(ParentDirectory, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetResourcesPath() + Seperator + "..", false));

		ParentDirectory = ZEDirectoryInfo::GetParentDirectory(ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
		ZETestCheckString(ParentDirectory, ZEPathManager::GetResourcesPath());

		ParentDirectory = ZEDirectoryInfo::GetParentDirectory(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetApplicationResourcesPath() + Seperator + ".." + Seperator + "..", false));
		ZETestCheckString(ParentDirectory, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetApplicationResourcesPath() + Seperator + ".." + Seperator + ".." + Seperator + "..", false));
	}
}
