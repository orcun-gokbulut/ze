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
#include "ZEPacking.h"

#include <FreeImage.h>
#include "ZEEndian.h"

enum ZETEImagePixelType
{
	ZETE_IPT_NONE					= 0,
	ZETE_IPT_ELEVATION_32F			= 1,
	ZETE_IPT_COLOR_RGBA_8			= 2,
	ZETE_IPT_COLOR_RGBA_16			= 3,
	ZETE_IPT_COLOR_GRAYSCALE_8		= 4,
	ZETE_IPT_COLOR_GRAYSCALE_16		= 5,
};

ZEPackStruct(
	struct ZETEBlockFileHeader
	{
		ZELittleEndian<ZEUInt64>	PositionX;
		ZELittleEndian<ZEUInt64>	PositionY;
		ZEUInt8						Level;
		ZELittleEndian<ZEUInt32>	Dimension;
		ZEUInt8						Type;
		float						MinValue;
		float						MaxValue;
	}
);

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

void ZETEBlock::SetDimension(ZESize Dimension)
{
	this->Dimension = Dimension;
}

ZESize ZETEBlock::GetDimension()
{
	return Dimension;
}

ZETEPixelType ZETEBlock::GetPixelType()
{
	return PixelType;
}

ZESize ZETEBlock::GetPixelSize()
{
	switch (PixelType)
	{
		default:
		case ZE_TPT_NONE:
			return 0;
		case ZE_TPT_ELEVATION:
			return 4;
		case ZE_TPT_COLOR:
			return 4;
		case ZE_TPT_GRAYSCALE:
			return 1;
	}
}

void* ZETEBlock::GetData()
{
	return (ZEUInt8*)Data;
}

ZESize ZETEBlock::GetPitch()
{
	return Pitch;
}

ZESize ZETEBlock::GetDataSize()
{
	return DataSize;
}

bool ZETEBlock::Create(ZETEPixelType PixelType, ZESize Dimension)
{
	if (this->PixelType != PixelType && this->Dimension != Dimension)
		Clean();

	this->PixelType = PixelType;
	this->Dimension = Dimension;
	Pitch = Dimension * GetPixelSize();
	DataSize = Dimension * Pitch;
	Data = new ZEUInt8[DataSize];

	return true;
}

void ZETEBlock::Clean()
{
	Dimension = 0;
	DataSize = 0;
	Pitch = 0;
	PixelType = ZE_TPT_NONE;
	
	if (Data == NULL)
	{
		delete[] (ZEUInt8*)Data;
		Data = NULL;
	}
}

bool ZETEBlock::Load(ZEFile* File)
{
	Clean();

	ZETEBlockFileHeader Header = {0};
	if (!File->Read(&Header, sizeof(ZETEBlockFileHeader), 1) == 1)
		return false;
	
	Create((ZETEPixelType)Header.Type, Header.Dimension);

	if (File->Read(GetData(), DataSize, 1) != 1)
	{
		Clean();
		return false;
	}

	PositionX = Header.PositionX;
	PositionY = Header.PositionY;
	Level = Header.Level;

	return true;
}

bool ZETEBlock::Save(ZEFile* File)
{
	ZETEBlockFileHeader Header;
	Header.PositionX = PositionX;
	Header.PositionY = PositionY;
	Header.Level = Level;
	Header.Dimension = Dimension;
	Header.Type = PixelType;

	if (File->Write(&Header, sizeof(ZETEBlockFileHeader), 1) != 1)
		return false;

	if (File->Write(GetData(), DataSize, 1) != 1)
		return false;

	return true;
}

bool ZETEBlock::DebugDump(const ZEString& Directory)
{
	FIBITMAP* Bitmap = FreeImage_ConvertFromRawBits((BYTE*)GetData(), GetDimension(), GetDimension(), GetPitch(), 32, 0, 0, 0, true);
	if (Bitmap == NULL)
		return false;
	if (!FreeImage_Save(FIF_PNG, Bitmap, ZEFormat::Format("{0}/L{1}-X{2}-Y{3}-ML{4}.png", Directory, GetLevel(), GetPositionX(), GetPositionY(), 0).ToCString()))
		return false;
	FreeImage_Unload(Bitmap);
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
