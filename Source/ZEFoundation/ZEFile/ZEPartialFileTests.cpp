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

ZETestSuite(ZEPartialFile)
{
	ZETest("ZEPartialFile::~ZEPartialFile()")
	{
		ZEPartialFile PartialFile;
		ZEFile File;

		File.Open("Destructor.txt", ZE_FOM_READ, ZE_FCM_CREATE);
		PartialFile.Open(&File, 20, 50);
		ZETestCheckEqual(PartialFile.GetStartPosition(), 20);
		ZETestCheckEqual(PartialFile.GetEndPosition(), 70);

		PartialFile.~ZEPartialFile();

		ZETestCheckEqual(PartialFile.GetStartPosition(), 0);
		ZETestCheckEqual(PartialFile.GetEndPosition(), 0);

		File.Close();
		remove("Destructor.txt");
	}

	ZETest("ZEPartialFile::ZEPartialFile()")
	{
		ZEPartialFile PartialFile;

		ZETestCheckEqual(PartialFile.GetStartPosition(), 0);
		ZETestCheckEqual(PartialFile.GetEndPosition(), 0);
		//EoF: false
	}

	ZETest("ZEInt64 ZEPartialFile::GetStartPosition() const")
	{
		ZEPartialFile PartialFile;

		ZEInt64 StartPosition = PartialFile.GetStartPosition();
		ZETestCheckEqual(StartPosition, 0);

		ZEFile File;
		File.Open("StartPosition.txt", ZE_FOM_READ, ZE_FCM_CREATE);
		PartialFile.Open(&File, 20, 50);

		StartPosition = PartialFile.GetStartPosition();
		ZETestCheckEqual(StartPosition, 20);

		PartialFile.Close();
		File.Close();
		remove("StartPosition.txt");
	}

	ZETest("void ZEPartialFile::SetStartPosition(const ZEInt64 Position)")
	{
		ZEPartialFile PartialFile;
		ZETestCheckEqual(PartialFile.GetStartPosition(), 0);

		ZEFile File;
		File.Open("StartPosition.txt", ZE_FOM_READ, ZE_FCM_CREATE);
		PartialFile.Open(&File, 20, 50);
		ZETestCheckEqual(PartialFile.GetStartPosition(), 20);

		PartialFile.SetStartPosition(25);
		ZETestCheckEqual(PartialFile.GetStartPosition(), 25);

		PartialFile.Close();
		File.Close();
		remove("StartPosition.txt");
	}

	ZETest("ZEInt64 ZEPartialFile::GetEndPosition() const")
	{
		ZEPartialFile PartialFile;

		ZEInt64 EndPosition = PartialFile.GetEndPosition();
		ZETestCheckEqual(EndPosition, 0);

		ZEFile File;
		File.Open("EndPosition.txt", ZE_FOM_READ, ZE_FCM_CREATE);
		PartialFile.Open(&File, 20, 50);

		EndPosition = PartialFile.GetEndPosition();
		ZETestCheckEqual(EndPosition, 70);

		PartialFile.Close();
		File.Close();
		remove("EndPosition.txt");
	}

	ZETest("void ZEPartialFile::SetEndPosition(const ZEInt64 Position)")
	{
		ZEPartialFile PartialFile;
		ZETestCheckEqual(PartialFile.GetEndPosition(), 0);

		ZEFile File;
		File.Open("EndPosition.txt", ZE_FOM_READ, ZE_FCM_CREATE);
		PartialFile.Open(&File, 20, 50);
		ZETestCheckEqual(PartialFile.GetEndPosition(), 70);

		PartialFile.SetEndPosition(30);
		ZETestCheckEqual(PartialFile.GetEndPosition(), 30);

		PartialFile.Close();
		File.Close();
		remove("EndPosition.txt");
	}

	ZETest("bool ZEPartialFile::Open(const ZEString& FilePath, const ZEFileOpenMode OpenType, const ZEFileCreationMode CreationType)")
	{
		ZEFile File;
		ZEPartialFile PartialFile;

		bool Open = File.Open("OpenTest.txt", ZE_FOM_READ, ZE_FCM_CREATE);

		bool Opened = PartialFile.Open("OpenTest.txt", ZE_FOM_READ, ZE_FCM_CREATE);
		ZETestCheck(Opened);
		ZETestCheckEqual(PartialFile.GetStartPosition(), 0);
		ZETestCheckEqual(PartialFile.GetEndPosition(), 0);

		ZEInt Closed = PartialFile.Close();
		ZETestCheckEqual(Closed, 0);
		Closed = File.Close();
		ZETestCheckEqual(Closed, 0);
		remove("OpenTest.txt");

		ZETestCase("open a partial file without a parent file")
		{
			Opened = PartialFile.Open("OpenTest.txt", ZE_FOM_READ, ZE_FCM_CREATE);
			ZETestCheck(Opened);

			PartialFile.Close();
			remove("OpenTest.txt");
		}
	}

	ZETest("bool ZEPartialFile::Open(const ZEFile* ParentFile, const ZEInt64 Offset, const ZEInt64 Size)")
	{
		ZEFile File;
		ZEPartialFile PartialFile;

		bool Open = File.Open("OpenTest.txt", ZE_FOM_READ, ZE_FCM_CREATE);

		bool Opened = PartialFile.Open(&File, 100, 50);
		ZETestCheck(Opened);
		ZETestCheckEqual(PartialFile.GetStartPosition(), 100);
		ZETestCheckEqual(PartialFile.GetEndPosition(), 150);

		PartialFile.Close();
		File.Close();
		remove("OpenTest.txt");

		ZETestCase("try to open a partial file without a parent file")
		{
// 			Opened = PartialFile.Open(&File, 100, 50);
// 			ZETestCheck(!Opened);
		}

		ZETestCase("open more than 1 partial file")
		{
			ZEPartialFile PartialFile1;
			ZEPartialFile PartialFile2;
			ZEPartialFile PartialFile3;

			Open = File.Open("OpenTest.txt", ZE_FOM_READ, ZE_FCM_CREATE);

			Opened = PartialFile1.Open(&File, 10, 50);
			ZETestCheck(Opened);
			ZETestCheckEqual(PartialFile1.GetStartPosition(), 10);
			ZETestCheckEqual(PartialFile1.GetEndPosition(), 60);

			Opened = PartialFile2.Open(&File, 60, 30);
			ZETestCheck(Opened);
			ZETestCheckEqual(PartialFile2.GetStartPosition(), 60);
			ZETestCheckEqual(PartialFile2.GetEndPosition(), 90);

			Opened = PartialFile3.Open(&File, 90, 10);
			ZETestCheck(Opened);
			ZETestCheckEqual(PartialFile3.GetStartPosition(), 90);
			ZETestCheckEqual(PartialFile3.GetEndPosition(), 100);

			PartialFile3.Close();
			PartialFile2.Close();
			PartialFile1.Close();
			File.Close();
			remove("OpenTest.txt");
		}

		ZETestCase("try to intersect the partial files")
		{
			ZEPartialFile PartialFile1;
			ZEPartialFile PartialFile2;

			Open = File.Open("OpenTest.txt", ZE_FOM_READ, ZE_FCM_CREATE);

			Opened = PartialFile1.Open(&File, 100, 100);
			ZETestCheck(Opened);
			ZETestCheckEqual(PartialFile1.GetStartPosition(), 100);
			ZETestCheckEqual(PartialFile1.GetEndPosition(), 200);

			Opened = PartialFile2.Open(&File, 150, 100);
			ZETestCheck(Opened);
			ZETestCheckEqual(PartialFile2.GetStartPosition(), 150);
			ZETestCheckEqual(PartialFile2.GetEndPosition(), 250);

			PartialFile2.Close();
			PartialFile1.Close();
			File.Close();
			remove("OpenTest.txt");
		}

		ZETestCase("open partial file on a partial file")
		{
			ZEPartialFile PartialFile1;
			ZEPartialFile PartialFile2;
			ZEPartialFile PartialFile3;

			Open = File.Open("OpenTest.txt", ZE_FOM_READ, ZE_FCM_CREATE);

			Opened = PartialFile1.Open(&File, 0, 100);
			ZETestCheck(Opened);
			ZETestCheckEqual(PartialFile1.GetStartPosition(), 0);
			ZETestCheckEqual(PartialFile1.GetEndPosition(), 100);

			Opened = PartialFile2.Open((ZEFile*)&PartialFile1, 0, 50);
			ZETestCheck(Opened);
			ZETestCheckEqual(PartialFile2.GetStartPosition(), 0);
			ZETestCheckEqual(PartialFile2.GetEndPosition(), 50);

			Opened = PartialFile3.Open((ZEFile*)&PartialFile2, 10, 30);
			ZETestCheck(Opened);
			ZETestCheckEqual(PartialFile3.GetStartPosition(), 10);
			ZETestCheckEqual(PartialFile3.GetEndPosition(), 40);

			PartialFile3.Close();
			PartialFile2.Close();
			PartialFile1.Close();
			File.Close();
			remove("OpenTest.txt");
		}
	}

	ZETest("ZEInt ZEPartialFile::Close()")
	{
		ZEFile File;
		ZEPartialFile PartialFile;

		File.Open("CloseTest.txt", ZE_FOM_READ, ZE_FCM_CREATE);
		PartialFile.Open(&File, 10, 30);
		ZETestCheckEqual(PartialFile.GetStartPosition(), 10);
		ZETestCheckEqual(PartialFile.GetEndPosition(), 40);

		ZEInt Closed = PartialFile.Close();
		ZETestCheckEqual(Closed, 0);
		ZETestCheckEqual(PartialFile.GetStartPosition(), 0);
		ZETestCheckEqual(PartialFile.GetEndPosition(), 0);

		File.Close();
		remove("CloseTest.txt");

		ZETestCase("close parent file first")
		{
			File.Open("CloseTest.txt", ZE_FOM_READ, ZE_FCM_CREATE);
			PartialFile.Open(&File, 10, 30);
			ZETestCheckEqual(PartialFile.GetStartPosition(), 10);
			ZETestCheckEqual(PartialFile.GetEndPosition(), 40);

			Closed = File.Close();
			ZETestCheckEqual(Closed, 0);

			Closed = PartialFile.Close();
			ZETestCheckEqual(Closed, 0);
			ZETestCheckEqual(PartialFile.GetStartPosition(), 0);
			ZETestCheckEqual(PartialFile.GetEndPosition(), 0);

			remove("CloseTest.txt");
		}

		ZETestCase("close partial file on a partial file first")
		{
			ZEPartialFile PartialFile1;
			File.Open("CloseTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			PartialFile.Open(&File, 0, 100);
			PartialFile1.Open((ZEFile*)&PartialFile, 30, 50);

			Closed = PartialFile1.Close();
			ZETestCheckEqual(Closed, 0);
			ZETestCheckEqual(PartialFile1.GetStartPosition(), 0);
			ZETestCheckEqual(PartialFile1.GetEndPosition(), 0);
			ZETestCheckEqual(PartialFile.GetStartPosition(), 0);
			ZETestCheckEqual(PartialFile.GetEndPosition(), 100);

			Closed = PartialFile.Close();
			ZETestCheckEqual(Closed, 0);
			ZETestCheckEqual(PartialFile.GetStartPosition(), 0);
			ZETestCheckEqual(PartialFile.GetEndPosition(), 0);

			File.Close();
			remove("CloseTest.txt");
		}

		ZETestCase("close parent partial file first")
		{
			ZEPartialFile PartialFile1;
			File.Open("CloseTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			PartialFile.Open(&File, 0, 100);
			PartialFile1.Open((ZEFile*)&PartialFile, 30, 50);

			Closed = PartialFile.Close();
			ZETestCheckEqual(Closed, 0);
			ZETestCheckEqual(PartialFile.GetStartPosition(), 0);
			ZETestCheckEqual(PartialFile.GetEndPosition(), 0);
			ZETestCheckEqual(PartialFile1.GetStartPosition(), 30);
			ZETestCheckEqual(PartialFile1.GetEndPosition(), 80);

			Closed = PartialFile1.Close();
			ZETestCheckEqual(Closed, 0);
			ZETestCheckEqual(PartialFile1.GetStartPosition(), 0);
			ZETestCheckEqual(PartialFile1.GetEndPosition(), 0);

			File.Close();
			remove("CloseTest.txt");
		}
	}

	ZETest("ZESize ZEPartialFile::Write(const void* Buffer, const ZESize Size, const ZESize Count)")
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
		ZETestCheckEqual(PartialFile.GetStartPosition(), 100);
		ZETestCheckEqual(PartialFile.GetEndPosition(), 200);
		ZETestCheckEqual(PartialFile.Tell(), 0);

		ZESize WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 10);
		ZETestCheckEqual(WriteCount, 10);
		ZETestCheckEqual(PartialFile.Tell(), 10);
		ZETestCheckEqual(File.Tell(), 0);

		WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 90);
		ZETestCheckEqual(WriteCount, 90);
		ZETestCheckEqual(PartialFile.Tell(), 100);
		//EoF false

		WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 1);
		ZETestCheckEqual(WriteCount, 0);
		ZETestCheckEqual(PartialFile.Tell(), 100);
		//EoF true

		PartialFile.Close();
		File.Close();
		remove("WriteTest.txt");

		ZETestCase("try to write count > partial file size")
		{
			File.Open("WriteTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			bool a = PartialFile.Open(&File, 0, 100);

			ZETestCheckEqual(PartialFile.Tell(), 0);

			WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 110);
			ZETestCheckEqual(WriteCount, 100);
			ZETestCheckEqual(PartialFile.Tell(), 100);
			//EoF true

			PartialFile.Close();
			File.Close();
			remove("WriteTest.txt");
		}
	}

	ZETest("ZEInt ZEPartialFile::Seek(const ZEInt64 Offset, const ZESeekFrom Origin)")
	{
		unsigned char* Buffer = new unsigned char[1024];
 		for (ZEInt I = 0; I < 1024; I++)
 		{
			Buffer[I] = I % 256;
		}

 		ZEFile File;
 		ZEPartialFile PartialFile;
 
 		ZETestCase("for seek form ZE_SF_CURRENT")
 		{
 			File.Open("SeekTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			PartialFile.Open(&File, 0, 100);
 			ZETestCheckEqual(PartialFile.Tell(), 0);
 
 			ZESize WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 50);
 			ZETestCheckEqual(PartialFile.Tell(), 50);
 
			ZEInt Seek = PartialFile.Seek(-10 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
 			ZETestCheckEqual(Seek, 0);
			ZETestCheckEqual(PartialFile.Tell(), 40);
 
 			Seek = PartialFile.Seek(-40 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
 			ZETestCheckEqual(Seek, 0);
			ZETestCheckEqual(PartialFile.Tell(), 0);
 
			Seek = PartialFile.Seek(-5 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
 			ZETestCheck(Seek != 0);
 			ZETestCheckEqual(PartialFile.Tell(), 0);

 			Seek = PartialFile.Seek(50 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
 			ZETestCheckEqual(Seek, 0);
 			ZETestCheckEqual(PartialFile.Tell(), 50);
 
 			Seek = PartialFile.Seek(10 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
 			ZETestCheckEqual(Seek, 0);
 			ZETestCheckEqual(PartialFile.Tell(), 60);

 			Seek = PartialFile.Seek(50 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
 			ZETestCheck(Seek != 0);
 			ZETestCheckEqual(PartialFile.Tell(), 60);
			
			Seek = PartialFile.Seek(40 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZETestCheckEqual(Seek, 0);
			ZETestCheckEqual(PartialFile.Tell(), 100);

			Seek = PartialFile.Seek(1 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);
			ZETestCheck(Seek != 0);
			ZETestCheckEqual(PartialFile.Tell(), 100);
 
 			PartialFile.Close();
 			File.Close();
 			remove("SeekTest.txt");
 		}
 
 		ZETestCase("for seek form ZE_SF_BEGINING")
 		{
 			File.Open("SeekTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			PartialFile.Open(&File, 0, 100);
 			ZETestCheckEqual(PartialFile.Tell(), 0);
 
 			ZESize WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 50);
 			ZETestCheckEqual(PartialFile.Tell(), 50);
 
 			ZEInt Seek = PartialFile.Seek(10 * (ZEInt64)sizeof(unsigned char), ZE_SF_BEGINING);
 			ZETestCheckEqual(Seek, 0);
 			ZETestCheckEqual(PartialFile.Tell(), 10);
 
 			Seek = PartialFile.Seek(0 * (ZEInt64)sizeof(unsigned char), ZE_SF_BEGINING);
 			ZETestCheckEqual(Seek, 0);
 			ZETestCheckEqual(PartialFile.Tell(), 0);
 			
 			Seek = PartialFile.Seek(110 * (ZEInt64)sizeof(unsigned char), ZE_SF_BEGINING);
 			ZETestCheck(Seek != 0);
 			ZETestCheckEqual(PartialFile.Tell(), 0);
 
 			Seek = PartialFile.Seek(50 * (ZEInt64)sizeof(unsigned char), ZE_SF_BEGINING);
 			ZETestCheckEqual(Seek, 0);
 			ZETestCheckEqual(PartialFile.Tell(), 50);
 
 			Seek = PartialFile.Seek(-10 * (ZEInt64)sizeof(unsigned char), ZE_SF_BEGINING);
 			ZETestCheck(Seek != 0);
 			ZETestCheckEqual(PartialFile.Tell(), 50);

			Seek = PartialFile.Seek(100 * (ZEInt64)sizeof(unsigned char), ZE_SF_BEGINING);
			ZETestCheckEqual(Seek, 0);
			ZETestCheckEqual(PartialFile.Tell(), 100);
 
 			PartialFile.Close();
 			File.Close();
 			remove("SeekTest.txt");
 		}
 
 		ZETestCase("for seek form ZE_SF_END")
		{
 			File.Open("SeekTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			PartialFile.Open(&File, 0, 100);
 			ZETestCheckEqual(PartialFile.Tell(), 0);
 
 			ZESize WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 50);
 			ZETestCheckEqual(PartialFile.Tell(), 50);
 
 			ZEInt Seek = PartialFile.Seek(-10 * (ZEInt64)sizeof(unsigned char), ZE_SF_END);
 			ZETestCheckEqual(Seek, 0);
			ZETestCheckEqual(PartialFile.Tell(), 90);
 
 			Seek = PartialFile.Seek(-50 * (ZEInt64)sizeof(unsigned char), ZE_SF_END);
 			ZETestCheckEqual(Seek, 0);
			ZETestCheckEqual(PartialFile.Tell(), 50);
 
 			Seek = PartialFile.Seek(-100 * (ZEInt64)sizeof(unsigned char), ZE_SF_END);
 			ZETestCheckEqual(Seek, 0);
 			ZETestCheckEqual(PartialFile.Tell(), 0);
 
 			Seek = PartialFile.Seek(-110 * (ZEInt64)sizeof(unsigned char), ZE_SF_END);
 			ZETestCheckEqual(Seek, -1);
 			ZETestCheckEqual(PartialFile.Tell(), 0);
 
 			Seek = PartialFile.Seek(60 * (ZEInt64)sizeof(unsigned char), ZE_SF_END);
 			ZETestCheckEqual(Seek, -1);
			ZETestCheckEqual(PartialFile.Tell(), 0);
 
 			PartialFile.Close();
 			File.Close();
 			remove("SeekTest.txt");	
 		}
	}

	ZETest("ZEInt64 ZEPartialFile::Tell() const")
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
		ZETestCheckEqual(PartialCursor, 0);

		ZESize WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 24);
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
		File.Close();
		remove("TellTest.txt");
	}

	ZETest("ZESize ZEPartialFile::Read(void* Buffer, const ZESize Size, const ZESize Count)")
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
		ZETestCheckEqual(ReadCount, 10);

		ZEInt Res = memcmp(Buffer, BufferRead, sizeof(unsigned char));
		ZETestCheckEqual(Res, 0);

		PartialFile.Close();
		File.Close();
		remove("ReadTest.txt");

		ZETestCase("try to read whole partial file")
		{
			File.Open("ReadTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			PartialFile.Open(&File, 0, 1024);
			WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 1024);

			Result = PartialFile.Seek(-1024 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ReadCount = PartialFile.Read(BufferRead, sizeof(unsigned char), 1024);
			ZETestCheckEqual(ReadCount, 1024);

			Res = memcmp(Buffer, BufferRead, sizeof(unsigned char));
			ZETestCheckEqual(Res, 0);

			//EoF false

			PartialFile.Close();
			File.Close();
			remove("ReadTest.txt");
		}

		ZETestCase("try to read bigger than partial file size")
		{
			File.Open("ReadTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			PartialFile.Open(&File, 0, 1024);
			WriteCount = PartialFile.Write(Buffer, sizeof(unsigned char), 1024);

			Result = PartialFile.Seek(-1024 * (ZEInt64)sizeof(unsigned char), ZE_SF_CURRENT);

			ReadCount = PartialFile.Read(BufferRead, sizeof(unsigned char), 1100);
			ZETestCheckEqual(ReadCount, 1024);
			//EoF true

			Res = memcmp(Buffer, BufferRead, sizeof(unsigned char));
			ZETestCheckEqual(Res, 0);

			PartialFile.Close();
			File.Close();
			remove("ReadTest.txt");
		}

		ZETestCase("try to write and read bigger than partial file size")
		{
			File.Open("ReadTest.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
			PartialFile.Open(&File, 0, 100);
			WriteCount = PartialFile.Write(Buffer, sizeof(ZEUInt), 26);
			ZETestCheckEqual(WriteCount, 25);
			//EoF true

			Result = PartialFile.Seek(-30 * (ZEInt64)sizeof(ZEUInt), ZE_SF_CURRENT);
			ZETestCheckEqual(Result, -1);
			ZEInt64 PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 100);
			Result = PartialFile.Seek(-25 * (ZEInt64)sizeof(ZEUInt), ZE_SF_CURRENT);
			ZETestCheckEqual(Result, 0);
			PartialCursor = PartialFile.Tell();
			ZETestCheckEqual(PartialCursor, 0);

			ReadCount = PartialFile.Read(BufferRead, sizeof(ZEUInt), 30);
			ZETestCheckEqual(ReadCount, 25);

			Res = memcmp(Buffer, BufferRead, sizeof(ZEUInt));
			ZETestCheckEqual(Res, 0);

			PartialFile.Close();
			File.Close();
			remove("ReadTest.txt");
		}
	}
}
