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
#include <math.h>


ZETestSuite(ZEAABBox)
{
	ZETest(AABB_Contructor)
	{
		ZEVector3 Max( 1.0f,  2.0f,  3.0f);
		ZEVector3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABBox AABB(Min, Max);

		ZETestCheckEqual(AABB.Max, ZEVector3(1.0f, 2.0f, 3.0f));
		ZETestCheckEqual(AABB.Min, ZEVector3(-1.0f, -2.0f, -3.0f));
	}

	ZETest(AABB_GetCenter)
	{
		ZEVector3 Max( 1.0f,  2.0f,  3.0f);
		ZEVector3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABBox AABB(Min, Max);

		ZEVector3 Center = AABB.GetCenter();

		ZETestCheckEqual(Center, ZEVector3(0.0f, 0.0f, 0.0f));
	}

	ZETest(AABB_GetVertex)
	{
		ZEVector3 Max( 1.0f,  2.0f,  3.0f);
		ZEVector3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABBox AABB(Min, Max);
		ZEVector3 Vertex1  = AABB.GetVertex(0);
		ZEVector3 Vertex2  = AABB.GetVertex(1);
		ZEVector3 Vertex3  = AABB.GetVertex(2);
		ZEVector3 Vertex4  = AABB.GetVertex(3);
		ZEVector3 Vertex5  = AABB.GetVertex(4);
		ZEVector3 Vertex6  = AABB.GetVertex(5);
		ZEVector3 Vertex7  = AABB.GetVertex(6);
		ZEVector3 Vertex8  = AABB.GetVertex(7);

		ZETestCheckEqual(Vertex1, ZEVector3(-1.0f, -2.0f, -3.0f));
		ZETestCheckEqual(Vertex2, ZEVector3(-1.0f, -2.0f,  3.0f));
		ZETestCheckEqual(Vertex3, ZEVector3(-1.0f,  2.0f, -3.0f));
		ZETestCheckEqual(Vertex4, ZEVector3(-1.0f,  2.0f,  3.0f));
		ZETestCheckEqual(Vertex5, ZEVector3( 1.0f, -2.0f, -3.0f));
		ZETestCheckEqual(Vertex6, ZEVector3( 1.0f, -2.0f,  3.0f));
		ZETestCheckEqual(Vertex7, ZEVector3( 1.0f,  2.0f, -3.0f));
		ZETestCheckEqual(Vertex8, ZEVector3( 1.0f,  2.0f,  3.0f));
	}

	ZETest(AABB_GetLength)
	{
		ZEVector3 Max( 1.0f,  2.0f,  3.0f);
		ZEVector3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABBox AABB(Min, Max);

		float Length = AABB.GetLenght();

		ZETestCheckEqual(Length, sqrt(56.0f));
	}

	ZETest(AABB_Transform)
	{
		ZEVector3 Max( 1.0f,  2.0f,  3.0f);
		ZEVector3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABBox AABB1(Min, Max);

		ZEAABBox AABB;

		ZEMatrix4x4 M( 1.0f,  1.0f,  2.0f,  3.0f, 
					   5.0f,  8.0f, 13.0f, 21.0f, 
					  34.0f, 55.0f,  1.0f,  1.0f, 
					   3.0f,  5.0f,  8.0f, 13.0f);
		ZEAABBox::Transform(AABB, AABB1, M);

		ZETestCheckEqual(AABB.Max, ZEVector3(116.0f, 187.0f, 39.0f));
		ZETestCheckEqual(AABB.Min, ZEVector3(-110.0f, -177.0f, -23.0f));

		ZEVector3 Max2( 1.0f,  2.0f,  5.0f);
		ZEVector3 Min2(-1.0f, -2.0f, -1.0f);
		ZEAABBox AABB2(Min2, Max2);
		ZEAABBox AABB3;

		ZEAABBox::Transform(AABB3, AABB2, ZEMatrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 
															  0.0f, 1.0f, 0.0f, 0.0f, 
															  0.0f, 0.0f, 1.0f, 0.0f, 
															  0.0f, 0.0f, 0.0f, 1.0f));

		ZETestCheckEqual(AABB3.Max, AABB2.Max);
		ZETestCheckEqual(AABB3.Min, AABB2.Min);



	}

	ZETest(AABB_PlaneHalfSpaceTest)
	{
		ZEVector3 Max( 1.0f,  2.0f,  3.0f);
		ZEVector3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABBox AABB(Min, Max);

		ZEVector3 n1(1.0f, 2.0f, 3.0f);
		ZEVector3 p1(100.0f, 100.0f, 100.0f);
		ZEPlane P1(n1, p1);

		ZEHalfSpace result1 = ZEAABBox::IntersectionTest(AABB, P1);

		ZETestCheckEqual(result1,-1);

		ZEVector3 n2(1.0f, 2.0f, 3.0f);
		ZEVector3 p2(0.0f, -6.0f, 0.0f);
		ZEPlane P2(n2, p2);

		ZEHalfSpace result2 = ZEAABBox::IntersectionTest(AABB, P2);

		ZETestCheckEqual(result2, ZE_HS_INTERSECTS);

		
		ZEVector3 n3(1.0f, 0.0f, 0.0f);
		ZEVector3 p3(0.0f, 0.0f, 0.0f);
		ZEPlane P3(n3, p3);

		ZEHalfSpace result3 = ZEAABBox::IntersectionTest(AABB, P3);

		ZETestCheckEqual(result3, ZE_HS_INTERSECTS);

		ZEVector3 n4(0.0f, 0.0f, 1.0f);
		ZEVector3 p4(-200.0f, -1000.0f, -100.0f);
		ZEPlane P4(n4, p4);


		ZEHalfSpace result4 = ZEAABBox::IntersectionTest(AABB, P4);

		ZETestCheckEqual(result4, 1);

	}

	ZETest(AABB_IntersectionTest1)
	{
		
		ZEVector3 Max( 1.0f,  2.0f,  3.0f);
		ZEVector3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABBox AABB(Min, Max);


		bool result = ZEAABBox::IntersectionTest(AABB, ZEVector3(1.0f, 2.0f, 3.0f));

		ZETestCheckEqual(result, true);

		ZEVector3 P(-1.0f, -2.0f, -3.0f);

		bool result2 = ZEAABBox::IntersectionTest(AABB, P);

		ZETestCheckEqual(result2, true);

	}
	ZETest(AABB_IntersectionTest2)
	{
		ZEVector3 Max( 1.0f,  2.0f,  3.0f);
		ZEVector3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABBox AABB1(Min, Max);


		ZEVector3 P0( 1.0f,  2.0f,  3.0f);
		ZEVector3 P1(-1.0f, -2.0f, -3.0f);

		ZELine L1(P0, P1);


		bool result1 = ZEAABBox::IntersectionTest(AABB1, L1);


		ZETestCheckEqual(result1, false);


		ZEVector3 P2( 0.0f,  0.0f,  0.0f);
		ZEVector3 P3(-1.0f, -2.0f, -3.0f);

		ZELine L2(P2, P3);

		bool result2 = ZEAABBox::IntersectionTest(AABB1, L2);

		ZETestCheckEqual(result2, false);
		
		ZEVector3 P4(0.0f, 100.0f, 0.0f);
		ZEVector3 P5(0.0f, 100.0f, 6.0f);

		ZELine L3(P4, P5);

		bool result3 = ZEAABBox::IntersectionTest(AABB1, L3);

		ZETestCheckEqual(result3, false);


		ZEVector3 P6(0.0f, 0.0f,  0.0f);
		ZEVector3 P7(0.0f, 0.0f,  1.0f);

		ZELine L4(P6, P7);


		bool result4 = ZEAABBox::IntersectionTest(AABB1, L4);


		ZETestCheckEqual(result4, true);
	}
	
	ZETest(AABB_IntersectionTest3)
	{
		ZEVector3 Max( 1.0f,  2.0f,  3.0f);
		ZEVector3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABBox AABB1(Min, Max);

		ZEVector3 p1(0.9f, 1.9f, 2.9f);
		ZEVector3 v1(0.0f, 0.0f, 1.0f);
		ZERay R1(v1,p1);

		bool result1 = ZEAABBox::IntersectionTest(AABB1, R1);
		ZETestCheckEqual(result1, true);

		ZEVector3 p2(100.0f, 100.0f, 100.0f);
		ZEVector3 v2(0.0f, 0.0f, 1.0f);
		ZERay R2(v2, p2);

		bool result2 = ZEAABBox::IntersectionTest(AABB1, R2);
		ZETestCheckEqual(result2, false);

		ZEVector3 p3(0.0f, 0.0f, 0.0f);
		ZEVector3 v3(1.0f, 0.0f, 0.0f);
		ZERay R3(v3, p3);

		bool result3 = ZEAABBox::IntersectionTest(AABB1, R3);
		ZETestCheckEqual(result3, true);

		ZEVector3 p4(-1.0f, -2.0f, -3.0f);
		ZEVector3 v4(2.0f,  4.0f,  6.0f);
		ZERay R4(v4, p4);
		
		bool result4 = ZEAABBox::IntersectionTest(AABB1, R4);
		ZETestCheckEqual(result4, true);
		

		ZEVector3  p5(1.0, 2.0f, 3.0f);
		ZEVector3 v5(1.0, 0.0f, 0.0f);
		ZERay R5(v5, p5);

		bool result5 = ZEAABBox::IntersectionTest(AABB1, R5);
		ZETestCheckEqual(result5, true);
	}

	ZETest(AABB_IntersectionTest4)
	{
		ZEVector3 Max( 1.0f,  2.0f,  3.0f);
		ZEVector3 Min(-1.0f, -2.0f, -3.0f);
		ZEAABBox AABB1(Min, Max);


		ZEVector3 P0( 1.0f,  2.0f,  3.0f);
		ZEVector3 P1(-1.0f, -2.0f, -3.0f);

		ZELineSegment LS1(P0, P1);


		bool result1 = ZEAABBox::IntersectionTest(AABB1, LS1);


		ZETestCheckEqual(result1, true);


		ZEVector3 P2( 0.0f,  0.0f,  0.0f);
		ZEVector3 P3(-1.0f, -2.0f, -3.0f);

		ZELineSegment LS2(P2, P3);


		bool result2 = ZEAABBox::IntersectionTest(AABB1, LS2);


		ZETestCheckEqual(result2, false);

		
		ZEVector3 P4(0.0f, 100.0f, 0.0f);
		ZEVector3 P5(0.0f, 100.0f, 6.0f);

		ZELineSegment LS3(P4, P5);


		bool result3 = ZEAABBox::IntersectionTest(AABB1,LS3);
	}

	ZETest(AABB_GenerateBoundingSphere1)
	{
		ZEVector3 Max( 1.0f,  2.0f,  3.0f);
		ZEVector3 Min(-1.0f, -2.0f, -3.0f);
		
		ZEAABBox AABB(Min, Max);
		ZEVector3 center = AABB.GetCenter();
		
		ZEBSphere BS;

		//AABB.GenerateBoundingSphere(BS);

		ZETestCheckEqual(center, BS.Position);

		float radius = ZEVector3::Distance(Max, center);
		
		ZETestCheckEqual(radius, BS.Radius);
	}

	ZETest(AABB_GenerateBoundingSphere2)
	{
		/*ZEVector3 Max( 1.0f,  2.0f,  3.0f);
		ZEVector3 Min(-1.0f, -2.0f, -3.0f);
		
		ZEAABBox AABB(Min, Max);
		ZEVector3 center = AABB.GetCenter();

		ZEMatrix4x4 M(1.0f, 0.0f, 0.0f, 0.0f, 
					  0.0f, 1.0f, 0.0f, 0.0f, 
					  0.0f, 0.0f, 1.0f, 0.0f, 
					  0.0f, 0.0f, 0.0f, 1.0f);
		
		ZEBSphere BS;

		AABB.GenerateBoundingSphere(BS, M);

		CHECK_EQUAL(center, BS.Position);

		float radius = ZEVector3::Distance(Max, center);
		
		CHECK_EQUAL(radius, BS.Radius);*/

	}

	ZETest(AABB_GenerateOBoundingBox1)
	{
		ZEVector3 Max( 1.0f,  2.0f,  5.0f);
		ZEVector3 Min(-1.0f, -2.0f, -1.0f);
		
		ZEAABBox AABB(Min, Max);
		ZEVector3 center = AABB.GetCenter();
		
		ZEOBBox OBB;

//		AABB.GenerateOBoundingBox(OBB);

		ZETestCheckEqual(OBB.Right, ZEVector3(2.0f, 0.0f, 0.0f));
		ZETestCheckEqual(OBB.Up, ZEVector3(0.0f, 4.0f, 0.0f));
		ZETestCheckEqual(OBB.Front, ZEVector3(0.0f, 0.0f, 6.0f));
	}
	
	ZETest(AABB_CollisionTest1)
	{
		ZEVector3 Max1( 1.0f,  2.0f,  5.0f);
		ZEVector3 Min1(-1.0f, -2.0f, -1.0f);
		
		ZEAABBox AABB1(Min1, Max1);

		ZEVector3 Max2( 1.0f,  2.0f,  3.0f);
		ZEVector3 Min2(-1.0f, -2.0f, -3.0f);
		
		ZEAABBox AABB2(Min2, Max2);

		bool result = ZEAABBox::IntersectionTest(AABB1, AABB2);
		ZETestCheckEqual(result, true);
	
		bool result2 = ZEAABBox::IntersectionTest(AABB2, AABB2);
		ZETestCheckEqual(result2, true);

		ZEVector3 Max3(0.0f, 0.0f, 0.0f);
		ZEVector3 Min3(-10.0f, -10.0f, -10.0f);
		ZEAABBox AABB3(Min3, Max3);

		bool result3 = ZEAABBox::IntersectionTest(AABB1, AABB3);
		ZETestCheckEqual(result3, true);
		
		ZEVector3 Max4(0.0f,  2.0f,  3.0f);
		ZEVector3 Min4(0.0f, -2.0f, -3.0f);
		ZEAABBox AABB4(Min4, Max4);

		bool result4 = ZEAABBox::IntersectionTest(AABB1, AABB4);
		ZETestCheckEqual(result4, true);


		ZEVector3 Max5(20.0f,  20.0f,  30.0f);
		ZEVector3 Min5(10.0f, 6.0f, 23.0f);
		ZEAABBox AABB5(Min5, Max5);

		bool result5 = ZEAABBox::IntersectionTest(AABB1, AABB5);
		ZETestCheckEqual(result5, false);

		ZEVector3 Max6( 1.0f,  2.0f, 10.0f);
		ZEVector3 Min6(-1.0f, -2.0f, 6.0f);
		
		ZEAABBox AABB6(Min6, Max6);

		bool result6 = ZEAABBox::IntersectionTest(AABB1, AABB6);
		ZETestCheckEqual(result6, false);

	}

	ZETest(AABB_CollisionTest2)
	{
		ZEVector3 Max1( 1.0f,  2.0f,  5.0f);
		ZEVector3 Min1(-1.0f, -2.0f, -1.0f);
		
		ZEAABBox AABB1(Min1, Max1);

		ZEBSphere BS1(Max1, 3.0f);
		bool result1 = ZEAABBox::IntersectionTest(AABB1, BS1);
		ZETestCheckEqual(result1, true);
		
		ZEBSphere BS2(ZEVector3(-10.0f, 0.0f, 0.0f), 0.1f);
		bool result2 = ZEAABBox::IntersectionTest(AABB1, BS2);
		ZETestCheckEqual(result2, false);

//		AABB1.GenerateBoundingSphere(BS1);
		bool result3 = ZEAABBox::IntersectionTest(AABB1, BS1);
		ZETestCheckEqual(result3, true);
		
		ZEBSphere BS3(ZEVector3(1.0f, 2.0f, 6.0f), 1.0f);
		bool result4 = ZEAABBox::IntersectionTest(AABB1, BS3);
		ZETestCheckEqual(result4, true);

		ZEBSphere BS4(ZEVector3(0.0f, 0.0f, 3.0f), 0.01f);
		bool result5 = ZEAABBox::IntersectionTest(AABB1, BS4);
		ZETestCheckEqual(result5, true);
	}

	ZETest(AABB_CollisionTest3)
	{
		//No function
	}
}

