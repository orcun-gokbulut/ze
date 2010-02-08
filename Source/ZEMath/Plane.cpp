//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Plane.cpp
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

#include "Plane.h"
#include "Definitions.h"
#include "Line.h"
#include "LineSegment.h"
#include "Ray.h"

#include <math.h>

bool ZEPlane::IntersectionTest(const ZEPlane& Plane, const ZELine& Line, float &t)
{	
	ZEVector3 w;
	ZEVector3::Sub(w, Line.p, Plane.p); 

	float D = ZEVector3::DotProduct(Plane.n, Line.v);
	float N = -ZEVector3::DotProduct(Plane.n, w);

	if (fabs(D) < ZE_ZEROTRESHOLD) 
	{
		return false;
       /* if (N == 0)
            return ZEPLANEINTERSECT_ONPLANE;
        else
            return ZEPLANEINTERSECT_PARALEL;*/
    }

    t = N / D;
	return true;
}

bool ZEPlane::IntersectionTest(const ZEPlane& Plane, const ZELineSegment& LineSegment, float &t)
{	
	ZEVector3 w;
	ZEVector3::Sub(w, LineSegment.p, Plane.p); 


	float D = ZEVector3::DotProduct(Plane.n, LineSegment.v);
	float N = -ZEVector3::DotProduct(Plane.n, w);

	if (fabs(D) < ZE_ZEROTRESHOLD) 
	{
       /* if (N == 0)
            return ZEPLANEINTERSECT_ONPLANE;
        else
            return ZEPLANEINTERSECT_PARALEL;*/
		return false;
    }

    t = N / D;
    if (t < 0 || t > 1)
        return true;

	return false;
}

bool ZEPlane::IntersectionTest(const ZEPlane& Plane, const ZERay& Ray, float &t)
{	
	ZEVector3 w;
	ZEVector3::Sub(w, Ray.p, Plane.p); 


	float D = ZEVector3::DotProduct(Plane.n, Ray.v);
	float N = -ZEVector3::DotProduct(Plane.n, w);

	if (fabs(D) < ZE_ZEROTRESHOLD) 
	{
       /* if (N == 0)
            return ZEPLANEINTERSECT_ONPLANE;
        else
            return ZEPLANEINTERSECT_PARALEL;*/
		return false;
    }

    t = N / D;
    if (t > 0)
        return true;

	return false;
}

bool ZEPlane::IntersectionTest(const ZEPlane & Plane1, const ZEPlane & Plane2, ZELine & Line)
{
	ZEVector3::CrossProduct(Line.v,Plane1.n,Plane2.n);
	if (Line.v.x == 0 && Line.v.y == 0 && Line.v.z == 0) 
		return false;

	ZEVector3 temp;
	ZEVector3::CrossProduct(temp,Line.v,Plane2.n);

	float dots = ZEVector3::DotProduct(Plane1.p,Plane1.n) - ZEVector3::DotProduct(Plane2.p,Plane1.n);
	dots = dots / ZEVector3::DotProduct(temp,Plane1.n);

	ZEVector3::Scale(Line.p,temp,dots);
	ZEVector3::Add(Line.p,Line.p,Plane2.p);

	return true;
}

ZEHalfSpace ZEPlane::TestHalfSpace(const ZEPlane& Plane, const ZEVector3& Point)
{
	ZEVector3 Temp;
	ZEVector3::Sub(Temp, Point, Plane.p);
	ZEVector3::Multiply(Temp, Plane.n, Temp);
	
	float D = Temp.x + Temp.y + Temp.z;

	if (fabs(D) < ZE_ZEROTRESHOLD)
		return ZEHALFSPACE_ONPLANE;
	else if (D > ZE_ZEROTRESHOLD)
		return ZEHALFSPACE_POSITIVESIDE;
	else
		return ZEHALFSPACE_NEGATIVESIDE;
}

float ZEPlane::Distance(const ZEPlane& Plane, const ZEVector3& Point)
{
	ZEVector3 Temp;
	ZEVector3::Sub(Temp, Point , Plane.p);

	float t = -ZEVector3::DotProduct(Plane.n, Temp) / ZEVector3::DotProduct(Plane.n, Plane.n);

	ZEVector3::Scale(Temp, Plane.n, t);
	ZEVector3::Add(Temp, Temp, Point);
	return fabs(ZEVector3::Distance(Point, Temp));
}

inline void ZEPlane::Create(ZEPlane& Plane, const ZEVector3& n, const ZEVector3& p)
{
	Plane.n = n;
	Plane.p = p;
}

void ZEPlane::Create(ZEPlane& Plane,const ZEVector3 &P1,const ZEVector3 &P2,const ZEVector3 &P3)
{
	ZEVector3 Sub1,Sub2;
	ZEVector3::Sub(Sub1,P2,P1);
	ZEVector3::Sub(Sub2,P3,P1);
	ZEVector3::CrossProduct(Plane.n,Sub1,Sub2);
	Plane.p = P3;
}

ZEPlane::ZEPlane(const ZEVector3& n, const ZEVector3& p)
{
	Create(*this, n, p);
}

ZEPlane::ZEPlane()
{
}
