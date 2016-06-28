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

#include "ZERSManager.h"
#include "ZEFile\ZEFileInfo.h"

void ZERSResource::AddReferance() const
{
	ReferenceCounterLock.Lock();
	ReferenceCount++;
	ReferenceCounterLock.Unlock();
}

void ZERSResource::Release() const
{
	ReferenceCounterLock.Lock();
	ReferenceCount--;
	if (ReferenceCount == 0)
		ZERSManager::GetInstance()->ReleaseResource(this);
	ReferenceCounterLock.Unlock();
}

ZETaskResult ZERSResource::AsyncLoaderFunction(ZETaskThread* TaskThread, void* Parameters)
{
	return LoadInternal((ZERSLoadingOptions*)Parameters) == false ? ZE_TR_DONE : ZE_TR_COOPERATING;
}

ZERSResource::~ZERSResource()
{

}

void ZERSResource::AddSubResource(const ZEHolder<ZERSResource>& Resource)
{
	if (SubResources.Exists(Resource))
		return;

	SubResources.Add(Resource);
}

void ZERSResource::RemoveSubResource(const ZEHolder<ZERSResource>& Resource)
{
	SubResources.RemoveValue(Resource);
}

void ZERSResource::SetFilePath(const ZEString& FilePath)
{
	this->FilePath = FilePath;
	FilePathNormalized = ZEFileInfo(FilePath).Normalize().Lower();
	Hash = FilePathNormalized.Lower().Hash();
}

void ZERSResource::SetGUID(const ZEGUID& GUID)
{
	this->GUID = GUID;
}

void ZERSResource::SetSize(ZERSMemoryPool Pool, ZESize NewSize)
{
	if (Pool >= ZERS_MP_TOTAL)
		return;

	Size[Pool] = NewSize;
}

void ZERSResource::SetLoadMethod(ZERSLoadMethod Method)
{
	LoadMethod = Method;
}

void ZERSResource::SetLoadProgress(ZEUInt Percent)
{
	if (Percent > 100)
		Percent = 100;

	LoadProgress = Percent;
}


bool ZERSResource::LoadInternal(const ZERSLoadingOptions* Option)
{
	return false;
}

ZERSResource::ZERSResource() : ManagerLink(this)
{
	State = ZERS_S_NOT_LOADED;
	Hash = 0;
	ReferenceCount = 0;
	Cached = true;
	CachePriority = 0;
	LoadProgress = 0;
	LoadMethod = ZERS_LM_SYNC;
	memset(Size, 0, sizeof(Size));
	
	AsyncLoader.SetPool(ZE_TPI_IO);
	AsyncLoader.SetFunction(ZEDelegateMethod(ZETaskFunction, ZERSResource, AsyncLoaderFunction, this));
}

const ZEString& ZERSResource::GetFilePath() const
{
	return FilePath;
}

ZERSState ZERSResource::GetState() const
{
	return State;
}

ZEGUID ZERSResource::GetGUID() const
{
	return GUID;
}

ZESize ZERSResource::GetReferanceCount() const
{
	return ReferenceCount;
}

ZESize ZERSResource::GetSize(ZERSMemoryPool Pool) const
{
	if (Pool > ZERS_MP_TOTAL)
	{
		return 0;
	}
	else if (Pool == ZERS_MP_TOTAL)
	{
		ZESize Sum = 0;
		for (ZESize I = 0; I < ZERS_MP_TOTAL; I++)
			Sum += Size[I];
		return Sum;
	}
	else
	{
		return Size[Pool];
	}
}

ZESize ZERSResource::GetTotalSize(ZERSMemoryPool Pool) const
{
	ZESize Sum = GetSize(Pool);
	for (ZESize I = 0; I < ZERS_MP_TOTAL; I++)
		Sum += SubResources[I]->GetTotalSize(Pool);
	
	return Sum;
}

ZERSLoadMethod ZERSResource::GetLoadMethod() const
{
	return LoadMethod;
}

ZEUInt ZERSResource::GetLoadProgress() const
{
	ZEUInt32 TotalProgress = 0;
	for (ZESize I = 0; I < SubResources.GetCount(); I++)
		TotalProgress += SubResources[I]->GetLoadProgress();

	return (LoadProgress + TotalProgress) / ((ZEUInt)SubResources.GetCount() + 1);
}

const ZEArray<ZEHolder<ZERSResource>>& ZERSResource::GetSubResources() const
{
	return SubResources;
}

void ZERSResource::SetCached(bool Enabled)
{
	Cached = Enabled;
}

bool ZERSResource::GetCached() const
{
	return Cached;
}

void ZERSResource::SetCachePriority(ZEInt Priority)
{
	CachePriority = Priority;
}

ZEInt ZERSResource::GetCachePriority() const
{
	return CachePriority;
}

void ZERSResource::Wait() const
{
	WaitSignal.Wait();
}
