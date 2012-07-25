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
#include "ZEEndian.h"
#include "ZETextureData.h"
#include "ZEDS/ZEPointer.h"
#include "ZEFile/ZEFile.h"
#include "ZETexturePixelConverter.h"
#include "ZETextureData.h"
#include "ZEPacking.h"
#include "ZETypes.h"
#include "ZEEndian.h"
#include "ZEDS/ZEArray.h"

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
		ZEUInt16		Id;
		ZEUInt16		Type;
		ZEUInt32		Count;
		ZEUInt8			Value[8];
	};
);

enum ZETIFFChunkType
{
	ZE_TIFF_CT_STRIP,
	ZE_TIFF_CT_TILE
};

enum ZETIFFCompressionType
{
	ZE_TIFF_CT_UNCOMPRESSED = 0,
	ZE_TIFF_CT_PACKBIT = 32773,
	ZE_TIFF_CT_LZW = 5
};

enum ZETIFFPixelFormat
{
	ZE_TIFF_PT_NONE,
	ZE_TIFF_PT_INDEXED,
	ZE_TIFF_PT_L8,
	ZE_TIFF_PT_LA8,
	ZE_TIFF_PT_L16,
	ZE_TIFF_PT_LA16,
	ZE_TIFF_PT_RGB8,
	ZE_TIFF_PT_RGBA8,
	ZE_TIFF_PT_RGB16,
	ZE_TIFF_PT_RGBA16
};

struct ZETIFFInfo
{
	ZEEndianness			Endianness;
	ZESize					Width;
	ZESize					Height;
	ZETIFFCompressionType	Compression;
	ZETIFFPixelFormat		PixelFormat;

	ZETIFFChunkType			ChunkType;
	ZESize					ChunkWidth;
	ZESize					ChunkHeight;
	ZETIFFEntry				ChunkOffsetsEntry;
	ZETIFFEntry				ChunkSizesEntry;
	ZEArray<ZEUInt32>		ChunkOffsets;
	ZEArray<ZEUInt32>		ChunkSizes;

	ZETIFFEntry				PaletteEntry;
	ZEPixelRGBA8				Palette[256];
};

ZESize GetPixelSize(ZETIFFPixelFormat Format)
{
	switch(Format)
	{
		case ZE_TIFF_PT_INDEXED:
			return 1;

		case ZE_TIFF_PT_L8:
			return 1;

		case ZE_TIFF_PT_LA8:
			return 2;

		case ZE_TIFF_PT_L16:
			return 2;

		case ZE_TIFF_PT_LA16:
			return 4;

		case ZE_TIFF_PT_RGB8:
			return 3;

		case ZE_TIFF_PT_RGBA8:
			return 4;

		case ZE_TIFF_PT_RGB16:
			return 6;

		case ZE_TIFF_PT_RGBA16:
			return 8;

		default:
			return 0;
	}

}

static ZETexturePixelFormat GetTextureFormat(ZETIFFPixelFormat PixelFormat)
{
	switch(PixelFormat)
	{
		case ZE_TIFF_PT_L8:
			return ZE_TPF_L8;

		case ZE_TIFF_PT_LA8:
			return ZE_TPF_LA8;

		case ZE_TIFF_PT_L16:
			return ZE_TPF_L16;

		case ZE_TIFF_PT_LA16:
			return ZE_TPF_LA16;

		case ZE_TIFF_PT_INDEXED:
		case ZE_TIFF_PT_RGB8:
		case ZE_TIFF_PT_RGBA8:
			return ZE_TPF_RGBA8;

		case ZE_TIFF_PT_RGB16:
		case ZE_TIFF_PT_RGBA16:
			return ZE_TPF_RGBA16;
	}

	return ZE_TPF_NOTSET;
}

static bool LoadEntryArray(ZEArray<ZEUInt32> Output, ZETIFFEntry* Entry, ZEFile* File, ZEEndianness Endianness)
{
	ZESize Count = ZEEndian::Uni(Entry->Count, Endianness);
	ZESize Offset = ZEEndian::Uni(*(ZEUInt32*)Entry->Value, Endianness);

	Output.SetCount(Count);
	File->Seek(Offset * sizeof(ZEUInt32), ZE_SF_BEGINING);
		
	if (Entry->Type == 0)
	{
		if (File->Read(Output.GetCArray(), Count * sizeof(ZEUInt32), 1) != 1)
		{
			zeError("Can not read strip offsets.");
			return false;
		}

		for (ZESize I = 0; I < Entry->Count; I++)
			Output[I] = ZEEndian::Uni(Output[I], Endianness);

	}
	else
	{
		ZESize ReadCount = 0;
		ZESize TotalReadCount = 0;
		while(TotalReadCount != Count)
		{
			ZEUInt16 Buffer[2048];
			ZESize ReadCount = TotalReadCount - ReadCount > 2048 ? 2048 : TotalReadCount - ReadCount;
			
			ZESize CurrentReadCount = File->Read(Buffer, sizeof(ZEUInt16), ReadCount);
			if (CurrentReadCount == 0)
				return false;

			for (ZESize I = 0; I < CurrentReadCount; I++)
				Output[TotalReadCount + I] = ZEEndian::Uni(Buffer[I], Endianness);
			
			TotalReadCount += ReadCount;
		}
	}
}

static ZESize LoadEntryArray(ZEUInt32* Output, ZESize OutputCount, ZETIFFEntry* Entry, ZEFile* File, ZEEndianness Endianness)
{
	ZESize Count = OutputCount > ZEEndian::Uni(Entry->Count, Endianness) ? OutputCount : ZEEndian::Uni(Entry->Count, Endianness);
	ZESize Offset = ZEEndian::Uni(*(ZEUInt32*)Entry->Value, Endianness);

	File->Seek(Offset * sizeof(ZEUInt32), ZE_SF_BEGINING);

	if (Entry->Type == 0)
	{
		if (File->Read(Output, Count * sizeof(ZEUInt32), 1) != 1)
		{
			zeError("Can not read strip offsets.");
			return 0;
		}

		for (ZESize I = 0; I < Entry->Count; I++)
			Output[I] = ZEEndian::Uni(Output[I], Endianness);

	}
	else
	{
		ZESize ReadCount = 0;
		ZESize TotalReadCount = 0;
		while(TotalReadCount != Count)
		{
			ZEUInt16 Buffer[2048];
			ZESize ReadCount = TotalReadCount - ReadCount > 2048 ? 2048 : TotalReadCount - ReadCount;

			ZESize CurrentReadCount = File->Read(Buffer, sizeof(ZEUInt16), ReadCount);
			if (CurrentReadCount == 0)
				return 0;

			for (ZESize I = 0; I < CurrentReadCount; I++)
				Output[TotalReadCount + I] = ZEEndian::Uni(Buffer[I], Endianness);

			TotalReadCount += ReadCount;
		}
	}

	return Count;
}

static ZEUInt LoadEntry(ZETIFFEntry* Entry, ZEEndianness Endian)
{
	switch(Entry->Type)
	{
		case 2: // ZEUInt16
			return ZEEndian::Uni(*(ZEUInt16*)Entry->Value, Endian);

		case 3: // ZEUInt32
			return ZEEndian::Uni(*(ZEUInt32*)Entry->Value, Endian);

		default:
			return 0;
	}
}

static bool UncompressPackBit(void* Destination, ZESize DestinationSize, void* Source, ZESize SourceSize)
{
	ZEUInt8* SourceCurrent = (ZEUInt8*)Source;
	ZEUInt8* SourceEnd = SourceCurrent + SourceSize;
	ZEUInt8* DestinationCurrent = (ZEUInt8*)Destination;
	ZEUInt8* DestinationEnd = DestinationCurrent + DestinationSize;

	while(SourceCurrent != SourceEnd)
	{
		ZESize Count = (*DestinationCurrent & 0x7F) + 1;
		if (*DestinationCurrent & 0x80)
		{ 
			if (SourceCurrent + 1 >= SourceEnd)
				return false;

			if (DestinationCurrent + Count > DestinationEnd)
				return false;

			memset(DestinationCurrent, SourceCurrent[1], Count);

			DestinationCurrent += Count;
			SourceCurrent += 2;
		}
		else
		{
			if (SourceCurrent + Count + 1 > SourceEnd)
				return false;

			if (DestinationCurrent + Count > DestinationEnd)
				return false;


			memcpy(DestinationCurrent, SourceCurrent + 1, Count);

			DestinationCurrent += Count;
			SourceCurrent += Count + 1;
		}
	}

	return true;
}

static bool UncompressLZW(void* Destination, ZESize DestinationSize, void* Source, ZESize SourceCount)
{

	return false;
}


static bool LoadHeader(ZEFile* File, ZETIFFInfo* Info)
{
	// Read Header
	ZETIFFHeader Header;
	ZEUInt64 Result = File->Read(&Header, sizeof(ZETIFFHeader), 1);
	if (Result != 1)
	{
		zeError("Can not read TIFF file header.");
		return false;
	}

	if (Header.Endianness == 'II')
		Info->Endianness = ZE_ET_LITTLE;
	else if (Header.Endianness == 'MM')
		Info->Endianness = ZE_ET_BIG;
	else
	{
		zeError("File is not a TIFF file.");
		return false;
	}

	if (ZEEndian::Uni(Header.Identifier, Info->Endianness) != 42)
	{
		zeError("File is not a TIFF file.");
		return false;
	}

	File->Seek(Header.EntryListOffset, ZE_SF_BEGINING);
	
	Info->ChunkType = ZE_TIFF_CT_STRIP;
	Info->Compression = ZE_TIFF_CT_UNCOMPRESSED;
	int PhotometricInterpretation = -1;
	ZEUInt32 BitsPerSample[4] = {0, 0, 0, 0};
	ZEUInt32 SamplesPerPixel = 1;
	ZEUInt32 ExtraSamples = 0;
	ZEUInt32 Orientation = 1;
	ZEUInt32 PlanarConfiguration = 1;

	ZELittleEndian<ZEUInt16> EntryCount;
	Result = File->Read(&EntryCount, sizeof(ZEUInt16), 1);
	if (Result != 1)
	{
		zeError("Can not read TIFF file entry count.");
		return false;
	}
	EntryCount = ZEEndian::Uni(EntryCount, Info->Endianness);

	for (ZESize I = 0; I < EntryCount; I++)
	{
		ZETIFFEntry Entry;
		Result = File->Read(&Entry, sizeof(ZETIFFEntry), 1);
		if (Result != 1)
		{
			zeError("Can not read TIFF entry count.");
			return false;
		}

		switch(Entry.Id)
		{
			case 0x0100: // Width
				Info->Height = LoadEntry(&Entry, Info->Endianness);
				break;

			case 0x0101: // Height
				Info->Height = LoadEntry(&Entry, Info->Endianness);
				break;

			case 0x0102: // Bits per Sample
				LoadEntryArray(BitsPerSample, 4, &Entry, File, Info->Endianness);
				break;

			case 0x0103: // Compression
				Info->Compression = (ZETIFFCompressionType)LoadEntry(&Entry, Info->Endianness);
				break;

			case 0x0106: // PhotrometricInterpretation
				PhotometricInterpretation = LoadEntry(&Entry, Info->Endianness);
				break;

			case 0x011C: // Planar Configuration
				PlanarConfiguration = LoadEntry(&Entry, Info->Endianness);
				break;
			case 0x0111: // Strip Offsets
				Info->ChunkOffsetsEntry = Entry;
				break;

			case 0x0112: // Orientation
				Orientation = LoadEntry(&Entry, Info->Endianness);
				break;

			case 0x0115: // Samples per Pixel
				SamplesPerPixel = LoadEntry(&Entry, Info->Endianness);
				break;

			case 0x0116: // Row Per Strip
				Info->ChunkHeight = LoadEntry(&Entry, Info->Endianness);
				break;

			case 0x0117: // StriptSizes
				Info->ChunkSizesEntry = Entry;
				break;

			case 0x0152: // ExtraSamples
				ExtraSamples = LoadEntry(&Entry, Info->Endianness);
				break;

			case 142: // TileWidth
				Info->ChunkWidth = LoadEntry(&Entry, Info->Endianness);
				Info->ChunkType = ZE_TIFF_CT_TILE;
				break;

			case 143: // TileHeight
				Info->ChunkHeight = LoadEntry(&Entry, Info->Endianness);
				Info->ChunkType = ZE_TIFF_CT_TILE;
				break;

			case 144: // TileOffsets
				Info->ChunkOffsetsEntry = Entry;
				Info->ChunkType = ZE_TIFF_CT_TILE;
				break;

			case 145: // TileSizes
				Info->ChunkSizesEntry = Entry;
				Info->ChunkType = ZE_TIFF_CT_TILE;
				break;

			default:
				break;
		}
	}

	if (PhotometricInterpretation == -1)
	{
		zeError("Photrometric Interpretation tag is missing.");
		return false;
	}

	if (Info->Width == 0)
	{
		zeError("Image Width tag is missing.");
		return false;
	}

	if (Info->Height == 0)
	{
		zeError("Image Length tag is missing.");
		return false;
	}

	if (PlanarConfiguration != 1)
	{
		zeError("Unsupported planar configuration. Planar TIFF files are not supported.");
		return false;
	}

	if (Orientation != 1)
	{
		zeError("Unsupported image orientation.");
		return false;
	}

	if (ExtraSamples == 1)
	{
		zeError("Pre-multiplied alpha values are not supported.");
		return false;
	}

	switch(PhotometricInterpretation)
	{
		case 1: // Black is Zero
			if (SamplesPerPixel == 1 && BitsPerSample[0] == 8)
				Info->PixelFormat = ZE_TIFF_PT_L8;	
			else if (SamplesPerPixel == 1 && BitsPerSample[0] == 16) 
				Info->PixelFormat = ZE_TIFF_PT_L16;	
			else if (SamplesPerPixel == 2 && BitsPerSample[0] == 8 && BitsPerSample[1] == 8)  
				Info->PixelFormat = ZE_TIFF_PT_LA8;
			else if (SamplesPerPixel == 2 && ExtraSamples == 2 && BitsPerSample[0] == 16 && BitsPerSample[0] == 16) 
				Info->PixelFormat = ZE_TIFF_PT_LA16;
			break;

		case 2: // RGB
			if (SamplesPerPixel == 2 && ExtraSamples == 2 && BitsPerSample[0] == 8 && BitsPerSample[1] == 8 && BitsPerSample[2] == 8)		
				Info->PixelFormat = ZE_TIFF_PT_RGB8;
			else if (SamplesPerPixel == 2 && ExtraSamples == 2 && BitsPerSample[0] == 8 && BitsPerSample[1] == 8 && BitsPerSample[2] == 8 && BitsPerSample[3] == 8)	
				Info->PixelFormat = ZE_TIFF_PT_RGBA8;
			else if (SamplesPerPixel == 2 && ExtraSamples == 2 && BitsPerSample[0] == 16 && BitsPerSample[1] == 16 && BitsPerSample[2] == 16)		
				Info->PixelFormat = ZE_TIFF_PT_RGB16;
			else if (SamplesPerPixel == 2 && ExtraSamples == 2 && BitsPerSample[0] == 16 && BitsPerSample[1] == 16 && BitsPerSample[2] == 16 && BitsPerSample[3] == 16)	
				Info->PixelFormat = ZE_TIFF_PT_RGBA16;
			break;
		
		case 3: // Palette
			if (BitsPerSample[0] == 8)
				Info->PixelFormat = ZE_TIFF_PT_INDEXED;
			break;
	}

	if (Info->PixelFormat != ZE_TIFF_PT_NONE)
	{
		zeError("Unsupported pixel format.");
		return false;
	}

	return true;
}

static bool LoadPalette(ZEFile* File, ZETIFFInfo* Info)
{
	ZEUInt16 RawPalette[256 * 3];
	File->Seek(ZEEndian::Uni(*(ZEUInt32*)Info->PaletteEntry.Value, Info->Endianness) * sizeof(ZEUInt32), ZE_SF_BEGINING);

	if (File->Read(RawPalette, 3 * 256 * sizeof(ZEUInt16), 1) != 1)
		return false;

	ZEPixelRGBA8* Palette = new ZEPixelRGBA8[256];
	for (ZESize I = 0; I < 256; I++)
	{
		Palette[I].A = 255;
		Palette[I].R = ZEEndian::Uni(RawPalette[3 * I], Info->Endianness) / 256;
		Palette[I].G = ZEEndian::Uni(RawPalette[3 * I + 1], Info->Endianness) / 256;
		Palette[I].B = ZEEndian::Uni(RawPalette[3 * I + 2], Info->Endianness) / 256;
	}

	return true;
}

static bool LoadStripOffsets(ZEFile* File, ZETIFFInfo* Info)
{
	Info->ChunkOffsets.SetCount(ZEEndian::Uni(*(ZEUInt32*)Info->ChunkOffsetsEntry.Count, Info->Endianness));

	File->Seek(ZEEndian::Uni(*(ZEUInt32*)Info->ChunkOffsetsEntry.Value, Info->Endianness) * sizeof(ZEUInt32), ZE_SF_BEGINING);

	if (File->Read(Info->ChunkOffsets.GetCArray(), Info->ChunkOffsets.GetCount() * sizeof(ZEUInt32), 1) != 1)
		return false;

	for (ZESize I = 0; I < Info->ChunkOffsets.GetCount(); I++)
		Info->ChunkOffsets[I] = ZEEndian::Uni(Info->ChunkOffsets[I], Info->Endianness);


	if (Info->Compression != ZE_TIFF_CT_UNCOMPRESSED)
	{
		Info->ChunkSizes.SetCount(ZEEndian::Uni(*(ZEUInt32*)Info->ChunkSizesEntry.Count, Info->Endianness));

		File->Seek(ZEEndian::Uni(*(ZEUInt32*)Info->ChunkSizesEntry.Value, Info->Endianness) * sizeof(ZEUInt32), ZE_SF_BEGINING);

		if (File->Read(Info->ChunkSizes.GetCArray(), Info->ChunkSizes.GetCount() * sizeof(ZEUInt32), 1) != 1)
			return false;

		for (ZESize I = 0; I < Info->ChunkSizes.GetCount(); I++)
			Info->ChunkSizes[I] = ZEEndian::Uni(Info->ChunkSizes[I], Info->Endianness);
	}

	return true;
}

// Indexed, RGB8, RGB16
static void ConvertData(void* Destination, void* Source, ZESize Size, ZETIFFInfo* Info)
{
	switch(Info->PixelFormat)
	{
		case ZE_TIFF_PT_INDEXED:
			ZETexturePixelConverter::ConvertIndexed((ZEPixelRGBA8*)Destination, Source, Size, Info->Palette);
			break;

		case ZE_TIFF_PT_RGB8:
			ZETexturePixelConverter::ConvertRGB8((ZEPixelRGBA8*)Destination, Source, Size);
			break;

		case ZE_TIFF_PT_RGB16:
			ZETexturePixelConverter::ConvertRGB16((ZEPixelRGBA16*)Destination, Source, Size);
			break;
	}
}

static ZETextureData* LoadData(ZEFile* File, ZETIFFInfo* Info)
{
	ZEPointer<ZETextureData> Texture = new ZETextureData();

	Texture->Create(ZE_TT_2D, GetTextureFormat(Info->PixelFormat), 1, 1, Info->Width, Info->Height);

	ZESize PixelSize = GetPixelSize(Info->PixelFormat);
	ZEUInt8* DestinationData = (ZEUInt8*)Texture->GetSurfaces()[0].GetLevels()[0].GetData();
	ZEUInt8* DestinationChunk;
	ZEUInt8* ChunkWidth;
	
	ZESize DestinationPixelSize = Texture->GetSurfaces()[0].GetLevels()[0].GetPitch() / Texture->GetSurfaces()[0].GetLevels()[0].GetWidth();

	ZESize PositionX = 0;
	ZESize PositionY = 0;

	ZESize PositionXIncrement;
	ZESize PositionYIncrement;

	if (Info->ChunkType == ZE_TIFF_CT_TILE)
	{

	}
	else
	{

	}

	if (Info->Compression == ZE_TIFF_CT_UNCOMPRESSED)
	{
		bool InternalFormat = 
			(Info->PixelFormat == ZE_TIFF_PT_RGBA8 || 
			(Info->PixelFormat == ZE_TIFF_PT_RGBA16 && Info->Endianness == ZE_ET_LITTLE) || 
			Info->PixelFormat == ZE_TIFF_PT_L8 ||
			(Info->PixelFormat == ZE_TIFF_PT_L16 && Info->Endianness == ZE_ET_LITTLE) ||
			Info->PixelFormat == ZE_TIFF_PT_LA8 || 
			(Info->PixelFormat == ZE_TIFF_PT_LA16 && Info->Endianness == ZE_ET_LITTLE));

		ZEPointer<ZEUInt8> SourceRow;
		if (!InternalFormat)
			SourceRow = new ZEUInt8[Info->Width * PixelSize];

		ZESize SourceRowPitch = Info->Width * PixelSize;
		for (ZESize I = 0; I < Info->ChunkOffsets.GetCount(); I++)
		{
			DestinationChunk = DestinationData + (Info->Width * PositionY + PositionX) * DestinationPixelSize;

			ZESize ChunkRowCount = Info->Height - I * Info->ChunkHeight >= Info->ChunkHeight ? Info->ChunkHeight : Info->Height % Info->ChunkHeight;
			File->Seek(Info->ChunkOffsets[I], ZE_SF_BEGINING);
			for (ZESize N = 0; I < ChunkRowCount; N++)
			{
				if (InternalFormat)
				{
					if (File->Read(DestinationChunk, SourceRowPitch, 1) != 1)
						return NULL;	
				}
				else
				{
					if (File->Read(SourceRow, SourceRowPitch, 1) != 1)
						return NULL;

					switch(Info->PixelFormat)
					{
						case ZE_TIFF_PT_INDEXED:
							ZETexturePixelConverter::ConvertIndexed((ZEPixelRGBA8*)DestinationChunk, SourceRow, Info->Width, Info->Palette);
							break;

						case ZE_TIFF_PT_RGB8:
							ZETexturePixelConverter::ConvertRGB8((ZEPixelRGBA8*)DestinationChunk, SourceRow, Info->Width);
							break;

						case ZE_TIFF_PT_L16:
							ZETexturePixelConverter::ConvertL16_BE((ZEPixelL16*)DestinationChunk, SourceRow, Info->Width);
							break;

						case ZE_TIFF_PT_LA16:
							ZETexturePixelConverter::ConvertLA16_BE((ZEPixelLA16*)DestinationChunk, SourceRow, Info->Width);
							break;

						case ZE_TIFF_PT_RGBA16:
							ZETexturePixelConverter::ConvertRGBA16_BE((ZEPixelRGBA16*)DestinationChunk, SourceRow, Info->Width);
							break;

						case ZE_TIFF_PT_RGB16:
							if (Info->Endianness == ZE_ET_BIG)
								ZETexturePixelConverter::ConvertRGB16_BE((ZEPixelRGBA16*)DestinationChunk, SourceRow, Info->Width);
							else
								ZETexturePixelConverter::ConvertRGB16((ZEPixelRGBA16*)DestinationChunk, SourceRow, Info->Width);
							break;
					}
				}

				PositionX += PositionXIncrement;
				PositionY += PositionYIncrement;
			}
		}
	}
	else if (Info->Compression == ZE_TIFF_CT_LZW)
	{
		ZESize SourceRowPitch = Info->Width * PixelSize;
		ZEPointer<ZEUInt8> SourceRow = new ZEUInt8[SourceRowPitch];
		for (ZESize I = 0; I < Info->ChunkOffsets.GetCount(); I++)
		{
			ZESize ChunkRowCount = Info->Height - I * Info->ChunkHeight >= Info->ChunkHeight ? Info->ChunkHeight : Info->Height % Info->ChunkHeight;
			File->Seek(Info->ChunkOffsets[I], ZE_SF_BEGINING);
			for (ZESize N = 0; I < ChunkRowCount; N++)
			{
				// Tiff read directly
				if (File->Read(SourceRow, SourceRowPitch, 1) != 1)
					return NULL;
			}
		}
	}
	else if (Info->Compression == ZE_TIFF_CT_PACKBIT)
	{

	}

	return Texture.Transfer();
}

bool ZETextureFileTIFF::LoadInfo(ZETextureDataInfo* Info, ZEFile* File)
{
	ZETIFFInfo TIFFInfo;
	if (!LoadInfo(Info, File))
		return false;

	Info->Width = TIFFInfo.Width;
	Info->Height = TIFFInfo.Height;


	Info->PixelFormat = GetTextureFormat(TIFFInfo.PixelFormat);
	Info->Type = ZE_TT_2D;
	Info->SurfaceCount = 1;
	Info->LevelCount = 1;

	return true;
}

ZETextureData* ZETextureFileTIFF::Load(ZEFile* File)
{
	if (File == NULL)
	{
		zeError("Null file pointer.");
		return NULL;
	}
	bool Result;

	ZETIFFInfo Info;
	Result = LoadHeader(File, &Info);
	if (!Result)
	{
		zeError("Can not load image header.");
		return false;
	}

	Result = LoadPalette(File, &Info);
	if (!Result)
	{
		zeError("Can not load palette.");
		return NULL;
	}

	Result = LoadStripOffsets(File, &Info);
	if (!Result)
	{
		zeError("Can not load strip offesets.");
		return NULL;
	}

	ZETextureData* Texture = LoadData(File, &Info);
	if (Texture == NULL)
	{
		zeError("Can not load image data.");
		return NULL;
	}

	return Texture;
}
