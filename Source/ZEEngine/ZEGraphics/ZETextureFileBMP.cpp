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

		// DIB Header
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
		ZEBigEndian<ZEUInt32> RedMask; 
		ZEBigEndian<ZEUInt32> GreenMask;
		ZEBigEndian<ZEUInt32> BlueMask;
		ZEBigEndian<ZEUInt32> AlphaMask;
	}
);

static inline ZESize GetPixelSize(ZEUInt16 BPP)
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

static __forceinline ZEARGB32 ConvertRow15(ZEUInt8* Value)
{
	ZEARGB32 Color;
	Color.R = (Value[1] << 1) & 0xF8;
	Color.G = (Value[1] << 6) | ((Value[0] >> 2) & 0x38); 
	Color.B = Value[0] << 3;
	Color.A = 255;

	return Color;
}

static __forceinline ZEARGB32 ConvertRow16(ZEUInt8* Value)
{
	ZEARGB32 Color;
	Color.R = Value[1] & 0xF8;
	Color.G = (Value[1] << 5) | ((Value[0] >> 3) & 0x1C); 
	Color.B = Value[0] << 3;
	Color.A = 255;
	return Color;
}

static __forceinline ZEARGB32 ConvertRow24(void* Value)
{
	ZEARGB32 Color;
	Color.A = 255;
	Color.B = ((ZEUInt8*)Value)[0];
	Color.G = ((ZEUInt8*)Value)[1];
	Color.R = ((ZEUInt8*)Value)[2];
	return Color;
}

static __forceinline ZEARGB32 ConvertRow32(void* Value)
{
	ZEARGB32 Color;
	Color.B = ((ZEUInt8*)Value)[0];
	Color.G = ((ZEUInt8*)Value)[1];
	Color.R = ((ZEUInt8*)Value)[2];
	Color.A = ((ZEUInt8*)Value)[3];
	return Color;
}

static ZEUInt8* LoadCompressedData(ZEFile* File, ZEBitmapHeader* Header)
{
	ZESize Width = Header->Width;
	ZESize Height = (Header->Height < 0 ? -Header->Height : Header->Height);
	ZESize CompressedDataSize = Header->DataSize;

	File->Seek(Header->DataPosition, ZE_SF_BEGINING);

	ZEPointer<ZEUInt8> CompressedData = new ZEUInt8[CompressedDataSize];
	ZEInt64 Result = File->Read(CompressedData, CompressedDataSize, 1);
	if (Result == NULL)
	{
		zeError("Corrupted file.");
		return NULL;
	}
	
	ZEPointer<ZEUInt16> UncompressedData = new ZEUInt16[Width * Height];
	memset(UncompressedData, 0, Width * Height * sizeof(ZEUInt16));

	ZEUInt16* CurrentData = UncompressedData;

	ZESize I = 0;
	while (I < CompressedDataSize)
	{
		if (CompressedData[I] == 0)
		{
			if (I + 1 >= CompressedDataSize)
				return NULL;

			if (CompressedData[I + 1] == 0) // End Of Line
			{
				ZESize Y = (CurrentData - UncompressedData) / Width;
				ZESize X = (CurrentData - UncompressedData) % Width;
				if (X != 0)
					CurrentData = UncompressedData + (Y + 1) * Width;
				I += 2;
			}
			else if (CompressedData[I + 1] == 1) // End Of Image
			{
				I += 2;
				break;
			}
			else if (CompressedData[I + 1] == 2) // Delta
			{
				if (I + 4 >= CompressedDataSize)
					return NULL;

				ZESize X = (CurrentData - UncompressedData) % Width;
				ZESize Y = (CurrentData - UncompressedData) / Width;

				X += CompressedData[I + 2];
				Y += CompressedData[I + 3];

				if (X + Y >= Width * Height)
					return NULL;

				CurrentData = UncompressedData  + Y * Width + X;
				
				I += 4;
			}
			else // Absolute Mode
			{
				if (I + 1 + CompressedData[I + 1] >= CompressedDataSize)
					return NULL;

				if (CurrentData + CompressedData[I + 1] > UncompressedData + Width * Height)
					return NULL;

				for (ZESize N = 0; N < CompressedData[I + 1]; N++)
				{
					*CurrentData = CompressedData[I + 2 + N];
					*CurrentData |= 0xFF00;
					CurrentData++;
				}

				if ((CompressedData[I + 1] % 2) == 1 && CompressedData[I + 2 + CompressedData[I + 1]] != 0)
					return NULL;

				I += 2 + CompressedData[I + 1] + CompressedData[I + 1] % 2;
			}

		}
		else // RLE Mode
		{
			if (I + 2 >= CompressedDataSize)
				return NULL;

			if (CurrentData + CompressedData[I] > UncompressedData + Width * Height)
				return NULL;

			for (ZESize N = 0; N < CompressedData[I]; N++)
			{
				*CurrentData = CompressedData[I + 1];
				*CurrentData |= 0xFF00;
				CurrentData++;
			}

			I += 2;
		}
	}

	return CompressedData.Transfer();
}

static ZEUInt8* LoadData(ZEFile* File, ZEBitmapHeader* Header)
{
	ZESSize Width = Header->Width;
	ZESSize Height = (Header->Height < 0 ? -Header->Height : Header->Height);

	File->Seek(Header->DataPosition, ZE_SF_BEGINING);

	ZEPointer<ZEUInt8> Buffer = new ZEUInt8[Width * Height * GetPixelSize(Header->BitsPerPixel)];
	ZEUInt64 Result = File->Read(Buffer, Width * Height * GetPixelSize(Header->BitsPerPixel), 1);
	if (Result != 1)
		return NULL;

	return Buffer.Transfer();
}

static ZETextureData* ConvertData(ZEBitmapHeader* Header, ZEARGB32* Palette, ZEUInt8* Data)
{
	ZESSize Width = Header->Width;
	ZESSize Height = (Header->Height < 0 ? -Header->Height : Header->Height);

	ZEPointer<ZETextureData> Texture = new ZETextureData();
	Texture->Create(ZE_TT_2D, ZE_TPF_I8_4, 1, 1, (ZEUInt)Width, (ZEUInt)Height);
	ZEARGB32* Destination = (ZEARGB32*)Texture->GetSurfaces()[0].GetLevels()[0].GetData();

	ZESize Align = (Header->BitsPerPixel / 8) * Width;
	Align += (Align % 4) != 0 ? 4 - (Align % 4) : 0;
	ZESSize Step = (Height > 0 ? -1 : 1);
	ZESSize Index = (Height > 0 ? Height - 1 : 0);

	ZESize PixelSize = GetPixelSize(Header->BitsPerPixel);
	switch(Header->BitsPerPixel)
	{
		case 8:
			for (ZESize I = 0; I < (ZESize)Height; I++)
			{
				ZEUInt8* SourceRow = Data + Index * Width * PixelSize;	
				ZEARGB32* DestinationRow = Destination + Index * (ZESize)Width;
				for (ZESize N = 0; N < (ZESize)Width; N++)
					DestinationRow[N] = Palette[SourceRow[N]];
				Index += Step;
			}
			break;

		case 16:
			if (Header->GreenMask == ZEEndian::Big(0x000003E0))
			{
				for (ZESize I = 0; I < (ZESize)Height; I++)
				{
					ZEUInt8* SourceRow = Data + Index * Width * PixelSize;	
					ZEARGB32* DestinationRow = Destination + Index * (ZESize)Width;
					for (ZESize N = 0; N < (ZESize)Width; N++)
						DestinationRow[N] = ConvertRow15(SourceRow + N * 2);
					Index += Step;
				}
			}
			else
			{
				for (ZESize I = 0; I < (ZESize)Height; I++)
				{
					ZEUInt8* SourceRow = Data + Index * Width * PixelSize;	
					ZEARGB32* DestinationRow = Destination + Index * (ZESize)Width;
					for (ZESize N = 0; N < (ZESize)Width; N++)
						DestinationRow[N] = ConvertRow16(SourceRow + N * 2);
					Index += Step;
				}
			}
			break;

		case 24:
			for (ZESize I = 0; I < (ZESize)Height; I++)
			{
				ZEUInt8* SourceRow = Data + Index * Width * PixelSize;	
				ZEARGB32* DestinationRow = Destination + Index * (ZESize)Width;
				for (ZESize N = 0; N < (ZESize)Width; N++)
					DestinationRow[N] = ConvertRow24(SourceRow + N * 3);
				Index += Step;
			}
			break;

		case 32:
			for (ZESize I = 0; I < (ZESize)Height; I++)
			{
				ZEUInt8* SourceRow = Data + Index * Width * PixelSize;	
				ZEARGB32* DestinationRow = Destination + Index * (ZESize)Width;
				for (ZESize N = 0; N < (ZESize)Width; N++)
					DestinationRow[N] = ConvertRow32(SourceRow + N * 4);
				Index += Step;
			}
			break;
	}

	return Texture.Transfer();
}

static bool LoadHeaders(ZEFile* File, ZEBitmapHeader* Header, ZEARGB32* Palette)
{
	ZEUInt64 Result;

	File->Seek(0, ZE_SF_BEGINING);
	Result = File->Read(Header, 1, sizeof(ZEBitmapHeader));
	if (Result < 54)
	{
		zeError("Can not load BMP file header.");
		return false;
	}

	if (Header->Header != 'MB')
	{
		zeError("Unknown BMP file format.");
		return false;
	}

	if (Header->Height == 0 || Header->Width <= 0)
	{
		zeError("Corrupted or malicious BMP file.");
		return false;
	}
	
	if (Header->CompressionType != 3)
	{
		if (Header->BitsPerPixel == 16) 
		{
			Header->RedMask   = ZEEndian::Big(0x00007C00);
			Header->GreenMask = ZEEndian::Big(0x000003E0); 
			Header->BlueMask  = ZEEndian::Big(0x0000001F);
		}
		else if (Header->BitsPerPixel == 24)
		{
			Header->RedMask   = ZEEndian::Big(0x00FF0000); 
			Header->GreenMask = ZEEndian::Big(0x0000FF00); 
			Header->BlueMask  = ZEEndian::Big(0x000000FF);
		}
		else if (Header->BitsPerPixel == 32)
		{
			Header->RedMask   = ZEEndian::Big(0x00FF0000); 
			Header->GreenMask = ZEEndian::Big(0x0000FF00); 
			Header->BlueMask  = ZEEndian::Big(0x000000FF);
		}
	}



	if (Header->BitsPerPixel != 8 && Header->BitsPerPixel != 16 && Header->BitsPerPixel != 24 && Header->BitsPerPixel != 32)
	{
		zeError("Unsuppored BMP file pixel format. Only 8, 16, 24 and 32 bit pixel formats supported.");
		return false;
	}
	
	switch(Header->CompressionType)
	{
		case 0:
			break;

		case 1:
			if (Header->BitsPerPixel != 8)
			{
				zeError("Unsupported compressed format.");
				return false;
			}
			break;

		case 3:
			if (!(Header->BitsPerPixel == 16 && 
				Header->RedMask   == ZEEndian::Big(0x0000F800) && 
				Header->GreenMask == ZEEndian::Big(0x000007E0) && 
				Header->BlueMask  == ZEEndian::Big(0x0000001F)) &&
				!(Header->BitsPerPixel == 16 &&
				Header->RedMask   == ZEEndian::Big(0x00007C00) && 
				Header->GreenMask == ZEEndian::Big(0x000003E0) && 
				Header->BlueMask  == ZEEndian::Big(0x0000001F)) &&
				!(Header->BitsPerPixel == 24 &&
				Header->RedMask   == ZEEndian::Big(0x00FF0000) && 
				Header->GreenMask == ZEEndian::Big(0x0000FF00) && 
				Header->BlueMask  == ZEEndian::Big(0x000000FF)) &&
				!(Header->BitsPerPixel == 32 &&
				Header->RedMask   == ZEEndian::Big(0x00FF0000) && 
				Header->GreenMask == ZEEndian::Big(0x0000FF00) && 
				Header->BlueMask  == ZEEndian::Big(0x000000FF)))
			{
				zeError("Unsupported color mask.");
				return false;
			}
			break;

		default:
			zeError("Unsupported encoding method.");
			return false;
	}

	memset(Palette, 0, 256 * sizeof(ZEARGB32));
	if (Header->BitsPerPixel == 8)
	{
		if (Header->PaletteEntryCount == 0)
			Header->PaletteEntryCount = 256;
		
		if (Header->PaletteEntryCount > 256)
		{
			zeError("Only 256 pallette entries supported.");
			return false;
		}

		File->Seek(14 + Header->Size, ZE_SF_BEGINING);
		if (File->Read(Palette, sizeof(ZEARGB32), Header->PaletteEntryCount) != Header->PaletteEntryCount)
		{
			zeError("Can not load BMP file's palate entries.");
			return false;
		}

		for (ZESize I = 0; I < Header->PaletteEntryCount; I++)
			Palette[I] = ConvertRow32(Palette + I);
	}

	return true;
}

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

static ZETextureData* Load(ZEFile* File)
{
	ZEBitmapHeader Header;
	ZEARGB32 Palette[256];

	if (!LoadHeaders(File, &Header, Palette))
		return NULL;

	ZEPointer<ZEUInt8> Data;
	if (Header.CompressionType == 1)
		Data = LoadCompressedData(File, &Header);
	else
		Data = LoadData(File, &Header);
	
	if (Data == NULL)
	{
		zeError("Can not load pixel data.");
		return NULL;
	}
	
	return ConvertData(&Header, Palette, Data);
}

ZETextureData* ZETextureFileBMP::Load(ZEFile* File)
{
	if (File == NULL)
	{
		zeError("Null file pointer.");
		return NULL;
	}

	ZETextureData* Texture = ::Load(File);
	if (Texture == NULL)
		zeError("Can not load BMP file.");

	return Texture;
}
