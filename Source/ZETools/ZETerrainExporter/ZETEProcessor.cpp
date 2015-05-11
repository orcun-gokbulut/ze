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
#include "ZETEBlock.h"

#include "ZEDS\ZEFormat.h"
#include "ZEMath\ZEMath.h"
#include "ZEFile\ZEDirectoryInfo.h"

#include <omp.h>


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

bool ZETEProcessor::GenerateBlocks()
{
	printf("Processing Patches.\n");
	printf("Generating Leaf Blocks.\n");

	Info.Progress = 0;

	ZESize BlockSize = PatchDatabase->GetBlockSize();
	ZEUInt64 StartBlockX = ZEMath::Align((ZEInt64)PatchDatabase->GetStartX(), BlockSize);
	ZEUInt64 StartBlockY = ZEMath::Align((ZEInt64)PatchDatabase->GetStartY(), BlockSize);
	ZEUInt64 EndBlockX = ZEMath::Align((ZEInt64)PatchDatabase->GetEndX(), BlockSize);
	ZEUInt64 EndBlockY = ZEMath::Align((ZEInt64)PatchDatabase->GetEndY(), BlockSize);
	ZEUInt64 BlockCountX = (EndBlockX - StartBlockX) / BlockSize + 1;
	ZEUInt64 BlockCountY = (EndBlockX - StartBlockY) / BlockSize + 1;

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

			ZEUInt Progress = (I * 100) / (BlockCountX * BlockCountY);
			if (Progress > Info.Progress)
				Info.Progress = Progress;

			ZEUInt64 IndexX = I % BlockCountX;
			ZEUInt64 IndexY = I / BlockCountX;
			ZEUInt64 PositionX = StartBlockX + IndexX * BlockSize;
			ZEUInt64 PositionY = StartBlockY + IndexY * BlockSize;

			printf("  Progress: %d%% (%lld of %lld). Thread: %d. L: %d, Y: %lld, X: %lld.\n", 
				Info.Progress,
				IndexY * BlockCountX + IndexX,
				BlockCountX * BlockCountY,
				omp_get_thread_num(),
				Block.GetLevel(),
				IndexY * BlockSize,
				IndexX * BlockSize);

			Block.SetPositionX(PositionX);
			Block.SetPositionY(PositionY);
			if (BlockDatabase->CheckBlock(&Block))
			{
				if (Regenerate)
					BlockDatabase->RemoveBlock(&Block);
				else
					continue;
			}

			if (!PatchDatabase->Intersect(PositionX, PositionY, BlockSize, BlockSize, IntersectedPatches, IntersectedPatchCount))
				continue;

			IntersectedPatches.Sort(ComparePatches);
			Block.Clean();

			for (ZESize I = 0; I < IntersectedPatchCount; I++)
				Resampler.Resample(IntersectedPatches[I], &Block);

			if (!BlockDatabase->StoreBlock(&Block))
				Error = true;

			if (DebugDump)
				Block.Dump(ZEFormat::Format("{0}/Dump/L{1}-Y{2}-X{3}.png", BlockDatabase->GetPath(), Block.GetLevel(), Block.GetPositionY(), Block.GetPositionX()));
		}
	}

	if (!Error)
		Info.Progress = 100;

	return !Error;
}

bool ZETEProcessor::GenerateLevel(ZEUInt64 StartX, ZEUInt64 StartY, ZEUInt64 EndX, ZEUInt64 EndY, ZEUInt Level)
{
	Info.Progress = 0;

	printf("Processing Level %d.\n", Level);
	ZEUInt SourceLevelScale = pow(2, Level);
	ZESize SourceLevelBlockSize = SourceLevelScale * BlockDatabase->GetBlockSize();

	ZEUInt DestinationLevelScale = pow(2, Level + 1);
	ZESize DestinationLevelBlockSize = DestinationLevelScale * BlockDatabase->GetBlockSize();

	ZEUInt64 BlockCountX = (BlockDatabase->GetEndX() - BlockDatabase->GetStartX() + DestinationLevelBlockSize - 1) / DestinationLevelBlockSize;
	ZEUInt64 BlockCountY = (BlockDatabase->GetEndY() - BlockDatabase->GetStartY() + DestinationLevelBlockSize - 1) / DestinationLevelBlockSize;
	
	bool Error = false;
	#pragma omp parallel
	{
		ZETEBlock Block00, Block01, Block10, Block11;
		ZETEBlock Output;
	
		Block00.SetLevel(Level);
		Block01.SetLevel(Level);
		Block10.SetLevel(Level);
		Block11.SetLevel(Level);
		Output.SetLevel(Level + 1);
		Output.SetPixelType(BlockDatabase->GetPixelType());
		Output.SetSize(BlockDatabase->GetBlockSize());

		ZETEResamplerIPP Resampler;

		#pragma omp for schedule(dynamic)
		for (ZEInt64 I = 0; I < BlockCountX * BlockCountY; I++)
		{
			ZEUInt Progress = (I * 100) / (BlockCountX * BlockCountY);
			if (Progress > Info.Progress)
				Info.Progress = Progress;

			ZEInt64 IndexX = I % BlockCountX;
			ZEInt64 IndexY = I / BlockCountX;
		
			ZEInt64 PositionX = BlockDatabase->GetStartX() + IndexX * DestinationLevelBlockSize;
			ZEInt64 PositionY = BlockDatabase->GetStartY() + IndexY * DestinationLevelBlockSize;

			printf("  Progress: %d%% (%lld of %lld). Thread: %d, Block: L: %d, Y: %lld, X: %lld.\n", 
				Info.Progress,
				IndexY * BlockCountX + IndexX,
				BlockCountX * BlockCountY,
				omp_get_thread_num(),
				Output.GetLevel(),
				IndexY * DestinationLevelBlockSize,
				IndexX * DestinationLevelBlockSize);

			Output.SetPositionX(PositionX);
			Output.SetPositionY(PositionY);
			if (BlockDatabase->CheckBlock(&Output))
			{
				if (Regenerate)
					BlockDatabase->RemoveBlock(&Output);
				else
					continue;
			}

			bool BlockLoadded = false;
			Block00.SetPositionX(PositionX);
			Block00.SetPositionY(PositionY);
			BlockLoadded |= BlockDatabase->LoadBlock(&Block00);

			Block01.SetPositionX(PositionX + SourceLevelBlockSize);
			Block01.SetPositionY(PositionY);
			BlockLoadded |= BlockDatabase->LoadBlock(&Block01);

			Block10.SetPositionX(PositionX);
			Block10.SetPositionY(PositionY + SourceLevelBlockSize);
			BlockLoadded |= BlockDatabase->LoadBlock(&Block10);

			Block11.SetPositionX(PositionX + SourceLevelBlockSize);
			Block11.SetPositionY(PositionY + SourceLevelBlockSize);
			BlockLoadded |= BlockDatabase->LoadBlock(&Block11);

			if (!BlockLoadded)
				continue;

			Output.Clean();
			Resampler.Downsample(&Output, &Block00, &Block01, &Block10, &Block11);
			BlockDatabase->StoreBlock(&Output);

			if (DebugDump)
				Output.Dump(ZEFormat::Format("{0}/Dump/L{1}-Y{2}-X{3}.png", BlockDatabase->GetPath(), Output.GetLevel(), Output.GetPositionY(), Output.GetPositionX()));
		}
	}

	if (!Error)
		Info.Progress = 100;

	return !Error;
}

bool ZETEProcessor::GenerateLevels()
{
	Info.Status = ZETE_PS_GENERATING_LEVELS;

	ZEUInt MaxLevel = 10;
	ZEUInt MinLevel = PatchDatabase->GetMinLevel();
	ZEUInt MaxLevelX = floor(log((BlockDatabase->GetEndX() - BlockDatabase->GetStartX())) / log(2));
	ZEUInt MaxLevelY = floor(log((BlockDatabase->GetEndY() - BlockDatabase->GetStartY())) / log(2));
	ZEUInt EffectiveMaxLevel = ZEMath::Min(MaxLevel, ZEMath::Min(MaxLevelX, MaxLevelY));

	for (ZEUInt I = MinLevel; I < EffectiveMaxLevel; I++)
		GenerateLevel(BlockDatabase->GetStartX(), BlockDatabase->GetStartY(), BlockDatabase->GetEndX(), BlockDatabase->GetEndY(), I);

	return true;
}

const ZETEProcessorInfo& ZETEProcessor::GetInfo()
{
	return Info;
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

void ZETEProcessor::SetRegenerate(bool Regenerate)
{
	this->Regenerate = Regenerate;
}

bool ZETEProcessor::GetRegenerate()
{
	return Regenerate;
}

void ZETEProcessor::SetDebugDump(bool Enable)
{
	DebugDump = Enable;
}

bool ZETEProcessor::GetDebugDump()
{
	return DebugDump;
}

bool ZETEProcessor::Generate()
{
	memset(&Info, 0, sizeof(ZETEProcessorInfo));

	if (PatchDatabase == NULL || BlockDatabase == NULL)
	{
		Info.Status = ZETE_PS_ERROR;
		return false;
	}

	if (DebugDump)
	{
		ZEDirectoryInfo DumpDirectory(BlockDatabase->GetPath() + "/Dump");
		DumpDirectory.Create();
	}

	if (PatchDatabase->GetBlockSize() != BlockDatabase->GetBlockSize())
	{
		Info.Status = ZETE_PS_ERROR;
		return false;
	}

	if (PatchDatabase->GetPixelType() != BlockDatabase->GetPixelType() || PatchDatabase->GetPixelType() == ZETE_PT_NONE)
	{
		Info.Status = ZETE_PS_ERROR;
		return false;
	}

	Info.TotalProgress = 0;

	if (!GenerateBlocks())
	{
		Info.Status = ZETE_PS_ERROR;
		return false;
	}

	Info.TotalProgress = 50;

	if (!GenerateLevels())
	{
		Info.Status = ZETE_PS_ERROR;
		return false;
	}

	Info.TotalProgress = 100;
	Info.Status = ZETE_PS_DONE;

	return true;
}

ZETEProcessor::ZETEProcessor()
{
	memset(&Info, 0, sizeof(ZETEProcessorInfo));

	Regenerate = false;
	DebugDump = false;

	PatchDatabase = NULL;
	BlockDatabase = NULL;
}
