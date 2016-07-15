//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERSResourceManager.cpp
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

#include "ZERSResourceManager.h"

#include "ZEError.h"
#include "ZEFile/ZEPathInfo.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZERSResourceLoadable.h"

void ZERSResourceManager::UpdateMemoryUsage(ZERSResource* Resource, ZESSize MemoryUsageDelta[ZERS_MEMORY_POOL_COUNT])
{
	ManagerLock.Lock();
	for (ZESize Pool = 0; Pool < ZERS_MEMORY_POOL_COUNT; Pool++)
	{
		if (Resource->IsShared())
			MemoryUsageShared[Pool] += MemoryUsageDelta[Pool];
		else
			MemoryUsagePrivate[Pool] += MemoryUsageDelta[Pool];
	}
	ManagerLock.Unlock();
}

const ZERSResourceLoadable* ZERSResourceManager::GetResourceInternal(const ZEString& FileName)
{
	ZEFileInfo FileInfo(FileName);
	ZEString FileNameNormalized = FileInfo.Normalize();
	if (FileNameNormalized.IsEmpty())
		return NULL;

	ZEUInt64 Hash = FileName.Lower().Hash();
	ze_for_each(Resource, SharedResources)
	{
		if (Resource->GetType() != ZERS_RT_LOADABLE)
			continue;

		const ZERSResourceLoadable* ResourceLoadable = static_cast<const ZERSResourceLoadable*>(Resource.GetPointer());
		if (ResourceLoadable->FileNameHash == Hash && ResourceLoadable->FileName == FileNameNormalized)
			return ResourceLoadable;
	}

	return NULL;
}

const ZERSResource* ZERSResourceManager::GetResourceInternal(const ZEGUID& GUID)
{
	ze_for_each(Resource, SharedResources)
	{
		if (Resource->GetGUID() == GUID)
			return Resource.GetPointer();	
	}

	return NULL;
}

void ZERSResourceManager::RegisterResource(const ZERSResource* Resource)
{
	ManagerLock.Lock();

	Resources.AddEnd(&Resource->ManagerLink);

	for (ZESize Pool = 0; Pool < ZERS_MEMORY_POOL_COUNT; Pool++)
		MemoryUsagePrivate[Pool] += Resource->MemoryUsage[Pool];

	ManagerLock.Unlock();
}

void ZERSResourceManager::UnregisterResource(const ZERSResource* Resource)
{
	ManagerLock.Lock();

	if (Resource->IsShared())
		DelistResource(Resource);
	Resources.Remove(&Resource->ManagerLink);

	for (ZESize Pool = 0; Pool < ZERS_MEMORY_POOL_COUNT; Pool++)
		MemoryUsagePrivate[Pool] -= Resource->MemoryUsage[Pool];

	ManagerLock.Unlock();
}

void ZERSResourceManager::EnlistResource(const ZERSResource* Resource)
{
	ManagerLock.Lock();
	
	SharedResources.AddEnd(&Resource->ManagerSharedLink);
	Resource->Shared = true;
	
	for (ZESize Pool = 0; Pool < ZERS_MEMORY_POOL_COUNT; Pool++)
	{
		MemoryUsagePrivate[Pool] -= Resource->MemoryUsage[Pool];
		MemoryUsageShared[Pool] += Resource->MemoryUsage[Pool];
	}

	ManagerLock.Unlock();
}

void ZERSResourceManager::DelistResource(const ZERSResource* Resource)
{
	ManagerLock.Lock();
	
	SharedResources.Remove(&Resource->ManagerSharedLink);
	Resource->Shared = true;
	
	for (ZESize Pool = 0; Pool < ZERS_MEMORY_POOL_COUNT; Pool++)
	{
		MemoryUsagePrivate[Pool] += Resource->MemoryUsage[Pool];
		MemoryUsageShared[Pool] -= Resource->MemoryUsage[Pool];
	}

	ManagerLock.Unlock();
}

void ZERSResourceManager::DestroyResource(const ZERSResource* Resource)
{
	DelistResource(Resource);
	UnregisterResource(Resource);
	delete Resource;

}

ZERSResourceManager::ZERSResourceManager()
{
	memset(MemoryUsagePrivate, 0, sizeof(MemoryUsagePrivate));
	memset(MemoryUsageShared, 0, sizeof(MemoryUsageShared));
}

ZERSResourceManager::~ZERSResourceManager()
{
	
}

ZESize ZERSResourceManager::GetPrivateResourceCount()
{
	return SharedResources.GetCount();
}

ZESize ZERSResourceManager::GetSharedResourceCount()
{
	return SharedResources.GetCount();
}

ZESize ZERSResourceManager::GetMemoryUsagePrivate(ZERSMemoryPool Pool)
{
	zeCheckError(Pool >= ZERS_MEMORY_POOL_COUNT, 0, "Unknown memory pool.");
	return MemoryUsagePrivate[Pool];
}

ZESize ZERSResourceManager::GetMemoryUsageShared(ZERSMemoryPool Pool)
{
	zeCheckError(Pool >= ZERS_MEMORY_POOL_COUNT, 0, "Unknown memory pool.");
	return MemoryUsageShared[Pool];
}

ZESize ZERSResourceManager::GetMemoryUsageTotal(ZERSMemoryPool Pool)
{
	return GetMemoryUsagePrivate(Pool) + GetMemoryUsageShared(Pool);
}

ZERSHolder<const ZERSResource> ZERSResourceManager::GetResource(const ZEGUID& GUID)
{
	ManagerLock.Lock();
	ZERSHolder<const ZERSResource> Resource = GetResourceInternal(GUID);
	ManagerLock.Unlock();

	return Resource;
}

ZERSHolder<const ZERSResourceLoadable> ZERSResourceManager::GetResource(const ZEString& FileName)
{
	ManagerLock.Lock();
	ZERSHolder<const ZERSResourceLoadable> Resource = GetResourceInternal(FileName);
	ManagerLock.Unlock();

	return Resource;
}

ZERSHolder<const ZERSResource> ZERSResourceManager::StageResource(const ZEGUID& GUID, ZERSInstanciator Insanciaor, ZERSResource** StagingResource)
{
	ManagerLock.Lock();
	const ZERSResource* Resource = GetResourceInternal(GUID);
	if (Resource == NULL)
	{
		ZERSResource* NewResouce = Insanciaor();
		NewResouce = Insanciaor();
		NewResouce->SetGUID(GUID);
		NewResouce->State = ZERS_RS_STAGING;
		SharedResources.AddEnd(&NewResouce->ManagerLink);
		ManagerLock.Unlock();

		if (StagingResource != NULL)
			*StagingResource = NewResouce;

		return NewResouce;
	}
	else
	{
		ManagerLock.Unlock();
		*StagingResource = NULL;
		return Resource;
	}
}

ZERSHolder<const ZERSResourceLoadable> ZERSResourceManager::StageResource(const ZEString& FileName, ZERSInstanciatorLoadable Insanciaor, ZERSResourceLoadable** StagingResource)
{
	ManagerLock.Lock();
	const ZERSResourceLoadable* Resource = GetResourceInternal(FileName);
	if (Resource == NULL)
	{
		ZERSResourceLoadable* NewResouce = Insanciaor();
		NewResouce = Insanciaor();
		NewResouce->FileName = ZEPathInfo(FileName).Normalize();
		NewResouce->FileNameHash = NewResouce->FileName.Lower().Hash();
		NewResouce->State = ZERS_RS_STAGING;
		SharedResources.AddEnd(&NewResouce->ManagerLink);
		ManagerLock.Unlock();

		if (StagingResource != NULL)
			*StagingResource = NewResouce;

		return NewResouce;
	}
	else
	{
		ManagerLock.Unlock();
		*StagingResource = NULL;
		return Resource;
	}
}

ZERSResourceManager* ZERSResourceManager::GetInstance()
{
	static ZERSResourceManager Manager;
	return &Manager;
}
