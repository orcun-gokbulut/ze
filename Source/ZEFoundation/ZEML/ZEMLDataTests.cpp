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


ZETestSuite(ZEMLData)
{
	ZETest("ZEMLDataProperty::~ZEMLDataProperty()")
	{

	}

	ZETest("ZEMLDataProperty::ZEMLDataProperty()")
	{
		ZEMLDataProperty DataProperty;

		ZETestCheckEqual(DataProperty.GetType(), ZEML_IT_INLINE_DATA);
		ZETestCheck(DataProperty.GetName() == NULL);
		ZETestCheck(DataProperty.GetData() == NULL);
	}

	ZETest("ZEMLDataProperty::ZEMLDataProperty(const ZEString& Name)")
	{
		ZEMLDataProperty DataProperty("TestDataProperty");

		ZETestCheckEqual(DataProperty.GetType(), ZEML_IT_INLINE_DATA);
		ZETestCheckString(DataProperty.GetName(), "TestDataProperty");
		ZETestCheck(DataProperty.GetData() == NULL);
	}

	ZETest("ZEMLDataProperty::ZEMLDataProperty(const ZEString& Name ,void* Data, ZEUInt64 DataSize, bool Cache)")
	{
		void* Data = new char[sizeof(unsigned char)];

		ZEMLDataProperty DataProperty("TestDataProperty", Data, sizeof(unsigned char), true);

		ZETestCheckEqual(DataProperty.GetType(), ZEML_IT_INLINE_DATA);
		ZETestCheckString(DataProperty.GetName(), "TestDataProperty");
		ZETestCheck(DataProperty.GetData() != Data);
		ZEInt Result = memcmp(Data, DataProperty.GetData(), sizeof(unsigned char));
		ZETestCheckEqual(Result, 0);
		ZETestCheckEqual(DataProperty.GetDataSize(), sizeof(unsigned char));

		ZETestCase("for cached false")
		{
			ZEMLDataProperty TestDataProperty("TestDataProperty", Data, sizeof(unsigned char), false);

			ZETestCheckEqual(TestDataProperty.GetType(), ZEML_IT_INLINE_DATA);
			ZETestCheckString(TestDataProperty.GetName(), "TestDataProperty");
			ZETestCheckEqual(TestDataProperty.GetData(), Data);
			Result = memcmp(Data, TestDataProperty.GetData(), sizeof(unsigned char));
			ZETestCheckEqual(Result, 0);
			ZETestCheckEqual(TestDataProperty.GetDataSize(), sizeof(unsigned char));
		}

		ZETestCase("for different DataSize and cached")
		{
			ZEMLDataProperty TestDataProperty("TestDataProperty", Data, sizeof(ZEInt32), true);

			ZETestCheckEqual(TestDataProperty.GetType(), ZEML_IT_INLINE_DATA);
			ZETestCheckString(TestDataProperty.GetName(), "TestDataProperty");
			ZETestCheck(TestDataProperty.GetData() != Data);
			Result = memcmp(Data, TestDataProperty.GetData(), sizeof(ZEInt32));
			ZETestCheckEqual(Result, 0);
			ZETestCheckEqual(TestDataProperty.GetDataSize(), sizeof(ZEInt32));
		}

		ZETestCase("for different DataSize and not cached")
		{
			ZEMLDataProperty TestDataProperty("TestDataProperty", Data, sizeof(ZEInt32), false);

			ZETestCheckEqual(TestDataProperty.GetType(), ZEML_IT_INLINE_DATA);
			ZETestCheckString(TestDataProperty.GetName(), "TestDataProperty");
			ZETestCheckEqual(TestDataProperty.GetData(), Data);
			Result = memcmp(Data, TestDataProperty.GetData(), sizeof(ZEInt32));
			ZETestCheckEqual(Result, 0);
			ZETestCheckEqual(TestDataProperty.GetDataSize(), sizeof(ZEInt32));
		}
	}

	ZETest("ZEUInt64 ZEMLDataProperty::GetTotalSize()")
	{
		ZEUInt64 TotalSize;
		void* Data = new char[sizeof(unsigned char)];
		
		ZETestCase("for DataSize unsigned char")
		{
			ZEMLDataProperty DataProperty("Test", Data, sizeof(unsigned char), false);
			ZETestCheckEqual(DataProperty.GetDataSize(), sizeof(unsigned char));

			TotalSize = DataProperty.GetTotalSize();
			ZETestCheckEqual(TotalSize, 17);
		}

		ZETestCase("for DataSize ZEInt8")
		{
			ZEMLDataProperty DataProperty("Test", Data, sizeof(ZEInt8), true);
			ZETestCheckEqual(DataProperty.GetDataSize(), sizeof(ZEInt8));

			TotalSize = DataProperty.GetTotalSize();
			ZETestCheckEqual(TotalSize, 17);
		}

		ZETestCase("for DataSize ZEInt16")
		{
			ZEMLDataProperty DataProperty("Test", Data, sizeof(ZEInt16), false);
			ZETestCheckEqual(DataProperty.GetDataSize(), sizeof(ZEInt16));

			TotalSize = DataProperty.GetTotalSize();
			ZETestCheckEqual(TotalSize, 18);
		}

		ZETestCase("for DataSize ZEInt32")
		{
			ZEMLDataProperty DataProperty("Test", Data, sizeof(ZEInt32), false);
			ZETestCheckEqual(DataProperty.GetDataSize(), sizeof(ZEInt32));

			TotalSize = DataProperty.GetTotalSize();
			ZETestCheckEqual(TotalSize, 20);
		}

		ZETestCase("for DataSize ZEInt64")
		{
			ZEMLDataProperty DataProperty("Test", Data, sizeof(ZEInt64), false);
			ZETestCheckEqual(DataProperty.GetDataSize(), sizeof(ZEInt64));

			TotalSize = DataProperty.GetTotalSize();
			ZETestCheckEqual(TotalSize, 24);
		}

		ZETestCase("for DataSize ZEUInt8")
		{
			ZEMLDataProperty DataProperty("Test", Data, sizeof(ZEUInt8), false);
			ZETestCheckEqual(DataProperty.GetDataSize(), sizeof(ZEUInt8));

			TotalSize = DataProperty.GetTotalSize();
			ZETestCheckEqual(TotalSize, 17);
		}

		ZETestCase("for DataSize boolean")
		{
			ZEMLDataProperty DataProperty("Test", Data, sizeof(bool), false);
			ZETestCheckEqual(DataProperty.GetDataSize(), sizeof(bool));

			TotalSize = DataProperty.GetTotalSize();
			ZETestCheckEqual(TotalSize, 17);
		}
	}

	ZETest("void ZEMLDataProperty::SetData(void* Data, ZEUInt64 DataSize, bool Cache)")
	{
		ZEMLDataProperty DataProperty;
		void* Data = new char[sizeof(unsigned char)];
		
		ZETestCase("for cached true")
		{
			DataProperty.SetData(Data, sizeof(unsigned char), true);
			ZETestCheck(DataProperty.GetData() != Data);
			ZEInt Result = memcmp(Data, DataProperty.GetData(), sizeof(unsigned char));
			ZETestCheckEqual(Result, 0);
			ZETestCheckEqual(DataProperty.GetDataSize(), sizeof(unsigned char));
		}

		ZETestCase("for cached false")
		{
			DataProperty.SetData(Data, sizeof(unsigned char), false);
			ZETestCheckEqual(DataProperty.GetData(), Data);
			ZEInt Result = memcmp(Data, DataProperty.GetData(), sizeof(unsigned char));
			ZETestCheckEqual(DataProperty.GetDataSize(), sizeof(unsigned char));
		}
	}

	ZETest("const void* ZEMLDataProperty::GetData()")
	{
		void* Data = new char[sizeof(unsigned char)];

		ZEMLDataProperty DataProperty;
		DataProperty.SetData(Data, sizeof(ZEInt32), true);

		const void* PropertyData = DataProperty.GetData();
		ZETestCheck(PropertyData != Data);
		ZEInt Result = memcmp(Data, PropertyData, sizeof(ZEInt32));
		ZETestCheckEqual(Result, 0);

		ZETestCase("for not cached")
		{
			DataProperty.SetData(Data, sizeof(ZEInt32), false);

			const void* TestData = DataProperty.GetData();
			ZETestCheckEqual(TestData, Data);
			Result = memcmp(Data, TestData, sizeof(ZEInt32));
			ZETestCheckEqual(Result, 0);
		}
	}
}
