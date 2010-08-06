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
#include <FreeImage.h>

bool ZEBitmap::Create(unsigned int Width, unsigned int Height, unsigned int PixelSize)
{
	if (Pixels != NULL && this->Width == Width && this->Height == Height && this->PixelSize == PixelSize)
		return true;

	if (Width == 0 || Height == 0 || Pixels == 0)
	{
		zeError("ZEBitmap", "Can not create bitmap with zero dimension.");
		return false;
	}
	
	this->Pitch = Width * PixelSize;
	this->Width = Width;
	this->Height = Height;
	this->PixelSize = PixelSize;
}

unsigned int ZEBitmap::GetWidth()
{
	return Width;
}

unsigned int ZEBitmap::GetHeight()
{
	return Height;
}

unsigned int ZEBitmap::GetPitch()
{
	return Pitch;
}

unsigned int ZEBitmap::GetPixelSize()
{
	return PixelSize;
}

unsigned int ZEBitmap::GetBPP()
{
	return PixelSize * 8;
}

ZEBYTE* ZEBitmap::GetPixels()
{
	return Pixels;
}

void ZEBitmap::CopyFrom(ZEBYTE* SourceBuffer, unsigned int SourcePitch, 
						unsigned int Width, unsigned int Height, 
						unsigned int SourceOffsetX, unsigned int SourceOffsetY,
						unsigned int DestinationOffsetX, unsigned int DestinationOffsetY)

{
	if (Height == 0)
		Height = this->Height;

	if (Width == 0)
		Width = this->Width;

	for (size_t I = 0; I < Height; I++)
		memcpy(Pixels + (SourceOffsetY + I) * Pitch + SourceOffsetX * PixelSize, 
			SourceBuffer + (SourceOffsetY + I) * SourcePitch + SourceOffsetX * PixelSize, 
			Width * PixelSize);
}

void ZEBitmap::CopyTo(ZEBYTE* DestinationBuffer, unsigned int DestinationPitch, 
					  unsigned int Width, unsigned int Height, 
					  unsigned int DestinationOffsetX, unsigned int DestinationOffsetY,
					  unsigned int SourceOffsetX, unsigned int SourceOffsetY)
{
	if (Height == 0)
		Height = this->Height;

	if (Width == 0)
		Width = this->Width;

	for (size_t I = 0; I < Height; I++)
		memcpy(DestinationBuffer + (DestinationOffsetY + I) * DestinationPitch + DestinationOffsetX * PixelSize, 
			Pixels + (SourceOffsetY + I) * Pitch + SourceOffsetX * PixelSize, 
			Width * PixelSize);
}

bool ZEBitmap::Load(const char* FileName)
{
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(FileName, 0);

	FIBITMAP* FIBitmap = FreeImage_Load (fif, FileName, 0);
	if (FIBitmap == NULL)
	{
		zeError("ZEBitmap", "Can not open bitmap file.");
		return false;
	}

	Width = FreeImage_GetWidth(FIBitmap);
	Height = FreeImage_GetHeight(FIBitmap);
	Pitch = FreeImage_GetPitch(FIBitmap);
	PixelSize = FreeImage_GetBPP(FIBitmap) / 8;
	
	FIBITMAP* ConvertedBitmap;
	if (PixelSize != 4)
	{
		ConvertedBitmap = FreeImage_ConvertTo32Bits(bitmap);
		FreeImage_Unload(FIBitmap);
	}
	else
		ConvertedBitmap = FIBitmap;

	PixelSize = 4;
	Create(Width, Height, PixelSize);

	FreeImage_ConvertToRawBits(Pixels, ConvertedBitmap, Bitmap->Pitch, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, FALSE);

	FreeImage_Unload(FIBitmap);

	return true;
}

void ZEBitmap::Save(const char* FileName, ZEBitmapFileFormat Format)
{
	FIBITMAP* FIBitmap = FreeImage_ConvertFromRawBits(Pixels, Width, Height, Pitch, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, FALSE);
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
