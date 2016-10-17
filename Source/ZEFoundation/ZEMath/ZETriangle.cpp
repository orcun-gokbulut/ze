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


ZEVector3 ZETriangle::GetNormal() const
{
	ZEVector3 Normal;
	ZETriangle::GetNormal(*this, Normal);
	return Normal;
}

ZEVector3 ZETriangle::GetTangent() const
{
	ZEVector3 Tangent;
	ZETriangle::GetTangent(*this, Tangent);
	return Tangent;
}

ZEVector3 ZETriangle::GetBinormal() const
{
	ZEVector3 Binormal;
	ZETriangle::GetBinormal(*this, Binormal);
	return Binormal;
}

float ZETriangle::GetArea() const
{
	return ZETriangle::GetArea(*this);
}

ZEPlane ZETriangle::GetSurfacePlane() const
{
	ZEPlane Plane;
	ZETriangle::GetSurfacePlane(*this, Plane);
	return Plane;
}

ZEVector3 ZETriangle::GetCentroid() const
{
	ZEVector3 Centroid;
	ZETriangle::GetCentroid(*this, Centroid);
	return Centroid;
}

ZEVector3 ZETriangle::GetBarycentricCoordinates(const ZEVector3& Point) const
{
	ZEVector3 BaryCoords;
	ZETriangle::GetBarycentricCoordinates(*this, Point, BaryCoords);
	return BaryCoords;
}

ZEAABBox ZETriangle::GetBoundingBox() const
{
	ZEAABBox BoundingBox;
	ZETriangle::GetBoundingBox(*this, BoundingBox);
	return BoundingBox;
}


ZETriangle::ZETriangle()
{

}

ZETriangle::ZETriangle(const ZEVector3& V0, const ZEVector3& V1, const ZEVector3& V2)
{
	Create(*this, V0, V1, V2);
}

void ZETriangle::Create(ZETriangle& Triangle, const ZEVector3& V0, const ZEVector3& V1, const ZEVector3& V2)
{
	Triangle.V0 = V0;
	Triangle.V1 = V1;
	Triangle.V2 = V2;
}

void ZETriangle::GetNormal(const ZETriangle& Triangle, ZEVector3& Normal)
{
	ZEVector3 Tangent, Binormal;
	GetTangent(Triangle, Tangent);
	GetBinormal(Triangle, Binormal);
	ZEVector3::CrossProduct(Normal, Tangent, Binormal);
	ZEVector3::Normalize(Normal, Normal);
}

void ZETriangle::GetTangent(const ZETriangle& Triangle, ZEVector3& Tangent)
{
	ZEVector3::Sub(Tangent, Triangle.V1, Triangle.V0);
	ZEVector3::Normalize(Tangent, Tangent);
}

void ZETriangle::GetBinormal(const ZETriangle& Triangle, ZEVector3& Binormal)
{
	ZEVector3::Sub(Binormal, Triangle.V2, Triangle.V0);
	ZEVector3::Normalize(Binormal, Binormal);
}

float ZETriangle::GetArea(const ZETriangle& Triangle)
{
	ZEVector3 Temp;
	ZEVector3 V0V1(Triangle.V1 - Triangle.V0);
	ZEVector3 V0V2(Triangle.V2 - Triangle.V0);
	ZEVector3::CrossProduct(Temp, V0V1, V0V2);

	return ZEVector3::Length(Temp) / 2.0f;
}

void ZETriangle::GetSurfacePlane(const ZETriangle& Triangle, ZEPlane& Plane)
{
	Plane.p = Triangle.V1;
	GetNormal(Triangle, Plane.n);
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


void ZETriangle::GetBoundingBox(const ZETriangle& Triangle, ZEAABBox& BoundingBox)
{
	ZEVector3::Max(BoundingBox.Max, Triangle.V0, Triangle.V1);
	ZEVector3::Max(BoundingBox.Max, BoundingBox.Max, Triangle.V2);
	ZEVector3::Min(BoundingBox.Min, Triangle.V0, Triangle.V1);
	ZEVector3::Min(BoundingBox.Min, BoundingBox.Max, Triangle.V2);
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

bool ZETriangle::GenerateVertexTangentBinormal(const ZETriangle& PositionTriangle, const ZETriangle& TexcoordsTriangle, ZEVector3& Tangent, ZEVector3& Binormal)
{
	const ZEVector3& v1 = PositionTriangle.V0;
	const ZEVector3& v2 = PositionTriangle.V1;
	const ZEVector3& v3 = PositionTriangle.V2;

	const ZEVector3& w1 = TexcoordsTriangle.V0;
	const ZEVector3& w2 = TexcoordsTriangle.V1;
	const ZEVector3& w3 = TexcoordsTriangle.V2;

	float x1 = v2.x - v1.x;
	float x2 = v3.x - v1.x;
	float y1 = v2.y - v1.y;
	float y2 = v3.y - v1.y;
	float z1 = v2.z - v1.z;
	float z2 = v3.z - v1.z;
	float s1 = w2.x - w1.x;
	float s2 = w3.x - w1.x;
	float t1 = w2.y - w1.y;
	float t2 = w3.y - w1.y;

	float rt = (s1 * t2 - s2 * t1);

	if (rt < 0.000000001f)
		return false;

	float r = 1.0f / rt;

	Tangent = ZEVector3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,	(t2 * z1 - t1 * z2) * r);
	Binormal = ZEVector3((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,	(s1 * z2 - s2 * z1) * r);

	if (!Tangent.IsValidNotInf() || !Binormal.IsValidNotInf())
		return false;

	Tangent.NormalizeSelf();
	Binormal.NormalizeSelf();

	return true;
}

bool ZETriangle::OrtogonolizeTangentBinormal(const ZEVector3& Normal, ZEVector3& Tangent, ZEVector3& Binormal)
{
	Tangent  = (Tangent  - Normal * ZEVector3::DotProduct(Normal, Tangent)).Normalize();
	Binormal = (Binormal - Normal * ZEVector3::DotProduct(Normal, Binormal)).Normalize();

	if (!Tangent.IsValidNotInf() || !Binormal.IsValidNotInf())
		return false;

	return true;
}
