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

#ifdef ZE_PLATFORM_WINDOWS
    #define WINDOWS_MEAN_AND_LEAN
    #include <windows.h>
#else
    #include <time.h>
#endif

float ZETimeCounter::GetElapsedTime()
{
	return (float)((EndTime - StartTime) * 1000) / (float)Frequency;
}

void ZETimeCounter::Reset()
{
	StartTime = 0;
	EndTime = 0;
	Frequency = 1;
}

void ZETimeCounter::Start()
{
    #ifdef ZE_PLATFORM_WINDOWS
        LARGE_INTEGER Temp;
        QueryPerformanceFrequency(&Temp);
        Frequency = Temp.QuadPart;

        QueryPerformanceCounter(&Temp);
        StartTime = Temp.QuadPart;
    #else
       timespec Temp;
       clock_getres(CLOCK_REALTIME, &Temp);
       Frequency = Temp.tv_nsec;
       clock_gettime(CLOCK_REALTIME, &Temp);
       StartTime = Temp.tv_nsec;
    #endif
}

void ZETimeCounter::Stop()
{
    #ifdef ZE_PLATFORM_WINDOWS
        LARGE_INTEGER Temp;
        QueryPerformanceCounter(&Temp);
        EndTime = Temp.QuadPart;
    #else
        timespec Temp;
        clock_gettime(CLOCK_REALTIME, &Temp);
        EndTime = Temp.tv_nsec;
    #endif
}

ZETimeCounter::ZETimeCounter()
{
	Reset();
}
