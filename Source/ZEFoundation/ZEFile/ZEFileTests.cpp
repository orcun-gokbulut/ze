//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFileTests.cpp
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
#include "ZEFile/ZEFile.h"
#include "ZEDS/ZEString.h"
#include <stdio.h>
#include "ZEError.h"
#include "ZEPathUtils.h"
#include "ZEPathManager.h"

void CreateFile(const ZEString Path)
{
	ZEFile* NewFile = new ZEFile();

	if (NewFile->Open(Path, ZE_FOM_READ_WRITE, ZE_FCM_CREATE))
		NewFile->Close();
}

ZE_TEST(ZEFile)
{
	ZE_TEST_ITEM("ZEFile::~ZEFile()")
	{
		ZEFile File;
		bool Opened = File.Open("Destructor.txt", ZE_FOM_READ, ZE_FCM_CREATE);
		bool Result = File.IsOpen();
		ZE_TEST_CHECK_ERROR(Result);

		File.~ZEFile();

		Result = File.IsOpen();
		ZE_TEST_CHECK_ERROR(!Result);

		remove("Destructor.txt");
	}

	ZE_TEST_ITEM("ZEFile::ZEFile()")
	{
		ZEFile File;

		ZE_TEST_CHECK_EQUAL(File.GetHandle(), NULL);
		ZE_TEST_CHECK_EQUAL(File.GetOpenMode(), ZE_FOM_READ);
		ZE_TEST_CHECK_EQUAL(File.GetCreationMode(), ZE_FCM_NONE);
	}

	ZE_TEST_ITEM("const ZEString& ZEFile::GetPath() const")
	{
		ZEFile File;

		const ZEString FilePath = File.GetPath();

		ZE_TEST_CASE("open file, then call ZEFile::GetPath()")
		{
			const ZEString Path = "GetPath.txt";

			bool Result = File.Open(Path, ZE_FOM_READ, ZE_FCM_CREATE);
			ZE_TEST_CHECK_ERROR(Result);

			const ZEString FilePath = File.GetPath();
			ZE_TEST_CHECK_STRING_EQUAL(FilePath, "GetPath.txt");

			ZEInt Closed = File.Close();
			remove(Path);
		}
	}

	ZE_TEST_ITEM("void* ZEFile::GetHandle() const")
	{
		ZEFile File;

		bool Result = File.Open("GetHandle.txt", ZE_FOM_READ, ZE_FCM_CREATE);
		ZE_TEST_CHECK_ERROR(Result);

		void* FileHandle = File.GetHandle();
		//FileHandle = File.File

		ZEInt Closed = File.Close();
		remove("GetHandle.txt");
	}

	ZE_TEST_ITEM("ZEFileOpenMode ZEFile::GetOpenMode() const")
	{
		ZEFile File;

		ZEFileOpenMode OpenMode = File.GetOpenMode();
		ZE_TEST_CHECK_EQUAL(OpenMode, ZE_FOM_READ);

		ZE_TEST_CASE("open file with open mode ZE_FOM_WRITE")
		{
			bool Result = File.Open("GetOpenMode.txt", ZE_FOM_WRITE, ZE_FCM_CREATE);
			ZE_TEST_CHECK_ERROR(Result);

			OpenMode = File.GetOpenMode();
			ZE_TEST_CHECK_EQUAL(OpenMode, ZE_FOM_WRITE);

			File.Close();
			remove("GetOpenMode.txt");
		}

		ZE_TEST_CASE("open file with open mode ZE_FOM_READ_WRITE")
		{
			bool Result = File.Open("GetOpenMode.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			ZE_TEST_CHECK_ERROR(Result);

			OpenMode = File.GetOpenMode();
			ZE_TEST_CHECK_EQUAL(OpenMode, ZE_FOM_READ_WRITE);

			File.Close();
			remove("GetOpenMode.txt");
		}
	}

	ZE_TEST_ITEM("ZEFileCreationMode ZEFile::GetCreationMode() const")
	{
		ZEFile File;

		ZEFileCreationMode CreationMode = File.GetCreationMode();
		ZE_TEST_CHECK_EQUAL(CreationMode, ZE_FCM_NONE);

		ZE_TEST_CASE("open file with creation mode ZE_FCM_CREATE")
		{
			bool Result = File.Open("GetCreationMode.txt", ZE_FOM_WRITE, ZE_FCM_CREATE);
			ZE_TEST_CHECK_ERROR(Result);

			CreationMode = File.GetCreationMode();
			ZE_TEST_CHECK_EQUAL(CreationMode, ZE_FCM_CREATE);

			File.Close();
			remove("GetCreationMode.txt");
		}

		ZE_TEST_CASE("open file with creation mode ZE_FCM_OVERWRITE")
		{
			bool Result = File.Open("GetCreationMode.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZE_TEST_CHECK_ERROR(Result);

			CreationMode = File.GetCreationMode();
			ZE_TEST_CHECK_EQUAL(CreationMode, ZE_FCM_OVERWRITE);

			File.Close();
			remove("GetCreationMode.txt");
		}
	}

	ZE_TEST_ITEM("ZEFile&	ZEFile::operator = (ZEFile& OtherFile)")
	{
		ZEFile File1;
		ZEFile File2;

		bool Result = File1.Open("File1.txt", ZE_FOM_READ, ZE_FCM_CREATE);
		ZE_TEST_CHECK_ERROR(Result);
		ZE_TEST_CHECK_EQUAL(File1.GetOpenMode(), ZE_FOM_READ);
		ZE_TEST_CHECK_EQUAL(File1.GetCreationMode(), ZE_FCM_CREATE);
		ZE_TEST_CHECK_STRING_EQUAL(File1.GetPath(), "File1.txt");

		File1.Close();
		remove("File1.txt");

		Result = File2.Open("File2.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ZE_TEST_CHECK_ERROR(Result);
		ZE_TEST_CHECK_EQUAL(File2.GetOpenMode(), ZE_FOM_READ_WRITE);
		ZE_TEST_CHECK_EQUAL(File2.GetCreationMode(), ZE_FCM_OVERWRITE);
		ZE_TEST_CHECK_STRING_EQUAL(File2.GetPath(), "File2.txt");

		ZEFile File = File1 = File2;
		ZE_TEST_CHECK_EQUAL(File.GetHandle(), File2.GetHandle());
		ZE_TEST_CHECK_EQUAL(File1.GetOpenMode(), ZE_FOM_READ_WRITE);
		ZE_TEST_CHECK_EQUAL(File1.GetCreationMode(), ZE_FCM_OVERWRITE);
		ZE_TEST_CHECK_STRING_EQUAL(File1.GetPath(), "File2.txt");

		File2.Close();
 		remove("File2.txt");
	}

	ZE_TEST_ITEM("bool ZEFile::Open(const ZEString& FilePath, const ZEFileOpenMode FileOpenMode, const ZEFileCreationMode FileCreationMode)")
	{
		ZEFile File;
		bool Result;

		ZE_TEST_CASE("for FileOpenMode ZE_FOM_READ and FileCreationMode ZE_FCM_NONE when file is not exist")
		{
			const ZEString Path = "FOM_Read FCM_NoneTest.txt";

			Result = File.Open(Path, ZE_FOM_READ, ZE_FCM_NONE);
			ZE_TEST_CHECK_ERROR(!Result);
		}

		ZE_TEST_CASE("for FileOpenMode ZE_FOM_READ and FileCreationMode ZE_FCM_NONE when file exists")
		{
			CreateFile("FOM_READ FCM_NONE.txt");
			const ZEString Path = "FOM_READ FCM_NONE.txt";

			Result = File.Open(Path, ZE_FOM_READ, ZE_FCM_NONE);
			ZE_TEST_CHECK_ERROR(Result);
			
			ZEInt Closed = File.Close();
			remove(Path);
		}

		ZE_TEST_CASE("for FileOpenMode ZE_FOM_READ and FileCreationMode ZE_FCM_CREATE")
		{
			const ZEString Path = "FOM_READ FCM_CREATE.txt";

			Result = File.Open(Path, ZE_FOM_READ, ZE_FCM_CREATE);
			ZE_TEST_CHECK_ERROR(Result);

			ZEInt Closed = File.Close();
			remove(Path);
		}

		ZE_TEST_CASE("for FileOpenMode ZE_FOM_READ and FileCreationMode ZE_FCM_OVERWRITE")
		{
			const ZEString Path = "FOM_READ FCM_OVERWRITE.txt";

			Result = File.Open(Path, ZE_FOM_READ, ZE_FCM_OVERWRITE);
			ZE_TEST_CHECK_ERROR(Result);

			ZEInt Closed = File.Close();
			remove(Path);
		}

		ZE_TEST_CASE("for FileOpenMode ZE_FOM_READ and FileCreationMode ZE_FCM_OVERWRITE when file exists")
		{
			CreateFile("FOM_READ FCM_OVERWRITE.txt");
			const ZEString Path = "FOM_READ FCM_OVERWRITE.txt";

			Result = File.Open(Path, ZE_FOM_READ, ZE_FCM_OVERWRITE);
			ZE_TEST_CHECK_ERROR(Result);

			ZEInt Closed = File.Close();
			remove(Path);
		}

		ZE_TEST_CASE("for FileOpenMode ZE_FOM_WRITE and FileCreationMode ZE_FCM_NONE when file is not exist")
		{
			const ZEString Path = "FOM_WRITE FCM_NONETest.txt";

			Result = File.Open(Path, ZE_FOM_WRITE, ZE_FCM_NONE);
			ZE_TEST_CHECK_ERROR(!Result);
		}

		ZE_TEST_CASE("for FileOpenMode ZE_FOM_WRITE and FileCreationMode ZE_FCM_NONE when file exists")
		{
			CreateFile("FOM_WRITE FCM_NONE.txt");
			const ZEString Path = "FOM_WRITE FCM_NONE.txt";

			Result = File.Open(Path, ZE_FOM_WRITE, ZE_FCM_NONE);
			ZE_TEST_CHECK_ERROR(Result);

			ZEInt Closed = File.Close();
			remove(Path);
		}

		ZE_TEST_CASE("for FileOpenMode ZE_FOM_WRITE and FileCreationMode ZE_FCM_CREATE")
		{
			const ZEString Path = "FOM_WRITE FCM_CREATE.txt";

			Result = File.Open(Path, ZE_FOM_WRITE, ZE_FCM_CREATE);
			ZE_TEST_CHECK_ERROR(Result);

			ZEInt Closed = File.Close();
			remove(Path);
		}

		ZE_TEST_CASE("for FileOpenMode ZE_FOM_WRITE and FileCreationMode ZE_FCM_OVERWRITE")
		{
			const ZEString Path = "FOM_WRITE FCM_OVERWRITE.txt";

			Result = File.Open(Path, ZE_FOM_WRITE, ZE_FCM_OVERWRITE);
			ZE_TEST_CHECK_ERROR(Result);

			ZEInt Closed = File.Close();
			remove(Path);
		}

		ZE_TEST_CASE("for FileOpenMode ZE_FOM_READ_WRITE and FileCreationMode ZE_FCM_NONE when file is not exist")
		{
			const ZEString Path = "FOM_READ_WRITE FCM_NONETest.txt";

			Result = File.Open(Path, ZE_FOM_READ_WRITE, ZE_FCM_NONE);
			ZE_TEST_CHECK_ERROR(!Result);
		}

		ZE_TEST_CASE("for FileOpenMode ZE_FOM_READ_WRITE and FileCreationMode ZE_FCM_NONE when file exists")
		{
			CreateFile("FOM_READ_WRITE FCM_NONE.txt");
			const ZEString Path = "FOM_READ_WRITE FCM_NONE.txt";

			Result = File.Open(Path, ZE_FOM_READ_WRITE, ZE_FCM_NONE);
			ZE_TEST_CHECK_ERROR(Result);

			ZEInt Closed = File.Close();
			remove(Path);
		}

		ZE_TEST_CASE("for FileOpenMode ZE_FOM_READ_WRITE and FileCreationMode ZE_FCM_CREATE")
		{
			const ZEString Path = "FOM_READ_WRITE FCM_CREATE.txt";

			Result = File.Open(Path, ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			ZE_TEST_CHECK_ERROR(Result);

			ZEInt Closed = File.Close();
			remove(Path);
		}

		ZE_TEST_CASE("for FileOpenMode ZE_FOM_READ_WRITE and FileCreationMode ZE_FCM_OVERWRITE")
		{
			const ZEString Path = "FOM_READ_WRITE FCM_OVERWRITE.txt";

			Result = File.Open(Path, ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
			ZE_TEST_CHECK_ERROR(Result);

			ZEInt Closed = File.Close();
			remove(Path);
		}
	}

	ZE_TEST_ITEM("bool ZEFile::IsOpen() const")
	{
		ZEFile File;

		bool Result = File.IsOpen();
		ZE_TEST_CHECK_ERROR(!Result);

		ZE_TEST_CASE("for open file")
		{
			const ZEString Path = "IsOpen.txt";

			bool Opened = File.Open(Path, ZE_FOM_READ, ZE_FCM_CREATE);
			ZE_TEST_CHECK_ERROR(Opened);

			Result = File.IsOpen();
			ZE_TEST_CHECK_ERROR(Result);

			ZEInt Closed = File.Close();
			remove(Path);
		}
	}

	ZE_TEST_ITEM("ZEInt ZEFile::Close()")
	{
		ZEFile File;

		ZEInt Result = File.Close();
		ZE_TEST_CHECK_EQUAL(Result, 0);

		ZE_TEST_CASE("call ZEFile::Close() after open the file")
		{
			const ZEString Path = "FileClose.txt";

			bool Opened = File.Open(Path, ZE_FOM_READ, ZE_FCM_CREATE);
			ZE_TEST_CHECK_ERROR(Opened);

			Result = File.Close();
			ZE_TEST_CHECK_EQUAL(Result, 0);
			remove(Path);
		}
	}

	ZE_TEST_ITEM("ZESize ZEFile::Write(const void* Buffer, const ZESize Size, const ZESize Count)")
	{
		unsigned char* Buffer = new unsigned char[1024];
		for ( ZEInt I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;

		ZE_TEST_CASE("try to write close file")
		{
			bool Opened = File.Open("WriteTests.txt", ZE_FOM_READ, ZE_FCM_NONE);
			ZE_TEST_CHECK_ERROR(!Opened);
			Opened = File.Open("WriteTests.txt", ZE_FOM_WRITE, ZE_FCM_NONE);
			ZE_TEST_CHECK_ERROR(!Opened);
			Opened = File.Open("WriteTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_NONE);
			ZE_TEST_CHECK_ERROR(!Opened);

			//ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
		}

		ZE_TEST_CASE("open file with FileOpenMode ZE_FOM_READ and FileCreationMode ZE_FCM_CREATE")
		{
			bool Opened = File.Open("WriteTest.txt", ZE_FOM_READ, ZE_FCM_CREATE);

			ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
			ZEInt Result = File.Flush();
			ZE_TEST_CHECK_EQUAL(WriteCount, 0);
			ZE_TEST_CHECK_EQUAL(Result, 0);

			File.Close();
			remove("WriteTest.txt");
		}

		ZE_TEST_CASE("open file with FileOpenMode ZE_FOM_READ and FileCreationMode ZE_FCM_OVERWRITE")
		{
			bool Opened = File.Open("WriteTest.txt", ZE_FOM_READ, ZE_FCM_OVERWRITE);

			ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
			ZEInt Result = File.Flush();
			ZE_TEST_CHECK_EQUAL(WriteCount, 0);
			ZE_TEST_CHECK_EQUAL(Result, 0);

			File.Close();
			remove("WriteTest.txt");
		}

		ZE_TEST_CASE("open file with FileOpenMode ZE_FOM_WRITE and FileCreationMode ZE_FCM_CREATE")
		{
			bool Opened = File.Open("WriteTest.txt", ZE_FOM_WRITE, ZE_FCM_CREATE);

			ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 500);
			ZEInt Result = File.Flush();
			ZE_TEST_CHECK_EQUAL(WriteCount, 500);
			ZE_TEST_CHECK_EQUAL(Result, 0);

			File.Close();
			remove("WriteTest.txt");
		}

		ZE_TEST_CASE("open file with FileOpenMode ZE_FOM_WRITE and FileCreationMode ZE_FCM_OVERWRITE")
		{
			bool Opened = File.Open("WriteTest.txt", ZE_FOM_WRITE, ZE_FCM_OVERWRITE);

			ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 500);
			ZEInt Result = File.Flush();
			ZE_TEST_CHECK_EQUAL(WriteCount, 500);
			ZE_TEST_CHECK_EQUAL(Result, 0);

			WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
			Result = File.Flush();
			ZE_TEST_CHECK_EQUAL(WriteCount, 1024);
			ZE_TEST_CHECK_EQUAL(Result, 0);

			File.Close();
			remove("WriteTest.txt");
		}

		ZE_TEST_CASE("open file with FileOpenMode ZE_FOM_READ_WRITE and FileCreationMode ZE_FCM_CREATE")
		{
			bool Opened = File.Open("WriteTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);

			ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
			ZEInt Result = File.Flush();
			ZE_TEST_CHECK_EQUAL(WriteCount, 1024);
			ZE_TEST_CHECK_EQUAL(Result, 0);

			File.Close();
			remove("WriteTest.txt");
		}

		ZE_TEST_CASE("open file with FileOpenMode ZE_FOM_READ_WRITE and FileCreationMode ZE_FCM_OVERWRITE")
		{
			bool Opened = File.Open("WriteTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);

			ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 600);
			ZEInt Result = File.Flush();
			ZE_TEST_CHECK_EQUAL(WriteCount, 600);
			ZE_TEST_CHECK_EQUAL(Result, 0);

			WriteCount = File.Write(Buffer, sizeof(unsigned char), 500);
			Result = File.Flush();
			ZE_TEST_CHECK_EQUAL(WriteCount, 500);
			ZE_TEST_CHECK_EQUAL(Result, 0);

			File.Close();
			remove("WriteTest.txt");
		}

		ZE_TEST_CASE("try to write exceed the buffer size")
		{
			bool Opened = File.Open("WriteTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);

			ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 1500);
			ZEInt Result = File.Flush();
			ZE_TEST_CHECK_EQUAL(WriteCount, 1500);
			ZE_TEST_CHECK_EQUAL(Result, 0);

			File.Close();

			Opened = File.Open("WriteTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			WriteCount = File.Write(Buffer, sizeof(unsigned char), 1500);
			Result = File.Flush();
			ZE_TEST_CHECK_EQUAL(WriteCount, 1500);
			ZE_TEST_CHECK_EQUAL(Result, 0);

			File.Close();
			remove("WriteTest.txt");
		}
	}

	ZE_TEST_ITEM("ZEInt ZEFile::Flush() const")
	{
		unsigned char* Buffer = new unsigned char[1024];
		for ( ZEInt I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;

		bool Opened = File.Open("Flush.txt", ZE_FOM_WRITE, ZE_FCM_CREATE);

		ZEInt Result = File.Flush();
		ZEInt64 Size = File.GetSize();
		ZE_TEST_CHECK_EQUAL(Result, 0);
		ZE_TEST_CHECK_EQUAL(Size, 0);

		ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 3);
		Result = File.Flush();
		Size = File.GetSize();
		ZE_TEST_CHECK_EQUAL(WriteCount, 3);
		ZE_TEST_CHECK_EQUAL(Result, 0);
		ZE_TEST_CHECK_EQUAL(Size, 3);

		File.Close();
		remove("Flush.txt");
	}

	ZE_TEST_ITEM("ZEInt64 ZEFile::GetSize()")
	{
		unsigned char* Buffer = new unsigned char[1024];
		for ( ZEInt I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;

		ZE_TEST_CASE("for open file")
		{
			bool Opened = File.Open("GetSize.txt", ZE_FOM_WRITE, ZE_FCM_CREATE);

			ZEInt64 FileSize = File.GetSize();
			ZE_TEST_CHECK_EQUAL(FileSize, 0);
		}

		ZE_TEST_CASE("call ZEFile::Write() and ZEFile::GetSize() respectively")
		{
			ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 100);
			ZEInt Result = File.Flush();
			ZE_TEST_CHECK_EQUAL(WriteCount, 100);
			ZE_TEST_CHECK_EQUAL(Result, 0);

			ZEInt64 FileSize = File.GetSize();
			ZE_TEST_CHECK_EQUAL(FileSize, 100);
		}

		File.Close();
		remove("GetSize.txt");
	}

	ZE_TEST_ITEM("ZEInt ZEFile::Seek(const ZEInt64 Offset, const ZESeekFrom Origin)")
	{
		unsigned char* Buffer = new unsigned char[1024];
		for ( ZEInt I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;

		ZE_TEST_CASE("for SeekForm ZE_SF_CURRENT")
		{
			bool Opened = File.Open("SeekTest.txt", ZE_FOM_WRITE, ZE_FCM_CREATE);
			ZEInt64 CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 0);

			ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 5);
			ZEInt Result = File.Flush();
			ZE_TEST_CHECK_EQUAL(WriteCount, 5);
			ZE_TEST_CHECK_EQUAL(Result, 0);
			CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 5);

			ZEInt Seek = File.Seek(-1 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZE_TEST_CHECK_EQUAL(Seek, 0);
			CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 4);

			Seek = File.Seek(-4 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZE_TEST_CHECK_EQUAL(Seek, 0);
			CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 0);

			Seek = File.Seek(5 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZE_TEST_CHECK_EQUAL(Seek, 0);
			CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 5);

			Seek = File.Seek(10 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZE_TEST_CHECK_EQUAL(Seek, 0);
			CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 15);

			Seek = File.Seek(-20 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZE_TEST_CHECK_ERROR(Seek != 0);
			CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 15);

			Seek = File.Seek(1500 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZE_TEST_CHECK_EQUAL(Seek, 0);
			CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 1515);

			File.Close();
			remove("SeekTest.txt");
		}

		ZE_TEST_CASE("for SeekForm ZE_SF_BEGINING")
		{
			bool Opened = File.Open("SeekTest.txt", ZE_FOM_WRITE, ZE_FCM_CREATE);
			ZEInt64 CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 0);

			ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 5);
			ZEInt Result = File.Flush();
			CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 5);

			ZEInt Seek = File.Seek(0 * (ZEInt64)sizeof(unsigned char), ZE_SF_BEGINING);
			ZE_TEST_CHECK_EQUAL(Seek, 0);
			CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 0);

			Seek = File.Seek(5 * (ZEInt64)sizeof(unsigned char), ZE_SF_BEGINING);
			ZE_TEST_CHECK_EQUAL(Seek, 0);
			CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 5);

			Seek = File.Seek(-3 * (ZEInt64)sizeof(unsigned char), ZE_SF_BEGINING);
			ZE_TEST_CHECK_ERROR(Seek != 0);
			CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 5);

			Seek = File.Seek(10 * (ZEInt64)sizeof(unsigned char), ZE_SF_BEGINING);
			ZE_TEST_CHECK_EQUAL(Seek, 0);
			CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 10);

			File.Close();
			remove("SeekTest.txt");
		}

		ZE_TEST_CASE("for SeekForm ZE_SF_END")
		{
			bool Opened = File.Open("SeekTest.txt", ZE_FOM_WRITE, ZE_FCM_CREATE);
			ZEInt64 CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 0);

			ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 5);
			ZEInt Result = File.Flush();
			CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 5);

			ZEInt Seek = File.Seek(-1 * (ZEInt64)sizeof(unsigned char), ZE_SF_END);
			ZE_TEST_CHECK_EQUAL(Seek, 0);
			CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 4);

			Seek = File.Seek(-5 * (ZEInt64)sizeof(unsigned char), ZE_SF_END);
			ZE_TEST_CHECK_EQUAL(Seek, 0);
			CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 0);

			Seek = File.Seek(-10 * (ZEInt64)sizeof(unsigned char), ZE_SF_END);
			ZE_TEST_CHECK_ERROR(Seek != 0);
			CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 0);

			Seek = File.Seek(1 * (ZEInt64)sizeof(unsigned char), ZE_SF_END);
			ZE_TEST_CHECK_EQUAL(Seek, 0);
			CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 6);

			Seek = File.Seek(-10 * (ZEInt64)sizeof(unsigned char), ZE_SF_END);
			ZE_TEST_CHECK_ERROR(Seek != 0);
			CursorPosition = File.Tell();
			ZE_TEST_CHECK_EQUAL(CursorPosition, 6);

			File.Close();
			remove("SeekTest.txt");
		}
	}

	ZE_TEST_ITEM("ZEInt64 ZEFile::Tell() const")
	{
		unsigned char* Buffer = new unsigned char[1024];
		for ( ZEInt I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;

		bool Opened = File.Open("TellTest.txt", ZE_FOM_WRITE, ZE_FCM_CREATE);

		ZEInt64 CursorPosition = File.Tell();
		ZE_TEST_CHECK_EQUAL(CursorPosition, 0);

		ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 10); 
		ZEInt Result = File.Flush();
		CursorPosition = File.Tell();
		ZE_TEST_CHECK_EQUAL(CursorPosition, 10);

		ZEInt Seek = File.Seek(-3 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
		CursorPosition = File.Tell();
		ZE_TEST_CHECK_EQUAL(CursorPosition, 7);

		Seek = File.Seek(-5 * (ZEInt64)sizeof(unsigned char), ZE_SF_END);
		CursorPosition = File.Tell();
		ZE_TEST_CHECK_EQUAL(CursorPosition, 5);

		Seek = File.Seek(3 * (ZEInt64)sizeof(unsigned char), ZE_SF_BEGINING);
		CursorPosition = File.Tell();
		ZE_TEST_CHECK_EQUAL(CursorPosition, 3);

		Seek = File.Seek(-4 * (ZEInt64)sizeof(unsigned char), ZE_SF_BEGINING);
		ZE_TEST_CHECK_ERROR(Seek != 0);
		CursorPosition = File.Tell();
		ZE_TEST_CHECK_EQUAL(CursorPosition, 3);

		File.Close();
		remove("TellTest.txt");
	}

	ZE_TEST_ITEM("ZESize ZEFile::Read(void* Buffer, const ZESize Size, const ZESize Count)")
	{
		unsigned char* Buffer = new unsigned char[1024];
		unsigned char* BufferRead = new unsigned char[1024];
		for ( ZEInt I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;

		bool Opened = File.Open("ReadTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
		ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 10);
		ZEInt Result = File.Flush();

		ZEInt Seek = File.Seek(-10 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

		ZESize ReadCount = File.Read(BufferRead, sizeof(unsigned char), 10);
		ZE_TEST_CHECK_EQUAL(ReadCount, 10);

		ZEInt Res = memcmp(Buffer, BufferRead, sizeof(unsigned char));
		ZE_TEST_CHECK_EQUAL(Res, 0);

		File.Close();
		remove("ReadTest.txt");

		ZE_TEST_CASE("try to read whole file")
		{
			Opened = File.Open("ReadTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
			Result = File.Flush();

			Seek = File.Seek(-1024 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ReadCount = File.Read(BufferRead, sizeof(unsigned char), 1024);
			ZE_TEST_CHECK_EQUAL(ReadCount, 1024);

			Res = memcmp(Buffer, BufferRead, sizeof(unsigned char));
			ZE_TEST_CHECK_EQUAL(Res, 0);

			File.Close();
			remove("ReadTest.txt");
		}

		ZE_TEST_CASE("try to read a part of file")
		{
			Opened = File.Open("ReadTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
			Result = File.Flush();

			Seek = File.Seek(-100 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ReadCount = File.Read(BufferRead, sizeof(unsigned char), 100);
			ZE_TEST_CHECK_EQUAL(ReadCount, 100);

			File.Close();
			remove("ReadTest.txt");
		}
	}

	ZE_TEST_ITEM("ZEInt ZEFile::Eof() const")
	{
		unsigned char* Buffer = new unsigned char[1024];
		unsigned char* BufferRead = new unsigned char[1024];
		for (ZEInt I = 0 ; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;

		bool Opened = File.Open("EofTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
		ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
		ZEInt Result = File.Flush();

		ZEInt IsEof = File.Eof();
		ZE_TEST_CHECK_EQUAL(IsEof, 0);

		ZEInt Seek = File.Seek(-1024 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
		ZEInt64 Cursor = File.Tell();
		ZE_TEST_CHECK_EQUAL(Cursor, 0);
		ZESize ReadCount = File.Read(BufferRead, sizeof(unsigned char), 1025);
		ZE_TEST_CHECK_EQUAL(ReadCount, 1024);

		IsEof = File.Eof();
		ZE_TEST_CHECK_ERROR(IsEof != 0);

		Seek = File.Seek(-1024 * (ZEInt64)sizeof(unsigned char), ZE_SF_END);
		Cursor = File.Tell();
		ZE_TEST_CHECK_EQUAL(Cursor, 0);
		ReadCount = File.Read(BufferRead, sizeof(unsigned char), 1024);
		ZE_TEST_CHECK_EQUAL(ReadCount, 1024);

		IsEof = File.Eof();
		ZE_TEST_CHECK_EQUAL(IsEof, 0);

		File.Close();
		remove("EofTest.txt");
	}

	ZE_TEST_ITEM("bool ZEFile::ReadFile(const ZEString& FilePath, void* Buffer, const ZESize BufferSize)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		unsigned char* Buffer = new unsigned char[1024];
		unsigned char* BufferRead = new unsigned char[1024];
		for (ZEInt I = 0 ; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;

		bool Opened = File.Open("ReadFile.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
		ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
		ZE_TEST_CHECK_EQUAL(WriteCount, 1024);
		File.Close();

		bool Result = ZEFile::ReadFile("ReadFile.txt", (void*)BufferRead, 1024);
		ZE_TEST_CHECK_ERROR(!Result);

		ZEInt CompResult = memcmp(Buffer, BufferRead, sizeof(unsigned char) * 1024);
		ZE_TEST_CHECK_EQUAL(CompResult, -1);
		remove("ReadFile.txt");

		ZE_TEST_CASE("open file with resources path in the begining of the file")
		{
			Opened = File.Open(ZEPathManager::GetResourcesPath() + Seperator + "ReadFile.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
			ZE_TEST_CHECK_EQUAL(WriteCount, 1024);
			File.Close();

			Result = ZEFile::ReadFile("ReadFile.txt", (void*)BufferRead, 1024);
			ZE_TEST_CHECK_ERROR(Result);

			CompResult = memcmp(Buffer, BufferRead, sizeof(unsigned char) * 1024);
			ZE_TEST_CHECK_EQUAL(CompResult, 0);
			remove(ZEPathManager::GetResourcesPath() + Seperator + "ReadFile.txt");
		}
	}

	ZE_TEST_ITEM("bool ZEFile::ReadTextFile(const ZEString& FilePath, char* Buffer, const ZESize BufferSize)")
	{
		ZEString Seperator = ZEPathUtils::GetSeperator();

		unsigned char* Buffer = new unsigned char[1025];
		for ( ZEInt I = 0; I < 1025; I++)
		{
		 	Buffer[I] = I % 256;
		}
 		ZEFile File;
 		bool Opened = File.Open("ReadTextFile.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
		ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
		File.Flush();
		File.Close();

		bool Result = ZEFile::ReadTextFile("ReadTextFile.txt", (char*)Buffer, 1025);
		ZE_TEST_CHECK_ERROR(!Result);

		remove("ReadTextFile.txt");

		ZE_TEST_CASE("open file with resources path in the begining of the file")
		{
			Opened = File.Open(ZEPathManager::GetResourcesPath() + Seperator + "ReadTextFile.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			ZESize WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
			File.Flush();
			File.Close();

			Result = ZEFile::ReadTextFile("ReadTextFile.txt", (char*)Buffer, 1025);
			ZE_TEST_CHECK_ERROR(Result);

			remove(ZEPathManager::GetResourcesPath() + Seperator + "ReadTextFile.txt");
		}
	}
}
