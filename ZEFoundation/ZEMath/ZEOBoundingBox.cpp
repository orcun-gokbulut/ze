//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEOBoundingBox.cpp
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

#include "ZEBoundingSphere.h"
#include "ZEAABoundingBox.h"
#include "ZEOBoundingBox.h"

#include <float.h>

#include <math.h>


ZEVector3 ZEOBoundingBox::GetVertex(unsigned char Index) const
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

ZEVector3 ZEOBoundingBox::GetEdge(unsigned char Index) const
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

void ZEOBoundingBox::Transform(ZEOBoundingBox& Output, const ZEMatrix4x4& Matrix, const ZEOBoundingBox& Input)
{
	ZEVector3 NewCenter, NewU, NewV, NewN;
	ZEMatrix4x4::Transform(NewCenter, Matrix, Input.Center);
	ZEMatrix4x4::Transform3x3(NewU, Matrix, Input.Right);
	ZEMatrix4x4::Transform3x3(NewU, Matrix, Input.Up);
	ZEMatrix4x4::Transform3x3(NewU, Matrix, Input.Front);
}

void ZEOBoundingBox::ConvertToSphere(ZEBoundingSphere& Sphere, const ZEOBoundingBox& Input)
{
	Sphere.Position = Input.Center;
	Sphere.Radius = Input.HalfSize.Max();
}

ZEHalfSpace ZEOBoundingBox::PlaneHalfSpaceTest(const ZEOBoundingBox& BoundingBox, const ZEPlane& Plane)
{
	ZEHalfSpace HS1 = ZEPlane::TestHalfSpace(Plane, BoundingBox.GetVertex(0));
	for (int I = 1; I < 8; I++)
	{
		ZEHalfSpace HS2 = ZEPlane::TestHalfSpace(Plane, BoundingBox.GetVertex(I));
		if (HS1 != HS2)
			return ZEHALFSPACE_INTERSECTS;
	}
	return HS1;
}

bool ZEOBoundingBox::IntersectionTest(const ZEOBoundingBox& BoundingBox, const ZEVector3 Point)
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

static inline void CheckGreaterSwap(float& a, float& b) 
{
	if (b > a)
	{
		float Temp;
		Temp = a;
		a = b;
		b = Temp;
	}
}

static inline void CheckLesserSwap(float& a, float& b) 
{
	if (b < a)
	{
		float Temp;
		Temp = a;
		a = b;
		b = Temp;
	}
}

bool ZEOBoundingBox::IntersectionTest(const ZEOBoundingBox& BoundingBox, const ZELine& Line, float& TMin, float& TMax)
{
	float T1, T2;
	float TMinI = FLT_MAX , TMaxI = FLT_MIN;

	ZEPlane XN(BoundingBox.GetEdge(0), -BoundingBox.Right);	
	if (!ZEPlane::IntersectionTest(XN, Line, T1))
		return false;

	ZEPlane XP(BoundingBox.GetEdge(1), BoundingBox.Right);
	ZEPlane::IntersectionTest(XP, Line, T2);
	
	CheckGreaterSwap(T1, T2);
	CheckGreaterSwap(TMinI, T1);
	CheckLesserSwap(TMaxI, T2);
	if (TMinI > TMaxI)
		return false;

	ZEPlane YN(BoundingBox.GetEdge(3), -BoundingBox.Up);
	if (!ZEPlane::IntersectionTest(YN, Line, T1))
		return false;
	
	ZEPlane YP(BoundingBox.GetEdge(4), BoundingBox.Up);
	ZEPlane::IntersectionTest(YP, Line, T2);

	CheckGreaterSwap(T1, T2);
	CheckGreaterSwap(TMinI, T1);
	CheckLesserSwap(TMaxI, T2);
	if (TMinI > TMaxI)
		return false;

	ZEPlane ZN(BoundingBox.GetEdge(5), -BoundingBox.Front);
	if (!ZEPlane::IntersectionTest(ZN, Line, T2));
		return false;

	ZEPlane ZP(BoundingBox.GetEdge(6), BoundingBox.Front);
	ZEPlane::IntersectionTest(ZP, Line, T2);

	CheckGreaterSwap(T1, T2);
	CheckGreaterSwap(TMinI, T1);
	CheckLesserSwap(TMaxI, T2);
	if (TMinI > TMaxI)
		return false;

	TMin = TMin;
	TMax = TMax;
	return true;
}

bool ZEOBoundingBox::IntersectionTest(const ZEOBoundingBox& BoundingBox, const ZERay& Ray, float& TMin, float& TMax)
{
	return false;
}

bool ZEOBoundingBox::IntersectionTest(const ZEOBoundingBox& BoundingBox, const ZELineSegment& LineSegment, float& TMin, float& TMax)
{
	return false;

}

ZEOBoundingBox::ZEOBoundingBox()
{
}

ZEOBoundingBox::ZEOBoundingBox(const ZEVector3 Center, const ZEVector3 Right, const ZEVector3 Up, const ZEVector3 Front)
{
	this->Center = Center;
	this->Right = Right;
	this->Up = Up;
	this->Front = Front;
}


/*
bool ZEOBoundingBox::CollisionTest(const ZEOBoundingBox& BoundingBox1, const ZEOBoundingBox& BoundingBox2)
{
	//ZEVector3 Temp;
	//float UDotU = ZEVector3::DotProduct(BoundingBox.U,BoundingBox.U);
	//float VDotV = ZEVector3::DotProduct(BoundingBox.V,BoundingBox.V);
	//float NDotN;
	//float factor;
	//ZEVector3 PointMinusBox;
	//ZEVector3::Sub(PointMinusBox,PointMinusBox,BoundingBox);

	//factor = (ZEVector3::DotProduct(BoundingBox.U,Point) - ZEVector3::DotProduct(BoundingBox.Position,BoundingBox.U)) / UDotU;
	//ZEVector3::Scale(Temp,BoundingBox.U,-factor);
	//ZEVector3::Add(Temp,Temp,PointMinusBox); // Temp =: Projection of point on YZ Plane of the oobb

	//float proj2d = ZEVector3::DotProduct(BoundingBox.V,Temp) / VDotV;
	//if (proj2d < 0 || proj2d > 1) return 0;

	//NDotN = ZEVector3::DotProduct(BoundingBox.N,BoundingBox.N);
	//proj2d = ZEVector3::DotProduct(BoundingBox.N,Temp) / NDotN;
	//if (proj2d < 0 || proj2d > 1) return 0;

	////-----------------

	//factor = (ZEVector3::DotProduct(BoundingBox.V,Point) - ZEVector3::DotProduct(BoundingBox.Position,BoundingBox.V)) / VDotV;
	//ZEVector3::Scale(Temp,BoundingBox.V,-factor);
	//ZEVector3::Add(Temp,Temp,PointMinusBox); // Temp =: Projection of point on XZ Plane of the oobb

	//proj2d = ZEVector3::DotProduct(BoundingBox.U,Temp) / UDotU;
	//if (proj2d < 0 || proj2d > 1) return 0;

	//proj2d = ZEVector3::DotProduct(BoundingBox.N,Temp) / NDotN;
	//if (proj2d < 0 || proj2d > 1) return 0;

	////------------------

	//factor = (ZEVector3::DotProduct(BoundingBox.N,Point) - ZEVector3::DotProduct(BoundingBox.Position,BoundingBox.N)) / NDotN;
	//ZEVector3::Scale(Temp,BoundingBox.N,-factor);
	//ZEVector3::Add(Temp,Temp,PointMinusBox); // Temp =: Projection of point on XY Plane of the oobb

	//proj2d = ZEVector3::DotProduct(BoundingBox.U,Temp) / UDotU;
	//if (proj2d < 0 || proj2d > 1) return 0;

	//proj2d = ZEVector3::DotProduct(BoundingBox.V,Temp) / VDotV;
	//if (proj2d < 0 || proj2d > 1) return 0;

	ZEASSERT(true "Not implamented");
	return false;
}
bool ZEOBoundingBox::CollisionTest(const ZEOBoundingBox& BoundingBox1, const ZEAABoundingBox& BoundingBox2)
{
	ZEASSERT(true "Not implamented");
	return false;
}

bool ZEOBoundingBox::CollisionTest(const ZEOBoundingBox& BoundingBox, const ZEBoundingSphere& BoundingSphere)
{
	ZEASSERT(true "Not implamented");
	return false;
}*/




