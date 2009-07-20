//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - BoundingSphere.cpp
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

#include "BoundingSphere.h"
#include "AABoundingBox.h"
#include "OBoundingBox.h"
#include "Ray.h"
#include "LineSegment.h"
#include "Line.h"

#include <math.h>

ZEHalfSpace ZEBoundingSphere::PlaneHalfSpaceTest(const ZEBoundingSphere& BoundingSphere, const ZEPlane& Plane)
{
	if (ZEPlane::Distance(Plane, BoundingSphere.Position) <= BoundingSphere.Radius) return ZEHALFSPACE_INTERSECTS;

	ZEVector3 temp;
	ZEVector3::Sub(temp,BoundingSphere.Position,Plane.p);
	return ((ZEVector3::DotProduct(temp,Plane.n) < 0) ? ZEHALFSPACE_NEGATIVESIDE : ZEHALFSPACE_POSITIVESIDE);
}


void ZEBoundingSphere::GetSurfaceNormal(ZEVector3& Normal, const ZEBoundingSphere& BoundingSphere, const ZEPoint3& Point)
{
	ZEVector3::Create(Normal, BoundingSphere.Position, Point);
	ZEVector3::Normalize(Normal, Normal);
}

bool ZEBoundingSphere::IntersectionTest(const ZEBoundingSphere& BoundingSphere, const ZEPoint3 Point)
{
	return ZEPoint3::Distance(BoundingSphere.Position, Point) <= BoundingSphere.Radius;
}

bool ZEBoundingSphere::IntersectionTest(const ZEBoundingSphere& BoundingSphere, const ZELine& Line)
{
	float temp;
	return ZELine::DistanceToPoint(Line, BoundingSphere.Position, temp) <= BoundingSphere.Radius;
}

bool ZEBoundingSphere::IntersectionTest(const ZEBoundingSphere& BoundingSphere, const ZERay& Ray)
{
	float temp;
	return ZERay::DistanceToPoint(Ray, BoundingSphere.Position, temp) <= BoundingSphere.Radius;
}

bool ZEBoundingSphere::IntersectionTest(const ZEBoundingSphere& BoundingSphere, const ZERay& Ray, float& MinT, float& MaxT)
{
	ZEVector3 Dest;
	ZEVector3::Sub(Dest, Ray.p, BoundingSphere.Position);
	float B = ZEVector3::DotProduct(Dest, Ray.v);
	float C = ZEVector3::DotProduct(Dest, Dest) - BoundingSphere.Radius*BoundingSphere.Radius;
	float D = B*B - C;
	if (D > 0)
	{
		MaxT = MinT = -B - sqrtf(D);
		return true;
	}
	
	return false;

	/*ZEVector3 L;

	ZEVector3::Sub(L, BoundingSphere.Position, Ray.p);

	float tca =	ZEVector3::DotProduct(L, Ray.v);
	if (tca < 0)
		return false;

	float LdotL =	ZEVector3::DotProduct(L, L);
	float dd = LdotL - tca * tca;
	float rr = BoundingSphere.Radius * BoundingSphere.Radius;
	if (dd > rr)
		return false;

	float thc = sqrtf(rr - dd);
	MinT = tca  - thc;
	MaxT = tca + thc;

	return true;*/
}

bool ZEBoundingSphere::IntersectionTest(const ZEBoundingSphere& BoundingSphere, const ZELineSegment& LineSegment)
{
	float temp;
 	return ZELineSegment::DistanceToPoint(LineSegment, BoundingSphere.Position, temp) <= BoundingSphere.Radius;
}

bool ZEBoundingSphere::CollisionTest(const ZEBoundingSphere& BoundingSphere, const ZEOBoundingBox& BoundingBox)
{
	return 0;
}


bool ZEBoundingSphere::CollisionTest(const ZEBoundingSphere& BoundingSphere, const ZEAABoundingBox& BoundingBox)
{
	float totalDistance = 0;

	if (BoundingSphere.Position.x > BoundingBox.Max.x)
		totalDistance += (BoundingSphere.Position.x - BoundingBox.Max.x) * (BoundingSphere.Position.x - BoundingBox.Max.x);
	else if (BoundingSphere.Position.x < BoundingBox.Min.x)
		totalDistance += (BoundingSphere.Position.x - BoundingBox.Min.x) * (BoundingSphere.Position.x - BoundingBox.Min.x);

	if (BoundingSphere.Position.y > BoundingBox.Max.y)
		totalDistance += (BoundingSphere.Position.y - BoundingBox.Max.y) * (BoundingSphere.Position.y - BoundingBox.Max.y);
	else if (BoundingSphere.Position.y < BoundingBox.Min.y)
		totalDistance += (BoundingSphere.Position.y - BoundingBox.Min.y) * (BoundingSphere.Position.y - BoundingBox.Min.y);

	if (BoundingSphere.Position.z > BoundingBox.Max.z)
		totalDistance += (BoundingSphere.Position.z - BoundingBox.Max.z) * (BoundingSphere.Position.z - BoundingBox.Max.z);
	else if (BoundingSphere.Position.z < BoundingBox.Min.z)
		totalDistance += (BoundingSphere.Position.z - BoundingBox.Min.z) * (BoundingSphere.Position.z - BoundingBox.Min.z);

	return sqrt(totalDistance) <= BoundingSphere.Radius;
}

bool ZEBoundingSphere::CollisionTest(const ZEBoundingSphere& BoundingSphere1, const ZEBoundingSphere& BoundingSphere2)
{
	return ZEVector3::Distance(BoundingSphere1.Position, BoundingSphere2.Position) <= BoundingSphere1.Radius + BoundingSphere2.Radius;
}


ZEBoundingSphere::ZEBoundingSphere()
{
}

ZEBoundingSphere::ZEBoundingSphere(const ZEPoint3& Position, float Radius)
{
	ZEBoundingSphere::Position = Position;
	ZEBoundingSphere::Radius = Radius;
}
