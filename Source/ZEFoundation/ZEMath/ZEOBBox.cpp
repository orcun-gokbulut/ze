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
#include "ZEMath.h"
#include "ZEError.h"

#include <float.h>
#include "ZEMath/ZEMath.h"

#define		X_AXIS		(ZEUInt)0x01
#define		Y_AXIS		(ZEUInt)0x02
#define		Z_AXIS		(ZEUInt)0x04

ZEVector3 ZEOBBox::GetVertex(ZEUInt Index) const
{
	ZEVector3 Max = Center + Up * HalfSize.x + Right * HalfSize.y + Front * HalfSize.z;
	ZEVector3 Min = Center - Up * HalfSize.x - Right * HalfSize.y - Front * HalfSize.z;

	return  ZEVector3(Index & X_AXIS ? Max.x : Min.x, Index & Y_AXIS ? Max.y : Min.y, Index & Z_AXIS ? Max.z : Min.z);
}

void ZEOBBox::CreateFromOrientation(ZEOBBox& BoundingBox, const ZEVector3& Position, const ZEQuaternion& Rotation, const ZEVector3& Size)
{
	BoundingBox.Center = Position;
	ZEQuaternion::VectorProduct(BoundingBox.Right, Rotation, ZEVector3::UnitX);
	ZEQuaternion::VectorProduct(BoundingBox.Up, Rotation, ZEVector3::UnitY);
	ZEQuaternion::VectorProduct(BoundingBox.Front, Rotation, ZEVector3::UnitZ);
	BoundingBox.HalfSize = Size * 0.5f;
}

void ZEOBBox::Transform(ZEOBBox& Output, const ZEMatrix4x4& Matrix, const ZEOBBox& Input)
{
	ZEMatrix4x4::Transform(Output.Center, Matrix, Input.Center);
	ZEMatrix4x4::Transform3x3(Output.Right, Matrix, Input.Right);
	ZEMatrix4x4::Transform3x3(Output.Up, Matrix, Input.Up);
	ZEMatrix4x4::Transform3x3(Output.Front, Matrix, Input.Front);
}

void ZEOBBox::ConvertToSphere(ZEBSphere& Sphere, const ZEOBBox& Input)
{
	Sphere.Position = Input.Center;
	Sphere.Radius = Input.HalfSize.Max();
}

ZEHalfSpace ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZEPlane& Plane)
{
	float Extent = 
		BoundingBox.HalfSize.x * ZEMath::Abs(ZEVector3::DotProduct(Plane.n, BoundingBox.Right)) +
		BoundingBox.HalfSize.y * ZEMath::Abs(ZEVector3::DotProduct(Plane.n, BoundingBox.Up)) +
		BoundingBox.HalfSize.z * ZEMath::Abs(ZEVector3::DotProduct(Plane.n, BoundingBox.Front));
	float Distance = ZEVector3::DotProduct(BoundingBox.Center - Plane.p, Plane.n);

	if (Distance - Extent > 0)
		return ZE_HS_POSITIVE_SIDE;
	else if (Distance + Extent < 0)
		return ZE_HS_NEGATIVE_SIDE;
	else
		return ZE_HS_INTERSECTS;
}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZEVector3& Point)
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
	float TempTMin = -FLT_MAX;
	float TempTMax = FLT_MAX;

	if (SlabTest(BoundingBox.Center, BoundingBox.Right, BoundingBox.HalfSize.x, &Line, TempTMin, TempTMax) == 0)
		return false;

	if (SlabTest(BoundingBox.Center, BoundingBox.Up, BoundingBox.HalfSize.y, &Line, TempTMin, TempTMax) == 0)
		return false;

	if (SlabTest(BoundingBox.Center, BoundingBox.Front, BoundingBox.HalfSize.z, &Line, TempTMin, TempTMax) == 0)
		return false;

	TMin = TempTMin;
	TMax = TempTMax;

	return true;
}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZERay& Ray)
{
	float Temp;
	return IntersectionTest(BoundingBox, Ray, Temp, Temp);
}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZERay& Ray, float& TMin)
{
	float Temp;
	return IntersectionTest(BoundingBox, Ray, TMin, Temp);
}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZERay& Ray, float& TMin, float& TMax)
{
	float TempTMin = -FLT_MAX;
	float TempTMax = FLT_MAX;

	ZEInt Result = SlabTest(BoundingBox.Center, BoundingBox.Right, BoundingBox.HalfSize.x, &Ray, TempTMin, TempTMax);
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

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZELineSegment& LineSegment)
{
	float Temp;
	return IntersectionTest(BoundingBox, LineSegment, Temp, Temp);
}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZELineSegment& LineSegment, float& TMin)
{
	float Temp;
	return IntersectionTest(BoundingBox, LineSegment, TMin, Temp);
}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZELineSegment& LineSegment, float& TMin, float& TMax)
{
	float TempTMin = -FLT_MAX;
	float TempTMax = FLT_MAX;

	ZEInt Result = SlabTest(BoundingBox.Center, BoundingBox.Right, BoundingBox.HalfSize.x, &LineSegment, TempTMin, TempTMax);

	if(Result == 0)
		return false;
	
	if (Result == 2)
		if (TempTMax < 0.0f && TempTMax > LineSegment.Length)
			return false;

	Result = SlabTest(BoundingBox.Center, BoundingBox.Up, BoundingBox.HalfSize.y, &LineSegment, TempTMin, TempTMax);

	if(Result == 0)
		return false;

	if (Result == 2)
		if (TempTMax < 0.0f && TempTMax > LineSegment.Length)
			return false;

	Result = SlabTest(BoundingBox.Center, BoundingBox.Front, BoundingBox.HalfSize.z, &LineSegment, TempTMin, TempTMax);

	if(Result == 0)
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

	if(TMax > LineSegment.Length) // Fazlalıkları çıkart
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

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZEBSphere& BoundingSphere)
{
	return ZEBSphere::IntersectionTest(BoundingSphere, BoundingBox);
}

bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox1, const ZEAABBox& BoundingBox2)
{
	return false;
}

static inline bool SeparatingAxisTest(const ZEVector3& Axis, const ZEOBBox& A, const ZEOBBox& B)
{
	float dA = A.HalfSize.x * ZEMath::Abs(ZEVector3::DotProduct(A.Right, Axis)) +
		A.HalfSize.y *  ZEMath::Abs(ZEVector3::DotProduct(A.Up, Axis)) +
		A.HalfSize.z *  ZEMath::Abs(ZEVector3::DotProduct(A.Front, Axis));

	float dB = B.HalfSize.x * ZEMath::Abs(ZEVector3::DotProduct(B.Right, Axis)) +
		B.HalfSize.y *  ZEMath::Abs(ZEVector3::DotProduct(B.Up, Axis)) +
		B.HalfSize.z *  ZEMath::Abs(ZEVector3::DotProduct(B.Front, Axis));

	return (ZEMath::Abs(ZEVector3::DotProduct(ZEVector3(A.Center, B.Center), Axis)) > (dA + dB)); 
}

bool ZEOBBox::IntersectionTest(const ZEOBBox& A, const ZEOBBox& B)
{
	if (SeparatingAxisTest(A.Right, A, B))
		return false;

	if (SeparatingAxisTest(A.Up, A, B))
		return false;

	if (SeparatingAxisTest(A.Front, A, B))
		return false;

	if (SeparatingAxisTest(B.Right, A, B))
		return false;

	if (SeparatingAxisTest(B.Up, A, B))
		return false;

	if (SeparatingAxisTest(B.Front, A, B))
		return false;

	ZEVector3 CrossProductOutput;

	ZEVector3::CrossProduct(CrossProductOutput, B.Right, A.Right);

	if (SeparatingAxisTest(CrossProductOutput, A, B))
		return false;

	ZEVector3::CrossProduct(CrossProductOutput, B.Right, A.Up);

	if (SeparatingAxisTest(CrossProductOutput, A, B))
		return false;

	ZEVector3::CrossProduct(CrossProductOutput, B.Right, A.Front);

	if (SeparatingAxisTest(CrossProductOutput, A, B))
		return false;

	ZEVector3::CrossProduct(CrossProductOutput, B.Up, A.Right);

	if (SeparatingAxisTest(CrossProductOutput, A, B))
		return false;

	ZEVector3::CrossProduct(CrossProductOutput, B.Up, A.Up);

	if (SeparatingAxisTest(CrossProductOutput, A, B))
		return false;

	ZEVector3::CrossProduct(CrossProductOutput, B.Up, A.Front);

	if (SeparatingAxisTest(CrossProductOutput, A, B))
		return false;

	ZEVector3::CrossProduct(CrossProductOutput, B.Front, A.Right);

	if (SeparatingAxisTest(CrossProductOutput, A, B))
		return false;

	ZEVector3::CrossProduct(CrossProductOutput, B.Front, A.Up);

	if (SeparatingAxisTest(CrossProductOutput, A, B))
		return false;

	ZEVector3::CrossProduct(CrossProductOutput, B.Front, A.Front);

	if (SeparatingAxisTest(CrossProductOutput, A, B))
		return false;

	return true;
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

ZEOBBox::ZEOBBox(const ZEVector3& Position, const ZEQuaternion& Rotation, const ZEVector3& Size)
{
	CreateFromOrientation(*this, Position, Rotation, Size);
}
