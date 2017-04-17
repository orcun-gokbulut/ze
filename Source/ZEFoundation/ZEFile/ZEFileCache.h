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

#include "ZEFile.h"
#include "ZETypes.h"
#include "ZEPartialFile.h"
#include "ZEDS/ZEString.h"
#include "ZEExport.ZEFoundation.h"


#define ZE_FILE_MAKEVERSION(Major, Minor)			((((ZEUInt32)(Major)) << 16) + (ZEUInt32)(Minor))
#define ZE_CACHE_VERSION							ZE_FILE_MAKEVERSION(0,1)

#define ZE_CACHE_HEADER								((ZEUInt32)((ZEUInt32)'CCH ' + (ZEUInt32)'HDR '))
#define	ZE_CACHE_IDENT_CHUNKID						((ZEUInt32)(ZE_CACHE_HEADER + (ZEUInt32)'IDEN'))
#define	ZE_CACHE_DATA_CHUNKID						((ZEUInt32)(ZE_CACHE_HEADER + (ZEUInt32)'DATA'))

#define ZE_CACHE_COMPLETENESS						((ZEUInt64)((((ZEUInt64)('FCCH')) << 32) + (ZEUInt64)('CMPT')))


struct ZECacheFileHeader
{
	ZEUInt32		ChunkId;
	ZEUInt32		Version;
};

struct ZECacheDataChunk
{
	ZEUInt32		ChunkId;
};

struct ZECacheIdentifierChunk
{
	ZEUInt32		ChunkId;
	ZEUInt64		ChunkHash;
	ZEUInt64		ChunkSize;
	ZEUInt64		ChunkPosition;
	ZEUInt64		IdentifierSize;
};

class ZECacheDataIdentifier
{
	public:
		virtual ZEUInt64			GetHash() const = 0;
		virtual ZEUInt64			Write(ZEFile* File) const = 0;
		virtual bool			Equal(ZEFile* File) const = 0;
};


class ZE_EXPORT_ZEFOUNDATION ZEFileCache
{
	private:
		ZEFile					File;

		bool					CheckCompleteness();
		bool					PrepareCacheForFirstUse();
		bool					CopyData(ZEFile* File, ZEUInt64 From, ZEUInt64 Size, ZEUInt64 To);

	public:
		bool					Open(const ZEString FileName);
		bool					Open(ZEFile* File);

		void					Close();
		bool					Clear();
		bool					IsOpen();
		ZEFile					GetFile();
		const ZEString			GetCacheFilePath();

		bool					AddData(const ZECacheDataIdentifier* Identifier, const void* Data, ZEUInt64 Size);
		bool					GetData(const ZECacheDataIdentifier* Identifier, void* Buffer, ZEUInt64 Offset, ZEUInt64 Size);
		
		bool					Allocate(ZEPartialFile* PartialFile, const ZECacheDataIdentifier* Identifier, ZEUInt64 ChunkSize);
		bool					OpenData(ZEPartialFile* PartialFile, const ZECacheDataIdentifier* Identifier);

		static bool				IsFileCache(ZEString FileName);
		bool					IdentifierExists(const ZECacheDataIdentifier* Identifier);

								ZEFileCache();
		virtual					~ZEFileCache();		
};

#endif
