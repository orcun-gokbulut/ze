//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERealTimeClock.cpp
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

#include "ZERealTimeClock.h"

#include "ZECore.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifdef GetCurrentTime
#undef GetCurrentTime
#endif


ZERealTimeClock::ZERealTimeClock()
{
	ResetTime();
}

ZERealTimeClock::~ZERealTimeClock()
{

}

ZEUInt64 ZERealTimeClock::GetCurrentTime()
{
	LARGE_INTEGER CurrentTick;
	QueryPerformanceCounter(&CurrentTick);
	return ((FrameTick - StartTick) * 1000000) / Frequency;
}

ZEUInt64 ZERealTimeClock::GetFrameTime()
{
	return ((FrameTick - StartTick) * 1000000) / Frequency;
}

ZEUInt64 ZERealTimeClock::GetFrameDeltaTime()
{
	return ((FrameTick - OldFrameTick) * 1000000) / Frequency;
}

void ZERealTimeClock::ResetFrameTime()
{
	LARGE_INTEGER CurrentTick;
	QueryPerformanceCounter(&CurrentTick);
	FrameTick = CurrentTick.QuadPart;
	OldFrameTick = CurrentTick.QuadPart;
}

void ZERealTimeClock::ResetTime()
{
	LARGE_INTEGER Freq;
	QueryPerformanceFrequency(&Freq);
	Frequency = Freq.QuadPart;

	LARGE_INTEGER CurrentTick;
	QueryPerformanceCounter(&CurrentTick);

	OldFrameTick = CurrentTick.QuadPart;
	FrameTick = CurrentTick.QuadPart;
	StartTick = CurrentTick.QuadPart;
}

void ZERealTimeClock::UpdateFrameTime()
{
	LARGE_INTEGER Freq;
	QueryPerformanceFrequency(&Freq);
	Frequency = Freq.QuadPart;

	LARGE_INTEGER CurrentTick;
	QueryPerformanceCounter(&CurrentTick);
	OldFrameTick = FrameTick;
	FrameTick = CurrentTick.QuadPart;
}

ZERealTimeClock* ZERealTimeClock::GetInstance()
{
	return ZECore::GetInstance()->GetRealTimeClock();
}
