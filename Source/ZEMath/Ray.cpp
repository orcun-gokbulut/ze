//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Ray.cpp
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

#include "Definitions.h"
#include "Ray.h"
#include "Vector.h"
#include "Plane.h"
#include <d3dx9.h>

void ZERay::Create(ZERay & Ray, const ZEVector3 & Start,const ZEVector3 & End)
{
	ZELine::Create(Ray,Start,End);
}

void ZERay::CreateParametric(ZERay & Ray,const ZEVector3 & v,const ZEVector3 & p)
{
	Ray.v = v;
	Ray.p = p;
}

float ZERay::MinimumDistance(const ZERay& Ray, const ZEVector3& Point)
{
	float TRay;

	return ZERay::MinimumDistance(Ray, Point, TRay);
}

float ZERay::MinimumDistance(const ZERay& Ray, const ZEVector3& Point, float& TRay)
{
	ZEVector3 Temp;
	ZEVector3::Sub(Temp, Point, Ray.p);

	TRay = ZEVector3::DotProduct(Temp, Ray.v);
	if (TRay < 0)
	{
		TRay = 0;
		return ZEVector3::Distance(Point, Ray.p);
	}

	ZEVector3::Scale(Temp, Ray.v, TRay);
	ZEVector3::Add(Temp, Ray.p, Temp);
	return ZEVector3::Distance(Temp, Point);
}

float ZERay::MinimumDistance(const ZERay& Ray, const ZELine& Line)
{
	return ZELine::MinimumDistance(Line, Ray);
}

float ZERay::MinimumDistance(const ZERay& Ray, const ZELine& Line, float& TRay, float& TLine)
{
	return ZELine::MinimumDistance(Line, Ray, TLine, TRay);
}

float ZERay::MinimumDistance(const ZERay& Ray, const ZELineSegment& LineSegment)
{
	float TRay, TLineSegment;

	return ZERay::MinimumDistance(Ray, LineSegment, TRay, TLineSegment);
}

float ZERay::MinimumDistance(const ZERay& Ray, const ZELineSegment& LineSegment, float& TRay, float& TRaySegment)
{
	// NOT IMPLAMENTED !!!!
	return 0;
}

float ZERay::MinimumDistance(const ZERay& RayA, const ZERay& RayB)
{
	float TRayA, TRayB;
	return ZERay::MinimumDistance(RayA, RayB, TRayA, TRayB);
}

float ZERay::MinimumDistance(const ZERay& RayA, const ZERay& RayB, float& TRayA, float& TRayB)
{
	// NOT IMPLAMENTED !!!!
	return 0;
}

void ZERay::GetPointOn(ZEVector3& Point, float TRay) const
{
	if (TRay < 0.0f)
		Point = p;
	else
	{
		ZEVector3::Scale(Point, v, TRay);
		ZEVector3::Add(Point, Point, p);
	}
}

ZEVector3 ZERay::GetPointOn(float TRay) const
{
	ZEVector3 Temp;

	if (TRay < 0.0f)
		Temp = p;
	else
	{
		ZEVector3::Scale(Temp, v, TRay);
		ZEVector3::Add(Temp, Temp, p);
	}

	return Temp;
}

ZERay::ZERay(const ZEVector3 & v,const ZEVector3 &p)
{
	this->v = v;
	this->p = p;
}

ZERay::ZERay() 
{
}

