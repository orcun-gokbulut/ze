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
int ZETestClass::DestroyedInstances;

#define ZETestCheckStatus(Status1) ZETestCheck(ZETestClass::Status == Status1)
#define ZETestCheckInstanceCount(Count) ZETestCheck(ZETestClass::Instances == Count)
#define ZETestCheckDestroyedInstaceCount(Count) ZETestCheck(ZETestClass::DestroyedInstances == Count)

#define ZETestReset() ZETestClass::Reset()

ZETestSuite(ZEPointerTests)
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
		ZETestCheckInstanceCount(0);
		ZETestCheckStatus(ZE_TCS_DESTRUCTED);

		ZETestClass* B = new ZETestClass();
		ZETestClass* C = new ZETestClass();
		ZETestClass* D = new ZETestClass();
		ZETestCheckInstanceCount(3);

		delete B;
		delete C;
		delete D;

		ZETestReset();
		ZETestCheckStatus(ZE_TCS_NOT_CONSTRUCTED);
		ZETestCheckInstanceCount(0);
	}

	ZETest(DefaultConstructor)
	{
		ZETestReset();

		ZEPointer<ZETestClass> Pointer1;
		ZETestCheck(Pointer1.IsNull());
		ZETestCheck(Pointer1.GetPointer() == NULL);

		ZEPointer<ZETestClass> Pointer2(NULL);
		ZETestCheck(Pointer2.IsNull());
		ZETestCheck(Pointer2.GetPointer() == NULL);

		ZEPointer<ZETestClass> Pointer3;
		Pointer3 = NULL;
		ZETestCheck(Pointer3.IsNull());
		ZETestCheck(Pointer3.GetPointer() == NULL);
	}

	ZETest(CreateRelease)
	{
		ZETestReset();

		ZETestClass* TestClass = new ZETestClass();
		ZEPointer<ZETestClass> Pointer;
			
		Pointer.Create(TestClass);
		ZETestCheck(!Pointer.IsNull());
		ZETestCheck(Pointer.GetPointer() == TestClass);

		Pointer.Release();
		ZETestCheck(Pointer.IsNull());
		ZETestCheck(Pointer.GetPointer() == NULL);
		ZETestCheckStatus(ZE_TCS_DESTRUCTED);
		ZETestCheckInstanceCount(0);
		ZETestCheckDestroyedInstaceCount(1);
	}

	ZETest(Assignment)
	{
		ZETestReset();
		ZETestScope()
		{
			ZETestClass* TestClass1 = new ZETestClass();
			ZEPointer<ZETestClass> Pointer1(TestClass1);
			ZETestCheck(!Pointer1.IsNull());
			ZETestCheck(Pointer1.GetPointer() == TestClass1);

			ZETestClass* TestClass2 = new ZETestClass();
			ZEPointer<ZETestClass> Pointer2;
			Pointer2 = TestClass2;
			ZETestCheck(!Pointer2.IsNull());
			ZETestCheck(Pointer2.GetPointer() == TestClass2);
		}
	}

	ZETest(AutoDeletion)
	{
		ZETestReset();
		ZETestScope()
		{
			ZEPointer<ZETestClass> Pointer(new ZETestClass());
			ZETestCheckStatus(ZE_TCS_CONSTRUCTED);
			ZETestCheckInstanceCount(1);
		}
		ZETestCheckStatus(ZE_TCS_DESTRUCTED);
		ZETestCheckInstanceCount(0);
		ZETestCheckDestroyedInstaceCount(1);
	}

	ZETest(ChainAssingments)
	{
		ZETestReset();
		ZETestScope()
		{
			ZETestClass* TestClass = new ZETestClass();
			ZEPointer<ZETestClass> Pointer1(TestClass);
			
			ZEPointer<ZETestClass> Pointer2(Pointer1);
			ZETestCheck(Pointer1.IsNull());
			ZETestCheck(!Pointer2.IsNull());
			ZETestCheck(Pointer2.GetPointer() == TestClass);
		
			ZEPointer<ZETestClass> Pointer3;
			Pointer3 = Pointer2;
			ZETestCheck(Pointer2.IsNull());
			ZETestCheck(!Pointer3.IsNull());
			ZETestCheck(Pointer3.GetPointer() == TestClass);
		}
		ZETestCheckStatus(ZE_TCS_DESTRUCTED);
		ZETestCheckInstanceCount(0);
		ZETestCheckDestroyedInstaceCount(1);
	}
}
