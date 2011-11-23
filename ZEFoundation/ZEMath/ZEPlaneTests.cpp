//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPlaneTests.cpp
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
#include <d3dx9.h>
#include <math.h>
#include "ZEMathIOStreamMapping.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEPlane.h"
#include "ZEMath/ZELine.h"
#include "ZEMath/ZERay.h"
#include "ZEMath/ZELineSegment.h"
#include "ZEMath/ZEMathDefinitions.h"

ZETestSuite(ZEPlane)
{
	ZETest(Constructor)
	{
		ZEVector3 n(1.0f, 2.0f, 3.0f);
		ZEVector3 p(0.0f, 6.0f, 0.0f);
		ZEPlane P(n, p);

		ZETestCheckEqual(P.n, n);
		ZETestCheckEqual(P.p, p);
	}

	ZETest(Create1)
	{
		ZEVector3 n(1.0f, 2.0f, 3.0f);
		ZEVector3 p(0.0f, 6.0f, 0.0f);
		ZEPlane P;
		ZEPlane::Create(P, n, p);
		
		ZETestCheckEqual(P.n, n);
		ZETestCheckEqual(P.p, p);

	}

	ZETest(Create2)
	{
		ZEVector3 P1(1.0f, 2.0f, 3.0f);
		ZEVector3 P2(0.0f, 3.0f, 1.0f);
		ZEVector3 P3(2.0f, 5.0f, 8.0f);
		ZEPlane P;

		ZEPlane::Create(P, P1, P2, P3);

		ZETestCheckEqual(P.p, P3);
		ZETestCheckEqual(P.n, ZEVector3(11.0f, 3.0f, -4.0f));
	}

	ZETest(Distance)
	{
		ZEVector3 p(-4.0f, 0.0f, 0.0f);
		ZEVector3 n(2.0f, -2.0f, 5.0f);
		ZEVector3 X(4.0f, -4.0f, 3.0f);
		ZEPlane P;
		ZEPlane::Create(P, n, p);
		float d = ZEPlane::Distance(P, X);
		ZETestCheckClose(d, 39.0f/sqrt(33.0f), 0.000001);
	}

	ZETest(TestHalfSpace)
	{
		ZEVector3 p(-4.0f, 0.0f, 0.0f);
		ZEVector3 n(2.0f, -2.0f, 5.0f);
		ZEPlane P(n, p);
		
		ZEVector3 X( 4.0f, -4.0f, 3.0f);
		ZEVector3 Y(-5.0f,  0.0f, 0.0f);
		ZEVector3 Z( 0.0f,  4.0f, 0.0f);
		ZEHalfSpace a = ZEPlane::TestHalfSpace(P, X);
		ZETestCheckEqual(a, 1);
		ZETestCheckEqual(ZEPlane::TestHalfSpace(P, Y), -1);
		ZETestCheckEqual(ZEPlane::TestHalfSpace(P, Z), 0);
	}

	ZETest(IntersectionTest1)
	{
		ZEVector3 P0( 1.0f, 2.0f, 3.0f);
		ZEVector3 P1(-4.0f, 0.0f, 0.0f);

		ZEVector3  p(0.0f,  4.0f, 0.0f);
		ZEVector3 n(2.0f, -2.0f, 5.0f);

		ZELine L1(P0, P1);
		ZEPlane P(n, p);
		
		float t;
		bool result1 = ZEPlane::IntersectionTest(P, L1, t);

		ZETestCheckEqual(result1, true);
		
		ZEVector3 P2( 1.0f,  2.0f,  3.0f);
		ZEVector3 P3( 5.0f,  6.0f , 3.0f);

		ZELine L2(P2, P3);

		bool result2 = ZEPlane::IntersectionTest(P, L2, t);
		
		ZETestCheckEqual(result2, false);

	}

	ZETest(IntersectionTest2)
	{
		ZEVector3 P0( 1.0f, 2.0f, 3.0f);
		ZEVector3 P1(-4.0f, 0.0f, 0.0f);

		ZEVector3  p(0.0f,  4.0f, 0.0f);
		ZEVector3 n(2.0f, -2.0f, 5.0f);

		ZELineSegment LS1(P0, P1);
		ZEPlane P(n, p);
		
		float t;
		bool result1 = ZEPlane::IntersectionTest(P, LS1, t);

		ZETestCheckEqual(result1, true);
		
		ZEVector3 P2( 1.0f,  2.0f,  3.0f);
		ZEVector3 P3( 5.0f,  6.0f , 3.0f);

		ZELineSegment LS2(P2, P3);

		bool result2 = ZEPlane::IntersectionTest(P, LS2, t);
		
		ZETestCheckEqual(result2, false);
	}
	
	ZETest(IntersectionTest3)
	{
		
		ZEVector3 P0( 1.0f, 2.0f, 3.0f);
		ZEVector3 P1(-4.0f, 0.0f, 0.0f);

		ZEVector3  p(0.0f,  4.0f, 0.0f);
		ZEVector3 n(2.0f, -2.0f, 5.0f);

		ZERay R1(P0, P1);
		ZEPlane P(n, p);
		
		float t;
		bool result1 = ZEPlane::IntersectionTest(P, R1, t);

		ZETestCheckEqual(result1, true);
		
		ZEVector3 P2( 1.0f,  2.0f,  3.0f);
		ZEVector3 P3( 5.0f,  6.0f , 3.0f);

		ZERay R2(P2, P3);

		bool result2 = ZEPlane::IntersectionTest(P, R2, t);
		
		ZETestCheckEqual(result2, false);
	}
	
	ZETest(IntersectionTest4)
	{
		ZEVector3  p(0.0f,  4.0f, 0.0f);
		ZEVector3 n(2.0f, -2.0f, 5.0f);
		ZEPlane P(n, p);

		ZEVector3  p0(5.0f,  2.0f, 1.0f);
		ZEVector3 n0(4.0f, -4.0f, 10.0f);
		ZEPlane P0(n0, p0);


		ZEVector3  p1(4.0f, 6.0f, 7.0f);
		ZEVector3 n1(2.0f, 2.0f, 0.0f);
		ZEPlane P1(n1, p1);

		ZELine L;

		bool result1 = ZEPlane::IntersectionTest(P, P1, L);
		bool result2 = ZEPlane::IntersectionTest(P, P,  L);
		bool result3 = ZEPlane::IntersectionTest(P, P0, L);
		
		ZETestCheckEqual(result1, true);
		ZETestCheckEqual(result2, true);
		ZETestCheckEqual(result3, false);
	}
}
