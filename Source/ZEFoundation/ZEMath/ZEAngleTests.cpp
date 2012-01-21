//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEAngleTests.cpp
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

/*#include "ZETest.h"
#include "ZEMathIOStreamMapping.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEMath.h"

static bool fequal(float a, float b)
{
	return ZEMath::Abs(a-b) < 0.00001;
}

ZETestSuite(ZEAngle)
{
	ZETest("ToDegreeRangeDistanceToRadian")
	{
		float TestItem1 = ZEAngle::ToDegree(ZE_PI_2);
		ZETestCheck(fequal(TestItem1, 90.0f));
		float TestItem2 = ZEAngle::Range(ZE_PI_2);
		ZETestCheck(fequal(TestItem2, ZE_PI_2));
		float TestItem3 = ZEAngle::Range(ZE_PI_2+ZE_PI);//270 degree
		ZETestCheck(fequal(TestItem3, -ZE_PI_2));
		float TestItem4 = ZEAngle::Range(-ZE_PI_2);
		ZETestCheck(fequal(TestItem4, -ZE_PI_2));
		float TestItem5= ZEAngle::Distance(ZE_PI_2, ZE_PI_4);
		ZETestCheck(fequal(TestItem5, ZE_PI_4));
		float TestItem6 = ZEAngle::Distance(ZE_PI_4, ZE_PI_2);
		ZETestCheck(fequal(TestItem6, ZE_PI_4));
		float TestItem7 = ZEAngle::ToRadian(ZE_PI_2);
		ZETestCheck(fequal(TestItem7, ZE_PI_2));
		

	}
	ZETest("trigonometry")
	{
		float TestItem1 = ZEAngle::Cos(ZE_PI_4);
		ZETestCheck(fequal(TestItem1,  0.7071067818f));
		float TestItem2 = ZEAngle::Cos(ZE_PI);
		ZETestCheck(fequal(TestItem2,  -1.0f));
		float TestItem3 = ZEAngle::Sin(ZE_PI_4);
		ZETestCheck(fequal(TestItem3, 0.70710678118f));//
		float TestItem4 = ZEAngle::Sin(ZE_PI_2);
		ZETestCheck(fequal(TestItem4, 1.0f));//
		float TestItem5 = ZEAngle::Tan(ZE_PI_4);
		ZETestCheck(fequal(TestItem5, 1.0f));//
		float TestItem6 = ZEAngle::Tan(ZE_PI_12);
		ZETestCheck(fequal(TestItem6, 0.2679491924f));//
		float TestItem7 = ZEAngle::Cot(ZE_PI_2);
		ZETestCheck(fequal(TestItem7, ));//
		float TestItem8 = ZEAngle::Cot((ZE_PI));
		ZETestCheck(fequal(TestItem8, ));//
		float TestItem9 = ZEAngle::ArcSin(ZE_PI_4);
		ZETestCheck(fequal(TestItem9, ));//
		float TestItem10 = ZEAngle::ArcCos(ZE_PI_4);
		ZETestCheck(fequal(TestItem10, ));//
		float TestItem11 = ZEAngle::ArcTan(ZE_PI_2);
		ZETestCheck(fequal(TestItem11, ));//
		float TestItem12 = ZEAngle::ArcTan2(ZE_PI_2);
		ZETestCheck(fequal(TestItem12, ));//

	}
	ZETest("ToVector")
	{
		ZEVector2 TestItem1 = ZEAngle::ToVector(ZE_PI_4);
		ZETestCheck(TestItem1, );
	}
	
}*/
