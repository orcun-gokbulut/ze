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
#include "ZEThread.h"
#include "ZEError.h"

ZETask* ZETaskPool::Schedule(ZETask* CurrentTask)
{
	ZETask* Task = NULL;

	SchedulerLock.Lock();

	if (CurrentTask != NULL)
	{
		QueuedTasks.Remove(&CurrentTask->Link);
		
		if (CurrentTask->GetStatus() == ZE_TS2_WAITING)
			QueuedTasks.AddEnd(&CurrentTask->Link);
		else
			InactiveTasks.AddBegin(&CurrentTask->Link);
	}

	// No Task Available
	if (QueuedTasks.GetCount() == 0)
	{
		SchedulerLock.Unlock();
		Signal.Signal();
		return NULL;
	}
	else
	{
		ZELink<ZETask>* ActiveTask = QueuedTasks.GetFirst();
		QueuedTasks.Remove(ActiveTask);
		RunningTasks.AddEnd(ActiveTask);
		ActiveTask->GetItem()->Status = ZE_TS2_RUNNING;
		SchedulerLock.Unlock();
		return ActiveTask->GetItem();
	}
}

void ZETaskPool::ThreadFunction(ZEThread* Thread, void* ExtraParameters)
{
	while(Thread->ControlPoint())
	{
		ZETask* Task = (ZETask*)ExtraParameters;
		Task->Thread = Thread;
		Task->Status = ZE_TS2_RUNNING;
		Task->Function(Task, Task->ExtraParameters);
		Task = Schedule(Task);
		if (Task == NULL)
			Thread->Suspend();
	}
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

const ZEList2<ZETask>& ZETaskPool::GetRunningTasks()
{
	return RunningTasks;
}

const ZEList2<ZETask>& ZETaskPool::GetQueuedTasks()
{
	return QueuedTasks;
}

const ZEList2<ZETask>& ZETaskPool::GetInactiveTasks()
{
	return InactiveTasks;
}

void ZETaskPool::SetThreadCount(ZEUInt Count)
{
	if (Count == Threads.GetCount())
	{
		return;
	}
	else if (Count > Threads.GetCount())
	{
		ZESize OldCount = Threads.GetCount();
		SchedulerLock.Lock();
		
		Threads.Resize(Count);
		for (ZESize I = OldCount; I < Count; I++)
		{
			Threads[I] = new ZEThread();
			Threads[I]->SetFunction(ZEThreadFunction::Create<ZETaskPool, &ZETaskPool::ThreadFunction>(this));
			Threads[I]->Run();
		}

		SchedulerLock.Unlock();
	}
	else if (Count < Threads.GetCount())
	{
		ZESize OldCount = Threads.GetCount();
		for (ZESize I = Count; I < OldCount; I++)
			Threads[I]->Exit();

		for (ZESize I = Count; I < OldCount; I++)
		{
			Threads[I]->Wait();
			delete Threads[I];
		}
		
		SchedulerLock.Lock();
		Threads.Resize(Count);
		SchedulerLock.Unlock();
	}
}

ZEUInt ZETaskPool::GetThreadCount()
{
	return (ZEUInt)Threads.GetCount();
}

void ZETaskPool::RunTask(ZETask* Task)
{
	if (Task->GetPool() != NULL)
		return;

	SchedulerLock.Lock();
	QueuedTasks.AddEnd(&Task->Link);
	SchedulerLock.Unlock();
}

void ZETaskPool::CancelTask(ZETask* Task)
{
	SchedulerLock.Lock();
	
	if (Task->GetStatus() == ZE_TS2_RUNNING)
	{
		zeError("Cannot remove running task. Task Name: %s", Task->GetName().ToCString());
		RunningTasks.Remove(&Task->Link);
	}
	else if (Task->GetStatus() == ZE_TS2_WAITING)
	{
		QueuedTasks.Remove(&Task->Link);
	}
	else
	{
		InactiveTasks.Remove(&Task->Link);
	}

	SchedulerLock.Unlock();
}

void ZETaskPool::Resume()
{
	SchedulerLock.Lock();
	ze_for_each(Thread, Threads)
	{
		(*Thread)->Run();
	}
	SchedulerLock.Unlock();
}

void ZETaskPool::Suspend()
{
	SchedulerLock.Lock();
	ze_for_each(Thread, Threads)
	{
		if (*Thread == ZEThread::GetCurrentThread())
			zeCriticalError("Task in a pool suspended its own pool. Pool: \"%s\".", GetName().ToCString());

		(*Thread)->Suspend();
	}
	SchedulerLock.Unlock();
}

void ZETaskPool::Wait()
{
	if (RunningTasks.GetCount() == 0)
		return;

	Signal.Wait();
}
