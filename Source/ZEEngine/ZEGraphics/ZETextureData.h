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
#include "ZETexture.h"
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


class ZETextureData;
class ZETextureSurface;
class ZETextureData;

class ZETextureLevel
{
	friend class ZETextureSurface;
	friend class ZETextureData;	

	private:
		ZETextureSurface*			Owner;
		ZESize						Level;
		void*						Data;

	public:
		ZETextureSurface*			GetOwner();

		ZESize						GetLevel();
		ZESize						GetWidth();
		ZESize						GetHeight();
		ZESize						GetPitch();
		ZESize						GetRowCount();
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
		ZESize						Surface;
		ZETextureData*			Owner;
		ZEArray<ZETextureLevel>		Levels;

	public:
		ZESize						GetSurface();
		ZETextureData*			GetOwner();
		ZEArray<ZETextureLevel>&	GetLevels();

		ZESize						GetSize();

									ZETextureSurface();
		virtual						~ZETextureSurface();

};


class ZETextureData
{
	private:
		ZESize						Width;
		ZESize						Height;
		ZESize						LevelCount;
		ZESize						SurfaceCount;
		
		ZETextureType				TextureType;
		ZETexturePixelFormat		PixelFormat;

		ZEArray<ZETextureSurface>	Surfaces;

	public:
									ZETextureData();
		virtual						~ZETextureData();

		ZEArray<ZETextureSurface>&	GetSurfaces();

		bool						IsEmpty();

		ZESize						GetTextureWidth();
		ZESize						GetTextureHeight();
		ZESize						GetTextureLevelCount();
		ZESize						GetTextureSurfaceCount();
		ZETextureType				GetTextureType();
		ZETexturePixelFormat		GetPixelFormat();

		ZESize						GetSize();
		ZESize						GetSizeOnDisk();

		void						CreateTexture(ZETextureType TextureType, ZETexturePixelFormat PixelFormat, ZESize SurfaceCount, ZESize LevelCount, ZESize Width, ZESize Height);
		void						CreateTexture(ZETextureData& TextureData);
		void						DestroyTexture();

		static void					ConvertToCubeTextureData(ZETextureData* Output, ZETextureData* TextureData);
		static void					ConvertToVolumeTextureData(ZETextureData* Output, ZETextureData* TextureData, ZESize TileCountX, ZESize TileCountY);
		
};


#endif
