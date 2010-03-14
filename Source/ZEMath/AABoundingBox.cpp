//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - AABoundingBox.cpp
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

ZEVector3 ZEAABoundingBox::GetCenter() const
{
	ZEVector3 Center;
	ZEVector3::Sub(Center, Max, Min);
	ZEVector3::Scale(Center, Center, 0.5);
	ZEVector3::Add(Center, Center, Min);
	return Center;
}

ZEVector3 ZEAABoundingBox::GetVertex(unsigned char Index) const
{
	ZEVector3 Vertex;

	if ((Index & 0x04) > 0)
		Vertex.x = Max.x;
	else
		Vertex.x = Min.x;

	if ((Index & 0x02) > 0)
		Vertex.y = Max.y;
	else
		Vertex.y = Min.y;

	if ((Index & 0x01) > 0)
		Vertex.z = Max.z;
	else
		Vertex.z = Min.z;

	return Vertex;
}

float ZEAABoundingBox::GetLenght() const
{
	ZEVector3 Temp;
	ZEVector3::Sub(Temp, Max, Min);
	
	return ZEVector3::Length(Temp);
}

void ZEAABoundingBox::Transform(ZEAABoundingBox& Output, const ZEAABoundingBox& Input, const ZEMatrix4x4& TransformMatrix)
{
	ZEVector3 Point;
	ZEMatrix4x4::Transform(Point, TransformMatrix, Input.GetVertex(0));
	Output.Min = Output.Max = Point;
	for (int I = 1; I < 8; I++)
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

ZEHalfSpace ZEAABoundingBox::PlaneHalfSpaceTest(const ZEAABoundingBox& BoundingBox, const ZEPlane& Plane)
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

ZEAABoundingBox::ZEAABoundingBox()
{
}

ZEAABoundingBox::ZEAABoundingBox(const ZEVector3 Min, const ZEVector3 Max)
{
	this->Min = Min;
	this->Max = Max;
}

void ZEAABoundingBox::GenerateBoundingSphere(ZEBoundingSphere& BoundingSphere) const
{
	BoundingSphere.Radius = 0.0f;
	BoundingSphere.Position = GetCenter();

	float Length;
	for (unsigned int I = 0; I < 8; I++)
	{
		Length = ZEVector3::DistanceSquare(BoundingSphere.Position, GetVertex(I));
		if (Length > BoundingSphere.Radius)
			BoundingSphere.Radius = Length;
	}

	BoundingSphere.Radius = sqrtf(BoundingSphere.Radius);

/*	MaxLength = ZEVector3::Length(Max);
	MinLength = ZEVector3::Length(Min);

	if (MaxLength > MinLength)
		BoundingSphere.Radius = MaxLength;
	else
		BoundingSphere.Radius = MinLength;*/
}


void ZEAABoundingBox::GenerateBoundingSphere(ZEBoundingSphere& BoundingSphere, const ZEMatrix4x4& Transform) const
{
	float MaxLength,MinLength;

	MaxLength = ZEVector3::Length(Max);
	MinLength = ZEVector3::Length(Min);

	if (MaxLength > MinLength)
		BoundingSphere.Radius = MaxLength;
	else
		BoundingSphere.Radius = MinLength;

	ZEVector3::Add(BoundingSphere.Position, BoundingSphere.Position, *((const ZEVector3*)Transform.M[4]));
}

void ZEAABoundingBox::GenerateOBoundingBox(ZEOBoundingBox& BoundingBox) const
{
	BoundingBox.Position = Min;

	BoundingBox.U.x = Max.x - Min.x;
	BoundingBox.U.y = 0;
	BoundingBox.U.z = 0;

	BoundingBox.V.x = 0;
	BoundingBox.V.y = Max.y - Min.y;
	BoundingBox.V.z = 0;

	BoundingBox.N.x = 0;
	BoundingBox.N.y = 0;
	BoundingBox.N.z = Max.z - Min.z;
}

void ZEAABoundingBox::GenerateOBoundingBox(ZEOBoundingBox& BoundingBox, const ZEMatrix4x4& Transform) const
{
	ZEVector3::Add(BoundingBox.Position, Min, *((const ZEVector3*)Transform.M[4]));

	ZEMatrix4x4::Transform3x3(BoundingBox.U, Transform, ZEVector3(Max.x - Min.x, 0.0f, 0.0f));
	ZEMatrix4x4::Transform3x3(BoundingBox.V, Transform, ZEVector3(0.0f, Max.y - Min.y, 0.0f));
	ZEMatrix4x4::Transform3x3(BoundingBox.N, Transform, ZEVector3(0.0f, 0.0f, Max.z - Min.z));
}

bool ZEAABoundingBox::IntersectionTest(const ZEAABoundingBox& BoundingBox, const ZEVector3 Point)
{
	return (BoundingBox.Min.x >= Point.x) && (BoundingBox.Min.y >= Point.y) && (BoundingBox.Min.z >= Point.z) && (BoundingBox.Max.x >= Point.x) && (BoundingBox.Max.y >= Point.y) && (BoundingBox.Max.z >= Point.z);
}

bool ZEAABoundingBox::IntersectionTest(const ZEAABoundingBox& BoundingBox, const ZELine& Line)
{
	float MinT, MaxT;
	return IntersectionTest(BoundingBox, Line, MinT, MaxT);
}

bool ZEAABoundingBox::IntersectionTest(const ZEAABoundingBox& BoundingBox, const ZERay& Ray)
{
	float MinT, MaxT;
	return IntersectionTest(BoundingBox, Ray, MinT, MaxT);
}

bool ZEAABoundingBox::IntersectionTest(const ZEAABoundingBox& BoundingBox, const ZELineSegment& LineSegment)
{
	float MinT, MaxT;
	return IntersectionTest(BoundingBox, LineSegment, MinT, MaxT);
}

bool ZEAABoundingBox::IntersectionTest(const ZEAABoundingBox& BoundingBox, const ZELine& Line, float& MinT, float& MaxT)
{
	bool TValid = false;

	if (Line.v.x == 0)
	{
		if(Line.p.x < BoundingBox.Min.x || Line.p.x > BoundingBox.Max.x)
			return false;
	}
	else
	{
		TValid = true;

		MinT = (BoundingBox.Min.x - Line.p.x) / Line.v.x;
		MaxT = (BoundingBox.Max.x - Line.p.x) / Line.v.x;

		if (MinT > MaxT)
		{
			float TempT = MinT;
			MinT = MaxT;
			MaxT = TempT;
		}
	}

	if (Line.v.y == 0)
	{
		if (Line.p.y < BoundingBox.Min.y || Line.p.y > BoundingBox.Max.y)
			return false;
	}
	else
	{
		if (TValid)
		{
			float tMinT,tMaxT;

			tMinT = (BoundingBox.Min.y - Line.p.y) / Line.v.y;
			tMaxT = (BoundingBox.Max.y - Line.p.y) / Line.v.y;

			if (tMinT > tMaxT)
			{
				float TempT = tMinT;
				tMinT = tMaxT;
				tMaxT = TempT;
			}

			if (tMinT > MaxT || MinT > tMaxT)
				return false;

			if (MinT < tMinT)
				MinT = tMinT;

			if (MaxT > tMaxT)
				MaxT = tMaxT;
		}
		else
		{
			TValid = true;

			MinT = (BoundingBox.Min.y - Line.p.y) / Line.v.y;
			MaxT = (BoundingBox.Max.y - Line.p.y) / Line.v.y;

			if (MinT > MaxT)
			{
				float TempT = MinT;
				MinT = MaxT;
				MaxT = TempT;
			}
		}
	}

	if (Line.v.z == 0)
	{
		if (Line.p.z < BoundingBox.Min.z || Line.p.z > BoundingBox.Max.z)
			return false;

		if (TValid == false)
		{
			MinT = 0.0f;
			MaxT = 0.0f;
		}

		return true;
	}
	else
	{
		if (TValid)
		{
			float tMinT,tMaxT;

			tMinT = (BoundingBox.Min.z - Line.p.z) / Line.v.z;
			tMaxT = (BoundingBox.Max.z - Line.p.z) / Line.v.z;

			if (tMinT > tMaxT)
			{
				float TempT = tMinT;
				tMinT = tMaxT;
				tMaxT = TempT;
			}

			if (MinT < tMinT)
				MinT = tMinT;

			if (MaxT > tMaxT)
				MaxT = tMaxT;

			if (tMinT > MaxT || MinT > tMaxT)
				return false;
			else
				return true;
		}
		else
		{
			MinT = (BoundingBox.Min.y - Line.p.y) / Line.v.y;
			MaxT = (BoundingBox.Max.y - Line.p.y) / Line.v.y;

			if (MinT > MaxT)
			{
				float TempT = MinT;
				MinT = MaxT;
				MaxT = TempT;
			}

			return true;
		}
	}
}

bool ZEAABoundingBox::IntersectionTest(const ZEAABoundingBox& BoundingBox, const ZERay& Ray, float & MinT, float& MaxT)
{
	if (IntersectionTest(BoundingBox, (ZELine)Ray, MinT, MaxT))
	{
		if (MinT < 0.0f)
		{
			if (MaxT >= 0.0f)
			{
				MinT = 0;
				return true;
			}
			else
				return false;
		}
		else
			return true;
	}
	else
		return false;
}

bool ZEAABoundingBox::IntersectionTest(const ZEAABoundingBox& BoundingBox, const ZELineSegment& LineSegment, float& MinT, float& MaxT)
{
	if (ZEAABoundingBox::IntersectionTest(BoundingBox, (ZELine)LineSegment, MinT, MaxT))
	{
		if (MinT < 0.0f)
			MinT = 0.0f;
		if (MaxT > 1.0f)
			MaxT = 1.0f;

		if (MinT > MaxT)
			return false;

		return true;
	}
	else
		return false;
}

bool ZEAABoundingBox::CollisionTest(const ZEAABoundingBox& BoundingBox1, const ZEOBoundingBox& BoundingBox2)
{
//	zeAssert(true "Not implamented");
	return false;	
}

bool ZEAABoundingBox::CollisionTest(const ZEAABoundingBox& BoundingBox1, const ZEAABoundingBox& BoundingBox2)
{
	return (BoundingBox1.Max.x <= BoundingBox2.Max.x && BoundingBox1.Max.y <= BoundingBox2.Max.y && BoundingBox1.Max.z <= BoundingBox2.Max.z
		&& BoundingBox1.Max.x >= BoundingBox2.Min.x && BoundingBox1.Max.y >= BoundingBox2.Min.y && BoundingBox1.Max.z >= BoundingBox2.Min.z)
		|| 
		(BoundingBox2.Max.x <= BoundingBox1.Max.x && BoundingBox2.Max.y <= BoundingBox1.Max.y && BoundingBox2.Max.z <= BoundingBox1.Max.z
		&& BoundingBox2.Max.x >= BoundingBox1.Min.x && BoundingBox2.Max.y >= BoundingBox1.Min.y && BoundingBox2.Max.z >= BoundingBox1.Min.z);
}

bool ZEAABoundingBox::CollisionTest(const ZEAABoundingBox& BoundingBox, const ZEBoundingSphere& BoundingSphere)
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
