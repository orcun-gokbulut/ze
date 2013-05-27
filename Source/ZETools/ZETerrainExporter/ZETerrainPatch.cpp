//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETerrainPatch.cpp
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

#include "ZETerrainPatch.h"
#include "ZEError.h"
#include "FreeImage.h"
#include "ZEMath\ZEMath.h"

static ZEUInt8 Lerp(ZEUInt8 A, ZEUInt8 B, float Blend)
{
	return (ZEUInt8)((float)A * Blend) + ((float)B * (1.0f - Blend));
}

static ZEUInt32 Lerp(ZEUInt32 A, ZEUInt32* B, float Blend)
{
	ZEUInt32 Temp;
	((ZEUInt8*)&Temp)[0] = Lerp(((ZEUInt8*)&A)[0], ((ZEUInt8*)&B)[0], Blend);
	((ZEUInt8*)&Temp)[1] = Lerp(((ZEUInt8*)&A)[1], ((ZEUInt8*)&B)[1], Blend);
	((ZEUInt8*)&Temp)[2] = Lerp(((ZEUInt8*)&A)[2], ((ZEUInt8*)&B)[2], Blend);
	((ZEUInt8*)&Temp)[3] = Lerp(((ZEUInt8*)&A)[3], ((ZEUInt8*)&B)[3], Blend);

	return Temp;
}

void ZETerrainPatch::UpdateScale()
{
	if (Width == 0 || Height == 0 || EndX - StartX <= 0 || EndY - StartY <= 0)
	{
		ScaleX = 1.0;
		ScaleY = 1.0;
		return;
	}

	ScaleX = (EndX - StartX) / (double)Width;
	ScaleY = (EndY - StartY) / (double)Height;

	float MinScale = (ScaleX < ScaleY ? ScaleX : ScaleY);
	double LevelScale = 1024;

	Level = 10;
	while(true)
	{
		if (MinScale >= LevelScale)
			break;

		Level--;
		LevelScale /= 2;
	}
}

void ZETerrainPatch::SetStartX(double x)
{
	StartX = x;
}

double ZETerrainPatch::GetStartX()
{
	return StartX;
}

void ZETerrainPatch::SetStartY(double y)
{
	StartY = y;
}

double ZETerrainPatch::GetStartY()
{
	return StartY;
}

void ZETerrainPatch::SetEndX(double EndX)
{
	this->EndX = EndX;
	UpdateScale();
}

double ZETerrainPatch::GetEndX()
{
	return EndX;
}

void ZETerrainPatch::SetEndY(double EndY)
{
	this->EndY = EndY;
	UpdateScale();
}

double ZETerrainPatch::GetEndY()
{
	return EndY;
}

double ZETerrainPatch::GetScaleX()
{
	return ScaleX;
}

double ZETerrainPatch::GetScaleY()
{
	return ScaleY;
}

ZESize ZETerrainPatch::GetWidth()
{
	return Width;
}

ZESize ZETerrainPatch::GetHeight()
{
	return Height;
}

void* ZETerrainPatch::GetData()
{
	return Data;
}

ZESize ZETerrainPatch::GetPitch()
{
	return Pitch;
}

ZETerrainPixelType ZETerrainPatch::GetPixelType()
{
	return PixelType;
}

ZESize ZETerrainPatch::GetPixelSize()
{
	return PixelSize;
}

ZEUInt ZETerrainPatch::GetLevel()
{
	return Level;
}

void ZETerrainPatch::SetPriority(ZEUInt Priority)
{
	this->Priority = Priority;
}

ZEUInt ZETerrainPatch::GetPriority()
{
	return Priority;
}

bool  ZETerrainPatch::BoundaryCheck(double x, double y)
{
	return (x >= StartX && x <= EndX) && (y >= StartY && y <= EndY);
}

void ZETerrainPatch::Clean()
{
	Width = 0;
	Height = 0;
	Pitch = 0;
	PixelSize = 0;

	if (Data == NULL)
	{
		delete[] (ZEUInt8*)Data;
		Data = NULL;
	}
}

void* ZETerrainPatch::SampleLocalRaw(ZESSize x, ZESSize y)
{
	static float Zero = 0;
	if (x < 0 || x >= (ZESSize)Width)
		return &Zero;

	if (y < 0 || y >= (ZESSize)Height)
		return &Zero;

	return (ZEUInt8*)GetData() + Pitch * y + x * PixelSize;
}

void ZETerrainPatch::SampleLocal(void* Output, double x, double y)
{
	double u = ScaleX * x - 0.5;
	double v = ScaleY * y - 0.5;
	ZESSize px = (ZESSize)u;
	ZESSize py = (ZESSize)v;
	double BlendV = u - (double)px;
	double BlendH = v - (double)py;

	if (PixelType == ZE_TPT_ELEVATION)
	{
		float Pixel00 = *(float*)SampleLocalRaw(px,		py);
		float Pixel10 = *(float*)SampleLocalRaw(px + 1, py);
		float Pixel01 = *(float*)SampleLocalRaw(px,		py + 1);
		float Pixel11 = *(float*)SampleLocalRaw(px + 1, py + 1);
		*(float*)Output = ZEMath::Lerp(ZEMath::Lerp(Pixel00, Pixel10, BlendV), ZEMath::Lerp(Pixel10, Pixel11, BlendV), BlendH);
	}
	else if (PixelType == ZE_TPT_COLOR)
	{
		ZEUInt32 Pixel00 = *(ZEUInt32*)SampleLocalRaw(px,		py);
		ZEUInt32 Pixel10 = *(ZEUInt32*)SampleLocalRaw(px + 1,	py);
		ZEUInt32 Pixel01 = *(ZEUInt32*)SampleLocalRaw(px,		py + 1);
		ZEUInt32 Pixel11 = *(ZEUInt32*)SampleLocalRaw(px + 1,	py + 1);
		*(ZEUInt32*)Output = Lerp(Lerp(Pixel00, Pixel10, BlendV), Lerp(Pixel10, Pixel11, BlendV), BlendH);
	}
	else if (PixelType == ZE_TPT_GRAYSCALE)
	{
		ZEUInt8 Pixel00 = *(ZEUInt8*)SampleLocalRaw(px,		py);
		ZEUInt8 Pixel10 = *(ZEUInt8*)SampleLocalRaw(px + 1,	py);
		ZEUInt8 Pixel01 = *(ZEUInt8*)SampleLocalRaw(px,		py + 1);
		ZEUInt8 Pixel11 = *(ZEUInt8*)SampleLocalRaw(px + 1,	py + 1);
		*(ZEUInt8*)Output = Lerp(Lerp(Pixel00, Pixel10, BlendV), Lerp(Pixel10, Pixel11, BlendV), BlendH);
	}
}

void ZETerrainPatch::Sample(void* Output, double x, double y)
{
	SampleLocal(Output, x - StartX, y - StartY);
}

bool ZETerrainPatch::Create(ZESize Width, ZESize Height, ZETerrainPixelType Type)
{
	Clean();

	this->PixelType = Type;
	this->Width = Width;
	this->Height = Height;
	
	switch(PixelType)
	{
		case ZE_TPT_ELEVATION:
		case ZE_TPT_COLOR:
			PixelSize = 4;
			break;

		case ZE_TPT_GRAYSCALE:
			PixelSize = 1;
			break;

		default:
			return false;
	}
	
	this->PixelSize = PixelSize; 
	this->Pitch = Width * PixelSize;
	this->Data = new ZEUInt8[Height * Pitch];

	UpdateScale();

	return true;
}

bool ZETerrainPatch::Load(const char* FileName, ZETerrainPixelType PixelType)
{
	Clean();
	FREE_IMAGE_FORMAT BitmapFileFormat = FreeImage_GetFileType(FileName);
	if (BitmapFileFormat == FIF_UNKNOWN)
	{
		zeError("Unknown bitmap file format.");
		return false;
	}

	FIBITMAP* Bitmap = FreeImage_Load(BitmapFileFormat, FileName);
	if (Bitmap == NULL)
	{
		zeError("Can not load bitmap.");
		return false;
	}

	if (!Create(FreeImage_GetWidth(Bitmap), FreeImage_GetHeight(Bitmap), PixelType))
	{
		FreeImage_Unload(Bitmap);
		return false;
	}

	switch(PixelType)
	{
		case ZE_TPT_COLOR:
			FreeImage_ConvertToRawBits((BYTE*)Data, Bitmap, Pitch, PixelSize, 0x00FF0000, 0x0000FF00, 0x000000FF);
			FreeImage_Unload(Bitmap);
			break;
	}
	
	return true;
}

ZETerrainPatch::ZETerrainPatch()
{
	Data = NULL;
	Clean();

	Level = 0;
	Priority = 0;
	
	StartX = 0.0;
	StartY = 0.0;
	EndX = 0.0;
	EndY = 0.0;
	ScaleX = 1.0;
	ScaleY = 1.0;
}

ZETerrainPatch::~ZETerrainPatch()
{
	Clean();
}
