//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERSManager.h
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
#include "ZERSResource.h"
#include "ZEDS\ZEList2.h"
#include "ZECommon.h"

typedef ZERSResource* ZERSInstanciator();

class ZERSManager : public ZEObject
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZERSManager)
	friend class ZERSResource;
	private:
		ZELock ManagerLock;
		ZEList2<const ZERSResource>	Resources;

		ZESize MemoryUsage[ZERS_P_TOTAL];
		ZESize CacheSize;
		ZESize CacheUsage;
		
		ZERSHolder<const ZERSResource> GetResourceInternal(const ZEString& FilePath);
		void RegisterResourceInternal(const ZERSResource* Resource);
		ZERSHolder<const ZERSResource> LoadResource(const ZEString& FilePath, ZERSInstanciator Instanciator, ZERSLoadingOptions* LoadingOptions);

		void RegisterResource(const ZERSResource* Resource);
		void ReleaseResource(const ZERSResource* Resource);
									
		ZERSManager();
		~ZERSManager();

	public:
		ZESize GetMemoryUsage(ZERSPool Pool = ZERS_P_TOTAL);
		ZESize GetCacheUsage();

		void SetCacheSize(ZESize Size);
		ZESize SetCacheSize();

		ZERSHolder<const ZERSResource> GetResource(const ZEString& FilePath);
		ZERSHolder<const ZERSResource> GetResource(const ZEGUID& GUID);

		static ZERSManager* GetInstance();
};
