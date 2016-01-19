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
#include "ZEModelResource.h"
#include "ZEModel.h"

const char* ZEModelHelper::GetName() const
{
	return HelperResource->Name;
}

ZEModelHelperOwnerType ZEModelHelper::GetOwnerType() const
{
	return ParentType;
}

ZEModelMesh* ZEModelHelper::GetMeshOwner() const
{
	return ParentMesh;
}

ZEModelBone* ZEModelHelper::GetBoneOwner() const
{
	return ParentBone;
}

void ZEModelHelper::SetPosition(const ZEVector3& Position)
{
	this->Position = Position;
}

const ZEVector3& ZEModelHelper::GetPosition() const
{
	return Position;
}

const ZEVector3 ZEModelHelper::GetModelPosition() const
{
	ZEVector3 Temp;

	if (ParentType == ZE_MHOT_MESH)
		ZEMatrix4x4::Transform(Temp, ParentMesh->GetTransform(), GetPosition());
	else if (ParentType == ZE_MHOT_BONE)
		ZEMatrix4x4::Transform(Temp, ParentBone->GetModelTransform(), GetPosition());
	else
		Temp = GetPosition();

	return Temp;
}

const ZEVector3 ZEModelHelper::GetWorldPosition() const
{
	ZEVector3 Temp;

	if (ParentType == ZE_MHOT_MESH)
		ZEMatrix4x4::Transform(Temp, ParentMesh->GetWorldTransform(), GetPosition());
	else if (ParentType == ZE_MHOT_BONE)
		ZEMatrix4x4::Transform(Temp, ParentBone->GetWorldTransform(), GetPosition());
	else
		ZEMatrix4x4::Transform(Temp, Model->GetWorldTransform(), GetPosition());

	return Temp;
}

void ZEModelHelper::SetRotation(const ZEQuaternion& Rotation)
{
	this->Rotation = Rotation;
}

const ZEQuaternion& ZEModelHelper::GetRotation() const
{
	return Rotation;
}

const ZEQuaternion ZEModelHelper::GetModelRotation() const
{
	ZEQuaternion Temp;

	if (ParentType == ZE_MHOT_MESH)
		ZEQuaternion::Product(Temp, ParentMesh->GetLocalRotation(), GetRotation());
	else if (ParentType == ZE_MHOT_BONE)
		ZEQuaternion::Product(Temp, ParentBone->GetModelRotation(), GetRotation());
	else
		Temp = GetRotation();

	ZEQuaternion::Normalize(Temp, Temp);
	return Temp;
}

const ZEQuaternion ZEModelHelper::GetWorldRotation() const
{
	ZEQuaternion Temp;

	if (ParentType == ZE_MHOT_MESH)
	{
		ZEQuaternion OwnerRotation;
		ZEQuaternion::Product(OwnerRotation, Model->GetWorldRotation(), ParentMesh->GetLocalRotation());
		ZEQuaternion::Product(Temp, OwnerRotation, GetRotation());
	}
	else if (ParentType == ZE_MHOT_BONE)
	{
		ZEQuaternion::Product(Temp, ParentBone->GetWorldRotation(), GetRotation());
	}
	else
	{
		ZEQuaternion::Product(Temp, Model->GetWorldRotation(), GetRotation());
	}

	ZEQuaternion::Normalize(Temp, Temp);
	return Temp;
}

void ZEModelHelper::SetScale(const ZEVector3& Scale)
{
	this->Scale = Scale;
}

const ZEVector3& ZEModelHelper::GetScale() const
{
	return Scale;
}

const ZEVector3 ZEModelHelper::GetModelScale() const
{
	ZEVector3 Temp;

	if (ParentType == ZE_MHOT_MESH)
		ZEVector3::Multiply(Temp, ParentMesh->GetScale(), GetScale());
	else
		Temp = GetScale();

	return Temp;
}

const ZEVector3 ZEModelHelper::GetWorldScale() const
{
	ZEVector3 Temp;

	if (ParentType == ZE_MHOT_MESH)
	{
		ZEVector3 OwnerScale;
		ZEVector3::Multiply(OwnerScale, Model->GetWorldScale(), ParentMesh->GetScale());
		ZEVector3::Multiply(Temp, OwnerScale, GetScale());
	}
	else
	{
		ZEVector3::Multiply(Temp, Model->GetWorldScale(), GetScale());
	}

	return Temp;
}

ZEVector3 ZEModelHelper::GetFront() const
{
	return Rotation * ZEVector3::UnitZ;
}

ZEVector3 ZEModelHelper::GetRight() const
{
	return Rotation * ZEVector3::UnitX;
}

ZEVector3 ZEModelHelper::GetUp() const
{
	return Rotation * ZEVector3::UnitY;
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

void ZEModelHelper::Load()
{


}

void ZEModelHelper::Load(ZEHolder<const ZEModelResource> Resource, const ZEModelResourceHelper* HelperResource)
{
	this->HelperResource = HelperResource;
	ParentType = (ZEModelHelperOwnerType)HelperResource->GetOwnerType();
	Position = HelperResource->GetPosition();
	Rotation = HelperResource->GetRotation();
	Scale = HelperResource->GetScale();

	switch (ParentType)
	{
	case ZE_MHOT_MESH:
		ParentMesh = &Model->Meshes[HelperResource->OwnerId];
		break;
	case ZE_MHOT_BONE:
		ParentBone = &Model->Bones[HelperResource->OwnerId];
		break;
	default:
		break;
	}
}

ZEModelHelper::ZEModelHelper()
{
	Model = NULL;

	ParentType = ZE_MHOT_MODEL;
	ParentMesh = NULL;
	ParentBone = NULL;

	Position = ZEVector3::Zero;
	Rotation = ZEQuaternion::Identity;
	Scale = ZEVector3::One;

	HelperResource = NULL;
}




