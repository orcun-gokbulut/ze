//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureFileTIFF.h
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


#pragma once
#ifndef __ZE_TEXTURE_FILE_TIFF_H__
#define __ZE_TEXTURE_FILE_TIFF_H__

#include "ZETextureFile.h"
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
	ZE_TIFF_PT_G8,
	ZE_TIFF_PT_G16, // Will be supported
	ZE_TIFF_PT_GA16, // Will be supported
	ZE_TIFF_PT_GA32,
	ZE_TIFF_PT_RGB24,
	ZE_TIFF_PT_RGBA32,
	ZE_TIFF_PT_RGB48, // Will be supported
	ZE_TIFF_PT_RGB64 // Will be supported
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
	ZEARGB32				Palette[256];
};

class ZETextureFileTIFF : public ZETextureFile
{
	private:
		static bool					UncompressPackBit(void* Destination, ZESize DestinationSize, void* Source, ZESize SourceSize);
		static bool					UncompressLZW(void* Destination, ZESize DestinationSize, void* Source, ZESize SourceCount);

		static bool					LoadEntryArray(ZEArray<ZEUInt32> Output, ZETIFFEntry* Entry, ZEFile* File, ZEEndianness Endianness);
		static ZESize				LoadEntryArray(ZEUInt32* Output, ZESize OutputSize, ZETIFFEntry* Entry, ZEFile* File, ZEEndianness Endianness);
		static ZEUInt				LoadEntry(ZETIFFEntry* Entry, ZEEndianness Endian);

		static bool					LoadHeader(ZEFile* File, ZETIFFInfo* Info);
		static bool					LoadPalette(ZEFile*, ZETIFFInfo* Info);
		static bool					LoadStripOffsets(ZEFile* File, ZETIFFInfo* Info);
		static ZETextureData*		LoadData(ZEFile* File, ZETIFFInfo* Info);

	public:
		virtual bool				CanLoad(ZEFile* File);
		virtual ZETextureData*		Load(ZEFile* File);
};

#endif
