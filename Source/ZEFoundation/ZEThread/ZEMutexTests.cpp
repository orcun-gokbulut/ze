//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMutexTests.cpp
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
#include "ZEMutex.h"
#include "ZEThread.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define WINAPI__stdcall

static ZEMutex Mutex;

void Function1(ZEThread* Thread, void* Parameter)
{
	bool Locked = Mutex.Lock(1000);

	while(Thread->ControlPoint())
	{
		printf("\nName: %s, Function1 \n", Thread->GetName());
		Thread->Exit();
	}
}

void Function2(ZEThread* Thread, void* Parameter)
{
	bool Waiting = Mutex.Wait(1000);

	while(Thread->ControlPoint())
	{
		printf("\nName: %s, Function2 \n", Thread->GetName());
		Thread->Exit();
	}
	Sleep(1000);
}

void Function3(ZEThread* Thread, void* Parameter)
{
	Mutex.Lock();

	while(Thread->ControlPoint());
	{
		printf("\nName: %s, Function3\n", Thread->GetName());
		Thread->Exit();
	}
}

void Function4(ZEThread* Thread, void* Parameter)
{
	Mutex.Wait();

	while(Thread->ControlPoint())
	{
		printf("\nName: %s, Function4\n", Thread->GetName());
		Thread->Exit();
	}
}

void Fun1(ZEThread* Thread, void* Parameter)
{
	while(Thread->ControlPoint())
	{
		bool Locked = Mutex.Lock(1000);

		printf("\nName: %s, Fun1\n", Thread->GetName());
		Thread->Exit();
	}
}

void Fun2(ZEThread* Thread, void* Parameter)
{
	while(Thread->ControlPoint())
	{
		bool Waiting = Mutex.Wait(1000);

		printf("\nName: %s, Fun2\n", Thread->GetName());
		Thread->Exit();
	}
}

void Fun3(ZEThread* Thread, void* Parameter)
{
	while(Thread->ControlPoint())
	{
		Mutex.Lock();

		printf("\nName: %s, Fun3\n", Thread->GetName());
		Thread->Exit();
	}
}

void Fun4(ZEThread* Thread, void* Parameter)
{
	while(Thread->ControlPoint())
	{
		Mutex.Wait();

		printf("\nName: %s, Fun4\n", Thread->GetName());
		Thread->Exit();
	}
}

void LockUnlock(ZEThread* Thread, void* Parameter)
{
	bool Locked = Mutex.Lock(1000);

	while(Thread->ControlPoint());
	{
		printf("\nName: %s, LockUnlock\n", Thread->GetName());

		Mutex.Unlock();

		Thread->Exit();
	}

	Sleep(1000);
}

void LockUnlock1(ZEThread* Thread, void* Parameter)
{
	bool Locked = Mutex.Lock(1000);
	Mutex.Unlock();

	while(Thread->ControlPoint());
	{
		printf("\nName: %s, LockUnlock1\n", Thread->GetName());
		Thread->Exit();
	}

	Sleep(1000);
}

void F(ZEThread* Thread, void* Parameter)
{
	while(Thread->ControlPoint())
	{
		printf("\nName: %s, F \n", Thread->GetName());
		Thread->Exit();
	}
}



ZETestSuite(ZEMutex)
{
	ZETest("ZEMutex::ZEMutex()")
	{
		ZEMutex TestMutex;
		ZETestCheck(!TestMutex.IsLocked());
	}

	ZETest("ZEMutex::ZEMutex(const ZEMutex& Lock)")
	{
		ZEMutex TestMutex1;

		ZEMutex TestMutex2(TestMutex1);
		//Handle of TestMutex1 != Handle of TestMutex2
	}

	ZETest("ZEMutex ZEMutex::operator=(const ZEMutex& Lock)")
	{
		ZEMutex TestMutex1;
		ZEMutex TestMutex2;

		TestMutex1 = TestMutex2;
		//Handle of TestMutex1 != Handle of TestMutex2
	}

	ZETest("run threads which call ZEMutex::Wait()")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function4>());
		Thread1.Run(NULL);

		bool Locked = Mutex.IsLocked();

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function4>());
		Thread2.Run(NULL);

		Locked = Mutex.IsLocked();

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function4>());
		Thread3.Run(NULL);

		Locked = Mutex.IsLocked();
		Sleep(1000);
	}

	ZETest("run threads which call ZEMutex::Wait(ZEUInt Milliseconds)")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function2>());
		Thread1.Run(NULL);

		bool Locked = Mutex.IsLocked();

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function2>());
		Thread2.Run(NULL);

		Locked = Mutex.IsLocked();

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function2>());
		Thread3.Run(NULL);

		Locked = Mutex.IsLocked();
		Sleep(1000);
	}

	ZETest("run threads which call ZEMutex::Lock(ZEUInt Milliseconds)")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function1>());
		Thread1.Run(NULL);

		bool Locked = Mutex.IsLocked();

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function1>());
		Thread2.Run(NULL);

		Locked = Mutex.IsLocked();

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function1>());
		Thread3.Run(NULL);

		Locked = Mutex.IsLocked();
		Sleep(1000);
		Mutex.Unlock();
	}

	ZETest("run threads which call ZEMutex::Lock()")
	{
// 		ZEThread Thread1;
// 		ZEThread Thread2;
// 		ZEThread Thread3;
// 
// 		Thread1.SetName("Thread1");
// 		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function3>());
// 		Thread1.Run(NULL);
// 
// 		bool Locked = Mutex.IsLocked();
// 
// 		Thread2.SetName("Thread2");
// 		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function3>());
// 		Thread2.Run(NULL);
// 
// 		Locked = Mutex.IsLocked();
// 
// 		Thread3.SetName("Thread3");
// 		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function3>());
// 		Thread3.Run(NULL);
// 
// 		Locked = Mutex.IsLocked();
// 		Sleep(3000);
	}

	ZETest("call ZEMutex::Lock(ZEUInt Milliseconds) and ZEMutex::Wait(ZEUInt Milliseconds) outside of the thread function")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&F>());
		Thread1.Run(NULL);

		bool Locked = Mutex.IsLocked();
		ZETestCheck(!Locked);
		bool Res = Mutex.Lock(3000); //true

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&F>());
		Thread2.Run(NULL);

		bool Waiting = Mutex.Wait(3000); //true

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&F>());
		Thread3.Run(NULL);

		Sleep(1000);
	}

	ZETest("run thread which calls ZEMutex::Wait()")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function4>());
		Thread1.Run(NULL);

		bool Locked = Mutex.IsLocked();
		ZETestCheck(!Locked);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&F>());
		Thread2.Run(NULL);

		Locked = Mutex.IsLocked();

		Thread3.SetName("Thread3");
		Locked = Mutex.IsLocked();
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&F>());
		Thread3.Run(NULL);

		Locked = Mutex.IsLocked();
		Sleep(1000);
	
		Mutex.Unlock();
	}

	ZETest("run threads which call ZEMutex::Wait() inside of the check condition")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Fun4>());
		Thread1.Run(NULL);

		bool Locked = Mutex.IsLocked();

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Fun4>());
		Thread2.Run(NULL);

		Locked = Mutex.IsLocked();

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&F>());
		Thread3.Run(NULL);

		Locked = Mutex.IsLocked();
		Sleep(1000);
	}

	ZETest("run threads which call ZEMutex::Wait(ZEUInt Milliseconds) inside of the check condition")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Fun2>());
		Thread1.Run(NULL);

		bool Locked = Mutex.IsLocked();

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Fun2>());
		Thread2.Run(NULL);

		Locked = Mutex.IsLocked();

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&F>());
		Thread3.Run(NULL);

		Locked = Mutex.IsLocked();
		Sleep(1000);
	}

	ZETest("run thread which calls ZEMutex::Wait(ZEUInt Milliseconds)")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function2>());
		Thread1.Run(NULL);

		bool Locked = Mutex.IsLocked();

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&F>());
		Thread2.Run(NULL);

		Locked = Mutex.IsLocked();

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&F>());
		Thread3.Run(NULL);

		Locked = Mutex.IsLocked();
		Sleep(1000);
	}

	ZETest("run threads which call ZEMutex::Wait(ZEUInt Milliseconds)")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function2>());
		Thread1.Run(NULL);

		bool Locked = Mutex.IsLocked();

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function2>());
		Thread2.Run(NULL);

		Locked = Mutex.IsLocked();

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&F>());
		Thread3.Run(NULL);

		Locked = Mutex.IsLocked();
 		Sleep(1000);
	}

	ZETest("run threads respectively which call ZEMutex::Wait(ZEUInt Milliseconds) and LockUnlock(ZEThread* Thread, void* Parameter)")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function2>());
		Thread1.Run(NULL);

		bool Locked = Mutex.IsLocked();

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&LockUnlock>());
		Thread2.Run(NULL);

		Locked = Mutex.IsLocked();

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&F>());
		Thread3.Run(NULL);

		Locked = Mutex.IsLocked();
 		Sleep(1000);
	}

	ZETest("run threads respectively which call LockUnlock(ZEThread* Thread, void* Parameter) and ZEMutex::Wait(ZEUInt Milliseconds)")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&LockUnlock>());
		Thread1.Run(NULL);

		bool Locked = Mutex.IsLocked();

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function2>());
		Thread2.Run(NULL);

		Locked = Mutex.IsLocked();

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&F>());
		Thread3.Run(NULL);

		Locked = Mutex.IsLocked();
		Sleep(3000);
	}

	ZETest("run threads respectively which call ZEMutex::Lock(ZEUInt Milliseconds) and ZEMutex::Wait(ZEUInt Milliseconds)")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function1>());
		Thread1.Run(NULL);

		bool Locked = Mutex.IsLocked();

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function2>());
		Thread2.Run(NULL);

		Locked = Mutex.IsLocked();

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&F>());
		Thread3.Run(NULL);

		Locked = Mutex.IsLocked();
		Sleep(1000);
	}
 
	ZETest("run threads respectively which call ZEMutex::Lock() and ZEMutex::Wait()")
	{
// 		ZEThread Thread1;
// 		ZEThread Thread2;
// 		ZEThread Thread3;
// 	
// 		Thread1.SetName("Thread1");
// 		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function3>());
// 		Thread1.Run(NULL);
// 	
// 		bool Locked = Mutex.IsLocked();
// 	
// 		Thread2.SetName("Thread2");
// 		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function4>());
// 		Thread2.Run(NULL);
// 	
// 		Locked = Mutex.IsLocked();
// 	
// 		Thread3.SetName("Thread3");
// 		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&F>());
// 		Thread3.Run(NULL);
// 	
// 		Locked = Mutex.IsLocked();
// 		Sleep(1000);
// 		Mutex.Unlock();
	}

	ZETest("run 3 threads first one calls ZEMutex::Lock(), the others call ZEMutex::Wait()")
	{
// 		ZEThread Thread1;
// 		ZEThread Thread2;
// 		ZEThread Thread3;
// 
// 		Thread1.SetName("Thread1");
// 		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function3>());
// 		Thread1.Run(NULL);
// 
// 		bool Locked = Mutex.IsLocked();
// 
// 		Thread2.SetName("Thread2");
// 		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function4>());
// 		Thread2.Run(NULL);
// 
// 		Locked = Mutex.IsLocked();
// 
// 		Thread3.SetName("Thread3");
// 		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&Function4>());
// 		Thread3.Run(NULL);
// 
// 		Locked = Mutex.IsLocked();
// 		Sleep(1000);
	}

	ZETest("run thread which calls LockUnlock1(ZEThread* Thread, void* Parameter)")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&LockUnlock1>());
		Thread1.Run(NULL);

		bool Locked = Mutex.IsLocked();

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&F>());
		Thread2.Run(NULL);

		Locked = Mutex.IsLocked();

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&F>());
		Thread3.Run(NULL);

		Locked = Mutex.IsLocked();
		Sleep(3000);
	}

	ZETest("run threads which call LockUnlock1(ZEThread* Thread, void* Parameter)")
	{
// 		ZEThread Thread1;
// 		ZEThread Thread2;
// 		ZEThread Thread3;
// 
// 		Thread1.SetName("Thread1");
// 		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&LockUnlock1>());
// 		Thread1.Run(NULL);
// 
// 		bool Locked = Mutex.IsLocked();
// 
// 		Thread2.SetName("Thread2");
// 		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&LockUnlock1>());
// 		Thread2.Run(NULL);
// 
// 		Locked = Mutex.IsLocked();
// 
// 		Thread3.SetName("Thread3");
// 		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&LockUnlock1>());
// 		Thread3.Run(NULL);
// 
// 		Locked = Mutex.IsLocked();
// 		Sleep(3000);
	}
	
	ZETest("run thread which calls ZEMutex::Lock(ZEUInt Milliseconds) and ZEMutex::Unlock()")
	{
// 		ZEThread Thread1;
// 		ZEThread Thread2;
// 		ZEThread Thread3;
// 	
// 		Thread1.SetName("Thread1");
// 		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&LockUnlock>());
// 		Thread1.Run(NULL);
// 	
// 		bool Locked = Mutex.IsLocked();
// 	
// 		Thread2.SetName("Thread2");
// 		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&F>());
// 		Thread2.Run(NULL);
// 	
// 		Locked = Mutex.IsLocked();
// 	
// 		Thread3.SetName("Thread3");
// 		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&F>());
// 		Thread3.Run(NULL);
// 	
// 		Locked = Mutex.IsLocked();
// 		Sleep(3000);
	}
	
	ZETest("run threads which call LockUnlock(ZEThread* Thread, void* Parameter)")
	{
// 		ZEThread Thread1;
// 		ZEThread Thread2;
// 		ZEThread Thread3;
// 	
// 		Thread1.SetName("Thread1");
// 		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&LockUnlock>());
// 		Thread1.Run(NULL);
// 	
// 		bool Locked = Mutex.IsLocked();
// 	
// 		Thread2.SetName("Thread2");
// 		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&LockUnlock>());
// 		Thread2.Run(NULL);
// 	
// 		Locked = Mutex.IsLocked();
// 	
// 		Thread3.SetName("Thread3");
// 		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&F>());
// 		Thread3.Run(NULL);
// 	
// 		Locked = Mutex.IsLocked();
// 		Sleep(3000);
// 		Locked = Mutex.IsLocked();
	}
}
