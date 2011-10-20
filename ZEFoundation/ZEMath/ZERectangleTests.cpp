//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERectangleTests.cpp
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
#include "ZEMath/ZERectangle.h"
#include "ZEMath/ZEVector.h"
#include <math.h>

ZETestSuiteAdd(Rectangle)
{

	ZETestItemAdd(RECT_Constructor)
	{
		ZEVector2 LU(1.0f, 4.0f);
		ZEVector2 RD(3.0f, 2.0f);
		ZERectangle R(LU, RD);
		ZETestCheckEqual(R.LeftUp, ZEVector2(1.0f, 4.0f));
		ZETestCheckEqual(R.RightDown, ZEVector2(3.0f, 2.0f));

		ZERectangle R2(LU, 2.0f, 2.0f);
		ZETestCheckEqual(R2.LeftUp, ZEVector2(1.0f, 4.0f));
		ZETestCheckEqual(R2.RightDown, ZEVector2(3.0f, 2.0f));



	}

	ZETestItemAdd(RECT_BoundingTest)
	{
		/*ZEVector2 LU(1.0f, 4.0f);
		ZEVector2 RD(3.0f, 2.0f);
		ZERectangle R(LU, RD);

		ZEVector2 P(2.0f, 3.0f);
		bool result = R.BoundingTest(P);
		ZETestCheckEqual(result, true);

		ZEVector2 P2(0.0f, 0.0f);
		bool result2 = R.BoundingTest(P2);
		ZETestCheckEqual(result2, false);

		ZEVector2 P3(2.0f, 2.0f);
		bool result3 = R.BoundingTest(P3);
		ZETestCheckEqual(result3, true);*/
	}

	ZETestItemAdd(RECT_GetCorner)
	{
		ZEVector2 LU(1.0f, 4.0f);
		ZEVector2 RD(3.0f, 2.0f);
		ZERectangle R(LU, RD);

		ZEVector2 C1 = R.GetCorner(ZE_RC_LEFTDOWN);
		ZEVector2 C2 = R.GetCorner(ZE_RC_LEFTUP);
		ZEVector2 C3 = R.GetCorner(ZE_RC_RIGHTDOWN);
		ZEVector2 C4 = R.GetCorner(ZE_RC_RIGHTUP);

		ZETestCheckEqual(C1, ZEVector2(1.0f, 2.0f));
		ZETestCheckEqual(C2, ZEVector2(1.0f, 4.0f));
		ZETestCheckEqual(C3, ZEVector2(3.0f, 2.0f));
		ZETestCheckEqual(C4, ZEVector2(3.0f, 4.0f));

	}

	ZETestItemAdd(RECT_GetWidth)
	{
		ZEVector2 LU(1.0f, 4.0f);
		ZEVector2 RD(3.0f, 2.0f);
		ZERectangle R(LU, RD);

		float w = R.GetWidth();

		ZETestCheckEqual(w, 2.0f);

	}

	ZETestItemAdd(RECT_GetHeight)
	{
		ZEVector2 LU(1.0f, 4.0f);
		ZEVector2 RD(3.0f, 2.0f);
		ZERectangle R(LU, RD);

		float h = R.GetHeight();

		ZETestCheckEqual(h, 2.0f);

	}



}
