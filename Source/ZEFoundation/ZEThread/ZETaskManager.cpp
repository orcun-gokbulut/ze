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
#include "ZETaskPool.h"
#include "ZETask.h"

ZETaskPool* ZETaskManager::GetRealTimePool()
{
	return &GetInstance()->RealTimePool;
}

ZETaskPool* ZETaskManager::GetIOPool()
{
	return &GetInstance()->IOPool;
}

ZETaskPool* ZETaskManager::GetConcurrentPool()
{
	return &GetInstance()->ConcurrentPool;
}

const ZEArray<ZETaskPool*>& ZETaskManager::GetPools()
{
	return Pools;
}

ZETaskPool* ZETaskManager::GetPool(ZEInt PoolId)
{
	for (ZESize I = 0; I < Pools.GetCount(); I++)
		if (Pools[I]->GetId() == PoolId)
			return Pools[I];

	return NULL;
}

void ZETaskManager::RegisterPool(ZETaskPool* Pool)
{
	if (Pools.Exists(Pool))
		return;

	Pools.Add(Pool);
}

void ZETaskManager::UnregisterPool(ZEInt PoolId)
{
	if (PoolId == ZE_TPI_REAL_TIME ||
		PoolId == ZE_TPI_CONCURENT ||
		PoolId == ZE_TPI_IO)
	{
		return;
	}

	for (ZESize I = 0; I < Pools.GetCount(); I++)
	{
		if (Pools[I]->GetId() == PoolId)
		{
			Pools.Remove(I);
			return;
		}
	}
}

ZETaskManager::ZETaskManager()
{
	RealTimePool.SetId(ZE_TPI_REAL_TIME);
	RealTimePool.SetName("RealTime Pool");
	RealTimePool.SetThreadCount(0);
	Pools.Add(&RealTimePool);

	IOPool.SetId(ZE_TPI_IO);
	IOPool.SetName("IO Pool");
	IOPool.SetThreadCount(0);
	Pools.Add(&IOPool);

	ConcurrentPool.SetId(ZE_TPI_CONCURENT);
	ConcurrentPool.SetName("Concurrent Pool");
	ConcurrentPool.SetThreadCount(8);
	Pools.Add(&ConcurrentPool);
}

ZETaskManager::~ZETaskManager()
{

}

#ifdef ZE_PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

ZEUInt ZETaskManager::GetThreadCount()
{
	return 2;
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);
	return SystemInfo.dwNumberOfProcessors;
}

#endif

ZETaskManager* ZETaskManager::GetInstance()
{
	static ZETaskManager TaskManager;
	return &TaskManager;
}
