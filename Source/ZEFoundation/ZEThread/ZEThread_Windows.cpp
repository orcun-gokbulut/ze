//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEThread_Windows.cpp
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

#include "ZEThread.h"
#include "ZEError.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static __declspec(thread) ZEThread* CurrentThread = NULL;

DWORD WINAPI ZEThread::ThreadFunction(LPVOID Thread)
{
	CurrentThread = (ZEThread*)Thread;
	CurrentThread->Status = ZE_TS_RUNNING;
	CurrentThread->Function(CurrentThread, CurrentThread->GetParameter());
	CurrentThread->Exit();
	return 0;
}

void ZEThread::Run()
{
	if (Status == ZE_TS_SUSPENDED)
	{
		ResumeThread(Handle);
		Status = ZE_TS_RUNNING;
		return;
	}
	else
	{
		if (IsAlive())
			return;

		Status = ZE_TS_STARING;
		Handle = CreateThread(NULL, 0, ThreadFunction, this, 0, NULL);
		if (Handle == NULL)
			zeCriticalError("Can not create thread.");
	}
}

void ZEThread::Terminate()
{
	if (!IsAlive())
		return;

	if (CurrentThread == this)
	{
		Exit();
		return;
	}
	else
	{
		DWORD Result = TerminateThread(Handle, 0);
		if (Result == -1)
			zeCriticalError("Can not terminate thread.");
	}

	Status = ZE_TS_TERMINATED;
}

void ZEThread::Suspend()
{
	if (IsAlive() || Status == ZE_TS_SUSPENDED)
		return;
	
	Status = ZE_TS_SUSPENDED;
	SuspendThread(Handle);
}

void ZEThread::Wait()
{
	zeCheckError(CurrentThread != this, ZE_VOID, "Cannot call own ZEThread::Wait function. Thread can not wait itself.");

	if (!IsAlive())
		return;

	if (Status == ZE_TS_TERMINATED)
		return;

	DWORD Result = WaitForSingleObject(Handle, INFINITE);
	if (Result != WAIT_OBJECT_0 && Result != WAIT_ABANDONED_0)
		zeCriticalError("Can not wait thread.");
}

bool ZEThread::Wait(ZEUInt Milliseconds)
{
	zeCheckError(CurrentThread != this, false, "Cannot call own ZEThread::Wait function. Thread can not wait itself.");

	if (!IsAlive())
		return false;

	DWORD Result = WaitForSingleObject(Handle, Milliseconds);
	if (Result != WAIT_OBJECT_0)
		return false;

	return true;
}

bool ZEThread::ControlPoint()
{
	zeCheckError(CurrentThread != this, false, "Cannot call another thread's ZEThread::ControlPoint function. Only owner thread can call it's own ControlPoint function.");

	if (Status == ZE_TS_RUNNING)
	{
		return true;
	}
	else if (Status == ZE_TS_SUSPENDED)
	{
		Suspend();
		return ControlPoint();
	}

	return false;
}

void ZEThread::Exit()
{
	if (!IsAlive())
		return;

	if (CurrentThread == this)
	{
		Status = ZE_TS_EXITING;
		ExitThread(EXIT_SUCCESS);
		CurrentThread->Status = ZE_TS_DONE;
	}
	else
	{
		Status = ZE_TS_EXITING;
	}
}

ZEThread::ZEThread()
{
	Handle = NULL;
	Status = ZE_TS_NONE;
}

ZEThread::~ZEThread()
{
	Exit();

	if (Handle != NULL)
	{
		if (!CloseHandle(Handle))
			zeCriticalError("Cannot close handle of the thread.");
		Handle = NULL;
	}
}

ZEThread* ZEThread::GetCurrentThread()
{
	return CurrentThread;
}
