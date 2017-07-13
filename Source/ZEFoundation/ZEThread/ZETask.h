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

enum  ZETaskPoolId
{
	ZE_TPI_REAL_TIME,
	ZE_TPI_CONCURENT,
	ZE_TPI_IO,
	ZE_TPI_DEFAULT = ZE_TPI_CONCURENT
};

enum ZETaskStatus
{
	ZE_TS2_FAILED		= -1,
	ZE_TS2_NONE			= 0,
	ZE_TS2_WAITING		= 1,
	ZE_TS2_RUNNING		= 2,
	ZE_TS2_DONE			= 3
};

enum ZETaskInstancingState
{
	ZE_TIS_NONE,
	ZE_TIS_INSTANCING,
	ZE_TIS_STOPPED
};

enum ZETaskResult
{
	ZE_TR_FAILED			= -1,
	ZE_TR_COOPERATING		= 0,
	ZE_TR_DONE				= 1,
	ZE_TR_STOP_INSTANCING	= 2
};

class ZETask;
class ZETaskThread;
class ZETaskPool;

typedef ZEDelegate<ZETaskResult (ZETaskThread* TaskThread, ZESize InstanceIndex, void* Parameter)> ZETaskFunction;

class ZETask
{
	friend class ZETaskManager;
	private:
		ZETaskManager*					Manager;
		ZEString						Name;
		volatile ZETaskStatus			Status;
		ZEInt							PoolId;
		ZETaskPool*						Pool;
		ZEInt							Priority;
		ZELink<ZETask>					Link;

		volatile ZETaskInstancingState	InstancingState;
		volatile ZESize					RunningInstanceCount;
		volatile ZESize					LastInstanceIndex;

		ZETaskFunction					Function;
		void*							Parameter;	
		
	public:
		ZETaskStatus					GetStatus() const;

		void							SetName(const ZEString& Name);
		const ZEString					GetName() const;

		void							SetPriority(ZEInt Priority);
		ZEInt							GetPriority() const;

		void							SetFunction(const ZETaskFunction& Function);
		const ZETaskFunction&			GetFunction() const;

		void							SetParameter(void* Parameter);
		void*							GetParameter() const;

		void							SetPoolId(ZEInt PoolId);
		ZEInt							GetPoolId() const;

		void							Run();
		void							RunInstanced();
		void							RunInstancedJoined();
		void							Wait();

										ZETask();
										~ZETask();
};
