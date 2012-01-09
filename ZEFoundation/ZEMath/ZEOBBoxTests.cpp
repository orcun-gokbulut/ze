//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEOBBoxTests.cpp
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
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEPlane.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZERay.h"

#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZELineSegment.h"
#include "ZEMath/ZEBSphere.h"
#include "ZEMath/ZEOBBox.h"
#include "ZEMath/ZEMathDefinitions.h"
#include "ZEQuaternion.h"
#include <math.h>


bool ZETestInternalCheckClose(const ZEVector3& Actual, const ZEVector3& Expected, const ZEVector3& Threshold = ZEVector3(ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD, ZE_TEST_CLOSE_THRESHOLD));

ZETestSuite(ZEOBBox)
{

	ZETest("void ZEOBBox::ConvertToSphere(ZEBSphere& Sphere, const ZEOBBox& Input)")
	{
		ZEOBBox OBBox(ZEVector3(10.000000f, 10.000000f, 10.000000f), 
					  ZEVector3(1.0000000f, 0.00000000f, 0.00000000f), 
					  ZEVector3(0.00000000f, 1.0000000f, 0.00000000f),
					  ZEVector3(0.00000000f, 0.00000000f, 1.0000000f),
					  ZEVector3(0.50000000f, 0.50000000f, 0.50000000f));

		ZEBSphere BSphere;

		ZEOBBox::ConvertToSphere(BSphere, OBBox);

		ZETestCheckClose(BSphere.Position, ZEVector3(10.000000f, 10.000000f, 10.000000f));
		ZETestCheckClose(BSphere.Radius, 0.50000000f);

	}

	ZETest("void ZEOBBox::CreateFromOrientation(ZEOBBox& BoundingBox, const ZEVector3& Position, const ZEQuaternion& Rotation, const ZEVector3& Size)")
	{
		ZEOBBox OBBox;
		ZEVector3 Position(10.0f, 10.0f, 10.0f);
		ZEQuaternion Rotation(ZE_PI_4, ZEVector3::UnitY);
		ZEVector3 Size(1.0f, 1.0f, 1.0f);

		ZEOBBox::CreateFromOrientation(OBBox, Position, Rotation, Size);

		ZETestCheckClose(OBBox.Center, ZEVector3(10.000000f, 10.000000f, 10.000000f));
		ZETestCheckClose(OBBox.Right, ZEVector3(0.70710671f, 0.00000000f, -0.70710683f));	 
		ZETestCheckClose(OBBox.Up, ZEVector3(0.00000000f, 0.99999994f, 0.00000000f));
		ZETestCheckClose(OBBox.Front, ZEVector3(0.70710683f, 0.00000000f, 0.70710671f));	 
		ZETestCheckClose(OBBox.HalfSize, ZEVector3(0.50000000f, 0.50000000f, 0.50000000f));
	}

	ZETest("ZEVector3 ZEOBBox::GetVertex(unsigned char Index) const")
	{
		ZEOBBox OBBox(ZEVector3(10.000000f, 10.000000f, 10.000000f), 
			ZEVector3(1.0000000f, 0.00000000f, 0.00000000f), 
			ZEVector3(0.00000000f, 1.0000000f, 0.00000000f),
			ZEVector3(0.00000000f, 0.00000000f, 1.0000000f),
			ZEVector3(0.50000000f, 0.50000000f, 0.50000000f));

		ZETestCheckClose(OBBox.GetVertex(0), ZEVector3(9.5000000f, 9.5000000f, 9.5000000f));
		ZETestCheckClose(OBBox.GetVertex(1), ZEVector3(9.5000000f, 10.500000f, 9.5000000f));
		ZETestCheckClose(OBBox.GetVertex(2), ZEVector3(10.500000f, 9.5000000f, 9.5000000f));
		ZETestCheckClose(OBBox.GetVertex(3), ZEVector3(10.500000f, 10.500000f, 9.5000000f));
		ZETestCheckClose(OBBox.GetVertex(4), ZEVector3(9.5000000f, 9.5000000f, 10.500000f));
		ZETestCheckClose(OBBox.GetVertex(5), ZEVector3(9.5000000f, 10.500000f, 10.500000f));
		ZETestCheckClose(OBBox.GetVertex(6), ZEVector3(10.500000f, 9.5000000f, 10.500000f));
		ZETestCheckClose(OBBox.GetVertex(7), ZEVector3(10.500000f, 10.500000f, 10.500000f));

	}

	ZETest("bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZEBSphere& BoundingSphere)")
	{
		ZEOBBox OBBox(ZEVector3(5.6958194f, 5.5886788f, 4.1537757f), 
					  ZEVector3(-0.77887583f, 0.15488222f, 0.60771441f), 
					  ZEVector3(0.53334928f, 0.67334312f, 0.51195753f),
					  ZEVector3(-0.32991502f, 0.72289240f, -0.60707134f),
					  ZEVector3(0.50000000f, 0.50000000f, 0.50000000f));

		ZEBSphere BSphere(ZEVector3(5.8966031f, 6.3509936f, 4.0408034f), 0.50000000f);

		ZETestCheck(ZEOBBox::IntersectionTest(OBBox, BSphere));
	}

	ZETest("bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZELine& Line)")
	{
		ZEOBBox OBBox;

		OBBox.Center = ZEVector3(10.000000f, 10.000000f, 10.000000f);
		OBBox.Right = ZEVector3(0.38041836f, 0.80166841f, -0.46113682f);
		OBBox.Up = ZEVector3(-0.10620502f, 0.53319407f, 0.83932251f);
		OBBox.Front = ZEVector3(0.91871643f, -0.27031359f, 0.28797263f);
		OBBox.HalfSize = ZEVector3(0.50000000f, 0.50000000f, 0.50000000f);

		ZELine Line;
		Line.v = ZEVector3(-0.93410873f, 0.20356718f, -0.29325953f);
		Line.p = ZEVector3(16.977676f, 8.3858871f, 12.569623f);

		ZETestCheck(ZEOBBox::IntersectionTest(OBBox, Line));
	}

	ZETest("bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZELine& Line, float& TMin)")
	{
		ZEOBBox OBBox;

		OBBox.Center = ZEVector3(10.000000f, 10.000000f, 10.000000f);
		OBBox.Right = ZEVector3(0.38041836f, 0.80166841f, -0.46113682f);
		OBBox.Up = ZEVector3(-0.10620502f, 0.53319407f, 0.83932251f);
		OBBox.Front = ZEVector3(0.91871643f, -0.27031359f, 0.28797263f);
		OBBox.HalfSize = ZEVector3(0.50000000f, 0.50000000f, 0.50000000f);

		ZELine Line;
		Line.v = ZEVector3(-0.93410873f, 0.20356718f, -0.29325953f);
		Line.p = ZEVector3(16.977676f, 8.3858871f, 12.569623f);

		float Tmin = 0.0f;

		ZETestCheck(ZEOBBox::IntersectionTest(OBBox, Line, Tmin));
		ZETestCheckClose(Tmin, 7.1034346f);
	}

	ZETest("bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZELine& Line, float& TMin, float& TMax)")
	{
		ZEOBBox OBBox;

		OBBox.Center = ZEVector3(10.000000f, 10.000000f, 10.000000f);
		OBBox.Right = ZEVector3(0.38041836f, 0.80166841f, -0.46113682f);
		OBBox.Up = ZEVector3(-0.10620502f, 0.53319407f, 0.83932251f);
		OBBox.Front = ZEVector3(0.91871643f, -0.27031359f, 0.28797263f);
		OBBox.HalfSize = ZEVector3(0.50000000f, 0.50000000f, 0.50000000f);

		ZELine Line;
		Line.v = ZEVector3(-0.93410873f, 0.20356718f, -0.29325953f);
		Line.p = ZEVector3(16.977676f, 8.3858871f, 12.569623f);

		float Tmin = 0.0f;
		float Tmax = 0.0f;

		ZETestCheck(ZEOBBox::IntersectionTest(OBBox, Line, Tmin, Tmax));
		ZETestCheckClose(Tmin, 7.1034346f);
		ZETestCheckClose(Tmax, 8.1057816f);

	}

	ZETest("bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZELineSegment& LineSegment)")
	{
		ZEOBBox OBBox;

		OBBox.Center = ZEVector3(10.000000f, 10.000000f, 10.000000f);
		OBBox.Right = ZEVector3(-0.82977504f, -0.55525047f, -0.051845364f);
		OBBox.Up = ZEVector3(0.55690390f, -0.82019585f, -0.12905233f);
		OBBox.Front = ZEVector3(0.029140044f, -0.13599007f, 0.99003798f);
		OBBox.HalfSize = ZEVector3(0.50000000f, 0.50000000f, 0.50000000f);

		ZELineSegment LineSegment;
		LineSegment.v = ZEVector3(-0.75767893f, -0.65243739f, -0.015757650f);
		LineSegment.p = ZEVector3(11.321975f, 10.950789f, 9.9900103f);
		LineSegment.Length = 5.1309547f;

		ZETestCheck(ZEOBBox::IntersectionTest(OBBox, LineSegment));
	}

	ZETest("bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZELineSegment& LineSegment, float& TMin)")
	{
		ZEOBBox OBBox;

		OBBox.Center = ZEVector3(10.000000f, 10.000000f, 10.000000f);
		OBBox.Right = ZEVector3(-0.82977504f, -0.55525047f, -0.051845364f);
		OBBox.Up = ZEVector3(0.55690390f, -0.82019585f, -0.12905233f);
		OBBox.Front = ZEVector3(0.029140044f, -0.13599007f, 0.99003798f);
		OBBox.HalfSize = ZEVector3(0.50000000f, 0.50000000f, 0.50000000f);

		ZELineSegment LineSegment;
		LineSegment.v = ZEVector3(-0.75767893f, -0.65243739f, -0.015757650f);
		LineSegment.p = ZEVector3(11.321975f, 10.950789f, 9.9900103f);
		LineSegment.Length = 5.1309547f;

		float Tmin = 0.0f;

		ZETestCheck(ZEOBBox::IntersectionTest(OBBox, LineSegment, Tmin));
		ZETestCheckClose(Tmin, 1.1336617f);
	}

	ZETest("bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZELineSegment& LineSegment, float& TMin, float& TMax)")
	{
		ZEOBBox OBBox;

		OBBox.Center = ZEVector3(10.000000f, 10.000000f, 10.000000f);
		OBBox.Right = ZEVector3(-0.82977504f, -0.55525047f, -0.051845364f);
		OBBox.Up = ZEVector3(0.55690390f, -0.82019585f, -0.12905233f);
		OBBox.Front = ZEVector3(0.029140044f, -0.13599007f, 0.99003798f);
		OBBox.HalfSize = ZEVector3(0.50000000f, 0.50000000f, 0.50000000f);

		ZELineSegment LineSegment;
		LineSegment.v = ZEVector3(-0.75767893f, -0.65243739f, -0.015757650f);
		LineSegment.p = ZEVector3(11.321975f, 10.950789f, 9.9900103f);
		LineSegment.Length = 5.1309547f;

		float Tmin = 0.0f;
		float Tmax = 0.0f;

		ZETestCheck(ZEOBBox::IntersectionTest(OBBox, LineSegment, Tmin, Tmax));
		ZETestCheckClose(Tmin, 1.1336617f);
		ZETestCheckClose(Tmax, 2.1419437f);
	}

	ZETest("ZEHalfSpace ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZEPlane& Plane)")
	{
		ZEOBBox OBBox;

		OBBox.Center = ZEVector3(10.000000f, 10.000000f, 10.000000f);
		OBBox.Right = ZEVector3(1.0000000f, 0.00000000f, 0.00000000f);
		OBBox.Up = ZEVector3(0.00000000f, 1.0000000f, 0.00000000f);
		OBBox.Front = ZEVector3(0.00000000f, 0.00000000f, 1.0000000f);
		OBBox.HalfSize = ZEVector3(0.50000000f, 0.50000000f, 0.50000000f);

		ZETestCase("OBBox is intersecting the Plane")
		{			
			ZEPlane Plane(ZEVector3(-0.38456747f, 0.80287558f, -0.45552018f), ZEVector3::Zero);
			ZETestCheckEqual(ZEOBBox::IntersectionTest(OBBox, Plane), ZE_HS_INTERSECTS);
		}

		ZETestCase("OBBox is on positive side of the Plane")
		{
			ZEPlane Plane(ZEVector3(-0.36254168f, 0.86610252f, 0.34413669f), ZEVector3::Zero);
			ZETestCheckEqual(ZEOBBox::IntersectionTest(OBBox, Plane), ZE_HS_POSITIVE_SIDE);
		}

		ZETestCase("OBBox is on negative side of the Plane")
		{
			ZEPlane Plane(ZEVector3(-0.67199951f, 0.72048306f, -0.17123325f), ZEVector3::Zero);
			ZETestCheckEqual(ZEOBBox::IntersectionTest(OBBox, Plane), ZE_HS_NEGATIVE_SIDE);
		}
	}

	ZETest("bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZERay& Ray)")
	{
		ZEOBBox OBBox;
		OBBox.Center = ZEVector3(10.000000f, 10.000000f, 10.000000f);
		OBBox.Right = ZEVector3(-0.75966138f, 0.65023738f, 0.0093534440f);
		OBBox.Up = ZEVector3(0.56787413f, 0.65629190f, 0.49677104f);
		OBBox.Front = ZEVector3(0.31688344f, 0.38269287f, -0.86782056f);
		OBBox.HalfSize = ZEVector3(0.50000000f, 0.50000000f, 0.50000000f);

		ZERay Ray;
		Ray.v = ZEVector3(-0.61863327f, -0.57760924f, -0.53259790f);
		Ray.p = ZEVector3(11.934474f, 11.744910f, 11.748796f);

		ZETestCheck(ZEOBBox::IntersectionTest(OBBox, Ray));
	}

	ZETest("bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZERay& Ray, float& TMin)")
	{
		ZEOBBox OBBox;
		OBBox.Center = ZEVector3(10.000000f, 10.000000f, 10.000000f);
		OBBox.Right = ZEVector3(-0.75966138f, 0.65023738f, 0.0093534440f);
		OBBox.Up = ZEVector3(0.56787413f, 0.65629190f, 0.49677104f);
		OBBox.Front = ZEVector3(0.31688344f, 0.38269287f, -0.86782056f);
		OBBox.HalfSize = ZEVector3(0.50000000f, 0.50000000f, 0.50000000f);

		ZERay Ray;
		Ray.v = ZEVector3(-0.61863327f, -0.57760924f, -0.53259790f);
		Ray.p = ZEVector3(11.934474f, 11.744910f, 11.748796f);

		float Tmin = 0.0f;

		ZETestCheck(ZEOBBox::IntersectionTest(OBBox, Ray, Tmin));
		ZETestCheckClose(Tmin, 2.6256785f);
	}

	ZETest("bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZERay& Ray, float& TMin, float& TMax)")
	{
		ZEOBBox OBBox;
		OBBox.Center = ZEVector3(10.000000f, 10.000000f, 10.000000f);
		OBBox.Right = ZEVector3(-0.75966138f, 0.65023738f, 0.0093534440f);
		OBBox.Up = ZEVector3(0.56787413f, 0.65629190f, 0.49677104f);
		OBBox.Front = ZEVector3(0.31688344f, 0.38269287f, -0.86782056f);
		OBBox.HalfSize = ZEVector3(0.50000000f, 0.50000000f, 0.50000000f);

		ZERay Ray;
		Ray.v = ZEVector3(-0.61863327f, -0.57760924f, -0.53259790f);
		Ray.p = ZEVector3(11.934474f, 11.744910f, 11.748796f);

		float Tmin = 0.0f;
		float Tmax = 0.0f;

		ZETestCheck(ZEOBBox::IntersectionTest(OBBox, Ray, Tmin, Tmax));
		ZETestCheckClose(Tmin, 2.6256785f);
		ZETestCheckClose(Tmax, 3.6307387f);
	}

	ZETest("bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox, const ZEVector3& Point)")
	{
		ZEOBBox OBBox(ZEVector3(1.9270762f, 4.0353851f, 4.9880261f), 
					  ZEVector3(0.63871807f, 0.25254911f, 0.72613001f), 
					  ZEVector3(-0.26050943f, -0.81708664f, 0.51333308f),
					  ZEVector3(0.72331238f, -0.51729584f, -0.45632324f),
					  ZEVector3(0.50000000f, 0.50000000f, 0.50000000f));

		ZETestCase("A point is given inside the ZEOBBox")
		{
			ZEVector3 Point(1.9233069f, 4.1004057f, 4.8904080f);

			ZETestCheck(ZEOBBox::IntersectionTest(OBBox, Point));
		}

		ZETestCase("A point is given outside the ZEOBBox")
		{
			ZEVector3 Point(1.5913267f, 3.3986328f, 5.2211065f);

			ZETestCheck(!(ZEOBBox::IntersectionTest(OBBox, Point)));
		}
	}

	ZETest("bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox1, const ZEOBBox& BoundingBox2)")
	{
		//Due to errors known this test is skipped. (Refer to Redmine : Bug #304)
		ZETestCheck(false);
	}

	ZETest("bool ZEOBBox::IntersectionTest(const ZEOBBox& BoundingBox1, const ZEAABBox& BoundingBox2)")
	{
		//Due to errors known this test is skipped. (Refer to Redmine : Bug #303)
		ZETestCheck(false);
	}

	ZETest("void ZEOBBox::Transform(ZEOBBox& Output, const ZEMatrix4x4& Matrix, const ZEOBBox& Input)")
	{
		ZEMatrix4x4 Translation;
		ZEVector3 Position(10.0f, 10.0f, 10.0f);
		ZEQuaternion Rotation(ZE_PI_4, ZEVector3::UnitY);
		ZEVector3 Size(1.0f, 1.0f, 1.0f);
		ZEMatrix4x4::CreateOrientation(Translation, Position, Rotation, Size);

		ZEOBBox OBBox(ZEVector3(0.000000f, 0.000000f, 0.000000f), 
			ZEVector3(1.0000000f, 0.00000000f, 0.00000000f), 
			ZEVector3(0.00000000f, 1.0000000f, 0.00000000f),
			ZEVector3(0.00000000f, 0.00000000f, 1.0000000f),
			ZEVector3(0.50000000f, 0.50000000f, 0.50000000f));

		ZEOBBox ResultOBBox;

		ZEOBBox::Transform(ResultOBBox, Translation, OBBox);

		ZETestCheckClose(ResultOBBox.Center, ZEVector3(10.000000f, 10.000000f, 10.000000f));
		ZETestCheckClose(ResultOBBox.Right, ZEVector3(0.70710671f, 0.00000000f, -0.70710683f));	 
		ZETestCheckClose(ResultOBBox.Up, ZEVector3(0.00000000f, 0.99999994f, 0.00000000f));
		ZETestCheckClose(ResultOBBox.Front, ZEVector3(0.70710683f, 0.00000000f, 0.70710671f));	 
		ZETestCheckClose(ResultOBBox.HalfSize, ZEVector3(0.50000000f, 0.50000000f, 0.50000000f));

	}

	static bool ZETestCheckOBBoxLineIntersection(ZEVector3 OctantFactor = ZEVector3(0.0f, 0.0f, 0.0f))
	{
		bool Result = true;
		float Tmin = 0.0f;
		float Tmax = 0.0f;

		ZEOBBox OBBox;

		OBBox.Center = ZEVector3(10.000000f, 10.000000f, 10.000000f) + OctantFactor;
		OBBox.Right = ZEVector3(0.38041836f, 0.80166841f, -0.46113682f);
		OBBox.Up = ZEVector3(-0.10620502f, 0.53319407f, 0.83932251f);
		OBBox.Front = ZEVector3(0.91871643f, -0.27031359f, 0.28797263f);
		OBBox.HalfSize = ZEVector3(0.50000000f, 0.50000000f, 0.50000000f);

		ZELine Line1;
		Line1.v = ZEVector3(-0.93410873f, 0.20356718f, -0.29325953f);
		Line1.p = ZEVector3(16.977676f, 8.3858871f, 12.569623f) + OctantFactor;

		if(!(ZEOBBox::IntersectionTest(OBBox, Line1, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 7.1034346f) && ZETestInternalCheckClose(Tmax, 8.1057816f)))
			Result = false;

		ZELine Line2;
		Line2.v = ZEVector3(-0.26174462f, 0.21900758f, 0.93996036f);
		Line2.p = ZEVector3(11.513317f, 8.6308079f, 5.0077610f) + OctantFactor;

		if(!(ZEOBBox::IntersectionTest(OBBox, Line2, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 4.9071836f) && ZETestInternalCheckClose(Tmax, 5.9784188f)))
			Result = false;

		ZELine Line3;
		Line3.v = ZEVector3(0.90315610f, 0.035789575f, 0.42781791f);
		Line3.p = ZEVector3(4.0037546f, 9.6702137f, 7.2946534f) + OctantFactor;

		if(!(ZEOBBox::IntersectionTest(OBBox, Line3, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 6.0415053f) && ZETestInternalCheckClose(Tmax, 7.1016474f)))
			Result = false;

		ZELine Line4;
		Line4.v = ZEVector3(0.17478859f, -0.35620472f, -0.91791463f);
		Line4.p = ZEVector3(9.1696711f, 11.462744f, 14.281624f) + OctantFactor;

		if(!(ZEOBBox::IntersectionTest(OBBox, Line4, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 4.0471039f) && ZETestInternalCheckClose(Tmax, 5.0686417f)))
			Result = false;

		ZELine Line5;
		Line5.v = ZEVector3(0.35102689f, 0.75509369f, -0.55372715f);
		Line5.p = ZEVector3(8.1486607f, 5.8428302f, 12.903716f) + OctantFactor;

		if(!(ZEOBBox::IntersectionTest(OBBox, Line5, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 4.9043326f) && ZETestInternalCheckClose(Tmax, 5.9101505f)))
			Result = false;

		ZELine Line6;
		Line6.v = ZEVector3(-0.23883815f, -0.91847336f, 0.31521943f);
		Line6.p = ZEVector3(12.071309f, 17.334459f, 7.7232180f) + OctantFactor;

		if(!(ZEOBBox::IntersectionTest(OBBox, Line6, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 7.4215555f) && ZETestInternalCheckClose(Tmax, 8.4498024f)))
			Result = false;
		
		//Line7 is not intersecting ZEOBBox but a very close line.

		ZELine Line7;
		Line7.v = ZEVector3(0.94934696f, -0.11395535f, 0.29283878f);
		Line7.p = ZEVector3(7.7549963f, 10.446317f, 8.5771942f) + OctantFactor;

		if(ZEOBBox::IntersectionTest(OBBox, Line7))
			Result = false;

		return Result;
	}

	ZETest("OBBox vs Line Intersection Test")
	{
		ZETestCheck(ZETestCheckOBBoxLineIntersection()); //FRU Octant
		ZETestCheck(ZETestCheckOBBoxLineIntersection(ZEVector3(0.0f, 0.0f, -20.0f))); //BRU Octant
		ZETestCheck(ZETestCheckOBBoxLineIntersection(ZEVector3(-20.0f, 0.0f, 0.0f))); //FLU Octant
		ZETestCheck(ZETestCheckOBBoxLineIntersection(ZEVector3(-20.0f, 0.0f, -20.0f))); //BLU Octant
		ZETestCheck(ZETestCheckOBBoxLineIntersection(ZEVector3(0.0f, -20.0f, 0.0f))); //FRD Octant
		ZETestCheck(ZETestCheckOBBoxLineIntersection(ZEVector3(0.0f, -20.0f, -20.0f))); //BRD Octant
		ZETestCheck(ZETestCheckOBBoxLineIntersection(ZEVector3(-20.0f, -20.0f, 0.0f))); //FLD Octant
		ZETestCheck(ZETestCheckOBBoxLineIntersection(ZEVector3(-20.0f, -20.0f, -20.0f))); //BLD Octant
	}

	static bool ZETestCheckOBBoxLineSegmentIntersection(ZEVector3 OctantFactor = ZEVector3(0.0f, 0.0f, 0.0f))
	{
		bool Result = true;
		float Tmin = 0.0f;
		float Tmax = 0.0f;

		ZEOBBox OBBox;

		OBBox.Center = ZEVector3(10.000000f, 10.000000f, 10.000000f) + OctantFactor;
		OBBox.Right = ZEVector3(-0.82977504f, -0.55525047f, -0.051845364f);
		OBBox.Up = ZEVector3(0.55690390f, -0.82019585f, -0.12905233f);
		OBBox.Front = ZEVector3(0.029140044f, -0.13599007f, 0.99003798f);
		OBBox.HalfSize = ZEVector3(0.50000000f, 0.50000000f, 0.50000000f);

		//LineSegment1 is generated inside ZEOBBox with no intersection

		ZELineSegment LineSegment1; 
		LineSegment1.v = ZEVector3(0.55852556f, -0.50984299f, 0.65430069f);
		LineSegment1.p = ZEVector3(9.6788292f, 10.200702f, 9.6376143f) + OctantFactor;
		LineSegment1.Length = 0.71010470f;

		if(ZEOBBox::IntersectionTest(OBBox, LineSegment1))
			Result = false;

		//LineSegment2 is generated inside ZEOBBox and intersects it.

		ZELineSegment LineSegment2;
		LineSegment2.v = ZEVector3(0.49342841f, -0.54030263f, 0.68161696f);
		LineSegment2.p = ZEVector3(9.9045048f, 9.9053087f, 9.6517887f) + OctantFactor;
		LineSegment2.Length = 5.8179879f;

		if(!(ZEOBBox::IntersectionTest(OBBox, LineSegment2, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 0.68347800f) && ZETestInternalCheckClose(Tmax, 0.68347800f)))
			Result = false;

		ZELineSegment LineSegment3;
		LineSegment3.v = ZEVector3(-0.75767893f, -0.65243739f, -0.015757650f);
		LineSegment3.p = ZEVector3(11.321975f, 10.950789f, 9.9900103f) + OctantFactor;
		LineSegment3.Length = 5.1309547f;

		if(!(ZEOBBox::IntersectionTest(OBBox, LineSegment3, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 1.1336617f) && ZETestInternalCheckClose(Tmax, 2.1419437f)))
			Result = false;

		ZELineSegment LineSegment4;
		LineSegment4.v = ZEVector3(0.013285048f, -0.13537958f, 0.99070477f);
		LineSegment4.p = ZEVector3(9.9539537f, 10.153339f, 7.8861427f) + OctantFactor;
		LineSegment4.Length = 3.3245032f;

		if(!(ZEOBBox::IntersectionTest(OBBox, LineSegment4, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 1.6155866f) && ZETestInternalCheckClose(Tmax, 2.6159539f)))
			Result = false;

		ZELineSegment LineSegment5;
		LineSegment5.v = ZEVector3(0.84545815f, 0.53051120f, 0.061308712f);
		LineSegment5.p = ZEVector3(8.2071304f, 8.5909195f, 9.9317913f) + OctantFactor;
		LineSegment5.Length = 3.5257425f;

		if(!(ZEOBBox::IntersectionTest(OBBox, LineSegment5, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 1.7748759f) && ZETestInternalCheckClose(Tmax, 2.7755911f)))
			Result = false;

		ZELineSegment LineSegment6;
		LineSegment6.v = ZEVector3(-0.0099379476f, 0.11057021f, -0.99381864f);
		LineSegment6.p = ZEVector3(10.080036f, 9.8736792f, 12.007704f) + OctantFactor;
		LineSegment6.Length = 6.7395854f;

		if(!(ZEOBBox::IntersectionTest(OBBox, LineSegment6, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 1.5083536f) && ZETestInternalCheckClose(Tmax, 2.5091100f)))
			Result = false;

		ZELineSegment LineSegment7;
		LineSegment7.v = ZEVector3(-0.65665442f, 0.74183059f, 0.13598645f);
		LineSegment7.p = ZEVector3(13.721167f, 5.6107125f, 9.2481136f) + OctantFactor;
		LineSegment7.Length = 6.4595170f;

		if(!(ZEOBBox::IntersectionTest(OBBox, LineSegment7, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 5.3136005f) && ZETestInternalCheckClose(Tmax, 6.3219814f)))
			Result = false;

		ZELineSegment LineSegment8;
		LineSegment8.v = ZEVector3(0.61870122f, -0.78382242f, -0.053211659f);
		LineSegment8.p = ZEVector3(6.1711383f, 15.015785f, 10.574327f) + OctantFactor;
		LineSegment8.Length = 6.9782095f;

		if(!(ZEOBBox::IntersectionTest(OBBox, LineSegment8, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 5.8536477f) && ZETestInternalCheckClose(Tmax, 6.8593678f)))
			Result = false;

		//LineSegment9 is not intersecting ZEOBBox but is a very close linesegment possibly tangent

		ZELineSegment LineSegment9; 
		LineSegment9.v = ZEVector3(0.81137818f, 0.55984843f, 0.16803417f);
		LineSegment9.p = ZEVector3(9.2562199f, 8.8444109f, 9.8467703f) + OctantFactor;
		LineSegment9.Length = 6.3142490f;

		if(ZEOBBox::IntersectionTest(OBBox, LineSegment9))
			Result = false;

		return Result;
	}

	ZETest("OBBox vs Line Segment Intersection Test")
	{
		ZETestCheck(ZETestCheckOBBoxLineSegmentIntersection()); //FRU Octant
		ZETestCheck(ZETestCheckOBBoxLineSegmentIntersection(ZEVector3(0.0f, 0.0f, -20.0f))); //BRU Octant
		ZETestCheck(ZETestCheckOBBoxLineSegmentIntersection(ZEVector3(-20.0f, 0.0f, 0.0f))); //FLU Octant
		ZETestCheck(ZETestCheckOBBoxLineSegmentIntersection(ZEVector3(-20.0f, 0.0f, -20.0f))); //BLU Octant
		ZETestCheck(ZETestCheckOBBoxLineSegmentIntersection(ZEVector3(0.0f, -20.0f, 0.0f))); //FRD Octant
		ZETestCheck(ZETestCheckOBBoxLineSegmentIntersection(ZEVector3(0.0f, -20.0f, -20.0f))); //BRD Octant
		ZETestCheck(ZETestCheckOBBoxLineSegmentIntersection(ZEVector3(-20.0f, -20.0f, 0.0f))); //FLD Octant
		ZETestCheck(ZETestCheckOBBoxLineSegmentIntersection(ZEVector3(-20.0f, -20.0f, -20.0f))); //BLD Octant
	}

	static bool ZETestCheckOBBoxRayIntersection(ZEVector3 OctantFactor = ZEVector3(0.0f, 0.0f, 0.0f))
	{
		bool Result = true;
		float Tmin = 0.0f;
		float Tmax = 0.0f;

		ZEOBBox OBBox;

		OBBox.Center = ZEVector3(10.000000f, 10.000000f, 10.000000f) + OctantFactor;
		OBBox.Right = ZEVector3(-0.75966138f, 0.65023738f, 0.0093534440f);
		OBBox.Up = ZEVector3(0.56787413f, 0.65629190f, 0.49677104f);
		OBBox.Front = ZEVector3(0.31688344f, 0.38269287f, -0.86782056f);
		OBBox.HalfSize = ZEVector3(0.50000000f, 0.50000000f, 0.50000000f);

		ZERay Ray1;
		Ray1.v = ZEVector3(-0.61863327f, -0.57760924f, -0.53259790f);
		Ray1.p = ZEVector3(11.934474f, 11.744910f, 11.748796f) + OctantFactor;

		if(!(ZEOBBox::IntersectionTest(OBBox, Ray1, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 2.6256785f) && ZETestInternalCheckClose(Tmax, 3.6307387f)))
			Result = false;

		ZERay Ray2;
		Ray2.v = ZEVector3(-0.29913560f, -0.27712166f, 0.91308355f);
		Ray2.p = ZEVector3(11.016970f, 10.808256f, 7.0725012f) + OctantFactor;

		if(!(ZEOBBox::IntersectionTest(OBBox, Ray2, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 2.6903148f) && ZETestInternalCheckClose(Tmax, 3.6971245f)))
			Result = false;

		ZERay Ray3;
		Ray3.v = ZEVector3(0.60099196f, 0.66999567f, 0.43579185f);
		Ray3.p = ZEVector3(6.9482040f, 6.5937800f, 7.7938762f) + OctantFactor;

		if(!(ZEOBBox::IntersectionTest(OBBox, Ray3, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 4.5759382f) && ZETestInternalCheckClose(Tmax, 5.5784554f)))
			Result = false;

		ZERay Ray4;
		Ray4.v = ZEVector3(0.28762913f, 0.39328644, -0.87326705f);
		Ray4.p = ZEVector3(8.5197401f, 8.0577402f, 14.397716f) + OctantFactor;

		if(!(ZEOBBox::IntersectionTest(OBBox, Ray4, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 4.5310888f) && ZETestInternalCheckClose(Tmax, 5.5315971f)))
			Result = false;

		ZERay Ray5;
		Ray5.v = ZEVector3(-0.76295590f, 0.64360756f, 0.060560785f);
		Ray5.p = ZEVector3(14.019847f, 6.3262229f, 9.6537371f) + OctantFactor;

		if(!(ZEOBBox::IntersectionTest(OBBox, Ray5, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 4.9524632f) && ZETestInternalCheckClose(Tmax, 5.9538126f)))
			Result = false;

		ZERay Ray6;
		Ray6.v = ZEVector3(0.76986492f, -0.63715166f, -0.036686510f);
		Ray6.p = ZEVector3(4.6621208f, 14.405806f, 10.174911f) + OctantFactor;

		if(!(ZEOBBox::IntersectionTest(OBBox, Ray6, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 6.4247799f) && ZETestInternalCheckClose(Tmax, 7.4253006f)))
			Result = false;

		//Ray7 is generated inside ZEOBBox and intersecting it.

		ZERay Ray7; 
		Ray7.v = ZEVector3(0.60140979f, -0.20434913f, 0.77236509f);
		Ray7.p = ZEVector3(9.7985239f, 10.026769f, 9.7466383f) + OctantFactor;

		if(!(ZEOBBox::IntersectionTest(OBBox, Ray7, Tmin, Tmax) && ZETestInternalCheckClose(Tmin, 1.1468979f) && ZETestInternalCheckClose(Tmax, 1.1468979f)))
			Result = false;

		return Result;
	}

	ZETest("OBBox vs Ray Intersection Test")
	{
		ZETestCheck(ZETestCheckOBBoxRayIntersection()); //FRU Octant
		ZETestCheck(ZETestCheckOBBoxRayIntersection(ZEVector3(0.0f, 0.0f, -20.0f))); //BRU Octant
		ZETestCheck(ZETestCheckOBBoxRayIntersection(ZEVector3(-20.0f, 0.0f, 0.0f))); //FLU Octant
		ZETestCheck(ZETestCheckOBBoxRayIntersection(ZEVector3(-20.0f, 0.0f, -20.0f))); //BLU Octant
		ZETestCheck(ZETestCheckOBBoxRayIntersection(ZEVector3(0.0f, -20.0f, 0.0f))); //FRD Octant
		ZETestCheck(ZETestCheckOBBoxRayIntersection(ZEVector3(0.0f, -20.0f, -20.0f))); //BRD Octant
		ZETestCheck(ZETestCheckOBBoxRayIntersection(ZEVector3(-20.0f, -20.0f, 0.0f))); //FLD Octant
		ZETestCheck(ZETestCheckOBBoxRayIntersection(ZEVector3(-20.0f, -20.0f, -20.0f))); //BLD Octant
	}
}
