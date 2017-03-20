//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELineTests.cpp
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
#include "ZEMath/ZELine.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZELineSegment.h"
#include "ZEMath/ZERay.h"


bool ZETestInternalCheckClose(const ZEVector3& Actual, const ZEVector3& Expected, const ZEVector3& Threshold = ZEVector3(ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD));

ZE_TEST(ZELine)
{
	ZE_TEST_ITEM("ZELine::ZELine(const ZEVector3& P0, const ZEVector3& P1)")
	{
		ZEVector3 P0(0.0f, 1.0f, 0.0f);
		ZEVector3 P1(1.0f, 2.0f, 3.0f);

		ZELine Line(P0, P1);
		ZE_TEST_CHECK_EQUAL(Line.p, P0);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Line.v, ZEVector3(0.30151134f, 0.30151134f, 0.90453403f));
	}
	ZE_TEST_ITEM("inline void ZELine::Create(ZELine& Line, const ZEVector3& P0, const ZEVector3& P1)")
	{
		ZELine Line;
		ZEVector3 P0(0.0f, 1.0f, 0.0f);
		ZEVector3 P1(1.0f, 2.0f, 3.0f);

		ZELine::Create(Line, P0, P1);
		ZE_TEST_CHECK_EQUAL(Line.p, P0);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Line.v, P1.Normalize());

	}
	ZE_TEST_ITEM("void ZELine::CreateParametric(ZELine& Line, const ZEVector3& v, const ZEVector3& p)")
	{
		ZELine Line;
		ZEVector3 V(1.0f, 1.0f, 2.0f);
		ZEVector3 P(1.0f, 4.0f, 9.0f);

		ZELine::CreateParametric(Line, V.Normalize(), P);
		ZE_TEST_CHECK_EQUAL(Line.p, P);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Line.v, ZEVector3(0.40824829f, 0.40824829f, 0.81649658f));
	}
	ZE_TEST_ITEM("ZEVector3 ZELine::GetPointOn(float TLine) const")
	{
		ZEVector3 P0(0.0f, 3.0f, 0.0f);
		ZEVector3 P1(1.0f, 4.0f, -1.0f);
		ZELine Line(P0, P1);

		ZEVector3 Point;
		float TLine = 3.0f;

		Point = Line.GetPointOn(TLine);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Point, ZEVector3(1.73205078f, 4.73205078f, -1.73205078f));
	}
	ZE_TEST_ITEM("void ZELine::GetPointOn(ZEVector3& Point, const ZELine& Line, float TLine)")
	{
		ZEVector3 P0(0.0f, 3.0f, 0.0f);
		ZEVector3 P1(1.0f, 4.0f, -1.0f);

		ZELine Line(P0, P1);

		float TLine = 3.0f;
		ZEVector3 Point;

		ZELine::GetPointOn(Point, Line, TLine);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Point, ZEVector3(1.73205078f, 4.73205078f, -1.73205078f));
	}
	ZE_TEST_ITEM("float ZELine::MinimumDistance(const ZELine& LineA, const ZELine& LineB, float& TLineA, float& TLineB)")
	{
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);
		ZEVector3 P3(5.0f, 8.0f, 2.0f);
		ZEVector3 P4(8.0f, 15.0f, 1.0f);

		ZELine LineA(P1, P2);
		ZELine LineB(P3, P4);
		float TLineA, TLineB;

		float Distance = ZELine::MinimumDistance(LineA, LineB, TLineA, TLineB);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 13.19090610f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(TLineA, 1.73205096f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(TLineB, 7.68114605f);
	}
	ZE_TEST_ITEM("float ZELine::MinimumDistance(const ZELine& LineA, const ZELine& LineB)")
	{
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);
		ZEVector3 P3(5.0f, 8.0f, 2.0f);
		ZEVector3 P4(8.0f, 15.0f, 1.0f);

		ZELine LineA(P1, P2);
		ZELine LineB(P3, P4);

		float Distance = ZELine::MinimumDistance(LineA, LineB);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 13.19090610f);
	}
	ZE_TEST_ITEM("float ZELine::MinimumDistance(const ZELine& Line, const ZEVector3& Point, float& TLine)")
	{
		ZEVector3 Point(5.0f, 8.0f, 2.0f);
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);

		ZELine Line(P1, P2);

		float TLine;

		float Distance = ZELine::MinimumDistance(Line, Point, TLine);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 4.61880214f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(TLine, 4.6188025f);
	}
	ZE_TEST_ITEM("float ZELine::MinimumDistance(const ZELine& Line, const ZEVector3& Point)")
	{
		ZEVector3 Point(5.0f, 8.0f, 2.0f);
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);

		ZELine Line(P1, P2);

		float Distance = ZELine::MinimumDistance(Line, Point);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 4.61880214f);
	}
	ZE_TEST_ITEM("float ZELine::MinimumDistance(const ZELine& Line, const ZELineSegment& LineSegment, float& TLine, float& TLineSegment)")
	{
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);
		ZEVector3 P3(5.0f, 8.0f, 2.0f);
		ZEVector3 P4(8.0f, 15.0f, 1.0f);

		ZELine Line(P1, P2);
		ZELineSegment LineSegment(P3, P4);

		float TLine, TLineSegment;
		float Distance = ZELine::MinimumDistance(Line, LineSegment, TLine, TLineSegment);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 5.7154760f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(TLine, 4.61880226f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(TLineSegment, 0.0f);
	}
	ZE_TEST_ITEM("float ZELine::MinimumDistance(const ZELine& Line, const ZELineSegment& LineSegment)")
	{
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);
		ZEVector3 P3(5.0f, 8.0f, 2.0f);
		ZEVector3 P4(8.0f, 15.0f, 1.0f);

		ZELine Line(P1, P2);
		ZELineSegment LineSegment(P3, P4);

		float Distance = ZELine::MinimumDistance(Line, LineSegment);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 5.7154760f);
	}
	ZE_TEST_ITEM("float ZELine::MinimumDistance(const ZELine& Line, const ZERay& Ray, float& TLine, float& TRay)")
	{
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);
		ZEVector3 P3(5.0f, 8.0f, 2.0f);
		ZEVector3 P4(8.0f, 10.0f, 3.0f);

		ZELine Line(P1, P2);
		ZERay Ray(P3.Normalize(), P4);

		float TLine, TRay;
		float Distance = ZELine::MinimumDistance(Line, Ray, TLine, TRay);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 16.309507f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(TLine, -6.92820334f);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(TRay, 0.00000000f);
	}
	ZE_TEST_ITEM("float ZELine::MinimumDistance(const ZELine& Line, const ZERay& Ray)")
	{
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);
		ZEVector3 P3(5.0f, 8.0f, 2.0f);
		ZEVector3 P4(8.0f, 10.0f, 3.0f);

		ZELine Line(P1, P2);
		ZERay Ray(P3, P4);

		float Distance = ZELine::MinimumDistance(Line, Ray);
		ZE_TEST_CHECK_EQUAL_OR_CLOSE(Distance, 16.309507f);
	}
}
