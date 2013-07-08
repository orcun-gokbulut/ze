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
#include "ZEGame/ZEScene.h"
#include "ZERenderer/ZELight.h"
#include "ZEInteriorResource.h"
#include "ZEMath/ZEViewVolume.h"
#include "ZEInterior/ZEInterior.h"
#include "ZERenderer/ZERenderer.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEPhysics/ZEPhysicalMesh.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEVertexBuffer.h"
#include "ZEPhysics/ZEPhysicalWorld.h"
#include "ZERenderer/ZEMaterialSimple.h"


bool ZEInteriorRoom::RayCastPoligons(const ZERay& Ray, float& MinT, ZESize& PoligonIndex)
{
	bool HaveIntersection = false;
	for (ZESize I = 0; I < Resource->Polygons.GetCount(); I++)
	{
		const ZEInteriorPolygon* Polygon = &Resource->Polygons[I];
		ZETriangle Triangle(Polygon->Vertices[0].Position, Polygon->Vertices[1].Position, Polygon->Vertices[2].Position);

		float RayT;
		if (ZETriangle::IntersectionTest(Triangle, Ray, RayT))
		{
			if (RayT < MinT)
			{
				MinT = RayT;
				PoligonIndex = I;
				HaveIntersection = true;
			}
		}
	}

	return HaveIntersection;
}

bool ZEInteriorRoom::RayCastOctreePoligons(const ZEOctree<ZESize>& Octree, const ZERay& Ray, float& MinT, ZESize& PoligonIndex)
{
	bool HaveIntersection = false;
	for (ZESize I = 0; I < Octree.GetItems().GetCount(); I++)
	{
		ZESize CurrentPoligonIndex = Octree.GetItem(I);
		const ZEInteriorPolygon* Polygon = &Resource->Polygons[CurrentPoligonIndex];

		ZETriangle Triangle(Polygon->Vertices[0].Position, Polygon->Vertices[1].Position, Polygon->Vertices[2].Position);

		float RayT;
		if (ZETriangle::IntersectionTest(Triangle, Ray, RayT))
		{
			if (RayT < MinT)
			{
				MinT = RayT;
				PoligonIndex = CurrentPoligonIndex;
				HaveIntersection = true;
			}
		}
	}

	return HaveIntersection;
}

bool ZEInteriorRoom::RayCastOctree(const ZEOctree<ZESize>& Octree, const ZERay& Ray, float& MinT, ZESize& PoligonIndex)
{
	float RayT;
	if (!ZEAABBox::IntersectionTest(Resource->Octree.GetBoundingBox(), Ray, RayT))
		return false;

	bool HaveIntersection = 0;
	HaveIntersection |= RayCastOctreePoligons(Octree, Ray, MinT, PoligonIndex);

	for (ZESize I = 0; I < 8; I++)
	{
		const ZEOctree<ZESize>* SubTree = Octree.GetNodes()[I];
		if (SubTree != NULL)
			HaveIntersection |= RayCastOctree(*SubTree, Ray, MinT, PoligonIndex);
	}

	return HaveIntersection;
}

bool ZEInteriorRoom::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	if (!ZEAABBox::IntersectionTest(GetWorldBoundingBox(), Parameters.Ray))
		return false;

	ZERay LocalRay;
	ZEMatrix4x4::Transform(LocalRay.p, GetInvWorldTransform(), Parameters.Ray.p);
	ZEMatrix4x4::Transform3x3(LocalRay.v, GetInvWorldTransform(), Parameters.Ray.v);
	LocalRay.v.NormalizeSelf();

	float RayT;
	if (!ZEAABBox::IntersectionTest(GetBoundingBox(), LocalRay, RayT))
		return false;

	bool Result = false;
	float MinT = ZE_FLOAT_MAX;
	ZESize PoligonIndex;

	if (Resource->HasOctree)
		Result = RayCastOctree(Resource->Octree, LocalRay, MinT, PoligonIndex);
	else
		Result = RayCastPoligons(LocalRay, MinT, PoligonIndex);

	if (Result)
	{
		ZEVector3 WorldPosition;
		ZEMatrix4x4::Transform(WorldPosition, GetWorldTransform(), LocalRay.GetPointOn(MinT));
		
		float DistanceSquare = ZEVector3::DistanceSquare(Parameters.Ray.p, WorldPosition);
		if (Report.Distance * Report.Distance > DistanceSquare && DistanceSquare < Parameters.MaximumDistance * Parameters.MaximumDistance)
		{
			Report.Distance = ZEMath::Sqrt(DistanceSquare);
			Report.Position = WorldPosition;
			Report.SubComponent = this;
			Report.PoligonIndex = PoligonIndex;

			if (Parameters.Extras.GetFlags(ZE_RCRE_NORMAL) || Parameters.Extras.GetFlags(ZE_RCRE_BINORMAL))
			{
				ZEVector3 V0 = Resource->Polygons[Report.PoligonIndex].Vertices[0].Position;
				ZEVector3 V1 = Resource->Polygons[Report.PoligonIndex].Vertices[1].Position;
				ZEVector3 V2 = Resource->Polygons[Report.PoligonIndex].Vertices[2].Position;

				ZEVector3 Binormal = ZEVector3(V0, V1);
				ZEVector3 Tangent = ZEVector3(V0, V2);
				ZEVector3 Normal;
				ZEVector3::CrossProduct(Normal, Binormal, Tangent);

				if (Parameters.Extras.GetFlags(ZE_RCRE_NORMAL))
				{
					ZEMatrix4x4::Transform3x3(Report.Normal, GetWorldTransform(), Normal);
					Report.Normal.NormalizeSelf();
				}

				if (Parameters.Extras.GetFlags(ZE_RCRE_BINORMAL))
				{
					ZEMatrix4x4::Transform3x3(Report.Binormal, GetWorldTransform(), Binormal);
					Report.Binormal.NormalizeSelf();
				}
			}
			
			return true;
		}
	}

	return false;
}


void ZEInteriorRoom::OnTransformChanged()
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
#include "ZEGraphics/ZEConstantBuffer.h"
#include "ZERenderer/ZECamera.h"
void ZEInteriorRoom::Draw(ZEDrawParameters* DrawParameters)
{
	IsDrawn = true;

	for(ZESize I = 0; I < RenderCommands.GetCount(); I++)
	{
		ZERenderCommand* Command = &RenderCommands[I];

		ZETransformationBuffer* Buffer = NULL;
		Command->TransformationBuffer->Lock((void**)&Buffer);

		ZEMatrix4x4 LocalTransform;
		ZEMatrix4x4::CreateOrientation(LocalTransform, Position, Rotation, Scale);
		
		Buffer->WorldMatrix = Owner->GetWorldTransform() * LocalTransform;
		Buffer->ViewMatrix = zeScene->GetActiveCamera()->GetViewTransform();
		Buffer->ProjectionMatrix = zeScene->GetActiveCamera()->GetProjectionTransform();
		
		Command->TransformationBuffer->Unlock();

		DrawParameters->Renderer->AddRenderCommand(Command);
	}
}

bool ZEInteriorRoom::Initialize(ZEInterior* Owner, ZEInteriorResourceRoom* Resource)
{	

	this->Owner = Owner;
	this->Resource = Resource;
	this->BoundingBox = Resource->BoundingBox;
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
		VertexBuffer = ZEVertexBuffer::CreateInstance();

		ZESize PolygonCount = Resource->Polygons.GetCount();
		
		ZEArray<ZEInteriorVertex> VertexData;
		VertexData.SetCount(PolygonCount * 3);

		ZEArray<bool> Processed;
		Processed.SetCount(PolygonCount);
		Processed.Fill(false);

		ZESize VertexIndex = 0;
		for (ZESize N = 0; N < PolygonCount; N++)
		{
			if (!Processed[N])
			{
				ZEMaterial* Material = Resource->Polygons[N].Material;
				ZERenderCommandDefault* RenderCommand = RenderCommands.Add();

				RenderCommand->Priority = 2;
				RenderCommand->Order = 1;
				RenderCommand->Material = Material;
				RenderCommand->FirstVertex = (ZEUInt)VertexIndex;
				RenderCommand->VertexBuffers[0] = VertexBuffer;
				RenderCommand->PrimitiveType = ZE_PT_TRIANGLE_LIST;
				RenderCommand->VertexLayout = NULL;	// Auto layout generation.
				
				RenderCommand->PrimitiveCount = 0;
				for (ZESize I = N; I < PolygonCount; I++)
				{
					if (Resource->Polygons[I].Material != Material)
						continue;

					VertexData[VertexIndex + 0] = Resource->Polygons[I].Vertices[0];
					VertexData[VertexIndex + 1] = Resource->Polygons[I].Vertices[1];
					VertexData[VertexIndex + 2] = Resource->Polygons[I].Vertices[2];
					VertexIndex += 3;

					Processed[I] = true;
					RenderCommand->PrimitiveCount++;
				}
			}
		}

		bool Result = VertexBuffer->CreateStatic((ZEUInt)PolygonCount * 3, sizeof(ZEInteriorVertex), (void*)VertexData.GetCArray());
		if (!Result)
		{
			ZE_DESTROY(VertexBuffer);
			return false;
		}

		ZEVertexBufferElement Elements[] = 
		{
			{"POSITION",	0, ZE_VET_FLOAT3, 0,	ZE_VU_PER_VERTEX, 0},
			{"NORMAL",		0, ZE_VET_FLOAT3, 12,	ZE_VU_PER_VERTEX, 0},
			{"TANGENT",		0, ZE_VET_FLOAT3, 24,	ZE_VU_PER_VERTEX, 0},
			{"BINORMAL",	0, ZE_VET_FLOAT3, 36,	ZE_VU_PER_VERTEX, 0},
			{"TEXCOORD",	0, ZE_VET_FLOAT2, 48,	ZE_VU_PER_VERTEX, 0},
		};

		VertexBuffer->RegisterElements(Elements, 5);
	}

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

ZEInteriorRoom::ZEInteriorRoom()
{
	Owner = NULL;
	Resource = NULL;
	PhysicalMesh = NULL;
	VertexBuffer = NULL;

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
