//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPlaneTests.cpp
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
#include "ZEMath/ZEPlane.h"
#include "ZEMath/ZELine.h"
#include "ZEMath/ZERay.h"
#include "ZEMath/ZELineSegment.h"
#include "ZEMath/ZEMath.h"

bool ZETestInternalCheckClose(const ZEVector3& Actual, const ZEVector3& Expected, const ZEVector3& Threshold = ZEVector3(ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD));


ZETestSuite(ZEPlane)
{
	ZETest("ZEPlane::ZEPlane(const ZEVector3 & n, const ZEVector3 & p)")
	{
		ZEVector3 Normal(1.0f, 2.0f, 3.0f);
		ZEVector3 Position(0.0f, 6.0f, 0.0f);

		ZEPlane Plane(Normal.Normalize(), Position);

		ZETestCheckClose(Plane.n, ZEVector3(0.26726124f, 0.53452247f, 0.80178368f));
		ZETestCheckClose(Plane.p, Position);
	}
	ZETest("ZEPlane::ZEPlane(const ZEVector3 & n, const ZEVector3 & p) with usin normalize")
	{
		ZEVector3 Normal(1.0f, 2.0f, 3.0f);
		ZEVector3 Position(0.0f, 6.0f, 0.0f);

		ZEPlane Plane(Normal.Normalize(), Position);

		ZETestCheckClose(Plane.n, ZEVector3(0.26726124f, 0.53452247f, 0.80178368f));
		ZETestCheckClose(Plane.p, Position);
	}

	ZETest("inline void ZEPlane::Create(ZEPlane & Plane, const ZEVector3 & n, const ZEVector3 & p)")
	{
		ZEVector3 Normal(1.0f, 2.0f, 3.0f);
		ZEVector3 Position(0.0f, 6.0f, 0.0f);
		ZEPlane Plane;

		ZEPlane::Create(Plane, Normal.Normalize(), Position);
		
		ZETestCheckClose(Plane.n, ZEVector3(0.26726124f, 0.53452247f, 0.80178368f));
		ZETestCheckClose(Plane.p, Position);

	}

	ZETest("void ZEPlane::Create(ZEPlane & Plane, float a, float b, float c, float d)")
	{
		//Due to errors known this test is skipped. (Refer to Redmine : Bug #208)

		ZETestCheck(false);
	}
	ZETest("void ZEPlane::Create(ZEPlane & Plane, const ZEVector3 & P1, const ZEVector3 & P2, const ZEVector3 & P3)")
	{
		ZEVector3 P1(1.0f, 2.0f, 3.0f);
		ZEVector3 P2(0.0f, 3.0f, 1.0f);
		ZEVector3 P3(2.0f, 5.0f, 8.0f);
		ZEPlane Plane;

		ZEPlane::Create(Plane, P1, P2, P3);

		ZETestCheckClose(Plane.n, ZEVector3(11.0f, 3.0f, -4.0f));
		ZETestCheckClose(Plane.p, P3);
	}

	ZETest("float ZEPlane::Distance(const ZEPlane & Plane, const ZEVector3 & Point)")
	{
		ZEVector3 Position(-4.0f, 0.0f, 0.0f);
		ZEVector3 Normal(2.0f, -2.0f, 5.0f);
		ZEVector3 Point(4.0f, -4.0f, 3.0f);
		ZEPlane Plane(Normal.Normalize(), Position);
		float ExpectedValue = 6.7890286f;

		float Distance = ZEPlane::Distance(Plane, Point);

		ZETestCheckClose(Distance, ExpectedValue);
	}

	ZETest("float ZEPlane::DistanceSigned(const ZEPlane & Plane, const ZEVector3 & Point)")
	{
		ZEPlane Plane(ZEVector3::UnitY, ZEVector3::Zero);

		ZEVector3 Point = ZEVector3::UnitY;

		float Distance = ZEPlane::DistanceSigned(Plane, Point);

		ZETestCheckClose(Distance, 1.0f);

		ZETestCase("Point is on the negative side of the Plane")
		{
			Point = ZEVector3(0.0f, -1.0f, 0.0f);

			Distance = ZEPlane::DistanceSigned(Plane, Point);

			ZETestCheckClose(Distance, -1.0f);
		}
	}

	ZETest("bool ZEPlane::IntersectionTest(const ZEPlane& Plane, const ZELine& Line, float &t)")
	{
		ZEPlane Plane(ZEVector3(-0.45415181f, 0.84296310f, -0.28823227f), ZEVector3(10.000000f, 10.000000f, 10.000000f));

		ZELine Line;

		Line.v = ZEVector3(0.79146004f, 0.19866920f, 0.57803273f);
		Line.p = ZEVector3(8.5463676f, 10.049344f, 8.4865379f);

		float Tmin = 0.0f;

		ZETestCheck(ZEPlane::IntersectionTest(Plane, Line, Tmin));
		ZETestCheckClose(Tmin, 3.1736126f);
	}

	ZETest("bool ZEPlane::IntersectionTest(const ZEPlane & Plane, const ZELineSegment & LineSegment, float & t)")
	{
		ZEPlane Plane(ZEVector3(-0.38192877f, 0.58054721f, 0.71906567f), ZEVector3(10.000000f, 10.000000f, 9.9583693f));

		ZELineSegment LineSegment;

		LineSegment.v = ZEVector3(0.69936323f, -0.45733869f, -0.54930186f);
		LineSegment.p = ZEVector3(8.3901768f, 11.452430f, 10.347249f);
		LineSegment.Length = 2.2602010f;

		float Tmin = 0.0f;

		ZETestCheck(ZEPlane::IntersectionTest(Plane, LineSegment, Tmin));
		ZETestCheckClose(Tmin, 1.8733033f);
	}

	ZETest("bool ZEPlane::IntersectionTest(const ZEPlane & Plane, const ZERay & Ray, float & t)")
	{
		ZEPlane Plane(ZEVector3(0.00011430681f, 0.027386654f, 0.99959040f), ZEVector3(10.000000f, 10.000000f, 10.000000f));

		ZERay Ray;

		Ray.v = ZEVector3(0.020899549f, -0.99848789f, -0.050844338f);
		Ray.p = ZEVector3(9.7643900f, 13.467221f, 10.190653f);

		float Tmin = 0.0f;

		ZETestCheck(ZEPlane::IntersectionTest(Plane, Ray, Tmin));
		ZETestCheckClose(Tmin, 3.6525095f);
	}

	ZETest("bool ZEPlane::IntersectionTest(const ZEPlane & Plane1, const ZEPlane & Plane2, ZELine & Line)")
	{
		ZEPlane Plane(ZEVector3(-0.26467854f, 0.92264909f, -0.28022793f), ZEVector3(10.000000f, 10.000000f, 10.000000f));

		ZEPlane Plane2(ZEVector3(0.47116715f, 0.70716697f, 0.52717787f), ZEVector3(0.00000000f, 0.00000000f, 0.00000000f));

		ZELine IntersectionLine;

		ZETestCheck(ZEPlane::IntersectionTest(Plane, Plane2, IntersectionLine));
		ZETestCheckClose(IntersectionLine.v, ZEVector3(0.68456811f, 0.0074984729f, -0.62189388f));
		ZETestCheckClose(IntersectionLine.p, ZEVector3(-1.9594314f, 2.8874888f, -2.1220863f));

		ZETestCase("A Plane is perpendicular-like to the other Plane")
		{
			ZEPlane Plane3(ZEVector3(0.80723447f, -0.11843055f, -0.57823169f), ZEVector3(10.000000f, 10.000000f, 9.8403788f));
			ZEPlane Plane4(ZEVector3(-0.24958232f, 0.75233221f, -0.60967606f), ZEVector3(0.00000000f, 0.00000000f, 0.00000000f));

			ZETestCheck(ZEPlane::IntersectionTest(Plane3, Plane4, IntersectionLine));
			ZETestCheckClose(IntersectionLine.v, ZEVector3(0.50722659f, 0.63646793f, 0.57775033f));
			ZETestCheckClose(IntersectionLine.p, ZEVector3(0.98940450f, -0.19849156f, -0.64996719f));
		}
	}

	ZETest("ZEHalfSpace ZEPlane::TestHalfSpace(const ZEPlane & Plane, const ZEVector3 & Point)")
	{
		ZEPlane Plane1(ZEVector3(-0.34949848f, -0.59127051f, -0.72622430f), ZEVector3(10.000000f, 10.000000f, 10.000000f));

		ZEVector3 Point1(3.0925016f, 4.8406324f, 3.4473403f);

		ZETestCheckEqual(ZEPlane::TestHalfSpace(Plane1, Point1), ZE_HS_POSITIVE_SIDE);

		ZETestCase("Point is on the negative side of Plane")
		{
			ZEPlane Plane2(ZEVector3(0.69542390f, 0.71835214f, -0.014615774f), ZEVector3(10.000000f, 10.000000f, 10.000000f));

			ZEVector3 Point2(4.1709037f, 2.8175602f, 1.2879727f);

			ZETestCheckEqual(ZEPlane::TestHalfSpace(Plane2, Point2), ZE_HS_NEGATIVE_SIDE);
		}
		ZETestCase("Point is on the Plane")
		{
			ZEPlane Plane3(ZEVector3::UnitY, ZEVector3::Zero);

			ZEVector3 Point3 = ZEVector3::UnitZ;

			ZETestCheckEqual(ZEPlane::TestHalfSpace(Plane3, Point3), ZE_HS_ON_PLANE);
		}
	}

	static bool ZETestCheckPlaneLineIntersection(ZEVector3 OctantFactor = ZEVector3(0.0f, 0.0f, 0.0f))
	{
		bool Result = true;
		float Tmin = 0.0f;

		ZEPlane Plane(ZEVector3(-0.45415181f, 0.84296310f, -0.28823227f), ZEVector3(10.000000f, 10.000000f, 10.000000f) + OctantFactor);

		ZELine Line1;

		Line1.v = ZEVector3(0.79146004f, 0.19866920f, 0.57803273f);
		Line1.p = ZEVector3(8.5463676f, 10.049344f, 8.4865379f) + OctantFactor;

		if(!(ZEPlane::IntersectionTest(Plane, Line1, Tmin) && ZETestInternalCheckClose(Tmin, 3.1736126f)))
			Result = false;

		ZELine Line2;

		Line2.v = ZEVector3(0.00000000f, 1.0000001f, 0.00000000f);
		Line2.p = ZEVector3(11.467774f, 7.7997770f, 10.874997f) + OctantFactor;

		if(!(ZEPlane::IntersectionTest(Plane, Line2, Tmin) && ZETestInternalCheckClose(Tmin, 3.2901812f)))
			Result = false;

		//Line3 is on the Plane

		ZELine Line3(ZEVector3(10.000000f, 10.000000f, 10.000000f) + OctantFactor, ZEVector3(11.467774f, 11.089958f, 10.874997f) + OctantFactor);

		if(!(ZEPlane::IntersectionTest(Plane, Line3, Tmin) && ZETestInternalCheckClose(Tmin, 0.00000000f)))
			Result = false;

		return Result;
	}

	ZETest("ZEPlane vs ZELine Intersection Test")
	{
		ZETestCheck(ZETestCheckPlaneLineIntersection()); //FRU Octant
		ZETestCheck(ZETestCheckPlaneLineIntersection(ZEVector3(0.0f, 0.0f, -20.0f))); //BRU Octant
		ZETestCheck(ZETestCheckPlaneLineIntersection(ZEVector3(-20.0f, 0.0f, 0.0f))); //FLU Octant
		ZETestCheck(ZETestCheckPlaneLineIntersection(ZEVector3(-20.0f, 0.0f, -20.0f))); //BLU Octant
		ZETestCheck(ZETestCheckPlaneLineIntersection(ZEVector3(0.0f, -20.0f, 0.0f))); //FRD Octant
		ZETestCheck(ZETestCheckPlaneLineIntersection(ZEVector3(0.0f, -20.0f, -20.0f))); //BRD Octant
		ZETestCheck(ZETestCheckPlaneLineIntersection(ZEVector3(-20.0f, -20.0f, 0.0f))); //FLD Octant
		ZETestCheck(ZETestCheckPlaneLineIntersection(ZEVector3(-20.0f, -20.0f, -20.0f))); //BLD Octant
	}

	static bool ZETestCheckPlaneLineSegmentIntersection(ZEVector3 OctantFactor = ZEVector3(0.0f, 0.0f, 0.0f))
	{
		bool Result = true;
		float Tmin = 0.0f;

		ZEPlane Plane(ZEVector3(-0.38192877f, 0.58054721f, 0.71906567f), ZEVector3(10.000000f, 10.000000f, 9.9583693f) + OctantFactor);

		ZELineSegment LineSegment1;

		LineSegment1.v = ZEVector3(0.69936323f, -0.45733869f, -0.54930186f);
		LineSegment1.p = ZEVector3(8.3901768f, 11.452430f, 10.347249f) + OctantFactor;
		LineSegment1.Length = 2.2602010f;

		if(!(ZEPlane::IntersectionTest(Plane, LineSegment1, Tmin) && ZETestInternalCheckClose(Tmin, 1.8733033f)))
			Result = false;

		//LineSegment2 is on the Plane

		ZELineSegment LineSegment2(ZEVector3(10.000000f, 10.000000f, 9.9583693f) + OctantFactor, ZEVector3(9.7002964f, 10.595695f, 9.3182402f) + OctantFactor);

		if(!(ZEPlane::IntersectionTest(Plane, LineSegment2, Tmin) && ZETestInternalCheckClose(Tmin, 0.00000000f)))
			Result = false;

		//LineSegment3 is not intersecting with the Plane

		ZELineSegment LineSegment3;

		LineSegment3.v = ZEVector3(-0.33849865f, 0.41687036f, 0.84358627f);
		LineSegment3.p = ZEVector3(10.903369f, 8.2332048f, 7.6489062f) + OctantFactor;
		LineSegment3.Length = 2.7092500f;

		if(ZEPlane::IntersectionTest(Plane, LineSegment3, Tmin))
			Result = false;

		return Result;
	}

	ZETest("ZEPlane vs ZELineSegment Intersection Test")
	{
		ZETestCheck(ZETestCheckPlaneLineSegmentIntersection()); //FRU Octant
		ZETestCheck(ZETestCheckPlaneLineSegmentIntersection(ZEVector3(0.0f, 0.0f, -20.0f))); //BRU Octant
		ZETestCheck(ZETestCheckPlaneLineSegmentIntersection(ZEVector3(-20.0f, 0.0f, 0.0f))); //FLU Octant
		ZETestCheck(ZETestCheckPlaneLineSegmentIntersection(ZEVector3(-20.0f, 0.0f, -20.0f))); //BLU Octant
		ZETestCheck(ZETestCheckPlaneLineSegmentIntersection(ZEVector3(0.0f, -20.0f, 0.0f))); //FRD Octant
		ZETestCheck(ZETestCheckPlaneLineSegmentIntersection(ZEVector3(0.0f, -20.0f, -20.0f))); //BRD Octant
		ZETestCheck(ZETestCheckPlaneLineSegmentIntersection(ZEVector3(-20.0f, -20.0f, 0.0f))); //FLD Octant
		ZETestCheck(ZETestCheckPlaneLineSegmentIntersection(ZEVector3(-20.0f, -20.0f, -20.0f))); //BLD Octant
	}

	static bool ZETestCheckPlaneRayIntersection(ZEVector3 OctantFactor = ZEVector3(0.0f, 0.0f, 0.0f))
	{
		bool Result = true;
		float Tmin = 0.0f;

		ZEPlane Plane(ZEVector3(0.00011430681f, 0.027386654f, 0.99959040f), ZEVector3(10.000000f, 10.000000f, 10.000000f) + OctantFactor);

		ZERay Ray1;

		Ray1.v = ZEVector3(0.020899549f, -0.99848789f, -0.050844338f);
		Ray1.p = ZEVector3(9.7643900f, 13.467221f, 10.190653f) + OctantFactor;

		if(!(ZEPlane::IntersectionTest(Plane, Ray1, Tmin) && ZETestInternalCheckClose(Tmin, 3.6525095f)))
			Result = false;

		//Ray2 is not intersecting with the Plane

		ZERay Ray2;

		Ray2.v = ZEVector3(-0.0062648291f, 0.99938762f, -0.034427460f);
		Ray2.p = ZEVector3(9.7939138f, 9.1170921f, 9.8901415f) + OctantFactor;

		if(ZEPlane::IntersectionTest(Plane, Ray2, Tmin))
			Result = false;

		return Result;
	}

	ZETest("ZEPlane vs ZERay Intersection Test")
	{
		ZETestCheck(ZETestCheckPlaneRayIntersection()); //FRU Octant
		ZETestCheck(ZETestCheckPlaneRayIntersection(ZEVector3(0.0f, 0.0f, -20.0f))); //BRU Octant
		ZETestCheck(ZETestCheckPlaneRayIntersection(ZEVector3(-20.0f, 0.0f, 0.0f))); //FLU Octant
		ZETestCheck(ZETestCheckPlaneRayIntersection(ZEVector3(-20.0f, 0.0f, -20.0f))); //BLU Octant
		ZETestCheck(ZETestCheckPlaneRayIntersection(ZEVector3(0.0f, -20.0f, 0.0f))); //FRD Octant
		ZETestCheck(ZETestCheckPlaneRayIntersection(ZEVector3(0.0f, -20.0f, -20.0f))); //BRD Octant
		ZETestCheck(ZETestCheckPlaneRayIntersection(ZEVector3(-20.0f, -20.0f, 0.0f))); //FLD Octant
		ZETestCheck(ZETestCheckPlaneRayIntersection(ZEVector3(-20.0f, -20.0f, -20.0f))); //BLD Octant
	}
}
