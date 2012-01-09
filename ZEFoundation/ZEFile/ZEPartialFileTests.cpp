//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPartialFileTests.cpp
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
#include "ZEFile/ZEPartialFile.h"
#include "ZEFile/ZEFile.h"
#include "ZEDS/ZEString.h"
#include <stdio.h>
#include "ZEError.h"



ZETestSuite(ZEPartialFile)
{
	ZETest("bool ZEPartialFile::Open(ZEFile* ParentFile, ZEQWORD Offset, ZEQWORD Size)")
	{
		ZEFile File;
		ZEPartialFile PartialFile;

		File.Open("ZEPartialFileOpenTests.txt", ZE_FM_READ_WRITE, true);

		bool Result = PartialFile.Open(&File, 1025, 100);
		ZETestCheck(Result == true);
		
		ZEQWORD StartPosition = PartialFile.GetStartPosition();
		ZETestCheckEqual(StartPosition, 1025);
		ZEQWORD EndPosition = PartialFile.GetEndPosition();
		ZETestCheckEqual(EndPosition, 1125);
		ZEQWORD Size = PartialFile.GetFileSize();
		ZETestCheckEqual(Size, 100);
		unsigned int ReferenceCount = File.GetReferenceCount();
		ZETestCheckEqual(ReferenceCount, 1);

		PartialFile.Close();
		remove("ZEPartialFileOpenTests.txt");
		
		ZETestCase("open more than 1 partial files")
		{
			ZEPartialFile PartialFile1;
			ZEPartialFile PartialFile2;
			ZEPartialFile PartialFile3;

			File.Open("ZEPartialFileOpenTests.txt", ZE_FM_READ_WRITE, true);

			bool Result1 = PartialFile1.Open(&File, 100, 100);
			ZETestCheck(Result1 == true);
			bool Result2 = PartialFile2.Open(&File, 200, 100);
			ZETestCheck(Result2 == true);
			bool Result3 = PartialFile3.Open(&File, 300, 100);
			ZETestCheck(Result3 == true);

			ReferenceCount = File.GetReferenceCount();
			ZETestCheckEqual(ReferenceCount, 3);

			PartialFile1.Close();
			PartialFile2.Close();
			PartialFile3.Close();
			remove("ZEPartialFileOpenTests.txt");	
		}

		ZETestCase("try to intersect the partial files")
		{
			ZEPartialFile PartialFile1;

			File.Open("ZEPartialFileOpenTests.txt", ZE_FM_READ_WRITE, true);

			Result = PartialFile.Open(&File, 100, 100);
			ZETestCheck(Result == true);
			StartPosition = PartialFile.GetStartPosition();
			ZETestCheckEqual(StartPosition, 100);
			EndPosition = PartialFile.GetEndPosition();
			ZETestCheckEqual(EndPosition, 200);
			Result = PartialFile1.Open(&File, 150, 100);
			ZETestCheck(Result == true);
			StartPosition = PartialFile1.GetStartPosition();
			ZETestCheckEqual(StartPosition, 150);
			EndPosition = PartialFile1.GetEndPosition();
			ZETestCheckEqual(EndPosition, 250);

			ReferenceCount = File.GetReferenceCount();
			ZETestCheckEqual(ReferenceCount, 2);

			PartialFile.Close();
			PartialFile1.Close();
			remove("ZEPartialFileOpenTests.txt");
		}

		ZETestCase("open partial file on a partial file")
		{
			ZEPartialFile PartialFile1;
			ZEPartialFile PartialFile2;

			File.Open("ZEPartialFileOpenTests.txt", ZE_FM_READ_WRITE, true);

			Result = PartialFile.Open(&File, 0, 100);
			ZETestCheck(Result == true);

			bool Result1 = PartialFile1.Open((ZEFile*)&PartialFile, 0, 50);
			ZETestCheck(Result1 == true);

			bool Result2 = PartialFile2.Open((ZEFile*)&PartialFile1, 50, 100);
			ZETestCheck(Result2 == true);

			PartialFile.Close();
			PartialFile1.Close();
			PartialFile2.Close();
			remove("ZEPartialFileOpenTests.txt");
		}
	}
	ZETest("ZEQWORD ZEPartialFile::Write(void* Buffer, ZEQWORD Size, ZEQWORD Count)")
	{
		unsigned char* Buffer = new unsigned char[1024];
		for ( int I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;
		ZEPartialFile PartialFile;

		File.Open("ZEPartialFileWriteTests.txt", ZE_FM_READ_WRITE, true);

		bool Result = PartialFile.Open(&File, 100, 100);	

		ZEQWORD PartialCursor = PartialFile.Tell();
		ZETestCheckEqual(PartialCursor, 0);
		ZEQWORD WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 10);
		ZETestCheckEqual(WriteCount, 10);
		PartialCursor = PartialFile.Tell();
		ZETestCheckEqual(PartialCursor, 10);
		ZEQWORD Size = PartialFile.GetFileSize();
		ZETestCheckEqual(Size, 100);
		ZEQWORD FileCursor = File.Tell();
		ZETestCheckEqual(FileCursor, 0);
		
		WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 90);
		ZETestCheckEqual(WriteCount, 90);
		PartialCursor = PartialFile.Tell();
		ZETestCheckEqual(PartialCursor, 100);
		bool isEof = PartialFile.Eof();
		ZETestCheck(isEof == false);
		
		WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 1);
		ZETestCheckEqual(WriteCount, 0);
		PartialCursor = PartialFile.Tell();
		ZETestCheckEqual(PartialCursor, 100);
		Size = PartialFile.GetFileSize();
		ZETestCheckEqual(Size, 100);
		isEof = PartialFile.Eof();
		ZETestCheck(isEof == true);
	
		PartialFile.Close();
		remove("ZEPartialFileWriteTests.txt");

		ZETestCase("try to write exceed the partial file size")
		{
			ZEPartialFile PartialFile1;

			File.Open("ZEPartialFileWriteTests.txt", ZE_FM_APPEND_ONLY, true);

			Result = PartialFile.Open(&File, 0, 50);
			ZETestCheck(Result == true);
			bool Result1 = PartialFile1.Open((ZEFile*)&PartialFile, 10, 30);
			ZETestCheck(Result1 == true);
			Size = PartialFile.GetFileSize();
			ZETestCheckEqual(Size, 50);
			ZEQWORD Size1 = PartialFile1.GetFileSize();
			ZETestCheckEqual(Size1, 30);
			unsigned int ReferenceCount = PartialFile.GetReferenceCount();
			ZETestCheckEqual(ReferenceCount, 1);
			ReferenceCount = File.GetReferenceCount();
			ZETestCheckEqual(ReferenceCount, 2);

			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 0);
			ZEQWORD PartialCursor1 = PartialFile1.Tell();
			ZETestCheckEqual(PartialCursor1, 0);

			WriteCount = PartialFile.Write(Buffer, 4, 9);
			isEof = PartialFile.Eof();
			ZETestCheckEqual(WriteCount, 9);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 36);
			PartialCursor1 = PartialFile1.Tell();
			ZETestCheckEqual(PartialCursor1, 0);
			WriteCount = PartialFile.Write(Buffer, 4, 4);
			isEof = PartialFile.Eof();
			ZETestCheckEqual(WriteCount, 3);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 48);
			isEof = PartialFile.Eof();
			ZETestCheck(isEof == true);
			WriteCount = PartialFile.Write(Buffer, 4, 1);
			ZETestCheckEqual(WriteCount, 0);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 48);
			isEof = PartialFile.Eof();
			ZETestCheck(isEof == true);
			WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 3);
			ZETestCheckEqual(WriteCount, 2);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 50);
			isEof = PartialFile.Eof();
			ZETestCheck(isEof == true);

			ZEQWORD WriteCount1 = PartialFile1.Write(Buffer, 4, 8);
			ZETestCheckEqual(WriteCount1, 7);
			PartialCursor1 = PartialFile1.Tell();
			ZETestCheckEqual(PartialCursor1, 28);
			bool isEof1 = PartialFile1.Eof();
			ZETestCheck(isEof1 == true);
			WriteCount1 = PartialFile1.Write(Buffer, sizeof(unsigned char), 3);
			ZETestCheckEqual(WriteCount1, 2);
			PartialCursor1 = PartialFile1.Tell();
			ZETestCheckEqual(PartialCursor1, 30);
			isEof1 = PartialFile1.Eof();
			ZETestCheck(isEof1 == true);

			PartialFile1.Close();
			PartialFile.Close();
			remove("ZEPartialFileWriteTests.txt");
		}

		ZETestCase("try to write count > partial file size")
		{
			
			File.Open("ZEPartialFileWriteTests.txt", ZE_FM_READ_WRITE, true);

			Result = PartialFile.Open(&File, 0, 100);

			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 0);
			WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 101);
			ZETestCheckEqual(WriteCount, 100);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 100);
			isEof = PartialFile.Eof();
			ZETestCheck(isEof == true);

			PartialFile.Close();
			remove("ZEPartialFileWriteTests.txt");
		}

		ZETestCase("open file in ZE_FM_READ_ONLY, and open a partial file then try to write")
		{
			File.Open("ZEPartialFileWriteTests.txt", ZE_FM_READ_WRITE, true);
			File.Close();

			bool Res = File.Open("ZEPartialFileWriteTests.txt", ZE_FM_READ_ONLY, true);
			ZETestCheck(Res == true);

			Result = PartialFile.Open(&File, 0, 100);
			ZETestCheck(Result == true);

			WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 50);
			ZETestCheckEqual(WriteCount, 0);

			PartialFile.Close();
			remove("ZEPartialFileWriteTests.txt");
		}
	}
	ZETest("ZEQWORD ZEPartialFile::GetStartPosition()")
	{
		ZEFile File;
		ZEPartialFile PartialFile;

		File.Open("ZEPartialFileStartPositionTests.txt", ZE_FM_READ_WRITE, true);

		bool Result = PartialFile.Open(&File, 10, 1000);

		ZEQWORD StartPosition = PartialFile.GetStartPosition();
		ZETestCheckEqual(StartPosition, 10);

		PartialFile.Close();
		remove("ZEPartialFileStartPositionTests.txt");
	}
	ZETest("ZEQWORD ZEPartialFile::GetEndPosition()")
	{
		ZEFile File;
		ZEPartialFile PartialFile;

		File.Open("ZEPartialFileEndPositionTests.txt", ZE_FM_READ_WRITE, true);

		bool Result = PartialFile.Open(&File, 10, 1000);

		ZEQWORD EndPosition = PartialFile.GetEndPosition();
		ZETestCheckEqual(EndPosition, 1010);

		PartialFile.Close();
		remove("ZEPartialFileEndPositionTests.txt");
	}
	ZETest("ZEQWORD ZEPartialFile::WriteFormated(const char* Format, ...)")
	{
		ZEFile File;
		ZEPartialFile PartialFile;

		File.Open("ZEPartialFileWriteFormatedTests.txt", ZE_FM_READ_WRITE, true);

		bool Result = PartialFile.Open(&File, 10, 300);

		ZEQWORD WriteSize = PartialFile.WriteFormated("This line contains Int: %d, Double: %lf, Float: %f, String: %s", 11, 11.11, 11.11f, "eleven");
		ZETestCheckEqual(WriteSize, 79);
		
		ZEQWORD PartialCursor = PartialFile.Tell();
		ZETestCheckEqual(PartialCursor, 79);

		PartialFile.Close();
		remove("ZEPartialFileWriteFormatedTests.txt");

		ZETestCase("try to write bigger size of data than partial file size")
		{
			File.Open("ZEPartialFileWriteFormatedTests.txt", ZE_FM_READ_WRITE, true);

			bool Result = PartialFile.Open(&File, 0, 50);

			ZEQWORD WriteSize = PartialFile.WriteFormated("This line contains Int: %d, Double: %lf, Float: %f, String: %s", 11, 11.11, 11.11f, "eleven");
			ZETestCheckEqual(WriteSize, 79);
			
			ZEQWORD PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 79);

			PartialFile.Close();
			remove("ZEPartialFileWriteFormatedTests.txt");
		}

		ZETestCase("try to write without openning file")
		{
			WriteSize = PartialFile.WriteFormated("This line contains Int: %d, Double: %lf, Float: %f, String: %s", 11, 11.11, 11.11f, "eleven");
			ZETestCheckEqual(WriteSize, 0);

			File.Close();
			remove("ZEPartialFileWriteFormatedTests.txt");
		}
	}
	ZETest("ZEQWORD ZEPartialFile::Tell()")
	{
		ZEFile File;
		ZEPartialFile PartialFile;

		File.Open("ZEPartialFileTellTests.txt", ZE_FM_READ_WRITE, true);

		bool Result = PartialFile.Open(&File, 0, 1024);

		unsigned char* Buffer = new unsigned char[1024];
		for (int I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEQWORD PartialCursor = PartialFile.Tell();
		ZETestCheckEqual(PartialCursor, 0);

		ZEQWORD WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 24);
		PartialCursor = PartialFile.Tell();
		ZETestCheckEqual(PartialCursor, 24);

		WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 1000);
		PartialCursor = PartialFile.Tell();
		ZETestCheckEqual(PartialCursor, 1024);

		WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 10);
		ZETestCheckEqual(WriteCount, 0);
		PartialCursor = PartialFile.Tell();
		ZETestCheckEqual(PartialCursor, 1024);

		PartialFile.Close();
		remove("ZEPartialFileTellTests.txt");
	}
	ZETest("bool ZEPartialFile::Seek(ZEINT64 Offset, ZESeekFrom Origin)")
	{
		unsigned char* Buffer = new unsigned char[1024];
		for (int I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;
		ZEPartialFile PartialFile;

		ZETestCase("for seek form ZE_SF_CURRENT")
		{
			File.Open("ZEPartialFileSeekTests.txt", ZE_FM_READ_WRITE, true);
			bool Partial = PartialFile.Open(&File, 0, 100);
			ZEQWORD PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 0);

			ZEQWORD WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 50);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 50);

			bool Result = PartialFile.Seek(-10 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZETestCheck(Result == true);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 40);

			Result = PartialFile.Seek(-40 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZETestCheck(Result == true);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 0);

			Result = PartialFile.Seek(-5 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZETestCheck(Result == false);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 0);

			Result = PartialFile.Seek(50 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZETestCheck(Result == true);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 50);

			Result = PartialFile.Seek(10 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZETestCheck(Result == true);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 60);

			Result = PartialFile.Seek(50 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZETestCheck(Result == false);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 60);

			PartialFile.Close();
			remove("ZEPartialFileSeekTests.txt");
		}

		ZETestCase("for seek form ZE_SF_BEGINING")
		{
			File.Open("ZEPartialFileSeekTests.txt", ZE_FM_READ_WRITE, true);
			bool Partial = PartialFile.Open(&File, 0, 100);
			ZEQWORD PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 0);

			ZEQWORD WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 50);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 50);

			bool Result = PartialFile.Seek(10 * (ZEINT64)sizeof(unsigned char), ZE_SF_BEGINING);
			ZETestCheck(Result == true);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 10);

			Result = PartialFile.Seek(0 * (ZEINT64)sizeof(unsigned char), ZE_SF_BEGINING);
			ZETestCheck(Result == true);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 0);
			
			Result = PartialFile.Seek(110 * (ZEINT64)sizeof(unsigned char), ZE_SF_BEGINING);
			ZETestCheck(Result == false);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 0);

			Result = PartialFile.Seek(50 * (ZEINT64)sizeof(unsigned char), ZE_SF_BEGINING);
			ZETestCheck(Result == true);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 50);

			Result = PartialFile.Seek(-10 * (ZEINT64)sizeof(unsigned char), ZE_SF_BEGINING);
			ZETestCheck(Result == false);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 50);

			PartialFile.Close();
			remove("ZEPartialFileSeekTests.txt");
		}

		ZETestCase("for seek form ZE_SF_END")
		{
			File.Open("ZEPartialFileSeekTests.txt", ZE_FM_READ_WRITE, true);
			bool Partial = PartialFile.Open(&File, 0, 100);
			ZEQWORD PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 0);

			ZEQWORD WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 50);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 50);

			bool Result = PartialFile.Seek(-10 * (ZEINT64)sizeof(unsigned char), ZE_SF_END);
			ZETestCheck(Result == true);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 90);

			Result = PartialFile.Seek(-50 * (ZEINT64)sizeof(unsigned char), ZE_SF_END);
			ZETestCheck(Result == true);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 50);

			Result = PartialFile.Seek(-100 * (ZEINT64)sizeof(unsigned char), ZE_SF_END);
			ZETestCheck(Result == true);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 0);

			Result = PartialFile.Seek(-110 * (ZEINT64)sizeof(unsigned char), ZE_SF_END);
			ZETestCheck(Result == false);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 0);

			Result = PartialFile.Seek(60 * (ZEINT64)sizeof(unsigned char), ZE_SF_END);
			ZETestCheck(Result == false);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 0);

			PartialFile.Close();
			remove("ZEPartialFileSeekTests.txt");	
		}
	}
	ZETest("void ZEPartialFile::Close()")
	{
		ZEFile File;
		ZEPartialFile PartialFile;

		File.Open("ZEPartialFileCloseTests.txt", ZE_FM_READ_WRITE, true);
		bool Result = PartialFile.Open(&File, 0, 100);
		unsigned int ReferenceCount = File.GetReferenceCount();
		ZETestCheckEqual(ReferenceCount, 1);

		PartialFile.Close();
		ReferenceCount = File.GetReferenceCount();
		ZETestCheckEqual(ReferenceCount, 0);
		ZEQWORD StartPosition = PartialFile.GetStartPosition();
		ZETestCheckEqual(StartPosition, 0);
		ZEQWORD EndPosition = PartialFile.GetEndPosition();
		ZETestCheckEqual(EndPosition, 0);
		bool isEof = PartialFile.Eof();
		ZETestCheck(isEof == false);
		
		remove("ZEPartialFileCloseTests.txt");

		ZETestCase("close parent file first")
		{
			File.Open("ZEPartialFileCloseTests.txt", ZE_FM_READ_WRITE, true);
			Result = PartialFile.Open(&File, 0, 100);
			EndPosition = PartialFile.GetEndPosition();
			ZETestCheckEqual(EndPosition, 100);

			ReferenceCount = File.GetReferenceCount();
			ZETestCheckEqual(ReferenceCount, 1);

			File.Close();
			EndPosition = PartialFile.GetEndPosition();
			ZETestCheckEqual(EndPosition, 100);

			remove("ZEPartialFileCloseTests.txt");
		}

		ZETestCase("close partial files first")
		{
			ZEPartialFile PartialFile1;
			File.Open("ZEPartialFileCloseTests.txt", ZE_FM_READ_WRITE, true);
			Result = PartialFile.Open(&File, 0, 50);
			Result = PartialFile1.Open(&File, 50, 100);

			ReferenceCount = File.GetReferenceCount();
			ZETestCheckEqual(ReferenceCount, 2);

			PartialFile.Close();
			PartialFile1.Close();

			bool isOpen = File.IsOpen();
			ZETestCheck(isOpen == false);

			remove("ZEPartialFileCloseTests.txt");
		}

		ZETestCase("close partial file on a partial file first")
		{
			ZEPartialFile PartialFile1;
			File.Open("ZEPartialFileCloseTests.txt", ZE_FM_READ_WRITE, true);
			Result = PartialFile.Open(&File, 0, 100);
			Result = PartialFile1.Open((ZEFile*)&PartialFile, 30, 50);

			ReferenceCount = PartialFile.GetReferenceCount();
			ZETestCheckEqual(ReferenceCount, 1);
			ReferenceCount = File.GetReferenceCount();
			ZETestCheckEqual(ReferenceCount, 2);

			PartialFile1.Close();
			
			EndPosition = PartialFile.GetEndPosition();
			ZETestCheckEqual(EndPosition, 0);

			remove("ZEPartialFileCloseTests.txt");
		}
	}
	ZETest("ZEQWORD ZEPartialFile::Read(void* Buffer, ZEQWORD Size, ZEQWORD Count)")
	{
		unsigned char* Buffer = new unsigned char[1024];
		unsigned char* BufferRead = new unsigned char[1024];
		for ( int I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;
		ZEPartialFile PartialFile;

		File.Open("ZEPartialFileReadTests.txt", ZE_FM_READ_WRITE, true);
		PartialFile.Open(&File, 0, 100);
		ZEQWORD WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 10);
		
		bool Result = PartialFile.Seek(-10 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);

		ZEQWORD ReadCount = PartialFile.Read(BufferRead, sizeof(unsigned char), 10);
		ZETestCheckEqual(ReadCount, 10);

		int Res = memcmp(Buffer, BufferRead, sizeof(unsigned char));
		ZETestCheckEqual(Res, 0);

		PartialFile.Close();
		remove("ZEPartialFileReadTests.txt");

		ZETestCase("try to read whole partial file")
		{
			File.Open("ZEPartialFileReadTests.txt", ZE_FM_READ_WRITE, true);
			PartialFile.Open(&File, 0, 1024);
			WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 1024);

			Result = PartialFile.Seek(-1024 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);

			ReadCount = PartialFile.Read(BufferRead, sizeof(unsigned char), 1024);
			ZETestCheckEqual(ReadCount, 1024);

			Res = memcmp(Buffer, BufferRead, sizeof(unsigned char));
			ZETestCheckEqual(Res, 0);

			bool isEof = PartialFile.Eof();
			ZETestCheck(isEof == false);

			PartialFile.Close();
			remove("ZEPartialFileReadTests.txt");
		}

		ZETestCase("try to read bigger than partial file size")
		{
			File.Open("ZEPartialFileReadTests.txt", ZE_FM_READ_WRITE, true);
			PartialFile.Open(&File, 0, 1024);
			WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 1024);

			Result = PartialFile.Seek(-1024 * (ZEINT64)sizeof(unsigned char), ZE_SF_CURRENT);

			ReadCount = PartialFile.Read(BufferRead, sizeof(unsigned char), 1100);
			ZETestCheckEqual(ReadCount, 1024);

			Res = memcmp(Buffer, BufferRead, sizeof(unsigned char));
			ZETestCheckEqual(Res, 0);

			bool isEof = PartialFile.Eof();
			ZETestCheck(isEof == true);

			PartialFile.Close();
			remove("ZEPartialFileReadTests.txt");
		}

		ZETestCase("try to write and read bigger than partial file size")
		{
			File.Open("ZEPartialFileReadTests.txt", ZE_FM_READ_WRITE, true);
			PartialFile.Open(&File, 0, 100);
			WriteCount = PartialFile.Write(Buffer, sizeof(unsigned int), 26);
			ZETestCheckEqual(WriteCount, 25);

			Result = PartialFile.Seek(-30 * (ZEINT64)sizeof(unsigned int), ZE_SF_CURRENT);
			ZETestCheck(Result == false);
			ZEQWORD PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 100);
			Result = PartialFile.Seek(-25 * (ZEINT64)sizeof(unsigned int), ZE_SF_CURRENT);
			ZETestCheck(Result == true);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 0);

			ReadCount = PartialFile.Read(BufferRead, sizeof(unsigned int), 30);
			ZETestCheckEqual(ReadCount, 25);

			Res = memcmp(Buffer, BufferRead, sizeof(unsigned int));
			ZETestCheckEqual(Res, 0);

			PartialFile.Close();
			remove("ZEPartialFileReadTests.txt");
		}
	}
	ZETest("bool ZEPartialFile::Eof()")
	{
		unsigned char* Buffer = new unsigned char[1024];
		for (int I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;
		ZEPartialFile PartialFile;

		File.Open("ZEPartialFileEofTests.txt", ZE_FM_READ_WRITE, true);
		PartialFile.Open(&File, 0, 100);

		bool isEof = PartialFile.Eof();
		ZETestCheck(isEof == false);

		ZEQWORD WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 150);
		ZETestCheckEqual(WriteCount, 100);

		isEof = PartialFile.Eof();
		ZETestCheck(isEof == true);

		PartialFile.Close();
		remove("ZEPartialFileEofTests.txt");
	}
	ZETest("ZEQWORD ZEPartialFile::GetFileSize() const")
	{
		unsigned char* Buffer = new unsigned char[1024];
		for (int I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;
		ZEPartialFile PartialFile;

		File.Open("ZEPartialFileSizeTests.txt", ZE_FM_READ_WRITE, true);
		PartialFile.Open(&File, 0, 100);

		ZEQWORD Size = PartialFile.GetFileSize();
		ZETestCheckEqual(Size, 100);

		ZEQWORD WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 150);

		Size = PartialFile.GetFileSize();
		ZETestCheckEqual(Size, 100);

		PartialFile.Close();
		remove("ZEPartialFileSizeTests.txt");
	}
}
