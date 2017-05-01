//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETimeManager.h
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

#pragma once

#include "ZEModule.h"

#include "ZEDS/ZEList2.h"
#include "ZEMeta/ZEEvent.h"

#include "ZETimeCounter.h"
#include "ZETimeParameters.h"

class ZETimer;

class ZE_EXPORT_ZEENGINE ZETimeManager : public ZEModule
{
	ZE_OBJECT
	friend class ZETimer;
	private:
		ZETimeParameters						Parameters;
		ZETickMode								Mode;
		ZEList2<ZETimer>						TimerList;
		ZETimeCounter							TimeCounter;
		float									FrameTimeInterval;
		double									FrameTimeRemainder;
		double									LastMeasuredTime;
		bool									Running;
		bool									AdvanceFrame;

		void									FireTimers(bool PostTick);

		void									UpdateFrameFixedInterval();
		void									UpdateFrameVariableInterval();
		void									UpdateFrameEventBased();

		virtual bool							InitializeInternal() override;
		virtual bool							DeinitializeInternal() override;

												ZETimeManager();
		virtual									~ZETimeManager() override;
	
	public:
		ZEUInt64								GetCycleId() const;
		double									GetCycleTime() const;
		double									GetCycleTimeDelta() const;

		ZEFrameType								GetFrameType() const;
		ZEUInt64								GetFrameId() const;
		double									GetFrameTime() const;
		double									GetFrameTimeDelta() const;

		const ZETimeParameters*					GetParameters() const;

		void									SetTickMode(ZETickMode Mode);
		ZETickMode								GetTickMode() const;

		void									SetFrameTimeInterval(float Interval);
		float									GetFrameTimeInterval() const;
			
		const ZEList2<ZETimer>					GetTimers() const;
		void									RegisterTimer(ZETimer* Timer);
		void									UnregisterTimer(ZETimer* Timer);

		void									Start();
		void									Pause();
		void									Reset();

		virtual void							PreProcess(const ZETimeParameters* Parameters) override;
		virtual void							Process(const ZETimeParameters* Parameters) override;
		virtual void							PostProcess(const ZETimeParameters* Parameters) override;

		ZE_EVENT(								OnTick,(const ZETimeParameters* Parameters));

		void									AdvanceFrameEvent();

		static ZETimeManager*					GetInstance();
		static ZETimeManager*					CreateInstance();
};
