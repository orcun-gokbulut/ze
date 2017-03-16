//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLDataTests.cpp
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

#include "ZEML/ZEMLData.h"
#include "ZETest/ZETest.h"
#include "ZETest/ZETestCheck.h"
#include "ZEError.h"
#include "ZEFile/ZEFile.h"
#include "ZEMLNode.h"


ZE_TEST(ZEMLData)
{
	ZE_TEST_ITEM("ZEMLDataProperty::~ZEMLDataProperty()")
	{

	}

	ZE_TEST_ITEM("ZEMLDataProperty::ZEMLDataProperty()")
	{
		ZEMLDataProperty DataProperty;

		ZE_TEST_CHECK_EQUAL(DataProperty.GetType(), ZEML_IT_INLINE_DATA);
		ZE_TEST_CHECK_ERROR(DataProperty.GetName() == NULL);
		ZE_TEST_CHECK_ERROR(DataProperty.GetData() == NULL);
	}

	ZE_TEST_ITEM("ZEMLDataProperty::ZEMLDataProperty(const ZEString& Name)")
	{
		ZEMLDataProperty DataProperty("TestDataProperty");

		ZE_TEST_CHECK_EQUAL(DataProperty.GetType(), ZEML_IT_INLINE_DATA);
		ZE_TEST_CHECK_STRING_EQUAL(DataProperty.GetName(), "TestDataProperty");
		ZE_TEST_CHECK_ERROR(DataProperty.GetData() == NULL);
	}

	ZE_TEST_ITEM("ZEMLDataProperty::ZEMLDataProperty(const ZEString& Name ,void* Data, ZEUInt64 DataSize, bool Cache)")
	{
		void* Data = new char[sizeof(unsigned char)];

		ZEMLDataProperty DataProperty("TestDataProperty", Data, sizeof(unsigned char), true);

		ZE_TEST_CHECK_EQUAL(DataProperty.GetType(), ZEML_IT_INLINE_DATA);
		ZE_TEST_CHECK_STRING_EQUAL(DataProperty.GetName(), "TestDataProperty");
		ZE_TEST_CHECK_ERROR(DataProperty.GetData() != Data);
		ZEInt Result = memcmp(Data, DataProperty.GetData(), sizeof(unsigned char));
		ZE_TEST_CHECK_EQUAL(Result, 0);
		ZE_TEST_CHECK_EQUAL(DataProperty.GetDataSize(), sizeof(unsigned char));

		ZE_TEST_CASE("for cached false")
		{
			ZEMLDataProperty TestDataProperty("TestDataProperty", Data, sizeof(unsigned char), false);

			ZE_TEST_CHECK_EQUAL(TestDataProperty.GetType(), ZEML_IT_INLINE_DATA);
			ZE_TEST_CHECK_STRING_EQUAL(TestDataProperty.GetName(), "TestDataProperty");
			ZE_TEST_CHECK_EQUAL(TestDataProperty.GetData(), Data);
			Result = memcmp(Data, TestDataProperty.GetData(), sizeof(unsigned char));
			ZE_TEST_CHECK_EQUAL(Result, 0);
			ZE_TEST_CHECK_EQUAL(TestDataProperty.GetDataSize(), sizeof(unsigned char));
		}

		ZE_TEST_CASE("for different DataSize and cached")
		{
			ZEMLDataProperty TestDataProperty("TestDataProperty", Data, sizeof(ZEInt32), true);

			ZE_TEST_CHECK_EQUAL(TestDataProperty.GetType(), ZEML_IT_INLINE_DATA);
			ZE_TEST_CHECK_STRING_EQUAL(TestDataProperty.GetName(), "TestDataProperty");
			ZE_TEST_CHECK_ERROR(TestDataProperty.GetData() != Data);
			Result = memcmp(Data, TestDataProperty.GetData(), sizeof(ZEInt32));
			ZE_TEST_CHECK_EQUAL(Result, 0);
			ZE_TEST_CHECK_EQUAL(TestDataProperty.GetDataSize(), sizeof(ZEInt32));
		}

		ZE_TEST_CASE("for different DataSize and not cached")
		{
			ZEMLDataProperty TestDataProperty("TestDataProperty", Data, sizeof(ZEInt32), false);

			ZE_TEST_CHECK_EQUAL(TestDataProperty.GetType(), ZEML_IT_INLINE_DATA);
			ZE_TEST_CHECK_STRING_EQUAL(TestDataProperty.GetName(), "TestDataProperty");
			ZE_TEST_CHECK_EQUAL(TestDataProperty.GetData(), Data);
			Result = memcmp(Data, TestDataProperty.GetData(), sizeof(ZEInt32));
			ZE_TEST_CHECK_EQUAL(Result, 0);
			ZE_TEST_CHECK_EQUAL(TestDataProperty.GetDataSize(), sizeof(ZEInt32));
		}
	}

	ZE_TEST_ITEM("ZEUInt64 ZEMLDataProperty::GetTotalSize()")
	{
		ZEUInt64 TotalSize;
		void* Data = new char[sizeof(unsigned char)];
		
		ZE_TEST_CASE("for DataSize unsigned char")
		{
			ZEMLDataProperty DataProperty("Test", Data, sizeof(unsigned char), false);
			ZE_TEST_CHECK_EQUAL(DataProperty.GetDataSize(), sizeof(unsigned char));

			TotalSize = DataProperty.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 17);
		}

		ZE_TEST_CASE("for DataSize ZEInt8")
		{
			ZEMLDataProperty DataProperty("Test", Data, sizeof(ZEInt8), true);
			ZE_TEST_CHECK_EQUAL(DataProperty.GetDataSize(), sizeof(ZEInt8));

			TotalSize = DataProperty.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 17);
		}

		ZE_TEST_CASE("for DataSize ZEInt16")
		{
			ZEMLDataProperty DataProperty("Test", Data, sizeof(ZEInt16), false);
			ZE_TEST_CHECK_EQUAL(DataProperty.GetDataSize(), sizeof(ZEInt16));

			TotalSize = DataProperty.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 18);
		}

		ZE_TEST_CASE("for DataSize ZEInt32")
		{
			ZEMLDataProperty DataProperty("Test", Data, sizeof(ZEInt32), false);
			ZE_TEST_CHECK_EQUAL(DataProperty.GetDataSize(), sizeof(ZEInt32));

			TotalSize = DataProperty.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 20);
		}

		ZE_TEST_CASE("for DataSize ZEInt64")
		{
			ZEMLDataProperty DataProperty("Test", Data, sizeof(ZEInt64), false);
			ZE_TEST_CHECK_EQUAL(DataProperty.GetDataSize(), sizeof(ZEInt64));

			TotalSize = DataProperty.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 24);
		}

		ZE_TEST_CASE("for DataSize ZEUInt8")
		{
			ZEMLDataProperty DataProperty("Test", Data, sizeof(ZEUInt8), false);
			ZE_TEST_CHECK_EQUAL(DataProperty.GetDataSize(), sizeof(ZEUInt8));

			TotalSize = DataProperty.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 17);
		}

		ZE_TEST_CASE("for DataSize boolean")
		{
			ZEMLDataProperty DataProperty("Test", Data, sizeof(bool), false);
			ZE_TEST_CHECK_EQUAL(DataProperty.GetDataSize(), sizeof(bool));

			TotalSize = DataProperty.GetTotalSize();
			ZE_TEST_CHECK_EQUAL(TotalSize, 17);
		}
	}

	ZE_TEST_ITEM("void ZEMLDataProperty::SetData(void* Data, ZEUInt64 DataSize, bool Cache)")
	{
		ZEMLDataProperty DataProperty;
		void* Data = new char[sizeof(unsigned char)];
		
		ZE_TEST_CASE("for cached true")
		{
			DataProperty.SetData(Data, sizeof(unsigned char), true);
			ZE_TEST_CHECK_ERROR(DataProperty.GetData() != Data);
			ZEInt Result = memcmp(Data, DataProperty.GetData(), sizeof(unsigned char));
			ZE_TEST_CHECK_EQUAL(Result, 0);
			ZE_TEST_CHECK_EQUAL(DataProperty.GetDataSize(), sizeof(unsigned char));
		}

		ZE_TEST_CASE("for cached false")
		{
			DataProperty.SetData(Data, sizeof(unsigned char), false);
			ZE_TEST_CHECK_EQUAL(DataProperty.GetData(), Data);
			ZEInt Result = memcmp(Data, DataProperty.GetData(), sizeof(unsigned char));
			ZE_TEST_CHECK_EQUAL(DataProperty.GetDataSize(), sizeof(unsigned char));
		}
	}

	ZE_TEST_ITEM("const void* ZEMLDataProperty::GetData()")
	{
		void* Data = new char[sizeof(unsigned char)];

		ZEMLDataProperty DataProperty;
		DataProperty.SetData(Data, sizeof(ZEInt32), true);

		const void* PropertyData = DataProperty.GetData();
		ZE_TEST_CHECK_ERROR(PropertyData != Data);
		ZEInt Result = memcmp(Data, PropertyData, sizeof(ZEInt32));
		ZE_TEST_CHECK_EQUAL(Result, 0);

		ZE_TEST_CASE("for not cached")
		{
			DataProperty.SetData(Data, sizeof(ZEInt32), false);

			const void* TestData = DataProperty.GetData();
			ZE_TEST_CHECK_EQUAL(TestData, Data);
			Result = memcmp(Data, TestData, sizeof(ZEInt32));
			ZE_TEST_CHECK_EQUAL(Result, 0);
		}
	}
}
