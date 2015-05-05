//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETEProcessor.cpp
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

#include "ZETEProcessor.h"
#include "ZETEResamplerIPP.h"
#include "ZETEPatch.h"

#include <ippj.h>

static int ComparePatches(ZETEPatch* const * Ap, ZETEPatch* const* Bp)
{
	ZETEPatch* A = *Ap;
	ZETEPatch* B = *Bp;

	if (A->GetLevel() > B->GetLevel())
	{
		return 1;
	}
	else if (A->GetLevel() < B->GetLevel())
	{
		return -1;
	}
	else
	{
		if (A->GetPriority() < B->GetPriority())
			return 1;
		else if (A->GetPriority() > B->GetPriority())
			return -1;
		else
			return 0;
	}
}

void ZETEProcessor::SetPatchDatabase(ZETEPatchDatabase* Database)
{
	PatchDatabase = Database;
}

ZETEPatchDatabase* ZETEProcessor::GetPatchDatabase()
{
	return PatchDatabase;
}

void ZETEProcessor::SetBlockDatabase(ZETEBlockDatabase* Database)
{
	BlockDatabase = Database;
}

ZETEBlockDatabase* ZETEProcessor::GetBlockDatabase()
{
	return BlockDatabase;
}

bool ZETEProcessor::Generate()
{
	if (PatchDatabase == NULL || BlockDatabase == NULL)
		return false;

	if (PatchDatabase->GetBlockSize() != BlockDatabase->GetBlockSize())
		return false;

	if (PatchDatabase->GetBlockType() != BlockDatabase->GetBlockType() || PatchDatabase->GetBlockType() == ZETE_PT_NONE)
		return false;

	ZESize BlockSize = PatchDatabase->GetBlockSize();
	ZEUInt64 BlockCountX = (ZEUInt64)(PatchDatabase->GetEndX() - PatchDatabase->GetStartX()) / BlockSize;
	ZEUInt64 BlockCountY = (ZEUInt64)(PatchDatabase->GetEndY() - PatchDatabase->GetStartY()) / BlockSize;

	bool Error = false;
	#pragma omp parallel
	{
		ZETEBlock Block;
		Block.SetSize(PatchDatabase->GetBlockSize());
		Block.SetPixelType(ZETE_PT_COLOR);

		ZEArray<ZETEPatch*> IntersectedPatches;
		ZESize IntersectedPatchCount = 0;
		IntersectedPatches.SetCount(PatchDatabase->GetPatches().GetCount());

		ZETEResamplerIPP Resampler;
		
		#pragma omp for schedule(dynamic)
		for (ZEInt64 I = 0; I < BlockCountX * BlockCountY; I++)
		{
			if (Error)
				continue;

			ZEUInt64 IndexX = I % BlockCountX;
			ZEUInt64 IndexY = I / BlockCountX;

			printf("Processing Block %lld of %lld. (X: %lld, Y: %lld, L: %lld) \n", 
				IndexY * BlockCountX + IndexX,
				BlockCountX * BlockCountY,
				IndexX * BlockSize,
				IndexY * BlockSize,
				0);

			if (!PatchDatabase->Intersect(IndexX * BlockSize, IndexY * BlockSize, BlockSize, BlockSize, IntersectedPatches, IntersectedPatchCount))
				continue;

			IntersectedPatches.Sort(ComparePatches);
			Block.SetPositionX(IndexX * BlockSize);
			Block.SetPositionY(IndexY * BlockSize);
			Block.Clean();

			for (ZESize I = 0; I < IntersectedPatchCount; I++)
				Resampler.Resample(IntersectedPatches[I], &Block);

			if (!BlockDatabase->StoreBlock(&Block))
				Error = true;
		}
	}

	return Error;
}

ZETEProcessor::ZETEProcessor()
{
	PatchDatabase = NULL;
	BlockDatabase = NULL;
}
