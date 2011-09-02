//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEOBBox.cpp
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
#include "ZEQuaternion.h"
#include "ZELine.h"
#include "ZERay.h"
#include "ZELineSegment.h"
#include "ZEMathDefinitions.h"

#include <float.h>
#include <math.h>

ZEVector3 ZEOBBox::GetVertex(unsigned char Index) const
{
	switch(Index)
	{
		case 0:
			return Center + Up * HalfSize.x;
		case 1:
			return Center - Up * HalfSize.x ;
		case 2:
			return Center + Right * HalfSize.y;
		case 3:
			return Center - Right * HalfSize.y;
		case 4:
			return Center + Front * HalfSize.z;
		case 5:
			return Center + Front * HalfSize.z;
	}
}

ZEVector3 ZEOBBox::GetEdge(unsigned char Index) const
{
	switch(Index)
	{
		case 0:
			return Center - Up * HalfSize.x - Right * HalfSize.y - Front * HalfSize.z;
		case 1:
			return Center + Up * HalfSize.x - Right * HalfSize.y - Front * HalfSize.z;
		case 2:
			return Center - Up * HalfSize.x + Right * HalfSize.y - Front * HalfSize.z;
		case 3:
			return Center + Up * HalfSize.x + Right * HalfSize.y - Front * HalfSize.z;
		case 4:
			return Center - Up * HalfSize.x - Right * HalfSize.y + Front * HalfSize.z;
		case 5:
			return Center + Up * HalfSize.x - Right * HalfSize.y + Front * HalfSize.z;
		case 6:
			return Center - Up * HalfSize.x + Right * HalfSize.y + Front * HalfSize.z;
		case 7:
			return Center + Up * HalfSize.x + Right * HalfSize.y + Front * HalfSize.z;
	}
}

void ZEOBBox::CreateFromOrientation(ZEOBBox& BoundingBox, const ZEVector3& Position, const ZEQuaternion& Rotation, const ZEVector3& Scale)
{
	BoundingBox.Center = Position;
	ZEQuaternion::VectorProduct(BoundingBox.Right, Rotation, ZEVector3::UnitX);
	ZEQuaternion::VectorProduct(BoundingBox.Up, Rotation, ZEVector3::UnitY);
	ZEQuaternion::VectorProduct(BoundingBox.Front, Rotation, ZEVector3::UnitZ);
	BoundingBox.HalfSize = Scale;
}

void ZEOBBox::Transform(ZEOBBox& Output, const ZEMatrix4x4& Matrix, const ZEOBBox& Input)
{
	ZEVector3 NewCenter, NewU, NewV, NewN;
	ZEMatrix4x4::Transform(NewCenter, Matrix, Input.Center);
	ZEMatrix4x4::Transform3x3(NewU, Matrix, Input.Right);
	ZEMatrix4x4::Transform3x3(NewU, Matrix, Input.Up);
	ZEMatrix4x4::Transform3x3(NewU, Matrix, Input.Front);
}

void ZEOBBox::ConvertToSphere(ZEBSphere& Sphere, const ZEOBBox& Input)
{
	Sphere.Position = Input.Center;
	Sphere.Radius = Input.HalfSize.Max();
}

ZEHalfSpace ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZEPlane& Plane)
{
	float Extent = 
		BoundingBox.HalfSize.x * fabs(ZEVector3::DotProduct(Plane.n, BoundingBox.Right)) +
		BoundingBox.HalfSize.y * fabs(ZEVector3::DotProduct(Plane.n, BoundingBox.Up)) +
		BoundingBox.HalfSize.z * fabs(ZEVector3::DotProduct(Plane.n, BoundingBox.Front));
	float Distance = ZEVector3::DotProduct(Plane.p - BoundingBox.Center, Plane.n);

	if (Distance - Extent > 0)
		return ZE_HS_POSITIVE_SIDE;
	else if (Distance + Extent < 0)
		return ZE_HS_NEGATIVE_SIDE;
	else
		ZE_HS_INTERSECTS;
}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZEVector3 Point)
{
	float Factor;

	ZEVector3 Position = BoundingBox.Center - 
		BoundingBox.Right * BoundingBox.HalfSize.x - 
		BoundingBox.Up * BoundingBox.HalfSize.y - 
		BoundingBox.Front * BoundingBox.HalfSize.z;

	ZEVector3 Right = BoundingBox.Right * (BoundingBox.HalfSize.x * 2.0f);
	ZEVector3 Up = BoundingBox.Up * (BoundingBox.HalfSize.y * 2.0f);
	ZEVector3 Front = BoundingBox.Front * (BoundingBox.HalfSize.z * 2.0f);

	Factor = (ZEVector3::DotProduct(Right, Point) - ZEVector3::DotProduct(Position, Right)) / ZEVector3::DotProduct(Right, Right);
	if (Factor < 0.0f || Factor > 1.0f) 
		return false;

	Factor = (ZEVector3::DotProduct(Up, Point) - ZEVector3::DotProduct(Position, Up)) / ZEVector3::DotProduct(Up, Up);
	if (Factor < 0.0f || Factor > 1.0f) 
		return false;

	Factor = (ZEVector3::DotProduct(Front, Point) - ZEVector3::DotProduct(Position, Front)) / ZEVector3::DotProduct(Front, Front);
	if (Factor < 0.0f || Factor > 1.0f) 
		return false;

	return true;
}

static inline void CheckSwap(float& a, float& b) 
{
	float Temp;
	Temp = a;
	a = b;
	b = Temp;
}

static inline int SlabTest(const ZEVector3& Center, const ZEVector3& PlaneNormal, float HalfSize, const ZELine* Line, float& TMin, float& TMax)
{
	float e = ZEVector3::DotProduct(PlaneNormal, Center - Line->p);
	float f = ZEVector3::DotProduct(PlaneNormal, Line->v);

	if (fabs(f) > ZE_ZERO_TRESHOLD)
	{
		float t1 = (e + HalfSize) / f;
		float t2 = (e - HalfSize) / f;
		if (t1 > t2) CheckSwap(t1, t2);
		if (t1 > TMin) TMin = t1;
		if (t2 < TMax) TMax = t2;
		if (TMin > TMax)
			return 0;

		return 1;
	}
	else if (-e - HalfSize > 0 || -e + HalfSize < 0) // Ray is parallel but test whether line lies between planes.
		return 0;

	return 2;
}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZELine& Line)
{
	float Temp;
	return IntersectionTest(BoundingBox, Line, Temp, Temp);
}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZELine& Line, float& TMin)
{
	float Temp;
	return IntersectionTest(BoundingBox, Line, TMin, Temp);
}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZELine& Line, float& TMin, float& TMax)
{
	float TempTMin = FLT_MAX;
	float TempTMax = FLT_MIN;

	if (!SlabTest(BoundingBox.Center, BoundingBox.Right, BoundingBox.HalfSize.x, &Line, TempTMin, TempTMax))
		return false;

	if (!SlabTest(BoundingBox.Center, BoundingBox.Up, BoundingBox.HalfSize.y, &Line, TempTMin, TempTMax))
		return false;

	if (!SlabTest(BoundingBox.Center, BoundingBox.Front, BoundingBox.HalfSize.z, &Line, TempTMin, TempTMax))
		return false;

	TMin = TempTMin;
	TMax = TempTMax;

	return true;
}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZERay& Line)
{
	float Temp;
	return IntersectionTest(BoundingBox, Line, Temp, Temp);
}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZERay& Line, float& TMin)
{
	float Temp;
	return IntersectionTest(BoundingBox, Line, TMin, Temp);
}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZERay& Ray, float& TMin, float& TMax)
{
	float TempTMin = -FLT_MAX;
	float TempTMax = FLT_MAX;

	int Result = SlabTest(BoundingBox.Center, BoundingBox.Right, BoundingBox.HalfSize.x, &Ray, TempTMin, TempTMax);
	if (Result == 0)
		return false;

	if (Result == 1 && TempTMax < 0.0f)
		return false;

	Result = SlabTest(BoundingBox.Center, BoundingBox.Up, BoundingBox.HalfSize.y, &Ray, TempTMin, TempTMax);
	if (Result == 0)
		return false;

	if (Result == 1 && TempTMax < 0.0f)
		return false;

	Result = SlabTest(BoundingBox.Center, BoundingBox.Front, BoundingBox.HalfSize.z, &Ray, TempTMin, TempTMax);
	if (Result == 0)
		return false;

	if (Result == 1 && TempTMax < 0.0f)
		return false;

	if (TMin > 0.0f)
	{
		TMin = TempTMin;
		TMax = TempTMax;
	}
	else
	{
		TMin = TMax = TempTMax;
	}

	return true;
}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZELineSegment& Line)
{
	float Temp;
	return IntersectionTest(BoundingBox, Line, Temp, Temp);
}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZELineSegment& Line, float& TMin)
{
	float Temp;
	return IntersectionTest(BoundingBox, Line, TMin, Temp);
}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZELineSegment& LineSegment, float& TMin, float& TMax)
{
	float TempTMin = FLT_MAX;
	float TempTMax = FLT_MIN;

	int Result = SlabTest(BoundingBox.Center, BoundingBox.Right, BoundingBox.HalfSize.x, &LineSegment, TempTMin, TempTMax);
		return false;
	
	if (Result == 2)
		if (TempTMax < 0.0f || TempTMax > 1.0f)
			return false;

	Result = SlabTest(BoundingBox.Center, BoundingBox.Up, BoundingBox.HalfSize.y, &LineSegment, TempTMin, TempTMax);
		return false;

	if (Result == 2)
		if (TempTMax < 0.0f || TempTMax > 1.0f)
			return false;

	Result = SlabTest(BoundingBox.Center, BoundingBox.Front, BoundingBox.HalfSize.z, &LineSegment, TempTMin, TempTMax);
		return false;

	if (Result == 2)
		if (TempTMax < 0.0f || TempTMax > 1.0f)
			return false;

	if (TMin > 0.0f)
	{
		TMin = TempTMin;
		TMax = TempTMax;
	}
	else
	{
		TMin = TMax = TempTMax;
	}

	return true;

}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZEBSphere& BoundingSphere)
{
	return ZEBSphere::IntersectionTest(BoundingSphere, BoundingBox);
}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox1, const ZEAABBox& BoundingBox2)
{

}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox1, const ZEOBBox& BoundingBox2)
{

}

ZEOBBox::ZEOBBox()
{

}

ZEOBBox::ZEOBBox(const ZEVector3& Center, const ZEVector3& Right, const ZEVector3& Up, const ZEVector3& Front, const ZEVector3& HalfSize)
{
	this->Center = Center;
	this->Right = Right;
	this->Up = Up;
	this->Front = Front;
	this->HalfSize = HalfSize;
}

ZEOBBox::ZEOBBox(const ZEVector3& Position, const ZEQuaternion& Rotation, const ZEVector3& Scale)
{
	CreateFromOrientation(*this, Position, Rotation, Scale);
}
