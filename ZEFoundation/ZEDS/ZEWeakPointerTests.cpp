//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEWeakPointerTests.cpp
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
#include "ZEWeakPointer.h"

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

ZETestSuiteAdd(ZEWeakPointer)
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

		ZEWeakPointer<ZETestClass> WeakPointer1;
		CHECK(WeakPointer1.IsNull());
		CHECK(WeakPointer1.GetReferenceCount() == 0);
		CHECK(WeakPointer1.GetWeakReferenceCount() == 0);
		CHECK(WeakPointer1.GetPointer() == 0);

		CHECK_STATUS(ZE_TCS_NOT_CONSTRUCTED);

		ZESharedPointer<ZETestClass> SharedNull;
		ZEWeakPointer<ZETestClass> WeakPointer2(SharedNull);
		CHECK(WeakPointer2.IsNull());
		CHECK(WeakPointer2.GetReferenceCount() == 0);
		CHECK(WeakPointer2.GetWeakReferenceCount() == 0);
		CHECK(WeakPointer2.GetPointer() == 0);

		CHECK_STATUS(ZE_TCS_NOT_CONSTRUCTED);

		ZEWeakPointer<ZETestClass> WeakPointer3;
		WeakPointer3 = SharedNull;
		CHECK(WeakPointer3.IsNull());
		CHECK(WeakPointer3.GetReferenceCount() == 0);
		CHECK(WeakPointer3.GetWeakReferenceCount() == 0);
		CHECK(WeakPointer3.GetPointer() == 0);

		CHECK_STATUS(ZE_TCS_NOT_CONSTRUCTED);
	}

	ZETestItemAdd(CopyRelease)
	{
		RESET();

		ZESharedPointer<ZETestClass> SharedPointer(new ZETestClass());
		ZETestClass* TestObject = new ZETestClass();
		ZEWeakPointer<ZETestClass> WeakPointer;

		WeakPointer.Copy(SharedPointer);
		CHECK(!WeakPointer.IsNull());
		CHECK(WeakPointer.GetReferenceCount() == 1);
		CHECK(WeakPointer.GetWeakReferenceCount() == 1);
		CHECK(WeakPointer.GetPointer() == SharedPointer.GetPointer());
		CHECK(SharedPointer.GetWeakReferenceCount() == 1);

		WeakPointer.Release();
		CHECK(WeakPointer.IsNull());
		CHECK(WeakPointer.GetReferenceCount() == 0);
		CHECK(WeakPointer.GetWeakReferenceCount() == 0);
		CHECK(WeakPointer.GetPointer() == NULL);
		CHECK(!SharedPointer.IsNull());
		CHECK(SharedPointer.GetReferenceCount() == 1);
		CHECK(SharedPointer.GetWeakReferenceCount() == 0);
	}

	ZETestItemAdd(Assingments)
	{
		RESET();
		ZESharedPointer<ZETestClass> SharedPointer(new ZETestClass());
		SCOPE()
		{
			ZEWeakPointer<ZETestClass> WeakPointer(SharedPointer);
			
			CHECK(!WeakPointer.IsNull());
			CHECK(WeakPointer.GetWeakReferenceCount() == 1);
			CHECK(WeakPointer.GetPointer() == SharedPointer.GetPointer());
		}

		RESET();
		SCOPE()
		{
			ZEWeakPointer<ZETestClass> WeakPointer;
			WeakPointer = SharedPointer;
			CHECK(WeakPointer.GetReferenceCount() == 1);
			CHECK(WeakPointer.GetWeakReferenceCount() == 1);
			CHECK(WeakPointer.GetPointer() == SharedPointer.GetPointer());
		}

		RESET();
		SCOPE()
		{
			ZEWeakPointer<ZETestClass> WeakPointer1(SharedPointer);	
			ZEWeakPointer<ZETestClass> WeakPointer2(WeakPointer1);	
			CHECK(WeakPointer2.GetReferenceCount() == 1);
			CHECK(WeakPointer2.GetWeakReferenceCount() == 2);
			CHECK(WeakPointer2.GetPointer() == SharedPointer.GetPointer());
		}

		RESET();
		SCOPE()
		{
			ZEWeakPointer<ZETestClass> WeakPointer1(SharedPointer);	
			ZEWeakPointer<ZETestClass> WeakPointer2;
			WeakPointer2 = WeakPointer1;	
			CHECK(WeakPointer2.GetReferenceCount() == 1);
			CHECK(WeakPointer2.GetWeakReferenceCount() == 2);
			CHECK(WeakPointer2.GetPointer() == SharedPointer.GetPointer());
		}
	}

	ZETestItemAdd(ReferanceCounting)
	{
		// Chain Assingment
		RESET();
		SCOPE() 
		{
			ZESharedPointer<ZETestClass> SharedPointer(new ZETestClass());

			SCOPE()
			{
				ZEWeakPointer<ZETestClass> WeakPointer1(SharedPointer);
				CHECK(WeakPointer1.GetWeakReferenceCount() == 1);
				CHECK_INSTANCE_COUNT(1);

				ZEWeakPointer<ZETestClass> WeakPointer2 = WeakPointer1;
				CHECK(WeakPointer1.GetWeakReferenceCount() == 2);
				CHECK(WeakPointer2.GetWeakReferenceCount() == 2);
				CHECK(WeakPointer2.GetPointer() == SharedPointer.GetPointer());
				CHECK_INSTANCE_COUNT(1);

				ZEWeakPointer<ZETestClass> WeakPointer3 = WeakPointer1;
				CHECK(WeakPointer1.GetWeakReferenceCount() == 3);
				CHECK(WeakPointer2.GetWeakReferenceCount() == 3);
				CHECK(WeakPointer3.GetWeakReferenceCount() == 3);
				CHECK(WeakPointer3.GetPointer() == SharedPointer.GetPointer());
				CHECK_INSTANCE_COUNT(1);

				ZEWeakPointer<ZETestClass> WeakPointer4(WeakPointer2);
				CHECK(WeakPointer1.GetWeakReferenceCount() == 4);
				CHECK(WeakPointer2.GetWeakReferenceCount() == 4);
				CHECK(WeakPointer3.GetWeakReferenceCount() == 4);
				CHECK(WeakPointer4.GetWeakReferenceCount() == 4);
				CHECK(WeakPointer4.GetPointer() == SharedPointer.GetPointer());
				CHECK_INSTANCE_COUNT(1);
			}

			CHECK_STATUS(ZE_TCS_CONSTRUCTED);
			CHECK_INSTANCE_COUNT(1);

			CHECK(!SharedPointer.IsNull());
			CHECK(SharedPointer.GetReferenceCount() == 1);
			CHECK(SharedPointer.GetWeakReferenceCount() == 0);
			CHECK(SharedPointer.GetPointer() != NULL);
		}

		// Shared pointer is dead and weak pointers are also dead.
		RESET();
		SCOPE() 
		{
			ZEWeakPointer<ZETestClass> WeakPointer1, WeakPointer2, WeakPointer3;
			SCOPE()
			{
				ZESharedPointer<ZETestClass> SharedPointer(new ZETestClass());
				WeakPointer1 = SharedPointer;
				WeakPointer2 = SharedPointer;
				WeakPointer3 = WeakPointer1;

				CHECK(SharedPointer.GetWeakReferenceCount() == 3);

				CHECK(!WeakPointer1.IsNull());
				CHECK(!WeakPointer2.IsNull());
				CHECK(!WeakPointer3.IsNull());
			}
			CHECK(WeakPointer1.IsNull());
			CHECK(WeakPointer2.IsNull());
			CHECK(WeakPointer3.IsNull());
		}

		// Original shared pointer is dead but another shared pointer is living and weak pointers are not dead.
		RESET();
		SCOPE() 
		{
			ZEWeakPointer<ZETestClass> WeakPointer1, WeakPointer2, WeakPointer3;
			ZESharedPointer<ZETestClass> SharedPointerScope;
			SCOPE()
			{
				ZESharedPointer<ZETestClass> SharedPointer(new ZETestClass());
				WeakPointer1 = SharedPointer;
				WeakPointer2 = SharedPointer;
				WeakPointer3 = WeakPointer1;

				SharedPointerScope = WeakPointer3.GetInstance();

				CHECK(SharedPointer.GetWeakReferenceCount() == 3);

				CHECK(!WeakPointer1.IsNull());
				CHECK(!WeakPointer2.IsNull());
				CHECK(!WeakPointer3.IsNull());

				CHECK(SharedPointer.GetReferenceCount() == 2);
			}

			CHECK(!WeakPointer1.IsNull());
			CHECK(!WeakPointer2.IsNull());
			CHECK(!WeakPointer3.IsNull());
			CHECK(SharedPointerScope.GetWeakReferenceCount() == 3);
			CHECK(SharedPointerScope.GetReferenceCount() == 1);
		}
	}
}
