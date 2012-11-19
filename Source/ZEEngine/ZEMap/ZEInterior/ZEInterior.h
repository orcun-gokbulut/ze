//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInterior.h
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
#ifndef __ZE_INTERIOR_H__
#define __ZE_INTERIOR_H__

#include "ZEGame/ZEEntity.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"


#include "ZEML/ZEMLNode.h"

enum ZEInteriorCullMode
{
	ZE_ICM_NONE,
	ZE_ICM_VIEW,
	ZE_ICM_FULL
};

struct ZEInteriorCullStatistics
{
	size_t	TotalRoomCount;
	size_t	CulledRoomCount;
	size_t	DrawedRoomCount;

	size_t	TotalInteriorPolygonCount;
	size_t	CulledInteriorPolygonCount;
	size_t	DrawedInteriorPolygonCount;

	size_t	TotalInteriorMaterialCount;
	size_t	CulledInteriorMaterialCount;
	size_t	DrawedInteriorMaterialCount;

};

ZE_META_ENTITY_DESCRIPTION(ZEInterior)

class ZEInteriorResource;
class ZEInteriorRoom;
class ZEInteriorDoor;
struct ZEDrawParameters;
struct ZEInteriorCullStatistics;
class ZERay;
class ZEVector3;
class ZEViewVolume;
class ZEViewFrustum;

class ZEInterior : public ZEEntity
{
	ZE_META_ENTITY(ZEInterior)
	friend class ZEInteriorDoor;
	private:
		ZEString								InteriorFile;
		ZEInteriorResource*						Resource;
		ZEArray<ZEInteriorRoom*>				Rooms;
		ZEArray<ZEInteriorDoor*>				Doors; 
		ZEInteriorCullMode						CullMode;
		ZEInteriorCullStatistics				Statistics;

		void									LoadInteriorResource(ZEInteriorResource* Resource);

		static bool								GenerateViewVolume(ZEViewFrustum& NewViewVolume, ZEInteriorDoor* Door, const ZEViewVolume* OldViewVolume);
		void									CullRoom(ZEInteriorDoor* Door, ZEDrawParameters* DrawParameters, ZEViewVolume* ViewVolume);
		void									CullRooms(ZEDrawParameters* DrawParameters);

		virtual	void							OnTransformChanged();

												ZEInterior();
												~ZEInterior();

	public:	
		const ZEArray<ZEInteriorRoom*>&			GetRooms();
		const ZEArray<ZEInteriorDoor*>&			GetDoors();

		const ZEInteriorCullStatistics&			GetCullStatistics();

		virtual ZEDrawFlags						GetDrawFlags() const;

		virtual bool							Initialize();
		virtual void							Deinitialize();

		ZEInteriorResource*						GetResource() const;

		virtual void							Draw(ZEDrawParameters* DrawParameters);
		virtual bool							CastRay(const ZERay& Ray, ZEVector3& Position, ZEVector3& Normal, float& MinT);

		virtual bool							SetInteriorFile(const ZEString& FileName);
		virtual const ZEString&					GetInteriorFile() const;

		void									SetCullMode(ZEInteriorCullMode Value);

		static ZEInterior*						CreateInstance();

};

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta>
		<class name="ZEInterior" parent="ZEEntity" description="Interior Map">
			<property name="InteriorFile"
				type="string"
				autogetset="true"
				default=""
				description="Interior file"
				semantic="ZE_PS_FILENAME"
				fileextension="*.zeInterior"/>
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/
#endif
