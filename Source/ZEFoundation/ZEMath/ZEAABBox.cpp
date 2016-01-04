//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEAABBox.cpp
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
#include "ZEMath.h"

#include <float.h>
#include "ZEMath/ZEMath.h"

#define		X_AXIS		(ZEUInt)0x01
#define		Y_AXIS		(ZEUInt)0x02
#define		Z_AXIS		(ZEUInt)0x04

ZEVector3 ZEAABBox::GetCenter() const
{
	ZEVector3 Center;
	ZEVector3::Add(Center, Max, Min);
	ZEVector3::Scale(Center, Center, 0.5);
	return Center;
}

ZEVector3 ZEAABBox::GetVertex(ZEUInt Index) const
{
	return  ZEVector3(Index & X_AXIS ? Max.x : Min.x, Index & Y_AXIS ? Max.y : Min.y, Index & Z_AXIS ? Max.z : Min.z);
}

float ZEAABBox::GetLength() const
{
	ZEVector3 Temp;
	ZEVector3::Sub(Temp, Max, Min);
	
	return ZEVector3::Length(Temp);
}

void ZEAABBox::Transform(ZEAABBox& Output, const ZEAABBox& Input, const ZEMatrix4x4& TransformMatrix)
{
	ZEVector3 Point;
	ZEMatrix4x4::Transform(Point, TransformMatrix, Input.GetVertex(0));
	Output.Min = Output.Max = Point;
	for (ZEInt I = 1; I < 8; I++)
	{
		ZEMatrix4x4::Transform(Point, TransformMatrix, Input.GetVertex(I));
		if (Point.x < Output.Min.x) Output.Min.x = Point.x;
		if (Point.y < Output.Min.y) Output.Min.y = Point.y;
		if (Point.z < Output.Min.z) Output.Min.z = Point.z;

		if (Point.x > Output.Max.x) Output.Max.x = Point.x;
		if (Point.y > Output.Max.y) Output.Max.y = Point.y;
		if (Point.z > Output.Max.z) Output.Max.z = Point.z;
	}
}

ZEHalfSpace ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZEPlane& Plane)
{
	ZEVector3 Center = (BoundingBox.Max + BoundingBox.Min) * 0.5f;
	ZEVector3 HalfDiameter = (BoundingBox.Max - BoundingBox.Min) * 0.5f;

	float Extent = HalfDiameter.x * ZEMath::Abs(Plane.n.x) + HalfDiameter.y * ZEMath::Abs(Plane.n.y) + HalfDiameter.z * ZEMath::Abs(Plane.n.z);

	float Distance = ZEVector3::DotProduct(Center - Plane.p, Plane.n);

	if (Distance - Extent > 0)
		return ZE_HS_POSITIVE_SIDE;
	else if (Distance + Extent < 0)
		return ZE_HS_NEGATIVE_SIDE;
	else
		return ZE_HS_INTERSECTS;
}

void ZEAABBox::GenerateBoundingSphere(ZEBSphere& BoundingSphere, const ZEAABBox& BoundingBox)
{
	float a = (BoundingBox.Max - BoundingBox.Min).Length();
	BoundingSphere.Radius = (BoundingBox.Max - BoundingBox.Min).Length() * 0.5f;
	BoundingSphere.Position = BoundingBox.GetCenter();
}

void ZEAABBox::GenerateOBoundingBox(ZEOBBox& OrientedBoundingBox, const ZEAABBox& BoundingBox)
{
	OrientedBoundingBox.Center = (BoundingBox.Min + BoundingBox.Max) * 0.5f;

	OrientedBoundingBox.HalfSize = (BoundingBox.Max - BoundingBox.Min) * 0.5f;
	OrientedBoundingBox.Right.x = 1.0f;
	OrientedBoundingBox.Right.y = 0;
	OrientedBoundingBox.Right.z = 0;

	OrientedBoundingBox.Up.x = 0;
	OrientedBoundingBox.Up.y = 1.0f;
	OrientedBoundingBox.Up.z = 0;

	OrientedBoundingBox.Front.x = 0;
	OrientedBoundingBox.Front.y = 0;
	OrientedBoundingBox.Front.z = 1.0f;
}

static inline void CheckSwap(float& a, float& b) 
{
	float Temp;
	Temp = a;
	a = b;
	b = Temp;
}

static inline ZEInt SlabTest(const ZEVector3& Center, const ZEVector3& PlaneNormal, float HalfSize, const ZELine* Line, float& TMin, float& TMax)
{
	float e = ZEVector3::DotProduct(PlaneNormal, Center - Line->p);
	float f = ZEVector3::DotProduct(PlaneNormal, Line->v);

	if (ZEMath::Abs(f) > ZE_ZERO_THRESHOLD)
	{
		float t1 = (e + HalfSize) / f;
		float t2 = (e - HalfSize) / f;
		if (t1 > t2) 
			CheckSwap(t1, t2);

		if (t1 > TMin) 
			TMin = t1;

		if (t2 < TMax) 
			TMax = t2;

		if (TMin > TMax)
			return 0;

		return 1;
	}
	else if (-e - HalfSize > 0 || -e + HalfSize < 0) // Ray is parallel but test whether line lies between planes.
		return 0;

	return 2;
}


bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZEVector3& Point)
{
	return (BoundingBox.Min.x <= Point.x) && (BoundingBox.Max.x >= Point.x) &&
		(BoundingBox.Min.y <= Point.y) && (BoundingBox.Max.y >= Point.y) && 
		(BoundingBox.Min.z <= Point.z) && (BoundingBox.Max.z >= Point.z);
}

bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZELine& Line)
{
	float Temp;
	return IntersectionTest(BoundingBox, Line, Temp, Temp);
}

bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZELine& Line, float& TMin)
{
	float Temp;
	return IntersectionTest(BoundingBox, Line, TMin, Temp);
}

bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZELine& Line, float& TMin, float& TMax)
{
	float TempTMin = -FLT_MAX;
	float TempTMax = FLT_MAX;

	ZEVector3 Center = BoundingBox.GetCenter();
	if (SlabTest(Center, ZEVector3::UnitX, (BoundingBox.Max.x - BoundingBox.Min.x) * 0.5f, &Line, TempTMin, TempTMax) == 0)
		return false;

	if (SlabTest(Center, ZEVector3::UnitY, (BoundingBox.Max.y - BoundingBox.Min.y) * 0.5f, &Line, TempTMin, TempTMax) == 0)
		return false;

	if (SlabTest(Center, ZEVector3::UnitZ, (BoundingBox.Max.z - BoundingBox.Min.z) * 0.5f, &Line, TempTMin, TempTMax) == 0)
		return false;

	TMin = TempTMin;
	TMax = TempTMax;

	return true;
}

bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZERay& Line)
{
	float Temp;
	return IntersectionTest(BoundingBox, Line, Temp, Temp);
}

bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZERay& Line, float& TMin)
{
	float Temp;
	return IntersectionTest(BoundingBox, Line, TMin, Temp);
}

bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZERay& Ray, float & TMin, float& TMax)
{
	float TempTMin = -FLT_MAX;
	float TempTMax = FLT_MAX;

	ZEVector3 Center = BoundingBox.GetCenter();
	ZEInt Result = SlabTest(Center, ZEVector3::UnitX, (BoundingBox.Max.x - BoundingBox.Min.x) * 0.5f, &Ray, TempTMin, TempTMax);
	if (Result == 0)
		return false;

	if (Result == 1 && TempTMax < 0.0f)
		return false;

	Result = SlabTest(Center, ZEVector3::UnitY, (BoundingBox.Max.y - BoundingBox.Min.y) * 0.5f, &Ray, TempTMin, TempTMax);
	if (Result == 0)
		return false;

	if (Result == 1 && TempTMax < 0.0f)
		return false;

	Result = SlabTest(Center, ZEVector3::UnitZ, (BoundingBox.Max.z - BoundingBox.Min.z) * 0.5f, &Ray, TempTMin, TempTMax);
	if (Result == 0)
		return false;

	if (Result == 1 && TempTMax < 0.0f)
		return false;

	if (TempTMin > 0.0f)
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

bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZELineSegment& Line)
{
	float Temp;
	return IntersectionTest(BoundingBox, Line, Temp, Temp);
}

bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZELineSegment& Line, float& TMin)
{
	float Temp;
	return IntersectionTest(BoundingBox, Line, TMin, Temp);
}

bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZELineSegment& LineSegment, float& TMin, float& TMax)
{
	float TempTMin = -FLT_MAX;
	float TempTMax = FLT_MAX;

	ZEVector3 Center = BoundingBox.GetCenter();
	ZEInt Result = SlabTest(Center, ZEVector3::UnitX, (BoundingBox.Max.x - BoundingBox.Min.x) * 0.5f, &LineSegment, TempTMin, TempTMax);
	if (Result == 0)
		return false;

	if (Result == 2)
		if (TempTMax < 0.0f && TempTMax > LineSegment.Length)
			return false;

	Result = SlabTest(Center, ZEVector3::UnitY, (BoundingBox.Max.y - BoundingBox.Min.y) * 0.5f, &LineSegment, TempTMin, TempTMax);
	if (Result == 0)
		return false;

	if (Result == 2)
		if (TempTMax < 0.0f && TempTMax > LineSegment.Length)
			return false;

	Result = SlabTest(Center, ZEVector3::UnitZ, (BoundingBox.Max.z - BoundingBox.Min.z) * 0.5f, &LineSegment, TempTMin, TempTMax);
	if (Result == 0)
		return false;

	if (Result == 2)
		if (TempTMax < 0.0f && TempTMax > LineSegment.Length)
			return false;

	if (TempTMin > 0.0f)
	{
		TMin = TempTMin;
		TMax = TempTMax;
	}
	else
	{
		TMin = TMax = TempTMax;
	}

	if(TMax > LineSegment.Length)
		TMax = TMin;

	if(TMin > LineSegment.Length)
	{
		TMin = TMax;
		return false;
	}

	if(TMax < 0.0f)
	{
		TMax = TMin;
		return false;
	}

	return true;
}

bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZEBSphere& BoundingSphere)
{
	return ZEBSphere::IntersectionTest(BoundingSphere, BoundingBox);
}

bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox1, const ZEAABBox& BoundingBox2)
{
	if (BoundingBox1.Min.x > BoundingBox2.Max.x || BoundingBox2.Min.x > BoundingBox1.Max.x)
		return false;

	if (BoundingBox1.Min.y > BoundingBox2.Max.y || BoundingBox2.Min.y > BoundingBox1.Max.y)
		return false;

	if (BoundingBox1.Min.z > BoundingBox2.Max.z || BoundingBox2.Min.z > BoundingBox1.Max.z)
		return false;

	return true;
}

bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox1, const ZEOBBox& BoundingBox2)
{
	ZEOBBox TempBoundingBox;
	GenerateOBoundingBox(TempBoundingBox, BoundingBox1);

	return ZEOBBox::IntersectionTest(TempBoundingBox, BoundingBox2);
}

void ZEAABBox::Generate(ZEAABBox& Output, const ZEVector3* Vertices, ZESize Count)
{
	if (Count == 0)
		return;

	Output.Min = Output.Max = Vertices[0];
	
	for (ZESize I = 1; I < Count; I++)
	{
		if (Output.Min.x > Vertices[I].x)
			Output.Min.x = Vertices[I].x;
		
		if (Output.Max.x < Vertices[I].x)
			Output.Max.x = Vertices[I].x;

		if (Output.Min.y > Vertices[I].y)
			Output.Min.y = Vertices[I].y;

		if (Output.Max.y < Vertices[I].y)
			Output.Max.y = Vertices[I].y;

		if (Output.Min.z > Vertices[I].z)
			Output.Min.z = Vertices[I].z;

		if (Output.Max.z < Vertices[I].z)
			Output.Max.z = Vertices[I].z;
	}
}

void ZEAABBox::Combine(ZEAABBox& Output, const ZEAABBox& A, const ZEAABBox& B)
{
	Output.Min = A.Min;
	Output.Max = A.Max;

	if (Output.Min.x > B.Min.x)
		Output.Min.x = B.Min.x;

	if (Output.Max.x < B.Max.x)
		Output.Max.x = B.Max.x;

	if (Output.Min.y > B.Min.y)
		Output.Min.y = B.Min.y;

	if (Output.Max.y < B.Max.y)
		Output.Max.y = B.Max.y;

	if (Output.Min.z > B.Min.z)
		Output.Min.z = B.Min.z;

	if (Output.Max.z < B.Max.z)
		Output.Max.z = B.Max.z;
}


ZEAABBox::ZEAABBox()
{
}

ZEAABBox::ZEAABBox(const ZEVector3& Min, const ZEVector3& Max)
{
	this->Min = Min;
	this->Max = Max;
}
