//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETEPatchFile.cpp
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

#include "ZETEPatchFile.h"
#include "ZEError.h"

#include <FreeImage.h>

bool ZETEPatchFile::GetData(void* Output, ZEUInt64 x, ZEUInt64 y, ZESize Width, ZESize Height)
{
	if (x + Width >= this->Width)
		return false;

	if (y + Height >= this->Height)
		return false;

	ZESize PixelSize = GetPixelSize();
	ZESize DestinationPitch = Width * PixelSize;

	ZEBYTE* Source = (ZEBYTE*)Data + y * Pitch + x * PixelSize;
	ZEBYTE* Destination = (ZEBYTE*)Output;

	for (ZEUInt64 I = 0; I < Height; I++)
	{
		memcpy(Destination, Source, DestinationPitch);
		Source += Pitch;
		Destination += DestinationPitch;
	}

	return true;
}

bool ZETEPatchFile::Create(ZESize Width, ZESize Height, ZETEPixelType Type)
{
	if (Data != NULL)
	{
		delete Data;
		Data = NULL;
	}

	PixelType = Type;
	this->Width = Width;
	this->Height = Height;
	
	Pitch = Width * GetPixelSize();
	Data = new ZEUInt8[Height * Pitch];

	UpdateLevelAndScaling();

	return true;
}

bool ZETEPatchFile::Load()
{
	Unload();

	PixelType = ZETE_PT_COLOR;

	FREE_IMAGE_FORMAT BitmapFileFormat = FreeImage_GetFileType(GetSource());
	if (BitmapFileFormat == FIF_UNKNOWN)
	{
		zeError("Unknown bitmap file format.");
		return false;
	}

	FIBITMAP* Bitmap = FreeImage_Load(BitmapFileFormat, GetSource());
	if (Bitmap == NULL)
	{
		zeError("Can not load bitmap.");
		return false;
	}

	if (!Create(FreeImage_GetWidth(Bitmap), FreeImage_GetHeight(Bitmap), ZETE_PT_COLOR))
	{
		FreeImage_Unload(Bitmap);
		return false;
	}

	switch(PixelType)
	{

		case ZETE_PT_COLOR:
			FreeImage_ConvertToRawBits((BYTE*)Data, Bitmap, Pitch, 32, 0, 0, 0, true);
			FreeImage_Unload(Bitmap);
			break;
	}
	
	return ZETEPatch::Load();
}

void ZETEPatchFile::Unload()
{
	PixelType = ZETE_PT_NONE;
	Width = 0;
	Height = 0;
	Pitch = 0;

	if (Data == NULL)
	{
		delete[] (ZEUInt8*)Data;
		Data = NULL;
	}
}

ZETEPatchFile::ZETEPatchFile()
{
	Data = NULL;
	PixelType = ZETE_PT_NONE;
	Width = 0;
	Height = 0;
	Pitch = 0;
}

ZETEPatchFile::~ZETEPatchFile()
{
	Unload();
}
