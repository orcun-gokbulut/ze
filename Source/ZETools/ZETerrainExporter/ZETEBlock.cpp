//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETEBlock.cpp
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

#include "ZETEBlock.h"

#include "ZEError.h"
#include "ZEFile\ZEFile.h"
#include "ZEDS\ZEFormat.h"

#include <FreeImage.h>

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

static void DownSample_CPU(float* Destination, float* Source, ZESize Size)
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

static void DownSample_CPU(ZEUInt32* Destination, ZEUInt32* Source, ZESize Size)
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

			PD[0] = (ZEUInt8)(((ZEUInt)P0[0] + (ZEUInt)P1[0] + (ZEUInt)P2[0] + (ZEUInt)P3[0]) /4);
			PD[1] = (ZEUInt8)(((ZEUInt)P0[1] + (ZEUInt)P1[1] + (ZEUInt)P2[1] + (ZEUInt)P3[1]) /4);
			PD[2] = (ZEUInt8)(((ZEUInt)P0[2] + (ZEUInt)P1[2] + (ZEUInt)P2[2] + (ZEUInt)P3[2]) /4);
			PD[3] = (ZEUInt8)(((ZEUInt)P0[3] + (ZEUInt)P1[3] + (ZEUInt)P2[3] + (ZEUInt)P3[3]) /4);
		}
	}
}

enum ZETEImagePixelType
{
	ZETE_IPT_ELEVATION_8,
	ZETE_IPT_ELEVATION_16,
	ZETE_IPT_COLOR_RGBA,
	ZETE_IPT_COLOR_GRAYSCALE
};

struct ZETERectangle
{
	ZETEImagePixelType PixelType;
	void* Data;
	ZESize OffsetX;
	ZESize OffsetY;
	ZESize Width;
	ZESize Height;
};

static void Rescale_CPU(ZEUInt32* Destination, ZEUInt32* Source, ZETERectangle)
{

}

void ZETEBlock::CalculateMipmaps()
{
	ZEUInt CurrentLevel = 0;
	ZEUInt NextLevel = 1;

	while(GetSize(CurrentLevel) != 0)
	{
		switch(PixelType)
		{
			case ZE_TPT_COLOR:
				DownSample_CPU((ZEUInt32*)GetData(CurrentLevel + 1), (ZEUInt32*)GetData(CurrentLevel), GetSize(CurrentLevel));
				break;
			case ZE_TPT_GRAYSCALE:
				DownSample_CPU((float*)GetData(CurrentLevel + 1), (float*)GetData(CurrentLevel), GetSize(CurrentLevel));
				break;
		}
		CurrentLevel++;
	}
}

void ZETEBlock::CalculateMinMax()
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

void ZETEBlock::SetPositionX(ZEInt64 x)
{
	PositionX = x;
}

ZEInt64 ZETEBlock::GetPositionX()
{
	return PositionX;
}

void ZETEBlock::SetPositionY(ZEInt64 y)
{
	PositionY = y;
}

ZEInt64 ZETEBlock::GetPositionY()
{
	return PositionY;
}

void ZETEBlock::SetLevel(ZEUInt Level)
{
	this->Level = Level;
}

ZEUInt ZETEBlock::GetLevel()
{
	return Level;
}

ZETEPixelType ZETEBlock::GetPixelType()
{
	return PixelType;
}

ZESize ZETEBlock::GetPixelSize()
{
	return PixelSize;
}

ZEUInt ZETEBlock::GetMipmapCount()
{
	return MipmapCount;
}

void* ZETEBlock::GetData(ZEUInt MipmapLevel)
{
	ZEUInt8* NormalizedData = (ZEUInt8*)Data;

	for (ZEUInt I = 0; I < MipmapLevel; I++)
		NormalizedData += GetSize(I) * GetPitch(I);
	
	return NormalizedData;
}

ZESize ZETEBlock::GetPitch(ZEUInt MipmapLevel)
{
	return Pitch >> MipmapLevel;
}


ZESize ZETEBlock::GetSize(ZEUInt MipmapLevel)
{
	return Size >> MipmapLevel;
}

void* ZETEBlock::Sample(ZESize x, ZESize y, ZEUInt MipmapLevel)
{
	return (ZEUInt8*)GetData(MipmapLevel) + y * GetPitch(MipmapLevel) + x * GetPixelSize();
}

void ZETEBlock::Update()
{
	if (PixelType == ZE_TPT_ELEVATION)
		CalculateMinMax();
	else
		CalculateMipmaps();
}

bool ZETEBlock::Create(ZETEPixelType PixelType, ZESize Size)
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

void ZETEBlock::Clean()
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

bool ZETEBlock::Load(ZEFile* File)
{
	Clean();

	ZETerrainChunkHeader Header = {0};
	if (!File->Read(&Header, sizeof(ZETerrainChunkHeader), 1) == 1)
		return false;
	
	Create((ZETEPixelType)Header.Type, Header.Size);

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

bool ZETEBlock::Save(ZEFile* File)
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

bool ZETEBlock::DebugDump(const ZEString& Directory)
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

ZETEBlock::ZETEBlock()
{
	PositionX = 0;
	PositionY = 0;
	Level = 0;
	Data = NULL;
	Clean();
}

ZETEBlock::~ZETEBlock()
{
	Clean();
}
