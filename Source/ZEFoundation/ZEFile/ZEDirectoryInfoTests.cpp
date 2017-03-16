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

ZE_TEST(ZEDirectoryInfo)
{
	ZE_TEST_ITEM("set paths")
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

	ZE_TEST_ITEM("ZEDirectoryInfo::~ZEDirectoryInfo()")
	{

	}

	ZE_TEST_ITEM("ZEDirectoryInfo::ZEDirectoryInfo()")
	{
		ZEDirectoryInfo Directory;
	}

	ZE_TEST_ITEM("ZEDirectoryInfo::ZEDirectoryInfo(const ZEString& DirectoryPath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEDirectoryInfo Directory(ZEPathManager::GetResourcesPath() + Seperator + "Directory");
		ZE_TEST_CHECK_STRING_EQUAL(Directory.GetName(), "Directory");
		ZE_TEST_CHECK_STRING_EQUAL(Directory.GetPath(), ZEPathManager::GetResourcesPath() + Seperator + "Directory");

		ZE_TEST_CASE("open a file through the specified path")
		{
			ZEFileTime CreationDate;
			ZEFileTime ModificationDate;
			ZEFile File;
			const ZEString FilePath = ZEPathManager::GetResourcesPath() + Seperator + "DirectoryInfo.txt";
			bool Opened = File.Open(FilePath, ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

			ZEDirectoryInfo DirectoryInfo(FilePath);

			ZE_TEST_CHECK_STRING_EQUAL(DirectoryInfo.GetName(), "DirectoryInfo.txt");
			ZE_TEST_CHECK_STRING_EQUAL(DirectoryInfo.GetPath(), FilePath);
			bool Created = DirectoryInfo.GetCreationDate(CreationDate);
			ZE_TEST_CHECK_ERROR(Created);
			//CreationDate will be the current date
			bool Modified = DirectoryInfo.GetModificationDate(ModificationDate);
			ZE_TEST_CHECK_ERROR(Modified);
			//ModificationDate will be the current date

			File.Close();
			remove(FilePath);
		}
	}

	ZE_TEST_ITEM("void ZEDirectoryInfo::SetPath(const ZEString& DirectoryPath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEDirectoryInfo Directory("resources\\DirName.txt");
		ZE_TEST_CHECK_STRING_EQUAL(Directory.GetName(), "DirName.txt");
		ZE_TEST_CHECK_STRING_EQUAL(Directory.GetPath(), "resources\\DirName.txt");

		Directory.SetPath(ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
		ZE_TEST_CHECK_STRING_EQUAL(Directory.GetName(), "DirName.txt");
		ZE_TEST_CHECK_STRING_EQUAL(Directory.GetPath(), ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
	}

	ZE_TEST_ITEM("const ZEString& ZEDirectoryInfo::GetName() const")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEDirectoryInfo Directory1("DirName.txt");
		const ZEString Name1 = Directory1.GetName();
		ZE_TEST_CHECK_STRING_EQUAL(Name1, "DirName.txt");

		ZEDirectoryInfo Directory2("resources\\DirName.txt");
		const ZEString Name2 = Directory2.GetName();
		ZE_TEST_CHECK_STRING_EQUAL(Name2, "DirName.txt");

		ZEDirectoryInfo Directory3(ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
		const ZEString Name3 = Directory3.GetName();
		ZE_TEST_CHECK_STRING_EQUAL(Name3, "DirName.txt");

		ZEDirectoryInfo Directory4(ZEPathManager::GetResourcesPath());
		const ZEString Name4 = Directory4.GetName();
		ZE_TEST_CHECK_STRING_EQUAL(Name4, "resources");
	}

	ZE_TEST_ITEM("const ZEString& ZEDirectoryInfo::GetPath() const")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEDirectoryInfo Directory1("DirName.txt");
		const ZEString Path1 = Directory1.GetPath();
		ZE_TEST_CHECK_STRING_EQUAL(Path1, "DirName.txt");

		ZEDirectoryInfo Directory2("resources\\DirName.txt");
		const ZEString Path2 = Directory2.GetPath();
		ZE_TEST_CHECK_STRING_EQUAL(Path2, "resources\\DirName.txt");

		ZEDirectoryInfo Directory3(ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
		const ZEString Path3 = Directory3.GetPath();
		ZE_TEST_CHECK_STRING_EQUAL(Path3, ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");

		ZEDirectoryInfo Directory4(ZEPathManager::GetResourcesPath());
		const ZEString Path4 = Directory4.GetPath();
		ZE_TEST_CHECK_STRING_EQUAL(Path4, ZEPathManager::GetResourcesPath());
	}

	ZE_TEST_ITEM("bool ZEDirectoryInfo::GetCreationDate(ZEFileTime& Time)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEFileTime CreationDate;
		ZEDirectoryInfo Directory(ZEPathManager::GetResourcesPath() + Seperator + "DirectoryInfo.txt");

		bool Created = Directory.GetCreationDate(CreationDate);
		ZE_TEST_CHECK_ERROR(!Created);
		//CreationDate 0

		ZEFile File;
		bool Opened = File.Open(ZEPathManager::GetResourcesPath() + Seperator +"DirectoryInfo.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

		Created = Directory.GetCreationDate(CreationDate);
		ZE_TEST_CHECK_ERROR(Created);
		//CreationDate = Creation

		File.Close();
		remove(ZEPathManager::GetResourcesPath() + Seperator + "DirectoryInfo.txt");
	}

	ZE_TEST_ITEM("bool ZEDirectoryInfo::GetModificationDate(ZEFileTime& Time)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEFileTime ModificationDate;
		ZEDirectoryInfo Directory(ZEPathManager::GetResourcesPath() + Seperator + "DirectoryInfo.txt");

		bool Modified = Directory.GetModificationDate(ModificationDate);
		ZE_TEST_CHECK_ERROR(!Modified);
		//ModificationDate 0

		ZEFile File;
		bool Opened = File.Open(ZEPathManager::GetResourcesPath() + Seperator + "DirectoryInfo.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

		Modified = Directory.GetModificationDate(ModificationDate);
		ZE_TEST_CHECK_ERROR(Modified);
		//ModificationDate = Modification

		File.Close();
		remove(ZEPathManager::GetResourcesPath() + Seperator + "DirectoryInfo.txt");
 	}

	ZE_TEST_ITEM("ZEArray<ZEFileInfo*>* ZEDirectoryInfo::GetFileList()")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEDirectoryInfo Directory1(ZEPathManager::GetResourcesPath());

		ZEArray<ZEFileInfo*>* FileList = Directory1.GetFileList();
		ZE_TEST_CHECK_EQUAL(FileList->GetCount(), 1);

		ZEDirectoryInfo Directory2(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "\\TestResources", false));

		FileList = Directory2.GetFileList();
		ZE_TEST_CHECK_EQUAL(FileList->GetCount(), 0);
	}

	ZE_TEST_ITEM("ZEArray<ZEDirectoryInfo*>* ZEDirectoryInfo::GetDirectoryList()")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEDirectoryInfo Directory1(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "\\TestResources", false));

		ZEArray<ZEDirectoryInfo*>* DirectoryList = Directory1.GetDirectoryList();
		ZE_TEST_CHECK_EQUAL(DirectoryList->GetCount(), 5);

		ZEDirectoryInfo Directory2(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + Seperator + "TestResources" + Seperator + "RunDir", false));

		DirectoryList = Directory2.GetDirectoryList();
		ZE_TEST_CHECK_EQUAL(DirectoryList->GetCount(), 3);
	}

	ZE_TEST_ITEM("bool ZEDirectoryInfo::IsDirectory(const ZEString& DirectoryPath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		bool Directory = ZEDirectoryInfo::IsDirectory(ZEPathManager::GetWorkingDrive());
		ZE_TEST_CHECK_ERROR(!Directory);

		Directory = ZEDirectoryInfo::IsDirectory(ZEPathManager::GetWorkingDirectory());
		ZE_TEST_CHECK_ERROR(Directory);

		Directory = ZEDirectoryInfo::IsDirectory(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false));
		ZE_TEST_CHECK_ERROR(Directory);

		Directory = ZEDirectoryInfo::IsDirectory(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + ".." + "DirName.txt", false));
		ZE_TEST_CHECK_ERROR(!Directory);

		Directory = ZEDirectoryInfo::IsDirectory(ZEPathManager::GetResourcesPath());
		ZE_TEST_CHECK_ERROR(Directory);

		Directory = ZEDirectoryInfo::IsDirectory(ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
		ZE_TEST_CHECK_ERROR(!Directory);

		Directory = ZEDirectoryInfo::IsDirectory("DirName.txt");
		ZE_TEST_CHECK_ERROR(!Directory);

		Directory = ZEDirectoryInfo::IsDirectory(ZEPathManager::GetWorkingDrive() + Seperator + "Test");
		ZE_TEST_CHECK_ERROR(!Directory);

		Directory = ZEDirectoryInfo::IsDirectory(ZEPathManager::GetWorkingDrive() + Seperator + "Test" + Seperator + "DirName.txt");
		ZE_TEST_CHECK_ERROR(!Directory);

		Directory = ZEDirectoryInfo::IsDirectory(ZEPathManager::GetSavedGamesPath());
		ZE_TEST_CHECK_ERROR(Directory);
	}

	ZE_TEST_ITEM("ZEString ZEDirectoryInfo::GetDirectoryName(const ZEString& DirectoryPath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEString DirectoryName = ZEDirectoryInfo::GetDirectoryName(ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
		ZE_TEST_CHECK_STRING_EQUAL(DirectoryName, "DirName.txt");

		DirectoryName = ZEDirectoryInfo::GetDirectoryName(ZEPathManager::GetResourcesPath());
		ZE_TEST_CHECK_STRING_EQUAL(DirectoryName, "resources");

		DirectoryName = ZEDirectoryInfo::GetDirectoryName(ZEPathManager::GetWorkingDrive());
		ZE_TEST_CHECK_STRING_EQUAL(DirectoryName, ZEPathManager::GetWorkingDrive());

		DirectoryName = ZEDirectoryInfo::GetDirectoryName(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false));
		ZE_TEST_CHECK_STRING_EQUAL(DirectoryName, "trunk");

		DirectoryName = ZEDirectoryInfo::GetDirectoryName(ZEPathManager::GetApplicationResourcesPath());
		ZE_TEST_CHECK_STRING_EQUAL(DirectoryName, "Engine");

		DirectoryName = ZEDirectoryInfo::GetDirectoryName(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetSavedGamesPath() + Seperator + "..", false));
		ZE_TEST_CHECK_STRING_EQUAL(DirectoryName, "Zinek");

		DirectoryName = ZEDirectoryInfo::GetDirectoryName("DirName.txt");
		ZE_TEST_CHECK_STRING_EQUAL(DirectoryName, "DirName.txt");
	}

	ZE_TEST_ITEM("ZEString ZEDirectoryInfo::GetParentDirectory(const ZEString& DirectoryPath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEString ParentDirectory = ZEDirectoryInfo::GetParentDirectory(ZEPathManager::GetWorkingDrive());

		ParentDirectory = ZEDirectoryInfo::GetParentDirectory(ZEPathManager::GetWorkingDirectory());
		ZE_TEST_CHECK_STRING_EQUAL(ParentDirectory, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false));

		ParentDirectory = ZEDirectoryInfo::GetParentDirectory(ZEPathManager::GetWorkingDirectory() + Seperator + "DirName.txt");
		ZE_TEST_CHECK_STRING_EQUAL(ParentDirectory, ZEPathManager::GetWorkingDirectory());

		ParentDirectory = ZEDirectoryInfo::GetParentDirectory(ZEPathManager::GetResourcesPath());
		ZE_TEST_CHECK_STRING_EQUAL(ParentDirectory, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetResourcesPath() + Seperator + "..", false));

		ParentDirectory = ZEDirectoryInfo::GetParentDirectory(ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
		ZE_TEST_CHECK_STRING_EQUAL(ParentDirectory, ZEPathManager::GetResourcesPath());

		ParentDirectory = ZEDirectoryInfo::GetParentDirectory(ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetApplicationResourcesPath() + Seperator + ".." + Seperator + "..", false));
		ZE_TEST_CHECK_STRING_EQUAL(ParentDirectory, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetApplicationResourcesPath() + Seperator + ".." + Seperator + ".." + Seperator + "..", false));
	}
}
