//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETask.cpp
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

#include "ZETask.h"
#include "ZETaskThread.h"
#include "ZETaskPool.h"
#include "ZETaskManager.h"

void ZETask::Setup()
{
	Status = ZE_TS2_WAITING;
	InstanceIndex = 0;
}

bool ZETask::Activate(ZETaskThread* Thread)
{
	TaskLock.Lock();
	if (InstanceIndex >= InstanceCount)
	{
		TaskLock.Unlock();
		return false;
	}

	for (ZESize I = 0; I < Dependencies.GetCount(); I++)
	{
		if (Dependencies[I]->GetStatus() != ZE_TS2_DONE)
		{
			TaskLock.Unlock();
			return true;
		}
	}

	Thread->SetTask(this);
	Thread->SetInstanceIndex(InstanceIndex);
	Threads.AddEnd(&Thread->TaskLink);
	InstanceIndex++;
	Status = ZE_TS2_RUNNING;
	TaskLock.Unlock();

	bool Finished = true;
	if (!Function.IsNull())
		Finished = Function(Thread, Thread->GetInstanceIndex(), Parameter);

	TaskLock.Lock();
	Threads.Remove(&Thread->TaskLink);

	bool Requeue = false;
	if (Finished && InstanceIndex >= InstanceCount && Threads.GetCount() == 0 && SubTasks.GetCount() == 0)
	{
		if (!PostFunction.IsNull())
			PostFunction(Thread, InstanceCount, Parameter);

		Status = ZE_TS2_DONE;
		Signal.Signal();	

		if (Parent != NULL)
			Parent->SubTaskDone(Thread);

		Requeue = false;
	}
	else if (Finished && InstanceIndex >= InstanceCount)
	{
		Status = ZE_TS2_WAITING;
		Requeue = false;
	}
	else
	{
		Status = ZE_TS2_WAITING;
		Requeue = true;
	}

	Thread->SetTask(NULL);
	Thread->SetInstanceIndex(0);
	TaskLock.Unlock();

	return Requeue;
}

void ZETask::SubTaskDone(ZETaskThread* TaskThread)
{
	TaskLock.Lock();

	SubTasks.Remove(&TaskThread->GetTask()->ParentLink);
	if (InstanceIndex < InstanceCount || Threads.GetCount() != 0 || SubTasks.GetCount() != 0)
	{
		TaskLock.Unlock();
		return;
	}

	TaskThread->SetTask(this);
	TaskThread->SetInstanceIndex(InstanceCount);
	if (!PostFunction.IsNull())
		PostFunction(TaskThread, InstanceCount, Parameter);

	Status = ZE_TS2_DONE;
	Signal.Signal();	

	if (Parent != NULL)
		Parent->SubTaskDone(TaskThread);

	TaskLock.Unlock();
}

void ZETask::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString ZETask::GetName() const
{
	return Name;
}

ZETask* ZETask::GetParent() const
{
	return Parent;
}

ZETaskStatus ZETask::GetStatus() const
{
	return Status;
}

const ZEList2<ZETaskThread>& ZETask::GetThreads() const
{
	return Threads;
}

const ZEArray<ZETask*>& ZETask::GetDependencies() const
{
	return Dependencies;
}

void ZETask::AddDependency(ZETask* Task)
{
	if (Task->Parent != NULL)
		return;

	Task->DependentCount++;
	Dependencies.Add(Task);

	Task->SetPriority(Task->GetPriority() + 1);
}

void ZETask::RemoveDependency(ZETask* Task)
{
	if (Task->Parent != this)
		return;

	Task->DependentCount--;
	Dependencies.RemoveValue(Task);
}

void ZETask::SetPriority(ZEInt Priority)
{
	this->Priority = Priority;
}

ZEInt ZETask::GetPriority() const
{
	return Priority;
}

void ZETask::SetInstanceCount(ZEInt InstanceCount)
{
	this->InstanceCount = InstanceCount;
}

ZEInt ZETask::GetInstanceCount() const
{
	return InstanceCount;
}

void ZETask::SetFunction(const ZETaskFunction& Function)
{
	this->Function = Function;
}

const ZETaskFunction& ZETask::GetFunction() const
{
	return Function;
}

void ZETask::SetPostFunction(const ZETaskPostFunction& Function)
{
	this->PostFunction = Function;
}

const ZETaskPostFunction& ZETask::GetPostFunction() const
{
	return PostFunction;
}

void ZETask::SetParameter(void* Parameters)
{
	this->Parameter = Parameters;
}

void* ZETask::GetParameter() const
{
	return Parameter;
}


void ZETask::SetPool(ZEInt PoolId)
{
	PoolId = PoolId;
}

ZEInt ZETask::GetPool() const
{
	if (Parent != NULL)
		return Parent->GetPool();
	else
		return PoolId;
}

const ZEList2<ZETask>& ZETask::GetSubTasks() const
{
	return SubTasks;
}

void ZETask::RunSubTask(ZETask* Task)
{
	if (Task->Parent != NULL)
		return;

	Task->PoolId = PoolId;
	Task->Parent = this;

	SubTasks.AddEnd(&Task->ParentLink);
}

void ZETask::Run()
{
	ZETaskPool* Pool = ZETaskManager::GetInstance()->GetPool(PoolId);
	if (Pool == NULL)
		return;

	if (Function.IsNull())
		return;

	Pool->RunTask(this);
}

void ZETask::Wait()
{
	if (Status == ZE_TS2_NONE)
		return;

	Signal.Wait();
}

ZETask::ZETask() : Link(this), ParentLink(this)
{
	Parent = NULL;
	Status = ZE_TS2_NONE;
	Priority = 0;
	PoolId = ZE_TPI_DEFAULT;
	Parameter = NULL;
	InstanceCount = 1;
	InstanceIndex = 0;
	DependentCount = 0;
}

ZETask::~ZETask()
{
	for (ZESize I = 0; I < Dependencies.GetCount(); I++)
		Dependencies[I]->DependentCount--;
}
