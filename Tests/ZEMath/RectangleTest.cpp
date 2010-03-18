//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - RectangleTest.cpp
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
#include "ZEMath/Rectangle.h"
#include "ZEMath/Vector.h"
#include <math.h>

SUITE(Rectangle)
{

	TEST(RECT_Constructor)
	{
		ZEPoint2 LU(1.0f, 4.0f);
		ZEPoint2 RD(3.0f, 2.0f);
		ZERectangle R(LU, RD);
		CHECK_EQUAL(R.LeftUp, ZEPoint2(1.0f, 4.0f));
		CHECK_EQUAL(R.RightDown, ZEPoint2(3.0f, 2.0f));

		ZERectangle R2(LU, 2.0f, 2.0f);
		CHECK_EQUAL(R2.LeftUp, ZEPoint2(1.0f, 4.0f));
		CHECK_EQUAL(R2.RightDown, ZEPoint2(3.0f, 2.0f));



	}

	TEST(RECT_BoundingTest)
	{
		

		ZEPoint2 LU(1.0f, 4.0f);
		ZEPoint2 RD(3.0f, 2.0f);
		ZERectangle R(LU, RD);

		ZEPoint2 P(2.0f, 3.0f);
		bool result = R.BoundingTest(P);
		CHECK_EQUAL(result, true);

		ZEPoint2 P2(0.0f, 0.0f);
		bool result2 = R.BoundingTest(P2);
		CHECK_EQUAL(result2, false);

		ZEPoint2 P3(2.0f, 2.0f);
		bool result3 = R.BoundingTest(P3);
		CHECK_EQUAL(result3, true);
	}

	TEST(RECT_GetCorner)
	{
		ZEPoint2 LU(1.0f, 4.0f);
		ZEPoint2 RD(3.0f, 2.0f);
		ZERectangle R(LU, RD);

		ZEPoint2 C1 = R.GetCorner(ZERECTANGLECORNER_LEFTDOWN);
		ZEPoint2 C2 = R.GetCorner(ZERECTANGLECORNER_LEFTUP);
		ZEPoint2 C3 = R.GetCorner(ZERECTANGLECORNER_RIGHTDOWN);
		ZEPoint2 C4 = R.GetCorner(ZERECTANGLECORNER_RIGHTUP);

		CHECK_EQUAL(C1, ZEPoint2(1.0f, 2.0f));
		CHECK_EQUAL(C2, ZEPoint2(1.0f, 4.0f));
		CHECK_EQUAL(C3, ZEPoint2(3.0f, 2.0f));
		CHECK_EQUAL(C4, ZEPoint2(3.0f, 4.0f));

	}

	TEST(RECT_GetWidth)
	{
		ZEPoint2 LU(1.0f, 4.0f);
		ZEPoint2 RD(3.0f, 2.0f);
		ZERectangle R(LU, RD);

		float w = R.GetWidth();

		CHECK_EQUAL(w, 2.0f);

	}

	TEST(RECT_GetHeight)
	{
		ZEPoint2 LU(1.0f, 4.0f);
		ZEPoint2 RD(3.0f, 2.0f);
		ZERectangle R(LU, RD);

		float h = R.GetHeight();

		CHECK_EQUAL(h, 2.0f);

	}



}
