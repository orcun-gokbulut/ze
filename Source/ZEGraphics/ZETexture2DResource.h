//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETexture2DResource.h
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
#ifndef	__ZE_TEXTURE_2D_RESOURCE_H__
#define __ZE_TEXTURE_2D_RESOURCE_H__

#include "ZETextureResource.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZETypes.h"

enum ZETextureQuality
{
	ZE_TQ_AUTO		= 0,
	ZE_TQ_VERY_HIGH	= 5,
	ZE_TQ_HIGH		= 4,
	ZE_TQ_NORMAL	= 3,
	ZE_TQ_LOW		= 2,
	ZE_TQ_VERY_LOW	= 1,	// şimdilik yok low ve very low aynı.
};
enum ZETextureCompressionType
{
	ZE_TCT_NONE		= 0,
	ZE_TCT_DXT1		= 1,	//BC1
	ZE_TCT_DXT3		= 2,	//BC2
	ZE_TCT_DXT5		= 3,	//BC3
	ZE_TCT_3DC		= 4,
	ZE_TCT_AUTO		= 5
};

enum ZETextureCompressionQuality
{
	ZE_TCQ_HIGH		= 3,
	ZE_TCQ_NORMAL	= 2,
	ZE_TCQ_LOW		= 1,
};

enum ZETextureDownSampling
{
	ZE_TDS_NONE		= 0,
	ZE_TDS_2X		= 1,
	ZE_TDS_4X		= 2,
	ZE_TDS_AUTO		= 3,
};

struct ZETextureLoadOptions
{
	ZETextureQuality					MinimumQuality;
	ZETextureQuality					MaximumQuality;

	bool								CustomQuality;
	struct 
	{
		ZETextureCompressionType		CompressionType;
		ZETextureCompressionQuality		CompressionQuality;
		ZETextureDownSampling			DownSample;
		bool							CacheFile;
		bool							MipMapping;
		unsigned int					MaximumMipmapLevel;
	};
};

static ZETextureLoadOptions Default = {ZE_TQ_VERY_LOW, ZE_TQ_VERY_HIGH, true, {ZE_TCT_DXT3, ZE_TCQ_NORMAL, ZE_TDS_NONE, true, true, 0}};

class ZETexture2D;
class ZEResourceFile;

class ZETexture2DResource : public ZETextureResource
{
	friend static ZETexture2DResource*		LoadFromFileCache(const char *FileName);
	friend static ZETexture2DResource*		LoadFromOriginalFile(ZEResourceFile* ResourceFile, ZETextureLoadOptions *UserOptions = &Default);
	friend static void						WriteToDevice(ZETexture2DResource* TextureResource, const unsigned char* SourceData, unsigned int Width, unsigned int Height, unsigned int BPP, unsigned int Level = 0, ZETextureLoadOptions *UserOptions = &Default);
	friend static void						CreateMipmaps(ZETexture2DResource* TextureResource, const unsigned char* Image, unsigned int Width, unsigned int Height, unsigned int BPP, unsigned int Pitch, bool IsResizeable = true, bool IsCompressible = true, unsigned int Level = 0, ZETextureLoadOptions *UserOptions = &Default);
	friend static void						Compress(ZETexture2DResource* TextureResource, ZEBYTE* Image, unsigned int Width, unsigned int Height, unsigned int Pitch, unsigned int BPP, unsigned int Level = 0, ZETextureLoadOptions *UserOptions = &Default);

	private:
		ZETexture2D*						Texture;

	protected:
											ZETexture2DResource();
		virtual 							~ZETexture2DResource();

	public:
		virtual const char*					GetResourceType() const;
		virtual ZETextureType				GetTextureType() const;

		const ZETexture2D*					GetTexture() const;

		static void							CacheResource(const char* FileName);
		static ZETexture2DResource*			LoadSharedResource(const char* FileName, ZETextureLoadOptions* UserOptions = &Default);
		static ZETexture2DResource*			LoadResource(const char* FileName, ZETextureLoadOptions* UserOptions = &Default);
		static ZETexture2DResource*			LoadResource(ZEResourceFile* ResourceFile, bool EmbededResource = true, ZETextureLoadOptions* UserOptions = &Default);

		
};

#endif

