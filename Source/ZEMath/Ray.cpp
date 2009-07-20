//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Ray.cpp
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
#include "Ray.h"
#include "Vector.h"
#include "Plane.h"
#include <d3dx9.h>

void ZERay::Create(ZERay & Ray, const ZEPoint3 & Start,const ZEPoint3 & End)
{
	ZELine::Create(Ray,Start,End);
}

void ZERay::CreateParametric(ZERay & Ray,const ZEVector3 & v,const ZEVector3 & p)
{
	Ray.v = v;
	Ray.p = p;
}

float ZERay::DistanceToPoint(const ZERay& Ray, const ZEPoint3& Point, float &t)
{
	ZEVector3 temp;
	ZEVector3::Sub(temp,Point,Ray.p);

	t = ZEVector3::DotProduct(temp,Ray.v);
	if (t <= 0)
	{
		t = 0;
		return ZEVector3::Distance(Point,Ray.p);
	}

	ZEVector3::Scale(temp,Ray.v,t);
	ZEVector3::Add(temp,Ray.p,temp);
	return ZEVector3::Distance(temp, Point);

}

void ZERay::GetPointOn(ZEPoint3& Point, float t) const
{
	ZELine::GetPointOn(Point,t);
}

ZERay::ZERay(const ZEPoint3 & n,const ZEPoint3 &p)
{
	this->v = n;
	this->p = p;
}

ZERay::ZERay() 
{
}

