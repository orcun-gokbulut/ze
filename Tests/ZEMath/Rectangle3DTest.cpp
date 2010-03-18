//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Rectangle3DTest.cpp
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
#include "ZEMath/Rectangle3D.h"
#include "ZEMath/Vector.h"
#include "ZEmath/Plane.h"
#include <math.h>


SUITE(Rectangle3D)
{
	TEST(RECT3D_Constructor)
	{
		ZEPoint3 A(1.0f, 2.0f, 3.0f);
		ZEPoint3 B(4.0f, 5.0f, 6.0f);
		ZEPoint3 C(7.0f, 8.0f, 9.0f);
		ZEPoint3 D(10.0f, 11.0f, 12.0f);
		
		ZERectangle3D R(A, B, C, D);

		CHECK_EQUAL(R.P1, A);
		CHECK_EQUAL(R.P2, B);
		CHECK_EQUAL(R.P3, C);
		CHECK_EQUAL(R.P4, D);
	}

	TEST(RECT3D_GetPlane)
	{
		ZEPoint3 A(1.0f, 4.0f, 6.0f);
		ZEPoint3 B(3.0f, 4.0f, 6.0f);
		ZEPoint3 C(1.0f, 2.0f, 6.0f);
		ZEPoint3 D(3.0f, 2.0f, 6.0f);
		
		ZERectangle3D R(A, B, C, D);

		ZEPlane P ;
		R.GetPlane(P);

		CHECK_EQUAL(P.n , ZEVector3(0.0f, 0.0f, -4.0f));
		CHECK_EQUAL(P.p , ZEPoint3(1.0f, 2.0f, 6.0f));
	}

	TEST(RECT3D_GetPoint)
	{
		ZEPoint3 A(1.0f, 4.0f, 6.0f);
		ZEPoint3 B(3.0f, 4.0f, 6.0f);
		ZEPoint3 C(1.0f, 2.0f, 6.0f);
		ZEPoint3 D(3.0f, 2.0f, 6.0f);
		
		ZERectangle3D R(A, B, C, D);

		ZEPoint3 P1 = R.GetPoint(0);
		ZEPoint3 P2 = R.GetPoint(1);
		ZEPoint3 P3 = R.GetPoint(2);
		ZEPoint3 P4 = R.GetPoint(3);

		CHECK_EQUAL(A, P1);
		CHECK_EQUAL(B, P2);
		CHECK_EQUAL(C, P3);
		CHECK_EQUAL(D, P4);
	}
}