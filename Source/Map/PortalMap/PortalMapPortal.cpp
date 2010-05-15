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
		/*for (size_t N = 0; N < Lights.GetCount(); N++)
			RenderOrders[I].Lights.Add(Lights[N]->GetRenderOrderLight());*/

		Renderer->AddToRenderList(&RenderOrders[I]);
	}
}

#include "Core/Console.h"
bool ZEPortalMapPortal::Initialize(ZEPortalMap* Owner, ZEPortalMapResourcePortal* Resource)
{	
	//int LastIteration = zeCore->GetFrameId() + 1;

	if (VertexBuffer == NULL)
		VertexBuffer = ZEStaticVertexBuffer::CreateInstance();

	if (!VertexBuffer->Create(Resource->Polygons.GetCount() * 3 * sizeof(ZEMapVertex)))
		return false;
	
	ZEArray<bool> Processed;
	Processed.SetCount(Resource->Polygons.GetCount());
	Processed.FillWith(false);

	size_t VertexIndex = 0;
	ZEMapVertex* Buffer = (ZEMapVertex*)VertexBuffer->Lock();	
	for (size_t N = 0; N < Resource->Polygons.GetCount(); N++)
	{
		//if (Resource->Polygons[N].LastIteration != LastIteration)
		if (!Processed[N])
		{
			zeLog("Portal", "New render order.");
			ZEMaterial* Material = Resource->Polygons[N].Material;
			ZERenderOrder* RenderOrder = RenderOrders.Add();

			RenderOrder->SetZero();
			RenderOrder->Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING;
			RenderOrder->Material = Material;
			RenderOrder->PrimitiveType = ZE_ROPT_TRIANGLE;
			RenderOrder->VertexBufferOffset = 0;
			RenderOrder->VertexBuffer = VertexBuffer;
			RenderOrder->VertexDeclaration = ZEMapVertex::GetVertexDeclaration();
			ZEMatrix4x4::CreateIdentity(RenderOrder->WorldMatrix);
			
			RenderOrder->PrimitiveCount = 0;
			for (size_t I = N; I < Resource->Polygons.GetCount(); I++)
			{
				memcpy(Buffer + VertexIndex, Resource->Polygons[I].Vertices, sizeof(ZEMapVertex) * 3);
				zeLog("Portal", "I: %d, N: %d, VertexIndex: %d, PrimitiveCount: %d, Position: [%f, %f, %f]", I, N, VertexIndex, RenderOrder->PrimitiveCount,
					Buffer[VertexIndex].Position.x, Buffer[VertexIndex].Position.y, Buffer[VertexIndex].Position.z);
				VertexIndex += 3;
				RenderOrder->PrimitiveCount++;
				//Resource->Polygons[I].LastIteration = LastIteration;
				Processed[I] = true;
			}

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
