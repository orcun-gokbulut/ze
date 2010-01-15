//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ResourceManager.cpp
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

#include "ResourceManager.h"
#include <string.h>
#include "Core.h"
#include "Core/Error.h"

#pragma warning(push)
#pragma warning(disable:4996 4267)

ZEResource* ZEResourceManager::_GetResource(const char* FileName)
{
	ZEResource* CurResource;
	for (size_t I = 0; I < this->SharedResources.GetCount(); I++)
	{
		CurResource = this->SharedResources.GetItem(I);
		if (_stricmp (CurResource->FileName, FileName) == 0)
				CurResource->ReferenceCount++;
				return CurResource;
	}
	return NULL;
}

const ZEResource* ZEResourceManager::GetResource(const char* FileName)
{
	ZEResource* CurResource;
	for (size_t I = 0; I < this->SharedResources.GetCount(); I++)
	{
		CurResource = this->SharedResources.GetItem(I);
		if (_stricmp (CurResource->FileName, FileName) == 0)
		{
			CurResource->ReferenceCount++;
			return CurResource;
		}
	}
	return NULL;
}

void ZEResourceManager::AddResource(ZEResource* Resource)
{
	this->SharedResources.Add(Resource);
}

void ZEResourceManager::ReleaseResource(ZEResource* Resource)
{
	if (Resource->ReferenceCount <= 0 && !Resource->Cached)
		SharedResources.DeleteValue(Resource);
}

void ZEResourceManager::ReleaseAllResources()
{
	ZEWARNINGASSERT(SharedResources.GetCount(), "[Resource Manager] Possible memory leak ! One or more shared resources are not released.");

	for (size_t I = 0; I < SharedResources.GetCount(); I++)
		delete SharedResources.GetItem(I);
	
	SharedResources.Clear();
}

void ZEResourceManager::UncacheResource(const char* FileName)
{
	ZEResource* Resource = _GetResource(FileName);
	if (Resource != NULL)
		if (Resource->Cached && Resource->Shared)
		{
			Resource->Cached = false;
			if (Resource->ReferenceCount <= 0)
				ReleaseResource(Resource);
		}
}

void ZEResourceManager::UncacheAllResources()
{

}

ZEResourceManager*  ZEResourceManager::GetInstance()
{
	return ZECore::GetInstance()->GetResourceManager();
}

#pragma warning(pop)
