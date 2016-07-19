//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERSTemplates.h
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
#include "ZEGUID.h"
#include "ZERSHolder.h"
#include "ZERSDefinitions.h"
#include "ZERSResourceManager.h"

class ZERSResource;
class ZERSResourceLoadable;

class ZERSTemplates
{
	public:
		template<typename ZERSResourceClass>
		static bool										CheckResourceClass(const ZERSResource* SharedResource);
		template<typename ZERSResourceClass>
		static ZERSResource*							Instanciator();
		template<typename ZERSResourceClass>
		static ZERSResourceLoadable*					InstanciatorLoadable();

		template<typename ZERSResourceClass>
		static ZERSHolder<const ZERSResourceClass>		GetResource(const ZEGUID& GUID);
		template<typename ZERSResourceClass>
		static ZERSHolder<const ZERSResourceClass>		GetResource(const ZEString& FileName);

		template<typename ZERSResourceClass>
		static ZERSHolder<ZERSResourceClass>			CreateResource();
		template<typename ZERSResourceClass>
		static ZERSHolder<const ZERSResourceClass>		CreateResourceShared(const ZEGUID& GUID, ZERSResourceClass** StagingResource);
		
		template<typename ZERSResourceClass>
		static ZERSHolder<ZERSResourceClass>			LoadResource(const ZEString& FileName);
		template<typename ZERSResourceClass>
		static ZERSHolder<const ZERSResourceClass>		LoadResourceShared(const ZEString& FileName);
};


// IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////////

template<typename ZERSResourceClass>
ZERSResource* ZERSTemplates::Instanciator()
{
	return new ZERSResourceClass();
}

template<typename ZERSResourceClass>
ZERSResourceLoadable* ZERSTemplates::InstanciatorLoadable()
{
	return new ZERSResourceClass();
}

template<typename ZERSResourceClass>
bool ZERSTemplates::CheckResourceClass(const ZERSResource* SharedResource)
{
	if (SharedResource->GetClass() != ZERSResourceClass::Class())
	{
		zeError("Shared resource class does not match requested resource class. "
			"GUID: \"%s\", "
			"Requested Resource Class: \"%s\", "
			"Shared Resource Class: \"%s\".",
			SharedResource->GetGUID().ToString().ToCString(),
			ZERSResourceClass::Class(),
			SharedResource->GetClass());

		return false;
	}

	return true;
}

template<typename ZERSResourceClass>
ZERSHolder<const ZERSResourceClass> ZERSTemplates::GetResource(const ZEGUID& GUID)
{
	ZERSHolder<const ZERSResource> SharedResource = ZERSResourceManager::GetInstance()->GetResource(GUID);
	
	if (SharedResource == NULL)
		return NULL;

	if (!CheckResourceClass<ZERSResourceClass>(SharedResource))
		return NULL;
	
	return static_cast<const ZERSResourceClass*>(SharedResource.GetPointer());
}

template<typename ZERSResourceClass>
ZERSHolder<const ZERSResourceClass> ZERSTemplates::GetResource(const ZEString& FileName)
{
	ZERSHolder<const ZERSResource> SharedResource = ZERSResourceManager::GetInstance()->GetResource(FileName);

	if (SharedResource == NULL)
		return NULL;

	if (!CheckResourceClass<ZERSResourceClass>(SharedResource))
		return NULL;

	return static_cast<const ZERSResourceClass*>(SharedResource.GetPointer());
}

template<typename ZERSResourceClass>
ZERSHolder<ZERSResourceClass> ZERSTemplates::CreateResource()
{
	return new ZERSResourceClass();
}

template<typename ZERSResourceClass>
ZERSHolder<const ZERSResourceClass> ZERSTemplates::CreateResourceShared(const ZEGUID& GUID, ZERSResourceClass** StagingResource)
{
	ZERSResource* StagingResourceTemp = NULL;
	ZERSHolder<const ZERSResource> SharedResource = ZERSResourceManager::GetInstance()->StageResource(GUID, Instanciator<ZERSResourceClass>, &StagingResourceTemp);
	
	if (SharedResource == NULL)
		return NULL;

	if (!CheckResourceClass<ZERSResourceClass>(SharedResource))
		return NULL;

	if (StagingResource != NULL)
		*StagingResource = static_cast<ZERSResourceClass*>(StagingResourceTemp);

	return static_cast<const ZERSResourceClass*>(SharedResource.GetPointer());
}

template<typename ZERSResourceClass>
ZERSHolder<ZERSResourceClass> ZERSTemplates::LoadResource(const ZEString& FileName)
{
	ZERSHolder<ZERSResourceClass> Resource = CreateResource<ZERSResourceClass>();
	Resource->Load(FileName);
	return Resource;
}

template<typename ZERSResourceClass>
ZERSHolder<const ZERSResourceClass> ZERSTemplates::LoadResourceShared(const ZEString& FileName)
{
	ZERSResourceLoadable* StagingResource = NULL;
	ZERSHolder<const ZERSResourceLoadable> SharedResource = ZERSResourceManager::GetInstance()->StageResource(FileName, InstanciatorLoadable<ZERSResourceClass>, &StagingResource);
	
	if (SharedResource == NULL)
		return NULL;

	if (!CheckResourceClass<ZERSResourceClass>(SharedResource))
		return NULL;

	if (StagingResource != NULL)
		StagingResource->Load(FileName);

	return static_cast<const ZERSResourceClass*>(SharedResource.GetPointer());
}
