//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFileInfoTests.cpp
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
#include "ZEFile/ZEFileInfo.h"
#include "ZEFile/ZEFile.h"
#include "ZEDS/ZEString.h"
#include <stdio.h>
#include "ZEError.h"
#include "ZEPathUtils.h"

ZETestSuite(ZEFileInfo)
{
	ZETest("ZEFileInfo::~ZEFileInfo()")
	{

	}

	ZETest("ZEFileInfo::ZEFileInfo()")
	{
		ZEFileInfo FileInfo;

	}

	ZETest("ZEFileInfo::ZEFileInfo(const ZEString& FilePath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEFileInfo FileInfo("resources" + Seperator + "FileInfo.txt");

		ZETestCheckString(FileInfo.GetName(), "FileInfo.txt");
		ZETestCheckString(FileInfo.GetPath(), "resources" + Seperator + "FileInfo.txt");
		ZETestCheckString(FileInfo.GetExtension(), ".txt");
	}

	ZETest("const ZEString& ZEFileInfo::GetName() const")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEFileInfo FileInfo("resources" + Seperator + "FileInfo.txt");

		const ZEString Name = FileInfo.GetName();
		ZETestCheckString(Name, "FileInfo.txt");
	}

	ZETest("void ZEFileInfo::SetPath(const ZEString& FilePath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();
		ZEFileInfo FileInfo;

		FileInfo.SetPath("FileInfo.txt");
		ZETestCheckString(FileInfo.GetPath(), "FileInfo.txt");

		ZETestCase("set different FileInfo name")
		{
			ZEFileInfo FileInfo2("FileInfo2.txt");
			ZETestCheckString(FileInfo2.GetPath(), "FileInfo2.txt");

			FileInfo2.SetPath("FileInfo.txt");
			ZETestCheckString(FileInfo2.GetPath(), "FileInfo.txt");
		}

		ZETestCase("path with a directory and without an extension")
		{
			FileInfo.SetPath("resources" + Seperator + "FileInfo3");
			ZETestCheckString(FileInfo.GetPath(), "resources" + Seperator + "FileInfo3");
		}
	}

	ZETest("const ZEString&	ZEFileInfo::GetPath() const")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEFileInfo FileInfo("resources" + Seperator + "FileInfo.txt");
		const ZEString Path = FileInfo.GetPath();
		ZETestCheckString(Path, "resources" + Seperator + "FileInfo.txt");

		ZEFileInfo FileInfo2(ZEPathManager::GetResourcesPath() + Seperator + "FileInfo2.txt");
		const ZEString Path2 = FileInfo2.GetPath();
		ZETestCheckString(Path2, ZEPathManager::GetResourcesPath() + Seperator + "FileInfo2.txt");

		ZEFileInfo FileInfo3(ZEPathManager::GetResourcesPath());
		const ZEString Path3 = FileInfo3.GetPath();
		ZETestCheckString(Path3, ZEPathManager::GetResourcesPath());
	}

	ZETest("const ZEString&	ZEFileInfo::GetExtension() const")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEFileInfo FileInfo("resources" + Seperator + "FileInfo.txt");
		const ZEString Extension = FileInfo.GetExtension();
		ZETestCheckString(Extension, ".txt");

		ZEFileInfo FileInfo2(ZEPathManager::GetResourcesPath() + Seperator + "Info.test");
		const ZEString Extension2 = FileInfo2.GetExtension();
		ZETestCheckString(Extension2, ".test");
	}

	ZETest("ZEString ZEFileInfo::GetFileName(const ZEString& FilePath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEString FileName = ZEFileInfo::GetFileName("resources" + Seperator + "FileInfo.txt");
		ZETestCheckString(FileName, "FileInfo.txt");

		ZETestCase("FileInfo path without a directory")
		{
			FileName = ZEFileInfo::GetFileName("FileInfo2.txt");
			ZETestCheckString(FileName, "FileInfo2.txt");
		}

		ZETestCase("FileInfo path without an extension")
		{
			FileName = ZEFileInfo::GetFileName("FileInfo3");
			ZETestCheckString(FileName, "FileInfo3");
		}

		ZETestCase("FileInfo path with a directory and without an extension")
		{
			FileName = ZEFileInfo::GetFileName("resources" + Seperator + "FileInfo4");
			ZETestCheckString(FileName, "FileInfo4");
		}

		ZETestCase("with an absolute path")
		{
			FileName = ZEFileInfo::GetFileName(ZEPathManager::GetResourcesPath());
			ZETestCheckString(FileName, "resources");
		}

		ZETestCase("with an absolute path with a file")
		{
			FileName = ZEFileInfo::GetFileName(ZEPathManager::GetResourcesPath() + Seperator + "Info.txt");
			ZETestCheckString(FileName, "Info.txt");
		}
	}

	ZETest("ZEString ZEFileInfo::GetFileExtension(const ZEString& FilePath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEString FileExtension = ZEFileInfo::GetFileExtension("resources" + Seperator + "FileInfo.txt");
		ZETestCheckString(FileExtension, ".txt");

		ZETestCase("FileInfo path without a directory")
		{
			FileExtension = ZEFileInfo::GetFileExtension("FileInfo2.test");
			ZETestCheckString(FileExtension, ".test");
		}

		ZETestCase("FileInfo path without an extension")
		{
			FileExtension = ZEFileInfo::GetFileExtension("resources\\FileInfo3");
			//ZETestCheckString(FileExtension, "");
		}

		ZETestCase("FileInfo path with an absolute path with an extension")
		{
			FileExtension = ZEFileInfo::GetFileExtension(ZEPathManager::GetResourcesPath() + Seperator + "Info.txt");
			ZETestCheckString(FileExtension, ".txt");
		}
	}

	ZETest("ZEString ZEFileInfo::GetParentDirectory(const ZEString& DirectoryPath)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		ZEString ParentDirectory = ZEFileInfo::GetParentDirectory("resources" + Seperator + "FileInfo.txt");
		ZETestCheckString(ParentDirectory, "resources");

		ZETestCase("FileInfo path without an extension")
		{
			ParentDirectory = ZEFileInfo::GetParentDirectory("resources" + Seperator + "FileInfo2");
			ZETestCheckString(ParentDirectory, "resources");
		}

		ZETestCase("for working directory")
		{
			ParentDirectory = ZEFileInfo::GetParentDirectory(ZEPathManager::GetWorkingDirectory());
			ZETestCheckString(ParentDirectory, ZEPathUtils::GetSimplifiedPath(ZEPathManager::GetWorkingDirectory() + Seperator + "..", false));
		}

		ZETestCase("for file of resources path")
		{
			ParentDirectory = ZEFileInfo::GetParentDirectory(ZEPathManager::GetResourcesPath() + Seperator + "Info.txt");
			ZETestCheckString(ParentDirectory, ZEPathManager::GetResourcesPath());
		}

		ZETestCase("FileInfo path without a directory")
		{
			ParentDirectory = ZEFileInfo::GetParentDirectory("FileInfo3.txt");
			//ZETestCheckString(ParentDirectory, "");
		}
	}

	ZETest("ZEInt64 ZEFileInfo::GetSize()")
	{
		unsigned char* Buffer = new unsigned char[1024];
		for ( ZEInt I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFileInfo FileInfo("FileInfo.txt");
		ZEFile File;
		bool Opened = File.Open("FileInfo.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 15);
		File.Flush();
		ZEInt64 FileSize = File.GetSize();
		File.Close();

		ZEInt64 Size = FileInfo.GetSize();
		ZETestCheckEqual(Size, 15);

		remove("FileInfo.txt");

		ZETestCase("call function without write to file")
		{
			ZEFileInfo Info("Info.txt");

			Size = Info.GetSize();
			ZETestCheckEqual(Size, -1);
		}

		ZETestCase("call function after open the file")
		{
			ZEFileInfo Info("Info.txt");
			File.Open("Info.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

			Size = Info.GetSize();
			ZETestCheckEqual(Size, 0);

			File.Close();
			remove("Info.txt");
		}
	}

	ZETest("ZEInt64 ZEFileInfo::GetFileSize(const ZEString& FilePath)")
	{
		unsigned char* Buffer = new unsigned char[1024];
		for (ZEInt I = 0 ; I < 1024 ; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFileInfo FileInfo("FileInfo.txt");
		ZEFile File;
		bool Opened = File.Open("FileInfo.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 15);
		File.Flush();
		ZEInt64 Size = File.GetSize();
		File.Close();

		ZEInt64 FileSize = FileInfo.GetFileSize("FileInfo.txt");
		ZETestCheckEqual(FileSize, 15);

		remove("FileInfo.txt");

		ZETestCase("call function without write to file")
		{
			ZEFileInfo Info("Info.txt");

			Size = Info.GetFileSize("Info.txt");
			ZETestCheckEqual(Size, -1);
		}

		ZETestCase("call function after open the file")
		{
			ZEFileInfo Info("Info.txt");
			File.Open("Info.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

			Size = Info.GetFileSize("Info.txt");
			ZETestCheckEqual(Size, 0);

			File.Close();
			remove("Info.txt");
		}
	}

	ZETest("bool ZEFileInfo::IsFile(const ZEString& FilePath)")
	{
		bool IsFile = ZEFileInfo::IsFile("FileInfo.txt");
		ZETestCheck(!IsFile);

		ZETestCase("open a file with the file info path")
		{
			ZEFile File;
			bool Opened = File.Open("FileInfo.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

			IsFile = ZEFileInfo::IsFile("FileInfo.txt");
			ZETestCheck(IsFile);

			File.Close();
			remove("FileInfo.txt");
		}
	}

	ZETest("bool ZEFileInfo::GetCreationDate(ZEFileTime& Time)")
	{
		ZEFileTime Time;
		ZEFileInfo FileInfo("FileInfo.txt");

		bool Created = FileInfo.GetCreationDate(Time);
		ZETestCheck(!Created);

		ZETestCase("open a file through the FileInfo path")
		{
			ZEFile File;
			File.Open("FileInfo.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

			Created = FileInfo.GetCreationDate(Time);
			ZETestCheck(Created);
			//Time will be the current time

			File.Close();
			remove("FileInfo.txt");
		}
	}

	ZETest("bool ZEFileInfo::GetModificationDate(ZEFileTime& Time)")
	{
		ZEFileTime Time;
		ZEFileInfo FileInfo("FileInfo.txt");

		bool Modified = FileInfo.GetModificationDate(Time);
		ZETestCheck(!Modified);

		ZETestCase("open a file through the FileInfo path")
		{
			ZEFile File;
			File.Open("FileInfo.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

			Modified = FileInfo.GetModificationDate(Time);
			ZETestCheck(Modified);
			//Time will be the current time

			File.Close();
			remove("FileInfo.txt");
		}
	}
}
