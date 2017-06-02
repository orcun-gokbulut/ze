//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETaskPool.h
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

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEList2.h"

class ZETask;
class ZEThread;
class ZETaskThread;

enum ZETaskSchedulePolicy
{
	ZE_TSP_ALWAYS_FIRST,
	ZE_TSP_ROUND_ROBIN
};

class ZETaskPool
{
	friend class ZETaskManager;
	private:
		ZETaskManager*					Manager;
		ZEInt							Id;
		ZEString						Name;
		ZEInt							Priority;
		ZEUInt							ReservedThreadCount;
		ZEUInt							RemainingReservedThreadCount;
		ZETaskSchedulePolicy			SchedulingPolicy;

		ZEUInt							ThreadCount;
		ZEInt							MaxThreadCount;
		
		ZEList2<ZETask>					QueuedTasks;
		ZEList2<ZETask>					RunningTasks;
		ZEList2<ZETask>					InstancingTasks;

	public:
		ZETaskManager*					GetManager() const;

		void							SetId(ZEInt Id);
		ZEInt							GetId() const;

		void							SetName(const ZEString& Name);
		const ZEString&					GetName() const;

		void							SetSchedulingPolicy(ZETaskSchedulePolicy Policy);
		ZETaskSchedulePolicy			GetSchedulingPolicy() const;

		void							SetReservedThreadCount(ZESize Count);
		ZEUInt							GetReservedThreadCount() const;

		void							SetMaxThreadCount(ZEUInt Count);
		ZESize							GetMaxThreadCount() const;

		const ZEList2<ZETask>&			GetRunningTasks() const;
		const ZEList2<ZETask>&			GetWaitingTasks() const;

										ZETaskPool();
										~ZETaskPool();
};
