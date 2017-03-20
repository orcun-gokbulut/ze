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

#include "ZETest/ZETest.h"
#include "ZEWeakPointer.h"
#include "ZEPointerTestClass.h"

ZE_TEST(ZEWeakPointer)
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

		ZEWeakPointer<ZETestClass> WeakPointer1;
		ZE_TEST_CHECK_ERROR(WeakPointer1.IsNull());
		ZE_TEST_CHECK_ERROR(WeakPointer1.GetReferenceCount() == 0);
		ZE_TEST_CHECK_ERROR(WeakPointer1.GetWeakReferenceCount() == 0);
		ZE_TEST_CHECK_ERROR(WeakPointer1.GetPointer() == 0);

		ZE_TEST_CHECK_ERRORStatus(ZE_TCS_NOT_CONSTRUCTED);

		ZESharedPointer<ZETestClass> SharedNull;
		ZEWeakPointer<ZETestClass> WeakPointer2(SharedNull);
		ZE_TEST_CHECK_ERROR(WeakPointer2.IsNull());
		ZE_TEST_CHECK_ERROR(WeakPointer2.GetReferenceCount() == 0);
		ZE_TEST_CHECK_ERROR(WeakPointer2.GetWeakReferenceCount() == 0);
		ZE_TEST_CHECK_ERROR(WeakPointer2.GetPointer() == 0);

		ZE_TEST_CHECK_ERRORStatus(ZE_TCS_NOT_CONSTRUCTED);

		ZEWeakPointer<ZETestClass> WeakPointer3;
		WeakPointer3 = SharedNull;
		ZE_TEST_CHECK_ERROR(WeakPointer3.IsNull());
		ZE_TEST_CHECK_ERROR(WeakPointer3.GetReferenceCount() == 0);
		ZE_TEST_CHECK_ERROR(WeakPointer3.GetWeakReferenceCount() == 0);
		ZE_TEST_CHECK_ERROR(WeakPointer3.GetPointer() == 0);

		ZE_TEST_CHECK_ERRORStatus(ZE_TCS_NOT_CONSTRUCTED);
	}

	ZE_TEST_ITEM("CopyRelease")
	{
		ZETestReset();

		ZESharedPointer<ZETestClass> SharedPointer(new ZETestClass());
		ZETestClass* TestObject = new ZETestClass();
		ZEWeakPointer<ZETestClass> WeakPointer;

		WeakPointer.Copy(SharedPointer);
		ZE_TEST_CHECK_ERROR(!WeakPointer.IsNull());
		ZE_TEST_CHECK_ERROR(WeakPointer.GetReferenceCount() == 1);
		ZE_TEST_CHECK_ERROR(WeakPointer.GetWeakReferenceCount() == 1);
		ZE_TEST_CHECK_ERROR(WeakPointer.GetPointer() == SharedPointer.GetPointer());
		ZE_TEST_CHECK_ERROR(SharedPointer.GetWeakReferenceCount() == 1);

		WeakPointer.Release();
		ZE_TEST_CHECK_ERROR(WeakPointer.IsNull());
		ZE_TEST_CHECK_ERROR(WeakPointer.GetReferenceCount() == 0);
		ZE_TEST_CHECK_ERROR(WeakPointer.GetWeakReferenceCount() == 0);
		ZE_TEST_CHECK_ERROR(WeakPointer.GetPointer() == NULL);
		ZE_TEST_CHECK_ERROR(!SharedPointer.IsNull());
		ZE_TEST_CHECK_ERROR(SharedPointer.GetReferenceCount() == 1);
		ZE_TEST_CHECK_ERROR(SharedPointer.GetWeakReferenceCount() == 0);
	}

	ZE_TEST_ITEM("Assingments")
	{
		ZETestReset();
		ZESharedPointer<ZETestClass> SharedPointer(new ZETestClass());
		ZETestScope()
		{
			ZEWeakPointer<ZETestClass> WeakPointer(SharedPointer);
			
			ZE_TEST_CHECK_ERROR(!WeakPointer.IsNull());
			ZE_TEST_CHECK_ERROR(WeakPointer.GetWeakReferenceCount() == 1);
			ZE_TEST_CHECK_ERROR(WeakPointer.GetPointer() == SharedPointer.GetPointer());
		}

		ZETestReset();
		ZETestScope()
		{
			ZEWeakPointer<ZETestClass> WeakPointer;
			WeakPointer = SharedPointer;
			ZE_TEST_CHECK_ERROR(WeakPointer.GetReferenceCount() == 1);
			ZE_TEST_CHECK_ERROR(WeakPointer.GetWeakReferenceCount() == 1);
			ZE_TEST_CHECK_ERROR(WeakPointer.GetPointer() == SharedPointer.GetPointer());
		}

		ZETestReset();
		ZETestScope()
		{
			ZEWeakPointer<ZETestClass> WeakPointer1(SharedPointer);	
			ZEWeakPointer<ZETestClass> WeakPointer2(WeakPointer1);	
			ZE_TEST_CHECK_ERROR(WeakPointer2.GetReferenceCount() == 1);
			ZE_TEST_CHECK_ERROR(WeakPointer2.GetWeakReferenceCount() == 2);
			ZE_TEST_CHECK_ERROR(WeakPointer2.GetPointer() == SharedPointer.GetPointer());
		}

		ZETestReset();
		ZETestScope()
		{
			ZEWeakPointer<ZETestClass> WeakPointer1(SharedPointer);	
			ZEWeakPointer<ZETestClass> WeakPointer2;
			WeakPointer2 = WeakPointer1;	
			ZE_TEST_CHECK_ERROR(WeakPointer2.GetReferenceCount() == 1);
			ZE_TEST_CHECK_ERROR(WeakPointer2.GetWeakReferenceCount() == 2);
			ZE_TEST_CHECK_ERROR(WeakPointer2.GetPointer() == SharedPointer.GetPointer());
		}
	}

	ZE_TEST_ITEM("ReferanceCounting")
	{
		// Chain Assingment
		ZETestReset();
		ZETestScope() 
		{
			ZESharedPointer<ZETestClass> SharedPointer(new ZETestClass());

			ZETestScope()
			{
				ZEWeakPointer<ZETestClass> WeakPointer1(SharedPointer);
				ZE_TEST_CHECK_ERROR(WeakPointer1.GetWeakReferenceCount() == 1);
				ZE_TEST_CHECK_ERRORInstanceCount(1);

				ZEWeakPointer<ZETestClass> WeakPointer2 = WeakPointer1;
				ZE_TEST_CHECK_ERROR(WeakPointer1.GetWeakReferenceCount() == 2);
				ZE_TEST_CHECK_ERROR(WeakPointer2.GetWeakReferenceCount() == 2);
				ZE_TEST_CHECK_ERROR(WeakPointer2.GetPointer() == SharedPointer.GetPointer());
				ZE_TEST_CHECK_ERRORInstanceCount(1);

				ZEWeakPointer<ZETestClass> WeakPointer3 = WeakPointer1;
				ZE_TEST_CHECK_ERROR(WeakPointer1.GetWeakReferenceCount() == 3);
				ZE_TEST_CHECK_ERROR(WeakPointer2.GetWeakReferenceCount() == 3);
				ZE_TEST_CHECK_ERROR(WeakPointer3.GetWeakReferenceCount() == 3);
				ZE_TEST_CHECK_ERROR(WeakPointer3.GetPointer() == SharedPointer.GetPointer());
				ZE_TEST_CHECK_ERRORInstanceCount(1);

				ZEWeakPointer<ZETestClass> WeakPointer4(WeakPointer2);
				ZE_TEST_CHECK_ERROR(WeakPointer1.GetWeakReferenceCount() == 4);
				ZE_TEST_CHECK_ERROR(WeakPointer2.GetWeakReferenceCount() == 4);
				ZE_TEST_CHECK_ERROR(WeakPointer3.GetWeakReferenceCount() == 4);
				ZE_TEST_CHECK_ERROR(WeakPointer4.GetWeakReferenceCount() == 4);
				ZE_TEST_CHECK_ERROR(WeakPointer4.GetPointer() == SharedPointer.GetPointer());
				ZE_TEST_CHECK_ERRORInstanceCount(1);
			}

			ZE_TEST_CHECK_ERRORStatus(ZE_TCS_CONSTRUCTED);
			ZE_TEST_CHECK_ERRORInstanceCount(1);

			ZE_TEST_CHECK_ERROR(!SharedPointer.IsNull());
			ZE_TEST_CHECK_ERROR(SharedPointer.GetReferenceCount() == 1);
			ZE_TEST_CHECK_ERROR(SharedPointer.GetWeakReferenceCount() == 0);
			ZE_TEST_CHECK_ERROR(SharedPointer.GetPointer() != NULL);
		}

		// Shared pointer is dead and weak pointers are also dead.
		ZETestReset();
		ZETestScope() 
		{
			ZEWeakPointer<ZETestClass> WeakPointer1, WeakPointer2, WeakPointer3;
			ZETestScope()
			{
				ZESharedPointer<ZETestClass> SharedPointer(new ZETestClass());
				WeakPointer1 = SharedPointer;
				WeakPointer2 = SharedPointer;
				WeakPointer3 = WeakPointer1;

				ZE_TEST_CHECK_ERROR(SharedPointer.GetWeakReferenceCount() == 3);

				ZE_TEST_CHECK_ERROR(!WeakPointer1.IsNull());
				ZE_TEST_CHECK_ERROR(!WeakPointer2.IsNull());
				ZE_TEST_CHECK_ERROR(!WeakPointer3.IsNull());
			}
			ZE_TEST_CHECK_ERROR(WeakPointer1.IsNull());
			ZE_TEST_CHECK_ERROR(WeakPointer2.IsNull());
			ZE_TEST_CHECK_ERROR(WeakPointer3.IsNull());
		}

		// Original shared pointer is dead but another shared pointer is living and weak pointers are not dead.
		ZETestReset();
		ZETestScope() 
		{
			ZEWeakPointer<ZETestClass> WeakPointer1, WeakPointer2, WeakPointer3;
			ZESharedPointer<ZETestClass> SharedPointerScope;
			ZETestScope()
			{
				ZESharedPointer<ZETestClass> SharedPointer(new ZETestClass());
				WeakPointer1 = SharedPointer;
				WeakPointer2 = SharedPointer;
				WeakPointer3 = WeakPointer1;

				SharedPointerScope = WeakPointer3.GetInstance();

				ZE_TEST_CHECK_ERROR(SharedPointer.GetWeakReferenceCount() == 3);

				ZE_TEST_CHECK_ERROR(!WeakPointer1.IsNull());
				ZE_TEST_CHECK_ERROR(!WeakPointer2.IsNull());
				ZE_TEST_CHECK_ERROR(!WeakPointer3.IsNull());

				ZE_TEST_CHECK_ERROR(SharedPointer.GetReferenceCount() == 2);
			}

			ZE_TEST_CHECK_ERROR(!WeakPointer1.IsNull());
			ZE_TEST_CHECK_ERROR(!WeakPointer2.IsNull());
			ZE_TEST_CHECK_ERROR(!WeakPointer3.IsNull());
			ZE_TEST_CHECK_ERROR(SharedPointerScope.GetWeakReferenceCount() == 3);
			ZE_TEST_CHECK_ERROR(SharedPointerScope.GetReferenceCount() == 1);
		}
	}
}
