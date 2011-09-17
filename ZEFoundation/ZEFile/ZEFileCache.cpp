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

#include "ZETypes.h"
#include "ZEFileCache.h"
#include "ZEFile/ZEResourceFile.h"

//#include "../ZEAssert.h"
//#include "..\Source/ZECore/ZEError.h"
//#include "..\Source/ZEDefinitions.h"

#include <string.h>

struct ZEChunkHeader
{
	ZEDWORD				Header;
	ZEDWORD				ChunkHash;
	ZEDWORD				ChunkPosition;
	ZEDWORD				ChunkSize;
	ZEDWORD				IdentifierSize;
};

// Helper function for copying Records
void CopyData(ZEFile* File, size_t From, size_t Size, size_t To)
{
	const size_t BufferSize = 65536;
	char Buffer[BufferSize];

	int Count = Size / BufferSize;
	int LeftOver = Size % BufferSize;
	for (size_t I = 0; I < Count; I++)
	{
		File->Seek(From + I * BufferSize, ZE_SF_BEGINING);
		File->Read(Buffer, BufferSize, 1);
		File->Seek(To + I * BufferSize, ZE_SF_BEGINING);
		File->Write(Buffer, BufferSize, 1);

		/*fseek(File, From + I * BufferSize, SEEK_SET);
		fread(Buffer, BufferSize, 1, File);
		fseek(File, To + I * BufferSize, SEEK_SET);
		fwrite(Buffer, BufferSize, 1, File);*/
	}

	if (LeftOver != 0)
	{
		File->Seek(From + Count * BufferSize, ZE_SF_BEGINING);
		File->Read(Buffer, LeftOver, 1);
		File->Seek(To + Count * BufferSize, ZE_SF_BEGINING);
		File->Write(Buffer, LeftOver, 1);

		/*fseek(File, From + Count * BufferSize, SEEK_SET);
		fread(Buffer, LeftOver, 1, File);
		fseek(File, To + Count * BufferSize, SEEK_SET);
		fwrite(Buffer, LeftOver, 1, File);*/
	}
}

// Constructor
ZEFileCache::ZEFileCache() : File(NULL)
{
	File = new ZEFile();
}

// Destructor
ZEFileCache::~ZEFileCache()
{
	if(File)
	{
		if(File->IsOpen())
			File->Close();

		delete File;
		File = NULL;
	}
}

// Opens the cache file
bool ZEFileCache::OpenCache(const char* CacheFileName)
{
	//zeAssert(File && File->IsOpen(), "Cache is already open...");
	
	File->Open(CacheFileName, ZE_FM_READ_WRITE, true);
	
	if (File->IsOpen())
	{
		strncpy(this->CacheFileName, CacheFileName, 256);
		return true;
	}

	return false;
}

// Closes the cache file
void ZEFileCache::CloseCache()
{
	if (File && File->IsOpen())
		File->Close();
}

// Empty the cache file
bool ZEFileCache::ClearCache()
{
	// If cache file is open close it
	CloseCache();

	// Remove the file
	if(remove(CacheFileName) == -1)
	{
		return false;
	}

	// ReOpen the cache	
	if(OpenCache(CacheFileName))
		return true;

	return false;
}

// Returns the cache file name
const char*	ZEFileCache::GetCacheFileName()
{
	return CacheFileName;
}

// Checks if the chunk exists
bool ZEFileCache::ChunkExists(const ZECacheChunkIdentifier* Identifier)
{
	if(!File || !File->IsOpen())
		return false;

	// Get end of file
	File->Seek(0, ZE_SF_END);
	size_t EndOfFile = 0;
	EndOfFile = File->Tell();

	// Get records start position
	File->Seek(-sizeof(ZEDWORD), ZE_SF_END);
	size_t RecordsStartPosition = 0;
	File->Read(&RecordsStartPosition, sizeof(ZEDWORD), 1);

	size_t NextHeaderPosition = RecordsStartPosition;
	size_t IdentifierSize = 0;

	// Search for the identifier between CurrentPosition && EndOfFile
	while(NextHeaderPosition < EndOfFile)
	{
		File->Seek(NextHeaderPosition, ZE_SF_BEGINING);

		// Get the header
		ZEChunkHeader Header;
		if (File->Read(&Header, sizeof(ZEChunkHeader), 1) != 1)
		{
			return false;
		}

		if (Header.Header != 'ZECH')
		{
			return false;
		}

		if(Identifier->Equal(File))
		{
			return true;
		}

		NextHeaderPosition += sizeof(ZEChunkHeader) + Header.IdentifierSize;
	}

	return false;

	/////////// OLD CODE /////////////////

	/*OpenCache();

	// Get end of file
	size_t EndOfFile = 0;
	fseek(File, 0, SEEK_END);
	EndOfFile = ftell(File);

	// Get records start position
	fseek(File, -sizeof(ZEDWORD), SEEK_END);
	size_t RecordsStartPosition = 0;
	fread(&RecordsStartPosition, sizeof(ZEDWORD), 1, File);

	size_t NextHeaderPosition = RecordsStartPosition;
	size_t IdentifierSize = 0;

	// Search for the identifier between CurrentPosition && EndOfFile
	while(NextHeaderPosition < EndOfFile)
	{
	fseek(File, NextHeaderPosition, SEEK_SET);

	// Get the header
	ZEChunkHeader Header;
	if (fread(&Header, sizeof(ZEChunkHeader), 1, File) != 1)
	{
	CloseCache();
	return false;
	}

	if (Header.Header != 'ZECH')
	{
	CloseCache();
	return false;
	}

	if(Identifier->Equal((void*)File))
	{
	CloseCache();
	return true;
	}

	NextHeaderPosition += sizeof(ZEChunkHeader) + Header.IdentifierSize;
	}

	CloseCache();
	return false;*/
}

// Create new chunk from Buffer
bool ZEFileCache::AddChunk(const ZECacheChunkIdentifier* Identifier, const void* Data, size_t Size)
{
	// If file is not open return
	if(!File || !File->IsOpen())
		return false;

	// Go to the end of records
	File->Seek(-sizeof(ZEDWORD), ZE_SF_END);
	// Get the old record's end position
	size_t OldRecordsEndPosition = File->Tell();

	// Read the old record's start position
	size_t OldRecordsStartPosition = 0;
	File->Read(&OldRecordsStartPosition, sizeof(ZEDWORD), 1);

	// Calculate the total record size
	size_t OldRecordsSize = OldRecordsEndPosition - OldRecordsStartPosition;
	// Calculate the new records start position
	size_t NewRecordStartPosition = OldRecordsStartPosition + Size;

	// If there exists old records, copy them to their new position
	if (OldRecordsSize != 0)
		CopyData(File, OldRecordsStartPosition, OldRecordsSize, NewRecordStartPosition);

	// Set the new chunk position
	size_t NewChunkPosition = OldRecordsStartPosition;
	
	// Goto new chunk position and write the data
	File->Seek(NewChunkPosition, ZE_SF_BEGINING);
	File->Write(Data, Size, 1);

	// Goto the new records end position
	File->Seek(NewRecordStartPosition + OldRecordsSize, ZE_SF_BEGINING);

	ZEChunkHeader Header;
	// Write the dummy header and the identifier parameter
	File->Write(&Header, sizeof(ZEChunkHeader), 1);
	size_t IdentifierSize = Identifier->Write(File);

	// Goto the header start position
	File->Seek(NewRecordStartPosition + OldRecordsSize, ZE_SF_BEGINING);
	Header.Header = 'ZECH';
	Header.ChunkPosition = NewChunkPosition;
	Header.ChunkHash = Identifier->GetHash();
	Header.ChunkSize = Size;
	Header.IdentifierSize = IdentifierSize;
	// Write the real header
	File->Write(&Header, sizeof(ZEChunkHeader), 1);

	// Go to the end of the records
	File->Seek(0, ZE_SF_END);
	// Write the records start position
	File->Write(&NewRecordStartPosition, sizeof(ZEDWORD), 1);
	// Flush
	File->Flush();

	return true;

	/////////// OLD CODE /////////////////

	/*fseek(File, -sizeof(ZEDWORD), SEEK_END);
	size_t OldRecordsEndPosition = ftell(File);

	size_t OldRecordsStartPosition = 0;
	fread(&OldRecordsStartPosition, sizeof(ZEDWORD), 1, File);

	size_t OldRecordsSize = OldRecordsEndPosition - OldRecordsStartPosition;
	size_t NewRecordStartPosition = OldRecordsStartPosition + Size;

	if (OldRecordsSize != 0)
		CopyData(File, OldRecordsStartPosition, OldRecordsSize, NewRecordStartPosition);

	size_t NewChunkPosition = OldRecordsStartPosition;

	fseek(File, NewChunkPosition, SEEK_SET);
	fwrite(Data, Size, 1, File);

	fseek(File, NewRecordStartPosition + OldRecordsSize, SEEK_SET);
	ZEChunkHeader Header;
	fwrite(&Header, sizeof(ZEChunkHeader), 1, File);
	size_t IdentifierSize = Identifier->Write(File);

	fseek(File, NewRecordStartPosition + OldRecordsSize, SEEK_SET);
	Header.Header = 'ZECH';
	Header.ChunkPosition = NewChunkPosition;
	Header.ChunkHash = Identifier->GetHash();
	Header.ChunkSize = Size;
	Header.IdentifierSize = IdentifierSize;
	fwrite(&Header, sizeof(ZEChunkHeader), 1, File);

	fseek(File, 0, SEEK_END);
	fwrite(&NewRecordStartPosition, sizeof(ZEDWORD), 1, File);

	fflush(File);

	CloseCache();*/
}

// Create new chunk from PartialFile
bool ZEFileCache::CreateChunk(ZEPartialFile& PartialFile, const ZECacheChunkIdentifier* Identifier, size_t ChunkSize)
{
	// If file is not open return
	if(!File || !File->IsOpen())
		return false;

	// Go to the end of records
	File->Seek(-sizeof(ZEDWORD), ZE_SF_END);
	// Get the old record's end position
	size_t OldRecordsEndPosition = File->Tell();

	// Read the old record's start position
	size_t OldRecordsStartPosition = 0;
	File->Read(&OldRecordsStartPosition, sizeof(ZEDWORD), 1);

	// Calculate the total record size
	size_t OldRecordsSize = OldRecordsEndPosition - OldRecordsStartPosition;
	// Calculate the new records start position
	size_t NewRecordStartPosition = OldRecordsStartPosition + ChunkSize;

	// If there exists old records, copy them to their new position
	if (OldRecordsSize != 0)
		CopyData(File, OldRecordsStartPosition, OldRecordsSize, NewRecordStartPosition);

	// Set the new chunk position
	size_t NewChunkPosition = OldRecordsStartPosition;

	// Set the partial file
	PartialFile.Open(File, NewChunkPosition, ChunkSize);

	// Goto the new records end position
	File->Seek(NewRecordStartPosition + OldRecordsSize, ZE_SF_BEGINING);

	ZEChunkHeader Header;
	// Write the dummy header and the identifier parameter
	File->Write(&Header, sizeof(ZEChunkHeader), 1);
	size_t IdentifierSize = Identifier->Write(File);

	// Goto the header start position
	File->Seek(NewRecordStartPosition + OldRecordsSize, ZE_SF_BEGINING);
	Header.Header = 'ZECH';
	Header.ChunkPosition = NewChunkPosition;
	Header.ChunkHash = Identifier->GetHash();
	Header.ChunkSize = ChunkSize;
	Header.IdentifierSize = IdentifierSize;
	// Write the real header
	File->Write(&Header, sizeof(ZEChunkHeader), 1);

	// Go to the end of the records
	File->Seek(0, ZE_SF_END);
	// Write the records start position
	File->Write(&NewRecordStartPosition, sizeof(ZEDWORD), 1);
	// Flush
	File->Flush();

	return true;
}

// Create new chunk from ResourceFile
bool ZEFileCache::CreateChunk(ZEPartialResourceFile& ResourceFile, const ZECacheChunkIdentifier* Identifier, size_t ChunkSize)
{
	// If file is not open return
	if(!File || !File->IsOpen())
		return false;

	// Go to the end of records
	File->Seek(-sizeof(ZEDWORD), ZE_SF_END);
	// Get the old record's end position
	size_t OldRecordsEndPosition = File->Tell();

	// Read the old record's start position
	size_t OldRecordsStartPosition = 0;
	File->Read(&OldRecordsStartPosition, sizeof(ZEDWORD), 1);

	// Calculate the total record size
	size_t OldRecordsSize = OldRecordsEndPosition - OldRecordsStartPosition;
	// Calculate the new records start position
	size_t NewRecordStartPosition = OldRecordsStartPosition + ChunkSize;

	// If there exists old records, copy them to their new position
	if (OldRecordsSize != 0)
		CopyData(File, OldRecordsStartPosition, OldRecordsSize, NewRecordStartPosition);

	// Set the new chunk position
	size_t NewChunkPosition = OldRecordsStartPosition;

	// Set the partial resource file
	ResourceFile.Open(File, NewChunkPosition, ChunkSize);

	// Goto the new records end position
	File->Seek(NewRecordStartPosition + OldRecordsSize, ZE_SF_BEGINING);

	ZEChunkHeader Header;
	// Write the dummy header and the identifier parameter
	File->Write(&Header, sizeof(ZEChunkHeader), 1);
	size_t IdentifierSize = Identifier->Write(File);

	// Goto the header start position
	File->Seek(NewRecordStartPosition + OldRecordsSize, ZE_SF_BEGINING);
	Header.Header = 'ZECH';
	Header.ChunkPosition = NewChunkPosition;
	Header.ChunkHash = Identifier->GetHash();
	Header.ChunkSize = ChunkSize;
	Header.IdentifierSize = IdentifierSize;
	// Write the real header
	File->Write(&Header, sizeof(ZEChunkHeader), 1);

	// Go to the end of the records
	File->Seek(0, ZE_SF_END);
	// Write the records start position
	File->Write(&NewRecordStartPosition, sizeof(ZEDWORD), 1);
	// Flush
	File->Flush();

	return true;
}

// Returns the data in a buffer
bool ZEFileCache::GetChunk(const ZECacheChunkIdentifier* Identifier, void* Buffer, size_t Offset, size_t Size)
{
	// If file is not open return
	if(!File || !File->IsOpen())
		return false;

	// Get hash from identifier
	ZEDWORD Hash = Identifier->GetHash();
	// Go to end of the file
	File->Seek(-sizeof(ZEDWORD), ZE_SF_END);

	// Get the records end position
	size_t RecordsEndPosition = File->Tell();

	// Read records start position from the end of the file
	size_t RecordStartPosition = 0;
	File->Read(&RecordStartPosition, sizeof(ZEDWORD), 1);

	// Set the beginning address for linear search
	size_t CurrentHeaderPosition = RecordStartPosition;
	while (true)
	{
		// Go to the next header position
		File->Seek(CurrentHeaderPosition, ZE_SF_BEGINING);

		// Read header
		ZEChunkHeader Header;
		if (File->Read(&Header, sizeof(ZEChunkHeader), 1) != 1)
		{
			return false;
		}

		// Check if the header readable
		if (Header.Header != 'ZECH')
		{
			return false;
		}

		// Check if hash, size and identifier matches 
		if (Header.ChunkHash == Hash && Header.ChunkSize == Size && Identifier->Equal(File))
		{
			// Goto the chunk data position
			File->Seek(Header.ChunkPosition, ZE_SF_BEGINING);

			// Read into buffer
			if (File->Read(Buffer, Size, 1) != 1)
			{
				return false;
			}
			else
			{
				return true;
			}
		}

		CurrentHeaderPosition += Header.IdentifierSize + sizeof(ZEChunkHeader);
	}

	return false;

	/////////// OLD CODE /////////////////

	/*OpenCache();

	ZEDWORD Hash = Identifier->GetHash();
	fseek(File, -sizeof(ZEDWORD), SEEK_END);
	size_t RecordsEndPosition = ftell(File);

	size_t RecordStartPosition = 0;
	fread(&RecordStartPosition, sizeof(ZEDWORD), 1, File);

	size_t CurrentHeaderPosition = RecordStartPosition;
	while (true)
	{
		fseek(File, CurrentHeaderPosition, SEEK_SET);

		ZEChunkHeader Header;
		if (fread(&Header, sizeof(ZEChunkHeader), 1, File) != 1)
		{
			CloseCache();
			return false;
		}

		if (Header.Header != 'ZECH')
		{
			CloseCache();
			return false;
		}

		ZEDWORD CurrentCursor = ftell(File);

		if (Header.ChunkHash == Hash && Header.ChunkSize == Size && Identifier->Equal(File))
		{
			fseek(File, Header.ChunkPosition, SEEK_SET);

			size_t Cursor = ftell(File);

			if (fread(Buffer, Size, 1, File) != 1)
			{
				CloseCache();
				return false;
			}
			else
			{
				CloseCache();
				return true;
			}
		}

		CurrentHeaderPosition += Header.IdentifierSize + sizeof(ZEChunkHeader);
	}

	CloseCache();
	return false;*/
}

// Returns the data as PartialFile
bool ZEFileCache::OpenChunk(ZEPartialFile& PartialFile, const ZECacheChunkIdentifier* Identifier)
{
	// If file is not open return
	if(!File || !File->IsOpen())
		return false;

 	// Go to the end of the file and save the position
 	File->Seek(0, ZE_SF_END);
 	size_t EndOfFile = File->Tell();
 	
 	// Get records start position
 	File->Seek(-sizeof(ZEDWORD), ZE_SF_END);
 	size_t RecordsStartPosition = 0;
 	File->Read(&RecordsStartPosition, sizeof(ZEDWORD), 1);
 
	// Set the next header start position
 	size_t NextHeaderPosition = RecordsStartPosition;
 
 	// Search for the identifier between RecordsStartPosition & EndOfFile
 	while(NextHeaderPosition < EndOfFile)
 	{
		// Goto next header position
 		File->Seek(NextHeaderPosition, ZE_SF_BEGINING);
 
 		// Read the header
 		ZEChunkHeader Header;
 		if (File->Read(&Header, sizeof(ZEChunkHeader), 1) != 1)
 		{
 			return false;
 		}
 
		// If header is not corrupted
 		if (Header.Header != 'ZECH')
 		{
 			return false;
 		}
	
		// If identifier matches
 		if(Identifier->Equal(File))
 		{
			// Open the PartialFile
			PartialFile.Open(File, Header.ChunkPosition, Header.ChunkSize);
			return true;
 		}
 
 		NextHeaderPosition += sizeof(ZEChunkHeader) + Header.IdentifierSize;
 	}
 
 	return false;
}

// Returns the data as ResourceFile
bool ZEFileCache::OpenChunk(ZEPartialResourceFile& ResourceFile, const ZECacheChunkIdentifier* Identifier)
{
	// If file is not open return
	if(!File || !File->IsOpen())
		return false;

	// Go to the end of the file and save the position
	File->Seek(0, ZE_SF_END);
	size_t EndOfFile = File->Tell();

	// Get records start position
	File->Seek(-sizeof(ZEDWORD), ZE_SF_END);
	size_t RecordsStartPosition = 0;
	File->Read(&RecordsStartPosition, sizeof(ZEDWORD), 1);

	// Set the next header start position
	size_t NextHeaderPosition = RecordsStartPosition;

	// Search for the identifier between RecordsStartPosition & EndOfFile
	while(NextHeaderPosition < EndOfFile)
	{
		// Goto next header position
		File->Seek(NextHeaderPosition, ZE_SF_BEGINING);

		// Read the header
		ZEChunkHeader Header;
		if (File->Read(&Header, sizeof(ZEChunkHeader), 1) != 1)
		{
			return false;
		}

		// If header is not corrupted
		if (Header.Header != 'ZECH')
		{
			return false;
		}

		// If identifier matches
		if(Identifier->Equal(File))
		{
			// Set the partial resource file
			ResourceFile.Open(File, Header.ChunkPosition, Header.ChunkSize);
			return true;
		}

		NextHeaderPosition += sizeof(ZEChunkHeader) + Header.IdentifierSize;
	}

	return false;
}


// ZEPartialFile* ZEFileCache::CreateChunk(const char* FileName, const ZECacheChunkIdentifier* Identifier, size_t TotalChunkSize)
// {
// 
// 	/* Find the size and the offset of the chunk data defined by the identifier */
// 	OpenCache();
// 
// 	// Get end of file
// 	File->Seek(0, ZE_SF_END);
// 	size_t EndOfFile = 0;
// 	EndOfFile = File->Tell();
// 
// 	// Get records start position
// 	File->Seek(-sizeof(ZEDWORD), ZE_SF_END);
// 	size_t RecordsStartPosition = 0;
// 	File->Read(&RecordsStartPosition, sizeof(ZEDWORD), 1);
// 
// 	size_t NextHeaderPosition = RecordsStartPosition;
// 	//size_t IdentifierSize = 0;
// 
// 	// Search for the identifier between CurrentPosition && EndOfFile
// 	while(NextHeaderPosition < EndOfFile)
// 	{
// 		File->Seek(NextHeaderPosition, ZE_SF_BEGINING);
// 
// 		// Get the header
// 		ZEChunkHeader Header;
// 		if (File->Read(&Header, sizeof(ZEChunkHeader), 1) != 1)
// 		{
// 			CloseCache();
// 			return false;
// 		}
// 
// 		if (Header.Header != 'ZECH')
// 		{
// 			CloseCache();
// 			return false;
// 		}
// 
// 		if(Identifier->Equal(File))
// 		{
// 			// Create the partial file
// 			return new ZEPartialFile(File, Header.ChunkPosition, Header.ChunkSize);
// 		}
// 
// 		NextHeaderPosition += sizeof(ZEChunkHeader) + Header.IdentifierSize;
// 	}
// 
// 	return NULL;
// }
// 
// bool ZEFileCache::OpenChunk( const ZECacheChunkIdentifier* Identifier, size_t TotalChunkSize)
// {
// 	OpenCache();
// 
// 	File->Seek(-sizeof(ZEDWORD), ZE_SF_END);
// 
// 	// Get old record end position
// 	size_t OldRecordsEndPosition = 0;
// 	OldRecordsEndPosition = File->Tell();
// 
// 	// Get old record start position
// 	size_t OldRecordsStartPosition = 0;
// 	File->Read(&OldRecordsStartPosition, sizeof(ZEDWORD), 1);
// 
// 	// Calculate the old record's total size and the new record start position
// 	size_t OldRecordsSize = OldRecordsEndPosition - OldRecordsStartPosition;
// 	size_t NewRecordStartPosition = OldRecordsStartPosition + TotalChunkSize;
// 
// 	// Copy all the records to their new position
// 	if(OldRecordsSize != 0)
// 		CopyData(File, OldRecordsStartPosition, OldRecordsSize, NewRecordStartPosition);
// 
// 	// Goto last record en position and write the empty header
// 	File->Seek(NewRecordStartPosition + OldRecordsSize, ZE_SF_BEGINING);
// 	ZEChunkHeader Header;
// 	File->Write(&Header, sizeof(ZEChunkHeader), 1);
// 	// Write the identifier
// 	size_t IdentifierSize = Identifier->Write(File);
// 
// 	// Go back to new header start position
// 	File->Seek(NewRecordStartPosition + OldRecordsSize, ZE_SF_BEGINING);
// 
// 	// Fill the header data
// 	Header.Header = 'ZECH';
// 	Header.ChunkPosition = OldRecordsStartPosition;
// 	Header.ChunkHash = Identifier->GetHash();
// 	Header.ChunkSize = TotalChunkSize;
// 	Header.IdentifierSize = IdentifierSize;
// 
// 	// Write the actual header data
// 	File->Write(&Header, sizeof(ZEChunkHeader), 1);
// 
// 	// Goto end of the file
// 	File->Seek(0, ZE_SF_END);
// 
// 	// Write the new record start position
// 	File->Write(&NewRecordStartPosition, sizeof(ZEDWORD), 1);
// 
// 	// Goto the beginning of the chunk data so that user can start adding data
// 	File->Seek(OldRecordsStartPosition, ZE_SF_BEGINING);
// 
// 	File->Flush();
// 
// 	return true;
// 
// 	/////////// OLD CODE /////////////////
// 
// 
// 
// 	/*OpenCache();
// 
// 	// Read the last element which is first record's start position
// 	fseek(File, -sizeof(ZEDWORD), SEEK_END);
// 
// 	// Get old record end position
// 	size_t OldRecordsEndPosition = 0;
// 	OldRecordsEndPosition = ftell(File);
// 
// 	// Get old record start position
// 	size_t OldRecordsStartPosition = 0;
// 	fread(&OldRecordsStartPosition, sizeof(ZEDWORD), 1, File);
// 
// 	// Calculate the old record's total size and the new record start position
// 	size_t OldRecordsSize = OldRecordsEndPosition - OldRecordsStartPosition;
// 	size_t NewRecordStartPosition = OldRecordsStartPosition + TotalChunkSize;
// 
// 	// Copy all the records to their new position
// 	if(OldRecordsSize != 0)
// 	CopyData(File, OldRecordsStartPosition, OldRecordsSize, NewRecordStartPosition);
// 
// 
// 	// Goto last record en position and write the empty header
// 	fseek(File, NewRecordStartPosition + OldRecordsSize, SEEK_SET);
// 	ZEChunkHeader Header;
// 	fwrite(&Header, sizeof(ZEChunkHeader), 1, File);
// 	// Write the identifier
// 	size_t IdentifierSize = Identifier->Write(File);
// 
// 	// Go back to new header start position
// 	fseek(File, NewRecordStartPosition + OldRecordsSize, SEEK_SET);
// 	// Fill the header data
// 	Header.Header = 'ZECH';
// 	Header.ChunkPosition = OldRecordsStartPosition;
// 	Header.ChunkHash = Identifier->GetHash();
// 	Header.ChunkSize = TotalChunkSize;
// 	Header.IdentifierSize = IdentifierSize;
// 
// 	// Write the actual header data
// 	fwrite(&Header, sizeof(ZEChunkHeader), 1, File);
// 
// 	// Goto end of the file
// 	fseek(File, 0, SEEK_END);
// 
// 	// Write the new record start position
// 	fwrite(&NewRecordStartPosition, sizeof(ZEDWORD), 1, File);
// 
// 	// Goto the beginning of the chunk data so that user can start adding data
// 	fseek(File, OldRecordsStartPosition, SEEK_SET);
// 
// 	// Flush
// 	fflush(File);
// 
// 	return true;*/
// }
// 
// // WARNING: this function may corrupt the cache if it writes more data than the empty space.
// size_t ZEFileCache::AddToChunk(void* Data, size_t Size, size_t Count)
// {
// 	// Directly write the data
// 	return File->Write(Data, Size, Count);
// 
// 	/////////// OLD CODE /////////////////
// 	// fwrite(Data, Size, 1, File);
// }
// 
// size_t ZEFileCache::GetFromChunk(void* Data, size_t Size, size_t Count)
// {
// 	// Directly read the data
// 	File->Read(Data, Size, Count);
// 
// 	/////////// OLD CODE /////////////////
// 	//fread(Data, Size, 1, File);
// }
// 
// // OK
// void ZEFileCache::CloseChunk()
// {
// 	CloseCache();
// }
