//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPlane.h
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

#pragma once
#ifndef	__ZE_MATH_PLANE_H__
#define __ZE_MATH_PLANE_H__

#include "ZEVector.h"
#include "ZELine.h"

enum ZEPlaneIntersect
{
	ZE_PI_NO_INTERSECT	= 0,
	ZE_PI_PARALLEL		= 1,
	ZE_PI_ON_PLANE		= 2,
	ZE_PI_INTERSECTS	= 3,
};
enum ZEHalfSpace
{
	ZE_HS_NEGATIVE_SIDE = -1,
	ZE_HS_ON_PLANE		 =  0,
	ZE_HS_POSITIVE_SIDE =  1,
	ZE_HS_INTERSECTS	 =  2,
};
class ZELine;
class ZELineSegment;
class ZERay;
class ZEPlane
{
	public:
		ZEVector3				p;
		ZEVector3				n;

		ZEVector4				ToABCD() const;

								ZEPlane();
								ZEPlane(const ZEVector3& n, const ZEVector3& p);

		static bool				IntersectionTest(const ZEPlane& Plane, const ZELine& Line, float &t);
		static bool				IntersectionTest(const ZEPlane& Plane, const ZELineSegment& LineSegment, float &t);
		static bool				IntersectionTest(const ZEPlane& Plane, const ZERay& Ray, float &t);
		static bool				IntersectionTest(const ZEPlane & Plane1, const ZEPlane & Plane2, ZELine & Line);

		static ZEHalfSpace		TestHalfSpace(const ZEPlane& Plane, const ZEVector3& Point);
		static float			DistanceSigned(const ZEPlane& Plane, const ZEVector3& Point);
		static float			Distance(const ZEPlane& Plane, const ZEVector3& Point);

		static void				Create(ZEPlane& Plane, const ZEVector3& n, const ZEVector3& p);
		static void				Create(ZEPlane& Plane,const ZEVector3 &P1,const ZEVector3 &P2, const ZEVector3 &P3);

};

#endif
