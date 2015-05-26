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
#include "ZEFile\ZEFileInfo.h"

#include <zlib.h>
#include "ZEMath\ZEMath.h"

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
		ZELittleEndian<ZEUInt64>	IndexX;
		ZELittleEndian<ZEUInt64>	IndexY;
		ZEInt8						Level;
		ZELittleEndian<ZEUInt32>	Size;
		ZEUInt8						Type;
	}
);

void ZETEBlock::Configure()
{
	if (Data != NULL)
	{
		delete[] Data;
		Data = NULL;
	}

	if (PixelType == ZETE_PT_NONE || Size == 0)
		return;

	DataSize = Size * Size * GetPixelSize();
	Data = new ZEBYTE[DataSize];
}

void ZETEBlock::SetIndexX(ZEUInt64 Index)
{
	IndexX = Index;
}

ZEInt64 ZETEBlock::GetIndexX()
{
	return IndexX;
}

void ZETEBlock::SetIndexY(ZEUInt64 Index)
{
	IndexY = Index;
}

ZEInt64 ZETEBlock::GetIndexY()
{
	return IndexY;
}

void ZETEBlock::SetLevel(ZEInt Level)
{
	this->Level = Level;
}

ZEInt ZETEBlock::GetLevel()
{
	return Level;
}

void ZETEBlock::SetSize(ZESize Size)
{
	if (this->Size == Size)
		return;

	this->Size = Size;
	Configure();
}

ZESize ZETEBlock::GetSize()
{
	return Size;
}

double ZETEBlock::GetLevelBlockSize()
{
	return ZEMath::Power(2, Level) * Size;
}

void ZETEBlock::SetPixelType(ZETEPixelType Type)
{
	if (PixelType == Type)
		return;

	PixelType = Type;
	Configure();
}

double ZETEBlock::GetPositionX()
{
	return IndexX * GetLevelBlockSize();
}

double ZETEBlock::GetPositionY()
{
	return IndexY * GetLevelBlockSize();
}

double ZETEBlock::GetEndX()
{
	return (IndexX + 1) * GetLevelBlockSize();
}

double ZETEBlock::GetEndY()
{
	return (IndexY + 1) * GetLevelBlockSize();
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
		case ZETE_PT_NONE:
			return 0;

		case ZETE_PT_ELEVATION:
			return 2;

		case ZETE_PT_COLOR:
			return 4;

		case ZETE_PT_GRAYSCALE:
			return 1;
	}
}

void* ZETEBlock::GetData()
{
	return (ZEUInt8*)Data;
}

ZESize ZETEBlock::GetPitch()
{
	return Size * GetPixelSize();
}

ZESize ZETEBlock::GetDataSize()
{
	return DataSize;
}

void ZETEBlock::Clean()
{
	if (Size == 0 || PixelType == ZETE_PT_NONE)
		return;

	memset(Data, 0, GetPitch() * GetSize());
}

bool ZETEBlock::Load(const ZEString& FileName)
{
	ZEFileInfo FileInfo(FileName);
	if (!FileInfo.IsExists())
		return false;
	
	ZEFile File;
	if (!File.Open(FileName, ZE_FOM_READ, ZE_FCM_NONE))
		return false;

	if (!Load(&File))
		return false;

	File.Close();

	return true;
}

bool ZETEBlock::Load(ZEFile* File)
{
	if (File == NULL)
		return false;

	ZETEBlockFileHeader Header = {0};
	if (!File->Read(&Header, sizeof(ZETEBlockFileHeader), 1) == 1)
		return false;

	SetIndexX(Header.IndexX);
	SetIndexY(Header.IndexY);
	SetLevel(Header.Level);
	SetSize(Header.Size);
	SetPixelType((ZETEPixelType)Header.Type);
	
	if (File->Read(GetData(), DataSize, 1) != 1)
		return false;

	return true;
}

bool ZETEBlock::Save(const ZEString& FileName)
{
	ZEFile File;
	if (!File.Open(FileName, ZE_FOM_WRITE, ZE_FCM_CREATE))
		return false;

	if (!Save(&File))
		return false;

	File.Close();

	return true;
}

bool ZETEBlock::Save(ZEFile* File)
{
	if (File == NULL)
		return false;

	if (Size == 0 || PixelType == ZETE_PT_NONE)
		return false;

	ZETEBlockFileHeader Header;
	Header.IndexX = GetIndexX();
	Header.IndexY = GetIndexY();
	Header.Level = Level;
	Header.Size = Size;
	Header.Type = PixelType;

	if (File->Write(&Header, sizeof(ZETEBlockFileHeader), 1) != 1)
		return false;

	if (File->Write(GetData(), DataSize, 1) != 1)
		return false;

	return true;
}

bool ZETEBlock::Dump(const ZEString& FileName)
{
	if (Data == NULL)
		return false;

	FIBITMAP* Bitmap = NULL;
	if (PixelType == ZETE_PT_COLOR)
	{
		Bitmap = FreeImage_ConvertFromRawBits((BYTE*)GetData(), GetSize(), GetSize(), GetPitch(), 32, 0, 0, 0, true);
	}
	else if (PixelType == ZETE_PT_GRAYSCALE)
	{
		Bitmap = FreeImage_ConvertFromRawBits((BYTE*)GetData(), GetSize(), GetSize(), GetPitch(), 8, 0, 0, 0, true);
	}
	else if (PixelType == ZETE_PT_ELEVATION)
	{
		Bitmap = FreeImage_AllocateExT(FIT_UINT16, GetSize(), GetSize(), 16, NULL);
		ZESize DestPitch = FreeImage_GetPitch(Bitmap);
		ZEBYTE* DestBuffer = FreeImage_GetBits(Bitmap);
		for (ZESize I = 0; I < Size; I++)
			memcpy(DestBuffer + I * DestPitch, (ZEBYTE*)Data + I * Size * 2, Size * 2);
	}

	if (Bitmap == NULL)
		return false;
	
	if (!FreeImage_Save(FIF_PNG, Bitmap, FileName))
		return false;
	
	FreeImage_Unload(Bitmap);
	
	return true;
}

ZETEBlock::ZETEBlock()
{
	IndexX = 0;
	IndexY = 0;
	Level = 0;
	Size = 0;

	Data = NULL;
	DataSize = 0;
	PixelType = ZETE_PT_NONE;
}

ZETEBlock::~ZETEBlock()
{
	if (Data != NULL)
	{
		delete Data;
		Data = NULL;
	}
}
