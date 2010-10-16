//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFileCache.cpp
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
#include <stdio.h>
#include "ZECore/ZEError.h"
#include <sys/stat.h> 


void ZEFileCache::WriteItemListToCacheFile()
{
	// Will Be Written
}

void ZEFileCache::DumpItemListOfCacheFile()
{
	fseek((FILE*)File, 0, SEEK_END);
	fwrite(Items.GetConstCArray(), sizeof(ZEFileCacheItem), Items.GetCount(), (FILE*)File);
	ZEDWORD ItemCount = Items.GetCount();
	fwrite(&ItemCount, sizeof(ZEDWORD), 1, (FILE*)File);
}

void ZEFileCache::Add(ZEDWORD Hash, void* Data, size_t Size)
{
	fseek((FILE*)File, sizeof(ZEFileCacheItem) * Items.GetCount() + sizeof(ZEDWORD), SEEK_END);
	ZEDWORD Position = ftell((FILE*)File);

	fwrite(Data, Size, 1, (FILE*)File);

	ZEFileCacheItem* Item = Items.Add();
	Item->FilePosition = Position;
	Item->Hash = Hash;
	Item->Size = Size;

	DumpItemListOfCacheFile();
	fflush((FILE*)File);
}

// NOT COMPLETE
ZEFileCacheScan ZEFileCache::Scan(ZEDWORD Hash)
{
	ZEFileCacheScan Scan;
	Scan.Cursor = 0;
	Scan.Hash = Hash;
	return Scan;
}

bool ZEFileCache::GetNextFile(ZECachePartialResourceFile& ResourceFile, ZEFileCacheScan& Scan)
{
	for (size_t I = Scan.Cursor; I < Items.GetCount(); I++)
	{
		if (Items[I].Hash == Scan.Hash)
		{
			ResourceFile.Initialize((FILE*)File, Items[I].FilePosition, Items[I].Size);
			return true;
		}
	}

	return false;
}

// Reads the list of items in the file cache from cache file
void ZEFileCache::ReadItemListFromCacheFile()
{
	// Go to the end
	fseek((FILE*)File, 0, SEEK_END);
	// Go backward to get the size of the list
	fseek((FILE*)File, -1 * sizeof(ZEWORD), SEEK_CUR);
	ZEDWORD ItemCount;
	// Get the item count
	fread(&ItemCount, sizeof(ZEDWORD), 1, (FILE*)File);
	// Go to the beginning of item list
	fseek((FILE*)File, -1 * (sizeof(ZEFileCacheItem) * ItemCount), SEEK_CUR);
	// Resize the Items array
	Items.Resize(ItemCount);
	// Read the list
	fread(Items.GetCArray(), sizeof(ZEFileCacheItem), ItemCount, (FILE*)File);
	// Reset the position indicator
	rewind((FILE*)File);
}

// For Creating a new cache which have the same name with an existing one. Deletes the previous one
bool ZEFileCache::CreateNewCacheFile(const char* FileName, bool OnlineMode)
{
	DeleteCacheFile(FileName);
	return OpenCacheFile(FileName, OnlineMode);
}

// Opens the cache file with specified options(online/offline) and gets the list of items stored in the cache file
bool ZEFileCache::OpenCacheFile(const char* FileName, bool OnlineMode)
{
	File = fopen(FileName, "rwb");
	if (File == NULL)
	{
		zeError("File Cache", "Can not open file cache. (File Name : \"%s\")", FileName);
		return false;
	}
	this->OnlineMode = OnlineMode;
	ReadItemListFromCacheFile();

	return true;
}

// Closes the cache file.
void ZEFileCache::CloseCacheFile()
{
	if (File != NULL)
	{
		fclose((FILE*)File);
		File = NULL;
	}
}

// Empties the list of items in the cache and creates a new cache with the same name
void ZEFileCache::ClearCacheFile(const char* FileName)
{
	Items.Clear(false);
	CreateNewCacheFile(FileName, true);
}

// Deletes an existing cache file
bool ZEFileCache::DeleteCacheFile(const char* FileName)
{
	if(CacheFileExists(FileName))
	{
		if(remove(FileName) != 0)
		{
			zeError("File Cache", "Can not delete existing cache. (File Name : \"%s\")", FileName);
			return false;
		}
		else
		{
			return true;
		}
	}
	return false;
}

// Checks if cache file exists
bool ZEFileCache::CacheFileExists(const char* FileName)
{
	char CompletePath[256];
	sprintf(CompletePath, "resources\\%s.cache", FileName);

	int Stat;
	struct stat FileInfo;

	Stat = stat(CompletePath, &FileInfo);

	if (Stat == 0)
		return true;

	return false;
}

// Constructor
ZEFileCache::ZEFileCache()
{
	File = NULL;
}

// Destructor
ZEFileCache::~ZEFileCache()
{
	CloseCacheFile();
}

// Initializes the parameters(that will be use to to write data to a partial cache file) 
void ZECachePartialResourceFile::Initialize(void* File, size_t StartPosition, size_t EndPosition)
{
	this->File = File;
	this->StartPosition = StartPosition;
	this->EndPosition = EndPosition;
}
