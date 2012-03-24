//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPortalMapPortal.cpp
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

#include "ZECore/ZECore.h"
#include "ZEPortalMapPortal.h"
#include "ZEPortalMapResource.h"
#include "ZEPhysics/ZEPhysicalMesh.h"
#include "ZEGraphics/ZELight.h"
#include "ZEGraphics/ZEVertexBuffer.h"
#include "ZEGraphics/ZERenderer.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGame/ZEScene.h"
#include "ZEPhysics/ZEPhysicalWorld.h"
#include "ZEMap/ZEPortalMap/ZEPortalMap.h"
#include "ZEMath/ZEViewVolume.h"

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

const ZEAABBox& ZEPortalMapPortal::GetBoundingBox()
{
	return Resource->BoundingBox;
}

ZEPhysicalMesh* ZEPortalMapPortal::GetPhysicalMesh()
{
	return PhysicalMesh;
}

size_t ZEPortalMapPortal::GetPolygonCount()
{
	return Resource->Polygons.GetCount();
}

void ZEPortalMapPortal::Draw(ZEDrawParameters* DrawParameters)
{
	for(ZESize I = 0; I < RenderCommands.GetCount(); I++)
	{
		RenderCommands[I].WorldMatrix = Owner->GetWorldTransform();
		RenderCommands[I].Lights.Clear();
		RenderCommands[I].Lights.MassAdd(DrawParameters->Lights.GetConstCArray(), DrawParameters->Lights.GetCount());

		DrawParameters->Renderer->AddToRenderList(&RenderCommands[I]);
	}
}

bool ZEPortalMapPortal::Initialize(ZEPortalMap* Owner, ZEPortalMapResourcePortal* Resource)
{	
	// Initialize Render Components
	if (VertexBuffer == NULL)
	{
		RenderCommands.Clear();
		VertexBuffer = ZEStaticVertexBuffer::CreateInstance();
		if (!VertexBuffer->Create(Resource->Polygons.GetCount() * 3 * sizeof(ZEMapVertex)))
			return false;

		ZEArray<bool> Processed;
		Processed.SetCount(Resource->Polygons.GetCount());
		Processed.Fill(false);

		ZESize VertexIndex = 0;
		ZEMapVertex* Buffer = (ZEMapVertex*)VertexBuffer->Lock();	
		for (ZESize N = 0; N < Resource->Polygons.GetCount(); N++)
		{
			if (!Processed[N])
			{
				ZEMaterial* Material = Resource->Polygons[N].Material;
				ZERenderCommand* RenderCommand = RenderCommands.Add();

				RenderCommand->SetZero();
				RenderCommand->Priority = 2;
				RenderCommand->Order = 1;
				RenderCommand->Flags = ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING;
				RenderCommand->Material = Material;
				RenderCommand->PrimitiveType = ZE_ROPT_TRIANGLE;
				RenderCommand->VertexBufferOffset = VertexIndex;
				RenderCommand->VertexBuffer = VertexBuffer;
				RenderCommand->VertexDeclaration = ZEMapVertex::GetVertexDeclaration();
				ZEMatrix4x4::CreateIdentity(RenderCommand->WorldMatrix);

				RenderCommand->PrimitiveCount = 0;
				for (ZESize I = N; I < Resource->Polygons.GetCount(); I++)
				{
					if (Resource->Polygons[I].Material != Material)
						continue;

					memcpy(Buffer + VertexIndex, Resource->Polygons[I].Vertices, sizeof(ZEMapVertex) * 3);
					VertexIndex += 3;
					RenderCommand->PrimitiveCount++;
					Processed[I] = true;
				}

			}
		}
		VertexBuffer->Unlock();
	}

	ZEArray<ZEVector3> PhysicalVertices;
	ZEArray<ZEPhysicalTriangle> PhysicalTriangles;

	PhysicalVertices.SetCount(Resource->Polygons.GetCount() * 3);
	PhysicalTriangles.SetCount(Resource->Polygons.GetCount());

	for (ZESize I = 0; I < PhysicalTriangles.GetCount(); I++)
	{
		PhysicalVertices[3 * I] = Resource->Polygons[I].Vertices[0].Position;
		PhysicalVertices[3 * I + 1] = Resource->Polygons[I].Vertices[1].Position;
		PhysicalVertices[3 * I + 2] = Resource->Polygons[I].Vertices[2].Position;
	}
	for (ZESize I = 0; I < PhysicalTriangles.GetCount(); I++)
	{
		PhysicalTriangles[I].MaterialIndex = 0;
		PhysicalTriangles[I].Indices[0] =  3 * (ZEUInt)I;
		PhysicalTriangles[I].Indices[1] =  3 * (ZEUInt)I + 1;
		PhysicalTriangles[I].Indices[2] =  3 * (ZEUInt)I + 2;
	}

	if (false)//(PhysicalMesh == NULL && Resource->HasPhysicalMesh)
	{
		PhysicalMesh = ZEPhysicalMesh::CreateInstance();
		PhysicalMesh->SetData(PhysicalVertices.GetConstCArray(), 
							  (ZEUInt)PhysicalVertices.GetCount(),
							  PhysicalTriangles.GetConstCArray(), 
							  (ZEUInt)PhysicalTriangles.GetCount(),
							  NULL, 0);
		PhysicalMesh->Initialize();
		zeScene->GetPhysicalWorld()->AddPhysicalObject(PhysicalMesh);
	}

	this->Resource = Resource;

	this->Owner = Owner;

	return true;
}

void ZEPortalMapPortal::Deinitialize()
{
	Owner = NULL;
	Resource = NULL;
	RenderCommands.Clear();
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

ZEPortalMapPortal* ZEPortalMapPortal::CreateInstance()
{
	return new ZEPortalMapPortal();
}
