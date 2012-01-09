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


#include "ZETest.h"
#include "ZEFile/ZEFile.h"
#include "ZEDS/ZEString.h"
#include <stdio.h>
#include "ZEError.h"

void CreateFile(const char* Path)
{
	ZEFile* NewFile = new ZEFile();
	
	if (NewFile->Open(Path, ZE_FM_READ_WRITE, true))
		NewFile->Close();
}

void DeleteFile(const char* Path)
{
	zeAssert(remove(Path) != 0, "Cannot Delete File");
}

ZETestSuite(ZEFile)
{
	ZETest("bool ZEFile::Open(const ZEString& FilePath, ZEFileMode Mode, bool Binary)")
	{
		ZEFile File;

		ZETestCase("for file mode ZE_FM_READ_ONLY")
		{
			CreateFile("ZEFileReadOnlyTests.txt");
			
			const ZEString FilePath = "ZEFileReadOnlyTests.txt";
			bool Binary = true;

			bool Result = File.Open(FilePath, ZE_FM_READ_ONLY, Binary);
			ZETestCheck(Result == true);
			
			bool isopen = File.IsOpen();
			File.Close();
			isopen = File.IsOpen();
			remove("ZEFileReadOnlyTests.txt");
		}

		ZETestCase("for file mode ZE_FM_READ_ONLY when file is not exist")
		{
			bool Result = File.Open("ZEFileReadOnlyTestsCase.txt", ZE_FM_READ_ONLY, true);
			ZETestCheck(Result == false);
		}

		ZETestCase("for file mode ZE_FM_WRITE_ONLY")
		{
			CreateFile("ZEFileWriteOnlyTests.txt");
			
			bool Result = File.Open("ZEFileWriteOnlyTests.txt", ZE_FM_WRITE_ONLY, true);
			ZETestCheck(Result == true);

			bool isopen = File.IsOpen();
			File.Close();
			isopen = File.IsOpen();
			remove("ZEFileWriteOnlyTests.txt");
		}

		ZETestCase("for file mode ZE_FM_APPEND_ONLY")
		{
			CreateFile("ZEFileAppendOnlyTests.txt");
	
			bool Result = File.Open("ZEFileAppendOnlyTests.txt", ZE_FM_APPEND_ONLY, true);
			ZETestCheck(Result == true);

			bool isopen = File.IsOpen();
			File.Close();
			isopen = File.IsOpen();
			remove("ZEFileAppendOnlyTests.txt");
		}

		ZETestCase("for file mode ZE_FM_READ_APPEND")
		{
			CreateFile("ZEFileReadAppendTests.txt");

			bool Result = File.Open("ZEFileReadAppendTests.txt", ZE_FM_READ_APPEND, true);
			ZETestCheck(Result == true);

			bool isopen = File.IsOpen();
			File.Close();
			isopen = File.IsOpen();
			remove("ZEFileReadAppendTests.txt");
		}

		ZETestCase("for file mode ZE_FM_READ_WRITE")
		{
			CreateFile("ZEFileReadWriteTests.txt");
			
			bool Result = File.Open("ZEFileReadWriteTests.txt", ZE_FM_READ_WRITE, true);
			ZETestCheck(Result == true);

			bool isopen = File.IsOpen();
			File.Close();
			isopen = File.IsOpen();
			remove("ZEFileReadWriteTests.txt");
		}
	}
	ZETest("bool ZEFile::IsOpen()")
	{
		ZEFile File;

		bool Result = File.IsOpen();
		ZETestCheck(Result == false);

		ZETestCase("for open file")
		{
			File.Open("ZEFileIsOpenTests.txt", ZE_FM_READ_WRITE, true);
			
			Result = File.IsOpen();
			ZETestCheck(Result == true);
			File.Close();
			Result = File.IsOpen();
			ZETestCheck(Result == false);
			remove("ZEFileIsOpenTests.txt");
		}
	}
	ZETest("ZEQWORD ZEFile::Write(const void* Buffer, ZEQWORD Size, ZEQWORD Count)")
	{

		unsigned char* Buffer = new unsigned char[1024];
		for ( int I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;

		ZETestCase("write for close file")
		{
			File.Open("ZEFileWriteTests.txt", ZE_FM_READ_WRITE, true);
			File.Close();

			ZEQWORD WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
			ZETestCheckEqual(WriteCount, 0);
			remove("ZEFileWriteTests.txt");
		}

		ZETestCase("write for file mode ZE_FM_READ_WRITE")
		{
			File.Open("ZEFileWriteTests.txt", ZE_FM_READ_WRITE, true);
		
			ZEQWORD WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
			File.Flush();
			ZETestCheckEqual(WriteCount, 1024);
			File.Close();
			remove("ZEFileWriteTests.txt");
		}

		ZETestCase("write for file mode ZE_FM_WRITE_ONLY")
		{
			File.Open("ZEFileWriteTests.txt", ZE_FM_WRITE_ONLY, true);

			ZEQWORD WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
			File.Flush();

			ZETestCheckEqual(WriteCount, 1024);
			ZEQWORD Size = File.GetFileSize();
			ZETestCheckEqual(Size, 1024);

			bool Result = File.Seek(-24 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZEQWORD CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 1000);
			WriteCount = File.Write(Buffer, sizeof(unsigned char), 28);
			Size = File.GetFileSize();
			ZETestCheckEqual(Size, 1028);
			CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 1028);
			
			File.Close();
			remove("ZEFileWriteTests.txt");
		}	

		ZETestCase("write for file mode ZE_FM_READ_ONLY")
		{
			CreateFile("ZEFileWriteTests.txt");
			
			File.Open("ZEFileWriteTests.txt", ZE_FM_READ_ONLY, true);

			ZEQWORD WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
			File.Flush();
			ZETestCheckEqual(WriteCount, 0);
			bool isopen = File.IsOpen();
			ZETestCheck(isopen == true);
			File.Close();
			isopen = File.IsOpen();
			ZETestCheck(isopen == false);
			
			remove("ZEFileWriteTests.txt");
		}

		ZETestCase("write for file mode ZE_FM_APPEND_ONLY")
		{
			File.Open("ZEFileWriteTests.txt", ZE_FM_APPEND_ONLY, true);

			ZEQWORD WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
			File.Flush();
			ZETestCheckEqual(WriteCount, 1024);
			ZEQWORD Size = File.GetFileSize();
			ZETestCheckEqual(Size, 1024);

			WriteCount = File.Write(Buffer, sizeof(unsigned char), 100);
			ZETestCheckEqual(WriteCount, 100);
			Size = File.GetFileSize();
			ZETestCheckEqual(Size, 1124);

			ZEQWORD CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 1124);
			bool Result = File.Seek(-100 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);
			CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 1024);
			WriteCount = File.Write(Buffer, sizeof(unsigned char), 50);
			Size = File.GetFileSize();
			ZETestCheckEqual(Size, 1174);
			
			File.Close();
			remove("ZEFileWriteTests.txt");
		}

		ZETestCase("write for file mode ZE_FM_READ_APPEND")
		{
			File.Open("ZEFileWriteTests.txt", ZE_FM_READ_APPEND, true);

			ZEQWORD WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
			File.Flush();
			ZETestCheckEqual(WriteCount, 1024);
			ZEQWORD Size = File.GetFileSize();
			ZETestCheckEqual(Size, 1024);

			ZEQWORD CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 1024);
			bool Result = File.Seek(-100 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);
			CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 924);
			WriteCount = File.Write(Buffer, sizeof(unsigned char), 50);
			Size = File.GetFileSize();
			ZETestCheckEqual(Size, 1074);
			
			File.Close();
			remove("ZEFileWriteTests.txt");
		}
	}
	ZETest("void ZEFile::Flush()")
	{
		unsigned char* Buffer = new unsigned char[1024];
		for ( int I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;
		File.Open("ZEFileFlushTests.txt", ZE_FM_APPEND_ONLY, true);
		ZEQWORD Size = File.GetFileSize();
		ZETestCheckEqual(Size, 0);

		ZEQWORD WriteCount = File.Write(Buffer, sizeof(unsigned char), 100);
		
		File.Flush();
		ZETestCheckEqual(WriteCount, 100);
		Size = File.GetFileSize();
		ZETestCheckEqual(Size, 100);
		File.Close();
		remove("ZEFileFlushTests.txt");
	}
	ZETest("ZEQWORD ZEFile::GetFileSize()")
	{
		ZEFile File;
		File.Open("ZEFileSizeTests.txt", ZE_FM_READ_WRITE, true);
		
		ZEQWORD Size = File.GetFileSize();
		ZETestCheckEqual(Size, 0);
		File.Close();
		remove("ZEFileSizeTests.txt");

		ZETestCase("for size != 0")
		{
			unsigned char* Buffer = new unsigned char[1024];
			for ( int I = 0; I < 1024; I++)
			{
				Buffer[I] = I % 256;
			}

			File.Open("ZEFileSizeTests.txt", ZE_FM_READ_WRITE, true);
			File.Write(Buffer, sizeof(unsigned char), 5);
			File.Flush();

			Size = File.GetFileSize();
			ZETestCheckEqual(Size, 5);
			File.Close();
			remove("ZEFileSizeTests.txt");
		}
	}
	ZETest("ZEQWORD ZEFile::GetStartPosition()")
	{
		unsigned char* Buffer = new unsigned char[1024];
		for ( int I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;
		File.Open("ZEFileStartPositionTests.txt", ZE_FM_WRITE_ONLY, true);

		ZEQWORD StartPosition = File.GetStartPosition();
		ZETestCheckEqual(StartPosition, 0);

		File.Write(Buffer, sizeof(unsigned char), 100);
		File.Flush();
		
		StartPosition = File.GetStartPosition();
		ZETestCheckEqual(StartPosition, 0);
		
		File.Close();
		remove("ZEFileStartPositionTests.txt");
	}
	ZETest("ZEQWORD ZEFile::GetEndPosition()")
	{
		unsigned char* Buffer = new unsigned char[1024];
		for ( int I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;
		File.Open("ZEFileEndPositionTests.txt", ZE_FM_WRITE_ONLY, true);

		ZEQWORD EndPosition = File.GetEndPosition();
		ZETestCheckEqual(EndPosition, 0);

		File.Write(Buffer, sizeof(unsigned char), 100);
		File.Flush();
		
		EndPosition = File.GetEndPosition();
		ZETestCheckEqual(EndPosition, 100);
		
		File.Close();
		remove("ZEFileEndPositionTests.txt");
		
	}
	ZETest("bool ZEFile::Seek(ZEINT64 Offset, ZESeekFrom Origin)")
	{
		unsigned char* Buffer = new unsigned char[1024];
		for ( int I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;

		ZETestCase("for seek form ZE_SF_CURRENT")
		{
			File.Open("ZEFileSeekTests.txt", ZE_FM_WRITE_ONLY, true);
			ZEQWORD CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 0);

			ZEQWORD WriteCount = File.Write(Buffer, sizeof(unsigned char), 5);
			CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 5);

			bool Result = File.Seek(-1 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZETestCheck(Result == true);
			CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 4);

			Result = File.Seek(-4 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZETestCheck(Result == true);
			CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 0);

			Result = File.Seek(5 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZETestCheck(Result == true);
			CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 5);

			Result = File.Seek(10 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZETestCheck(Result == true);
			CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 15);

			Result = File.Seek(-20 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZETestCheck(Result == false);
			CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 15);

			File.Close();
			remove("ZEFileSeekTests.txt");
		}
		
		ZETestCase("for seek form ZE_SF_BEGINING")
		{
			File.Open("ZEFileSeekTests.txt", ZE_FM_WRITE_ONLY, true);
			ZEQWORD CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 0);

			ZEQWORD WriteCount = File.Write(Buffer, sizeof(unsigned char), 5);
			CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 5);

			bool Result = File.Seek(0 * (ZEINT64)sizeof(unsigned char), ZE_SF_BEGINING);
			ZETestCheck(Result == true);
			CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 0);

			Result = File.Seek(5 * (ZEINT64)sizeof(unsigned char), ZE_SF_BEGINING);
			ZETestCheck(Result == true);
			CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 5);

			Result = File.Seek(-3 * (ZEINT64)sizeof(unsigned char), ZE_SF_BEGINING);
			ZETestCheck(Result == false);
			CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 5);
			
			Result = File.Seek(10 * (ZEINT64)sizeof(unsigned char), ZE_SF_BEGINING);
			ZETestCheck(Result == true);
			CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 10);

			File.Close();
			remove("ZEFileSeekTests.txt");
		}

		ZETestCase("for seek form ZE_SF_END")
		{
			File.Open("ZEFileSeekTests.txt", ZE_FM_WRITE_ONLY, true);
			ZEQWORD CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 0);

			ZEQWORD WriteCount = File.Write(Buffer, sizeof(unsigned char), 5);
			File.Flush();
			CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 5);

			bool Result = File.Seek(-1 * (ZEINT64)sizeof(unsigned char), ZE_SF_END);
			ZETestCheck(Result == true);
			CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 4);

			Result = File.Seek(-5 * (ZEINT64)sizeof(unsigned char), ZE_SF_END);
			ZETestCheck(Result == true);
			CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 0);

			Result = File.Seek(-10 * (ZEINT64)sizeof(unsigned char), ZE_SF_END);
			ZETestCheck(Result == false);
			CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 0);
			
			Result = File.Seek(1 * (ZEINT64)sizeof(unsigned char), ZE_SF_END);
			ZETestCheck(Result == true);
			CursorPosition = File.Tell();
			ZETestCheckEqual(CursorPosition, 6);

			
			File.Close();
			remove("ZEFileSeekTests.txt");
		}
	}

	ZETest("ZEQWORD ZEFile::Tell()")
	{
		unsigned char* Buffer = new unsigned char[1024];

		ZEFile File;
		File.Open("ZEFileTellTests.txt", ZE_FM_WRITE_ONLY, true);
		
		ZEQWORD CursorPosition = File.Tell();
		ZETestCheckEqual(CursorPosition, 0);

		for ( int I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEQWORD WriteCount = File.Write(Buffer, sizeof(unsigned char), 10); 
		File.Flush();
		bool Result = File.Seek(-3 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);
		CursorPosition = File.Tell();
		ZETestCheckEqual(CursorPosition, 7);
		
		Result = File.Seek(-5 * (ZEINT64)sizeof(unsigned char), ZE_SF_END);
		CursorPosition = File.Tell();
		ZETestCheckEqual(CursorPosition, 5);
		
		Result = File.Seek(3 * (ZEINT64)sizeof(unsigned char), ZE_SF_BEGINING);
		CursorPosition = File.Tell();
		ZETestCheckEqual(CursorPosition, 3);

		Result = File.Seek(-4 * (ZEINT64)sizeof(unsigned char), ZE_SF_BEGINING);
		ZETestCheck(Result == false);
		CursorPosition = File.Tell();
		ZETestCheckEqual(CursorPosition, 3);
		
		File.Close();
		remove("ZEFileTellTests.txt");
	}
	ZETest("void ZEFile::Close()")
	{
		ZEFile File;
		File.Open("ZEFileCloseTests.txt", ZE_FM_READ_WRITE, true);
		bool isopen = File.IsOpen();
		ZETestCheck(isopen == true);

		File.Close();
		isopen = File.IsOpen();
		ZETestCheck(isopen == false);
		remove("ZEFileCloseTests.txt");
	}
	ZETest("ZEQWORD ZEFile::Read(void* Buffer, ZEQWORD Size, ZEQWORD Count)")
	{
		unsigned char* Buffer = new unsigned char[1024];
		unsigned char* BufferRead = new unsigned char[1024];
		for ( int I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}
				
		ZEFile File;
		File.Open("ZEFileReadTests.txt", ZE_FM_READ_WRITE, true);
		ZEQWORD WriteCount = File.Write(Buffer, sizeof(unsigned char), 10);
		File.Flush();

		bool Result = File.Seek(-10 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);

		ZEQWORD ReadCount = File.Read(BufferRead, sizeof(unsigned char), 10);
		ZETestCheckEqual(ReadCount, 10);

		int Res = memcmp(Buffer, BufferRead, sizeof(unsigned char));
		ZETestCheckEqual(Res, 0);
		
		File.Close();
		remove("ZEFileReadTests.txt");

		ZETestCase("try to read whole file")
		{
			File.Open("ZEFileReadTests.txt", ZE_FM_READ_WRITE, true);
			WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
			File.Flush();

			Result = File.Seek(-1024 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);

			ReadCount = File.Read(BufferRead, sizeof(unsigned char), 1024);
			ZETestCheckEqual(ReadCount, 1024);

			Res = memcmp(Buffer, BufferRead, sizeof(unsigned char));
			ZETestCheckEqual(Res, 0);

			File.Close();
			remove("ZEFileReadTests.txt");
		}
	}
	ZETest("bool ZEFile::Eof()")
	{
		unsigned char* Buffer = new unsigned char[1024];
		unsigned char* BufferRead = new unsigned char[1024];
		for (int I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;
		File.Open("ZEFileEofTests.txt", ZE_FM_READ_WRITE, true);

		ZEQWORD WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
		File.Flush();

		bool IsEof = File.Eof();
		ZETestCheck(IsEof == false);

		bool Result = File.Seek(-1024 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);

		ZEQWORD ReadCount = File.Read(BufferRead, sizeof(unsigned char), 1025);
		IsEof = File.Eof();
		ZETestCheck(IsEof == true);

		File.Close();
		remove("ZEFileEofTests.txt");

	}
	ZETest("ZEFileType ZEFile::GetFileType() const")
	{
		ZEFile File;
		File.Open("ZEFileTypeTests.txt", ZE_FM_READ_WRITE, true);

		ZEFileType Type = File.GetFileType();
		ZETestCheck(Type == ZE_FT_FILE);
		
		File.Close();
		remove("ZEFileTypeTests.txt");
	}
	ZETest("unsigned int ZEFile::GetReferenceCount() const")
	{
		ZEFile File;
		File.Open("ZEFileReferenceCountTests.txt", ZE_FM_READ_WRITE, true);

		unsigned int ReferenceCount = File.GetReferenceCount();
		ZETestCheckEqual(ReferenceCount, 0);

		File.Close();
		remove("ZEFileReferenceCountTests.txt");
	}
	ZETest("ZEQWORD ZEFile::WriteFormated(const char* Format, ...)")
	{
		ZEFile File;
		bool Result = File.Open("ZEFileWriteFormatedTests.txt", ZE_FM_APPEND_ONLY, true);

		ZEQWORD WriteCount = File.WriteFormated("This Line Contains Int: %d, Double: %lf, Float: %f, String: %s", 11, 11.11, 11.11f, "eleven");
		File.Flush();
		ZETestCheckEqual(WriteCount, 79);
		
		File.Close();
		remove("ZEFileWriteFormatedTests.txt");
	}
	ZETest("void* ZEFile::GetFileHandle() const")
	{
		ZEFile File;
		bool Result = File.Open("ZEFileHandleTests.txt", ZE_FM_APPEND_ONLY, true);

		FILE* CFile = (FILE*)(File.GetFileHandle());
		//ZETestCheck(File == CFile);
		
		File.Close();
		remove("ZEFileHandleTests.txt");
	}
	ZETest("const ZEString ZEFile::GetFilePath() const")
	{
		ZEFile File;
		bool Result = File.Open("ZEFilePathTests.txt", ZE_FM_READ_WRITE, true);

		const ZEString FilePath = File.GetFilePath();
		ZETestCheckString(FilePath, "ZEFilePathTests.txt");

		File.Close();
		remove("ZEFilePathTests.txt");
	}
	ZETest("ZEFile& ZEFile::operator = (ZEFile& OtherFile)")
	{
		ZEFile File;
		ZEFile OtherFile;

		unsigned char* Buffer = new unsigned char[1024];
		for ( int I = 0; I < 100; I++)
		{
			Buffer[I] = I % 256;
		}

		OtherFile.Open("ZEFileOperatorTests.txt", ZE_FM_WRITE_ONLY, true);

		ZEQWORD WriteCount = OtherFile.Write(&Buffer, sizeof(unsigned char), 2);
		OtherFile.Flush();

		ZEQWORD CursorPosition = File.Tell();
		ZETestCheckEqual(CursorPosition, 0);

		File = OtherFile; 

		CursorPosition = File.Tell();
		ZETestCheckEqual(CursorPosition, 2);
		const ZEString FilePath = File.GetFilePath();
		ZETestCheckString(FilePath, "ZEFileOperatorTests.txt");
		FILE* CFile = (FILE*)(File.GetFileHandle());
		//ZETestCheck(OtherFile.File == CFile);

		File.Close();
		OtherFile.Close();
		remove("ZEFileOperatorTests.txt");

	}

		//////////////////////////////////////
		/* Static Functions of class ZEFile */
		//////////////////////////////////////

	ZETest("ZEString ZEFile::GetFileName(const ZEString& Path)")
	{
		const ZEString FilePath = "resources\\Items0\\FileTest0\\TestTest0\\ZEFileTests0.txt";

		ZEString FileName = ZEFile::GetFileName(FilePath);
		ZETestCheckString(FileName, "ZEFileTests0.txt");
	}
	ZETest("ZEString ZEFile::GetFileExtension(const ZEString& FilePath)")
	{
		const ZEString FilePath = "resources\\Items0\\FileTest0\\TestTest0\\ZEFileTests0.txt";

		ZEString FileExtension = ZEFile::GetFileExtension(FilePath);
		ZETestCheckString(FileExtension, ".txt");
	}
	ZETest("ZEString ZEFile::GetParentDirectory(const ZEString& FilePath)")
	{
		const ZEString FilePath = "resources\\Items0\\FileTest0\\TestTest0\\ZEFileTests0.txt";

		ZEString ParentDirectory = ZEFile::GetParentDirectory(FilePath);
		ZETestCheckString(ParentDirectory, "resources\\Items0\\FileTest0\\TestTest0");
	}
	ZETest("bool ZEFile::IsDirectoryExists(const ZEString& FilePath)")
	{
		const ZEString FilePath = "resources\\Items0\\FileTest0\\TestTest0\\ZEFileTests0.txt";

		bool IsDirectory = ZEFile::IsDirectoryExists(FilePath);
		ZETestCheck(IsDirectory == false);
	}
	ZETest("bool ZEFile::IsFileExists(const ZEString& FilePath)")
	{
		const ZEString FilePath = "resources\\Items0\\FileTest0\\TestTest0\\ZEFileTests0.txt";

		bool IsFile = ZEFile::IsFileExists(FilePath);
		ZETestCheck(IsFile == true);
	}
	ZETest("ZEQWORD ZEFile::GetFileSize(const ZEString& FilePath)")
	{
		const ZEString FilePath = "resources\\Items0\\FileTest0\\TestTest0\\ZEFileTests0.txt";

		ZEQWORD Size = ZEFile::GetFileSize(FilePath);
		//ZETestCheckEqual(Size, 0);
	}

	ZETest("bool ZEFile::ReadFile(const ZEString& FilePath, void* Buffer, ZEQWORD BufferSize)")
	{
		/*unsigned char* Buffer = new unsigned char[1024];
		unsigned char* BufferRead = new unsigned char[1024];
		for (int I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}
		ZEFile File;
		//ZEFile* FileTest = ZEFile::Open("resources\\ZEFileReadFileTests.txt");
		
		bool Res = File.Open("resources\\ZEFileReadFileTests.txt", ZE_FM_READ_WRITE, true);
		zeAssert(!Res, "Dosyay� acamad�m");
		ZEQWORD WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
		ZETestCheckEqual(WriteCount, 1024);
		File.Close();
		
		bool Result = ZEFile::ReadFile("ZEFileReadFileTests.txt", (void*)BufferRead, 1024);
		
		int CompResult = memcmp(Buffer, BufferRead, sizeof(unsigned char) * 1024);
		ZETestCheckEqual(CompResult, 0);


		remove("resources\\ZEFileReadFileTests.txt");*/
		
	}
	ZETest("bool ZEFile::ReadTextFile(const ZEString& FilePath, char* Buffer, ZEQWORD BufferSize)")
	{
		/*unsigned char* Buffer = new unsigned char[1025];
		for ( int I = 0; I < 1025; I++)
		{
			Buffer[I] = I % 256;
		}
		ZEFile File;
		bool Res = File.Open("resources\\ZEFileReadTextTests.txt", ZE_FM_READ_WRITE, true);
		ZEQWORD WriteCount = File.Write(Buffer, sizeof(unsigned char), 1024);
		File.Flush();
		File.Close();
		bool Result = ZEFile::ReadTextFile("ZEFileReadTextTests.txt", (char*)Buffer, 1025);
		
		remove("resources\\ZEFileReadTextTests.txt");*/
	}

	ZETest("ZEFile* ZEFile::Open(const ZEString& FilePath)")
	{
		/*ZEFile* File = ZEFile::Open("resources\\Items0\\FileTest0\\TestTest0\\ZEFileTests0.txt");
		zeAssert(!File, "Static open cal�smad�");*/
	}
}
