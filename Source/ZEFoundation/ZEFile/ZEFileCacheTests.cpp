//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFileCacheTests.cpp
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

#include "ZEFileCache.h"
#include "ZEFile.h"
#include "ZETest/ZETest.h"
#include "ZETDefinitions.h"
#include "ZETypes.h"
#include "ZEError.h"

/*void DeleteFile(const char* filepath)
{
	remove(filepath);
}*/


class ZECacheTextDataIdentifier : public ZECacheDataIdentifier
{
	public:
		char ItemName[ZE_MAX_FILE_NAME_SIZE];

		ZECacheTextDataIdentifier(const char* ItemName)
		{
			sprintf_s(this->ItemName, sizeof(char) * ZE_MAX_FILE_NAME_SIZE, "%s", ItemName);
		}
		
		~ZECacheTextDataIdentifier()
		{

		}

		virtual ZEUInt64	GetHash() const
		{
			ZEUInt Hash = 0;
			ZEUInt I = 0;	
			ZEInt Char = 0;

			while (I < ZE_MAX_FILE_NAME_SIZE)
			{
				Char = ItemName[I];
				Hash = Char + (Hash << 6) + (Hash << 16) - Hash;
				I++;
			}

			return (ZEUInt64)Hash;
		}
		virtual ZEUInt64	Write(ZEFile* File) const
		{
			ZEUInt64 BytesWritten = 0;
			ZEUInt64 WriteCount = 0;
			ZESize ZeCharSize = sizeof(char);

			WriteCount = File->Write(ItemName, (ZEUInt64)ZeCharSize, ZE_MAX_FILE_NAME_SIZE);
			if (WriteCount != ZE_MAX_FILE_NAME_SIZE)
			{
				return 0;
			}
			else
			{
				BytesWritten += WriteCount * (ZEUInt64)ZeCharSize;
				WriteCount = 0;
			}

			return BytesWritten ;
		}
		virtual bool Equal(ZEFile* File) const
		{
			char ItemNameBuffer[ZE_MAX_FILE_NAME_SIZE];
			if (File->Read(ItemNameBuffer, sizeof(char), ZE_MAX_FILE_NAME_SIZE) != ZE_MAX_FILE_NAME_SIZE)
			{
				zeDebugCheck(true, "Cannot read item name from cache: \"%s\".", File->GetPath().GetValue());
				return false;
			}

			ZEUInt I = 0;
			ZEUInt StringLenght = strlen(ItemNameBuffer);
			while (I < StringLenght)
			{
				if(ItemNameBuffer[I] != ItemName[I])
					return false;
				I++;
			}

			return true;
		}

}; // Class ZECacheTextDataIdentifier




ZE_TEST(ZEFileCache)
{
	ZE_TEST_ITEM(" bool ZEFileCache::Open(const ZEString FileName)")
	{
		ZEFileCache FileCache;

		FileCache.Open("ZEFileCacheOpenTests.txt");
		ZE_TEST_CHECK_ERROR(FileCache.IsOpen());

		FileCache.Close();
		remove("ZEFileCacheOpenTests.txt");
	}

	ZE_TEST_ITEM(" bool ZEFileCache::Open(ZEFile * File)")
	{
		ZEFile File;
		File.Open("ZEFileCacheOpenTests2.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
		ZEFileCache FileCache;

		FileCache.Open(&File);
		ZE_TEST_CHECK_ERROR(FileCache.IsOpen());

		FileCache.Close();
		File.Close();
		remove("ZEFileCacheOpenTests2.txt");
	}

	ZE_TEST_ITEM("bool ZEFileCache::IsOpen()")
	{
		ZEFileCache FileCache;

		ZE_TEST_CASE("True")
		{
			FileCache.Open("ZEFileCacheIsOpenTests.txt");
			ZE_TEST_CHECK_ERROR(FileCache.IsOpen());

			FileCache.Close();
			remove("ZEFileCacheIsOpenTests.txt");
		}

		ZE_TEST_CASE("False")
		{
			FileCache.Open("ZEFileCacheIsOpenTests2.txt");
			FileCache.Close();
			ZE_TEST_CHECK_ERROR(!FileCache.IsOpen());

			remove("ZEFileCacheIsOpenTests2.txt");
		}
	}

	ZE_TEST_ITEM("ZEString ZEFileCache::GetCacheFilePath()")
	{
		ZEFileCache FileCache;
		FileCache.Open("ZEFileCacheGetCacheFilePathTests.txt");

		const ZEString FilePath = FileCache.GetCacheFilePath();
		ZE_TEST_CHECK_STRING_EQUAL(FilePath, "ZEFileCacheGetCacheFilePathTests.txt");

		FileCache.Close();
		remove("ZEFileCacheGetCacheFilePathTests.txt");
	}

	ZE_TEST_ITEM("ZEFile ZEFileCache::GetFile()")
	{
		ZEFileCache FileCache;
		FileCache.Open("ZEFileCacheGetFileTests.txt");

		ZEFile File = FileCache.GetFile();
		ZE_TEST_CHECK_EQUAL(File.GetOpenMode(), ZE_FOM_READ_WRITE);
		ZE_TEST_CHECK_EQUAL(File.GetCreationMode(), ZE_FCM_CREATE);
		ZE_TEST_CHECK_STRING_EQUAL(File.GetPath(), "ZEFileCacheGetFileTests.txt");

		FileCache.Close();
		remove("ZEFileCacheGetFileTests.txt");
	}

	ZE_TEST_ITEM("void ZEFileCache::Close()")
	{
		ZEFileCache FileCache;
		FileCache.Open("ZEFileCacheCloseTests.txt");
		ZE_TEST_CHECK_ERROR(FileCache.IsOpen());

		FileCache.Close();
		ZE_TEST_CHECK_ERROR(!FileCache.IsOpen());
		
		remove("ZEFileCacheCloseTests.txt");
	}

	ZE_TEST_ITEM("static bool ZEFileCache::IsFileCache(ZEString FileName)")
	{
		ZE_TEST_CASE("True")
		{
			bool Result = ZEFileCache::IsFileCache("ZEFileCacheCloseTests.txt");
			ZE_TEST_CHECK_ERROR(Result);
			remove("ZEFileCacheCloseTests.txt");
		}

		ZE_TEST_CASE("False")
		{
// 			ZEFile File;
// 			File.Open("ZEFileCacheIsFileCacheTests.txt", ZE_FOM_READ_WRITE, ZE_FCM_CREATE);
// 
// 			bool Result = ZEFileCache::IsFileCache("ZEFileCacheIsFileCacheTests.txt");
// 			ZE_TEST_CHECK_ERROR(!Result);
// 
// 			File.Close();
// 			remove("ZEFileCacheIsFileCacheTests.txt");
			
			//ZE_TEST_CHECK_ERROR(!ZEFileCache::IsFileCache("ZEfile.txt"));
		}
	}

	ZE_TEST_ITEM("bool ZEFileCache::AddData(const ZECacheDataIdentifier * Identifier, const void * Data, ZEUInt64 Size)")
	{
		ZECacheTextDataIdentifier Identifier1("ZETestData1");
		ZECacheTextDataIdentifier Identifier2("ZETestData2");
		ZECacheTextDataIdentifier Identifier3("ZETestData3");
		ZECacheTextDataIdentifier Identifier4("ZETestData4");
		ZECacheTextDataIdentifier Identifier5("ZETestData5");
		ZECacheTextDataIdentifier Identifier6("ZETestData6");
		ZECacheTextDataIdentifier Identifier7("ZETestData7");
		ZECacheTextDataIdentifier Identifier8("ZETestData8");
		ZECacheTextDataIdentifier Identifier9("ZETestData9");
		ZECacheTextDataIdentifier Identifier10("ZETestData10");

		unsigned char* Buffer = new unsigned char[4999];
		for (ZEInt I = 0; I < 4999; I++)
		{
			Buffer[I] = I % 256;
		}

		ZEFileCache FileCache;
		FileCache.Open("ZETestData.cagri");
		
		ZE_TEST_CHECK_ERROR(FileCache.AddData(&Identifier1, Buffer, sizeof(unsigned char) * 4999));
		ZE_TEST_CHECK_ERROR(FileCache.AddData(&Identifier2, Buffer, sizeof(unsigned char) * 4999));
		ZE_TEST_CHECK_ERROR(FileCache.AddData(&Identifier3, Buffer, sizeof(unsigned char) * 4999));
		ZE_TEST_CHECK_ERROR(FileCache.AddData(&Identifier5, Buffer, sizeof(unsigned char) * 4999));
		ZE_TEST_CHECK_ERROR(FileCache.AddData(&Identifier4, Buffer, sizeof(unsigned char) * 4999));
		ZE_TEST_CHECK_ERROR(FileCache.AddData(&Identifier6, Buffer, sizeof(unsigned char) * 4999));
		ZE_TEST_CHECK_ERROR(FileCache.AddData(&Identifier7, Buffer, sizeof(unsigned char) * 4999));
		ZE_TEST_CHECK_ERROR(FileCache.AddData(&Identifier8, Buffer, sizeof(unsigned char) * 4999));
		ZE_TEST_CHECK_ERROR(FileCache.AddData(&Identifier9, Buffer, sizeof(unsigned char) * 4999));
		ZE_TEST_CHECK_ERROR(FileCache.AddData(&Identifier10, Buffer, sizeof(unsigned char) * 4999));
		ZE_TEST_CHECK_ERROR(FileCache.IdentifierExists(&Identifier1));
		ZE_TEST_CHECK_ERROR(FileCache.IdentifierExists(&Identifier7));
		ZE_TEST_CHECK_ERROR(FileCache.IdentifierExists(&Identifier10));
		FileCache.Close();

		remove("ZETestData.cagri");
	}

	ZE_TEST_ITEM("bool ZEFileCache::Clear()")
	{
		ZEFileCache FileCache;

		ZE_TEST_CASE("True (after open)")
		{
			unsigned char* Buffer = new unsigned char[4999];
			for (ZEInt I = 0; I < 4999; I++)
			{
				Buffer[I] = I % 256;
			}

			ZEFile File;
			//ZEUInt64 Word;
			FileCache.Open("ZEFileCacheClearTests.txt");
			ZECacheTextDataIdentifier Identifier1("ZETestData1");
			FileCache.AddData(&Identifier1, Buffer, sizeof(unsigned char) * 4999);
			
			ZE_TEST_CHECK_ERROR(FileCache.Clear());

			FileCache.Close();
			//File = FileCache.GetFile();
			//Word = File.GetFileSize();
			//ZE_TEST_CHECK_EQUAL(Word, 0);
		}

		ZE_TEST_CASE("false(still close)")
		{
			ZE_TEST_CHECK_ERROR(!FileCache.Clear());

			remove("ZEFileCacheClearTests.txt");
		}
	}

	ZE_TEST_ITEM("bool ZEFileCache::GetData(const ZECacheDataIdentifier * Identifier, void * Buffer, ZEUInt64 Offset, ZEUInt64 Size)")
	{

		ZECacheTextDataIdentifier Identifier1("ZETestData1");
		ZECacheTextDataIdentifier Identifier2("ZETestData2");
		ZECacheTextDataIdentifier Identifier3("ZETestData3");
		ZECacheTextDataIdentifier Identifier4("ZETestData4");
		ZECacheTextDataIdentifier Identifier5("ZETestData5");
		ZECacheTextDataIdentifier Identifier6("ZETestData6");
		ZECacheTextDataIdentifier Identifier7("ZETestData7");
		ZECacheTextDataIdentifier Identifier8("ZETestData8");
		ZECacheTextDataIdentifier Identifier9("ZETestData9");
		ZECacheTextDataIdentifier Identifier10("ZETestData10");

		unsigned char* Buffer1 = new unsigned char[4999];
		unsigned char* Buffer2 = new unsigned char[4999];
		unsigned char* Buffer = new unsigned char[4999];
		for (ZEInt I = 0; I < 4999; I++)
		{
			Buffer1[I] = I % 256;
			Buffer2[4998 - I] = I % 256;
		}

		ZEFileCache FileCache;
		FileCache.Open("ZETestData.gokay");

		FileCache.AddData(&Identifier1, Buffer1, sizeof(unsigned char) * 4999);
		FileCache.AddData(&Identifier2, Buffer2, sizeof(unsigned char) * 4999);
		FileCache.AddData(&Identifier3, Buffer1, sizeof(unsigned char) * 4999);
		FileCache.AddData(&Identifier5, Buffer2, sizeof(unsigned char) * 4999);
		FileCache.AddData(&Identifier4, Buffer1, sizeof(unsigned char) * 4999);
		FileCache.AddData(&Identifier6, Buffer2, sizeof(unsigned char) * 4999);
		FileCache.AddData(&Identifier7, Buffer2, sizeof(unsigned char) * 4999);
		FileCache.AddData(&Identifier8, Buffer2, sizeof(unsigned char) * 4999);
		FileCache.AddData(&Identifier9, Buffer1, sizeof(unsigned char) * 4999);
		FileCache.AddData(&Identifier10, Buffer1, sizeof(unsigned char) * 4999);
		
		ZE_TEST_CHECK_ERROR(FileCache.GetData(&Identifier1, Buffer, 0, sizeof(unsigned char) * 4999));
		ZE_TEST_CHECK_ERROR(!memcmp(Buffer1, Buffer, sizeof(unsigned char) * 4999));
		
		ZE_TEST_CHECK_ERROR(FileCache.GetData(&Identifier5, Buffer, 0, sizeof(unsigned char) * 4999));
		ZE_TEST_CHECK_ERROR(!memcmp(Buffer2, Buffer, sizeof(unsigned char) * 4999));

		ZE_TEST_CHECK_ERROR(FileCache.GetData(&Identifier10, Buffer, 0, sizeof(unsigned char) * 4999));
		ZE_TEST_CHECK_ERROR(!memcmp(Buffer1, Buffer, sizeof(unsigned char) * 4999));

		ZE_TEST_CHECK_ERROR(FileCache.GetData(&Identifier7, Buffer, 0, sizeof(unsigned char) * 4999));
		ZE_TEST_CHECK_ERROR(!memcmp(Buffer2, Buffer, sizeof(unsigned char) * 4999));

		FileCache.Close();
		remove("ZETestData.gokay");	
	}

	ZE_TEST_ITEM("bool ZEFileCache::Allocate(ZEPartialFile* PartialFile, const ZECacheDataIdentifier* Identifier, ZEUInt64 ChunkSize)")
	{
		ZE_TEST_CASE("writing by using partial file")
		{
			ZEPartialFile PartialFile;
			ZEPartialFile PartialFile2;
			ZECacheTextDataIdentifier Identifier1("ZETestData1");
			ZECacheTextDataIdentifier Identifier2("ZETestData2");
			ZECacheTextDataIdentifier Identifier3("ZETestData3");
			ZECacheTextDataIdentifier Identifier4("ZETestData4");
			ZECacheTextDataIdentifier Identifier5("ZETestData5");
			ZEFileCache FileCache;
			unsigned char* Buffer = new unsigned char[4999];
			for (ZEInt I = 0; I < 4999; I++)
			{
				Buffer[I] = I % 256;
			}

			FileCache.Open("ZETestData.elif");
			ZE_TEST_CHECK_ERROR(FileCache.Allocate(&PartialFile, &Identifier1, sizeof(unsigned char) * 4999 * 2));
			ZE_TEST_CHECK_ERROR(FileCache.Allocate(&PartialFile2, &Identifier3, sizeof(unsigned char) * 4999 * 4));
			PartialFile.Write(Buffer, sizeof(unsigned char), 4999);
			PartialFile2.Write(Buffer, sizeof(unsigned char), 4999);
			PartialFile2.Seek(4999, ZE_SF_CURRENT);
			PartialFile2.Write(Buffer, sizeof(unsigned char), 4999);

			PartialFile2.Close();
			PartialFile.Close();
			FileCache.Close();

			remove("ZETestData.elif");
		}

		ZE_TEST_CASE("writing file after closing cache")
		{
			ZEPartialFile PartialFile;
			ZEPartialFile PartialFile2;
			ZEFileCache FileCache;
			ZECacheTextDataIdentifier Identifier1("ZETestData1");
			ZECacheTextDataIdentifier Identifier3("ZETestData3");
			unsigned char* Buffer = new unsigned char[4999];
			for (ZEInt I = 0; I < 4999; I++)
			{
				Buffer[I] = I % 256;
			}

			FileCache.Open("ZETestData.cagri2");
			ZE_TEST_CHECK_ERROR(FileCache.Allocate(&PartialFile, &Identifier1, sizeof(unsigned char) * 4999 * 2));
			ZE_TEST_CHECK_ERROR(FileCache.Allocate(&PartialFile2, &Identifier3, sizeof(unsigned char) * 4999 * 4));

			FileCache.Close();
			ZE_TEST_CHECK_EQUAL(PartialFile.Write(Buffer, sizeof(unsigned char), 4999), 4999);
			ZE_TEST_CHECK_EQUAL(PartialFile2.Write(Buffer, sizeof(unsigned char), 4999), 4999);
			ZE_TEST_CHECK_EQUAL(PartialFile2.Seek(4999, ZE_SF_CURRENT), 0);
			ZE_TEST_CHECK_EQUAL(PartialFile2.Write(Buffer, sizeof(unsigned char), 4999), 4999);
			remove("ZETestData.cagri2");
		}
	}

	ZE_TEST_ITEM("bool ZEFileCache::OpenData(ZEPartialFile* PartialFile, const ZECacheDataIdentifier* Identifier)")
	{
		
		ZEPartialFile PartialFile;
		ZEPartialFile PartialFile2;
		ZECacheTextDataIdentifier Identifier1("ZETestData1");
		ZECacheTextDataIdentifier Identifier2("ZETestData2");
		ZECacheTextDataIdentifier Identifier3("ZETestData3");
		ZECacheTextDataIdentifier Identifier4("ZETestData4");
		ZECacheTextDataIdentifier Identifier5("ZETestData5");
		ZEFileCache FileCache;
		unsigned char* Buffer = new unsigned char[4999];
		unsigned char* BufferRead = new unsigned char[4999];
		for (ZEInt I = 0; I < 4999; I++)
		{
			Buffer[I] = I % 256;
		}

		FileCache.Open("ZETestData.elif");
		ZE_TEST_CHECK_ERROR(FileCache.Allocate(&PartialFile, &Identifier1, sizeof(unsigned char) * 4999 * 2));
		ZE_TEST_CHECK_ERROR(FileCache.Allocate(&PartialFile2, &Identifier3, sizeof(unsigned char) * 4999 * 2));
		PartialFile.Write(Buffer, sizeof(unsigned char), 4999);
		PartialFile2.Write(Buffer, sizeof(unsigned char),4999);
		PartialFile2.Seek(4999, ZE_SF_CURRENT);
		PartialFile2.Write(Buffer, sizeof(unsigned char), 4999);
		
		PartialFile.Close();
		PartialFile2.Close();
		FileCache.Close();
		

		FileCache.Open("ZETestData.elif");
		ZE_TEST_CHECK_ERROR(FileCache.OpenData(&PartialFile, &Identifier1));
		ZE_TEST_CHECK_ERROR(FileCache.OpenData(&PartialFile2, &Identifier3));
		PartialFile.Read(BufferRead, sizeof(unsigned char), 4999);
		PartialFile2.Seek(2 * 4999, ZE_SF_BEGINING);
		PartialFile2.Read(BufferRead, sizeof(unsigned char), 4999);
		ZE_TEST_CHECK_ERROR(!memcmp(BufferRead, Buffer, 4999));

		PartialFile.Close();
		PartialFile2.Close();
		FileCache.Close();
		remove("ZETestData.elif");

	}
}
