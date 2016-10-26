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
class ZERSResourceIdentifier;

class ZERSTemplates
{
	public:
		template<typename ZERSResourceObject>
		static bool										CheckResourceClass(const ZERSResource* SharedResource);
		template<typename ZERSResourceObject>
		static ZERSResource*							InstanciatorFunction(const void* InstanciatorParameter);

		template<typename ZERSResourceObject>
		static ZERSHolder<const ZERSResourceObject>		GetResource(const ZEGUID& GUID);
		template<typename ZERSResourceObject>
		static ZERSHolder<const ZERSResourceObject>		GetResource(const ZEString& FileName);

		template<typename ZERSResourceObject>
		static ZERSHolder<ZERSResourceObject>			CreateResource(ZERSInstanciator Instanciator = NULL, const void* InstanciatorParameter = NULL);
		template<typename ZERSResourceObject>
		static ZERSHolder<const ZERSResourceObject>		CreateResourceShared(const ZEGUID& GUID, ZERSResourceObject** StagingResource, ZERSInstanciator Instanciator = NULL, const void* InstanciatorParameter = NULL);
		
		template<typename ZERSResourceObject>
		static ZERSHolder<ZERSResourceObject>			LoadResource(const ZEString& FileName, ZERSInstanciator Instanciator = NULL, const void* InstanciatorParameter = NULL);
		template<typename ZERSResourceObject>
		static ZERSHolder<const ZERSResourceObject>		LoadResourceShared(const ZEString& FileName, ZERSInstanciator Instanciator = NULL, const void* InstanciatorParameter = NULL);
};


// IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////////

template<typename ZERSResourceObject>
ZERSResource* ZERSTemplates::InstanciatorFunction(const void* InstanciatorParameter)
{
	return new ZERSResourceObject();
}

template<typename ZERSResourceObject>
bool ZERSTemplates::CheckResourceClass(const ZERSResource* SharedResource)
{
	if (SharedResource->GetClass() != ZERSResourceObject::Class())
	{
		zeError("Shared resource class does not match requested resource class. "
			"GUID: \"%s\", "
			"Requested Resource Class: \"%s\", "
			"Shared Resource Class: \"%s\".",
			SharedResource->GetGUID().ToString().ToCString(),
			ZERSResourceObject::Class()->GetName(),
			SharedResource->GetClass()->GetName());

		return false;
	}

	return true;
}

template<typename ZERSResourceObject>
ZERSHolder<const ZERSResourceObject> ZERSTemplates::GetResource(const ZEGUID& GUID)
{
	return static_cast<const ZERSResourceObject*>(ZERSResourceManager::GetInstance()->GetResource(ZERSResourceObject::Class(), GUID).GetPointer());
}

template<typename ZERSResourceObject>
ZERSHolder<const ZERSResourceObject> ZERSTemplates::GetResource(const ZEString& FileName)
{
	return static_cast<const ZERSResourceObject*>( ZERSResourceManager::GetInstance()->GetResource(ZERSResourceObject::Class(), FileName).GetPointer());
}

template<typename ZERSResourceObject>
ZERSHolder<ZERSResourceObject> ZERSTemplates::CreateResource(ZERSInstanciator Instanciator, const void* InstanciatorParameter)
{
	if (Instanciator == NULL)
		return NULL;

	ZERSResource* Resource = Instanciator(InstanciatorParameter);
	if (Resource == NULL)
		return NULL;

	zeDebugCheck(ZERSResourceObject::Class() != Resource->GetClass(), "Cannot create resource. Ilvallid Instanciator. Resource Class: \"%s\".", ZERSResourceObject::Class()->GetName());

	return static_cast<ZERSResourceObject*>(Resource);
}

template<typename ZERSResourceObject>
ZERSHolder<const ZERSResourceObject> ZERSTemplates::CreateResourceShared(const ZEGUID& GUID, ZERSResourceObject** StagingResource, ZERSInstanciator Instanciator, const  void* InstanciatorParameter)
{
	if (Instanciator == NULL)
		return NULL;

	ZERSResource* StagingResourceTemp = NULL;
	ZERSHolder<const ZERSResource> SharedResource = ZERSResourceManager::GetInstance()->StageResource(ZERSResourceObject::Class(), GUID, Instanciator, InstanciatorParameter, &StagingResourceTemp);
	
	if (SharedResource == NULL)
		return NULL;

	if (StagingResource != NULL)
		*StagingResource = static_cast<ZERSResourceObject*>(StagingResourceTemp);
	else
		StagingResourceTemp->StagingRealized();

	return static_cast<const ZERSResourceObject*>(SharedResource.GetPointer());
}

template<typename ZERSResourceObject>
ZERSHolder<ZERSResourceObject> ZERSTemplates::LoadResource(const ZEString& FileName, ZERSInstanciator Instanciator, const void* InstanciatorParameter)
{
	if (Instanciator == NULL)
		return NULL;

	ZERSHolder<ZERSResourceObject> Resource = static_cast<ZERSResourceObject*>(Instanciator(InstanciatorParameter));
	if (Resource == NULL)
		return NULL;

	Resource->SetFileName(FileName);
	Resource->StagingRealized();
	Resource->Load();

	return Resource;
}

template<typename ZERSResourceObject>
ZERSHolder<const ZERSResourceObject> ZERSTemplates::LoadResourceShared(const ZEString& FileName, ZERSInstanciator Instanciator, const void* InstanciatorParameter)
{
	if (Instanciator == NULL)
		return NULL;

	ZERSResource* StagingResource = NULL;
	ZERSHolder<const ZERSResource> SharedResource = ZERSResourceManager::GetInstance()->StageResource(ZERSResourceObject::Class(), FileName, Instanciator, InstanciatorParameter, &StagingResource);
	
	if (SharedResource == NULL)
		return NULL;

	if (StagingResource != NULL)
	{
		StagingResource->StagingRealized();
		StagingResource->SetFileName(FileName);
		StagingResource->Load();
	}

	return static_cast<const ZERSResourceObject*>(SharedResource.GetPointer());
}
