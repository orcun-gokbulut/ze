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

void ZETask::SetPoolId(ZEInt PoolId)
{
	PoolId = PoolId;
}

ZEInt ZETask::GetPoolId() const
{
	return PoolId;
}

void ZETask::Run()
{
	if (Function.IsNull())
		return;

	ZETaskManager::GetInstance()->RunTask(this);
}

void ZETask::RunInstanced()
{
	if (Function.IsNull())
		return;

	ZETaskManager::GetInstance()->RunTaskInstanced(this, true);
}

void ZETask::Wait()
{
	if (Status == ZE_TS2_NONE)
		return;

	while(Status == ZE_TS_RUNNING);
}

ZETask::ZETask() : Link(this)
{
	Manager = NULL;
	Status = ZE_TS2_NONE;
	Priority = 0;
	PoolId = ZE_TPI_DEFAULT;
	Pool = NULL;
	Parameter = NULL;
	InstancingState = ZE_TIS_NONE;
	LastInstanceIndex = 0;
	RunningInstanceCount = 0;
}

ZETask::~ZETask()
{
	ZETaskManager::GetInstance()->RemoveTask(this);
}
