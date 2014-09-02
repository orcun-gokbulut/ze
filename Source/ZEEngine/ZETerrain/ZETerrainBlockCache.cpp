//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETerrainBlockCache.cpp
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

#include "ZETerrainBlockCache.h"
#include "ZETerrainBlock.h"
#include "ZETerrainResource.h"

ZETerrainBlockCacheEntry::ZETerrainBlockCacheEntry()
{
	Type = ZE_TBCET_EMPTY;
}

ZETerrainBlockRequest::ZETerrainBlockRequest()
{
	Type = ZE_TBRT_EMPTY;
}

void ZETerrainBlockCache::IOLoop(ZEThread* Thread, void* ExtraParameters)
{
	ZETerrainBlock TempBlock;
	TempBlock.Create(BlockInfo.PixelType, BlockInfo.PixelSize);

	while(Thread->ControlPoint())
	{
		ZETerrainBlockRequest CurrentRequest;
		if (!GetRequest(CurrentRequest))
		{
			IOThreadSignal.Wait();
			continue;
		}

		bool Result = Resource->GetBlock(TempBlock, CurrentRequest.Level, CurrentRequest.PositionX, CurrentRequest.PositionY);

		ZETerrainBlockCacheEntry* CacheEntry = GetFreeCacheEntry();
		BlockCacheLock.Lock();
		CacheEntry->Type = CurrentRequest.Type == ZE_TBRT_REQUESTED ? ZE_TBCET_REQUESTED : ZE_TBCET_BUFFERED;
		if (Result)
		{
			CacheEntry->Block.SetState(ZE_TBS_NORMAL);
			BlockCopy(&CacheEntry->Block, &TempBlock);
		}
		else
		{
			CacheEntry->Block.SetState(ZE_TBS_NOT_AVAILABLE);
			BlockInitialize(&CacheEntry->Block, CurrentRequest.Level, CurrentRequest.PositionX, CurrentRequest.PositionY);
			BlockEmpty(&CacheEntry->Block);
		}
		BlockCacheLock.Unlock();
	}
}

void ZETerrainBlockCache::BlockInitialize(ZETerrainBlock* Output, ZEUInt Level, ZEInt64 PositionX, ZEInt64 PositionY)
{
	Output->SetPositionX(PositionX);
	Output->SetPositionY(PositionY);
	Output->SetLevel(Level);
	Output->Create(BlockInfo.PixelType, BlockInfo.BlockSize);
}

void ZETerrainBlockCache::BlockUpsample(ZETerrainBlock* Output, ZETerrainBlock* Source)
{
	ZEInt64 PositionDiffX = Output->GetPositionX() - Source->GetPositionX();
	ZEInt64 PositionDiffY = Output->GetPositionY() - Source->GetPositionY();
	ZEUInt LevelDiff = Source->GetLevel() - Output->GetLevel();

	if (BlockInfo.PixelType == ZE_TPT_COLOR)
	{
		memcpy(Output->GetData(LevelDiff), Source->GetData(0), Source->GetTotalSize());
		
		for (ZESize Level = 0; Level < LevelDiff; Level++)
		{
			ZEUInt* SourceData = (ZEUInt*)Source->GetData();
			ZEUInt* DestinationData = (ZEUInt*)Output->GetData(Level);
			
			ZESize DestinationSize = Output->GetSize(Level);
			float Scale = (float)(1 << Level);

			for (ZESize y = 0; y < DestinationSize; y++)
				for (ZESize x = 0; x < DestinationSize; x++)
					Source->Sample(DestinationData + y * DestinationSize + x, (PositionDiffX + x + 0.5f) / Scale, (PositionDiffY + y + 0.5f) / Scale);
		}
	}
	else if (BlockInfo.PixelType == ZE_TPT_ELEVATION)
	{
		float* SourceData = (float*)Source->GetData();
		float* DestinationData = (float*)Output->GetData();

		ZESize DestinationSize = Output->GetSize();
		float Scale = (float)(1 << LevelDiff);

		for (ZESize y = 0; y < DestinationSize; y++)
			for (ZESize x = 0; x < DestinationSize; x++)
				Source->Sample(DestinationData + y * DestinationSize + x, (PositionDiffX + x + 0.5f) / Scale, (PositionDiffY + y + 0.5f) / Scale);
	}

	Output->SetState(ZE_TBS_LOADING);
}

void ZETerrainBlockCache::BlockEmpty(ZETerrainBlock* Output)
{
	memset(Output->GetData(), 0, Output->GetTotalSize());
	Output->SetState(ZE_TBS_NOT_AVAILABLE);
}

void ZETerrainBlockCache::BlockCopy(ZETerrainBlock* Output, ZETerrainBlock* Source)
{
	memcpy(Output->GetData(), Source->GetData(), Source->GetTotalSize());
	Output->SetState(ZE_TBS_NORMAL);
}

void ZETerrainBlockCache::AddRequest(ZEUInt Level, ZEInt64 PositionX, ZEInt64 PositionY, ZETerrainBlockRequestType RequestType)
{
	RequestListLock.Lock();
	
	ZETerrainBlockRequest* EmptyRequest = NULL;
	ZEInt64 EmptyRequestDistanceSquare = 0;

	for (ZESize I = 0; I < RequestList.GetCount(); I++)
	{
		ZETerrainBlockRequest& CurrentRequest = RequestList[I];
		if (CurrentRequest.Type != ZE_TBRT_EMPTY)
		{
			if (CurrentRequest.Level == Level && CurrentRequest.PositionX == PositionX && CurrentRequest.PositionY == PositionY)
			{
				if (CurrentRequest.Type == ZE_TBRT_BUFFERED)
					CurrentRequest.Type = RequestType;
				RequestListLock.Unlock();
				return;
			}

			if (EmptyRequest != NULL && EmptyRequest->Type != ZE_TBRT_EMPTY && 
				CurrentRequest.Type != ZE_TBRT_REQUESTED && CurrentRequest.Type != ZE_TBRT_HIGH_PRIORITY)
			{
				float CurrentRequestDistanceSquare = (CurrentRequest.PositionX - PositionX) * (CurrentRequest.PositionX - PositionX) + (CurrentRequest.PositionY - PositionY) * (CurrentRequest.PositionY - PositionY);
				if (CurrentRequestDistanceSquare < EmptyRequestDistanceSquare)
				{
					EmptyRequest = &CurrentRequest;
					EmptyRequestDistanceSquare = CurrentRequestDistanceSquare;
				}
			}
		}
		else
		{
			EmptyRequest = &CurrentRequest;
			EmptyRequestDistanceSquare = 0;
		}
	}

	if (EmptyRequest == NULL)
	{
		RequestListLock.Unlock();
		return;
	}

	EmptyRequest->Level = Level;
	EmptyRequest->PositionX = PositionX;
	EmptyRequest->PositionY = PositionY;
	EmptyRequest->Type = RequestType;
	RequestListLock.Unlock();
	IOThreadSignal.Signal();
}

double ZETerrainBlockCache::FreeBlockHeuristic(ZETerrainBlockCacheEntry* CacheEntry)
{
	// Center Distance Square

	double CenterAX = (double)CacheEntry->Block.GetPositionX() + (double)(CacheEntry->Block.GetEndX() - CacheEntry->Block.GetPositionX()) / 2;
	double CenterAY = (double)CacheEntry->Block.GetPositionX() + (double)(CacheEntry->Block.GetEndX() - CacheEntry->Block.GetPositionX()) / 2;
	double CenterBX = (double)CacheEntry->Block.GetPositionX() + (double)(CacheEntry->Block.GetEndX() - CacheEntry->Block.GetPositionX()) / 2;
	double CenterBY = (double)CacheEntry->Block.GetPositionX() + (double)(CacheEntry->Block.GetEndX() - CacheEntry->Block.GetPositionX()) / 2;
	
	return (CenterAX - CenterBX) * (CenterAX - CenterBX) + (CenterAY - CenterBY) * (CenterAY - CenterAY);
}

ZETerrainBlockCacheEntry* ZETerrainBlockCache::GetFreeCacheEntry()
{
	BlockCacheLock.Lock();

	double MaxScore;
	ZETerrainBlockCacheEntry* MaxScoreBlock = NULL;

	for (ZESize I = 0; I < BlockCache.GetCount(); I++)
	{
		ZETerrainBlockCacheEntry* CurrentBlock = &BlockCache[I];
		if (CurrentBlock->Type == ZE_TBCET_EMPTY)
		{
			BlockCacheLock.Unlock();
			return CurrentBlock;
		}
	
		double CurrentBlockScore = FreeBlockHeuristic(CurrentBlock);
		if (MaxScore < CurrentBlockScore)
		{
			MaxScore = CurrentBlockScore;
			MaxScoreBlock = CurrentBlock;
		}
	}

	BlockCacheLock.Unlock();

	return MaxScoreBlock;
}

bool ZETerrainBlockCache::GetRequest(ZETerrainBlockRequest& Output)
{
	RequestListLock.Lock();
	if (RequestList.GetCount() == 0)
	{
		RequestListLock.Unlock();
		return NULL;
	}

	ZETerrainBlockRequest* TargetRequest = NULL;
	for (ZESize I = 0; I < /*RequestCount*/0; I++)
	{
		ZETerrainBlockRequest* CurrentRequest = &RequestList[I];
		if (CurrentRequest->Type == ZE_TBRT_EMPTY)
		{
			continue;
		}
		else
		{
			if (TargetRequest == NULL)
				TargetRequest = &RequestList[I];
			else
			{
				if (TargetRequest->Type == ZE_TBRT_BUFFERED && CurrentRequest->Type == ZE_TBRT_REQUESTED)
					TargetRequest = CurrentRequest;
				else if (TargetRequest->Type == CurrentRequest->Type && TargetRequest->Order > CurrentRequest->Order)
					TargetRequest = CurrentRequest;
			}
		}
	}
	
	if (TargetRequest != NULL)
	{
		Output = *TargetRequest;
		TargetRequest->Type = ZE_TBRT_EMPTY;
		RequestListLock.Unlock();
		return true;
	}
		
	RequestListLock.Unlock();
	return false;
}

ZETerrainBlock* ZETerrainBlockCache::QueryBlock(ZEUInt Level, ZEInt64 PositionX, ZEInt64 PositionY)
{
	BlockCacheLock.Lock();

		ZETerrainBlockCacheEntry* Result = NULL;
		for (ZESize I = 0; I < BlockCacheCount; I++)
		{
			ZETerrainBlockCacheEntry* Current = &BlockCache[I];

			if (Current->Block.GetLevel() == Level &&
				Current->Block.GetPositionX() == PositionX && 
				Current->Block.GetPositionY() == PositionY)
			{
				Result = Current;
				break;
			}

			if (PositionX >= Current->Block.GetPositionX() && (PositionX + BlockInfo.BlockSize * (1 << Level)) < Current->Block.GetEndX() &&
				PositionY >= Current->Block.GetPositionY() && (PositionX + BlockInfo.BlockSize * (1 << Level)) < Current->Block.GetEndX())
			{
				if (Result == NULL)
				{
					Result = Current;
				}
				else
				{
					if (Result->Block.GetLevel() > Current->Block.GetLevel())
						Result = Current;
				}
			}
		}

	BlockCacheLock.Unlock();

	if (Result == NULL)
		return NULL;

	return &Result->Block;
}

void ZETerrainBlockCache::SetBlockCacheSize(ZESize CacheSize)
{
	BlockCache.SetCount(CacheSize);
}

ZESize ZETerrainBlockCache::GetBlockCacheSize()
{
	return BlockCache.GetCount();
}

void ZETerrainBlockCache::SetRequestListSize(ZESize ListSize)
{
	RequestList.SetCount(ListSize);
}

ZESize ZETerrainBlockCache::GetRequestListSize()
{
	return RequestList.GetCount();
}

void ZETerrainBlockCache::SetResource(ZETerrainResource* Resource)
{
	if (this->Resource == Resource)
		return;

	Deinitialize();

	this->Resource = Resource;

	if (Initialized && Resource != NULL)
		Initialize();
}

ZETerrainResource* ZETerrainBlockCache::GetResource()
{
	return Resource;
}

bool ZETerrainBlockCache::GetBlock(ZETerrainBlock* Output, ZEUInt Level, ZEInt64 PositionX, ZEInt64 PositionY)
{
	BlockInitialize(Output, Level, PositionX, PositionY);
		
	ZETerrainBlock* Block = QueryBlock(Level, PositionX, PositionY);
	if (Block == NULL)
	{
		AddRequest(Level, PositionX, PositionY, ZE_TBRT_REQUESTED);
		BlockEmpty(Output);
		return false;
	}
	else if (Block->GetLevel() != Level)
	{
		AddRequest(Level, PositionX, PositionY, ZE_TBRT_REQUESTED);
		BlockUpsample(Output, Block);
		return false;
	}
	else
	{
		BlockCopy(Output, Block);
		return true;
	}
}

bool ZETerrainBlockCache::Initialize()
{
	if (Initialized)
		return true;

	if (Resource != NULL)
	{
		RequestList.SetCount(RequestList.GetCount());
		BlockCache.SetCount(BlockCache.GetCount());
		IOThread.Run(NULL);
	}
	
	Initialized = true;
	
	return true;
}

void ZETerrainBlockCache::Deinitialize()
{
	if (!Initialized)
		return;

	IOThread.Terminate();
	IOThreadSignal.Signal();
	IOThread.Wait();
	
	RequestList.Clear();
	BlockCache.Clear();
}

ZETerrainBlockCache::ZETerrainBlockCache()
{
	Initialized = false;
	Resource = NULL;
	BlockCacheCount = 0;
//	RequestCount = 0;
	RequestList.SetCount(200);
	BlockCache.SetCount(200);
	IOThread.SetFunction(ZEThreadFunction::Create<ZETerrainBlockCache, &ZETerrainBlockCache::IOLoop>(this));
}

ZETerrainBlockCache::~ZETerrainBlockCache()
{
	Deinitialize();
}
