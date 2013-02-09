//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEThreadTests.cpp
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

//#define ZE_PLATFORM_WINDOWS

#include "ZETest/ZETest.h"
#include "ZEThread.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define WINAPI__stdcall


void TestFunction1(ZEThread* Thread, void* Parameter)
{
	while(Thread->ControlPoint())
	{
		printf("\nName: %s, TestFunction1 \n", Thread->GetName());

		Thread->Exit();
	}
}

void TestFunction2(ZEThread* Thread, void* Parameter)
{
	printf("\nName: %s, TestFuction2 \n", Thread->GetName());

	Thread->Exit();
}

void TestFunction3(ZEThread* Thread, void* Parameter)
{
	while(Thread->ControlPoint())
	{
		printf("\nName: %s, TestFunction3 \n", Thread->GetName());

		Thread->Sleep(1000);
	}
}

void TestFunction4(ZEThread* Thread, void* Parameter)
{
	while(Thread->ControlPoint())
	{
		printf("\Name: %s, TestFunction4 \n", Thread->GetName());
	}
}

void TestFunction5(ZEThread* Thread, void* Parameter)
{
	while(Thread->ControlPoint())
	{
		printf("\nName: %s, TestFunction5 \n", Thread->GetName());

		Thread->Sleep(1000);

		Thread->Exit();
	}
}


ZETestSuite(ZEThread)
{
	ZETest("void ZEThread::SetName(const ZEString& Name)")
	{
		ZEThread Thread;

		Thread.SetName("Thread");
		ZETestCheckEqual(Thread.GetName(), "Thread");
	}

	ZETest("const ZEString& ZEThread::GetName()")
	{
		ZEThread Thread;

		Thread.SetName("Thread");

		const ZEString Name = Thread.GetName();
		ZETestCheckEqual(Name, "Thread");
	}

	ZETest("ZEThreadStatus ZEThread::GetStatus()")
	{
		ZEThread Thread;

		ZEThreadStatus Status = Thread.GetStatus();
		ZETestCheckEqual(Status, ZE_TS_NONE);
	}

	ZETest("bool ZEThread::IsAlive()")
	{
		ZEThread Thread;

		bool Alive = Thread.IsAlive();
		ZETestCheck(!Alive);
	}

	ZETest("void ZEThread::SetParameter(void* Parameter)")
	{
		ZEThread Thread;

		Thread.SetParameter("a");
		ZETestCheckEqual(Thread.GetParameter(), "a");
	}

	ZETest("void* ZEThread::GetParameter()")
	{
		ZEThread Thread;

		Thread.SetParameter("a");

		void* Parameter = Thread.GetParameter();
		ZETestCheckEqual(Parameter, "a");
	}

	ZETest("void ZEThread::SetFunction(ZEThreadFunction Function)")
	{
		ZEThread Thread;

		Thread.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction4>());
		ZEThreadFunction Function = Thread.GetFunction();
	}

	ZETest("ZEThreadFunction& ZEThread::GetFunction()")
	{
		ZEThread Thread;

		Thread.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction4>());

		ZEThreadFunction Function = Thread.GetFunction();
	}

	ZETest("void ZEThread::Wait()")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
		Thread1.Run(NULL);

		Thread1.Wait();

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
		Thread2.Run(NULL);

		Thread1.Wait();

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
		Thread3.Run(NULL);

		Thread1.Wait();

		Sleep(3000);
	}

	ZETest("run threads which call Exit() and call ZEThread::Wait(ZEIntMilliseconds) through threads")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
		Thread1.Run(NULL);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
		Thread2.Run(NULL);

		bool Waiting = Thread1.Wait(300);
		ZETestCheck(!Waiting);

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
		Thread3.Run(NULL);

		Waiting = Thread1.Wait(300);
		ZETestCheck(!Waiting);
		Waiting = Thread2.Wait(300);
		ZETestCheck(!Waiting);

		Sleep(1000);
	}

	ZETest("Wait(ZEIntMilliseconds)")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction4>());
		Thread1.Run(NULL);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction4>());
		Thread2.Run(NULL);

		bool Alive = Thread1.IsAlive(); //true
		ZEThreadStatus Status1 = Thread1.GetStatus(); //running
		Alive = Thread2.IsAlive(); //false
		ZEThreadStatus Status2 = Thread2.GetStatus(); //none

		bool Waiting = Thread1.Wait(1000); //false WaitForSingleObject returns 258
		Waiting = Thread2.Wait(1000);
		ZETestCheck(!Waiting);

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
		Thread3.Run(NULL);

		Alive = Thread3.IsAlive(); //false
		ZEThreadStatus Status3 = Thread3.GetStatus(); //none
		Alive = Thread2.IsAlive(); //true
		Status2 = Thread2.GetStatus(); //running
		Alive = Thread1.IsAlive(); //true
		Status1 = Thread1.GetStatus(); //running

		Waiting = Thread1.Wait(1000); //false WaitForSingleObject returns 258
		Waiting = Thread2.Wait(1000); //false WaitForSingleObject returns 258
		Waiting = Thread3.Wait(1000); //false

		Sleep(2000);
	}

	ZETest("run threads with ZEThread::Sleep(ZEUInt Milliseconds), ZEThread::Exit() and ZEThread::Exit() respectively")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction5>());
		Thread1.Run(NULL);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
		Thread2.Run(NULL);

		bool Alive = Thread1.IsAlive();
		ZEThreadStatus Status1 = Thread1.GetStatus();
		Alive = Thread2.IsAlive();
		ZEThreadStatus Status2 = Thread2.GetStatus();

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
		Thread3.Run(NULL);

		Alive = Thread1.IsAlive();
		Status1 = Thread1.GetStatus();
		Alive = Thread2.IsAlive();
		Status2 = Thread2.GetStatus();
		Alive = Thread3.IsAlive();
		ZEThreadStatus Status3 = Thread3.GetStatus();

		Sleep(1000);
	}

	ZETest("set different functions which include Sleep(ZEUInt Milliseconds), Exit() functions individually")
	{
		ZEThreadStatus Status1;
		ZEThreadStatus Status2;
		ZEThreadStatus Status3;
		bool Alive;
		ZEThread* Current;

		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;
		ZEThread Thread4;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
		Thread1.Run(NULL);

		Alive = Thread1.IsAlive(); //false
		Status1 = Thread1.GetStatus(); //none
		Current = ZEThread::GetCurrentThread();
		ZETestCheckEqual(Current, NULL);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
		Thread2.Run(NULL);

		Alive = Thread2.IsAlive(); //false
		Status2 = Thread2.GetStatus();//none
		Alive = Thread1.IsAlive(); //true		
		Status1 = Thread1.GetStatus(); //running
		Current = ZEThread::GetCurrentThread();
		ZETestCheckEqual(Current, NULL);
		
		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction5>());
		Thread3.Run(NULL);

		Alive = Thread3.IsAlive(); //false
		Status3 = Thread3.GetStatus(); //none
		Alive = Thread2.IsAlive(); //true
		Status2 = Thread2.GetStatus(); //running
		Alive = Thread1.IsAlive(); //false
		Status1 = Thread1.GetStatus(); //done
		Current = ZEThread::GetCurrentThread();
		ZETestCheckEqual(Current, NULL);

		Thread4.SetName("Thread4");
		Thread4.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
		Thread4.Run(NULL);

		Alive = Thread4.IsAlive(); //false
		ZEThreadStatus Status4 = Thread4.GetStatus(); //none
		Alive = Thread3.IsAlive(); //true
		Status3 = Thread3.GetStatus(); //running
		Alive = Thread2.IsAlive(); //false
		Status2 = Thread2.GetStatus(); //done
		Alive = Thread1.IsAlive(); //false
		Status1 = Thread1.GetStatus(); //done
		Current = ZEThread::GetCurrentThread();
		ZETestCheckEqual(Current, NULL);

		Sleep(3000);
	}

	ZETest("set function which call Sleep(ZEUInt Milliseconds)")
	{
		ZEThread Thread1;
		ZEThread Thread2;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction3>());
		Thread1.Run(NULL);

		bool Alive = Thread1.IsAlive(); //false
		ZEThreadStatus Status1 = Thread1.GetStatus(); //none

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction3>());
		Thread2.Run(NULL);

		Alive = Thread2.IsAlive(); //false
		Alive = Thread1.IsAlive(); //true
		ZEThreadStatus Status2 = Thread2.GetStatus(); //none
		Status1 = Thread1.GetStatus(); //running

		Alive = Thread2.IsAlive(); //true
		Alive = Thread1.IsAlive(); //true
		Status2 = Thread2.GetStatus(); //running
		Status1 = Thread1.GetStatus(); //running

		Sleep(3000);
	}

	ZETest("set function which includes ControlPoint(), then call Wait(ZEUInt Miilliseconds) through a thread")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction4>());
		Thread1.Run(NULL);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction4>());
		Thread2.Run(NULL);

		bool Alive = Thread1.IsAlive(); //true
		ZEThreadStatus Status1 = Thread1.GetStatus(); //running
		Alive = Thread2.IsAlive(); //false
		ZEThreadStatus Status2 = Thread2.GetStatus(); //none

		bool Waiting = Thread1.Wait(1000); //false WaitForSingleObject returns 258

		Status1 = Thread1.GetStatus(); //running
		Status2 = Thread2.GetStatus(); //running

		Waiting = Thread2.Wait(1000);

		Status2 = Thread2.GetStatus();
		Status1 = Thread1.GetStatus();

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction4>());
		Thread3.Run(NULL);

		Alive = Thread3.IsAlive(); //false
		ZEThreadStatus Status3 = Thread3.GetStatus(); //none
		Alive = Thread2.IsAlive(); //true
		Status2 = Thread2.GetStatus(); //running
		Alive = Thread1.IsAlive(); //true
		Status1 = Thread1.GetStatus(); //running

		Sleep(1000);
	}

	ZETest("set different functions to different threads")
	{
		ZEThreadStatus Status1;
		ZEThreadStatus Status2;
		ZEThreadStatus Status3;
		bool Alive;
		ZEThread* Current;

		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction4>());
		Thread1.Run(NULL);

		Alive = Thread1.IsAlive(); //false
		Status1 = Thread1.GetStatus(); //none
		Current = ZEThread::GetCurrentThread();
		ZETestCheckEqual(Current, NULL);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
		Thread2.Run(NULL);

		Alive = Thread2.IsAlive(); //false
		Alive = Thread1.IsAlive(); //true
		Status2 = Thread2.GetStatus(); //none
		Status1 = Thread1.GetStatus(); // running
		Current = ZEThread::GetCurrentThread();
		ZETestCheckEqual(Current, NULL);

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction3>());
		Thread3.Run(NULL);

		Alive = Thread3.IsAlive(); //false
		Alive = Thread2.IsAlive(); //true
		Alive = Thread1.IsAlive(); //true
		Status3 = Thread3.GetStatus(); //none
		Status2 = Thread2.GetStatus(); //running
		Status1 = Thread1.GetStatus(); //running
		Current = ZEThread::GetCurrentThread();
		ZETestCheckEqual(Current, NULL);

		Sleep(1000);
	}

	ZETest("set function which calls Exit()")
	{
		ZEThreadStatus Status1;
		ZEThreadStatus Status2;
		ZEThreadStatus Status3;
		bool Alive;
		ZEThread* Current;

		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
		Thread1.Run(NULL);

		Alive = Thread1.IsAlive(); //false
		Status1 = Thread1.GetStatus(); //none
		Current = ZEThread::GetCurrentThread();
		ZETestCheckEqual(Current, NULL);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
		Thread2.Run(NULL);

		Alive = Thread2.IsAlive(); //false
		Alive = Thread1.IsAlive(); //true
		Status2 = Thread2.GetStatus(); //none
		Status1 = Thread1.GetStatus(); //running
		Current = ZEThread::GetCurrentThread();
		ZETestCheckEqual(Current, NULL);

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
		Thread3.Run(NULL);

		Current = ZEThread::GetCurrentThread();
		Alive = Thread3.IsAlive(); //false
		Alive = Thread2.IsAlive(); //false
		Alive = Thread1.IsAlive(); //false
		Status3 = Thread3.GetStatus(); //none
		Status2 = Thread2.GetStatus(); //done
		Status1 = Thread1.GetStatus(); //done
		ZETestCheckEqual(Current, NULL);

		Sleep(1000);
	}

	ZETest("set function which calls Exit() without ControlPoint()")
	{
		ZEThreadStatus Status1;
		ZEThreadStatus Status2;
		ZEThreadStatus Status3;
		bool Alive;
		ZEThread* Current;

		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction2>());
		Thread1.Run(NULL);

		Alive = Thread1.IsAlive(); //false
		Status1 = Thread1.GetStatus(); //none
		Current = ZEThread::GetCurrentThread();
		ZETestCheckEqual(Current, NULL);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction2>());
		Thread2.Run(NULL);

		Alive = Thread2.IsAlive(); //false
		Alive = Thread1.IsAlive(); //false
		Status2 = Thread2.GetStatus(); //none
		Status1 = Thread1.GetStatus(); //done
		Current = ZEThread::GetCurrentThread();

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction2>());
		Thread3.Run(NULL);

		Alive = Thread3.IsAlive(); //false
		Alive = Thread2.IsAlive(); //true
		Alive = Thread1.IsAlive(); //false
		Status3 = Thread3.GetStatus(); //none
		Status2 = Thread2.GetStatus(); //running
		Status1 = Thread1.GetStatus(); //done
		Current = ZEThread::GetCurrentThread();

		Sleep(1000);
	}

	ZETest("bool ZEThread::Wait(ZEUInt Milliseconds)")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;
		ZEThread Thread4;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction4>());
		Thread1.Run(NULL);

		bool Alive = Thread2.IsAlive();
		ZETestCheck(!Alive);

		bool Waiting = Thread2.Wait(1000);
		ZETestCheck(!Waiting);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction4>());
		Thread2.Run(NULL);

		Alive = Thread1.IsAlive(); //true
		ZEThreadStatus Status1 = Thread1.GetStatus(); //running
		Alive = Thread2.IsAlive(); //false
		ZEThreadStatus Status2 = Thread2.GetStatus(); //none

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction4>());
		Thread3.Run(NULL);

		Alive = Thread3.IsAlive(); //false
		ZEThreadStatus Status3 = Thread3.GetStatus(); //none
		Alive = Thread2.IsAlive(); //true
		Status2 = Thread2.GetStatus(); //running
		Alive = Thread1.IsAlive(); //true
		Status1 = Thread1.GetStatus(); //running

		Waiting = Thread2.Wait(1000); //false WaitForSingleObject returns 258

		Thread4.SetName("Thread4");
		Thread4.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction4>());
		Thread4.Run(NULL);

		Sleep(1000);

		ZETestCase("set function with ControlPoint() and Exit(), then call Wait(ZEUInt Milliseconds)")
		{
			ZEThread ThreadA;
			ZEThread ThreadB;
			ZEThread ThreadC;

			ThreadA.SetName("ThreadA");
			ThreadA.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
			ThreadA.Run(NULL);

			ThreadB.SetName("ThreadB");

			Waiting = ThreadB.Wait(1000);
			ZETestCheck(!Waiting);

			ThreadB.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
			ThreadB.Run(NULL);

			Alive = ThreadA.IsAlive(); //true
			ZEThreadStatus StatusA = ThreadA.GetStatus(); //running
			Alive = ThreadB.IsAlive(); //false
			ZEThreadStatus StatusB = ThreadB.GetStatus(); //none

			ThreadC.SetName("ThreadC");

			Waiting = ThreadC.Wait(1000);
			ZETestCheck(!Waiting);
			Waiting = ThreadA.Wait(1000);

			ThreadC.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
			ThreadC.Run(NULL);

			Alive = ThreadC.IsAlive(); //false
			ZEThreadStatus StatusC = ThreadC.GetStatus(); //none
			Alive = ThreadB.IsAlive(); //true
			StatusB = ThreadB.GetStatus(); //running
			Alive = ThreadA.IsAlive(); //true
			StatusA = ThreadA.GetStatus(); //running

			Sleep(1000);
		}
	}

	ZETest("set function with Sleep(ZEUInt Milliseconds), then call Wait(ZEUInt Milliseconds) through a thread")
	{
		ZEThread ThreadA;
		ZEThread ThreadB;
		ZEThread ThreadC;

		ThreadA.SetName("ThreadA");
		ThreadA.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction3>());
		ThreadA.Run(NULL);

		ThreadB.SetName("ThreadB");

		bool Alive = ThreadB.IsAlive();
		ZETestCheck(!Alive);
		bool Waiting = ThreadB.Wait(1000);
		ZETestCheck(!Waiting);

		ThreadB.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction3>());
		ThreadB.Run(NULL);

		Alive = ThreadA.IsAlive(); //true
		ZEThreadStatus StatusA = ThreadA.GetStatus(); //running
		Alive = ThreadB.IsAlive(); //false
		ZEThreadStatus StatusB = ThreadB.GetStatus(); //none

		Waiting = ThreadB.Wait(1000);
		ZETestCheck(!Waiting);

		ThreadC.SetName("ThreadC");
		
		Alive = ThreadC.IsAlive();
		ZETestCheck(!Alive);
		Waiting = ThreadC.Wait(1000);
		ZETestCheck(!Waiting);

		ThreadC.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction3>());
		ThreadC.Run(NULL);

		Alive = ThreadC.IsAlive(); //false
		ZEThreadStatus StatusC = ThreadC.GetStatus(); //none
		Alive = ThreadB.IsAlive(); //true
		StatusB = ThreadB.GetStatus(); //running
		Alive = ThreadA.IsAlive(); //true
		StatusA = ThreadA.GetStatus(); //running

		Sleep(1000);
	}

	ZETest("Wait(ZEIntMilliseconds)")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction4>());
		Thread1.Run(NULL);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction4>());
		Thread2.Run(NULL);

		bool Alive = Thread1.IsAlive(); //true
		ZEThreadStatus Status1 = Thread1.GetStatus(); //running
		Alive = Thread2.IsAlive(); //false
		ZEThreadStatus Status2 = Thread2.GetStatus(); //none

		bool Waiting = Thread1.Wait(3000); //false WaitForSingleObject returns 258
		Waiting = Thread2.Wait(3000);
		ZETestCheck(!Waiting);

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction1>());
		Thread3.Run(NULL);

		Alive = Thread3.IsAlive(); //false
		ZEThreadStatus Status3 = Thread3.GetStatus(); //none
		Alive = Thread2.IsAlive(); //true
		Status2 = Thread2.GetStatus(); //running
		Alive = Thread1.IsAlive(); //true
		Status1 = Thread1.GetStatus(); //running

		Waiting = Thread1.Wait(3000); //false WaitForSingleObject returns 258
		Waiting = Thread2.Wait(3000); //false WaitForSingleObject returns 258
		Waiting = Thread3.Wait(3000); //false

		Sleep(3000);
	}

	ZETest("set function which calls Exit() without ControlPoint(), then call ZEThread::Terminate() through a thread")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction2>());
		Thread1.Run(NULL);

		bool Alive = Thread1.IsAlive(); //false
		ZEThreadStatus Status1 = Thread1.GetStatus(); //none

		Thread1.Terminate();

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction2>());
		Thread2.Run(NULL);

		Alive = Thread2.IsAlive(); //false
		ZEThreadStatus Status2 = Thread2.GetStatus(); //none
		Alive = Thread1.IsAlive(); //true
		Status1 = Thread1.GetStatus(); //running

		Thread1.Terminate();

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&TestFunction2>());
		Thread3.Run(NULL);

		Thread2.Terminate();

		Alive = Thread3.IsAlive(); //false
		ZEThreadStatus Status3 = Thread3.GetStatus(); //none
		Alive = Thread2.IsAlive(); //false
		Status2 = Thread2.GetStatus(); //terminated
		Alive = Thread1.IsAlive(); //false
		Status1 = Thread1.GetStatus(); //terminated

		Sleep(3000);
	}
}
