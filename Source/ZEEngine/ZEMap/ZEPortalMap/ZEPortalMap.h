//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPortalMap.h
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
#ifndef __ZE_PORTAL_MAP_H__
#define __ZE_PORTAL_MAP_H__

#include "ZEGame/ZEEntity.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"

struct ZEPortalMapCullStatistics
{
	size_t	TotalPortalCount;
	size_t	CulledPortalCount;
	size_t	DrawedPortalCount;

	size_t	TotalMapPolygonCount;
	size_t	CulledMapPolygonCount;
	size_t	DrawedMapPolygonCount;

	size_t	TotalMapMaterialCount;
	size_t	CulledMapMaterialCount;
	size_t	DrawedMapMaterialCount;

};

ZE_META_ENTITY_DESCRIPTION(ZEPortalMap)

class ZEPortalMapResource;
class ZEPortalMapPortal;
class ZEPortalMapDoor;
struct ZEDrawParameters;
struct ZEPortalMapCullStatistics;
class ZERay;
class ZEVector3;
class ZEViewVolume;
class ZEViewFrustum;

class ZEPortalMap : public ZEEntity
{
	ZE_META_ENTITY(ZEPortalMap)
	friend class ZEPortalMapDoor;
	private:
		ZEString								PortalMapFile;
		ZEPortalMapResource*					Resource;
		ZEArray<ZEPortalMapPortal*>				Portals;
		ZEArray<ZEPortalMapDoor*>				Doors; 
		ZEPortalMapCullStatistics				Statistics;

		void									LoadPortalResource(ZEPortalMapResource* Resource);

		static bool								GenerateViewVolume(ZEViewFrustum& NewViewVolume, const ZEPortalMapDoor* Door, const ZEViewVolume* OldViewVolume);
		void									CullPortal(ZEPortalMapDoor* Door, ZEDrawParameters* DrawParameters, ZEViewVolume* ViewVolume);
		void									CullPortals(ZEDrawParameters* DrawParameters);


												ZEPortalMap();
												~ZEPortalMap();

	public:	
		const ZEArray<ZEPortalMapPortal*>&		GetPortals();
		const ZEArray<ZEPortalMapDoor*>&		GetDoors();

		const ZEPortalMapCullStatistics&		GetCullStatistics();

		virtual ZEDrawFlags						GetDrawFlags() const;

		virtual bool							Initialize();
		virtual void							Deinitialize();

		ZEPortalMapResource*					GetResource() const;

		virtual void							Draw(ZEDrawParameters* DrawParameters);
		virtual bool							CastRay(const ZERay& Ray, ZEVector3& Position, ZEVector3& Normal, float& MinT);

		virtual bool							SetMapFile(const ZEString& FileName);
		virtual const ZEString&					GetMapFile() const;

		static ZEPortalMap*						CreateInstance();

};

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta>
		<class name="ZEPortalMap" parent="ZEEntity" description="Portal map">
			<property name="MapFile"
				type="string"
				autogetset="true"
				default=""
				description="Map file"
				semantic="ZE_PS_FILENAME"
				fileextension="*.zeMap"/>
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/
#endif
