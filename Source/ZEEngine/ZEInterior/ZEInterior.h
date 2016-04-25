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

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEML/ZEMLNode.h"
#include "ZEGame/ZEEntity.h"
#include "ZEGame/ZERayCast.h"

ZE_ENUM(ZEInteriorCullMode)
{
	ZE_ICM_NONE,
	ZE_ICM_VIEW,
	ZE_ICM_FULL
};

class ZERay;
class ZEViewVolume;
class ZEViewFrustum;
class ZEInteriorResource;
class ZEInteriorRoom;
class ZEInteriorDoor;
class ZEInteriorHelper;

struct ZEExtraRenderParameters
{
	ZEUInt VertexOffset;
	ZEUInt VertexCount;
	ZERNMaterial* Material;
	ZEInteriorRoom* Room;
};

class ZEInterior : public ZEEntity
{
	friend class ZEInteriorDoor;
	friend class ZEInteriorHelper;
	friend class ZEInteriorDebugDrawer;

	ZE_OBJECT

	private:
		const ZEInteriorResource*				InteriorResource;

		ZEArray<ZEInteriorRoom*>				Rooms;
		ZEArray<ZEInteriorDoor*>				Doors;
		ZEArray<ZEInteriorHelper*>				Helpers;
		ZEInteriorCullMode						CullMode;

		void									LoadInteriorResource();

		static bool								GenerateViewVolume(ZEViewFrustum& NewViewVolume, ZEInteriorDoor* Door, const ZEViewVolume* OldViewVolume);
		void									CullRoom(ZEInteriorDoor* Door, const ZERNPreRenderParameters* Parameters, ZEViewVolume* ViewVolume);
		void									CullRooms(const ZERNPreRenderParameters* Parameters);

		virtual	void							ParentTransformChanged();

	protected:
		virtual bool							InitializeSelf();
		virtual bool							DeinitializeSelf();

												ZEInterior();
		virtual									~ZEInterior();

	public:	
		const ZEArray<ZEInteriorRoom*>&			GetRooms();
		const ZEArray<ZEInteriorDoor*>&			GetDoors();
		const ZEArray<ZEInteriorHelper*>&		GetHelpers();

		ZEInteriorRoom*							GetRoom(const ZEString& Name);
		ZEInteriorDoor*							GetDoor(const ZEString& Name);
		ZEInteriorHelper*						GetHelper(const ZEString& Name);

		virtual ZEDrawFlags						GetDrawFlags() const;

		virtual void							SetInteriorFile(const ZEString& InteriorFile);
		virtual const ZEString&					GetInteriorFile() const;

		void									SetInteriorResource(const ZEInteriorResource* InteriorResource);	
		const ZEInteriorResource*				GetInteriorResource() const;

		void									SetCullMode(ZEInteriorCullMode Value);
		ZEInteriorCullMode						GetCullMode() const;

		virtual bool							PreRender(const ZERNPreRenderParameters* Parameters);
		virtual void							Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command);

		virtual bool							RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters);

		static ZEInterior*						CreateInstance();
};
