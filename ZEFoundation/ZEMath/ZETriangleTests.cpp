//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETriangleTests.cpp
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

#include <math.h>
#include <UnitTestCpp/UnitTest++.h>
#include "ZEMath/ZETriangle.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZELineSegment.h"
#include "ZEMath/ZERay.h"
#include "ZEMath/ZEMathDefinitions.h"
#include "ZEMathIOStreamMapping.h"


SUITE(ZETriangle)
{

	TEST(TRI_Constructor)
	{
		ZEVector3 V0(1.0f, 2.0f, 3.0f);
		ZEVector3 V1(0.0f, 0.0f, 3.0f);
		ZEVector3 V2(4.0f, 5.0f, 6.0f);

		ZETriangle T(V0, V1, V2);

		CHECK_EQUAL(T.V0, ZEVector3(1.0f, 2.0f, 3.0f));
		CHECK_EQUAL(T.V1, ZEVector3(0.0f, 0.0f, 3.0f));
		CHECK_EQUAL(T.V2, ZEVector3(4.0f, 5.0f, 6.0f));
	}

	TEST(TRI_Create)
	{
		ZEVector3 V0(1.0f, 2.0f, 3.0f);
		ZEVector3 V1(0.0f, 0.0f, 3.0f);
		ZEVector3 V2(4.0f, 5.0f, 6.0f);
		
		ZETriangle T;
		ZETriangle::Create(T, V0, V1, V2);

		CHECK_EQUAL(T.V0, ZEVector3(1.0f, 2.0f, 3.0f));
		CHECK_EQUAL(T.V1, ZEVector3(0.0f, 0.0f, 3.0f));
		CHECK_EQUAL(T.V2, ZEVector3(4.0f, 5.0f, 6.0f));
	}
	TEST(TRI_GetNormal)
	{
		ZEVector3 N;

		ZEVector3 V0(1.0f, 2.0f, 3.0f);
		ZEVector3 V1(0.0f, 0.0f, 3.0f);
		ZEVector3 V2(4.0f, 5.0f, 6.0f);

		ZETriangle T(V0, V1, V2);

		ZETriangle::GetNormal(T, N);

		CHECK_EQUAL(N, ZEVector3((-6.0f/sqrt(54.0f)), (3.0f/sqrt(54.0f)), (3.0f/sqrt(54.0f))));
	}

	TEST(TRI_GetSurfacePlane)
	{
		ZEVector3 V0(1.0f, 2.0f, 3.0f);
		ZEVector3 V1(0.0f, 0.0f, 3.0f);
		ZEVector3 V2(4.0f, 5.0f, 6.0f);

		ZEVector3 N;

		
		ZEPlane P;
		ZETriangle T(V0, V1, V2);
		ZETriangle::GetSurfacePlane(T, P);

		ZETriangle::GetNormal(T, N);

		CHECK_EQUAL(P.n, N);
		CHECK_EQUAL(P.p, T.V1);
	}

	TEST(TRI_InsideTest)
	{
		ZEVector3 P(1.0f, 2.0f, 3.0f);

		ZEVector3 V0(1.0f, 2.0f, 3.0f);
		ZEVector3 V1(0.0f, 0.0f, 3.0f);
		ZEVector3 V2(4.0f, 5.0f, 6.0f);

		ZETriangle T(V0, V1, V2);

		bool result = ZETriangle::InsideTest(T, P);

		CHECK_EQUAL(result, true);
	}

	TEST(TRI_IntersectionTest1)
	{
		ZEVector3 V0(1.0f, 2.0f, 3.0f);
		ZEVector3 V1(0.0f, 0.0f, 3.0f);
		ZEVector3 V2(4.0f, 5.0f, 6.0f);
		ZEVector3 N;

		ZEVector3::CrossProduct(N, V0, V1);

		ZETriangle T(V0, V1, V2);
		ZELine L;
		L.p = V0;
		L.v = N;

		float t;

		bool result = ZETriangle::IntersectionTest(T, L, t);
		
		CHECK_EQUAL(result, true);
	}

	TEST(TRI_IntersectionTest2)
	{
		ZEVector3 V0(1.0f, 2.0f, 3.0f);
		ZEVector3 V1(0.0f, 0.0f, 3.0f);
		ZEVector3 V2(4.0f, 5.0f, 6.0f);

		ZEVector3 N ;
		ZEVector3::CrossProduct(N, V0, V1);

		ZETriangle T(V0, V1, V2);
		ZELineSegment LS;
		LS.p = V0;
		LS.v = N;

		float t;

		bool result = ZETriangle::IntersectionTest(T, LS, t);
		
		CHECK_EQUAL(result, false);
	}

	TEST(TRI_IntersectionTest3)
	{
		ZEVector3 V0(1.0f, 2.0f, 3.0f);
		ZEVector3 V1(0.0f, 0.0f, 3.0f);
		ZEVector3 V2(4.0f, 5.0f, 6.0f);

		ZEVector3 N ;
		ZEVector3::CrossProduct(N, V0, V1);

		ZETriangle T(V0, V1, V2);
		ZERay R;
		R.p = ZEVector3(1.0f, 2.0f, 3.0f);
		R.v = N;

		float t;

		bool result = ZETriangle::IntersectionTest(T, R, t);
		
		CHECK_EQUAL(result, true);
	}

	TEST(TRI_IntersectionTest4)
	{
		ZEVector3 V0(1.0f, 2.0f, 3.0f);
		ZEVector3 V1(0.0f, 0.0f, 3.0f);
		ZEVector3 V2(4.0f, 5.0f, 6.0f);

		ZEVector3 N ;
		ZEVector3::CrossProduct(N, V0, V1);

		ZETriangle T(V0, V1, V2);
		ZEPlane P;
		P.n = V0;
		P.p = N;

		ZELine L;
		L.p = V0;
		L.v = N;

		bool result = ZETriangle::IntersectionTest(T, P, L);
		
		CHECK_EQUAL(result, false);
	}


}
