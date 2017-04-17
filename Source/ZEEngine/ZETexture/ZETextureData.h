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

#include "ZETypes.h"
#include "ZEEndian.h"
#include "ZEPacking.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEDS/ZEArray.h"
#include "ZEExport.ZEEngine.h"

class ZETextureData;
class ZETextureSurface;
class ZETextureData;

struct ZETextureDataInfo
{
	ZEUInt							SurfaceCount;
	ZEUInt							LevelCount;
	ZEUInt							Width;
	ZEUInt							Height;
	ZEGRFormat						PixelFormat;
	ZEGRTextureType					Type;
};

class ZE_EXPORT_ZEENGINE ZETextureLevel
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
		ZEGRTextureType					GetType();
		ZEGRFormat						GetPixelFormat();

		ZESize							GetSize();
		ZESize							GetSizeOnDisk();

		void							Create(ZEGRTextureType TextureType, ZEGRFormat PixelFormat, ZEUInt SurfaceCount, ZEUInt LevelCount, ZEUInt Width, ZEUInt Height);
		void							Create(ZETextureData& TextureData);
		void							Destroy();

		static void						ConvertToCubeTextureData(ZETextureData* Output, ZETextureData* TextureData);
		static void						ConvertToVolumeTextureData(ZETextureData* Output, ZETextureData* TextureData, ZEUInt HorizTileCount, ZEUInt VertTileCount);
		
										ZETextureData();
										~ZETextureData();
};
