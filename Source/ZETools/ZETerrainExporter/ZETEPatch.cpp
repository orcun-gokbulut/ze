//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETEPatch.cpp
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

#include "ZETEPatch.h"

#include "ZEError.h"
#include "ZEMath\ZEMath.h"

#include <FreeImage.h>
#include <ippi.h>
#include <ippcore.h>
#include <ipps.h>

void ZETEPatch::UpdateLevelAndScaling()
{
	if (Width == 0 || Height == 0 || EndX - StartX <= 0 || EndY - StartY <= 0)
	{
		Level = 0;
		LevelScalingWidth = 1.0;
		LevelScalingHeight = 1.0;
		return;
	}

	LevelScalingWidth = (EndX - StartX) / (double)Width;
	LevelScalingHeight = (EndY - StartY) / (double)Height;

	double MinScale = (LevelScalingWidth < LevelScalingHeight ? LevelScalingWidth : LevelScalingHeight);

	double LevelTemp = log(MinScale) / log(2);
	Level = (ZEUInt)floor(LevelTemp + 0.5);

	double LevelScale = pow(2, Level);
	LevelScalingWidth = LevelScale / LevelScalingWidth;
	LevelScalingHeight = LevelScale / LevelScalingHeight;
}

void ZETEPatch::SetStartX(double x)
{
	if (StartX == x)
		return;

	StartX = x;
	UpdateLevelAndScaling();
}

double ZETEPatch::GetStartX()
{
	return StartX;
}

void ZETEPatch::SetStartY(double y)
{
	if (StartY == y)
		return;

	StartY = y;
	UpdateLevelAndScaling();
}

double ZETEPatch::GetStartY()
{
	return StartY;
}

void ZETEPatch::SetEndX(double x)
{
	if (EndX == x)
		return;

	EndX = x;
	UpdateLevelAndScaling();
}

double ZETEPatch::GetEndX()
{
	return EndX;
}

void ZETEPatch::SetEndY(double y)
{
	if (EndY == y)
		return;

	EndY = y;
	UpdateLevelAndScaling();
}

double ZETEPatch::GetEndY()
{
	return EndY;
}

double ZETEPatch::GetLevelScaleWidth()
{
	return LevelScalingWidth;
}

double ZETEPatch::GetLevelScaleHeight()
{
	return LevelScalingHeight;
}

ZESize ZETEPatch::GetWidth()
{
	return Width;
}

ZESize ZETEPatch::GetHeight()
{
	return Height;
}

void* ZETEPatch::GetData()
{
	return Data;
}

ZESize ZETEPatch::GetPitch()
{
	return Pitch;
}

ZETEPixelType ZETEPatch::GetPixelType()
{
	return PixelType;
}

ZESize ZETEPatch::GetPixelSize()
{
	switch (PixelType)
	{
		default:
		case ZE_TPT_NONE:
			return 0;

		case ZE_TPT_ELEVATION:
			return 4;

		case ZE_TPT_COLOR:
			return 4;

		case ZE_TPT_GRAYSCALE:
			return 1;
	}
}

ZEUInt ZETEPatch::GetLevel()
{
	return Level;
}

void ZETEPatch::SetPriority(ZEUInt Priority)
{
	this->Priority = Priority;
}

ZEUInt ZETEPatch::GetPriority()
{
	return Priority;
}

bool  ZETEPatch::BoundaryCheck(double x, double y)
{
	return (x >= StartX && x <= EndX) && (y >= StartY && y <= EndY);
}

void ZETEPatch::Clean()
{
	PixelType = ZE_TPT_NONE;
	Width = 0;
	Height = 0;
	Pitch = 0;

	if (Data == NULL)
	{
		delete[] (ZEUInt8*)Data;
		Data = NULL;
	}
}

void ZETEPatch::Resample(void* BlockData, ZESize BlockPitch, ZEUInt64 BlockX, ZEUInt64 BlockY, ZESize BlockSize)
{
	double PositionX = (BlockX - StartX) / pow(2, Level);
	double PositionY = (BlockY - StartY) / pow(2, Level);

	IppiSize SourceSize;
	SourceSize.width = Width;
	SourceSize.height = Height;

	IppiSize DestSize;
	DestSize.width = BlockSize;
	DestSize.height = BlockSize;

	IppiPoint Offset;
	Offset.x = PositionX;
	Offset.y = PositionY;

	IppiRect SourceROI;
	SourceROI.x = PositionX;
	SourceROI.y = PositionY;
	SourceROI.width = 512;
	SourceROI.height = 512;

	int SpecSize;
	int InitBufferSize;
	int BufferSize;
	ippiResizeGetSize_8u(SourceSize, DestSize, ippLinear, 0, &SpecSize, &InitBufferSize);

	IppiResizeSpec_32f* Spec = (IppiResizeSpec_32f*)ippsMalloc_8u(SpecSize);

	Ipp8u* InitBuffer = NULL;
	if (InitBufferSize != 0)
		InitBuffer = ippsMalloc_8u(InitBufferSize);
	ippiResizeLinearInit_8u(SourceSize, DestSize, Spec);

	ippiResizeGetBufferSize_8u(Spec, DestSize, 4, &BufferSize);
	Ipp8u* Buffer = ippsMalloc_8u(BufferSize);

	IppStatus Result = ippiResizeLinear_8u_C4R((const Ipp8u*)Data, Pitch, (Ipp8u*)BlockData, BlockPitch, Offset, DestSize, ippBorderRepl, 0, Spec, Buffer);

	ippsFree(Buffer);
	ippsFree(Spec);
	if (InitBuffer != NULL)
		ippsFree(InitBuffer);
}

bool ZETEPatch::Create(ZESize Width, ZESize Height, ZETEPixelType Type)
{
	Clean();

	PixelType = Type;
	this->Width = Width;
	this->Height = Height;
	Pitch = Width * GetPixelSize();
	Data = new ZEUInt8[Height * Pitch];

	UpdateLevelAndScaling();

	return true;
}

bool ZETEPatch::Load(const char* FileName, ZETEPixelType PixelType)
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
			FreeImage_ConvertToRawBits((BYTE*)Data, Bitmap, Pitch, 32, 0, 0, 0, true);
			FreeImage_Unload(Bitmap);
			break;
	}
	
	return true;
}

ZETEPatch::ZETEPatch()
{
	Data = NULL;
	Clean();

	Level = 0;
	Priority = 0;
	StartX = 0.0;
	StartY = 0.0;
	EndX = 0.0;
	EndY = 0.0;
	LevelScalingWidth = 1.0;
	LevelScalingHeight = 1.0;
}

ZETEPatch::~ZETEPatch()
{
	Clean();
}
