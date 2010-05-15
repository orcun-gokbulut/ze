//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PortalMap.cpp
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

#include "PortalMap.h"
#include "PortalMapResource.h" 
#include "PortalMapPortal.h"
#include "PortalMapDoor.h"
#include "Core/Error.h"
#include "Core/Console.h"

ZEPortalMap::ZEPortalMap()
{
	Resource = NULL;
}

ZEPortalMap::~ZEPortalMap()
{
	Deinitialize();
}

const ZEArray<ZEPortalMapPortal>& ZEPortalMap::GetPortals()
{
	return Portals;
}

const ZEArray<ZEPortalMapDoor>& ZEPortalMap::GetDoors()
{
	return Doors;
}

bool ZEPortalMap::Initialize()
{
	if (Resource != NULL)
	{
		Portals.SetCount(Resource->GetPortals().GetCount());
		
		for (size_t I = 0; I < Portals.GetCount(); I++)
			Portals[I].Initialize(this, (ZEPortalMapResourcePortal*)&Resource->GetPortals()[I]);
		
		Doors.SetCount(Resource->GetDoors().GetCount());
		for (size_t I = 0; I < Doors.GetCount(); I++)
			Doors[I].Initialize(this, &Resource->GetDoors()[I]);
	}

	return true;
}

void ZEPortalMap::Deinitialize()
{
	if (Resource != NULL)
	{
		Resource->Release();
		Resource = NULL;
	}
}

ZEMapResource* ZEPortalMap::GetResource()
{
	return Resource;
}

bool ZEPortalMap::SetResource(ZEMapResource* Resource)
{
	if (strcmp(Resource->GetResourceType(), "Portal Map Resource") == 0)
	{
		this->Resource = (ZEPortalMapResource*)Resource;
		Initialize();
		return true;
	}

	return false;
}

void ZEPortalMap::Render(ZERenderer* Renderer,  const ZESmartArray<ZELight*>& SceneLights)
{
	for (size_t I = 0; I < Portals.GetCount(); I++)
		Portals[I].Draw(Renderer, SceneLights);
}

bool ZEPortalMap::CastRay(const ZERay& Ray, ZEVector3& Position, ZEVector3& Normal, float& MinT)
{
	return false;
}

ZEPortalMap* ZEPortalMap::CreateInstance()
{
	return new ZEPortalMap();
}
