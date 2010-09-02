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

#include <freeimage.h>
#include <sys/stat.h> 
#include <string>
#include <stdio.h>

#include "ZETextureResource.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZETypes.h"
#include "Squish.h"


enum CompressionType
{
	ZE_T2DR_Dxt1,	//BC1
	ZE_T2DR_Dxt3,	//BC2
	ZE_T2DR_Dxt5,	//BC3
	ZE_T2DR_BC5,	

};

enum CompressionSpeed
{
	ZE_T2DR_Normal,
	ZE_T2DR_Fast,
	ZE_T2DR_SuperFast
};

class ZETexture2D;
class ZEResourceFile;

class ZETexture2DResource : public ZETextureResource
{
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
		static ZETexture2DResource*			LoadSharedResource(const char* FileName);
		static ZETexture2DResource*			LoadResource(const char* FileName);
		static ZETexture2DResource*			LoadResource(ZEResourceFile* ResourceFile, bool EmbededResource = true, bool CreateMipMap = TRUE, unsigned int MinTextureQuality = 1);

		static bool							CheckInFileCache(const char* FileName);
		static ZETexture2DResource*			LoadFromFileCache(const char* FileName);
		static bool							LoadFromOriginalFile(ZEResourceFile* ResourceFile, unsigned char* &Image, unsigned int &Width, unsigned int &Height, unsigned int &BPP, unsigned int &Pitch);
		static void							SaveToFileCache(const ZEBYTE* Image, int Width, int Height, int BPP, int Pitch, const char* FileName);
		
		//static void						CopyFromChunk(ZEBYTE* Output, const ZEBYTE* Input, unsigned int ChunkX, unsigned int ChunkY, unsigned int Pitch);
		//static void						CopyToChunk(ZEBYTE* Output, const ZEBYTE* Input, unsigned int ChunkX, unsigned int ChunkY, unsigned int Pitch);
		static ZEBYTE*						Compress(ZEBYTE* Image, int Width, int Height, int Pitch, CompressionType CType, CompressionSpeed CSpeed);

		static void							WriteToDevice(ZETexture2DResource* TextureResource, const unsigned char* Source,int Width, int Height, int BPP, unsigned int Pitch, unsigned int Level = 0);

		static void							CreateMipmaps(ZETexture2DResource* TextureResource, const unsigned char* Image, unsigned int Width, unsigned int Height, unsigned int BPP, unsigned int Pitch, unsigned int Level, bool DoResize = FALSE, bool DoCompress = FALSE, CompressionType Type = ZE_T2DR_Dxt3, CompressionSpeed Speed = ZE_T2DR_Normal);
		static unsigned int					GetMipmapCount(unsigned int Width, unsigned int Height);

		static bool							Rescale(const unsigned char* DestinationData, const unsigned char* SourceData, unsigned int &SourceWidth, unsigned int &SourceHeight, unsigned int &SourcePitch, unsigned int &SourceBPP, unsigned int RescaleRatio);
};

#endif




