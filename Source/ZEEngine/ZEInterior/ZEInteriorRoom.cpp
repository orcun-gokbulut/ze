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

#include "ZEInteriorRoom.h"

#include "ZEMath/ZEViewVolume.h"
#include "ZEMath/ZEMath.h"

#include "ZEInterior.h"
#include "ZEInteriorResource.h"
#include "ZECore/ZECore.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEGRVertexBuffer.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZERenderer/ZELight.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNSimpleMaterial.h"
#include "ZERenderer/ZERNShaderSlots.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZEPhysics/ZEPhysicalMesh.h"
#include "ZEPhysics/ZEPhysicalWorld.h"


void ZEInteriorRoom::RayCastOctreePoligons(const ZEOctree<ZESize>& Octree, ZERayCastReport& Report, ZERayCastHelper& Helper)
{
	for (ZESize I = 0; I < Octree.GetItems().GetCount(); I++)
	{
		ZESize CurrentPoligonIndex = Octree.GetItem(I);
		const ZEInteriorPolygon* Polygon = &Resource->Polygons[CurrentPoligonIndex];
		
		Helper.RayCastPolygon(Polygon->Vertices[0].Position, Polygon->Vertices[1].Position, Polygon->Vertices[2].Position);
		if (Report.CheckDone())
			break;
	}
}

void ZEInteriorRoom::RayCastOctree(const ZEOctree<ZESize>& Octree, ZERayCastReport& Report, ZERayCastHelper& Helper)
{
	float RayT;
	if (!ZEAABBox::IntersectionTest(Resource->Octree.GetBoundingBox(), Helper.GetLocalRay(), RayT))
		return;

	RayCastOctreePoligons(Octree, Report, Helper);
	if (Report.CheckDone())
		return;

	for (ZESize I = 0; I < 8; I++)
	{
		const ZEOctree<ZESize>* SubTree = Octree.GetNodes()[I];
		if (SubTree != NULL)
			RayCastOctree(*SubTree, Report, Helper);

		if (Report.CheckDone())
			return;
	}
}

void ZEInteriorRoom::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	ZERayCastHelper Helper;
	Helper.SetReport(&Report);
	Helper.SetWorldTransform(&GetWorldTransform());
	Helper.SetInvWorldTransform(&GetInvWorldTransform());
	Helper.SetObject(Owner);
	Helper.SetSubObject(this);

	if (!Helper.RayCastBoundingBox(GetWorldBoundingBox(), GetBoundingBox()))
		return;

	if (Parameters.Components.GetFlags(ZE_RCRE_POLYGONS))
	{
		if (Resource->HasOctree)
		{
			RayCastOctree(Resource->Octree, Report, Helper);
		}
		else
		{
			Helper.RayCastMesh(Resource->Polygons.GetConstCArray(), Resource->Polygons.GetCount(), sizeof(ZEInteriorPolygon),
				offsetof(ZEInteriorPolygon, Vertices[0].Position), offsetof(ZEInteriorPolygon, Vertices[1].Position), offsetof(ZEInteriorPolygon, Vertices[2].Position));
		}
	}
}

void ZEInteriorRoom::ParentTransformChanged()
{
	DirtyFlags.RaiseFlags(ZE_IRDF_ALL);

	if (PhysicalMesh != NULL)
	{
		PhysicalMesh->SetPosition(Owner->GetWorldPosition() + Position);
		PhysicalMesh->SetRotation(Owner->GetWorldRotation() * Rotation);
		PhysicalMesh->SetScale(Owner->GetWorldScale() * Scale);
	}
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

const ZEAABBox& ZEInteriorRoom::GetBoundingBox() const
{
	return BoundingBox;
}

const ZEAABBox& ZEInteriorRoom::GetWorldBoundingBox() const
{
	if (DirtyFlags.GetFlags(ZE_IRDF_WORLD_BOUNDING_BOX))
	{
		ZEAABBox::Transform(WorldBoundingBox, BoundingBox, GetWorldTransform());
		DirtyFlags.UnraiseFlags(ZE_IRDF_WORLD_BOUNDING_BOX);
	}

	return WorldBoundingBox;
	
}

const ZEMatrix4x4& ZEInteriorRoom::GetTransform() const
{
	if (DirtyFlags.GetFlags(ZE_IRDF_TRANSFORM))
	{
		ZEMatrix4x4::CreateOrientation(LocalTransform, Position, Rotation, Scale);
		DirtyFlags.UnraiseFlags(ZE_IRDF_TRANSFORM);
	}

	return LocalTransform;
}

const ZEMatrix4x4& ZEInteriorRoom::GetWorldTransform() const
{
	if (DirtyFlags.GetFlags(ZE_IRDF_WORLD_TRANSFORM))
	{
		ZEMatrix4x4::Multiply(WorldTransform, Owner->GetWorldTransform(), GetTransform());
		DirtyFlags.UnraiseFlags(ZE_IRDF_WORLD_TRANSFORM);
	}

	return WorldTransform;
}

const ZEMatrix4x4& ZEInteriorRoom::GetInvWorldTransform() const
{
	if (DirtyFlags.GetFlags(ZE_IRDF_INV_WORLD_TRANSFORM))
	{
		ZEMatrix4x4::Inverse(InvWorldTransform, GetWorldTransform());
		DirtyFlags.UnraiseFlags(ZE_IRDF_INV_WORLD_TRANSFORM);
	}

	return InvWorldTransform;
}

void ZEInteriorRoom::SetPosition(const ZEVector3& NewPosition)
{
	Position = NewPosition;
	DirtyFlags.RaiseFlags(ZE_IRDF_ALL);

	if (PhysicalMesh != NULL)
		PhysicalMesh->SetPosition(Owner->GetWorldPosition() + Position);
}

const ZEVector3& ZEInteriorRoom::GetPosition() const
{
	return Position;
}

void ZEInteriorRoom::SetRotation(const ZEQuaternion& NewRotation)
{
	Rotation = NewRotation;
	DirtyFlags.RaiseFlags(ZE_IRDF_ALL);

	if (PhysicalMesh != NULL)
		PhysicalMesh->SetRotation(Owner->GetWorldRotation() * Rotation);
}

const ZEQuaternion& ZEInteriorRoom::GetRotation() const
{
	return Rotation;
}

void ZEInteriorRoom::SetScale(const ZEVector3& NewScale)
{
	Scale = NewScale;
	DirtyFlags.RaiseFlags(ZE_IRDF_ALL);

	if (PhysicalMesh != NULL)
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

void ZEInteriorRoom::PreRender(const ZERNPreRenderParameters* Parameters)
{
	IsDrawn = true;
	
	ZESize RenderCommandCount = RenderCommands.GetCount();
	for(ZESize I = 0; I < RenderCommandCount; I++)
		Parameters->Renderer->AddCommand(&RenderCommands[I]);
}

void ZEInteriorRoom::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	ZEGRContext* Context = Parameters->Context;
	ConstantBuffer->SetData(&GetWorldTransform());
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBuffer);
	Context->SetVertexBuffers(0, 1, VertexBuffer.GetPointerToPointer());

	ZEExtraRenderParameters* ExtraParameters = static_cast<ZEExtraRenderParameters*>(Command->ExtraParameters);
	ExtraParameters->Material->SetupMaterial(Context, Parameters->Stage);

	Context->Draw(ExtraParameters->VertexCount, ExtraParameters->VertexOffset);

	ExtraParameters->Material->CleanupMaterial(Context, Parameters->Stage);

	IsDrawn = false;
}

bool ZEInteriorRoom::Initialize(ZEInterior* Owner, ZEInteriorResourceRoom* Resource)
{	
	this->Owner = Owner;
	this->Resource = Resource;
	this->BoundingBox = Resource->BoundingBox;
	this->Position = Resource->Position;
	this->Rotation = Resource->Rotation;
	this->Scale = Resource->Scale;
	
	VertexBuffer = ZEGRVertexBuffer::Create(Resource->Polygons.GetCount() * 3, sizeof(ZEInteriorVertex));
	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(ZEMatrix4x4));

	ZEArray<bool> Processed;
	Processed.SetCount(Resource->Polygons.GetCount());
	Processed.Fill(false);

	ZESize CurrentVertexIndex = 0;
	ZESize StartVertexIndex = 0;
	ZEInteriorVertex* Buffer;
	VertexBuffer->Lock((void**)&Buffer);	
		ZESize PolygonCount = Resource->Polygons.GetCount();
		for (ZESize N = 0; N < PolygonCount; N++)
		{
			StartVertexIndex = CurrentVertexIndex;

			if (!Processed[N])
			{
				ZERNMaterial* Material = Resource->Polygons[N].Material;

				for (ZESize I = N; I < PolygonCount; I++)
				{
					if (Resource->Polygons[I].Material != Material)
						continue;

					memcpy(Buffer + CurrentVertexIndex, Resource->Polygons[I].Vertices, sizeof(ZEInteriorVertex) * 3);
					CurrentVertexIndex += 3;
					Processed[I] = true;
				}

				ZEExtraRenderParameters* ExtraParameters = new ZEExtraRenderParameters();
				ExtraParameters->Material = Material;
				ExtraParameters->Room = this;
				ExtraParameters->VertexOffset = StartVertexIndex;
				ExtraParameters->VertexCount = CurrentVertexIndex - StartVertexIndex;

				ZERNCommand* RenderCommand = RenderCommands.Add();
				RenderCommand->Entity = Owner;
				RenderCommand->Order = 1;
				RenderCommand->Priority = 2;
				RenderCommand->StageMask = Material->GetStageMask();
				RenderCommand->ExtraParameters = ExtraParameters;
			}
		}
	VertexBuffer->Unlock();

	if (Resource->HasPhysicalMesh)
	{
		ZEArray<ZEPhysicalTriangle> PhysicalTriangles;
		PhysicalTriangles.SetCount(Resource->PhysicalMesh.Polygons.GetCount());

		for (ZESize I = 0; I < PhysicalTriangles.GetCount(); I++)
		{
			PhysicalTriangles[I].MaterialIndex = 0;
			PhysicalTriangles[I].Indices[0] =  Resource->PhysicalMesh.Polygons[I].Indices[0];
			PhysicalTriangles[I].Indices[1] =  Resource->PhysicalMesh.Polygons[I].Indices[1];
			PhysicalTriangles[I].Indices[2] =  Resource->PhysicalMesh.Polygons[I].Indices[2];
		}

		if (PhysicalMesh == NULL)
		{
			PhysicalMesh = ZEPhysicalMesh::CreateInstance();
			PhysicalMesh->SetData(Resource->PhysicalMesh.Vertices.GetConstCArray(), 
				(ZEUInt)Resource->PhysicalMesh.Vertices.GetCount(),
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
	}

	return true;
}

void ZEInteriorRoom::Deinitialize()
{
	Owner = NULL;
	Resource = NULL;
	RenderCommands.Clear();

	VertexBuffer.Release();
	ConstantBuffer.Release();

	ExtraRenderParameters.Clear();

	if (PhysicalMesh != NULL)
	{
		PhysicalMesh->Destroy();
		PhysicalMesh = NULL;
	}
}

ZEInteriorRoom::ZEInteriorRoom()
{
	Owner = NULL;
	Resource = NULL;
	PhysicalMesh = NULL;

	CullPass = false;
	IsDrawn = false;
	IsPersistentDraw = false;

	DirtyFlags.RaiseFlags(ZE_IRDF_ALL);
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
