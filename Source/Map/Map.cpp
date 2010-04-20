//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Map.cpp
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

#include "Map.h"
#include "MapResource.h" 
#include "Core/Error.h"
#include "Core/Console.h"
#include "Graphics/RenderOrder.h"
#include "Graphics/Light.h"
#include "Graphics/ViewVolume.h"
#include "ZEMath/Triangle.h"
#include "ZEMath/Ray.h"
#include "ZEMath/Ray.h"
#include "Physics/PhysicalStaticMesh.h"

ZEEnvironment::ZEEnvironment()
{
}

ZEEnvironment::~ZEEnvironment()
{
}

void ZEEnvironment::Destroy()
{
	Deinitialize();
	delete this;
}

bool ZEPortalMap::Initialize()
{
	PortalBBoxCanvas.Clean();
	PortalBBoxCanvas.AddWireframeBox(1.0f, 1.0f, 1.0f);

	if (PortalBBoxMaterial == NULL)
		PortalBBoxMaterial = ZEFixedMaterial::CreateInstance();

	PortalBBoxMaterial->SetZero();
	PortalBBoxMaterial->SetAmbientEnabled(true);
	PortalBBoxMaterial->SetAmbientColor(ZEVector3(0.75f, 0.75f, 0.75f));
	PortalBBoxMaterial->SetLightningEnabled(false);
	PortalBBoxMaterial->UpdateMaterial();

	PortalBBoxRenderOrder.SetZero();
	PortalBBoxRenderOrder.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_TRANSPARENT | ZE_ROF_ENABLE_Z_CULLING;
	PortalBBoxRenderOrder.VertexDeclaration = ZESimpleVertex::GetVertexDeclaration();
	PortalBBoxRenderOrder.PrimitiveType = ZE_ROPT_LINE;
	PortalBBoxRenderOrder.Material = PortalBBoxMaterial;
	PortalBBoxRenderOrder.VertexBuffer = &PortalBBoxCanvas;
	PortalBBoxRenderOrder.PrimitiveCount = PortalBBoxCanvas.Vertices.GetCount() / 2;

	ZEOctree::OctreeBBoxCanvas.Clean();
	ZEOctree::OctreeBBoxCanvas.AddWireframeBox(1.0f, 1.0f, 1.0f);

	if (ZEOctree::OctreeBBoxMaterial == NULL)
		ZEOctree::OctreeBBoxMaterial = ZEFixedMaterial::CreateInstance();
	
	ZEOctree::OctreeBBoxMaterial->SetZero();
	ZEOctree::OctreeBBoxMaterial->SetAmbientEnabled(true);
	ZEOctree::OctreeBBoxMaterial->SetAmbientColor(ZEVector3(0.25f, 0.25f, 0.25f));
	ZEOctree::OctreeBBoxMaterial->SetLightningEnabled(false);
	ZEOctree::OctreeBBoxMaterial->UpdateMaterial();

	ZEOctree::OctreeBBoxRenderOrder.SetZero();
	ZEOctree::OctreeBBoxRenderOrder.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_TRANSPARENT | ZE_ROF_ENABLE_Z_CULLING;
	ZEOctree::OctreeBBoxRenderOrder.PrimitiveType = ZE_ROPT_LINE;
	ZEOctree::OctreeBBoxRenderOrder.VertexDeclaration = ZESimpleVertex::GetVertexDeclaration();
	ZEOctree::OctreeBBoxRenderOrder.Material = PortalBBoxMaterial;
	ZEOctree::OctreeBBoxRenderOrder.VertexBuffer = &PortalBBoxCanvas;
	ZEOctree::OctreeBBoxRenderOrder.PrimitiveCount = PortalBBoxCanvas.Vertices.GetCount() / 2;

	return true;
}

void ZEPortalMap::Deinitialize()
{
	if (PortalBBoxMaterial != NULL)
	{
		PortalBBoxMaterial->Destroy();
		PortalBBoxMaterial = NULL;
	}

	if (MapResource != NULL)
	{
		MapResource->Release();
		MapResource = NULL;
	}
}

const char* ZEPortalMap::GetFileName()
{
	if (MapResource != NULL)
		return MapResource->GetFileName();
	else
		return "";
}

bool ZEPortalMap::Load(const char* FileName)
{
	zeLog("Loading map file \"%s\".\r\n", FileName);
	if (MapResource != NULL)
	{
		MapResource->Release();
		MapResource = NULL;
	}

	if (strcmp(FileName, "") == 0)
		return true;

	MapResource = ZEMapResource::LoadResource(FileName);
	if (MapResource == NULL)
	{
		zeError("Map", "Could not load map file. (FileName : \"%s\")", FileName);
		return false;
	}

	return true;
}

void ZEPortalMap::RenderPortal(ZEMapPortal* Portal, ZERenderer* Renderer, const ZEViewVolume& ViewVolume, ZESmartArray<ZELight*>& SceneLights)
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
}
void ZEPortalMap::Render(ZERenderer* Renderer, const ZEViewVolume& ViewVolume, ZESmartArray<ZELight*>& SceneLights)
{
	if (MapResource != NULL)
		for (size_t I = 0; I < MapResource->Portals.GetCount(); I++)
			RenderPortal(&MapResource->Portals[I], Renderer, ViewVolume, SceneLights);
}

bool ZEPortalMap::CastRay(const ZERay& Ray, ZEVector3& Position, ZEVector3& Normal, float& MinT)
{
	if (MapResource == NULL)
		return false;

	float T, CurMinT = 100000000000000.0f;
	bool Found = false;
	for (size_t I = 0; I < MapResource->Portals.GetCount(); I++)
	{
		ZEMapPortal* CurrentPortal = &MapResource->Portals[I];
		//if (ZEAABoundingBox::IntersectionTest(CurrentPortal->BoundingBox,Ray))
			for (size_t N = 0; N < CurrentPortal->Polygons.GetCount(); N++)
			{
				ZEMapPolygon& MapPolygon = MapResource->Portals[I].Polygons[N];
				ZETriangle Triangle(MapPolygon.Vertices[0].Position, MapPolygon.Vertices[1].Position, MapPolygon.Vertices[2].Position);
				if (ZETriangle::IntersectionTest(Triangle, Ray, T) && (!Found || MinT > T))
				{
					MinT = T;
					Ray.GetPointOn(Position, T);
					ZETriangle::GetNormal(Triangle, Normal);
					Found = true;
				}
			}
	}
	return Found;
}

ZEPortalMap::ZEPortalMap()
{
	MapResource = NULL;
	PortalBBoxMaterial = NULL;
}

ZEPortalMap::~ZEPortalMap()
{
	Destroy();
}

ZEPortalMap* ZEPortalMap::CreateInstance()
{
	return new ZEPortalMap();	
}
