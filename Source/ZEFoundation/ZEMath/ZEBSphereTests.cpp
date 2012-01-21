//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEBSphereTests.cpp
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
#include "ZEMathIOStreamMapping.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEPlane.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZERay.h"
#include "ZEMath/ZELineSegment.h"
#include "ZEMath/ZEBSphere.h"
#include "ZEMath/ZEOBBox.h"
#include "ZEMath/ZEMath.h"

bool ZETestInternalCheckClose(const ZEVector3& Actual, const ZEVector3& Expected, const ZEVector3& Threshold = ZEVector3(ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD));


ZETestSuite(ZEBSphere)
{
	ZETest("void ZEBSphere::GetSurfaceNormal(ZEVector3& Normal, const ZEBSphere& BoundingSphere, const ZEVector3& Point)")
	{
		ZEBSphere BSphere(ZEVector3::Zero, 0.5f);
		ZEVector3 PointX(0.5f, 0.0f, 0.0f);
		ZEVector3 ResultX;

		ZEBSphere::GetSurfaceNormal(ResultX, BSphere, PointX);

		ZEVector3 PointY(0.0f, 0.5f, 0.0f);
		ZEVector3 ResultY;

		ZEBSphere::GetSurfaceNormal(ResultY, BSphere, PointY);

		ZEVector3 PointZ(0.0f, 0.0f, 0.5f);
		ZEVector3 ResultZ;

		ZEBSphere::GetSurfaceNormal(ResultZ, BSphere, PointZ);

		ZETestCheckClose(ResultX, ZEVector3::UnitX);
		ZETestCheckClose(ResultY, ZEVector3::UnitY);
		ZETestCheckClose(ResultZ, ZEVector3::UnitZ);

		ZETestCase("Normal for a random point on the ZEBSphere ")
		{
			ZEVector3 RandomResult;
			ZEVector3 RandomPoint(0.27284408f, 0.35951829f, -0.21518064f);
			ZELine ValidationLine(ZEVector3::Zero, RandomPoint);

			ZEBSphere::GetSurfaceNormal(RandomResult, BSphere, RandomPoint);

			ZETestCheckClose(RandomResult, ValidationLine.v);
		}
	}

	ZETest("bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZEAABBox& BoundingBox)")
	{
		ZEAABBox AABBox(ZEVector3(2.8628070f, 2.3242471f, 1.1984191f), ZEVector3(5.2628069f, 4.7242470f, 3.5984192f));

		ZETestCase("ZEBSphere is intersecting with ZEAABBox")
		{
			ZEBSphere BSphere(ZEVector3(4.1487913f, 3.5950582f, 3.5657606f), 0.50000000f);

			ZETestCheck(ZEBSphere::IntersectionTest(BSphere, AABBox));
		}

		ZETestCase("ZEBSphere is not intersecting with ZEAABBox")
		{
			ZEBSphere BSphere(ZEVector3(5.7484822f, 4.8597264f, 3.6342573f), 0.50000000f);

			ZETestCheck(!(ZEBSphere::IntersectionTest(BSphere, AABBox)));
		}

		ZETestCase("ZEBSphere is inside ZEAABBox")
		{
			ZEBSphere BSphere(ZEVector3(4.0418930f, 3.4679542f, 2.4831066f), 0.50000000f);

			ZETestCheck(ZEBSphere::IntersectionTest(BSphere, AABBox));
		}
	}

	ZETest("bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZELine& Line)")
	{
		ZEBSphere BSphere(ZEVector3(10.000000f, 10.000000f, 10.000000f), 0.50000000f);

		ZELine Line;
		Line.v = ZEVector3(0.55743307f, -0.15931815f, -0.81479216f);
		Line.p = ZEVector3(7.7504172f, 10.595081f, 13.302707f);

		ZETestCheck(ZEBSphere::IntersectionTest(BSphere, Line));
	}

	ZETest("bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZELine& Line, float& MinT)")
	{
		ZEBSphere BSphere(ZEVector3(10.000000f, 10.000000f, 10.000000f), 0.50000000f);

		ZELine Line;
		Line.v = ZEVector3(0.55743307f, -0.15931815f, -0.81479216f);
		Line.p = ZEVector3(7.7504172f, 10.595081f, 13.302707f);

		float Tmin = 0.0f;

		ZETestCheck(ZEBSphere::IntersectionTest(BSphere, Line, Tmin));
		ZETestCheckClose(Tmin, 3.5423064f);

	}

	ZETest("bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZELine& Line, float& MinT, float& MaxT)")
	{

		ZEBSphere BSphere(ZEVector3(10.000000f, 10.000000f, 10.000000f), 0.50000000f);

		ZELine Line;
		Line.v = ZEVector3(0.55743307f, -0.15931815f, -0.81479216f);
		Line.p = ZEVector3(7.7504172f, 10.595081f, 13.302707f);

		float Tmin = 0.0f;
		float Tmax = 0.0f;

		ZETestCheck(ZEBSphere::IntersectionTest(BSphere, Line, Tmin, Tmax));
		ZETestCheckClose(Tmin, 3.5423064f);
		ZETestCheckClose(Tmax, 4.5373311f);

	}

	ZETest("bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZELineSegment& LineSegment)")
	{
		ZEBSphere BSphere(ZEVector3(10.000000f, 10.000000f, 10.000000f), 0.50000000f);

		ZELineSegment LineSegment;
		LineSegment.v = ZEVector3(0.75925839f, 0.34364790f, 0.55265969f);
		LineSegment.p = ZEVector3(9.7715054f, 9.8616552f, 9.8219385f);
		LineSegment.Length = 3.8951080f;

		ZETestCheck(ZEBSphere::IntersectionTest(BSphere, LineSegment)); 
	}

	ZETest("bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZELineSegment& LineSegment, float& MinT)")
	{
		ZEBSphere BSphere(ZEVector3(10.000000f, 10.000000f, 10.000000f), 0.50000000f);

		ZELineSegment LineSegment;
		LineSegment.v = ZEVector3(0.75925839f, 0.34364790f, 0.55265969f);
		LineSegment.p = ZEVector3(9.7715054f, 9.8616552f, 9.8219385f);
		LineSegment.Length = 3.8951080f;

		float Tmin = 0.0f;

		ZETestCheck(ZEBSphere::IntersectionTest(BSphere, LineSegment, Tmin)); 
		ZETestCheckClose(Tmin, 0.81841898f);
	}

	ZETest("bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZELineSegment& LineSegment, float& MinT, float& MaxT)")
	{
		ZEBSphere BSphere(ZEVector3(10.000000f, 10.000000f, 10.000000f), 0.50000000f);

		ZELineSegment LineSegment;
		LineSegment.v = ZEVector3(0.75925839f, 0.34364790f, 0.55265969f);
		LineSegment.p = ZEVector3(9.7715054f, 9.8616552f, 9.8219385f);
		LineSegment.Length = 3.8951080f;

		float Tmin = 0.0f;
		float Tmax = 0.0f;

		ZETestCheck(ZEBSphere::IntersectionTest(BSphere, LineSegment, Tmin, Tmax)); 
		ZETestCheckClose(Tmin, 0.81841898f);
		ZETestCheckClose(Tmax, 0.81841898f);
	}

	ZETest("bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZEOBBox& BoundingBox)")
	{
		ZEOBBox OBBox;
		OBBox.Center = ZEVector3(6.3469343f, 1.6893826f, 4.0636616f);
		OBBox.Right = ZEVector3(0.84767789f, 0.091925949f, 0.52248615f);
		OBBox.Up = ZEVector3(0.19911075f, 0.85774630f, -0.47394738f);
		OBBox.Front = ZEVector3(-0.49172863f, 0.50578731f, 0.70878917f);
		OBBox.HalfSize = ZEVector3(0.50000000f, 0.50000000f, 0.50000000f);

		ZETestCase("ZEBSphere is intersecting with ZEOBBox")
		{
			ZEBSphere BSphere(ZEVector3(6.3338995f, 2.1556947f, 3.7471128f), 0.50000000f);

			ZETestCheck(ZEBSphere::IntersectionTest(BSphere, OBBox));
		}

		ZETestCase("ZEBSphere is not intersecting with ZEOBBox")
		{
			ZEBSphere BSphere(ZEVector3(5.8656621f, 2.1556947f, 4.8377638f), 0.50000000f);

			ZETestCheck(!(ZEBSphere::IntersectionTest(BSphere, OBBox)));
		}

		ZETestCase("ZEBSphere is on the same center point with ZEOBBox")
		{
			ZEBSphere BSphere(ZEVector3(6.3469343f, 1.6893826f, 4.0636616f), 0.50000000f);

			ZETestCheck(ZEBSphere::IntersectionTest(BSphere, OBBox));
		}

	}

	ZETest("ZEHalfSpace ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZEPlane& Plane)")
	{
		ZEBSphere BSphere(ZEVector3(10.000000f, 10.000000f, 10.000000f), 0.50000000f);

		ZETestCase("ZEBSphere is intersecting with ZEPlane")
		{
			ZEPlane Plane(ZEVector3(-0.69621903f, 0.71745402f, -0.023213712f), ZEVector3::Zero);
			ZETestCheckEqual(ZEBSphere::IntersectionTest(BSphere, Plane), ZE_HS_INTERSECTS);
		}

		ZETestCase("ZEBSphere is on positive side of the ZEPlane")
		{
			ZEPlane Plane(ZEVector3(0.43667358f, 0.87336510f, 0.21575357f), ZEVector3::Zero);
			ZETestCheckEqual(ZEBSphere::IntersectionTest(BSphere, Plane), ZE_HS_POSITIVE_SIDE);
		}

		ZETestCase("ZEBSphere is on negative side of the ZEPlane")
		{
			ZEPlane Plane(ZEVector3(-0.67470241f, 0.72473758f, -0.13975741f), ZEVector3::Zero);
			ZETestCheckEqual(ZEBSphere::IntersectionTest(BSphere, Plane), ZE_HS_NEGATIVE_SIDE);
		}
	}

	ZETest("bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZERay& Ray)")
	{
		ZEBSphere BSphere(ZEVector3(10.000000f, 10.000000f, 10.000000f), 0.50000000f);

		ZERay Ray;
		Ray.v = ZEVector3(-0.58191913f, -0.57352078f, -0.57657975f);
		Ray.p = ZEVector3(10.000567f, 9.6644440f, 9.9470882f);

		ZETestCheck(ZEBSphere::IntersectionTest(BSphere, Ray));
	}

	ZETest("bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZERay& Ray, float& MinT)")
	{
		ZEBSphere BSphere(ZEVector3(10.000000f, 10.000000f, 10.000000f), 0.50000000f);

		ZERay Ray;
		Ray.v = ZEVector3(-0.58191913f, -0.57352078f, -0.57657975f);
		Ray.p = ZEVector3(10.000567f, 9.6644440f, 9.9470882f);

		float Tmin = 0.0f;

		ZETestCheck(ZEBSphere::IntersectionTest(BSphere, Ray, Tmin));
		ZETestCheckClose(Tmin, 0.20651792f);
	}

	ZETest("bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZERay& Ray, float& MinT, float& MaxT)")
	{
		ZEBSphere BSphere(ZEVector3(10.000000f, 10.000000f, 10.000000f), 0.50000000f);

		ZERay Ray;
		Ray.v = ZEVector3(-0.58191913f, -0.57352078f, -0.57657975f);
		Ray.p = ZEVector3(10.000567f, 9.6644440f, 9.9470882f);

		float Tmin = 0.0f;
		float Tmax = 0.0f;

		ZETestCheck(ZEBSphere::IntersectionTest(BSphere, Ray, Tmin, Tmax));
		ZETestCheckClose(Tmin, 0.20651792f);
		ZETestCheckClose(Tmax, 0.20651792f);
	}

	ZETest("bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere, const ZEVector3& Point)")
	{
		ZEVector3 Point(5.5782952f, 1.7799615f, 3.5723135f);

		ZETestCase("ZEBSphere contains the point")
		{
			ZEBSphere BSphere(ZEVector3(5.6335278f, 1.7817698f, 3.5722213f), 0.50000000f);
			ZETestCheck(ZEBSphere::IntersectionTest(BSphere, Point));
		}

		ZETestCase("ZEBSphere does not contain the point")
		{
			ZEBSphere BSphere(ZEVector3(5.1590552f, 1.4696054f, 3.5722213f), 0.50000000f);
			ZETestCheck(!(ZEBSphere::IntersectionTest(BSphere, Point)));
		}
	}

	ZETest("bool ZEBSphere::IntersectionTest(const ZEBSphere& BoundingSphere1, const ZEBSphere& BoundingSphere2)")
	{
		ZEBSphere StationaryBSphere(ZEVector3(2.5424056f, 1.6334116f, 3.3176670f), 1.5000000f);

		ZETestCase("ZEBSpheres are intersecting")
		{
			ZEBSphere BSphere(ZEVector3(2.5110879f, 3.6074083f, 3.3110132f), 0.50000000f);
			ZETestCheck(ZEBSphere::IntersectionTest(StationaryBSphere, BSphere));
		}

		ZETestCase("ZEBSphere is inside the other ZEBSphere")
		{
			ZEBSphere BSphere(ZEVector3(2.5110879f, 1.5797714f, 3.3110132f), 0.50000000f);
			ZETestCheck(ZEBSphere::IntersectionTest(StationaryBSphere, BSphere));
		}

		ZETestCase("ZEBSpheres are not intersecting")
		{
			ZEBSphere BSphere(ZEVector3(3.8370419f, 2.0862474f, 5.6824718f), 0.50000000f);
			ZETestCheck(!(ZEBSphere::IntersectionTest(StationaryBSphere, BSphere)));
		}
	
	}

	static bool ZETestCheckBSphereLineIntersection(ZEVector3 OctantFactor = ZEVector3(0.0f, 0.0f, 0.0f))
	{
		bool Result = true;
		float Tmin = 0.0f;
		float Tmax = 0.0f;

		ZEBSphere BSphere(ZEVector3(10.000000f, 10.000000f, 10.000000f) + OctantFactor, 0.50000000f);

		//Line1 intersects ZEBSphere

		ZELine Line1;
		Line1.v = ZEVector3(0.55743307f, -0.15931815f, -0.81479216f);
		Line1.p = ZEVector3(7.7504172f, 10.595081f, 13.302707f) + OctantFactor;

		if(!(ZEBSphere::IntersectionTest(BSphere, Line1, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 3.5423064f) && ZETestInternalCheckClose(Tmax, 4.5373311f)))
			Result = false;

		//Line2 is the Polar axis of the ZEBSphere.

		ZELine Line2;
		Line2.v = ZEVector3(0.0f, -1.0f, 0.0f);
		Line2.p = ZEVector3(10.0f, 10.0f, 10.0f) + OctantFactor;

		if(!(ZEBSphere::IntersectionTest(BSphere, Line2, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, -0.50000000f) && ZETestInternalCheckClose(Tmax, 0.50000000f)))
			Result = false;

		//Line3 is not intersecting ZEBSphere

		ZELine Line3;
		Line3.v = ZEVector3(0.76012230f, 0.089084864f, -0.64364439f);
		Line3.p = ZEVector3(7.9298010f, 10.284784f, 11.760486f) + OctantFactor;

		if(ZEBSphere::IntersectionTest(BSphere, Line3))
			Result = false;

		return Result;
	}

	ZETest("ZEBSphere vs ZELine Intersection Test")
	{
		ZETestCheck(ZETestCheckBSphereLineIntersection()); //FRU Octant
		ZETestCheck(ZETestCheckBSphereLineIntersection(ZEVector3(0.0f, 0.0f, -20.0f))); //BRU Octant
		ZETestCheck(ZETestCheckBSphereLineIntersection(ZEVector3(-20.0f, 0.0f, 0.0f))); //FLU Octant
		ZETestCheck(ZETestCheckBSphereLineIntersection(ZEVector3(-20.0f, 0.0f, -20.0f))); //BLU Octant
		ZETestCheck(ZETestCheckBSphereLineIntersection(ZEVector3(0.0f, -20.0f, 0.0f))); //FRD Octant
		ZETestCheck(ZETestCheckBSphereLineIntersection(ZEVector3(0.0f, -20.0f, -20.0f))); //BRD Octant
		ZETestCheck(ZETestCheckBSphereLineIntersection(ZEVector3(-20.0f, -20.0f, 0.0f))); //FLD Octant
		ZETestCheck(ZETestCheckBSphereLineIntersection(ZEVector3(-20.0f, -20.0f, -20.0f))); //BLD Octant
	}

	static bool ZETestCheckBSphereLineSegmentIntersection(ZEVector3 OctantFactor = ZEVector3(0.0f, 0.0f, 0.0f))
	{
		bool Result = true;
		float Tmin = 0.0f;
		float Tmax = 0.0f;

		ZEBSphere BSphere(ZEVector3(10.000000f, 10.000000f, 10.000000f) + OctantFactor, 0.50000000f);

		//LineSegment1 is generated inside ZEBSphere and intersects it.

		ZELineSegment LineSegment1;
		LineSegment1.v = ZEVector3(0.75925839f, 0.34364790f, 0.55265969f);
		LineSegment1.p = ZEVector3(9.7715054f, 9.8616552f, 9.8219385f) + OctantFactor;
		LineSegment1.Length = 3.8951080f;

		if(!(ZEBSphere::IntersectionTest(BSphere, LineSegment1, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 0.81841898f) && ZETestInternalCheckClose(Tmax, 0.81841898f)))
			Result = false;

		ZELineSegment LineSegment2;
		LineSegment2.v = ZEVector3(0.00000000f, -1.0000000f, 0.00000000f);
		LineSegment2.p = ZEVector3(10.001863f, 10.759313f, 9.9986048f) + OctantFactor;
		LineSegment2.Length = 4.5312967f;

		if(!(ZEBSphere::IntersectionTest(BSphere, LineSegment2, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 0.25931805f) && ZETestInternalCheckClose(Tmax, 1.2593071f)))
			Result = false;

		//LineSegment3 is generated inside ZEBSphere but not intersecting it.

		ZELineSegment LineSegment3;
		LineSegment3.v = ZEVector3(0.86825013f, 0.10560116f, 0.48475799f);
		LineSegment3.p = ZEVector3(9.6363983f, 9.7894020f, 9.7872982f) + OctantFactor;
		LineSegment3.Length = 0.61226231f;

		if(ZEBSphere::IntersectionTest(BSphere, LineSegment3))
			Result = false;

		ZELineSegment LineSegment4;
		LineSegment4.v = ZEVector3(0.76129007f, -0.089085706f, -0.64226270f);
		LineSegment4.p = ZEVector3(8.0034475f, 10.744366f, 11.681805f) + OctantFactor;
		LineSegment4.Length = 5.1187778f;

		if(ZEBSphere::IntersectionTest(BSphere, LineSegment4))
			Result = false;

		return Result;
	}

	ZETest("ZEBSphere vs ZELine Intersection Test")
	{
		ZETestCheck(ZETestCheckBSphereLineSegmentIntersection()); //FRU Octant
		ZETestCheck(ZETestCheckBSphereLineSegmentIntersection(ZEVector3(0.0f, 0.0f, -20.0f))); //BRU Octant
		ZETestCheck(ZETestCheckBSphereLineSegmentIntersection(ZEVector3(-20.0f, 0.0f, 0.0f))); //FLU Octant
		ZETestCheck(ZETestCheckBSphereLineSegmentIntersection(ZEVector3(-20.0f, 0.0f, -20.0f))); //BLU Octant
		ZETestCheck(ZETestCheckBSphereLineSegmentIntersection(ZEVector3(0.0f, -20.0f, 0.0f))); //FRD Octant
		ZETestCheck(ZETestCheckBSphereLineSegmentIntersection(ZEVector3(0.0f, -20.0f, -20.0f))); //BRD Octant
		ZETestCheck(ZETestCheckBSphereLineSegmentIntersection(ZEVector3(-20.0f, -20.0f, 0.0f))); //FLD Octant
		ZETestCheck(ZETestCheckBSphereLineSegmentIntersection(ZEVector3(-20.0f, -20.0f, -20.0f))); //BLD Octant
	}

	static bool ZETestCheckBSphereRayIntersection(ZEVector3 OctantFactor = ZEVector3(0.0f, 0.0f, 0.0f))
	{
		bool Result = true;
		float Tmin = 0.0f;
		float Tmax = 0.0f;

		ZEBSphere BSphere(ZEVector3(10.000000f, 10.000000f, 10.000000f) + OctantFactor, 0.50000000f);

		//Ray1 is generated inside ZEBSphere and intersects it.

		ZERay Ray1;
		Ray1.v = ZEVector3(-0.58191913f, -0.57352078f, -0.57657975f);
		Ray1.p = ZEVector3(10.000567f, 9.6644440f, 9.9470882f) + OctantFactor;

		if(!(ZEBSphere::IntersectionTest(BSphere, Ray1, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 0.20651792f) && ZETestInternalCheckClose(Tmax, 0.20651792f)))
			Result = false;

		ZERay Ray2;
		Ray2.v = ZEVector3(0.93651170f, -0.34759009f, 0.046120405f);
		Ray2.p = ZEVector3(4.3607345f, 11.931516f, 9.8327894f) + OctantFactor;

		if(!(ZEBSphere::IntersectionTest(BSphere, Ray2, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 5.4961514f) && ZETestInternalCheckClose(Tmax, 6.4245000f)))
			Result = false;

		ZERay Ray3;
		Ray3.v = ZEVector3(0.82473522f, 0.019201333f, -0.56519306f);
		Ray3.p = ZEVector3(8.5050440f, 9.4574652f, 11.027550f) + OctantFactor;

		if(ZEBSphere::IntersectionTest(BSphere, Ray3))
			Result = false;

		return Result;
	}

	ZETest("ZEBSphere vs ZERay Intersection Test")
	{
		ZETestCheck(ZETestCheckBSphereRayIntersection()); //FRU Octant
		ZETestCheck(ZETestCheckBSphereRayIntersection(ZEVector3(0.0f, 0.0f, -20.0f))); //BRU Octant
		ZETestCheck(ZETestCheckBSphereRayIntersection(ZEVector3(-20.0f, 0.0f, 0.0f))); //FLU Octant
		ZETestCheck(ZETestCheckBSphereRayIntersection(ZEVector3(-20.0f, 0.0f, -20.0f))); //BLU Octant
		ZETestCheck(ZETestCheckBSphereRayIntersection(ZEVector3(0.0f, -20.0f, 0.0f))); //FRD Octant
		ZETestCheck(ZETestCheckBSphereRayIntersection(ZEVector3(0.0f, -20.0f, -20.0f))); //BRD Octant
		ZETestCheck(ZETestCheckBSphereRayIntersection(ZEVector3(-20.0f, -20.0f, 0.0f))); //FLD Octant
		ZETestCheck(ZETestCheckBSphereRayIntersection(ZEVector3(-20.0f, -20.0f, -20.0f))); //BLD Octant
	}
}
