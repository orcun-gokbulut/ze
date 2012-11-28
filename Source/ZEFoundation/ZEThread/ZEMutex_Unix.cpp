//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMutex_Unix.cpp
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

#include "ZEMutex.h"
#include "ZEError.h"

#include <errno.h>

#if defined(ZE_PLATFORM_IOS) || defined(ZE_PLATFORM_ANDROID)
#include <sys/time.h>
int pthread_mutex_timedlock (pthread_mutex_t *mutex, struct timespec *timeout)
{
    struct timeval timenow;
    struct timespec sleepytime;
    int retcode;
    
    sleepytime.tv_sec = 0;
    sleepytime.tv_nsec = 10000000;
    
    while ((retcode = pthread_mutex_trylock (mutex)) == EBUSY)
    {
        gettimeofday (&timenow, NULL);
        if (timenow.tv_sec >= timeout->tv_sec && (timenow.tv_usec * 1000) >= timeout->tv_nsec)
        {
            return ETIMEDOUT;
        }
        
        nanosleep (&sleepytime, NULL);
    }
    
    return retcode;
}
#endif

bool ZEMutex::TryToLock()
{
    return Lock(0);
}

void ZEMutex::Lock()
{
    if (pthread_mutex_lock(&Mutex) == 0)
        Locked = true;
    else
        zeCriticalError("Can not lock mutex.");
}

#ifdef ZE_PLATFORM_MACOSX
int pthread_mutex_timedlock(pthread_mutex_t* Mutex, const struct timespec* TimeOut)
{
    int Result;
    timespec Remaining;
    timespec Elapsed;
    timespec Timestamp;

    Remaining = *TimeOut;
    while ((Result = pthread_mutex_trylock(Mutex)) == EBUSY)
    {
        Timestamp.tv_sec = 0;
        Timestamp.tv_nsec = (Remaining.tv_sec > 0 ? 10000000 : (Remaining.tv_nsec < 10000000 ? Remaining.tv_nsec : 10000000));
        nanosleep(&Timestamp, &Elapsed);
        Timestamp.tv_nsec -= Elapsed.tv_nsec;
        if (Timestamp.tv_nsec <= Remaining.tv_nsec)
        {
            Remaining.tv_nsec -= Timestamp.tv_nsec;
        }
        else
        {
            Remaining.tv_sec--;
            Remaining.tv_nsec = (1000000 - (Timestamp.tv_nsec - Remaining.tv_nsec));
        }
        
        if (Remaining.tv_sec < 0 || (!Remaining.tv_sec && Remaining.tv_nsec <= 0))
        {
            return ETIMEDOUT;
        }
    }
    
    return Result;
}
#endif

bool ZEMutex::Lock(ZEUInt Milliseconds)
{
    if (Milliseconds == 0)
    {
        if (pthread_mutex_trylock(&Mutex) == 0)
        {
            Locked = true;
            return true;
        }
        else
            return false;
    }
    else
    {
        timespec Time;
        Time.tv_sec = Milliseconds / 1000;
        Time.tv_sec = (Milliseconds % 1000) * 1000;

        int Result = pthread_mutex_timedlock(&Mutex, &Time);
        if (Result != 0)
        {
            if (Result == ETIMEDOUT)
                return false;
            else
                zeCriticalError("Can not wait signal.");
        }
    }

    return true;
}

void ZEMutex::Unlock()
{
    if (pthread_mutex_unlock(&Mutex) == 0)
        Locked = false;
    else
        zeCriticalError("Can not unlock mutex.");

}

void ZEMutex::Wait()
{
    Lock();
    Unlock();
}

bool ZEMutex::Wait(ZEUInt Milliseconds)
{
    if (Lock(Milliseconds))
        Unlock();
    else
        return false;
}

ZEMutex::ZEMutex()
{
    Locked = false;
    if (pthread_mutex_init(&Mutex, NULL) != 0)
        zeCriticalError("Can not create mutex.");
}

ZEMutex::ZEMutex(const ZEMutex& Lock)
{
    zeDebugCheckWarning("Trying to copy mutex.");
    Locked = false;
    if (pthread_mutex_init(&Mutex, NULL) != 0)
        zeCriticalError("Can not create mutex.");
}

ZEMutex::~ZEMutex()
{
    pthread_mutex_destroy(&Mutex);
}
