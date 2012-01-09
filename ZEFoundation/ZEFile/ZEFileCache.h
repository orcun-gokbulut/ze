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
#include "ZEDS\ZEString.h"
#include "zepartialfile.h"


#define ZE_FILE_MAKEVERSION(Major, Minor)			((((ZEDWORD)(Major)) << 16) + (ZEDWORD)(Minor))
#define ZE_CACHE_VERSION							ZE_FILE_MAKEVERSION(0,1)

#define ZE_CACHE_HEADER								((ZEDWORD)((ZEDWORD)'CCH ' + (ZEDWORD)'HDR '))
#define	ZE_CACHE_IDENT_CHUNKID						((ZEDWORD)(ZE_CACHE_HEADER + (ZEDWORD)'IDEN'))
#define	ZE_CACHE_DATA_CHUNKID						((ZEDWORD)(ZE_CACHE_HEADER + (ZEDWORD)'DATA'))

#define ZE_CACHE_COMPLETENESS						((ZEQWORD)((((ZEQWORD)('FCCH')) << 32) + (ZEQWORD)('CMPT')))


struct ZECacheFileHeader
{
	ZEDWORD		ChunkId;
	ZEDWORD		Version;
};

struct ZECacheDataChunk
{
	ZEDWORD		ChunkId;
};

struct ZECacheIdentifierChunk
{
	ZEDWORD		ChunkId;
	ZEQWORD		ChunkHash;
	ZEQWORD		ChunkSize;
	ZEQWORD		ChunkPosition;
	ZEQWORD		IdentifierSize;
};

class ZECacheDataIdentifier
{
	public:
		virtual ZEQWORD			GetHash() const = 0;
		virtual ZEQWORD			Write(ZEFile* File) const = 0;
		virtual bool			Equal(ZEFile* File) const = 0;
};


class ZEFileCache
{
	private:
		ZEFile					File;

		bool					CheckCompleteness();
		bool					PrepareCacheForFirstUse();
		bool					CopyData(ZEFile* File, ZEQWORD From, ZEQWORD Size, ZEQWORD To);

	public:
		bool					Open(const ZEString FileName);
		bool					Open(ZEFile* File);

		void					Close();
		bool					Clear();
		bool					IsOpen();
		ZEFile					GetFile();
		const ZEString			GetCacheFilePath();

		bool					AddData(const ZECacheDataIdentifier* Identifier, const void* Data, ZEQWORD Size);
		bool					GetData(const ZECacheDataIdentifier* Identifier, void* Buffer, ZEQWORD Offset, ZEQWORD Size);
		
		bool					Allocate(ZEPartialFile* PartialFile, const ZECacheDataIdentifier* Identifier, ZEQWORD ChunkSize);
		bool					OpenData(ZEPartialFile* PartialFile, const ZECacheDataIdentifier* Identifier);

		static bool				IsFileCache(ZEString FileName);
		bool					IdentifierExists(const ZECacheDataIdentifier* Identifier);

								ZEFileCache();
		virtual					~ZEFileCache();		
};

#endif
