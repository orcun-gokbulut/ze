//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETerrainBlock.cpp
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

#include "ZETerrainBlock.h"
#include "ZEError.h"
#include "ZEFile\ZEFile.h"
#include "ZEDS\ZEFormat.h"

struct ZETerrainChunkHeader
{
	ZEUInt64	PositionX;
	ZEUInt64	PositionY;
	ZEUInt8		Level;
	ZEUInt32	Size;
	ZEUInt8		Type;
	float		MinValue;
	float		MaxValue;
};

void ZETerrainBlock::SetState(ZETerrainBlockState State)
{
	this->State = State;
}

bool ZETerrainBlock::GetState()
{
	return State;
}

void ZETerrainBlock::SetPositionX(ZEUInt64 x)
{
	PositionX = x;
}

ZEUInt64 ZETerrainBlock::GetPositionX()
{
	return PositionX;
}

void ZETerrainBlock::SetPositionY(ZEUInt64 y)
{
	PositionY = y;
}

ZEUInt64 ZETerrainBlock::GetPositionY()
{
	return PositionY;
}

ZEUInt64 ZETerrainBlock::GetEndX()
{
	return PositionX + Size * (1 << Level);
}

ZEUInt64 ZETerrainBlock::GetEndY()
{
	return PositionY + Size * (1 << Level);
}

void ZETerrainBlock::SetLevel(ZEUInt Level)
{
	this->Level = Level;
}

ZEUInt ZETerrainBlock::GetLevel()
{
	return Level;
}

ZETerrainPixelType ZETerrainBlock::GetPixelType()
{
	return PixelType;
}

ZESize ZETerrainBlock::GetPixelSize()
{
	return PixelSize;
}

ZEUInt ZETerrainBlock::GetMipmapCount()
{
	return MipmapCount;
}

void* ZETerrainBlock::GetData(ZEUInt MipmapLevel)
{
	ZEUInt8* NormalizedData = (ZEUInt8*)Data;

	for (ZEUInt I = 0; I < MipmapLevel; I++)
		NormalizedData += GetSize(I) * GetPitch(I);
	
	return NormalizedData;
}

ZESize ZETerrainBlock::GetPitch(ZEUInt MipmapLevel)
{
	return Pitch >> MipmapLevel;
}


ZESize ZETerrainBlock::GetSize(ZEUInt MipmapLevel)
{
	return Size >> MipmapLevel;
}

ZESize ZETerrainBlock::GetTotalSize()
{
	return TotalSize;
}


float ZETerrainBlock::GetMinValue()
{
	return MinValue;
}

float ZETerrainBlock::GetMaxValue()
{
	return MaxValue;
}

void ZETerrainBlock::Sample(void* Output, float x, float y)
{
	Output = (ZEUInt8*)Data + (ZESize)y * Size * PixelSize + (ZESize)x * PixelSize;
}

bool ZETerrainBlock::Create(ZETerrainPixelType PixelType, ZESize Size)
{
	if (this->PixelType == PixelType && this->Size == Size)
		return true;

	Clean();

	this->PixelType = PixelType;
	switch(PixelType)
	{
		case ZE_TPT_COLOR:
		case ZE_TPT_ELEVATION:
			PixelSize = 4;
			break;

		case ZE_TPT_GRAYSCALE:
			PixelSize = 1;
			break;

		default:
			Clean();
			return false;
	}

	this->Size = Size;
	ZESize TempSize = Size;
	MipmapCount = 0;
	TotalSize = 0;
	if (PixelType != ZE_TPT_ELEVATION)
	{
		while(TempSize != 0)
		{
			MipmapCount++;
			TotalSize += TempSize * TempSize * PixelSize;
			TempSize = TempSize >> 1;
		}
	}

	this->Pitch = Size * GetPixelSize();
	this->Data = new ZEUInt8[TotalSize];

	return true;
}

void ZETerrainBlock::Clean()
{
	Size = 0;
	TotalSize = 0;
	Pitch = 0;
	PixelType = ZE_TPT_NONE;
	MinValue = 0;
	MaxValue = 0;
	PixelSize = 0;

	if (Data == NULL)
	{
		delete[] (ZEUInt8*)Data;
		Data = NULL;
	}
}

bool ZETerrainBlock::Load(ZEFile* File)
{
	Clean();

	ZETerrainChunkHeader Header = {0};
	if (!File->Read(&Header, sizeof(ZETerrainChunkHeader), 1) == 1)
		return false;
	
	Create((ZETerrainPixelType)Header.Type, Header.Size);

	if (File->Read(GetData(), TotalSize, 1) != 1)
	{
		Clean();
		return false;
	}

	PositionX = Header.PositionX;
	PositionY = Header.PositionY;
	Level = Header.Level;
	MinValue = Header. MinValue;
	MaxValue = Header.MaxValue;

	return true;
}

bool ZETerrainBlock::Save(ZEFile* File)
{
	ZETerrainChunkHeader Header;
	Header.PositionX = PositionX;
	Header.PositionY = PositionY;
	Header.Level = Level;
	Header.Size = Size;
	Header.Type = PixelType;
	Header.MinValue = MinValue;
	Header.MaxValue = MaxValue;

	if (File->Write(&Header, sizeof(ZETerrainChunkHeader), 1) != 1)
		return false;

	if (File->Write(GetData(), TotalSize, 1) != 1)
		return false;

	return true;
}

ZETerrainBlock::ZETerrainBlock()
{
	PositionX = 0;
	PositionY = 0;
	Level = 0;
	State = ZE_TBS_EMPTY;
	Data = NULL;
	Clean();
}

ZETerrainBlock::~ZETerrainBlock()
{
	Clean();
}
