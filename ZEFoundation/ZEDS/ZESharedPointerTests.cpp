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

#define ZETestReset() ZETestClass::Reset()
#define ZETestCheckStatus(Status1) ZETestCheck(ZETestClass::Status == Status1)
#define ZETestCheckInstanceCount(Count) ZETestCheck(ZETestClass::InstanceCount == Count)

ZETestSuite(ZESharedPointer)
{
	ZETest(TestClassTest)
	{
		ZETestReset();
		ZETestCheckStatus(ZE_TCS_NOT_CONSTRUCTED);
		ZETestCheckInstanceCount(0);

		ZETestClass* A = new ZETestClass();
		ZETestCheckStatus(ZE_TCS_CONSTRUCTED);
		ZETestCheckInstanceCount(1);

		delete A;
		ZETestCheckStatus(ZE_TCS_DESTRUCTED);
		ZETestCheckInstanceCount(0);

		A = new ZETestClass();
		ZETestClass* B = new ZETestClass();
		ZETestCheckStatus(ZE_TCS_CONSTRUCTED);
		ZETestCheckInstanceCount(2);

		delete A;
		ZETestCheckStatus(ZE_TCS_DESTRUCTED);
		ZETestCheckInstanceCount(1);
		delete B;
		ZETestCheckStatus(ZE_TCS_DESTRUCTED);
		ZETestCheckInstanceCount(0);
	}

	ZETest(DefaultConstructor)
	{
		ZETestReset();

		ZESharedPointer<ZETestClass> SharedPointer1;
		ZETestCheck(SharedPointer1.IsNull());
		ZETestCheck(SharedPointer1.GetReferenceCount() == 0);
		ZETestCheck(SharedPointer1.GetPointer() == 0);

		ZETestCheckStatus(ZE_TCS_NOT_CONSTRUCTED);

		ZESharedPointer<ZETestClass> SharedPointer2(NULL);
		ZETestCheck(SharedPointer2.IsNull());
		ZETestCheck(SharedPointer2.GetReferenceCount() == 0);
		ZETestCheck(SharedPointer2.GetPointer() == 0);

		ZETestCheckStatus(ZE_TCS_NOT_CONSTRUCTED);

	}

	ZETest(CreateRelease)
	{
		ZETestReset();
		ZETestClass* TestClass = new ZETestClass();
		ZESharedPointer<ZETestClass> SharedPointer;

		SharedPointer.Create(TestClass);
		ZETestCheck(!SharedPointer.IsNull());
		ZETestCheck(SharedPointer.GetReferenceCount() == 1);
		ZETestCheck(SharedPointer.GetPointer() != NULL);
		ZETestCheckStatus(ZE_TCS_CONSTRUCTED);
		ZETestCheckInstanceCount(1);

		SharedPointer.Release();
		ZETestCheck(SharedPointer.IsNull());
		ZETestCheck(SharedPointer.GetReferenceCount() == 0);
		ZETestCheck(SharedPointer.GetPointer() == NULL);
		ZETestCheckStatus(ZE_TCS_DESTRUCTED);
		ZETestCheckInstanceCount(0);
	}

	ZETest(Assingments)
	{
		ZETestReset();
		ZETestScope()
		{
			ZETestClass* TestClass = new ZETestClass();
			ZESharedPointer<ZETestClass> SharedPointer(TestClass);

			ZETestCheck(!SharedPointer.IsNull());
			ZETestCheck(SharedPointer.GetReferenceCount() == 1);
			ZETestCheck(SharedPointer.GetPointer() != NULL);
			ZETestCheckStatus(ZE_TCS_CONSTRUCTED);
			ZETestCheckInstanceCount(1);
		}

		ZETestReset();
		ZETestScope()
		{
			ZETestClass* TestClass = new ZETestClass();
			ZESharedPointer<ZETestClass> SharedPointer;
			SharedPointer = TestClass;

			ZETestCheck(!SharedPointer.IsNull());
			ZETestCheck(SharedPointer.GetReferenceCount() == 1);
			ZETestCheck(SharedPointer.GetPointer() != NULL);
			ZETestCheckStatus(ZE_TCS_CONSTRUCTED);
			ZETestCheckInstanceCount(1);
		}
	}

	ZETest(ReferanceCounting)
	{
		ZETestReset();
		ZESharedPointer<ZETestClass> SharedPointerScope;
		ZETestScope()
		{
			ZESharedPointer<ZETestClass> SharedPointer1;
			SharedPointer1 = new ZETestClass();
			ZETestCheck(SharedPointer1.GetReferenceCount() == 1);
			ZETestCheckInstanceCount(1);

			ZESharedPointer<ZETestClass> SharedPointer2 = SharedPointer1;
			ZETestCheck(SharedPointer1.GetReferenceCount() == 2);
			ZETestCheck(SharedPointer2.GetReferenceCount() == 2);
			ZETestCheck(SharedPointer2.GetPointer() == SharedPointer1.GetPointer());
			ZETestCheckInstanceCount(1);

			ZESharedPointer<ZETestClass> SharedPointer3 = SharedPointer1;
			ZETestCheck(SharedPointer1.GetReferenceCount() == 3);
			ZETestCheck(SharedPointer2.GetReferenceCount() == 3);
			ZETestCheck(SharedPointer3.GetReferenceCount() == 3);
			ZETestCheck(SharedPointer3.GetPointer() == SharedPointer1.GetPointer());
			ZETestCheckInstanceCount(1);

			ZESharedPointer<ZETestClass> SharedPointer4(SharedPointer2);
			ZETestCheck(SharedPointer1.GetReferenceCount() == 4);
			ZETestCheck(SharedPointer2.GetReferenceCount() == 4);
			ZETestCheck(SharedPointer3.GetReferenceCount() == 4);
			ZETestCheck(SharedPointer4.GetReferenceCount() == 4);
			ZETestCheck(SharedPointer4.GetPointer() == SharedPointer1.GetPointer());
			ZETestCheckInstanceCount(1);

			SharedPointerScope = SharedPointer1;
		}

		ZETestCheckStatus(ZE_TCS_CONSTRUCTED);
		ZETestCheckInstanceCount(1);

		ZETestCheck(!SharedPointerScope.IsNull());
		ZETestCheck(SharedPointerScope.GetReferenceCount() == 1);
		ZETestCheck(SharedPointerScope.GetPointer() != NULL);

		SharedPointerScope.Release();

		ZETestCheckStatus(ZE_TCS_DESTRUCTED);
		ZETestCheckInstanceCount(0);

		ZETestCheck(SharedPointerScope.IsNull());
		ZETestCheck(SharedPointerScope.GetReferenceCount() == 0);
		ZETestCheck(SharedPointerScope.GetPointer() == NULL);
	}
}
