//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELineSegment.cpp
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

#include "ZELineSegment.h"

#include "ZEError.h"
#include "ZEMath.h"
#include "ZERay.h"
#include "ZEVector.h"
#include "ZEPlane.h"
#include "ZEBSphere.h"
#include "ZEAABBox.h"
#include "ZEOBBox.h"


// ZELineSegment
//////////////////////////////////////////////////////////////////////////////////////

void ZELineSegment::Create(ZELineSegment& LineSegment, const ZEVector3& Start, const ZEVector3& End)
{
	ZEVector3::Sub(LineSegment.v, End, Start);
	LineSegment.p = Start;
}

void ZELineSegment::CreateParametric(ZELineSegment& LineSegment, const ZEVector3& v, const ZEVector3& p)
{
	LineSegment.v = v;
	LineSegment.p = p;
}

float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZEVector3& Point)
{
	float TLineSegment;
	return ZELineSegment::MinimumDistance(LineSegment, Point, TLineSegment);
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
		ZELineSegment::GetEndPoint(w, LineSegment);
		return ZEVector3::Distance(Point, w);
	}

    TLineSegment = c1 / c2;

	ZELineSegment::GetPointOn(w, LineSegment, TLineSegment);
    return ZEVector3::Distance(Point, w);
}		

float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZELine& Line)
{
	float TLineSegment, TLine;
	return ZELineSegment::MinimumDistance(LineSegment, Line, TLineSegment, TLine);
}

float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZELine& Line, float& TLineSegment, float& TLine)
{
	ZEVector3 w(Line.p, LineSegment.p);

	float a = ZEVector3::DotProduct(LineSegment.v, LineSegment.v);
    float b = ZEVector3::DotProduct(LineSegment.v, Line.v);
    float c = ZEVector3::DotProduct(Line.v, Line.v);
	float d = ZEVector3::DotProduct(LineSegment.v, w);
    float e = ZEVector3::DotProduct(Line.v, w);
	float D = a * c - b * b ;
    float sN, sD = D;
    float tN, tD = D;

	if (D < ZE_ZERO_THRESHOLD) 
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

    TLineSegment  = (ZEMath::Abs(sN) < ZE_ZERO_THRESHOLD ? 0.0f : sN / sD);
    TLine = (ZEMath::Abs(tN) < ZE_ZERO_THRESHOLD ? 0.0f : tN / tD);

	ZEVector3 P1, P2;
	ZEVector3::Scale(P1, LineSegment.v, TLineSegment);
	ZEVector3::Add(P1, P1, LineSegment.p);
	ZEVector3::Scale(P2, Line.v, TLine);
	ZEVector3::Add(P2, P2, Line.p);
	ZEVector3::Sub(w, P1, P2);
	return ZEVector3::Length(w);
}

float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegmentA, const ZELineSegment& LineSegmentB)
{
	float TLineSegmentA, TLineSegmentB;
	return ZELineSegment::MinimumDistance(LineSegmentA, LineSegmentB, TLineSegmentA, TLineSegmentB);
}

float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegmentA, const ZELineSegment& LineSegmentB, float& TLineSegmentA, float &TLineSegmentB)
{
	ZEVector3 w(LineSegmentA.p, LineSegmentB.p);
	float a = ZEVector3::DotProduct(LineSegmentA.v, LineSegmentA.v);
	float b = ZEVector3::DotProduct(LineSegmentA.v, LineSegmentB.v);
	float c = ZEVector3::DotProduct(LineSegmentB.v, LineSegmentB.v);
	float d = ZEVector3::DotProduct(LineSegmentA.v, w);
	float e = ZEVector3::DotProduct(LineSegmentB.v, w);
	float D = a*c - b*b;
	float sN, sD = D;
	float tN, tD = D;

	if (D < ZE_ZERO_THRESHOLD) 
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

    TLineSegmentA = (ZEMath::Abs(sN) < ZE_ZERO_THRESHOLD ? 0.0f : sN / sD);
    TLineSegmentB = (ZEMath::Abs(tN) < ZE_ZERO_THRESHOLD ? 0.0f : tN / tD);

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

float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZERay& Ray, float& TLineSegment, float& TRay)
{
	ZEVector3 w(LineSegment.p, Ray.p);
	float a = ZEVector3::DotProduct(LineSegment.v, LineSegment.v);
	float b = ZEVector3::DotProduct(LineSegment.v, Ray.v);
	float c = ZEVector3::DotProduct(Ray.v, Ray.v);
	float d = ZEVector3::DotProduct(LineSegment.v, w);
	float e = ZEVector3::DotProduct(Ray.v, w);
	float D = a*c - b*b;
	float sN, sD = D;
	float tN, tD = D;

	if (D < ZE_ZERO_THRESHOLD) 
	{
        sN = 0.0; 
        sD = 1.0;      
        tN = e;
        tD = c;
    }
    else 
	{
        sN = -(b*e - c*d);
        tN = -(a*e - b*d);
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

    TLineSegment = (ZEMath::Abs(sN) < ZE_ZERO_THRESHOLD ? 0.0f : sN / sD);
    TRay = (ZEMath::Abs(tN) < ZE_ZERO_THRESHOLD ? 0.0f : tN / tD);

	return ZEVector3::Length(ZEVector3(LineSegment.GetPointOn(TLineSegment), Ray.GetPointOn(TRay)));
}

void ZELineSegment::GetPointOn(ZEVector3& Point, const ZELineSegment& LineSegment, float TLineSegment)
{
	if (TLineSegment < 0.0f)
	{
		Point = LineSegment.p;
	}
	else if (TLineSegment * TLineSegment> LineSegment.v.LengthSquare())
	{
		ZEVector3::Add(Point, LineSegment.p, LineSegment.v);
	}
	else
	{
		ZEVector3::Scale(Point, LineSegment.v, TLineSegment);
		ZEVector3::Add(Point, Point, LineSegment.p);
	}
}

void ZELineSegment::GetStartPoint(ZEVector3& StartPoint, const ZELineSegment& LineSegment)
{
	StartPoint = LineSegment.p;
}

void ZELineSegment::GetEndPoint(ZEVector3& EndPoint, const ZELineSegment& LineSegment)
{
	ZEVector3::Add(EndPoint, LineSegment.p, LineSegment.v);
}

ZEVector3 ZELineSegment::GetDirection() const
{
	return v.Normalize();
}

float ZELineSegment::GetLength() const
{
	return v.Length();
}

float ZELineSegment::GetLengthSquare() const
{
	return v.LengthSquare();
}

ZEVector3 ZELineSegment::GetPointOn(float TLineSegment) const
{
	ZEVector3 Temp;
	if (TLineSegment < 0.0f)
	{
		Temp = p;
	}
	else if (TLineSegment * TLineSegment > v.LengthSquare())
	{
		ZEVector3::Add(Temp, p, v);
	}
	else
	{
		ZEVector3::Scale(Temp, v, TLineSegment);
		ZEVector3::Add(Temp, Temp, p);
	}

	return Temp;
}

const ZEVector3& ZELineSegment::GetStartPoint() const
{
	return p;
}

ZEVector3 ZELineSegment::GetEndPoint() const
{
	return p + v;
}

ZELineSegment::ZELineSegment(const ZEVector3& Start, const ZEVector3& End)
{
	Create(*this, Start, End);
}

ZELineSegment::ZELineSegment()
{

}


// ZEViewLineSegment
//////////////////////////////////////////////////////////////////////////////////////

ZEViewVolumeType ZEViewLineSegment::GetViewVolumeType() const
{
	return ZE_VVT_LINE;
}

bool ZEViewLineSegment::IntersectionTest(const ZEBSphere& BoundingSphere) const
{
	return ZEBSphere::IntersectionTest(BoundingSphere, *this);
}

bool ZEViewLineSegment::IntersectionTest(const ZEAABBox& BoundingBox) const
{
	return ZEAABBox::IntersectionTest(BoundingBox, *this);
}

bool ZEViewLineSegment::IntersectionTest(const ZEOBBox& BoundingBox) const
{
	return ZEOBBox::IntersectionTest(BoundingBox, *this);
}

bool ZEViewLineSegment::IntersectionTest(const ZERectangle3D& Rectangle) const
{
	zeDebugCheck(true, "NOT IMPLEMENTED");
	return false;
}

ZEViewLineSegment::ZEViewLineSegment(const ZEVector3& P0, const ZEVector3& P1)
{
	Create(*this, P0, P1);
}

ZEViewLineSegment::ZEViewLineSegment()
{

}
