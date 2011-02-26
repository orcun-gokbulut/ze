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
#include "ZEConsole.h"
#include "ZECore/ZEError.h"
#include <sys/stat.h> 

void ZEFileCacheItem::reset(void)
{
	Index = Hash = FilePosition = Size = 0;
}

ZEFileCacheItem::ZEFileCacheItem(ZEFileCacheItem &CacheItem)
{
	Index = CacheItem.Index;
	Hash = CacheItem.Hash;
	FilePosition = CacheItem.FilePosition;
	Size = CacheItem.Size;
}


// Initializes the parameters(that will be use to write data to a partial cache file) 
void ZECachePartialResourceFile::Initialize(void* File, size_t StartPosition, size_t EndPosition)
{
	this->File = File;
	this->StartPosition = StartPosition;
	this->EndPosition = EndPosition;
}

// Get data of a valid cache item represented by ZEFileCacheItem
bool ZEFileCache::GetChunk(ZECachePartialResourceFile& ResourceFile, ZEFileCacheItem& Item)
{
	ResourceFile.Initialize((FILE*)File, Item.FilePosition, Item.FilePosition + Item.Size);
	return true;
}


// Scan by hash
// Searches the item list in cache file one by one to find the specified hash value
// Returns first found! User should pass a ZEFileCacheItem struct with a specified hash value.  
// The function fills the rest of the struct ! Afterwards The Chunk can be loaded by sending the 
// filled ZEFileCacheItem to GetChunk() function. If the returned item by GetChunk() it is not the 
// desired item user should use ScanByHash() again with the same struct filled before so that the function will
// set the  ZEFileCacheItem struct with the information of the next item which have the same hash.
// Function returns true if it finds/fills the struct or false if search fails. in  case of getting a false 
// return data the ZEFileCacheItem must be reseted by reset() to be able to be used again. 
// Also if u get a false return the last state of struct is conserved
bool ZEFileCache::ScanByHash(ZEFileCacheItem &FileScan)
{
	size_t I = 0;
	
	if (FileScan.Index > 0)
		I = FileScan.Index;
	
	if (IsOnlineModeOn())
	{
		for ( ; I < Items.GetCount(); I++)
			if (Items[I].Hash == FileScan.Hash)
			{
				FileScan.Index = I;
				FileScan.FilePosition = Items[I].FilePosition;
				FileScan.Size = Items[I].Size;
				return true;
			}
	}
	else
	{
		// Go to the end to get the size of the list
		fseek((FILE*)File, -sizeof(ZEWORD), SEEK_END);
		ZEDWORD ItemCount;
		// Get the item count
		fread(&ItemCount, sizeof(ZEDWORD), 1, (FILE*)File);
		// Go to the beginning of item list + Index + 1
		fseek((FILE*)File, -(sizeof(ZEFileCacheItem) * (ItemCount - I + 1)) -sizeof(ZEDWORD), SEEK_END);
		
		ZEFileCacheItem	Item(0, 0, 0, 0);
		
		// Check until we find the first item in the list that fits the hash
		for ( ; I < ItemCount; I++)
		{	
			// Read the item data
			fread(&Item, sizeof(ZEFileCacheItem), 1, (FILE*)File);

			if (Item.Hash == FileScan.Hash)
			{
				FileScan.Index = I;
				FileScan.FilePosition = Item.FilePosition;
				FileScan.Size = Item.Size;
				return Scan;
			}
		}
	}

	zeError("File Cache:", "Cannot find the data specified by the hash %d in specified cache file \"%s\"", FileScan.Hash, File);
	return false;

}

// Adds data to cache file
// The mode does not make much difference since we need to get the whole list 
// into the memory and write it to the end of the cache file in both modes
void ZEFileCache::AddChunk(ZEDWORD Hash, void* Data, ZEDWORD Size)
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
		// Flush after an output 
		fflush((FILE*)File);
	}
	else // If mode is offline
	{
		// Get item list for temp usage
		ReadItemListFromCacheFile();
		fseek((FILE*)File, -sizeof(ZEFileCacheItem) * Items.GetCount() - sizeof(ZEDWORD), SEEK_END);
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
		// Flush after an output 
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

// Reads the list of items from cache files to memory
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
bool ZEFileCache::CreateCacheFile(const char* FileName, bool OnlineMode)
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
	CreateCacheFile(FileName, OnlineMode);
}

// Deletes an existing cache file
// Also deletes from memory
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
// When mode changes from online to offline the item list is dumped
// In opposite situation the item list is read from the cache file
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
bool ZEFileCache::IsOnlineModeOn()
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

