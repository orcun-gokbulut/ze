//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Rectangle.h
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
#ifndef __ZE_MATH_RECTANGLE_H__
#define __ZE_MATH_RECTANGLE_H__

#include "Vector.h"
#include "Plane.h"

enum ZERectangleCorner
{
	ZE_RC_LEFTDOWN		= 0,
	ZE_RC_LEFTUP		= 1,
	ZE_RC_RIGHTDOWN		= 2,
	ZE_RC_RIGHTUP		= 3
};

enum ZERectangleIntersectionResult
{
	ZE_RIC_NO_INTERSECTION = 0,
	ZE_RIC_A_COVERS_B      = 1,
	ZE_RIC_B_COVERS_A      = 2,
	ZE_RIC_INTERSECTS	   = 3,
};


class ZERectangle
{
	public:
		ZEVector2				LeftUp, RightDown;

		bool					IsEmpty();
		bool					BoundingTest(const ZEVector2& Point) const;

		void					SetPosition(const ZEVector2& Position);
		const ZEVector2&		GetPosition() const;

		void					SetWidth(float Width);
		float					GetWidth() const;
	
		void					SetHeight(float Height);
		float					GetHeight() const;

		ZEVector2				GetCorner(ZERectangleCorner Corner) const;

		static bool				IntersectionTest(const ZERectangle& RectangleA, const ZERectangle& RectangleB);
		static bool				Intersection(ZERectangle& Intersection, const ZERectangle& RectangleA, const ZERectangle& RectangleB);
		static bool				BoundingTest(const ZERectangle& Rectangle, const ZEVector2& Point);

								ZERectangle();
								ZERectangle(const ZEVector2& LeftUp, const ZEVector2& RightDown);
								ZERectangle(const ZEVector2& LeftUp, float Width, float Height);
};
#endif
