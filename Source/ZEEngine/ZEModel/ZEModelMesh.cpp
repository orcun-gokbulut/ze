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
#include "ZEModelDraw.h"
#include "ZEModelMeshLOD.h"
#include "ZEMDResource.h"
#include "ZEMDResourceMesh.h"
#include "ZEMDResourceLOD.h"

#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEViewVolume.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZERenderer/ZERNView.h"
#include "ZERenderer/ZERNMaterial.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNRenderParameters.h"

#define ZEMD_MDF_TRANSFORM					0x0001
#define ZEMD_MDF_INV_TRANSFORM				0x0002
#define ZEMD_MDF_MODEL_TRANSFORM			0x0004
#define ZEMD_MDF_MODEL_INV_TRANSFORM		0x0008
#define ZEMD_MDF_MODEL_BOUNDING_BOX			0x0010
#define ZEMD_MDF_WORLD_TRANSFORM			0x0020
#define ZEMD_MDF_WORLD_INV_TRANSFORM		0x0040
#define ZEMD_MDF_WORLD_BOUNDING_BOX			0x0080
#define ZEMD_MDF_CONSTANT_BUFFER			0x0100

struct ZEModelMeshConstants
{
	ZEMatrix4x4	WorldTransform;
	ZEMatrix4x4	WorldTransformInverseTranspose;
	ZEMatrix4x4	PreSkinTransform;
	ZEVector4	ClippingPlane0;
	ZEVector4	ClippingPlane1;
	ZEVector4	ClippingPlane2;
	ZEVector4	ClippingPlane3;
};

void ZEModelMesh::SetModel(ZEModel* Model)
{
	if (this->Model == Model)
		return;

	this->Model = Model;
	TransformChangedWorld();
}

void ZEModelMesh::SetParent(ZEModelMesh* Mesh)
{
	if (Parent == Mesh)
		return;

	Parent = Mesh;
	ParentChanged();
}

void ZEModelMesh::ParentChanged()
{
	if (Parent != NULL)
		SetModel(Parent->Model);

	TransformChangedModel();

	ze_for_each(ChildMesh, ChildMeshes)
		ChildMesh->ParentChanged();
}

void ZEModelMesh::TransformChangedLocal()
{
	DirtyFlags.RaiseFlags(
		ZEMD_MDF_TRANSFORM | ZEMD_MDF_INV_TRANSFORM |
		ZEMD_MDF_MODEL_TRANSFORM | ZEMD_MDF_MODEL_INV_TRANSFORM	| ZEMD_MDF_MODEL_BOUNDING_BOX |
		ZEMD_MDF_WORLD_TRANSFORM | ZEMD_MDF_WORLD_INV_TRANSFORM	| ZEMD_MDF_WORLD_BOUNDING_BOX |
		ZEMD_MDF_CONSTANT_BUFFER);

	ze_for_each(ChildMesh, ChildMeshes)
		ChildMesh->TransformChangedModel();

	if (Model != NULL)
		Model->ChildBoundingBoxChanged();
}

void ZEModelMesh::TransformChangedModel()
{
	DirtyFlags.RaiseFlags(
		ZEMD_MDF_MODEL_TRANSFORM | ZEMD_MDF_MODEL_INV_TRANSFORM	| ZEMD_MDF_MODEL_BOUNDING_BOX |
		ZEMD_MDF_WORLD_TRANSFORM | ZEMD_MDF_WORLD_INV_TRANSFORM	| ZEMD_MDF_WORLD_BOUNDING_BOX |
		ZEMD_MDF_CONSTANT_BUFFER);

	ze_for_each(ChildMesh, ChildMeshes)
		ChildMesh->TransformChangedModel();
}

void ZEModelMesh::TransformChangedWorld()
{
	DirtyFlags.RaiseFlags(ZEMD_MDF_WORLD_TRANSFORM | ZEMD_MDF_WORLD_INV_TRANSFORM | ZEMD_MDF_WORLD_BOUNDING_BOX |
		ZEMD_MDF_CONSTANT_BUFFER);

	ze_for_each(ChildMesh, ChildMeshes)
		ChildMesh->TransformChangedWorld();
}

void ZEModelMesh::UpdateConstantBuffer()
{
	if (!DirtyFlags.GetFlags(ZEMD_MDF_CONSTANT_BUFFER))
		return;

	ZEModelMeshConstants Constants;

	Constants.WorldTransform = GetWorldTransform();
	Constants.WorldTransformInverseTranspose = GetInvWorldTransform().Transpose();
	Constants.PreSkinTransform = GetModelTransform();

	if (ClippingPlanes.GetCount() > 0)
		Constants.ClippingPlane0 = Constants.WorldTransformInverseTranspose * ClippingPlanes[0].ToABCD();
	else
		Constants.ClippingPlane0 = ZEVector4::Zero;

	if (ClippingPlanes.GetCount() > 1)
		Constants.ClippingPlane1 = Constants.WorldTransformInverseTranspose * ClippingPlanes[1].ToABCD();
	else
		Constants.ClippingPlane1 = ZEVector4::Zero;

	if (ClippingPlanes.GetCount() > 2)
		Constants.ClippingPlane2 = Constants.WorldTransformInverseTranspose * ClippingPlanes[2].ToABCD();
	else
		Constants.ClippingPlane2 = ZEVector4::Zero;

	if (ClippingPlanes.GetCount() > 3)
		Constants.ClippingPlane3 = Constants.WorldTransformInverseTranspose * ClippingPlanes[3].ToABCD();
	else
		Constants.ClippingPlane3 = ZEVector4::Zero;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZEMD_MDF_CONSTANT_BUFFER);
}

ZEModelMesh::ZEModelMesh() : ParentLink(this), ModelLink(this)
{
	DirtyFlags.RaiseAll();

	Resource = NULL;
	Model = NULL;
	Parent = NULL;
	Visible = true;

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
	Unload();

	if (GetModel() != NULL)
		GetModel()->RemoveMesh(this);
}

bool ZEModelMesh::Load(const ZEMDResourceMesh* Resource)
{
	if (Resource == NULL)
		return true;

	DirtyFlags.RaiseAll();

	AnimationType = ZE_MAT_PREDEFINED;
	
	SetName(Resource->GetName());
	SetPosition(Resource->GetPosition());
	SetRotation(Resource->GetRotation());
	SetScale(Resource->GetScale());
	SetVisible(Resource->GetVisible());
	SetBoundingBox(Resource->GetBoundingBox());
	ConstantBuffer = ZEGRConstantBuffer::CreateResource(sizeof(ZEModelMeshConstants));

	ze_for_each(ResourceLOD, Resource->GetLODs())
	{
		ZEModelMeshLOD* NewLOD = ZEModelMeshLOD::CreateInstance();
		AddLOD(NewLOD);
		NewLOD->Load(ResourceLOD.GetPointer());
	}

	this->Resource = Resource;

	return true;
}

bool ZEModelMesh::Unload()
{
	DirtyFlags.RaiseAll();

	while (LODs.GetFirst() != NULL)
	{
		ZEModelMeshLOD* LOD = LODs.GetFirst()->GetItem();
		LOD->Destroy();
	}

	return true;
}

ZEModel* ZEModelMesh::GetModel() const
{
	return Model;
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
	if (DirtyFlags.GetFlags(ZEMD_MDF_MODEL_TRANSFORM))
	{
		if (Parent == NULL)
		{
			ModelTransform = GetTransform();
		}
		else
		{
			ZEMatrix4x4::Multiply(ModelTransform, Parent->GetModelTransform(), GetTransform());
		}

		DirtyFlags.UnraiseFlags(ZEMD_MDF_MODEL_TRANSFORM);
	}
	
	return ModelTransform;
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
	if (DirtyFlags.GetFlags(ZEMD_MDF_WORLD_TRANSFORM))
	{
		if (Model == NULL)
		{
			WorldTransform = GetModelTransform();
		}
		else
		{
			ZEMatrix4x4::Multiply(WorldTransform, Model->GetWorldTransform(), GetModelTransform());
		}

		DirtyFlags.UnraiseFlags(ZEMD_MDF_WORLD_TRANSFORM);
	}

	return WorldTransform;
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

void ZEModelMesh::SetPosition(const ZEVector3& Position)
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

const ZEQuaternion& ZEModelMesh::GetRotation() const
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
	if (Model == NULL)
	{
		SetModelPosition(WorldPosition);
	}
	else
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
}

const ZEVector3 ZEModelMesh::GetWorldPosition() const
{
	if (Model == NULL)
	{
		return GetModelPosition();
	}
	else
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
}

void ZEModelMesh::SetWorldRotation(const ZEQuaternion& WorldRotation)
{
	if (Model == NULL)
	{
		SetModelRotation(WorldRotation);
	}
	else
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
}

const ZEQuaternion ZEModelMesh::GetWorldRotation() const
{
	if (Model == NULL)
	{
		return GetModelRotation();
	}
	else
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
}

void ZEModelMesh::SetWorldScale(const ZEVector3& WorldScale)
{
	if (Model == NULL)
	{
		SetModelScale(WorldScale);
	}
	else
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
}

const ZEVector3 ZEModelMesh::GetWorldScale() const
{
	if (Model == NULL)
	{
		return GetModelScale();
	}
	else
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
}

void ZEModelMesh::SetAnimationType(ZEModelAnimationType AnimationType)
{
	this-> AnimationType = AnimationType;
}

ZEModelAnimationType ZEModelMesh::GetAnimationType()
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

	LOD->Mesh = this;
	LODs.AddEnd(&LOD->MeshLink);
}

void ZEModelMesh::RemoveLOD(ZEModelMeshLOD* LOD)
{
	zeCheckError(LOD == NULL, ZE_VOID, "Mesh cannot be null.");
	zeCheckError(LOD->GetMesh() != this, ZE_VOID, "Mesh is not a child mesh of this mesh.");

	LOD->Mesh = NULL;
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
	zeCheckError(Mesh->GetModel() != GetModel(), ZE_VOID, "Mesh is already used by a different model.");
	zeCheckError(ChildMeshes.Exists(&Mesh->ParentLink), ZE_VOID, "Mesh is already a child of this mesh.");

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
	if (ClippingPlanes.GetCount() == Count)
		return;

	if (Count > 4)
		return;

	ClippingPlanes.Resize(Count);
	DirtyFlags.RaiseFlags(ZEMD_MDF_CONSTANT_BUFFER);
}

ZESize ZEModelMesh::GetClippingPlaneCount()
{
	return ClippingPlanes.GetCount();
}

void ZEModelMesh::SetClippingPlane(ZESize Index, const ZEPlane& Plane)
{
	if (Index > ClippingPlanes.GetCount())
		return;

	ClippingPlanes[Index] = Plane;
	DirtyFlags.RaiseFlags(ZEMD_MDF_CONSTANT_BUFFER);
}

const ZEPlane& ZEModelMesh::GetClippingPlane(ZESize Index)
{
	return ClippingPlanes[Index];
}

bool ZEModelMesh::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (!Visible)
		return false;

	ZEAABBox BoundingBox = GetWorldBoundingBox();
	if (Parameters->View->ViewVolume != NULL && !Parameters->View->ViewVolume->IntersectionTest(BoundingBox))
		return false;

	UpdateConstantBuffer();

	float ClosestBoundingBoxEdgeDistanceSquare = FLT_MAX;
	float CurrentBoundingBoxEdgeDistanceSquare;

	for (ZEUInt I = 0; I < 8; I++)
	{
		CurrentBoundingBoxEdgeDistanceSquare = ZEVector3::DistanceSquare(Parameters->View->Position, BoundingBox.GetVertex(I));

		if (CurrentBoundingBoxEdgeDistanceSquare < ClosestBoundingBoxEdgeDistanceSquare)
			ClosestBoundingBoxEdgeDistanceSquare = CurrentBoundingBoxEdgeDistanceSquare;
	}
	
	float DrawOrder = 0.0f;

	float EntityDistanceSquare = ZEVector3::DistanceSquare(Parameters->View->Position, GetWorldPosition());	
	if (!CustomDrawOrderEnabled)
		DrawOrder = ClosestBoundingBoxEdgeDistanceSquare;
	else
		DrawOrder = ClosestBoundingBoxEdgeDistanceSquare * (CustomDrawOrder + 1);
 	
	float CurrentDistanceSquare = 0.0f;
	ZEModelMeshLOD* CurrentLOD = NULL;

	ze_for_each(LOD, LODs)
	{
		float LODDistanceSquare = LOD->GetStartDistance() * LOD->GetEndDistance();

		if (LODDistanceSquare < ClosestBoundingBoxEdgeDistanceSquare)
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

	if (ClosestBoundingBoxEdgeDistanceSquare > (CurrentLOD->GetEndDistance() * CurrentLOD->GetEndDistance()))
		return false;

	if (CurrentLOD->GetVertexType() == ZEMD_VT_SKINNED)
		GetModel()->UpdateConstantBufferBoneTransforms();

	ze_for_each(Draw, CurrentLOD->GetDraws())
	{
		Draw->RenderCommand.Entity = GetModel();
		Draw->RenderCommand.Priority = 0;
		Draw->RenderCommand.Order = DrawOrder;
		Draw->RenderCommand.InstanceTag = (CurrentLOD->GetVertexType() == ZEMD_VT_NORMAL ? &Draw->InstanceTag : NULL);

		if (Draw->GetMaterial() == NULL || !Draw->GetMaterial()->IsLoaded())
			continue;

		if (!Draw->GetMaterial()->PreRender(Draw->RenderCommand))
			continue;

		Parameters->Renderer->AddCommand(&Draw->RenderCommand);
	}

	return true;
}

void ZEModelMesh::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	ZERayCastHelper Helper;
	Helper.SetReport(&Report);
	Helper.SetWorldTransform(&GetWorldTransform());
	Helper.SetInvWorldTransform(&GetInvWorldTransform());
	Helper.SetObject(Model);
	Helper.SetSubObject(this);

	if (!Helper.RayCastBoundingBox(GetWorldBoundingBox(), GetBoundingBox()))
		return;

	if (Resource != NULL)
		Helper.RayCastMesh(Resource->GetGeometry().GetConstCArray(), Resource->GetGeometry().GetCount(), sizeof(ZEVector3));
}

ZEModelMesh* ZEModelMesh::CreateInstance()
{
	return new ZEModelMesh();
}
