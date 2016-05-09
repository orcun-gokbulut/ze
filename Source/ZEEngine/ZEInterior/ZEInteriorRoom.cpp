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

#include "ZECore/ZECore.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEViewVolume.h"
#include "ZEInterior.h"
#include "ZEInteriorResource.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEGRVertexBuffer.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEPhysics/ZEPhysicalMesh.h"
#include "ZEPhysics/ZEPhysicalWorld.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNShaderSlots.h"
#include "ZERenderer/ZERNCuller.h"
#include "ZERenderer/ZERNMaterial.h"
#include "ZERenderer/ZERNRenderParameters.h"

#define ZE_IRDF_TRANSFORM						1
#define ZE_IRDF_WORLD_TRANSFORM					2
#define ZE_IRDF_INV_WORLD_TRANSFORM				4
#define ZE_IRDF_WORLD_BOUNDING_BOX				8
#define ZE_IRDF_CONSTANT_BUFFER					16

struct ZEInteriorTransformConstants
{
	ZEMatrix4x4	WorldTransform;
	ZEMatrix4x4	WorldTransformInverseTranspose;
	ZEMatrix4x4	PreSkinTransform;
	ZEVector4	ClippingPlane0;
	ZEVector4	ClippingPlane1;
	ZEVector4	ClippingPlane2;
	ZEVector4	ClippingPlane3;
};

void ZEInteriorRoom::UpdateConstantBuffer()
{
	if (!DirtyFlags.GetFlags(ZE_IRDF_CONSTANT_BUFFER))
		return;

	ZEInteriorTransformConstants Constants;

	Constants.WorldTransform = GetWorldTransform();
	Constants.WorldTransformInverseTranspose = GetInvWorldTransform().Transpose();

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZE_IRDF_CONSTANT_BUFFER);
}

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

void ZEInteriorRoom::ParentTransformChanged()
{
	DirtyFlags.RaiseAll();

	if (PhysicalMesh != NULL)
	{
		PhysicalMesh->SetPosition(Owner->GetWorldPosition() + Position);
		PhysicalMesh->SetRotation(Owner->GetWorldRotation() * Rotation);
		PhysicalMesh->SetScale(Owner->GetWorldScale() * Scale);
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

	DirtyFlags.RaiseAll();

	Position = ZEVector3::Zero;
	Rotation = ZEQuaternion::Identity;
	Scale = ZEVector3::One;
}

ZEInteriorRoom::~ZEInteriorRoom()
{
	Deinitialize();
}

ZEInterior* ZEInteriorRoom::GetOwner()
{
	return Owner;
}

const ZEString& ZEInteriorRoom::GetName()
{
	if (Resource != NULL)
		return Resource->Name;
	else
		return ZEString::Empty;
}

const ZEArray<ZEInteriorDoor*>& ZEInteriorRoom::GetDoors()
{
	return Doors;
}

ZEPhysicalMesh* ZEInteriorRoom::GetPhysicalMesh()
{
	return PhysicalMesh;
}

size_t ZEInteriorRoom::GetPolygonCount()
{
	return Resource->Polygons.GetCount();
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
	DirtyFlags.RaiseAll();

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
	DirtyFlags.RaiseAll();

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
	DirtyFlags.RaiseAll();

	if (PhysicalMesh != NULL)
		PhysicalMesh->SetScale(Owner->GetWorldScale() * Scale);
}

const ZEVector3& ZEInteriorRoom::GetScale() const
{
	return Scale;
}

void ZEInteriorRoom::SetPersistentDraw(bool Enabled)
{
	IsPersistentDraw = Enabled;
}

bool ZEInteriorRoom::Initialize(ZEInterior* Owner, ZEInteriorResourceRoom* Resource)
{	
	this->Owner = Owner;
	this->Resource = Resource;
	this->BoundingBox = Resource->BoundingBox;
	this->Position = Resource->Position;
	this->Rotation = Resource->Rotation;
	this->Scale = Resource->Scale;

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(ZEInteriorTransformConstants));
	
	ZESize PolygonCount = Resource->Polygons.GetCount();

	ZEArray<bool> Processed;
	Processed.SetCount(PolygonCount);
	Processed.Fill(false);

	ZEArray<ZEInteriorVertex> Vertices;
	Vertices.SetCount(PolygonCount * 3);

	ZESize CurrentVertexIndex = 0;
	ZESize StartVertexIndex = 0;
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

				memcpy(&Vertices[CurrentVertexIndex], Resource->Polygons[I].Vertices, sizeof(ZEInteriorVertex) * 3);

				CurrentVertexIndex += 3;
				Processed[I] = true;
			}

			ZEExtraRenderParameters ExtraParameters;
			ExtraParameters.Material = Material;
			ExtraParameters.Room = this;
			ExtraParameters.VertexOffset = StartVertexIndex;
			ExtraParameters.VertexCount = CurrentVertexIndex - StartVertexIndex;

			ExtraRenderParameters.AddByRef(ExtraParameters);
		}
	}

	VertexBuffer = ZEGRVertexBuffer::Create(Vertices.GetCount(), sizeof(ZEInteriorVertex), ZEGR_RU_GPU_READ_ONLY, Vertices.GetCArray());

	ZESize ExtraParameterCount = ExtraRenderParameters.GetCount();
	RenderCommands.SetCount(ExtraParameterCount);
	for (ZESize I = 0; I < ExtraParameterCount; I++)
	{
		RenderCommands[I].Entity = Owner;
		RenderCommands[I].Order = 0;
		RenderCommands[I].Priority = 0;
		RenderCommands[I].StageMask = ExtraRenderParameters[I].Material->GetStageMask();
		RenderCommands[I].ExtraParameters = &ExtraRenderParameters[I];
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
	if (PhysicalMesh != NULL)
	{
		PhysicalMesh->Destroy();
		PhysicalMesh = NULL;
	}

	Doors.Clear();
	RenderCommands.Clear();
	ExtraRenderParameters.Clear();

	VertexBuffer.Release();
	ConstantBuffer.Release();

	DirtyFlags.RaiseAll();
}

void ZEInteriorRoom::PreRender(const ZERNCullParameters* CullParameters)
{
	IsDrawn = true;
	
	ZESize RenderCommandCount = RenderCommands.GetCount();
	for(ZESize I = 0; I < RenderCommandCount; I++)
		CullParameters->Renderer->AddCommand(&RenderCommands[I]);
}

void ZEInteriorRoom::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	ZEGRContext* Context = Parameters->Context;
	ZERNStage* Stage = Parameters->Stage;

	ZEExtraRenderParameters* ExtraParameters = static_cast<ZEExtraRenderParameters*>(Command->ExtraParameters);
	if (!ExtraParameters->Material->SetupMaterial(Context, Stage))
		return;

	UpdateConstantBuffer();

	Context->SetConstantBuffers(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, 1, ConstantBuffer.GetPointerToPointer());
	Context->SetVertexBuffers(0, 1, VertexBuffer.GetPointerToPointer());

	Context->Draw(ExtraParameters->VertexCount, ExtraParameters->VertexOffset);

	ExtraParameters->Material->CleanupMaterial(Context, Stage);

	IsDrawn = false;
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

ZEInteriorRoom* ZEInteriorRoom::CreateInstance()
{
	return new ZEInteriorRoom();
}
