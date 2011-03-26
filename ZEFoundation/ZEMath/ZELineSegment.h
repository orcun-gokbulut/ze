//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELineSegment.h
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
#ifndef	__ZE_MATH_LINESEGMENT_H__
#define __ZE_MATH_LINESEGMENT_H__

#include "ZEVector.h"
#include "ZELine.h"

class ZEPlane;
class ZELineSegment : public ZELine
{
	public:
		static void			Create(ZELine& Line, const ZEVector3& Start, const ZEVector3& End);
		static void			CreateParametric(ZELine& Line, const ZEVector3& v, const ZEVector3& p);

		static float		Length(const ZELineSegment& LineSegment);

		static float		MinimumDistance(const ZELineSegment& LineSegment, const ZEVector3& Point);
		static float		MinimumDistance(const ZELineSegment& LineSegment, const ZEVector3& Point, float &TLine);
		static float		MinimumDistance(const ZELineSegment& LineSegment, const ZELine& Line);
		static float		MinimumDistance(const ZELineSegment& LineSegment, const ZELine& Line, float& TLineSegment, float& TLine);
		static float		MinimumDistance(const ZELineSegment& LineSegmentA, const ZELineSegment& LineSegmentB);
		static float		MinimumDistance(const ZELineSegment& LineSegmentA, const ZELineSegment& LineSegmentB, float& TLineSegmentA, float& TLineSegment);
		static float		MinimumDistance(const ZELineSegment& LineSegment, const ZERay& Ray);
		static float		MinimumDistance(const ZELineSegment& LineSegment, const ZERay& Ray, float& TLine, float& TRay);

		void				GetPointOn(ZEVector3& Point, float TLineSegment) const;
		ZEVector3			GetPointOn(float TLineSegment) const;

		float				GetLenght() const;
		void				GetSegmentStartPoint(ZEVector3& StartPoint) const;
		void				GetSegmentEndPoint(ZEVector3& EndPoint) const;

							ZELineSegment(const ZEVector3& Start, const ZEVector3& End);
							ZELineSegment();
};
#endif




