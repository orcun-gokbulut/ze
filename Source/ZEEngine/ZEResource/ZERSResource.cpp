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
	Lock.Lock();

	ReferenceCount++;
	
	if (Parent != NULL)
		Parent->Reference();

	Lock.Unlock();
}

void ZERSResource::Release() const
{
	Lock.Lock();
	
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

	Lock.Unlock();
}

void ZERSResource::Destroy() const
{
	if (IsShared())
		ZERSResourceManager::GetInstance()->DestroyResource(this);
	else
		delete this;
}

void ZERSResource::UpdateMemoryConsumption()
{
	Lock.Lock();
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
	Lock.Unlock();
}

void ZERSResource::SetMemoryUsage(ZERSMemoryPool Pool, ZESize Size)
{
	MemoryUsage[Pool] = Size;

	if (Parent != NULL)
		Parent->UpdateMemoryConsumption();
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

ZERSResource::ZERSResource() : ManagerLink(this), ManagerSharedLink(this)
{
	GUID = ZEGUID::Zero;
	State = ZERS_RS_NONE;
	Parent = NULL;
	Shared = false;
	ReferenceCount = 0;
	memset(MemoryUsage, 0, sizeof(MemoryUsage));
	memset(MemoryUsageSelf, 0, sizeof(MemoryUsage));
}

ZERSResource::~ZERSResource()
{
	if (Shared)
		Unshare();

	while(ChildResources.GetCount() != 0)
		ChildResources.GetFirstItem()->Destroy();

	if (Parent != NULL)
		Parent->RemoveChildResource(this);
}

ZERSResource* ZERSResource::GetParent()
{
	return Parent;
}

const ZERSResource* ZERSResource::GetParent() const
{
	return Parent;
}

ZERSResourceType ZERSResource::GetType() const
{
	return ZERS_RT_NORMAL;
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

bool ZERSResource::IsShared() const
{
	return Shared;
}

void ZERSResource::Share()
{
	if (IsShared())
		return;

	zeCheckError(GUID == ZEGUID::Zero, ZE_VOID, "Cannot share resource. Ilvalid GUID.");
	ZERSResourceManager::GetInstance()->EnlistResource(this);
}

void ZERSResource::Unshare()
{
	if (!IsShared())
		return;

	ZERSResourceManager::GetInstance()->DelistResource(this);
}


bool ZERSResource::IsStaging()
{
	return State == ZERS_RS_STAGING;
}

void ZERSResource::StagingWait()
{
	while(State == ZERS_RS_STAGING);
}

void ZERSResource::StagingRealize()
{
	if (State == ZERS_RS_STAGING)
		State = ZERS_RS_ALIVE;
}
