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
#include "ZEDS\ZEArray.h"
#include "ZEDS\ZEList2.h"
#include "ZEDS\ZEDelegate.h"
#include "ZEThread.h"
#include "ZESignal.h"

enum  ZETaskPoolId
{
	ZE_TPI_REAL_TIME,
	ZE_TPI_CONCURENT,
	ZE_TPI_IO,
	ZE_TPI_DEFAULT = ZE_TPI_CONCURENT
};

enum ZETaskStatus
{
	ZE_TS2_NONE,
	ZE_TS2_WAITING,
	ZE_TS2_RUNNING,
	ZE_TS2_DONE
};

class ZETask;
class ZETaskThread;
class ZETaskPool;

typedef ZEDelegate<bool (ZETaskThread* TaskThread, ZEInt InstanceIndex, void* ExtraParameters)> ZETaskFunction;
typedef ZEDelegate<void (ZETaskThread* TaskThread, ZEInt InstanceCount, void* ExtraParameters)> ZETaskPostFunction;

class ZETask
{
	friend class Manager;
	friend class ZETaskPool;
	private:
		ZETask*					Parent;
		ZEString				Name;
		ZETaskStatus			Status;
		ZEInt					PoolId;
		ZEInt					Priority;
		ZELink<ZETask>			Link;
		ZELink<ZETask>			ParentLink;
	
		ZEList2<ZETask>			SubTasks;
		ZEArray<ZETask*>		Dependencies;
		ZESize					DependentCount;

		ZETaskFunction			Function;
		ZETaskPostFunction		PostFunction;
		void*					Parameter;	
		ZEInt					InstanceCount;
		ZEInt					InstanceIndex;

		ZESignal				Signal;
		ZELock					TaskLock;
		ZEList2<ZETaskThread>	Threads;

		void					Setup();
		bool					Activate(ZETaskThread* Thread);
		void					SubTaskDone(ZETaskThread* SubTask = NULL);

	public:
		ZETask*					GetParent() const;
		ZETaskStatus			GetStatus() const;
		const ZEList2<ZETaskThread>& GetThreads() const;

		void					SetName(const ZEString& Name);
		const ZEString			GetName() const;

		void					SetPriority(ZEInt Priority);
		ZEInt					GetPriority() const;

		void					SetInstanceCount(ZEInt InstanceCount);
		ZEInt					GetInstanceCount() const;

		void					SetFunction(const ZETaskFunction& Function);
		const ZETaskFunction&	GetFunction() const;

		void					SetPostFunction(const ZETaskPostFunction& Function);
		const ZETaskPostFunction& GetPostFunction() const;

		void					SetParameter(void* Parameter);
		void*					GetParameter() const;

		void					SetPool(ZEInt PoolId);
		ZEInt					GetPool() const;

		const ZEArray<ZETask*>&	GetDependencies() const;
		void					AddDependency(ZETask* Task);
		void					RemoveDependency(ZETask* Task);

		const ZEList2<ZETask>&	GetSubTasks() const;
		void					RunSubTask(ZETask* Task);

		void					Run();
		void					Wait();

								ZETask();
								~ZETask();
};
