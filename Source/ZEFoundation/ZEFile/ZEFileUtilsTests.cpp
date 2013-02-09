//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFileUtilsTests.cpp
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
#include "ZEFile/ZEFileUtils.h"
#include "ZEFile/ZEFile.h"
#include "ZEDS/ZEString.h"
#include <stdio.h>
#include "ZEError.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define WINAPI__stdcall
#include "ZEPathManager.h"
#include "ZEPathUtils.h"


ZETestSuite(ZEFileUtils)
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

	ZETest("bool ZEFileUtils::GetCreationTime(ZEFileTime* Output, const ZEString& Path)")
	{
		ZEFileTime Output;
		const ZEString Path = "CreationTime.txt";

		bool Created = ZEFileUtils::GetCreationTime(&Output, Path);
		ZETestCheck(!Created);

		ZETestCase("open a file through the path")
		{
			ZEFile File;
			File.Open("FileUtils.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

			Created = ZEFileUtils::GetCreationTime(&Output, "FileUtils.txt");
			ZETestCheck(Created);
			//Output will be the creation time of the file

			File.Close();
		}

		ZETestCase("for a newly created file")
		{
			ZEFile File;
			File.Open("NewFile.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

			Created = ZEFileUtils::GetCreationTime(&Output, "NewFile.txt");
			ZETestCheck(Created);
			//Output will be the current time

			File.Close();
			remove("NewFile.txt");
		}
	}

	ZETest("void ZEFileUtils::GetCreationTime(ZEFileTime* Output, const ZEFileSearchStream* FindData)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEFileTime Output;
		ZEFileSearchStream FindData;

		bool OpenStream = ZEFileUtils::OpenSearchStream(&FindData, ZEPathManager::GetWorkingDirectory());
		ZETestCheck(OpenStream);
		
		ZEFileUtils::GetCreationTime(&Output, &FindData);
		//Output will be the creation time of the stream
		
		ZETestCase("call OpenSearchStream with a new file through the path")
		{
			ZEFileSearchStream FindData2;
			ZEFile File;
			bool Opened = File.Open(ZEPathManager::GetResourcesPath() + Seperator + "New.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			OpenStream = ZEFileUtils::OpenSearchStream(&FindData2, ZEPathManager::GetResourcesPath() + Seperator + "New.txt");
			ZETestCheck(!OpenStream);

			ZEFileUtils::GetCreationTime(&Output, &FindData2);

			File.Close();
			remove(ZEPathManager::GetResourcesPath() + Seperator + "New.txt");
		}

		ZETestCase("different path for OpenSearchStream")
		{
			OpenStream = ZEFileUtils::OpenSearchStream(&FindData, ZEPathManager::GetResourcesPath());
			ZETestCheck(OpenStream);

			ZEFileUtils::GetCreationTime(&Output, &FindData);
			//Output will be the creation time of the stream
		}
	}

	ZETest("bool ZEFileUtils::GetModificationTime(ZEFileTime* Output, const ZEString& Path)")
	{
		ZEFileTime Output;
		const ZEString Path = "ModificationTime.txt";

		bool Modified = ZEFileUtils::GetModificationTime(&Output, Path);
		ZETestCheck(!Modified);

		ZETestCase("for an existing file")
		{
			Modified = ZEFileUtils::GetModificationTime(&Output, "FileUtils.txt");
			ZETestCheck(Modified);
			//Output will be the modification time of the file
		}

		ZETestCase("for a newly opened file")
		{
			ZEFile File;
			File.Open(Path, ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

			Modified = ZEFileUtils::GetModificationTime(&Output, Path);
			ZETestCheck(Modified);
			//Output will be the current time

			File.Close();
			remove("ModificationTime.txt");
		}
	}

	ZETest("void ZEFileUtils::GetModificationTime(ZEFileTime* Output, const ZEFileSearchStream* FindData)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEFileTime Output;
		ZEFileSearchStream FindData;
		bool OpenStream = ZEFileUtils::OpenSearchStream(&FindData, ZEPathManager::GetWorkingDirectory());
		ZETestCheck(OpenStream);

		ZEFileUtils::GetModificationTime(&Output, &FindData);
		//Output will be the last access time of the stream

		ZETestCase("call OpenSearchStream with a new file through the path")
		{
			ZEFileTime Output2;
			ZEFileSearchStream FindData2;
			ZEFile File;
			bool Opened = File.Open(ZEPathManager::GetResourcesPath() + Seperator + "New.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			OpenStream = ZEFileUtils::OpenSearchStream(&FindData2, ZEPathManager::GetResourcesPath() + Seperator + "New.txt");
			ZETestCheck(!OpenStream);

			ZEFileUtils::GetModificationTime(&Output2, &FindData2);

			File.Close();
			remove(ZEPathManager::GetResourcesPath() + Seperator + "New.txt");
		}

		ZETestCase("different path for OpenSearchStream")
		{
			OpenStream = ZEFileUtils::OpenSearchStream(&FindData, ZEPathManager::GetResourcesPath());
			ZETestCheck(OpenStream);

			ZEFileUtils::GetModificationTime(&Output, &FindData);
			//Output will be the last access time of the stream
		}
	}

	ZETest("bool ZEFileUtils::IsFile(const ZEString& Path)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		const ZEString Path = "IsFile.txt";

		bool IsFile = ZEFileUtils::IsFile(Path);
		ZETestCheck(!IsFile);

		ZETestCase("for an existent file")
		{
			IsFile = ZEFileUtils::IsFile("FileUtils.txt");
			ZETestCheck(IsFile);
		}

		ZETestCase("for an existing file with absolute path")
		{
			IsFile = ZEFileUtils::IsFile(ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
			ZETestCheck(IsFile);
		}
	}

	ZETest("bool ZEFileUtils::IsFile(const ZEFileSearchStream* FindData)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		const ZEFileSearchStream FindData;
		
		bool IsFile = ZEFileUtils::IsFile(&FindData);
		ZETestCheck(IsFile);

		ZETestCase("OpenSearchStream")
		{
			ZEFileSearchStream Stream;
			bool OpenStream = ZEFileUtils::OpenSearchStream(&Stream, ZEPathManager::GetWorkingDirectory());
			ZETestCheck(OpenStream);

			IsFile = ZEFileUtils::IsFile(&Stream);
			ZETestCheck(!IsFile);
		}

		ZETestCase("call OpenSearchStream with a path includes a file")
		{
			ZEFileSearchStream SearchStream;
			bool OpenStream = ZEFileUtils::OpenSearchStream(&SearchStream, ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
			ZETestCheck(!OpenStream);

			IsFile = ZEFileUtils::IsFile(&SearchStream);
			ZETestCheck(IsFile);
		}
	}

	ZETest("bool ZEFileUtils::IsDirectory(const ZEString& Path)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		const ZEString Path = "IsDirectory.txt";

		bool IsDirectory = ZEFileUtils::IsDirectory(Path);
		ZETestCheck(!IsDirectory);

		ZETestCase("for an existing file")
		{
			IsDirectory = ZEFileUtils::IsDirectory("FileUtils.txt");
			ZETestCheck(!IsDirectory);
		}

		ZETestCase("file path with directory")
		{
			IsDirectory = ZEFileUtils::IsDirectory("resources" + Seperator + "IsDirectory.txt");
			ZETestCheck(!IsDirectory);
		}

		ZETestCase("open a file with a directory")
		{
			ZEFile File;
			File.Open(ZEPathManager::GetResourcesPath() + Seperator + "IsDirectory.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

			IsDirectory = ZEFileUtils::IsDirectory(ZEPathManager::GetResourcesPath() + Seperator + "IsDirectory.txt");
			ZETestCheck(!IsDirectory);

			File.Close();
			remove(ZEPathManager::GetResourcesPath() + Seperator + "IsDirectory.txt");
		}

		ZETestCase("path without a file")
		{
			IsDirectory = ZEFileUtils::IsDirectory("resources");
			ZETestCheck(IsDirectory);
		}

		ZETestCase("with working directory path")
		{
			IsDirectory = ZEFileUtils::IsDirectory(ZEPathManager::GetWorkingDirectory());
			ZETestCheck(IsDirectory);
		}

		ZETestCase("with an absolute path with a file")
		{
			IsDirectory = ZEFileUtils::IsDirectory(ZEPathManager::GetWorkingDirectory() + Seperator + "File.txt");
			ZETestCheck(!IsDirectory);
		}

		ZETestCase("different absolute path without a file")
		{
			IsDirectory = ZEFileUtils::IsDirectory(ZEPathManager::GetResourcesPath());
			ZETestCheck(IsDirectory);
		}
	}

	ZETest("bool ZEFileUtils::IsDirectory(const ZEFileSearchStream* FindData)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		const ZEFileSearchStream FindData;

		bool IsDirectory = ZEFileUtils::IsDirectory(&FindData);
		ZETestCheck(!IsDirectory);

		ZETestCase("OpenSearchStream")
		{
			ZEFileSearchStream Stream;
			bool OpenStream = ZEFileUtils::OpenSearchStream(&Stream, ZEPathManager::GetResourcesPath());
			ZETestCheck(OpenStream);

			IsDirectory = ZEFileUtils::IsDirectory(&Stream);
			ZETestCheck(IsDirectory);
		}

		ZETestCase("call OpenSerachStream with a path includes a file")
		{
			ZEFileSearchStream SearchStream;
			bool OpenStream = ZEFileUtils::OpenSearchStream(&SearchStream, ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
			ZETestCheck(!OpenStream);

			IsDirectory = ZEFileUtils::IsDirectory(&SearchStream);
			ZETestCheck(!IsDirectory);
		}

		ZETestCase("OpenSearchStream with working directory path")
		{
			ZEFileSearchStream DirStream;
			bool OpenStream = ZEFileUtils::OpenSearchStream(&DirStream, ZEPathManager::GetWorkingDirectory());
			ZETestCheck(OpenStream);

			IsDirectory = ZEFileUtils::IsDirectory(&DirStream);
			ZETestCheck(IsDirectory);
		}
	}

	ZETest("ZEString ZEFileUtils::GetFileName(const ZEFileSearchStream* FindData)")
	{
		const ZEFileSearchStream FindData;

		ZEString FileName = ZEFileUtils::GetFileName(&FindData);

		ZETestCase("OpenSearchStream")
		{
			ZEFileSearchStream Stream;
			bool OpenStream = ZEFileUtils::OpenSearchStream(&Stream, ZEPathManager::GetResourcesPath());
			ZETestCheck(OpenStream);

			FileName = ZEFileUtils::GetFileName(&Stream);
			ZETestCheckString(FileName, ".");
		}
	}

	ZETest("ZEInt64 ZEFileUtils::GetFileSize(const ZEString& Path)")
	{
		ZEInt64 FileSize = ZEFileUtils::GetFileSize("FileUtils.txt");
		ZETestCheckEqual(FileSize, 0);

		ZETestCase("call function without open the file")
		{
			FileSize = ZEFileUtils::GetFileSize("Utils.txt");
			ZETestCheckEqual(FileSize, -1);
		}

		ZETestCase("write to file")
		{
			unsigned char* Buffer = new unsigned char[1024];
			for (ZEInt I = 0 ; I < 1024 ; I++)
			{
				Buffer[I] = I % 256;
			}

			ZEFile File;
			File.Open("FileUtilsSize.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 15);
			File.Flush();
			ZEInt64 Size = File.GetSize();
			File.Close();
			FileSize = ZEFileUtils::GetFileSize("FileUtilsSize.txt");
			ZETestCheckEqual(FileSize, 15);

			remove("FileUtilsSize.txt");
		}
	}

	ZETest("ZEInt64 ZEFileUtils::GetFileSize(const ZEFileSearchStream* FindData)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEFileSearchStream FindData;

		ZEInt64 FileSize = ZEFileUtils::GetFileSize(&FindData);

		ZETestCase("OpenSearchStream")
		{
			ZEFileSearchStream SearchStream;
			bool OpenStream = ZEFileUtils::OpenSearchStream(&SearchStream, ZEPathManager::GetWorkingDirectory());
			ZETestCheck(OpenStream);

			FileSize = ZEFileUtils::GetFileSize(&SearchStream);
			ZETestCheckEqual(FileSize, 0);
		}

		ZETestCase("OpenSearchStream with a different path")
		{
			ZEFileSearchStream SearchStream1;
			bool OpenStream = ZEFileUtils::OpenSearchStream(&SearchStream1, ZEPathManager::GetResourcesPath());
			ZETestCheck(OpenStream);

			FileSize = ZEFileUtils::GetFileSize(&SearchStream1);
			ZETestCheckEqual(FileSize, 0);
		}

		ZETestCase("OpenSearchStream with resources path including a file")
		{
			unsigned char* Buffer = new unsigned char[1024];
			for (ZEInt I = 0 ; I < 1024 ; I++)
			{
				Buffer[I] = I % 256;
			}

			ZEFile File;
			File.Open(ZEPathManager::GetResourcesPath() + Seperator + "Util.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 15);
			File.Flush();
			ZEInt64 Size = File.GetSize();
			File.Close();

			ZEFileSearchStream Stream;
			bool OpenStream = ZEFileUtils::OpenSearchStream(&Stream, ZEPathManager::GetResourcesPath() + Seperator + "Util.txt");
			ZETestCheck(!OpenStream);

			FileSize = ZEFileUtils::GetFileSize(&Stream);

			remove(ZEPathManager::GetResourcesPath() + Seperator + "Util.txt");
		}

		ZETestCase("open and write file through the resources path, then open the stream and go to file using FindNextInStream function")
		{
			unsigned char* Buffer = new unsigned char[1024];
			for (ZEInt I = 0 ; I < 1024 ; I++)
			{
				Buffer[I] = I % 256;
			}

			ZEFile File;
			File.Open(ZEPathManager::GetResourcesPath() + Seperator + "Util.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 100);
			File.Flush();
			ZEInt64 Size = File.GetSize();
			File.Close();

			ZEFileSearchStream Stream;
			bool OpenStream = ZEFileUtils::OpenSearchStream(&Stream, ZEPathManager::GetResourcesPath());
			ZETestCheck(OpenStream);
			ZETestCheckString(ZEFileUtils::GetFileName(&Stream), ".");
			FileSize = ZEFileUtils::GetFileSize(&Stream);
			ZETestCheckEqual(FileSize, 0);

			bool NextStream = ZEFileUtils::FindNextInStream(&Stream);
			ZETestCheck(NextStream);
			ZETestCheckString(ZEFileUtils::GetFileName(&Stream), "..");
			FileSize = ZEFileUtils::GetFileSize(&Stream);
			ZETestCheckEqual(FileSize, 0);

			NextStream = ZEFileUtils::FindNextInStream(&Stream);
			ZETestCheck(NextStream);
			ZETestCheckString(ZEFileUtils::GetFileName(&Stream), "DirName.txt");
			FileSize = ZEFileUtils::GetFileSize(&Stream);
			ZETestCheckEqual(FileSize, 0);
			
			NextStream = ZEFileUtils::FindNextInStream(&Stream);
			ZETestCheck(NextStream);
			ZETestCheckString(ZEFileUtils::GetFileName(&Stream), "Engine");
			FileSize = ZEFileUtils::GetFileSize(&Stream);
			ZETestCheckEqual(FileSize, 0);
			
			NextStream = ZEFileUtils::FindNextInStream(&Stream);
			ZETestCheck(NextStream);
			ZETestCheckString(ZEFileUtils::GetFileName(&Stream), "Util.txt");
			FileSize = ZEFileUtils::GetFileSize(&Stream);
			ZETestCheckEqual(FileSize, 100);
			
			remove(ZEPathManager::GetResourcesPath() + Seperator + "Util.txt");
		}
	}

	ZETest("bool ZEFileUtils::OpenSearchStream(ZEFileSearchStream* FindData, const ZEString& Path)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();
		ZEFileSearchStream FindData;

		bool OpenStream = ZEFileUtils::OpenSearchStream(&FindData, ZEPathManager::GetResourcesPath());
		ZETestCheck(OpenStream);

		OpenStream = ZEFileUtils::OpenSearchStream(&FindData, ZEPathManager::GetResourcesPath() + Seperator + "DirName.txt");
		ZETestCheck(!OpenStream);
	}

	ZETest("bool ZEFileUtils::CloseSearchStream(ZEFileSearchStream* FindData)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();
		ZEFileSearchStream FindData;

		bool CloseStream = ZEFileUtils::CloseSearchStream(&FindData);
		ZETestCheck(!CloseStream);

		bool OpenStream = ZEFileUtils::OpenSearchStream(&FindData, ZEPathManager::GetResourcesPath());
		ZETestCheck(OpenStream);

		CloseStream = ZEFileUtils::CloseSearchStream(&FindData);
		ZETestCheck(CloseStream);
	}

	ZETest("bool ZEFileUtils::FindNextInStream(ZEFileSearchStream *FindData)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();
		ZEFileSearchStream FindData;
		bool OpenStream = ZEFileUtils::OpenSearchStream(&FindData, ZEPathManager::GetResourcesPath());
		ZETestCheck(OpenStream);
		ZETestCheckString(ZEFileUtils::GetFileName(&FindData), ".");

		bool FindNext = ZEFileUtils::FindNextInStream(&FindData);
		ZETestCheck(FindNext);
		ZETestCheckString(ZEFileUtils::GetFileName(&FindData), "..");

		FindNext = ZEFileUtils::FindNextInStream(&FindData);
		ZETestCheck(FindNext);
		ZETestCheckString(ZEFileUtils::GetFileName(&FindData), "DirName.txt");

		FindNext = ZEFileUtils::FindNextInStream(&FindData);
		ZETestCheck(FindNext);
		ZETestCheckString(ZEFileUtils::GetFileName(&FindData), "Engine");

		ZETestCase("OpenSearchStream with a different path")
		{
			ZEFileSearchStream Stream;
			OpenStream = ZEFileUtils::OpenSearchStream(&Stream, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetResourcesPath() + Seperator + "..", false));
			ZETestCheck(OpenStream);
			ZETestCheckString(ZEFileUtils::GetFileName(&Stream), ".");

			FindNext = ZEFileUtils::FindNextInStream(&Stream);
			ZETestCheck(FindNext);
			ZETestCheckString(ZEFileUtils::GetFileName(&Stream), "..");
		}

		ZETestCase("call OpenSearchStream with a path includes a file")
		{
			ZEFileSearchStream SearchStream;
			OpenStream = ZEFileUtils::OpenSearchStream(&SearchStream, ZEPathManager::GetResourcesPath() + Seperator + "FileUtils.txt");
			ZETestCheck(!OpenStream);

			FindNext = ZEFileUtils::FindNextInStream(&SearchStream);
			ZETestCheck(!FindNext);
		}
	}

	ZETest("void ZEFileUtils::GetErrorString(ZEString& ErrorString, const ZEInt ErrorId)")
	{
		ZEString ErrorString;
		ZEFileUtils::GetErrorString(ErrorString, 0);
		ZETestCheckString(ErrorString, "Cannot convert error to string. ErrorId = 0.");

		ZEFileUtils::GetErrorString(ErrorString, 1);
		ZETestCheckString(ErrorString, "Cannot convert error to string. ErrorId = 1.");

		ZEFileUtils::GetErrorString(ErrorString, 2);
		ZETestCheckString(ErrorString, "Cannot convert error to string. ErrorId = 2.");

		ZEFileUtils::GetErrorString(ErrorString, 3);
		ZETestCheckString(ErrorString, "Cannot convert error to string. ErrorId = 3.");
	}
}
