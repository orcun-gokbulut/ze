//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETRLevel.cpp
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

#include "ZETRLevel.h"
#include "ZETRLayer.h"

#include "ZEGraphics\ZEGRTexture2D.h"
#include "ZEMath\ZEMath.h"
#include "ZERenderer\ZETerrainMaterial.h"

#define ZETR_LEVEL_BLOCK_COUNT 3

ZETRLevelBlock::ZETRLevelBlock()
{
	IndexX = 0;
	IndexY = 0;
	LastStatus = ZETR_BRS_NONE;
	Cleaned = false;
}

void ZETRLevel::CleanBlock(ZESize LocalIndexX, ZESize LocalIndexY)
{
	ZESize BlockSize = Layer->GetBlockSize();

	void* DestBuffer;
	ZESize DestPitch;
	ZESize Pitch = 0;
	if (Layer->GetPixelType() == ZETR_PT_ELEVATION)
		Pitch = sizeof(float) * Layer->GetBlockSize();
	else if (Layer->GetPixelType() == ZETR_PT_COLOR)
		Pitch = 4 * Layer->GetBlockSize();
	else if (Layer->GetPixelType() == ZETR_PT_GRAYSCALE)
		Pitch = Layer->GetBlockSize();

	Texture->Lock(&DestBuffer, &DestPitch, 0, LocalIndexX * BlockSize, LocalIndexY * BlockSize, BlockSize, BlockSize);
	for (ZESize I = 0; I < BlockSize; I++)
		memset((ZEBYTE*)DestBuffer + I * DestPitch, 0, Pitch);

	Texture->Unlock(0);
}

void ZETRLevel::LoadBlock(ZESize LocalIndexX, ZESize LocalIndexY, ZETRBlock* Block)
{
	ZESize BlockSize = Layer->GetBlockSize();
	ZEBYTE* SourceBuffer = (ZEBYTE*)Block->GetData();
	ZESize SourcePitch = Block->GetPitch();

	void* DestBuffer;
	ZESize DestPitch;
	Texture->Lock(&DestBuffer, &DestPitch, 0, LocalIndexX * BlockSize, LocalIndexY * BlockSize, BlockSize, BlockSize);

	if (Layer->GetPixelType() == ZETR_PT_ELEVATION)
	{
		for (ZESize Y = 0; Y < BlockSize; Y++)
		{
			float* DestValue = (float*)((ZEBYTE*)DestBuffer + Y * DestPitch);
			ZEUInt16* SrcValue = (ZEUInt16*)((ZEBYTE*)SourceBuffer + Y * SourcePitch);
			for (ZESize X = 0; X < BlockSize; X++)
				DestValue[X] = SrcValue[X];
		}
	}
	else
	{
		for (ZESize I = 0; I < BlockSize; I++)
			memcpy((ZEBYTE*)DestBuffer + I * DestPitch, SourceBuffer + I * SourcePitch, SourcePitch);
	}
	
	Texture->Unlock(0);
}

void ZETRLevel::ProcessBlock(ZESSize IndexX, ZESSize IndexY)
{
	int LocalIndexX = ZEMath::CircularMod(IndexX, (ZESSize)GetBlockCount());
	int LocalIndexY = ZEMath::CircularMod(IndexY, (ZESSize)GetBlockCount());

	ZETRLevelBlock* Block = &Blocks[LocalIndexX][LocalIndexY];
	if (Block->IndexX != IndexX ||
		Block->IndexY != IndexY)
	{
		Block->IndexX = IndexX;
		Block->IndexY = IndexY;
		Block->LastStatus = ZETR_BRS_NONE;
		Block->Cleaned = false;
	}
	
	if (Block->LastStatus == ZETR_BRS_AVAILABLE || Block->LastStatus == ZETR_BRS_NOT_AVAILABLE)
	{
		return;
	}
	else if (Block->LastStatus == ZETR_BRS_NONE || Block->LastStatus == ZETR_BRS_LOADING)
	{
		ZETRBlock* NewBlock = Layer->GetBlockCache()->RequestBlock(IndexX, IndexY, Level);
		Block->LastStatus = NewBlock->GetStatus();
		if (Block->LastStatus == ZETR_BRS_AVAILABLE)
		{
			LoadBlock(LocalIndexX, LocalIndexY, NewBlock);
		}
		else
		{
			if (Block->Cleaned)
				return;

			CleanBlock(LocalIndexX, LocalIndexY);
			Block->Cleaned = true;
		}
	}
}

void ZETRLevel::SetLevel(ZEInt Level)
{
	this->Level = Level;
	LevelScale = ZEMath::Power(2, Level);
	LevelBlockSize = LevelScale * Layer->GetBlockSize();
}

bool ZETRLevel::InitializeSelf()
{
	if (Layer == NULL || Layer->GetPixelType() == ZETR_PT_NONE)
		return false;

	ZESize BlockSize = Layer->GetBlockSize();
	Texture = ZEGRTexture2D::CreateInstance();
	bool Result = false;
	
	switch (Layer->GetPixelType())
	{
		case ZETR_PT_ELEVATION:
			Result = Texture->Create(BlockSize * GetBlockCount(), BlockSize * GetBlockCount(), 1, ZEGR_TF_R32F, false);
			break;

		case ZETR_PT_COLOR:
			Result = Texture->Create(BlockSize * GetBlockCount(), BlockSize * GetBlockCount(), 1, ZEGR_TF_R8G8B8A8, false);
			break;

		case ZETR_PT_GRAYSCALE:
			Result = Texture->Create(BlockSize * GetBlockCount(), BlockSize * GetBlockCount(), 1, ZEGR_TF_R8, false);
			break;

		default:
			break;
	}

	if (!Result)
	{
		Texture->Destroy();
		Texture = NULL;
		return false;
	}

	return true;
}

void ZETRLevel::DeinitializeSelf()
{
	if (Texture != NULL)
	{
		Texture->Destroy();
		Texture = NULL;
	}
}

ZETRLayer* ZETRLevel::GetLayer()
{
	return Layer;
}

ZEInt ZETRLevel::GetLevel()
{
	return Level;
}

float ZETRLevel::GetLevelScale()
{
	return LevelScale;
}

float ZETRLevel::GetLevelBlockSize()
{
	return LevelBlockSize;
}

ZESize  ZETRLevel::GetBlockCount()
{
	return ZETR_LEVEL_BLOCK_COUNT;
}

ZEGRTexture2D* ZETRLevel::GetTexture()
{
	return Texture;
}

void ZETRLevel::Process()
{
	ZEVector3 Position = Layer->GetViewPosition();

	ZEInt64 IndexX = ZEMath::Floor(Position.x / LevelBlockSize);
	ZEInt64 IndexY = ZEMath::Floor(Position.z / LevelBlockSize);

	ProcessBlock(IndexX - 1, IndexY + 1);
	ProcessBlock(IndexX,	 IndexY + 1);
	ProcessBlock(IndexX + 1, IndexY + 1);

	ProcessBlock(IndexX - 1, IndexY);
	ProcessBlock(IndexX,	 IndexY);
	ProcessBlock(IndexX + 1, IndexY);

	ProcessBlock(IndexX - 1, IndexY - 1);
	ProcessBlock(IndexX,	 IndexY - 1);
	ProcessBlock(IndexX + 1, IndexY - 1);
}

ZETRLevel::ZETRLevel()
{
	Level = 0;
	Layer = NULL;
	Texture = NULL;
}
