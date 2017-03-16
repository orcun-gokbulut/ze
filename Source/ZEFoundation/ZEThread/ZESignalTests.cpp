//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESignalTests.cpp
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
#include "ZESignal.h"
#include "ZEThread.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define WINAPI__stdcall

static ZESignal Signal;

void SignalFunction1(ZEThread* Thread, void* Parameter)
{
	while(Thread->ControlPoint())
	{
		printf("\nName: %s, SignalFunction1\n", Thread->GetName());
		Thread->Exit();
	}
	Sleep(1000);
}

void SignalFunction2(ZEThread* Thread, void* Parameter)
{
	Signal.Wait();

	while(Thread->ControlPoint())
	{
		printf("\nName: %s, SignalFunction2\n", Thread->GetName());
		Thread->Exit();
	}
	Sleep(1000);
}

void SignalFunction3(ZEThread* Thread, void* Parameter)
{
	bool Waiting = Signal.Wait(3000);

	while(Thread->ControlPoint())
	{
		printf("\nName: %s, SignalFunction3\n", Thread->GetName());
		Thread->Exit();
	}
	Sleep(1000);
}

void SignalFunction4(ZEThread* Thread, void* Parameter)
{
	Signal.Signal();

	while(Thread->ControlPoint())
	{
		printf("\nName: %s, SignalFunction4\n", Thread->GetName());
		Thread->Exit();
	}
	Sleep(1000);
}

ZE_TEST(ZESignal)
{
	ZE_TEST_ITEM("ZESignal::ZESignal()")
	{
		ZESignal TestSignal;
	}

	ZE_TEST_ITEM("ZESignal::ZESignal(const ZESignal& Other)")
	{
		ZESignal SignalA;

		ZESignal SignalB(SignalA);
	}

	ZE_TEST_ITEM("ZESignal ZESignal::operator=(const ZESignal& Other)")
	{
		ZESignal SignalA;
		ZESignal SignalB;

		SignalA = SignalB;
	}

	ZE_TEST_ITEM("run thread which calls ZESignal::Wait()")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&SignalFunction2>());
		Thread1.Run(NULL);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&SignalFunction1>());
		Thread2.Run(NULL);

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&SignalFunction1>());
		Thread3.Run(NULL);

		Sleep(1000);
		Signal.Signal();
	}

	ZE_TEST_ITEM("run threads which call ZESignal::Wait(), and the last one calls ZESignal::Signal()")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&SignalFunction2>());
		Thread1.Run(NULL);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&SignalFunction2>());
		Thread2.Run(NULL);

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&SignalFunction4>());
		Thread3.Run(NULL);

		Sleep(1000);
		Signal.Signal();
	}

	ZE_TEST_ITEM("run threads which call ZESignal::Wait()")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&SignalFunction2>());
		Thread1.Run(NULL);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&SignalFunction2>());
		Thread2.Run(NULL);

		Signal.Signal();

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&SignalFunction2>());
		Thread3.Run(NULL);

		Signal.Signal();
		Sleep(1000);
		Signal.Signal();
		Sleep(1000);
	}

	ZE_TEST_ITEM("run thread which calls ZESignal::Wait(ZEUInt Milliseconds)")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&SignalFunction3>());
		Thread1.Run(NULL);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&SignalFunction1>());
		Thread2.Run(NULL);

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&SignalFunction1>());
		Thread3.Run(NULL);

		Sleep(3000);
	}

	ZE_TEST_ITEM("run threads which call ZESignal::Wait(ZEUInt Milliseconds)")
	{
		ZEThread Thread1;
		ZEThread Thread2;
		ZEThread Thread3;

		Thread1.SetName("Thread1");
		Thread1.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&SignalFunction3>());
		Thread1.Run(NULL);

		Thread2.SetName("Thread2");
		Thread2.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&SignalFunction3>());
		Thread2.Run(NULL);

		Thread3.SetName("Thread3");
		Thread3.SetFunction(ZEDelegate<void (ZEThread*, void*)>::Create<&SignalFunction3>());
		Thread3.Run(NULL);

		Sleep(5000);
	}
}
