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


ZEString ZERSResourceIdentifier::ToString() const
{
	return ZEString::Empty;
}

void ZERSResource::Reference() const
{
	zeDebugCheck(Manager == NULL, "Resource is not registered.");
	zeDebugCheck(TargetState == ZERS_RS_DESTROYED, "Resource is destroyed but referenced.");

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
		{
			Destroy();
			return;
		}
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
	ReferenceCountLock.Unlock();
	Resource->UpdateState();
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

bool ZERSResource::UpdateStateSerial()
{
	ZETaskResult Result = ZE_TR_COOPERATING;

	StateLock.Lock();
	{
		while (Result == ZE_TR_COOPERATING)
			Result = UpdateStateFunction(NULL, 0, NULL);
	}
	StateLock.Unlock();

	return (Result == ZE_TR_DONE);
}

void ZERSResource::UpdateState()
{
	UpdateStateTask.Run();
}

ZETaskResult ZERSResource::UpdateStateFunction(ZETaskThread* TaskThread, ZESize InstanceIndex, void* Parameters)
{
	if (State == ZERS_RS_NONE)
	{
		ReloadFlag = false;
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
			StagingRealized();
			return ZE_TR_COOPERATING;
		}
		else if (TargetState == ZERS_RS_DESTROYED)
		{
			State = ZERS_RS_DESTROYING;
		}
	}
	else if (State == ZERS_RS_STAGED)
	{
		ReloadFlag = false;
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
		else if (ReloadFlag == true && TargetState == ZERS_RS_STAGED)
		{
			UnloadInternalDone = false;
			State = ZERS_RS_UNLOADING;
		}
		else if (TargetState == ZERS_RS_DESTROYED)
		 {
			 State = ZERS_RS_UNLOADING;
		}
	}
	else if (State < 0)
	{
		if (TargetState == ZERS_RS_DESTROYED)
		{
			TargetState = ZERS_RS_DESTROYING;
		}
		else
		{
			return ZE_TR_DONE;
		}
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
				SetLocalLoadProgress(100);
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
			else if (ChildResourceState == ZERS_RS_LOADING)
			{
				return ZE_TR_COOPERATING;
			}
		}

		ze_for_each(ExternalResource, ExternalResources)
		{
			if ((*ExternalResource)->IsFailed())
				return ZE_TR_FAILED;
			else if (!(*ExternalResource)->IsLoaded())
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
				SetLocalLoadProgress(0);
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

void ZERSResource::SetLocalLoadProgress(ZEUInt Percentage)
{
	if (Percentage > 100)
		Percentage = 100;

	LocalLoadProgress = Percentage;
}

void ZERSResource::SetLocalLoadProgress(ZESize Index, ZESize Count, ZEUInt StartPercentage, ZEUInt EndPercentage)
{
	if (Count == 0)
	{
		SetLocalLoadProgress(0);
		return;
	}

	SetLocalLoadProgress(StartPercentage + (Index + 1) * (EndPercentage - StartPercentage) / Count);
}

ZEUInt ZERSResource::GetLocalLoadProgress() const
{
	return LocalLoadProgress;
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

	ResourceLock.Lock();

	if (ExternalResources.Exists(Resource))
	{
		ResourceLock.Unlock();
		return;
	}

	ExternalResources.Add(Resource);
	
	ResourceLock.Unlock();
}

void ZERSResource::UnregisterExternalResource(const ZERSResource* Resource)
{
	if (Resource == NULL)
		return;

	ResourceLock.Lock();
	ExternalResources.RemoveValue(Resource);
	ResourceLock.Unlock();
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
	Index = 0;
	Manager = NULL;
	Group = NULL;
	GUID = ZEGUID::Zero;
	Identifier = NULL;
	State = ZERS_RS_NONE;
	Parent = NULL;
	Shared = false;
	ReferenceCount = 0;
	memset(MemoryUsage, 0, sizeof(MemoryUsage));
	memset(MemoryUsageSelf, 0, sizeof(MemoryUsage));
	TargetState = ZERS_RS_NONE;
	FileNameHash = 0;
	LocalLoadProgress = 0;
	LoadInternalDone = false;
	UnloadInternalDone = false;
	ReloadFlag = false;
	UpdateStateTask.SetPoolId(ZE_TPI_CONCURENT);
	UpdateStateTask.SetFunction(ZEDelegateMethod(ZETaskFunction, ZERSResource, UpdateStateFunction, this));
}

ZERSResource::~ZERSResource()
{
	if (Parent != NULL)
		Parent->RemoveChildResource(this);

	zeDebugCheck(ManagerSharedLink.GetInUse(), "Resource is not unshared.");
	zeDebugCheck(Manager != NULL || ManagerLink.GetInUse(), "Resource is not unregistered. (Check class destructor)");

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


ZESize ZERSResource::GetIndex() const
{
	return Index;
}

void ZERSResource::SetGUID(const ZEGUID& GUID)
{
	zeCheckError(Shared, ZE_VOID, "You cannot change enlisted resource's GUID.");
	this->GUID = GUID;
}

void ZERSResource::SetFileName(const ZEString& FileName)
{
	this->FileName = FileName;
}

void ZERSResource::SetIdentifier(const ZERSResourceIdentifier* Identifier)
{
	this->Identifier = Identifier;
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

const ZERSResourceIdentifier* ZERSResource::GetIdentifier() const
{
	return Identifier;
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

	ZEUInt TotalProgress = LocalLoadProgress;
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

bool ZERSResource::IsDestroyed() const
{
	return TargetState == ZERS_RS_DESTROYED || TargetState == ZERS_RS_DESTROYING;
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

void ZERSResource::Load()
{
	zeCheckError(IsDestroyed(), ZE_VOID, "Cannot load destroyed resource.");
	zeDebugCheck(Manager == NULL, "Resource is not registered.");

	if (IsLoadedOrLoading())
		return;

	ResourceLock.Lock();
	this->FileName = ZEPathInfo(FileName).Normalize();
	FileNameHash = this->FileName.Lower().Hash();
	TargetState = ZERS_RS_LOADED;
	ResourceLock.Unlock();

	zeLog("Loading resource. Resource Class: \"%s\", File Name: \"%s\"", GetClass()->GetName(), this->FileName.ToCString());
	UpdateState();
}

void ZERSResource::Unload()
{
	if (IsDestroyed())
		return;

	zeDebugCheck(Manager == NULL, "Resource is not registered.");

	if (State < ZERS_RS_STAGED)
		return;

	zeLog("Unloading resource. Resource Class: \"%s\", File Name: \"%s\".", GetClass()->GetName(), this->FileName.ToCString());
	TargetState = ZERS_RS_STAGED;
	UpdateState();
}

void ZERSResource::Reload()
{
	zeCheckError(IsDestroyed(), ZE_VOID, "Cannot reload destroyed resource.");
	zeCheckError(IsShared(), ZE_VOID, "Cannot reload shared resource.");
	zeDebugCheck(Manager == NULL, "Resource is not registered.");

	ReloadFlag = true;

	if (TargetState <= ZERS_RS_LOADED)
		TargetState = ZERS_RS_LOADED;

	UpdateState();
}

bool ZERSResource::LoadSerial()
{
	zeCheckError(IsDestroyed(), false, "Cannot serial load destroyed resource.");
	zeCheckError(IsShared(), false, "Cannot serial load shared resource.");
	zeDebugCheck(Manager == NULL, "Resource is not registered.");

	if (IsLoaded())
		return true;

	ResourceLock.Lock();
	this->FileName = ZEPathInfo(FileName).Normalize();
	FileNameHash = this->FileName.Lower().Hash();
	TargetState = ZERS_RS_LOADED;
	ResourceLock.Unlock();

	zeLog("Loading resource. Resource Class: \"%s\", File Name: \"%s\"", GetClass()->GetName(), this->FileName.ToCString());
	return UpdateStateSerial();
}

bool ZERSResource::UnloadSerial()
{
	zeCheckError(IsDestroyed(), false, "Cannot serial unload destroyed resource.");
	zeCheckError(IsShared(), false, "Cannot serial unload shared resource. Serial load/unload/reload operations are not allowed on shared resources.");
	zeDebugCheck(Manager == NULL, "Resource is not registered.");

	if (State < ZERS_RS_STAGED)
		return true;

	zeLog("Unloading resource. Resource Class: \"%s\", File Name: \"%s\".", GetClass()->GetName(), this->FileName.ToCString());
	TargetState = ZERS_RS_STAGED;
	return UpdateStateSerial();
}

bool ZERSResource::ReloadSerial()
{
	zeCheckError(IsDestroyed(), false, "Cannot serial reload destroyed resource.");
	zeCheckError(IsShared(), false, "Cannot serial reload shared resource. Serial load/unload/reload operations are not allowed on shared resources.");
	zeDebugCheck(Manager == NULL, "Resource is not registered.");

	ReloadFlag = true;

	if (TargetState <= ZERS_RS_LOADED)
		TargetState = ZERS_RS_LOADED;

	UpdateState();

	return true;
}

void ZERSResource::WaitStaging() const
{
	while(State < ZERS_RS_STAGED && State > ZERS_RS_DESTROYING);
}

void ZERSResource::WaitLoading() const
{
	while(State < ZERS_RS_LOADED && State > ZERS_RS_DESTROYING);
}

void ZERSResource::WaitUnloading() const
{
	while(State >= ZERS_RS_STAGED);
}
