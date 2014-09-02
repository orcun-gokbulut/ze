//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETerrainBlockCache.h
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
#ifndef __ZE_TERRAIN_BLOCK_CACHE_H__
#define __ZE_TERRAIN_BLOCK_CACHE_H__

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZETerrainBlock.h"
#include "ZEThread/ZEThread.h"
#include "ZEThread/ZELock.h"
#include "ZEThread/ZESignal.h"

enum ZETerrainBlockCacheEntryType
{
	ZE_TBCET_EMPTY,
	ZE_TBCET_REQUESTED,
	ZE_TBCET_BUFFERED,
	ZE_TBCET_NOT_AVAILABLE
};

enum ZETerrainBlockRequestType
{
	ZE_TBRT_EMPTY,
	ZE_TBRT_HIGH_PRIORITY,
	ZE_TBRT_REQUESTED,
	ZE_TBRT_BUFFERED,
};

struct ZETerrainBlockCacheEntry
{
	ZETerrainBlockCacheEntryType		Type;
	ZETerrainBlock						Block;

										ZETerrainBlockCacheEntry();
};

struct ZETerrainBlockRequest
{
	ZESize								Order;
	ZETerrainBlockRequestType			Type;
	ZEUInt64							PositionX;
	ZEUInt64							PositionY;
	ZEUInt								Level;

										ZETerrainBlockRequest();
};

class ZETerrainResource;

class ZETerrainBlockCache
{
	private:
		ZESize								BlockCacheCount;
		ZEArray<ZETerrainBlockCacheEntry>	BlockCache;
		ZELock								BlockCacheLock;

		ZESize								RequestStart;
		ZESize								RequestEnd;
		ZEArray<ZETerrainBlockRequest>		RequestList;
		ZELock								RequestListLock;
		
		ZETerrainBlockInfo					BlockInfo;

		ZEThread							IOThread;
		ZESignal							IOThreadSignal;
			
		ZETerrainResource*					Resource;

		bool								Initialized;

		void								IOLoop(ZEThread* Thread, void* ExtraParameters);

		void								BlockInitialize(ZETerrainBlock* Output, ZEUInt Level, ZEInt64 PositionX, ZEInt64 PositionY);
		void								BlockUpsample(ZETerrainBlock* Output, ZETerrainBlock* Source);
		void								BlockEmpty(ZETerrainBlock* Output);
		void								BlockCopy(ZETerrainBlock* Output, ZETerrainBlock* Source);

		double								FreeBlockHeuristic(ZETerrainBlockCacheEntry* Entry);
		ZETerrainBlockCacheEntry*			GetFreeCacheEntry();
		bool								GetRequest(ZETerrainBlockRequest& Output);

		void								AddRequest(ZEUInt Level, ZEInt64 PositionX, ZEInt64 PositionY, ZETerrainBlockRequestType RequestType);
		ZETerrainBlock*						QueryBlock(ZEUInt Level, ZEInt64 PositionX, ZEInt64 PositionY);

	public:
		void								SetBlockCacheSize(ZESize BlockCount);
		ZESize								GetBlockCacheSize();

		void								SetRequestListSize(ZESize RequestListSize);
		ZESize								GetRequestListSize();

		void								SetResource(ZETerrainResource* Resource);
		ZETerrainResource*					GetResource();

		bool								GetBlock(ZETerrainBlock* OutputBlock, ZEUInt Level, ZEInt64 PositionX, ZEInt64 PositionY);

		bool								Initialize();
		void								Deinitialize();

											ZETerrainBlockCache();
											~ZETerrainBlockCache();
};

#endif
