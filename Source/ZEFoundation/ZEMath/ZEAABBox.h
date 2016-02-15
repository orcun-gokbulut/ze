//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEAABBox.h
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
#ifndef __ZE_MATH_AA_BOUNDING_BOX_H__
#define __ZE_MATH_AA_BOUNDING_BOX_H__

#include "ZETypes.h"
#include "ZEVector.h"
#include "ZEMatrix.h"
#include "ZEPlane.h"

class ZEBSphere;
class ZEOBBox;

class ZEAABBox
{
	public:
		ZEVector3					Min, Max;

		ZEVector3					GetCenter() const;
		ZEVector3					GetVertex(ZEUInt Index) const;
		float						GetLength() const;

		static void					GenerateBoundingSphere(ZEBSphere& BoundingSphere, const ZEAABBox& BoundingBox);
		static void					GenerateOBoundingBox(ZEOBBox& OrientedBoundingBox, const ZEAABBox& BoundingBox);

		static void					Transform(ZEAABBox& Output, const ZEAABBox& Input, const ZEMatrix4x4& TransformMatrix);

		static ZEHalfSpace			IntersectionTest(const ZEAABBox& BoundingBox, const ZEPlane& Plane);

		static bool					IntersectionTest(const ZEAABBox& BoundingBox, const ZEVector3& Point);
		static bool					IntersectionTest(const ZEAABBox& BoundingBox, const ZELine& Line);
		static bool					IntersectionTest(const ZEAABBox& BoundingBox, const ZELine& Line, float& TMin);
		static bool					IntersectionTest(const ZEAABBox& BoundingBox, const ZELine& Line, float& TMin, float& TMax);
		static bool					IntersectionTest(const ZEAABBox& BoundingBox, const ZERay& Ray);
		static bool					IntersectionTest(const ZEAABBox& BoundingBox, const ZERay & Ray, float& TMin);
		static bool					IntersectionTest(const ZEAABBox& BoundingBox, const ZERay & Ray, float& TMin, float& TMax);
		static bool					IntersectionTest(const ZEAABBox& BoundingBox, const ZELineSegment& LineSegment);
		static bool					IntersectionTest(const ZEAABBox& BoundingBox, const ZELineSegment& LineSegment, float& TMin);
		static bool					IntersectionTest(const ZEAABBox& BoundingBox, const ZELineSegment& LineSegment, float& TMin, float& TMax);
		
		static bool					IntersectionTest(const ZEAABBox& BoundingBox1, const ZEOBBox& BoundingBox2);
		static bool					IntersectionTest(const ZEAABBox& BoundingBox1, const ZEAABBox& BoundingBox2);
		static bool					IntersectionTest(const ZEAABBox& BoundingBox, const ZEBSphere& BoundingSphere);

		static void					Generate(ZEAABBox& Output, const ZEVector3* Vertices, ZESize Count);
		static void					Combine(ZEAABBox& Output, const ZEAABBox& A, const ZEAABBox& B);

		static const ZEAABBox		Zero;

									ZEAABBox();
									ZEAABBox(const ZEVector3& Min, const ZEVector3& Max);

								
};
#endif
