//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERSResourceManager.h
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

#include "ZEMeta\ZEObject.h"

#include "ZECommon.h"
#include "ZEDS\ZEList2.h"
#include "ZEDS\ZEArray.h"
#include "ZERSHolder.h"
#include "ZERSDefinitions.h"

class ZERSResource;
class ZERSResourceGroup;

class ZERSResourceManager : public ZEObject
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZERSResourceManager)
	friend class ZERSResource;
	private:
		ZESize									ResourceGroupIndex;
		ZEArray<ZERSResourceGroup*>				ResourceGroups;

		ZESize									ResourceIndex;
		ZESize									ResourceCount;
		ZESize									SharedResourceCount;

		ZESize									MemoryUsage[ZERS_MEMORY_POOL_COUNT];
		ZESize									MemoryUsageShared[ZERS_MEMORY_POOL_COUNT];
		ZELock									ManagerLock;

		void									UpdateMemoryUsage(ZERSResource* Resource, ZESSize MemoryUsageDelta[ZERS_MEMORY_POOL_COUNT]);

		ZERSResourceGroup*						CreateResourceGroup(ZEClass* ResourceClass);
		ZERSResourceGroup*						GetResourceGroupInternal(ZEClass* ResourceClass);

		ZERSHolder<const ZERSResource>			GetResourceInternal(ZEClass* ResourceClass, const ZEGUID& GUID);
		ZERSHolder<const ZERSResource>			GetResourceInternal(ZEClass* ResourceClass, const ZEString& FileName);
	
		void									RegisterResourceInternal(ZERSResource* Resource);
		void									UnregisterResourceInternal(ZERSResource* Resource);

		void									ShareResourceInternal(ZERSResource* Resource);
		void									UnshareResourceInternal(ZERSResource* Resource);

		void									UnregisterResource(ZERSResource* Resource);

		void									DestroyResource(const ZERSResource* Resource);

												ZERSResourceManager();
												~ZERSResourceManager();

	public:
		ZEArray<const ZERSResourceGroup*>		GetResourceGroups();
		const ZERSResourceGroup*				GetResourceGroup(ZEClass* ResourceClass);

		ZESize									GetResourceCount(); 
		ZESize									GetSharedResourceCount();

		ZESize									GetMemoryUsage(ZERSMemoryPool Pool);
		ZESize									GetMemoryUsageShared(ZERSMemoryPool Pool);
		ZESize									GetMemoryUsageTotal();

		ZERSHolder<const ZERSResource>			GetResource(ZEClass* ResourceClass, const ZEGUID& GUID);
		ZERSHolder<const ZERSResource>			GetResource(ZEClass* ResourceClass, const ZEString& FileName);

		void									RegisterResource(ZERSResource* Resource);

		void									ShareResource(ZERSResource* Resource);
		void									UnshareResource(ZERSResource* Resource);

		ZERSHolder<const ZERSResource>			StageResource(ZEClass* ResourceClass, const ZEGUID& GUID, ZERSInstanciator Insanciaor, const void* InstanciatorParameter = NULL, ZERSResource** StagingResource = NULL);
		ZERSHolder<const ZERSResource>			StageResource(ZEClass* ResourceClass, const ZEString& FileName, ZERSInstanciator Insanciaor, const void* InstanciatorParameter = NULL, ZERSResource** StagingResource = NULL);

		// Commands
		ZEString								ShowStats();

		ZEString								ListResources(const ZEString& Filter = ZEString::Empty);
		ZEString								ShowResource(ZESize Index);
		
		ZEString								ListResourceGroups(const ZEString& Filter = ZEString::Empty);
		ZEString								ShowResourceGroup(ZESize Index);

		static ZERSResourceManager*				GetInstance();
};
