//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFileCache.h
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

#pragma once
#ifndef __ZE_FILE_CACHE_H__
#define __ZE_FILE_CACHE_H__

#include "ZEArray.h"
#include "ZETypes.h"
#include "..\Source\ZECore\ZEFile.h"

#include <stdio.h>

class ZEResourceFile;
class ZEPartialResourceFile;


class ZECacheChunkIdentifier
{
	public:
		virtual ZEDWORD					GetHash() const = 0;
		virtual size_t					Write(ZEFile* File) const = 0;
		virtual bool					Equal(ZEFile* File) const = 0;
};


class ZEFileCache
{
	private:

		ZEFile*							File;
		char							CacheFileName[256];
		//const char*						CacheFileName;

	public:

										// Constructor
										ZEFileCache();
										// Destructor
										~ZEFileCache();										
										// Opens the cache file
		bool							OpenCache(const char* CacheFileName);
										// Closes the cache file
		void							CloseCache();
										// Empty the cache file
		bool							ClearCache();
										// Returns the cache file name
		const char*						GetCacheFileName();
										// Checks if the chunk exists
		bool							ChunkExists(const ZECacheChunkIdentifier* Identifier);
										// Create new chunk from Buffer
		bool							AddChunk(const ZECacheChunkIdentifier* Identifier, const void* Data, size_t Size);
										// Create new chunk from PartialFile
		bool							CreateChunk(ZEPartialFile& PartialFile, const ZECacheChunkIdentifier* Identifier, size_t ChunkSize);
										// Create new chunk from ResourceFile
		bool							CreateChunk(ZEPartialResourceFile& ResourceFile, const ZECacheChunkIdentifier* Identifier, size_t ChunkSize);
										// Returns the data in a buffer
		bool							GetChunk(const ZECacheChunkIdentifier* Identifier, void* Buffer, size_t Offset, size_t Size);
										// Returns the data as PartialFile
		bool							OpenChunk(ZEPartialFile& PartialFile, const ZECacheChunkIdentifier* Identifier);
										// Returns the data as ResourceFile
		bool							OpenChunk(ZEPartialResourceFile& ResourceFile, const ZECacheChunkIdentifier* Identifier);



										// KALKACAK
// 		bool							OpenChunk(const ZECacheChunkIdentifier* Identifier, size_t TotalChunkSize);
// 		size_t							AddToChunk(void* Data, size_t Size, size_t Count);
// 		size_t							GetFromChunk(void* Data, size_t Size, size_t Count);
// 		void							CloseChunk();

										
};

#endif
