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



#include "ZETest.h"
#include <d3dx9.h>
#include <math.h>
#include "ZEMathIOStreamMapping.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZELineSegment.h"
#include "ZEMath/ZELine.h"
#include "ZEMath/ZEMathDefinitions.h"
#include "ZEMath/ZERay.h"

bool ZETestInternalCheckClose(const ZEVector3& Actual, const ZEVector3& Expected, const ZEVector3& Threshold = ZEVector3(ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD));

ZETestSuite(ZELineSegment)
{
	ZETest("ZELineSegment::ZELineSegment(const ZEVector3& Start, const ZEVector3& End)")
	{
		ZEVector3 Start(0.0f, 1.0f, 0.0f);
		ZEVector3 End(1.0f, 2.0f, 3.0f);

		ZELineSegment LineSegment(Start, End);
		ZETestCheckEqual(LineSegment.Length, 3.3166249f);
		ZETestCheckEqual(LineSegment.v, ZEVector3(0.30151135f, 0.30151135f, 0.90453398f));
		ZETestCheckEqual(LineSegment.p, ZEVector3(0.00000000f, 1.0000000f, 0.00000000f));
	}
	ZETest("inline void ZELineSegment::Create(ZELineSegment& LineSegment, const ZEVector3& Start, const ZEVector3& End)")
	{
		ZELineSegment LineSegment;
		ZEVector3 Start(0.0f, 0.0f, 1.0f);
		ZEVector3 End(0.0f, 1.0f, 2.0f);

		ZELineSegment::Create(LineSegment, Start, End);
		ZETestCheckEqual(LineSegment.Length, 1.41421356f);
		ZETestCheckEqual(LineSegment.v, ZEVector3(0.0f, 0.70710678f, 0.70710678f));
		ZETestCheckEqual(LineSegment.p, ZEVector3(0.0f, 0.0f, 1.0f));

		ZETestCase("Create with normalized end vector")
		{
			ZELineSegment LineSegment;
			ZEVector3 Start(0.0f, 1.0f, 2.0f);
			ZEVector3 End(0.0f, 1.0f, 0.0f);

			ZELineSegment::Create(LineSegment, Start, End);
			ZETestCheckEqual(LineSegment.Length, 2.0f);
			ZETestCheckEqual(LineSegment.v, ZEVector3(0.0f, 0.0f, -1.0f));
			ZETestCheckEqual(LineSegment.p, ZEVector3(0.0f, 1.0f, 2.0f));
		}
	}
	ZETest("void ZELineSegment::CreateParametric(ZELineSegment& LineSegment, const ZEVector3& v, const ZEVector3& p)")
	{
		ZELineSegment LineSegment;
		ZEVector3 V(1.0f, 4.0f, 9.0f);
		ZEVector3 P(1.0f, 0.0f, 0.0f);

		ZELineSegment::CreateParametric(LineSegment, V, P);
		ZETestCheckClose(LineSegment.Length, 9.89949493f);
		ZETestCheckClose(LineSegment.v, ZEVector3(0.101015254f, 0.4040610178f, 0.90913729f));
		ZETestCheckEqual(LineSegment.p, ZEVector3(1.0f, 0.0f, 0.0f));
	}
	ZETest("void ZELineSegment::GetPointOn(ZEVector3& Point, const ZELineSegment& LineSegment, float TLineSegment)")
	{
		ZEVector3 Start(0.0f, 3.0f, 0.0f);
		ZEVector3 End(1.0f, 4.0f, -1.0f);
		ZELineSegment LineSegment(Start, End);
		ZEVector3 Point;

		ZETestCase("for TLineSegment < 0")
		{
			ZELineSegment::GetPointOn(Point, LineSegment, -0.5f);
			ZETestCheckEqual(Point, ZEVector3(0.0f, 3.0f, 0.0f));
		}
		ZETestCase("for TLineSegment > Length")
		{
			ZELineSegment::GetPointOn(Point, LineSegment, 3.0f);
			ZETestCheckEqual(Point, ZEVector3(0.99999997f, 6.19615237f, -0.99999997f));
		}
		ZETestCase("for TLineSegment = 0")
		{
			ZELineSegment::GetPointOn(Point, LineSegment, 0.0f);
			ZETestCheckEqual(Point, ZEVector3(0.0f, 3.0f, 0.0f));
		}
		ZETestCase("for 0 < TLineSegment < Length")
		{
			ZELineSegment::GetPointOn(Point, LineSegment, 0.5f);
			ZETestCheckEqual(Point, ZEVector3(0.28867513f, 3.28867513f, -0.28867513f));
		}
	}
	ZETest("ZEVector3 ZELineSegment::GetPointOn(float TLineSegment) const")
	{
		ZEVector3 Start(0.0f, 3.0f, 0.0f);
		ZEVector3 End(1.0f, 4.0f, -1.0f);
		ZELineSegment LineSegment(Start, End);

		ZETestCase("for TLineSegment < 0")
		{
			ZEVector3 Point = LineSegment.GetPointOn(-0.5f);
			ZETestCheckEqual(Point, ZEVector3(0.0f, 3.0f, 0.0f));
		}
		ZETestCase("for TLineSegment > Length")
		{
			ZEVector3 Point = LineSegment.GetPointOn(3.0f);
			ZETestCheckClose(Point, ZEVector3(0.99999997f, 6.19615237f, -0.99999997f));
		}
		ZETestCase("for TLineSegment = 0")
		{
			ZEVector3 Point = LineSegment.GetPointOn(0.0f);
			ZETestCheckEqual(Point, ZEVector3(0.0f, 3.0f, 0.0f));
		}
		ZETestCase("for 0 < TLineSegment < Length")
		{
			ZEVector3 Point = LineSegment.GetPointOn(0.5f);
			ZETestCheckClose(Point, ZEVector3(0.28867513f, 3.28867513f, -0.28867513f));
		}
	}

	ZETest("ZEVector3 ZELineSegment::GetEndPoint() const")
	{
		ZEVector3 Start(-3.0f, -1.0f, 1.0f);
		ZEVector3 End(0.0f, 3.0f, -5.0f);
		ZELineSegment LineSegment(Start, End);
		ZEVector3 EndPoint;

		EndPoint = LineSegment.GetEndPoint();
		ZETestCheckClose(EndPoint, ZEVector3(0.0f, 3.0f, -5.0f));
	}

	ZETest("void ZELineSegment::GetEndPoint(ZEVector3& EndPoint, const ZELineSegment& LineSegment)")
	{
		ZEVector3 Start(-3.0f, -1.0f, 1.0f);
		ZEVector3 End(0.0f, 3.0f, -5.0f);
		ZELineSegment LineSegment(Start, End);
		ZEVector3 EndPoint;

		ZELineSegment::GetEndPoint(EndPoint, LineSegment);
		ZETestCheckClose(EndPoint, ZEVector3(0.0f, 3.0f, -5.0f));
	}

	ZETest("const ZEVector3& ZELineSegment::GetStartPoint() const")
	{
		ZEVector3 Start(-3.0f, -1.0f, 1.0f);
		ZEVector3 End(0.0f, 3.0f, -5.0f);
		ZELineSegment LineSegment(Start, End);
		ZEVector3 StartPoint;

		StartPoint = LineSegment.GetStartPoint();
		ZETestCheckEqual(StartPoint, ZEVector3(-3.0f, -1.0f, 1.0f));
	}

	ZETest("void ZELineSegment::GetStartPoint(ZEVector3& StartPoint, const ZELineSegment& LineSegment)")
	{
		ZEVector3 Start(-3.0f, -1.0f, 1.0f);
		ZEVector3 End(0.0f, 3.0f, -5.0f);
		ZELineSegment LineSegment(Start, End);
		ZEVector3 StartPoint;

		ZELineSegment::GetStartPoint(StartPoint, LineSegment);
		ZETestCheckEqual(StartPoint, ZEVector3(-3.0f, -1.0f, 1.0f));
	}
	ZETest("float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZEVector3& Point)")
	{
		ZEVector3 Point(5.0f, 8.0f, 2.0f);
		ZEVector3 Start(0.0f, 3.0f, 0.0f);
		ZEVector3 End(1.0f, 4.0f, -1.0f);

		ZELineSegment LineSegment(Start, End);

		float Distance = ZELineSegment::MinimumDistance(LineSegment,Point);
		ZETestCheckClose(Distance, 6.40312423f);
	}
	ZETest("float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZEVector3& Point, float& TLineSegment)")
	{
		ZEVector3 Point(5.0f, 8.0f, 2.0f);
		ZEVector3 Start(0.0f, 3.0f, 0.0f);
		ZEVector3 End(1.0f, 4.0f, -1.0f);

		ZELineSegment LineSegment(Start, End);

		float TLineSegment;

		float Distance = ZELineSegment::MinimumDistance(LineSegment, Point, TLineSegment);
		ZETestCheckEqual(Distance, 6.40312423f);
		ZETestCheckEqual(TLineSegment, 4.61880226f);
	}
	ZETest("float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZELine& Line)")
	{
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);
		ZEVector3 P3(5.0f, 8.0f, 2.0f);
		ZEVector3 P4(8.0f, 15.0f, 1.0f);

		ZELineSegment LineSegment(P1, P2);
		ZELine Line(P3, P4);

		float Distance = ZELineSegment::MinimumDistance(LineSegment, Line);
		ZETestCheckClose(Distance, 3.86641339f);
	}
	ZETest("float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZELine& Line, float& TLineSegment, float& TLine)")
	{
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);
		ZEVector3 P3(5.0f, 8.0f, 2.0f);
		ZEVector3 P4(8.0f, 15.0f, 1.0f);

		ZELineSegment LineSegment(P1, P2);
		ZELine Line(P3, P4);

		float TLineSegment, TLine;
		float Distance = ZELineSegment::MinimumDistance(LineSegment, Line, TLineSegment, TLine);
		ZETestCheckClose(Distance, 3.86641339f);
		ZETestCheckEqual(TLineSegment, 0.0f);
		ZETestCheckClose(TLine, -6.24906751f);
	}
	ZETest("float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZERay& Ray)")
	{
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);
		ZEVector3 P3(5.0f, 8.0f, 2.0f);
		ZEVector3 P4(8.0f, 10.0f, 3.0f);

		ZELineSegment LineSegment(P1, P2);
		ZERay Ray(P3.Normalize(), P4);

		float Distance = ZELineSegment::MinimumDistance(LineSegment, Ray);
		ZETestCheckClose(Distance, 21.391888f);
	}
	ZETest("float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegment, const ZERay& Ray, float& TLineSegment, float& TRay)")
	{
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);
		ZEVector3 P3(5.0f, 8.0f, 2.0f);
		ZEVector3 P4(8.0f, 10.0f, 3.0f);

		ZELineSegment LineSegment(P1, P2);
		ZERay Ray(P3.Normalize(), P4);

		float TLineSegment, TRay;
		float Distance = ZELineSegment::MinimumDistance(LineSegment, Ray, TLineSegment, TRay);
		ZETestCheckClose(Distance, 21.391888f);
		ZETestCheckClose(TLineSegment, 0.0f);
		ZETestCheckClose(TRay, 10.576907f);
	}
	ZETest("float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegmentA, const ZELineSegment& LineSegmentB)")
	{
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);
		ZEVector3 P3(5.0f, 8.0f, 2.0f);
		ZEVector3 P4(8.0f, 15.0f, 1.0f);

		ZELineSegment LineSegmentA(P1, P2);
		ZELineSegment LineSegmentB(P3, P4);

		float Distance = ZELineSegment::MinimumDistance(LineSegmentA, LineSegmentB);
		ZETestCheckEqual(Distance, 6.5193453f); 
	}
	ZETest("float ZELineSegment::MinimumDistance(const ZELineSegment& LineSegmentA, const ZELineSegment& LineSegmentB, float& TLineSegmentA, float &TLineSegmentB)")
	{
		ZEVector3 P1(0.0f, 3.0f, 0.0f);
		ZEVector3 P2(1.0f, 4.0f, -1.0f);
		ZEVector3 P3(5.0f, 8.0f, 2.0f);
		ZEVector3 P4(8.0f, 15.0f, 1.0f);

		ZELineSegment LineSegmentA(P1, P2);
		ZELineSegment LineSegmentB(P3, P4);

		float TLineSegmentA, TLineSegmentB;
		float Distance = ZELineSegment::MinimumDistance(LineSegmentA, LineSegmentB, TLineSegmentA, TLineSegmentB);
		ZETestCheckEqual(Distance, 6.5193453f);
		ZETestCheckEqual(TLineSegmentA, 0.0f);
		ZETestCheckEqual(TLineSegmentB, 1.0000000f);
	}
}
