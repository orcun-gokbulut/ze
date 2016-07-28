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
#include "ZERSResourceGroup.h"
#include "ZEError.h"
#include "ZEFile/ZEPathInfo.h"
#include "ZEFile/ZEFileInfo.h"

static __declspec(thread) bool StagingInstanciator = false;

void ZERSResourceManager::UpdateMemoryUsage(ZERSResource* Resource, ZESSize MemoryUsageDelta[ZERS_MEMORY_POOL_COUNT])
{
	ManagerLock.Lock();

	ZERSResourceGroup* Group = GetResourceGroupInternal(Resource->GetClass());
	zeDebugCheck(Group == NULL, "Resource group corruption.");

	Group->UpdateMemoryUsage(Resource, MemoryUsageDelta);
	
	for (ZESize Pool = 0; Pool < ZERS_MEMORY_POOL_COUNT; Pool++)
	{
		MemoryUsage[Pool] += MemoryUsageDelta[Pool];
		if (Resource->IsShared())
			MemoryUsageShared[Pool] += MemoryUsageDelta[Pool];
	}
	ManagerLock.Unlock();
}

ZERSResourceGroup* ZERSResourceManager::CreateResourceGroup(ZEClass* ResourceClass)
{
	ZERSResourceGroup* ParentGroup = NULL;

	if (ResourceClass != ZERSResource::Class())
	{
		ParentGroup = GetResourceGroupInternal(ResourceClass->GetParentClass());
		if (ParentGroup == NULL)
			ParentGroup = CreateResourceGroup(ResourceClass->GetParentClass());
	}

	ZERSResourceGroup* ResourceGroup = new ZERSResourceGroup();
	ResourceGroup->ResourceClass = ResourceClass;
	ResourceGroup->Parent = ParentGroup;
	ResourceGroups.Add(ResourceGroup);

	if (ResourceGroup->Parent != NULL)
		ResourceGroup->Parent->ChildGroups.Add(ResourceGroup);
	
	return ResourceGroup;
}

ZERSResourceGroup* ZERSResourceManager::GetResourceGroupInternal(ZEClass* ResourceClass)
{
	for (ZESize I = 0; I < ResourceGroups.GetCount(); I++)
	{
		if (ResourceGroups[I]->GetResourceClass() == ResourceClass)
			return ResourceGroups[I];
	}

	return NULL;
}

ZERSHolder<const ZERSResource> ZERSResourceManager::GetResourceInternal(ZEClass* ResourceClass, const ZEString& FileName)
{
	ZERSResourceGroup* Group = GetResourceGroupInternal(ResourceClass);
	if (Group == NULL)
		return NULL;

	ZEFileInfo FileInfo(FileName);
	ZEString FileNameNormalized = FileInfo.Normalize();
	if (FileNameNormalized.IsEmpty())
		return NULL;

	ZEUInt64 Hash = FileName.Lower().Hash();
	ze_for_each(Resource, Group->SharedResources)
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

ZERSHolder<const ZERSResource> ZERSResourceManager::GetResourceInternal(ZEClass* ResourceClass, const ZEGUID& GUID)
{
	ZERSResourceGroup* Group = GetResourceGroupInternal(ResourceClass);
	if (Group == NULL)
		return NULL;
	
	ze_for_each(Resource, Group->SharedResources)
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
	if (Resource->Manager != NULL)
		return;

	zeDebugCheck(Resource->Manager != NULL, "Resource is already registered.");

	ZERSResourceGroup* Group = GetResourceGroupInternal(Resource->GetClass());
	if (Group == NULL)
	{
		Group = CreateResourceGroup(Resource->GetClass());
		zeDebugCheck(Group == NULL, "Resource group corruption.");
	}

	Resource->Manager = this;
	Resource->Group = Group;
	ResourceCount++;

	Group->RegisterResource(Resource);

	for (ZESize Pool = 0; Pool < ZERS_MEMORY_POOL_COUNT; Pool++)
		MemoryUsage[Pool] += Resource->MemoryUsage[Pool];
}

void ZERSResourceManager::UnregisterResourceInternal(ZERSResource* Resource)
{
	if (Resource->Manager == NULL)
		return;

	ZERSResourceGroup* Group = GetResourceGroupInternal(Resource->GetClass());
	zeDebugCheck(Group == NULL, "Resource group corruption.");
	
	Resource->Manager = NULL;
	Resource->Group = NULL;
	ResourceCount--;

	Group->UnregisterResource(Resource);

	for (ZESize Pool = 0; Pool < ZERS_MEMORY_POOL_COUNT; Pool++)
		MemoryUsage[Pool] -= Resource->MemoryUsage[Pool];
}

void ZERSResourceManager::ShareResourceInternal(ZERSResource* Resource)
{
	ZERSResourceGroup* Group = GetResourceGroupInternal(Resource->GetClass());
	zeDebugCheck(Group == NULL, "Resource group corruption.");

	Group->ShareResource(Resource);
	Resource->Shared = true;
	SharedResourceCount++;
	
	for (ZESize Pool = 0; Pool < ZERS_MEMORY_POOL_COUNT; Pool++) 
		MemoryUsageShared[Pool] += Resource->MemoryUsage[Pool];
}

void ZERSResourceManager::UnshareResourceInternal(ZERSResource* Resource)
{
	if (!Resource->IsShared())
		return;
		
	ZERSResourceGroup* Group = GetResourceGroupInternal(Resource->GetClass());
	zeDebugCheck(Group == NULL, "Resource group corruption.");

	Group->UnshareResource(Resource);
	Resource->Shared = false;
	SharedResourceCount--;

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

ZEArray<const ZERSResourceGroup*> ZERSResourceManager::GetResourceGroups()
{
	ManagerLock.Lock();
	ZEArray<const ZERSResourceGroup*> Snapshot;
	Snapshot.MassAdd(ResourceGroups.GetCArray(), ResourceGroups.GetCount());
	ManagerLock.Unlock();

	return Snapshot;
}

ZESize ZERSResourceManager::GetResourceCount()
{
	return ResourceCount;
}

ZESize ZERSResourceManager::GetSharedResourceCount()
{
	return SharedResourceCount;
}

ZESize ZERSResourceManager::GetMemoryUsage(ZERSMemoryPool Pool)
{
	zeCheckError(Pool >= ZERS_MEMORY_POOL_COUNT, 0, "Cannot get memory usage. Unknown memory pool. Memory Pool: %s.", ZERSMemoryPool_Declaration()->ToText(Pool).ToCString());
	return MemoryUsage[Pool];
}

ZESize ZERSResourceManager::GetMemoryUsageShared(ZERSMemoryPool Pool)
{
	zeCheckError(Pool >= ZERS_MEMORY_POOL_COUNT, 0, "Cannot get memory usage. Unknown memory pool. Memory Pool: %s.", ZERSMemoryPool_Declaration()->ToText(Pool).ToCString());
	return MemoryUsageShared[Pool];
}

ZESize ZERSResourceManager::GetMemoryUsageTotal(ZERSMemoryPool Pool)
{
	return GetMemoryUsage(Pool) + GetMemoryUsageShared(Pool);
}

ZERSHolder<const ZERSResource> ZERSResourceManager::GetResource(ZEClass* ResourceClass, const ZEGUID& GUID)
{
	zeCheckError(ResourceClass == NULL, NULL, "Cannot get resource Resource Class is NULL.");

	ManagerLock.Lock();
	ZERSHolder<const ZERSResource> Resource = GetResourceInternal(ResourceClass, GUID);
	ManagerLock.Unlock();

	return Resource;
}

ZERSHolder<const ZERSResource> ZERSResourceManager::GetResource(ZEClass* ResourceClass, const ZEString& FileName)
{
	zeCheckError(ResourceClass == NULL, NULL, "Cannot get resource. Resource Class is NULL.");

	ManagerLock.Lock();
	ZERSHolder<const ZERSResource> Resource = GetResourceInternal(ResourceClass, FileName);
	ManagerLock.Unlock();

	return Resource;
}

void ZERSResourceManager::RegisterResource(ZERSResource* Resource)
{
	if (!StagingInstanciator)
	{
		ManagerLock.Lock();
		RegisterResourceInternal(Resource);
		ManagerLock.Unlock();
	}
	else
	{
		RegisterResourceInternal(Resource);
	}
}

void ZERSResourceManager::UnregisterResource(ZERSResource* Resource)
{
	ManagerLock.Lock();
	UnregisterResourceInternal(Resource);
	ManagerLock.Unlock();
}

void ZERSResourceManager::ShareResource(ZERSResource* Resource)
{
	if (Resource == NULL)
		return;

	ManagerLock.Lock();
	ShareResourceInternal(Resource);
	ManagerLock.Unlock();
}

void ZERSResourceManager::UnshareResource(ZERSResource* Resource)
{
	if (Resource == NULL)
		return;

	ManagerLock.Lock();
	UnshareResourceInternal(Resource);
	ManagerLock.Unlock();
}

ZERSHolder<const ZERSResource> ZERSResourceManager::StageResource(ZEClass* ResourceClass, const ZEGUID& GUID, ZERSInstanciator Insanciator, const void* InstanciatorParameter, ZERSResource** StagingResource)
{
	zeCheckError(ResourceClass == NULL, NULL, "Cannot stage resource. Resource Class is NULL.");

	ManagerLock.Lock();
	ZERSHolder<const ZERSResource> Resource = GetResourceInternal(ResourceClass, GUID);
	if (Resource == NULL)
	{
		StagingInstanciator = true;
		ZERSResource* NewResouce = Insanciator(InstanciatorParameter);
		StagingInstanciator = false;

		if (NewResouce == NULL)
		{
			zeError("Cannot stage resource. Instanciator returns NULL. Resource Class: \"%s\".", ResourceClass->GetName());
			ManagerLock.Unlock();
			return NULL;
		}

		if (NewResouce->GetClass() != ResourceClass)
		{
			zeError("Cannot stage resource. Invalid Instanciator. Resource Class: \"%s\".", ResourceClass->GetName());
			ManagerLock.Unlock();	 
			return NULL;
		}

		NewResouce->SetGUID(GUID);
		NewResouce->State = ZERS_RS_STAGING;
		NewResouce->TargetState = ZERS_RS_STAGED;
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

ZERSHolder<const ZERSResource> ZERSResourceManager::StageResource(ZEClass* ResourceClass, const ZEString& FileName, ZERSInstanciator Insanciator, const void* InstanciatorParameter, ZERSResource** StagingResource)
{
	zeCheckError(ResourceClass == NULL, NULL, "Cannot stage resource. Resource Class is NULL.");

	ManagerLock.Lock();
	const ZERSResource* Resource = GetResourceInternal(ResourceClass, FileName);
	if (Resource == NULL)
	{
		StagingInstanciator = true;
		ZERSResource* NewResouce = Insanciator(InstanciatorParameter);
		StagingInstanciator = false;

		if (NewResouce == NULL)
		{
			zeError("Cannot stage resource. Instanciator returns NULL. Resource Class: \"%s\".", ResourceClass->GetName());
			ManagerLock.Unlock();
			return NULL;
		}

		if (NewResouce->GetClass() != ResourceClass)
		{
			zeError("Cannot stage resource. Invalid Instanciator. Resource Class: \"%s\".", ResourceClass->GetName());
			ManagerLock.Unlock();	 
			return NULL;
		}

		NewResouce->FileName = ZEPathInfo(FileName).Normalize();
		NewResouce->FileNameHash = NewResouce->FileName.Lower().Hash();
		NewResouce->State = ZERS_RS_STAGING;
		NewResouce->TargetState = ZERS_RS_STAGED;
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
