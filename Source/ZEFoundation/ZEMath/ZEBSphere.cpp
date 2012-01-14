//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEBSphere.cpp
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

#include "ZEBSphere.h"
#include "ZEAABBox.h"
#include "ZEOBBox.h"
#include "ZERay.h"
#include "ZELineSegment.h"
#include "ZELine.h"

#include <math.h>

static inline void SWAP(float a, float b)
{
	float t;
	t = a;
	a = b;
	b = t;
}

void ZEBSphere::GetSurfaceNormal(ZEVector3& Normal, const ZEBSphere& BoundingSphere, const ZEVector3& Point)
{
	ZEVector3::Create(Normal, BoundingSphere.Position, Point);
	ZEVector3::Normalize(Normal, Normal);
}

ZEHalfSpace ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZEPlane& Plane)
{
	if (ZEPlane::Distance(Plane, BoundingSphere.Position) <= BoundingSphere.Radius) 
		return ZE_HS_INTERSECTS;

	ZEVector3 temp;
	ZEVector3::Sub(temp,BoundingSphere.Position,Plane.p);
	return ((ZEVector3::DotProduct(temp,Plane.n) < 0) ? ZE_HS_NEGATIVE_SIDE : ZE_HS_POSITIVE_SIDE);
}

bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZEVector3& Point)
{
	return ZEVector3::DistanceSquare(BoundingSphere.Position, Point) <= BoundingSphere.Radius * BoundingSphere.Radius;
}

bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZELine& Line)
{
	ZEVector3 a = Line.p - BoundingSphere.Position;
	float b = ZEVector3::DotProduct(Line.v, a);
	float c = ZEVector3::DotProduct(a, a) - BoundingSphere.Radius * BoundingSphere.Radius;
	float d = b * b - c;
	
	return (d > 0);
}

bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZELine& Line, float& MinT)
{
	ZEVector3 a = Line.p - BoundingSphere.Position;
	float b = ZEVector3::DotProduct(Line.v, a);
	float c = ZEVector3::DotProduct(a, a) - BoundingSphere.Radius * BoundingSphere.Radius;
	float d = b * b - c;
	
	if (d > 0)
	{
		float sqrtd = sqrtf(d);

		float MaxT = -b + sqrtd;
		MinT = -b - sqrtd;
		
		if (MinT > MaxT) 
			MinT = MaxT;

		return true;
	}

	return false;
}


bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZELine& Line, float& MinT, float& MaxT)
{
	ZEVector3 a = Line.p - BoundingSphere.Position;
	float b = ZEVector3::DotProduct(Line.v, a);
	float c = ZEVector3::DotProduct(a, a) - BoundingSphere.Radius * BoundingSphere.Radius;
	float d = b * b - c;

	if (d > 0)
	{
		float sqrtd = sqrtf(d);

		MinT = -b - sqrtd;
		MaxT = -b + sqrtd;

		if (MinT > MaxT) 
			SWAP(MinT, MaxT);

		return true;
	}

	return false;
}

bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZERay& Ray)
{
	ZEVector3 l =  BoundingSphere.Position - Ray.p;
	float s = ZEVector3::DotProduct(l, Ray.v);
	float ll = ZEVector3::DotProduct(l, l);
	float rr = BoundingSphere.Radius * BoundingSphere.Radius;

	if (s < 0 && ll > rr)
		return false;

	float mm = ll - s * s;
	if (mm > rr)
		return false;

	return true;
}

bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZERay& Ray, float& MinT)
{
	ZEVector3 l =  BoundingSphere.Position - Ray.p;
	float s = ZEVector3::DotProduct(l, Ray.v);
	float ll = ZEVector3::DotProduct(l, l);
	float rr = BoundingSphere.Radius * BoundingSphere.Radius;

	if (s < 0 && ll > rr)
		return false;

	float mm = ll - s * s;
	if (mm > rr)
		return false;

	float q = sqrt(rr - mm);
	if (ll > rr)
		MinT = s - q;
	else
		MinT = s + q;

	return true;
}

bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZERay& Ray, float& MinT, float& MaxT)
{
	ZEVector3 l =  BoundingSphere.Position - Ray.p;
	float s = ZEVector3::DotProduct(l, Ray.v);
	float ll = ZEVector3::DotProduct(l, l);
	float rr = BoundingSphere.Radius * BoundingSphere.Radius;

	if (s < 0 && ll > rr)
		return false;

	float mm = ll - s * s;
	if (mm > rr)
		return false;

	float q = sqrt(rr - mm);
	if (ll > rr)
	{
		MinT = s - q;
		MaxT = s + q;
	}
	else
	{
		MinT = s + q;
		MaxT = s - q;
	}

	if(MaxT < 0.0f)
		MaxT = MinT;

	return true;
}

bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZELineSegment& LineSegment)
{
	float Mint, Maxt;
	return IntersectionTest(BoundingSphere, LineSegment, Mint, Maxt);	 
}

bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZELineSegment& LineSegment, float& MinT)
{
	float Mint, Maxt;
	bool Result = IntersectionTest(BoundingSphere, LineSegment, Mint, Maxt);
	MinT = Mint;
	return Result;
}

bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZELineSegment& LineSegment, float& MinT, float& MaxT)
{
	float Min, Max;
	bool Result = IntersectionTest(BoundingSphere, (ZELine)LineSegment, Min, Max);
	if (!Result)
		return false;

	if (Max > 0.0f && Min < 0.0f)
		Min = Max;

	if(Max < 0.0f)
		return false;

	if (Min > LineSegment.Length)
		return false;

	if(Max > LineSegment.Length)
		Max = Min;

	MinT = Min;
	MaxT = Max;

	return true;
}

bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere1, const ZEBSphere& BoundingSphere2)
{
	ZEVector3 Displacement = BoundingSphere1.Position - BoundingSphere2.Position;
	float DistanceSquare = ZEVector3::DotProduct(Displacement, Displacement);
	float r = (BoundingSphere1.Radius + BoundingSphere2.Radius);
	
	if (DistanceSquare > r * r)
		return false;

	return true;
}

bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZEAABBox& BoundingBox)
{
	ZEVector3 e = (BoundingBox.Min - BoundingSphere.Position).ClampLower(0.0f) + 
		(BoundingSphere.Position - BoundingBox.Max).ClampLower(0.0f);
	float d = ZEVector3::DotProduct(e, e);
	if (d > BoundingSphere.Radius * BoundingSphere.Radius)
		return false;

	return true;
}

bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZEOBBox& BoundingBox)
{
	ZEVector3 LocalPosition = BoundingSphere.Position - BoundingBox.Center;
	
	ZEVector3 NewPoint;
	NewPoint.x = ZEVector3::DotProduct(LocalPosition, BoundingBox.Right);
	NewPoint.y = ZEVector3::DotProduct(LocalPosition, BoundingBox.Up);
	NewPoint.z = ZEVector3::DotProduct(LocalPosition, BoundingBox.Front);

	//NewPoint += Translation;

	ZEVector3 e = (-BoundingBox.HalfSize - NewPoint).ClampLower(0.0f) + 
		(NewPoint - BoundingBox.HalfSize).ClampLower(0.0f);
	float d = ZEVector3::DotProduct(e, e);
	if (d > BoundingSphere.Radius * BoundingSphere.Radius)
		return false;

	return true;
}

ZEBSphere::ZEBSphere()
{

}

ZEBSphere::ZEBSphere(const ZEVector3& Position, float Radius)
{
	ZEBSphere::Position = Position;
	ZEBSphere::Radius = Radius;
}




