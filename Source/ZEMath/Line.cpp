//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Line.cpp
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
#include "Line.h"
#include "Vector.h"
#include "Plane.h"
#include <d3dx9.h>

void ZELine::Create(ZELine& Line, const ZEPoint3& P0, const ZEPoint3& P1)
{
	ZEVector3::Sub(Line.v, P1, P0);
	Line.p = P0;

}

void ZELine::CreateParametric(ZELine& Line, const ZEVector3& v, const ZEPoint3& p)
{
	Line.v = v;
	Line.p = p;
}

float ZELine::MinimumDistance(const ZELine& LineA, const ZELine& LineB, float& tA, float &tB)
{
	ZEVector3 w(LineA.p, LineB.p);
	float    a = ZEVector3::DotProduct(LineA.v, LineA.v);
    float    b = ZEVector3::DotProduct(LineA.v, LineB.v);
    float    c = ZEVector3::DotProduct(LineB.v, LineB.v);
    float    d = ZEVector3::DotProduct(LineA.v, w);
    float    e = ZEVector3::DotProduct(LineB.v, w);
    float    D = a*c - b*b;

	if (D < ZE_ZEROTRESHOLD) 
	{
        tA = 0.0;
        tB = (b>c ? d/b : e/c);
    }
    else 
	{
        tA = (b*e - c*d) / D;
        tB = (a*e - b*d) / D;
    }

	ZEVector3 P1, P2;
	ZEVector3::Scale(P1, LineA.v, tA);
	ZEVector3::Add(P1, P1, w);
	ZEVector3::Scale(P2, LineB.v, tB);
	ZEVector3::Sub(w, P1, P2);
	return ZEVector3::Length(w);
}

float ZELine::DistanceToPoint(const ZELine& Line, const ZEPoint3& Point, float &t)
{
	ZEVector3 Temp;
	ZEVector3::Sub(Temp, Point, Line.p);

	t = ZEVector3::DotProduct(Line.v, Temp) / ZEVector3::DotProduct(Line.v, Line.v);
	
	ZEVector3::Scale(Temp, Line.v, t);
	ZEVector3::Add(Temp, Temp, Point);
	return ZEVector3::Distance(Point, Temp);
}

void ZELine::GetPointOn(ZEPoint3& Point, float t) const
{
	ZEVector3::Scale(Point, v, t);
	ZEVector3::Add(Point, Point, p);
}

ZELine::ZELine()
{
}

ZELine::ZELine(const ZEPoint3& v, const ZEPoint3& p)
{
	this->v = v;
	this->p = p;
}
