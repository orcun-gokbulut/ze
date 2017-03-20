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

#include "ZETest/ZETest.h"
#include "ZESharedPointer.h"
#include "ZEPointerTestClass.h"

ZE_TEST(ZESharedPointer)
{
	ZE_TEST_ITEM("TestClassTest")
	{
		ZETestReset();
		ZE_TEST_CHECK_ERRORStatus(ZE_TCS_NOT_CONSTRUCTED);
		ZE_TEST_CHECK_ERRORInstanceCount(0);

		ZETestClass* A = new ZETestClass();
		ZE_TEST_CHECK_ERRORStatus(ZE_TCS_CONSTRUCTED);
		ZE_TEST_CHECK_ERRORInstanceCount(1);

		delete A;
		ZE_TEST_CHECK_ERRORStatus(ZE_TCS_DESTRUCTED);
		ZE_TEST_CHECK_ERRORInstanceCount(0);

		A = new ZETestClass();
		ZETestClass* B = new ZETestClass();
		ZE_TEST_CHECK_ERRORStatus(ZE_TCS_CONSTRUCTED);
		ZE_TEST_CHECK_ERRORInstanceCount(2);

		delete A;
		ZE_TEST_CHECK_ERRORStatus(ZE_TCS_DESTRUCTED);
		ZE_TEST_CHECK_ERRORInstanceCount(1);
		delete B;
		ZE_TEST_CHECK_ERRORStatus(ZE_TCS_DESTRUCTED);
		ZE_TEST_CHECK_ERRORInstanceCount(0);
	}

	ZE_TEST_ITEM("DefaultConstructor")
	{
		ZETestReset();

		ZESharedPointer<ZETestClass> SharedPointer1;
		ZE_TEST_CHECK_ERROR(SharedPointer1.IsNull());
		ZE_TEST_CHECK_ERROR(SharedPointer1.GetReferenceCount() == 0);
		ZE_TEST_CHECK_ERROR(SharedPointer1.GetPointer() == 0);

		ZE_TEST_CHECK_ERRORStatus(ZE_TCS_NOT_CONSTRUCTED);

		ZESharedPointer<ZETestClass> SharedPointer2(NULL);
		ZE_TEST_CHECK_ERROR(SharedPointer2.IsNull());
		ZE_TEST_CHECK_ERROR(SharedPointer2.GetReferenceCount() == 0);
		ZE_TEST_CHECK_ERROR(SharedPointer2.GetPointer() == 0);

		ZE_TEST_CHECK_ERRORStatus(ZE_TCS_NOT_CONSTRUCTED);

	}

	ZE_TEST_ITEM("CreateRelease")
	{
		ZETestReset();
		ZETestClass* TestClass = new ZETestClass();
		ZESharedPointer<ZETestClass> SharedPointer;

		SharedPointer.Create(TestClass);
		ZE_TEST_CHECK_ERROR(!SharedPointer.IsNull());
		ZE_TEST_CHECK_ERROR(SharedPointer.GetReferenceCount() == 1);
		ZE_TEST_CHECK_ERROR(SharedPointer.GetPointer() != NULL);
		ZE_TEST_CHECK_ERRORStatus(ZE_TCS_CONSTRUCTED);
		ZE_TEST_CHECK_ERRORInstanceCount(1);

		SharedPointer.Release();
		ZE_TEST_CHECK_ERROR(SharedPointer.IsNull());
		ZE_TEST_CHECK_ERROR(SharedPointer.GetReferenceCount() == 0);
		ZE_TEST_CHECK_ERROR(SharedPointer.GetPointer() == NULL);
		ZE_TEST_CHECK_ERRORStatus(ZE_TCS_DESTRUCTED);
		ZE_TEST_CHECK_ERRORInstanceCount(0);
	}

	ZE_TEST_ITEM("Assingments")
	{
		ZETestReset();
		ZETestScope()
		{
			ZETestClass* TestClass = new ZETestClass();
			ZESharedPointer<ZETestClass> SharedPointer(TestClass);

			ZE_TEST_CHECK_ERROR(!SharedPointer.IsNull());
			ZE_TEST_CHECK_ERROR(SharedPointer.GetReferenceCount() == 1);
			ZE_TEST_CHECK_ERROR(SharedPointer.GetPointer() != NULL);
			ZE_TEST_CHECK_ERRORStatus(ZE_TCS_CONSTRUCTED);
			ZE_TEST_CHECK_ERRORInstanceCount(1);
		}

		ZETestReset();
		ZETestScope()
		{
			ZETestClass* TestClass = new ZETestClass();
			ZESharedPointer<ZETestClass> SharedPointer;
			SharedPointer = TestClass;

			ZE_TEST_CHECK_ERROR(!SharedPointer.IsNull());
			ZE_TEST_CHECK_ERROR(SharedPointer.GetReferenceCount() == 1);
			ZE_TEST_CHECK_ERROR(SharedPointer.GetPointer() != NULL);
			ZE_TEST_CHECK_ERRORStatus(ZE_TCS_CONSTRUCTED);
			ZE_TEST_CHECK_ERRORInstanceCount(1);
		}
	}

	ZE_TEST_ITEM("ReferanceCounting")
	{
		ZETestReset();
		ZESharedPointer<ZETestClass> SharedPointerScope;
		ZETestScope()
		{
			ZESharedPointer<ZETestClass> SharedPointer1;
			SharedPointer1 = new ZETestClass();
			ZE_TEST_CHECK_ERROR(SharedPointer1.GetReferenceCount() == 1);
			ZE_TEST_CHECK_ERRORInstanceCount(1);

			ZESharedPointer<ZETestClass> SharedPointer2 = SharedPointer1;
			ZE_TEST_CHECK_ERROR(SharedPointer1.GetReferenceCount() == 2);
			ZE_TEST_CHECK_ERROR(SharedPointer2.GetReferenceCount() == 2);
			ZE_TEST_CHECK_ERROR(SharedPointer2.GetPointer() == SharedPointer1.GetPointer());
			ZE_TEST_CHECK_ERRORInstanceCount(1);

			ZESharedPointer<ZETestClass> SharedPointer3 = SharedPointer1;
			ZE_TEST_CHECK_ERROR(SharedPointer1.GetReferenceCount() == 3);
			ZE_TEST_CHECK_ERROR(SharedPointer2.GetReferenceCount() == 3);
			ZE_TEST_CHECK_ERROR(SharedPointer3.GetReferenceCount() == 3);
			ZE_TEST_CHECK_ERROR(SharedPointer3.GetPointer() == SharedPointer1.GetPointer());
			ZE_TEST_CHECK_ERRORInstanceCount(1);

			ZESharedPointer<ZETestClass> SharedPointer4(SharedPointer2);
			ZE_TEST_CHECK_ERROR(SharedPointer1.GetReferenceCount() == 4);
			ZE_TEST_CHECK_ERROR(SharedPointer2.GetReferenceCount() == 4);
			ZE_TEST_CHECK_ERROR(SharedPointer3.GetReferenceCount() == 4);
			ZE_TEST_CHECK_ERROR(SharedPointer4.GetReferenceCount() == 4);
			ZE_TEST_CHECK_ERROR(SharedPointer4.GetPointer() == SharedPointer1.GetPointer());
			ZE_TEST_CHECK_ERRORInstanceCount(1);

			SharedPointerScope = SharedPointer1;
		}

		ZE_TEST_CHECK_ERRORStatus(ZE_TCS_CONSTRUCTED);
		ZE_TEST_CHECK_ERRORInstanceCount(1);

		ZE_TEST_CHECK_ERROR(!SharedPointerScope.IsNull());
		ZE_TEST_CHECK_ERROR(SharedPointerScope.GetReferenceCount() == 1);
		ZE_TEST_CHECK_ERROR(SharedPointerScope.GetPointer() != NULL);

		SharedPointerScope.Release();

		ZE_TEST_CHECK_ERRORStatus(ZE_TCS_DESTRUCTED);
		ZE_TEST_CHECK_ERRORInstanceCount(0);

		ZE_TEST_CHECK_ERROR(SharedPointerScope.IsNull());
		ZE_TEST_CHECK_ERROR(SharedPointerScope.GetReferenceCount() == 0);
		ZE_TEST_CHECK_ERROR(SharedPointerScope.GetPointer() == NULL);
	}
}
