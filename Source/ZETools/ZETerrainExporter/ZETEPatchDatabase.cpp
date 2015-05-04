//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETEPatchDatabase.cpp
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


#include "ZEDS/ZEArray.h"
#include "ZEFile/ZEFile.h"
#include "ZEDS/ZEFormat.h"
#include "ZETEPatch.h"
#include "ZETEPatchDatabase.h"
#include "ZETEBlock.h"

#include <FreeImage.h>
#include "ZETEResamplerIPP.h"

void ZETEPatchDatabase::CalculateDimensions()
{
	if (Patches.GetCount() == 0)
	{
		StartX = 0;
		StartY = 0;
		EndX = 0;
		EndY = 0;
		LevelCount = 0;
	}
	else
	{
		ZETEPatch* CurrentPatch = Patches[0];
		StartX = CurrentPatch->GetStartX();
		StartY = CurrentPatch->GetStartY();
		EndX = CurrentPatch->GetEndX();
		EndY = CurrentPatch->GetEndY();
		LevelCount = CurrentPatch->GetLevel();

		for (ZESize I = 1; I < Patches.GetCount(); I++)
		{
			CurrentPatch = Patches[I];

			if (StartX > CurrentPatch->GetStartX())
				StartX = CurrentPatch->GetStartX();

			if (StartY > CurrentPatch->GetStartY())
				StartY = CurrentPatch->GetStartY();

			if (EndX < CurrentPatch->GetEndX())
				EndX = CurrentPatch->GetEndX();

			if (EndY < CurrentPatch->GetEndY())
				EndY = CurrentPatch->GetEndY();

			if (LevelCount > CurrentPatch->GetLevel())
				LevelCount = CurrentPatch->GetLevel();
		}
	}
}

void ZETEPatchDatabase::SetBlockType(ZETEPixelType Type)
{
	this->PixelType = Type;
}

ZETEPixelType ZETEPatchDatabase::GetBlockType()
{
	return PixelType;
}

bool ZETEPatchDatabase::AddPatch(ZETEPatch* Patch)
{
	if (PixelType == ZETE_PT_NONE)
		PixelType = Patch->GetPixelType();

	if (Patch->GetPixelType() != PixelType)
		return false;

	if (Patches.Exists(Patch))
		return true;

	Patches.Add(Patch);
	Patch->Database = this;

	CalculateDimensions();

	return true;
}

void ZETEPatchDatabase::RemovePatch(ZETEPatch* Patch)
{
	Patches.RemoveValue(Patch);
	Patch->Database = NULL;
	CalculateDimensions();
}

const ZEArray<ZETEPatch*>& ZETEPatchDatabase::GetPatches()
{
	return Patches;
}

double ZETEPatchDatabase::GetStartX()
{
	return StartX;
}

double ZETEPatchDatabase::GetStartY()
{
	return StartY;
}

double ZETEPatchDatabase::GetEndX()
{
	return EndX;
}

double ZETEPatchDatabase::GetEndY()
{
	return EndY;
}

void ZETEPatchDatabase::GetUnitSize(double UnitSize)
{
	this->UnitSize = UnitSize;
}

double ZETEPatchDatabase::GetUnitSize()
{
	return UnitSize;
}

void ZETEPatchDatabase::SetBlockSize(ZESize BlockSize)
{
	this->BlockSize = BlockSize;
}

ZESize ZETEPatchDatabase::GetBlockSize()
{
	return BlockSize;
}

void ZETEPatchDatabase::SetPath(const ZEString& Path)
{
	this->Path = Path;
}

const ZEString& ZETEPatchDatabase::GetPath()
{
	return Path;
}

bool ZETEPatchDatabase::Intersect(double PositionX, double PositionY, double Width, double Height, ZEArray<ZETEPatch*>& Output, ZESize& OutputSize)
{
	OutputSize = 0;
	for (ZESize I = 0; I < Patches.GetCount(); I++)
	{
		ZETEPatch* CurrentPatch = Patches[I];
		if (CurrentPatch->Intersect(PositionX, PositionY, Width, Height))
		{
			Output[OutputSize] = CurrentPatch;
			OutputSize++;
		}
	}

	return (OutputSize != 0);
}

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

void ZETEPatchDatabase::GenerateBlocks()
{
	ZEUInt64 BlockCountX = (ZEUInt64)(EndX - StartX) / BlockSize;
	ZEUInt64 BlockCountY = (ZEUInt64)(EndY - StartY) / BlockSize;

	#pragma omp parallel
	{
		ZETEBlock Block;
		Block.SetSize(BlockSize);
		Block.SetPixelType(ZETE_PT_COLOR);

		ZEArray<ZETEPatch*> IntersectedPatches;
		ZESize IntersectedPatchCount = 0;
		IntersectedPatches.SetCount(this->Patches.GetCount());

		ZETEResamplerIPP Resampler;
		
		#pragma omp for schedule(dynamic)
		for (ZEInt64 I = 0; I < BlockCountX * BlockCountY; I++)
		{
			ZEUInt64 IndexX = I % BlockCountX;
			ZEUInt64 IndexY = I / BlockCountX;

			printf("Processing Block %lld of %lld. (X: %lld, Y: %lld, L: %lld) \n", 
				IndexY * BlockCountX + IndexX,
				BlockCountX * BlockCountY,
				IndexX * BlockSize,
				IndexY * BlockSize,
				0);

			if (!Intersect(IndexX * BlockSize, IndexY * BlockSize, BlockSize, BlockSize, IntersectedPatches, IntersectedPatchCount))
				continue;

			IntersectedPatches.Sort(ComparePatches);
			Block.SetPositionX(IndexX * BlockSize);
			Block.SetPositionY(IndexY * BlockSize);
			Block.Clean();

			for (ZESize I = 0; I < IntersectedPatchCount; I++)
				Resampler.Resample(IntersectedPatches[I], &Block);

			Block.Save(ZEFormat::Format("{0}/Y{1}-X{2}-Z{3}.zeBlock", Path, Block.GetPositionY(), Block.GetPositionX(), Block.GetLevel()));
		}

	}
}

ZETEPatchDatabase::ZETEPatchDatabase()
{
	StartX = 0.0;
	StartY = 0.0;
	EndX = 0.0;
	EndY = 0.0;
	UnitSize = 1.0;
	LevelCount = 0;
	BlocksPerChunk = 128;
	BlockSize = 256;
	PixelType = ZETE_PT_NONE;
}

ZETEPatchDatabase::~ZETEPatchDatabase()
{
	for (ZESize I = 0; I < Patches.GetCount(); I++)
		delete Patches[I];

	Patches.Clear();
}
