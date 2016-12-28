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

#include "ZETypes.h"
#include "ZEPacking.h"
#include "ZEDS/ZEString.h"
#include "ZEMath/ZEVector.h"

enum ZEBitmapPixelFormat
{
	ZE_BPF_NONE,
	ZE_BPF_ARGB,
	ZE_BPF_RGB,
	ZE_BPF_GRAYSCALE
};

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

#pragma pack(push, 1)
struct ZEPixelARGB
{
	ZEUInt8						b, g, r, a;

	ZEVector4					ToVector4() const;

								ZEPixelARGB();
								ZEPixelARGB(ZEUInt Color);
								ZEPixelARGB(ZEUInt8 a, ZEUInt8 r, ZEUInt8 g, ZEUInt8 b);

	static ZEPixelARGB			Lerp(const ZEPixelARGB& A, const ZEPixelARGB& B, float T);
};

struct ZEPixelRGB
{
	ZEUInt8						b, g, r;

	ZEVector4					ToVector4() const;

								ZEPixelRGB();
								ZEPixelRGB(ZEUInt Color);
								ZEPixelRGB(ZEUInt8 r, ZEUInt8 g, ZEUInt8 b);

	static ZEPixelRGB			Lerp(const ZEPixelRGB& A, const ZEPixelRGB& B, float T);
};

struct ZEPixelGrayscale
{
	ZEUInt8						l;

	ZEVector4					ToVector4() const;

								ZEPixelGrayscale();
								ZEPixelGrayscale(ZEUInt Color);

	static ZEPixelGrayscale		Lerp(const ZEPixelGrayscale& A, const ZEPixelGrayscale& B, float T);
};

#pragma pack(pop)

struct ZEBitmapAddressingOptions
{
	ZEBitmapAddressingMode					AddressingX;
	ZEBitmapAddressingMode					AddressingY;
};

struct ZEBitmapSamplingOptions : public ZEBitmapAddressingOptions
{
	ZEBitmapFilteringMode					Filter;
	ZEVector4								BorderColor;
};

class ZEBitmap
{
	private:
		ZESize								Width;
		ZESize								Height;
		ZESize								Pitch;
		ZEBitmapPixelFormat					Format;
		ZESize								PixelSize;
		void*								Data;

	public:
		ZESize								GetWidth() const;
		ZESize								GetHeight() const;
		ZESize								GetPitch() const;
		ZEBitmapPixelFormat					GetFormat() const;
		ZESize								GetPixelSize() const;
		ZESize								GetBPP() const;
		ZESize								GetSize() const;

		void								SetData(void* Data);
		void*								GetData();
		const void*							GetData() const;

		void								SetRow(ZESize Index, void* Data);
		void*								GetRow(ZESize Index);
		const void*							GetRow(ZESize Index) const;

		ZEPixelARGB*						GetPixelsARGB();
		const ZEPixelARGB*					GetPixelsARGB() const;
		ZEPixelRGB*							GetPixelsRGB();
		const ZEPixelRGB*					GetPixelsRGB() const;
		ZEPixelGrayscale*					GetPixelsGrayscale();
		const ZEPixelGrayscale*				GetPixelsGrayscale() const;

		void*								GetPixel(ZESize x, ZESize y);
		const void*							GetPixel(ZESize x, ZESize y) const;
		ZEPixelARGB&						GetPixelARGB(ZESize x, ZESize y);
		const ZEPixelARGB&					GetPixelARGB(ZESize x, ZESize y) const;
		ZEPixelRGB&							GetPixelRGB(ZESize x, ZESize y);
		const ZEPixelRGB&					GetPixelRGB(ZESize x, ZESize y) const;
		ZEPixelGrayscale&					GetPixelGrayscale(ZESize x, ZESize y);
		const ZEPixelGrayscale&				GetPixelGrayscale(ZESize x, ZESize y) const;
		ZEVector4							GetPixelVector(ZESize x, ZESize y) const;
		
		void*								AddressPixel(ZESSize x, ZESSize y, const ZEBitmapAddressingOptions& Options);
		const void*							AddressPixel(ZESSize x, ZESSize y, const ZEBitmapAddressingOptions& Options) const;

		ZEVector4							SamplePixel(const ZEVector2& Texcoords, const ZEBitmapSamplingOptions& Options) const;

		void								Fill(ZEUInt Color);

		void								CopyFrom(void* SourceBuffer, ZESize SourcePitch, 
												ZESize Width, ZESize Height, 
												ZESize SourceOffsetX = 0, ZESize SourceOffsetY = 0,
												ZESize DestinationOffsetX = 0, ZESize DestinationOffsetY = 0);

		void								CopyTo(void* DestinationBuffer, ZESize DestinationPitch, 
												ZESize Width, ZESize Height, 
												ZESize DestinationOffsetX = 0, ZESize DestinationOffsetY = 0,
												ZESize SourceOffsetX = 0, ZESize SourceOffsetY = 0);

		bool								Create(ZESize Width, ZESize Height, ZEBitmapPixelFormat Format);
		void								Release();
		
		bool								Load(const ZEString& Filename);
		bool								Save(const ZEString& FileName, ZEBitmapFileFormat Format) const;

											ZEBitmap();
											~ZEBitmap();
};
