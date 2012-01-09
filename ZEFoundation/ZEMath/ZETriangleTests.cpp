//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETriangleTests.cpp
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

#include <math.h>
#include "ZETest.h"
#include "ZEMath/ZETriangle.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZELineSegment.h"
#include "ZEMath/ZERay.h"
#include "ZEMath/ZEMathDefinitions.h"
#include "ZEMathIOStreamMapping.h"


bool ZETestInternalCheckClose(const ZEVector3& Actual, const ZEVector3& Expected, const ZEVector3& Threshold = ZEVector3(ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD));

ZETestSuite(ZETriangle)
{

	ZETest("void ZETriangle::Create(ZETriangle& Triangle, const ZEVector3& V0, const ZEVector3& V1, const ZEVector3& V2)")
	{
		ZEVector3 Point0(10.000000f, 10.000000f, 11.154700f);
		ZEVector3 Point1(11.000000f, 10.000000f, 9.4226494f);
		ZEVector3 Point2(9.0000000f, 10.000000f, 9.4226494f);

		ZETriangle TriangleManual;
		TriangleManual.V0 = Point0;
		TriangleManual.V1 = Point1;
		TriangleManual.V2 = Point2;

		ZETriangle TriangleFromCreate;
		ZETriangle::Create(TriangleFromCreate, Point0, Point1, Point2);

		ZETestCheckClose(TriangleManual.V0, TriangleFromCreate.V0);
		ZETestCheckClose(TriangleManual.V1, TriangleFromCreate.V1);
		ZETestCheckClose(TriangleManual.V2, TriangleFromCreate.V2);
	}

	ZETest("void ZETriangle::GetBarrycentiricCoordinates(const ZETriangle& Triangle, const ZEVector3& Point, ZEVector3& BarryCoords)")
	{
		//Due to errors known this test is skipped. (Refer to Redmine : Bug #222)

		ZETestCheck(false);
	}

	ZETest("void ZETriangle::GetNormal(const ZETriangle& Triangle, ZEVector3& Normal)")
	{
		ZEVector3 P0(10.000000f, 10.000000f, 11.154700f);
		ZEVector3 P1(11.000000f, 10.000000f, 9.4226494f);
		ZEVector3 P2(9.0000000f, 10.000000f, 9.4226494f);
		ZETriangle Triangle(P0, P1, P2);
		ZEVector3 Normal;

		ZETriangle::GetNormal(Triangle, Normal);

		ZETestCheckClose(Normal, ZEVector3::UnitY);

		ZETestCase("Triangle is rotated.")
		{
			ZEVector3 P3;
			ZEVector3 P4;
			ZEVector3 P5;

			ZEMatrix4x4 Rotation;
			ZEMatrix4x4::CreateRotation(Rotation, ZEQuaternion(ZE_PI_2, ZEVector3::UnitZ));

			ZEMatrix4x4::Transform(P3, Rotation, P0);
			ZEMatrix4x4::Transform(P4, Rotation, P1);
			ZEMatrix4x4::Transform(P5, Rotation, P2);

			Triangle.V0 = P3;
			Triangle.V1 = P4;
			Triangle.V2 = P5;

			ZETriangle::GetNormal(Triangle, Normal);

			ZETestCheckClose(Normal, ZEVector3::UnitX);
		}
	}

	ZETest("void ZETriangle::GetSurfacePlane(const ZETriangle& Triangle, ZEPlane& Plane)")
	{
		ZEVector3 P0(10.000000f, 10.000000f, 11.154700f);
		ZEVector3 P1(11.000000f, 10.000000f, 9.4226494f);
		ZEVector3 P2(9.0000000f, 10.000000f, 9.4226494f);
		ZETriangle Triangle(P0, P1, P2);
		ZEPlane Plane;

		ZETriangle::GetSurfacePlane(Triangle, Plane);

		ZETestCheckClose(Plane.p, ZEVector3(11.000000f, 10.000000f, 9.4226494f));
		ZETestCheckClose(Plane.n, ZEVector3::UnitY);
	}

	ZETest("bool ZETriangle::InsideTest(const ZETriangle& Triangle, const ZEVector3& Point)")
	{
		ZEVector3 P0(3.9586830f, 1.6137114f, 5.5713701f);
		ZEVector3 P1(4.7865424f, 2.1746445f, 3.8393197f);
		ZEVector3 P2(3.1308239f, 1.0527782f, 3.8393197f);
		ZETriangle Triangle(P0, P1, P2);

		ZEVector3 Point(5.1249733f, 0.90792572f, 4.3332620f);

		ZETestCheck(ZETriangle::InsideTest(Triangle, Point));

		ZETestCase("Triangle is not encapsulating the Point")
		{
			ZEVector3 Point2(5.1612906f, 3.1676991f, 1.6791284f);

			ZETestCheck(!ZETriangle::InsideTest(Triangle, Point2));
		}
	}

	ZETest("bool ZETriangle::IntersectionTest(const ZETriangle& Triangle, const ZELine& Line, float &t)")
	{
		ZEVector3 P0(9.0017929f, 10.580025f, 9.8238201f);
		ZEVector3 P1(10.948232f, 10.496342f, 10.275554f);
		ZEVector3 P2(10.049975f, 8.9236336f, 9.4273300f);
		ZETriangle Triangle(P0, P1, P2);

		ZELine Line;
		Line.v = ZEVector3(-0.38718852f, -0.39475161f, 0.83322042f);
		Line.p = ZEVector3(12.062664f, 11.787186f, 6.9270778f);

		float Tmin = 0.0f;

		ZETestCheck(ZETriangle::IntersectionTest(Triangle, Line, Tmin));
		ZETestCheckClose(Tmin, 3.8094397f);
	}

	ZETest("bool ZETriangle::IntersectionTest(const ZETriangle& Triangle, const ZELineSegment& LineSegment, float &t)")
	{
		ZEVector3 P0(9.0347013f, 9.6489248f, 10.527431f);
		ZEVector3 P1(10.749244f, 10.678031f, 10.558678f);
		ZEVector3 P2(10.216055f, 9.6730442f, 8.9138918f);
		ZETriangle Triangle(P0, P1, P2);

		ZELineSegment LineSegment;
		LineSegment.v = ZEVector3(0.50670159f, -0.70327902f, 0.49865022f);
		LineSegment.p = ZEVector3(8.8860369f, 12.257408f, 8.7163496f);
		LineSegment.Length = 4.3358254f;

		float Tmin = 0.0f;

		ZETestCheck(ZETriangle::IntersectionTest(Triangle, LineSegment, Tmin));
		ZETestCheckClose(Tmin, 2.8498566f);
	}

	ZETest("bool ZETriangle::IntersectionTest(const ZETriangle& Triangle, const ZERay& Ray, float &t)")
	{	
		ZETriangle Triangle;
		Triangle.V0 = ZEVector3(9.0087843f, 9.5797234f, 10.261058f);
		Triangle.V1 = ZEVector3(10.600632f, 10.774746f, 10.453930f);
		Triangle.V2 = ZEVector3(10.390584f, 9.6455307f, 8.8169260f);

		ZERay Ray;
		Ray.v = ZEVector3(0.53839123f, -0.70682514f, 0.45883888f);
		Ray.p = ZEVector3(7.2682037f, 14.054374f, 7.5404005f);

		float Tmin = 0.0f;

		ZETestCheck(ZETriangle::IntersectionTest(Triangle, Ray, Tmin));
		ZETestCheckClose(Tmin, 5.4100742f);
	}

	ZETest("bool ZETriangle::IntersectionTest(const ZETriangle& Triangle, const ZEPlane & Plane2, ZELine & Line)")
	{
		//Due to errors known this test is skipped. (Refer to Redmine : Bug #320)
		ZETestCheck(false);
	}

	static bool ZETestCheckTriangleLineIntersection(ZEVector3 OctantFactor = ZEVector3(0.0f, 0.0f, 0.0f))
	{
		bool Result = true;
		float Tmin = 0.0f;

		ZETriangle Triangle;

		Triangle.V0 = ZEVector3(9.0017929f, 10.580025f, 9.8238201f) + OctantFactor;
		Triangle.V1 = ZEVector3(10.948232f, 10.496342f, 10.275554f) + OctantFactor;
		Triangle.V2 = ZEVector3(10.049975f, 8.9236336f, 9.4273300f) + OctantFactor;

		ZELine Line1;
		Line1.v = ZEVector3(-0.38718852f, -0.39475161f, 0.83322042f);
		Line1.p = ZEVector3(12.062664f, 11.787186f, 6.9270778f) + OctantFactor;

		if(!(ZETriangle::IntersectionTest(Triangle, Line1, Tmin) && ZETestInternalCheckClose(Tmin, 3.8094397f)))
			Result = false;

		ZELine Line2;
		Line2.v = ZEVector3(0.98387963f, -0.16010539f, 0.079670519f);
		Line2.p = ZEVector3(8.0869722f, 10.140938f, 9.7258902f) + OctantFactor;

		if(ZETriangle::IntersectionTest(Triangle, Line2, Tmin))
			Result = false;

		return Result;
	}

	ZETest("ZETriangle vs ZELine Intersection Test")
	{
		ZETestCheck(ZETestCheckTriangleLineIntersection()); //BRU Octant
		ZETestCheck(ZETestCheckTriangleLineIntersection(ZEVector3(0.0f, 0.0f, -20.0f))); //FRU Octant
		ZETestCheck(ZETestCheckTriangleLineIntersection(ZEVector3(-20.0f, 0.0f, 0.0f))); //BLU Octant
		ZETestCheck(ZETestCheckTriangleLineIntersection(ZEVector3(-20.0f, 0.0f, -20.0f))); //FLU Octant
		ZETestCheck(ZETestCheckTriangleLineIntersection(ZEVector3(0.0f, -20.0f, 0.0f))); //BRD Octant
		ZETestCheck(ZETestCheckTriangleLineIntersection(ZEVector3(0.0f, -20.0f, -20.0f))); //FRD Octant
		ZETestCheck(ZETestCheckTriangleLineIntersection(ZEVector3(-20.0f, -20.0f, 0.0f))); //BLD Octant
		ZETestCheck(ZETestCheckTriangleLineIntersection(ZEVector3(-20.0f, -20.0f, -20.0f))); //FLD Octant
	}

	static bool ZETestCheckTriangleLineSegmentIntersection(ZEVector3 OctantFactor = ZEVector3(0.0f, 0.0f, 0.0f))
	{
		bool Result = true;
		float Tmin = 0.0f;

		ZETriangle Triangle;

		Triangle.V0 = ZEVector3(9.0347013f, 9.6489248f, 10.527431f) + OctantFactor;
		Triangle.V1 = ZEVector3(10.749244f, 10.678031f, 10.558678f) + OctantFactor;
		Triangle.V2 = ZEVector3(10.216055f, 9.6730442f, 8.9138918f) + OctantFactor;

		ZELineSegment LineSegment1;
		LineSegment1.v = ZEVector3(0.50670159f, -0.70327902f, 0.49865022f);
		LineSegment1.p = ZEVector3(8.8860369f, 12.257408f, 8.7163496f) + OctantFactor;
		LineSegment1.Length = 4.3358254f;

		if(!(ZETriangle::IntersectionTest(Triangle, LineSegment1, Tmin) && ZETestInternalCheckClose(Tmin, 2.8498566f)))
			Result = false;

		ZELineSegment LineSegment2;
		LineSegment2.v = ZEVector3(-0.46256810f, -0.76432955f, -0.44925642f);
		LineSegment2.p = ZEVector3(12.639089f, 14.156202f, 12.431654f) + OctantFactor;
		LineSegment2.Length = 5.9912410f;

		if(!(ZETriangle::IntersectionTest(Triangle, LineSegment2, Tmin) && ZETestInternalCheckClose(Tmin, 5.2104802f)))
			Result = false;

		ZELineSegment LineSegment3;
		LineSegment3.v = ZEVector3(0.77742010f, 0.57286626f, 0.25969636f);
		LineSegment3.p = ZEVector3(8.6955757f, 8.8888540f, 9.9036570f) + OctantFactor;
		LineSegment3.Length = 5.7169409f;

		if(ZETriangle::IntersectionTest(Triangle, LineSegment3, Tmin))
			Result = false;

		return Result;

	}

	ZETest("ZETriangle vs ZELineSegment Intersection Test")
	{
		ZETestCheck(ZETestCheckTriangleLineSegmentIntersection()); //BRU Octant
		ZETestCheck(ZETestCheckTriangleLineSegmentIntersection(ZEVector3(0.0f, 0.0f, -20.0f))); //FRU Octant
		ZETestCheck(ZETestCheckTriangleLineSegmentIntersection(ZEVector3(-20.0f, 0.0f, 0.0f))); //BLU Octant
		ZETestCheck(ZETestCheckTriangleLineSegmentIntersection(ZEVector3(-20.0f, 0.0f, -20.0f))); //FLU Octant
		ZETestCheck(ZETestCheckTriangleLineSegmentIntersection(ZEVector3(0.0f, -20.0f, 0.0f))); //BRD Octant
		ZETestCheck(ZETestCheckTriangleLineSegmentIntersection(ZEVector3(0.0f, -20.0f, -20.0f))); //FRD Octant
		ZETestCheck(ZETestCheckTriangleLineSegmentIntersection(ZEVector3(-20.0f, -20.0f, 0.0f))); //BLD Octant
		ZETestCheck(ZETestCheckTriangleLineSegmentIntersection(ZEVector3(-20.0f, -20.0f, -20.0f))); //FLD Octant
	}

	static bool ZETestCheckTriangleRayIntersection(ZEVector3 OctantFactor = ZEVector3(0.0f, 0.0f, 0.0f))
	{
		bool Result = true;
		float Tmin = 0.0f;

		ZETriangle Triangle;

		Triangle.V0 = ZEVector3(9.0087843f, 9.5797234f, 10.261058f) + OctantFactor;
		Triangle.V1 = ZEVector3(10.600632f, 10.774746f, 10.453930f) + OctantFactor;
		Triangle.V2 = ZEVector3(10.390584f, 9.6455307f, 8.8169260f) + OctantFactor;

		ZERay Ray1;
		Ray1.v = ZEVector3(0.53839123f, -0.70682514f, 0.45883888f);
		Ray1.p = ZEVector3(7.2682037f, 14.054374f, 7.5404005f) + OctantFactor;

		if(!(ZETriangle::IntersectionTest(Triangle, Ray1, Tmin) && ZETestInternalCheckClose(Tmin, 5.4100742f)))
			Result = false;

		ZERay Ray2;
		Ray2.v = ZEVector3(0.72406358f, 0.024997098f, -0.68928021f);
		Ray2.p = ZEVector3(5.4548116f, 9.6055441f, 13.810198f) + OctantFactor;

		if(!(ZETriangle::IntersectionTest(Triangle, Ray2, Tmin) && ZETestInternalCheckClose(Tmin, 5.8756337f, 0.0001f)))
			Result = false;

		ZERay Ray3;
		Ray3.v = ZEVector3(-0.53216153f, -0.77124685f, -0.34925985f);
		Ray3.p = ZEVector3(11.897768f, 13.046916f, 11.116466f) + OctantFactor;

		if(ZETriangle::IntersectionTest(Triangle, Ray3, Tmin))
			Result = false;

		return Result;
	}


	ZETest("ZETriangle vs ZERay Intersection Test")
	{
		ZETestCheck(ZETestCheckTriangleRayIntersection()); //BRU Octant
		ZETestCheck(ZETestCheckTriangleRayIntersection(ZEVector3(0.0f, 0.0f, -20.0f))); //FRU Octant
		ZETestCheck(ZETestCheckTriangleRayIntersection(ZEVector3(-20.0f, 0.0f, 0.0f))); //BLU Octant
		ZETestCheck(ZETestCheckTriangleRayIntersection(ZEVector3(-20.0f, 0.0f, -20.0f))); //FLU Octant
		ZETestCheck(ZETestCheckTriangleRayIntersection(ZEVector3(0.0f, -20.0f, 0.0f))); //BRD Octant
		ZETestCheck(ZETestCheckTriangleRayIntersection(ZEVector3(0.0f, -20.0f, -20.0f))); //FRD Octant
		ZETestCheck(ZETestCheckTriangleRayIntersection(ZEVector3(-20.0f, -20.0f, 0.0f))); //BLD Octant
		ZETestCheck(ZETestCheckTriangleRayIntersection(ZEVector3(-20.0f, -20.0f, -20.0f))); //FLD Octant
	}

}
