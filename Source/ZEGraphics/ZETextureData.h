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
			
			unsigned int			Width;
			unsigned int			Height;

			unsigned int 			Depth;
			unsigned int 			MipmapCount;
			
			struct SurfaceData
			{
				struct MipmapData
				{
					void*			Data;
					unsigned int 	RowCount;
					unsigned int 	RowSize;

				}*Mipmaps;

			}*Surfaces;

		}Texture;
		
		bool						CheckBoundaries(unsigned int Depth);
		bool						CheckBoundaries(unsigned int Depth, unsigned int MipLevel);

	public:
									ZETextureData();
		virtual						~ZETextureData();
		
		void						CreateTexture(ZETexturePixelFormat PixelFormat, unsigned int Depth, unsigned int MipmapCount, unsigned int Width, unsigned int Height);
		void						DestroyTexture();

		void						AddSurface(unsigned int Count = 1);
		void						RemoveSurface(unsigned int Count);
		void						AddMipmap(unsigned int Count = 1);
		void						RemoveMipmap(unsigned int Count);

		void						AllocateMipmap(unsigned int Depth, unsigned int MipLevel, unsigned int RowSize, unsigned int RowCount);
		void						FreeMipmap(unsigned int Depth, unsigned int MipLevel);

		void						CopyMipmapDataFrom(unsigned int Depth, unsigned int MipLevel, void* SourceData, unsigned int SourcePitch);
		void						CopyMipmapDataTo(unsigned int Depth, unsigned int MipLevel, void* Dest, unsigned int DestPitch);

		bool						IsEmpty();

		void						SetDepth(unsigned int Value);
		unsigned int				GetDepth();
		void						SetWidth(unsigned int Value);
		unsigned int				GetWidth();
		void						SetHeight(unsigned int Value);
		unsigned int				GetHeight();
		void						SetMipmapCount(unsigned int Value);
		unsigned int				GetMipmapCount();
		void						SetPixelFormat(ZETexturePixelFormat PixelFormat);
		ZETexturePixelFormat		GetPixelFormat();
		
		unsigned int				GetMipmapRowCount(unsigned int Depth, unsigned int MipLevel);
		unsigned int				GetMipmapRowSize(unsigned int Depth, unsigned int MipLevel);

		void*						GetMipmapData(unsigned int Depth, unsigned int MipLevel);
		unsigned int				GetMipmapDataSize(unsigned int Depth, unsigned int MipLevel);
		unsigned int				GetSurfaceSize(unsigned int Depth);
		unsigned int				GetTextureSize();
		unsigned int				GetSizeOnDisk();

		

		

};


#endif /* __ZE_TEXTURE_DATA_H__ */
