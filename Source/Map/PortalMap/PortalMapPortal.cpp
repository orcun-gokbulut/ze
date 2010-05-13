//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PortalMapPortal.cpp
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
#include "Core/Core.h"
#include "PortalMapPortal.h"
#include "PortalMapResource.h"
#include "Physics/PhysicalStaticMesh.h"
#include "Graphics/Light.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/Renderer.h"

ZEPortalMap* ZEPortalMapPortal::GetOwner()
{
	return Owner;
}

const char* ZEPortalMapPortal::GetName()
{
	if (Resource != NULL)
		return Resource->Name;
	else
		return "";
}

const ZEArray<ZEPortalMapDoor*>& ZEPortalMapPortal::GetDoors()
{
	return Doors;
}

const ZEAABoundingBox& ZEPortalMapPortal::GetBoundingBox()
{
	return Resource->BoundingBox;
}

ZEPhysicalStaticMesh* ZEPortalMapPortal::GetPhysicalMesh()
{
	return PhysicalMesh;
}

void ZEPortalMapPortal::Draw(ZERenderer* Renderer,  const ZESmartArray<ZELight*>& Lights)
{
	for(size_t I = 0; I < RenderOrders.GetCount(); I++)
	{
		RenderOrders[I].Lights.Clear();
		for (size_t N = 0; N < Lights.GetCount(); N++)
			RenderOrders[I].Lights.Add(Lights[N]->GetRenderOrderLight());

		Renderer->AddToRenderList(&RenderOrders[I]);
	}
}

bool ZEPortalMapPortal::Initialize(ZEPortalMap* Owner, const ZEPortalMapResourcePortal* Resource)
{
	int LastIteration = zeCore->GetFrameId() + 1;
	if (VertexBuffer == NULL)
		VertexBuffer = ZEStaticVertexBuffer::CreateInstance();

	if (!VertexBuffer->Create(Resource->Polygons.GetCount() * 3 * sizeof(ZEMapVertex)))
		return false;

	ZEMapVertex* Buffer = (ZEMapVertex*)VertexBuffer->Lock();

	size_t VertexIndex = 0;
	int RenderOrderIndex = -1;
	for (size_t I = 0; I < Resource->Polygons.GetCount(); I++)
		if (Resource->Polygons[I].LastIteration != LastIteration)
		{
			ZEMaterial* Material = Resource->Polygons[I].Material;
			
			ZERenderOrder* RenderOrder = RenderOrders.Add();
			
			RenderOrder->SetZero();
			RenderOrder->Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING;
			RenderOrder->Material = Material;
			RenderOrder->PrimitiveType = ZE_ROPT_TRIANGLE;
			RenderOrder->VertexBufferOffset = sizeof(ZEMapVertex) * VertexIndex;
			RenderOrder->VertexBuffer = VertexBuffer;
			RenderOrder->VertexDeclaration = ZEMapVertex::GetVertexDeclaration();
			ZEMatrix4x4::CreateIdentity(RenderOrder->WorldMatrix);
			
			RenderOrder->PrimitiveCount = 0;
			for (size_t N = I; N < Resource->Polygons.GetCount(); N++)
				if (Resource->Polygons[N].Material == Material)
				{
					(Buffer + VertexIndex)[0] = Resource->Polygons[N].Vertices[0];
					(Buffer + VertexIndex)[1] = Resource->Polygons[N].Vertices[1];
					(Buffer + VertexIndex)[2] = Resource->Polygons[N].Vertices[2];
					//memcpy(Buffer + VertexIndex, Resource->Polygons[N].Vertices, sizeof(ZEMapVertex) * 3);
					RenderOrder->PrimitiveCount++;
					VertexIndex += 3;
					(*(unsigned int*)&Resource->Polygons[N].LastIteration) = LastIteration;	
				} 
		}
	VertexBuffer->Unlock();

	return true;
}

void ZEPortalMapPortal::Deinitialize()
{
	Owner = NULL;
	Resource = NULL;
	if (VertexBuffer != NULL)
	{
		VertexBuffer->Destroy();
		VertexBuffer = NULL;
	}

	if (PhysicalMesh != NULL)
	{
		PhysicalMesh->Destroy();
		PhysicalMesh = NULL;
	}
}

ZEPortalMapPortal::ZEPortalMapPortal()
{
	Owner = NULL;
	Resource = NULL;
	PhysicalMesh = NULL;
	VertexBuffer = NULL;
}

ZEPortalMapPortal::~ZEPortalMapPortal()
{
	Deinitialize();
}
