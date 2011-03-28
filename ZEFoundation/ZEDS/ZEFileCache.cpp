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
	File = fopen(FileName, "r+b");
	
	if (File == NULL)
		File = fopen(FileName, "w+b");

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
}

bool ZEFileCache::GetChunkData(const ZECacheChunkIdentifier* Identifier, void* Buffer, size_t Offset, size_t Size)
{
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
			return false;

		if (Header.Header != 'ZECH')
			return false;

		ZEDWORD CurrentCursor = ftell(File);
		if (Header.ChunkHash == Hash && Identifier->Equal(File))
			return true;

		CurrentHeaderPosition += Header.IdentifierSize + sizeof(ZEChunkHeader);
	}

	return false;
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
