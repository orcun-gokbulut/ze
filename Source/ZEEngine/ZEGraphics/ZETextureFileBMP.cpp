//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileBMP.cpp
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

#include "ZETextureFileBMP.h"

#include "ZEError.h"
#include "ZEPacking.h"
#include "ZEEndian.h"
#include "ZETextureData.h"
#include "ZEDS/ZEPointer.h"
#include "ZEFile/ZEFile.h"

ZEPackStruct
(
	struct ZEBitmapHeader
	{
		ZELittleEndian<ZEUInt16> Header;
		ZELittleEndian<ZEUInt32> FileSize;
		ZELittleEndian<ZEUInt16> Reserved0;
		ZELittleEndian<ZEUInt16> Reserved1;
		ZELittleEndian<ZEUInt32> DataPosition;
	}
);

ZEPackStruct
(
	struct ZEBitmapDIBHeader
	{
		ZELittleEndian<ZEUInt32> Size;

		// V1-2
		ZELittleEndian<ZEInt32>  Width;
		ZELittleEndian<ZEInt32>  Height;
		ZELittleEndian<ZEUInt16> PlaneCount;
		ZELittleEndian<ZEUInt16> BitsPerPixel;

		// V3
		ZELittleEndian<ZEUInt32> CompressionType;
		ZELittleEndian<ZEUInt32> DataSize;
		ZELittleEndian<ZEInt32>  HorizontalResolution;
		ZELittleEndian<ZEInt32>  VerticalResoulution;
		ZELittleEndian<ZEUInt32> PaletteEntryCount;
		ZELittleEndian<ZEUInt32> ImportantColorCount;

		// V4 Partial
		ZELittleEndian<ZEUInt32> RedMask; 
		ZELittleEndian<ZEUInt32> GreenMask;
		ZELittleEndian<ZEUInt32> BlueMask;
		ZELittleEndian<ZEUInt32> AlphaMask;
	}
);

ZEPackStruct
(
	struct ZEBitmapRGB565
	{
		ZEUInt8 R : 5;
		ZEUInt8 G : 6;
		ZEUInt8 B : 5;
	}
);

ZEPackStruct
(
	struct ZEBitmapRGB555
	{
		ZEUInt8 Reserved : 1;
		ZEUInt8 R : 5;
		ZEUInt8 G : 6;
		ZEUInt8 B : 5;
	}
);


ZEPackStruct
(
	struct ZEBitmapRGB888
	{
		ZEUInt8 B;
		ZEUInt8 G;
		ZEUInt8 R;
	}
);

ZEPackStruct
(
	struct ZEBitmapRGB8888
	{
		ZEUInt8 B;
		ZEUInt8 G;
		ZEUInt8 R;
		ZEUInt8 A;
	}
);



bool ZETextureFileBMP::CanLoad(ZEFile* File)
{
	ZEUInt16 Header;
	
	ZEUInt64 Result = File->Read(&Header, sizeof(Header), 1);
	if (Result == 0)
		return false;

	if (Header != 'MB')
		return false;

	return true;
}

static bool Uncompress(ZEUInt8* Dest, ZESize Width, ZESize Height, ZEUInt8* Data, ZESize DataSize)
{
	memset(Dest, 0, Width * Height);

	ZESSize PositionX = 0;
	ZESSize PositionY = Height - 1;

	ZEUInt8* CurrDest = Dest;

	for (ZESize I = 0; I < DataSize; I++)
	{
		ZEUInt8 Command = Data[I];
		I++;

		if (Command != 0)
		{
			ZEUInt8 Value = *Data;
			Data++;

			for (ZESize N = 0; N < Command; N++)
			{
				*CurrDest = Value;
				CurrDest++;
			}
		}
		else
		{
			ZEUInt8 SecondCommand = *Data;
			Data++;

			if (SecondCommand == 0)
			{
				ZESSize CurrPosY = (CurrDest - Dest) / Width;
				ZESSize CurrPosX = CurrDest - Dest + CurrPosY * Width;
				CurrPosY++;
				CurrDest = Dest + CurrPosY * Width + CurrPosX;
			}
			else if (SecondCommand == 1)
			{
				break;
			}
			else if (SecondCommand == 2)
			{
				ZESSize CurrPosY = (CurrDest - Dest) / Width;
				ZESSize CurrPosX = CurrDest - Dest + CurrPosY * Width;
				CurrPosX += *Data;
				Data++;
				CurrPosY += *Data;
				Data++;
				CurrDest = Dest + CurrPosY * Width + CurrPosX;
			}
			else
			{
				for (ZESize N = 0; N < SecondCommand; N++)
				{
					*CurrDest = *Data;
					CurrDest++;
					Data++;
				}
			}
		}
	}

	return true;
}

static ZETextureData* Load8BitCompressed(ZEFile* File, ZEBitmapHeader* Header, ZEBitmapDIBHeader* DIBHeader, ZEBitmapRGB8888* Palette)
{
	File->Seek(Header->DataPosition, ZE_SF_BEGINING);

	ZESize CompressedDataSize = DIBHeader->DataSize;
	ZEPointer<ZEUInt8> CompressedData = new ZEUInt8[CompressedDataSize];

	ZEUInt64 Result = File->Read(CompressedData, DIBHeader->DataSize, 1);
	if (Result != 1)
	{
		zeError("Corrupted bitmap data.");
		return NULL;
	}

	ZEPointer<ZETextureData> Texture = new ZETextureData();
	Texture->CreateTexture(ZE_TT_2D, ZE_TPF_I8, 1, 1, DIBHeader->Width, DIBHeader->Height);
	ZETextureLevel* Level = &Texture->GetSurfaces()[0].GetLevels()[0];

	if (!Uncompress((ZEUInt8*)Level->GetData(), DIBHeader->Width, DIBHeader->Height, CompressedData, CompressedDataSize))
		return NULL;

	return Texture.Transfer();
}

static ZETextureData* Load8Bit(ZEFile* File, ZEBitmapHeader* Header, ZEBitmapDIBHeader* DIBHeader, ZEBitmapRGB8888* Palette)
{
	ZESSize Width = DIBHeader->Width;
	ZESSize Height = (DIBHeader->Height < 0 ? -DIBHeader->Height : DIBHeader->Height);

	ZEPointer<ZETextureData> Texture = new ZETextureData();
	Texture->CreateTexture(ZE_TT_2D, ZE_TPF_I8_4, 1, 1, (ZEUInt)Width, (ZEUInt)Height);
	ZEBitmapRGB8888* Destination = (ZEBitmapRGB8888*)Texture->GetSurfaces()[0].GetLevels()[0].GetData();

	File->Seek(Header->DataPosition, ZE_SF_BEGINING);

	ZESize Align = Width + (Width % 4 != 0 ? 4 - Width % 4 : 0);
	ZEPointer<ZEUInt8> SourceRowData = new ZEUInt8[Align];

	ZESize Step, Index, Count; 
	if (DIBHeader->Height > 0)
	{
		Step = -1;
		Index = Height - 1;
		Count = Height;
	}
	else
	{
		Step = 1;
		Index = 0;
		Count = -Height;
	}

	for (ZESize Y = 0; Y < Count; Y++)
	{
		ZEBitmapRGB8888* DestinationRow = Destination + Index * Width;	
		ZEUInt64 Result = File->Read(SourceRowData, Align, 1);
		if (Result != 1)
		{
			zeError("Corrupted bitmap data.");
			return NULL;
		}

		for (ZESize I = 0; I < (ZESize)Width; I++)
		{
			ZEBitmapRGB8888 Temp = Palette[SourceRowData[I]];
			/*Temp.A = Palette[SourceRowData[I]].B;
			Temp.R = Palette[SourceRowData[I]].G;
			Temp.G = Palette[SourceRowData[I]].R;
			Temp.B = Palette[SourceRowData[I]].A;*/
			DestinationRow[I] = Temp;
		}

		Index += Step;
	}

	return Texture.Transfer();
}

static ZETextureData* Load16Bit(ZEFile* File, ZEBitmapHeader* Header, ZEBitmapDIBHeader* DIBHeader)
{
	ZEInt32 Width = DIBHeader->Width;
	ZEInt32 Height = (DIBHeader->Height < 0 ? -DIBHeader->Height : DIBHeader->Height);

	ZETextureData* Output = new ZETextureData();
	Output->CreateTexture(ZE_TT_2D, ZE_TPF_I8, 1, 1, Width, Height);
	ZEBitmapRGB8888* Destination = (ZEBitmapRGB8888*)Output->GetSurfaces()[0].GetLevels()[0].GetData();

	File->Seek(Header->DataPosition, ZE_SF_BEGINING);

	ZESize Align = (ZESize)Width * 2;
	Align = (Align % 4 != 0 ? 4 - Align % 4 : 0);

	ZEUInt8* SourceRow = new ZEUInt8[Align];

	ZESSize Step = (Height > 0 ? -1 : 1);
	ZESSize Index = (Height > 0 ? (ZESize)Height - 1 : 0);

	for (ZESize I = 0; I < (ZESize)Height; I++)
	{
		ZEUInt64 Result = File->Read(SourceRow, Align, 1);
		if (Result != 1)
		{
			zeError("Corrupted bitmap data.");
			return NULL;
		}

		ZEBitmapRGB8888* DestinationRow = Destination + Index * (ZESize)Width;	
		for (ZESize N = 0; N < (ZESize)Width; N++)
		{
			ZEBitmapRGB8888* DestinationColor =  (ZEBitmapRGB8888*)DestinationRow + Index;
			if (DIBHeader->GreenMask == 0x003E0000)
			{
				ZEBitmapRGB555* SourceColor = (ZEBitmapRGB555*)SourceRow + N;
				DestinationColor->A = 255;
				DestinationColor->R = SourceColor->R;
				DestinationColor->G = SourceColor->G;
				DestinationColor->B = SourceColor->B;
			}
			else
			{
				ZEBitmapRGB565* SourceColor = (ZEBitmapRGB565*)SourceRow + N;
				DestinationColor->A = 255;
				DestinationColor->R = SourceColor->R;
				DestinationColor->G = SourceColor->G;
				DestinationColor->B = SourceColor->B;
			}
		}

		Index += Step;
	}

	return Output;
}

static ZETextureData* Load24Bit(ZEFile* File, ZEBitmapHeader* Header, ZEBitmapDIBHeader* DIBHeader)
{
	return NULL;
}

static ZETextureData* Load32Bit(ZEFile* File, ZEBitmapHeader* Header, ZEBitmapDIBHeader* DIBHeader)
{
	return NULL;
}

static ZETextureData* Load(ZEFile* File)
{
	ZEUInt64 Result;

	ZEBitmapHeader Header;
	Result = File->Read(&Header, sizeof(Header), 1);
	if (Result == 0)
	{
		zeError("Can not load BMP file header.");
		return NULL;
	}

	if (Header.Header != 'MB')
	{
		zeError("Unknown BMP file.");
		return NULL;
	}

	ZEBitmapDIBHeader DIBHeader;
	DIBHeader.Size = 0;
	File->Read(&DIBHeader.Size, sizeof(DIBHeader.Size), 1);
	if (DIBHeader.Size < 40)
	{
		zeError("Unsupported BMP file version. Only version 3, 4 and 5 are supported.");
		return NULL;
	}

	File->Seek(sizeof(ZEBitmapHeader), ZE_SF_BEGINING);
	Result = File->Read(&DIBHeader, sizeof(ZEBitmapDIBHeader), 1);
	if (Result == 0)
	{
		zeError("Can not load BMP file DIB header.");
		return NULL;
	}

	if (DIBHeader.BitsPerPixel != 8 && DIBHeader.BitsPerPixel != 16 && DIBHeader.BitsPerPixel != 24 && DIBHeader.BitsPerPixel != 32)
	{
		zeError("Unsuppored BMP file pixel format. Only 8, 16, 24 and 32 bit pixel formats supported.");
		return NULL;
	}

	ZEBitmapRGB8888 Pallete[256];
	if (DIBHeader.BitsPerPixel == 8)
	{
		if (DIBHeader.PaletteEntryCount == 0)
			DIBHeader.PaletteEntryCount = 256;

		File->Seek(DIBHeader.Size, ZE_SF_BEGINING);
		if (File->Read(Pallete, sizeof(ZEBitmapRGB8888), DIBHeader.PaletteEntryCount) != DIBHeader.PaletteEntryCount)
		{
			zeError("Can not load BMP file's palate entries.");
			return NULL;
		}
	}

	if (DIBHeader.CompressionType != 0) 
	{
		if (DIBHeader.CompressionType != 1 && DIBHeader.BitsPerPixel != 8)
		{
			zeError("Unsupported compression format.");
			return NULL;
		}
	}

	if (DIBHeader.Height == 0 || DIBHeader.Width <= 0)
	{
		zeError("Corrupted or malicious BMP file.");
		return NULL;
	}

	switch(DIBHeader.BitsPerPixel)
	{
		case 8:
			if (DIBHeader.CompressionType == 0)
				return Load8Bit(File, &Header, &DIBHeader, Pallete);
			else
				return Load8BitCompressed(File, &Header, &DIBHeader, Pallete);

			break;

		case 16:
			return Load16Bit(File, &Header, &DIBHeader);
			break;

		case 24:
			return Load24Bit(File, &Header, &DIBHeader);
			break;

		case 32:
			return Load32Bit(File, &Header, &DIBHeader);
			break;
	}
	
	zeError("Unknow error.");
	return NULL;
}

ZETextureData* ZETextureFileBMP::Load(ZEFile* File)
{
	ZETextureData* Texture = ::Load(File);
	if (Texture == NULL)
		zeError("Can not load BMP file.");

	return Texture;
}
