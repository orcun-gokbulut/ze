//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ModelBone.cpp
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

#include "ModelBone.h"
#include "Model.h"
#include <stdio.h>
#include "Core/Core.h"

const char* ZEModelBone::GetName()
{
	return BoneResource->Name;
}

ZEModelBone* ZEModelBone::GetParentBone()
{
	return ParentBone;
}

const ZEArray<ZEModelBone*> ZEModelBone::GetChildBones()
{
	return ChildBones;
}

bool ZEModelBone::IsRootBone()
{
	return ParentBone == NULL;
}

const ZEAABoundingBox& ZEModelBone::GetLocalBoundingBox()
{
	return BoneResource->BoundingBox;
}

const ZEAABoundingBox& ZEModelBone::GetModelBoundingBox()
{
	if (UpdateModelBoundingBox)
	{
		ZEAABoundingBox::Transform(ModelBoundingBox, BoneResource->BoundingBox, GetModelTransform());
		UpdateModelBoundingBox = false;
	}
	return ModelBoundingBox;
}

const ZEAABoundingBox& ZEModelBone::GetWorldBoundingBox()
{
	if (UpdateWorldBoundingBox)
	{
		ZEAABoundingBox::Transform(WorldBoundingBox, BoneResource->BoundingBox, GetWorldTransform());
		UpdateWorldBoundingBox = false;
	}
	return WorldBoundingBox;
}

const ZEMatrix4x4& ZEModelBone::GetInverseTransform()
{
	return BoneResource->InverseTransform;
}

const ZEMatrix4x4& ZEModelBone::GetLocalTransform()
{
	//if (UpdateLocalTransform)
	{
		if (ParentBone == NULL)
			LocalTransform = GetRelativeTransform();
		else
		{
			ZEMatrix4x4::Multiply(LocalTransform, GetRelativeTransform(), ParentBone->GetLocalTransform());
			return LocalTransform;
		}
		UpdateLocalTransform = false;
	}
	return LocalTransform;
}

const ZEMatrix4x4& ZEModelBone::GetWorldTransform()
{
//	if (UpdateWorldTransform)
	{
		ZEMatrix4x4::Multiply(WorldTransform, GetLocalTransform(), Owner->GetWorldTransform());
		UpdateWorldTransform = false;
	}

	return WorldTransform;
}

const ZEMatrix4x4& ZEModelBone::GetModelTransform()
{
	//if (UpdateWorldTransform)
	{
		ZEMatrix4x4::Multiply(WorldTransform, GetLocalTransform(), Owner->GetLocalTransform());
		UpdateWorldTransform = false;
	}

	return WorldTransform;
}

const ZEMatrix4x4& ZEModelBone::GetVertexTransform()
{
//	if (UpdateVertexTransform)
	{
		ZEMatrix4x4::Multiply(VertexTransform, BoneResource->InverseTransform, GetLocalTransform());
		UpdateVertexTransform = false;
	}

	return VertexTransform;
}

const ZEMatrix4x4& ZEModelBone::GetRelativeTransform()
{
//	if (UpdateRelativeTransform)
	{
		ZEMatrix4x4 Temp1, Temp2;
		ZEMatrix4x4::CreateTranslation(Temp1, RelativePosition);
		ZEMatrix4x4::CreateRotation(Temp2, RelativeRotation);
		ZEMatrix4x4::Multiply(RelativeTransform, Temp2, Temp1);

		UpdateRelativeTransform = false;
	}
	return RelativeTransform;
}

const ZEVector3& ZEModelBone::GetRelativePosition()
{
	return RelativePosition;
}

void ZEModelBone::SetRelativePosition(const ZEVector3& Position)
{
	UpdateRelativeTransform = true;
	UpdateVertexTransform = true;
	UpdateModelTransform = true;
	UpdateWorldTransform = true;
	UpdateModelBoundingBox = true;
	UpdateWorldBoundingBox = true;
	Owner->UpdateBoundingBox();

	RelativePosition = Position;
}

const ZEQuaternion& ZEModelBone::GetRelativeRotation()
{
	return RelativeRotation;
}

void ZEModelBone::SetRelativeRotation(const ZEQuaternion& Rotation)
{
	UpdateRelativeTransform = true;
	UpdateVertexTransform = true;
	UpdateModelTransform = true;
	UpdateWorldTransform = true;
	UpdateModelBoundingBox = true;
	UpdateWorldBoundingBox = true;
	Owner->UpdateBoundingBox();
	Owner->UpdateBoneTransforms();
	RelativeRotation = Rotation;
}

const ZEVector3 ZEModelBone::GetModelPosition()
{
	if (ParentBone == NULL)
		return RelativePosition;
	else
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, ParentBone->GetModelTransform(), RelativePosition);
		return Temp;
	}
}

const ZEQuaternion ZEModelBone::GetModelRotation()
{
	if (ParentBone == NULL)
		return RelativeRotation;
	else
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, RelativeRotation, ParentBone->GetModelRotation());
		return Temp;
	}
}

const ZEVector3 ZEModelBone::GetWorldPosition()
{
	if (ParentBone == NULL)
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, Owner->GetWorldTransform(), RelativePosition);
		return Temp;
	}
	else
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, ParentBone->GetWorldTransform(), RelativePosition);
		return Temp;
	}
}

const ZEQuaternion ZEModelBone::GetWorldRotation()
{
	if (ParentBone == NULL)
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, RelativeRotation, Owner->GetWorldRotation());
		return Temp;
	}
	else
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, RelativeRotation, ParentBone->GetWorldRotation());
		return Temp;
	}
}

void ZEModelBone::SetAnimationType(ZEModelAnimationType AnimationType)
{
	this->AnimationType = AnimationType;
}

ZEModelAnimationType ZEModelBone::GetAnimationType()
{
	return AnimationType;
}

void ZEModelBone::AddChild(ZEModelBone* Bone)
{
	Bone->ParentBone = this;
	ChildBones.Add(Bone);
}

void ZEModelBone::RemoveChild(ZEModelBone* Bone)
{
	Bone->ParentBone = NULL;
	ChildBones.DeleteValue(Bone);
}

void ZEModelBone::SetPhysicsEnabled(bool Enabled)
{
	PhysicsEnabled = Enabled;
}

bool ZEModelBone::GetPhysicsEnabled()
{
	return PhysicsEnabled;
}

void ZEModelBone::Initialize(ZEModel* Model, const ZEModelResourceBone* BoneResource)
{
	Owner = Model;
	this->BoneResource = BoneResource;
	RelativePosition = BoneResource->RelativePosition;
	RelativeRotation = BoneResource->RelativeRotation;

	UpdateRelativeTransform = true;
	UpdateVertexTransform = true;
	UpdateModelTransform = true;
	UpdateWorldTransform = true;
	UpdateModelBoundingBox = true;
	UpdateWorldBoundingBox = true;
	Owner->UpdateBoneTransforms();
}

void ZEModelBone::Deinitialize()
{
	Owner = NULL;
	ParentBone = NULL;
	ChildBones.Clear();

	UpdateRelativeTransform = true;
	UpdateVertexTransform = true;
	UpdateModelTransform = true;
	UpdateWorldTransform = true;

	UpdateModelBoundingBox = true;
	UpdateWorldBoundingBox = true;
}

void ZEModelBone::ModelWorldTransformChanged()
{
	UpdateVertexTransform = true;
	UpdateWorldTransform = true;
	UpdateWorldBoundingBox = true;
}

void ZEModelBone::ModelTransformChanged()
{
	UpdateVertexTransform = true;
	UpdateModelTransform = true;
	UpdateWorldTransform = true;
	UpdateModelBoundingBox = true;
	UpdateWorldBoundingBox = true;
}

ZEModelBone::ZEModelBone()
{
	Owner = NULL;
	ParentBone = NULL;
	UpdateRelativeTransform = true;
	UpdateVertexTransform = true;
	UpdateModelTransform = true;
	UpdateWorldTransform = true;

	UpdateModelBoundingBox = true;
	UpdateWorldBoundingBox = true;
}

ZEModelBone::~ZEModelBone()
{
	Deinitialize();
}
