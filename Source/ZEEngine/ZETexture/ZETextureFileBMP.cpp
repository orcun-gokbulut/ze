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
#include "ZEPointer/ZEPointer.h"
#include "ZEFile/ZEFile.h"
#include "ZETexturePixelConverter.h"
#include "ZECompression/ZEDecompressorBMPRLE.h"
#include "ZEMath/ZEMath.h"

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


static ZETextureData* LoadData(ZEFile* File, ZEBitmapHeader* Header, ZEPixelRGBA8* Palette)
{
	File->Seek(Header->DataPosition, ZE_SF_BEGINING);

	ZESize PixelSize = GetPixelSize(Header->BitsPerPixel);
	ZESSize Width = Header->Width;
	ZESSize Height = (Header->Height < 0 ? -Header->Height : Header->Height);

	ZEPointer<ZETextureData> Texture = new ZETextureData();
	Texture->Create(ZE_TT_2D, ZE_TPF_I8_4, 1, 1, (ZEUInt)Width, (ZEUInt)Height);
	
	ZEPixelRGBA8* Destination = (ZEPixelRGBA8*)Texture->GetSurfaces()[0].GetLevels()[0].GetData();
	ZEPixelRGBA8* DestinationRow = Destination + (Header->Height > 0 ? Height - 1 : 0) * Width;
	ZESSize DestinationRowStep = (Header->Height > 0 ? -1 : 1) * Width;
	ZEPixelRGBA8* DestinationEnd = Destination + Width * Height;

	ZESize Align = (Header->BitsPerPixel / 8) * Width;
	Align += (Align % 4) != 0 ? 4 - (Align % 4) : 0;

	if (Header->CompressionType == 1)
	{
		const ZESize BlockSize = 32 * 1024;
		ZEUInt8 Block[BlockSize];
		ZESize BlockCount = Header->DataSize / BlockSize;
		ZESize LastBlockSize = BlockSize;
		if (Header->DataSize % BlockSize != 0)
		{
			BlockCount++;
			LastBlockSize = Header->DataSize % BlockSize;
		}

		ZEPointer<ZEUInt16> DecompressedRow = new ZEUInt16[Width];
		ZEDecompressorBMPRLE Decompressor;
		Decompressor.SetInput(Block);
		Decompressor.SetInputSize(BlockSize);
		Decompressor.SetOutput(DecompressedRow);
		Decompressor.SetOutputSize(Width);

		for (ZESize I = 0; I < BlockCount; I++)
		{
			if (I == BlockCount - 1)
				Decompressor.SetInputSize(LastBlockSize);

			ZESize BytesRead = File->Read(Block, 1, Decompressor.GetInputSize());
			if (BytesRead !=  Decompressor.GetInputSize())
				return NULL;
			Decompressor.SetInput(Block);

			while(true)
			{
				if (DestinationRow < Destination || DestinationRow >= DestinationEnd)
					break;

				Decompressor.Decompress();
				if(Decompressor.GetState() == ZE_DS_INPUT_PROCESSED)
				{
					break;
				}
				else if (Decompressor.GetState() == ZE_DS_OUTPUT_FULL)
				{
					ZETexturePixelConverter::ConvertIndexed16(DestinationRow, DecompressedRow, Width, Palette);
					DestinationRow += DestinationRowStep;
				}
				else
				{
					return NULL;
				}
			}
		}
	}
	else
	{
		ZEPointer<ZEUInt8> SourceRow = new ZEUInt8[Align];
		switch(Header->BitsPerPixel)
		{
			case 8:
				for (ZESize I = 0; I < Height; I++)
				{
					if (File->Read(SourceRow, Align, 1) != 1)
						return NULL;

					ZETexturePixelConverter::ConvertIndexed(DestinationRow, SourceRow, Width, Palette);
					DestinationRow += DestinationRowStep;
				}
				break;

			case 16:
				if (Header->GreenMask == ZEEndian::Big(0x000003E0))
				{
					for (ZESize I = 0; I < Height; I++)
					{
						if (File->Read(SourceRow, Align, 1) != 1)
							return NULL;

						ZETexturePixelConverter::ConvertBGRX5551(DestinationRow, SourceRow, Width);
						DestinationRow += DestinationRowStep;
					}
				}
				else
				{
					for (ZESize I = 0; I < Height; I++)
					{
						if (File->Read(SourceRow, Align, 1) != 1)
							return NULL;

						ZETexturePixelConverter::ConvertBGR565(DestinationRow, SourceRow, Width);
						DestinationRow += DestinationRowStep;
					}
				}
				break;		

			case 24:
				for (ZESize I = 0; I < Height; I++)
				{
					if (File->Read(SourceRow, Align, 1) != 1)
						return NULL;

					ZETexturePixelConverter::ConvertBGR8(DestinationRow, SourceRow, Width);
					DestinationRow += DestinationRowStep;
				}
				break;

			case 32:
				for (ZESize I = 0; I < Height; I++)
				{
					if (File->Read(SourceRow, Align, 1) != 1)
						return NULL;

					ZETexturePixelConverter::ConvertBGRX8(DestinationRow, SourceRow, Width);
					DestinationRow += DestinationRowStep;
				}
				break;
		}
	}

	return Texture.Transfer();
}

static bool LoadHeader(ZEFile* File, ZEBitmapHeader* Header, ZEPixelRGBA8* Palette, bool EnableErrorOutput)
{
	ZEUInt64 FileSize = File->GetSize();

	ZEUInt64 Result;
	File->Seek(0, ZE_SF_BEGINING);
	Result = File->Read(Header, 1, sizeof(ZEBitmapHeader));
	if (Result < 54)
	{
		if (EnableErrorOutput)
			zeError("Can not load BMP file header.");
		return false;
	}

	if (Header->Header != 'MB')
	{
		if (EnableErrorOutput)
			zeError("Unknown BMP file format.");
		return false;
	}

	ZEInt Width = ZEMath::Abs((ZEInt32)Header->Width);
	ZEInt Height = ZEMath::Abs((ZEInt32)Header->Height);

	if (Header->Height == 0 || Header->Width == 0)
	{
		if (EnableErrorOutput)
			zeError("Corrupted or malicious BMP file. Zero width or height.");
		return false;
	}

	if (Header->Width < 0)
	{
		if (EnableErrorOutput)
			zeError("Corrupted or malicious BMP file. Negative width.");
		return false;
	}

	if (Width > 65536 || Height > 65536)
	{
		if (EnableErrorOutput)
			zeError("Corrupted or malicious BMP file. Width or height of a BMP file can not exceed 65536.");
		return false;
	}

	if (Width * Height * 4 == 0)
	{
		if (EnableErrorOutput)
			zeError("Corrupted or malicious BMP file. 32bit size overflow detected.");
		return false;
	}

	if (Header->DataSize > FileSize) 
	{
		if (EnableErrorOutput)
			zeError("Corrupted or malicious BMP file. File size is too small.");
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
		if (EnableErrorOutput)
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
				if (EnableErrorOutput)
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
				if (EnableErrorOutput)
					zeError("Unsupported color mask.");
				return false;
			}
			break;

		default:
			if (EnableErrorOutput)
				zeError("Unsupported encoding method.");
			return false;
	}

	memset(Palette, 0, 256 * sizeof(ZEPixelRGBA8));
	if (Header->BitsPerPixel == 8)
	{
		if (Header->PaletteEntryCount == 0)
			Header->PaletteEntryCount = 256;
		
		if (Header->PaletteEntryCount > 256)
		{
			if (EnableErrorOutput)
				zeError("Only 256 pallette entries supported.");
			return false;
		}

		ZEUInt32 PaletteData[256];
		File->Seek(14 + Header->Size, ZE_SF_BEGINING);
		if (File->Read(PaletteData, sizeof(ZEPixelRGBA8), Header->PaletteEntryCount) != Header->PaletteEntryCount)
		{
			if (EnableErrorOutput)
				zeError("Can not load BMP file's palate entries.");
			return false;
		}

		ZETexturePixelConverter::ConvertBGRX8(Palette, PaletteData, Header->PaletteEntryCount);
	}

	return true;
}

static ZETextureData* Load(ZEFile* File)
{
	if (File == NULL || !File->IsOpen())
	{
		zeError("File is not opened.");
		return NULL;
	}

	ZEBitmapHeader Header;
	ZEPixelRGBA8 Palette[256];
	if (!LoadHeader(File, &Header, Palette, true))
		return NULL;

	ZETextureData* Data = LoadData(File, &Header, Palette);
	if (Data == NULL)
		zeError("Can not load pixel data.");
	
	return Data;
}

bool ZETextureFileBMP::LoadInfo(ZETextureDataInfo* Info, ZEFile* File)
{
	if (File == NULL || !File->IsOpen())
	{
		zeError("File is not opened.");
		return NULL;
	}

	ZEPixelRGBA8 Palette[256];
	ZEBitmapHeader Header;
	if (!LoadHeader(File, &Header, Palette, false))
		return false;

	Info->Width = ZEMath::Abs((ZEInt16)Header.Width);
	Info->Height = ZEMath::Abs((ZEInt16)Header.Height);
	Info->PixelFormat = ZE_TPF_I8_4;
	Info->Type = ZE_TT_2D;
	Info->SurfaceCount = 1;
	Info->LevelCount = 1;

	return true;
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
