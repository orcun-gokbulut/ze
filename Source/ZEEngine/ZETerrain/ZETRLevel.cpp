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

#include "ZEGraphics\ZETexture2D.h"
#include "ZEMath\ZEMath.h"

#define ZETR_LEVEL_BLOCK_COUNT 3

ZETRLevelBlock::ZETRLevelBlock()
{
	IndexX = 0;
	IndexY = 0;
	LastStatus = ZETR_BRS_NONE;
	Cleaned = false;
}

void ZETRLevel::CalculateTextureTransform()
{
	TextureTransform = ZEMatrix3x3::Identity;
}

void ZETRLevel::ProcessBlock(ZESSize IndexX, ZESSize IndexY)
{
	int LocalIndexX = ZEMath::CircularMod(IndexX, (ZESSize)ZETR_LEVEL_BLOCK_COUNT);
	int LocalIndexY = ZEMath::CircularMod(IndexY, (ZESSize)ZETR_LEVEL_BLOCK_COUNT);

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
			ZESize BlockSize = Layer->GetBlockSize();
			ZEBYTE* SourceBuffer = (ZEBYTE*)NewBlock->GetData();
			ZESize SourcePitch = NewBlock->GetPitch();

			void* DestBuffer;
			ZESize DestPitch;
			Texture->Lock(&DestBuffer, &DestPitch, 0, LocalIndexX * BlockSize, LocalIndexY * BlockSize, BlockSize, BlockSize);
				for (ZESize I = 0; I < BlockSize; I++)
					memcpy((ZEBYTE*)DestBuffer + I * DestPitch, SourceBuffer + I * SourcePitch, SourcePitch);
			Texture->Unlock(0);
		}
		else
		{
			if (Block->Cleaned)
				return;

			ZESize BlockSize = Layer->GetBlockSize();
			ZESize SourcePitch = BlockSize * NewBlock->GetPixelSize();

			void* DestBuffer;
			ZESize DestPitch;
			Texture->Lock(&DestBuffer, &DestPitch, 0, LocalIndexX * BlockSize, LocalIndexY * BlockSize, BlockSize, BlockSize);
				for (ZESize I = 0; I < BlockSize; I++)
					memset((ZEBYTE*)DestBuffer + I * DestPitch, 0, SourcePitch);
			Texture->Unlock(0);

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
	if (Layer == NULL)
		return false;

	ZESize BlockSize = Layer->GetBlockSize();
	Texture = ZETexture2D::CreateInstance();
	if (!Texture->Create(BlockSize * ZETR_LEVEL_BLOCK_COUNT, BlockSize * ZETR_LEVEL_BLOCK_COUNT, 0, ZE_TPF_RGBA8, false))
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

ZETexture2D* ZETRLevel::GetTexture()
{
	return Texture;
}

const ZEMatrix3x3& ZETRLevel::GetTextureTransform()
{
	return TextureTransform;
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

	CalculateTextureTransform();
}

ZETRLevel::ZETRLevel()
{
	Level = 0;
	Layer = NULL;
	Texture = NULL;
}
