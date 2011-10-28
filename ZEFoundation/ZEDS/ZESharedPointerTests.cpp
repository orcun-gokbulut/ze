//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESharedPointerTests.cpp
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
#include "ZESharedPointer.h"

enum ZETestClassStatus
{
	ZE_TCS_NOT_CONSTRUCTED,
	ZE_TCS_CONSTRUCTED,
	ZE_TCS_DESTRUCTED
};

class ZETestClass
{
public:
	static int Status;
	static int InstanceCount;

	static void Reset()
	{
		Status = ZE_TCS_NOT_CONSTRUCTED;
		InstanceCount = 0;
	}

	ZETestClass()
	{
		InstanceCount++;
		Status = ZE_TCS_CONSTRUCTED;
	}

	~ZETestClass()
	{
		InstanceCount--;
		Status = ZE_TCS_DESTRUCTED;
	}
};

int ZETestClass::Status;
int ZETestClass::InstanceCount;

#define RESET() ZETestClass::Reset()
#define SCOPE() 
#define CHECK_STATUS(Status1) CHECK(ZETestClass::Status == Status1)
#define CHECK_INSTANCE_COUNT(Count) CHECK(ZETestClass::InstanceCount == Count)

ZETestSuiteAdd(ZESharedPointer)
{
	ZETestItemAdd(TestClassTest)
	{
		RESET();
		CHECK_STATUS(ZE_TCS_NOT_CONSTRUCTED);
		CHECK_INSTANCE_COUNT(0);

		ZETestClass* A = new ZETestClass();
		CHECK_STATUS(ZE_TCS_CONSTRUCTED);
		CHECK_INSTANCE_COUNT(1);

		delete A;
		CHECK_STATUS(ZE_TCS_DESTRUCTED);
		CHECK_INSTANCE_COUNT(0);

		A = new ZETestClass();
		ZETestClass* B = new ZETestClass();
		CHECK_STATUS(ZE_TCS_CONSTRUCTED);
		CHECK_INSTANCE_COUNT(2);

		delete A;
		CHECK_STATUS(ZE_TCS_DESTRUCTED);
		CHECK_INSTANCE_COUNT(1);
		delete B;
		CHECK_STATUS(ZE_TCS_DESTRUCTED);
		CHECK_INSTANCE_COUNT(0);
	}

	ZETestItemAdd(DefaultConstructor)
	{
		RESET();

		ZESharedPointer<ZETestClass> SharedPointer1;
		CHECK(SharedPointer1.IsNull());
		CHECK(SharedPointer1.GetReferenceCount() == 0);
		CHECK(SharedPointer1.GetPointer() == 0);

		CHECK_STATUS(ZE_TCS_NOT_CONSTRUCTED);

		ZESharedPointer<ZETestClass> SharedPointer2(NULL);
		CHECK(SharedPointer2.IsNull());
		CHECK(SharedPointer2.GetReferenceCount() == 0);
		CHECK(SharedPointer2.GetPointer() == 0);

		CHECK_STATUS(ZE_TCS_NOT_CONSTRUCTED);

	}

	ZETestItemAdd(CreateRelease)
	{
		RESET();
		ZETestClass* TestClass = new ZETestClass();
		ZESharedPointer<ZETestClass> SharedPointer;

		SharedPointer.Create(TestClass);
		CHECK(!SharedPointer.IsNull());
		CHECK(SharedPointer.GetReferenceCount() == 1);
		CHECK(SharedPointer.GetPointer() != NULL);
		CHECK_STATUS(ZE_TCS_CONSTRUCTED);
		CHECK_INSTANCE_COUNT(1);

		SharedPointer.Release();
		CHECK(SharedPointer.IsNull());
		CHECK(SharedPointer.GetReferenceCount() == 0);
		CHECK(SharedPointer.GetPointer() == NULL);
		CHECK_STATUS(ZE_TCS_DESTRUCTED);
		CHECK_INSTANCE_COUNT(0);
	}

	ZETestItemAdd(Assingments)
	{
		RESET();
		SCOPE()
		{
			ZETestClass* TestClass = new ZETestClass();
			ZESharedPointer<ZETestClass> SharedPointer(TestClass);

			CHECK(!SharedPointer.IsNull());
			CHECK(SharedPointer.GetReferenceCount() == 1);
			CHECK(SharedPointer.GetPointer() != NULL);
			CHECK_STATUS(ZE_TCS_CONSTRUCTED);
			CHECK_INSTANCE_COUNT(1);
		}

		RESET();
		SCOPE()
		{
			ZETestClass* TestClass = new ZETestClass();
			ZESharedPointer<ZETestClass> SharedPointer;
			SharedPointer = TestClass;

			CHECK(!SharedPointer.IsNull());
			CHECK(SharedPointer.GetReferenceCount() == 1);
			CHECK(SharedPointer.GetPointer() != NULL);
			CHECK_STATUS(ZE_TCS_CONSTRUCTED);
			CHECK_INSTANCE_COUNT(1);
		}
	}

	ZETestItemAdd(ReferanceCounting)
	{
		RESET();
		ZESharedPointer<ZETestClass> SharedPointerScope;
		SCOPE()
		{
			ZESharedPointer<ZETestClass> SharedPointer1;
			SharedPointer1 = new ZETestClass();
			CHECK(SharedPointer1.GetReferenceCount() == 1);
			CHECK_INSTANCE_COUNT(1);

			ZESharedPointer<ZETestClass> SharedPointer2 = SharedPointer1;
			CHECK(SharedPointer1.GetReferenceCount() == 2);
			CHECK(SharedPointer2.GetReferenceCount() == 2);
			CHECK(SharedPointer2.GetPointer() == SharedPointer1.GetPointer());
			CHECK_INSTANCE_COUNT(1);

			ZESharedPointer<ZETestClass> SharedPointer3 = SharedPointer1;
			CHECK(SharedPointer1.GetReferenceCount() == 3);
			CHECK(SharedPointer2.GetReferenceCount() == 3);
			CHECK(SharedPointer3.GetReferenceCount() == 3);
			CHECK(SharedPointer3.GetPointer() == SharedPointer1.GetPointer());
			CHECK_INSTANCE_COUNT(1);

			ZESharedPointer<ZETestClass> SharedPointer4(SharedPointer2);
			CHECK(SharedPointer1.GetReferenceCount() == 4);
			CHECK(SharedPointer2.GetReferenceCount() == 4);
			CHECK(SharedPointer3.GetReferenceCount() == 4);
			CHECK(SharedPointer4.GetReferenceCount() == 4);
			CHECK(SharedPointer4.GetPointer() == SharedPointer1.GetPointer());
			CHECK_INSTANCE_COUNT(1);

			SharedPointerScope = SharedPointer1;
		}

		CHECK_STATUS(ZE_TCS_CONSTRUCTED);
		CHECK_INSTANCE_COUNT(1);

		CHECK(!SharedPointerScope.IsNull());
		CHECK(SharedPointerScope.GetReferenceCount() == 1);
		CHECK(SharedPointerScope.GetPointer() != NULL);

		SharedPointerScope.Release();

		CHECK_STATUS(ZE_TCS_DESTRUCTED);
		CHECK_INSTANCE_COUNT(0);

		CHECK(SharedPointerScope.IsNull());
		CHECK(SharedPointerScope.GetReferenceCount() == 0);
		CHECK(SharedPointerScope.GetPointer() == NULL);
	}
}
