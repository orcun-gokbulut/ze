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
#include <math.h>

ZETestSuite(ZEBSphere)
{
	ZETest("BS_Constructor")
	{
		ZEVector3 Position(1.0f, 2.0f, 3.0f);
		float R = 3.0f; 
		ZEBSphere BS(Position, R);

		ZETestCheckEqual(BS.Position, ZEVector3(1.0f, 2.0f, 3.0f));
		ZETestCheckEqual(BS.Radius, 3.0f);
	}


	ZETest("BS_PlaneHalfSpaceTest")
	{
		ZEVector3 Position(1.0f, 2.0f, 3.0f);
		float R = 3.0f; 
		ZEBSphere BS(Position, R);
		
		ZEVector3 n1(1.0f, 2.0f, 3.0f);
		ZEVector3 p1(100.0f, 100.0f, 100.0f);
		ZEPlane P1(n1, p1);

		ZEHalfSpace result1 = ZEBSphere::IntersectionTest(BS, P1);

		ZETestCheckEqual(result1,-1);


		ZEVector3 n2(1.0f, 2.0f, 3.0f);
		ZEVector3 p2(0.0f, -6.0f, 0.0f);
		ZEPlane P2(n2, p2);


		ZEHalfSpace result2 = ZEBSphere::IntersectionTest(BS, P2);

		ZETestCheckEqual(result2, 1);

		
		ZEVector3 n3(1.0f, 0.0f, 0.0f);
		ZEVector3 p3(0.0f, 0.0f, 0.0f);
		ZEPlane P3(n3, p3);


		ZEHalfSpace result3 = ZEBSphere::IntersectionTest(BS, P3);

		ZETestCheckEqual(result3, ZE_HS_INTERSECTS);
		

		
		
		ZEVector3 n4(0.0f, 0.0f, 1.0f);
		ZEVector3 p4(-200.0f, -1000.0f, -100.0f);
		ZEPlane P4(n4, p4);


		ZEHalfSpace result4 = ZEBSphere::IntersectionTest(BS, P4);

		ZETestCheckEqual(result4, 1);
	}

	ZETest("BS_GetSurfaceNormal")
	{	
		ZEVector3 Position(1.0f, 2.0f, 3.0f);
		float R = 3.0f; 
		ZEBSphere BS(Position, R);

		ZEVector3 Point(1.0f, 2.0f, 10.0f);
		ZEVector3 Normal;

		ZEBSphere::GetSurfaceNormal(Normal, BS, Point);

		ZETestCheckEqual(Normal, ZEVector3(0.0f, 0.0f, 1.0f));

		
		ZEVector3 Position2(0.0f, 0.0f, 0.0f);
		float R2 = 6.0f; 
		ZEBSphere BS2(Position2, R2);
		ZEVector3 Point2(1.0f, 1.0f, 1.0f);

		ZEBSphere::GetSurfaceNormal(Normal, BS2, Point2);

		ZETestCheckEqual(Normal, ZEVector3(1.0f/sqrt(3.0f), 1.0f/sqrt(3.0f), 1.0f/sqrt(3.0f)));

		
		ZEVector3 Position3(1.0f, 2.0f, 3.0f);
		float R3 = 5.0f; 
		ZEBSphere BS3(Position3, R3);

		ZEVector3 Point3(1.0f, 1.0f, 1.0f);
		ZEVector3 Normal1;
		ZEVector3 Sub;
		ZEVector3::Sub(Sub,Point3, Position3);

		ZEVector3::Normalize(Normal1, Sub);

		ZEBSphere::GetSurfaceNormal(Normal, BS3, Point3);

		ZETestCheckEqual(Normal, Normal1);

	}
	ZETest("BS_IntersectionTest1")
	{
		ZEVector3 Position(1.0f, 2.0f, 3.0f);
		float R = 3.0f; 
		ZEBSphere BS(Position, R);
		
		ZEVector3 P1(1.0f, 2.0f, 3.0f);
		
		bool result1 = ZEBSphere::IntersectionTest(BS, P1);
		ZETestCheckEqual(result1, true);

		ZEVector3 P2(10.0f, 20.0f, 30.0f);
		
		bool result2 = ZEBSphere::IntersectionTest(BS, P2);
		ZETestCheckEqual(result2, false);

		ZEVector3 P3(1.0f, 2.0f, 6.0f);
		
		bool result3 = ZEBSphere::IntersectionTest(BS, P3);
		ZETestCheckEqual(result3, true);
	}

	ZETest("BS_IntersectionTest2")
	{
		ZEVector3 Position(1.0f, 2.0f, 3.0f);
		float R = 3.0f; 
		ZEBSphere BS(Position, R);

		ZEVector3 P0(1.0f, 2.0f, 3.0f);
		ZEVector3 P1(2.0f, 5.0f, 6.0f);
		ZELine L(P0, P1);

		bool result = ZEBSphere::IntersectionTest(BS, L);
		ZETestCheckEqual(result, true);

		ZEVector3 P2(0.0f, 0.0f,  6.8f);
		ZEVector3 P3(0.0f, 0.0f, 12.6f);
		ZELine L2(P2, P3);

		bool result2 = ZEBSphere::IntersectionTest(BS, L2);
		ZETestCheckEqual(result2, false);

		ZEVector3 P4(1.0f, 2.0f, 6.0f);
		ZEVector3 P5(2.0f, 4.0f, 6.0f);
		ZELine L3(P4, P5);

		bool result3 = ZEBSphere::IntersectionTest(BS, L3);
		ZETestCheckEqual(result3, true);
	}

	ZETest("BS_IntersectionTest3")
	{
		ZEVector3 Position(1.0f, 2.0f, 3.0f);
		float R = 3.0f; 
		ZEBSphere BS(Position, R);

		ZEVector3 P0(1.0f, 2.0f, 3.0f);
		ZEVector3 P1(2.0f, 5.0f, 6.0f);
		ZERay R1;
		ZERay::Create(R1, P0, P1);

		bool result = ZEBSphere::IntersectionTest(BS, R1);
		ZETestCheckEqual(result, true);

		ZEVector3 P2(0.0f, 0.0f,  6.8f);
		ZEVector3 P3(0.0f, 0.0f, 12.6f);
		ZERay R2;
		ZERay::Create(R2, P2, P3);

		bool result2 = ZEBSphere::IntersectionTest(BS, R2);
		ZETestCheckEqual(result2, false);

		ZEVector3 P4(1.0f, 2.0f, 6.0f);
		ZEVector3 P5(2.0f, 4.0f, 6.0f);
		ZERay R3;
		ZERay::Create(R3, P4, P5);

		bool result3 = ZEBSphere::IntersectionTest(BS, R3);
		ZETestCheckEqual(result3, true);

		ZEVector3 P6(1.0f, 2.0f, 3.0f);
		ZEVector3 P7(1.5f, 2.5f, 2.5f);
		ZERay R4;
		ZERay::Create(R4, P6, P7);

		bool result4 = ZEBSphere::IntersectionTest(BS, R4);
		ZETestCheckEqual(result4, true);

	}
//
	ZETest("BS_IntersectionTest4")
	{
		float MinT;
		float MaxT;

		ZEVector3 Position(1.0f, 2.0f, 3.0f);
		float R = 3.0f; 
		ZEBSphere BS(Position, R);

		ZEVector3 P0(1.0f, 2.0f, 3.0f);
		ZEVector3 P1(2.0f, 5.0f, 6.0f);
		ZERay R1;
		ZERay::Create(R1, P0, P1);

		bool result = ZEBSphere::IntersectionTest(BS, R1, MinT, MaxT);
		ZETestCheckEqual(result, true);

		ZEVector3 P2(0.0f, 0.0f,  6.8f);
		ZEVector3 P3(0.0f, 0.0f, 12.6f);
		ZERay R2;
		ZERay::Create(R2, P2, P3);

		bool result2 = ZEBSphere::IntersectionTest(BS, R2, MinT, MaxT);
		ZETestCheckEqual(result2, false);

		ZEVector3 P4(1.0f, 2.0f, 6.0f);
		ZEVector3 P5(2.0f, 4.0f, 6.0f);
		ZERay R3;
		ZERay::Create(R3, P4, P5);

		bool result3 = ZEBSphere::IntersectionTest(BS, R3, MinT, MaxT);
		ZETestCheckEqual(result3, true);

		ZEVector3 P6(1.0f, 2.0f, 3.0f);
		ZEVector3 P7(1.5f, 2.5f, 2.5f);
		ZERay R4;
		ZERay::Create(R4, P6, P7);

		bool result4 = ZEBSphere::IntersectionTest(BS, R4, MinT, MaxT);
		ZETestCheckEqual(result4, true);	}
//
	ZETest("BS_IntersectionTest5")
	{
		ZEVector3 Position(1.0f, 2.0f, 3.0f);
		float R = 3.0f; 
		ZEBSphere BS(Position, R);

		ZEVector3 P0(1.0f, 2.0f, 3.0f);
		ZEVector3 P1(2.0f, 5.0f, 6.0f);
		ZELineSegment LS1(P0, P1);
		

		bool result = ZEBSphere::IntersectionTest(BS, LS1);
		ZETestCheckEqual(result, true);

		ZEVector3 P2(0.0f, 0.0f,  6.8f);
		ZEVector3 P3(0.0f, 0.0f, 12.6f);
		ZELineSegment LS2(P2, P3);

		bool result2 = ZEBSphere::IntersectionTest(BS, LS2);
		ZETestCheckEqual(result2, false);

		ZEVector3 P4(1.0f, 2.0f, 6.0f);
		ZEVector3 P5(2.0f, 4.0f, 6.0f);
		ZELineSegment LS3(P4, P5);

		bool result3 = ZEBSphere::IntersectionTest(BS, LS3);
		ZETestCheckEqual(result3, true);

		ZEVector3 P6(1.0f, 2.0f, 3.0f);
		ZEVector3 P7(1.5f, 2.5f, 2.5f);
		ZELineSegment LS4(P6, P7);

		bool result4 = ZEBSphere::IntersectionTest(BS, LS4);
		ZETestCheckEqual(result4, true);

	}

	ZETest("BS_CollisionTest1")
	{
		ZEVector3 Position(1.0f, 2.0f, 3.0f);
		float R = 3.0f; 
		ZEBSphere BS(Position, R);

		ZEVector3 Max( 1.0f,  2.0f,  3.0f);
		ZEVector3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABBox AABB(Min, Max);

		bool result = ZEBSphere::IntersectionTest(BS, AABB);
		ZETestCheckEqual(result, true);
		
		ZEVector3 Max2( 1.0f, 100.0f,  3.0f);
		ZEVector3 Min2(-1.0f,  98.0f, -3.0f);
		ZEAABBox AABB2(Min2, Max2);

		bool result2 = ZEBSphere::IntersectionTest(BS, AABB2);
		ZETestCheckEqual(result2, false);
		
		ZEVector3 Max3( 1.0f, 7.0f,  3.0f);
		ZEVector3 Min3(-1.0f, 5.0f, -3.0f);
		ZEAABBox AABB3(Min3, Max3);

		bool result3 = ZEBSphere::IntersectionTest(BS, AABB3);
		ZETestCheckEqual(result3, true);

	}

	ZETest("BS_CollisionTest2")
	{
		ZEVector3 Position1(1.0f, 2.0f, 3.0f);
		float R1 = 3.0f; 
		ZEBSphere BS1(Position1, R1);
		
		ZEVector3 Position2(1.0f, 2.0f, 3.0f);
		float R2 = 1.0f; 
		ZEBSphere BS2(Position2, R2);

		bool result = ZEBSphere::IntersectionTest(BS1, BS2);
		ZETestCheckEqual(result, true);

		ZEVector3 Position3(1.0f, 5.0f, 3.0f);
		float R3 = 5.0f; 
		ZEBSphere BS3(Position3, R3);

		bool result2 = ZEBSphere::IntersectionTest(BS1, BS3);
		ZETestCheckEqual(result2, true);
		
		ZEVector3 Position4(1.0f, 2.0f, 7.0f);
		float R4 = 1.0f; 
		ZEBSphere BS4(Position4, R4);

		bool result3 = ZEBSphere::IntersectionTest(BS1, BS4);
		ZETestCheckEqual(result3, true);

		ZEVector3 Position5(0.0f, 0.0f, 15.0f);
		float R5 = 1.0f; 
		ZEBSphere BS5(Position5, R5);

		bool result4 = ZEBSphere::IntersectionTest(BS1, BS5);
		ZETestCheckEqual(result4, false);
	}

	ZETest("BS_CollisionTest3")
	{
		//Nofuntion
	}
}
