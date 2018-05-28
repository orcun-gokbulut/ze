//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESectorManager.h
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

#include "ZEMeta/ZEObject.h"
#include "ZESector.h"
#include "ZEDS/ZEList2.h"
#include "ZEDS/ZEArray.h"

class ZESectorSelector;
class ZEGeographicEntity;
class ZEScene;

class ZESectorManager : public ZEObject
{
	ZE_OBJECT
	friend class ZEScene;
	private:
		ZEScene*							Scene;
		ZESector* 							OriginSector;
		ZEUInt32							CacheDepth;

		ZEList2<ZEGeographicEntity>			Sectors;
		ZEArray<ZESector*>					SectorCache;
		ZEArray<ZESector*>					ActiveSectors;

		ZEList2<ZEGeographicEntity>			Selectors;
	
		void								ActivateSector(ZESector* Sector);
		void								DeactivateSector(ZESector* Sector);

		void								AddToCache(ZESector* Sector);
		void								RemoveFromCache(ZESector* Sector);

		void								FollowLinks(ZEArray<ZESector*>& ActivateList, ZEArray<ZESector*>& CacheList, ZESector* Sector, ZESSize ActivateDepth, ZESSize CacheDepth);
		void								UpdateTransformations();
		void								UpdateActiveSectors();

											ZESectorManager();
		virtual								~ZESectorManager();

	public:
		ZEScene*							GetScene() const;

		void								SetCacheDepth(ZEUInt32 Depth);
		ZESize								GetCacheDepth() const;

		const ZEList2<ZEGeographicEntity>&	GetSectors() const;
		ZEArray<ZESector*>					GetSectors(const ZEVector3d& Position) const;
		ZESector*							GetSector(const ZEGUID& Id) const;
		ZESector*							GetSector(const ZEVector3d& Position, bool Proximity = false) const;
		ZESector*							GetSector(const ZEString& Name);
		ZESector*							GetOriginSector();
		bool								AddSector(ZESector* Sector);
		void								RemoveSector(ZESector* Sector);
		void								ClearSectors();

		const ZEList2<ZEGeographicEntity>&	GetSelectors() const;
		bool								AddSelector(ZESectorSelector* Selector);
		void								RemoveSelector(ZESectorSelector* Selector);
		void								ClearSelectors();

		virtual void						Process(float Time);

		bool								Serialize(const ZEString& FileName);
		bool								Unserialize(const ZEString& FileName);

		virtual void						Destroy();
		static ZESectorManager*				CreateInstance();
};
