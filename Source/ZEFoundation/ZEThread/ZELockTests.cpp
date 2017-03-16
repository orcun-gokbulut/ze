//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELockTests.cpp
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
#include "ZELock.h"
#include "ZEThread.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define WINAPI__stdcall

static ZELock* TestLock = new ZELock();
static ZELock Lock;
static ZELock* LockA = new ZELock();

static ZEInt Value = 0;

void FunctionA(ZEThread* Thread, void* Parameter)
{
	while(Thread->ControlPoint())
	{
		Value++;
		printf("\nName: %s \n", Thread->GetName());
		printf("Value = %d \n", Value);

		Thread->Exit();
	}
}

void FunctionB(ZEThread* Thread, void* Parameter)
{
	while(Thread->ControlPoint())
	{
		TestLock->Lock();
		Value *= 2;
		printf("\nName: %s \n", Thread->GetName());
		printf("Value = %d \n", Value);

		Thread->Exit();
	}
}

void TestFunction(ZEThread* Thread, void* Parameter)
{
	while(Thread->ControlPoint())
	{
		printf("\nName: %s, TestFunction \n", Thread->GetName());

		Thread->Exit();
	}
}

void FunctionWithUnlock(ZEThread* Thread, void* Parameter)
{
	while(Thread->ControlPoint())
	{
		printf("\nName: %s, FunctionWithUnlock \n", Thread->GetName());

		Lock.Unlock();
		Thread->Exit();
	}
}


ZE_TEST(ZELock)
{
	ZE_TEST_ITEM("ZELock::ZELock()")
	{
		ZELock LockTest;

		//CurrentNumber: 1
		//NextNumber   : 0
	}

	ZE_TEST_ITEM("ZELock::ZELock(const ZELock& Lock)")
	{
		ZELock LockTest1;

		bool Unlocked = LockTest1.Unlock();
		ZE_TEST_CHECK_ERROR(Unlocked);
		//CurrentNumber: 2
		//NextNumber   : 0

		ZELock LockTest2(LockTest1);
		//CurrentNumber: 1
		//NextNumber   : 0
	}

	ZE_TEST_ITEM("ZELock ZELock::operator=(const ZELock& Lock)")
	{
		ZELock LockTest1;
		ZELock LockTest2;

		bool Unlocked = LockTest2.Unlock();
		ZE_TEST_CHECK_ERROR(Unlocked);
		//CurrentNumber: 2
		//NextNumber   : 0

		LockTest1 = LockTest2;
		//CurrentNumber : 1
		//NextNumber    : 0
	}

	ZE_TEST_ITEM("run threads function with Value and ZELock::Lock() and function with Value")
	{
		ZEThread Thread1;
		ZEThread Thread2;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&FunctionB>());
		Thread1.Run(NULL);

		bool Locked = TestLock->IsLocked();
		ZE_TEST_CHECK_ERROR(!Locked);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&FunctionA>());
		Thread2.Run(NULL);

		Locked = TestLock->IsLocked();
		//ZE_TEST_CHECK_ERROR(Locked);
		
		Sleep(1000);
	}

	ZE_TEST_ITEM("run threads and call ZELock::Lock() and ZELock::Wait() outside of the thread functions")
	{
		ZEThread Thread1;
		ZEThread Thread2;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction>());
		Thread1.Run(NULL);
		
		bool Locked = LockA->IsLocked();
		ZE_TEST_CHECK_ERROR(!Locked);
		LockA->Lock();
		Locked = LockA->IsLocked();
		ZE_TEST_CHECK_ERROR(Locked);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction>());
		Thread2.Run(NULL);

		//LockA->Wait(); //waiting
		Locked = LockA->IsLocked();

		Sleep(1000);	
	}

	ZE_TEST_ITEM("call ZELock::Wait() outside of the thread")
	{
		bool Unlocked = LockA->Unlock();
		ZEThread Thread1;
		ZEThread Thread2;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction>());
		Thread1.Run(NULL);

		bool Locked = LockA->IsLocked();
		ZE_TEST_CHECK_ERROR(!Locked);
		
		LockA->Wait(); //!waiting

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction>());
		Thread2.Run(NULL);

		Locked = LockA->IsLocked();
		ZE_TEST_CHECK_ERROR(!Locked);

		Sleep(1000);
	}


	//for next test
	static ZELock TestLock1;
	void FirstCallLock(ZEThread* Thread, void* Parameter)
	{
		TestLock1.Lock();
		while(Thread->ControlPoint())
		{
			printf("\nName: %s, FirstCallLock \n");
			Thread->Exit();
		}
	}

	void FirstCallWait(ZEThread* Thread, void* Parameter)
	{
		TestLock1.Wait();
		while(Thread->ControlPoint())
		{
			printf("\nName: %s, FirstCallWait \n");
			Thread->Exit();
		}
	}

	ZE_TEST_ITEM("run threads with ZELock::Lock() and ZELock::Wait() respectively")
	{
		ZEThread Thread1;
		ZEThread Thread2;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&FirstCallLock>());
		Thread1.Run(NULL);

		bool Locked = TestLock1.IsLocked();
		ZE_TEST_CHECK_ERROR(!Locked);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&FirstCallWait>());
		Thread2.Run(NULL);

		Locked = TestLock1.IsLocked();
		//ZE_TEST_CHECK_ERROR(Locked);

		Sleep(1000);

		Locked = TestLock1.IsLocked();
		ZE_TEST_CHECK_ERROR(Locked);
		bool Unlocked = TestLock1.Unlock();
	}

	ZE_TEST_ITEM("run threads with ZELock::Wait() and ZELock::Lock() respectively")
	{
		ZEThread Thread1;
		ZEThread Thread2;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&FirstCallWait>());
		Thread1.Run(NULL);

		bool Locked = TestLock1.IsLocked(); //false
		ZE_TEST_CHECK_ERROR(!Locked);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&FirstCallLock>());
		Thread2.Run(NULL);

		Locked = TestLock1.IsLocked(); //false
		ZE_TEST_CHECK_ERROR(!Locked);

		Sleep(1000);

		Locked = TestLock1.IsLocked(); //true
		ZE_TEST_CHECK_ERROR(Locked);
		bool Unlocked = TestLock1.Unlock();
	}

	//for next test
	static ZELock TestLock2;
	void CallLockAndUnlock(ZEThread* Thread, void* Parameter)
	{
		TestLock2.Lock();
		while(Thread->ControlPoint())
		{
			printf("\nName: %s, LockAndUnlock \n", Thread->GetName());

			TestLock2.Unlock();
			Thread->Exit();
		}
	}

	void CallWait(ZEThread* Thread, void* Parameter)
	{
		TestLock2.Wait();
		while(Thread->ControlPoint())
		{
			printf("\nName: %s, CallWait \n", Thread->GetName());

			Thread->Exit();
		}
	}

	ZE_TEST_ITEM("run threads with functions CallLockAndUnlock and CallWait respectively")
	{
		ZEThread Thread1;
		ZEThread Thread2;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&CallLockAndUnlock>());
		Thread1.Run(NULL);

		bool Locked = TestLock2.IsLocked();
		ZE_TEST_CHECK_ERROR(!Locked);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&CallWait>());
		Thread2.Run(NULL);

		Locked = TestLock2.IsLocked();
		//ZE_TEST_CHECK_ERROR(Locked);

		Sleep(1000);
	}

	//for next test
	static ZELock TestLock3;
	void LockFirst(ZEThread* Thread, void* Parameter)
	{
		while(Thread->ControlPoint())
		{
			TestLock3.Lock();
			printf("\nName: %s, LockFirst \n", Thread->GetName());

			Thread->Exit();
		}
	}

	void WaitFirst(ZEThread* Thread, void* Parameter)
	{
		while(Thread->ControlPoint())
		{
			TestLock3.Wait();
			printf("\nName: %s, WaitFirst \n", Thread->GetName());

			Thread->Exit();
		}
	}

	ZE_TEST_ITEM("run threads respectively which call ZELock::Lock() and ZELock::Wait() first inside of the check condition")
	{
		ZEThread Thread1;
		ZEThread Thread2;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&LockFirst>());
		Thread1.Run(NULL);

		bool Locked = TestLock3.IsLocked();
		ZE_TEST_CHECK_ERROR(!Locked);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&WaitFirst>());
		Thread2.Run(NULL);

		Locked = TestLock3.IsLocked();
		//ZE_TEST_CHECK_ERROR(Locked);

		Sleep(1000);

		Locked = TestLock3.IsLocked();
		bool Unlocked = TestLock3.Unlock();
	}

	ZE_TEST_ITEM("run threads respectively which call ZELock::Wait() and ZELock::Lock() first inside of the check condition")
	{
		ZEThread Thread1;
		ZEThread Thread2;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&WaitFirst>());
		Thread1.Run(NULL);

		bool Locked = TestLock2.IsLocked();
		ZE_TEST_CHECK_ERROR(!Locked);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&LockFirst>());
		Thread2.Run(NULL);

		Locked = TestLock2.IsLocked();
		ZE_TEST_CHECK_ERROR(!Locked);

		Sleep(1000);

		Locked = TestLock2.IsLocked();
		ZE_TEST_CHECK_ERROR(!Locked);
	}

	//for next test
	static ZELock Lock1;
	void FunctionWithLock(ZEThread* Thread, void* Parameter)
	{
		while(Thread->ControlPoint())
		{
			printf("\nName: %s, FunctionWithLock \n", Thread->GetName());

			Lock1.Lock();
			Thread->Exit();
		}
	}

	ZE_TEST_ITEM("run threads with functions FunctionWithLock")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		bool Locked = Lock1.IsLocked();
		ZE_TEST_CHECK_ERROR(!Locked);

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&FunctionWithLock>());
		Thread1.Run(NULL);

		Locked = Lock1.IsLocked();
		ZE_TEST_CHECK_ERROR(!Locked);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&FunctionWithLock>());
		Thread2.Run(NULL);

		Locked = Lock1.IsLocked();
		ZE_TEST_CHECK_ERROR(!Locked);

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction>());
		Thread3.Run(NULL);

		Sleep(1000);
		Locked = Lock1.IsLocked();
		ZE_TEST_CHECK_ERROR(Locked);
		bool Unlocked = Lock1.Unlock();
	}

	//for Test IsLocked
	static ZELock LockIsLocked;
	void FunctionLock(ZEThread* Thread, void* Parameter)
	{
		while(Thread->ControlPoint())
		{
			printf("\nName: %s, FunctionLock \n", Thread->GetName());

			LockIsLocked.Lock();
			Thread->Exit();
		}
	}

	ZE_TEST_ITEM("bool ZELock::IsLocked()")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		bool Locked = LockIsLocked.IsLocked();
		ZE_TEST_CHECK_ERROR(!Locked);

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&FunctionLock>());
		Thread1.Run(NULL);

		Locked = LockIsLocked.IsLocked();

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&FunctionLock>());
		Thread2.Run(NULL);

		Locked = LockIsLocked.IsLocked();

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction>());
		Thread3.Run(NULL);

		Locked = LockIsLocked.IsLocked();
		//ZE_TEST_CHECK_ERROR(Locked);

		Sleep(1000);
		bool Unlocked = LockIsLocked.Unlock();
		//Unlocked = LockIsLocked.Unlock();
 	}
 }
