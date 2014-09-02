//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETerrainLayer.cpp
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

#include "ZETerrain.h"
#include "ZETerrainLayer.h"
#include "ZETerrainLevel.h"
#include "ZETerrainResource.h"

ZETerrain2* ZETerrainLayer::GetOwner()
{
	return Owner;
}

const ZEArray<ZETerrainLevel>& ZETerrainLayer::GetLevels()
{
	return Levels;
}

void ZETerrainLayer::SetResource(ZETerrainResource* Resource)
{
	if (this->Resource != Resource)
		return;

	this->Resource = Resource;

	if (Initialized)
	{
		Deinitialize();
		Initialize();
	}
}

ZETerrainResource* ZETerrainLayer::GetResource()
{
	return Resource;
}

void ZETerrainLayer::SetLevelCount(ZEUInt LevelCount)
{
	if (this->LevelCount == LevelCount)
		return;

	this->LevelCount = LevelCount;

	if (Initialized)
	{
		Deinitialize();
		Initialize();
	}
}

ZEUInt ZETerrainLayer::GetLevelCount()
{
	return LevelCount;
}

void ZETerrainLayer::SetPrimitiveScale(float Scale)
{
	if (PrimitiveScale == Scale)
		return;

	PrimitiveScale = Scale;

	if (Initialized)
	{
		Deinitialize();
		Initialize();
	}
}

float ZETerrainLayer::GetPrimitiveScale()
{
	return PrimitiveScale;
}

bool ZETerrainLayer::Initialize()
{
	if (Initialized)
		return true;

	if (Resource != NULL)
	{
		Levels.SetCount(LevelCount);
		for (ZESize I = 0; I < LevelCount; I++)
			Levels[I].Initialize();

		BlockCache.SetResource(Resource);
		BlockCache.Initialize();

		LevelBlockSize = Resource->GetInfo().BlockSize;
		LevelAreaSize = (Owner->GetPrimitiveSize() * 4 + 2) * (1 << LevelOffset);
		LevelBlockCount = ((Owner->GetPrimitiveSize() * 6) * (1 << LevelOffset)) / LevelBlockSize + 1;
		if (LevelBlockCount < 2)
			LevelBlockCount = 2;
		LevelTextureSize = LevelBlockCount * LevelBlockSize;
	}

	Initialized = true;
	return true;
}

void ZETerrainLayer::Deinitialize()
{
	if (!Initialized)
		return;

	Levels.Clear();
	BlockCache.Deinitialize();

	Initialized = false;
}

void ZETerrainLayer::Stream(ZEInt64 PositionX, ZEInt64 PositionY, ZEUInt MinLevel, ZEUInt MaxLevel)
{
	if (!Initialized || Resource == NULL)
		return;

	MaxLevel = MaxLevel > (ZEUInt)Levels.GetCount() - 1 ? (ZEUInt)Levels.GetCount() : MaxLevel;
	for(ZEUInt I = MinLevel; I < MaxLevel; I++)
		Levels[I].Stream(PositionX, PositionY);
}

ZETerrainLayer::ZETerrainLayer()
{
	Initialized = false;
	Owner = NULL;
	Resource = NULL;
	PrimitiveScale = 1.0f;
	LevelCount = 10;
}

ZETerrainLayer::~ZETerrainLayer()
{
	Deinitialize();
}
