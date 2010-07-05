//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - AABoundingBoxTest.cpp
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


SUITE(ZEAABoundingBox)
{
	TEST(AABB_Contructor)
	{
		ZEPoint3 Max( 1.0f,  2.0f,  3.0f);
		ZEPoint3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABoundingBox AABB(Min, Max);

		CHECK_EQUAL(AABB.Max, ZEPoint3(1.0f, 2.0f, 3.0f));
		CHECK_EQUAL(AABB.Min, ZEPoint3(-1.0f, -2.0f, -3.0f));
	}

	TEST(AABB_GetCenter)
	{
		ZEPoint3 Max( 1.0f,  2.0f,  3.0f);
		ZEPoint3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABoundingBox AABB(Min, Max);

		ZEPoint3 Center = AABB.GetCenter();

		CHECK_EQUAL(Center, ZEPoint3(0.0f, 0.0f, 0.0f));
	}

	TEST(AABB_GetVertex)
	{
		ZEPoint3 Max( 1.0f,  2.0f,  3.0f);
		ZEPoint3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABoundingBox AABB(Min, Max);
		ZEPoint3 Vertex1  = AABB.GetVertex(0);
		ZEPoint3 Vertex2  = AABB.GetVertex(1);
		ZEPoint3 Vertex3  = AABB.GetVertex(2);
		ZEPoint3 Vertex4  = AABB.GetVertex(3);
		ZEPoint3 Vertex5  = AABB.GetVertex(4);
		ZEPoint3 Vertex6  = AABB.GetVertex(5);
		ZEPoint3 Vertex7  = AABB.GetVertex(6);
		ZEPoint3 Vertex8  = AABB.GetVertex(7);

		CHECK_EQUAL(Vertex1, ZEPoint3(-1.0f, -2.0f, -3.0f));
		CHECK_EQUAL(Vertex2, ZEPoint3(-1.0f, -2.0f,  3.0f));
		CHECK_EQUAL(Vertex3, ZEPoint3(-1.0f,  2.0f, -3.0f));
		CHECK_EQUAL(Vertex4, ZEPoint3(-1.0f,  2.0f,  3.0f));
		CHECK_EQUAL(Vertex5, ZEPoint3( 1.0f, -2.0f, -3.0f));
		CHECK_EQUAL(Vertex6, ZEPoint3( 1.0f, -2.0f,  3.0f));
		CHECK_EQUAL(Vertex7, ZEPoint3( 1.0f,  2.0f, -3.0f));
		CHECK_EQUAL(Vertex8, ZEPoint3( 1.0f,  2.0f,  3.0f));
	}

	TEST(AABB_GetLength)
	{
		ZEPoint3 Max( 1.0f,  2.0f,  3.0f);
		ZEPoint3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABoundingBox AABB(Min, Max);

		float Length = AABB.GetLenght();

		CHECK_EQUAL(Length, sqrt(56.0f));
	}

	TEST(AABB_Transform)
	{
		ZEPoint3 Max( 1.0f,  2.0f,  3.0f);
		ZEPoint3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABoundingBox AABB1(Min, Max);

		ZEAABoundingBox AABB;

		ZEMatrix4x4 M( 1.0f,  1.0f,  2.0f,  3.0f, 
					   5.0f,  8.0f, 13.0f, 21.0f, 
					  34.0f, 55.0f,  1.0f,  1.0f, 
					   3.0f,  5.0f,  8.0f, 13.0f);
		ZEAABoundingBox::Transform(AABB, AABB1, M);

		CHECK_EQUAL(AABB.Max, ZEVector3(116.0f, 187.0f, 39.0f));
		CHECK_EQUAL(AABB.Min, ZEVector3(-110.0f, -177.0f, -23.0f));

		ZEPoint3 Max2( 1.0f,  2.0f,  5.0f);
		ZEPoint3 Min2(-1.0f, -2.0f, -1.0f);
		ZEAABoundingBox AABB2(Min2, Max2);
		ZEAABoundingBox AABB3;

		ZEAABoundingBox::Transform(AABB3, AABB2, ZEMatrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 
															  0.0f, 1.0f, 0.0f, 0.0f, 
															  0.0f, 0.0f, 1.0f, 0.0f, 
															  0.0f, 0.0f, 0.0f, 1.0f));

		CHECK_EQUAL(AABB3.Max, AABB2.Max);
		CHECK_EQUAL(AABB3.Min, AABB2.Min);



	}

	TEST(AABB_PlaneHalfSpaceTest)
	{
		ZEPoint3 Max( 1.0f,  2.0f,  3.0f);
		ZEPoint3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABoundingBox AABB(Min, Max);

		ZEVector3 n1(1.0f, 2.0f, 3.0f);
		ZEPoint3 p1(100.0f, 100.0f, 100.0f);
		ZEPlane P1(n1, p1);

		ZEHalfSpace result1 = ZEAABoundingBox::PlaneHalfSpaceTest(AABB, P1);

		CHECK_EQUAL(result1,-1);

		ZEVector3 n2(1.0f, 2.0f, 3.0f);
		ZEPoint3 p2(0.0f, -6.0f, 0.0f);
		ZEPlane P2(n2, p2);

		ZEHalfSpace result2 = ZEAABoundingBox::PlaneHalfSpaceTest(AABB, P2);

		CHECK_EQUAL(result2, ZEHALFSPACE_INTERSECTS);

		
		ZEVector3 n3(1.0f, 0.0f, 0.0f);
		ZEPoint3 p3(0.0f, 0.0f, 0.0f);
		ZEPlane P3(n3, p3);

		ZEHalfSpace result3 = ZEAABoundingBox::PlaneHalfSpaceTest(AABB, P3);

		CHECK_EQUAL(result3, ZEHALFSPACE_INTERSECTS);

		ZEVector3 n4(0.0f, 0.0f, 1.0f);
		ZEPoint3 p4(-200.0f, -1000.0f, -100.0f);
		ZEPlane P4(n4, p4);


		ZEHalfSpace result4 = ZEAABoundingBox::PlaneHalfSpaceTest(AABB, P4);

		CHECK_EQUAL(result4, 1);

	}

	TEST(AABB_IntersectionTest1)
	{
		
		ZEPoint3 Max( 1.0f,  2.0f,  3.0f);
		ZEPoint3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABoundingBox AABB(Min, Max);


		bool result = ZEAABoundingBox::IntersectionTest(AABB, ZEPoint3(1.0f, 2.0f, 3.0f));

		CHECK_EQUAL(result, true);

		ZEPoint3 P(-1.0f, -2.0f, -3.0f);

		bool result2 = ZEAABoundingBox::IntersectionTest(AABB, P);

		CHECK_EQUAL(result2, true);

	}
	TEST(AABB_IntersectionTest2)
	{
		ZEPoint3 Max( 1.0f,  2.0f,  3.0f);
		ZEPoint3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABoundingBox AABB1(Min, Max);


		ZEPoint3 P0( 1.0f,  2.0f,  3.0f);
		ZEPoint3 P1(-1.0f, -2.0f, -3.0f);

		ZELine L1(P0, P1);


		bool result1 = ZEAABoundingBox::IntersectionTest(AABB1, L1);


		CHECK_EQUAL(result1, false);


		ZEPoint3 P2( 0.0f,  0.0f,  0.0f);
		ZEPoint3 P3(-1.0f, -2.0f, -3.0f);

		ZELine L2(P2, P3);

		bool result2 = ZEAABoundingBox::IntersectionTest(AABB1, L2);

		CHECK_EQUAL(result2, false);
		
		ZEPoint3 P4(0.0f, 100.0f, 0.0f);
		ZEPoint3 P5(0.0f, 100.0f, 6.0f);

		ZELine L3(P4, P5);

		bool result3 = ZEAABoundingBox::IntersectionTest(AABB1, L3);

		CHECK_EQUAL(result3, false);


		ZEPoint3 P6(0.0f, 0.0f,  0.0f);
		ZEPoint3 P7(0.0f, 0.0f,  1.0f);

		ZELine L4(P6, P7);


		bool result4 = ZEAABoundingBox::IntersectionTest(AABB1, L4);


		CHECK_EQUAL(result4, true);
	}
	
	TEST(AABB_IntersectionTest3)
	{
		ZEPoint3 Max( 1.0f,  2.0f,  3.0f);
		ZEPoint3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABoundingBox AABB1(Min, Max);

		ZEPoint3 p1(0.9f, 1.9f, 2.9f);
		ZEVector3 v1(0.0f, 0.0f, 1.0f);
		ZERay R1(v1,p1);

		bool result1 = ZEAABoundingBox::IntersectionTest(AABB1, R1);
		CHECK_EQUAL(result1, true);

		ZEPoint3 p2(100.0f, 100.0f, 100.0f);
		ZEVector3 v2(0.0f, 0.0f, 1.0f);
		ZERay R2(v2, p2);

		bool result2 = ZEAABoundingBox::IntersectionTest(AABB1, R2);
		CHECK_EQUAL(result2, false);

		ZEPoint3 p3(0.0f, 0.0f, 0.0f);
		ZEVector3 v3(1.0f, 0.0f, 0.0f);
		ZERay R3(v3, p3);

		bool result3 = ZEAABoundingBox::IntersectionTest(AABB1, R3);
		CHECK_EQUAL(result3, true);

		ZEPoint3 p4(-1.0f, -2.0f, -3.0f);
		ZEVector3 v4(2.0f,  4.0f,  6.0f);
		ZERay R4(v4, p4);
		
		bool result4 = ZEAABoundingBox::IntersectionTest(AABB1, R4);
		CHECK_EQUAL(result4, true);
		

		ZEPoint3  p5(1.0, 2.0f, 3.0f);
		ZEVector3 v5(1.0, 0.0f, 0.0f);
		ZERay R5(v5, p5);

		bool result5 = ZEAABoundingBox::IntersectionTest(AABB1, R5);
		CHECK_EQUAL(result5, true);
	}

	TEST(AABB_IntersectionTest4)
	{
		ZEPoint3 Max( 1.0f,  2.0f,  3.0f);
		ZEPoint3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABoundingBox AABB1(Min, Max);


		ZEPoint3 P0( 1.0f,  2.0f,  3.0f);
		ZEPoint3 P1(-1.0f, -2.0f, -3.0f);

		ZELineSegment LS1(P0, P1);


		bool result1 = ZEAABoundingBox::IntersectionTest(AABB1, LS1);


		CHECK_EQUAL(result1, true);


		ZEPoint3 P2( 0.0f,  0.0f,  0.0f);
		ZEPoint3 P3(-1.0f, -2.0f, -3.0f);

		ZELineSegment LS2(P2, P3);


		bool result2 = ZEAABoundingBox::IntersectionTest(AABB1, LS2);


		CHECK_EQUAL(result2, false);

		
		ZEPoint3 P4(0.0f, 100.0f, 0.0f);
		ZEPoint3 P5(0.0f, 100.0f, 6.0f);

		ZELineSegment LS3(P4, P5);


		bool result3 = ZEAABoundingBox::IntersectionTest(AABB1,LS3);
	}

	TEST(AABB_GenerateBoundingSphere1)
	{
		ZEPoint3 Max( 1.0f,  2.0f,  3.0f);
		ZEPoint3 Min(-1.0f, -2.0f, -3.0f);
		
		ZEAABoundingBox AABB(Min, Max);
		ZEVector3 center = AABB.GetCenter();
		
		ZEBoundingSphere BS;

		AABB.GenerateBoundingSphere(BS);

		CHECK_EQUAL(center, BS.Position);

		float radius = ZEVector3::Distance(Max, center);
		
		CHECK_EQUAL(radius, BS.Radius);
	}

	TEST(AABB_GenerateBoundingSphere2)
	{
		/*ZEPoint3 Max( 1.0f,  2.0f,  3.0f);
		ZEPoint3 Min(-1.0f, -2.0f, -3.0f);
		
		ZEAABoundingBox AABB(Min, Max);
		ZEVector3 center = AABB.GetCenter();

		ZEMatrix4x4 M(1.0f, 0.0f, 0.0f, 0.0f, 
					  0.0f, 1.0f, 0.0f, 0.0f, 
					  0.0f, 0.0f, 1.0f, 0.0f, 
					  0.0f, 0.0f, 0.0f, 1.0f);
		
		ZEBoundingSphere BS;

		AABB.GenerateBoundingSphere(BS, M);

		CHECK_EQUAL(center, BS.Position);

		float radius = ZEVector3::Distance(Max, center);
		
		CHECK_EQUAL(radius, BS.Radius);*/

	}

	TEST(AABB_GenerateOBoundingBox1)
	{
		ZEPoint3 Max( 1.0f,  2.0f,  5.0f);
		ZEPoint3 Min(-1.0f, -2.0f, -1.0f);
		
		ZEAABoundingBox AABB(Min, Max);
		ZEVector3 center = AABB.GetCenter();
		
		ZEOBoundingBox OBB;

		AABB.GenerateOBoundingBox(OBB);

		CHECK_EQUAL(OBB.U, ZEVector3(2.0f, 0.0f, 0.0f));
		CHECK_EQUAL(OBB.V, ZEVector3(0.0f, 4.0f, 0.0f));
		CHECK_EQUAL(OBB.N, ZEVector3(0.0f, 0.0f, 6.0f));
	}
	
	TEST(AABB_CollisionTest1)
	{
		ZEPoint3 Max1( 1.0f,  2.0f,  5.0f);
		ZEPoint3 Min1(-1.0f, -2.0f, -1.0f);
		
		ZEAABoundingBox AABB1(Min1, Max1);

		ZEPoint3 Max2( 1.0f,  2.0f,  3.0f);
		ZEPoint3 Min2(-1.0f, -2.0f, -3.0f);
		
		ZEAABoundingBox AABB2(Min2, Max2);

		bool result = ZEAABoundingBox::CollisionTest(AABB1, AABB2);
		CHECK_EQUAL(result, true);
	
		bool result2 = ZEAABoundingBox::CollisionTest(AABB2, AABB2);
		CHECK_EQUAL(result2, true);

		ZEPoint3 Max3(0.0f, 0.0f, 0.0f);
		ZEPoint3 Min3(-10.0f, -10.0f, -10.0f);
		ZEAABoundingBox AABB3(Min3, Max3);

		bool result3 = ZEAABoundingBox::CollisionTest(AABB1, AABB3);
		CHECK_EQUAL(result3, true);
		
		ZEPoint3 Max4(0.0f,  2.0f,  3.0f);
		ZEPoint3 Min4(0.0f, -2.0f, -3.0f);
		ZEAABoundingBox AABB4(Min4, Max4);

		bool result4 = ZEAABoundingBox::CollisionTest(AABB1, AABB4);
		CHECK_EQUAL(result4, true);


		ZEPoint3 Max5(20.0f,  20.0f,  30.0f);
		ZEPoint3 Min5(10.0f, 6.0f, 23.0f);
		ZEAABoundingBox AABB5(Min5, Max5);

		bool result5 = ZEAABoundingBox::CollisionTest(AABB1, AABB5);
		CHECK_EQUAL(result5, false);

		ZEPoint3 Max6( 1.0f,  2.0f, 10.0f);
		ZEPoint3 Min6(-1.0f, -2.0f, 6.0f);
		
		ZEAABoundingBox AABB6(Min6, Max6);

		bool result6 = ZEAABoundingBox::CollisionTest(AABB1, AABB6);
		CHECK_EQUAL(result6, false);

	}

	TEST(AABB_CollisionTest2)
	{
		ZEPoint3 Max1( 1.0f,  2.0f,  5.0f);
		ZEPoint3 Min1(-1.0f, -2.0f, -1.0f);
		
		ZEAABoundingBox AABB1(Min1, Max1);

		ZEBoundingSphere BS1(Max1, 3.0f);
		bool result1 = ZEAABoundingBox::CollisionTest(AABB1, BS1);
		CHECK_EQUAL(result1, true);
		
		ZEBoundingSphere BS2(ZEPoint3(-10.0f, 0.0f, 0.0f), 0.1f);
		bool result2 = ZEAABoundingBox::CollisionTest(AABB1, BS2);
		CHECK_EQUAL(result2, false);

		AABB1.GenerateBoundingSphere(BS1);
		bool result3 = ZEAABoundingBox::CollisionTest(AABB1, BS1);
		CHECK_EQUAL(result3, true);
		
		ZEBoundingSphere BS3(ZEPoint3(1.0f, 2.0f, 6.0f), 1.0f);
		bool result4 = ZEAABoundingBox::CollisionTest(AABB1, BS3);
		CHECK_EQUAL(result4, true);

		ZEBoundingSphere BS4(ZEPoint3(0.0f, 0.0f, 3.0f), 0.01f);
		bool result5 = ZEAABoundingBox::CollisionTest(AABB1, BS4);
		CHECK_EQUAL(result5, true);
	}

	TEST(AABB_CollisionTest3)
	{
		//No function
	}



}

