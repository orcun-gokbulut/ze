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


static void SetThreadName(DWORD ThreadId, LPCSTR ThreadName)
{
	#ifdef ZE_DEBUG_ENABLE
		if (ThreadId == -1)
			return;

		typedef struct tagTHREADNAME_INFO
		{
			DWORD dwType; // must be 0x1000
			LPCSTR szName; // pointer to name (in user addr space)
			DWORD dwThreadID; // thread ID (-1=caller thread)
			DWORD dwFlags; // reserved for future use, must be zero
		} THREADNAME_INFO;

		THREADNAME_INFO Info;
		Info.dwType = 0x1000;
		Info.szName = ThreadName;
		Info.dwThreadID = ThreadId;
		Info.dwFlags = 0;

		__try
		{
			RaiseException(0x406D1388, 0, sizeof(Info) / sizeof(DWORD), (ULONG_PTR*)&Info);
		}
		__except(EXCEPTION_CONTINUE_EXECUTION)
		{

		}
	#endif
}

static __declspec(thread) ZEThread* CurrentThread = NULL;

inline void CloseHandleSafe(void*& Handle)
{
	void* Temp = Handle;
	Handle = NULL;
	CloseHandle(Handle);
}

DWORD WINAPI ZEThread::ThreadFunction(LPVOID Thread)
{
	CurrentThread = (ZEThread*)Thread;
	CurrentThread->Function(CurrentThread, CurrentThread->GetParameter());
	CurrentThread->Exit();
	return 0;
}

ZEInt ZEThread::GetId()
{
	return ThreadId;
}

void ZEThread::SetName(const ZEString& Name)
{
	this->Name = Name;
	SetThreadName(ThreadId, Name);
}

void ZEThread::Run()
{
	if (Handle != NULL)
	{
		ResumeThread(Handle);
		Status = ZE_TS_RUNNING;
		return;
	}
	else
	{
		Handle = CreateThread(NULL, 0, ThreadFunction, this, 0, (LPDWORD)&ThreadId);
		if (Handle == NULL)
			zeCriticalError("Can not create thread.");
		SetThreadName(ThreadId, Name);
		Status = ZE_TS_RUNNING;
	}
}

void ZEThread::Suspend()
{
	if (Handle == NULL)
		return;

	Status = ZE_TS_SUSPENDED;
	SuspendThread(Handle);
}

void ZEThread::Terminate()
{
	if (Handle == NULL)
		return;

	if (CurrentThread == this)
	{
		Exit();
	}
	else
	{
		TerminateThread(Handle, 0);
		Status = ZE_TS_NOT_RUNNING;
		ThreadId = 0;
		CloseHandleSafe(Handle);
	}
}

void ZEThread::Exit()
{
	if (Handle == NULL)
		return;

	if (CurrentThread == this || Status == ZE_TS_SUSPENDED)
	{
		ExitThread(EXIT_SUCCESS);
		Status = ZE_TS_NOT_RUNNING;
		ThreadId = 0;
		CloseHandleSafe(Handle);
	}
	else if (Status == ZE_TS_RUNNING)
	{
		Status = ZE_TS_EXITING;
	}
}

void ZEThread::Wait()
{
	zeCheckError(CurrentThread == this, ZE_VOID, "Cannot call own ZEThread::Wait function. Thread can not wait itself.");

	if (Handle == NULL)
		return;

	DWORD Result = WaitForSingleObject(Handle, INFINITE);
	if (Result != WAIT_OBJECT_0 && Result != WAIT_ABANDONED_0)
		zeCriticalError("Can not wait thread.");
}

ZEThread::ZEThread()
{
	Handle = NULL;
	Status = ZE_TS_NOT_RUNNING;
	Parameter = NULL;
	ThreadId = 0;
}

ZEThread::~ZEThread()
{
	Terminate();
}

ZEThread* ZEThread::GetCurrentThread()
{
	return CurrentThread;
}
