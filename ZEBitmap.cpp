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
						unsigned int Width = 0, unsigned int Height = 0, 
						unsigned int OffsetX = 0, unsigned int OffsetY = 0)
{

}

void ZEBitmap::CopyTo(ZEBYTE* DestinationBuffer, unsigned int DestinationPitch, 
					  unsigned int Width = 0, unsigned int Height = 0, 
					  unsigned int OffsetX = 0, unsigned int OffsetY = 0)
{
}

bool ZEBitmap::Load(const char* Filename)
{
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(FileName, 0);

	FIBITMAP* bitmap = FreeImage_Load (fif, FileName, 0);
	if (Bitmap == NULL)
	{
		zeError("ZEBitmap", "Can not open bitmap file.");
		return false;
	}


	Width = FreeImage_GetWidth(ConvertedBitmap);
	Height = FreeImage_GetHeight(ConvertedBitmap);
	Pitch = FreeImage_GetPitch(ConvertedBitmap);
	PixelSize = FreeImage_GetBPP(ConvertedBitmap) / 8;
	
	if (PixelSize != 4)
	FIBITMAP* ConvertedBitmap = FreeImage_ConvertTo32Bits(bitmap);
	Create(Width, Height, PixelSize);

	PixelSize = 4;
	FreeImage_ConvertToRawBits(Bitmap->Data, ConvertedBitmap, Bitmap->Pitch, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, FALSE);

	return true;
}

void ZEBitmap::Save(const char* FileName, ZEBitmapFileFormat Format)
{
	FIBITMAP* FIBitmap = FreeImage_ConvertFromRawBits(Bitmap->Data, Bitmap->Width, Bitmap->Height, Bitmap->Pitch, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, FALSE);
	FreeImage_Save(FIF_BMP, FIBitmap, FileName);

	FreeImage_Unload(FIBitmap);

	// Deinitialise
	FreeImage_DeInitialise();
}
