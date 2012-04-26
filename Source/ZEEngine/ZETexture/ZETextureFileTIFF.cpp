//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileTIFF.cpp
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

#include "ZETextureFileTIFF.h"

#include "ZEError.h"
#include "ZEPacking.h"
#include "ZEEndian.h"
#include "ZETextureData.h"
#include "ZEDS/ZEPointer.h"
#include "ZEFile/ZEFile.h"

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
	struct ZETIFFHeader
	{
		ZEUInt16		Endianness;
		ZEUInt16		Identifier;
		ZEUInt32		EntryListOffset;
	};
);

ZEPackStruct
(
	struct ZETIFFEntry
	{
		ZEUInt16		TagId;
		ZEUInt16		TagType;
		ZEUInt32		Count;
		ZEUInt8			Value[8];
	};
);

struct ZETIFFInfo
{
	ZESize			Width;
	ZESize			Height;
	int				PhotometricInterpretation;
	int				SamplesPerPixel;
	int				BitsPerSample;
	int				Compression;
	ZESize			RowPerStript;

	ZESize			StripOffsetCount;
	ZESize			StripOffsets;

	ZESize			StripSizeCount;
	ZESize			StripSizes;

	ZESize			PaletteOffset;
};


static ZEUInt32* LoadEntryArray(ZEFile* File, ZESize Offset, ZESize Count, ZEEndianType Endianness)
{
	ZEUInt32* Output;

	Output = new ZEUInt32[Count];
	File->Seek(Offset * sizeof(ZEUInt32), ZE_SF_BEGINING);
		
	if (File->Read(Output, Count * sizeof(ZEUInt32), 1) != 1)
	{
		zeError("Can not read strip offsets.");
		return NULL;
	}

	for (ZESize I = 0; I < Count; I++)
		Output[I] = ZEEndian::Uni(Output[I], Endianness);
	
	return Output;
}

static ZEUInt LoadEntry(ZETIFFEntry* Entry, ZESize& Count, ZEEndianType Endian)
{
	Count = ZEEndian::Uni(*(ZEUInt32*)Entry->Value, Endian);

	switch(Entry->TagType)
	{
		case 2: // ZEUInt16
			return ZEEndian::Uni(*(ZEUInt16*)Entry->Value, Endian);

		case 3: // ZEUInt32
			return ZEEndian::Uni(*(ZEUInt32*)Entry->Value, Endian);

		default:
			return 0;
	}
}

static ZEUInt LoadEntry(ZETIFFEntry* Entry, ZEEndianType Endian)
{
	switch(Entry->TagType)
	{
		case 2: // ZEUInt16
			return ZEEndian::Uni(*(ZEUInt16*)Entry->Value, Endian);

		case 3: // ZEUInt32
			return ZEEndian::Uni(*(ZEUInt32*)Entry->Value, Endian);

		default:
			return 0;
	}
}

static ZEARGB32* LoadPallete(ZEFile* File, ZETIFFInfo* Info, ZEEndianType Endianness)
{
	ZEUInt16 RawPalette[256 * 3];
	File->Seek(Info->PaletteOffset * sizeof(ZEUInt32), ZE_SF_BEGINING);

	if (File->Read(RawPalette, 3 * 256 * sizeof(ZEUInt16), 1) != 1)
	{
		zeError("Can not read strip offsets.");
		return NULL;
	}

	ZEARGB32* Palette = new ZEARGB32[256];
	for (ZESize I = 0; I < 256; I++)
	{
		Palette[I].A = 255;
		Palette[I].R = ZEEndian::Uni(RawPalette[3 * I], Endianness) / 256;
		Palette[I].G = ZEEndian::Uni(RawPalette[3 * I + 1], Endianness) / 256;
		Palette[I].B = ZEEndian::Uni(RawPalette[3 * I + 2], Endianness) / 256;
	}
	return Palette;
}

static ZETextureData* LoadData(ZEFile* File, ZETIFFInfo* Info, ZEARGB32* Palette)
{
	return NULL;
}



static bool LoadHeader(ZEFile* File, ZETIFFInfo* Info, ZEARGB32* Palette)
{
	// Read Header
	ZETIFFHeader Header;
	ZEUInt64 Result = File->Read(&Header, sizeof(ZETIFFHeader), 1);
	if (Result != 1)
	{
		zeError("Can not read TIFF file header.");
		return false;
	}

	ZEEndianType Endianness;
	if (Header.Endianness == 'II')
		Endianness = ZE_ET_LITTLE;
	else if (Header.Endianness == 'MM')
		Endianness = ZE_ET_BIG;
	else
	{
		zeError("File is not a TIFF file.");
		return false;
	}

	if (ZEEndian::Uni(Header.Identifier, Endianness) != 42)
	{
		zeError("File is not a TIFF file.");
		return false;
	}

	File->Seek(Header.EntryListOffset, ZE_SF_BEGINING);
		
	ZELittleEndian<ZEUInt16> EntryCount;
	Result = File->Read(&EntryCount, sizeof(ZEUInt16), 1);
	if (Result != 1)
	{
		zeError("Can not read TIFF file entry count.");
		return false;
	}
	EntryCount = ZEEndian::Uni(EntryCount, Endianness);

	bool HasWidth = false;
	bool HasHeight = false;
	bool HasPhotrometricInterpretation = false;;
	Info->Compression = 1;
	Info->BitsPerSample = 1;
	Info->SamplesPerPixel = 1;
	Info->StripOffsetCount = 0;
	Info->StripSizeCount = 0;

	for (ZESize I = 0; I < EntryCount; I++)
	{
		ZETIFFEntry Entry;
		Result = File->Read(&Entry, sizeof(ZETIFFEntry), 1);
		if (Result != 1)
		{
			zeError("Can not read TIFF entry count.");
			return false;
		}

		switch(Entry.TagId)
		{
			case 0x0100: // Width
				HasWidth = true;
				Info->Height = LoadEntry(&Entry, Endianness);
				break;

			case 0x0101: // Height
				HasHeight = true;
				Info->Height = LoadEntry(&Entry, Endianness);
				break;

			case 0x0102: // Bits per Sample
				Info->BitsPerSample = LoadEntry(&Entry, Endianness);
				break;

			case 0x0103: // Compression
				Info->Compression = LoadEntry(&Entry, Endianness);
				break;

			case 0x0106: // PhotrometricInterpretation
				HasPhotrometricInterpretation = true;
				Info->PhotometricInterpretation = LoadEntry(&Entry, Endianness);
				break;

			case 0x0111: // Strip Offsets
				Info->StripOffsets = LoadEntry(&Entry, Info->StripOffsetCount, Endianness);
				break;

			case 0x0115: // Samples per Pixel
				Info->SamplesPerPixel = LoadEntry(&Entry, Endianness);
				break;

			case 0x0116: // Row Per Strip
				Info->RowPerStript = LoadEntry(&Entry, Endianness);
				break;

			case 0x0117: // StriptSizes
				Info->StripSizes = LoadEntry(&Entry, Info->StripSizeCount, Endianness);
				break;

			default:
				break;
		}
	}

	if (!HasPhotrometricInterpretation)
	{
		zeError("Photrometric Interpretation tag is missing.");
		return false;
	}

	if (!HasWidth)
	{
		zeError("Image Width tag is missing.");
		return false;
	}

	if (!HasHeight)
	{
		zeError("Image Length tag is missing.");
		return false;
	}

	// Check Support
	if (Info->Compression != 0)
	{
		zeError("Compressed TIFF files are not supported.");
		return false;
	}

	// Allowed Types
	// G8
	// A8G8

	// G16
	// A16G16
	// Indexed

	// A1R5G5B5
	// X1R5G5B5
	// R5G6B5

	// RGB888
	// ARGB8888


	// Check Image Type
	// Check Bits Per Pixel
	// Check Compression
	// Read Palette

	return true;
}

static ZETextureData* Load(ZEFile* File)
{
	/*ZETargaHeader Header;
	ZEARGB32 Palette[256];

	if (!LoadHeader(File, &Header, Palette))
		return NULL;
	
	return LoadData(File, &Header, Palette);*/

	return NULL;
}

bool ZETextureFileTIFF::CanLoad(ZEFile* File)
{
	/*File->Seek(-(ZESSize)sizeof(ZETargaFooter), ZE_SF_END);

	ZETargaFooter Footer;

	ZEInt64 Result;
	Result = File->Read(&Footer, sizeof(ZETargaFooter), 1);
	if (Result != 1)
		return false;

	if (strncmp(Footer.Signature, "TRUEVISION-XFILE.", 17) == 0)
		return true;

	if (ZEFile::GetFileExtension(File->GetFilePath()).Lower() != "tga")
		return false;*/

	return true;
}

ZETextureData* ZETextureFileTIFF::Load(ZEFile* File)
{
	if (File == NULL)
	{
		zeError("Null file pointer.");
		return NULL;
	}
	ZETextureData* Texture = ::Load(File);
	if (Texture == NULL)
		zeError("Can not load TGA file.");

	return Texture;
}
