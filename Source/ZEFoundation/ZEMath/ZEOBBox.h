//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEOBBox.h
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
#ifndef __ZE_MATH_O_BOUNDING_BOX_H__
#define __ZE_MATH_O_BOUNDING_BOX_H__

#include "ZETypes.h"
#include "ZEVector.h"
#include "ZEMatrix.h"
#include "ZEPlane.h"

class ZEBSphere;
class ZEAABBox;

class ZEOBBox
{
	public:
		ZEVector3				Center;
		ZEVector3				Right, Up, Front;
		ZEVector3				HalfSize;

		ZEVector3				GetVertex(ZEUInt Index) const;

		static void				CreateFromOrientation(ZEOBBox& BoundingBox, const ZEVector3& Position, const ZEQuaternion& Rotation, const ZEVector3& Size);
		static void				ConvertToSphere(ZEBSphere& Sphere, const ZEOBBox& Input);

		static void				Transform(ZEOBBox& Output, const ZEMatrix4x4& Matrix, const ZEOBBox& Input);

		static ZEHalfSpace		IntersectionTest(const ZEOBBox& BoundingBox, const ZEPlane& Plane);

		static bool				IntersectionTest(const ZEOBBox& BoundingBox, const ZEVector3& Point);

		static bool				IntersectionTest(const ZEOBBox& BoundingBox, const ZELine& Line);
		static bool				IntersectionTest(const ZEOBBox& BoundingBox, const ZELine& Line, float& TMin);
		static bool				IntersectionTest(const ZEOBBox& BoundingBox, const ZELine& Line, float& TMin, float& TMax);

		static bool				IntersectionTest(const ZEOBBox& BoundingBox, const ZERay& Ray);
		static bool				IntersectionTest(const ZEOBBox& BoundingBox, const ZERay& Ray, float& TMin);
		static bool				IntersectionTest(const ZEOBBox& BoundingBox, const ZERay& Ray, float& TMin, float& TMax);

		static bool				IntersectionTest(const ZEOBBox& BoundingBox, const ZELineSegment& LineSegment);
		static bool				IntersectionTest(const ZEOBBox& BoundingBox, const ZELineSegment& LineSegment, float& TMin);
		static bool				IntersectionTest(const ZEOBBox& BoundingBox, const ZELineSegment& LineSegment, float& TMin, float& TMax);

		static bool				IntersectionTest(const ZEOBBox& BoundingBox, const ZEBSphere& BoundingSphere);
		static bool				IntersectionTest(const ZEOBBox& BoundingBox1, const ZEAABBox& BoundingBox2);
		static bool				IntersectionTest(const ZEOBBox& BoundingBox1, const ZEOBBox& BoundingBox2);

								ZEOBBox();
								ZEOBBox(const ZEVector3& Position, const ZEQuaternion& Rotation, const ZEVector3& Size);
								ZEOBBox(const ZEVector3& Center, const ZEVector3& Right, const ZEVector3& Up,const ZEVector3& Direction, const ZEVector3& HalfSize);
};
#endif




