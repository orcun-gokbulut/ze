//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETimeCounter.cpp
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

#include "ZETimeCounter.h"

#include "ZEPlatform.h"

#ifdef ZE_PLATFORM_WINDOWS
    #define WINDOWS_MEAN_AND_LEAN
    #include <windows.h>
#elif defined(ZE_PLATFORM_DARWIN)
    #include <mach/clock.h>
    #include <mach/mach.h>
    #include <mach/mach_time.h>
#elif defined(ZE_PLATFORM_UNIX)
    #include <time.h>
#endif

ZEUInt64 ZETimeCounter::Frequency = 0;

static inline ZEUInt64 GetClock()
{
    #ifdef ZE_PLATFORM_WINDOWS
        LARGE_INTEGER Temp;
        QueryPerformanceCounter(&Temp);
        return Temp.QuadPart;
    #elif defined(ZE_PLATFORM_DARWIN)
        clock_serv_t Clock;
        mach_timespec_t TimeSpec;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &Clock);
        clock_get_time(Clock, &TimeSpec);
        mach_port_deallocate(mach_task_self(), Clock);
        return (ZEUInt64)TimeSpec.tv_sec * (ZEUInt64)1000000000 + (ZEUInt64)TimeSpec.tv_nsec;
    #elif ZE_PLATFORM_UNIX
        timespec Temp;
        clock_gettime(CLOCK_REALTIME, &Temp);
        return (ZEUInt64)Temp.tv_sec * (ZEUInt64)1000000000 + (ZEUInt64)Temp.tv_nsec;
    #endif
}

static inline ZEUInt64 GetFreq()
{
    #ifdef ZE_PLATFORM_WINDOWS
        LARGE_INTEGER Temp;
        QueryPerformanceFrequency(&Temp);
        return Temp.QuadPart;
    #elif defined(ZE_PLATFORM_DARWIN)
        mach_timebase_info_data_t Info;
        mach_timebase_info(&Info);
        return (ZEUInt64)((double) Info.numer / (double) Info.denom);
    #elif ZE_PLATFORM_UNIX
        timespec Temp;
        clock_getres(CLOCK_REALTIME, &Temp);
        return (ZEUInt64)1000000000 / (ZEUInt64)Temp.tv_nsec;
    #endif
}

void ZETimeCounter::UpdateAcumulatedTime() const
{
	ZEUInt64 CurrentTime = GetClock();
	AcumulatedTime += CurrentTime - LastMeasuredTime;
	LastMeasuredTime = CurrentTime;
}

ZETimeCounterState ZETimeCounter::GetState() const
{
	return State;
}

double ZETimeCounter::GetTimeSeconds() const
{
	if (State == ZE_TCS_RUNNING)
		UpdateAcumulatedTime();

	return (double)AcumulatedTime / (double)Frequency;
}

double ZETimeCounter::GetTimeMilliseconds() const
{
	if (State == ZE_TCS_RUNNING)
		UpdateAcumulatedTime();

	return ((double)AcumulatedTime * 1000.0) / (double)Frequency;
}

ZEUInt64 ZETimeCounter::GetTimeMicroseconds() const
{
    if (State == ZE_TCS_RUNNING)
        UpdateAcumulatedTime();

	return (AcumulatedTime * 1000000ull) / Frequency;
}

ZEUInt64 ZETimeCounter::GetTimeNanoSeconds() const
{
	if (State == ZE_TCS_RUNNING)
		UpdateAcumulatedTime();

	return (AcumulatedTime * 1000ull) / Frequency;
}

void ZETimeCounter::Start()
{
    if (State == ZE_TCS_RUNNING)
		return;
	else if (State == ZE_TCS_STOPPED)
		Reset();

	State = ZE_TCS_RUNNING;
	LastMeasuredTime = GetClock();
}

void ZETimeCounter::Pause()
{
	if (State != ZE_TCS_RUNNING)
		return;

	UpdateAcumulatedTime();
	State = ZE_TCS_PAUSED;
}

void ZETimeCounter::Stop()
{
	UpdateAcumulatedTime();
	State = ZE_TCS_STOPPED;
}

void ZETimeCounter::Reset()
{
	State = ZE_TCS_STOPPED;
	LastMeasuredTime = 0;
	AcumulatedTime = 0;
}

ZETimeCounter::ZETimeCounter()
{
	if (Frequency == 0)
		Frequency = GetFreq();

	Reset();
}
