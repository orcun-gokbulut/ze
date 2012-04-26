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

ZETestSuite(ZEWeakPointer)
{
	ZETest("TestClassTest")
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

	ZETest("DefaultConstructor")
	{
		ZETestReset();

		ZEWeakPointer<ZETestClass> WeakPointer1;
		ZETestCheck(WeakPointer1.IsNull());
		ZETestCheck(WeakPointer1.GetReferenceCount() == 0);
		ZETestCheck(WeakPointer1.GetWeakReferenceCount() == 0);
		ZETestCheck(WeakPointer1.GetPointer() == 0);

		ZETestCheckStatus(ZE_TCS_NOT_CONSTRUCTED);

		ZESharedPointer<ZETestClass> SharedNull;
		ZEWeakPointer<ZETestClass> WeakPointer2(SharedNull);
		ZETestCheck(WeakPointer2.IsNull());
		ZETestCheck(WeakPointer2.GetReferenceCount() == 0);
		ZETestCheck(WeakPointer2.GetWeakReferenceCount() == 0);
		ZETestCheck(WeakPointer2.GetPointer() == 0);

		ZETestCheckStatus(ZE_TCS_NOT_CONSTRUCTED);

		ZEWeakPointer<ZETestClass> WeakPointer3;
		WeakPointer3 = SharedNull;
		ZETestCheck(WeakPointer3.IsNull());
		ZETestCheck(WeakPointer3.GetReferenceCount() == 0);
		ZETestCheck(WeakPointer3.GetWeakReferenceCount() == 0);
		ZETestCheck(WeakPointer3.GetPointer() == 0);

		ZETestCheckStatus(ZE_TCS_NOT_CONSTRUCTED);
	}

	ZETest("CopyRelease")
	{
		ZETestReset();

		ZESharedPointer<ZETestClass> SharedPointer(new ZETestClass());
		ZETestClass* TestObject = new ZETestClass();
		ZEWeakPointer<ZETestClass> WeakPointer;

		WeakPointer.Copy(SharedPointer);
		ZETestCheck(!WeakPointer.IsNull());
		ZETestCheck(WeakPointer.GetReferenceCount() == 1);
		ZETestCheck(WeakPointer.GetWeakReferenceCount() == 1);
		ZETestCheck(WeakPointer.GetPointer() == SharedPointer.GetPointer());
		ZETestCheck(SharedPointer.GetWeakReferenceCount() == 1);

		WeakPointer.Release();
		ZETestCheck(WeakPointer.IsNull());
		ZETestCheck(WeakPointer.GetReferenceCount() == 0);
		ZETestCheck(WeakPointer.GetWeakReferenceCount() == 0);
		ZETestCheck(WeakPointer.GetPointer() == NULL);
		ZETestCheck(!SharedPointer.IsNull());
		ZETestCheck(SharedPointer.GetReferenceCount() == 1);
		ZETestCheck(SharedPointer.GetWeakReferenceCount() == 0);
	}

	ZETest("Assingments")
	{
		ZETestReset();
		ZESharedPointer<ZETestClass> SharedPointer(new ZETestClass());
		ZETestScope()
		{
			ZEWeakPointer<ZETestClass> WeakPointer(SharedPointer);
			
			ZETestCheck(!WeakPointer.IsNull());
			ZETestCheck(WeakPointer.GetWeakReferenceCount() == 1);
			ZETestCheck(WeakPointer.GetPointer() == SharedPointer.GetPointer());
		}

		ZETestReset();
		ZETestScope()
		{
			ZEWeakPointer<ZETestClass> WeakPointer;
			WeakPointer = SharedPointer;
			ZETestCheck(WeakPointer.GetReferenceCount() == 1);
			ZETestCheck(WeakPointer.GetWeakReferenceCount() == 1);
			ZETestCheck(WeakPointer.GetPointer() == SharedPointer.GetPointer());
		}

		ZETestReset();
		ZETestScope()
		{
			ZEWeakPointer<ZETestClass> WeakPointer1(SharedPointer);	
			ZEWeakPointer<ZETestClass> WeakPointer2(WeakPointer1);	
			ZETestCheck(WeakPointer2.GetReferenceCount() == 1);
			ZETestCheck(WeakPointer2.GetWeakReferenceCount() == 2);
			ZETestCheck(WeakPointer2.GetPointer() == SharedPointer.GetPointer());
		}

		ZETestReset();
		ZETestScope()
		{
			ZEWeakPointer<ZETestClass> WeakPointer1(SharedPointer);	
			ZEWeakPointer<ZETestClass> WeakPointer2;
			WeakPointer2 = WeakPointer1;	
			ZETestCheck(WeakPointer2.GetReferenceCount() == 1);
			ZETestCheck(WeakPointer2.GetWeakReferenceCount() == 2);
			ZETestCheck(WeakPointer2.GetPointer() == SharedPointer.GetPointer());
		}
	}

	ZETest("ReferanceCounting")
	{
		// Chain Assingment
		ZETestReset();
		ZETestScope() 
		{
			ZESharedPointer<ZETestClass> SharedPointer(new ZETestClass());

			ZETestScope()
			{
				ZEWeakPointer<ZETestClass> WeakPointer1(SharedPointer);
				ZETestCheck(WeakPointer1.GetWeakReferenceCount() == 1);
				ZETestCheckInstanceCount(1);

				ZEWeakPointer<ZETestClass> WeakPointer2 = WeakPointer1;
				ZETestCheck(WeakPointer1.GetWeakReferenceCount() == 2);
				ZETestCheck(WeakPointer2.GetWeakReferenceCount() == 2);
				ZETestCheck(WeakPointer2.GetPointer() == SharedPointer.GetPointer());
				ZETestCheckInstanceCount(1);

				ZEWeakPointer<ZETestClass> WeakPointer3 = WeakPointer1;
				ZETestCheck(WeakPointer1.GetWeakReferenceCount() == 3);
				ZETestCheck(WeakPointer2.GetWeakReferenceCount() == 3);
				ZETestCheck(WeakPointer3.GetWeakReferenceCount() == 3);
				ZETestCheck(WeakPointer3.GetPointer() == SharedPointer.GetPointer());
				ZETestCheckInstanceCount(1);

				ZEWeakPointer<ZETestClass> WeakPointer4(WeakPointer2);
				ZETestCheck(WeakPointer1.GetWeakReferenceCount() == 4);
				ZETestCheck(WeakPointer2.GetWeakReferenceCount() == 4);
				ZETestCheck(WeakPointer3.GetWeakReferenceCount() == 4);
				ZETestCheck(WeakPointer4.GetWeakReferenceCount() == 4);
				ZETestCheck(WeakPointer4.GetPointer() == SharedPointer.GetPointer());
				ZETestCheckInstanceCount(1);
			}

			ZETestCheckStatus(ZE_TCS_CONSTRUCTED);
			ZETestCheckInstanceCount(1);

			ZETestCheck(!SharedPointer.IsNull());
			ZETestCheck(SharedPointer.GetReferenceCount() == 1);
			ZETestCheck(SharedPointer.GetWeakReferenceCount() == 0);
			ZETestCheck(SharedPointer.GetPointer() != NULL);
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

				ZETestCheck(SharedPointer.GetWeakReferenceCount() == 3);

				ZETestCheck(!WeakPointer1.IsNull());
				ZETestCheck(!WeakPointer2.IsNull());
				ZETestCheck(!WeakPointer3.IsNull());
			}
			ZETestCheck(WeakPointer1.IsNull());
			ZETestCheck(WeakPointer2.IsNull());
			ZETestCheck(WeakPointer3.IsNull());
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

				ZETestCheck(SharedPointer.GetWeakReferenceCount() == 3);

				ZETestCheck(!WeakPointer1.IsNull());
				ZETestCheck(!WeakPointer2.IsNull());
				ZETestCheck(!WeakPointer3.IsNull());

				ZETestCheck(SharedPointer.GetReferenceCount() == 2);
			}

			ZETestCheck(!WeakPointer1.IsNull());
			ZETestCheck(!WeakPointer2.IsNull());
			ZETestCheck(!WeakPointer3.IsNull());
			ZETestCheck(SharedPointerScope.GetWeakReferenceCount() == 3);
			ZETestCheck(SharedPointerScope.GetReferenceCount() == 1);
		}
	}
}
