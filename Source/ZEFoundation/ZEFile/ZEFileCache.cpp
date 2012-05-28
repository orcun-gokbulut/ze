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

#include "ZEError.h"
#include "ZEFileCache.h"

bool ZEFileCache::CheckCompleteness()
{
	ZESize ZEQuadWordSize = sizeof(ZEUInt64);

	ZEUInt64 Cursor = File.Tell();
	// Go to end of the file and read cache completeness flag
	ZEUInt64 Complete = 0;

	if (!File.Seek(-1 * (ZEInt64)ZEQuadWordSize, ZE_SF_END))
	{
		zeAssert(true, "File Cache", "Cannot seek");
		return false;
	}
	
	if (File.Read(&Complete, ZEQuadWordSize, 1) != 1)
	{
		zeAssert(true, "File Cache", "Cannot read from cache");
		return false;
	}
	
	if (!File.Seek(Cursor, ZE_SF_BEGINING))
	{
		zeAssert(true, "File Cache", "Cannot seek");
		return false;
	}
	
	if (Complete == ZE_CACHE_COMPLETENESS)
		return true;
	
	zeLog("Cannot verify completeness of cache file: \"%s\".", File.GetFilePath().ToCString());
	return false;
}

bool ZEFileCache::PrepareCacheForFirstUse()
{
	// Write file header
	ZECacheFileHeader Header;
	Header.ChunkId = ZE_CACHE_HEADER;
	Header.Version = ZE_CACHE_VERSION;

	if (File.Write(&Header, sizeof(ZECacheFileHeader), 1) != 1)
	{
		zeAssert(true, "Can not write file header to file: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}
	

	// Write records start position
	// Since there is no records just write cursor position
	ZEUInt64 Cursor = File.Tell();
	if (File.Write(&Cursor, sizeof(ZEUInt64), 1) != 1)
	{
		zeAssert(true, "Can not write records start position to file: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}
	
	// Write the completeness flag
	ZEUInt64 Complete = ZE_CACHE_COMPLETENESS;
	if (File.Write(&Complete, sizeof(ZEUInt64), 1) != 1)
	{
		zeAssert(true, "Can not write completeness flag to file: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}
	

	File.Flush();
	return true;
}

// Helper function for copying Records
bool ZEFileCache::CopyData(ZEFile* File, ZEUInt64 From, ZEUInt64 Size, ZEUInt64 To)
{
	const ZEInt64 BufferSize = 32768; // 32K
	char Buffer[BufferSize];

	ZEInt64 Count = Size / BufferSize;
	ZEInt64 LeftOver = Size % BufferSize;

	// First copy leftover
	if (LeftOver != 0)
	{
		// Go to beginning of leftover
		if (!File->Seek((ZEInt64)From + (ZEInt64)Size - LeftOver, ZE_SF_BEGINING))
		{
			zeAssert(true, "Can not seek. File: \"%s\".", File->GetFilePath().GetValue());
			return false;
		}

		if (File->Read(Buffer, LeftOver, 1) != 1)
		{
			zeAssert(true, "Can not read. File: \"%s\".", File->GetFilePath().GetValue());
			return false;
		}

		if (!File->Seek((ZEInt64)To + (ZEInt64)Size - LeftOver, ZE_SF_BEGINING))
		{
			zeAssert(true, "Can not seek. File: \"%s\".", File->GetFilePath().GetValue());
			return false;
		}

		if (File->Write(Buffer, LeftOver, 1) != 1)
		{
			zeAssert(true, "Can not write. File: \"%s\".", File->GetFilePath().GetValue());
			return false;
		}
	}

	// Copy backward to avoid corruption due to overlap
	for (ZESize I = (ZESize)Count; I > 0; --I)
	{
		if (!File->Seek((ZEInt64)From + (I - 1) * (ZEInt64)BufferSize, ZE_SF_BEGINING))
		{
			zeAssert(true, "Can not seek. File: \"%s\".", File->GetFilePath().GetValue());
			return false;
		}
		
		if (File->Read(Buffer, BufferSize, 1) != 1)
		{
			zeAssert(true, "Can not read. File: \"%s\".", File->GetFilePath().GetValue());
			return false;
		}
		
		if (!File->Seek((ZEInt64)To + (I - 1) * (ZEInt64)BufferSize, ZE_SF_BEGINING))
		{
			zeAssert(true, "Can not seek. File: \"%s\".", File->GetFilePath().GetValue());
			return false;
		}
		
		if (File->Write(Buffer, BufferSize, 1) != 1)
		{
			zeAssert(true, "Can not write. File: \"%s\".", File->GetFilePath().GetValue());
			return false;
		}
	}

	

	File->Flush();
	return true;
}

bool ZEFileCache::IsFileCache(ZEString FileName)
{
	ZEFile Check;

	if(!Check.Open(FileName, ZE_FM_READ_WRITE, true))
	{
		Check.Close();
		zeError("Cannot open file to check the format");
		return false;
	}

	if (Check.GetFileSize() == 0)
	{
		Check.Close();
		return true;
	}

	ZECacheFileHeader Header;
	// if file is empty we wont read anything
	if (Check.Read(&Header, sizeof(ZECacheFileHeader) ,1) != 1)
	{
		zeAssert(true, "Can not read. File: \"%s\".", FileName.GetValue());
		Check.Close();
		return false;
	}

	if(Header.ChunkId == ZE_CACHE_HEADER)
	{
		Check.Close();
		return true;
	}

	Check.Close();
	return false;
}

ZEFileCache::ZEFileCache()
{
	 // Empty
}

ZEFileCache::~ZEFileCache()
{
	// Empty
}

// Opens the cache file
bool ZEFileCache::Open(ZEString FileName)
{
	// If file is not open return
	if(IsOpen())
	{
		zeLog("Cache is already open with the file \"%s\".", File.GetFilePath().ToCString());
		return false;
	}

	// Check if it is a File cache
	if (!IsFileCache(FileName))
		return false;

	// Open
	if (!File.Open(FileName.GetValue(), ZE_FM_READ_WRITE, true))
		return false;

	// If it is a new file
	if (File.GetFileSize() == 0)
	{
		PrepareCacheForFirstUse();
		return true;
	}
	else // Check format and completeness
	{
		// If cache is not complete
		if(!CheckCompleteness())
		{
			zeLog("Previous data cannot be trusted, cleaning cache.");
			zeAssert(true, "Identifier Chunk Id does not match! Possible Corruption");
			Clear();
		}

		return true;
	}

	return false;
}

// Opens cache with a previously opened ZEFile
bool ZEFileCache::Open(ZEFile* File)
{
	this->File = *File;

	// If it is a new file
	if (this->File.GetFileSize() == 0)
	{
		PrepareCacheForFirstUse();
		return true;
	}
	else // Check format and completeness
	{
		// If cache is not complete
		if(!CheckCompleteness())
		{
			zeLog("Previous data cannot be trusted, cleaning cache.");
			zeAssert(true, "Identifier Chunk Id does not match! Possible Corruption");
			Clear();
		}

		return true;
	}

	return false;
}

// Closes the cache file
void ZEFileCache::Close()
{
	if (IsOpen())
		File.Close();

}

// Empty the cache file
bool ZEFileCache::Clear()
{
	// If file is not open return
	if(!IsOpen())
	{
		zeLog("Cahce is not open, cannot clear content.");
		return false;
	}

	ZEString FileName;

	FileName = File.GetFilePath();
	Close();

	// Remove the file
	if(remove(FileName.GetValue()) == -1)
		return false;

	// ReOpen the cache	
	if(Open(FileName))
		return true;

	return false;
}

// Returns the cache file name
const ZEString ZEFileCache::GetCacheFilePath()
{
	return File.GetFilePath();
}

ZEFile ZEFileCache::GetFile()
{
	return File;
}

bool ZEFileCache::IsOpen()
{
	if(File.IsOpen())
		return true;

	return false;
}

// Checks if the chunk exists
bool ZEFileCache::IdentifierExists(const ZECacheDataIdentifier* Identifier)
{
	// If file is not open return
	if(!IsOpen())
	{
		zeLog("Cahce is not open, cannot check identifier.");
		return false;
	}

	if(!CheckCompleteness())
	{
		zeLog("Previous data cannot be trusted, cleaning cache.");
		zeAssert(true, "Identifier Chunk Id does not match! Possible Corruption");
		Clear();
		return false;
	}

	ZESize ZEQuadWordSize = sizeof(ZEUInt64);
	ZESize ZEIdentifierChunkSize = sizeof(ZECacheIdentifierChunk);
	// This is the size of Completeness flag + Record start position data size
	// They are both always stored at the end of the cache file
	ZESize ZECacheExtraDataSize = 2 * ZEQuadWordSize;

	// Get end of file
	if (!File.Seek((ZEInt64)0, ZE_SF_END))
	{
		zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}
	
	ZEUInt64 EndOfFile = File.Tell();

	// Get records start and end position
	if (!File.Seek(-1 * (ZEInt64)ZECacheExtraDataSize, ZE_SF_END))
	{
		zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
		false;
	}
	
	ZEUInt64 RecordsEndPosition = File.Tell();
	ZEUInt64 RecordsStartPosition = 0;

	if (File.Read(&RecordsStartPosition, ZEQuadWordSize, 1) != 1)
	{
		zeAssert(true, "Can not read. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}
	
	// If there is no record
	if((RecordsStartPosition + ZECacheExtraDataSize) == EndOfFile)
		return false;

	ZEUInt64 NextHeaderPosition = RecordsStartPosition;

	// Search for the identifier between CurrentPosition && EndOfFile
	while (NextHeaderPosition < RecordsEndPosition)
	{
		if (!File.Seek((ZEInt64)NextHeaderPosition, ZE_SF_BEGINING))
		{
			zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
			return false;
		}
		
		// Get the header
		ZECacheIdentifierChunk Header = {0};
		if (File.Read(&Header, ZEIdentifierChunkSize, 1) != 1)
		{
			zeAssert(true, "Can not read. File: \"%s\".", File.GetFilePath().GetValue());
			return false;
		}
		
		if (Header.ChunkId != ZE_CACHE_IDENT_CHUNKID)
		{
			zeLog("Identifier Chunk Id does not match! Possible Corruption");
			zeLog("Cleaning cache: \"%s\".", File.GetFilePath().ToCString());
			zeAssert(true, "Identifier Chunk Id does not match! Possible Corruption");
			Clear();
			return false;
		}

		if(Identifier->Equal(&File))
			return true;

		NextHeaderPosition += (ZEIdentifierChunkSize + Header.IdentifierSize);
	}

	return false;
}

// Create new chunk from Buffer
bool ZEFileCache::AddData(const ZECacheDataIdentifier* Identifier, const void* Data, ZEUInt64 Size)
{
	// If file is not open return
	if(!IsOpen())
	{
		zeLog("Cachce is not open, cannot add data.");
		return false;
	}

	if(!CheckCompleteness())
	{
		zeLog("Previous data cannot be trusted, cleaning cache.");
		zeAssert(true, "Identifier Chunk Id does not match! Possible Corruption");
		Clear();
		return false;
	}

	ZEUInt64 TempCount = 0;
	bool Result = false;

	ZESize ZEQuadWordSize = sizeof(ZEUInt64);
	ZESize ZEDataChunkSize = sizeof(ZECacheDataChunk);
	ZESize ZEIdentifierChunkSize = sizeof(ZECacheIdentifierChunk);
	// This is the size of Completeness flag + Record start position data size
	// They are both always stored at the end of the cache file
	ZESize ZECacheExtraDataSize = 2 * ZEQuadWordSize;

	// Go to the end of records
	if (!File.Seek(-1 * (ZEInt64)ZECacheExtraDataSize, ZE_SF_END))
	{
		zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}
	
	// Get the old record's end position
	ZEUInt64 OldRecordsEndPosition = File.Tell();

	// Read the old record's start position
	ZEUInt64 OldRecordsStartPosition = 0;
	if (File.Read(&OldRecordsStartPosition, ZEQuadWordSize, 1) != 1)
	{
		zeAssert(true, "Can not read. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}

	// Calculate the total record size
	ZEUInt64 OldRecordsSize = OldRecordsEndPosition - OldRecordsStartPosition;
	// Calculate the new records start position including
	ZEUInt64 NewRecordStartPosition = OldRecordsStartPosition + Size + ZEDataChunkSize;

	// Copy previous records to new position
	if (OldRecordsSize != 0)
		CopyData(&File, OldRecordsStartPosition, OldRecordsSize, NewRecordStartPosition);

	// Set the new chunk position
	ZEUInt64 NewChunkPosition = OldRecordsStartPosition;

	// Goto new chunk position
	if (!File.Seek((ZEInt64)NewChunkPosition, ZE_SF_BEGINING))
	{
		zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}
	
	// Write data header
	ZECacheDataChunk DataChunk = {ZE_CACHE_DATA_CHUNKID};
	if (File.Write(&DataChunk, ZEDataChunkSize, 1) != 1)
	{
		zeAssert(true, "Can not write. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}
	
	// Write data
	if (File.Write(Data, Size, 1) != 1)
	{
		zeAssert(true, "Can not write. File: \"%s\".", File.GetFilePath().ToCString());
		return false;
	}
	
	// Goto the new records end position
	if (!File.Seek((ZEInt64)(NewRecordStartPosition + OldRecordsSize), ZE_SF_BEGINING))
	{
		zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}
	
	ZECacheIdentifierChunk IdentifierChunk;
	// Write the dummy header and the identifier parameter
	if (File.Write(&IdentifierChunk, ZEIdentifierChunkSize, 1) != 1)
	{
		zeAssert(true, "Can not write. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}
	
	ZEUInt64 IdentifierSize = Identifier->Write(&File);

	// Goto the header start position
	if (!File.Seek((ZEInt64)(NewRecordStartPosition + OldRecordsSize), ZE_SF_BEGINING))
	{
		zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}
	
	IdentifierChunk.ChunkId = ZE_CACHE_IDENT_CHUNKID;
	IdentifierChunk.ChunkPosition = NewChunkPosition;
	IdentifierChunk.ChunkHash = Identifier->GetHash();
	IdentifierChunk.ChunkSize = Size;
	IdentifierChunk.IdentifierSize = IdentifierSize;
	
	// Write the real header
	if (File.Write(&IdentifierChunk, ZEIdentifierChunkSize, 1) != 1)
	{
		zeAssert(true, "Can not write. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}
	

	// Go to the end of the records
	if (!File.Seek((ZEInt64)0, ZE_SF_END))
	{
		zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}
	
	// Write the records start position
	if (File.Write(&NewRecordStartPosition, ZEQuadWordSize, 1) != 1)
	{
		zeAssert(true, "Can not write. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}
	
	// Write completeness flag
	ZEUInt64 Complete = ZE_CACHE_COMPLETENESS;
	if (File.Write(&Complete, ZEQuadWordSize, 1) != 1)
	{
		zeAssert(true, "Can not write. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}
	
	File.Flush();
	return true;
}

// Create new chunk from PartialFile
bool ZEFileCache::Allocate(ZEPartialFile* PartialFile, const ZECacheDataIdentifier* Identifier, ZEUInt64 ChunkSize)
{
	// If file is not open return
	if(!IsOpen())
	{
		zeLog("Cachce is not open, cannot allocate.");
		return false;
	}

	if(!CheckCompleteness())
	{
		zeLog("Previous data cannot be trusted, cleaning cache.");
		zeAssert(true, "Identifier Chunk Id does not match! Possible Corruption");
		Clear();
		return false;
	}

	ZESize ZEQuadWordSize = sizeof(ZEUInt64);
	ZESize ZEDataChunkSize = sizeof(ZECacheDataChunk);
	ZESize ZEIdentifierChunkSize = sizeof(ZECacheIdentifierChunk);
	// This is the size of Completeness flag + Record start position data size
	// They are both always stored at the end of the cache file
	ZESize ZECacheExtraDataSize = 2 * ZEQuadWordSize;

	// Go to the end of records
	if (!File.Seek(-1 * (ZEInt64)ZECacheExtraDataSize, ZE_SF_END))
	{
		zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}
		
	// Get the old record's end position
	ZEUInt64 OldRecordsEndPosition = File.Tell();

	// Read the old record's start position
	ZEUInt64 OldRecordsStartPosition = 0;
	if (File.Read(&OldRecordsStartPosition, ZEQuadWordSize, 1) != 1)
	{
		zeAssert(true, "Can not read. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}
		
	// Calculate the total record size
	ZEUInt64 OldRecordsSize = OldRecordsEndPosition - OldRecordsStartPosition;
	// Calculate the new records start position
	ZEUInt64 NewRecordStartPosition = OldRecordsStartPosition + ChunkSize + ZEDataChunkSize;

	// If there exists old records, copy them to their new position
	if (OldRecordsSize != 0)
		CopyData(&File, OldRecordsStartPosition, OldRecordsSize, NewRecordStartPosition);

	// Set the new chunk position
	ZEUInt64 NewChunkPosition = OldRecordsStartPosition;

	// Go to chunk start pos and write data header
	if (!File.Seek((ZEInt64)NewChunkPosition, ZE_SF_BEGINING))
	{
		zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}

	ZECacheDataChunk DataChunk = {ZE_CACHE_DATA_CHUNKID};
	if (File.Write(&DataChunk, ZEDataChunkSize, 1) != 1)
	{
		zeAssert(true, "Can not write. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}

	// Set the partial file
	PartialFile->Open(&File, NewChunkPosition + ZEDataChunkSize, ChunkSize);

	// Goto the new records end position
	if (!File.Seek((ZEInt64)(NewRecordStartPosition + OldRecordsSize), ZE_SF_BEGINING))
	{
		zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}

	ZECacheIdentifierChunk IdentifierChunk;
	// Write the dummy header and the identifier parameter
	if (File.Write(&IdentifierChunk, ZEIdentifierChunkSize, 1) != 1)
	{
		zeAssert(true, "Can not write. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}

	ZEUInt64 IdentifierSize = Identifier->Write(&File);
	if (IdentifierSize == 0)
	{
		zeAssert(true, "Can not write identifier. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}

	// Goto the header start position
	if (!File.Seek((ZEInt64)(NewRecordStartPosition + OldRecordsSize), ZE_SF_BEGINING))
	{
		zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}

	IdentifierChunk.ChunkId = ZE_CACHE_IDENT_CHUNKID;
	IdentifierChunk.ChunkPosition = NewChunkPosition;
	IdentifierChunk.ChunkHash = Identifier->GetHash();
	IdentifierChunk.ChunkSize = ChunkSize;
	IdentifierChunk.IdentifierSize = IdentifierSize;
	
	// Write the real header
	if (File.Write(&IdentifierChunk, ZEIdentifierChunkSize, 1) != 1)
	{
		zeAssert(true, "Can not write. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}

	// Go to the end of the records
	if (!File.Seek((ZEInt64)0, ZE_SF_END))
	{
		zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}

	// Write the records start position
	if (File.Write(&NewRecordStartPosition, ZEQuadWordSize, 1) != 1)
	{
		zeAssert(true, "Can not write. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}
	
	// Write the completeness flag
	ZEUInt64 Complete = ZE_CACHE_COMPLETENESS;
	if (File.Write(&Complete, ZEQuadWordSize, 1) != 1)
	{
		zeAssert(true, "Can not write. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}

	File.Flush();
	return true;
}

// Returns the data in a buffer
bool ZEFileCache::GetData(const ZECacheDataIdentifier* Identifier, void* Buffer, ZEUInt64 Offset, ZEUInt64 Size)
{
	// If file is not open return
	if(!IsOpen())
	{
		zeLog("Cachce is not open, cannot get data.");
		return false;
	}

	if(!CheckCompleteness())
	{
		zeLog("Previous data cannot be trusted, cleaning cache.");
		zeAssert(true, "Identifier Chunk Id does not match! Possible Corruption");
		Clear();
		return false;
	}

	ZESize ZEQuadWordSize = sizeof(ZEUInt64);
	ZESize ZEDataChunkSize = sizeof(ZECacheDataChunk);
	ZESize ZEIdentifierChunkSize = sizeof(ZECacheIdentifierChunk);
	// This is the size of Completeness flag + Record start position data size
	// They are both always stored at the end of the cache file
	ZESize ZECacheExtraDataSize = 2 * ZEQuadWordSize;

	// Get hash from identifier
	ZEUInt64 Hash = Identifier->GetHash();
	// Go to end of records
	if (!File.Seek(-1 * (ZEInt64)ZECacheExtraDataSize, ZE_SF_END))
	{
		zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}

	// Get the records end position
	ZEUInt64 RecordsEndPosition = File.Tell();

	// Read records start position from the end of the file
	ZEUInt64 RecordStartPosition = 0;
	if (File.Read(&RecordStartPosition, ZEQuadWordSize, 1) != 1)
	{
		zeAssert(true, "Can not read. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}

	// Set the beginning address for linear search
	ZEUInt64 CurrentHeaderPosition = RecordStartPosition;
	while (CurrentHeaderPosition < RecordsEndPosition)
	{
		// Go to the next header position
		if (!File.Seek((ZEInt64)CurrentHeaderPosition, ZE_SF_BEGINING))
		{
			zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
			return false;
		}

		// Read header
		ZECacheIdentifierChunk IdentifierChunk;
		if (File.Read(&IdentifierChunk, ZEIdentifierChunkSize, 1) != 1)
		{
			zeAssert(true, "Can not read. File: \"%s\".", File.GetFilePath().GetValue());
			return false;
		}

		// Check if the header readable
		if (IdentifierChunk.ChunkId != ZE_CACHE_IDENT_CHUNKID)
		{
			zeLog("Identifier Chunk Id does not match! Possible Corruption. File Name: \"%s\".", (File.GetFilePath()).ToCString());
			return false;
		}

		// Check if hash, size and identifier matches 
		if (IdentifierChunk.ChunkHash == Hash && IdentifierChunk.ChunkSize == Size && Identifier->Equal(&File))
		{
			// Goto the chunk data position
			if (!File.Seek((ZEInt64)IdentifierChunk.ChunkPosition, ZE_SF_BEGINING))
			{
				zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
				return true;
			}

			// Check if data readable
			ZECacheDataChunk DataChunk;
			if (File.Read(&DataChunk, ZEDataChunkSize, 1) != 1)
			{
				zeAssert(true, "Can not read. File: \"%s\".", File.GetFilePath().GetValue());
				return false;
			}

			if(DataChunk.ChunkId != ZE_CACHE_DATA_CHUNKID)
			{
				zeLog("Data Chunk Id does not match! Possible Corruption. File Name: \"%s\".", File.GetFilePath().ToCString());
				return false;
			}

			// Read into buffer
			if (File.Read(Buffer, Size, 1) == 1)
			{
				return true;
			}
		}

		CurrentHeaderPosition += IdentifierChunk.IdentifierSize + ZEIdentifierChunkSize;
	}

	return false;
}

// Returns the data as PartialFile
bool ZEFileCache::OpenData(ZEPartialFile* PartialFile, const ZECacheDataIdentifier* Identifier)
{
	// If file is not open return
	if(!IsOpen())
	{
		zeLog("Cachce is not open, cannot open data.");
		return false;
	}

	if(!CheckCompleteness())
	{
		zeLog("Previous data cannot be trusted, cleaning cache.");
		zeAssert(true, "Identifier Chunk Id does not match! Possible Corruption");
		Clear();
		return false;
	}

	ZESize ZEQuadWordSize = sizeof(ZEUInt64);
	ZESize ZEDataChunkSize = sizeof(ZECacheDataChunk);
	ZESize ZEIdentifierChunkSize = sizeof(ZECacheIdentifierChunk);
	// This is the size of Completeness flag + Record start position data size
	// They are both always stored at the end of the cache file
	ZESize ZECacheExtraDataSize = 2 * ZEQuadWordSize;

	// Get records start and end position
	if (!File.Seek(-1 * (ZEInt64)ZECacheExtraDataSize, ZE_SF_END))
	{
		zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
		return true;
	}
	ZEUInt64 RecordEndPosition = File.Tell();

	ZEUInt64 RecordsStartPosition = 0;
	if(File.Read(&RecordsStartPosition, ZEQuadWordSize, 1) != 1)
	{
		zeAssert(true, "Can not read. File: \"%s\".", File.GetFilePath().GetValue());
		return false;
	}

	// Set the next header start position
	ZEUInt64 NextHeaderPosition = RecordsStartPosition;

	// Search for the identifier between RecordsStartPosition & EndOfFile
	while(NextHeaderPosition < RecordEndPosition)
	{
		// Goto next header position
		if (!File.Seek((ZEInt64)NextHeaderPosition, ZE_SF_BEGINING))
		{
			zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
			return false;
		}

		// Read the header
		ZECacheIdentifierChunk IdentifierChunk;
		if (File.Read(&IdentifierChunk, ZEIdentifierChunkSize, 1) != 1)
		{
			zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
			return false;
		}

		// If header is not corrupted
		if (IdentifierChunk.ChunkId != ZE_CACHE_IDENT_CHUNKID)
		{
			zeLog("Identifier Chunk Id does not match! Possible Corruption. File Name: \"%s\".", File.GetFilePath().ToCString());
			return false;
		}

		// If identifier matches
		if(Identifier->Equal(&File))
		{
			// Read data chunk id
			if (!File.Seek(IdentifierChunk.ChunkPosition, ZE_SF_BEGINING))
			{
				zeAssert(true, "Can not seek. File: \"%s\".", File.GetFilePath().GetValue());
				return false;
			}

			ZECacheDataChunk	DataChunk;
			if (File.Read(&DataChunk, ZEDataChunkSize, 1) != 1)
			{
				zeAssert(true, "Can not read. File: \"%s\".", File.GetFilePath().GetValue());
				return true;
			}

			if(DataChunk.ChunkId != ZE_CACHE_DATA_CHUNKID)
			{
				zeLog("Data Chunk Id does not match! Possible Corruption. File Name: \"%s\".", File.GetFilePath().ToCString());
				return false;
			}

			// Open the PartialFile
			PartialFile->Open(&File, IdentifierChunk.ChunkPosition + ZEDataChunkSize, IdentifierChunk.ChunkSize);
			return true;
		}

		NextHeaderPosition += ZEIdentifierChunkSize + IdentifierChunk.IdentifierSize;
	}

	return false;
}
