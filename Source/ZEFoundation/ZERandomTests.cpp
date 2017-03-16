//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERandomTests.cpp
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

#include "ZEFoundation/ZERandom.h"
#include "ZETest/ZETest.h"
#include "ZETest/ZETestCheck.h"
#include "ZEError.h"

ZE_TEST(ZERandom)
{
	ZE_TEST_ITEM("float ZERandom::GetFloat()")
	{
		float Value1 = ZERandom::GetFloat();

		float Value2 = ZERandom::GetFloat();

		float Value3 = ZERandom::GetFloat();

		float Value4 = ZERandom::GetFloat();

		float Value5 = ZERandom::GetFloat();

		float Value6 = ZERandom::GetFloat();

	}

	ZE_TEST_ITEM("float ZERandom::GetFloatPositive()")
	{
		float Value1 = ZERandom::GetFloatPositive();
		ZE_TEST_CHECK_ERROR(Value1 >= 0.0f);

		float Value2 = ZERandom::GetFloatPositive();
		ZE_TEST_CHECK_ERROR(Value2 >= 0.0f);

		float Value3 = ZERandom::GetFloatPositive();
		ZE_TEST_CHECK_ERROR(Value3 >= 0.0f);

		float Value4 = ZERandom::GetFloatPositive();
		ZE_TEST_CHECK_ERROR(Value4 >= 0.0f);

		float Value5 = ZERandom::GetFloatPositive();
		ZE_TEST_CHECK_ERROR(Value5 >= 0.0f);

		for (ZEInt I = 0 ; I < 100 ; I++)
		{
			float Value = ZERandom::GetFloatPositive();
			printf("\n Value%d = %f\n", I, Value);
			ZE_TEST_CHECK_ERROR(Value >= 0.0f);
		}
	}

	ZE_TEST_ITEM("ZEInt ZERandom::GetInt()")
	{
		ZEInt Value1 = ZERandom::GetInt();
		ZE_TEST_CHECK_ERROR(Value1 <= 2147483647 || Value1 >= -2147483648);

		ZEInt Value2 = ZERandom::GetInt();
		ZE_TEST_CHECK_ERROR(Value2 <= 2147483647 || Value2 >= -2147483648);

		ZEInt Value3 = ZERandom::GetInt();
		ZE_TEST_CHECK_ERROR(Value3 <= 2147483647 || Value3 >= -2147483648);

		ZEInt Value4 = ZERandom::GetInt();
		ZE_TEST_CHECK_ERROR(Value4 <= 2147483647 || Value4 >= -2147483648);

		ZEInt Value5 = ZERandom::GetInt();
		ZE_TEST_CHECK_ERROR(Value5 <= 2147483647 || Value5 >= -2147483648);

		ZEInt Value6 = ZERandom::GetInt();
		ZE_TEST_CHECK_ERROR(Value6 <= 2147483647 || Value6 >= -2147483648);

		for (ZEInt I = 0 ; I < 100 ; I++)
		{
			ZEInt Value = ZERandom::GetInt();
			printf("\n Value%d = %d\n", I, Value);
			ZE_TEST_CHECK_ERROR(Value <= 2147483647 || Value >= -2147483648);
		}
	}

	ZE_TEST_ITEM("ZEUInt ZERandom::GetUInt()")
	{
		ZEUInt Value1 = ZERandom::GetUInt();
		ZE_TEST_CHECK_ERROR(Value1 >= 0 && Value1 <= 2147483647);

		ZEUInt Value2 = ZERandom::GetUInt();
		ZE_TEST_CHECK_ERROR(Value2 >= 0 && Value2 <= 2147483647);

		ZEUInt Value3 = ZERandom::GetUInt();
		ZE_TEST_CHECK_ERROR(Value3 >= 0 && Value3 <= 2147483647);

		ZEUInt Value4 = ZERandom::GetUInt();
		ZE_TEST_CHECK_ERROR(Value4 >= 0 && Value4 <= 2147483647);

		ZEUInt Value5 = ZERandom::GetUInt();
		ZE_TEST_CHECK_ERROR(Value5 >= 0 && Value5 <= 2147483647);

		ZEUInt Value6 = ZERandom::GetUInt();
		ZE_TEST_CHECK_ERROR(Value6 >= 0 && Value6 <= 2147483647);

		for (ZEInt I = 0 ; I < 100 ; I++)
		{
			ZEUInt Value = ZERandom::GetUInt();
			printf("\n Value%d = %d\n", I, Value);
			ZE_TEST_CHECK_ERROR(Value >= 0 && Value <= 2147483647);
		}
	}

	ZE_TEST_ITEM("ZEInt8 ZERandom::GetInt8()")
	{
		ZEInt8 Value1 = ZERandom::GetInt8();
		ZE_TEST_CHECK_ERROR(Value1 <= 127 || Value1 >= -128);

		ZEInt8 Value2 = ZERandom::GetInt8();
		ZE_TEST_CHECK_ERROR(Value2 <= 127 || Value2 >= -128);

		ZEInt8 Value3 = ZERandom::GetInt8();
		ZE_TEST_CHECK_ERROR(Value3 <= 127 || Value3 >= -128);

		ZEInt8 Value4 = ZERandom::GetInt8();
		ZE_TEST_CHECK_ERROR(Value4 <= 127 || Value4 >= -128);

		ZEInt8 Value5 = ZERandom::GetInt8();
		ZE_TEST_CHECK_ERROR(Value5 <= 127 || Value5 >= -128);

		ZEInt8 Value6 = ZERandom::GetInt8();
		ZE_TEST_CHECK_ERROR(Value6 <= 127 || Value6 >= -128);

		for (ZEInt I = 0 ; I < 100 ; I++)
		{
			ZEInt8 Value = ZERandom::GetInt8();
			printf("\n Value%d = %d\n", I, Value);
			ZE_TEST_CHECK_ERROR(Value <= 127 || Value >= -128);
		}
	}

	ZE_TEST_ITEM("ZEInt16 ZERandom::GetInt16()")
	{
		ZEInt16 Value1 = ZERandom::GetInt16();
		ZE_TEST_CHECK_ERROR(Value1 <= 32767 || Value1 >= -32768);

		ZEInt16 Value2 = ZERandom::GetInt16();
		ZE_TEST_CHECK_ERROR(Value2 <= 32767 || Value2 >= -32768);

		ZEInt16 Value3 = ZERandom::GetInt16();
		ZE_TEST_CHECK_ERROR(Value3 <= 32767 || Value3 >= -32768);

		ZEInt16 Value4 = ZERandom::GetInt16();
		ZE_TEST_CHECK_ERROR(Value4 <= 32767 || Value4 >= -32768);

		ZEInt16 Value5 = ZERandom::GetInt16();
		ZE_TEST_CHECK_ERROR(Value5 <= 32767 || Value5 >= -32768);

		ZEInt16 Value6 = ZERandom::GetInt16();
		ZE_TEST_CHECK_ERROR(Value6 <= 32767 || Value6 >= -32768);

		for (ZEInt I = 0 ; I < 100 ; I++)
		{
			ZEInt16 Value = ZERandom::GetInt16();
			printf("\n Value%d = %d\n", I, Value);
			ZE_TEST_CHECK_ERROR(Value <= 32767 || Value >= -32768);
		}
	}

	ZE_TEST_ITEM("ZEInt32 ZERandom::GetInt32()")
	{
		ZEInt32 Value1 = ZERandom::GetInt32();
		ZE_TEST_CHECK_ERROR(Value1 <= 4294967295 || Value1 >= -2147483648); 

		ZEInt32 Value2 = ZERandom::GetInt32();
		ZE_TEST_CHECK_ERROR(Value2 <= 4294967295 || Value2 >= -2147483648);

		ZEInt32 Value3 = ZERandom::GetInt32();
		ZE_TEST_CHECK_ERROR(Value3 <= 4294967295 || Value3 >= -2147483648);

		ZEInt32 Value4 = ZERandom::GetInt32();
		ZE_TEST_CHECK_ERROR(Value4 <= 4294967295 || Value4 >= -2147483648);

		ZEInt32 Value5 = ZERandom::GetInt32();
		ZE_TEST_CHECK_ERROR(Value5 <= 4294967295 || Value5 >= -2147483648);

		ZEInt32 Value6 = ZERandom::GetInt32();
		ZE_TEST_CHECK_ERROR(Value6 <= 4294967295 || Value6 >= -2147483648);

		for (ZEInt I = 0 ; I < 100 ; I++)
		{
			ZEInt32 Value = ZERandom::GetInt32();
			printf("\n Value%d = %d\n", I, Value);
			ZE_TEST_CHECK_ERROR(Value <= 4294967295 || Value >= -2147483648);
		}
	}

	ZE_TEST_ITEM("ZEInt64 ZERandom::GetInt64()")
	{
		ZEInt64 Value1 = ZERandom::GetInt64();
		ZE_TEST_CHECK_ERROR(Value1 <= 18446744073709551615 || Value1 >= -9223372036854775808);

		ZEInt64 Value2 = ZERandom::GetInt64();
		ZE_TEST_CHECK_ERROR(Value2 <= 18446744073709551615 || Value2 >= -9223372036854775808);

		ZEInt64 Value3 = ZERandom::GetInt64();
		ZE_TEST_CHECK_ERROR(Value3 <= 18446744073709551615 || Value3 >= -9223372036854775808);

		ZEInt64 Value4 = ZERandom::GetInt64();
		ZE_TEST_CHECK_ERROR(Value4 <= 18446744073709551615 || Value4 >= -9223372036854775808);

		ZEInt64 Value5 = ZERandom::GetInt64();
		ZE_TEST_CHECK_ERROR(Value5 <= 18446744073709551615 || Value5 >= -9223372036854775808);

		ZEInt64 Value6 = ZERandom::GetInt64();
		ZE_TEST_CHECK_ERROR(Value6 <= 18446744073709551615 || Value6 >= -9223372036854775808);

		for (ZEInt I = 0 ; I < 100 ; I++)
		{
			ZEInt64 Value = ZERandom::GetInt64();
			printf("\n Value%d = %d\n", I, Value);
			ZE_TEST_CHECK_ERROR(Value <= 18446744073709551615 || Value >= -9223372036854775808);
		}
	}

	ZE_TEST_ITEM("ZEUInt8 ZERandom::GetUInt8()")
	{
		ZEUInt8 Value1 = ZERandom::GetUInt8();
		ZE_TEST_CHECK_ERROR(Value1 >= 0 && Value1 <= 255);

		ZEUInt8 Value2 = ZERandom::GetUInt8();
		ZE_TEST_CHECK_ERROR(Value2 >= 0 && Value2 <= 255);

		ZEUInt8 Value3 = ZERandom::GetUInt8();
		ZE_TEST_CHECK_ERROR(Value3 >= 0 && Value3 <= 255);

		ZEUInt8 Value4 = ZERandom::GetUInt8();
		ZE_TEST_CHECK_ERROR(Value4 >= 0 && Value4 <= 255);

		ZEUInt8 Value5 = ZERandom::GetUInt8();
		ZE_TEST_CHECK_ERROR(Value5 >= 0 && Value5 <= 255);

		ZEUInt8 Value6 = ZERandom::GetUInt8();
		ZE_TEST_CHECK_ERROR(Value6 >= 0 && Value6 <= 255);

		for (ZEInt I = 0 ; I < 100 ; I++)
		{
			ZEUInt8 Value = ZERandom::GetUInt8();
			printf("\n Value%d = %d\n", I, Value);
			ZE_TEST_CHECK_ERROR(Value >= 0 && Value <= 255);
		}
	}

	ZE_TEST_ITEM("ZEUInt16 ZERandom::GetUInt16()")
	{
		ZEUInt16 Value1 = ZERandom::GetUInt16();
		ZE_TEST_CHECK_ERROR(Value1 >= 0 && Value1 <= 65535);

		ZEUInt16 Value2 = ZERandom::GetUInt16();
		ZE_TEST_CHECK_ERROR(Value2 >= 0 && Value2 <= 65535);

		ZEUInt16 Value3 = ZERandom::GetUInt16();
		ZE_TEST_CHECK_ERROR(Value3 >= 0 && Value3 <= 65535);

		for (ZEInt I = 0 ; I < 100 ; I++)
		{
			ZEUInt16 Value = ZERandom::GetUInt16();
			printf("\n Value%d = %d\n", I, Value);
			ZE_TEST_CHECK_ERROR(Value >= 0 && Value <= 65535);
		}
	}

	ZE_TEST_ITEM("ZEUInt32 ZERandom::GetUInt32()")
	{
		ZEUInt32 Value1 = ZERandom::GetUInt32();
		ZE_TEST_CHECK_ERROR(Value1 >= 0 && Value1 <= 4294967295);

		ZEUInt32 Value2 = ZERandom::GetUInt32();
		ZE_TEST_CHECK_ERROR(Value2 >= 0 && Value2 <= 4294967295);

		ZEUInt32 Value3 = ZERandom::GetUInt32();
		ZE_TEST_CHECK_ERROR(Value3 >= 0 && Value3 <= 4294967295);

		for (ZEInt I = 0 ; I < 100 ; I++)
		{
			ZEUInt32 Value = ZERandom::GetUInt32();
			printf("\n Value%d = %d\n", I, Value);
			ZE_TEST_CHECK_ERROR(Value >= 0 && Value <= 4294967295);
		}
	}

	ZE_TEST_ITEM("ZEUInt64 ZERandom::GetUInt64()")
	{
		ZEUInt64 Value1 = ZERandom::GetUInt64();
		ZE_TEST_CHECK_ERROR(Value1 >= 0 && Value1 <= 18446744073709551615);

		ZEUInt64 Value2 = ZERandom::GetUInt64();
		ZE_TEST_CHECK_ERROR(Value2 >= 0 && Value2 <= 18446744073709551615);

		ZEUInt64 Value3 = ZERandom::GetUInt64();
		ZE_TEST_CHECK_ERROR(Value3 >= 0 && Value3 <= 18446744073709551615);

		for (ZEInt I = 0 ; I < 100 ; I++)
		{
			ZEUInt64 Value = ZERandom::GetUInt64();
			printf("\n Value%d = %d\n", I, Value);
			ZE_TEST_CHECK_ERROR(Value >= 0 && Value <= 18446744073709551615);
		}
	}

	ZE_TEST_ITEM("bool ZERandom::GetBool()")
	{
		bool Value1 = ZERandom::GetBool();
		ZE_TEST_CHECK_ERROR(Value1 || !Value1);

		bool Value2 = ZERandom::GetBool();
		ZE_TEST_CHECK_ERROR(Value2 || !Value2);

		bool Value3 = ZERandom::GetBool();
		ZE_TEST_CHECK_ERROR(Value3 || !Value3);

		for (ZEInt I = 0 ; I < 100 ; I++)
		{
			bool Value = ZERandom::GetBool();
			ZE_TEST_CHECK_ERROR(Value || !Value);
		}
	}

	ZE_TEST_ITEM("mixed")
	{
  		ZEInt64 Value1 = ZERandom::GetInt64();
		ZE_TEST_CHECK_ERROR(Value1 <= 18446744073709551615 || Value1 >= -9223372036854775808);
  		ZEInt32 Value2 = ZERandom::GetInt32();
		ZE_TEST_CHECK_ERROR(Value2 <= 4294967295 || Value2 >= -2147483648);
  		ZEInt16 Value3 = ZERandom::GetInt16();
		ZE_TEST_CHECK_ERROR(Value3 <= 32767 || Value3 >= -32768);
  		ZEInt8 Value4 = ZERandom::GetInt8();
		ZE_TEST_CHECK_ERROR(Value4 <= 127 || Value4 >= -128);
  		ZEInt Value5 = ZERandom::GetInt();
		ZE_TEST_CHECK_ERROR(Value5 <= 2147483647 || Value5 >= -2147483648);
  		float Value6 = ZERandom::GetFloatPositive();
		ZE_TEST_CHECK_ERROR(Value6 >= 0.0f);
  		float Value7 = ZERandom::GetFloat();
		ZEUInt Value8 = ZERandom::GetUInt();
		ZE_TEST_CHECK_ERROR(Value8 >= 0 && Value8 <= 2147483647);
		ZEUInt8 Value9 = ZERandom::GetUInt8();
		ZE_TEST_CHECK_ERROR(Value9 >= 0 && Value9 <= 255);
		ZEUInt16 Value10 = ZERandom::GetUInt16();
		ZE_TEST_CHECK_ERROR(Value10 >= 0 && Value10 <= 65535);
		ZEUInt32 Value11 = ZERandom::GetUInt32();
		ZE_TEST_CHECK_ERROR(Value11 >= 0 && Value11 <= 4294967295);
		ZEUInt64 Value12 = ZERandom::GetUInt64();
		ZE_TEST_CHECK_ERROR(Value12 >= 0 && Value12 <= 18446744073709551615);
		bool Value13 = ZERandom::GetBool();
		ZE_TEST_CHECK_ERROR(Value13 || !Value13);
	}
}
