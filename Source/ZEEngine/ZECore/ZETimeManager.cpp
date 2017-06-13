//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETimeManager.cpp
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

#include "ZETimeManager.h"

#include "ZETimer.h"
#include "ZECore.h"
#include "ZEMath\ZEMath.h"

void ZETimeManager::FireTimers(bool PostTick)
{
	ze_for_each(Timer, TimerList)
	{
		if(!Timer->Running)
			continue;

		Timer->RemainingIntervalTime -= Parameters.FrameTimeDelta;

		if (Timer->RemainingIntervalTime > 0.0f)
			continue;

		Timer->OnTime(&Parameters);

		if (Timer->GetRepeating())
			Timer->Reset();
		else
			Timer->Stop();

		if (Timer->Temporary)
			delete Timer.GetPointer();
	}
}

void ZETimeManager::UpdateFrameFixedInterval()
{
	Parameters.Mode = ZE_TM_FIXED_INTERVAL;

	double ElapsedTime = Parameters.CycleTime - LastFrameTime;
	
	if (ElapsedTime < FrameInterval)
	{
		Parameters.FrameType = ZE_TT_INTERMEDIATE;
		Parameters.FrameTime = LastFrameTime;
		Parameters.FrameTimeDelta = ElapsedTime;
		Parameters.FrameTimeInterval = FrameInterval;
	}
	else
	{
		Parameters.FrameId++;

		double RemainingFrameTime = ElapsedTime - FrameInterval;

		if (RemainingFrameTime < FrameInterval)
		{
			Parameters.FrameType = ZE_TT_NORMAL;
			Parameters.FrameTime = LastFrameTime + FrameInterval;
			Parameters.FrameTimeDelta = FrameInterval;
			Parameters.FrameTimeInterval = FrameInterval;
			LastFrameTime = Parameters.CycleTime - ZEMath::Mod(Parameters.CycleTime, FrameInterval);
		}
		else if (RemainingFrameTime > FrameJumpThreshold)
		{
			int JumpedFrameCount = ZEMath::Ceil(ElapsedTime / FrameInterval);
			Parameters.FrameType = ZE_TT_JUMPED;
			Parameters.FrameId += JumpedFrameCount;
			Parameters.FrameTimeDelta = JumpedFrameCount * FrameInterval;
			Parameters.FrameTimeInterval = FrameInterval;
			Parameters.FrameTime = LastFrameTime + Parameters.FrameTimeDelta;
			LastFrameTime = Parameters.CycleTime - ZEMath::Mod(Parameters.CycleTime, FrameInterval);
		}
		else
		{
			Parameters.FrameType = ZE_TT_DROPPED;
			Parameters.FrameTimeDelta = FrameInterval;
			Parameters.FrameTimeInterval = FrameInterval;
			Parameters.FrameTime = LastFrameTime + FrameInterval;
			LastFrameTime += FrameInterval;
		}

		OnTick(&Parameters);

	}
}

void ZETimeManager::UpdateFrameVariableInterval()
{
	Parameters.Mode = ZE_TM_FIXED_INTERVAL;
	Parameters.FrameType = ZE_TT_NORMAL;
	Parameters.FrameId++;
	Parameters.FrameTime = Parameters.CycleTime;
	Parameters.FrameTimeDelta = Parameters.CycleTimeDelta;
	Parameters.FrameTimeInterval = Parameters.CycleTimeDelta;

	OnTick(&Parameters);
	FireTimers(true);
}

void ZETimeManager::UpdateFrameEventBased()
{
	if (!AdvanceFrame)
		return;

	ZETimeParameters Parameters;
	Parameters.Mode = ZE_TM_EVENT_BASED;
	Parameters.FrameType = ZE_TT_NORMAL;
	Parameters.FrameId++;
	Parameters.FrameTime = Parameters.CycleTime;
	Parameters.FrameTimeDelta = Parameters.CycleTimeDelta;

	OnTick(&Parameters);
	FireTimers(true);

	AdvanceFrame = false;
}

bool ZETimeManager::InitializeInternal()
{
	if (!ZEInitializable::InitializeInternal())
		return false;

	Start();

	return true;
}

bool ZETimeManager::DeinitializeInternal()
{
	ze_for_each(Timer, TimerList)
	{
		if (Timer->Temporary)
			delete Timer.GetPointer();
	}

	return ZEInitializable::DeinitializeInternal();
}

ZETimeManager::ZETimeManager()
{
	Mode = ZE_TM_FIXED_INTERVAL;
	FrameInterval = 1.0 / 30.0; // 60 HZ
	FrameJumpThreshold = 0.150;
	Reset();
}

ZETimeManager::~ZETimeManager()
{
	Deinitialize();
}

ZEUInt64 ZETimeManager::GetCycleId() const
{
	return Parameters.CycleId;
}

double ZETimeManager::GetCycleTime() const
{
	return Parameters.CycleTime;
}

double ZETimeManager::GetCycleTimeDelta() const
{
	return Parameters.CycleTimeDelta;
}

ZEFrameType ZETimeManager::GetFrameType() const
{
	return Parameters.FrameType;
}

ZEUInt64 ZETimeManager::GetFrameId() const
{
	return Parameters.FrameId;
}

double ZETimeManager::GetFrameTime() const
{
	return Parameters.FrameTime;
}

double ZETimeManager::GetFrameTimeDelta() const
{
	return Parameters.FrameTimeDelta;
}

const ZETimeParameters* ZETimeManager::GetParameters() const
{
	return &Parameters;
}

void ZETimeManager::SetTickMode(ZETickMode Mode)
{
	Mode = Mode;
}

ZETickMode ZETimeManager::GetTickMode() const
{
	return Mode;
}

void ZETimeManager::SetFrameInterval(float Interval)
{
	FrameInterval = Interval;
}

float ZETimeManager::GetFrameInterval() const
{
	return FrameInterval;
}

void ZETimeManager::SetFrameJumpThreshold(ZEUInt Frames)
{
	FrameJumpThreshold = Frames;
}

ZEUInt ZETimeManager::GetFrameJumpThreshold() const
{
	return FrameJumpThreshold;
}

void ZETimeManager::RegisterTimer(ZETimer* Timer)
{
	zeCheckError(Timer == NULL, ZE_VOID, "Timer is NULL.");
	zeCheckError(Timer->ManagerLink.GetInUse(), ZE_VOID, "Timer is already registered.");

	TimerList.AddEnd(&Timer->ManagerLink);
}

void ZETimeManager::UnregisterTimer(ZETimer* Timer)
{
	zeCheckError(Timer == NULL, ZE_VOID, "Timer is NULL.");
	zeCheckError(!Timer->ManagerLink.GetInUse(), ZE_VOID, "Timer is not registered.");

	TimerList.Remove(&Timer->ManagerLink);
}

void ZETimeManager::PreProcess(const ZETimeParameters* ParametersOld)
{
	if (!Running)
		return;
	
	Parameters.Mode = Mode;
	Parameters.CycleId++;
	Parameters.CycleTime = TimeCounter.GetTimeSeconds();

	if (LastMeasuredTime == 0)
		LastMeasuredTime = Parameters.CycleTime;

	Parameters.CycleTimeDelta = Parameters.CycleTime - LastMeasuredTime;
	LastMeasuredTime = Parameters.CycleTime;

	if (Mode == ZE_TM_FIXED_INTERVAL)
		UpdateFrameFixedInterval();
	else if (Mode == ZE_TM_VARIABLE_INTERVAL)
		UpdateFrameVariableInterval();

	static const ZESize CycleHistorySize = 300;
	static ZETimeParameters CycleHistory[CycleHistorySize];

	CycleHistory[Parameters.CycleId % CycleHistorySize] = Parameters;

	if (Parameters.CycleId % CycleHistorySize == CycleHistorySize - 1)
		for (ZESize I = 0; I < CycleHistorySize; I++)
		{
			const char* FrameType;
			switch (CycleHistory[I].FrameType)
			{
				case ZE_TT_NORMAL:
					FrameType = "Normal";
					break;

				case ZE_TT_DROPPED:
					FrameType = "Dropped";
					break;

				case ZE_TT_INTERMEDIATE:
					FrameType = "Intermediate";
					break;

				case ZE_TT_JUMPED:
					FrameType = "Jumped";
					break;

				default:
					FrameType = "Unknown";
					break;
			}

			zeLog("CID: %I64u, CT: %f, CTD: %f, FT:%s, FID: %I64u, FT: %f, FTD: %f.",
				CycleHistory[I].CycleId,
				CycleHistory[I].CycleTime,
				CycleHistory[I].CycleTimeDelta,
				FrameType,
				CycleHistory[I].FrameId,
				CycleHistory[I].FrameTime,
				CycleHistory[I].FrameTimeDelta);
		}
}

void ZETimeManager::Process(const ZETimeParameters* ParametersOld)
{
	if (Parameters.FrameType == ZE_TT_INTERMEDIATE)
		return;

	FireTimers(false);
}

void ZETimeManager::PostProcess(const ZETimeParameters* ParametersOld)
{
	if (Parameters.FrameType == ZE_TT_INTERMEDIATE)
		return;

	FireTimers(true);
}

void ZETimeManager::Start()
{
	if (Running)
		return;

	TimeCounter.Start();
	Running = true;
}

void ZETimeManager::Pause()
{
	Running = false;
	TimeCounter.Pause();
}

void ZETimeManager::Reset()
{
	TimeCounter.Reset();

	Parameters.FrameType = ZE_TT_INTERMEDIATE;
	Parameters.CycleId = 0;
	Parameters.CycleTime = 0;
	Parameters.CycleTimeDelta = 0.0f;
	Parameters.FrameId = 0;
	Parameters.FrameTime = 0;
	Parameters.FrameTimeDelta = 0.0;
	
	LastMeasuredTime = Parameters.FrameTime;
	Running = false;
	AdvanceFrame = false;
}

void ZETimeManager::AdvanceFrameEvent()
{
	AdvanceFrame = true;
}

ZETimeManager* ZETimeManager::GetInstance()
{
	return ZECore::GetInstance()->GetTimeManager();
}

ZETimeManager* ZETimeManager::CreateInstance()
{
	return new ZETimeManager();
}
