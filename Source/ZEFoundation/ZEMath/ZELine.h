//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELine.h
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
#ifndef	__ZE_MATH_LINE_H__
#define __ZE_MATH_LINE_H__

#include "ZEVector.h"
#include "ZEViewVolume.h"

class ZEPlane;
class ZELineSegment;
class ZERay;

class ZELine
{
	public:
		ZEVector3						v;
		ZEVector3						p;


		static void						Create(ZELine& Line, const ZEVector3& P0, const ZEVector3& P1);
		static void						CreateParametric(ZELine& Line, const ZEVector3& v, const ZEVector3& p);

		static float					MinimumDistance(const ZELine& Line, const ZEVector3& Point);
		static float					MinimumDistance(const ZELine& Line, const ZEVector3& Point, float &TLine);
		static float					MinimumDistance(const ZELine& LineA, const ZELine& LineB);
		static float					MinimumDistance(const ZELine& LineA, const ZELine& LineB, float& TLineA, float& TLineB);
		static float					MinimumDistance(const ZELine& Line, const ZELineSegment& LineSegment);
		static float					MinimumDistance(const ZELine& Line, const ZELineSegment& LineSegment, float& TLine, float& TLineSegment);
		static float					MinimumDistance(const ZELine& Line, const ZERay& Ray);
		static float					MinimumDistance(const ZELine& Line, const ZERay& Ray, float& TLine, float& TRay);

		static void						GetPointOn(ZEVector3& Point, const ZELine& Line, float TLine);
		ZEVector3						GetPointOn(float TLine) const;

										ZELine(const ZEVector3& P0, const ZEVector3& P1);
										ZELine();
};

class ZEViewLine : public ZELine, public ZEViewVolume
{
	public :
		virtual ZEViewVolumeType		GetViewVolumeType() const;

		virtual bool					IntersectionTest(const ZEBSphere& BoundingSphere) const;
		virtual bool					IntersectionTest(const ZEAABBox& BoundingBox) const;
		virtual bool					IntersectionTest(const ZEOBBox& BoundingBox) const;
		virtual bool					IntersectionTest(const ZERectangle3D& Rectangle) const;


										ZEViewLine(const ZEVector3& P0, const ZEVector3& P1);
										ZEViewLine();
};

#endif
