//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPolygon.cpp
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

#include "ZEPolygon.h"
#include "ZEMath/ZERay.h"
#include "ZEMath/ZELineSegment.h"
#include "ZEMath/ZEPlane.h"

bool ZEPolygon::IsValid() const
{
	return Vertices.GetCount() >= 3;
}

ZEVector3 ZEPolygon::GetCentroid() const
{
	return ZEVector3::Zero;
}

ZEVector3 ZEPolygon::GetNormal() const
{
	zeDebugCheck(!IsValid(), "Polygon is not valid.");

	ZEVector3 Normal;
	ZEVector3::CrossProduct(Normal, Vertices[1] - Vertices[0], Vertices.GetLastItem() - Vertices[0]);
	Normal.NormalizeSelf();

	return Normal;
}

float ZEPolygon::GetArea() const
{
	zeDebugCheck(!IsValid(), "Polygon is not valid.");

	ZEVector3 Sum;
	for(ZESize I = 0; I < Vertices.GetCount() - 1; I++)
	{
		ZEVector3 Temp;
		ZEVector3::CrossProduct(Temp, Vertices[I], Vertices[I + 1]);
		ZEVector3::Add(Sum, Sum, Temp);
	}

	ZEVector3 Temp;
	ZEVector3::CrossProduct(Temp, Vertices[Vertices.GetCount() - 1], Vertices[0]);
	ZEVector3::Add(Sum, Sum, Temp);

	return Vertices.GetCount() / 2 * ZEVector3::DotProduct(Sum, GetNormal());
}

ZEArray<ZELineSegment> ZEPolygon::GetEdges() const
{
	ZEArray<ZELineSegment> Edges;
	for (ZESize I = 1; I < Vertices.GetCount(); I++)
		Edges.Add(ZELineSegment(Vertices[I - 1], Vertices[I]));

	return Edges;
}

bool ZEPolygon::IntersectionTest(const ZEPolygon& Polygon, const ZEVector3& Point)
{

	ZEVector3 Normal = Polygon.GetNormal();

	for (ZESize I = 0; I < Polygon.Vertices.GetCount(); I++)
	{
		ZEVector3 EdgeNormal;
		ZEVector3::CrossProduct(EdgeNormal, Normal, (Polygon.Vertices[Polygon.Vertices.Circular(I + 1)] - Polygon.Vertices[I]));
		if (ZEVector3::DotProduct(Point - Polygon.Vertices[I], EdgeNormal) < 0)
			return false;
	}
	
	return true;
}

bool ZEPolygon::IntersectionTest(const ZEPolygon& Polygon, const ZELine& Line)
{
	float Temp;
	return IntersectionTest(Polygon, Line, Temp);
}

bool ZEPolygon::IntersectionTest(const ZEPolygon& Polygon, const ZELine& Line, float& t)
{
	float tt;
	if (ZEPlane::IntersectionTest(ZEPlane(Polygon.GetNormal(), Polygon.Vertices[0]), Line, tt))
		if (IntersectionTest(Polygon, Line.GetPointOn(t)))
		{
			t = tt;
			return true;
		}

	return false;
}

bool ZEPolygon::IntersectionTest(const ZEPolygon& Polygon, const ZERay& Ray)
{
	float Temp;
	return IntersectionTest(Polygon, Ray, Temp);
}

bool ZEPolygon::IntersectionTest(const ZEPolygon& Polygon, const ZERay& Ray, float& t)
{
	float tt;
	if (ZEPlane::IntersectionTest(ZEPlane(Polygon.GetNormal(), Polygon.Vertices[0]), Ray, tt))
		if (IntersectionTest(Polygon, Ray.GetPointOn(t)))
		{
			t = tt;
			return true;
		}

	return false;
}

bool ZEPolygon::IntersectionTest(const ZEPolygon& Polygon, const ZELineSegment& LineSegment)
{
	float Temp;
	return IntersectionTest(Polygon, LineSegment, Temp);
}

bool ZEPolygon::IntersectionTest(const ZEPolygon& Polygon, const ZELineSegment& LineSegment, float& t)
{
	float tt;
	if (ZEPlane::IntersectionTest(ZEPlane(Polygon.GetNormal(), Polygon.Vertices[0]), LineSegment, tt))
		if (IntersectionTest(Polygon, LineSegment.GetPointOn(t)))
		{
			t = tt;
			return true;
		}

	return false;
}
