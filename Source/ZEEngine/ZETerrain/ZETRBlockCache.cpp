//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETRBlockCache.cpp
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

#include "ZETRBlockCache.h"
#include "ZEDS\ZEFormat.h"

bool ZETRBlockCache::InitializeSelf()
{
	CrawlerThread.SetFunction(ZEThreadFunction::Create<ZETRBlockCache, &ZETRBlockCache::Crawler>(this));
	//CrawlerThread.Run(NULL);

	Blocks.SetCount(256);
	Blocks.Fill(NULL);
	LastBlockSquence = 0;

	return true;
}

void ZETRBlockCache::DeinitializeSelf()
{
	CrawlerSignal.Signal();
	CrawlerThread.Wait();

	for (ZESize I = 0; I < Blocks.GetCount(); I++)
	{
		if (Blocks[I] != NULL)
			delete Blocks[I];
	}
	Blocks.Clear();
	LastBlockSquence = 0;
}

void ZETRBlockCache::SetPath(const ZEString& Path)
{
	this->Path = Path;
}

const ZEString& ZETRBlockCache::GetPath()
{
	return Path;
}

void ZETRBlockCache::LoadBlock(ZETRBlock* Block)
{
	if (Block == NULL || Block->Status != ZETR_BRS_NONE)
		return;
		
	Block->Load(
		ZEFormat::Format("{0}/L{1}-Y{2}-X{3}.zeBlock", 
			Path, 
			Block->GetLevel(), Block->GetIndexY(), Block->GetIndexX()));
}

void ZETRBlockCache::Crawler(ZEThread* Thread, void* ExtraParameters)
{
	/*while(Thread->ControlPoint())
	{
		CrawlerSignal.Wait();*/
		for (ZESize I = 0; I < Blocks.GetCount(); I++)
		{
			/*if (!Thread->ControlPoint())
				return;*/

			LoadBlock(Blocks[I]);
		}
	//}
}

ZETRBlock* ZETRBlockCache::RequestBlock(ZEInt64 IndexX, ZEInt64 IndexY, ZEInt Level)
{
	for (ZESize I = 0; I < Blocks.GetCount(); I++)
	{
		ZETRBlock* CurrentBlock = Blocks[I];
		if (CurrentBlock != NULL &&
			CurrentBlock->GetIndexX() == IndexX &&
			CurrentBlock->GetIndexY() == IndexY &&
			CurrentBlock->GetLevel() == Level)
		{
			LastBlockSquence++;
			CurrentBlock->BlockSquence = LastBlockSquence;
			return CurrentBlock;
		}
	}

	// Scan for free space
	ZESSize NotAvailableBlockIndex = -1;
	ZESize OldestSequenceBlockIndex = 0;
	ZETRBlock** NewBlock = NULL;
	for (ZESize I = 0; I < Blocks.GetCount(); I++)
	{
		ZETRBlock* CurrentBlock = Blocks[I];
		if (CurrentBlock == NULL)
		{
			NewBlock = &Blocks[I];
			break;
		}

		if (CurrentBlock->GetStatus() == ZETR_BRS_NOT_AVAILABLE)
		{
			NotAvailableBlockIndex = I;
		}

		if (Blocks[OldestSequenceBlockIndex]->GetStatus() != ZETR_BRS_LOADING &&
			Blocks[OldestSequenceBlockIndex]->GetBlockSequence() > CurrentBlock->GetBlockSequence())
		{
			OldestSequenceBlockIndex = I;
		}
	}

	// If not free space RECLAIM used space
	if (NewBlock == NULL && NotAvailableBlockIndex != -1)
	{
		delete Blocks[NotAvailableBlockIndex];
		NewBlock = &Blocks[NotAvailableBlockIndex];
	}

	if (NewBlock == NULL)
	{
		delete Blocks[OldestSequenceBlockIndex];
		NewBlock = &Blocks[OldestSequenceBlockIndex];
	}

	LastBlockSquence++;
	*NewBlock = new ZETRBlock();
	(*NewBlock)->BlockSquence = LastBlockSquence;
	(*NewBlock)->IndexX = IndexX;
	(*NewBlock)->IndexY = IndexY;
	(*NewBlock)->Level = Level;
	
	//CrawlerSignal.Signal();
	Crawler(&CrawlerThread, NULL);

	return *NewBlock;
}

void ZETRBlockCache::Clean()
{
	for (ZESize I = 0; I < Blocks.GetCount(); I++)
	{
		delete Blocks[I];
		Blocks[I] = NULL;
	}
}
