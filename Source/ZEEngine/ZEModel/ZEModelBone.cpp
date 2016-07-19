//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelBone.cpp
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

#include "ZEModelBone.h"

#include "ZEModel.h"
#include "ZEMDResourceBone.h"

#include "ZEMath/ZEMath.h"
#include "ZEGame/ZEScene.h"

#define ZEMD_BDF_INITIAL_TRANSFORM				0x0001
#define ZEMD_BDF_INV_INITIAL_TRANSFORM			0x0002
#define ZEMD_BDF_INITIAL_MODEL_TRANSFORM		0x0004
#define ZEMD_BDF_INV_INITIAL_MODEL_TRANSFORM	0x0008
#define ZEMD_BDF_TRANSFORM						0x0010
#define ZEMD_BDF_INV_TRANSFORM					0x0020
#define ZEMD_BDF_MODEL_TRANSFORM				0x0040
#define ZEMD_BDF_MODEL_INV_TRANSFORM			0x0080
#define ZEMD_BDF_MODEL_BOUNDING_BOX				0x0100
#define ZEMD_BDF_WORLD_TRANSFORM				0x0200
#define ZEMD_BDF_WORLD_INV_TRANSFORM			0x0400
#define ZEMD_BDF_WORLD_BOUNDING_BOX				0x0800
#define ZEMD_BDF_VERTEX_TRANSFORM				0x1000

#include <stdio.h>


void ZEModelBone::SetModel(ZEModel* Model)
{
	if (this->Model == Model)
		return;

	this->Model = Model;
	TransformChangedWorld();
}

void ZEModelBone::SetParent(ZEModelBone* Bone)
{
	if (Parent == Bone)
		return;

	Parent = Bone;
	ParentChanged();
}

// void ZEModelBone::LocalTransformChanged()
// {
// 	DirtyFlags.RaiseFlags(
// 		ZEMD_BDF_TRANSFORM | ZEMD_BDF_INV_TRANSFORM |
// 		ZEMD_BDF_MODEL_TRANSFORM | ZEMD_BDF_MODEL_INV_TRANSFORM |
// 		ZEMD_BDF_WORLD_TRANSFORM | ZEMD_BDF_WORLD_INV_TRANSFORM |
// 		ZEMD_BDF_VERTEX_TRANSFORM |
// 		ZEMD_BDF_MODEL_BOUNDING_BOX |
// 		ZEMD_BDF_WORLD_BOUNDING_BOX);
// 
// 	ze_for_each(ChildBone, ChildBones)
// 		ChildBone->ParentTransformChanged();
// 
// 	if (Model != NULL)
// 	{
// 		Model->ChildBoundingBoxChanged();
// 		Model->DirtyConstantBufferSkin = true;
// 	}
// }
// 
// void ZEModelBone::ParentTransformChanged()
// {
// 	DirtyFlags.RaiseFlags(
// 		ZEMD_BDF_MODEL_TRANSFORM | ZEMD_BDF_INV_MODEL_TRANSFORM |
// 		ZEMD_BDF_WORLD_TRANSFORM | ZEMD_BDF_INV_WORLD_TRANSFORM |
// 		ZEMD_BDF_VERTEX_TRANSFORM |
// 		ZEMD_BDF_MODEL_BOUNDING_BOX |
// 		ZEMD_BDF_WORLD_BOUNDING_BOX);
// 
// 	ze_for_each(ChildBone, ChildBones)
// 		ChildBone->ParentTransformChanged();
// }

void ZEModelBone::ParentChanged()
{
	if (Parent != NULL)
		SetModel(Parent->Model);

	TransformChangedInitialModel();
	TransformChangedModel();

	ze_for_each(ChildBone, ChildBones)
		ChildBone->ParentChanged();
}

void ZEModelBone::TransformChangedInitialLocal()
{
	DirtyFlags.RaiseFlags(
		ZEMD_BDF_INITIAL_TRANSFORM		 | ZEMD_BDF_INV_INITIAL_TRANSFORM | 
		ZEMD_BDF_INITIAL_MODEL_TRANSFORM | ZEMD_BDF_INV_INITIAL_MODEL_TRANSFORM |
		ZEMD_BDF_VERTEX_TRANSFORM);

	ze_for_each(ChildBone, ChildBones)
		ChildBone->TransformChangedInitialModel();

	if (Model != NULL)
	{
		Model->ChildBoundingBoxChanged();
		Model->DirtyConstantBufferSkin = true;
	}
}

void ZEModelBone::TransformChangedInitialModel()
{
	DirtyFlags.RaiseFlags(
		ZEMD_BDF_INITIAL_MODEL_TRANSFORM | ZEMD_BDF_INV_INITIAL_MODEL_TRANSFORM |
		ZEMD_BDF_VERTEX_TRANSFORM);

	ze_for_each(ChildBone, ChildBones)
		ChildBone->TransformChangedInitialModel();
}

void ZEModelBone::TransformChangedLocal()
{
	DirtyFlags.RaiseFlags(
		ZEMD_BDF_TRANSFORM | ZEMD_BDF_INV_TRANSFORM |
		ZEMD_BDF_MODEL_TRANSFORM | ZEMD_BDF_MODEL_INV_TRANSFORM	| ZEMD_BDF_MODEL_BOUNDING_BOX |
		ZEMD_BDF_WORLD_TRANSFORM | ZEMD_BDF_WORLD_INV_TRANSFORM	| ZEMD_BDF_WORLD_BOUNDING_BOX |
		ZEMD_BDF_VERTEX_TRANSFORM);

	ze_for_each(ChildBone, ChildBones)
		ChildBone->TransformChangedModel();
}

void ZEModelBone::TransformChangedModel()
{
	DirtyFlags.RaiseFlags(
		ZEMD_BDF_MODEL_TRANSFORM | ZEMD_BDF_MODEL_INV_TRANSFORM	| ZEMD_BDF_MODEL_BOUNDING_BOX |
		ZEMD_BDF_WORLD_TRANSFORM | ZEMD_BDF_WORLD_INV_TRANSFORM	| ZEMD_BDF_WORLD_BOUNDING_BOX |
		ZEMD_BDF_VERTEX_TRANSFORM);

	ze_for_each(ChildBone, ChildBones)
		ChildBone->TransformChangedModel();
}

void ZEModelBone::TransformChangedWorld()
{
	DirtyFlags.RaiseFlags(ZEMD_BDF_WORLD_TRANSFORM | ZEMD_BDF_WORLD_INV_TRANSFORM | ZEMD_BDF_WORLD_BOUNDING_BOX);

	ze_for_each(ChildBone, ChildBones)
		ChildBone->TransformChangedWorld();
}

bool ZEModelBone::Load()
{
	if (Resource == NULL)
		return true;

	DirtyFlags.RaiseAll();

	SetName(Resource->GetName());
	SetBoundingBox(Resource->GetBoundingBox());
	SetInitialPosition(Resource->GetPosition());
	SetInitialRotation(Resource->GetRotation());
	SetPosition(Resource->GetPosition());
	SetRotation(Resource->GetRotation());

	return true;
}

bool ZEModelBone::Unload()
{
	DirtyFlags.RaiseAll();

	return true;
}

ZEModel* ZEModelBone::GetModel() const
{
	return Model;
}

ZEModelBone* ZEModelBone::GetParent() const
{
	return Parent;
}

void ZEModelBone::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEModelBone::GetName() const
{
	return Name;
}

void ZEModelBone::SetBoundingBox(const ZEAABBox& BoundingBox)
{
	this->BoundingBox = BoundingBox;
	DirtyFlags.RaiseFlags(ZEMD_BDF_WORLD_BOUNDING_BOX | ZEMD_BDF_WORLD_BOUNDING_BOX);
}

const ZEAABBox& ZEModelBone::GetBoundingBox() const
{
	return BoundingBox;
}

const ZEAABBox& ZEModelBone::GetModelBoundingBox() const
{
	if (DirtyFlags.GetFlags(ZEMD_BDF_MODEL_BOUNDING_BOX))
	{
		ZEAABBox::Transform(ModelBoundingBox, BoundingBox, GetModelTransform());
		DirtyFlags.UnraiseFlags(ZEMD_BDF_MODEL_BOUNDING_BOX);
	}

	return ModelBoundingBox;
}

const ZEAABBox& ZEModelBone::GetWorldBoundingBox() const
{
	if (DirtyFlags.GetFlags(ZEMD_BDF_WORLD_BOUNDING_BOX))
	{
		ZEAABBox::Transform(WorldBoundingBox, BoundingBox, GetWorldTransform());
		DirtyFlags.UnraiseFlags(ZEMD_BDF_WORLD_BOUNDING_BOX);
	}

	return WorldBoundingBox;
}

const ZEMatrix4x4& ZEModelBone::GetInitialTransform() const
{
	if (DirtyFlags.GetFlags(ZEMD_BDF_INITIAL_TRANSFORM))
	{
		ZEMatrix4x4::CreateOrientation(InitialTransform, InitialPosition, InitialRotation);
		DirtyFlags.UnraiseFlags(ZEMD_BDF_INITIAL_TRANSFORM);
	}

	return InitialTransform;
}

const ZEMatrix4x4& ZEModelBone::GetInvInitialTransform() const
{
	if (DirtyFlags.GetFlags(ZEMD_BDF_INV_INITIAL_TRANSFORM))
	{
		ZEMatrix4x4::Inverse(InvInitialTransform, GetInitialTransform());
		DirtyFlags.UnraiseFlags(ZEMD_BDF_INV_INITIAL_TRANSFORM);
	}

	return InvInitialTransform;
}

const ZEMatrix4x4& ZEModelBone::GetInitialModelTransform() const
{
	if (DirtyFlags.GetFlags(ZEMD_BDF_INITIAL_MODEL_TRANSFORM))
	{
		if (Parent == NULL)
			InitialModelTransform = GetTransform();
		else
			ZEMatrix4x4::Multiply(InitialModelTransform, Parent->GetInitialModelTransform(), GetInitialTransform());

		DirtyFlags.UnraiseFlags(ZEMD_BDF_INITIAL_MODEL_TRANSFORM);
	}

	return InitialModelTransform;
}

const ZEMatrix4x4& ZEModelBone::GetInvInitialModelTransform() const
{
	if (DirtyFlags.GetFlags(ZEMD_BDF_INV_INITIAL_MODEL_TRANSFORM))
	{
		ZEMatrix4x4::Inverse(InvInitialModelTransform, GetInitialModelTransform());
		DirtyFlags.UnraiseFlags(ZEMD_BDF_INV_INITIAL_MODEL_TRANSFORM);
	}

	return InvInitialModelTransform;
}

const ZEMatrix4x4& ZEModelBone::GetTransform() const
{
	if (DirtyFlags.GetFlags(ZEMD_BDF_TRANSFORM))
	{
		ZEMatrix4x4::CreateOrientation(Transform, Position, Rotation);
		DirtyFlags.UnraiseFlags(ZEMD_BDF_TRANSFORM);
	}

	return Transform;
}

const ZEMatrix4x4& ZEModelBone::GetInvTransform() const
{
	if (DirtyFlags.GetFlags(ZEMD_BDF_INV_TRANSFORM))
	{
		ZEMatrix4x4::Inverse(InvTransform, GetTransform());
		DirtyFlags.UnraiseFlags(ZEMD_BDF_INV_TRANSFORM);
	}

	return InvTransform;
}

const ZEMatrix4x4& ZEModelBone::GetModelTransform() const
{
	if (DirtyFlags.GetFlags(ZEMD_BDF_MODEL_TRANSFORM))
	{
		if (Parent == NULL)
			ModelTransform = GetTransform();
		else
			ZEMatrix4x4::Multiply(ModelTransform, Parent->GetModelTransform(), GetTransform());

		DirtyFlags.UnraiseFlags(ZEMD_BDF_MODEL_TRANSFORM);
	}

	return ModelTransform;
}

const ZEMatrix4x4& ZEModelBone::GetInvModelTransform() const
{
	if (DirtyFlags.GetFlags(ZEMD_BDF_MODEL_INV_TRANSFORM))
	{
		ZEMatrix4x4::Inverse(InvModelTransform, GetModelTransform());
		DirtyFlags.UnraiseFlags(ZEMD_BDF_MODEL_INV_TRANSFORM);
	}

	return InvModelTransform;
}

const ZEMatrix4x4& ZEModelBone::GetWorldTransform() const
{
	if (DirtyFlags.GetFlags(ZEMD_BDF_WORLD_TRANSFORM))
	{
		if (Model != NULL)
			ZEMatrix4x4::Multiply(WorldTransform, Model->GetWorldTransform(), GetModelTransform());
		else
			WorldTransform = GetModelTransform();

		DirtyFlags.UnraiseFlags(ZEMD_BDF_WORLD_TRANSFORM);
	}

	return WorldTransform;
}

const ZEMatrix4x4& ZEModelBone::GetInvWorldTransform() const
{
	if (DirtyFlags.GetFlags(ZEMD_BDF_WORLD_INV_TRANSFORM))
	{
		ZEMatrix4x4::Inverse(InvWorldTransform, GetWorldTransform());
		DirtyFlags.UnraiseFlags(ZEMD_BDF_WORLD_INV_TRANSFORM);
	}

	return InvWorldTransform;
}

const ZEMatrix4x4& ZEModelBone::GetVertexTransform() const
{
	if (DirtyFlags.GetFlags(ZEMD_BDF_VERTEX_TRANSFORM))
	{
		ZEMatrix4x4::Multiply(VertexTransform, GetModelTransform(), GetInvInitialModelTransform());
		DirtyFlags.UnraiseFlags(ZEMD_BDF_VERTEX_TRANSFORM);
	}

	return VertexTransform;
}

void ZEModelBone::SetInitialPosition(const ZEVector3& Position)
{
	InitialPosition = Position;
	TransformChangedInitialLocal();
}


const ZEVector3& ZEModelBone::GetInitialPosition() const
{
	return InitialPosition;
}

void ZEModelBone::SetInitialRotation(const ZEQuaternion& Rotation)
{
	InitialRotation = Rotation;
	TransformChangedInitialLocal();
}

const ZEQuaternion& ZEModelBone::GetInitialRotation() const
{
	return InitialRotation;
}

void ZEModelBone::SetPosition(const ZEVector3& Position)
{
	this->Position = Position;
	TransformChangedLocal();
}

const ZEVector3& ZEModelBone::GetPosition() const
{
	return Position;
}

void ZEModelBone::SetRotation(const ZEQuaternion& Rotation)
{
	this->Rotation = Rotation;
	TransformChangedLocal();
}

const ZEQuaternion& ZEModelBone::GetRotation() const
{
	return Rotation;
}

void ZEModelBone::SetModelPosition(const ZEVector3& ModelPosition)
{
	if (Parent == NULL)
	{
		SetPosition(ModelPosition);
	}
	else
	{
		ZEVector3 Result;
		ZEMatrix4x4::Transform(Result, Parent->GetInvModelTransform(), ModelPosition);
		SetPosition(Result);
	}
}

const ZEVector3 ZEModelBone::GetModelPosition() const
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

void ZEModelBone::SetModelRotation(const ZEQuaternion& ModelRotation)
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

const ZEQuaternion ZEModelBone::GetModelRotation() const
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

void ZEModelBone::SetWorldPosition(const ZEVector3& WorldPosition)
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
			ZEMatrix4x4::Transform(Result, Model->GetInvWorldTransform(), WorldPosition);
			SetPosition(Result);
		}
		else
		{
			ZEVector3 Result;
			ZEMatrix4x4::Transform(Result, Parent->GetInvWorldTransform(), WorldPosition);
			SetPosition(Result);
		}
	}
}

const ZEVector3 ZEModelBone::GetWorldPosition() const
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

void ZEModelBone::SetWorldRotation(const ZEQuaternion& WorldRotation)
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

const ZEQuaternion ZEModelBone::GetWorldRotation() const
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

void ZEModelBone::SetAnimationType(ZEModelAnimationType AnimationType)
{
	this->AnimationType = AnimationType;
}

ZEModelAnimationType ZEModelBone::GetAnimationType() const
{
	return AnimationType;
}

const ZEList2<ZEModelBone>& ZEModelBone::GetChildBones() const
{
	return ChildBones;
}

void ZEModelBone::AddChildBone(ZEModelBone* Bone)
{
	zeCheckError(Bone == NULL, ZE_VOID, "Bone cannot be null.");
	zeCheckError(Bone->GetParent() != NULL, ZE_VOID, "Bone is already added to another bone or model.");
	zeCheckError(Bone->GetModel() != NULL, ZE_VOID, "Bone is already used by a model.");

	Bone->SetParent(this);
	ChildBones.AddEnd(&Bone->ParentLink);
}

void ZEModelBone::RemoveChildBone(ZEModelBone* Bone)
{
	zeCheckError(Bone == NULL, ZE_VOID, "Bone cannot be null.");
	zeCheckError(!ChildBones.Exists(&Bone->ParentLink), ZE_VOID, "Bone is not a child of this mesh.");

	Bone->SetParent(NULL);
	ChildBones.Remove(&Bone->ParentLink);
}

void ZEModelBone::SetResource(ZERSHolder<const ZEMDResourceBone> Resource)
{
	this->Resource = Resource;
}

ZERSHolder<const ZEMDResourceBone> ZEModelBone::GetResource()
{
	return Resource;
}

void ZEModelBone::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	ZERayCastHelper Helper;
	Helper.SetReport(&Report);
	Helper.SetWorldTransform(&GetWorldTransform());
	Helper.SetInvWorldTransform(&GetInvWorldTransform());
	Helper.SetObject(Model);
	Helper.SetSubObject(this);

	Helper.RayCastBoundingBox(GetWorldBoundingBox(), GetBoundingBox());
}

ZEModelBone::ZEModelBone() : ParentLink(this), ModelLink(this)
{
	DirtyFlags.RaiseAll();
	Model = NULL;
	Parent = NULL;

	InitialPosition = ZEVector3::Zero;
	InitialRotation = ZEQuaternion::Identity;
	Position = ZEVector3::Zero;
	Rotation = ZEQuaternion::Identity;

	InitialTransform = ZEMatrix4x4::Identity;
	InvInitialTransform = ZEMatrix4x4::Identity;
	InitialModelTransform = ZEMatrix4x4::Identity;
	InvInitialModelTransform = ZEMatrix4x4::Identity;
	Transform = ZEMatrix4x4::Identity;
	InvTransform = ZEMatrix4x4::Identity;
	ModelTransform = ZEMatrix4x4::Identity;
	InvModelTransform = ZEMatrix4x4::Identity;
	WorldTransform = ZEMatrix4x4::Identity;
	InvWorldTransform = ZEMatrix4x4::Identity;

	BoundingBox = ZEAABBox::Zero;
	ModelBoundingBox = ZEAABBox::Zero;
	WorldBoundingBox = ZEAABBox::Zero;

	AnimationType = ZE_MAT_NOANIMATION;
}

ZEModelBone::~ZEModelBone()
{
	Unload();
}
