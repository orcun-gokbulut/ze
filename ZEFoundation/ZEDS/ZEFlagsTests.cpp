//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFlagsTests.cpp
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

#include "ZEDS/ZEFlags.h"
#include "ZETest.h"

ZETestSuite(ZEFlagS)
{
	ZETest("RaisebitUnraisebitSetflagSetbit")
	
	{	
		int a = 0;
		int b = 16;
		ZEFlags TestItem1;
		ZEFlags TestItem2;
		ZEFlags TestItem3;
		ZEFlags TestItem4;
		TestItem1.SetFlags(a, true);
		TestItem4.SetFlags(b, false);
		TestItem1.RaiseBit(4);
		TestItem4.UnraiseBit(4);
		ZETestCheck(TestItem1 == 16);
		ZETestCheck(TestItem4 == 0);
		TestItem1.UnraiseBit(4);
		ZETestCheck(TestItem1 == 0);
		//setbit from empty one raise
		TestItem2.SetBit(4, true);
		ZETestCheck(TestItem2 == 16);
		//setbit from empty one unraise
		TestItem3.SetBit(4, false);
		ZETestCheck(TestItem3 == 0);
		ZETestCheckEqual(TestItem1.GetBit(4), 0);
		ZETestCheckEqual(TestItem4.GetBit(4), 0);
		ZETestCheckEqual(TestItem1.GetFlags(a), 1);
		ZETestCheckEqual(TestItem4.GetFlags(b), 0);
	}

	ZETest("operators")
	{
		ZEFlags TestItem1;
		ZEFlags TestItem2;
		ZEFlags TestItem3;
		ZEFlags TestItem4;
		ZEFlags TestItem5;
		ZEFlags TestItem6;
		ZEFlags TestItem7;
		ZEFlags TestItem8;
		ZEFlags TestItem9 = 5;
		ZEFlags TestItem10 = 3;
		TestItem9 = TestItem10;
		ZETestCheckEqual(TestItem9, TestItem10);
		TestItem1.SetBit(4, true);
		TestItem2.SetBit(4, true);
		TestItem3.SetBit(4, false);
		TestItem6.SetBit(4, true);
		TestItem7.SetBit(4, false);
		TestItem8.SetBit(0, false);
		//[]
		ZETestCheckEqual(TestItem1[4], 1);
		//& 
		TestItem4 = TestItem1 & TestItem2;
		ZETestCheck(TestItem4 == 16);
		//&=
		TestItem1 &= TestItem2;
		ZETestCheck(TestItem2 == 16);
		//|
		TestItem5 = TestItem1 | TestItem3;
		ZETestCheck(TestItem5 == 16);
		//|=
		TestItem1 |= TestItem3;
		ZETestCheck(TestItem1 == 16);
		//^
		TestItem1 = TestItem6 ^ TestItem7;
		ZETestCheck(TestItem1 == 16);
		//^=
		TestItem6 ^= TestItem7;
		ZETestCheck(TestItem6 == 16);
		//~
		unsigned int z =  ~TestItem8;
		ZETestCheckEqual(z, 4294967295);
		

	}
}
