//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEAABoundingBox.h
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
#ifndef __ZE_MATH_AABOUNDINGBOX_H__
#define __ZE_MATH_AABOUNDINGBOX_H__
#include "ZEVector.h"
#include "ZEMatrix.h"
#include "ZEPlane.h"

class ZEBoundingSphere;
class ZEOBoundingBox;
class ZEAABoundingBox
{
public:
	ZEVector3					Min, Max;

	ZEVector3					GetCenter() const;
	ZEVector3					GetVertex(unsigned char Index) const;
	float						GetLenght() const;


	void						GenerateBoundingSphere(ZEBoundingSphere& BoundingSphere) const;
	void						GenerateBoundingSphere(ZEBoundingSphere& BoundingSphere, const ZEMatrix4x4& Transform) const;
	void						GenerateOBoundingBox(ZEOBoundingBox& BoundingBox) const;
	void						GenerateOBoundingBox(ZEOBoundingBox& BoundingBox, const ZEMatrix4x4& Transform) const;

	static void					Transform(ZEAABoundingBox& Output, const ZEAABoundingBox& Input, const ZEMatrix4x4& TransformMatrix);

	static ZEHalfSpace			PlaneHalfSpaceTest(const ZEAABoundingBox& BoundingBox, const ZEPlane& Plane);

	static bool					IntersectionTest(const ZEAABoundingBox& BoundingBox, const ZEVector3 Point);
	static bool					IntersectionTest(const ZEAABoundingBox& BoundingBox, const ZELine& Line);
	static bool					IntersectionTest(const ZEAABoundingBox& BoundingBox, const ZELine& Line, float& MinT, float& MaxT);
	static bool					IntersectionTest(const ZEAABoundingBox& BoundingBox, const ZERay& Ray);
	static bool					IntersectionTest(const ZEAABoundingBox& BoundingBox, const ZERay & Ray, float& MinT, float& MaxT);
	static bool					IntersectionTest(const ZEAABoundingBox& BoundingBox, const ZELineSegment& LineSegment);
	static bool					IntersectionTest(const ZEAABoundingBox& BoundingBox, const ZELineSegment& LineSegment, float& MinT, float& MaxT);
	
	static bool					CollisionTest(const ZEAABoundingBox& BoundingBox1, const ZEOBoundingBox& BoundingBox2);
	static bool					CollisionTest(const ZEAABoundingBox& BoundingBox1, const ZEAABoundingBox& BoundingBox2);
	static bool					CollisionTest(const ZEAABoundingBox& BoundingBox, const ZEBoundingSphere& BoundingSphere);

								ZEAABoundingBox();
								ZEAABoundingBox(const ZEVector3 Min, const ZEVector3 Max);

								
};
#endif


