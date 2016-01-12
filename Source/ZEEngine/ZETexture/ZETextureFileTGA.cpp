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
#include "ZEPointer/ZEPointer.h"
#include "ZEFile/ZEFile.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZETexturePixelConverter.h"
#include "ZECompression/ZEDecompressorTGARLE.h"

#define ZE_TIT_TYPE_MASK				0x03
#define ZE_TIT_NO_IMAGE					0x00
#define ZE_TIT_INDEXED					0x01
#define ZE_TIT_COLOR					0x02
#define ZE_TIT_GRAYSCALE				0x03
#define ZE_TIT_COMPRESSED				0x08

#define ZE_TIO_RIGHT_TO_LEFT			0x10
#define ZE_TIO_TOP_TO_BOTTOM			0x20

ZEPackStruct
(
	struct ZETargaHeader
	{
		ZEUInt8						IdSize;
		ZEUInt8						ColorMapType;
		ZEUInt8						ImageType;
		
		// Palette
		ZELittleEndian<ZEUInt16>	PaletteFirstEntry;
		ZELittleEndian<ZEUInt16>	PaletteSize;
		ZEUInt8						PaletteEntryBPP;
		
		// Image
		ZELittleEndian<ZEUInt16>	XOrgin;
		ZELittleEndian<ZEUInt16>	YOrgin;
		ZELittleEndian<ZEUInt16>	Width;
		ZELittleEndian<ZEUInt16>	Height;
		ZEUInt8						BPP;
		ZEUInt8						ImageDescriptor;
	}
);

ZEPackStruct
(
	struct ZETargaFooter
	{
		ZELittleEndian<ZEUInt32>	ExtensionOffset;
		ZELittleEndian<ZEUInt32>	DevelopperAreaOffset;
		char						Signature[18];
	}
);

#define ZE_TGA_DECOMPRESS_BUFFER_SIZE 4096

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

static __forceinline void ConvertColorRow(ZEPixelRGBA8* Destination, ZEUInt8* Source, ZESize BPP, ZESize Width)
{
	switch(BPP)
	{
		case 16:
			ZETexturePixelConverter::ConvertBGRX5551(Destination, Source,  Width);
			break;

		case 24:
			ZETexturePixelConverter::ConvertBGR8(Destination, Source,  Width);
			break;

		case 32:
			ZETexturePixelConverter::ConvertBGRA8(Destination, Source, Width);
			break;
	}
}


static ZETextureData* LoadData(ZEFile* File, ZETargaHeader* Header, ZEPixelRGBA8* Palette)
{
	ZESize Width = Header->Width;
	ZESize Height = Header->Height; 
	ZESize PixelSize = GetPixelSize(Header->BPP);
	ZESize BPP = Header->BPP;

	ZESSize Step = (Header->ImageDescriptor & ZE_TIO_TOP_TO_BOTTOM ? 1 : -1);
	ZESize Index = (Header->ImageDescriptor & ZE_TIO_TOP_TO_BOTTOM ? 0 :  Header->Height - 1);

	ZESize DestinationPixelSize = 0;
	ZEPointer<ZETextureData> Texture = new ZETextureData();
	if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_COLOR || (Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_INDEXED)
	{
		Texture->Create(ZEGR_TT_2D, ZEGR_TF_R8G8B8A8_UNORM, 1, 1, Width, Height);
		DestinationPixelSize = 4;
	}
	else
	{
		if (Header->BPP == 8)
		{
			Texture->Create(ZEGR_TT_2D, ZEGR_TF_R8_UNORM, 1, 1, Width, Height);
			DestinationPixelSize = 1;
		}
		else
		{
			Texture->Create(ZEGR_TT_2D, ZEGR_TF_R8G8_UNORM, 1, 1, Width, Height);
			DestinationPixelSize = 2;
		}
	}

	ZEUInt8* Destination = (ZEUInt8*)Texture->GetSurfaces()[0].GetLevels()[0].GetData();
	ZEUInt8* DestinationEnd = (ZEUInt8*)Destination + Height * Width * DestinationPixelSize;
	ZEUInt8* DestinationRow = (ZEUInt8*)Destination + Index * Width * DestinationPixelSize;
	ZESSize	DestinationRowStep = Step * Width * DestinationPixelSize;

	File->Seek(sizeof(ZETargaHeader) + Header->PaletteSize * GetPixelSize(Header->PaletteEntryBPP) + Header->IdSize, ZE_SF_BEGINING);

	ZESize SourceRowSize = Header->Width * PixelSize;
	ZEPointer<ZEUInt8> SourceRow = new ZEUInt8[SourceRowSize];

	if ((Header->ImageType & ZE_TIT_COMPRESSED) == 0)
	{
		if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_COLOR)
		{
			for (ZESize I = 0; I < Height; I++)
			{
				if (File->Read(SourceRow, SourceRowSize, 1) != 1)
					return NULL;

				ConvertColorRow((ZEPixelRGBA8*)DestinationRow, SourceRow, BPP, Width);
				DestinationRow += DestinationRowStep;
			}
		}
		else if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_GRAYSCALE && Header->BPP == 8)
		{
			for (ZESize I = 0; I < Height; I++)
			{
				if (File->Read(DestinationRow, SourceRowSize, 1) != 1)
					return NULL;

				DestinationRow += DestinationRowStep;
			}
		}
		else if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_GRAYSCALE && Header->BPP == 16)
		{
			for (ZESize I = 0; I < Height; I++)
			{
				if (File->Read(SourceRow, SourceRowSize, 1) != 1)
					return NULL;
				ZETexturePixelConverter::ConvertAL8((ZEPixelLA8*)DestinationRow, SourceRow, Width);
				DestinationRow += DestinationRowStep;
			}
		}
		else if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_INDEXED)
		{
			for (ZESize I = 0; I < Height; I++)
			{
				if (File->Read(SourceRow, SourceRowSize, 1) != 1)
					return NULL;

				ZETexturePixelConverter::ConvertIndexed((ZEPixelRGBA8*)DestinationRow, SourceRow, Width, Palette);
				DestinationRow += DestinationRowStep;
			}
		}
	}
	else
	{
		const ZESize BlockSize = 32 * 1024;
		ZEUInt8 Block[BlockSize];

		ZEDecompressorTGARLE Decompressor;
		Decompressor.SetWordSize(PixelSize);
		Decompressor.SetInput(Block);
		Decompressor.SetInputSize(BlockSize);
		Decompressor.SetOutputSize(SourceRowSize);
		Decompressor.SetOutput(SourceRow);

		while (true)
		{
			if (DestinationRow < Destination || DestinationRow >= DestinationEnd)
				break;

			Decompressor.SetInputSize(File->Read(Block, 1, BlockSize));
			if (Decompressor.GetInputSize() ==  0)
				return NULL;

			while(true)
			{
				if (DestinationRow < Destination || DestinationRow >= DestinationEnd)
					break;

				Decompressor.Decompress();
				if (Decompressor.GetState() == ZE_DS_INPUT_PROCESSED)
					break;
				else if (Decompressor.GetState() == ZE_DS_OUTPUT_FULL)
				{
					if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_COLOR)
						ConvertColorRow((ZEPixelRGBA8*)DestinationRow, SourceRow, BPP, Width);
					else if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_GRAYSCALE && Header->BPP == 8)
						memcpy(DestinationRow, SourceRow, SourceRowSize);
					else if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_GRAYSCALE && Header->BPP == 16)
						ZETexturePixelConverter::ConvertAL8((ZEPixelLA8*)DestinationRow, SourceRow, Width);
					else if ((Header->ImageType & ZE_TIT_TYPE_MASK) == ZE_TIT_INDEXED)
						ZETexturePixelConverter::ConvertIndexed((ZEPixelRGBA8*)DestinationRow, SourceRow, Width, Palette);
					
					DestinationRow = DestinationRow + DestinationRowStep;
				}
				else
					return NULL;
			}
		}
	}

	return Texture.Transfer();
}

static bool LoadHeader(ZEFile* File, ZETargaHeader* Header, ZEPixelRGBA8* Palette, bool OutputError)
{
	File->Seek(-(ZESSize)sizeof(ZETargaFooter), ZE_SF_END);
	
	ZETargaFooter Footer;

	ZEInt64 Result;
	Result = File->Read(&Footer, sizeof(ZETargaFooter), 1);
	if (Result != 1)
	{
		if (OutputError)
			zeError("Can not load TGA file footer.");
		return false;
	}

	if (strncmp(Footer.Signature, "TRUEVISION-XFILE.", 17) != 0)
	{
		if (ZEFileInfo(File->GetPath()).GetExtension().Lower() != ".tga")
		{
			if (OutputError)
				zeError("Can not identifty the file. File is not a TGA 2.0 file and it's extension is not .TGA.");
			return false;
		}
	}

	File->Seek(0, ZE_SF_BEGINING);
	Result = File->Read(Header, sizeof(ZETargaHeader), 1);
	if (Result != 1)
	{
		if (OutputError)
			zeError("Can not load TGA file header.");
		return false;
	}

	if (Header->BPP != 8 && Header->BPP != 16 && Header->BPP != 24 && Header->BPP != 32)
	{
		if (OutputError)
			zeError("Unsupported pixel size.");
		return false;
	}

	if (Header->Height == 0 || Header->Width == 0)
	{
		if (OutputError)
			zeError("Corrupted or malicious TGA file. Zero width or height.");
		return false;
	}

	if (Header->Width * Header->Height * 4 == 0)
	{
		if (OutputError)
			zeError("Corrupted or malicious TGA file. 32bit size overflow detected.");
		return false;
	}

	if (Header->ImageType & ZE_TIT_COMPRESSED)
	{
		ZESize PotentialSize = Header->Width * Header->Height * (Header->BPP / 8) / 128;
		if (File->GetSize() < PotentialSize) 
		{
			if (OutputError)
				zeError("Corrupted or malicious TGA file. File size is too small.");
			return false;
		}
	}
	else
	{
		ZESize PotentialSize = Header->Width * Header->Height * (Header->BPP / 8);
		if (File->GetSize() < PotentialSize) 
		{
			if (OutputError)
				zeError("Corrupted or malicious TGA file. File size is too small.");
			return false;
		}
	}

	if ((Header->ImageDescriptor & ZE_TIO_RIGHT_TO_LEFT) != 0)
	{
		if (OutputError)
			zeError("Right to left images are not supported.");
		return false;
	}

	if (Header->ImageType != ZE_TIT_INDEXED && 
		Header->ImageType != ZE_TIT_GRAYSCALE &&
		Header->ImageType != ZE_TIT_COLOR && 
		Header->ImageType != (ZE_TIT_COMPRESSED | ZE_TIT_INDEXED) && 
		Header->ImageType != (ZE_TIT_COMPRESSED | ZE_TIT_GRAYSCALE) && 
		Header->ImageType != (ZE_TIT_COMPRESSED | ZE_TIT_COLOR))
	{
		if (OutputError)
			zeError("Unsupported image type.");
		return false;
	}

	if ((Header->ImageDescriptor & ZE_TIO_RIGHT_TO_LEFT) != 0)
	{
		if (OutputError)
			zeError("Right to left image data is not supported.");
		return false;
	}

	if (Header->ColorMapType == 1)
	{
		if (Header->BPP != 8)
		{
			if (OutputError)
				zeError("Unsupported indexed pixel size.");
			return false;
		}

		ZESize PalettePixelSize = GetPixelSize(Header->PaletteEntryBPP);
		if (PalettePixelSize != 2 && PalettePixelSize != 3 && PalettePixelSize != 4)
		{
			if (OutputError)
				zeError("Unsupported palette entry size. Only 24bit and 32bit palette entries supported.");
			return false;
		}

		memset(Palette, 0, PalettePixelSize * 256);

		File->Seek(sizeof(ZETargaHeader) + Header->IdSize, ZE_SF_BEGINING);

		ZESize PaletteSize = (Header->PaletteSize > 256 ? 256 : Header->PaletteSize);
		ZEUInt32 PaletteTemp[256];
		Result = File->Read(PaletteTemp, PalettePixelSize, PaletteSize);
		if (Result != PaletteSize)
		{
			zeError("Can not load palette.");
			return false;
		}

		if (PalettePixelSize == 4)
			ZETexturePixelConverter::ConvertBGRA8(Palette, (ZEUInt8*)PaletteTemp, PaletteSize);
		if (PalettePixelSize == 3)
			ZETexturePixelConverter::ConvertBGR8(Palette, (ZEUInt8*)PaletteTemp, PaletteSize);
		else
			ZETexturePixelConverter::ConvertBGRX5551(Palette, (ZEUInt8*)PaletteTemp, PaletteSize);
	}

	if (Header->ImageType == ZE_TIT_GRAYSCALE && (Header->BPP != 8 && Header->BPP != 16))
	{
		if (OutputError)
			zeError("Only 8-bit or 16-bit grayscale images are supported.");
		return false;
	}

	return true;
}

static ZETextureData* LoadImage(ZEFile* File)
{
	ZETargaHeader Header;
	ZEPixelRGBA8 Palette[256];

	if (!LoadHeader(File, &Header, Palette, true))
		return NULL;
	
	return LoadData(File, &Header, Palette);
}

bool ZETextureFileTGA::LoadInfo(ZETextureDataInfo* Info, ZEFile* File)
{
	if (File == NULL || !File->IsOpen())
		return false;

	ZEPixelRGBA8 Palette[256];
	ZETargaHeader Header;
	if (!LoadHeader(File, &Header, Palette, false))
		return false;

	Info->Width = Header.Width;
	Info->Height = Header.Height;
	if (Header.ImageType == ZE_TIT_COLOR || Header.ImageType == ZE_TIT_INDEXED)
	{
		Info->PixelFormat = ZEGR_TF_R8G8B8A8_UNORM;
	}
	else
	{
		if (Header.BPP == 8)
			Info->PixelFormat = ZEGR_TF_R8_UNORM;
		else
			Info->PixelFormat = ZEGR_TF_R8G8_UNORM;
	}

	Info->Type = ZEGR_TT_2D;
	Info->SurfaceCount = 1;
	Info->LevelCount = 1;

	return true;
}

ZETextureData* ZETextureFileTGA::Load(ZEFile* File)
{
	if (File == NULL || !File->IsOpen())
	{
		zeError("Null file pointer.");
		return NULL;
	}

	ZETextureData* Texture = LoadImage(File);
	if (Texture == NULL)
		zeError("Can not load TGA file.");

	return Texture;
}
