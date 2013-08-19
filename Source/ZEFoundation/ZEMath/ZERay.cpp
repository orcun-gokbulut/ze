//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERay.cpp
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

#include "ZEMath.h"
#include "ZERay.h"
#include "ZELineSegment.h"
#include "ZEVector.h"
#include "ZEPlane.h"

#include "ZEMath/ZEMath.h"

void ZERay::Create(ZERay & Ray, const ZEVector3 & P0,const ZEVector3 & P1)
{
	ZELine::Create(Ray, P0, P1);
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
	float TRay, TLine;
	return ZERay::MinimumDistance(Ray, Line, TRay, TLine);
}

float ZERay::MinimumDistance(const ZERay& Ray, const ZELine& Line, float& TRay, float& TLine)
{
	ZEVector3 w(Ray.p, Line.p);
	float    a = ZEVector3::DotProduct(Ray.v, Ray.v);
    float    b = ZEVector3::DotProduct(Ray.v, Line.v);
    float    c = ZEVector3::DotProduct(Line.v, Line.v);
    float    d = ZEVector3::DotProduct(Ray.v, w);
    float    e = ZEVector3::DotProduct(Line.v, w);
    float    D = a*c - b*b;
    float    sN, sD = D;
    float    tN, tD = D;

	if (D < ZE_ZERO_THRESHOLD) 
	{
        sN = 0.0; 
        sD = 1.0;      
        tN = e;
        tD = c;
    }
    else 
	{
        sN = -(b*e - c*d);
        tN = -(a*e - b*d);
        if (sN < 0.0) 
		{
            sN = 0.0;
            tN = e;
            tD = c;
        }
    }

    TRay = (ZEMath::Abs(sN) < ZE_ZERO_THRESHOLD ? 0.0f : sN / sD);
    TLine = (ZEMath::Abs(tN) < ZE_ZERO_THRESHOLD ? 0.0f : tN / tD);

	return ZEVector3::Length(ZEVector3(Ray.GetPointOn(TRay), Line.GetPointOn(TLine)));
}

float ZERay::MinimumDistance(const ZERay& Ray, const ZELineSegment& LineSegment)
{
	return ZELineSegment::MinimumDistance(LineSegment, Ray);
}

float ZERay::MinimumDistance(const ZERay& Ray, const ZELineSegment& LineSegment, float& TRay, float& TLineSegment)
{
	return ZELineSegment::MinimumDistance(LineSegment, Ray, TLineSegment, TRay);
}

float ZERay::MinimumDistance(const ZERay& RayA, const ZERay& RayB)
{
	float TRayA, TRayB;
	return ZERay::MinimumDistance(RayA, RayB, TRayA, TRayB);
}

float ZERay::MinimumDistance(const ZERay& RayA, const ZERay& RayB, float& TRayA, float& TRayB)
{
	ZEVector3 w(RayA.p, RayB.p);
	float    a = ZEVector3::DotProduct(RayA.v, RayA.v);
    float    b = ZEVector3::DotProduct(RayA.v, RayB.v);
    float    c = ZEVector3::DotProduct(RayB.v, RayB.v);
    float    d = ZEVector3::DotProduct(RayA.v, w);
    float    e = ZEVector3::DotProduct(RayB.v, w);
    float    D = a*c - b*b;
    float    sN, sD = D;
    float    tN, tD = D;

	if (D < ZE_ZERO_THRESHOLD) 
	{
        sN = 0.0; 
        sD = 1.0;      
        tN = e;
        tD = c;
    }
    else 
	{
        sN = (b*e - c*d);
        tN = (a*e - b*d);
        if (sN < 0.0) 
		{
            sN = 0.0;
            tN = e;
            tD = c;
        }
    }

    if (tN < 0.0) 
	{
        tN = 0.0;
        if (-d < 0.0)
            sN = 0.0;
        else if (-d > a)
            sN = sD;
        else
		{
            sN = -d;
            sD = a;
        }
    }

    TRayA = (ZEMath::Abs(sN) < ZE_ZERO_THRESHOLD ? 0.0f : sN / sD);
    TRayB = (ZEMath::Abs(tN) < ZE_ZERO_THRESHOLD ? 0.0f : tN / tD);

	ZEVector3 P1, P2;
	ZEVector3::Scale(P1, RayA.v, TRayA);
	ZEVector3::Add(P1, P1, w);
	ZEVector3::Scale(P2, RayB.v, TRayB);
	ZEVector3::Sub(w, P1, P2);
	return ZEVector3::Length(w);
}

void ZERay::GetPointOn(ZEVector3& Point, const ZERay& Ray, float TRay)
{
	if (TRay < 0.0f)
		Point = Ray.p;
	else
	{
		ZEVector3::Scale(Point, Ray.v, TRay);
		ZEVector3::Add(Point, Point, Ray.p);
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


const ZEVector3& ZERay::GetStartPoint() const
{
	return p;
}


ZERay::ZERay(const ZEVector3 & Direction,const ZEVector3 &Position)
{
	this->v = Direction.Normalize();
	this->p = Position;
}

ZERay::ZERay() 
{
}
