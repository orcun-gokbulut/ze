//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETriangle.cpp
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

#include "ZETriangle.h"
#include "ZEPlane.h"
#include "ZEMath.h"
#include "ZELineSegment.h"
#include "ZELine.h"
#include "ZERay.h"
#include "ZEMath/ZEMath.h"

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

void ZETriangle::GetBarycentricCoordinates(const ZETriangle& Triangle, const ZEVector3& Point, ZEVector3& BaryCoords)
{
	float Area = ZETriangle::GetArea(Triangle);

	ZETriangle Triangle1(Triangle.V0, Triangle.V1, Point);
	ZETriangle Triangle2(Triangle.V1, Triangle.V2, Point);
	ZETriangle Triangle3(Triangle.V2, Triangle.V0, Point);

	float Area1 = ZETriangle::GetArea(Triangle1);
	float Area2 = ZETriangle::GetArea(Triangle2);
	float Area3 = ZETriangle::GetArea(Triangle3);

	BaryCoords = ZEVector3(Area1 / Area, Area2 / Area, Area3 / Area);
}

void ZETriangle::GetCentroid(const ZETriangle& Triangle, ZEVector3& Centroid)
{
	Centroid.x = (Triangle.V0.x + Triangle.V1.x + Triangle.V2.x) / 3.0f;
	Centroid.y = (Triangle.V0.y + Triangle.V1.y + Triangle.V2.y) / 3.0f;
	Centroid.z = (Triangle.V0.z + Triangle.V1.z + Triangle.V2.z) / 3.0f;
}

void ZETriangle::GetSurfacePlane(const ZETriangle& Triangle, ZEPlane& Plane)
{
	Plane.p = Triangle.V1;
	GetNormal(Triangle, Plane.n);
}

float ZETriangle::GetArea(const ZETriangle& Triangle)
{
	ZEVector3 Temp;
	ZEVector3 V0V1(Triangle.V1 - Triangle.V0);
	ZEVector3 V0V2(Triangle.V2 - Triangle.V0);
	ZEVector3::CrossProduct(Temp, V0V1, V0V2);

	return ZEVector3::Length(Temp) / 2.0f;
}

bool ZETriangle::InsideTest(const ZETriangle& Triangle, const ZEVector3& Point)
{
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
		ZELine::GetPointOn(Point, Line, t);
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
		ZELineSegment::GetPointOn(Point, LineSegment, t);
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
		ZERay::GetPointOn(Point, Ray, t);
		if (InsideTest(Triangle, Point))
			return true;
	}

	return false;
}

bool ZETriangle::IntersectionTest(const ZETriangle& Triangle, const ZEPlane & Plane2, ZELine & Line)
{
	return false;
}

ZEVector3 ZETriangle::GetCentroid() const
{
	ZEVector3 Temp;
	Temp.x = (V0.x + V1.x + V2.x) / 3.0f;
	Temp.y = (V0.y + V1.y + V2.y) / 3.0f;
	Temp.z = (V0.z + V1.z + V2.z) / 3.0f;
	return Temp;
}

ZETriangle::ZETriangle()
{
	
}

ZETriangle::ZETriangle(const ZEVector3& V0, const ZEVector3& V1, const ZEVector3& V2)
{
	Create(*this, V0, V1, V2);
}
