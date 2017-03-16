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


#include "ZETest/ZETest.h"
#include "ZEFile/ZEPartialFile.h"
#include "ZEFile/ZEFile.h"
#include "ZEDS/ZEString.h"
#include <stdio.h>
#include "ZEError.h"

ZE_TEST(ZEPartialFile)
{
	ZE_TEST_ITEM("ZEPartialFile::~ZEPartialFile()")
	{
		ZEPartialFile PartialFile;
		ZEFile File;

		File.Open("Destructor.txt", ZE_FOM_READ, ZE_FCM_CREATE);
		PartialFile.Open(&File, 20, 50);
		ZE_TEST_CHECK_EQUAL(PartialFile.GetStartPosition(), 20);
		ZE_TEST_CHECK_EQUAL(PartialFile.GetEndPosition(), 70);

		PartialFile.~ZEPartialFile();

		ZE_TEST_CHECK_EQUAL(PartialFile.GetStartPosition(), 0);
		ZE_TEST_CHECK_EQUAL(PartialFile.GetEndPosition(), 0);

		File.Close();
		remove("Destructor.txt");
	}

	ZE_TEST_ITEM("ZEPartialFile::ZEPartialFile()")
	{
		ZEPartialFile PartialFile;

		ZE_TEST_CHECK_EQUAL(PartialFile.GetStartPosition(), 0);
		ZE_TEST_CHECK_EQUAL(PartialFile.GetEndPosition(), 0);
		//EoF: false
	}

	ZE_TEST_ITEM("ZEInt64 ZEPartialFile::GetStartPosition() const")
	{
		ZEPartialFile PartialFile;

		ZEInt64 StartPosition = PartialFile.GetStartPosition();
		ZE_TEST_CHECK_EQUAL(StartPosition, 0);

		ZEFile File;
		File.Open("StartPosition.txt", ZE_FOM_READ, ZE_FCM_CREATE);
		PartialFile.Open(&File, 20, 50);

		StartPosition = PartialFile.GetStartPosition();
		ZE_TEST_CHECK_EQUAL(StartPosition, 20);

		PartialFile.Close();
		File.Close();
		remove("StartPosition.txt");
	}

	ZE_TEST_ITEM("void ZEPartialFile::SetStartPosition(const ZEInt64 Position)")
	{
		ZEPartialFile PartialFile;
		ZE_TEST_CHECK_EQUAL(PartialFile.GetStartPosition(), 0);

		ZEFile File;
		File.Open("StartPosition.txt", ZE_FOM_READ, ZE_FCM_CREATE);
		PartialFile.Open(&File, 20, 50);
		ZE_TEST_CHECK_EQUAL(PartialFile.GetStartPosition(), 20);

		PartialFile.SetStartPosition(25);
		ZE_TEST_CHECK_EQUAL(PartialFile.GetStartPosition(), 25);

		PartialFile.Close();
		File.Close();
		remove("StartPosition.txt");
	}

	ZE_TEST_ITEM("ZEInt64 ZEPartialFile::GetEndPosition() const")
	{
		ZEPartialFile PartialFile;

		ZEInt64 EndPosition = PartialFile.GetEndPosition();
		ZE_TEST_CHECK_EQUAL(EndPosition, 0);

		ZEFile File;
		File.Open("EndPosition.txt", ZE_FOM_READ, ZE_FCM_CREATE);
		PartialFile.Open(&File, 20, 50);

		EndPosition = PartialFile.GetEndPosition();
		ZE_TEST_CHECK_EQUAL(EndPosition, 70);

		PartialFile.Close();
		File.Close();
		remove("EndPosition.txt");
	}

	ZE_TEST_ITEM("void ZEPartialFile::SetEndPosition(const ZEInt64 Position)")
	{
		ZEPartialFile PartialFile;
		ZE_TEST_CHECK_EQUAL(PartialFile.GetEndPosition(), 0);

		ZEFile File;
		File.Open("EndPosition.txt", ZE_FOM_READ, ZE_FCM_CREATE);
		PartialFile.Open(&File, 20, 50);
		ZE_TEST_CHECK_EQUAL(PartialFile.GetEndPosition(), 70);

		PartialFile.SetEndPosition(30);
		ZE_TEST_CHECK_EQUAL(PartialFile.GetEndPosition(), 30);

		PartialFile.Close();
		File.Close();
		remove("EndPosition.txt");
	}

	ZE_TEST_ITEM("bool ZEPartialFile::Open(const ZEString& FilePath, const ZEFileOpenMode OpenType, const ZEFileCreationMode CreationType)")
	{
		ZEFile File;
		ZEPartialFile PartialFile;

		bool Open = File.Open("OpenTest.txt", ZE_FOM_READ, ZE_FCM_CREATE);

		bool Opened = PartialFile.Open("OpenTest.txt", ZE_FOM_READ, ZE_FCM_CREATE);
		ZE_TEST_CHECK_ERROR(Opened);
		ZE_TEST_CHECK_EQUAL(PartialFile.GetStartPosition(), 0);
		ZE_TEST_CHECK_EQUAL(PartialFile.GetEndPosition(), 0);

		ZEInt Closed = PartialFile.Close();
		ZE_TEST_CHECK_EQUAL(Closed, 0);
		Closed = File.Close();
		ZE_TEST_CHECK_EQUAL(Closed, 0);
		remove("OpenTest.txt");

		ZE_TEST_CASE("open a partial file without a parent file")
		{
			Opened = PartialFile.Open("OpenTest.txt", ZE_FOM_READ, ZE_FCM_CREATE);
			ZE_TEST_CHECK_ERROR(Opened);

			PartialFile.Close();
			remove("OpenTest.txt");
		}
	}

	ZE_TEST_ITEM("bool ZEPartialFile::Open(const ZEFile* ParentFile, const ZEInt64 Offset, const ZEInt64 Size)")
	{
		ZEFile File;
		ZEPartialFile PartialFile;

		bool Open = File.Open("OpenTest.txt", ZE_FOM_READ, ZE_FCM_CREATE);

		bool Opened = PartialFile.Open(&File, 100, 50);
		ZE_TEST_CHECK_ERROR(Opened);
		ZE_TEST_CHECK_EQUAL(PartialFile.GetStartPosition(), 100);
		ZE_TEST_CHECK_EQUAL(PartialFile.GetEndPosition(), 150);

		PartialFile.Close();
		File.Close();
		remove("OpenTest.txt");

		ZE_TEST_CASE("try to open a partial file without a parent file")
		{
// 			Opened = PartialFile.Open(&File, 100, 50);
// 			ZE_TEST_CHECK_ERROR(!Opened);
		}

		ZE_TEST_CASE("open more than 1 partial file")
		{
			ZEPartialFile PartialFile1;
			ZEPartialFile PartialFile2;
			ZEPartialFile PartialFile3;

			Open = File.Open("OpenTest.txt", ZE_FOM_READ, ZE_FCM_CREATE);

			Opened = PartialFile1.Open(&File, 10, 50);
			ZE_TEST_CHECK_ERROR(Opened);
			ZE_TEST_CHECK_EQUAL(PartialFile1.GetStartPosition(), 10);
			ZE_TEST_CHECK_EQUAL(PartialFile1.GetEndPosition(), 60);

			Opened = PartialFile2.Open(&File, 60, 30);
			ZE_TEST_CHECK_ERROR(Opened);
			ZE_TEST_CHECK_EQUAL(PartialFile2.GetStartPosition(), 60);
			ZE_TEST_CHECK_EQUAL(PartialFile2.GetEndPosition(), 90);

			Opened = PartialFile3.Open(&File, 90, 10);
			ZE_TEST_CHECK_ERROR(Opened);
			ZE_TEST_CHECK_EQUAL(PartialFile3.GetStartPosition(), 90);
			ZE_TEST_CHECK_EQUAL(PartialFile3.GetEndPosition(), 100);

			PartialFile3.Close();
			PartialFile2.Close();
			PartialFile1.Close();
			File.Close();
			remove("OpenTest.txt");
		}

		ZE_TEST_CASE("try to intersect the partial files")
		{
			ZEPartialFile PartialFile1;
			ZEPartialFile PartialFile2;

			Open = File.Open("OpenTest.txt", ZE_FOM_READ, ZE_FCM_CREATE);

			Opened = PartialFile1.Open(&File, 100, 100);
			ZE_TEST_CHECK_ERROR(Opened);
			ZE_TEST_CHECK_EQUAL(PartialFile1.GetStartPosition(), 100);
			ZE_TEST_CHECK_EQUAL(PartialFile1.GetEndPosition(), 200);

			Opened = PartialFile2.Open(&File, 150, 100);
			ZE_TEST_CHECK_ERROR(Opened);
			ZE_TEST_CHECK_EQUAL(PartialFile2.GetStartPosition(), 150);
			ZE_TEST_CHECK_EQUAL(PartialFile2.GetEndPosition(), 250);

			PartialFile2.Close();
			PartialFile1.Close();
			File.Close();
			remove("OpenTest.txt");
		}

		ZE_TEST_CASE("open partial file on a partial file")
		{
			ZEPartialFile PartialFile1;
			ZEPartialFile PartialFile2;
			ZEPartialFile PartialFile3;

			Open = File.Open("OpenTest.txt", ZE_FOM_READ, ZE_FCM_CREATE);

			Opened = PartialFile1.Open(&File, 0, 100);
			ZE_TEST_CHECK_ERROR(Opened);
			ZE_TEST_CHECK_EQUAL(PartialFile1.GetStartPosition(), 0);
			ZE_TEST_CHECK_EQUAL(PartialFile1.GetEndPosition(), 100);

			Opened = PartialFile2.Open((ZEFile*)&PartialFile1, 0, 50);
			ZE_TEST_CHECK_ERROR(Opened);
			ZE_TEST_CHECK_EQUAL(PartialFile2.GetStartPosition(), 0);
			ZE_TEST_CHECK_EQUAL(PartialFile2.GetEndPosition(), 50);

			Opened = PartialFile3.Open((ZEFile*)&PartialFile2, 10, 30);
			ZE_TEST_CHECK_ERROR(Opened);
			ZE_TEST_CHECK_EQUAL(PartialFile3.GetStartPosition(), 10);
			ZE_TEST_CHECK_EQUAL(PartialFile3.GetEndPosition(), 40);

			PartialFile3.Close();
			PartialFile2.Close();
			PartialFile1.Close();
			File.Close();
			remove("OpenTest.txt");
		}
	}

	ZE_TEST_ITEM("ZEInt ZEPartialFile::Close()")
	{
		ZEFile File;
		ZEPartialFile PartialFile;

		File.Open("CloseTest.txt", ZE_FOM_READ, ZE_FCM_CREATE);
		PartialFile.Open(&File, 10, 30);
		ZE_TEST_CHECK_EQUAL(PartialFile.GetStartPosition(), 10);
		ZE_TEST_CHECK_EQUAL(PartialFile.GetEndPosition(), 40);

		ZEInt Closed = PartialFile.Close();
		ZE_TEST_CHECK_EQUAL(Closed, 0);
		ZE_TEST_CHECK_EQUAL(PartialFile.GetStartPosition(), 0);
		ZE_TEST_CHECK_EQUAL(PartialFile.GetEndPosition(), 0);

		File.Close();
		remove("CloseTest.txt");

		ZE_TEST_CASE("close parent file first")
		{
			File.Open("CloseTest.txt", ZE_FOM_READ, ZE_FCM_CREATE);
			PartialFile.Open(&File, 10, 30);
			ZE_TEST_CHECK_EQUAL(PartialFile.GetStartPosition(), 10);
			ZE_TEST_CHECK_EQUAL(PartialFile.GetEndPosition(), 40);

			Closed = File.Close();
			ZE_TEST_CHECK_EQUAL(Closed, 0);

			Closed = PartialFile.Close();
			ZE_TEST_CHECK_EQUAL(Closed, 0);
			ZE_TEST_CHECK_EQUAL(PartialFile.GetStartPosition(), 0);
			ZE_TEST_CHECK_EQUAL(PartialFile.GetEndPosition(), 0);

			remove("CloseTest.txt");
		}

		ZE_TEST_CASE("close partial file on a partial file first")
		{
			ZEPartialFile PartialFile1;
			File.Open("CloseTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			PartialFile.Open(&File, 0, 100);
			PartialFile1.Open((ZEFile*)&PartialFile, 30, 50);

			Closed = PartialFile1.Close();
			ZE_TEST_CHECK_EQUAL(Closed, 0);
			ZE_TEST_CHECK_EQUAL(PartialFile1.GetStartPosition(), 0);
			ZE_TEST_CHECK_EQUAL(PartialFile1.GetEndPosition(), 0);
			ZE_TEST_CHECK_EQUAL(PartialFile.GetStartPosition(), 0);
			ZE_TEST_CHECK_EQUAL(PartialFile.GetEndPosition(), 100);

			Closed = PartialFile.Close();
			ZE_TEST_CHECK_EQUAL(Closed, 0);
			ZE_TEST_CHECK_EQUAL(PartialFile.GetStartPosition(), 0);
			ZE_TEST_CHECK_EQUAL(PartialFile.GetEndPosition(), 0);

			File.Close();
			remove("CloseTest.txt");
		}

		ZE_TEST_CASE("close parent partial file first")
		{
			ZEPartialFile PartialFile1;
			File.Open("CloseTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			PartialFile.Open(&File, 0, 100);
			PartialFile1.Open((ZEFile*)&PartialFile, 30, 50);

			Closed = PartialFile.Close();
			ZE_TEST_CHECK_EQUAL(Closed, 0);
			ZE_TEST_CHECK_EQUAL(PartialFile.GetStartPosition(), 0);
			ZE_TEST_CHECK_EQUAL(PartialFile.GetEndPosition(), 0);
			ZE_TEST_CHECK_EQUAL(PartialFile1.GetStartPosition(), 30);
			ZE_TEST_CHECK_EQUAL(PartialFile1.GetEndPosition(), 80);

			Closed = PartialFile1.Close();
			ZE_TEST_CHECK_EQUAL(Closed, 0);
			ZE_TEST_CHECK_EQUAL(PartialFile1.GetStartPosition(), 0);
			ZE_TEST_CHECK_EQUAL(PartialFile1.GetEndPosition(), 0);

			File.Close();
			remove("CloseTest.txt");
		}
	}

	ZE_TEST_ITEM("ZESize ZEPartialFile::Write(const void* Buffer, const ZESize Size, const ZESize Count)")
	{
		unsigned char* Buffer = new unsigned char[1024];
		for ( ZEInt I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;
		ZEPartialFile PartialFile;

		File.Open("WriteTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
		PartialFile.Open(&File, 100, 100);
		ZE_TEST_CHECK_EQUAL(PartialFile.GetStartPosition(), 100);
		ZE_TEST_CHECK_EQUAL(PartialFile.GetEndPosition(), 200);
		ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 0);

		ZESize WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 10);
		ZE_TEST_CHECK_EQUAL(WriteCount, 10);
		ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 10);
		ZE_TEST_CHECK_EQUAL(File.Tell(), 0);

		WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 90);
		ZE_TEST_CHECK_EQUAL(WriteCount, 90);
		ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 100);
		//EoF false

		WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 1);
		ZE_TEST_CHECK_EQUAL(WriteCount, 0);
		ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 100);
		//EoF true

		PartialFile.Close();
		File.Close();
		remove("WriteTest.txt");

		ZE_TEST_CASE("try to write count > partial file size")
		{
			File.Open("WriteTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			bool a = PartialFile.Open(&File, 0, 100);

			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 0);

			WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 110);
			ZE_TEST_CHECK_EQUAL(WriteCount, 100);
			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 100);
			//EoF true

			PartialFile.Close();
			File.Close();
			remove("WriteTest.txt");
		}
	}

	ZE_TEST_ITEM("ZEInt ZEPartialFile::Seek(const ZEInt64 Offset, const ZESeekFrom Origin)")
	{
		unsigned char* Buffer = new unsigned char[1024];
 		for (ZEInt I = 0; I < 1024; I++)
 		{
			Buffer[I] = I % 256;
		}

 		ZEFile File;
 		ZEPartialFile PartialFile;
 
 		ZE_TEST_CASE("for seek form ZE_SF_CURRENT")
 		{
 			File.Open("SeekTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			PartialFile.Open(&File, 0, 100);
 			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 0);
 
 			ZESize WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 50);
 			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 50);
 
			ZEInt Seek = PartialFile.Seek(-10 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
 			ZE_TEST_CHECK_EQUAL(Seek, 0);
			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 40);
 
 			Seek = PartialFile.Seek(-40 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
 			ZE_TEST_CHECK_EQUAL(Seek, 0);
			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 0);
 
			Seek = PartialFile.Seek(-5 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
 			ZE_TEST_CHECK_ERROR(Seek != 0);
 			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 0);

 			Seek = PartialFile.Seek(50 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
 			ZE_TEST_CHECK_EQUAL(Seek, 0);
 			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 50);
 
 			Seek = PartialFile.Seek(10 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
 			ZE_TEST_CHECK_EQUAL(Seek, 0);
 			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 60);

 			Seek = PartialFile.Seek(50 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
 			ZE_TEST_CHECK_ERROR(Seek != 0);
 			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 60);
			
			Seek = PartialFile.Seek(40 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZE_TEST_CHECK_EQUAL(Seek, 0);
			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 100);

			Seek = PartialFile.Seek(1 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZE_TEST_CHECK_ERROR(Seek != 0);
			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 100);
 
 			PartialFile.Close();
 			File.Close();
 			remove("SeekTest.txt");
 		}
 
 		ZE_TEST_CASE("for seek form ZE_SF_BEGINING")
 		{
 			File.Open("SeekTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			PartialFile.Open(&File, 0, 100);
 			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 0);
 
 			ZESize WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 50);
 			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 50);
 
 			ZEInt Seek = PartialFile.Seek(10 * (ZEInt64)sizeof(unsigned char), ZE_SF_BEGINING);
 			ZE_TEST_CHECK_EQUAL(Seek, 0);
 			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 10);
 
 			Seek = PartialFile.Seek(0 * (ZEInt64)sizeof(unsigned char), ZE_SF_BEGINING);
 			ZE_TEST_CHECK_EQUAL(Seek, 0);
 			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 0);
 			
 			Seek = PartialFile.Seek(110 * (ZEInt64)sizeof(unsigned char), ZE_SF_BEGINING);
 			ZE_TEST_CHECK_ERROR(Seek != 0);
 			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 0);
 
 			Seek = PartialFile.Seek(50 * (ZEInt64)sizeof(unsigned char), ZE_SF_BEGINING);
 			ZE_TEST_CHECK_EQUAL(Seek, 0);
 			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 50);
 
 			Seek = PartialFile.Seek(-10 * (ZEInt64)sizeof(unsigned char), ZE_SF_BEGINING);
 			ZE_TEST_CHECK_ERROR(Seek != 0);
 			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 50);

			Seek = PartialFile.Seek(100 * (ZEInt64)sizeof(unsigned char), ZE_SF_BEGINING);
			ZE_TEST_CHECK_EQUAL(Seek, 0);
			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 100);
 
 			PartialFile.Close();
 			File.Close();
 			remove("SeekTest.txt");
 		}
 
 		ZE_TEST_CASE("for seek form ZE_SF_END")
		{
 			File.Open("SeekTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			PartialFile.Open(&File, 0, 100);
 			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 0);
 
 			ZESize WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 50);
 			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 50);
 
 			ZEInt Seek = PartialFile.Seek(-10 * (ZEInt64)sizeof(unsigned char), ZE_SF_END);
 			ZE_TEST_CHECK_EQUAL(Seek, 0);
			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 90);
 
 			Seek = PartialFile.Seek(-50 * (ZEInt64)sizeof(unsigned char), ZE_SF_END);
 			ZE_TEST_CHECK_EQUAL(Seek, 0);
			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 50);
 
 			Seek = PartialFile.Seek(-100 * (ZEInt64)sizeof(unsigned char), ZE_SF_END);
 			ZE_TEST_CHECK_EQUAL(Seek, 0);
 			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 0);
 
 			Seek = PartialFile.Seek(-110 * (ZEInt64)sizeof(unsigned char), ZE_SF_END);
 			ZE_TEST_CHECK_EQUAL(Seek, -1);
 			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 0);
 
 			Seek = PartialFile.Seek(60 * (ZEInt64)sizeof(unsigned char), ZE_SF_END);
 			ZE_TEST_CHECK_EQUAL(Seek, -1);
			ZE_TEST_CHECK_EQUAL(PartialFile.Tell(), 0);
 
 			PartialFile.Close();
 			File.Close();
 			remove("SeekTest.txt");	
 		}
	}

	ZE_TEST_ITEM("ZEInt64 ZEPartialFile::Tell() const")
	{
		ZEFile File;
		ZEPartialFile PartialFile;

		File.Open("TellTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
		PartialFile.Open(&File, 0, 1024);

		unsigned char* Buffer = new unsigned char[1024];
		for (ZEInt I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEInt64 PartialCursor = PartialFile.Tell();
		ZE_TEST_CHECK_EQUAL(PartialCursor, 0);

		ZESize WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 24);
		PartialCursor = PartialFile.Tell();
		ZE_TEST_CHECK_EQUAL(PartialCursor, 24);

		WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 1000);
		PartialCursor = PartialFile.Tell();
		ZE_TEST_CHECK_EQUAL(PartialCursor, 1024);

		WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 10);
		ZE_TEST_CHECK_EQUAL(WriteCount, 0);
		PartialCursor = PartialFile.Tell();
		ZE_TEST_CHECK_EQUAL(PartialCursor, 1024);

		PartialFile.Close();
		File.Close();
		remove("TellTest.txt");
	}

	ZE_TEST_ITEM("ZESize ZEPartialFile::Read(void* Buffer, const ZESize Size, const ZESize Count)")
	{
		unsigned char* Buffer = new unsigned char[1024];
		unsigned char* BufferRead = new unsigned char[1024];
		for ( ZEInt I = 0; I < 1024; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFile File;
		ZEPartialFile PartialFile;

		File.Open("ReadTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
		PartialFile.Open(&File, 0, 100);
		ZESize WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 10);

		ZEInt Result = PartialFile.Seek(-10 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

		ZESize ReadCount = PartialFile.Read(BufferRead, sizeof(unsigned char), 10);
		ZE_TEST_CHECK_EQUAL(ReadCount, 10);

		ZEInt Res = memcmp(Buffer, BufferRead, sizeof(unsigned char));
		ZE_TEST_CHECK_EQUAL(Res, 0);

		PartialFile.Close();
		File.Close();
		remove("ReadTest.txt");

		ZE_TEST_CASE("try to read whole partial file")
		{
			File.Open("ReadTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			PartialFile.Open(&File, 0, 1024);
			WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 1024);

			Result = PartialFile.Seek(-1024 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ReadCount = PartialFile.Read(BufferRead, sizeof(unsigned char), 1024);
			ZE_TEST_CHECK_EQUAL(ReadCount, 1024);

			Res = memcmp(Buffer, BufferRead, sizeof(unsigned char));
			ZE_TEST_CHECK_EQUAL(Res, 0);

			//EoF false

			PartialFile.Close();
			File.Close();
			remove("ReadTest.txt");
		}

		ZE_TEST_CASE("try to read bigger than partial file size")
		{
			File.Open("ReadTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			PartialFile.Open(&File, 0, 1024);
			WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 1024);

			Result = PartialFile.Seek(-1024 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ReadCount = PartialFile.Read(BufferRead, sizeof(unsigned char), 1100);
			ZE_TEST_CHECK_EQUAL(ReadCount, 1024);
			//EoF true

			Res = memcmp(Buffer, BufferRead, sizeof(unsigned char));
			ZE_TEST_CHECK_EQUAL(Res, 0);

			PartialFile.Close();
			File.Close();
			remove("ReadTest.txt");
		}

		ZE_TEST_CASE("try to write and read bigger than partial file size")
		{
			File.Open("ReadTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			PartialFile.Open(&File, 0, 100);
			WriteCount = PartialFile.Write(Buffer, sizeof(ZEUInt), 26);
			ZE_TEST_CHECK_EQUAL(WriteCount, 25);
			//EoF true

			Result = PartialFile.Seek(-30 * (ZEInt64)sizeof(ZEUInt), ZE_SF_CURRENT);
			ZE_TEST_CHECK_EQUAL(Result, -1);
			ZEInt64 PartialCursor = PartialFile.Tell();
			ZE_TEST_CHECK_EQUAL(PartialCursor, 100);
			Result = PartialFile.Seek(-25 * (ZEInt64)sizeof(ZEUInt), ZE_SF_CURRENT);
			ZE_TEST_CHECK_EQUAL(Result, 0);
			PartialCursor = PartialFile.Tell();
			ZE_TEST_CHECK_EQUAL(PartialCursor, 0);

			ReadCount = PartialFile.Read(BufferRead, sizeof(ZEUInt), 30);
			ZE_TEST_CHECK_EQUAL(ReadCount, 25);

			Res = memcmp(Buffer, BufferRead, sizeof(ZEUInt));
			ZE_TEST_CHECK_EQUAL(Res, 0);

			PartialFile.Close();
			File.Close();
			remove("ReadTest.txt");
		}
	}
}
