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

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZELink.h"
#include "ZEFile/ZEFile.h"
#include "ZEMeta/ZEObject.h"
#include "ZERSHolder.h"
#include "ZEThread/ZESignal.h"
#include "ZEThread/ZETask.h"


ZE_META_FORWARD_DECLARE(ZERSManager, "ZEResourceManager.h")

enum ZERSLoadMethod
{
	ZERS_LM_SYNC,
	ZERS_LM_ASYNC,
	ZERS_LM_ITERATIVE
};

enum ZERSStatus
{
	ZERS_S_NOT_LOADED,
	ZERS_S_LOADING,
	ZERS_S_SEMI_LOADED, // Iterating
	ZERS_S_FULLY_LOADED,
	ZERS_S_NOT_FOUND
};

enum ZERSPool
{
	ZERS_P_CPU,
	ZERS_P_GPU,
	ZERS_P_TOTAL
};

class ZERSLoadingOptions
{

};

class ZERSResource : public ZEObject
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZERSResource)
	friend class ZERSManager;
	template<typename ZEResourceClass>
	friend class ZERSHolder;
	private:
		ZERSStatus				Status;

		ZEString				FilePath;
		ZEString				FilePathNormalized;
		ZEUInt64				Hash;
		ZEGUID					GUID;
		ZEUInt					ReferenceCount;

		bool					Cached;
		ZEInt					CachePriority;
		
		ZEUInt					Iteration;
		ZEUInt					LoadProgress;
		ZERSLoadMethod			LoadMethod;
		ZESize					Size[ZERS_P_TOTAL];
		
		ZEArray<ZERSHolder<ZERSResource> > SubResources;

		ZELink<ZERSResource>	ManagerLink;
		ZELock					ReferenceCounterLock;
		ZELock					IterationLock;
		ZESignal				WaitSignal;
		ZETask					AsyncLoader;

		void					AddReferance();
		void					Release();
		bool					AsyncLoaderFunction(ZETaskThread* TaskThread, int InstanceIndex, void* Parameters);

		virtual					~ZERSResource();

	protected:
		void					AddSubResource(const ZERSHolder<ZERSResource>& Resource);
		void					RemoveSubResource(const ZERSHolder<ZERSResource>& Resource);

		void					SetFilePath(const ZEString& FilePath);
		void					SetGUID(const ZEGUID& GUID);
		void					SetSize(ZERSPool Pool, ZESize Size);
		void					SetLoadMethod(ZERSLoadMethod Method);
		void					SetLoadProgress(ZEUInt Size);

		// Loading Functions
		virtual bool			LoadInternal(const ZERSLoadingOptions* Option);
		template<typename ZEResourceClass> 
		static ZERSResource*	Instanciator();
		template<typename ZEResourceClass>
		static ZERSHolder<ZERSResource>	LoadTemplate(const ZEString& FileName, const ZERSLoadingOptions* Options = NULL);

								ZERSResource();

	public:
		const ZEString&			GetFilePath() const;
		ZERSStatus				GetStatus() const;
		ZEUInt64				GetHash() const;
		ZEGUID					GetGUID() const;
		ZESize					GetReferanceCount() const;

		ZESize					GetSize(ZERSPool Pool = ZERS_P_TOTAL) const;
		ZESize					GetTotalSize(ZERSPool Pool = ZERS_P_TOTAL) const;

		ZERSLoadMethod			GetLoadMethod() const;
		ZEUInt					GetLoadProgress() const;

		const ZEArray<ZERSHolder<ZERSResource>>& GetSubResources() const;

		void					SetCached(bool Enabled);
		bool					GetCached() const;

		void					SetCachePriority(ZEInt Priority);
		ZEInt					GetCachePriority() const;

		ZEUInt					GetIteration();
		void					LockIteration();
		void					UnlockIteration();

		void					Wait();
};


template<typename ZEResourceClass>
ZERSResource*	ZERSResource::Instanciator()
{
	return new ZEResourceClass();
}

template<typename ZEResourceClass>
ZERSHolder<ZERSResource> ZERSResource::LoadTemplate(const ZEString& FileName, const ZERSLoadingOptions* Options)
{
	zeLog("Loading resource. Resource Class: \"%s\", File Path: \"%s\".", GetClass()->GetName(), FileName.ToCString());
	return ZERSManager::GetInstance()->LoadResource(FileName, Instanciator<ZEResourceClass>(), Options);
}
