//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInteriorRoom.cpp
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
#include "ZEInteriorRoom.h"
#include "ZEInteriorResource.h"
#include "ZEPhysics/ZEPhysicalMesh.h"
#include "ZEGraphics/ZELight.h"
#include "ZEGraphics/ZEVertexBuffer.h"
#include "ZEGraphics/ZERenderer.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGame/ZEScene.h"
#include "ZEPhysics/ZEPhysicalWorld.h"
#include "ZEMap/ZEInterior/ZEInterior.h"
#include "ZEMath/ZEViewVolume.h"
#include "ZEGraphics/ZESimpleMaterial.h"

void ZEInteriorRoom::DebugDraw(ZERenderer* Renderer)
{
	if (DebugDrawComponents.Material == NULL)
	{
		DebugDrawComponents.Material = ZESimpleMaterial::CreateInstance();

		DebugDrawComponents.BoxRenderCommand.SetZero();
		DebugDrawComponents.BoxRenderCommand.Material = DebugDrawComponents.Material;
		DebugDrawComponents.BoxRenderCommand.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_NO_Z_WRITE;
		DebugDrawComponents.BoxRenderCommand.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
		DebugDrawComponents.BoxRenderCommand.VertexBuffer = &DebugDrawComponents.BoxCanvas;
		DebugDrawComponents.BoxRenderCommand.PrimitiveType = ZE_ROPT_LINE;
	}

	DebugDrawComponents.BoxCanvas.Clean();
	DebugDrawComponents.BoxCanvas.SetColor(ZEVector4(0.7f, 0.5f, 0.0f, 1.0f));

	ZEAABBox BoundingBox = GetBoundingBox();
	DebugDrawComponents.BoxCanvas.SetRotation(ZEQuaternion::Identity);
	DebugDrawComponents.BoxCanvas.SetTranslation(ZEVector3::Zero);
	DebugDrawComponents.BoxCanvas.AddWireframeBox((BoundingBox.Max.x - BoundingBox.Min.x), (BoundingBox.Max.y - BoundingBox.Min.y), (BoundingBox.Max.z - BoundingBox.Min.z));
	ZEMatrix4x4 LocalMatrix;
	ZEMatrix4x4::CreateOrientation(LocalMatrix, Position, Rotation, Scale);
	DebugDrawComponents.BoxRenderCommand.WorldMatrix = Owner->GetWorldTransform() * LocalMatrix;
	DebugDrawComponents.BoxRenderCommand.PrimitiveCount = DebugDrawComponents.BoxCanvas.Vertices.GetCount() / 2;
	DebugDrawComponents.BoxRenderCommand.Priority = 4;
	Renderer->AddToRenderList(&DebugDrawComponents.BoxRenderCommand);
}

ZEInterior* ZEInteriorRoom::GetOwner()
{
	return Owner;
}

const char* ZEInteriorRoom::GetName()
{
	if (Resource != NULL)
		return Resource->Name;
	else
		return "";
}

const ZEArray<ZEInteriorDoor*>& ZEInteriorRoom::GetDoors()
{
	return Doors;
}

const ZEAABBox& ZEInteriorRoom::GetBoundingBox()
{
	return BoundingBox;
}

const ZEAABBox& ZEInteriorRoom::GetWorldBoundingBox()
{
	if (TransformChanged)
	{
		ZEMatrix4x4 Transform;
		ZEMatrix4x4::CreateOrientation(Transform, Position, Rotation, Scale);

		ZEMatrix4x4 WorldTransform;
		ZEMatrix4x4::Multiply(WorldTransform, Owner->GetWorldTransform(), Transform);

		ZEAABBox::Transform(WorldBoundingBox, BoundingBox, WorldTransform);
		TransformChanged = false;
	}

	return WorldBoundingBox;
	
}

void ZEInteriorRoom::SetPosition(const ZEVector3& NewPosition)
{
	Position = NewPosition;
	TransformChanged = true;
	PhysicalMesh->SetPosition(Owner->GetWorldPosition() + Position);
}

const ZEVector3& ZEInteriorRoom::GetPosition() const
{
	return Position;
}

void ZEInteriorRoom::SetRotation(const ZEQuaternion& NewRotation)
{
	Rotation = NewRotation;
	TransformChanged = true;
	PhysicalMesh->SetRotation(Owner->GetWorldRotation() * Rotation);
}

const ZEQuaternion& ZEInteriorRoom::GetRotation() const
{
	return Rotation;
}

void ZEInteriorRoom::SetScale(const ZEVector3& NewScale)
{
	Scale = NewScale;
	TransformChanged = true;
	PhysicalMesh->SetScale(Owner->GetWorldScale() * Scale);
}

const ZEVector3& ZEInteriorRoom::GetScale() const
{
	return Scale;
}

ZEPhysicalMesh* ZEInteriorRoom::GetPhysicalMesh()
{
	return PhysicalMesh;
}

size_t ZEInteriorRoom::GetPolygonCount()
{
	return Resource->Polygons.GetCount();
}

void ZEInteriorRoom::SetPersistentDraw(bool Enabled)
{
	IsPersistentDraw = Enabled;
}

void ZEInteriorRoom::Draw(ZEDrawParameters* DrawParameters)
{
	IsDrawn = true;

	for(ZESize I = 0; I < RenderCommands.GetCount(); I++)
	{
		ZEMatrix4x4 LocalTransform;
		ZEMatrix4x4::CreateOrientation(LocalTransform, Position, Rotation, Scale);
		RenderCommands[I].WorldMatrix = Owner->GetWorldTransform() * LocalTransform;
		RenderCommands[I].Lights.Clear();
		RenderCommands[I].Lights.MassAdd(DrawParameters->Lights.GetConstCArray(), DrawParameters->Lights.GetCount());

		DrawParameters->Renderer->AddToRenderList(&RenderCommands[I]);
	}
}

bool ZEInteriorRoom::Initialize(ZEInterior* Owner, ZEInteriorRoomResource* Resource)
{	

	this->Owner = Owner;
	this->Resource = Resource;
	this->BoundingBox = Resource->BoundingBox;
	this->TransformChanged = true;
	this->Position = Resource->Position;
	this->Rotation = Resource->Rotation;
	this->Scale = Resource->Scale;

	ZEMatrix4x4 LocalTransform;
	ZEMatrix4x4::CreateOrientation(LocalTransform, Position, Rotation, Scale);
	ZEMatrix4x4 WorldTransform = Owner->GetWorldTransform() * LocalTransform;
	

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
				RenderCommand->WorldMatrix = WorldTransform;

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

	if (PhysicalMesh == NULL && Resource->HasPhysicalMesh)
	{
		PhysicalMesh = ZEPhysicalMesh::CreateInstance();
		PhysicalMesh->SetData(PhysicalVertices.GetConstCArray(), 
							  (ZEUInt)PhysicalVertices.GetCount(),
							  PhysicalTriangles.GetConstCArray(), 
							  (ZEUInt)PhysicalTriangles.GetCount(),
							  NULL, 0);

		PhysicalMesh->SetPosition(Owner->GetWorldPosition() + Position);
		PhysicalMesh->SetRotation(Owner->GetWorldRotation() * Rotation);
		PhysicalMesh->SetScale(Owner->GetWorldScale() * Scale);
		PhysicalMesh->SetEnabled(Resource->PhysicalMesh.PhysicalMeshEnabled);
		PhysicalMesh->Initialize();
		zeScene->GetPhysicalWorld()->AddPhysicalObject(PhysicalMesh);
	}

	return true;
}

void ZEInteriorRoom::Deinitialize()
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

	if (DebugDrawComponents.Material != NULL)
	{
		DebugDrawComponents.Material->Release();
		DebugDrawComponents.Material = NULL;
	}
}

ZEInteriorRoom::ZEInteriorRoom()
{
	Owner = NULL;
	Resource = NULL;
	PhysicalMesh = NULL;
	VertexBuffer = NULL;
	DebugDrawComponents.Material = NULL;

	CullPass = false;
	IsDrawn = false;
	IsPersistentDraw = false;

	TransformChanged = false;
	Position = ZEVector3::Zero;
	Rotation = ZEQuaternion::Identity;
	Scale = ZEVector3::One;
}

ZEInteriorRoom::~ZEInteriorRoom()
{
	Deinitialize();
}

ZEInteriorRoom* ZEInteriorRoom::CreateInstance()
{
	return new ZEInteriorRoom();
}
