//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERSResource.cpp
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

#include "ZERSResource.h"

#include "ZERSResourceManager.h"
#include "ZEFile\ZEPathInfo.h"


void ZERSResource::Reference() const
{
	zeDebugCheck(Manager == NULL, "Resource is not registered.");

	ReferenceCountLock.Lock();
	if (Parent != NULL)
		Parent->Reference();
	ReferenceCount++;
	ReferenceCountLock.Unlock();
}

void ZERSResource::Release() const
{
	ReferenceCountLock.Lock();
	ReferenceCount--;
	if (Parent != NULL)
	{
		Parent->Release();
	}
	else
	{
		if (ReferenceCount <= 0)
			Destroy();
	}
	ReferenceCountLock.Unlock();
}

void ZERSResource::Destroy() const
{
	ZERSResource* Resource = const_cast<ZERSResource*>(this);
	
	if (Shared)
		Resource->Unshare();

	while(ChildResources.GetCount() != 0)
		ChildResources.GetFirstItem()->Destroy();

	if (Parent != NULL)
		Parent->RemoveChildResource(Resource);

	Resource->TargetState = ZERS_RS_DESTROYED;
	Resource->PreDestroy();
	Resource->UpdateStateTask.Run();
}

void ZERSResource::UpdateMemoryConsumption()
{
	ResourceLock.Lock();
	{
		ZESize OldMemoryUsage[ZERS_MEMORY_POOL_COUNT];
		memcpy(OldMemoryUsage, MemoryUsage, sizeof(MemoryUsage));
		memset(MemoryUsage, 0, sizeof(MemoryUsage));

		for (ZEUInt Pool = 0 ; Pool < ZERS_MEMORY_POOL_COUNT; Pool++)
		{
			for (ZESize I = 0; I < ChildResources.GetCount(); I++)
				MemoryUsage[Pool] += ChildResources[I]->GetMemoryUsage((ZERSMemoryPool)Pool);

			MemoryUsage[Pool] += MemoryUsageSelf[Pool];
		}

		if (Parent != NULL)
		{
			Parent->UpdateMemoryConsumption();
		}
		else
		{
			ZESSize MemoryUsageDelta[ZERS_MEMORY_POOL_COUNT];
			for (ZESize Pool = 0; Pool < ZERS_MEMORY_POOL_COUNT; Pool++)
				MemoryUsageDelta[Pool] = (ZESSize)MemoryUsage[Pool] - (ZESSize)OldMemoryUsage[Pool];

			ZERSResourceManager::GetInstance()->UpdateMemoryUsage(this, MemoryUsageDelta);
		}
	}
	ResourceLock.Unlock();
}

ZETaskResult ZERSResource::UpdateStateFunction(ZETaskThread* TaskThread, void* Parameters)
{
	if (State == ZERS_RS_NONE)
	{
		if (TargetState == ZERS_RS_NONE)
		{
			return ZE_TR_DONE;
		}
		else if (TargetState == ZERS_RS_STAGED)
		{
			return ZE_TR_COOPERATING;
		}
		else if (TargetState == ZERS_RS_LOADED)
		{
			return ZE_TR_COOPERATING;
		}
		else if (TargetState == ZERS_RS_DESTROYED)
		{
			State = ZERS_RS_DESTROYING;
		}
	}
	else if (State == ZERS_RS_STAGED)
	{
		if (TargetState == ZERS_RS_STAGED)
		{
			return ZE_TR_DONE;
		}
		else if (TargetState == ZERS_RS_LOADED)
		{
			LoadInternalDone = false;
			State = ZERS_RS_LOADING;
		}
		else if (TargetState == ZERS_RS_DESTROYED)
		{
			State = ZERS_RS_DESTROYING;
		}
	}
	else if (State == ZERS_RS_LOADED)
	{
		 if (TargetState == ZERS_RS_LOADED)
		 {
			 return ZE_TR_DONE;
		 }
		 else if (TargetState == ZERS_RS_STAGED)
		 {
			 UnloadInternalDone = false;
			 State = ZERS_RS_UNLOADING;
		 }
		 else if (TargetState == ZERS_RS_DESTROYED)
		 {
			 State = ZERS_RS_DESTROYING;
		 }
	}
	else if (State < 0)
	{
		if (TargetState == ZERS_RS_DESTROYED)
			TargetState = ZERS_RS_DESTROYING;
		else
			return ZE_TR_DONE;
	}

	if (State == ZERS_RS_LOADING)
	{
		if (!LoadInternalDone)
		{
			ZETaskResult Result = LoadInternal();
			if (Result == ZE_TR_DONE)
			{
				zeLog("Resource loaded. Resource Class: \"%s\", File Name: \"%s\"", GetClass()->GetName(), this->FileName.ToCString());
				LoadInternalDone = true;
				SetLoadProgress(100);
			}
			else if (Result == ZE_TR_FAILED)
			{
				zeError("Resource loading failed. Resource Class: \"%s\", File Name: \"%s\"", GetClass()->GetName(), this->FileName.ToCString());
				State = ZERS_RS_ERROR_LOADING;
				return ZE_TR_COOPERATING;
			}
			else
			{
				return Result;
			}
		}

		ze_for_each(ChildResource, ChildResources)
		{
			ZERSResourceState ChildResourceState = (*ChildResource)->State;
			if (ChildResourceState < 0)
			{
				State = ZERS_RS_ERROR_LOADING;
				return ZE_TR_COOPERATING;
			}
			else if (ChildResourceState <= ZERS_RS_LOADING)
			{
				return ZE_TR_COOPERATING;
			}
		}

		ze_for_each(ExternalResource, ExternalResources)
		{
			if ((*ExternalResource)->State <= ZERS_RS_LOADING)
				return ZE_TR_COOPERATING;
		}

		State = ZERS_RS_LOADED;
		return ZE_TR_COOPERATING;
	}
	else if (State == ZERS_RS_UNLOADING)
	{
		if (!UnloadInternalDone)
		{
			ZETaskResult Result = UnloadInternal();
			if (Result == ZE_TR_DONE)
			{
				zeLog("Resource unloaded. Resource Class: \"%s\", File Name: \"%s\"", GetClass()->GetName(), this->FileName.ToCString());
				SetLoadProgress(0);
				UnloadInternalDone = true;
			}
			else if (Result == ZE_TR_FAILED)
			{
				zeError("Resource unloading failed. Resource Class: \"%s\", File Name: \"%s\"", GetClass()->GetName(), this->FileName.ToCString());			
				State = ZERS_RS_ERROR_UNLOADING;
				return ZE_TR_COOPERATING;
			}
			else
			{
				return Result;
			}
		}

		ze_for_each(ChildResource, ChildResources)
		{
			ZERSResourceState ChildResourceState = (*ChildResource)->State;
			if (ChildResourceState < 0)
			{
				State = ZERS_RS_ERROR_UNLOADING;
				return ZE_TR_COOPERATING;
			}
			else if (State > ZERS_RS_STAGED)
			{
				return ZE_TR_COOPERATING;
			}
		}

		State = ZERS_RS_STAGED;
		return ZE_TR_COOPERATING;
	}
	else if (State == ZERS_RS_STAGING)
	{
		if (TargetState > ZERS_RS_STAGED)
			return ZE_TR_COOPERATING;
	}
	else if (State == ZERS_RS_DESTROYING)
	{
		delete this;
		return ZE_TR_DONE;
	}

	return ZE_TR_DONE;
}


void ZERSResource::SetMemoryUsage(ZERSMemoryPool Pool, ZESize Size)
{
	MemoryUsageSelf[Pool] = Size;
	UpdateMemoryConsumption();
}

void ZERSResource::SetLoadProgress(ZEUInt Percentage)
{
	if (Percentage > 100)
		Percentage = 100;

	LoadProgress = Percentage;
}

void ZERSResource::SetLoadProgress(ZESize Index, ZESize Count, ZEUInt StartPercentage, ZEUInt EndPercentage)
{
	if (Count == 0)
	{
		SetLoadProgress(0);
		return;
	}

	SetLoadProgress(StartPercentage + (Index + 1) * (EndPercentage - StartPercentage) / Count);
}

void ZERSResource::AddChildResource(ZERSResource* ChildResource)
{
	zeCheckError(ChildResource == NULL, ZE_VOID, "Cannot add child resource. ChildResource is NULL.");
	zeCheckError(ChildResource->Parent != NULL, ZE_VOID, "Cannot add child resource. Resource is already child resource of a resource.");

	ChildResources.Add(ChildResource);
	ChildResource->Parent = this;

	if (Parent != NULL)
		Parent->UpdateMemoryConsumption();
}

void ZERSResource::RemoveChildResource(ZERSResource* ChildResource)
{
	zeCheckError(ChildResource == NULL, ZE_VOID, "Cannot remove child resource. ChildResource is NULL.");
	zeCheckError(ChildResource->Parent != NULL, ZE_VOID, "Cannot remove child resource. Child resource is not owned by this resource.");

	ChildResources.RemoveValue(ChildResource);
	ChildResource->Parent = NULL;

	if (Parent != NULL)
		Parent->UpdateMemoryConsumption();
}

void ZERSResource::RegisterExternalResource(const ZERSResource* Resource)
{
	if (Resource == NULL)
		return;

	zeCheckError(ExternalResources.Exists(Resource), ZE_VOID, "Resource is already added as external resource.");
	ExternalResources.Add(Resource);
}

void ZERSResource::UnregisterExternalResource(const ZERSResource* Resource)
{
	if (Resource == NULL)
		return;

	ExternalResources.RemoveValue(Resource);
}


void ZERSResource::Register()
{
	if (Manager != NULL)
		return;

	ZERSResourceManager::GetInstance()->RegisterResource(this);
}

void ZERSResource::Unregister()
{
	if (Manager == NULL)
		return;

	ZERSResourceManager::GetInstance()->UnregisterResource(this);
}

ZETaskResult ZERSResource::LoadInternal()
{
	return ZE_TR_DONE;
}

ZETaskResult ZERSResource::UnloadInternal()
{
	return ZE_TR_DONE;
}

void ZERSResource::PreDestroy()
{

}

ZERSResource::ZERSResource() : ManagerLink(this), ManagerSharedLink(this)
{
	Manager = NULL;
	Group = NULL;
	GUID = ZEGUID::Zero;
	State = ZERS_RS_NONE;
	Parent = NULL;
	Shared = false;
	ReferenceCount = 0;
	memset(MemoryUsage, 0, sizeof(MemoryUsage));
	memset(MemoryUsageSelf, 0, sizeof(MemoryUsage));
	TargetState = ZERS_RS_NONE;
	FileNameHash = 0;
	LoadProgress = 0;
	LoadInternalDone = false;
	UnloadInternalDone = false;
	Destroying = false;
	UpdateStateTask.SetPool(ZE_TPI_IO);
	UpdateStateTask.SetFunction(ZEDelegateMethod(ZETaskFunction, ZERSResource, UpdateStateFunction, this));
}

ZERSResource::~ZERSResource()
{
	if (Parent != NULL)
		Parent->RemoveChildResource(this);

	zeDebugCheck(ManagerSharedLink.GetInUse(), "Resource is shared.");
	zeDebugCheck(Manager != NULL || ManagerLink.GetInUse(), "Resource is not registered.");

	ZERSResourceManager::GetInstance()->UnregisterResource(this);

	State = ZERS_RS_DESTROYED;
}

ZERSResource* ZERSResource::GetParent()
{
	return Parent;
}

const ZERSResource* ZERSResource::GetParent() const
{
	return Parent;
}

const ZERSResourceManager* ZERSResource::GetManager() const
{
	return Manager;
}

const ZERSResourceGroup* ZERSResource::GetGroup() const
{
	return Group;
}

void ZERSResource::SetGUID(const ZEGUID& GUID)
{
	zeCheckError(Shared, ZE_VOID, "You cannot change enlisted resource's GUID.");
	this->GUID = GUID;
}

const ZEGUID& ZERSResource::GetGUID() const
{
	return GUID;
}

const ZEString& ZERSResource::GetFileName() const
{
	return FileName;
}

ZEUInt32 ZERSResource::GetFileNameHash() const
{
	return FileNameHash;
}

ZERSResourceState ZERSResource::GetState() const
{
	return State;
}

ZESize ZERSResource::GetReferenceCount() const
{
	return ReferenceCount;
}

ZESize ZERSResource::GetMemoryUsage(ZERSMemoryPool Pool) const
{
	zeCheckError(Pool >= ZERS_MEMORY_POOL_COUNT, 0, "Unknown memory pool.");
	return MemoryUsage[Pool];
}

ZESize ZERSResource::GetTotalMemoryUsage() const
{
	ZESize Total = 0;
	for (ZESize Pool = 0; Pool < ZERS_MEMORY_POOL_COUNT; Pool++)
		Total += GetMemoryUsage((ZERSMemoryPool)Pool);

	return Total;
}

ZEUInt ZERSResource::GetLoadProgress() const
{
	ResourceLock.Lock();

	ZEUInt TotalProgress = LoadProgress;
	ZEUInt TotalItems = 1;

	ze_for_each(ExternalResource, ExternalResources)
	{
		TotalProgress += (*ExternalResource)->GetLoadProgress();
		TotalItems++;
	}

	ZESize LoadableCount = 0;
	ze_for_each(ChildResource, ChildResources)
	{
		TotalProgress += (*ChildResource)->GetLoadProgress();
		TotalItems++;
	}

	ResourceLock.Unlock();

	return TotalProgress / TotalItems;
}

bool ZERSResource::IsStaged() const
{
	return State >= ZERS_RS_STAGED;
}

bool ZERSResource::IsLoaded() const
{
	return State >= ZERS_RS_LOADED;
}


bool ZERSResource::IsLoadedOrLoading() const
{
	return State >= ZERS_RS_LOADING;
}

bool ZERSResource::IsFailed() const
{
	return State <= 0;
}

bool ZERSResource::IsShared() const
{
	return Shared;
}

void ZERSResource::Share()
{
	zeDebugCheck(Manager == NULL, "Resource is not registered.");

	if (IsShared() || TargetState == ZERS_RS_DESTROYED)
		return;

	zeCheckError(GUID == ZEGUID::Zero, ZE_VOID, "Cannot share resource. Ilvalid GUID.");
	ZERSResourceManager::GetInstance()->ShareResource(this);
}

void ZERSResource::Unshare()
{
	if (!IsShared())
		return;

	ZERSResourceManager::GetInstance()->UnshareResource(this);
}

void ZERSResource::StagingRealized()
{
	if (State == ZERS_RS_NONE || State == ZERS_RS_STAGING)
		State = ZERS_RS_STAGED;
}

void ZERSResource::StagingFailed()
{
	if (State == ZERS_RS_STAGING)
		State = ZERS_RS_ERROR_STAGING;
}

void ZERSResource::Load(const ZEString& FileName)
{
	zeDebugCheck(Manager == NULL, "Resource is not registered.");

	if (TargetState == ZERS_RS_DESTROYED)
		return;

	if (State == ZERS_RS_LOADING || State == ZERS_RS_LOADED)
		return;

	if (Destroying)
		return;

	this->FileName = ZEPathInfo(FileName).Normalize();
	FileNameHash = this->FileName.Lower().Hash();
	zeLog("Loading resource. Resource Class: \"%s\", File Name: \"%s\"", GetClass()->GetName(), this->FileName.ToCString());

	TargetState = ZERS_RS_LOADED;
	UpdateStateTask.Run();
}

void ZERSResource::Unload()
{
	zeDebugCheck(Manager == NULL, "Resource is not registered.");

	if (State < ZERS_RS_STAGED)
		return;

	if (Destroying)
		return;

	zeLog("Unloading resource. Resource Class: \"%s\", File Name: \"%s\".", GetClass()->GetName(), this->FileName.ToCString());
	TargetState = ZERS_RS_STAGED;
	UpdateStateTask.Run();
}

void ZERSResource::WaitStaging() const
{
	while(State < ZERS_RS_STAGED && State > 0);
}

void ZERSResource::WaitLoading() const
{
	while(State < ZERS_RS_LOADED && State > 0);
}

void ZERSResource::WaitUnloading() const
{
	while(State >= ZERS_RS_STAGED);
}
