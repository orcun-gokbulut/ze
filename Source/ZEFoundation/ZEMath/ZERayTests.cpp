//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERayTests.cpp
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



#include "ZETest/ZETest.h"
#include "ZEMath/ZEMath.h"

#include "ZEMathIOStreamMapping.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZERay.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZELineSegment.h"

bool ZETestInternalCheckClose(const ZEVector3& Actual, const ZEVector3& Expected, const ZEVector3& Threshold = ZEVector3(ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD));


ZE_TEST(ZERay)
{
	ZE_TEST_ITEM("void ZERay::Create(ZERay & Ray, const ZEVector3 & Start,const ZEVector3 & End)")
	{
		ZERay Ray;
		ZEVector3 Start(0.0f, 1.0f, 0.0f);
		ZEVector3 End(1.0f, 2.0f, 3.0f);

		ZERay::Create(Ray, Start, End);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Ray.v, ZEVector3(0.30151134f, 0.30151134f, 0.90453403f));
		ZE_TEST_CHECK_EQUAL(Ray.p, ZEVector3(0.0f, 1.0f, 0.0f));
	}
	ZE_TEST_ITEM("void ZERay::CreateParametric(ZERay & Ray,const ZEVector3 & v,const ZEVector3 & p)")
	{
		ZERay Ray;
		ZEVector3 v(1.0f, 2.0f, 3.0f);
		ZEVector3 p(2.0f, 3.0f, 4.0f);

		ZERay::CreateParametric(Ray, v, p);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Ray.v, ZEVector3(0.26726124f, 0.53452248f, 0.80178372f));
		ZE_TEST_CHECK_EQUAL(Ray.p, ZEVector3(2.0f, 3.0f, 4.0f));
	}
	ZE_TEST_ITEM("ZEVector3 ZERay::GetPointOn(float TRay) const")
	{
		ZEVector3 v(0.0f, 3.0f, 0.0f);
		ZEVector3 p(1.0f, 4.0f, -1.0f);

		ZERay Ray(v.Normalize(), p);

		ZEVector3 Point;
		float TRay = 0.5f;

		Point = Ray.GetPointOn(TRay);
		ZE_TEST_CHECK_EQUAL(Point, ZEVector3(1.0f, 4.5f, -1.0f));

		ZE_TEST_CASE("for TRay < 0")
		{
			float TRay = -1.0f;

			Point = Ray.GetPointOn(TRay);
			ZE_TEST_CHECK_EQUAL(Point, ZEVector3(1.0f, 4.0f, -1.0f));
		}

	}
	ZE_TEST_ITEM("void ZERay::GetPointOn(ZEVector3& Point, const ZERay& Ray, float TRay)")
	{
		ZEVector3 v(0.0f, 3.0f, 0.0f);
		ZEVector3 p(1.0f, 4.0f, -1.0f);

		ZERay Ray(v.Normalize(), p);

		ZEVector3 Point;
		float TRay = 3.0f;

		ZERay::GetPointOn(Point, Ray, TRay);
		ZE_TEST_CHECK_EQUAL(Point, ZEVector3(1.0f, 7.0f, -1.0f));

		ZE_TEST_CASE("for TRay < 0")
		{
			float TRay = -1.0f;

			ZERay::GetPointOn(Point, Ray, TRay);
			ZE_TEST_CHECK_EQUAL(Point, ZEVector3(1.0f, 4.0f, -1.0f));
		}
	}
	ZE_TEST_ITEM("const ZEVector3& ZERay::GetStartPoint() const")
	{
		ZEVector3 v(0.0f, 3.0f, 0.0f);
		ZEVector3 p(1.0f, 4.0f, -1.0f);
		ZEVector3 StartPoint;

		ZERay Ray(v.Normalize(), p);

		StartPoint = Ray.GetStartPoint();
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(StartPoint, ZEVector3(1.0f, 4.0f, -1.0f));
	}
	ZE_TEST_ITEM("float ZERay::MinimumDistance(const ZERay& Ray, const ZELine& Line)")
	{
		ZEVector3 v(0.0f, 3.0f, 0.0f);
		ZEVector3 p(1.0f, 4.0f, -1.0f);
		ZEVector3 P1(5.0f, 8.0f, 2.0f);
		ZEVector3 P2(8.0f, 10.0f, 3.0f);

		ZERay Ray(v.Normalize(), p);

		ZELine Line(P1, P2);

		float Distance = ZERay::MinimumDistance(Ray, Line);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 1.58113883f);
	}
	ZE_TEST_ITEM("float ZERay::MinimumDistance(const ZERay& Ray, const ZELine& Line, float& TRay, float& TLine)")
	{
		ZEVector3 v(0.0f, 3.0f, 0.0f);
		ZEVector3 p(1.0f, 4.0f, -1.0f);
		ZEVector3 P1(5.0f, 8.0f, 2.0f);
		ZEVector3 P2(8.0f, 10.0f, 3.0f);

		ZERay Ray(v.Normalize(), p);

		ZELine Line(P1, P2);

		float TRay, TLine;

		float Distance = ZERay::MinimumDistance(Ray, Line, TRay, TLine);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 1.58113883f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(TRay, 0.99999991f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(TLine, -5.61248646f);
	}
	ZE_TEST_ITEM("float ZERay::MinimumDistance(const ZERay& Ray, const ZELineSegment& LineSegment)")
	{
		ZEVector3 v(0.0f, 3.0f, 0.0f);
		ZEVector3 p(1.0f, 4.0f, -1.0f);
		ZEVector3 P1(5.0f, 8.0f, 2.0f);
		ZEVector3 P2(8.0f, 10.0f, 3.0f);

		ZERay Ray(v.Normalize(), p);
		ZELineSegment LineSegment(P1, P2);

		float Distance = ZERay::MinimumDistance(Ray, LineSegment);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 7.3684468f);
	}
	ZE_TEST_ITEM("float ZERay::MinimumDistance(const ZERay& Ray, const ZELineSegment& LineSegment, float& TRay, float& TLineSegment)")
	{
		ZEVector3 v(0.0f, 3.0f, 0.0f);
		ZEVector3 p(1.0f, 4.0f, -1.0f);
		ZEVector3 P1(5.0f, 8.0f, 2.0f);
		ZEVector3 P2(8.0f, 10.0f, 3.0f);

		ZERay Ray(v.Normalize(), p);

		ZELineSegment LineSegment(P1,P2);

		float TRay, TLineSegment;

		float Distance = ZERay::MinimumDistance(Ray, LineSegment, TRay, TLineSegment);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 7.3684468f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(TRay, 0.00000000f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(TLineSegment, 1.0000000f);
	}
	ZE_TEST_ITEM("float ZERay::MinimumDistance(const ZERay& Ray, const ZEVector3& Point)")
	{
		ZEVector3 Point(5.0f, 8.0f, 2.0f);
		ZEVector3 v(0.0f, 3.0f, 0.0f);
		ZEVector3 p(1.0f, 4.0f, -1.0f);

		ZERay Ray(v.Normalize(), p);

		float Distance = ZERay::MinimumDistance(Ray, Point);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 5.0000000f);
	}
	ZE_TEST_ITEM("float ZERay::MinimumDistance(const ZERay& Ray, const ZEVector3& Point, float& TRay)")
	{
		ZEVector3 Point(5.0f, 8.0f, 2.0f);
		ZEVector3 v(0.0f, 3.0f, 0.0f);
		ZEVector3 p(1.0f, 4.0f, -1.0f);

		ZERay Ray(v.Normalize(), p);
		float TRay;

		float Distance = ZERay::MinimumDistance(Ray, Point, TRay);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 5.0000000f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(TRay, 4.0000000f);
	}
	ZE_TEST_ITEM("float ZERay::MinimumDistance(const ZERay& RayA, const ZERay& RayB)")
	{
		ZEVector3 v1(0.0f, 3.0f, 0.0f);
		ZEVector3 p1(1.0f, 4.0f, -1.0f);
		ZEVector3 v2(5.0f, 8.0f, 2.0f);
		ZEVector3 p2(8.0f, 15.0f, 1.0f);

		ZERay RayA(v1.Normalize(), p1);

		ZERay RayB(v2.Normalize(), p2);

		float Distance = ZERay::MinimumDistance(RayA, RayB);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 0.75491214f);
	}
	ZE_TEST_ITEM("float ZERay::MinimumDistance(const ZERay& RayA, const ZERay& RayB, float& TRayA, float& TRayB)")
	{
		ZEVector3 v1(0.0f, 3.0f, 0.0f);
		ZEVector3 p1(1.0f, 4.0f, -1.0f);
		ZEVector3 v2(5.0f, 8.0f, 2.0f);
		ZEVector3 p2(8.0f, 15.0f, 1.0f);

		ZERay RayA(v1.Normalize(), p1);

		ZERay RayB(v2.Normalize(), p2);

		float TRayA, TRayB;
		float Distance = ZERay::MinimumDistance(RayA, RayB, TRayA, TRayB);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 0.75491214f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(TRayA, 0.00000000f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(TRayB, 13.16928678f);
	}
	ZE_TEST_ITEM("ZERay::ZERay(const ZEVector3 & v,const ZEVector3 &p)")
	{
		ZEVector3 v(0.0f, 1.0f, 0.0f);
		ZEVector3 p(1.0f, 2.0f, 3.0f);

		ZERay Ray(v.Normalize(), p);
		ZE_TEST_CHECK_EQUAL(Ray.v, ZEVector3(0.0f, 1.0f, 0.0f));
		ZE_TEST_CHECK_EQUAL(Ray.p, ZEVector3(1.0f, 2.0f, 3.0f));
	}
}
