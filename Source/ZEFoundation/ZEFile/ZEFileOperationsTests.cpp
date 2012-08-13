//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFileOperationsTests.cpp
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

#include <ZETest/ZETest.h>
#include <ZEDS/ZEString.h>
#include <ZEFoundation/ZERandom.h>
#include "ZEFileOperations.h"
#include "ZEFileInfo.h"
#include "ZEDirectoryInfo.h"
#include "ZEFile.h"


void CreateTestFile(const char* Path)
{
	ZEFile* NewFile = new ZEFile();

	if (NewFile->Open(Path, ZE_FOM_READ_WRITE, ZE_FCM_CREATE))
		NewFile->Close();
}

ZETestSuite(ZEFileOperations)
{
	ZETest("static bool	CreateFolder(const ZEString& DestinationParentPath, const ZEString& Name)")
	{
		ZEPathManager::CustomizePaths();

		ZEString RunDirPath = ZEPathManager::GetWorkingDirectory();

		bool ResultForCreation = ZEFileOperations::CreateFolder(RunDirPath, "ZEFileOpsTestFolder");
		ZETestCheck(ResultForCreation);

		bool ResultForCheck = ZEDirectoryInfo::IsDirectory("ZEFileOpsTestFolder");
		ZETestCheck(ResultForCheck);

		ZETestCase("Creating a directory with a relative path to working directory.")
		{
			bool ResultForCreation = ZEFileOperations::CreateFolder("ZEFileOpsTestFolder", "ZEFileOpsTestFolderTrivial");
			ZETestCheck(ResultForCreation);

			bool ResultForCheck = ZEDirectoryInfo::IsDirectory(RunDirPath + "\\ZEFileOpsTestFolder" + "\\ZEFileOpsTestFolderTrivial");
			ZETestCheck(ResultForCheck);
		}

		ZETestCase("Trying to create an already existing directory")
		{
			bool ResultForCreation = ZEFileOperations::CreateFolder(RunDirPath, "ZEFileOpsTestFolder");
			ZETestCheck(!ResultForCreation);
		}
	}

	ZETest("static bool	Rename(const ZEString& Name, ZEFileInfo* File)")
	{
		ZEString RunDirPath = ZEPathManager::GetWorkingDirectory();

		ZEFileOperations::CreateFolder("ZEFileOpsTestFolder", "ZEFileOpsTestSubFolder01");
		ZEString TestFilePath = RunDirPath + "\\ZEFileOpsTestFolder" + "\\ZEFileOpsTestSubFolder01" + "\\ZEFileOpsTestFile.txt";
		CreateTestFile(TestFilePath);

		bool ResultForCreation = ZEFileInfo::IsFile(TestFilePath);
		ZETestCheck(ResultForCreation);

		ZEFileInfo TestFile(TestFilePath);

		bool ResultForRenaming = ZEFileOperations::Rename("ZEFileOpsTestFile01.txt", &TestFile);
		ZETestCheck(ResultForRenaming);

		bool ResultForCheck = ZEFileInfo::IsFile(RunDirPath + "\\ZEFileOpsTestFolder" + "\\ZEFileOpsTestSubFolder01" + "\\ZEFileOpsTestFile01.txt");
		ZETestCheck(ResultForCheck);

		ZETestCase("Renaming a file to an already existing file's name")
		{
			CreateTestFile(TestFilePath);

			bool ResultForCreation2 = ZEFileInfo::IsFile(TestFilePath);
			ZETestCheck(ResultForCreation2);

			ZEFileInfo TestFile2(TestFilePath);

			bool ResultForRenaming2 = ZEFileOperations::Rename("ZEFileOpsTestFile01.txt", &TestFile2);
			ZETestCheck(!ResultForRenaming2);

			ResultForRenaming2 = ZEFileOperations::Rename("ZEFileOpsTestFile03.txt", &TestFile2);
			ZETestCheck(ResultForRenaming2);
		}

	}

	ZETest("static bool	Rename(const ZEString& Name, ZEFolderInfo* Folder)")
	{
		ZEString RunDirPath = ZEPathManager::GetWorkingDirectory();

		ZEFileOperations::CreateFolder(RunDirPath + "\\ZEFileOpsTestFolder", "ZEFileOpsTestSubFolderTemp");
		ZEDirectoryInfo TestFolder(ZEString("ZEFileOpsTestFolder") + "\\ZEFileOpsTestSubFolderTemp");

		bool ResultForRenaming = ZEFileOperations::Rename("ZEFileOpsTestSubFolder02", &TestFolder);
		ZETestCheck(ResultForRenaming);

		bool ResultForCheck = ZEDirectoryInfo::IsDirectory(RunDirPath + "\\ZEFileOpsTestFolder" + "\\ZEFileOpsTestSubFolder02");
		ZETestCheck(ResultForCheck);

		ZETestCase("Renaming a folder to an already existing folder's name")
		{
			ZEDirectoryInfo TestFolder2(RunDirPath + "\\ZEFileOpsTestFolder" + "\\ZEFileOpsTestFolderTrivial");

			bool ResultForRenaming2 = ZEFileOperations::Rename("ZEFileOpsTestSubFolder02", &TestFolder2);
			ZETestCheck(!ResultForRenaming2);
		}
	}

	ZETest("static bool	Copy(const ZEString& DestinationParentPath, ZEFileInfo* File)")
	{
		ZEString RunDirPath = ZEPathManager::GetWorkingDirectory();
		ZEString TestFilePath = RunDirPath + "\\ZEFileOpsTestFolder" + "\\ZEFileOpsTestSubFolder01" + "\\ZEFileOpsTestFile01.txt";
		unsigned char* WriteBuffer = new unsigned char[50];
		unsigned char* ReadBuffer = new unsigned char[50];

		for (ZEInt I = 0; I < 50; I++)
		{
			WriteBuffer[I] = ZERandom::GetUInt() % 128;
		}

		ZEFile ZEFileOpsTestFile01;
		ZEFileOpsTestFile01.Open(TestFilePath, ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
		ZEFileOpsTestFile01.Write(WriteBuffer, sizeof(unsigned char), 50);
		ZEFileOpsTestFile01.Close();

		ZEFileInfo ZEFileOpsTestFile01Info(ZEString("ZEFileOpsTestFolder") + "\\ZEFileOpsTestSubFolder01" + "\\ZEFileOpsTestFile01.txt");

		bool ResultforCreation = ZEFileOperations::Copy(RunDirPath + "\\ZEFileOpsTestFolder" + "\\ZEFileOpsTestSubFolder02", &ZEFileOpsTestFile01Info);
		ZETestCheck(ResultforCreation);

		ZEString TestResultFilePath = RunDirPath + "\\ZEFileOpsTestFolder" + "\\ZEFileOpsTestSubFolder02" + "\\ZEFileOpsTestFile01.txt";
		bool ResultforCheck = ZEFileInfo::IsFile(TestResultFilePath);
		ZETestCheck(ResultforCheck);

		ZEFile ZEFileOpsTestFile02;
		ZEFileOpsTestFile02.Open(TestResultFilePath, ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
		ZEFileOpsTestFile02.Read(ReadBuffer, sizeof(unsigned char), 50);
		ZEFileOpsTestFile02.Close();

		bool CompareFlag = true;

		for (ZEInt I = 0; I < 50; I++)
		{
			if (ReadBuffer[I] != WriteBuffer[I])
				CompareFlag = false;
		}

		ZETestCheck(CompareFlag);

		ZETestCase("Trying to copy a file as an already existing file")
		{
			bool ResultforCreation2 = ZEFileOperations::Copy(RunDirPath + "\\ZEFileOpsTestFolder" + "\\ZEFileOpsTestSubFolder02", &ZEFileOpsTestFile01Info);
			ZETestCheck(!ResultforCreation2);
		}

		ZEFileInfo ZEFileOpsTestFile02Info(TestResultFilePath);
		ZEFileOperations::Rename("ZEFileOpsTestFile02.txt", &ZEFileOpsTestFile02Info);

		delete WriteBuffer;
		delete ReadBuffer;

	}

	ZETest("static bool	Copy(const ZEString& DestinationParentPath, ZEDirectoryInfo* Folder)")
	{
		ZEString RunDirPath = ZEPathManager::GetWorkingDirectory();

		ZEString TestFolderPath = RunDirPath + "\\ZEFileOpsTestFolder" + "\\ZEFileOpsTestFolderTrivial";

		ZEDirectoryInfo Folder(ZEString("ZEFileOpsTestFolder") + "\\ZEFileOpsTestSubFolder01");

		bool ResultForCreation = ZEFileOperations::Copy(TestFolderPath, &Folder);
		ZETestCheck(ResultForCreation);

		bool ResultForCheck = ZEDirectoryInfo::IsDirectory(TestFolderPath + "\\ZEFileOpsTestSubFolder01");
		ZETestCheck(ResultForCheck);

		bool ResultForCheck2 = ZEFileInfo::IsFile(TestFolderPath + "\\ZEFileOpsTestSubFolder01" + "\\ZEFileOpsTestFile01.txt");
		ZETestCheck(ResultForCheck2);

		bool ResultForCheck3 = ZEFileInfo::IsFile(TestFolderPath + "\\ZEFileOpsTestSubFolder01" + "\\ZEFileOpsTestFile03.txt");
		ZETestCheck(ResultForCheck3);

		ZETestCase("Trying to copy folder as an already existing folder")
		{
			bool ResultForCreation2 = ZEFileOperations::Copy(TestFolderPath, &Folder);
			ZETestCheck(!ResultForCreation2);
		}
	}

	ZETest("static bool	Move(const ZEString& DestinationParentPath, ZEFileInfo* File)")
	{
		ZEString RunDirPath = ZEPathManager::GetWorkingDirectory();

		ZEString TestFilePath = RunDirPath + "\\ZEFileOpsTestFolder" + "\\ZEFileOpsTestFolderTrivial" + "\\ZEFileOpsTestSubFolder01" + "\\ZEFileOpsTestFile01.txt";
		ZEString DestinationPath = RunDirPath + "\\ZEFileOpsTestFolder" + "\\ZEFileOpsTestFolderTrivial";

		ZEFileInfo File(TestFilePath);

		bool ResultForCreation = ZEFileOperations::Move(DestinationPath, &File);
		ZETestCheck(ResultForCreation);

		bool ResultForCheck = ZEFileInfo::IsFile(DestinationPath + "\\ZEFileOpsTestFile01.txt");
		ZETestCheck(ResultForCheck);

		bool ResultForCheck2 = ZEFileInfo::IsFile(TestFilePath);
		ZETestCheck(!ResultForCheck2);

		bool ResultForCompare = (strcmp(File.GetPath(), DestinationPath + "\\ZEFileOpsTestFile01.txt") == 0);
		ZETestCheck(ResultForCompare);

		ZETestCase("Trying to move a file onto an already existing file")
		{
			ZEFileInfo File2(RunDirPath + "\\ZEFileOpsTestFolder" + "\\ZEFileOpsTestSubFolder01" + "\\ZEFileOpsTestFile01.txt");
			bool ResultForCreation2 = ZEFileOperations::Move(DestinationPath, &File2);
			ZETestCheck(!ResultForCreation2);
		}
	}

	ZETest("static bool	Move(const ZEString& DestinationParentPath, ZEDirectoryInfo* Folder)")
	{
		ZEString RunDirPath = ZEPathManager::GetWorkingDirectory();

		ZEDirectoryInfo Folder(RunDirPath + "\\ZEFileOpsTestFolder" + "\\ZEFileOpsTestSubFolder02");

		ZEString TestFolderPath = RunDirPath + "\\ZEFileOpsTestFolder";
		ZEString DestinationPath = RunDirPath + "\\ZEFileOpsTestFolder" + "\\ZEFileOpsTestFolderTrivial";

		bool ResultForCreation = ZEFileOperations::Move(DestinationPath, &Folder);
		ZETestCheck(ResultForCreation);

		bool ResultForCheck = ZEDirectoryInfo::IsDirectory(DestinationPath + "\\ZEFileOpsTestSubFolder02");
		ZETestCheck(ResultForCheck);

		bool ResultForCheck2 = ZEDirectoryInfo::IsDirectory(TestFolderPath + "\\ZEFileOpsTestSubFolder02");
		ZETestCheck(!ResultForCheck2);

		bool ResultForCompare = (strcmp(Folder.GetPath(), DestinationPath + "\\ZEFileOpsTestSubFolder02") == 0);
		ZETestCheck(ResultForCompare);

		ZETestCase("Trying to move a folder onto an already existing folder")
		{
			ZEDirectoryInfo Folder2(RunDirPath + "\\ZEFileOpsTestFolder" + "\\ZEFileOpsTestFolderTrivial" + "\\ZEFileOpsTestSubFolder01");

			bool ResultForCreation2 = ZEFileOperations::Move(DestinationPath, &Folder2);
			ZETestCheck(!ResultForCreation2);
		}
	}

	ZETest("static bool	Delete(ZEFileInfo* File)")
	{
		ZEString RunDirPath = ZEPathManager::GetWorkingDirectory();

		ZEString TestFilePath = RunDirPath + "\\ZEFileOpsTestFolder" + "\\ZEFileOpsTestFolderTrivial" + "\\ZEFileOpsTestFile01.txt";

		bool ResultForCheck = ZEFileInfo::IsFile(TestFilePath);
		ZETestCheck(ResultForCheck);

		ZEFileInfo File(TestFilePath);

		bool ResultForDeletion = ZEFileOperations::Delete(&File);
		ZETestCheck(ResultForDeletion);

		bool ResultForCheck2 = ZEFileInfo::IsFile(TestFilePath);
		ZETestCheck(!ResultForCheck2);

		ZETestCase("Trying to delete an already deleted file")
		{
			bool ResultForDeletion2 = ZEFileOperations::Delete(&File);
			ZETestCheck(!ResultForDeletion2);
		}
	}

	ZETest("static bool	Delete(ZEDirectoryInfo* Folder)")
	{
		ZEString RunDirPath = ZEPathManager::GetWorkingDirectory();

		ZEString TestFolderPath = RunDirPath + "\\ZEFileOpsTestFolder";

		bool ResultForCheck = ZEDirectoryInfo::IsDirectory(TestFolderPath);
		ZETestCheck(ResultForCheck);

		ZEDirectoryInfo Folder(TestFolderPath);

		bool ResultForDeletion = ZEFileOperations::Delete(&Folder);
		ZETestCheck(ResultForDeletion);

		bool ResultForCheck2 = ZEDirectoryInfo::IsDirectory(TestFolderPath);
		ZETestCheck(!ResultForCheck2);

		ZETestCase("Trying to delete an already deleted folder")
		{
			bool ResultForDeletion2 = ZEFileOperations::Delete(&Folder);
			ZETestCheck(!ResultForDeletion2);
		}
	}
}

