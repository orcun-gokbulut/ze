//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGUIDTests.cpp
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

#include "ZEFoundation/ZEGUID.h"
#include "ZETest/ZETest.h"
#include "ZETest/ZETestCheck.h"
#include "ZEError.h"
#include "ZERandom.h"

ZE_TEST(ZEGUID)
{
	ZE_TEST_ITEM("ZEGUID::ZEGUID()")
	{
		ZEGUID TestItem;
		ZE_TEST_CHECK_EQUAL(TestItem.Data1, 3435973836);
		ZE_TEST_CHECK_EQUAL(TestItem.Data2, 52428);
		ZE_TEST_CHECK_EQUAL(TestItem.Data3, 52428);
		ZE_TEST_CHECK_EQUAL(TestItem.Data4, 14757395258967641292);
	}

	ZE_TEST_ITEM("ZEGUID::ZEGUID(ZEUInt32 Data1, ZEUInt16 Data2, ZEUInt16 Data3, ZEUInt64 Data4)")
	{
		ZEUInt32 Data1 = ZERandom::GetUInt32();
		ZEUInt16 Data2 = ZERandom::GetUInt16();
		ZEUInt16 Data3 = ZERandom::GetUInt16();
		ZEUInt64 Data4 = ZERandom::GetUInt64();

		ZEGUID TestItem1(Data1, Data2, Data3, Data4);
		ZE_TEST_CHECK_EQUAL(TestItem1.Data1, Data1);
		ZE_TEST_CHECK_ERROR(TestItem1.Data1 >= 0 && TestItem1.Data1 <= 4294967295);
		ZE_TEST_CHECK_EQUAL(TestItem1.Data2, Data2);
		ZE_TEST_CHECK_ERROR(TestItem1.Data2 >= 0 && TestItem1.Data2 <= 65535);
		ZE_TEST_CHECK_EQUAL(TestItem1.Data3, Data3);
		ZE_TEST_CHECK_ERROR(TestItem1.Data3 >= 0 && TestItem1.Data3 <= 65535);
		ZE_TEST_CHECK_EQUAL(TestItem1.Data4, Data4);
		ZE_TEST_CHECK_ERROR(TestItem1.Data4 >= 0 && TestItem1.Data4 <= 18446744073709551615);

		Data1 = 0x01234567;
		Data2 = 0x0123;
		Data3 = 0x2310;
		Data4 = 0x0123456789ABCDEF;

		ZEGUID TestItem2(Data1, Data2, Data3, Data4);
		ZE_TEST_CHECK_EQUAL(TestItem2.Data1, Data1);
		ZE_TEST_CHECK_EQUAL(TestItem2.Data2, Data2);
		ZE_TEST_CHECK_EQUAL(TestItem2.Data3, Data3);
		ZE_TEST_CHECK_EQUAL(TestItem2.Data4, Data4);
	}

	ZE_TEST_ITEM("ZEGUID::ZEGUID(ZEUInt32 Data1, ZEUInt16 Data2, ZEUInt16 Data3, ZEUInt16 Data4FirstTwo, ZEUInt64 Data4Remaining)")
	{
		ZEUInt32 Data1 = 0x01234567;
		ZEUInt16 Data2 = 0x0123;
		ZEUInt16 Data3 = 0x2310;
		ZEUInt16 Data4FirstTwo = 0x0123;
		ZEUInt64 Data4Remaining = 0x0123456789ABCDEF;

		ZEGUID TestItem1(Data1, Data2, Data3, Data4FirstTwo, Data4Remaining);
		ZE_TEST_CHECK_EQUAL(TestItem1.Data1, Data1);
		ZE_TEST_CHECK_EQUAL(Data1, 19088743);
		ZE_TEST_CHECK_EQUAL(TestItem1.Data2, Data2);
		ZE_TEST_CHECK_EQUAL(Data2, 291);
		ZE_TEST_CHECK_EQUAL(TestItem1.Data3, Data3);
		ZE_TEST_CHECK_EQUAL(Data3, 8976);
		ZE_TEST_CHECK_EQUAL(Data4FirstTwo, 291);
		ZE_TEST_CHECK_EQUAL(Data4Remaining, 81985529216486895);
		ZE_TEST_CHECK_EQUAL(TestItem1.Data4, 9927935178558959);

		Data1 = 0x604857D3;
		Data2 = 0x01D3;
		Data3 = 0x03A2;
		Data4FirstTwo = 0x0456;
		Data4Remaining = 0xEFCDAB8967452301;

		ZEGUID TestItem2(Data1, Data2, Data3, Data4FirstTwo, Data4Remaining);
		ZE_TEST_CHECK_EQUAL(TestItem2.Data1, Data1);
		ZE_TEST_CHECK_EQUAL(Data1, 1615353811);
		ZE_TEST_CHECK_EQUAL(TestItem2.Data2, Data2);
		ZE_TEST_CHECK_EQUAL(Data2, 467);
		ZE_TEST_CHECK_EQUAL(TestItem2.Data3, Data3);
		ZE_TEST_CHECK_EQUAL(Data3, 930);
		ZE_TEST_CHECK_EQUAL(Data4FirstTwo, 1110);
		ZE_TEST_CHECK_EQUAL(Data4Remaining, 17279655951921914625);
		ZE_TEST_CHECK_EQUAL(TestItem2.Data4, 57890976858317569);
	}

	ZE_TEST_ITEM("ZEGUID ZEGUID::Generate()")
	{
		ZEGUID TestItem = ZEGUID::Generate();

		ZE_TEST_CHECK_ERROR(TestItem.Data1 >= 0 && TestItem.Data1 <= 4294967295);
		//ZE_TEST_CHECK_EQUAL(TestItem.Data1, 1767006096);
		ZE_TEST_CHECK_ERROR(TestItem.Data2 >= 0 && TestItem.Data2 <= 65535);
		//ZE_TEST_CHECK_EQUAL(TestItem.Data2, 5705);
		ZE_TEST_CHECK_ERROR(TestItem.Data3 >= 0 && TestItem.Data3 <= 65535);
		//ZE_TEST_CHECK_EQUAL(TestItem.Data3, 28145);
		ZE_TEST_CHECK_ERROR(TestItem.Data4 >= 0 && TestItem.Data4 <= 18446744073709551615);
		//ZE_TEST_CHECK_EQUAL(TestItem.Data4, 6553091204868276715);
	}

	ZE_TEST_ITEM("ZEInt ZEGUID::Compare(const ZEGUID& Other) const")
	{
		ZEUInt32 Data1 = 0x01234567;
		ZEUInt16 Data2 = 0x0123;
		ZEUInt16 Data3 = 0x2310;
		ZEUInt64 Data4 = 0x0123456789ABCDEF;

		ZEGUID TestItem1(Data1, Data2, Data3, Data4);
		ZEGUID TestItem2(Data1, Data2, Data3, Data4);

		ZEInt Result = TestItem1.Compare(TestItem2);
		ZE_TEST_CHECK_EQUAL(Result, 0);

		ZE_TEST_CASE("call function with a parameter whose data values are bigger")
		{
			ZEGUID TestItem3(Data1, Data2, Data3, 6553091204868276715);

			Result = TestItem1.Compare(TestItem3);
			ZE_TEST_CHECK_EQUAL(Result, -1);

			ZEGUID TestItem4(Data1, Data2, 28145, 6553091204868276715);

			Result = TestItem1.Compare(TestItem4);
			ZE_TEST_CHECK_EQUAL(Result, -1);

			ZEGUID TestItem5(Data1, 5705, 28145, 6553091204868276715);

			Result = TestItem1.Compare(TestItem5);
			ZE_TEST_CHECK_EQUAL(Result, -1);

			ZEGUID TestItem6(1767006096, 5705, 28145, 6553091204868276715);

			Result = TestItem1.Compare(TestItem6);
			ZE_TEST_CHECK_EQUAL(Result, -1);
		}

		ZE_TEST_CASE("call function with parameter whose data values are smaller")
		{
			ZEGUID TestItem3(Data1, Data2, Data3, 0x0023456789ABCDEF);

			Result = TestItem1.Compare(TestItem3);
			ZE_TEST_CHECK_EQUAL(Result, 1);

			ZEGUID TestItem4(Data1, Data2, 0x1310, 0x0023456789ABCDEF);

			Result = TestItem1.Compare(TestItem4);
			ZE_TEST_CHECK_EQUAL(Result, 1);

			ZEGUID TestItem5(Data1, 0x0023, Data3, Data4);

			Result = TestItem1.Compare(TestItem5);
			ZE_TEST_CHECK_EQUAL(Result, 1);

			ZEGUID TestItem6(0x00234567, Data2, Data3, Data4);

			Result = TestItem1.Compare(TestItem6);
			ZE_TEST_CHECK_EQUAL(Result, 1);
		}
	}

	ZE_TEST_ITEM("bool ZEGUID::Equals(const ZEGUID& Other) const")
	{
		ZEGUID TestItem1(0x01234567, 0x0123, 0x2301, 0x0123456789ABCDEF);
		ZEGUID TestItem2(0x01234567, 0x0123, 0x2301, 0x0123456789ABCDEF);

		bool Equal = TestItem1.Equals(TestItem2);
		ZE_TEST_CHECK_ERROR(Equal);

		ZE_TEST_CASE("different Data1 values")
		{
			ZEGUID TestItem3(0x00234567, 0x0123, 0x2301, 0x0123456789ABCDEF);

			Equal = TestItem1.Equals(TestItem3);
			ZE_TEST_CHECK_ERROR(!Equal);
		}

		ZE_TEST_CASE("different Data2 values")
		{
			ZEGUID TestItem3(0x01234567, 0x001, 0x2301, 0x0123456789ABCDEF);

			Equal = TestItem1.Equals(TestItem3);
			ZE_TEST_CHECK_ERROR(!Equal);
		}

		ZE_TEST_CASE("different Data3 values")
		{
			ZEGUID TestItem3(0x01234567, 0x0123, 0x0123, 0x0123456789ABCDEF);

			Equal = TestItem1.Equals(TestItem3);
			ZE_TEST_CHECK_ERROR(!Equal);
		}

		ZE_TEST_CASE("different Data4 values")
		{
			ZEGUID TestItem3(0x01234567, 0x0123, 0x2301, 0x0023456789ABCDEF);

			Equal = TestItem1.Equals(TestItem3);
			ZE_TEST_CHECK_ERROR(!Equal);
		}

		ZE_TEST_CASE("different values")
		{
			ZEGUID TestItem3(0x00234567, 0x001, 0x0123, 0x0023456789ABCDEF);

			Equal = TestItem1.Equals(TestItem3);
			ZE_TEST_CHECK_ERROR(!Equal);
		}
	}

	ZE_TEST_ITEM("bool ZEGUID::operator<(const ZEGUID& Other) const")
	{
		ZEGUID TestItem1(0x01234567, 0x0123, 0x2301, 0x0123456789ABCDEF);
		ZEGUID TestItem2(TestItem1);

		bool Result = TestItem1 < TestItem2;
		ZE_TEST_CHECK_ERROR(!Result);

		ZE_TEST_CASE("parameter with bigger values")
		{
			ZEGUID TestItem3(0x11234567, 0x0123, 0x2301, 0x0123456789ABCDEF);

			Result = TestItem1 < TestItem3;
			ZE_TEST_CHECK_ERROR(Result);
		}

		ZE_TEST_CASE("parameter with smaller values")
		{
			ZEGUID TestItem3(0x00234567, 0x0123, 0x2301, 0x0123456789ABCDEF);

			Result = TestItem1 < TestItem3;
			ZE_TEST_CHECK_ERROR(!Result);
		}

		ZE_TEST_CASE("item created by ZEGUID()")
		{
			ZEGUID TestItem3;

			Result = TestItem1 < TestItem3;
			ZE_TEST_CHECK_ERROR(Result);
		}
	}

	ZE_TEST_ITEM("bool ZEGUID::operator>(const ZEGUID& Other) const")
	{
		ZEGUID TestItem1(0x01234567, 0x0123, 0x2301, 0x0123456789ABCDEF);
		ZEGUID TestItem2(TestItem1);

		bool Result = TestItem1 > TestItem2;
		ZE_TEST_CHECK_ERROR(!Result);

		ZE_TEST_CASE("parameter with smaller values")
		{
			ZEGUID TestItem3(0x00234567, 0x0123, 0x2301, 0x0123456789ABCDEF);

			Result = TestItem1 > TestItem3;
			ZE_TEST_CHECK_ERROR(Result);
		}

		ZE_TEST_CASE("parameter with bigger values")
		{
			ZEGUID TestItem3(0x11234567, 0x0123, 0x2301, 0x0123456789ABCDEF);

			Result = TestItem1 > TestItem3;
			ZE_TEST_CHECK_ERROR(!Result);
		}

		ZE_TEST_CASE("item created by ZEGUID()")
		{
			ZEGUID TestItem3;

			Result = TestItem1 > TestItem3;
			ZE_TEST_CHECK_ERROR(!Result);
		}
	}

	ZE_TEST_ITEM("bool ZEGUID::operator<=(const ZEGUID& Other) const")
	{
 		ZEGUID TestItem1(0x01234567, 0x0123, 0x2301, 0x0123456789ABCDEF);
		ZEGUID TestItem2(TestItem1);

		bool Result = TestItem1 <= TestItem2;
		ZE_TEST_CHECK_ERROR(Result);

		ZE_TEST_CASE("parameter with bigger value")
		{
			ZEGUID TestItem3(0x01234567, 0x2301, 0x2301, 0x0123456789ABCDEF);

			Result = TestItem1 <= TestItem3;
			ZE_TEST_CHECK_ERROR(Result);
		}

		ZE_TEST_CASE("parameter with smaller value")
		{
			ZEGUID TestItem3(0x01234567, 0x0123, 0x2301, 0x0103456789ABCDEF);

			Result = TestItem1 <= TestItem3;
			ZE_TEST_CHECK_ERROR(!Result);
		}
	}

	ZE_TEST_ITEM("bool ZEGUID::operator>=(const ZEGUID& Other) const")
	{
		ZEGUID TestItem1(0x67452301, 0x4523, 0x2301, 0xEFCDAB8967452301);
		ZEGUID TestItem2(TestItem1);

		bool Result = TestItem1 >= TestItem2;
		ZE_TEST_CHECK_ERROR(Result);

		ZE_TEST_CASE("parameter with bigger value")
		{
			ZEGUID TestItem3(0x67452301, 0x4523, 0x3301, 0xEFCDAB8967452301);

			Result = TestItem1 >= TestItem3;
			ZE_TEST_CHECK_ERROR(!Result);
		}

		ZE_TEST_CASE("parameter with smaller value")
		{
			ZEGUID TestItem3(0x01234567, 0x4523, 0x2301, 0xEFCDAB8967452301);

			Result = TestItem1 >= TestItem3;
			ZE_TEST_CHECK_ERROR(Result);
		}
	}

	ZE_TEST_ITEM("bool ZEGUID::operator==(const ZEGUID& Other) const")
	{
		ZEGUID TestItem1;
		ZEGUID TestItem2;

		bool Result = TestItem1 == TestItem2;
		ZE_TEST_CHECK_ERROR(Result);

		ZE_TEST_CASE("create with ZEGUID::ZEGUID(ZEUInt32 Data1, ZEUInt16 Data2, ZEUInt16 Data3, ZEUInt64 Data4)")
		{
			ZEGUID TestItem3(0, 0, 0, 0);

			Result = TestItem1 == TestItem3;
			ZE_TEST_CHECK_ERROR(!Result);
		}

		ZE_TEST_CASE("for true")
		{
			ZEGUID TestItem3(0x01234567, 0x0123, 0x2301, 0x0123456789ABCDEF);
			ZEGUID TestItem4(19088743, 291, 8961, 81985529216486895);

			Result = TestItem3 == TestItem4;
			ZE_TEST_CHECK_ERROR(Result);
		}
	}

	ZE_TEST_ITEM("bool ZEGUID::operator!=(const ZEGUID& Other) const")
	{
		ZEGUID TestItem1(0x01234567, 0x0123, 0x2301, 0x0123456789ABCDEF);
		ZEGUID TestItem2(19088743, 291, 8961, 81985529216486895);

		bool Result = TestItem1 != TestItem2;
		ZE_TEST_CHECK_ERROR(!Result);

		ZE_TEST_CASE("for different ZEGUIDs")
		{
			ZEGUID TestItem3(0x67452301, 0x0123, 0x2301, 0x0123456789ABCDEF);

			Result = TestItem1 != TestItem3;
			ZE_TEST_CHECK_ERROR(Result);
		}
	}

	ZE_TEST_ITEM("ZEString ZEGUID::ToString()")
	{
		ZEGUID Item;
		ZE_TEST_CHECK_EQUAL(Item.Data1, 3435973836);
		ZE_TEST_CHECK_EQUAL(Item.Data2, 52428);
		ZE_TEST_CHECK_EQUAL(Item.Data3, 52428);
		ZE_TEST_CHECK_EQUAL(Item.Data4, 14757395258967641292);

		ZEString ItemString = Item.ToString();
		//ZE_TEST_CHECK_STRING_EQUAL(ItemString, "CCCCCCCC-CCCC-CCCC-CCCCCCCCCCCC");

		ZE_TEST_CASE("values in decimal form")
		{
			ZEGUID TestItem(19088743, 291, 8961, 81985529216486895);

			ItemString = TestItem.ToString();
			ZE_TEST_CHECK_STRING_EQUAL(ItemString, "01234567-0123-2301-0123-456789ABCDEF");
		}

		ZE_TEST_CASE("values in hexadecimal form")
		{
			ZEGUID TestItem(0x01234567, 0x0123, 0x2301, 0x0123456789ABCDEF);

			ItemString = TestItem.ToString();
			ZE_TEST_CHECK_STRING_EQUAL(ItemString, "01234567-0123-2301-0123-456789ABCDEF");
		}

		ZE_TEST_CASE("for different values")
		{
			ZEGUID TestItem(1615353811, 467, 930, 11564982415960004267);
			
			ItemString = TestItem.ToString();
			ZE_TEST_CHECK_STRING_EQUAL(ItemString, "604857D3-01D3-03A2-A07F-123BC0EF56AB");
		}
	}
}
