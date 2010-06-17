//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPortalMapPortalOctree.cpp
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

/*#include "PortalMapPortalOctree.h"
#include "ZEGraphics\ZEGraphicsModule.h"
#include "ZEGraphics\ZERenderer.h"
#include "ZEGraphics\ZERenderOrder.h"
#include "ZEGraphics\ZEViewVolume.h"
#include "ZEGraphics\ZELight.h"
#include "ZEMapResource.h"

ZEFixedMaterial* ZEOctree::OctreeBBoxMaterial;
ZERenderOrder ZEOctree::OctreeBBoxRenderOrder;
ZECanvas ZEOctree::OctreeBBoxCanvas;

bool ZEPortalMapOctree::Initialize()
{
	OctreeBBoxCanvas.Clean();
	OctreeBBoxCanvas.AddWireframeBox(1.0f, 1.0f, 1.0f);

	if (OctreeBBoxMaterial != NULL)
	{
		OctreeBBoxMaterial->Destroy();
		OctreeBBoxMaterial = NULL;
	}

	OctreeBBoxMaterial = ZEFixedMaterial::CreateInstance();

	OctreeBBoxMaterial->SetZero();
	OctreeBBoxMaterial->SetLightningEnabled(false);
	OctreeBBoxMaterial->SetAmbientEnabled(true);
	OctreeBBoxMaterial->SetAmbientColor(ZEVector3(1.0f, 1.0f, 0.0f));

	OctreeBBoxRenderOrder.SetZero();
	OctreeBBoxRenderOrder.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_TRANSPARENT | ZE_ROF_ENABLE_Z_CULLING;
	OctreeBBoxRenderOrder.PrimitiveType = ZE_ROPT_LINE;
	OctreeBBoxRenderOrder.Material = OctreeBBoxMaterial;
	OctreeBBoxRenderOrder.VertexBuffer = &OctreeBBoxCanvas;

	return true;
}

bool ZEPortalMapOctree::Destroy()
{
	return true;
}


void ZEPortalMapOctree::Render(ZERenderer* Renderer, const ZEViewVolume& ViewVolume, ZESmartArray<ZELight*>& Lights)
{
	ZEMatrix4x4::CreateOrientation(OctreeBBoxRenderOrder.WorldMatrix, BoundingBox.GetCenter(), 
	ZEQuaternion::Identity, 
	ZEVector3(BoundingBox.Max.x - BoundingBox.Min.x, 
		BoundingBox.Max.y - BoundingBox.Min.y, 
		BoundingBox.Max.z - BoundingBox.Min.z)
	);
	Renderer->AddToRenderList(&OctreeBBoxRenderOrder);

	for (size_t I = 0; I < 8; I++)
			if (SubTrees[I] != NULL)
				SubTrees[I]->Render(Renderer, ViewVolume, Lights);
	/*if (RenderOrders.GetCount() != 0)
	{
		ZESmartArray<ZELight*> NodeLights;
		for (size_t I = 0; I < Lights.GetCount(); I++)
			if (Lights[I]->GetViewVolume().CullTest(BoundingBox))
				NodeLights.Add(Lights[I]);

		for (size_t I = 0; I < RenderOrders.GetCount(); I++)
		{
			RenderOrders[I].Lights.Clear();
			for (size_t N = 0; N < NodeLights.GetCount(); N++)
				RenderOrders[I].Lights.Add(NodeLights[N]->GetRenderOrderLight());

			Renderer->AddToRenderList(&RenderOrders[I]);
		}

		for (size_t I = 0; I < 8; I++)
			if (SubTrees[I] != NULL && ViewVolume.CullTest(SubTrees[I]->BoundingBox))
				SubTrees[I]->Render(Renderer, ViewVolume, NodeLights);

	}
	else
		for (size_t I = 0; I < 8; I++)
			if (SubTrees[I] != NULL && ViewVolume.CullTest(SubTrees[I]->BoundingBox))
				SubTrees[I]->Render(Renderer, ViewVolume, Lights);*/
/*}


ZEPortalMapOctree::ZEPortalMapOctree()
{
	VertexBuffer = NULL;
	for (size_t I = 0; I < 8; I++)
		SubTrees[I] = NULL;
}

ZEPortalMapOctree::~ZEPortalMapOctree()
{
	if (VertexBuffer != NULL)
		VertexBuffer->Destroy();

	for (size_t I = 0; I < 8; I++)
		if (SubTrees[I] != NULL)
			delete SubTrees[I];
}
*/




