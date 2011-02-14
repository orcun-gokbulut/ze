//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELineTest.cpp
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

#include <UnitTest/UnitTest++.h>
#include <d3dx9.h>
#include <math.h>
#include "ZEIOStreamMapping.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZELine.h"
#include "ZEMath/ZEDefinitions.h"

SUITE(ZELine)
{
	TEST(LN_Constructor)
	{
		ZEVector3 P0(0.0f, 1.0f, 0.0f);
		ZEVector3 P1(1.0f, 2.0f, 3.0f);
		ZELine L(P0, P1);
		CHECK_EQUAL(L.p, P0);
		CHECK_EQUAL(L.v, ZEVector3(1.0f, 1.0f, 3.0f));
	}

	TEST(LN_Create)
	{
		ZELine L;
		ZEVector3 P0(0.0f, 1.0f, 0.0f);
		ZEVector3 P1(1.0f, 2.0f, 3.0f);
		ZELine::Create(L, P0, P1);
		CHECK_EQUAL(L.p, P0);
		CHECK_EQUAL(L.v, ZEVector3(1.0f, 1.0f, 3.0f));
	}

	TEST(LN_CreateParamatric)
	{
		ZELine L;
		ZEVector3 V(1.0, 4.0f, 9.0f);
		ZEVector3 P (1.0, 0.0f, 0.0f);
		ZELine::CreateParametric(L, V, P);
		CHECK_EQUAL(L.p, P);
		CHECK_EQUAL(L.v, V);
	}

	TEST(LN_MinimumDistance1)
	{
		ZEVector3 P1(0,3,0);
		ZEVector3 P2(1,4,-1);
		ZEVector3 P3(5,8,2);
		ZEVector3 P4(8,15,1);
		
		ZELine L1(P1,P2);
		ZELine L2(P3,P4);
		
		float d = ZELine::MinimumDistance(L1, L2);

		CHECK_EQUAL(d, sqrtf(14.0f));
	}

	TEST(LN_MinimumDistance2)
	{
		ZEVector3 P1(0,3,0);
		ZEVector3 P2(1,4,-1);
		ZEVector3 P3(5,8,2);
		ZEVector3 P4(8,15,1);
		
		ZELine L1(P1,P2);
		ZELine L2(P3,P4);
		
		float tA, tB;
		float d = ZELine::MinimumDistance(L1, L2, tA, tB);

		CHECK_EQUAL(d, sqrtf(14.0f));
	}

	TEST(LN_DistanceToPoint)
	{
		ZEVector3 P(5,8,2);
		ZEVector3 P1(0,3,0);
		ZEVector3 P2(1,4,-1);
		ZELine L(P1,P2);
		float t;
		float d;

		d = ZELine::MinimumDistance(L,P,t);

		CHECK_CLOSE(d, 4.61880215, 0.000001);
	}

	TEST(LN_GetPointOn)
	{
		ZEVector3 P1(0,3,0);
		ZEVector3 P2(1,4,-1);
		ZEVector3 A;
		ZEVector3 B;
		const ZELine L=ZELine::ZELine(P1,P2);
		ZEVector3 P;
		ZEVector3 P3;
		L.GetPointOn(P,3);
		A=P;
		
		ZEVector3::Scale(P,L.v,3);
		ZEVector3::Add(P,P,L.p);
		B = P;

		CHECK_EQUAL(A, B);
	}
}
