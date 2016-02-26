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
}

void ZETask::Activate(ZETaskThread* Thread)
{
	TaskLock.Lock();
	{
		if (Status != ZE_TS2_WAITING)
		{
			TaskLock.Unlock();
			return;
		}

		Thread->SetTask(this);
		Status = ZE_TS2_RUNNING;
	}
	TaskLock.Unlock();

	ZETaskResult Result = ZE_TR_DONE;
	if (!Function.IsNull())
		Result = Function(Thread, Parameter);

	TaskLock.Lock();
	{
		if (Result == ZE_TR_DONE)
		{
			Status = ZE_TS2_DONE;
			Signal.Signal();
		}
		else if (Result == ZE_TR_COOPERATING)
		{
			Status = ZE_TS2_WAITING;
		}
		else
		{
			Status = ZE_TS2_FAILED;
		}
	}
	TaskLock.Unlock();
}

ZETaskStatus ZETask::GetStatus() const
{
	return Status;
}


void ZETask::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString ZETask::GetName() const
{
	return Name;
}

void ZETask::SetPriority(ZEInt Priority)
{
	this->Priority = Priority;
}

ZEInt ZETask::GetPriority() const
{
	return Priority;
}

void ZETask::SetFunction(const ZETaskFunction& Function)
{
	this->Function = Function;
}

const ZETaskFunction& ZETask::GetFunction() const
{
	return Function;
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
	return PoolId;
}


void ZETask::Run()
{
	if (Function.IsNull())
		return;

	ZETaskPool* Pool = ZETaskManager::GetInstance()->GetPool(PoolId);
	if (Pool == NULL)
		return;

	Pool->RunTask(this);
}

void ZETask::Wait()
{
	if (Status == ZE_TS2_NONE)
		return;

	Signal.Wait();
}

ZETask::ZETask() : Link(this)
{
	Status = ZE_TS2_NONE;
	Priority = 0;
	PoolId = ZE_TPI_DEFAULT;
	Parameter = NULL;
}

ZETask::~ZETask()
{

}
