//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERectangle3D.cpp
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

#include "ZERectangle3D.h"
//#include "Core/Error.h"

void ZERectangle3D::GetPlane(ZEPlane & Plane) const
{
	ZEPlane::Create(Plane,P1,P2,P3);
}

const ZEVector3& ZERectangle3D::GetPoint(ZEUInt Index) const
{
	switch(Index)
	{
		case 0:
			return P1;
		case 1:
			return P2;
		case 2:
			return P3;
		case 3:
			return P4;
		default:
			return P1;
	}
}

ZELine ZERectangle3D::GetBorderLine(ZEUInt Index) const
{
	switch(Index)
	{
		default:
		case 0:
			return ZELine(P1, P2);
		case 1:
			return ZELine(P2, P3);
		case 2:
			return ZELine(P3, P4);
		case 3:
			return ZELine(P4, P1);

	}
}

ZELineSegment ZERectangle3D::GetBorder(ZEUInt Index) const
{
	switch(Index)
	{
		default:
		case 0:
			return ZELineSegment(P1, P2);
		case 1:
			return ZELineSegment(P2, P3);
		case 2:
			return ZELineSegment(P3, P4);
		case 3:
			return ZELineSegment(P4, P1);
	}
}

ZEHalfSpace ZERectangle3D::IntersectionTest(const ZERectangle3D& Rectangle, const ZEPlane& Plane)
{
	ZEHalfSpace Result1 = ZEPlane::TestHalfSpace(Plane, Rectangle.P1);
	ZEHalfSpace Result2 = ZEPlane::TestHalfSpace(Plane, Rectangle.P2);
	ZEHalfSpace Result3 = ZEPlane::TestHalfSpace(Plane, Rectangle.P3);
	ZEHalfSpace Result4 = ZEPlane::TestHalfSpace(Plane, Rectangle.P4);

	if (Result1 == Result2 && Result1 == Result3 && Result1 == Result4)
		return Result1;
	else
		return ZE_HS_INTERSECTS;
}

void ZERectangle3D::IntersectionTest(ZEArray<ZEVector3>& IntersectedPoints, const ZEPlane& Plane, const ZEArray<ZEVector3>& Points)
{
	IntersectedPoints.Clear();
	if (Points.GetCount() == 0)
		return;

	bool CurrentResult = ZEPlane::TestHalfSpace(Plane, Points[0]) != ZE_HS_NEGATIVE_SIDE;
	for (size_t I = 0; I < Points.GetCount(); I++)
	{
		bool NextResult = ZEPlane::TestHalfSpace(Plane, Points[Points.Circular(I + 1)]) != ZE_HS_NEGATIVE_SIDE;
		if (CurrentResult)
		{
			IntersectedPoints.Add(Points[Points.Circular(I)]);

			if (!NextResult)
			{
				ZELineSegment Edge(Points[Points.Circular(I)], Points[Points.Circular(I + 1)]);
				float t;
				ZEPlane::IntersectionTest(Plane, Edge, t);
				IntersectedPoints.Add(Edge.GetPointOn(t));
			}
		}
		else if (!CurrentResult && NextResult)
		{
			ZELineSegment Edge(Points[Points.Circular(I)], Points[Points.Circular(I + 1)]);
			float t;
			ZEPlane::IntersectionTest(Plane, Edge, t);
			IntersectedPoints.Add(Edge.GetPointOn(t));
			IntersectedPoints.Add(Points[Points.Circular(I + 1)]);
		}

		CurrentResult = NextResult;
	}
}
		
ZERectangle3D::ZERectangle3D()
{
}

ZERectangle3D::ZERectangle3D(const ZEVector3& P1, const ZEVector3& P2, const ZEVector3& P3, const ZEVector3& P4)
{
	this->P1 = P1;
	this->P2 = P2;
	this->P3 = P3;
	this->P4 = P4;
}
