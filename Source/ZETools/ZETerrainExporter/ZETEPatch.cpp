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
#include "ZETEPatchDatabase.h"

#include "ZEError.h"
#include "ZEMath\ZEMath.h"

#include <FreeImage.h>

void ZETEPatch::UpdateLevelAndScaling()
{
	if (Width == 0 || Height == 0 || EndX - StartX <= 0 || EndY - StartY <= 0)
	{
		Level = 0;
		LevelScalingX = 1.0;
		LevelScalingY = 1.0;
		PixelScaleX = 0.0;
		PixelScaleY = 0.0;
	}
	else
	{
		PixelScaleX = (EndX - StartX) / (double)Width;
		PixelScaleY = (EndY - StartY) / (double)Height;

		double MinScale = (PixelScaleX < PixelScaleY ? PixelScaleX : PixelScaleY);

		double LevelTemp = log(MinScale) / log(2);
		Level = (ZEInt)floor(LevelTemp + 0.5);

		double LevelScale = pow(2, Level);
		LevelScalingX = PixelScaleX / LevelScale;
		LevelScalingY = PixelScaleY / LevelScale;
	}

	if (Database != NULL)
		Database->CalculateDimensions();
}

void ZETEPatch::SetPositionX(double x)
{
	if (StartX == x)
		return;

	StartX = x;
	UpdateLevelAndScaling();
}

double ZETEPatch::GetPositionX()
{
	return StartX;
}

void ZETEPatch::SetPositionY(double y)
{
	if (StartY == y)
		return;

	StartY = y;
	UpdateLevelAndScaling();
}

double ZETEPatch::GetPositionY()
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

double ZETEPatch::GetPixelScaleX()
{
	return PixelScaleX;
}

double ZETEPatch::GetPixelScaleY()
{
	return PixelScaleY;
}

double ZETEPatch::GetLevelScaleX()
{
	return LevelScalingX;
}

double ZETEPatch::GetLevelScaleY()
{
	return LevelScalingY;
}

ZESize ZETEPatch::GetWidth()
{
	return Width;
}

ZESize ZETEPatch::GetHeight()
{
	return Height;
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
		case ZETE_PT_NONE:
			return 0;

		case ZETE_PT_ELEVATION:
			return 2;

		case ZETE_PT_COLOR:
			return 4;

		case ZETE_PT_GRAYSCALE:
			return 1;
	}
}

ZEInt ZETEPatch::GetLevel()
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

void ZETEPatch::SetSource(const ZEString& Source)
{
	this->Source = Source;
}

const ZEString& ZETEPatch::GetSource()
{
	return Source;
}

bool ZETEPatch::Intersect(double Px, double Py, double Width, double Height)
{
	return (StartX <= Px + Width && Px <= EndX &&
			StartY <= Py + Width && Py <= EndY);
}

bool ZETEPatch::IsLoaded()
{
	return Loaded;
}

bool ZETEPatch::Load()
{
	Loaded = true;
	if (Database != NULL)
		Database->CalculateDimensions();

	return true;
}

void ZETEPatch::Unload()
{
	Loaded = false;
}

ZETEPatch::ZETEPatch()
{
	Database = NULL;
	Level = 0;
	Priority = 0;
	StartX = 0.0;
	StartY = 0.0;
	EndX = 0.0;
	EndY = 0.0;
	LevelScalingX = 1.0;
	LevelScalingY = 1.0;
}

ZETEPatch::~ZETEPatch()
{
}
