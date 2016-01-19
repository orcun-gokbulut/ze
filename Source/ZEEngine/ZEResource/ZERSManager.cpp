//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERSManager.cpp
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

#include "ZERSManager.h"

#include "ZEError.h"
#include "ZEFile/ZEPathInfo.h"
#include "ZEFile/ZEFileInfo.h"

ZEHolder<const ZERSResource> ZERSManager::GetResourceInternal(const ZEString& FilePath)
{
	ZEFileInfo FileInfo(FilePath);
	ZEString FilePathNormalized = FileInfo.Normalize();
	FilePathNormalized.LowerSelf();
	FilePathNormalized.Trim();
	ZEUInt64 Hash = FilePathNormalized.Hash();

	if (FilePathNormalized.IsEmpty())
		return NULL;

	ze_for_each(Resource, Resources)
	{
		if (Resource->GetHash() == Hash && Resource->FilePathNormalized == FilePathNormalized)
		{
			ManagerLock.Unlock();
			return &Resource.GetItem();
		}
	}

	return NULL;
}

void ZERSManager::RegisterResourceInternal(const ZERSResource* Resource)
{
	if (Resource->ManagerLink.GetInUse())
		return;

	Resources.AddEnd(&Resource->ManagerLink);

	for (ZESize I = 0; I < ZERS_MP_TOTAL; I++)
		MemoryUsage[I] += Resource->GetSize((ZERSMemoryPool)I);
}

ZEHolder<const ZERSResource> ZERSManager::LoadResource(const ZEString& FilePath, ZERSInstanciator Instanciator, ZERSLoadingOptions* LoadingOptions)
{
	ManagerLock.Lock();

	ZEHolder<const ZERSResource> AvailableResource = GetResourceInternal(FilePath);
	if (AvailableResource != NULL)
	{
		ManagerLock.Unlock();
		if (AvailableResource->GetLoadMethod() == ZERS_LM_SYNC)
			AvailableResource->Wait();
		return AvailableResource;
	}

	ZEHolder<ZERSResource> Resource;
	Resource = Instanciator();
	Resource->SetFilePath(FilePath);
	RegisterResourceInternal(Resource);

	ManagerLock.Unlock();

	if (Resource->GetLoadMethod() == ZERS_LM_SYNC)
	{
		Resource->LoadInternal(LoadingOptions);
		Resource->WaitSignal.Signal();
	}
	else if (Resource->GetLoadMethod() == ZERS_LM_CUSTOM)
	{
		Resource->LoadInternal(LoadingOptions);
	}
	else
	{
		Resource->AsyncLoader.SetParameter(LoadingOptions);
		Resource->AsyncLoader.Run();
	}

	return Resource;
}

void ZERSManager::RegisterResource(const ZERSResource* Resource)
{
	ManagerLock.Lock();
	RegisterResourceInternal(Resource);
	ManagerLock.Unlock();
}

void ZERSManager::ReleaseResource(const ZERSResource* Resource)
{
	ManagerLock.Lock();
	if (Resource->GetReferanceCount() != 0)
	{
		ManagerLock.Unlock();
		return;
	}

	if (!Resource->ManagerLink.GetInUse())
	{
		ManagerLock.Unlock();
		return;
	}

	Resources.Remove(&Resource->ManagerLink);
	for (ZESize I = 0; I < ZERS_MP_TOTAL; I++)
		MemoryUsage[I] -= Resource->GetSize((ZERSMemoryPool)I);

	ManagerLock.Unlock();
}


ZERSManager::ZERSManager()
{
	CacheUsage = 0;
	CacheSize = 1024*1024*1024;
	memset(MemoryUsage, 1, sizeof(MemoryUsage));
}

ZERSManager::~ZERSManager()
{

}

ZEHolder<const ZERSResource> ZERSManager::GetResource(const ZEGUID& GUID)
{
	ManagerLock.Lock();
	ze_for_each(Resource, Resources)
	{
		if (Resource->GetGUID() == GUID)
		{
			ManagerLock.Unlock();
			return &Resource.GetItem();
		}
	}
	ManagerLock.Unlock();

	return NULL;
}

ZEHolder<const ZERSResource> ZERSManager::GetResource(const ZEString& FilePath)
{
	ManagerLock.Lock();
	ZEHolder<const ZERSResource> Resource = GetResourceInternal(FilePath);
	ManagerLock.Unlock();
	return Resource;
}

ZERSManager* ZERSManager::GetInstance()
{
	static ZERSManager Manager;
	return &Manager;
}
