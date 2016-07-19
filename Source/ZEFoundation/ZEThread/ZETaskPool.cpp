//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETaskPool.cpp
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

#include "ZETaskPool.h"

#include "ZETask.h"
#include "ZETaskThread.h"
#include "ZEError.h"
#include "ZEDS\ZEFormat.h"

ZETaskThread* ZETaskPool::RequestThread()
{
	ZETaskThread* Thread = NULL;

	ZELink<ZETaskThread>* ThreadLink = SuspendedThreads.GetFirst();
	if (ThreadLink == NULL)
	{
		 if (ThreadCount > SuspendedThreads.GetCount() + ActiveThreads.GetCount())
		 {
			Thread = new ZETaskThread();
			Thread->SetFunction(ZEThreadFunction::Create<ZETaskPool, &ZETaskPool::Schedule>(this));
			ActiveThreads.AddEnd(&Thread->PoolLink);
		 }
	}
	else
	{
		Thread = ThreadLink->GetItem();
		SuspendedThreads.Remove(ThreadLink);
		ActiveThreads.AddEnd(ThreadLink);
	}
	
	return Thread;
}

void ZETaskPool::ReleaseThread(ZETaskThread* Thread)
{
	if (ThreadCount > SuspendedThreads.GetCount() + ActiveThreads.GetCount())
	{
		ActiveThreads.Remove(&Thread->PoolLink);
		DeletedThreads.AddEnd(&Thread->PoolLink);
	}
	else
	{
		ActiveThreads.Remove(&Thread->PoolLink);
		SuspendedThreads.AddEnd(&Thread->PoolLink);
	}
}

void ZETaskPool::PurgeThreads()
{
	ZELink<ZETaskThread>* Thread;
	while(Thread = DeletedThreads.GetFirst())
	{
		DeletedThreads.Remove(Thread);
		delete Thread->GetItem();
	}
}

void ZETaskPool::Schedule(ZEThread* Thread, void* ExtraParameter)
{
	ZETaskThread* TaskThread = static_cast<ZETaskThread*>(Thread);

	while(Thread->ControlPoint())
	{
		ZETask* Task = NULL;
		SchedulerLock.Lock();
		{
			TaskThread->TaskDestroyed = false;
			if (Tasks.GetFirst() != NULL)
			{
				Task = Tasks.GetFirst()->GetItem();
				Tasks.Remove(&Task->Link);
			}		
			else
			{
				ReleaseThread(TaskThread);
				SchedulerLock.Unlock();
				Thread->Suspend();
				continue;
			}
		}
		
		static_cast<ZETaskThread*>(Thread)->SetTask(Task);
		Task->Status = ZE_TS2_RUNNING;
		SchedulerLock.Unlock();

		ZETaskResult Result = ZE_TR_DONE;
		if (!Task->Function.IsNull())
			Result = Task->Function(static_cast<ZETaskThread*>(Thread), Task->GetParameter());

		if (TaskThread->TaskDestroyed)
			continue;

		SchedulerLock.Lock();
		{
			if (Result == ZE_TR_DONE)
			{
				Task->Status = ZE_TS2_DONE;
			}
			else if (Result == ZE_TR_COOPERATING)
			{
				Task->Status = ZE_TS2_WAITING;
				Reschedule(Task);
			}
			else
			{
				Task->Status = ZE_TS2_FAILED;
			}				
		}
		SchedulerLock.Unlock();
	}
}

void ZETaskPool::Reschedule(ZETask* Task)
{
	ZEInt TaskEffectivePriority = Task->GetPriority();
	ze_for_each_reverse(CurrentTask, Tasks)
	{
		if (TaskEffectivePriority > CurrentTask->GetPriority())
		{
			Tasks.InsertBefore(&CurrentTask->Link, &Task->Link);
			return;
		}
	}

	Tasks.AddEnd(&Task->Link);
}

void ZETaskPool::RunTask(ZETask* Task)
{
	SchedulerLock.Lock();
	if (Task->Status == ZE_TS2_RUNNING)
	{
		SchedulerLock.Unlock();
		return;
	}

	PurgeThreads();
	Reschedule(Task);

	ZETaskThread* Thread = RequestThread();
	SchedulerLock.Unlock();

	if (Thread != NULL)
		Thread->Run();
}

void ZETaskPool::TaskDestroyed(ZETask* Task)
{
	SchedulerLock.Lock();
	if (Task->GetStatus() != ZE_TS2_RUNNING)
		Tasks.Remove(&Task->Link);
	
	ze_for_each(Thread, ActiveThreads)
	{
		if (Thread->Task == Task)
			Thread->TaskDestroyed = true;
	}

	SchedulerLock.Unlock();
}

void ZETaskPool::SetId(ZEInt Id)
{
	this->Id = Id;
}

ZEInt ZETaskPool::GetId()
{
	return Id;
}

void ZETaskPool::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZETaskPool::GetName()
{
	return Name;
}

const ZEList2<ZETask>& ZETaskPool::GetTasks()
{
	return Tasks;
}

void ZETaskPool::SetThreadCount(ZEUInt Count)
{
	ThreadCount = Count;
}

ZEUInt ZETaskPool::GetThreadCount()
{
	return (ZEUInt)ThreadCount;
}

ZETaskPool::ZETaskPool()
{
	Id = 0;
}

ZETaskPool::~ZETaskPool()
{
	SetThreadCount(0);
}
