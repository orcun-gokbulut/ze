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
#include "ZEDS/ZEFormat.h"
#include "ZEDS/ZEFastDelegate.h"
#include "ZERegEx/ZEWildcard.h"
#include "ZEFile/ZEPathInfo.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZECore/ZEConsole.h"
#include "ZECore/ZECommand.h"
#include "ZECore/ZECommandSection.h"
#include "ZECore/ZECommandManager.h"
#include "ZEMeta/ZEObject.h"

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
	ResourceGroup->Index = ResourceGroupIndex;
	ResourceGroupIndex++;
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

ZERSHolder<const ZERSResource> ZERSResourceManager::GetResourceInternal(ZEClass* ResourceClass, const ZEString& FileName, const ZERSResourceIdentifier* Identifer)
{
	ZERSResourceGroup* Group = GetResourceGroupInternal(ResourceClass);
	if (Group == NULL)
		return NULL;

	ZEString FileNameNormalized = ZEFileInfo(FileName).Normalize();
	ZEUInt64 Hash = FileNameNormalized.Lower().Hash();

	if (FileNameNormalized.IsEmpty())
		return NULL;

	ze_for_each(Resource, Group->SharedResources)
	{
		if (Resource->GetFileNameHash() == 0)
			continue;

		if (Resource->GetFileNameHash() == Hash && 
			Resource->GetFileName().EqualsIncase(FileNameNormalized))
		{
			if (Identifer != NULL)
			{
				if (Resource->GetIdentifier() != NULL && !Identifer->Equals(Resource->GetIdentifier()))
					continue;
			}
		
			return Resource.GetPointer();
		}
	}

	ze_for_each(SubGroup, Group->ChildGroups)
	{
		ZERSHolder<const ZERSResource> Resource = GetResourceInternal(SubGroup.GetItem()->ResourceClass, FileName, Identifer);
		if (!Resource.IsNull())
			return Resource.GetPointer();
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
	Resource->Index = ResourceIndex;
	ResourceCount++;
	ResourceIndex++;

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

static bool CommandSpitAndGlue(ZECommand* Command, const ZECommandParameterList* Parameters)
{
	ZERSResourceManager* Manager = ZERSResourceManager::GetInstance();
	ZEConsole* Console = ZEConsole::GetInstance();
	ZEValue Parameter = Parameters->GetCount() != 0 ? Parameters->GetItem(0) : ZEValue();

	if (Command->GetName() == "ShowStats")
	{
		Console->OutputRaw(Manager->ShowStats().ToCString());
	}
	else if (Command->GetName() == "ListResources")
	{
		if (Parameters->GetCount() == 0)
			Console->OutputRaw(Manager->ListResources().ToCString());
		else
			Console->OutputRaw(Manager->ListResources(Parameter.GetString()));
	}
	else if (Command->GetName() == "ShowResource")
	{
		if (Parameters->GetCount() == 0)
			Console->OutputRaw("Command Error: Missing arguments.");
		else
			Console->OutputRaw(Manager->ShowResource(Parameter.GetInt32()));
	}
	else if (Command->GetName() == "ListResourceGroups")
	{
		if (Parameters->GetCount() == 0)
			Console->OutputRaw(Manager->ListResourceGroups().ToCString());
		else
			Console->OutputRaw(Manager->ListResourceGroups(Parameter.GetString()));
	}
	else if (Command->GetName() == "ShowResourceGroup")
	{
		if (Parameters->GetCount() == 0)
			Console->OutputRaw("Command Error: Missing arguments.");
		else
			Console->OutputRaw(Manager->ShowResourceGroup(Parameter.GetInt32()));
	}

	return true;
}

static ZECommandSection Commands;

ZERSResourceManager::ZERSResourceManager()
{
	ResourceIndex = 0;
	ResourceGroupIndex = 0;
	ResourceCount = 0;
	SharedResourceCount = 0;

	memset(MemoryUsage, 0, sizeof(MemoryUsage));
	memset(MemoryUsageShared, 0, sizeof(MemoryUsageShared));

	Commands.SetName("ResourceManager");
	Commands.AddCommand(new ZECommand("ShowStats",			&CommandSpitAndGlue));
	Commands.AddCommand(new ZECommand("ListResources",		&CommandSpitAndGlue));
	Commands.AddCommand(new ZECommand("ShowResource",		&CommandSpitAndGlue));
	Commands.AddCommand(new ZECommand("ListResourceGroups", &CommandSpitAndGlue));
	Commands.AddCommand(new ZECommand("ShowResourceGroup",	&CommandSpitAndGlue));
	ZECommandManager::GetInstance()->RegisterSection(&Commands);
}

ZERSResourceManager::~ZERSResourceManager()
{
	ZECommandManager::GetInstance()->UnregisterSection(&Commands);

	ze_for_each(ResourceGroup, ResourceGroups)
		delete ResourceGroup.GetItem();
}

ZEArray<const ZERSResourceGroup*> ZERSResourceManager::GetResourceGroups()
{
	ManagerLock.Lock();
	ZEArray<const ZERSResourceGroup*> Snapshot;
	Snapshot.AddMultiple(ResourceGroups.GetCArray(), ResourceGroups.GetCount());
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

ZESize ZERSResourceManager::GetMemoryUsageTotal()
{
	return GetMemoryUsage(ZERS_MP_CPU) + GetMemoryUsage(ZERS_MP_GPU);
}

ZERSHolder<const ZERSResource> ZERSResourceManager::GetResource(ZEClass* ResourceClass, const ZEGUID& GUID)
{
	zeCheckError(ResourceClass == NULL, NULL, "Cannot get resource Resource Class is NULL.");

	ManagerLock.Lock();
	ZERSHolder<const ZERSResource> Resource = GetResourceInternal(ResourceClass, GUID);
	ManagerLock.Unlock();

	return Resource;
}

ZERSHolder<const ZERSResource> ZERSResourceManager::GetResource(ZEClass* ResourceClass, const ZEString& FileName, const ZERSResourceIdentifier* Identifier)
{
	zeCheckError(ResourceClass == NULL, NULL, "Cannot get resource. Resource Class is NULL.");

	ManagerLock.Lock();
	ZERSHolder<const ZERSResource> Resource = GetResourceInternal(ResourceClass, FileName, Identifier);
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

		if (!ZEClass::IsDerivedFrom(ResourceClass, NewResouce->GetClass()))
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


ZEString ZERSResourceManager::ShowStats()
{
	ManagerLock.Lock();

	ZEString Output = ZEFormat::Format(
		"ZERSResourceManager Stats:\n"
		"  Resource Count: {0}\n"
		"  Shared Resource Count: {1}\n"
		"  Resource Group Count: {2}\n"
		"  CPU Memory Usage:{3}\n"
		"  GPU Memory Usage:{4}\n"
		"  CPU Shared Memory Usage:{5}\n"
		"  GPU Shared Memory Usage:{6}\n"
		"  Total Memory Usage:{7}\n",
		ResourceCount, 
		SharedResourceCount,
		ResourceGroups.GetCount(),
		MemoryUsage[ZERS_MP_CPU],
		MemoryUsage[ZERS_MP_GPU],
		MemoryUsageShared[ZERS_MP_CPU],
		MemoryUsageShared[ZERS_MP_GPU],
		GetMemoryUsageTotal());

	ManagerLock.Unlock();

	return Output;
}

ZEString ZERSResourceManager::ListResources(const ZEString& Filter)
{
	ZEWildcard Matcher;
	bool FilterEnabled = !Filter.IsEmpty();
	if (FilterEnabled)
		FilterEnabled = Matcher.Compile(Filter);
	
	ZESize ResourceCount = 0;
	ZEString Output;
	ManagerLock.Lock();
	for (ZESize I = 0; I < ResourceGroups.GetCount(); I++)
	{
		ZERSResourceGroup* Group = ResourceGroups[I];
		
		bool GroupNameMatched = true;
		if (FilterEnabled)
			GroupNameMatched = Matcher.Match(Group->GetResourceClass()->GetName());

		ze_for_each(Resource, Group->Resources)
		{
			if (FilterEnabled && !GroupNameMatched && !Matcher.Match(Resource->GetFileName()))
				continue;

			Output += ZEFormat::Format(
				"{0} "
				"{1}, "
				"{2}, "
				"RefCount: {3}",
				Resource->GetIndex(),
				Resource->GetClass()->GetName(),
				ZERSResourceState_Declaration()->ToText(Resource->GetState()),
				Resource->GetReferenceCount());

			if (!Resource->GetGUID().Equals(ZEGUID::Zero))
				Output += ZEFormat::Format(", GUID: {0}", Resource->GetGUID().ToString());

			if (!Resource->GetFileName().IsEmpty())
				Output += ZEFormat::Format(", File Name: {0}", Resource->GetFileName());

			Output += "\n";

			ResourceCount++;
		}
	}
	ManagerLock.Unlock();

	if (ResourceCount != 0)
		Output += ZEFormat::Format("{0} resource listed.", ResourceCount);
	else
		Output = "No matching resource has been found.";

	return Output;
}

ZEString ZERSResourceManager::ShowResource(ZESize Index)
{
	ZEString Output;
	ManagerLock.Lock();
	for (ZESize I = 0; I < ResourceGroups.GetCount(); I++)
	{
		ZERSResourceGroup* Group = ResourceGroups[I];
		ze_for_each(Resource, Group->Resources)
		{
			if (Resource->GetIndex() != Index)
				continue;

			Output = ZEFormat::Format(
				"Index: {0}\n"
				"Class: {1}\n"
				"GUID: {2}\n"
				"File Name: {3}\n"
				"Hash: {4}\n"
				"State: {5}\n"
				"Shared: {6}\n"
				"Reference Count: {7}\n"
				"CPU Memory Usage: {8}\n"
				"GPU Memory Usage: {9}\n",
				Resource->Index,
				Resource->GetClass()->GetName(),
				Resource->GetGUID().ToString(),
				Resource->GetFileName(),
				Resource->GetFileNameHash(),
				ZERSResourceState_Declaration()->ToText(Resource->GetState()),
				Resource->IsShared(),
				Resource->GetReferenceCount(),
				Resource->GetMemoryUsage(ZERS_MP_CPU),
				Resource->GetMemoryUsage(ZERS_MP_GPU),
				Resource->GetTotalMemoryUsage());

			break;
		}
	}
	ManagerLock.Unlock();

	if (Output.IsEmpty())
		Output = "Resource not found.\n";

	return Output;
}


ZEString ZERSResourceManager::ListResourceGroups(const ZEString& Filter)
{
	ZEWildcard Matcher;
	bool FilterEnabled = !Filter.IsEmpty();
	if (FilterEnabled)
		FilterEnabled = Matcher.Compile(Filter);

	ZESize ResourceGroupCount = 0;
	ZEString Output;
	ManagerLock.Lock();
	for (ZESize I = 0; I < ResourceGroups.GetCount(); I++)
	{
		ZERSResourceGroup* Group = ResourceGroups[I];
		
		if (FilterEnabled && !Matcher.Match(Filter))
			continue;

		Output += ZEFormat::Format("{0} {1}\n", Group->GetIndex(), Group->GetResourceClass()->GetName());

		ResourceGroupCount++;
	}
	ManagerLock.Unlock();

	if (ResourceGroupCount != 0)
		Output += ZEFormat::Format("{0} resource group listed.", ResourceGroupCount);
	else
		Output = "No matching resource group has been found.";

	return Output;
}

ZEString ZERSResourceManager::ShowResourceGroup(ZESize Index)
{
	ZEString Output;
	ManagerLock.Lock();
	for (ZESize I = 0; I < ResourceGroups.GetCount(); I++)
	{
		ZERSResourceGroup* Group = ResourceGroups[I];
		if (Group->Index != Index)
			continue;

		Output = ZEFormat::Format(
			"Index: {0}\n"
			"Class: {1}\n"
			"Parent Class: {2}\n"
			"Resource Count: {3}\n"
			"Shared Resource Count: {4}\n"
			"CPU Memory Usage: {5}\n"
			"GPU Memory Usage: {6}\n"
			"Shared CPU Memory Usage: {7}\n"
			"Shared GPU Memory Usage: {8}\n"
			"Total Memory Usage: {9}\n"
			"\n"
			"Child Groups:",
			Group->GetIndex(),
			Group->GetResourceClass()->GetName(),
			(Group->GetParent() != NULL ? Group->GetParent()->GetResourceClass()->GetName() : ""),
			Group->GetResourceCount(),
			Group->GetSharedResourceCount(),
			Group->GetMemoryUsage(ZERS_MP_CPU),
			Group->GetMemoryUsage(ZERS_MP_GPU),
			Group->GetMemoryUsageShared(ZERS_MP_CPU),
			Group->GetMemoryUsageShared(ZERS_MP_GPU),
			Group->GetMemoryUsageTotal());

		for (ZESize I = 0; I < Group->GetChildGroups().GetCount(); I++)
			Output += ZEFormat::Format("  {0}\n", Group->GetChildGroups()[I]->GetResourceClass()->GetName());

		break;
	}
	ManagerLock.Unlock();

	if (Output.IsEmpty())
		Output = "Resource Group not found\n";

	return Output;
}

ZERSResourceManager* ZERSResourceManager::GetInstance()
{
	static ZERSResourceManager Manager;
	return &Manager;
}
