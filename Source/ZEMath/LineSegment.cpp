//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - LineSegment.cpp
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

#include "Definitions.h"
#include "LineSegment.h"
#include "Vector.h"
#include "Plane.h"
#include <d3dx9.h>

inline void ZELineSegment::Create(ZELine& Line, const ZEPoint3& Start, const ZEPoint3& End)
{
	ZEVector3::Sub(Line.v, End, Start);
	Line.p = Start;
}

inline void ZELineSegment::CreateParametric(ZELine& Line, const ZEVector3& v, const ZEPoint3& p)
{
	Line.v = v;
	Line.p = p;
}

float ZELineSegment::Length(const ZELineSegment& LineSegment)
{
	return ZEVector3::Length(LineSegment.v);
}

float ZELineSegment::GetLenght() const
{
	return ZEVector3::Length(this->v);
}

void ZELineSegment::GetSegmentStartPoint(ZEPoint3& StartPoint) const
{
	StartPoint = p;
}

void ZELineSegment::GetSegmentEndPoint(ZEPoint3& EndPoint) const
{
	ZEVector3::Add(EndPoint, p, v);
}
		
float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegmentA, const ZELineSegment& LineSegmentB, float& tA, float &tB)
{
	ZEVector3 w(LineSegmentA.p, LineSegmentB.p);
	float    a = ZEVector3::DotProduct(LineSegmentA.v, LineSegmentA.v);
    float    b = ZEVector3::DotProduct(LineSegmentA.v, LineSegmentB.v);
    float    c = ZEVector3::DotProduct(LineSegmentB.v, LineSegmentB.v);
    float    d = ZEVector3::DotProduct(LineSegmentA.v, w);
    float    e = ZEVector3::DotProduct(LineSegmentB.v, w);
    float    D = a*c - b*b;
    float    sN, sD = D;
    float    tN, tD = D;

	if (D < ZE_ZEROTRESHOLD) 
	{
        sN = 0.0; 
        sD = 1.0;      
        tN = e;
        tD = c;
    }
    else 
	{
        sN = (b*e - c*d);
        tN = (a*e - b*d);
        if (sN < 0.0) 
		{
            sN = 0.0;
            tN = e;
            tD = c;
        }
        else if (sN > sD) 
		{
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }

    if (tN < 0.0) 
	{
        tN = 0.0;
        if (-d < 0.0)
            sN = 0.0;
        else if (-d > a)
            sN = sD;
        else {
            sN = -d;
            sD = a;
        }
    }
    else if (tN > tD) 
	{
        tN = tD;
        if ((-d + b) < 0.0)
            sN = 0;
        else if ((-d + b) > a)
            sN = sD;
        else {
            sN = (-d + b);
            sD = a;
        }
    }

    tA = (fabs(sN) < ZE_ZEROTRESHOLD ? 0.0f : sN / sD);
    tB = (fabs(tN) < ZE_ZEROTRESHOLD ? 0.0f : tN / tD);

	ZEVector3 P1, P2;
	ZEVector3::Scale(P1, LineSegmentA.v, tA);
	ZEVector3::Add(P1, P1, w);
	ZEVector3::Scale(P2, LineSegmentB.v, tB);
	ZEVector3::Sub(w, P1, P2);
	return ZEVector3::Length(w);
}

float ZELineSegment::DistanceToPoint(const ZELineSegment& LineSegment, const ZEPoint3& Point, float &t)
{
	ZEVector3 w;
	ZEVector3::Sub(w, Point, LineSegment.p);

	float c1 = ZEVector3::DotProduct(w, LineSegment.v);
    if (c1 <= 0)
		return ZEVector3::Distance(Point, LineSegment.p);

	float c2 = ZEVector3::DotProduct(LineSegment.v, LineSegment.v);
    if (c2 <= c1)
	{	
		LineSegment.GetSegmentEndPoint(w);
		return ZEVector3::Distance(Point, w);
	}

    t = c1 / c2;

	LineSegment.GetPointOn(w, t);
    return ZEVector3::Distance(Point, w);
}

void ZELineSegment::GetPointOn(ZEPoint3& Point, float t) const
{
	if (t < 0)
		Point = p;
	else if (t > 1)
		ZEVector3::Add(Point, p, v);
	else
	{
		ZEVector3::Scale(Point, v, t);
		ZEVector3::Add(Point, Point, p);
	}
}

ZELineSegment::ZELineSegment(const ZEPoint3& Start, const ZEPoint3& End)
{
	Create(*this, Start, End);
}

ZELineSegment::ZELineSegment()
{
}
