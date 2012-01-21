//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEAABBoxTests.cpp
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
#include "ZEMath/ZEAngle.h"
#include <math.h>
#include "ZEQuaternion.h"

bool ZETestInternalCheckClose(const ZEVector3& Actual, const ZEVector3& Expected, const ZEVector3& Threshold = ZEVector3(ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD));

ZETestSuite(ZEAABBox)
{
	ZETest("void ZEAABBox::GenerateBoundingSphere(ZEBSphere& BoundingSphere, const ZEAABBox& BoundingBox)")
	{
		ZEAABBox AABBox(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f), ZEVector3(10.500000f, 10.500000f, 10.500000f));
		ZEBSphere Result;

		ZEAABBox::GenerateBoundingSphere(Result, AABBox);

		ZETestCheckClose(Result.Position, ZEVector3(10.000000f, 10.000000f, 10.000000f));
		ZETestCheckClose(Result.Radius, 0.86602539f);
	}

	ZETest("void ZEAABBox::GenerateOBoundingBox(ZEOBBox& OrientedBoundingBox, const ZEAABBox& BoundingBox)")
	{
		ZEAABBox AABBox(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f), ZEVector3(10.500000f, 10.500000f, 10.500000f));
		ZEOBBox Result;

		ZEAABBox::GenerateOBoundingBox(Result, AABBox);

		ZETestCheckClose(Result.Center, ZEVector3(10.000000f, 10.000000f, 10.000000f));
		ZETestCheckClose(Result.Right, ZEVector3(1.0000000f, 0.00000000f, 0.00000000f));
		ZETestCheckClose(Result.Up, ZEVector3(0.00000000f, 1.0000000f, 0.00000000f));
		ZETestCheckClose(Result.Front, ZEVector3(0.00000000f, 0.00000000f, 1.0000000f));
		ZETestCheckClose(Result.HalfSize, ZEVector3(0.50000000f, 0.50000000f, 0.50000000f));
	}

	ZETest("ZEVector3 ZEAABBox::GetCenter() const")
	{
		ZEAABBox AABBox(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f), ZEVector3(10.500000f, 10.500000f, 10.500000f));

		ZEVector3 Center = AABBox.GetCenter();

		ZETestCheckClose(Center, ZEVector3(10.000000f, 10.000000f, 10.000000f));
	}

	ZETest("float ZEAABBox::GetLenght() const")
	{
		ZEAABBox AABBox(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f), ZEVector3(10.500000f, 10.500000f, 10.500000f));
		
		float LengthOfDiagonal = AABBox.GetLenght();

		ZETestCheckClose(LengthOfDiagonal, ZEVector3::Distance(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f), ZEVector3(10.500000f, 10.500000f, 10.500000f)));
	}

	ZETest("ZEVector3 ZEAABBox::GetVertex(unsigned char Index) const")
	{
		ZEAABBox AABBox(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f), ZEVector3(10.500000f, 10.500000f, 10.500000f));
  
		ZETestCheckClose(AABBox.GetVertex(0), ZEVector3(9.5000000f, 9.5000000f, 9.5000000f));
		ZETestCheckClose(AABBox.GetVertex(1), ZEVector3(10.500000f, 9.5000000f, 9.5000000f));
		ZETestCheckClose(AABBox.GetVertex(2), ZEVector3(9.5000000f, 10.500000f, 9.5000000f));
		ZETestCheckClose(AABBox.GetVertex(3), ZEVector3(10.500000f, 10.500000f, 9.5000000f));
		ZETestCheckClose(AABBox.GetVertex(4), ZEVector3(9.5000000f, 9.5000000f, 10.500000f));
		ZETestCheckClose(AABBox.GetVertex(5), ZEVector3(10.500000f, 9.5000000f, 10.500000f));
		ZETestCheckClose(AABBox.GetVertex(6), ZEVector3(9.5000000f, 10.500000f, 10.500000f));
		ZETestCheckClose(AABBox.GetVertex(7), ZEVector3(10.500000f, 10.500000f, 10.500000f));
	}

	ZETest("bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZEBSphere& BoundingSphere)")
	{
		ZEAABBox AABBox(ZEVector3(1.9993184f, 5.0916834f, 4.0711741f), ZEVector3(2.9993184f, 6.0916834f, 5.0711741f));

		ZEBSphere BSphere(ZEVector3(2.3270669f, 5.8254647f, 4.0549030f), 0.50000000f);

		ZETestCheck(ZEAABBox::IntersectionTest(AABBox, BSphere));
	}

	ZETest("bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZELine& Line)")
	{
		ZEAABBox AABBox(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f), ZEVector3(10.500000f, 10.500000f, 10.500000f));

		ZELine Line;
		Line.v = ZEVector3(-0.99329460f, -0.11474650f, -0.014108747f);
		Line.p = ZEVector3(16.774904f, 10.661500f, 10.084741f);

		ZETestCheck(ZEAABBox::IntersectionTest(AABBox, Line));
	}

	ZETest("bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZELine& Line, float& TMin)")
	{
		ZEAABBox AABBox(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f), ZEVector3(10.500000f, 10.500000f, 10.500000f));

		ZELine Line;
		Line.v = ZEVector3(-0.99329460f, -0.11474650f, -0.014108747f);
		Line.p = ZEVector3(16.774904f, 10.661500f, 10.084741f);

		float Tmin = 0.0f;

		ZETestCheck(ZEAABBox::IntersectionTest(AABBox, Line, Tmin));
		ZETestCheckClose(Tmin, 6.3172641f);
	}

	ZETest("bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZELine& Line, float& TMin, float& TMax)")
	{
		ZEAABBox AABBox(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f), ZEVector3(10.500000f, 10.500000f, 10.500000f));

		ZELine Line;
		Line.v = ZEVector3(-0.99329460f, -0.11474650f, -0.014108747f);
		Line.p = ZEVector3(16.774904f, 10.661500f, 10.084741f);

		float Tmin = 0.0f;
		float Tmax = 0.0f;

		ZETestCheck(ZEAABBox::IntersectionTest(AABBox, Line, Tmin, Tmax));
		ZETestCheckClose(Tmin, 6.3172641f);
		ZETestCheckClose(Tmax, 7.3240147f);
	}

	ZETest("bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZELineSegment& Line)")
	{
		ZEAABBox AABBox(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f), ZEVector3(10.500000f, 10.500000f, 10.500000f));

		ZELineSegment LineSegment;
		LineSegment.v = ZEVector3(-0.99725264f, -0.065749310f, -0.034122884f);
		LineSegment.p = ZEVector3(12.655836f, 10.099570f, 10.100832f);
		LineSegment.Length = 3.5186925f;

		ZETestCheck(ZEAABBox::IntersectionTest(AABBox, LineSegment));
	}

	ZETest("bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZELineSegment& Line, float& TMin)")
	{
		ZEAABBox AABBox(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f), ZEVector3(10.500000f, 10.500000f, 10.500000f));

		ZELineSegment LineSegment;
		LineSegment.v = ZEVector3(-0.99725264f, -0.065749310f, -0.034122884f);
		LineSegment.p = ZEVector3(12.655836f, 10.099570f, 10.100832f);
		LineSegment.Length = 3.5186925f;

		float Tmin = 0.0f;

		ZETestCheck(ZEAABBox::IntersectionTest(AABBox, LineSegment, Tmin));
		ZETestCheckClose(Tmin, 2.1617754f);
	}

	ZETest("bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZELineSegment& LineSegment, float& TMin, float& TMax)")
	{
		ZEAABBox AABBox(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f), ZEVector3(10.500000f, 10.500000f, 10.500000f));

		ZELineSegment LineSegment;
		LineSegment.v = ZEVector3(-0.99725264f, -0.065749310f, -0.034122884f);
		LineSegment.p = ZEVector3(12.655836f, 10.099570f, 10.100832f);
		LineSegment.Length = 3.5186925f;

		float Tmin = 0.0f;
		float Tmax = 0.0f;

		ZETestCheck(ZEAABBox::IntersectionTest(AABBox, LineSegment, Tmin, Tmax));
		ZETestCheckClose(Tmin, 2.1617754f);
		ZETestCheckClose(Tmax, 3.1645303f);
	}

	ZETest("ZEHalfSpace ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZEPlane& Plane)")
	{
		ZEAABBox AABBox(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f), ZEVector3(10.500000f, 10.500000f, 10.500000f));

		ZETestCase("AABBox is intersecting the Plane")
		{
			ZEPlane Plane(ZEVector3(-0.36145043f, 0.79461271f, -0.48779538f), ZEVector3::Zero);
			ZETestCheckEqual(ZEAABBox::IntersectionTest(AABBox, Plane), ZE_HS_INTERSECTS);
		}

		ZETestCase("AABBox is on positive side of the Plane")
		{
			ZEPlane Plane(ZEVector3(0.56687558f, 0.77218467f, 0.28702420f), ZEVector3::Zero);
			ZETestCheckEqual(ZEAABBox::IntersectionTest(AABBox, Plane), ZE_HS_POSITIVE_SIDE);
		}

		ZETestCase("AABBox is on negative side of the Plane")
		{
			ZEPlane Plane(ZEVector3(-0.64037079f, 0.71312839f, -0.28525996f), ZEVector3::Zero);
			ZETestCheckEqual(ZEAABBox::IntersectionTest(AABBox, Plane), ZE_HS_NEGATIVE_SIDE);
		}
	}

	ZETest("bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZERay& Line)")
	{
		ZEAABBox AABBox(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f), ZEVector3(10.500000f, 10.500000f, 10.500000f));

		ZERay Ray;
		Ray.v = ZEVector3(-0.99582845f, -0.089878544f, 0.015733540f);
		Ray.p = ZEVector3(19.678036f, 11.065289f, 9.8985271f);

		ZETestCheck(ZEAABBox::IntersectionTest(AABBox, Ray));
	}

	ZETest("bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZERay& Line, float& TMin)")
	{
		ZEAABBox AABBox(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f), ZEVector3(10.500000f, 10.500000f, 10.500000f));

		ZERay Ray;
		Ray.v = ZEVector3(-0.99582845f, -0.089878544f, 0.015733540f);
		Ray.p = ZEVector3(19.678036f, 11.065289f, 9.8985271f);

		float Tmin = 0.0f;

		ZETestCheck(ZEAABBox::IntersectionTest(AABBox, Ray, Tmin));
		ZETestCheckClose(Tmin, 9.2164831f);
	}

	ZETest("bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZERay& Ray, float & TMin, float& TMax)")
	{
		ZEAABBox AABBox(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f), ZEVector3(10.500000f, 10.500000f, 10.500000f));

		ZERay Ray;
		Ray.v = ZEVector3(-0.99582845f, -0.089878544f, 0.015733540f);
		Ray.p = ZEVector3(19.678036f, 11.065289f, 9.8985271f);

		float Tmin = 0.0f;
		float Tmax = 0.0f;

		ZETestCheck(ZEAABBox::IntersectionTest(AABBox, Ray, Tmin, Tmax));
		ZETestCheckClose(Tmin, 9.2164831f);
		ZETestCheckClose(Tmax, 10.220672f);
	}

	ZETest("bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox, const ZEVector3& Point)")
	{
		ZEAABBox AABBox(ZEVector3(3.6039853f, 2.2905931f, 3.3190460f), ZEVector3(4.6039853f, 3.2905931f, 4.3190460f));

		ZETestCase("A point is given inside the ZEAABBox")
		{
			ZEVector3 Point(4.0286264f, 2.4825892f, 4.0162358f);

			ZETestCheck(ZEAABBox::IntersectionTest(AABBox, Point));
		}

		ZETestCase("A point is given outside the ZEAABBox")
		{
			ZEVector3 Point(5.1911983f, 2.1796625f, 3.8908355f);

			ZETestCheck(!(ZEAABBox::IntersectionTest(AABBox, Point)));
		}
	}

	ZETest("bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox1, const ZEAABBox& BoundingBox2)")
	{
		ZEAABBox AABBox(ZEVector3(5.3589416f, 1.9059958f, 5.1632857f), ZEVector3(6.3589416f, 2.9059958f, 6.1632857f));

		ZETestCase("Two AABBoxes are intersecting")
		{
			ZEAABBox ExampleAABBox1(ZEVector3(5.7656059f, 2.7649648f, 5.0221748f), ZEVector3(6.1656055f, 3.1649649f, 5.4221745f));
			ZETestCheck(ZEAABBox::IntersectionTest(AABBox, ExampleAABBox1));
		}

		ZETestCase("Two AABBoxes are not intersecting")
		{
			ZEAABBox ExampleAABBox2(ZEVector3(6.5741935f, 3.3494554f, 0.56329846f), ZEVector3(6.9741931f, 3.7494555f, 0.96329844f));
			ZETestCheck(!(ZEAABBox::IntersectionTest(AABBox, ExampleAABBox2)));
		}

	}

	ZETest("bool ZEAABBox::IntersectionTest(const ZEAABBox& BoundingBox1, const ZEOBBox& BoundingBox2)")
	{
		//Due to errors known this test is skipped. (Refer to Redmine : Bug #303)
		ZETestCheck(false);
	}

	ZETest("void ZEAABBox::Transform(ZEAABBox& Output, const ZEAABBox& Input, const ZEMatrix4x4& TransformMatrix)")
	{
		ZEAABBox AABBox(ZEVector3(4.5f, -0.5f, -0.5f), ZEVector3(5.5f, 0.5f, 0.5f));
		ZEMatrix4x4 TransformMatrix;
		ZEAABBox TranslatedBox;
		ZEAABBox RotatedBox;
		ZEAABBox ResultingBox;

		ZEMatrix4x4::CreateTranslation(TransformMatrix, ZEVector3(-5.0f, 0.0f, 0.0f));
		ZEAABBox::Transform(TranslatedBox, AABBox, TransformMatrix);

		ZEMatrix4x4::CreateRotation(TransformMatrix, ZEQuaternion(ZE_PI_4, ZEVector3::UnitY));
		ZEAABBox::Transform(RotatedBox, TranslatedBox, TransformMatrix);

		ZEMatrix4x4::CreateTranslation(TransformMatrix, ZEVector3(10.0f, 10.0f, 10.0f));
		ZEAABBox::Transform(ResultingBox, RotatedBox, TransformMatrix);

		ZETestCheckClose(ResultingBox.Min, ZEVector3(9.2928934f, 9.5000000f, 9.2928934f));
		ZETestCheckClose(ResultingBox.Max, ZEVector3(10.707107f, 10.500000f, 10.707107f));
	}

	ZETest("ZEAABBox::ZEAABBox(const ZEVector3& Min, const ZEVector3& Max)")
	{
		ZEAABBox AABBox(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f), ZEVector3(10.500000f, 10.500000f, 10.500000f));

		ZETestCheckClose(AABBox.Min, ZEVector3(9.5000000f, 9.5000000f, 9.5000000f));
		ZETestCheckClose(AABBox.Max, ZEVector3(10.500000f, 10.500000f, 10.500000f));
	}

	static bool ZETestCheckAABBoxLineIntersection(ZEVector3 OctantFactor = ZEVector3(0.0f, 0.0f, 0.0f))
	{
		bool Result = true;
		float Tmin = 0.0f;
		float Tmax = 0.0f;

		ZEAABBox AABBox(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f) + OctantFactor, ZEVector3(10.500000f, 10.500000f, 10.500000f) + OctantFactor);

		ZELine Line1;
		Line1.v = ZEVector3(-0.99329460f, -0.11474650f, -0.014108747f);
		Line1.p = ZEVector3(16.774904f, 10.661500f, 10.084741f) + OctantFactor;

		if(!(ZEAABBox::IntersectionTest(AABBox, Line1, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 6.3172641f) && ZETestInternalCheckClose(Tmax, 7.3240147f)))
			Result = false;

		ZELine Line2;
		Line2.v = ZEVector3(0.014830370f, -0.14943798f, 0.98865986f);
		Line2.p = ZEVector3(10.044523f, 10.216407f, 6.5978117f) + OctantFactor;
		
		if(!(ZEAABBox::IntersectionTest(AABBox, Line2, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 2.9354770f) && ZETestInternalCheckClose(Tmax, 3.9469473f)))
			Result = false;

		ZELine Line3;
		Line3.v = ZEVector3(0.99215567f, -0.12467457f, -0.0091299117f);
		Line3.p = ZEVector3(4.6364651f, 10.554062f, 10.027736f) + OctantFactor;

		if(!(ZEAABBox::IntersectionTest(AABBox, Line3, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 4.9019876f) && ZETestInternalCheckClose(Tmax, 5.9098940f)))
			Result = false;

		ZELine Line4;
		Line4.v = ZEVector3(0.034625635f, -0.14449228f, -0.98889995f);
		Line4.p = ZEVector3(9.8649702f, 10.554062f, 15.073883f) + OctantFactor;

		if(!(ZEAABBox::IntersectionTest(AABBox, Line4, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 4.6252232f) && ZETestInternalCheckClose(Tmax, 5.6364479f)))
			Result = false;

		ZELine Line5;
		Line5.v = ZEVector3(0.00000000f, -1.0000000f, 0.00000000f);
		Line5.p = ZEVector3(9.9600716f, 16.092976f, 9.7253780f) + OctantFactor;

		if(!(ZEAABBox::IntersectionTest(AABBox, Line5, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 5.5929756f) && ZETestInternalCheckClose(Tmax, 6.5929756f)))
			Result = false;

		ZELine Line6;
		Line6.v = ZEVector3(0.0064803958f, 0.99980003f, -0.018919595f);
		Line6.p = ZEVector3(9.9608850f, 5.6548214f, 10.021140f) + OctantFactor;

		if(!(ZEAABBox::IntersectionTest(AABBox, Line6, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 3.8459477f) && ZETestInternalCheckClose(Tmax, 4.8461475f)))
			Result = false;

		//Line7 is not intersecting with ZEAABBox

		ZELine Line7;
		Line7.v = ZEVector3(0.41802260f, -0.90830052f, -0.015730977f);
		Line7.p = ZEVector3(8.3835869f, 14.714685f, 9.4565907f) + OctantFactor;

		if(ZEAABBox::IntersectionTest(AABBox, Line7))
			Result = false;

		return Result;
	}

	ZETest("ZEAABBox vs ZELine Intersection Test")
	{
		ZETestCheck(ZETestCheckAABBoxLineIntersection()); //FRU Octant
		ZETestCheck(ZETestCheckAABBoxLineIntersection(ZEVector3(0.0f, 0.0f, -20.0f))); //BRU Octant
		ZETestCheck(ZETestCheckAABBoxLineIntersection(ZEVector3(-20.0f, 0.0f, 0.0f))); //FLU Octant
		ZETestCheck(ZETestCheckAABBoxLineIntersection(ZEVector3(-20.0f, 0.0f, -20.0f))); //BLU Octant
		ZETestCheck(ZETestCheckAABBoxLineIntersection(ZEVector3(0.0f, -20.0f, 0.0f))); //FRD Octant
		ZETestCheck(ZETestCheckAABBoxLineIntersection(ZEVector3(0.0f, -20.0f, -20.0f))); //BRD Octant
		ZETestCheck(ZETestCheckAABBoxLineIntersection(ZEVector3(-20.0f, -20.0f, 0.0f))); //FLD Octant
		ZETestCheck(ZETestCheckAABBoxLineIntersection(ZEVector3(-20.0f, -20.0f, -20.0f))); //BLD Octant
	}

	static bool ZETestCheckAABBoxLineSegmentIntersection(ZEVector3 OctantFactor = ZEVector3(0.0f, 0.0f, 0.0f))
	{
		bool Result = true;
		float Tmin = 0.0f;
		float Tmax = 0.0f;

		ZEAABBox AABBox(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f) + OctantFactor, ZEVector3(10.500000f, 10.500000f, 10.500000f) + OctantFactor);

		ZELineSegment LineSegment1;
		LineSegment1.v = ZEVector3(-0.99725264f, -0.065749310f, -0.034122884f);
		LineSegment1.p = ZEVector3(12.655836f, 10.099570f, 10.100832f) + OctantFactor;
		LineSegment1.Length = 3.5186925f;

		if(!(ZEAABBox::IntersectionTest(AABBox, LineSegment1, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 2.1617754f) && ZETestInternalCheckClose(Tmax, 3.1645303f)))
			Result = false;

		ZELineSegment LineSegment2;
		LineSegment2.v = ZEVector3(0.0099984342f, -0.010796630f, 0.99989176f);
		LineSegment2.p = ZEVector3(10.102241f, 10.050242f, 6.8905826f) + OctantFactor;
		LineSegment2.Length = 5.1760311f;

		if(!(ZEAABBox::IntersectionTest(AABBox, LineSegment2, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 2.6097000f) && ZETestInternalCheckClose(Tmax, 3.6098082f)))
			Result = false;

		ZELineSegment LineSegment3;
		LineSegment3.v = ZEVector3(0.99811029f, -0.060759414f, -0.0091843307f);
		LineSegment3.p = ZEVector3(7.7263494f, 9.9217901f, 9.9582348f) + OctantFactor;
		LineSegment3.Length = 3.4266181f;

		if(!(ZEAABBox::IntersectionTest(AABBox, LineSegment3, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 1.7770087f) && ZETestInternalCheckClose(Tmax, 2.7789021f)))
			Result = false;

		//LineSegment4 is generated inside ZEAABBox and intersects it.

		ZELineSegment LineSegment4;
		LineSegment4.v = ZEVector3(0.044985123f, 0.0042031393f, -0.99897891f);
		LineSegment4.p = ZEVector3(9.8547077f, 10.051089f, 12.609111f) + OctantFactor;
		LineSegment4.Length = 2.7656484f;

		if(!(ZEAABBox::IntersectionTest(AABBox, LineSegment4, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 2.1112666f) && ZETestInternalCheckClose(Tmax, 2.1112666f)))
			Result = false;

		ZELineSegment LineSegment5;
		LineSegment5.v = ZEVector3(0.00000000f, -1.0000001f, 0.00000000f);
		LineSegment5.p = ZEVector3(10.142118f, 11.837045f, 9.9351234f) + OctantFactor;
		LineSegment5.Length = 4.7650075f;

		if(!(ZEAABBox::IntersectionTest(AABBox, LineSegment5, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 1.3370446f) && ZETestInternalCheckClose(Tmax, 2.3370445f)))
			Result = false;

		ZELineSegment LineSegment6;
		LineSegment6.v = ZEVector3(0.32853904f, 0.94441056f, 0.012289822f);
		LineSegment6.p = ZEVector3(8.9158411f, 7.1708980f, 9.9690113f) + OctantFactor;
		LineSegment6.Length = 5.0694299f;

		if(!(ZEAABBox::IntersectionTest(AABBox, LineSegment6, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 2.4661965f) && ZETestInternalCheckClose(Tmax, 3.5250580f)))
			Result = false;

		//LineSegment7 is generated inside the ZEABBox and is not intersecting it.

		ZELineSegment LineSegment7;
		LineSegment7.v = ZEVector3(-0.72443914f, 0.31057435f, 0.61541182f);
		LineSegment7.p = ZEVector3(10.400770f, 9.5853825f, 9.7201071f) + OctantFactor;
		LineSegment7.Length = 0.77462077f;

		if(ZEAABBox::IntersectionTest(AABBox, LineSegment7))
			Result = false;

		//LineSegment8 is not intersecting with ZEAABBox

		ZELineSegment LineSegment8;
		LineSegment8.v = ZEVector3(0.00000000f, -1.0000001f, 0.00000000f);
		LineSegment8.p = ZEVector3(9.9835348f, 13.186707f, 9.5741491f) + OctantFactor;
		LineSegment8.Length = 0.84148681f;

		if(ZEAABBox::IntersectionTest(AABBox, LineSegment8))
			Result = false;

		return Result;
	}

	ZETest("ZEAABBox vs ZELineSegment Intersection Test")
	{
		ZETestCheck(ZETestCheckAABBoxLineSegmentIntersection()); //FRU Octant
		ZETestCheck(ZETestCheckAABBoxLineSegmentIntersection(ZEVector3(0.0f, 0.0f, -20.0f))); //BRU Octant
		ZETestCheck(ZETestCheckAABBoxLineSegmentIntersection(ZEVector3(-20.0f, 0.0f, 0.0f))); //FLU Octant
		ZETestCheck(ZETestCheckAABBoxLineSegmentIntersection(ZEVector3(-20.0f, 0.0f, -20.0f))); //BLU Octant
		ZETestCheck(ZETestCheckAABBoxLineSegmentIntersection(ZEVector3(0.0f, -20.0f, 0.0f))); //FRD Octant
		ZETestCheck(ZETestCheckAABBoxLineSegmentIntersection(ZEVector3(0.0f, -20.0f, -20.0f))); //BRD Octant
		ZETestCheck(ZETestCheckAABBoxLineSegmentIntersection(ZEVector3(-20.0f, -20.0f, 0.0f))); //FLD Octant
		ZETestCheck(ZETestCheckAABBoxLineSegmentIntersection(ZEVector3(-20.0f, -20.0f, -20.0f))); //BLD Octant
	}

	static bool ZETestCheckAABBoxRayIntersection(ZEVector3 OctantFactor = ZEVector3(0.0f, 0.0f, 0.0f))
	{
		bool Result = true;
		float Tmin = 0.0f;
		float Tmax = 0.0f;

		ZEAABBox AABBox(ZEVector3(9.5000000f, 9.5000000f, 9.5000000f) + OctantFactor, ZEVector3(10.500000f, 10.500000f, 10.500000f) + OctantFactor);

		ZERay Ray1;
		Ray1.v = ZEVector3(-0.99582845f, -0.089878544f, 0.015733540f);
		Ray1.p = ZEVector3(19.678036f, 11.065289f, 9.8985271f) + OctantFactor;
		
		if(!(ZEAABBox::IntersectionTest(AABBox, Ray1, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 9.2164831f) && ZETestInternalCheckClose(Tmax, 10.220672f)))
			Result = false;
		
		ZERay Ray2;
		Ray2.v = ZEVector3(-0.0065576322f, -0.10480724f, 0.99447095f);
		Ray2.p = ZEVector3(10.041148f, 10.257440f, 7.1438832f) + OctantFactor;

		if(!(ZEAABBox::IntersectionTest(AABBox, Ray2, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 2.3692162f) && ZETestInternalCheckClose(Tmax, 3.3747761f)))
			Result = false;

		ZERay Ray3;
		Ray3.v = ZEVector3(0.99265850f, -0.11971225f, 0.017265320f);
		Ray3.p = ZEVector3(6.4497113f, 10.303510f, 9.9274464f) + OctantFactor;

		if(!(ZEAABBox::IntersectionTest(AABBox, Ray3, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 3.0728481f) && ZETestInternalCheckClose(Tmax, 4.0802436f)))
			Result = false;

		ZERay Ray4;
		Ray4.v = ZEVector3(-0.019994941f, -0.0099999011f, -0.99975014f);
		Ray4.p = ZEVector3(10.137927f, 9.9174070f, 13.656519f) + OctantFactor;

		if(!(ZEAABBox::IntersectionTest(AABBox, Ray4, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 3.1573079f) && ZETestInternalCheckClose(Tmax, 4.1575580f)))
			Result = false;

		ZERay Ray5;
		Ray5.v = ZEVector3(0.080997236f, -0.99549091f, 0.049369309f);
		Ray5.p = ZEVector3(9.8805714f, 14.766686f, 10.028404f) + OctantFactor;

		if(!(ZEAABBox::IntersectionTest(AABBox, Ray5, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 4.2860126f) && ZETestInternalCheckClose(Tmax, 5.2905421f)))
			Result = false;

		ZERay Ray6;
		Ray6.v = ZEVector3(-0.19709927f, 0.98006666f, -0.024925858f);
		Ray6.p = ZEVector3(10.515674f, 7.3652949f, 10.053101f) + OctantFactor;

		if(!(ZEAABBox::IntersectionTest(AABBox, Ray6, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 2.1781223f) && ZETestInternalCheckClose(Tmax, 3.1984611f)))
			Result = false;

		//Ray7 is generated inside ZEAABBox and intersects it.

		ZERay Ray7;
		Ray7.v = ZEVector3(-0.58591026f, 0.66373110f, -0.46494108f);
		Ray7.p = ZEVector3(10.270999f, 9.6563883f, 10.102864f) + OctantFactor;

		if(!(ZEAABBox::IntersectionTest(AABBox, Ray7, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 1.2710143f) && ZETestInternalCheckClose(Tmax, 1.2710143f)))
			Result = false;

		//Ray8 is not intersecting ZEABBox.

		ZERay Ray8;
		Ray8.v = ZEVector3(0.00000000f, 1.0000000f, 0.00000000f);
		Ray8.p = ZEVector3(9.9520073f, 10.611802f, 9.8793821f) + OctantFactor;

		if(ZEAABBox::IntersectionTest(AABBox, Ray8))
			Result = false;

		return Result;
	}

	ZETest("ZEAABBox vs ZERay Intersection Test")
	{
		ZETestCheck(ZETestCheckAABBoxRayIntersection()); //FRU Octant
		ZETestCheck(ZETestCheckAABBoxRayIntersection(ZEVector3(0.0f, 0.0f, -20.0f))); //BRU Octant
		ZETestCheck(ZETestCheckAABBoxRayIntersection(ZEVector3(-20.0f, 0.0f, 0.0f))); //FLU Octant
		ZETestCheck(ZETestCheckAABBoxRayIntersection(ZEVector3(-20.0f, 0.0f, -20.0f))); //BLU Octant
		ZETestCheck(ZETestCheckAABBoxRayIntersection(ZEVector3(0.0f, -20.0f, 0.0f))); //FRD Octant
		ZETestCheck(ZETestCheckAABBoxRayIntersection(ZEVector3(0.0f, -20.0f, -20.0f))); //BRD Octant
		ZETestCheck(ZETestCheckAABBoxRayIntersection(ZEVector3(-20.0f, -20.0f, 0.0f))); //FLD Octant
		ZETestCheck(ZETestCheckAABBoxRayIntersection(ZEVector3(-20.0f, -20.0f, -20.0f))); //BLD Octant
	}
}

