//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEBitmap.cpp
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

#include "ZEBitmap.h"
#include "ZEError.h"
#include "ZETypes.h"
#include "ZEMath/ZEMath.h"
#include "ZEFile/ZEFileInfo.h"

#include <FreeImage.h>
#include <memory.h>


// ZEPixelARGB
//////////////////////////////////////////////////////////////////////////////////////

ZEVector4 ZEPixelARGB::ToVector4() const
{
	return ZEVector4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);
}

ZEPixelARGB::ZEPixelARGB()
{
	r = 0x00;
	g = 0x00;
	b = 0x00;
	a = 0xFF;
}

ZEPixelARGB::ZEPixelARGB(ZEUInt Color)
{
	this->a = (Color & 0xFF000000) >> 24;
	this->r = (Color & 0x00FF0000) >> 16;
	this->g = (Color & 0x0000FF00) >> 8;
	this->b = (Color & 0x000000FF);
}

ZEPixelARGB::ZEPixelARGB(unsigned char a, unsigned char r, unsigned char g, unsigned char b)
{
	this->a = a;
	this->r = r;
	this->g = g;
	this->b = b;
}

ZEPixelARGB ZEPixelARGB::Lerp(const ZEPixelARGB& A, const ZEPixelARGB& B, float T)
{
	ZEPixelARGB Temp;
	Temp.a = (unsigned char)((float)A.a * (1.0f - T) + (float)B.a * T);
	Temp.r = (unsigned char)((float)A.r * (1.0f - T) + (float)B.r * T);
	Temp.g = (unsigned char)((float)A.g * (1.0f - T) + (float)B.g * T);
	Temp.b = (unsigned char)((float)A.b * (1.0f - T) + (float)B.b * T);

	return Temp;
}

// ZEPixelRGB
//////////////////////////////////////////////////////////////////////////////////////

ZEVector4 ZEPixelRGB::ToVector4() const
{
	return ZEVector4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 1.0f);
}

ZEPixelRGB::ZEPixelRGB()
{
	r = 0x00;
	g = 0x00;
	b = 0x00;
}

ZEPixelRGB::ZEPixelRGB(ZEUInt Color)
{
	this->r = (Color & 0x00FF0000) >> 16;
	this->g = (Color & 0x0000FF00) >> 8;
	this->b = (Color & 0x000000FF);
}

ZEPixelRGB::ZEPixelRGB(unsigned char r, unsigned char g, unsigned char b)
{
	this->r = r;
	this->g = g;
	this->b = b;
}

ZEPixelRGB ZEPixelRGB::Lerp(const ZEPixelRGB& A, const ZEPixelRGB& B, float T)
{
	ZEPixelRGB Temp;
	Temp.r = (unsigned char)((float)A.r * (1.0f - T) + (float)B.r * T);
	Temp.g = (unsigned char)((float)A.g * (1.0f - T) + (float)B.g * T);
	Temp.b = (unsigned char)((float)A.b * (1.0f - T) + (float)B.b * T);

	return Temp;
}


// ZEPixelGrayscale
//////////////////////////////////////////////////////////////////////////////////////

ZEVector4 ZEPixelGrayscale::ToVector4() const
{
	return ZEVector4((float)l / 255.0f, 0.0f, 0.0f, 1.0f);
}

ZEPixelGrayscale::ZEPixelGrayscale()
{
	l = 0x00;
}

ZEPixelGrayscale::ZEPixelGrayscale(ZEUInt Color)
{
	this->l = Color;
}

ZEPixelGrayscale ZEPixelGrayscale::Lerp(const ZEPixelGrayscale& A, const ZEPixelGrayscale& B, float T)
{
	ZEPixelGrayscale Temp;
	Temp.l = (unsigned char)((float)A.l * (1.0f - T) + (float)B.l * T);
	return Temp;
}


// ZEPixelL
//////////////////////////////////////////////////////////////////////////////////////

ZESize ZEBitmap::GetWidth() const
{
	return Width;
}

ZESize ZEBitmap::GetHeight() const
{
	return Height;
}

ZESize ZEBitmap::GetPitch() const
{
	return Pitch;
}

ZEBitmapPixelFormat ZEBitmap::GetFormat() const
{
	return Format;
}

ZESize ZEBitmap::GetPixelSize() const
{
	return PixelSize;
}

ZESize ZEBitmap::GetBPP() const
{
	return PixelSize * 8;
}

ZESize ZEBitmap::GetSize() const
{
	return Height * Width * PixelSize; 
}

void ZEBitmap::SetData(void* Data)
{

	memcpy(this->Data, Data, Pitch * Height);
}

void* ZEBitmap::GetData()
{
	return Data;
}

const void* ZEBitmap::GetData() const
{
	return Data;
}

void ZEBitmap::SetRow(ZESize Index, void* Data)
{
	memcpy(GetRow(Index), Data, Pitch);
}

void* ZEBitmap::GetRow(ZESize Index)
{
	zeDebugCheck(Index >= GetHeight(), "Index is out of range");

	return (ZEUInt8*)Data + Index * Pitch;
}

const void* ZEBitmap::GetRow(ZESize Index) const
{
	zeDebugCheck(Index >= GetHeight(), "Index is out of range");

	return (ZEUInt8*)Data + Index * Pitch;
}

ZEPixelARGB* ZEBitmap::GetPixelsARGB()
{
	zeDebugCheck(GetFormat() != ZE_BPF_ARGB, "Wrong pixel format");

	return static_cast<ZEPixelARGB*>(GetData());
}

const ZEPixelARGB* ZEBitmap::GetPixelsARGB() const
{
	zeDebugCheck(GetFormat() != ZE_BPF_ARGB, "Wrong pixel format");

	return static_cast<const ZEPixelARGB*>(GetData());
}

ZEPixelRGB* ZEBitmap::GetPixelsRGB()
{
	zeDebugCheck(GetFormat() != ZE_BPF_RGB, "Wrong pixel format");

	return static_cast<ZEPixelRGB*>(GetData());
}

const ZEPixelRGB* ZEBitmap::GetPixelsRGB() const
{
	zeDebugCheck(GetFormat() != ZE_BPF_RGB, "Wrong pixel format");

	return static_cast<const ZEPixelRGB*>(GetData());
}

ZEPixelGrayscale* ZEBitmap::GetPixelsGrayscale()
{
	zeDebugCheck(GetFormat() != ZE_BPF_GRAYSCALE, "Wrong pixel format");

	return static_cast<ZEPixelGrayscale*>(GetData());
}

const ZEPixelGrayscale* ZEBitmap::GetPixelsGrayscale() const
{
	zeDebugCheck(GetFormat() != ZE_BPF_GRAYSCALE, "Wrong pixel format");

	return static_cast<const ZEPixelGrayscale*>(GetData());
}

void* ZEBitmap::GetPixel(ZESize x, ZESize y)
{
	zeDebugCheck(x >= GetWidth(), "x is out of range");

	return (ZEBYTE*)GetRow(y) + x * PixelSize;
}

const void* ZEBitmap::GetPixel(ZESize x, ZESize y) const
{
	zeDebugCheck(x >= GetWidth(), "x is out of range");

	return (ZEBYTE*)GetRow(y) + x * PixelSize;
}

ZEPixelARGB& ZEBitmap::GetPixelARGB(ZESize x, ZESize y)
{
	zeDebugCheck(GetFormat() != ZE_BPF_ARGB, "Wrong pixel format");
	zeDebugCheck(x >= GetWidth(), "x is out of range");

	return *reinterpret_cast<ZEPixelARGB*>((ZEBYTE*)GetRow(y) + x * sizeof(ZEPixelARGB));
}

const ZEPixelARGB& ZEBitmap::GetPixelARGB(ZESize x, ZESize y) const
{
	zeDebugCheck(GetFormat() != ZE_BPF_ARGB, "Wrong pixel format");
	zeDebugCheck(x >= GetWidth(), "x is out of range");

	return *reinterpret_cast<const ZEPixelARGB*>((const ZEBYTE*)GetRow(y) + x * sizeof(ZEPixelARGB));
}

ZEPixelRGB& ZEBitmap::GetPixelRGB(ZESize x, ZESize y)
{
	zeDebugCheck(GetFormat() != ZE_BPF_RGB, "Wrong pixel format");
	zeDebugCheck(x >= GetWidth(), "x is out of range");

	return *reinterpret_cast<ZEPixelRGB*>((ZEBYTE*)GetRow(y) + x * sizeof(ZEPixelRGB));
}

const ZEPixelRGB& ZEBitmap::GetPixelRGB(ZESize x, ZESize y) const
{
	zeDebugCheck(GetFormat() != ZE_BPF_RGB, "Wrong pixel format");
	zeDebugCheck(x >= GetWidth(), "x is out of range");

	return *reinterpret_cast<const ZEPixelRGB*>((const ZEBYTE*)GetRow(y) + x * sizeof(ZEPixelRGB));
}

ZEPixelGrayscale& ZEBitmap::GetPixelGrayscale(ZESize x, ZESize y)
{
	zeDebugCheck(GetFormat() != ZE_BPF_RGB, "Wrong pixel format");
	zeDebugCheck(x >= GetWidth(), "x is out of range");

	return *reinterpret_cast<ZEPixelGrayscale*>((ZEBYTE*)GetRow(y) + x * sizeof(ZEPixelGrayscale));
}

const ZEPixelGrayscale& ZEBitmap::GetPixelGrayscale(ZESize x, ZESize y) const
{
	zeDebugCheck(GetFormat() != ZE_BPF_RGB, "Wrong pixel format");
	zeDebugCheck(x >= GetWidth(), "x is out of range");

	return *reinterpret_cast<const ZEPixelGrayscale*>((const ZEBYTE*)GetRow(y) + x * sizeof(ZEPixelGrayscale));
}

ZEVector4 ZEBitmap::GetPixelVector(ZESize x, ZESize y) const
{
	switch (Format)
	{
		case ZE_BPF_ARGB:
			return GetPixelARGB(x, y).ToVector4();

		case ZE_BPF_RGB:
			return GetPixelRGB(x, y).ToVector4();
			
		case ZE_BPF_GRAYSCALE:
			return GetPixelGrayscale(x, y).ToVector4();

		default:
			return ZEVector4::Zero;
	}
}

void* ZEBitmap::AddressPixel(ZESSize x, ZESSize y, const ZEBitmapAddressingOptions& Options)
{
	switch(Options.AddressingX)
	{
		case ZE_BAM_WRAP:
		{
			x = x % GetWidth();
			break;
		}

		case ZE_BAM_CLAMP:
		{
			if (x >= (ZESSize)Width)
				x = Width -1;
			else if (x < 0)
				x = 0;
			break;
		}

		case ZE_BAM_MIRROR:
		{
			bool Odd = ((x / Width) % 2) == 1;
			if (Odd)
			{
				if (x >= (ZESSize)Width)
					x = Width - 1;
				else if (x < 0)
					x = 0;
			}
			else
			{
				if (x >= (ZESSize)Width)
					x = Width - x - 1;
				else if (x < 0)
					x = Width - 1;
			}
			break;
		}

		case ZE_BAM_BORDER:
		{
			if (x < 0 || x >= (ZESSize)Width)
				return NULL;
			break;
		}
	}

	switch(Options.AddressingY)
	{
		case ZE_BAM_WRAP:
		{
			y = y % GetHeight();
			break;
		}

		case ZE_BAM_CLAMP:
		{
			if (y >= (ZEInt)Height)
				y = Height - 1;
			else if (y < 0)
				y = 0;
			break;
		}

		case ZE_BAM_MIRROR:
		{
			bool Odd = ((y / Height) % 2) == 1;
			if (Odd)
			{
				if (y >= (ZESSize)Height)
					y = Height - 1;
				else if (y < 0)
					y = 0;
			}
			else
			{
				if (y >= (ZESSize)Height)
					y = Height - y - 1;
				else if (y < 0)	
					y = Height - 1;
			}
			break;
		}

		case ZE_BAM_BORDER:
		{
			if (y < 0 || y >= (ZEInt)Height)
				return NULL;
			break;
		}
	}

	return GetPixel(x, y);
}

const void* ZEBitmap::AddressPixel(ZESSize x, ZESSize y, const ZEBitmapAddressingOptions& Options) const
{
	return const_cast<ZEBitmap*>(this)->AddressPixel(x, y, Options);
}

template<typename ZEPixelType>
ZEVector4 SamplePixelInternal(const ZEBitmap* Bitmap, const ZEVector2& Texcoords, const ZEBitmapSamplingOptions& Options)
{
	if (Options.Filter == ZE_BFM_POINT)
	{
		ZESSize x = (ZESSize)Texcoords.x;
		ZESSize y = (ZESSize)Texcoords.y;

		const void* Address = Bitmap->AddressPixel(x, y, Options);
		return (Address != NULL ? static_cast<const ZEPixelType*>(Address)->ToVector4() : Options.BorderColor);
	}
	else if (Options.Filter == ZE_BFM_BILINEAR)
	{
		ZESSize x = (ZESSize)Texcoords.x;
		ZESSize y = (ZESSize)Texcoords.y;

		float RatioU = Texcoords.x - (float)x;
		float RatioV = Texcoords.y - (float)y;

		const void* AddressA = Bitmap->AddressPixel(x, y, Options);
		ZEVector4 A = (AddressA != NULL ? static_cast<const ZEPixelType*>(AddressA)->ToVector4() : Options.BorderColor);

		const void* AddressB = Bitmap->AddressPixel(x + 1, y, Options);
		ZEVector4 B = (AddressB != NULL ? static_cast<const ZEPixelType*>(AddressB)->ToVector4() : Options.BorderColor);

		const void* AddressC = Bitmap->AddressPixel(x, y + 1, Options);
		ZEVector4 C = (AddressC != NULL ? static_cast<const ZEPixelType*>(AddressC)->ToVector4() : Options.BorderColor);

		const void* AddressD = Bitmap->AddressPixel(x + 1, y + 1, Options);
		ZEVector4 D = (AddressD != NULL ? static_cast<const ZEPixelType*>(AddressD)->ToVector4() : Options.BorderColor);

		ZEVector4 Row0, Row1, Output;
		ZEVector4::Lerp(Row0, A, B, RatioU);
		ZEVector4::Lerp(Row1, C, D, RatioU);

		ZEVector4::Lerp(Output, Row0, Row1, RatioV);

		return Output;
	}

	return ZEVector4::Zero;
}

ZEVector4 ZEBitmap::SamplePixel(const ZEVector2& Texcoords, const ZEBitmapSamplingOptions& Options) const
{
	switch (Format)
	{
		case ZE_BPF_ARGB:
			return SamplePixelInternal<ZEPixelARGB>(this, Texcoords, Options);
			
		case ZE_BPF_RGB:
			return SamplePixelInternal<ZEPixelRGB>(this, Texcoords, Options);

		case ZE_BPF_GRAYSCALE:
			return SamplePixelInternal<ZEPixelGrayscale>(this, Texcoords, Options);

		default:
			return ZEVector4::Zero;
	}
}

void ZEBitmap::CopyFrom(void* SourceBuffer, ZESize SourcePitch, ZESize Width, ZESize Height, ZESize SourceOffsetX, ZESize SourceOffsetY, ZESize DestinationOffsetX, ZESize DestinationOffsetY)
{
	if (Height == 0)
		Height = this->Height;

	if (Width == 0)
		Width = this->Width;

	for (ZESize I = 0; I < (ZESize)Height; I++)
	{
		memcpy((ZEUInt8*)Data + (DestinationOffsetY + I) * Pitch + DestinationOffsetX * PixelSize, 
			(ZEUInt8*)SourceBuffer + (SourceOffsetY + I) * SourcePitch + SourceOffsetX * PixelSize, 
			Width * PixelSize);
	}
}

void ZEBitmap::CopyTo(void* DestinationBuffer, ZESize DestinationPitch, ZESize Width, ZESize Height, ZESize DestinationOffsetX, ZESize DestinationOffsetY, ZESize SourceOffsetX, ZESize SourceOffsetY)
{
	if (Height == 0)
		Height = this->Height;

	if (Width == 0)
		Width = this->Width;

	for (ZESize I = 0; I < Height; I++)
	{
		memcpy((ZEUInt8*)DestinationBuffer + (DestinationOffsetY + I) * DestinationPitch + DestinationOffsetX * PixelSize, 
			(ZEUInt8*)Data + (SourceOffsetY + I) * Pitch + SourceOffsetX * PixelSize, 
			Width * PixelSize);
	}
}

void ZEBitmap::Fill(ZEUInt Color)
{
	memset(Data, Color, (ZESize)Height * Pitch);
}

bool ZEBitmap::Create(ZESize Width, ZESize Height, ZEBitmapPixelFormat Format)
{
	if (Data != NULL && this->Width == Width && this->Height == Height && this->Format == Format)
		return true;

	if (Width == 0 || Height == 0)
	{
		zeError("Cannot create ZEBitmap. Can not create bitmap with zero dimension.");
		return false;
	}

	ZESize PixelSize = 0;
	switch (Format)
	{
		case ZE_BPF_ARGB:
			PixelSize = 4;
			break;

		case ZE_BPF_RGB:
			PixelSize = 3;
			break;

		case ZE_BPF_GRAYSCALE:
			PixelSize = 1;
			break;

		default:
		case ZE_BPF_NONE:
			zeError("Cannot create ZEBitmap. Unknown format.");
			return false;
	}

	Release();

	this->Pitch		= Width * PixelSize;
	this->Width		= Width;
	this->Height	= Height;
	this->Format	= Format;
	this->PixelSize = PixelSize;
	this->Data	= new ZEUInt8[Width * Height * PixelSize];

	return true;
}

bool ZEBitmap::Load(const ZEString& FileName)
{
	ZERealPath RealPath = ZEPathInfo(FileName).GetRealPath();
	if ((RealPath.Access & ZE_PA_READ) == 0)
	{
		zeError("Cannot load ZEBitmap from file. Read access denied. File Name: \"%s\".", FileName.ToCString());
		return false;
	}

	FREE_IMAGE_FORMAT FIFormat = FreeImage_GetFileType(RealPath.Path, 0);
	if (FIFormat == FIF_UNKNOWN)
		FIFormat = FreeImage_GetFIFFromFilename(RealPath.Path);

	FIBITMAP* FIBitmap = FreeImage_Load(FIFormat, RealPath.Path, 0);
	if (FIBitmap == NULL)
	{
		zeError("Cannot load ZEBitmap from file. Internal error. File Name: \"%s\".", FileName.ToCString());
		return false;
	}


	ZEUInt Width = FreeImage_GetWidth(FIBitmap);
	ZEUInt Height = FreeImage_GetHeight(FIBitmap);
	ZEUInt BPP = FreeImage_GetBPP(FIBitmap);
	FREE_IMAGE_TYPE Type = FreeImage_GetImageType(FIBitmap);

	ZEBitmapPixelFormat Format = ZE_BPF_NONE;

	if (Type != FIT_BITMAP)
	{
		FreeImage_Unload(FIBitmap);
		zeError("Cannot load ZEBitmap from file. Unsupported pixel format. Unsupported pixel format. File Name: \"%s\".", FileName.ToCString());
		return false;
	}

	switch (BPP)
	{
		case 8:
			Format = ZE_BPF_GRAYSCALE;
			break;
		
		case 24:
			Format = ZE_BPF_RGB;
			break;
		
		case 32:
			Format = ZE_BPF_ARGB;
			break;

		default:
			FreeImage_Unload(FIBitmap);
			zeError("Cannot load ZEBitmap from file. Unsupported pixel format. Unsupported pixel format. File Name: \"%s\".", FileName.ToCString());
			return false;
	}

	this->Create(Width, Height, Format);

	switch (Format)
	{
		case ZE_BPF_ARGB:
			FreeImage_ConvertToRawBits((BYTE*)Data, FIBitmap, (int)Pitch, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, TRUE);
			break;

		case ZE_BPF_RGB:
			FreeImage_ConvertToRawBits((BYTE*)Data, FIBitmap, (int)Pitch, 24, 0x00FF0000, 0x0000FF00, 0x000000FF, TRUE);
			break;

		case ZE_BPF_GRAYSCALE:
			FreeImage_ConvertToRawBits((BYTE*)Data, FIBitmap, (int)Pitch, 8, 0x000000FF, 0x00000000, 0x00000000, TRUE);
			break;
	}

	FreeImage_Unload(FIBitmap);

	return true;
}

bool ZEBitmap::Save(const ZEString& FileName, ZEBitmapFileFormat FileFormat) const
{
	FREE_IMAGE_FORMAT FIFormat;
	switch(FileFormat)
	{
		case ZE_BFF_BMP:
			FIFormat = FIF_BMP;
			break;

		case ZE_BFF_TGA:
			FIFormat = FIF_TARGA;
			break;

		case ZE_BFF_PNG:
			FIFormat = FIF_PNG;
			break;

		case ZE_BFF_JPG:
			FIFormat = FIF_JPEG;
			break;

		case ZE_BFF_TIFF:
			FIFormat = FIF_TIFF;
			break;
	
		default:
			zeError("Cannot save ZEBitmap to file. Unknwon file format. File Name: \"%s\".", FileName.ToCString());
			return false;
	}


	FIBITMAP* FIBitmap = NULL;
	switch (Format)
	{
		case ZE_BPF_ARGB:
			FreeImage_ConvertFromRawBits((BYTE*)Data, (int)Width, (int)Height, (int)Pitch, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, TRUE);
			break;

		case ZE_BPF_RGB:
			FreeImage_ConvertFromRawBits((BYTE*)Data, (int)Width, (int)Height, (int)Pitch, 24, 0x00FF0000, 0x0000FF00, 0x000000FF, TRUE);
			break;

		case ZE_BPF_GRAYSCALE:
			FreeImage_ConvertFromRawBits((BYTE*)Data, (int)Width, (int)Height, (int)Pitch, 8, 0x00000000, 0x00000000, 0x000000FF, TRUE);
			break;

		default:
			zeError("Cannot save ZEBitmap to file. Unknwon pixel format. File Name: \"%s\".", FileName.ToCString());
			return false;
	}

	if (!FreeImage_Save(FIFormat, FIBitmap, FileName))
	{
		zeError("Cannot save ZEBitmap to file. Internal error. File Name: \"%s\".", FileName.ToCString());
		return false;
	}

	FreeImage_Unload(FIBitmap);

	return true;
}

void ZEBitmap::Release()
{
	if (Data != NULL)
		delete[] Data;
	
	Width = 0;
	Height = 0;
	Format = ZE_BPF_NONE;
	Pitch = 0;
	PixelSize = 0;
	Data = NULL;
}

ZEBitmap::ZEBitmap()
{
	Width = 0;
	Height = 0;
	Pitch = 0;
	PixelSize = 0;
	Data = NULL;
}

ZEBitmap::~ZEBitmap()
{
	Release();
}
