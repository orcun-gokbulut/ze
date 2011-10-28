//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPointerTests.cpp
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
#include "ZEPointer.h"

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
		static int Instances;
		static int DestroyedInstances;

		static void Reset()
		{
			Status = ZE_TCS_NOT_CONSTRUCTED;
			Instances = 0;
			DestroyedInstances = 0;
		}

		ZETestClass()
		{
			Instances++;
			Status = ZE_TCS_CONSTRUCTED;
		}

		~ZETestClass()
		{
			Instances--;
			DestroyedInstances++;
			Status = ZE_TCS_DESTRUCTED;
		}
};

int ZETestClass::Status;
int ZETestClass::Instances;

#define CHECK_STATUS(Status1) CHECK(ZETestClass::Status == Status1)
#define CHECK_INSTANCE_COUNT(Count) CHECK(ZETestClass::InstanceCount == Count)
#define CHECK_DESTROYED_INSTANCE_COUNT(Count) CHECK(ZETestClass::DestroyedInstances == Count)

#define RESET() ZETestClass::Reset()
#define SCOPE()

ZETestSuiteAdd(ZEPointerTests)
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
		CHECK_INSTANCE_COUNT(0);
		CHECK_STATUS(ZE_TCS_DESTRUCTED);

		ZETestClass* B = new ZETestClass();
		ZETestClass* C = new ZETestClass();
		ZETestClass* D = new ZETestClass();
		CHECK_INSTANCE_COUNT(3);

		delete B;
		delete C;
		delete D;

		RESET();
		CHECK_STATUS(ZE_TCS_NOT_CONSTRUCTED);
		CHECK_INSTANCE_COUNT(0);
	}

	ZETestItemAdd(DefaultConstructor)
	{
		RESET();

		ZEPointer<ZETestClass> Pointer1;
		CHECK(Pointer1.IsNull());
		CHECK(Pointer1.GetPointer() == NULL);

		ZEPointer<ZETestClass> Pointer2(NULL);
		CHECK(Pointer2.IsNull());
		CHECK(Pointer2.GetPointer() == NULL);

		ZEPointer<ZETestClass> Pointer3;
		Pointer3 = NULL;
		CHECK(Pointer3.IsNull());
		CHECK(Pointer3.GetPointer() == NULL);
	}

	ZETestItemAdd(CreateRelease)
	{
		RESET();

		ZETestClass TestClass = new ZETestClass();
		ZEPointer<ZETestClass> Pointer;
			
		Pointer.Create(TestClass);
		CHECK(!Pointer.IsNull());
		CHECK(Pointer.GetPointer() == TestClass);
		CHECK(Pointer == true);
		CHECK(!Pointer == false);

		Pointer.Release();
		CHECK(Pointer.IsNull());
		CHECK(Pointer.GetPointer() == NULL);
		CHECK(Pointer == false);
		CHECK(!Pointer == true);
		CHECK_STATUS(ZE_TCS_DESTRUCTED);
		CHECK_INSTANCE_COUNT(0);
		CHECK_DESTROYED_INSTANCE_COUNT(1);
	}

	ZETestItemAdd(Assignment)
	{
		RESET();
		SCOPE()
		{
			ZETestClass* TestClass = new ZETestClass();
			ZEPointer<ZETestClass> Pointer1(TestClass);
			CHECK(!Pointer1.IsNull());
			CHECK(Pointer1.GetPointer() == TestClass);

			ZEPointer<ZETestClass> Pointer2();
			Pointer2 = TestClass;
			CHECK(!Pointer2.IsNull());
			CHECK(Pointer2.GetPointer() == TestClass);
		}
	}

	ZETestItemAdd(AutoDeletion)
	{
		RESET();
		SCOPE()
		{
			ZEPointer<ZETestClass> Pointer(new ZETestClass());
			CHECK_STATUS(ZE_TCS_CONSTRUCTED);
			CHECK_INSTANCE_COUNT(1);
		}
		CHECK_STATUS(ZE_TCS_DESTRUCTED);
		CHECK_INSTANCE_COUNT(0);
		CHECK_DESTROYED_INSTANCE_COUNT(1);
	}

	ZETestItemAdd(ChainAssingments)
	{
		RESET();
		SCOPE()
		{
			ZETestClass TestClass = new ZETestClass();
			ZEPointer<ZEPointer> Pointer1(TestClass);
			
			ZEPointer<ZEPointer> Pointer2(Pointer1);
			CHECK(Pointer1.IsNull());
			CHECK(!Pointer2.IsNull());
			CHECK(Pointer2.GetPointer() == TestClass);
		
			ZEPointer<ZEPointer> Pointer3;
			Pointer3 = Pointer2;
			CHECK(Pointer2.IsNull());
			CHECK(!Pointer3.IsNull());
			CHECK(Pointer3.GetPointer() == TestClass);
		}
		CHECK_STATUS(ZE_TCS_DESTRUCTED);
		CHECK_INSTANCE_COUNT(0);
		CHECK_DESTROYED_INSTANCE_COUNT(1);
	}
}
