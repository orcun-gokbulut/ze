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


class ZERSResource : public ZEObject, public ZEReferenceCounted
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZERSResource)
	friend class ZERSResourceManager;
	friend class ZERSResourceLoadable;
	friend class ZERSTemplates;
	template<typename ZERSResourceClass> friend class ZERSHolder;
	private:
		ZEGUID									GUID;
		ZERSResourceState						State;
		ZERSResource*							Parent;
		mutable ZELink<const ZERSResource>		ManagerLink;
		mutable ZELink<const ZERSResource>		ManagerSharedLink;
		ZEArray<ZERSResource*>					ChildResources;
		mutable bool							Shared;
		ZESize									MemoryUsageSelf[ZERS_MEMORY_POOL_COUNT];
		ZESize									MemoryUsage[ZERS_MEMORY_POOL_COUNT];
		
		virtual void							Reference() const;
		virtual void							Release() const;
		virtual void							Destroy() const;

		void									UpdateMemoryConsumption();

	protected:
		mutable ZELock							ResourceLock;

		void									SetMemoryUsage(ZERSMemoryPool Pool, ZESize Size);

		void									AddChildResource(ZERSResource* Resource);
		void									RemoveChildResource(ZERSResource* Resource);

		void									SetGUID(const ZEGUID& GUID);

												ZERSResource();
		virtual									~ZERSResource();

	public:
		ZERSResourceType						GetType() const;

		const ZEGUID&							GetGUID() const;
		ZERSResourceState						GetState() const;
		ZERSResource*							GetParent();
		const ZERSResource*						GetParent() const;
				
		ZESize									GetReferenceCount() const;
		ZESize									GetMemoryUsage(ZERSMemoryPool Pool) const;
		ZESize									GetTotalMemoryUsage() const;

		bool									IsShared() const;
		void									Share();
		void									Unshare();

		bool									IsStaging();
		void									StagingWait();
		void									StagingRealize();
};
