//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Triangle.cpp
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

#include "Triangle.h"
#include "Plane.h"
#include "Definitions.h"
#include "LineSegment.h"
#include "Line.h"
#include "Ray.h"
#include <math.h>

void ZETriangle::Create(ZETriangle& Triangle, const ZEVector3& V0, const ZEVector3& V1, const ZEVector3& V2)
{
	Triangle.V0 = V0;
	Triangle.V1 = V1;
	Triangle.V2 = V2;
}

void ZETriangle::GetNormal(const ZETriangle& Triangle, ZEVector3& Normal)
{
	ZEVector3 U, V;
	ZEVector3::Sub(U, Triangle.V1, Triangle.V0);
	ZEVector3::Sub(V, Triangle.V2, Triangle.V0);
	ZEVector3::CrossProduct(Normal, U, V);
	ZEVector3::Normalize(Normal, Normal);
}

void ZETriangle::GetBarrycentiricCoordinates(const ZETriangle& Triangle, const ZEPoint3& Point, ZEVector3& BarryCoords)
{
}

void ZETriangle::GetSurfacePlane(const ZETriangle& Triangle, ZEPlane& Plane)
{
	Plane.p = Triangle.V1;
	GetNormal(Triangle, Plane.n);
}

bool ZETriangle::InsideTest(const ZETriangle& Triangle, const ZEPoint3& Point)
{//
	ZEVector3 U, V, N, W;
	ZEVector3::Sub(U, Triangle.V1, Triangle.V0);
	ZEVector3::Sub(V, Triangle.V2, Triangle.V0);
	ZEVector3::Sub(W, Point, Triangle.V0);

    float  uu, uv, vv, wu, wv, D;   
	uu = ZEVector3::DotProduct(U,U);
    uv = ZEVector3::DotProduct(U,V);
    vv = ZEVector3::DotProduct(V,V);
    wu = ZEVector3::DotProduct(W,U);
    wv = ZEVector3::DotProduct(W,V);
    D = uv * uv - uu * vv;

    float s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0f || s > 1.0f)        
        return false;
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0f || (s + t) > 1.0f)
        return false;

    return true;
}

bool ZETriangle::IntersectionTest(const ZETriangle& Triangle, const ZELine& Line, float &t)
{
	ZEPlane Plane;
	ZEVector3 Point;

	GetSurfacePlane(Triangle, Plane);
	if (ZEPlane::IntersectionTest(Plane, Line, t))
	{
		Line.GetPointOn(Point, t);
		if (InsideTest(Triangle, Point))
			return true;
	}

	return false;
}

bool ZETriangle::IntersectionTest(const ZETriangle& Triangle, const ZELineSegment& LineSegment, float &t)
{
	ZEPlane Plane;
	ZEVector3 Point;

	GetSurfacePlane(Triangle, Plane);
	if (ZEPlane::IntersectionTest(Plane, LineSegment, t))
	{
		LineSegment.GetPointOn(Point, t);
		if (InsideTest(Triangle, Point))
			return true;
	}

	return false;
}

bool ZETriangle::IntersectionTest(const ZETriangle& Triangle, const ZERay& Ray, float &t)
{
	ZEPlane Plane;
	ZEVector3 Point;

	GetSurfacePlane(Triangle, Plane);
	if (ZEPlane::IntersectionTest(Plane, Ray, t))
	{
		Ray.GetPointOn(Point, t);
		if (InsideTest(Triangle, Point))
			return true;
	}

	return false;
}

bool ZETriangle::IntersectionTest(const ZETriangle& Triangle, const ZEPlane & Plane2, ZELine & Line)
{
	return false;
}

ZETriangle::ZETriangle()
{
}

ZETriangle::ZETriangle(const ZEVector3& V0, const ZEVector3& V1, const ZEVector3& V2)
{
	Create(*this, V0, V1, V2);
}
