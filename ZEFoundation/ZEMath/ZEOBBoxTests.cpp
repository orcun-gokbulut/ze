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
#include <math.h>

ZETestSuite(ZEOBBox)
{
	ZETest("OBB_Constructor")
	{
		ZEVector3 Position(1.0f, 2.0f, 3.0f);
		ZEVector3 U(1.0f, 0.0f, 0.0f);
		ZEVector3 V(0.0f, 1.0f, 0.0f);
		ZEVector3 N(0.0f, 0.0f, 1.0f);

		ZEOBBox OBB(Position, U, V, N, ZEVector3(0.5f, 0.5f, 0.5f));

		ZETestCheckEqual(OBB.Center, ZEVector3(1.0f, 2.0f, 3.0f));
		ZETestCheckEqual(OBB.Right, ZEVector3(1.0f, 0.0f, 0.0f));
		ZETestCheckEqual(OBB.Up, ZEVector3(0.0f, 1.0f, 0.0f));
		ZETestCheckEqual(OBB.Front, ZEVector3(0.0f, 0.0f, 1.0f));

	}

	ZETest("OBB_GetCenter")
	{
		//No funtion
	}

	ZETest("OBB_GetVertex")
	{
		ZEOBBox OBB(ZEVector3(1.0f, 2.0f, 3.0f), 
			ZEVector3::UnitX, ZEVector3::UnitY, ZEVector3::UnitZ, 
			ZEVector3(1.0f, 1.0f, 1.0f));


		ZETestCheckEqual(OBB.GetVertex(0), ZEVector3(1.0f, 2.0f, 3.0f));
		ZETestCheckEqual(OBB.GetVertex(1), ZEVector3(2.0f, 2.0f, 3.0f));
		ZETestCheckEqual(OBB.GetVertex(2), ZEVector3(1.0f, 3.0f, 3.0f));
		ZETestCheckEqual(OBB.GetVertex(3), ZEVector3(2.0f, 3.0f, 3.0f));
		ZETestCheckEqual(OBB.GetVertex(4), ZEVector3(1.0f, 2.0f, 4.0f));
		ZETestCheckEqual(OBB.GetVertex(5), ZEVector3(2.0f, 2.0f, 4.0f));
		ZETestCheckEqual(OBB.GetVertex(6), ZEVector3(1.0f, 3.0f, 4.0f));
		ZETestCheckEqual(OBB.GetVertex(7), ZEVector3(2.0f, 3.0f, 4.0f));
	}

	ZETest("OBB_PlaneHalfSpaceTest")
	{
		
		ZEVector3 Position(1.0f, 2.0f, 3.0f);
		ZEVector3 U(1.0f, 0.0f, 0.0f);
		ZEVector3 V(0.0f, 1.0f, 0.0f);
		ZEVector3 N(0.0f, 0.0f, 1.0f);

		ZEOBBox OBB(Position, U, V, N, ZEVector3(0.5f, 0.5f, 0.5f));

		ZEVector3 n1(1.0f, 2.0f, 3.0f);
		ZEVector3 p1(100.0f, 100.0f, 100.0f);
		ZEPlane P1(n1, p1);

		ZEHalfSpace result1 = ZEOBBox::IntersectionTest(OBB, P1);

		ZETestCheckEqual(result1,-1);

		ZEVector3 n2(1.0f, 2.0f, 3.0f);
		ZEVector3 p2(0.0f, -6.0f, 0.0f);
		ZEPlane P2(n2, p2);

		ZEHalfSpace result2 = ZEOBBox::IntersectionTest(OBB, P2);

		ZETestCheckEqual(result2, 1);

		
		ZEVector3 n3(1.0f, 0.0f, 0.0f);
		ZEVector3 p3(0.0f, 0.0f, 0.0f);
		ZEPlane P3(n3, p3);

		ZEHalfSpace result3 = ZEOBBox::IntersectionTest(OBB, P3);

		ZETestCheckEqual(result3, 1);

		ZEVector3 n4(0.0f, 0.0f, 1.0f);
		ZEVector3 p4(-200.0f, -1000.0f, -100.0f);
		ZEPlane P4(n4, p4);


		ZEHalfSpace result4 = ZEOBBox::IntersectionTest(OBB, P4);

		ZETestCheckEqual(result4, 1);
	}

	ZETest("OBB_IntersectionTest1")
	{
		ZEVector3 Position(1.0f, 2.0f, 3.0f);
		ZEVector3 U(1.0f, 0.0f, 0.0f);
		ZEVector3 V(0.0f, 1.0f, 0.0f);
		ZEVector3 N(0.0f, 0.0f, 1.0f);

		ZEOBBox OBB(Position, U, V, N, ZEVector3(0.5f, 0.5f, 0.5f));

		
		bool result = ZEOBBox::IntersectionTest(OBB, ZEVector3(1.0f, 2.0f, 3.0f));

		ZETestCheckEqual(result, true);

		ZEVector3 P(-1.0f, -2.0f, -3.0f);

		bool result2 = ZEOBBox::IntersectionTest(OBB, P);

		ZETestCheckEqual(result2, false);

		bool result3 = ZEOBBox::IntersectionTest(OBB, ZEVector3(1.5f, 2.5f, 3.5f));

		ZETestCheckEqual(result3, true);

	}

	ZETest("OBB_IntersectionTest2")
	{
	
	}

	ZETest("OBB_IntersectionTest3")
	{
	
	}

	ZETest("OBB_IntersectionTest4")
	{

	}
}
