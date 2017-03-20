//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELineSegmentTests.cpp
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
#include "ZEMath/ZELineSegment.h"
#include "ZEMath/ZELine.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZERay.h"

bool ZETestInternalCheckClose(const ZEVector3& Actual, const ZEVector3& Expected, const ZEVector3& Threshold = ZEVector3(ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD));

ZE_TEST(ZELineSegment)
{
	ZE_TEST_ITEM("ZELineSegment::ZELineSegment(const ZEVector3& Start, const ZEVector3& End)")
	{
		ZEVector3 Start(0.0f, 1.0f, 0.0f);
		ZEVector3 End(1.0f, 2.0f, 3.0f);

		ZELineSegment LineSegment(Start, End);
		ZE_TEST_CHECK_EQUAL(LineSegment.Length, 3.3166249f);
		ZE_TEST_CHECK_EQUAL(LineSegment.v, ZEVector3(0.30151135f, 0.30151135f, 0.90453398f));
		ZE_TEST_CHECK_EQUAL(LineSegment.p, ZEVector3(0.00000000f, 1.0000000f, 0.00000000f));
	}
	ZE_TEST_ITEM("inline void ZELineSegment::Create(ZELineSegment& LineSegment, const ZEVector3& Start, const ZEVector3& End)")
	{
		ZELineSegment LineSegment;
		ZEVector3 Start(0.0f, 0.0f, 1.0f);
		ZEVector3 End(0.0f, 1.0f, 2.0f);

		ZELineSegment::Create(LineSegment, Start, End);
		ZE_TEST_CHECK_EQUAL(LineSegment.Length, 1.41421356f);
		ZE_TEST_CHECK_EQUAL(LineSegment.v, ZEVector3(0.0f, 0.70710678f, 0.70710678f));
		ZE_TEST_CHECK_EQUAL(LineSegment.p, ZEVector3(0.0f, 0.0f, 1.0f));

		ZE_TEST_CASE("Create with normalized end vector")
		{
			ZELineSegment LineSegment;
			ZEVector3 Start(0.0f, 1.0f, 2.0f);
			ZEVector3 End(0.0f, 1.0f, 0.0f);

			ZELineSegment::Create(LineSegment, Start, End);
			ZE_TEST_CHECK_EQUAL(LineSegment.Length, 2.0f);
			ZE_TEST_CHECK_EQUAL(LineSegment.v, ZEVector3(0.0f, 0.0f, -1.0f));
			ZE_TEST_CHECK_EQUAL(LineSegment.p, ZEVector3(0.0f, 1.0f, 2.0f));
		}
	}
	ZE_TEST_ITEM("void ZELineSegment::CreateParametric(ZELineSegment& LineSegment, const ZEVector3& v, const ZEVector3& p)")
	{
		ZELineSegment LineSegment;
		ZEVector3 V(1.0f, 4.0f, 9.0f);
		ZEVector3 P(1.0f, 0.0f, 0.0f);

		ZELineSegment::CreateParametric(LineSegment, V, P);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(LineSegment.Length, 9.89949493f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(LineSegment.v, ZEVector3(0.101015254f, 0.4040610178f, 0.90913729f));
		ZE_TEST_CHECK_EQUAL(LineSegment.p, ZEVector3(1.0f, 0.0f, 0.0f));
	}
	ZE_TEST_ITEM("void ZELineSegment::GetPointOn(ZEVector3& Point, const ZELineSegment& LineSegment, float TLineSegment)")
	{
		ZEVector3 Start(0.0f, 3.0f, 0.0f);
		ZEVector3 End(1.0f, 4.0f, -1.0f);
		ZELineSegment LineSegment(Start, End);
		ZEVector3 Point;

		ZE_TEST_CASE("for TLineSegment < 0")
		{
			ZELineSegment::GetPointOn(Point, LineSegment, -0.5f);
			ZE_TEST_CHECK_EQUAL(Point, ZEVector3(0.0f, 3.0f, 0.0f));
		}
		ZE_TEST_CASE("for TLineSegment > Length")
		{
			ZELineSegment::GetPointOn(Point, LineSegment, 3.0f);
			ZE_TEST_CHECK_EQUAL(Point, ZEVector3(0.99999997f, 6.19615237f, -0.99999997f));
		}
		ZE_TEST_CASE("for TLineSegment = 0")
		{
			ZELineSegment::GetPointOn(Point, LineSegment, 0.0f);
			ZE_TEST_CHECK_EQUAL(Point, ZEVector3(0.0f, 3.0f, 0.0f));
		}
		ZE_TEST_CASE("for 0 < TLineSegment < Length")
		{
			ZELineSegment::GetPointOn(Point, LineSegment, 0.5f);
			ZE_TEST_CHECK_EQUAL(Point, ZEVector3(0.28867513f, 3.28867513f, -0.28867513f));
		}
	}
	ZE_TEST_ITEM("ZEVector3 ZELineSegment::GetPointOn(float TLineSegment) const")
	{
		ZEVector3 Start(0.0f, 3.0f, 0.0f);
		ZEVector3 End(1.0f, 4.0f, -1.0f);
		ZELineSegment LineSegment(Start, End);

		ZE_TEST_CASE("for TLineSegment < 0")
		{
			ZEVector3 Point = LineSegment.GetPointOn(-0.5f);
			ZE_TEST_CHECK_EQUAL(Point, ZEVector3(0.0f, 3.0f, 0.0f));
		}
		ZE_TEST_CASE("for TLineSegment > Length")
		{
			ZEVector3 Point = LineSegment.GetPointOn(3.0f);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Point, ZEVector3(0.99999997f, 6.19615237f, -0.99999997f));
		}
		ZE_TEST_CASE("for TLineSegment = 0")
		{
			ZEVector3 Point = LineSegment.GetPointOn(0.0f);
			ZE_TEST_CHECK_EQUAL(Point, ZEVector3(0.0f, 3.0f, 0.0f));
		}
		ZE_TEST_CASE("for 0 < TLineSegment < Length")
		{
			ZEVector3 Point = LineSegment.GetPointOn(0.5f);
			ZE_TEST_CHECK_EQUAL_OR_CLOSE(Point, ZEVector3(0.28867513f, 3.28867513f, -0.28867513f));
		}
	}

	ZE_TEST_ITEM("ZEVector3 ZELineSegment::GetEndPoint() const")
	{
		ZEVector3 Start(-3.0f, -1.0f, 1.0f);
		ZEVector3 End(0.0f, 3.0f, -5.0f);
		ZELineSegment LineSegment(Start, End);
		ZEVector3 EndPoint;

		EndPoint = LineSegment.GetEndPoint();
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(EndPoint, ZEVector3(0.0f, 3.0f, -5.0f));
	}

	ZE_TEST_ITEM("void ZELineSegment::GetEndPoint(ZEVector3& EndPoint, const ZELineSegment& LineSegment)")
	{
		ZEVector3 Start(-3.0f, -1.0f, 1.0f);
		ZEVector3 End(0.0f, 3.0f, -5.0f);
		ZELineSegment LineSegment(Start, End);
		ZEVector3 EndPoint;

		ZELineSegment::GetEndPoint(EndPoint, LineSegment);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(EndPoint, ZEVector3(0.0f, 3.0f, -5.0f));
	}

	ZE_TEST_ITEM("const ZEVector3& ZELineSegment::GetStartPoint() const")
	{
		ZEVector3 Start(-3.0f, -1.0f, 1.0f);
		ZEVector3 End(0.0f, 3.0f, -5.0f);
		ZELineSegment LineSegment(Start, End);
		ZEVector3 StartPoint;

		StartPoint = LineSegment.GetStartPoint();
		ZE_TEST_CHECK_EQUAL(StartPoint, ZEVector3(-3.0f, -1.0f, 1.0f));
	}

	ZE_TEST_ITEM("void ZELineSegment::GetStartPoint(ZEVector3& StartPoint, const ZELineSegment& LineSegment)")
	{
		ZEVector3 Start(-3.0f, -1.0f, 1.0f);
		ZEVector3 End(0.0f, 3.0f, -5.0f);
		ZELineSegment LineSegment(Start, End);
		ZEVector3 StartPoint;

		ZELineSegment::GetStartPoint(StartPoint, LineSegment);
		ZE_TEST_CHECK_EQUAL(StartPoint, ZEVector3(-3.0f, -1.0f, 1.0f));
	}
	ZE_TEST_ITEM("float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZEVector3& Point)")
	{
		ZEVector3 Point(5.0f, 8.0f, 2.0f);
		ZEVector3 Start(0.0f, 3.0f, 0.0f);
		ZEVector3 End(1.0f, 4.0f, -1.0f);

		ZELineSegment LineSegment(Start, End);

		float Distance = ZELineSegment::MinimumDistance(LineSegment,Point);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 6.40312423f);
	}
	ZE_TEST_ITEM("float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZEVector3& Point, float& TLineSegment)")
	{
		ZEVector3 Point(5.0f, 8.0f, 2.0f);
		ZEVector3 Start(0.0f, 3.0f, 0.0f);
		ZEVector3 End(1.0f, 4.0f, -1.0f);

		ZELineSegment LineSegment(Start, End);

		float TLineSegment;

		float Distance = ZELineSegment::MinimumDistance(LineSegment, Point, TLineSegment);
		ZE_TEST_CHECK_EQUAL(Distance, 6.40312423f);
		ZE_TEST_CHECK_EQUAL(TLineSegment, 4.61880226f);
	}
	ZE_TEST_ITEM("float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZELine& Line)")
	{
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);
		ZEVector3 P3(5.0f, 8.0f, 2.0f);
		ZEVector3 P4(8.0f, 15.0f, 1.0f);

		ZELineSegment LineSegment(P1, P2);
		ZELine Line(P3, P4);

		float Distance = ZELineSegment::MinimumDistance(LineSegment, Line);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 3.86641339f);
	}
	ZE_TEST_ITEM("float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZELine& Line, float& TLineSegment, float& TLine)")
	{
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);
		ZEVector3 P3(5.0f, 8.0f, 2.0f);
		ZEVector3 P4(8.0f, 15.0f, 1.0f);

		ZELineSegment LineSegment(P1, P2);
		ZELine Line(P3, P4);

		float TLineSegment, TLine;
		float Distance = ZELineSegment::MinimumDistance(LineSegment, Line, TLineSegment, TLine);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 3.86641339f);
		ZE_TEST_CHECK_EQUAL(TLineSegment, 0.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(TLine, -6.24906751f);
	}
	ZE_TEST_ITEM("float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZERay& Ray)")
	{
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);
		ZEVector3 P3(5.0f, 8.0f, 2.0f);
		ZEVector3 P4(8.0f, 10.0f, 3.0f);

		ZELineSegment LineSegment(P1, P2);
		ZERay Ray(P3.Normalize(), P4);

		float Distance = ZELineSegment::MinimumDistance(LineSegment, Ray);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 21.391888f);
	}
	ZE_TEST_ITEM("float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZERay& Ray, float& TLineSegment, float& TRay)")
	{
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);
		ZEVector3 P3(5.0f, 8.0f, 2.0f);
		ZEVector3 P4(8.0f, 10.0f, 3.0f);

		ZELineSegment LineSegment(P1, P2);
		ZERay Ray(P3.Normalize(), P4);

		float TLineSegment, TRay;
		float Distance = ZELineSegment::MinimumDistance(LineSegment, Ray, TLineSegment, TRay);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 21.391888f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(TLineSegment, 0.0f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(TRay, 10.576907f);
	}
	ZE_TEST_ITEM("float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegmentA, const ZELineSegment& LineSegmentB)")
	{
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);
		ZEVector3 P3(5.0f, 8.0f, 2.0f);
		ZEVector3 P4(8.0f, 15.0f, 1.0f);

		ZELineSegment LineSegmentA(P1, P2);
		ZELineSegment LineSegmentB(P3, P4);

		float Distance = ZELineSegment::MinimumDistance(LineSegmentA, LineSegmentB);
		ZE_TEST_CHECK_EQUAL(Distance, 6.5193453f); 
	}
	ZE_TEST_ITEM("float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegmentA, const ZELineSegment& LineSegmentB, float& TLineSegmentA, float &TLineSegmentB)")
	{
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);
		ZEVector3 P3(5.0f, 8.0f, 2.0f);
		ZEVector3 P4(8.0f, 15.0f, 1.0f);

		ZELineSegment LineSegmentA(P1, P2);
		ZELineSegment LineSegmentB(P3, P4);

		float TLineSegmentA, TLineSegmentB;
		float Distance = ZELineSegment::MinimumDistance(LineSegmentA, LineSegmentB, TLineSegmentA, TLineSegmentB);
		ZE_TEST_CHECK_EQUAL(Distance, 6.5193453f);
		ZE_TEST_CHECK_EQUAL(TLineSegmentA, 0.0f);
		ZE_TEST_CHECK_EQUAL(TLineSegmentB, 1.0000000f);
	}
}
