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

inline void ZELineSegment::Create(ZELine& Line, const ZEVector3& Start, const ZEVector3& End)
{
	ZEVector3::Sub(Line.v, End, Start);
	Line.p = Start;
}

void ZELineSegment::CreateParametric(ZELine& Line, const ZEVector3& v, const ZEVector3& p)
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

void ZELineSegment::GetSegmentStartPoint(ZEVector3& StartPoint) const
{
	StartPoint = p;
}

void ZELineSegment::GetSegmentEndPoint(ZEVector3& EndPoint) const
{
	ZEVector3::Add(EndPoint, p, v);
}

float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZEVector3& Point)
{
	float TLineSegment;
	return ZELineSegment::MinimumDistance(LineSegment, Point);
}	

float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZEVector3& Point, float& TLineSegment)
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

    TLineSegment = c1 / c2;

	LineSegment.GetPointOn(w, TLineSegment);
    return ZEVector3::Distance(Point, w);
}		

float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZELine& Line)
{
	return ZELine::MinimumDistance(Line, LineSegment);
}

float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZELine& Line, float& TLineSegment, float& TLine)
{
	return ZELine::MinimumDistance(Line, LineSegment, TLine, TLineSegment);
}

float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegmentA, const ZELineSegment& LineSegmentB)
{
	float TLineSegmentA, TLineSegmentB;
	return ZELineSegment::MinimumDistance(LineSegmentA, LineSegmentB, TLineSegmentA, TLineSegmentB);
}

float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegmentA, const ZELineSegment& LineSegmentB, float& TLineSegmentA, float &TLineSegmentB)
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

	if (D < ZE_ZERO_TRESHOLD) 
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
        else
		{
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
        else
		{
            sN = (-d + b);
            sD = a;
        }
    }

    TLineSegmentA = (fabs(sN) < ZE_ZERO_TRESHOLD ? 0.0f : sN / sD);
    TLineSegmentB = (fabs(tN) < ZE_ZERO_TRESHOLD ? 0.0f : tN / tD);

	ZEVector3 P1, P2;
	ZEVector3::Scale(P1, LineSegmentA.v, TLineSegmentA);
	ZEVector3::Add(P1, P1, w);
	ZEVector3::Scale(P2, LineSegmentB.v, TLineSegmentB);
	ZEVector3::Sub(w, P1, P2);
	return ZEVector3::Length(w);
}

float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZERay& Ray)
{
	float TLineSegment, TRay;
	return ZELineSegment::MinimumDistance(LineSegment, Ray, TLineSegment, TRay);
}

float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZERay& Ray, float& TLine, float& TRay)
{
	// NOT IMPLAMENTED
	return 0;
}

void ZELineSegment::GetPointOn(ZEVector3& Point, float TLineSegment) const
{
	if (TLineSegment < 0.0f)
		Point = p;
	else if (TLineSegment > 1.0f)
		ZEVector3::Add(Point, p, v);
	else
	{
		ZEVector3::Scale(Point, v, TLineSegment);
		ZEVector3::Add(Point, Point, p);
	}
}

ZEVector3 ZELineSegment::GetPointOn(float TLineSegment) const
{
	ZEVector3 Temp;
	if (TLineSegment < 0.0f)
		Temp = p;
	else if (TLineSegment > 1.0f)
		ZEVector3::Add(Temp, p, v);
	else
	{
		ZEVector3::Scale(Temp, v, TLineSegment);
		ZEVector3::Add(Temp, Temp, p);
	}

	return Temp;
}

ZELineSegment::ZELineSegment(const ZEVector3& Start, const ZEVector3& End)
{
	Create(*this, Start, End);
}

ZELineSegment::ZELineSegment()
{

}
