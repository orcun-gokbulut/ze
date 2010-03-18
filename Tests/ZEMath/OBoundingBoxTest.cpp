//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - OBoundingBoxTest.cpp
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

#include "UnitTest/UnitTest++.h"
#include "../IOStreamMapping.h"
#include "ZEMath/AABoundingBox.h"
#include "ZEMath/Vector.h"
#include "ZEmath/Plane.h"
#include "ZEMath/Matrix.h"
#include "ZEMath/Ray.h"
#include "ZEMath/LineSegment.h"
#include "ZEMath/BoundingSphere.h"
#include "ZEMath/OBoundingBox.h"
#include <math.h>

SUITE(ZEOBoundingBox)
{
	TEST(OBB_Constructor)
	{
		ZEPoint3 Position(1.0f, 2.0f, 3.0f);
		ZEVector3 U(1.0f, 0.0f, 0.0f);
		ZEVector3 V(0.0f, 1.0f, 0.0f);
		ZEVector3 N(0.0f, 0.0f, 1.0f);

		ZEOBoundingBox OBB(Position, U, V, N);

		CHECK_EQUAL(OBB.Position,ZEPoint3(1.0f, 2.0f, 3.0f));
		CHECK_EQUAL(OBB.U,ZEVector3(1.0f, 0.0f, 0.0f));
		CHECK_EQUAL(OBB.V,ZEVector3(0.0f, 1.0f, 0.0f));
		CHECK_EQUAL(OBB.N,ZEVector3(0.0f, 0.0f, 1.0f));

	}

	TEST(OBB_GetCenter)
	{
		//No funtion
	}

	TEST(OBB_GetVertex)
	{
		ZEPoint3 Position(1.0f, 2.0f, 3.0f);
		ZEVector3 U(1.0f, 0.0f, 0.0f);
		ZEVector3 V(0.0f, 1.0f, 0.0f);
		ZEVector3 N(0.0f, 0.0f, 1.0f);

		ZEOBoundingBox OBB(Position, U, V, N);

		ZEPoint3 V0 = OBB.GetVertex(0);
		ZEPoint3 V1 = OBB.GetVertex(1);
		ZEPoint3 V2 = OBB.GetVertex(2);
		ZEPoint3 V3 = OBB.GetVertex(3);
		ZEPoint3 V4 = OBB.GetVertex(4);
		ZEPoint3 V5 = OBB.GetVertex(5);
		ZEPoint3 V6 = OBB.GetVertex(6);
		ZEPoint3 V7 = OBB.GetVertex(7);

		CHECK_EQUAL(V0, ZEPoint3(1.0f, 2.0f, 3.0f));
		CHECK_EQUAL(V1, ZEPoint3(2.0f, 2.0f, 3.0f));
		CHECK_EQUAL(V2, ZEPoint3(1.0f, 3.0f, 3.0f));
		CHECK_EQUAL(V3, ZEPoint3(2.0f, 3.0f, 3.0f));
		CHECK_EQUAL(V4, ZEPoint3(1.0f, 2.0f, 4.0f));
		CHECK_EQUAL(V5, ZEPoint3(2.0f, 2.0f, 4.0f));
		CHECK_EQUAL(V6, ZEPoint3(1.0f, 3.0f, 4.0f));
		CHECK_EQUAL(V7, ZEPoint3(2.0f, 3.0f, 4.0f));
	}

	TEST(OBB_PlaneHalfSpaceTest)
	{
		
		ZEPoint3 Position(1.0f, 2.0f, 3.0f);
		ZEVector3 U(1.0f, 0.0f, 0.0f);
		ZEVector3 V(0.0f, 1.0f, 0.0f);
		ZEVector3 N(0.0f, 0.0f, 1.0f);

		ZEOBoundingBox OBB(Position, U, V, N);

		ZEVector3 n1(1.0f, 2.0f, 3.0f);
		ZEPoint3 p1(100.0f, 100.0f, 100.0f);
		ZEPlane P1(n1, p1);

		ZEHalfSpace result1 = ZEOBoundingBox::PlaneHalfSpaceTest(OBB, P1);

		CHECK_EQUAL(result1,-1);

		ZEVector3 n2(1.0f, 2.0f, 3.0f);
		ZEPoint3 p2(0.0f, -6.0f, 0.0f);
		ZEPlane P2(n2, p2);

		ZEHalfSpace result2 = ZEOBoundingBox::PlaneHalfSpaceTest(OBB, P2);

		CHECK_EQUAL(result2, 1);

		
		ZEVector3 n3(1.0f, 0.0f, 0.0f);
		ZEPoint3 p3(0.0f, 0.0f, 0.0f);
		ZEPlane P3(n3, p3);

		ZEHalfSpace result3 = ZEOBoundingBox::PlaneHalfSpaceTest(OBB, P3);

		CHECK_EQUAL(result3, 1);

		ZEVector3 n4(0.0f, 0.0f, 1.0f);
		ZEPoint3 p4(-200.0f, -1000.0f, -100.0f);
		ZEPlane P4(n4, p4);


		ZEHalfSpace result4 = ZEOBoundingBox::PlaneHalfSpaceTest(OBB, P4);

		CHECK_EQUAL(result4, 1);
	}

	TEST(OBB_IntersectionTest1)
	{
		ZEPoint3 Position(1.0f, 2.0f, 3.0f);
		ZEVector3 U(1.0f, 0.0f, 0.0f);
		ZEVector3 V(0.0f, 1.0f, 0.0f);
		ZEVector3 N(0.0f, 0.0f, 1.0f);

		ZEOBoundingBox OBB(Position, U, V, N);

		
		bool result = ZEOBoundingBox::IntersectionTest(OBB, ZEPoint3(1.0f, 2.0f, 3.0f));

		CHECK_EQUAL(result, true);

		ZEPoint3 P(-1.0f, -2.0f, -3.0f);

		bool result2 = ZEOBoundingBox::IntersectionTest(OBB, P);

		CHECK_EQUAL(result2, false);

		bool result3 = ZEOBoundingBox::IntersectionTest(OBB, ZEPoint3(1.5f, 2.5f, 3.5f));

		CHECK_EQUAL(result3, true);

	}

	TEST(OBB_IntersectionTest2)
	{
	
	}

	TEST(OBB_IntersectionTest3)
	{
	
	}

	TEST(OBB_IntersectionTest4)
	{

	}
}
