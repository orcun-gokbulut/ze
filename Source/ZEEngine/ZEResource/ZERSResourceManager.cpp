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

#include "ZERSResource.h"
#include "ZEError.h"
#include "ZEFile/ZEPathInfo.h"
#include "ZEFile/ZEFileInfo.h"

void ZERSResourceManager::UpdateMemoryUsage(ZERSResource* Resource, ZESSize MemoryUsageDelta[ZERS_MEMORY_POOL_COUNT])
{
	ManagerLock.Lock();
	for (ZESize Pool = 0; Pool < ZERS_MEMORY_POOL_COUNT; Pool++)
	{
		MemoryUsage[Pool] += MemoryUsageDelta[Pool];
		if (Resource->IsShared())
			MemoryUsageShared[Pool] += MemoryUsageDelta[Pool];
	}
	ManagerLock.Unlock();
}

ZERSHolder<const ZERSResource> ZERSResourceManager::GetResourceInternal(const ZEString& FileName)
{
	ZEFileInfo FileInfo(FileName);
	ZEString FileNameNormalized = FileInfo.Normalize();
	if (FileNameNormalized.IsEmpty())
		return NULL;

	ZEUInt64 Hash = FileName.Lower().Hash();
	ze_for_each(Resource, SharedResources)
	{
		Resource->ResourceLock.Lock();
		if (Resource->GetFileNameHash() == 0)
		{
			Resource->ResourceLock.Unlock();
			continue;
		}

		if (Resource->GetFileNameHash() == Hash && Resource->GetFileName() == FileNameNormalized)
		{
			ZERSHolder<const ZERSResource> Output;
			Output.Overwrite(Output);
			Resource->ReferenceCount++;
			Resource->ResourceLock.Unlock();

			return Output;
		}
		Resource->ResourceLock.Unlock();
	}

	return NULL;
}

ZERSHolder<const ZERSResource> ZERSResourceManager::GetResourceInternal(const ZEGUID& GUID)
{
	ze_for_each(Resource, SharedResources)
	{
		Resource->ResourceLock.Lock();
		if (Resource->GetGUID() == GUID)
		{	
			ZERSHolder<const ZERSResource> Output;
			Output.Overwrite(Resource.GetPointer());
			Resource->ReferenceCount++;
			Resource->ResourceLock.Unlock();
			return Output;
		}
		Resource->ResourceLock.Unlock();
	}

	return NULL;
}

void ZERSResourceManager::RegisterResourceInternal(ZERSResource* Resource)
{
	Resources.AddEnd(&Resource->ManagerLink);

	for (ZESize Pool = 0; Pool < ZERS_MEMORY_POOL_COUNT; Pool++)
		MemoryUsage[Pool] += Resource->MemoryUsage[Pool];
}

void ZERSResourceManager::UnregisterResourceInternal(ZERSResource* Resource)
{
	Resources.Remove(&Resource->ManagerLink);
	for (ZESize Pool = 0; Pool < ZERS_MEMORY_POOL_COUNT; Pool++)
		MemoryUsage[Pool] -= Resource->MemoryUsage[Pool];
}

void ZERSResourceManager::ShareResourceInternal(ZERSResource* Resource)
{
	SharedResources.AddEnd(&Resource->ManagerSharedLink);
	Resource->Shared = true;
	
	for (ZESize Pool = 0; Pool < ZERS_MEMORY_POOL_COUNT; Pool++)
		MemoryUsageShared[Pool] += Resource->MemoryUsage[Pool];
}

void ZERSResourceManager::UnshareResourceInternal(ZERSResource* Resource)
{
	SharedResources.Remove(&Resource->ManagerSharedLink);
	Resource->Shared = false;
	
	for (ZESize Pool = 0; Pool < ZERS_MEMORY_POOL_COUNT; Pool++)
		MemoryUsageShared[Pool] -= Resource->MemoryUsage[Pool];
}

void ZERSResourceManager::DestroyResource(const ZERSResource* Resource)
{
	ManagerLock.Lock();
	if (Resource->IsShared())
		UnshareResourceInternal(const_cast<ZERSResource*>(Resource));
		UnregisterResourceInternal(const_cast<ZERSResource*>(Resource));
	ManagerLock.Unlock();
	delete Resource;
}

ZERSResourceManager::ZERSResourceManager()
{
	memset(MemoryUsage, 0, sizeof(MemoryUsage));
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

ZESize ZERSResourceManager::GetMemoryUsage(ZERSMemoryPool Pool)
{
	zeCheckError(Pool >= ZERS_MEMORY_POOL_COUNT, 0, "Unknown memory pool.");
	return MemoryUsage[Pool];
}

ZESize ZERSResourceManager::GetMemoryUsageShared(ZERSMemoryPool Pool)
{
	zeCheckError(Pool >= ZERS_MEMORY_POOL_COUNT, 0, "Unknown memory pool.");
	return MemoryUsageShared[Pool];
}

ZESize ZERSResourceManager::GetMemoryUsageTotal(ZERSMemoryPool Pool)
{
	return GetMemoryUsage(Pool) + GetMemoryUsageShared(Pool);
}

ZERSHolder<const ZERSResource> ZERSResourceManager::GetResource(const ZEGUID& GUID)
{
	ManagerLock.Lock();
	ZERSHolder<const ZERSResource> Resource = GetResourceInternal(GUID);
	ManagerLock.Unlock();

	return Resource;
}

ZERSHolder<const ZERSResource> ZERSResourceManager::GetResource(const ZEString& FileName)
{
	ManagerLock.Lock();
	ZERSHolder<const ZERSResource> Resource = GetResourceInternal(FileName);
	ManagerLock.Unlock();

	return Resource;
}

void ZERSResourceManager::RegisterResource(ZERSResource* Resource)
{
	ManagerLock.Lock();
	RegisterResourceInternal(Resource);
	ManagerLock.Unlock();
}

void ZERSResourceManager::UnregisterResource(ZERSResource* Resource)
{
	ManagerLock.Lock();
	UnregisterResourceInternal(Resource);
	ManagerLock.Unlock();
}

void ZERSResourceManager::ShareResource(ZERSResource* Resource)
{
	ManagerLock.Lock();
	ShareResourceInternal(Resource);
	ManagerLock.Unlock();
}

void ZERSResourceManager::UnshareResource(ZERSResource* Resource)
{
	ManagerLock.Lock();
	UnshareResourceInternal(Resource);
	ManagerLock.Unlock();
}

ZERSHolder<const ZERSResource> ZERSResourceManager::StageResource(const ZEGUID& GUID, ZERSInstanciator Insanciator, ZERSResource** StagingResource)
{
	ManagerLock.Lock();
	ZERSHolder<const ZERSResource> Resource = GetResourceInternal(GUID);
	if (Resource == NULL)
	{
		ZERSResource* NewResouce = Insanciator();
		if (NewResouce == NULL)
		{
			ManagerLock.Unlock();
			return NULL;
		}

		NewResouce->SetGUID(GUID);
		NewResouce->State = ZERS_RS_STAGING;
		NewResouce->TargetState = ZERS_RS_STAGED;
		RegisterResourceInternal(NewResouce);
		ShareResourceInternal(NewResouce);
		ManagerLock.Unlock();

		if (StagingResource != NULL)
			*StagingResource = NewResouce;
		else
			NewResouce->StagingRealized();

		return NewResouce;
	}
	else
	{
		ManagerLock.Unlock();
		*StagingResource = NULL;
		return Resource;
	}
}

ZERSHolder<const ZERSResource> ZERSResourceManager::StageResource(const ZEString& FileName, ZERSInstanciator Insanciator, ZERSResource** StagingResource)
{
	ManagerLock.Lock();
	const ZERSResource* Resource = GetResourceInternal(FileName);
	if (Resource == NULL)
	{
		ZERSResource* NewResouce = Insanciator();
		if (NewResouce == NULL)
		{
			ManagerLock.Unlock();
			return NULL;
		}

		NewResouce->FileName = ZEPathInfo(FileName).Normalize();
		NewResouce->FileNameHash = NewResouce->FileName.Lower().Hash();
		NewResouce->State = ZERS_RS_STAGING;
		NewResouce->TargetState = ZERS_RS_STAGED;
		RegisterResourceInternal(NewResouce);
		ShareResourceInternal(NewResouce);
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
