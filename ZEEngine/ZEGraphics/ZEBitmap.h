//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEBitmap.h
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
#ifndef __ZE_BITMAP_H__
#define __ZE_BITMAP_H__

#include <ZEMath/ZEVector.h>

enum ZEBitmapFileFormat
{
	ZE_BFF_BMP,
	ZE_BFF_TGA,
	ZE_BFF_PNG,
	ZE_BFF_JPG,
	ZE_BFF_TIFF
};

enum ZEBitmapAddressingMode
{
	ZE_BAM_WRAP,
	ZE_BAM_CLAMP,
	ZE_BAM_MIRROR,
	ZE_BAM_BORDER,
};

enum ZEBitmapFilteringMode
{
	ZE_BFM_POINT,
	ZE_BFM_BILINEAR
};

struct ZEPixelColor
{
	unsigned char b, g, r, a;

	static ZEPixelColor	Lerp(const ZEPixelColor& A, const ZEPixelColor& B, float T);
	static ZEVector4 LerpFloat(const ZEPixelColor& A, const ZEPixelColor& B, float T);

	ZEPixelColor();
	ZEPixelColor(unsigned int Color);
	ZEPixelColor(unsigned char a, unsigned char r, unsigned char g, unsigned char b);
};

struct ZEBitmapSamplingOptions
{
	ZEBitmapFilteringMode		Filter;
	ZEBitmapAddressingMode		AddressingX;
	ZEBitmapAddressingMode		AddressingY;
	ZEPixelColor				BorderColor;
};

class ZEBitmap
{
	private:
		unsigned int			Width;
		unsigned int			Height;
		unsigned int			Pitch;
		unsigned int			PixelSize;
		void*					Pixels;

	public:
		bool					Create(unsigned int Width, unsigned int Height, unsigned int PixelSize);

		unsigned int			GetWidth();
		unsigned int			GetHeight();
		unsigned int			GetPitch();
		unsigned int			GetPixelSize();
		unsigned int			GetBPP();
		unsigned int			GetSize();

		ZEPixelColor*			GetPixels();
		ZEPixelColor&			GetPixel(unsigned int x, unsigned int y);
		ZEVector4				GetPixelFloat(unsigned int x, unsigned int y);

		ZEPixelColor&			SamplePixel(int x, int y, ZEBitmapSamplingOptions* Options = 0);
		ZEVector4				SamplePixelFloat(int x, int y, ZEBitmapSamplingOptions* Options = 0);		
		ZEVector4				SamplePixelFloat(const ZEVector2& TextureCoordinate, ZEBitmapSamplingOptions* Options = 0);
	
		ZEPixelColor*			GetRow(unsigned int Index);

		void					CopyFrom(void* SourceBuffer, unsigned int SourcePitch, 
									unsigned int Width, unsigned int Height, 
									unsigned int SourceOffsetX = 0, unsigned int SourceOffsetY = 0,
									unsigned int DestinationOffsetX = 0, unsigned int DestinationOffsetY = 0);

		void					CopyTo(void* DestinationBuffer, unsigned int DestinationPitch, 
									unsigned int Width, unsigned int Height, 
									unsigned int DestinationOffsetX = 0, unsigned int DestinationOffsetY = 0,
									unsigned int SourceOffsetX = 0, unsigned int SourceOffsetY = 0);

		void					Fill(unsigned int Color);
		void					Clear();

		bool					Load(const char* Filename);
		void					Save(const char* FileName, ZEBitmapFileFormat Format);

		void					Release();

								ZEBitmap();
								~ZEBitmap();
};

#endif
