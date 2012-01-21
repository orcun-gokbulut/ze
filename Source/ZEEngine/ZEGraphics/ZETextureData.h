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

#include "ZETexture.h"
#include "ZEGraphics/ZETextureOptions.h"

class ZEFile;
class ZEPartialFile;
class ZETextureLoader;

class ZETextureData
{
	protected:
		// Empty
	
	private:
		struct TextureData
		{
			ZETexturePixelFormat	PixelFormat;
			
			ZEUInt			Width;
			ZEUInt			Height;

			ZEUInt 			Depth;
			ZEUInt 			MipmapCount;
			
			struct SurfaceData
			{
				struct MipmapData
				{
					void*			Data;
					ZEUInt 	RowCount;
					ZEUInt 	RowSize;

				}*Mipmaps;

			}*Surfaces;

		}Texture;
		
		bool						CheckBoundaries(ZEUInt Depth);
		bool						CheckBoundaries(ZEUInt Depth, ZEUInt MipLevel);

	public:
									ZETextureData();
		virtual						~ZETextureData();
		
		void						CreateTexture(ZETexturePixelFormat PixelFormat, ZEUInt Depth, ZEUInt MipmapCount, ZEUInt Width, ZEUInt Height);
		void						DestroyTexture();

		void						AddSurface(ZEUInt Count = 1);
		void						RemoveSurface(ZEUInt Count);
		void						AddMipmap(ZEUInt Count = 1);
		void						RemoveMipmap(ZEUInt Count);

		void						AllocateMipmap(ZEUInt Depth, ZEUInt MipLevel, ZEUInt RowSize, ZEUInt RowCount);
		void						FreeMipmap(ZEUInt Depth, ZEUInt MipLevel);

		void						CopyMipmapDataFrom(ZEUInt Depth, ZEUInt MipLevel, void* SourceData, ZEUInt SourcePitch);
		void						CopyMipmapDataTo(ZEUInt Depth, ZEUInt MipLevel, void* Dest, ZEUInt DestPitch);

		bool						IsEmpty();

		void						SetDepth(ZEUInt Value);
		ZEUInt				GetDepth();
		void						SetWidth(ZEUInt Value);
		ZEUInt				GetWidth();
		void						SetHeight(ZEUInt Value);
		ZEUInt				GetHeight();
		void						SetMipmapCount(ZEUInt Value);
		ZEUInt				GetMipmapCount();
		void						SetPixelFormat(ZETexturePixelFormat PixelFormat);
		ZETexturePixelFormat		GetPixelFormat();
		
		ZEUInt				GetMipmapRowCount(ZEUInt Depth, ZEUInt MipLevel);
		ZEUInt				GetMipmapRowSize(ZEUInt Depth, ZEUInt MipLevel);

		void*						GetMipmapData(ZEUInt Depth, ZEUInt MipLevel);
		ZEUInt				GetMipmapDataSize(ZEUInt Depth, ZEUInt MipLevel);
		ZEUInt				GetSurfaceSize(ZEUInt Depth);
		ZEUInt				GetTextureSize();
		ZEUInt				GetSizeOnDisk();

		

		

};


#endif /* __ZE_TEXTURE_DATA_H__ */
