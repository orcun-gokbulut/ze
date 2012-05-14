//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETextureData.h
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
#ifndef __ZE_TEXTURE_DATA_H__
#define __ZE_TEXTURE_DATA_H__

#include "ZETypes.h"
#include "ZEPacking.h"
#include "ZEGraphics/ZETexture.h"
#include "ZEDS/ZEArray.h"

// Compressed Dxt1 Block Size
#define		ZE_DXT_1_COMPRESSION_BLOCK_WIDTH	4	// Pixels
#define		ZE_DXT_1_COMPRESSION_BLOCK_HEIGHT	4	// Pixels
#define		ZE_DXT_1_INPUT_BLOCK_SIZE			64	// Bytes
#define		ZE_DXT_1_OUTPUT_BLOCK_SIZE			8	// Bytes

// Compressed Dxt3 Block Size
#define		ZE_DXT_3_COMPRESSION_BLOCK_WIDTH	4	// Pixels
#define		ZE_DXT_3_COMPRESSION_BLOCK_HEIGHT	4	// Pixels
#define		ZE_DXT_3_INPUT_BLOCK_SIZE			64	// Bytes
#define		ZE_DXT_3_OUTPUT_BLOCK_SIZE			16	// Bytes

// Compressed Dxt5 Block Size
#define		ZE_DXT_5_COMPRESSION_BLOCK_WIDTH	4	// Pixels
#define		ZE_DXT_5_COMPRESSION_BLOCK_HEIGHT	4	// Pixels
#define		ZE_DXT_5_INPUT_BLOCK_SIZE			64	// Bytes
#define		ZE_DXT_5_OUTPUT_BLOCK_SIZE			16	// Bytes

// Uncompressed ARGB size
#define		ZE_I8_4_COMPRESSION_BLOCK_WIDTH		1	// Pixels
#define		ZE_I8_4_COMPRESSION_BLOCK_HEIGHT	1	// Pixels
#define		ZE_I8_4_INPUT_BLOCK_SIZE			4	// Bytes
#define		ZE_I8_4_OUTPUT_BLOCK_SIZE			4	// Bytes

// Uncompressed ZE_TPF_I8 size
#define		ZE_I8_COMPRESSION_BLOCK_WIDTH		1	// Pixels
#define		ZE_I8_COMPRESSION_BLOCK_HEIGHT		1	// Pixels
#define		ZE_I8_INPUT_BLOCK_SIZE				1	// Bytes
#define		ZE_I8_OUTPUT_BLOCK_SIZE				1	// Bytes

// Uncompressed ZE_TPF_I16 size
#define		ZE_I16_COMPRESSION_BLOCK_WIDTH		1	// Pixels
#define		ZE_I16_COMPRESSION_BLOCK_HEIGHT		1	// Pixels
#define		ZE_I16_INPUT_BLOCK_SIZE				2	// Bytes
#define		ZE_I16_OUTPUT_BLOCK_SIZE			2	// Bytes

// Uncompressed ZE_TPF_I16_2 size
#define		ZE_I16_2_COMPRESSION_BLOCK_WIDTH	1	// Pixels
#define		ZE_I16_2_COMPRESSION_BLOCK_HEIGHT	1	// Pixels
#define		ZE_I16_2_INPUT_BLOCK_SIZE			2	// Bytes
#define		ZE_I16_2_OUTPUT_BLOCK_SIZE			2	// Bytes

// Uncompressed ZE_TPF_I32 size
#define		ZE_I32_COMPRESSION_BLOCK_WIDTH		1	// Pixels
#define		ZE_I32_COMPRESSION_BLOCK_HEIGHT		1	// Pixels
#define		ZE_I32_INPUT_BLOCK_SIZE				2	// Bytes
#define		ZE_I32_OUTPUT_BLOCK_SIZE			2	// Bytes

// Uncompressed ZE_TPF_F32 size
#define		ZE_F32_COMPRESSION_BLOCK_WIDTH		1	// Pixels
#define		ZE_F32_COMPRESSION_BLOCK_HEIGHT		1	// Pixels
#define		ZE_F32_INPUT_BLOCK_SIZE				4	// Bytes
#define		ZE_F32_OUTPUT_BLOCK_SIZE			4	// Bytes

// Uncompressed ZE_TPF_F32_2 size
#define		ZE_F32_2_COMPRESSION_BLOCK_WIDTH	1	// Pixels
#define		ZE_F32_2_COMPRESSION_BLOCK_HEIGHT	1	// Pixels
#define		ZE_F32_2_INPUT_BLOCK_SIZE			8	// Bytes
#define		ZE_F32_2_OUTPUT_BLOCK_SIZE			8	// Bytes

// Uncompressed ZE_TPF_F32_4 size
#define		ZE_F32_4_COMPRESSION_BLOCK_WIDTH	1	// Pixels
#define		ZE_F32_4_COMPRESSION_BLOCK_HEIGHT	1	// Pixels
#define		ZE_F32_4_INPUT_BLOCK_SIZE			16	// Bytes
#define		ZE_F32_4_OUTPUT_BLOCK_SIZE			16	// Bytes


ZEPackStruct
(
	struct ZEG8	
	{
		ZEUInt8 G;
	}
);

ZEPackStruct
( 
	struct ZEARGB32
	{
		ZEUInt8 B;
		ZEUInt8 G;
		ZEUInt8 R;
		ZEUInt8 A;
	}
);

ZEPackStruct
(
	struct ZEG16
	{
		ZEUInt16 G;
	}
);

ZEPackStruct
(
	struct ZEAG32
	{
		ZEUInt16 G;
		ZEUInt16 A;
	}
);

ZEPackStruct
(
	struct ZEG32
	{
		ZEUInt32 G;
	}
);

ZEPackStruct
(
	struct ZEG32F
	{
		float G;
	}
);

ZEPackStruct
(
	struct ZEAG32F
	{
		float G;
		float A;
	}
);

ZEPackStruct
(
	struct ZEARGB32F
	{
		float B;
		float G;
		float R;
		float A;
	}
);

ZEPackStruct
(
	struct ZERBG24
	{
		ZEUInt8 B;
		ZEUInt8 G;
		ZEUInt8 R;
	}
);


class ZETextureData;
class ZETextureSurface;
class ZETextureData;

struct ZETextureDataInfo
{
	ZEUInt							SurfaceCount;
	ZEUInt							LevelCount;
	ZEUInt							Width;
	ZEUInt							Height;
	ZETexturePixelFormat			PixelFormat;
	ZETextureType					Type;
};

class ZETextureLevel
{
	friend class ZETextureSurface;
	friend class ZETextureData;	

	private:
		ZETextureSurface*			Owner;
		ZEUInt						Level;
		void*						Data;

	public:
		ZETextureSurface*			GetOwner();

		ZEUInt						GetLevel();
		ZEUInt						GetWidth();
		ZEUInt						GetHeight();
		ZESize						GetPitch();
		ZEUInt						GetRowCount();
		ZESize						GetSize();

		void*						GetData();

		void						CopyFrom(void* SourceData, ZESize SourcePitch);
		void						CopyTo(void* Dest, ZESize DestPitch);

									ZETextureLevel();
		virtual						~ZETextureLevel();

};


class ZETextureSurface
{
	friend class ZETextureData;

	private:
		ZETextureData*				Owner;
		ZEUInt						Surface;
		ZEArray<ZETextureLevel>		Levels;

	public:
		ZETextureData*				GetOwner();

		ZEUInt						GetSurface();
		ZEArray<ZETextureLevel>&	GetLevels();
		ZEUInt						GetWidth();
		ZEUInt						GetHeight();

		ZESize						GetSize();

									ZETextureSurface();
		virtual						~ZETextureSurface();

};


class ZETextureData
{
	private:
		ZETextureDataInfo				Info;
		ZEArray<ZETextureSurface>		Surfaces;

	public:
		ZEArray<ZETextureSurface>&		GetSurfaces();

		bool							IsEmpty();
		
		const ZETextureDataInfo&		GetInfo();
		ZEUInt							GetWidth();
		ZEUInt							GetHeight();
		ZEUInt							GetLevelCount();
		ZEUInt							GetSurfaceCount();
		ZETextureType					GetType();
		ZETexturePixelFormat			GetPixelFormat();

		ZESize							GetSize();
		ZESize							GetSizeOnDisk();

		void							Create(ZETextureType TextureType, ZETexturePixelFormat PixelFormat, ZEUInt SurfaceCount, ZEUInt LevelCount, ZEUInt Width, ZEUInt Height);
		void							Create(ZETextureData& TextureData);
		void							Destroy();

		static void						ConvertToCubeTextureData(ZETextureData* Output, ZETextureData* TextureData);
		static void						ConvertToVolumeTextureData(ZETextureData* Output, ZETextureData* TextureData, ZEUInt HorizTileCount, ZEUInt VertTileCount);
		
										ZETextureData();
										~ZETextureData();
};


#endif
