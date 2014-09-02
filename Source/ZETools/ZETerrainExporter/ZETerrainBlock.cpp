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
#include "FreeImage.h"
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

static void DownSample(float* Destination, float* Source, ZESize Size)
{
	ZESize HalfSize = Size / 2;
	for (ZESize y = 0; y < HalfSize; y++)
	{
		float* SourceRowOdd = Source + Size * 2 * y;
		float* SourceRowEven = Source + Size * (2 * y + 1);
		float* DestinationRow = Destination + HalfSize * y;

		for (ZESize x = 0; x < HalfSize; x++)
			DestinationRow[x] = (SourceRowOdd[2 * x] + SourceRowOdd[2 * x + 1] + SourceRowEven[2 * x] + SourceRowEven[2 * x + 1]) /4;
	}
}

static void DownSample(ZEUInt32* Destination, ZEUInt32* Source, ZESize Size)
{
	ZESize HalfSize = Size / 2;
	for (ZESize y = 0; y < HalfSize; y++)
	{
		ZEUInt32* SourceRowOdd = Source + Size * 2 * y;
		ZEUInt32* SourceRowEven = Source + Size * (2 * y + 1);
		ZEUInt32* DestinationRow = Destination + HalfSize * y;

		for (ZESize x = 0; x < HalfSize; x++)
		{
			ZEUInt8* P0 = (ZEUInt8*)&SourceRowOdd[2 * x];
			ZEUInt8* P1 = (ZEUInt8*)&SourceRowOdd[2 * x + 1];
			ZEUInt8* P2 = (ZEUInt8*)&SourceRowEven[2 * x];
			ZEUInt8* P3 = (ZEUInt8*)&SourceRowEven[2 * x + 1];
			ZEUInt8* PD = (ZEUInt8*)&DestinationRow[x];

			PD[0] = ((ZEUInt)P0[0] + (ZEUInt)P1[0] + (ZEUInt)P2[0] + (ZEUInt)P3[0]) /4;
			PD[1] = ((ZEUInt)P0[1] + (ZEUInt)P1[1] + (ZEUInt)P2[1] + (ZEUInt)P3[1]) /4;
			PD[2] = ((ZEUInt)P0[2] + (ZEUInt)P1[2] + (ZEUInt)P2[2] + (ZEUInt)P3[2]) /4;
			PD[3] = ((ZEUInt)P0[3] + (ZEUInt)P1[3] + (ZEUInt)P2[3] + (ZEUInt)P3[3]) /4;
		}
	}
}

void ZETerrainBlock::CalculateMipmaps()
{
	ZEUInt CurrentLevel = 0;
	ZEUInt NextLevel = 1;

	while(GetSize(CurrentLevel) != 0)
	{
		switch(PixelType)
		{
			case ZE_TPT_COLOR:
				DownSample((ZEUInt32*)GetData(CurrentLevel + 1), (ZEUInt32*)GetData(CurrentLevel), GetSize(CurrentLevel));
				break;
			case ZE_TPT_GRAYSCALE:
				DownSample((float*)GetData(CurrentLevel + 1), (float*)GetData(CurrentLevel), GetSize(CurrentLevel));
				break;
		}
		CurrentLevel++;
	}
}

void ZETerrainBlock::CalculateMinMax()
{
	if (Data == NULL || PixelType != ZE_TPT_ELEVATION)
		return;

	MaxValue = MinValue =  *(float*)Sample(0, 0);
	for (ZESize y = 0; y < Size; y++)
	{
		for (ZESize x = 0; x < Size; x++)
		{
			float Value = *(float*)Sample(x, y);
			if (Value < MinValue)
				MinValue = Value;
			else if (Value > MaxValue)
				MaxValue = Value;
		}
	}
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

void* ZETerrainBlock::Sample(ZESize x, ZESize y, ZEUInt MipmapLevel)
{
	return (ZEUInt8*)GetData(MipmapLevel) + y * GetPitch(MipmapLevel) + x * GetPixelSize();
}

void ZETerrainBlock::Update()
{
	if (PixelType == ZE_TPT_ELEVATION)
		CalculateMinMax();
	else
		CalculateMipmaps();
}

bool ZETerrainBlock::Create(ZETerrainPixelType PixelType, ZESize Size)
{
	if (this->PixelType != PixelType && this->Size != Size)
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
	else
		MipmapCount = 1;

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

bool ZETerrainBlock::DebugDump(const ZEString& Directory)
{
	for (ZESize I = 0; I < MipmapCount; I++)
	{
		FIBITMAP* Bitmap = FreeImage_ConvertFromRawBits((BYTE*)GetData(I), GetSize(I), GetSize(I), GetPitch(I), 32, 0, 0, 0, true);
		if (Bitmap == NULL)
			return false;
		if (!FreeImage_Save(FIF_PNG, Bitmap, ZEFormat::Format("{4}/L{0}-X{1}-Y{2}-ML{3}.png", GetLevel(), GetPositionX(), GetPositionY(), I, Directory).ToCString()))
			return false;
		FreeImage_Unload(Bitmap);
	}
}

ZETerrainBlock::ZETerrainBlock()
{
	PositionX = 0;
	PositionY = 0;
	Level = 0;
	Data = NULL;
	Clean();
}

ZETerrainBlock::~ZETerrainBlock()
{
	Clean();
}
