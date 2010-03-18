//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - OBoundingBox.cpp
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

#include <math.h>


ZEVector3 ZEOBoundingBox::GetVertex(unsigned char Index) const
{
	switch(Index)
	{
		case 0:
			return Position;
		case 1:
			return Position + U;
		case 2:
			return Position + V;
		case 3:
			return Position + U + V;
		case 4:
			return Position + N;
		case 5:
			return Position + N + U;
		case 6:
			return Position + N + V;
		case 7:
			return Position + N + U + V;
	}
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
	float factor;

	factor = (ZEVector3::DotProduct(BoundingBox.U,Point) - ZEVector3::DotProduct(BoundingBox.Position,BoundingBox.U)) / ZEVector3::DotProduct(BoundingBox.U,BoundingBox.U);
	if (factor < 0 || factor > 1) return 0;

	factor = (ZEVector3::DotProduct(BoundingBox.V,Point) - ZEVector3::DotProduct(BoundingBox.Position,BoundingBox.V)) / ZEVector3::DotProduct(BoundingBox.V,BoundingBox.V);
	if (factor < 0 || factor > 1) return 0;

	factor = (ZEVector3::DotProduct(BoundingBox.N,Point) - ZEVector3::DotProduct(BoundingBox.Position,BoundingBox.N)) / ZEVector3::DotProduct(BoundingBox.N,BoundingBox.N);
	if (factor < 0 || factor > 1) return 0;

	return true;
}

bool ZEOBoundingBox::IntersectionTest(const ZEOBoundingBox& BoundingBox, const ZELine& Line)
{
//	ZEASSERT(true, "Not implamented");
	return false;
}

bool ZEOBoundingBox::IntersectionTest(const ZEOBoundingBox& BoundingBox, const ZERay& Ray)
{
//	ZEASSERT(true, "Not implamented");
	return false;
}

bool ZEOBoundingBox::IntersectionTest(const ZEOBoundingBox& BoundingBox, const ZELineSegment& LineSegment)
{
//	ZEASSERT(true, "Not implamented");
	return false;
}

ZEOBoundingBox::ZEOBoundingBox()
{
}

ZEOBoundingBox::ZEOBoundingBox(const ZEVector3 Position, const ZEVector3 U, const ZEVector3 V,const ZEVector3 N)
{
	this->Position = Position;
	this->U = U;
	this->V = V;
	this->N = N;
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
