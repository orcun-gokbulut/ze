//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEProfiler.cpp
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

#include "ZEProfiler.h"
#include "ZEProfilerManager.h"

#include <time.h>
#include <Windows.h>

ZEProfiler::ZEProfiler(void)
{
	FrameCount = 0;
	TotalCount = 0;
	FrameMaximumTime = -1;
	TotalMaximumTime = -1;
	FrameMinimumTime = -1;
	TotalMinimumTime = -1;
	TotalTime = 0;
	FrameTotalTime = 0;
	StartTime = 0;
	EndTime = 0;
	ProfilerActive = false;
}

ZEProfiler::~ZEProfiler(void)
{
}

void ZEProfiler::SetName(const ZEString& Name)
{
	ZEProfiler::Name.SetValue(Name);
}

const ZEString& ZEProfiler::GetName()
{
	return ZEProfiler::Name;
}

void ZEProfiler::Start()
{
	ProfilerActive = true;
	StartTime = 0;
	EndTime = 0;
	StartTime = GetTickCount64();
}

void ZEProfiler::Stop()
{
	EndTime = GetTickCount64();
	if(Manager->GetInstance()->Stack.GetLastItem()->Name == ZEProfiler::Name)
	{
		PassedTime = EndTime - StartTime;

		if(FrameMaximumTime < 0)
			FrameMaximumTime = PassedTime;
		else if(PassedTime > FrameMaximumTime)
			FrameMaximumTime = PassedTime;

		if(FrameMinimumTime < 0)
			FrameMinimumTime = PassedTime;
		else if(FrameMinimumTime > PassedTime)
			FrameMinimumTime = PassedTime;

		if(TotalMaximumTime < 0)
			TotalMaximumTime = PassedTime;
		else if(TotalMaximumTime < PassedTime)
			TotalMaximumTime = PassedTime;

		if(TotalMinimumTime < 0)
			TotalMinimumTime = PassedTime;
		else if(TotalMinimumTime > PassedTime)
			TotalMinimumTime = PassedTime;

		FrameTotalTime += PassedTime;
		TotalTime += PassedTime;
		TotalCount++;
		FrameCount++;

		ProfilerActive = false;
		StartTime = 0;
		EndTime = 0;
		Manager->GetInstance()->Stack.DeleteAt(Manager->GetInstance()->Stack.GetSize()-1);
	}
	
}

ZEINT64 ZEProfiler::GetFrameCount()
{
	return FrameCount;
}

ZEINT64 ZEProfiler::GetTotalCount()
{
	return TotalCount;
}

ZEINT64 ZEProfiler::GetFrameMaximumTime()
{
	return FrameMaximumTime;
}

ZEINT64 ZEProfiler::GetTotalMaximumTime()
{
	return TotalMaximumTime;
}

ZEINT64 ZEProfiler::GetFrameMinimumTime()
{
	return FrameMinimumTime;
}

ZEINT64 ZEProfiler::GetTotalMinimumTime()
{
	return TotalMinimumTime;
}

ZEINT64 ZEProfiler::GetTotalTime()
{
	return TotalTime;
}

ZEINT64 ZEProfiler::GetFrameTotalTime()
{
	return FrameTotalTime;
}

ZEINT64 ZEProfiler::GetTotalAverageTime()
{
	if(TotalCount == 0)
		return 0;
	return (int)(TotalTime / TotalCount);
}

ZEINT64 ZEProfiler::GetFrameAverageTime()
{
	if(FrameCount == 0)
		return 0;
	return (int)(FrameTotalTime / FrameCount);
}

void ZEProfiler::ResetFrame()
{
	FrameCount = 0;
	FrameMaximumTime = -1;
	FrameMinimumTime = -1;
	FrameTotalTime = 0;
	StartTime = 0;
	EndTime = 0;
}

void ZEProfiler::ResetTotal()
{
	FrameCount = 0;
	TotalCount = 0;
	FrameMaximumTime = -1;
	TotalMaximumTime = -1;
	FrameMinimumTime = -1;
	TotalMinimumTime = -1;
	TotalTime = 0;
	FrameTotalTime = 0;
	StartTime = 0;
	EndTime = 0;
}

ZEProfiler* ZEProfiler::GetParent()
{
	return ParentProfiler;
}

void ZEProfiler::SetParent(ZEProfiler *ParentProfiler)
{
	ZEProfiler::ParentProfiler = ParentProfiler;
}

void ZEProfiler::SetManager(ZEProfilerManager *Manager)
{
	ZEProfiler::Manager = Manager;
}
