//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEThread_Unix.cpp
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

#ifdef ZE_PLATFORM_LINUX
    #include <sys/prctl.h>
#endif

static ZEThread* CurrentThread = NULL;

void* ZEThread::ThreadFunction(void* Thread)
{
    CurrentThread = (ZEThread*)Thread;

    #ifdef ZE_PLATFORM_LINUX
        prctl(PR_SET_NAME, CurrentThread->GetName().ToCString(), 0, 0, 0);
    #endif

	CurrentThread->Status = ZE_TS_RUNNING;
    CurrentThread->Function(CurrentThread, CurrentThread->GetParameter());
    CurrentThread->Exit();

	return 0;
}

void ZEThread::Run(void* Parameter)
{
    if (IsAlive())
		return;

	int Result = pthread_create(&Thread, NULL, ThreadFunction, this);
    if (Result != 0)
        zeCriticalError("Can not create thread.");
}

void ZEThread::Terminate()
{
    if (!IsAlive())
		return;

	int Result = pthread_cancel(Thread);
    if (Result != 0)
		zeCriticalError("Can not terminate thread.");

    Status = ZE_TS_TERMINATED;
}

void ZEThread::Sleep(ZEUInt Milliseconds)
{
    zeDebugCheck(CurrentThread != this, "A thread can only use it's own sleep function.");
    usleep(Milliseconds);
}


void ZEThread::Wait()
{
    zeDebugCheck(CurrentThread == this, "You can not wait your own thread.");

    if (!IsAlive())
		return;
    
	pthread_join(Thread, NULL);
}

bool ZEThread::Wait(ZEUInt Milliseconds)
{
    #ifndef ZE_PLATFORM_LINUX
        zeCriticalError("Unix does not support ZEThread::Wait(ZEUInt Milliseconds) function.");
    #else
        zeDebugCheck(CurrentThread == this, "A thread can not wait its own.");

        if (!IsAlive())
            return false;
    
        if (Milliseconds == 0)
            return pthread_tryjoin_np(Thread, NULL) == 0;
        else
        {
            timespec Time;
            Time.tv_sec = Milliseconds / 1000;
            Time.tv_sec = (Milliseconds % 1000) * 1000;

            return pthread_timedjoin_np(Thread, NULL, &Time) == 0;
        }
    #endif
    return true;
}

bool ZEThread::ControlPoint()
{
    zeDebugCheck(CurrentThread != this, "A thread can only use it's own control point function.");

    if (Status == ZE_TS_RUNNING)
        return true;

	return false;
}

void ZEThread::Exit()
{
    if (!IsAlive())
		return;

	if (CurrentThread == this)
    {
        CurrentThread->Status = ZE_TS_DONE;
        pthread_exit(NULL);
    }
    else
        Status == ZE_TS_EXITING;
}

ZEThread::ZEThread()
{
	Status = ZE_TS_NONE;
}

ZEThread::~ZEThread()
{
    Exit();
}

ZEThread* ZEThread::GetCurrentThread()
{
    return CurrentThread;
}
