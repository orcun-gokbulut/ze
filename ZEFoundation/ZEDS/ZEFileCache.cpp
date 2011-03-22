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
#include "ZETypes.h"
#include <stdio.h>


struct ZEChunkHeader
{
	ZEDWORD				Header;
	ZEDWORD				ChunkHash;
	ZEDWORD				ChunkPosition;
	ZEDWORD				ChunkSize;
	ZEDWORD				IdentifierSize;
};

bool ZEFileCache::OpenCache(const char* FileName)
{
	File = fopen(FileName, "a+b");
	if (File == NULL)
		return false;

	return true;
}

void ZEFileCache::CloseCache()
{
	if (File != NULL)
	{
		fclose(File);
		File = NULL;
	}
}

void CopyData(FILE* File, size_t From, size_t Size, size_t To)
{
	const size_t BufferSize = 65536;
	char Buffer[BufferSize];

	int Count = Size / BufferSize;
	int LeftOver = Size % BufferSize;
	for (size_t I = 0; I < Count; I++)
	{
		fseek(File, From + I * BufferSize, SEEK_SET);
		fread(Buffer, BufferSize, 1, File);
		fseek(File, To + I * BufferSize, SEEK_SET);
		fwrite(Buffer, BufferSize, 1, File);
	}

	if (LeftOver != 0)
	{
		fseek(File, From + Count * BufferSize, SEEK_SET);
		fread(Buffer, LeftOver, 1, File);
		fseek(File, To + Count * BufferSize, SEEK_SET);
		fwrite(Buffer, LeftOver, 1, File);
	}
}

void ZEFileCache::AddChunk(const ZECacheChunkIdentifier* Identifier, const void* Data, size_t Size)
{
	fseek(File, -sizeof(ZEDWORD), SEEK_END);
	size_t HeaderStart;

	// Find Chunk Count
	ZEDWORD EndOfFile = 0;
	EndOfFile = ftell(File);

	ZEDWORD DataCursor = 0;
	fread(&DataCursor, sizeof(ZEDWORD), 1, File);

	// Copy headers to further location
	ZEDWORD HeadersCursor = DataCursor + Size;
	if (EndOfFile != 0)
		CopyData(File, HeadersCursor, EndOfFile - DataCursor - sizeof(ZEDWORD), DataCursor);

	// Copy Chunk Data
	fseek(File, DataCursor, SEEK_SET);
	fwrite(Data, Size, 1, File);

	// Add new header
	//   Write Identifier
	fseek(File, 0, SEEK_END);
	//fseek(File, sizeof(ZEChunkHeader), SEEK_CUR);

	//   Write Header
	ZEChunkHeader NewHeader;
	NewHeader.Header = 'ZECH';
	NewHeader.ChunkPosition = DataCursor;
	NewHeader.ChunkHash = Identifier->GetHash();
	NewHeader.ChunkSize = Size;
//	NewHeader.IdentifierSize = IdentifierSize;

//	size_t IdentifierSize = Identifier->Write(File);

	//fseek(File, -IdentifierSize - sizeof(ZEChunkHeader), SEEK_END);
	fwrite(&NewHeader, sizeof(ZEChunkHeader), 1, File);

	// Set headers start position
	fseek(File, 0, SEEK_END);
	fwrite(&HeadersCursor, sizeof(ZEDWORD), 1, File);

	// Flush stream
	fflush(File);
}

bool ZEFileCache::GetChunkData(const ZECacheChunkIdentifier* Identifier, void* Buffer, size_t Offset, size_t Size)
{
	fseek(File, -sizeof(ZEDWORD), SEEK_END);
	ZEDWORD EndOfFile = ftell(File);

	ZEDWORD FirstHeaderCursor = 0;
	fread(&FirstHeaderCursor, sizeof(ZEDWORD), 1, File);
	fseek(File, FirstHeaderCursor - 1, SEEK_SET);
	
	ZEDWORD Hash = Identifier->GetHash();

	while (true)
	{
		ZEChunkHeader CurrentHeader;
		if (fread(&CurrentHeader, sizeof(ZEChunkHeader), 1, File) != sizeof(ZEChunkHeader))
			return false;

		if (CurrentHeader.Header != 'ZECH')
			return false;

		ZEDWORD CurrentCursor = ftell(File);
		if (CurrentHeader.ChunkHash == Hash)
		{
			if (Identifier->Equal(File))
			{
				return false;
			}
		}

		fseek(File, CurrentCursor + CurrentHeader.IdentifierSize, SEEK_SET);
	}
}

void ZEFileCache::ClearCache()
{
	CloseCache();
}

ZEFileCache::ZEFileCache()
{
	File = NULL;
}

ZEFileCache::~ZEFileCache()
{
	CloseCache();
}
