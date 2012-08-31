//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEBSphere.h
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
#ifndef __ZE_MATH_B_SPHERE_H__
#define __ZE_MATH_B_SPHERE_H__

#include "ZEVector.h"
#include "ZEMatrix.h"
#include "ZEPlane.h"

class ZEOBBox;
class ZEAABBox;
class ZEBSphere
{
	public:
		ZEVector3				Position;
		float					Radius;

		static void				GetSurfaceNormal(ZEVector3& Normal, const ZEBSphere& BoundingSphere, const ZEVector3& Point);

		static bool				IntersectionTest(const ZEBSphere& BoundingSphere, const ZEVector3& Point);
		static ZEHalfSpace		IntersectionTest(const ZEBSphere& BoundingSphere, const ZEPlane& Plane);

		static bool				IntersectionTest(const ZEBSphere& BoundingSphere, const ZELine& Line);
		static bool				IntersectionTest(const ZEBSphere& BoundingSphere, const ZELine& Line, float& MinT);
		static bool				IntersectionTest(const ZEBSphere& BoundingSphere, const ZELine& Line, float& MinT, float& MaxT);

		static bool				IntersectionTest(const ZEBSphere& BoundingSphere, const ZELineSegment& LineSegment);
		static bool				IntersectionTest(const ZEBSphere& BoundingSphere, const ZELineSegment& LineSegment, float& MinT);
		static bool				IntersectionTest(const ZEBSphere& BoundingSphere, const ZELineSegment& LineSegment, float& MinT, float& MaxT);

		static bool				IntersectionTest(const ZEBSphere& BoundingSphere, const ZERay& Ray);
		static bool				IntersectionTest(const ZEBSphere& BoundingSphere, const ZERay& Ray, float& MinT);
		static bool				IntersectionTest(const ZEBSphere& BoundingSphere, const ZERay& Ray, float& MinT, float& MaxT);

		static bool				IntersectionTest(const ZEBSphere& BoundingSphere, const ZEAABBox& BoundingBox);
		static bool				IntersectionTest(const ZEBSphere& BoundingSphere, const ZEOBBox& BoundingBox);
		static bool				IntersectionTest(const ZEBSphere& BoundingSphere1, const ZEBSphere& BoundingSphere2);

								ZEBSphere();
								ZEBSphere(const ZEVector3& Position, float Radius);
};
#endif
