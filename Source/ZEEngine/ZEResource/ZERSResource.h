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

#include "ZERSDefinitions.h"

#include "ZETypes.h"
#include "ZEGUID.h"
#include "ZECommon.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZELink.h"
#include "ZEDS/ZEString.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEThread/ZETask.h"

class ZERSResourceManager;
class ZERSResourceGroup;

class ZERSResourceIdentifier
{
	public:
		virtual bool							Equals(const ZERSResourceIdentifier* Identifier) const = 0;
		virtual ZEString						ToString() const;
};

class ZERSResource : public ZEObject, public ZEReferenceCounted
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZERSResource)
	friend class ZERSResourceManager;
	friend class ZERSResourceGroup;
	friend class ZERSTemplates;
	template<typename ZERSResourceGroup> friend class ZERSHolder;
	private:
		const ZERSResourceManager*				Manager;
		const ZERSResourceGroup*				Group;

		ZERSResource*							Parent;
		mutable ZELink<ZERSResource>			ManagerLink;
		mutable ZELink<const ZERSResource>		ManagerSharedLink;

		ZEGUID									GUID;
		ZEString								FileName;
		ZEUInt32								FileNameHash;
		const ZERSResourceIdentifier*			Identifier;

		ZEArray<ZERSResource*>					ChildResources;
		ZEArray<const ZERSResource*>			ExternalResources;

		ZESize									Index;
		mutable bool							Shared;
		ZESize									MemoryUsageSelf[ZERS_MEMORY_POOL_COUNT];
		ZESize									MemoryUsage[ZERS_MEMORY_POOL_COUNT];
		ZERSResourceState						State;
		ZERSResourceState						TargetState;
		ZETask									UpdateStateTask;

		ZEUInt									LocalLoadProgress;

		bool									LoadInternalDone;
		bool									UnloadInternalDone;
		bool									ReloadFlag;

		mutable ZELock							ResourceLock;
		ZELock									StateLock;

		virtual void							Reference() const;
		virtual void							Release() const;
		virtual void							Destroy() const;

		ZETaskResult							UpdateStateFunction(ZETaskThread* TaskThread, void* Parameters);
		void									UpdateMemoryConsumption();

		bool									UpdateStateSerial();
		void									UpdateState();

	protected:
		void									SetGUID(const ZEGUID& GUID);
		void									SetFileName(const ZEString& FileName);
		void									SetIdentifier(const ZERSResourceIdentifier* Identifier);

		void									SetMemoryUsage(ZERSMemoryPool Pool, ZESize Size);

		void									SetLocalLoadProgress(ZEUInt Percentage);
		void									SetLocalLoadProgress(ZESize Index, ZESize Count, ZEUInt StartPercentage, ZEUInt EndPercentage);
		ZEUInt									GetLocalLoadProgress();

		void									AddChildResource(ZERSResource* Resource);
		void									RemoveChildResource(ZERSResource* Resource);

		void									RegisterExternalResource(const ZERSResource* Resource);
		void									UnregisterExternalResource(const ZERSResource* Resource);

		void									Register();
		void									Unregister();

		virtual ZETaskResult					LoadInternal();
		virtual ZETaskResult					UnloadInternal();
		virtual void							PreDestroy();

												ZERSResource();
		virtual									~ZERSResource();

	public:
		ZERSResource*							GetParent();
		const ZERSResource*						GetParent() const;
		const ZERSResourceManager*				GetManager() const;
		const ZERSResourceGroup*				GetGroup() const;

		ZESize									GetIndex() const;
		const ZEGUID&							GetGUID() const;
		const ZEString&							GetFileName() const;
		ZEUInt32								GetFileNameHash() const;
		const ZERSResourceIdentifier*			GetIdentifier() const;

		ZERSResourceState						GetState() const;
		ZEUInt									GetLoadProgress() const;
		ZESize									GetReferenceCount() const;
		ZESize									GetMemoryUsage(ZERSMemoryPool Pool) const;
		ZESize									GetTotalMemoryUsage() const;

		bool									IsDestroyed() const;
		bool									IsStaged() const;
		bool									IsLoaded() const;
		bool									IsLoadedOrLoading() const;
		bool									IsFailed() const;

		bool									IsShared() const;
		void									Share();
		void									Unshare();

		void									StagingRealized();
		void									StagingFailed();

		void									Load();
		void									Unload();
		void									Reload();

		bool									LoadSerial();
		bool									UnloadSerial();
		bool									ReloadSerial();

		void									WaitStaging() const;
		void									WaitLoading() const;
		void									WaitUnloading() const;

};
