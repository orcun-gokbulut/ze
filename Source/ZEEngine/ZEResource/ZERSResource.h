//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERSResource.h
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

#include "ZEMeta/ZEObject.h"
#include "ZEPointer/ZEReferenceCounted.h"

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZELink.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEFile/ZEFile.h"
#include "ZEThread/ZESignal.h"
#include "ZEThread/ZETask.h"

enum ZERSLoadMethod
{
	ZERS_LM_SYNC,
	ZERS_LM_ASYNC,
	ZERS_LM_CUSTOM
};

enum ZERSState
{
	ZERS_S_NOT_LOADED,
	ZERS_S_LOADING,
	ZERS_S_ITERATING,
	ZERS_S_LOADED,
	ZERS_S_ERROR
};

enum ZERSMemoryPool
{
	ZERS_MP_CPU,
	ZERS_MP_GPU,
	ZERS_MP_TOTAL
};

class ZERSLoadingOptions
{

};

class ZERSManager;

class ZERSResource : public ZEObject, public ZEReferenceCounted
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZERSResource)
	friend class ZERSManager;
	template<typename ZEResourceClass>
	friend class ZEHolder;
	private:
		mutable ZEUInt ReferenceCount;
		mutable ZELock ReferenceCounterLock;
		mutable ZELink<const ZERSResource> ManagerLink;

		ZERSState State;

		ZEString FilePath;
		ZEString FilePathNormalized;
		ZEUInt64 Hash;
		ZEGUID GUID;

		bool Cached;
		ZEInt CachePriority;
		
		ZEUInt LoadProgress;
		ZERSLoadMethod LoadMethod;
		ZESize Size[ZERS_MP_TOTAL];
		
		ZEArray<ZEHolder<ZERSResource> > SubResources;

		ZESignal WaitSignal;
		ZETask AsyncLoader;

		void AddReferance() const;
		void Release() const;
		ZETaskResult AsyncLoaderFunction(ZETaskThread* TaskThread, void* Parameters);

	protected:
		void AddSubResource(const ZEHolder<ZERSResource>& Resource);
		void RemoveSubResource(const ZEHolder<ZERSResource>& Resource);

		void SetFilePath(const ZEString& FilePath);
		void SetGUID(const ZEGUID& GUID);
		void SetSize(ZERSMemoryPool Pool, ZESize Size);
		void SetLoadMethod(ZERSLoadMethod Method);
		void SetLoadProgress(ZEUInt Size);

		// Loading Functions
		virtual bool LoadInternal(const ZERSLoadingOptions* Option);

		template<typename ZEResourceClass> 
		static ZERSResource* Instanciator();
		template<typename ZEResourceClass>
		static ZEHolder<const ZEResourceClass> LoadTemplate(const ZEString& FileName, const ZERSLoadingOptions* Options = NULL);

		virtual ~ZERSResource();

	public:
		ZERSState GetState() const;
		ZEGUID GetGUID() const;
		const ZEString& GetFilePath() const;
		ZESize GetReferanceCount() const;

		ZESize GetSize(ZERSMemoryPool Pool = ZERS_MP_TOTAL) const;
		ZESize GetTotalSize(ZERSMemoryPool Pool = ZERS_MP_TOTAL) const;

		ZERSLoadMethod GetLoadMethod() const;
		ZEUInt GetLoadProgress() const;

		const ZEArray<ZEHolder<ZERSResource>>& GetSubResources() const;

		void SetCached(bool Enabled);
		bool GetCached() const;

		void SetCachePriority(ZEInt Priority);
		ZEInt GetCachePriority() const;

		void Wait() const;

		ZERSResource();
};


template<typename ZEResourceClass>
ZERSResource*	ZERSResource::Instanciator()
{
	return new ZEResourceClass();
}

template<typename ZEResourceClass>
ZEHolder<const ZEResourceClass> ZERSResource::LoadTemplate(const ZEString& FileName, const ZERSLoadingOptions* Options)
{
	zeLog("Loading resource. Resource Class: \"%s\", File Path: \"%s\".", ZEResourceClass::Class()->GetName(), FileName.ToCString());
	return ZERSManager::GetInstance()->LoadResource(FileName, Instanciator<ZEResourceClass>, Options).Cast<const ZEResourceClass>();
}
