//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERSResourceGroup.cpp
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

#include "ZERSResourceGroup.h"

#include "ZERSResource.h"
#include "ZERSResourceManager.h"

void ZERSResourceGroup::UpdateMemoryUsage(ZERSResource* Resource, ZESSize MemoryUsageDelta[ZERS_MEMORY_POOL_COUNT])
{
	for (ZESize Pool = 0; Pool < ZERS_MEMORY_POOL_COUNT; Pool++)
	{
		MemoryUsage[Pool] += MemoryUsageDelta[Pool];
		if (Resource->IsShared())
			MemoryUsageShared[Pool] += MemoryUsageDelta[Pool];
	}

	if (Parent != NULL)
		Parent->UpdateMemoryUsage(Resource, MemoryUsageDelta);
}

void ZERSResourceGroup::RegisterResource(const ZERSResource* Resource)
{
	if (Resource->GetClass() == GetResourceClass())
		Resources.AddEnd(&Resource->ManagerLink);
	
	ResourceCount++;
	CreateCount++;

	for (ZESize I = 0; I < ZERS_MEMORY_POOL_COUNT; I++)
	{
		MemoryUsage[I] += Resource->GetMemoryUsage((ZERSMemoryPool)I);
		if (Resource->IsShared())
			MemoryUsageShared[I] += Resource->GetMemoryUsage((ZERSMemoryPool)I);
	}

	if (Parent != NULL)
		Parent->RegisterResource(Resource);
}

void ZERSResourceGroup::UnregisterResource(const ZERSResource* Resource)
{
	if (Resource->GetClass() == GetResourceClass())
		Resources.Remove(&Resource->ManagerLink);

	ResourceCount--;
	DestroyCount++;

	for (ZESize I = 0; I < ZERS_MEMORY_POOL_COUNT; I++)
	{
		MemoryUsage[I] -= Resource->GetMemoryUsage((ZERSMemoryPool)I);
		if (Resource->IsShared())
			MemoryUsageShared[I] -= Resource->GetMemoryUsage((ZERSMemoryPool)I);
	}

	if (Parent != NULL)
		Parent->UnregisterResource(Resource);
}

void ZERSResourceGroup::ShareResource(const ZERSResource* Resource)
{
	if (Resource->GetClass() == GetResourceClass())
		SharedResources.AddEnd(&Resource->ManagerSharedLink);

	SharedResourceCount++;

	for (ZESize I = 0; I < ZERS_MEMORY_POOL_COUNT; I++)
		MemoryUsageShared[I] += Resource->GetMemoryUsage((ZERSMemoryPool)I);

	if (Parent != NULL)
		Parent->ShareResource(Resource);
}

void ZERSResourceGroup::UnshareResource(const ZERSResource* Resource)
{
	if (Resource->GetClass() == GetResourceClass())
		SharedResources.Remove(&Resource->ManagerSharedLink);

	SharedResourceCount--;

	for (ZESize I = 0; I < ZERS_MEMORY_POOL_COUNT; I++)
		MemoryUsageShared[I] -= Resource->GetMemoryUsage((ZERSMemoryPool)I);

	if (Parent != NULL)
		Parent->UnshareResource(Resource);
}

ZERSResourceGroup::ZERSResourceGroup()
{
	Parent = NULL;
	ResourceClass = NULL;

	ResourceCount = 0;
	SharedResourceCount = 0;

	CreateCount = 0;
	DestroyCount = 0;

	memset(MemoryUsage, 0, sizeof(MemoryUsage));
	memset(MemoryUsageShared, 0, sizeof(MemoryUsageShared));
}

ZERSResourceGroup::~ZERSResourceGroup()
{

}

ZERSResourceGroup* ZERSResourceGroup::GetParent() const
{
	return Parent;
}

ZEClass* ZERSResourceGroup::GetResourceClass() const
{
	return ResourceClass;
}

const ZEArray<ZERSResourceGroup*> ZERSResourceGroup::GetChildGroups() const
{
	return ChildGroups;
}

ZESize ZERSResourceGroup::GetResourceCount() const
{
	return Resources.GetCount();
}

ZESize ZERSResourceGroup::GetSharedResourceCount() const
{
	return SharedResources.GetCount();
}

ZESize ZERSResourceGroup::GetMemoryUsage(ZERSMemoryPool Pool) const
{
	zeCheckError(Pool >= ZERS_MEMORY_POOL_COUNT, 0, "Unknown memory pool.");
	return MemoryUsage[Pool];
}

ZESize ZERSResourceGroup::GetMemoryUsageShared(ZERSMemoryPool Pool) const
{
	zeCheckError(Pool >= ZERS_MEMORY_POOL_COUNT, 0, "Unknown memory pool.");
	return MemoryUsageShared[Pool];
}

ZESize ZERSResourceGroup::GetMemoryUsageTotal(ZERSMemoryPool Pool) const
{
	return GetMemoryUsage(Pool) + GetMemoryUsageShared(Pool);
}

ZESize ZERSResourceGroup::GetMemoryUsageTotal() const
{
	ZESize Total = 0;
	for (ZESize I = 0; I < ZERS_MEMORY_POOL_COUNT; I++)
		Total += GetMemoryUsage((ZERSMemoryPool)I) + GetMemoryUsageShared((ZERSMemoryPool)I);

	return Total;
}
