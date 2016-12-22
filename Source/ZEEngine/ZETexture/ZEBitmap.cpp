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
#include <memory.h>
#include "ZEMath/ZEMath.h"

#include <FreeImage.h>
#include "ZEFile/ZEPathInfo.h"
#include "ZEFile/ZEFileInfo.h"

ZEPixelColor ZEPixelColor::Lerp(const ZEPixelColor& A, const ZEPixelColor& B, float T)
{
	ZEPixelColor Temp;
	Temp.a = (unsigned char)((float)A.a * (1.0f - T) + (float)B.a * T);
	Temp.r = (unsigned char)((float)A.r * (1.0f - T) + (float)B.r * T);
	Temp.g = (unsigned char)((float)A.g * (1.0f - T) + (float)B.g * T);
	Temp.b = (unsigned char)((float)A.b * (1.0f - T) + (float)B.b * T);

	return Temp;
}

ZEVector4 ZEPixelColor::LerpFloat(const ZEPixelColor& A, const ZEPixelColor& B, float T)
{
	ZEVector4 Temp;
	Temp.w = (unsigned char)((float)A.a * (1.0f - T) + (float)B.a * T);
	Temp.x = (unsigned char)((float)A.r * (1.0f - T) + (float)B.r * T);
	Temp.y = (unsigned char)((float)A.g * (1.0f - T) + (float)B.g * T);
	Temp.z = (unsigned char)((float)A.b * (1.0f - T) + (float)B.b * T);

	return Temp;
}


ZEPixelColor::ZEPixelColor()
{
	r = 0x00;
	g = 0x00;
	b = 0x00;
	a = 0xFF;
}

ZEPixelColor::ZEPixelColor(ZEUInt Color)
{
	this->a = (Color & 0xFF000000) >> 24;
	this->r = (Color & 0x00FF0000) >> 16;
	this->g = (Color & 0x0000FF00) >> 8;
	this->b = (Color & 0x000000FF);
}

ZEPixelColor::ZEPixelColor(unsigned char a, unsigned char r, unsigned char g, unsigned char b)
{
	this->a = a;
	this->r = r;
	this->g = g;
	this->b = b;
}

bool ZEBitmap::Create(ZEUInt Width, ZEUInt Height, ZESize PixelSize)
{
	if (Pixels != NULL && this->Width == Width && this->Height == Height && this->PixelSize == PixelSize)
		return true;

	if (Width == 0 || Height == 0 || PixelSize == 0)
	{
		zeError("Can not create bitmap with zero dimension.");
		return false;
	}
	
	this->Pitch		= (ZESize)Width * PixelSize;
	this->Width		= Width;
	this->Height	= Height;
	this->PixelSize = PixelSize;
	this->Pixels	= new ZEUInt8[(ZESize)Width * (ZESize)Height * PixelSize];

	return true;
}

ZEUInt ZEBitmap::GetWidth()
{
	return Width;
}

ZEUInt ZEBitmap::GetHeight()
{
	return Height;
}

ZESize ZEBitmap::GetPitch()
{
	return Pitch;
}

ZESize ZEBitmap::GetPixelSize()
{
	return PixelSize;
}

ZESize ZEBitmap::GetBPP()
{
	return PixelSize * 8;
}

ZESize ZEBitmap::GetSize()
{
	return (ZESize)Height * (ZESize)Width * PixelSize; 
}

ZEPixelColor* ZEBitmap::GetPixels()
{
	return (ZEPixelColor*)Pixels;
}

ZEPixelColor& ZEBitmap::GetPixel(ZEUInt x, ZEUInt y)
{
	return *(ZEPixelColor*)((ZEUInt8*)Pixels + (ZESize)y * Pitch + (ZESize)x * 4);
}

ZEVector4 ZEBitmap::GetPixelFloat(ZEUInt x, ZEUInt y)
{
	ZEPixelColor& Color = GetPixel(x, y);
	ZEVector4 FloatColor;

	FloatColor.x = (float)Color.r / 255.0f;
	FloatColor.y = (float)Color.g / 255.0f;
	FloatColor.z = (float)Color.b / 255.0f;
	FloatColor.w = (float)Color.a / 255.0f;

	return FloatColor;
}

ZEPixelColor* ZEBitmap::GetRow(ZEUInt Index)
{
	return (ZEPixelColor*)((ZEUInt8*)Pixels + (ZESize)Index * Pitch);
}

ZEPixelColor& ZEBitmap::SamplePixel(ZEInt x, ZEInt y, ZEBitmapSamplingOptions* UserOptions)
{
	static ZEBitmapSamplingOptions DefaultOptions = {ZE_BFM_BILINEAR, ZE_BAM_WRAP, ZE_BAM_WRAP, ZEPixelColor(0, 0, 0 ,0)};

	ZEBitmapSamplingOptions* Options;
	if (UserOptions == NULL)
		Options = &DefaultOptions;
	else
		Options = UserOptions;

	switch(Options->AddressingX)
	{
		case ZE_BAM_WRAP:
		{
			x = x % GetWidth();
			break;
		}

		case ZE_BAM_CLAMP:
		{
			if (x >= (ZEInt)Width)
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
				if (x >= (ZEInt)Width)
					x = Width - 1;
				else if (x < 0)
					x = 0;
			}
			else
			{
				if (x >= (ZEInt)Width)
					x = Width - x - 1;
				else if (x < 0)
					x = Width - 1;
			}
			break;
		}

		case ZE_BAM_BORDER:
		{
			if (x >= (ZEInt)Width)
				return Options->BorderColor;
			else if (x < 0)
				return Options->BorderColor;
		}
	}

	switch(Options->AddressingY)
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
				if (y >= (ZEInt)Height)
					y = Height - 1;
				else if (y < 0)
					y = 0;
			}
			else
			{
				if (y >= (ZEInt)Height)
					y = Height - y - 1;
				else if (y < 0)	
					y = Height - 1;
			}
			break;
		}

		case ZE_BAM_BORDER:
		{
			if (y >= (ZEInt)Height)
				return Options->BorderColor;
			else if (y < 0)
				return Options->BorderColor;
			break;
		}
	}

	return GetPixel(x, y);
}

ZEVector4 ZEBitmap::SamplePixelFloat(ZEInt x, ZEInt y, ZEBitmapSamplingOptions* UserOptions)
{
	ZEPixelColor Color = SamplePixel(x, y, UserOptions);
	ZEVector4 FloatColor;

	FloatColor.x = (float)Color.r / 255.0f;
	FloatColor.y = (float)Color.g / 255.0f;
	FloatColor.z = (float)Color.b / 255.0f;
	FloatColor.w = (float)Color.a / 255.0f;

	return FloatColor;
}

ZEVector4 ZEBitmap::SamplePixelFloat(const ZEVector2& TextureCoordinate, ZEBitmapSamplingOptions* UserOptions)
{
	static ZEBitmapSamplingOptions DefaultOptions = {ZE_BFM_BILINEAR, ZE_BAM_WRAP, ZE_BAM_WRAP, ZEPixelColor(0, 0, 0 ,0)};

	ZEBitmapSamplingOptions* Options;
	if (UserOptions == NULL)
		Options = &DefaultOptions;
	else
		Options = UserOptions;

	if (Options->Filter == ZE_BFM_POINT)
	{
		return SamplePixelFloat((ZEInt)ZEMath::Floor(TextureCoordinate.x), (ZEInt)ZEMath::Floor(TextureCoordinate.y), Options);
	}

	if (Options->Filter == ZE_BFM_BILINEAR)
	{
		ZEInt x = (ZEInt)ZEMath::Floor(TextureCoordinate.x);
		ZEInt y = (ZEInt)ZEMath::Floor(TextureCoordinate.y);

		float RatioU = TextureCoordinate.x - (float)x;
		float RatioV = TextureCoordinate.y - (float)y;

		ZEVector4 A = SamplePixelFloat(x, y, Options);
		ZEVector4 B = SamplePixelFloat(x + 1, y, Options);
		ZEVector4 C = SamplePixelFloat(x, y + 1, Options);
		ZEVector4 D = SamplePixelFloat(x + 1, y + 1, Options);

		ZEVector4 Row0, Row1, Output;
		ZEVector4::Lerp(Row0, A, B, RatioU);
		ZEVector4::Lerp(Row1, C, D, RatioU);
		
		ZEVector4::Lerp(Output, Row0, Row1, RatioV);

		return Output;
	}

	return ZEVector4::Zero;
}

void ZEBitmap::CopyFrom(void* SourceBuffer, ZESize SourcePitch, 
						ZEUInt Width, ZEUInt Height, 
						ZEUInt SourceOffsetX, ZEUInt SourceOffsetY,
						ZEUInt DestinationOffsetX, ZEUInt DestinationOffsetY)

{
	if (Height == 0)
		Height = this->Height;

	if (Width == 0)
		Width = this->Width;

	for (ZESize I = 0; I < (ZESize)Height; I++)
		memcpy((ZEUInt8*)Pixels + ((ZESize)DestinationOffsetY + I) * Pitch + (ZESize)DestinationOffsetX * PixelSize, 
			(ZEUInt8*)SourceBuffer + ((ZESize)SourceOffsetY + I) * SourcePitch + (ZESize)SourceOffsetX * PixelSize, 
			(ZESize)Width * PixelSize);
}

void ZEBitmap::CopyTo(void* DestinationBuffer, ZESize DestinationPitch, 
					  ZEUInt Width, ZEUInt Height, 
					  ZEUInt DestinationOffsetX, ZEUInt DestinationOffsetY,
					  ZEUInt SourceOffsetX, ZEUInt SourceOffsetY)
{
	if (Height == 0)
		Height = this->Height;

	if (Width == 0)
		Width = this->Width;

	for (ZESize I = 0; I < (ZESize)Height; I++)
		memcpy((ZEUInt8*)DestinationBuffer + ((ZESize)DestinationOffsetY + I) * DestinationPitch + (ZESize)DestinationOffsetX * PixelSize, 
			(ZEUInt8*)Pixels + ((ZESize)SourceOffsetY + I) * Pitch + (ZESize)SourceOffsetX * PixelSize, 
			(ZESize)Width * PixelSize);
}

void ZEBitmap::Fill(ZEUInt Color)
{
	memset(Pixels, Color, (ZESize)Height * Pitch);
}

void ZEBitmap::Clear()
{
	Fill(0x00);
}

bool ZEBitmap::Load(const char* FileName)
{
	ZERealPath RealPath = ZEPathInfo(FileName).GetRealPath();
	if ((RealPath.Access | ZE_PA_READ) == 0)
	{
		zeError("Can not open bitmap file.");
		return false;
	}

	FREE_IMAGE_FORMAT FIFormat = FreeImage_GetFileType(RealPath.Path, 0);

	if (FIFormat == FIF_UNKNOWN)
		FIFormat = FreeImage_GetFIFFromFilename(RealPath.Path);
	ZEString Dasda = FreeImage_GetFormatFromFIF(FIFormat);

	FIBITMAP* FIBitmap = FreeImage_Load(FIFormat, RealPath.Path, 0);

	if (FIBitmap == NULL)
	{
		zeError("Can not open bitmap file.");
		return false;
	}

	Width		= FreeImage_GetWidth(FIBitmap);
	Height		= FreeImage_GetHeight(FIBitmap);
	Pitch		= (ZESize)FreeImage_GetPitch(FIBitmap);
	PixelSize	= (ZESize)FreeImage_GetBPP(FIBitmap) / 8;
	
	FIBITMAP* FIConvertedBitmap;
	if (PixelSize != 4)
	{
		FIConvertedBitmap = FreeImage_ConvertTo32Bits(FIBitmap);
		FreeImage_Unload(FIBitmap);
	}
	else
	{
		FIConvertedBitmap = FIBitmap;
	}

	PixelSize = 4;
	this->Create(Width, Height, PixelSize);

	FreeImage_ConvertToRawBits((BYTE*)Pixels, FIConvertedBitmap, (int)Pitch, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, TRUE);

	FreeImage_Unload(FIConvertedBitmap);

	return true;
}

void ZEBitmap::Save(const char* FileName, ZEBitmapFileFormat Format)
{
	FIBITMAP* FIBitmap = FreeImage_ConvertFromRawBits((BYTE*)Pixels, (int)Width, (int)Height, (int)Pitch, PixelSize * 8, 0x00FF0000, 0x0000FF00, 0x000000FF, TRUE);
	FREE_IMAGE_FORMAT FIFormat;
	switch(Format)
	{
		default:
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
	}

	FreeImage_Save(FIFormat, FIBitmap, FileName);

	FreeImage_Unload(FIBitmap);
}

void ZEBitmap::Release()
{
	if (Pixels != NULL)
		delete[] Pixels;
	
	Width = 0;
	Height = 0;
	Pitch = 0;
	PixelSize = 0;
	Pixels = NULL;
}

ZEBitmap::ZEBitmap()
{
	Width = 0;
	Height = 0;
	Pitch = 0;
	PixelSize = 0;
	Pixels = NULL;
}

ZEBitmap::~ZEBitmap()
{
	Release();
}
