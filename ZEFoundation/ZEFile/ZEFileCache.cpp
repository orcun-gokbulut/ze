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
#include "ZECore/ZEError.h"
#include "ZECore/ZEConsole.h"


// OK
bool ZEFileCache::CheckCompleteness()
{
	size_t ZEQuadWordSize = sizeof(ZEQWORD);

	ZEQWORD Cursor = File.Tell();
	// Go to end of the file and read cache completeness flag
	ZEQWORD Complete = 0;

	File.Seek(-1 * (ZEINT64)ZEQuadWordSize, ZE_SF_END);
	File.Read(&Complete, ZEQuadWordSize, 1);
	File.Seek(Cursor, ZE_SF_BEGINING);

	if (Complete == ZE_CACHE_COMPLETENESS)
		return true;
	
	zeLog("File Cache", "Cannot verify completeness of cache file: \"%s\".", File.GetFileName().GetValue());
	return false;
}

// OK
void ZEFileCache::PrepareCacheForFirstUse()
{
	// Write file header
	ZECacheFileHeader Header;
	Header.ChunkId = ZE_CACHE_HEADER;
	Header.Version = ZE_CACHE_VERSION;

	if (File.Write(&Header, sizeof(ZECacheFileHeader), 1) != 1)
		zeError("File Cache", "Can not write file header to file: \"%s\".", File.GetFileName().GetValue());

	// Write records start position
	// Since there is no records just write cursor position
	ZEQWORD Cursor = File.Tell();
	if (File.Write(&Cursor, sizeof(ZEQWORD), 1) != 1)
		zeError("File Cache", "Can not write records start position to file: \"%s\".", File.GetFileName().GetValue());

	// Write the completeness flag
	ZEQWORD Complete = ZE_CACHE_COMPLETENESS;
	if (File.Write(&Complete, sizeof(ZEQWORD), 1) != 1)
		zeError("File Cache", "Can not write completeness flag to file: \"%s\".", File.GetFileName().GetValue());

	File.Flush();
}

// OK
// Helper function for copying Records
void ZEFileCache::CopyData(ZEFile* File, ZEQWORD From, ZEQWORD Size, ZEQWORD To)
{
	const size_t BufferSize = 65536;
	char Buffer[BufferSize];

	size_t Count = (size_t)Size / BufferSize;
	size_t LeftOver = (size_t)Size % BufferSize;

	for (size_t I = 0; I < Count; I++)
	{
		File->Seek((ZEINT64)(From + I * BufferSize), ZE_SF_BEGINING);
		File->Read(Buffer, BufferSize, 1);
		File->Seek((ZEINT64)(To + I * BufferSize), ZE_SF_BEGINING);
		File->Write(Buffer, BufferSize, 1);
	}

	if (LeftOver != 0)
	{
		File->Seek((ZEINT64)(From + Count * BufferSize), ZE_SF_BEGINING);
		File->Read(Buffer, LeftOver, 1);
		File->Seek((ZEINT64)(To + Count * BufferSize), ZE_SF_BEGINING);
		File->Write(Buffer, LeftOver, 1);
	}
}

// OK
bool ZEFileCache::IsFileCache(ZEString FileName)
{
	ZEFile Check;

	if(!Check.Open(FileName, ZE_FM_READ_WRITE, true))
	{
		Check.Close();
		zeError("File Cache", "Cannot open file to check the format");
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
		zeError("File Cache", "Cannot read header from file");
		Check.Close();
		return false;
	}

	if(Header.ChunkId == ZE_CACHE_HEADER)
	{
		Check.Close();
		return true;
	}

	return false;
}

// OK
ZEFileCache::ZEFileCache()
{
	 // Empty
}

// OK
ZEFileCache::~ZEFileCache()
{
	CloseCache();
}

// OK
// Opens the cache file
bool ZEFileCache::OpenCache(ZEString FileName)
{
	if (IsOpen())
		return false;

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
		ZECacheFileHeader Header;
		File.Read(&Header, sizeof(ZECacheFileHeader), 1);

		// If format is correct
		if(Header.ChunkId == ZE_CACHE_HEADER)
		{
			// If cache is not complete
			if(!CheckCompleteness())
			{
				zeLog("File Cache", "Previous data cannot be trusted, cleaning cache.");
				ClearCache();
			}
				

			return true;
		}
	}

	return false;
}

// OK
// Closes the cache file
void ZEFileCache::CloseCache()
{
	if (IsOpen())
		File.Close();
}

// OK
// Empty the cache file
bool ZEFileCache::ClearCache()
{
	if (!IsOpen())
		return false;


	ZEString FileName;

	FileName = File.GetFileName();
	CloseCache();

	// Remove the file
	if(remove(FileName.GetValue()) == -1)
		return false;

	// ReOpen the cache	
	if(OpenCache(FileName))
		return true;

	return false;
}

// OK
// Returns the cache file name
const ZEString ZEFileCache::GetCacheFileName()
{
	return File.GetFileName();
}

bool ZEFileCache::IsOpen()
{
	if(File.IsOpen())
		return true;

	return false;
}

// OK
// Checks if the chunk exists
bool ZEFileCache::ChunkExists(const ZECacheDataIdentifier* Identifier)
{
	if(!IsOpen())
		return false;
	if(!CheckCompleteness())
	{
		zeLog("File Cache", "Previous data cannot be trusted, cleaning cache.");
		ClearCache();
		return false;
	}

	size_t ZEQuadWordSize = sizeof(ZEQWORD);
	size_t ZEIdentifierChunkSize = sizeof(ZECacheIdentifierChunk);
	// This is the size of Completeness flag + Record start position data size
	// They are both always stored at the end of the cache file
	size_t ZECacheExtraDataSize = 2 * ZEQuadWordSize;

	// Get end of file
	File.Seek((ZEINT64)0, ZE_SF_END);
	ZEQWORD EndOfFile = File.Tell();

	// Get records start and end position
	File.Seek(-1 * (ZEINT64)ZECacheExtraDataSize, ZE_SF_END);
	ZEQWORD RecordsEndPosition = File.Tell();
	ZEQWORD RecordsStartPosition = 0;
	File.Read(&RecordsStartPosition, ZEQuadWordSize, 1);

	// If there is no record
	if((RecordsStartPosition + ZECacheExtraDataSize) == EndOfFile)
		return false;

	ZEQWORD NextHeaderPosition = RecordsStartPosition;

	// Search for the identifier between CurrentPosition && EndOfFile
	while(NextHeaderPosition < RecordsEndPosition)
	{
		File.Seek((ZEINT64)NextHeaderPosition, ZE_SF_BEGINING);

		// Get the header
		ZECacheIdentifierChunk Header;
		if (File.Read(&Header, ZEIdentifierChunkSize, 1) != 1)
			return false;

		if (Header.ChunkId != ZE_CACHE_IDENT_CHUNKID)
		{
			zeLog("File Cache", "Identifier Chunk Id does not match! Possible Corruption");
			zeLog("File Cache", "Cleaning cache: \"%s\".", File.GetFileName().GetValue());
			ClearCache();
		}

		if(Identifier->Equal(&File))
			return true;

		NextHeaderPosition += ZEIdentifierChunkSize + Header.IdentifierSize;
	}

	return false;
}

// OK
// Create new chunk from Buffer
bool ZEFileCache::AddChunk(const ZECacheDataIdentifier* Identifier, const void* Data, ZEQWORD Size)
{
	// If file is not open return
	if(!IsOpen())
		return false;
	if(!CheckCompleteness())
	{
		zeLog("File Cache", "Previous data cannot be trusted, cleaning cache.");
		ClearCache();
		return false;
	}

	size_t ZEQuadWordSize = sizeof(ZEQWORD);
	size_t ZEDataChunkSize = sizeof(ZECacheDataChunk);
	size_t ZEIdentifierChunkSize = sizeof(ZECacheIdentifierChunk);
	// This is the size of Completeness flag + Record start position data size
	// They are both always stored at the end of the cache file
	size_t ZECacheExtraDataSize = 2 * ZEQuadWordSize;

	// Go to the end of records
	File.Seek(-1 * (ZEINT64)ZECacheExtraDataSize, ZE_SF_END);
	// Get the old record's end position
	ZEQWORD OldRecordsEndPosition = File.Tell();

	// Read the old record's start position
	ZEQWORD OldRecordsStartPosition = 0;
	File.Read(&OldRecordsStartPosition, ZEQuadWordSize, 1);

	// Calculate the total record size
	ZEQWORD OldRecordsSize = OldRecordsEndPosition - OldRecordsStartPosition;
	// Calculate the new records start position including
	ZEQWORD NewRecordStartPosition = OldRecordsStartPosition + Size + ZEDataChunkSize;

	// Copy previous records to new position
	if (OldRecordsSize != 0)
		CopyData(&File, OldRecordsStartPosition, OldRecordsSize, NewRecordStartPosition);

	// Set the new chunk position
	ZEQWORD NewChunkPosition = OldRecordsStartPosition;

	

	// Goto new chunk position
	File.Seek((ZEINT64)NewChunkPosition, ZE_SF_BEGINING);
	// Write data header
	ZECacheDataChunk DataChunk = {ZE_CACHE_DATA_CHUNKID};
	File.Write(&DataChunk, ZEDataChunkSize, 1);
	// Write data
	File.Write(Data, Size, 1);

	// Goto the new records end position
	File.Seek((ZEINT64)(NewRecordStartPosition + OldRecordsSize), ZE_SF_BEGINING);

	ZECacheIdentifierChunk IdentifierChunk;
	// Write the dummy header and the identifier parameter
	File.Write(&IdentifierChunk, ZEIdentifierChunkSize, 1);
	ZEQWORD IdentifierSize = Identifier->Write(&File);

	// Goto the header start position
	File.Seek((ZEINT64)(NewRecordStartPosition + OldRecordsSize), ZE_SF_BEGINING);
	IdentifierChunk.ChunkId = ZE_CACHE_IDENT_CHUNKID;
	IdentifierChunk.ChunkPosition = NewChunkPosition;
	IdentifierChunk.ChunkHash = Identifier->GetHash();
	IdentifierChunk.ChunkSize = Size;
	IdentifierChunk.IdentifierSize = IdentifierSize;
	// Write the real header
	File.Write(&IdentifierChunk, ZEIdentifierChunkSize, 1);

	// Go to the end of the records
	File.Seek((ZEINT64)0, ZE_SF_END);
	// Write the records start position
	File.Write(&NewRecordStartPosition, ZEQuadWordSize, 1);
	// Write completeness flag
	ZEQWORD Complete = ZE_CACHE_COMPLETENESS;
	File.Write(&Complete, ZEQuadWordSize, 1);

	File.Flush();
	return true;
}

// OK
// Create new chunk from PartialFile
bool ZEFileCache::CreateChunk(ZEPartialFile& PartialFile, const ZECacheDataIdentifier* Identifier, ZEQWORD ChunkSize)
{
	// If file is not open return
	if(!IsOpen())
		return false;
	if(!CheckCompleteness())
	{
		zeLog("File Cache", "Previous data cannot be trusted, cleaning cache.");
		ClearCache();
		return false;
	}

	size_t ZEQuadWordSize = sizeof(ZEQWORD);
	size_t ZEDataChunkSize = sizeof(ZECacheDataChunk);
	size_t ZEIdentifierChunkSize = sizeof(ZECacheIdentifierChunk);
	// This is the size of Completeness flag + Record start position data size
	// They are both always stored at the end of the cache file
	size_t ZECacheExtraDataSize = 2 * ZEQuadWordSize;

	// Go to the end of records
	File.Seek(-1 * (ZEINT64)ZECacheExtraDataSize, ZE_SF_END);
	// Get the old record's end position
	ZEQWORD OldRecordsEndPosition = File.Tell();

	// Read the old record's start position
	ZEQWORD OldRecordsStartPosition = 0;
	File.Read(&OldRecordsStartPosition, ZEQuadWordSize, 1);

	// Calculate the total record size
	ZEQWORD OldRecordsSize = OldRecordsEndPosition - OldRecordsStartPosition;
	// Calculate the new records start position
	ZEQWORD NewRecordStartPosition = OldRecordsStartPosition + ChunkSize + ZEDataChunkSize;

	// If there exists old records, copy them to their new position
	if (OldRecordsSize != 0)
		CopyData(&File, OldRecordsStartPosition, OldRecordsSize, NewRecordStartPosition);

	// Set the new chunk position
	ZEQWORD NewChunkPosition = OldRecordsStartPosition;

	// Go to chunk start pos and write data header
	File.Seek((ZEINT64)NewChunkPosition, ZE_SF_BEGINING);
	ZECacheDataChunk DataChunk = {ZE_CACHE_DATA_CHUNKID};
	File.Write(&DataChunk, ZEDataChunkSize, 1);

	// Set the partial file
	PartialFile.Open(&File, NewChunkPosition + ZEDataChunkSize, ChunkSize);

	// Goto the new records end position
	File.Seek((ZEINT64)(NewRecordStartPosition + OldRecordsSize), ZE_SF_BEGINING);

	ZECacheIdentifierChunk IdentifierChunk;
	// Write the dummy header and the identifier parameter
	File.Write(&IdentifierChunk, ZEIdentifierChunkSize, 1);
	ZEQWORD IdentifierSize = Identifier->Write(&File);

	// Goto the header start position
	File.Seek((ZEINT64)(NewRecordStartPosition + OldRecordsSize), ZE_SF_BEGINING);
	IdentifierChunk.ChunkId = ZE_CACHE_IDENT_CHUNKID;
	IdentifierChunk.ChunkPosition = NewChunkPosition;
	IdentifierChunk.ChunkHash = Identifier->GetHash();
	IdentifierChunk.ChunkSize = ChunkSize;
	IdentifierChunk.IdentifierSize = IdentifierSize;
	// Write the real header
	File.Write(&IdentifierChunk, ZEIdentifierChunkSize, 1);

	// Go to the end of the records
	File.Seek((ZEINT64)0, ZE_SF_END);
	// Write the records start position
	File.Write(&NewRecordStartPosition, ZEQuadWordSize, 1);
	// Write the completeness flag
	ZEQWORD Complete = ZE_CACHE_COMPLETENESS;
	File.Write(&Complete, ZEQuadWordSize, 1);

	File.Flush();
	return true;
}

// OK
// Returns the data in a buffer
bool ZEFileCache::GetChunk(const ZECacheDataIdentifier* Identifier, void* Buffer, ZEQWORD Offset, ZEQWORD Size)
{
	// If file is not open return
	if(!IsOpen())
		return false;
	if(!CheckCompleteness())
	{
		zeLog("File Cache", "Previous data cannot be trusted, cleaning cache.");
		ClearCache();
		return false;
	}

	size_t ZEQuadWordSize = sizeof(ZEQWORD);
	size_t ZEDataChunkSize = sizeof(ZECacheDataChunk);
	size_t ZEIdentifierChunkSize = sizeof(ZECacheIdentifierChunk);
	// This is the size of Completeness flag + Record start position data size
	// They are both always stored at the end of the cache file
	size_t ZECacheExtraDataSize = 2 * ZEQuadWordSize;

	// Get hash from identifier
	ZEQWORD Hash = Identifier->GetHash();
	// Go to end of records
	File.Seek(-1 * (ZEINT64)ZECacheExtraDataSize, ZE_SF_END);

	// Get the records end position
	ZEQWORD RecordsEndPosition = File.Tell();

	// Read records start position from the end of the file
	ZEQWORD RecordStartPosition = 0;
	File.Read(&RecordStartPosition, ZEQuadWordSize, 1);

	// Set the beginning address for linear search
	ZEQWORD CurrentHeaderPosition = RecordStartPosition;
	while (CurrentHeaderPosition < RecordsEndPosition)
	{
		// Go to the next header position
		File.Seek((ZEINT64)CurrentHeaderPosition, ZE_SF_BEGINING);

		// Read header
		ZECacheIdentifierChunk IdentifierChunk;
		if (File.Read(&IdentifierChunk, ZEIdentifierChunkSize, 1) != 1)
			return false;

		// Check if the header readable
		if (IdentifierChunk.ChunkId != ZE_CACHE_IDENT_CHUNKID)
		{
			zeLog("File Cache", "Identifier Chunk Id does not match! Possible Corruption. File Name: \"%s\".", (File.GetFileName()).GetValue());
			return false;
		}

		// Check if hash, size and identifier matches 
		if (IdentifierChunk.ChunkHash == Hash && IdentifierChunk.ChunkSize == Size && Identifier->Equal(&File))
		{
			// Goto the chunk data position
			File.Seek((ZEINT64)IdentifierChunk.ChunkPosition, ZE_SF_BEGINING);

			// Check if data readable
			ZECacheDataChunk DataChunk;
			File.Read(&DataChunk, ZEDataChunkSize, 1);
			if(DataChunk.ChunkId != ZE_CACHE_DATA_CHUNKID)
			{
				zeLog("File Cache", "Data Chunk Id does not match! Possible Corruption. File Name: \"%s\".", File.GetFileName().GetValue());
				return false;
			}

			// Read into buffer
			if (File.Read(Buffer, Size, 1) == 1)
				return true;
		}

		CurrentHeaderPosition += IdentifierChunk.IdentifierSize + ZEIdentifierChunkSize;
	}

	return false;
}

// OK
// Returns the data as PartialFile
bool ZEFileCache::OpenChunk(ZEPartialFile& PartialFile, const ZECacheDataIdentifier* Identifier)
{
	// If file is not open return
	if(!IsOpen())
		return false;
	if(!CheckCompleteness())
	{
		zeLog("File Cache", "Previous data cannot be trusted, cleaning cache.");
		ClearCache();
		return false;
	}

	size_t ZEQuadWordSize = sizeof(ZEQWORD);
	size_t ZEDataChunkSize = sizeof(ZECacheDataChunk);
	size_t ZEIdentifierChunkSize = sizeof(ZECacheIdentifierChunk);
	// This is the size of Completeness flag + Record start position data size
	// They are both always stored at the end of the cache file
	size_t ZECacheExtraDataSize = 2 * ZEQuadWordSize;

	// Get records start and end position
	File.Seek(-1 * (ZEINT64)ZECacheExtraDataSize, ZE_SF_END);
	ZEQWORD RecordEndPosition = File.Tell();

	ZEQWORD RecordsStartPosition = 0;
	if(File.Read(&RecordsStartPosition, ZEQuadWordSize, 1) != 1)
		return false;

	// Set the next header start position
	ZEQWORD NextHeaderPosition = RecordsStartPosition;

	// Search for the identifier between RecordsStartPosition & EndOfFile
	while(NextHeaderPosition < RecordEndPosition)
	{
		// Goto next header position
		File.Seek((ZEINT64)NextHeaderPosition, ZE_SF_BEGINING);

		// Read the header
		ZECacheIdentifierChunk IdentifierChunk;
		if (File.Read(&IdentifierChunk, ZEIdentifierChunkSize, 1) != 1)
			return false;

		// If header is not corrupted
		if (IdentifierChunk.ChunkId != ZE_CACHE_IDENT_CHUNKID)
		{
			zeLog("File Cache", "Identifier Chunk Id does not match! Possible Corruption. File Name: \"%s\".", File.GetFileName().GetValue());
			return false;
		}

		// If identifier matches
		if(Identifier->Equal(&File))
		{
			// Read data chunk id
			File.Seek(IdentifierChunk.ChunkPosition, ZE_SF_BEGINING);
			ZECacheDataChunk	DataChunk;
			File.Read(&DataChunk, ZEDataChunkSize, 1);

			if(DataChunk.ChunkId != ZE_CACHE_DATA_CHUNKID)
			{
				zeLog("File Cache", "Data Chunk Id does not match! Possible Corruption. File Name: \"%s\".", File.GetFileName().GetValue());
				return false;
			}

			// Open the PartialFile
			PartialFile.Open(&File, IdentifierChunk.ChunkPosition + ZEDataChunkSize, IdentifierChunk.ChunkSize);
			return true;
		}

		NextHeaderPosition += ZEIdentifierChunkSize + IdentifierChunk.IdentifierSize;
	}

	return false;
}
