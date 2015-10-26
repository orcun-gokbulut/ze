//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETask.h
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

#include "ZETypes.h"
#include "ZEThread\ZELock.h"
#include "ZEDS\ZEList2.h"
#include "ZEDS\ZEDelegate.h"
#include "ZEDS\ZEArray.h"

enum  ZETaskPool
{
	ZE_TP_REAL_TIME,
	ZE_TP_COMPUTE,
	ZE_TP_IO
};

enum ZETaskStatus
{
	ZE_TS_NONE,
	ZE_TS_RUNNING,
	ZE_TS_WAITING,
	ZE_TS_DONE
};
/*
typedef ZEDelegate<void (ZETask* Task, void* ExtraParameters)> ZETaskFunction;

class ZEThread;

class ZETask
{
	friend class ZETaskManager;
	private:
		ZETaskManager*		TaskManager;
		ZEString			Name;
		ZETaskStatus		Status;
		ZEThread*			Thread;
		ZEInt				Priority;
		ZELink<ZETask>		Link;
		ZELink<ZETask>		QueueLink;
		
	public:
		void				SetName(const ZEString& Name);
		const ZEString		GetName();

		void				SetPool(ZETaskPool Pool);
		ZETaskPool			GetPool();

		void				SetPriority(ZEInt Priority);
		ZEInt				GetPriority();

		void				Cooperate();
		void				End();

							ZETask();
							~ZETask();
};


class ZETaskManager : public ZEInitializable
{
	private:
		ZEList2<ZETask>		Tasks;
		ZEList2<ZETask>		Queues[3];	
		ZEArray<ZEThread*>	Threads;

		void				Schedule();

	public:
		void				SetThreadCount(ZETaskPool Pool, ZESize ThreadCount);
		ZESize				GetThreadCount(ZETaskPool Pool);
		ZESize				GetTotalThreadCount();

		ZESize				GetTotalSystemThreadCount();
		ZESize				GetTotalSystemCoreCount();

		void				RegisterTask(ZETask* Task);
};

// Core Count = n
// Realtime Thread Count = n - 1
// Compute Thread Count = 1
// IO Thread Count = 2

// Run Task 
// Find Next Task
// Cooperate
// End Task - Finished

*/
