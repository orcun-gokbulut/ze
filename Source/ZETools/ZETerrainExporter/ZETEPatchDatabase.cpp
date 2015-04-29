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
	if (PixelType == ZE_TPT_NONE)
		PixelType = Patch->GetPixelType();

	if (Patch->GetPixelType() != PixelType)
		return false;

	if (Patches.Exists(Patch))
		return true;

	Patches.Add(Patch);
	CalculateDimensions();
}

void ZETEPatchDatabase::RemovePatch(ZETEPatch* Patch)
{
	Patches.DeleteValue(Patch);
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

void ZETEPatchDatabase::SetBlocksPerChunks(ZESize BlocksPerChunk)
{
	this->BlocksPerChunk = BlocksPerChunk;
}

ZESize ZETEPatchDatabase::GetBlocksPerChunks()
{
	return BlocksPerChunk;
}

void ZETEPatchDatabase::SetBlockSize(ZESize BlockSize)
{
	this->BlockSize = BlockSize;
}

ZESize ZETEPatchDatabase::GetBlockSize()
{
	return BlockSize;
}

bool ZETEPatchDatabase::CheckBlockAvailable(ZESize Level, double x, double y)
{
	for (ZESize I = 0; I < Patches.GetCount(); I++)
	{
		ZETEPatch* CurrentPatch = Patches[I];
		if (CurrentPatch->GetLevel() <= Level &&
			CurrentPatch->BoundaryCheck(x, y))
			return true;
	}

	return false;
}

static ZEInt PartchSortFunction(ZETEPatch* const* A, ZETEPatch* const * B)
{
	if ((*A)->GetPriority() > (*B)->GetPriority())
		return 1;
	else if ((*A)->GetPriority() < (*B)->GetPriority())
		return -1;
	else
		return 0;
}

void ZETEPatchDatabase::UpdateOrder()
{
	Patches.Sort(PartchSortFunction);
}

struct ZETerrainBlockIndex
{
	ZEUInt			Level;
	ZEUInt64		PositionX;
	ZEUInt64		PositionY;
	ZEUInt64		Offset;
};

struct ZETerrainBlockInfo
{
	ZESize						Size;
	ZETEPixelType			PixelType;
	ZESize						PixelSize;
	ZESize						MipmapCount;
};

static ZEInt TerrainIndexCompare(const ZETerrainBlockIndex* A, const ZETerrainBlockIndex* B)
{
	if (A->Level > B->Level)
	{
		return 1;
	}
	else if (A->Level < B->Level)
	{
		return -1;
	}
	else
	{
		if (A->PositionX > B->PositionX)
		{
			return 1;
		}
		else if (A->PositionX < B->PositionX)
		{
			return -1;
		}
		else
		{
			if (A->PositionY > B->PositionY)
				return 1;
			else if (A->PositionY < B->PositionY)
				return -1;
			else
				return 0;
		}
	}
}

void ZETEPatchDatabase::Save(const ZEString& Path)
{
	ZEArray<ZETerrainBlockIndex> Indexes;
	
	ZEFile DataFile;
	DataFile.Open(Path + ".Data.ZETerrain", ZE_FOM_WRITE, ZE_FCM_CREATE);
	
	ZESize MipmapCount = 1;
	ZETEBlock Block;
		
	ZEUInt64 StartXN = (ZEUInt64)GetStartX() - ((ZEUInt64)GetStartX() % BlockSize);
	ZEUInt64 StartYN = (ZEUInt64)GetStartY() - ((ZEUInt64)GetStartY() % BlockSize);

	for (ZEUInt Level = 0; Level <= LevelCount; Level++)
	{
		for (ZEUInt64 YN = StartYN; YN < GetEndY(); YN += BlockSize)
		{
			for (ZEUInt64 XN = StartXN; XN < GetEndX(); XN += BlockSize)
			{
				//if (SampleBlock(Block, XN, YN, Level))
				{
					ZETerrainBlockIndex Index;

					Index.PositionX = XN;
					Index.PositionY = YN;
					Index.Level = Level;
					Index.Offset = DataFile.Tell();
					Indexes.Add(Index);
					Block.Save(&DataFile);
				}
			}
		}
	}
	DataFile.Close();

	Indexes.Sort(TerrainIndexCompare);

	ZEFile IndexFile;
	IndexFile.Open(Path + ".Index.ZETerrain", ZE_FOM_WRITE, ZE_FCM_CREATE);
	
	ZETerrainBlockInfo Info;
	Info.PixelType = PixelType;
	Info.Size = BlockSize;
	Info.MipmapCount = MipmapCount;
	switch(PixelType)
	{
		case ZE_TPT_COLOR:
		case ZE_TPT_ELEVATION:
			Info.PixelSize = 4;
			break;

		case ZE_TPT_GRAYSCALE:
			Info.PixelSize = 1;
			break;
	}

	IndexFile.Write(&Info, sizeof(ZETerrainBlockInfo), 1);
	
	ZEUInt32 IndexCount = Indexes.GetCount();
	IndexFile.Write(&IndexCount, sizeof(IndexCount), 1);
	IndexFile.Write(Indexes.GetCArray(), sizeof(ZETerrainBlockIndex), IndexCount);
	IndexFile.Close();
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
	PixelType = ZE_TPT_NONE;
}

ZETEPatchDatabase::~ZETEPatchDatabase()
{
	for (ZESize I = 0; I < Patches.GetCount(); I++)
		delete Patches[I];
	Patches.Clear();
}
