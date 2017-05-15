//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETaskManager.cpp
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

#include "ZETaskManager.h"

#include "ZETask.h"
#include "ZETaskPool.h"
#include "ZETaskThread.h"

ZETaskThread* ZETaskManager::RequestThread()
{
	ZETaskThread* Thread = NULL;

	ZELink<ZETaskThread>* ThreadLink = SuspendedThreads.GetFirst();
	if (ThreadLink == NULL)
	{
		ZESize AliveThreadCount = SuspendedThreads.GetCount() + FiringThreads.GetCount() + ActiveThreads.GetCount();
		 if (AliveThreadCount < ThreadCount)
		 {
			Thread = new ZETaskThread();
			Thread->SetFunction(ZEThreadFunction::Create<ZETaskManager, &ZETaskManager::Scheduler>(this));
			FiringThreads.AddEnd(&Thread->PoolLink);
		 }
	}
	else
	{
		Thread = ThreadLink->GetItem();
		SuspendedThreads.Remove(ThreadLink);
		FiringThreads.AddEnd(&Thread->PoolLink);
	}
	
	return Thread;
}

void ZETaskManager::ReleaseThread(ZETaskThread* Thread)
{
	ZESize AliveThreadCount = SuspendedThreads.GetCount() + FiringThreads.GetCount() + ActiveThreads.GetCount();
	if (AliveThreadCount > ThreadCount)
	{
		ActiveThreads.Remove(&Thread->PoolLink);
		SchedulerLock.Unlock();
		Thread->Destroy();
	}
	else
	{
		ActiveThreads.Remove(&Thread->PoolLink);
		SuspendedThreads.AddEnd(&Thread->PoolLink);
	}
}

ZETask* ZETaskManager::RequestTask(ZESize& QueuedTaskCount)
{
	QueuedTaskCount = 0;
	
	ZETaskPool* TaskPool = NULL;
	// Hard Constraints (Reserved Threads, Always First Policy)
	for (ZESize I = 0; I < TaskPools.GetCount(); I++)
	{
		ZETaskPool* CurrentPool = TaskPools[I];
		if (CurrentPool->QueuedTasks.GetCount() == 0)
			continue;

		QueuedTaskCount += CurrentPool->QueuedTasks.GetCount() + CurrentPool->InstancingTasks.GetCount();

		if (TaskPool != NULL)
			continue;

		if (CurrentPool->RemainingReservedThreadCount == 0)
			continue;

		if (CurrentPool->SchedulingPolicy != ZE_TSP_ALWAYS_FIRST)
			continue;

		TaskPool = CurrentPool;
		LastScheduledPoolIndex = I;
		break;
	}

	// Soft Constraints (Pool Priority)
	if (TaskPool == NULL)
	{
		for (ZESize I = 0; I < TaskPools.GetCount(); I++)
		{
			ZESize Index = TaskPools.Circular(LastScheduledPoolIndex + I + 1);
			ZETaskPool* CurrentPool = TaskPools[Index];
				
			if (CurrentPool->QueuedTasks.GetCount() == 0 && CurrentPool->InstancingTasks.GetCount() == 0)
				continue;

			TaskPool = CurrentPool;
			LastScheduledPoolIndex = Index;
			break;
		}
	}

	if (TaskPool == NULL)
		return NULL;

	ZETask* Task = NULL;
	if (TaskPool->InstancingTasks.GetFirst() != NULL)
		Task = TaskPool->InstancingTasks.GetFirst()->GetItem();
	
	if (Task == NULL)
		Task = TaskPool->QueuedTasks.GetFirst()->GetItem();

	return Task;
}

void ZETaskManager::Scheduler(ZEThread* Thread, void* ExtraParameter)
{
	ZETaskThread* TaskThread = static_cast<ZETaskThread*>(Thread);
	SchedulerLock.Lock();
	{
		FiringThreads.Remove(&TaskThread->PoolLink);
		ActiveThreads.AddEnd(&TaskThread->PoolLink);
	}
	SchedulerLock.Unlock();

	while(Thread->ControlPoint())
	{
		ZETask* Task = NULL;
		SchedulerLock.Lock();
		{
			ZESize QueuedTaskCount = 0;
			TaskThread->TaskDestroyed = false;
			Task = RequestTask(QueuedTaskCount);
			if (Task == NULL)
			{
				// No task remaining so suspend thread
				ReleaseThread(TaskThread);
				SchedulerLock.Unlock();

				TaskThread->Suspend();

				SchedulerLock.Lock();
				FiringThreads.Remove(&TaskThread->PoolLink);
				ActiveThreads.AddEnd(&TaskThread->PoolLink);
				SchedulerLock.Unlock();

				continue;
			}

			// Check for new threads in the pool to wake up
			while (QueuedTaskCount != 0)
			{
				ZETaskThread* Thread = RequestThread();
				if (Thread == NULL)
					break;

				Thread->Run();

				QueuedTaskCount--;
			}
		
			static_cast<ZETaskThread*>(Thread)->SetTask(Task);

			if (Task->InstancingState == ZE_TIS_NONE)
			{
				Task->Pool->QueuedTasks.Remove(&Task->Link);
				Task->Pool->RunningTasks.AddEnd(&Task->Link);
			}
			else
			{
				if (Task->Status == ZE_TS2_WAITING)
				{
					Task->Pool->QueuedTasks.Remove(&Task->Link);
					Task->Pool->InstancingTasks.AddEnd(&Task->Link);
				}
				Task->RunningInstanceCount++;
			}

			Task->Status = ZE_TS2_RUNNING;

			Task->LastInstanceIndex++;
		}
		SchedulerLock.Unlock();

		ZETaskResult Result = ZE_TR_DONE;
		if (!Task->Function.IsNull())
			Result = Task->Function(static_cast<ZETaskThread*>(Thread), Task->LastInstanceIndex - 1, Task->GetParameter());

		if (TaskThread->TaskDestroyed)
			continue;

		SchedulerLock.Lock();
		{
			if (Task->InstancingState == ZE_TIS_NONE)
			{
				Task->Pool->RunningTasks.Remove(&Task->Link);

				if (Result == ZE_TR_DONE)
				{
					Task->Status = ZE_TS2_DONE;
				}
				else if (Result == ZE_TR_COOPERATING)
				{
					Task->Status = ZE_TS2_WAITING;
					Task->Pool->QueuedTasks.AddEnd(&Task->Link);
				}
				else
				{
					Task->Status = ZE_TS2_FAILED;
				}
			}
			else
			{
				if (Result == ZE_TR_STOP_INSTANCING)
				{
					Task->InstancingState = ZE_TIS_STOPPED;
					if (Task->Link.GetInUse())
						Task->Pool->InstancingTasks.Remove(&Task->Link);
				}
				else if ( Result == ZE_TR_COOPERATING)
				{
					zeCriticalError("Instanced tasks that cooperates not supported.");	
				}
				else if (Result == ZE_TR_FAILED)
				{
					Task->Status = ZE_TS2_FAILED;
					Task->InstancingState = ZE_TIS_STOPPED;
					if (Task->Link.GetInUse())
						Task->Pool->InstancingTasks.Remove(&Task->Link);
				}
		
				Task->RunningInstanceCount--;
				if (Task->RunningInstanceCount == 0 && Task->InstancingState == ZE_TIS_STOPPED)
				{
					Task->InstancingState = ZE_TIS_NONE;
					if (Task->Status == ZE_TS2_RUNNING)
						Task->Status = ZE_TS2_DONE;
				}
			}
		}
		SchedulerLock.Unlock();
	}
}

void ZETaskManager::SchedulerJoined(ZETask* Task)
{
	while (true)
	{
		SchedulerLock.Lock();
		{
			if (Task->Status == ZE_TS2_DONE && Task->Status == ZE_TS2_FAILED)
			{
				SchedulerLock.Unlock();
				return;
			}
		
			if (Task->InstancingState == ZE_TIS_STOPPED)
			{
				SchedulerLock.Unlock();

				while(Task->Status == ZE_TS2_RUNNING);

				return;
			}

			while (true)
			{
				ZETaskThread* Thread = RequestThread();
				if (Thread == NULL)
					break;

				Thread->Run();
			}

			Task->RunningInstanceCount++;
			Task->Status = ZE_TS2_RUNNING;
		}
		SchedulerLock.Unlock();

		ZETaskResult Result = ZE_TR_DONE;
		if (!Task->Function.IsNull())
			Result = Task->Function(NULL, Task->LastInstanceIndex, Task->GetParameter());

		Task->LastInstanceIndex++;

		SchedulerLock.Lock();
		{
			if (Result == ZE_TR_STOP_INSTANCING)
			{
				Task->InstancingState = ZE_TIS_STOPPED;
				if (Task->Link.GetInUse())
					Task->Pool->InstancingTasks.Remove(&Task->Link);
			}
			if (Result == ZE_TR_COOPERATING)
			{
				zeError("Instanced tasks that cooperates not supported.");	
			}
			else if (Result == ZE_TR_FAILED)
			{
				Task->Status = ZE_TS2_FAILED;
				Task->InstancingState = ZE_TIS_STOPPED;
				if (Task->Link.GetInUse())
					Task->Pool->InstancingTasks.Remove(&Task->Link);
			}

			Task->RunningInstanceCount--;
			if (Task->RunningInstanceCount == 0 && Task->InstancingState == ZE_TIS_STOPPED)
			{
				if (Task->Status == ZE_TS2_RUNNING)
					Task->Status = ZE_TS2_DONE;

				Task->InstancingState = ZE_TIS_NONE;
				SchedulerLock.Unlock();

				return;
			}
		}

		SchedulerLock.Unlock();
	}
}

void ZETaskManager::RemoveTask(ZETask* Task)
{
	SchedulerLock.Lock();
	{
		if (Task->GetStatus() == ZE_TS2_WAITING)
			Task->Pool->QueuedTasks.Remove(&Task->Link);
	
		if (Task->GetStatus() == ZE_TS2_RUNNING)
		{
			Task->Pool->RunningTasks.Remove(&Task->Link);
			ze_for_each(Thread, ActiveThreads)
			{
				if (Thread->Task == Task)
					Thread->TaskDestroyed = true;
			}
		}
	}
	SchedulerLock.Unlock();
}

void ZETaskManager::TerminateTasks()
{
	SetThreadCount(0);

	SchedulerLock.Lock();
	{
		for (ZESize I = 0; I < TaskPools.GetCount(); I++)
		{
			ZETaskPool* CurrentPool = TaskPools[I];
			CurrentPool->QueuedTasks.Clear();
			ze_for_each(CurrentTask, CurrentPool->RunningTasks)
				RemoveTask(CurrentTask.GetPointer());
		}
	}
	SchedulerLock.Unlock();

	ZEArray<ZEThread*> Threads;
	Threads.SetCount(SuspendedThreads.GetCount());
	ze_for_each(Thread, Threads)
		Threads[Thread.GetIndex()] = SuspendedThreads[Thread.GetIndex()];

	SuspendedThreads.Clear();

	ze_for_each(Thread, Threads)
		Thread.GetItem()->Destroy();
}

const ZEArray<ZETaskPool*>& ZETaskManager::GetPools() const
{
	return TaskPools;
}

ZETaskPool* ZETaskManager::GetPool(ZEInt PoolId)
{
	for (ZESize I = 0; I < TaskPools.GetCount(); I++)
		if (TaskPools[I]->GetId() == PoolId)
			return TaskPools[I];

	return NULL;
}

void ZETaskManager::RegisterPool(ZETaskPool* Pool)
{
	zeCheckError(Pool == NULL, ZE_VOID, "Cannot add Task Pool. Pool is NULL.");
	zeCheckError(Pool->Manager != NULL, ZE_VOID, "Cannot add Task Pool. Task Pool already added to a manager. Pool Name: \"%s\".", Pool->GetName().ToCString());

	Pool->Manager = this;
	TaskPools.Add(Pool);
}

void ZETaskManager::UnregisterPool(ZETaskPool* Pool)
{
	TaskPools.LockWrite();
	{
		zeCheckError(Pool == NULL, ZE_VOID, "Cannot remove Task Pool. Pool is NULL.");
		zeCheckError(Pool->Manager != this, ZE_VOID, "Cannot remove Task Pool. Task Pool does not belong to this manager. Pool Name: \"%s\".", Pool->GetName().ToCString());
		zeCheckError(Pool->RunningTasks.GetCount() != 0 || Pool->QueuedTasks.GetCount() != 0, ZE_VOID, "Cannot remove Task Pool. There are task associated with this pool. Pool Name: \"%s\".", Pool->GetName().ToCString());

		Pool->Manager = NULL;
		TaskPools.RemoveValue(Pool);
	}
	TaskPools.UnlockWrite();
}

void ZETaskManager::SetThreadCount(ZESize Count)
{
	ThreadCount = Count;
}

ZESize ZETaskManager::GetThreadCount() const
{
	return (ZEUInt)ThreadCount;
}

void ZETaskManager::RunTask(ZETask* Task)
{
	zeCheckError(Task == NULL, ZE_VOID, "Cannot run task. Task is NULL.");

	ZETaskThread* Thread = NULL;
	SchedulerLock.Lock();
	{
		if (Task->Status == ZE_TS2_RUNNING || Task->Status == ZE_TS2_WAITING)
		{
			SchedulerLock.Unlock();
			return;
		}

		Task->Manager = this;
		Task->Pool = GetPool(Task->PoolId);
		if (Task->Pool == NULL)
			zeCriticalError("Cannot run task. Task does not have a pool. Task Name: \"%s\".", Task->GetName().ToCString());

		Task->Status = ZE_TS2_WAITING;
		Task->InstancingState = ZE_TIS_NONE;
		Task->LastInstanceIndex = 0;
		Task->RunningInstanceCount = 0;
		Task->Pool->QueuedTasks.AddEnd(&Task->Link);

		Thread = RequestThread();
	}
	SchedulerLock.Unlock();

	if (Thread != NULL)
		Thread->Run();
}

void ZETaskManager::RunTaskInstanced(ZETask* Task, bool UseCurrentThread)
{
	zeCheckError(Task == NULL, ZE_VOID, "Cannot run task. Task is NULL.");

	ZETaskThread* Thread = NULL;
	SchedulerLock.Lock();
	{
		if (Task->Status == ZE_TS2_RUNNING || Task->Status == ZE_TS2_WAITING)
		{
			SchedulerLock.Unlock();
			return;
		}

		Task->Manager = this;
		Task->Pool = GetPool(Task->PoolId);
		if (Task->Pool == NULL)
			zeCriticalError("Cannot run task. Task does not have a pool. Task Name: \"%s\".", Task->GetName().ToCString());

		Task->Status = ZE_TS2_RUNNING;
		Task->InstancingState = ZE_TIS_INSTANCING;
		Task->LastInstanceIndex = 0;
		Task->RunningInstanceCount = 0;
		Task->Pool->InstancingTasks.AddEnd(&Task->Link);
	}
	SchedulerLock.Unlock();

	SchedulerJoined(Task);
}

void ZETaskManager::RunTaskSerial(ZETask* Task)
{
	zeCheckError(Task == NULL, ZE_VOID, "Cannot run task. Task is NULL.");

	while (Task->Function(NULL, 0, Task->Parameter) == ZE_TR_COOPERATING);
}

ZETaskManager::ZETaskManager()
{
	LastScheduledPoolIndex = 0;
}

ZETaskManager::~ZETaskManager()
{
	TerminateTasks();
	for (ZESize I = 0; I < TaskPools.GetCount(); I++)
		delete TaskPools[I];
}

ZETaskManager* ZETaskManager::GetInstance()
{
	static ZETaskManager TaskManager;
	return &TaskManager;
}
