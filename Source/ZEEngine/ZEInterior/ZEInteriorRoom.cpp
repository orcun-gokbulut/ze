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
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEPhysics/ZEPhysicalMesh.h"
#include "ZEPhysics/ZEPhysicalWorld.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNShaderSlots.h"
#include "ZERenderer/ZERNMaterial.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZEModel/ZEMDVertex.h"

#define ZE_IRDF_TRANSFORM						1
#define ZE_IRDF_WORLD_TRANSFORM					2
#define ZE_IRDF_INV_WORLD_TRANSFORM				4
#define ZE_IRDF_WORLD_BOUNDING_BOX				8
#define ZE_IRDF_CONSTANT_BUFFER					16

struct ZEInteriorTransformConstants
{
	ZEMatrix4x4	WorldTransform;
	ZEMatrix4x4	WorldTransformInverseTranspose;
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

void ZEInteriorRoom::RayCastOctreePoligons(const ZEOctree<ZESize>& Octree, ZERayCastReport& Report, ZERayCastHelper& Helper)
{
	for (ZESize I = 0; I < Octree.GetItems().GetCount(); I++)
	{
		ZESize CurrentPoligonIndex = Octree.GetItems()[0];
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
		const ZEOctree<ZESize>* SubTree = Octree.GetNode(I);
		if (SubTree != NULL)
			RayCastOctree(*SubTree, Report, Helper);

		if (Report.CheckDone())
			return;
	}
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

template<typename ZEVertexTypeV1, typename ZEVertexTypeV2>
static void VerifyVertexNormals(const ZEVertexTypeV1& InputVertex, const ZEVertexTypeV2& OutputVertex, ZESize Index)
{
	// Regenerated Normal
	ZEVector3 RegeneratedNormal;
	RegeneratedNormal.x = (float)OutputVertex.Normal.M[0] / (float)0x7FFF;
	RegeneratedNormal.y = (float)OutputVertex.Normal.M[1] / (float)0x7FFF;
	float NormalSign = (2.0f * (OutputVertex.Normal.M[0] & 0x0001) - 1.0f);
	RegeneratedNormal.z = ZEMath::Sqrt(ZEMath::Saturate(1.0f - RegeneratedNormal.x * RegeneratedNormal.x - RegeneratedNormal.y * RegeneratedNormal.y)) * NormalSign;
	RegeneratedNormal.NormalizeSelf();	
	if (!RegeneratedNormal.IsValid())
		zeWarning("Validation check failed. Regenerated normal is not valid. Index: %u.", Index);

	float ErrorNormal = (RegeneratedNormal - InputVertex.Normal).Abs().Max();
	if (ErrorNormal >= 0.02f)
		zeWarning("Validation check failed. Regenerated normal is not equals to original normal. Index: %u, Error: %f.", Index, ErrorNormal);

	// Regenerated Tangent
	ZEVector3 RegeneratedTangent;
	RegeneratedTangent.x = (float)OutputVertex.Tangent.M[0] / (float)0x7FFF;
	RegeneratedTangent.y = (float)OutputVertex.Tangent.M[1] / (float)0x7FFF;
	float TangentSign = (2.0f * (OutputVertex.Tangent.M[0] & 0x0001) - 1.0f);
	RegeneratedTangent.z = ZEMath::Sqrt(ZEMath::Saturate(1.0f - RegeneratedTangent.x * RegeneratedTangent.x - RegeneratedTangent.y * RegeneratedTangent.y)) * TangentSign;
	RegeneratedTangent.NormalizeSelf();
	if (!RegeneratedTangent.IsValid())
		zeWarning("Validation check failed. Regenerated tangent is not valid. Index: %u.", Index);

	float ErrorTangent = (RegeneratedTangent - InputVertex.Tangent).Abs().Max();
	if (ErrorTangent >= 0.02f)
		zeWarning("Validation check failed. Regenerated tangent is not equals to original tangent. Index: %u, Error: %f.", Index, ErrorTangent);


	// Calculated Binormal
	float BinormalDirection = (2.0f * (OutputVertex.Tangent.M[1] & 0x0001) - 1.0f);
	ZEVector3 CalculatedBinormal;
	ZEVector3::CrossProduct(CalculatedBinormal, InputVertex.Tangent, InputVertex.Normal);
	CalculatedBinormal *= BinormalDirection;
	CalculatedBinormal.NormalizeSelf();
	if (!CalculatedBinormal.IsValid())
		zeWarning("Validation check failed. Calculated binormal is not valid. Index: %u.", Index);

	float ErrorCalculatedBinormal = (InputVertex.Binormal - CalculatedBinormal).Abs().Sum();
	if (ErrorCalculatedBinormal >= 0.2f)
		zeWarning("Validation check failed. Calculated binormal is not equals to original binormal. Index: %u, Error: %f.", Index, ErrorCalculatedBinormal);


	// Regenerated Binormal
	ZEVector3 RegeneratedBinormal;
	ZEVector3::CrossProduct(RegeneratedBinormal, RegeneratedTangent, RegeneratedNormal);
	RegeneratedBinormal = RegeneratedBinormal.Normalize() * BinormalDirection;
	if (!RegeneratedBinormal.IsValid())
		zeWarning("Validation check failed. Regenerated binormal is not valid. Index: %u.", Index);

	float ErrorBinormal = (RegeneratedBinormal - CalculatedBinormal).Abs().Max();
	if (ErrorBinormal >= 0.05f)
		zeWarning("Validation check failed. Regenerated binormal is not equals to calculated binormal. Index: %u, Error: %f.", Index, ErrorNormal);
}

template<typename ZEVertexTypeV1, typename ZEVertexTypeV2>
static void ConvertVertexNormals(const ZEVertexTypeV1& InputVertex, ZEVertexTypeV2& OutputVertex)
{
	//zeDebugCheck(InputVertex.Normal == InputVertex.Tangent, "Same normal-tangent");

	const ZEVector3& Normal = InputVertex.Normal;
	const ZEVector3& Tangent = InputVertex.Tangent;
	const ZEVector3& Binormal = InputVertex.Binormal;

	OutputVertex.Normal.M[0] = (ZEInt16)(Normal.x * (float)0x7FFF);
	OutputVertex.Normal.M[1] = (ZEInt16)(Normal.y * (float)0x7FFF);
	if (Normal.z < 0.0f)
		OutputVertex.Normal.M[0] &= 0xFFFE;
	else
		OutputVertex.Normal.M[0] |= 0x0001;

	OutputVertex.Tangent.M[0] = (ZEInt16)(Tangent.x * (float)0x7FFF);
	OutputVertex.Tangent.M[1] = (ZEInt16)(Tangent.y * (float)0x7FFF);
	if (Tangent.z < 0.0f)
		OutputVertex.Tangent.M[0] &= 0xFFFE;
	else
		OutputVertex.Tangent.M[0] |= 0x0001;

	ZEVector3 CalculatedBinormal;
	ZEVector3::CrossProduct(CalculatedBinormal, Tangent, Normal);
	CalculatedBinormal.NormalizeSelf();
	//if (!CalculatedBinormal.IsValid())
		//zeError("Calculated normal is not valid.");

	if (ZEVector3::DotProduct(CalculatedBinormal, Binormal) < 0.0f) // Mirrored
	{
		CalculatedBinormal *= -1.0f;
		OutputVertex.Tangent.M[1] &= 0xFFFE;
	}
	else
	{
		OutputVertex.Tangent.M[1] |= 0x0001;
	}

	return;
}

bool ZEInteriorRoom::Initialize(ZEInterior* Owner, ZEInteriorResourceRoom* Resource)
{	
	this->Owner = Owner;
	this->Resource = Resource;
	this->BoundingBox = Resource->BoundingBox;
	this->Position = Resource->Position;
	this->Rotation = Resource->Rotation;
	this->Scale = Resource->Scale;

	ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(ZEInteriorTransformConstants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);
	
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
			const ZERNMaterial* Material = Resource->Polygons[N].Material;
			for (ZESize I = N; I < PolygonCount; I++)
			{
				if (Resource->Polygons[I].Material != Material)
					continue;

				Vertices[CurrentVertexIndex].Position = Resource->Polygons[I].Vertices[0].Position;
				Vertices[CurrentVertexIndex].Normal = Resource->Polygons[I].Vertices[0].Normal;
				Vertices[CurrentVertexIndex].Tangent = Resource->Polygons[I].Vertices[0].Tangent;
				Vertices[CurrentVertexIndex].Binormal = Resource->Polygons[I].Vertices[0].Binormal;
				Vertices[CurrentVertexIndex].Texcoord = Resource->Polygons[I].Vertices[0].Texcoord;
				//Vertices[CurrentVertexIndex].Reserved0 = 1.0f;
				//Vertices[CurrentVertexIndex].Texcoords = Resource->Polygons[I].Vertices[0].Texcoord;
				//ConvertVertexNormals(Resource->Polygons[I].Vertices[0], Vertices[CurrentVertexIndex]);
				//VerifyVertexNormals(Resource->Polygons[I].Vertices[0], Vertices[CurrentVertexIndex], I);

				Vertices[CurrentVertexIndex + 1].Position = Resource->Polygons[I].Vertices[1].Position;
				Vertices[CurrentVertexIndex + 1].Normal = Resource->Polygons[I].Vertices[1].Normal;
				Vertices[CurrentVertexIndex + 1].Tangent = Resource->Polygons[I].Vertices[1].Tangent;
				Vertices[CurrentVertexIndex + 1].Binormal = Resource->Polygons[I].Vertices[1].Binormal;
				Vertices[CurrentVertexIndex + 1].Texcoord = Resource->Polygons[I].Vertices[1].Texcoord;
				//Vertices[CurrentVertexIndex + 1].Reserved0 = 1.0f;
				//Vertices[CurrentVertexIndex + 1].Texcoords = Resource->Polygons[I].Vertices[1].Texcoord;
				//ConvertVertexNormals(Resource->Polygons[I].Vertices[1], Vertices[CurrentVertexIndex + 1]);
				//VerifyVertexNormals(Resource->Polygons[I].Vertices[1], Vertices[CurrentVertexIndex + 1], I);

				Vertices[CurrentVertexIndex + 2].Position = Resource->Polygons[I].Vertices[2].Position;
				Vertices[CurrentVertexIndex + 2].Normal = Resource->Polygons[I].Vertices[2].Normal;
				Vertices[CurrentVertexIndex + 2].Tangent = Resource->Polygons[I].Vertices[2].Tangent;
				Vertices[CurrentVertexIndex + 2].Binormal = Resource->Polygons[I].Vertices[2].Binormal;
				Vertices[CurrentVertexIndex + 2].Texcoord = Resource->Polygons[I].Vertices[2].Texcoord;
				//Vertices[CurrentVertexIndex + 2].Reserved0 = 1.0f;
				//Vertices[CurrentVertexIndex + 2].Texcoords = Resource->Polygons[I].Vertices[2].Texcoord;
				//ConvertVertexNormals(Resource->Polygons[I].Vertices[2], Vertices[CurrentVertexIndex + 2]);
				//VerifyVertexNormals(Resource->Polygons[I].Vertices[2], Vertices[CurrentVertexIndex + 2], I);

				CurrentVertexIndex += 3;
				Processed[I] = true;
			}

			ZEInteriorRoomDraw Draw;
			Draw.Material = Material;
			Draw.VertexOffset = StartVertexIndex;
			Draw.VertexCount = CurrentVertexIndex - StartVertexIndex;

			Draws.AddByRef(Draw);
		}
	}

	VertexBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_VERTEX_BUFFER, Vertices.GetCount() * sizeof(ZEInteriorVertex), sizeof(ZEInteriorVertex), ZEGR_RU_IMMUTABLE, ZEGR_RBF_VERTEX_BUFFER, ZEGR_TF_NONE, Vertices.GetCArray());

	ZESize DrawCount = Draws.GetCount();
	for (ZESize I = 0; I < DrawCount; I++)
	{
		Draws[I].RenderCommand.Entity = GetOwner();
		Draws[I].RenderCommand.Order = 0;
		Draws[I].RenderCommand.Priority = 0;
		Draws[I].RenderCommand.ExtraParameters = &Draws[I];
		Draws[I].RenderCommand.Callback = ZEDelegateMethod(ZERNCommandCallback, ZEInteriorRoom, Render, this);

		if (Draws[I].Material == NULL || !Draws[I].Material->PreRender(Draws[I].RenderCommand))
			continue;
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
			GetOwner()->GetScene()->GetPhysicalWorld()->AddPhysicalObject(PhysicalMesh);
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
	Draws.Clear();

	VertexBuffer.Release();
	ConstantBuffer.Release();

	DirtyFlags.RaiseAll();
}

void ZEInteriorRoom::PreRender(const ZERNPreRenderParameters* Parameters)
{
	IsDrawn = true;
	ZESize RenderCommandCount = Draws.GetCount();
	for(ZESize I = 0; I < RenderCommandCount; I++)
	{
		if (!Draws[I].Material->PreRender(Draws[I].RenderCommand))
			continue;
		
		if (Parameters->Type != ZERN_RT_SHADOW)
		{
			Draws[I].RenderCommand.Reset();
			Parameters->CommandList->AddCommand(&Draws[I].RenderCommand);
		}
		else
		{
			Draws[I].RenderCommandShadow.Reset();
			Parameters->CommandList->AddCommand(&Draws[I].RenderCommandShadow);
		}
	}
}

void ZEInteriorRoom::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	ZEGRContext* Context = Parameters->Context;
	const ZERNStage* Stage = Parameters->Stage;

	ZEInteriorRoomDraw* Draw = static_cast<ZEInteriorRoomDraw*>(Command->ExtraParameters);
	if (!Draw->Material->SetupMaterial(Context, Stage))
		return;

	UpdateConstantBuffer();

	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBuffer);
	Context->SetVertexBuffer(0, VertexBuffer);

	Context->Draw(Draw->VertexCount, Draw->VertexOffset);

	Draw->Material->CleanupMaterial(Context, Stage);

	IsDrawn = false;
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

ZEInteriorRoom* ZEInteriorRoom::CreateInstance()
{
	return new ZEInteriorRoom();
}
