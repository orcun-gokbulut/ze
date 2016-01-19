//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelMesh.cpp
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

#include "ZEModelMesh.h"

#include "ZEModel.h"
#include "ZEModelMeshLOD.h"
#include "ZERenderer\ZERNCuller.h"
#include "ZERenderer\ZERNView.h"
#include "ZERenderer\ZERNMaterial.h"
#include "ZERenderer\ZERNRenderer.h"
#include "ZEMath\ZEMath.h"

#define ZEMD_MDF_TRANSFORM						0x0001
#define ZEMD_MDF_INV_TRANSFORM					0x0002
#define ZEMD_MDF_MODEL_TRANSFORM				0x0004
#define ZEMD_MDF_MODEL_INV_TRANSFORM			0x0008
#define ZEMD_MDF_MODEL_BOUNDING_BOX				0x0010
#define ZEMD_MDF_WORLD_TRANSFORM				0x0020
#define ZEMD_MDF_WORLD_INV_TRANSFORM			0x0040
#define ZEMD_MDF_WORLD_BOUNDING_BOX				0x0080

void ZEModelMesh::ParentChanged()
{
	if (Parent == NULL)
		Model = NULL;
	else
		Model = Parent->Model;

	TransformChangedModel();

	ze_for_each(ChildMesh, ChildMeshes)
		ChildMesh->ParentChanged();
}

void ZEModelMesh::TransformChangedLocal()
{
	DirtyFlags.RaiseFlags(
		ZEMD_MDF_TRANSFORM | ZEMD_MDF_INV_TRANSFORM |
		ZEMD_MDF_MODEL_TRANSFORM | ZEMD_MDF_MODEL_INV_TRANSFORM	| ZEMD_MDF_MODEL_BOUNDING_BOX |
		ZEMD_MDF_WORLD_TRANSFORM | ZEMD_MDF_WORLD_INV_TRANSFORM	| ZEMD_MDF_WORLD_BOUNDING_BOX);

	ze_for_each(ChildMesh, ChildMeshes)
		ChildMesh->TransformChangedModel();
}

void ZEModelMesh::TransformChangedModel()
{
	DirtyFlags.RaiseFlags(
		ZEMD_MDF_MODEL_TRANSFORM | ZEMD_MDF_MODEL_INV_TRANSFORM	| ZEMD_MDF_MODEL_BOUNDING_BOX |
		ZEMD_MDF_WORLD_TRANSFORM | ZEMD_MDF_WORLD_INV_TRANSFORM	| ZEMD_MDF_WORLD_BOUNDING_BOX);

	ze_for_each(ChildMesh, ChildMeshes)
		ChildMesh->TransformChangedModel();
}

ZEModel* ZEModelMesh::GetModel() const
{
	return Model;
}

void ZEModelMesh::SetParent(ZEModelMesh* Mesh)
{
	if (Parent == Mesh)
		return;

	Parent = Mesh;
	ParentChanged();
}

ZEModelMesh* ZEModelMesh::GetParent() const
{
	return Parent;
}

void ZEModelMesh::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEModelMesh::GetName() const
{
	return Name;
}

void ZEModelMesh::SetBoundingBox(const ZEAABBox& BoundingBox)
{
	this->BoundingBox = BoundingBox;
	DirtyFlags.RaiseFlags(ZEMD_MDF_WORLD_BOUNDING_BOX | ZEMD_MDF_MODEL_BOUNDING_BOX);
}

const ZEAABBox& ZEModelMesh::GetBoundingBox() const
{
	return BoundingBox;
}

const ZEAABBox& ZEModelMesh::GetModelBoundingBox() const
{
	if (DirtyFlags.GetFlags(ZEMD_MDF_MODEL_BOUNDING_BOX))
	{
		ZEAABBox::Transform(ModelBoundingBox, BoundingBox, GetModelTransform());
		DirtyFlags.UnraiseFlags(ZEMD_MDF_MODEL_BOUNDING_BOX);
	}

	return ModelBoundingBox;
}

const ZEAABBox& ZEModelMesh::GetWorldBoundingBox() const
{
	if (DirtyFlags.GetFlags(ZEMD_MDF_WORLD_BOUNDING_BOX))
	{
		ZEAABBox::Transform(WorldBoundingBox, BoundingBox, GetWorldTransform());
		DirtyFlags.UnraiseFlags(ZEMD_MDF_WORLD_BOUNDING_BOX);
	}

	return WorldBoundingBox;
}

const ZEMatrix4x4& ZEModelMesh::GetTransform() const
{
	if (DirtyFlags.GetFlags(ZEMD_MDF_TRANSFORM))
	{
		ZEMatrix4x4::CreateOrientation(Transform, Position, Rotation, Scale);
		DirtyFlags.UnraiseFlags(ZEMD_MDF_TRANSFORM);
	}

	return Transform;
}

const ZEMatrix4x4& ZEModelMesh::GetInvTransform() const
{
	if (DirtyFlags.GetFlags(ZEMD_MDF_INV_TRANSFORM))
	{
		ZEMatrix4x4::Inverse(InvTransform, GetTransform());
		DirtyFlags.UnraiseFlags(ZEMD_MDF_INV_TRANSFORM);
	}

	return InvTransform;
}

const ZEMatrix4x4& ZEModelMesh::GetModelTransform() const
{
	if (Parent == NULL)
	{
		ModelTransform = GetTransform();
	}
	else
	{
		ZEMatrix4x4::Multiply(ModelTransform, Parent->GetModelTransform(), GetTransform());
		return ModelTransform;
	}
}

const ZEMatrix4x4& ZEModelMesh::GetInvModelTransform() const
{
	if (DirtyFlags.GetFlags(ZEMD_MDF_MODEL_INV_TRANSFORM))
	{
		ZEMatrix4x4::Inverse(InvModelTransform, GetModelTransform());
		DirtyFlags.UnraiseFlags(ZEMD_MDF_MODEL_INV_TRANSFORM);
	}

	return InvModelTransform;
}

const ZEMatrix4x4& ZEModelMesh::GetWorldTransform() const
{
	if (Parent == NULL)
	{
		ZEMatrix4x4::Multiply(WorldTransform, Model->GetWorldTransform(), GetTransform());
		return WorldTransform;
	}
	else
	{
		ZEMatrix4x4::Multiply(WorldTransform, Parent->GetWorldTransform(), GetTransform());
		return WorldTransform;
	}
}

const ZEMatrix4x4& ZEModelMesh::GetInvWorldTransform() const
{
	if (DirtyFlags.GetFlags(ZEMD_MDF_WORLD_INV_TRANSFORM))
	{
		ZEMatrix4x4::Inverse(InvWorldTransform, GetWorldTransform());
		DirtyFlags.UnraiseFlags(ZEMD_MDF_WORLD_INV_TRANSFORM);
	}

	return InvWorldTransform;
}

void ZEModelMesh::SetPosition(const ZEVector3& LocalPosition)
{
	this->Position = Position;
	TransformChangedLocal();
}

const ZEVector3& ZEModelMesh::GetPosition() const
{
	return Position;
}

void ZEModelMesh::SetRotation(const ZEQuaternion& Rotation)
{
	this->Rotation = Rotation;
	TransformChangedLocal();
}

const ZEQuaternion& ZEModelMesh::GetLocalRotation() const
{
	return Rotation;
}

void ZEModelMesh::SetScale(const ZEVector3& Scale)
{
	this->Scale = Scale;
	TransformChangedLocal();
}

const ZEVector3& ZEModelMesh::GetScale() const
{
	return Scale;
}

void ZEModelMesh::SetModelPosition(const ZEVector3& ModelPosition)
{
	if (Parent == NULL)
	{
		SetPosition(ModelPosition);
	}
	else
	{
		ZEVector3 Result;
		ZEMatrix4x4::Transform(Result, Parent->GetModelTransform().Inverse(), ModelPosition);
		SetPosition(Result);
	}
}

const ZEVector3 ZEModelMesh::GetModelPosition() const
{
	if (Parent == NULL)
	{
		return Position;
	}
	else
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, Parent->GetModelTransform(), Position);
		return Temp;
	}
}

void ZEModelMesh::SetModelRotation(const ZEQuaternion& ModelRotation)
{
	if (Parent == NULL)
	{
		SetRotation(ModelRotation);
	}
	else
	{
		ZEQuaternion Temp, Result;
		ZEQuaternion::Conjugate(Temp, Parent->GetModelRotation());
		ZEQuaternion::Product(Result, Temp, ModelRotation);
		SetRotation(Result);
	}
}

const ZEQuaternion ZEModelMesh::GetModelRotation() const
{
	if (Parent == NULL)
	{
		return Rotation;
	}
	else
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, Parent->GetModelRotation(), Rotation);
		ZEQuaternion::Normalize(Temp, Temp);
		return Temp;
	}
}

void ZEModelMesh::SetModelScale(const ZEVector3& ModelScale)
{
	if (Parent == NULL)
	{
		SetScale(ModelScale);
	}
	else
	{
		ZEVector3 Temp;
		ZEVector3::Divide(Temp, ModelScale, Parent->GetModelScale());
		SetScale(Temp);
	}		
}

const ZEVector3 ZEModelMesh::GetModelScale() const
{
	if (Parent == NULL)
	{
		return Scale;
	}
	else
	{
		ZEVector3 Temp;
		ZEVector3::Multiply(Temp, Parent->GetModelScale(), Scale);
		return Temp;
	}
}

void ZEModelMesh::SetWorldPosition(const ZEVector3& WorldPosition)
{
	if (Parent == NULL)
	{
		ZEVector3 Result;
		ZEMatrix4x4::Transform(Result, Model->GetWorldTransform().Inverse(), WorldPosition);
		SetPosition(Result);
	}
	else
	{
		ZEVector3 Result;
		ZEMatrix4x4::Transform(Result, Parent->GetWorldTransform().Inverse(), WorldPosition);
		SetPosition(Result);
	}
}

const ZEVector3 ZEModelMesh::GetWorldPosition() const
{
	if (Parent == NULL)
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, Model->GetWorldTransform(), Position);
		return Temp;
	}
	else
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, Parent->GetWorldTransform(), Position);
		return Temp;
	}
}

void ZEModelMesh::SetWorldRotation(const ZEQuaternion& WorldRotation)
{
	if (Parent == NULL)
	{	
		ZEQuaternion Temp, Result;
		ZEQuaternion::Conjugate(Temp, Model->GetWorldRotation());
		ZEQuaternion::Product(Result, Temp, WorldRotation);
		SetRotation(Result);
	}
	else
	{
		ZEQuaternion Temp, Result;
		ZEQuaternion::Conjugate(Temp, Parent->GetWorldRotation());
		ZEQuaternion::Product(Result, Temp, WorldRotation);
		SetRotation(Result);
	}
}

const ZEQuaternion ZEModelMesh::GetWorldRotation() const
{
	if (Parent == NULL)
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, Model->GetWorldRotation(), Rotation);
		ZEQuaternion::Normalize(Temp, Temp);
		return Temp;
	}
	else
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, Parent->GetWorldRotation(), Rotation);
		ZEQuaternion::Normalize(Temp, Temp);
		return Temp;
	}
}

void ZEModelMesh::SetWorldScale(const ZEVector3& WorldScale)
{
	if (Parent == NULL)
	{
		ZEVector3 Temp;
		ZEVector3::Divide(Temp, WorldScale, Model->GetWorldScale());
		SetScale(Temp);
	}
	else
	{
		ZEVector3 Temp;
		ZEVector3::Divide(Temp, WorldScale, Parent->GetWorldScale());
		SetScale(Temp);
	}
}

const ZEVector3 ZEModelMesh::GetWorldScale() const
{
	if (Parent == NULL)
	{
		ZEVector3 Temp;
		ZEVector3::Multiply(Temp, Model->GetWorldScale(), Scale);
		return Temp;
	}
	else
	{
		ZEVector3 Temp;
		ZEVector3::Multiply(Temp, Parent->GetWorldScale(), Scale);
		return Temp;
	}
}

void ZEModelMesh::SetAnimationType(ZEModelAnimationType AnimationType)
{
	this-> AnimationType = AnimationType;
}

ZEModelAnimationType ZEModelMesh::GetAnimationType() const
{
	return AnimationType;
}

void ZEModelMesh::SetVisible(bool Visible)
{
	this->Visible = Visible;
}

bool ZEModelMesh::GetVisible() const
{
	return Visible;
}

const ZEList2<ZEModelMeshLOD>& ZEModelMesh::GetLODs()
{
	return LODs;
}

void ZEModelMesh::AddLOD(ZEModelMeshLOD* LOD)
{
	zeCheckError(LOD == NULL, ZE_VOID, "Mesh cannot be null.");
	zeCheckError(LOD->GetMesh() != NULL, ZE_VOID, "Mesh is already added to another mesh.");
	zeCheckError(LOD->GetModel() != GetModel(), ZE_VOID, "Mesh is already used by a model.");
	
	LOD->Mesh = this;
	LOD->Model = GetModel();
	LODs.AddEnd(&LOD->MeshLink);

}

void ZEModelMesh::RemoveLOD(ZEModelMeshLOD* LOD)
{
	zeCheckError(LOD == NULL, ZE_VOID, "Mesh cannot be null.");
	zeCheckError(!LODs.Exists(&LOD->MeshLink), ZE_VOID, "Mesh is not a child mesh of this mesh.");

	LOD->Mesh = NULL;
	LOD->Model = NULL;
	LODs.Remove(&LOD->MeshLink);
}

const ZEList2<ZEModelMesh>& ZEModelMesh::GetChildMeshes()
{
	return ChildMeshes;
}

void ZEModelMesh::AddChildMesh(ZEModelMesh* Mesh)
{
	zeCheckError(Mesh == NULL, ZE_VOID, "Mesh cannot be null.");
	zeCheckError(Mesh->GetParent() != NULL, ZE_VOID, "Mesh is already added to another mesh.");
	zeCheckError(Mesh->GetModel() != NULL, ZE_VOID, "Mesh is already used by a model.");

	Mesh->SetParent(this);
	ChildMeshes.AddEnd(&Mesh->ParentLink);
}

void ZEModelMesh::RemoveChildMesh(ZEModelMesh* Mesh)
{
	zeCheckError(Mesh == NULL, ZE_VOID, "Mesh cannot be null.");
	zeCheckError(!ChildMeshes.Exists(&Mesh->ParentLink), ZE_VOID, "Mesh is not a child of this mesh.");
	
	Mesh->SetParent(NULL);
	ChildMeshes.Remove(&Mesh->ParentLink);
}

void ZEModelMesh::SetCustomDrawOrderEnabled(bool Enabled)
{
	CustomDrawOrderEnabled = Enabled;
}

bool ZEModelMesh::GetCustomDrawOrderEnabled() const
{
	return CustomDrawOrderEnabled;
}

void ZEModelMesh::SetCustomDrawOrder(ZEUInt8 DrawOrder)
{
	CustomDrawOrder = DrawOrder;
}

ZEUInt8 ZEModelMesh::GetCustomDrawOrder() const
{
	return CustomDrawOrder;
}


void ZEModelMesh::SetClippingPlaneCount(ZESize Count)
{
	ClippingPlanes.Resize(Count);
}

ZESize ZEModelMesh::GetClippingPlaneCount() const
{
	return ClippingPlanes.GetCount();
}

void ZEModelMesh::SetClippingPlane(ZESize Index, const ZEPlane& Plane)
{
	ClippingPlanes[Index] = Plane;
}

const ZEPlane& ZEModelMesh::GetClippingPlane(ZESize Index) const
{
	return ClippingPlanes[Index];
}

bool ZEModelMesh::PreRender(const ZERNCullParameters* CullParameters)
{
	if (!Visible)
		return false;

	//if (CullParameters->View->ViewVolume->CullTest(GetWorldBoundingBox()))
		//return false;

	ZEVector3 WorldPosition;
	ZEMatrix4x4::Transform(WorldPosition, GetWorldTransform(), ZEVector3::Zero);
	float EntityDistanceSquare = ZEVector3::DistanceSquare(CullParameters->View->Position, GetWorldPosition());	

	float DrawOrder = 0.0f;	
	if (!CustomDrawOrderEnabled)
		DrawOrder = EntityDistanceSquare;
	else
		DrawOrder = EntityDistanceSquare * (CustomDrawOrder + 1);
 	
	float CurrentDistanceSquare = 0.0f;
	ZEModelMeshLOD* CurrentLOD = NULL;
	ze_for_each(LOD, LODs)
	{
		float LODDistanceSquare = LOD->GetStartDistance() * LOD->GetStartDistance();
		if (LODDistanceSquare < EntityDistanceSquare)
		{
			if (CurrentDistanceSquare <= LODDistanceSquare)
			{
				CurrentDistanceSquare = LODDistanceSquare;
				CurrentLOD = LOD.GetPointer();
			}
		}
	}
	
	if (CurrentLOD == NULL)
		return false;

	if (EntityDistanceSquare > (CurrentLOD->GetEndDistance() * CurrentLOD->GetEndDistance()))
		return false;

	ze_for_each(Draw, CurrentLOD->GetDraws())
	{
		Draw->RenderCommand.Priority = 0;
		Draw->RenderCommand.Order = DrawOrder;
		Draw->RenderCommand.Entity = Model;
		Draw->RenderCommand.ExtraParameters = CurrentLOD;
		Draw->RenderCommand.StageMask = Draw->GetMaterial()->GetStageMask();
		CullParameters->Renderer->AddCommand(&Draw->RenderCommand);
	}

	return true;
}

bool ZEModelMesh::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	ZERay LocalRay;
	ZEMatrix4x4::Transform(LocalRay.p, GetInvWorldTransform(), Parameters.Ray.p);
	ZEMatrix4x4::Transform3x3(LocalRay.v, GetInvWorldTransform(), Parameters.Ray.v);
	LocalRay.v.NormalizeSelf();

	float RayT;
	if (!ZEAABBox::IntersectionTest(GetBoundingBox(), LocalRay, RayT))
		return false;
	/*
	float MinT = ZE_FLOAT_MAX;
	ZESize PoligonIndex;
	if (RayCastPoligons(LocalRay, MinT, PoligonIndex))
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

// 			if (Parameters.Extras.GetFlags(ZE_RCRE_NORMAL) || Parameters.Extras.GetFlags(ZE_RCRE_BINORMAL))
// 			{
// 				ZEVector3 V0 = MeshResource->LODs[0].Vertices[3 * Report.PoligonIndex].Position;
// 				ZEVector3 V1 = MeshResource->LODs[0].Vertices[3 * Report.PoligonIndex + 1].Position;
// 				ZEVector3 V2 = MeshResource->LODs[0].Vertices[3 * Report.PoligonIndex + 2].Position;
// 
// 				ZEVector3 Binormal = ZEVector3(V0, V1);
// 				ZEVector3 Tangent = ZEVector3(V0, V2);
// 				ZEVector3 Normal;
// 				ZEVector3::CrossProduct(Normal, Binormal, Tangent);
// 
// 				if (Parameters.Extras.GetFlags(ZE_RCRE_NORMAL))
// 				{
// 					ZEMatrix4x4::Transform3x3(Report.Normal, GetWorldTransform(), Normal);
// 					Report.Normal.NormalizeSelf();
// 				}
// 
// 				if (Parameters.Extras.GetFlags(ZE_RCRE_BINORMAL))
// 				{
// 					ZEMatrix4x4::Transform3x3(Report.Binormal, GetWorldTransform(), Binormal);
// 					Report.Binormal.NormalizeSelf();
// 				}
// 			}

			return true;
		}
	}*/

	return false;
}

void ZEModelMesh::LoadResource(ZEHolder<const ZEModelResource> ModelResource, const ZEModelResourceMesh* MeshResource)
{
	this->ModelResource = ModelResource;
	this->MeshResource = MeshResource;

	AnimationType = ZE_MAT_PREDEFINED;
	SetPosition(MeshResource->GetPosition());
	SetRotation(MeshResource->GetRotation());
	SetScale(MeshResource->GetScale());
	SetVisible(MeshResource->GetVisible());
	SetBoundingBox(MeshResource->GetBoundingBox());

	ze_for_each(ResourceLOD, MeshResource->GetLODs())
	{
		ZEPointer<ZEModelMeshLOD> NewLOD = new ZEModelMeshLOD();
		NewLOD->LoadResource(ModelResource, &(*ResourceLOD));
		AddLOD(NewLOD.Transfer());
	}
}

ZEModelMesh::ZEModelMesh() : ParentLink(this)
{
	DirtyFlags.RaiseAll();

	Model = NULL;
	Parent = NULL;
	Visible = true;
	MeshResource = NULL;
	
	Position = ZEVector3::Zero;
	Rotation = ZEQuaternion::Identity;
	Scale = ZEVector3::One;

	BoundingBox = ZEAABBox::Zero;
	ModelBoundingBox = ZEAABBox::Zero;
	WorldBoundingBox = ZEAABBox::Zero;

	Transform = ZEMatrix4x4::Identity;
	InvTransform = ZEMatrix4x4::Identity;
	ModelTransform = ZEMatrix4x4::Identity;
	InvModelTransform = ZEMatrix4x4::Identity;
	WorldTransform = ZEMatrix4x4::Identity;
	InvWorldTransform = ZEMatrix4x4::Identity;

	AnimationType = ZE_MAT_NOANIMATION;
	CustomDrawOrderEnabled = false;
	CustomDrawOrder = 0;
}

ZEModelMesh::~ZEModelMesh()
{
	ze_for_each(Mesh, ChildMeshes)
	{
		RemoveChildMesh(Mesh.GetPointer());
		delete Mesh.GetPointer();
	}

	ze_for_each(LOD, LODs)
	{
		RemoveLOD(LOD.GetPointer());
		delete LOD.GetPointer();
	}
}
