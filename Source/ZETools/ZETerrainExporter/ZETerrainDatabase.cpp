//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETerrainDatabase.cpp
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
#include "ZETerrainDatabase.h"
#include "ZETerrainPatch.h"
#include "FreeImage.h"
#include "ZEFile/ZEFile.h"
#include "ZETerrainBlock.h"
#include "ZEDS/ZEFormat.h"

void ZETerrainPatchDatabase::CalculateDimensions()
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
		ZETerrainPatch* CurrentPatch = Patches[0];
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

void ZETerrainPatchDatabase::SetBlockType(ZETerrainPixelType Type)
{
	this->PixelType = Type;
}

ZETerrainPixelType ZETerrainPatchDatabase::GetBlockType()
{
	return PixelType;
}

bool ZETerrainPatchDatabase::AddPatch(ZETerrainPatch* Patch)
{
	if (PixelType == ZE_TPT_NONE)
		PixelType = Patch->GetPixelType();

	if (Patch->GetPixelType() != PixelType)
		return false;

	if (Patches.Exists(Patch))
		return true;

	Patches.Add(Patch);
	CalculateDimensions();
}

void ZETerrainPatchDatabase::RemovePatch(ZETerrainPatch* Patch)
{
	Patches.DeleteValue(Patch);
	CalculateDimensions();
}

const ZEArray<ZETerrainPatch*>& ZETerrainPatchDatabase::GetPatches()
{
	return Patches;
}

double ZETerrainPatchDatabase::GetStartX()
{
	return StartX;
}

double ZETerrainPatchDatabase::GetStartY()
{
	return StartY;
}

double ZETerrainPatchDatabase::GetEndX()
{
	return EndX;
}

double ZETerrainPatchDatabase::GetEndY()
{
	return EndY;
}

void ZETerrainPatchDatabase::GetUnitSize(double UnitSize)
{
	this->UnitSize = UnitSize;
}

double ZETerrainPatchDatabase::GetUnitSize()
{
	return UnitSize;
}

void ZETerrainPatchDatabase::SetBlocksPerChunks(ZESize BlocksPerChunk)
{
	this->BlocksPerChunk = BlocksPerChunk;
}

ZESize ZETerrainPatchDatabase::GetBlocksPerChunks()
{
	return BlocksPerChunk;
}

void ZETerrainPatchDatabase::SetBlockSize(ZESize BlockSize)
{
	this->BlockSize = BlockSize;
}

ZESize ZETerrainPatchDatabase::GetBlockSize()
{
	return BlockSize;
}

bool ZETerrainPatchDatabase::CheckBlockAvailable(ZESize Level, double x, double y)
{
	for (ZESize I = 0; I < Patches.GetCount(); I++)
	{
		ZETerrainPatch* CurrentPatch = Patches[I];
		if (CurrentPatch->GetLevel() <= Level &&
			CurrentPatch->BoundaryCheck(x, y))
			return true;
	}

	return false;
}

static ZEInt PartchSortFunction(ZETerrainPatch* const* A, ZETerrainPatch* const * B)
{
	if ((*A)->GetPriority() > (*B)->GetPriority())
		return 1;
	else if ((*A)->GetPriority() < (*B)->GetPriority())
		return -1;
	else
		return 0;
}

void ZETerrainPatchDatabase::UpdateOrder()
{
	Patches.Sort(PartchSortFunction);
}

bool ZETerrainPatchDatabase::SampleBlock(ZETerrainBlock& Output, ZEUInt64 x, ZEUInt64 y, ZEUInt Level)
{
	Output.Create(PixelType, BlockSize);
	Output.SetPositionX(x);
	Output.SetPositionY(y);
	Output.SetLevel(Level);

	for (ZESize py = 0; py < BlockSize; py++)
		for (ZESize px = 0; px < BlockSize; px++)
				SamplePixel(Output.Sample(px, py), x + px + 0.5, y + py + 0.5, Level);
	
	Output.Update();

	return true;
}

bool ZETerrainPatchDatabase::SamplePixel(void* Output, double x, double y, ZEUInt Level)
{
	for (ZESize I = 0; I < Patches.GetCount(); I++)
	{
		if (Patches[I]->BoundaryCheck(x, y))
		{
			Patches[I]->Sample(Output, x, y);
			return true;
		}
	}

	return false;
}

void ZETerrainPatchDatabase::Save(const ZEString& Path)
{
	ZEUInt64 StartXN = (ZEUInt64)GetStartX() - ((ZEUInt64)GetStartX() % BlockSize);
	ZEUInt64 StartYN = (ZEUInt64)GetStartY() - ((ZEUInt64)GetStartY() % BlockSize);

	ZETerrainBlock Block;
	for (ZEUInt Level = 0; Level <= LevelCount; Level++)
	{
		do 
		{
			do 
			{
				if (SampleBlock(Block, StartXN, StartYN, Level))
				{
					ZEFile File;
					File.Open(ZEFormat::Format("{0}\\L{1}-X{2}-Y{3}.zeTerrain", Path, Level, StartXN, StartYN), ZE_FOM_WRITE, ZE_FCM_CREATE);
					Block.Save(&File);
					File.Close();
				}
				StartXN += BlockSize;
			}
			while(StartXN <= GetEndX());
			StartYN += BlockSize;
		}
		while(StartXN <= GetEndX());
	}
}

ZETerrainPatchDatabase::ZETerrainPatchDatabase()
{
	StartX = 0.0;
	StartY = 0.0;
	EndX = 0.0;
	EndY = 0.0;
	UnitSize = 1.0;
	LevelCount = 0;
	BlocksPerChunk = 128;
	BlockSize = 256;
	PixelType = ZE_TPT_NONE;
}

ZETerrainPatchDatabase::~ZETerrainPatchDatabase()
{
	for (ZESize I = 0; I < Patches.GetCount(); I++)
		delete Patches[I];
	Patches.Clear();
}
