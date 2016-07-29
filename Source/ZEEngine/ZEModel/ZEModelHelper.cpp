//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelHelper.cpp
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

#include "ZEModelHelper.h"

#include "ZEModel.h"
#include "ZEMDResource.h"
#include "ZEMDResourceHelper.h"

bool ZEModelHelper::Load()
{
	Unload();

	if (Resource == NULL)
		return true;

	Position = Resource->GetPosition();
	Rotation = Resource->GetRotation();
	Scale = Resource->GetScale();
	UserDefinedProperties = Resource->GetUserDefinedProperties();

	if (Model != NULL)
	{
		switch (ParentType)
		{
			case ZE_MHPT_MESH:
				SetParentMesh(Resource->GetOwnerId() < Model->Meshes.GetCount() ? Model->Meshes[Resource->GetOwnerId()] : NULL);
				break;

			case ZE_MHPT_BONE:
				SetParentBone(Resource->GetOwnerId() < Model->Bones.GetCount() ? Model->Bones[Resource->GetOwnerId()] : NULL);
				break;

			default:
				SetParentMesh(NULL);
				break;
		}
	}
	else
	{
		SetParentMesh(NULL);
	}


	return true;
}

bool ZEModelHelper::Unload()
{
	return true;
}

ZEModel* ZEModelHelper::GetModel() const
{
	return Model;
}

ZEModelHelperParentType ZEModelHelper::GetParentType() const
{
	return ParentType;
}

void ZEModelHelper::SetParentMesh(ZEModelMesh* Mesh)
{
	zeCheckError(Mesh != NULL && GetModel() != NULL && Mesh->GetModel() != GetModel(), ZE_VOID, "Cannot set helper parent to a mesh. Mesh belongs to another model.");

	ParentMesh = Mesh;
	ParentBone = NULL;

	if (Mesh != NULL)
		ParentType = ZE_MHPT_MESH;
	else
		ParentType = ZE_MHPT_MODEL;
}

ZEModelMesh* ZEModelHelper::GetParentMesh() const
{
	if (ParentType != ZE_MHPT_MESH)
		return NULL;

	return ParentMesh;
}

void ZEModelHelper::SetParentBone(ZEModelBone* Bone)
{
	zeCheckError(Bone != NULL && GetModel() != NULL && Bone->GetModel() != GetModel(), ZE_VOID, "Cannot set helper parent to a bone. Bone belongs to another model.");

	ParentBone = Bone;
	ParentMesh = NULL;

	if (Bone != NULL)
		ParentType = ZE_MHPT_BONE;
	else
		ParentType = ZE_MHPT_MODEL;
}

ZEModelBone* ZEModelHelper::GetParentBone() const
{
	if (ParentType != ZE_MHPT_BONE)
		return NULL;

	return ParentBone;
}

void ZEModelHelper::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEModelHelper::GetName() const
{
	return Name;
}

void ZEModelHelper::SetPosition(const ZEVector3& Position)
{
	this->Position = Position;
}

const ZEVector3& ZEModelHelper::GetPosition() const
{
	return Position;
}

void ZEModelHelper::SetRotation(const ZEQuaternion& Rotation)
{
	this->Rotation = Rotation;
}

const ZEQuaternion& ZEModelHelper::GetRotation() const
{
	return Rotation;
}

void ZEModelHelper::SetScale(const ZEVector3& Scale)
{
	this->Scale = Scale;
}

const ZEVector3& ZEModelHelper::GetScale() const
{
	return Scale;
}

const ZEVector3 ZEModelHelper::GetModelPosition() const
{
	if (ParentType == ZE_MHPT_MESH && ParentMesh != NULL)
		return ParentMesh->GetModelTransform() * GetPosition();
	else if (ParentType == ZE_MHPT_BONE && ParentBone != NULL)
		return ParentBone->GetModelTransform() * GetPosition();
	else
		return  GetPosition();
}

const ZEVector3 ZEModelHelper::GetWorldPosition() const
{
	if (ParentType == ZE_MHPT_MODEL && Model != NULL)
		return Model->GetWorldTransform() * GetPosition();
	else if (ParentType == ZE_MHPT_MESH && ParentMesh != NULL)
		return ParentMesh->GetWorldTransform() * GetPosition();
	else if (ParentType == ZE_MHPT_BONE && ParentBone != NULL)
		return ParentBone->GetWorldTransform() * GetPosition();
	else
		return GetPosition();
}

const ZEQuaternion ZEModelHelper::GetModelRotation() const
{
	if (ParentType == ZE_MHPT_MESH && ParentMesh != NULL)
		return (ParentMesh->GetModelRotation() * GetRotation()).Normalize();
	else if (ParentType == ZE_MHPT_BONE && ParentBone != NULL)
		return (ParentBone->GetModelRotation() * GetRotation()).Normalize();
	else
		return GetRotation();
}

const ZEQuaternion ZEModelHelper::GetWorldRotation() const
{
	if (ParentType == ZE_MRHPT_MODEL && Model != NULL)
		return (Model->GetWorldRotation() * GetRotation()).Normalize();
	else if (ParentType == ZE_MHPT_MESH && ParentMesh != NULL)
		return (ParentMesh->GetWorldRotation() * GetRotation()).Normalize();
	else if (ParentType == ZE_MHPT_BONE && ParentBone != NULL)
		return (ParentBone->GetWorldRotation() * GetRotation()).Normalize();
	else
		return GetRotation();
}

const ZEVector3 ZEModelHelper::GetModelScale() const
{
	if (ParentType == ZE_MHPT_MESH && ParentMesh != NULL)
		return ParentMesh->GetModelScale() * GetScale();
	else
		return GetScale();
}

const ZEVector3 ZEModelHelper::GetWorldScale() const
{
	if (ParentType == ZE_MHPT_MODEL && Model != NULL)
		return Model->GetWorldScale() * GetScale();
	else if (ParentType == ZE_MHPT_MESH && ParentMesh != NULL)
		return ParentMesh->GetWorldScale() * GetScale();
	else if (ParentType == ZE_MHPT_BONE && ParentBone != NULL && ParentBone->GetModel() != NULL)
		return ParentBone->GetModel()->GetWorldScale() * GetScale();
	else
		return GetScale();
}

ZEVector3 ZEModelHelper::GetFront() const
{
	return GetRotation() * ZEVector3::UnitZ;
}

ZEVector3 ZEModelHelper::GetRight() const
{
	return GetRotation() * ZEVector3::UnitX;
}

ZEVector3 ZEModelHelper::GetUp() const
{
	return GetRotation() * ZEVector3::UnitY;
}

ZEVector3 ZEModelHelper::GetModelFront() const
{
	return GetModelRotation() * ZEVector3::UnitZ;
}

ZEVector3 ZEModelHelper::GetModelRight() const
{
	return GetModelRotation() * ZEVector3::UnitX;
}

ZEVector3 ZEModelHelper::GetModelUp() const
{
	return GetModelRotation() * ZEVector3::UnitY;
}

ZEVector3 ZEModelHelper::GetWorldFront() const
{
	return GetWorldRotation() * ZEVector3::UnitZ;
}

ZEVector3 ZEModelHelper::GetWorldRight() const
{
	return GetWorldRotation() * ZEVector3::UnitX;
}

ZEVector3 ZEModelHelper::GetWorldUp() const
{
	return GetWorldRotation() * ZEVector3::UnitY;
}

void ZEModelHelper::SetUserDefinedProperties(const ZEString& Properties)
{
	UserDefinedProperties = Properties;
}

const ZEString& ZEModelHelper::GetUserDefinedProperties() const
{
	return UserDefinedProperties;
}

void ZEModelHelper::SetResource(ZERSHolder<const ZEMDResourceHelper> Resource)
{
	this->Resource = Resource;
}

ZERSHolder<const ZEMDResourceHelper> ZEModelHelper::GetResource()
{
	return Resource;
}

ZEModelHelper::ZEModelHelper() : ModelLink(this)
{
	Model = NULL;

	ParentType = ZE_MHPT_MODEL;
	ParentMesh = NULL;
	ParentBone = NULL;
	
	Position = ZEVector3::Zero;
	Rotation = ZEQuaternion::Identity;
	Scale = ZEVector3::One;
}

ZEModelHelper::~ZEModelHelper()
{
	Unload();
	if (GetModel() != NULL)
		GetModel()->RemoveHelper(this);
}
