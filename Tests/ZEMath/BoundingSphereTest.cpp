//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - BoundingSphereTest.cpp
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


SUITE(ZEBoundingSphere)
{
	TEST(BS_Constructor)
	{
		ZEPoint3 Position(1.0f, 2.0f, 3.0f);
		float R = 3.0f; 
		ZEBoundingSphere BS(Position, R);

		CHECK_EQUAL(BS.Position, ZEPoint3(1.0f, 2.0f, 3.0f));
		CHECK_EQUAL(BS.Radius, 3.0f);
	}


	TEST(BS_PlaneHalfSpaceTest)
	{
		ZEPoint3 Position(1.0f, 2.0f, 3.0f);
		float R = 3.0f; 
		ZEBoundingSphere BS(Position, R);
		
		ZEVector3 n1(1.0f, 2.0f, 3.0f);
		ZEPoint3 p1(100.0f, 100.0f, 100.0f);
		ZEPlane P1(n1, p1);

		ZEHalfSpace result1 = ZEBoundingSphere::PlaneHalfSpaceTest(BS, P1);

		CHECK_EQUAL(result1,-1);


		ZEVector3 n2(1.0f, 2.0f, 3.0f);
		ZEPoint3 p2(0.0f, -6.0f, 0.0f);
		ZEPlane P2(n2, p2);


		ZEHalfSpace result2 = ZEBoundingSphere::PlaneHalfSpaceTest(BS, P2);

		CHECK_EQUAL(result2, 1);

		
		ZEVector3 n3(1.0f, 0.0f, 0.0f);
		ZEPoint3 p3(0.0f, 0.0f, 0.0f);
		ZEPlane P3(n3, p3);


		ZEHalfSpace result3 = ZEBoundingSphere::PlaneHalfSpaceTest(BS, P3);

		CHECK_EQUAL(result3, ZEHALFSPACE_INTERSECTS);
		

		
		
		ZEVector3 n4(0.0f, 0.0f, 1.0f);
		ZEPoint3 p4(-200.0f, -1000.0f, -100.0f);
		ZEPlane P4(n4, p4);


		ZEHalfSpace result4 = ZEBoundingSphere::PlaneHalfSpaceTest(BS, P4);

		CHECK_EQUAL(result4, 1);
	}
}

	TEST(BS_GetSurfaceNormal)
	{	
		ZEPoint3 Position(1.0f, 2.0f, 3.0f);
		float R = 3.0f; 
		ZEBoundingSphere BS(Position, R);

		ZEPoint3 Point(1.0f, 2.0f, 10.0f);
		ZEVector3 Normal;

		ZEBoundingSphere::GetSurfaceNormal(Normal, BS, Point);

		CHECK_EQUAL(Normal, ZEVector3(0.0f, 0.0f, 1.0f));

		
		ZEPoint3 Position2(0.0f, 0.0f, 0.0f);
		float R2 = 6.0f; 
		ZEBoundingSphere BS2(Position2, R2);
		ZEPoint3 Point2(1.0f, 1.0f, 1.0f);

		ZEBoundingSphere::GetSurfaceNormal(Normal, BS2, Point2);

		CHECK_EQUAL(Normal, ZEVector3(1.0f/sqrt(3.0f), 1.0f/sqrt(3.0f), 1.0f/sqrt(3.0f)));

		
		ZEPoint3 Position3(1.0f, 2.0f, 3.0f);
		float R3 = 5.0f; 
		ZEBoundingSphere BS3(Position3, R3);

		ZEPoint3 Point3(1.0f, 1.0f, 1.0f);
		ZEVector3 Normal1;
		ZEVector3 Sub;
		ZEVector3::Sub(Sub,Point3, Position3);

		ZEVector3::Normalize(Normal1, Sub);

		ZEBoundingSphere::GetSurfaceNormal(Normal, BS3, Point3);

		CHECK_EQUAL(Normal, Normal1);

	}
	TEST(BS_IntersectionTest1)
	{
		ZEPoint3 Position(1.0f, 2.0f, 3.0f);
		float R = 3.0f; 
		ZEBoundingSphere BS(Position, R);
		
		ZEPoint3 P1(1.0f, 2.0f, 3.0f);
		
		bool result1 = ZEBoundingSphere::IntersectionTest(BS, P1);
		CHECK_EQUAL(result1, true);

		ZEPoint3 P2(10.0f, 20.0f, 30.0f);
		
		bool result2 = ZEBoundingSphere::IntersectionTest(BS, P2);
		CHECK_EQUAL(result2, false);

		ZEPoint3 P3(1.0f, 2.0f, 6.0f);
		
		bool result3 = ZEBoundingSphere::IntersectionTest(BS, P3);
		CHECK_EQUAL(result3, true);
	}

	TEST(BS_IntersectionTest2)
	{
		ZEPoint3 Position(1.0f, 2.0f, 3.0f);
		float R = 3.0f; 
		ZEBoundingSphere BS(Position, R);

		ZEPoint3 P0(1.0f, 2.0f, 3.0f);
		ZEPoint3 P1(2.0f, 5.0f, 6.0f);
		ZELine L(P0, P1);

		bool result = ZEBoundingSphere::IntersectionTest(BS, L);
		CHECK_EQUAL(result, true);

		ZEPoint3 P2(0.0f, 0.0f,  6.8f);
		ZEPoint3 P3(0.0f, 0.0f, 12.6f);
		ZELine L2(P2, P3);

		bool result2 = ZEBoundingSphere::IntersectionTest(BS, L2);
		CHECK_EQUAL(result2, false);

		ZEPoint3 P4(1.0f, 2.0f, 6.0f);
		ZEPoint3 P5(2.0f, 4.0f, 6.0f);
		ZELine L3(P4, P5);

		bool result3 = ZEBoundingSphere::IntersectionTest(BS, L3);
		CHECK_EQUAL(result3, true);
	}

	TEST(BS_IntersectionTest3)
	{
		ZEPoint3 Position(1.0f, 2.0f, 3.0f);
		float R = 3.0f; 
		ZEBoundingSphere BS(Position, R);

		ZEPoint3 P0(1.0f, 2.0f, 3.0f);
		ZEPoint3 P1(2.0f, 5.0f, 6.0f);
		ZERay R1;
		ZERay::Create(R1, P0, P1);

		bool result = ZEBoundingSphere::IntersectionTest(BS, R1);
		CHECK_EQUAL(result, true);

		ZEPoint3 P2(0.0f, 0.0f,  6.8f);
		ZEPoint3 P3(0.0f, 0.0f, 12.6f);
		ZERay R2;
		ZERay::Create(R2, P2, P3);

		bool result2 = ZEBoundingSphere::IntersectionTest(BS, R2);
		CHECK_EQUAL(result2, false);

		ZEPoint3 P4(1.0f, 2.0f, 6.0f);
		ZEPoint3 P5(2.0f, 4.0f, 6.0f);
		ZERay R3;
		ZERay::Create(R3, P4, P5);

		bool result3 = ZEBoundingSphere::IntersectionTest(BS, R3);
		CHECK_EQUAL(result3, true);

		ZEPoint3 P6(1.0f, 2.0f, 3.0f);
		ZEPoint3 P7(1.5f, 2.5f, 2.5f);
		ZERay R4;
		ZERay::Create(R4, P6, P7);

		bool result4 = ZEBoundingSphere::IntersectionTest(BS, R4);
		CHECK_EQUAL(result4, true);

	}
//
	TEST(BS_IntersectionTest4)
	{
		float MinT;
		float MaxT;

		ZEPoint3 Position(1.0f, 2.0f, 3.0f);
		float R = 3.0f; 
		ZEBoundingSphere BS(Position, R);

		ZEPoint3 P0(1.0f, 2.0f, 3.0f);
		ZEPoint3 P1(2.0f, 5.0f, 6.0f);
		ZERay R1;
		ZERay::Create(R1, P0, P1);

		bool result = ZEBoundingSphere::IntersectionTest(BS, R1, MinT, MaxT);
		CHECK_EQUAL(result, true);

		ZEPoint3 P2(0.0f, 0.0f,  6.8f);
		ZEPoint3 P3(0.0f, 0.0f, 12.6f);
		ZERay R2;
		ZERay::Create(R2, P2, P3);

		bool result2 = ZEBoundingSphere::IntersectionTest(BS, R2, MinT, MaxT);
		CHECK_EQUAL(result2, false);

		ZEPoint3 P4(1.0f, 2.0f, 6.0f);
		ZEPoint3 P5(2.0f, 4.0f, 6.0f);
		ZERay R3;
		ZERay::Create(R3, P4, P5);

		bool result3 = ZEBoundingSphere::IntersectionTest(BS, R3, MinT, MaxT);
		CHECK_EQUAL(result3, true);

		ZEPoint3 P6(1.0f, 2.0f, 3.0f);
		ZEPoint3 P7(1.5f, 2.5f, 2.5f);
		ZERay R4;
		ZERay::Create(R4, P6, P7);

		bool result4 = ZEBoundingSphere::IntersectionTest(BS, R4, MinT, MaxT);
		CHECK_EQUAL(result4, true);	}
//
	TEST(BS_IntersectionTest5)
	{
		ZEPoint3 Position(1.0f, 2.0f, 3.0f);
		float R = 3.0f; 
		ZEBoundingSphere BS(Position, R);

		ZEPoint3 P0(1.0f, 2.0f, 3.0f);
		ZEPoint3 P1(2.0f, 5.0f, 6.0f);
		ZELineSegment LS1(P0, P1);
		

		bool result = ZEBoundingSphere::IntersectionTest(BS, LS1);
		CHECK_EQUAL(result, true);

		ZEPoint3 P2(0.0f, 0.0f,  6.8f);
		ZEPoint3 P3(0.0f, 0.0f, 12.6f);
		ZELineSegment LS2(P2, P3);

		bool result2 = ZEBoundingSphere::IntersectionTest(BS, LS2);
		CHECK_EQUAL(result2, false);

		ZEPoint3 P4(1.0f, 2.0f, 6.0f);
		ZEPoint3 P5(2.0f, 4.0f, 6.0f);
		ZELineSegment LS3(P4, P5);

		bool result3 = ZEBoundingSphere::IntersectionTest(BS, LS3);
		CHECK_EQUAL(result3, true);

		ZEPoint3 P6(1.0f, 2.0f, 3.0f);
		ZEPoint3 P7(1.5f, 2.5f, 2.5f);
		ZELineSegment LS4(P6, P7);

		bool result4 = ZEBoundingSphere::IntersectionTest(BS, LS4);
		CHECK_EQUAL(result4, true);

	}

	TEST(BS_CollisionTest1)
	{
		ZEPoint3 Position(1.0f, 2.0f, 3.0f);
		float R = 3.0f; 
		ZEBoundingSphere BS(Position, R);

		ZEPoint3 Max( 1.0f,  2.0f,  3.0f);
		ZEPoint3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABoundingBox AABB(Min, Max);

		bool result = ZEBoundingSphere::CollisionTest(BS, AABB);
		CHECK_EQUAL(result, true);
		
		ZEPoint3 Max2( 1.0f, 100.0f,  3.0f);
		ZEPoint3 Min2(-1.0f,  98.0f, -3.0f);
		ZEAABoundingBox AABB2(Min2, Max2);

		bool result2 = ZEBoundingSphere::CollisionTest(BS, AABB2);
		CHECK_EQUAL(result2, false);
		
		ZEPoint3 Max3( 1.0f, 7.0f,  3.0f);
		ZEPoint3 Min3(-1.0f, 5.0f, -3.0f);
		ZEAABoundingBox AABB3(Min3, Max3);

		bool result3 = ZEBoundingSphere::CollisionTest(BS, AABB3);
		CHECK_EQUAL(result3, true);

	}

	TEST(BS_CollisionTest2)
	{
		ZEPoint3 Position1(1.0f, 2.0f, 3.0f);
		float R1 = 3.0f; 
		ZEBoundingSphere BS1(Position1, R1);
		
		ZEPoint3 Position2(1.0f, 2.0f, 3.0f);
		float R2 = 1.0f; 
		ZEBoundingSphere BS2(Position2, R2);

		bool result = ZEBoundingSphere::CollisionTest(BS1, BS2);
		CHECK_EQUAL(result, true);

		ZEPoint3 Position3(1.0f, 5.0f, 3.0f);
		float R3 = 5.0f; 
		ZEBoundingSphere BS3(Position3, R3);

		bool result2 = ZEBoundingSphere::CollisionTest(BS1, BS3);
		CHECK_EQUAL(result, true);
		
		ZEPoint3 Position4(1.0f, 2.0f, 7.0f);
		float R4 = 1.0f; 
		ZEBoundingSphere BS4(Position4, R4);

		bool result3 = ZEBoundingSphere::CollisionTest(BS1, BS4);
		CHECK_EQUAL(result3, true);

		ZEPoint3 Position5(0.0f, 0.0f, 15.0f);
		float R5 = 1.0f; 
		ZEBoundingSphere BS5(Position5, R5);

		bool result4 = ZEBoundingSphere::CollisionTest(BS1, BS5);
		CHECK_EQUAL(result4, false);
	}

	TEST(BS_CollisionTest3)
	{
		//Nofuntion
	}
