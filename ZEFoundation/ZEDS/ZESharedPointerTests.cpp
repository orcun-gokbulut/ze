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
	static int FunctionRunCount;

	int Variable;

	static void Reset()
	{
		Status = ZE_TCS_NOT_CONSTRUCTED;
		FunctionRunCount = 0;
		InstanceCount = 0;
	}

	void Function()
	{
		FunctionRunCount++;
	}

	ZETestClass()
	{
		Variable = 0xABCD1234;
		InstanceCount++;
		Status = ZE_TCS_CONSTRUCTED;
	}

	~ZETestClass()
	{
		InstanceCount--;
		Status = ZE_TCS_DESTRUCTED;
	}
};
#define RESET() ZETestClass::Reset()
#define SCOPE() 
#define CHECK_STATUS(Status1) CHECK(ZETestClass::Status == Status1)
#define CHECK_INSTANTS(Count) CHECK(ZETestClass::InstanceCount == Count)

void FunctionCallByData(ZESharedPointer<ZETestClass> Test)
{
	Test->Function();
}

void FunctionCallByRef(ZESharedPointer<ZETestClass>& Test)
{
	Test->Function();
}
/*
ZETestSuiteAdd(ZESharedPointer)
{
	ZETestItemAdd(TestClassTest)
	{
		RESET();
		CHECK_STATUS(ZE_TCS_NOT_CONSTRUCTED);
		CHECK_INSTANTS(0);
		CHECK(ZETestClass::FunctionRunCount == 0);

		ZETestClass* A = new ZETestClass();
		CHECK_STATUS(ZE_TCS_CONSTRUCTED);
		CHECK_INSTANTS(1);

		CHECK(A->Variable == 0xABCD1234);

		A->Function();
		CHECK(ZETestClass::FunctionRunCount == 1);

		delete A;
		CHECK_STATUS(ZE_TCS_DESTRUCTED);
		CHECK_INSTANTS(0);

		A = new ZETestClass();
		ZETestClass* B = new ZETestClass();
		CHECK_STATUS(ZE_TCS_CONSTRUCTED);
		CHECK_INSTANTS(2);

		A->Function();
		CHECK(ZETestClass::FunctionRunCount == 2);

		delete A;
		CHECK_STATUS(ZE_TCS_DESTRUCTED);
		CHECK_INSTANTS(1);
		delete B;
		CHECK_STATUS(ZE_TCS_DESTRUCTED);
		CHECK_INSTANTS(0);
	}

	ZETestItemAdd(DefaultConstructor)
	{
		RESET();

		ZESharedPointer<ZETestClass> T;
		CHECK(T.IsNull());
		CHECK(T.GetReferanceCount() == 0);

		CHECK_STATUS(ZE_TCS_NOT_CONSTRUCTED);
	}

	ZETestItemAdd(Create)
	{
		RESET();

		ZETestClass* TestClass = new ZETestClass();
		ZESharedPointer<ZETestClass> T;
		T.Create(TestClass);
	
		CHECK(!T.IsNull());
		CHECK(T.GetReferanceCount() == 1);
		CHECK(T.GetPointer() == NULL);
		CHECK_STATUS(ZE_TCS_CONSTRUCTED);
		CHECK_INSTANTS(1);
	}

	ZETestItemAdd(Release)
	{
		RESET();

		ZESharedPointer<ZETestClass> T;
		T.Create(new ZETestClass());
		T.Release();
		CHECK(T.IsNull());
		CHECK(T.GetReferanceCount() == 0);
		CHECK(T1.GetPointer() == NULL);
		CHECK_STATUS(ZE_TCS_DESTRUCTED);
		CHECK_INSTANTS(0);
	}


	ZETestItemAdd(RawConstructor)
	{
		RESET();
	
		SCOPE()
		{
			ZESharedPointer<ZETestClass> T1(NULL);
			CHECK(T1.IsNull());
			CHECK(T1.ReferanceCount() == 0);
			CHECK(T1.GetPointer() == Test2);

			ZETestClass Test = new ZETestClass();
			ZESharedPointer<ZETestClass> T2();

			CHECK(!T2.IsNull());
			CHECK(T2.GetReferanceCount() == 1);
			CHECK(T2.GetPointer() == Test);


		}
	}

	ZETestItemAdd(Copy)
	{
		RESET();

		SCOPE()
		{
			ZESharedPointer<ZETestClass> T1;
			ZESharedPointer<ZETestClass> T2;
			ZETestClass* Test = new ZETestClass();
			T1 = Test;

			T2.Copy(T1);
			CHECK(T1.IsNull());
			CHECK(ZETestClass::ConstructorRunCount == 1 && 
				ZETestClass::DestructorRunCount == 0 && 
				ZETestClass::Status == ZE_TCS_CONSTRUCTED);
			CHECK(!T2.IsNull());
			CHECK(T2.GetPointer() == Test);

			ZESharedPointer<ZETestClass> T3;
			T2.Copy(T3);
			CHECK(T2.IsNull());
			CHECK(ZETestClass::ConstructorRunCount == 1 && 
				ZETestClass::DestructorRunCount == 1 && 
				ZETestClass::Status == ZE_TCS_DESTRUCTED);

			ZESharedPointer<ZETestClass> T4(new ZETestClass());
			T4 = NULL;
			CHECK(T4.IsNull());
			CHECK(ZETestClass::ConstructorRunCount == 2 && 
				ZETestClass::DestructorRunCount == 2 && 
				ZETestClass::Status == ZE_TCS_DESTRUCTED);
		}
	}

	ZETestItemAdd(Operators)
	{
		RESET();

		SCOPE()
		{
			ZESharedPointer<ZETestClass> T1 = new ZETestClass();
			CHECK(T1->Variable == 0xABCD1234);

			T1->Function();
			CHECK(ZETestClass::FunctionRunCount == 1);

			(*T1).Function();
			CHECK(ZETestClass::FunctionRunCount == 2);
		}
		CHECK(ZETestClass::ConstructorRunCount == 1 && 
			ZETestClass::DestructorRunCount == 1 && 
			ZETestClass::Status == ZE_TCS_DESTRUCTED);
	}

	ZETestItemAdd(FunctionCall)
	{
		RESET();

		SCOPE()
		{
			RESET();
			ZESharedPointer<ZETestClass> T1 = new ZETestClass();
			FunctionCallByData(T1);
			CHECK(ZETestClass::Status == ZE_TCS_DESTRUCTED);

			RESET();
			ZESharedPointer<ZETestClass> T2 = new ZETestClass();
			FunctionCallByRef(T2);
			CHECK(ZETestClass::Status == ZE_TCS_CONSTRUCTED);
		}
		CHECK(ZETestClass::Status == ZE_TCS_DESTRUCTED);
	}

	ZETestItemAdd(EmptyPointer)
	{
		RESET();

		SCOPE()
		{
			ZESharedPointer<ZETestClass> T1 = NULL;
			CHECK(T1.IsNull());
			T1.Release();
			CHECK(ZETestClass::Status == ZE_TCS_NOT_CONSTRUCTED &&
				ZETestClass::ConstructorRunCount == 0 &&
				ZETestClass::DestructorRunCount == 0);

			ZESharedPointer<ZETestClass> T2(NULL);
			CHECK(T2.IsNull());
			T2.Release();
			CHECK(ZETestClass::Status == ZE_TCS_NOT_CONSTRUCTED &&
				ZETestClass::ConstructorRunCount == 0 &&
				ZETestClass::DestructorRunCount == 0);

			ZESharedPointer<ZETestClass> T3;
			T3.Create(NULL);
			CHECK(ZETestClass::Status == ZE_TCS_NOT_CONSTRUCTED &&
				ZETestClass::ConstructorRunCount == 0 &&
				ZETestClass::DestructorRunCount == 0);
		}
	}
}*/
