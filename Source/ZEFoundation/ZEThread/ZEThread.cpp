//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEThread.cpp
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

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static DWORD WINAPI RunThread(LPVOID Thread)
{
	ZEThread* CurrentThread = (ZEThread*)Thread;

	CurrentThread->Status = ZE_TS_RUNNING;
	CurrentThread->Function(CurrentThread->GetParameter());
	CurrentThread->Status = ZE_TS_DONE;

	return 0;
}

ZEThreadStatus ZEThread::GetStatus()
{
	return Status;
}

void ZEThread::SetParameter(void* Parameter)
{
	this->Parameter = Parameter;
}

void* ZEThread::GetParameter()
{
	return Parameter;
}

void ZEThread::Run(void* Parameter)
{
	if (Handle == NULL)
	{
		Handle = CreateThread(NULL, 0,  RunThread, this, CREATE_SUSPENDED, NULL);
		if (Handle == NULL)
		{
			//zeCriticalError("Can not create thread.");
			return;
		}
	}

	if (Status == ZE_TS_RUNNING)
		return;

	DWORD Result = ResumeThread(Handle);
	if (Result == -1)
	{
		//zeCriticalError("Can not resume thread.");
		return;
	}
}

void ZEThread::Suspend()
{
	if (Handle != NULL)
		return;
	
	DWORD Result = SuspendThread(Handle);
	if (Result == -1)
	{
		//zeCriticalError("Can not suspend thread.");
		return;
	}

	Status = ZE_TS_SUSPENDED;
}

void ZEThread::Terminate()
{
	if (Handle == NULL)
		return;

	DWORD Result = TerminateThread(Handle, 0);
	if (Result == -1)
	{
		//zeCriticalError("Can not terminate thread.");
		return;
	}

	Status = ZE_TS_TERMINATED;
}

ZEThread::ZEThread()
{
	Handle = NULL;
	Status = ZE_TS_NONE;
}

ZEThread::~ZEThread()
{
	if (Handle != NULL)
	{
		Terminate();
		CloseHandle(Handle);
	}
}
