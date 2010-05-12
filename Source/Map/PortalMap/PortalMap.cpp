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
		Doors.SetCount(Resource->GetDoors().GetCount());
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

/*void ZEPortalMap::RenderPortal(ZEMapPortal* Portal, ZERenderer* Renderer, const ZEViewVolume& ViewVolume, ZESmartArray<ZELight*>& SceneLights)
{
	if (ViewVolume.CullTest(Portal->BoundingBox))
	{
		ZEMatrix4x4::CreateOrientation(PortalBBoxRenderOrder.WorldMatrix, Portal->BoundingBox.GetCenter(), 
			ZEQuaternion::Identity, 
			ZEVector3(Portal->BoundingBox.Max.x - Portal->BoundingBox.Min.x, 
			Portal->BoundingBox.Max.y - Portal->BoundingBox.Min.y, 
			Portal->BoundingBox.Max.z - Portal->BoundingBox.Min.z)
			);
		//Renderer->AddToRenderList(&PortalBBoxRenderOrder); 

		ZESmartArray<ZELight*> PortalLights;
		for (size_t I = 0; I < SceneLights.GetCount(); I++)
		{
			const ZEViewVolume& LightViewVolume = SceneLights[I]->GetViewVolume();
			if (SceneLights[I]->GetLightType() != ZE_LT_DIRECTIONAL || LightViewVolume.CullTest(Portal->BoundingBox))
				PortalLights.Add(SceneLights[I]);
		}

		if (Portal->Octree != NULL)
			Portal->Octree->Render(Renderer, ViewVolume, PortalLights);
		else
			for (size_t I = 0; I < Portal->RenderOrders.GetCount(); I++)
			{
				if (Portal->RenderOrders[I].Lights.GetCount() != SceneLights.GetCount())
					Portal->RenderOrders[I].Lights.SetCount(SceneLights.GetCount());

				for (size_t N = 0; N < PortalLights.GetCount(); N++)
					Portal->RenderOrders[I].Lights[N] = PortalLights[N]->GetRenderOrderLight();

				Renderer->AddToRenderList(&Portal->RenderOrders[I]);
			}
	}
}*/

void ZEPortalMap::Render(ZERenderer* Renderer,  const ZESmartArray<const ZELight*>& SceneLights)
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
