//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileTGA.cpp
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

#include "ZETextureFileTGA.h"

#include "ZEError.h"
#include "ZEPacking.h"
#include "ZEEndian.h"
#include "ZETextureData.h"
#include "ZEDS/ZEPointer.h"
#include "ZEFile/ZEFile.h"

#define ZE_TIT_NO_IMAGE					0
#define ZE_TIT_INDEXED					1
#define ZE_TIT_COLOR					2
#define ZE_TIT_GRAYSCALE				3
#define ZE_TIT_INDEXED_COMPRESSED		9
#define ZE_TIT_COLOR_COMPRESSED			10	
#define ZE_TIT_GRAYSCALE_COMPRESSED		11

#define ZE_TIO_LEFT						
#define ZE_TIO_RIGHT
#define ZE_TIO_TOP
#define ZE_TIO_BOTTOM


ZEPackStruct
(
	struct ZETargaHeader
	{
		ZEUInt8						IdSize;
		ZEUInt8						ColorMapType;
		ZEUInt8						ImageType;
		ZEUInt8						PalleteFirstEntry;
		ZEUInt8						PalleteSize;
		ZEUInt8						PalleteEntryBPP;
			
		ZELittleEndian<ZEUInt16>	XOrgin;
		ZELittleEndian<ZEUInt16>	YOrgin;
		ZELittleEndian<ZEUInt16>	Width;
		ZELittleEndian<ZEUInt16>	Height;
		ZELittleEndian<ZEUInt16>	BPP;
		ZEUInt8						ImageDescriptor;
	}
);

static inline ZESize GetPixelSize(ZEUInt8 BPP)
{
	switch(BPP)
	{
		case 8:
			return 1;

		case 15:
			return 2;

		case 16:
			return 2;

		case 24:
			return 3;

		case 32:
			return 4;

		default:
			return 0;
	}
}

static inline ZEARGB32 ConvertFrom15(void* Value)
{
	ZEARGB32 Color;
	Color.R = (((ZEUInt8*)Value)[1] << 1) & 0xF8;
	Color.G = (((ZEUInt8*)Value)[1] << 6) | ((((ZEUInt8*)Value)[0] >> 2) & 0x38); 
	Color.B = ((ZEUInt8*)Value)[0] << 3;
	Color.A = 255;

	return Color;
}

static inline ZEARGB32 ConvertFromA15(void* Value)
{
	ZEARGB32 Color;
	Color.R = (((ZEUInt8*)Value)[1] << 1) & 0xF8;
	Color.G = (((ZEUInt8*)Value)[1] << 6) | ((((ZEUInt8*)Value)[0] >> 2) & 0x38); 
	Color.B = ((ZEUInt8*)Value)[0] << 3;
	Color.A = (((ZEUInt8*)Value)[1] & 0x80) == 0 ? 0 : 255;

	return Color;
}

static inline ZEARGB32 ConvertFrom16(void* Value)
{
	ZEARGB32 Color;
	Color.R = ((ZEUInt8*)Value)[1] & 0xF8;
	Color.G = (((ZEUInt8*)Value)[1] << 5) | ((((ZEUInt8*)Value)[0] >> 3) & 0x1C); 
	Color.B = ((ZEUInt8*)Value)[0] << 3;
	Color.A = 255;
	return Color;
}

static inline ZEARGB32 ConvertFrom24(void* Value)
{
	ZEARGB32 Color;
	Color.A = 255;
	Color.B = ((ZEUInt8*)Value)[0];
	Color.G = ((ZEUInt8*)Value)[1];
	Color.R = ((ZEUInt8*)Value)[2];
	return Color;
}

static inline ZEARGB32 ConvertFrom32(void* Value)
{
	ZEARGB32 Color;
	Color.B = ((ZEUInt8*)Value)[0];
	Color.G = ((ZEUInt8*)Value)[1];
	Color.R = ((ZEUInt8*)Value)[2];
	Color.A = ((ZEUInt8*)Value)[3];
	return Color;
}

static ZETextureData* LoadData(ZEFile* File, ZETargaHeader* Header, ZEARGB32* Pallete)
{
	return NULL;
}

static bool LoadHeader(ZEFile* File, ZETargaHeader& Header, ZEARGB32* Pallete)
{
	File->Seek(0, ZE_SF_BEGINING);
	
	ZEInt64 Result;
	Result = File->Read(&Header, sizeof(ZETargaHeader), 1);
	if (Result != 1)
	{
		zeError("Can not load TGA file header.");
		return false;
	}

	if (Header.BPP != 8 && Header.BPP != 16 && Header.BPP != 24 && Header.BPP != 32)
	{
		zeError("Unsupported pixel size.");
		return false;
	}

	if (Header.Width == 0 || Header.Height == 0)
	{
		zeError("Zero sized TGAs are not supported.");
		return false;
	}

	if (Header.ImageType != 1 && Header.ImageType != 2 && Header.ImageType != 2 && 
		Header.ImageType != 9 && Header.ImageType != 10 && Header.ImageType != 11)
	{
		zeError("Unsupported image type.");
		return false;
	}

	if (Header.ImageDescriptor & 0x08)
	{
		zeError("Right to left image data is not supported.");
		return false;
	}

	ZESize PalletePixelSize = GetPixelSize(Header.PalleteEntryBPP);

	if (Header.ColorMapType == 1)
	{
		if (Header.BPP != 8)
		{
			zeError("Unsupported indexed pixel size.");
			return false;
		}

		ZESize PalleteSize = (Header.PalleteSize > 256 ? 256 : Header.PalleteSize);
		memset(&Pallete, 0, sizeof(ZEARGB32) * 256);

		ZEUInt32 PalleteTemp[256];
		Result = File->Read(PalleteTemp, PalletePixelSize, PalleteSize);
		if (Result != PalleteSize)
		{
			zeError("Can not load pallete.");
			return false;
		}

		for (ZESize I = Header.PalleteFirstEntry; I < PalleteSize; I++)
		{
			switch(Header.PalleteEntryBPP)
			{
				case 15:
					Pallete[I] = ConvertFrom15(PalleteTemp + I * PalletePixelSize);
					break;

				case 16:
					Pallete[I] = ConvertFrom16(PalleteTemp + I * PalletePixelSize);
					break;

				case 24:
					Pallete[I] = ConvertFrom24(PalleteTemp + I * PalletePixelSize);
					break;

				case 32:
					Pallete[I] = ConvertFrom32(PalleteTemp + I * PalletePixelSize);
					break;
			}
		}
	}

	File->Seek(sizeof(Header) + Header.PalleteSize * PalletePixelSize + Header.IdSize, ZE_SF_BEGINING);

	return true;
}


bool ZETextureFileTGA::CanLoad(ZEFile* File)
{
	return false;
}

ZETextureData* ZETextureFileTGA::Load(ZEFile* File)
{
	/*ZETextureData* Texture = ::Load(File);
	if (Texture == NULL)
		zeError("Can not load TGA file.");
		*/
	return NULL; //Texture;
}
