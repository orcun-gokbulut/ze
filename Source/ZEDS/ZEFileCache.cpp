//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFileCache.cpp
 ------------------------------------------------------------------------------
 Copyright (C) 2008-2021 YiÄŸit OrÃ§un GÃ–KBULUT. All rights reserved.

 This file is part of the Zinek Engine  Software. Zinek Engine Software and the
 accompanying  materials are  made available  under the  terms of Zinek  Engine
 Commercial License or the GNU General Public License Version 3.

                      ZINEK ENGINE COMMERCIAL LICENSE
 Licensees  holding  valid  Zinek Engine Commercial  License(s) may  use  Zinek
 Engine  Software in  accordance  with   the  commercial  license  agreement(s)
 (which can only be  issued  by  copyright  owner "YiÄŸit  OrÃ§un  GÃ–KBULUT") and
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
  Name: YiÄŸit OrÃ§un GÃ–KBULUT
  Contact: orcun.gokbulut@gmail.com
  Github: https://www.github.com/orcun-gokbulut/ZE
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

#include "ZEFileCache.h"
#include <stdio.h>
#include "ZECore/ZEError.h"
#include <sys/stat.h> 

// EKSÝK
// Initializes the parameters(that will be use to write data to a partial cache file) 
void ZECachePartialResourceFile::Initialize(void* File, size_t StartPosition, size_t EndPosition)
{
	this->File = File;
	this->StartPosition = StartPosition;
	this->EndPosition = EndPosition;
}

// EKSÝK
// Returns the first data that matches to hash. 
// If it is not the needed item user have to call getnextchunk() function
ZEPartialResourceFile ZEFileCache::GetChunk(ZECachePartialResourceFile& ResourceFile, ZEFileCacheScan& Scan)
{
	for (size_t I = 0; I < Items.GetCount(); I++)
	{
		if(Items[I].Hash == Scan.Hash)
			ResourceFile.Initialize((FILE*)File, Items[I].FilePosition, Items[I].FilePosition + Items[I].Size);
	}
}

// EKSÝK
bool ZEFileCache::GetNextFile(ZECachePartialResourceFile& ResourceFile, ZEFileCacheScan& Scan)
{
	//bir sonraki dosyayý bulmak için deðiþtirmek lazým lazým
	for (size_t I = Scan.Cursor; I < Items.GetCount(); I++)
	{
		if (Items[I].Hash == Scan.Hash)
		{
			ResourceFile.Initialize((FILE*)File, Items[I].FilePosition, Items[I].FilePosition + Items[I].Size);
			return true;
		}
	}

	return false;
}

// EKSÝK
// Scan by hash
ZEFileCacheScan ZEFileCache::Scan(ZEDWORD Hash)
{
	// hash ile arama yapar ilk buldugu(dogru item olmayabilir) item için bir ZEFileCacheScan dondurur.
	// dondurulen item getchunk()'a gönderilerek alýnabilir.
	// Istenen item gelmezse getnextfile()/getnextchunk() ile bir sonraki item alýnarak denenebilir
}

// Adds data to cache file
void ZEFileCache::AddChunk(ZEDWORD Hash, void* Data, size_t Size)
{
	// If online mode on
	if (this->OnlineMode == true)
	{
		// Go to the beginning of item list(also means the end of previously added data)
		fseek((FILE*)File, -sizeof(ZEFileCacheItem) * Items.GetCount() - sizeof(ZEDWORD), SEEK_END);
		// Save position
		ZEDWORD Position = ftell((FILE*)File);
		// Write the data
		fwrite(Data, Size, 1, (FILE*)File);
		// Add to Items List and set the item properties
		ZEFileCacheItem* Item = Items.Add();
		Item->FilePosition = Position;
		Item->Hash = Hash;
		Item->Size = Size;
		// Write the item list
		WriteItemListToCacheFile();
		// flush after an output 
		fflush((FILE*)File);
	}
	else
	{
		// Get item list for temp usage
		ReadItemListFromCacheFile();
		fseek(File, -sizeof(ZEFileCacheItem) * Items.GetCount() - sizeof(ZEDWORD), SEEK_END);
		// Save data position
		ZEDWORD Position = ftell((FILE*)File);
		// Write the data
		fwrite(Data, Size, 1, (FILE*)File);
		// Add the item to new item list
		ZEFileCacheItem* Item = Items.Add();
		Item->FilePosition = Position;
		Item->Hash = Hash;
		Item->Size = Size;
		// Write the item list
		WriteItemListToCacheFile();
		// flush after an output 
		fflush((FILE*)File);
		// Delete item list from memory
		Items.Clear(false);
	}
	
}

// Writes the item list to the end of cache file
void ZEFileCache::WriteItemListToCacheFile()
{
	if (this->OnlineMode == true)
	{
		// Go to the end
		fseek((FILE*)File, 0, SEEK_END);
		// Write the item list
		fwrite(Items.GetConstCArray(), sizeof(ZEFileCacheItem), Items.GetCount(), (FILE*)File);
		ZEDWORD ItemCount = Items.GetCount();
		// Write the item count
		fwrite(&ItemCount, sizeof(ZEDWORD), 1, (FILE*)File);
	}
}

// Reads the list of items from cache files
void ZEFileCache::ReadItemListFromCacheFile()
{
	if (this->OnlineMode == true)
	{
		// Go backward to get the size of the list
		fseek((FILE*)File, -sizeof(ZEWORD), SEEK_END);
		ZEDWORD ItemCount;
		// Get the item count
		fread(&ItemCount, sizeof(ZEDWORD), 1, (FILE*)File);
		// Go to the beginning of item list
		fseek((FILE*)File, -(sizeof(ZEFileCacheItem) * ItemCount) -sizeof(ZEDWORD), SEEK_END);
		// Resize the Items array
		Items.Resize(ItemCount);
		// Read the list
		fread(Items.GetCArray(), sizeof(ZEFileCacheItem), ItemCount, (FILE*)File);
		// Reset the position indicator
		rewind((FILE*)File);
	}
}

// For Creating a new cache which have the same name and mode with an existing one. Deletes the previous one
// It Automatically opens the file
bool ZEFileCache::CreateNewCacheFile(const char* FileName, bool OnlineMode)
{
	// Delete if there is already a cache file with the same name
	if(DeleteCacheFile(FileName))
	{
		zeLog("File Cache:", "Deleting the existing cache file which have the name: \"%s\".", FileName);
	}
	// Create and open new cache file
	OpenCacheFile( FileName, OnlineMode);
	return true;
}

// Opens the cache file with specified option(online/offline)
// If online mode on the records list are kept in memory
// Otherwise operations are done directly on cache file(e.g. search operation). 
bool ZEFileCache::OpenCacheFile(const char* FileName, bool OnlineMode)
{
	File = fopen(FileName, "rwb");
	if (File == NULL)
	{
		zeError("File Cache", "Can not open file cache. (File Name : \"%s\")", FileName);
		return false;
	}
	this->OnlineMode = OnlineMode;
	
	if (OnlineMode == true)
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
		
		// Delete the items list if exist
		if (this->OnlineMode == true)
			Items.Clear(false);
	}
}

// Empties the list of items in the cache and creates a new cache with the same name
void ZEFileCache::ClearCacheFile(const char* FileName)
{
	if (this->OnlineMode == true)
		Items.Clear(false);
	CreateNewCacheFile(FileName, OnlineMode);
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
		
		// Delete Items list if it exist
		if (this->OnlineMode == true)
			Items.Clear(false);
		
		return true;
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

// For Changing Mode
void ZEFileCache::SetMode(bool OnlineMode)
{
	// If mode is online but user wants offline
	if (this->OnlineMode && !OnlineMode)
	{	
		this->OnlineMode = OnlineMode;
		Items.Clear(false);
	}
	// If mode is offline but user wants online
	if (!this->OnlineMode && OnlineMode)
	{
		this->OnlineMode = OnlineMode;
		ReadItemListFromCacheFile();
	}
}

// Get Mode
void ZEFileCache::IsOnlineModeOn()
{
	return this->OnlineMode;
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

