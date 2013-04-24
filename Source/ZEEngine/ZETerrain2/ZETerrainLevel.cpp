//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETerrainLevel.cpp
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
#include "ZETerrainLevel.h"
#include "ZETerrainLayer.h"
#include "ZETerrainResource.h"
#include "ZEGraphics\ZETexture2D.h"

ZETerrainLayer* ZETerrainLevel::GetOwner()
{
	return Owner;
}

ZETexture2D* ZETerrainLevel::GetTexture()
{
	return Texture;
}

const ZEMatrix4x4& ZETerrainLevel::GetTexcoordMatrix()
{
	return TexcoordMatrix;
}

float ZETerrainLevel::GetMinHeight()
{
	return MinHeight;
}

float ZETerrainLevel::GetMaxHeight()
{
	return MaxHeight;
}

ZESize ZETerrainLevel::ConvertToBlockCoords(ZEInt64 Value)
{
	return (Value / (1 << Level) / Owner->LevelBlockSize);
}

ZESize ZETerrainLevel::ConvertToBlockIndex(ZEInt64 PositionX, ZEInt64 PositionY)
{
	return (ConvertToBlockCoords(PositionY)  % Owner->LevelBlockCount) * Owner->LevelBlockCount + ConvertToBlockCoords(PositionX) % Owner->LevelBlockCount;
}

void ZETerrainLevel::CopyBlockToTexture(ZETerrainBlock* Block)
{
	for (ZESize I = 0; I < Block->GetMipmapCount(); I++)
	{
		void* Buffer = NULL;
		ZESize Pitch = 0;
		Texture->Lock(&Buffer, &Pitch, I,
			(ConvertToBlockCoords(PositionX) * Owner->LevelBlockSize) / (1 << I),
			(ConvertToBlockCoords(PositionY) * Owner->LevelBlockSize) / (1 << I),
			Owner->LevelBlockSize  / (1 << I), 
			Owner->LevelBlockSize  / (1 << I));

		memcpy(Buffer, Block->GetData(Level), Owner->LevelBlockSize / (1 << I) * Block->GetPixelSize());
		Texture->Unlock(I);
	}
}

void ZETerrainLevel::UpdateLoadingBlocks()
{
	for (ZESize I = 0; I < Blocks.GetCount(); I++)
	{
		ZETerrainBlock* Block = &Blocks[I];
		if (Block->GetState() == ZE_TBS_LOADING)
		{
			Owner->BlockCache.GetBlock(Block, Block->GetLevel(), Block->GetPositionX(), Block->GetPositionY());
			if (Block->GetState() == ZE_TBS_LOADING)
				continue;
			CopyBlockToTexture(Block);
		}
	}
}

bool ZETerrainLevel::Initialize()
{
	if (Initialized)
		return true;
	
	Blocks.SetCount(Owner->LevelBlockCount * Owner->LevelBlockCount);
	for (ZESize I = 0; I < Blocks.GetCount(); I++)
		Blocks[I].SetState(ZE_TBS_EMPTY);

	Texture = ZETexture2D::CreateInstance();
	ZETexturePixelFormat Format = ZE_TPF_NOTSET;
	switch(Owner->Resource->GetInfo().PixelType)
	{
		case ZE_TPT_COLOR:
			Format = ZE_TPF_I8_4;
			break;

		case ZE_TPT_ELEVATION:
			Format = ZE_TPF_F32;
			break;

		case ZE_TPT_GRAYSCALE:
			Format = ZE_TPF_I8;
			break;
	}

	if (!Texture->Create(Owner->LevelTextureSize, Owner->LevelTextureSize, Owner->Resource->GetInfo().MipmapCount, Format))
		return false;

	Initialized = true;

	return true;
}

void ZETerrainLevel::Deinitialize()
{
	if (!Initialized)
		return;

	Blocks.Clear();
	if (Texture != NULL)
	{
		Texture->Destroy();
		Texture = NULL;
	}

	Initialized = false;
}

static ZEInt TextureWrap(ZEInt Value, ZEUInt Size)
{
	ZEInt Modulus = Value % Size;

	if(Modulus < 0)
		Modulus += Size;

	return Modulus;
}

void ZETerrainLevel::Stream(ZEInt64 PositionX, ZEUInt64 PositionY)
{
	PositionX = ZEMath::Align(PositionX, LevelScale);
	PositionY = ZEMath::Align(PositionY, LevelScale);

	if (this->PositionX == PositionX || this->PositionY == PositionY)
		return;

	this->PositionX = PositionX;
	this->PositionY = PositionY;

	float MinHeight = ZE_FLOAT_MAX;
	float MaxHeight = ZE_FLOAT_MIN;

	for (ZEInt64 x = PositionX - (Owner->LevelAreaSize / 2) * LevelScale; x < PositionX + Owner->LevelAreaSize * LevelScale; x += Owner->LevelBlockSize * LevelScale)
	{
		for (ZEInt64 y = PositionX - (Owner->LevelAreaSize / 2) * LevelScale; y < PositionX + Owner->LevelAreaSize * LevelScale; y += Owner->LevelBlockSize * LevelScale)
		{
			ZEInt64 BlockAlignedX = ZEMath::Align(x, Owner->LevelBlockSize * LevelScale);
			ZEInt64 BlockAlignedY = ZEMath::Align(y, Owner->LevelBlockSize * LevelScale);

			ZETerrainBlock* Block = &Blocks[ConvertToBlockIndex(BlockAlignedX, BlockAlignedY)];
			if (Block->GetState() == ZE_TBS_EMPTY || BlockAlignedX != Block->GetPositionX() || BlockAlignedY != Block->GetPositionY())
			{
				Owner->BlockCache.GetBlock(Block, Level, BlockAlignedX, BlockAlignedY);
				CopyBlockToTexture(Block);

				if (Block->GetMaxValue() > MaxHeight)
					MaxHeight = Block->GetMaxValue();
				if (Block->GetMinValue() < MinHeight)
					MinHeight = Block->GetMinValue();
			}
		}
	}

	UpdateLoadingBlocks();
}

ZETerrainLevel::ZETerrainLevel()
{
	Texture = NULL;
}

ZETerrainLevel::~ZETerrainLevel()
{
	Deinitialize();
}
