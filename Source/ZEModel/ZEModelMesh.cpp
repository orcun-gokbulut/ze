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
#include "ZEModelFileFormat.h"
#include "ZEGraphics\ZERenderer.h"
#include "ZEGame\ZEScene.h"
#include "ZEPhysics\ZEPhysicalRigidBody.h"
#include "ZEGame\ZEDrawParameters.h"
/*
void ZEModelMesh::UpdatePhysicalBody()
{
	PhysicalBody = ZEPhysicalRigidBody::CreateInstance();

	PhysicalBody->SetEnabled(MeshResource->PhysicalBody.Enabled);
	PhysicalBody.Mass;
	PhysicalBody->SetLinearDamping(MeshResource->PhysicalBody.LinearDamping);
	PhysicalBody->SetAngularDamping(MeshResource->PhysicalBody.AngularDamping);
	PhysicalBody->SetPosition(GetWorldTransform() * MeshResource->PhysicalBody.Position);
	PhysicalBody->SetRotation(Owner->GetWorldRotation() * Rotation * MeshResource->PhysicalBody.Orientation);
	
	PhysicalBody->SetMassCenterPosition(MeshResource->PhysicalBody.MassCenter);
	
	//PhysicalBody->SetMassCenterPosition(MeshResource->PhysicalBody.MassCenterPosition);
	//PhysicalBody->SetMassCenterRotation(MeshResource->PhysicalBody.MassCenterRotation);

	for (size_t I = 0; I < MeshResource->PhysicalBody.Shapes.GetCount(); I++)
	{
		ZE
	}

}*/

void ZEModelMesh::SetActiveLOD(size_t LOD)
{
	AutoLOD = false;
	this->ActiveLOD = LOD;
}

size_t ZEModelMesh::GetActiveLOD()
{
	return ActiveLOD;
}

void ZEModelMesh::SetAutoLOD(bool Enabled)
{
	AutoLOD = Enabled;
}

bool ZEModelMesh::GetAutoLOD()
{
	return AutoLOD;
}

void ZEModelMesh::SetVisible(bool Visible)
{
	this->Visible = Visible;
}

bool ZEModelMesh::GetVisible()
{
	return Visible;
}

const char* ZEModelMesh::GetName()
{
	return MeshResource->Name;
}

const ZEAABoundingBox& ZEModelMesh::GetLocalBoundingBox()
{
	return LocalBoundingBox;
}

const ZEAABoundingBox& ZEModelMesh::GetModelBoundingBox()
{
	if (UpdateModelBoundingBox)
	{
		ZEAABoundingBox::Transform(ModelBoundingBox, LocalBoundingBox, GetModelTransform());
		UpdateModelBoundingBox = false;
	}
	
	return ModelBoundingBox;
}


const ZEAABoundingBox& ZEModelMesh::GetWorldBoundingBox()
{
	if (UpdateWorldBoundingBox)
	{
		ZEAABoundingBox::Transform(WorldBoundingBox, GetLocalBoundingBox(), GetWorldTransform());
		UpdateWorldBoundingBox = false;
	}
	return WorldBoundingBox;
}

const ZEMatrix4x4& ZEModelMesh::GetLocalTransform()
{
	if (UpdateLocalTransform)
	{
		ZEMatrix4x4::CreateOrientation(LocalTransform, Position, Rotation, Scale);
		UpdateLocalTransform = false;
	}
	
	return LocalTransform;
}

const ZEMatrix4x4& ZEModelMesh::GetModelTransform()
{
	if (UpdateModelTransform)
	{
		ZEMatrix4x4::Multiply(ModelTransform, GetLocalTransform(), Owner->GetLocalTransform());
		UpdateModelTransform = false;
	}
	return ModelTransform;	
	
}

const ZEMatrix4x4& ZEModelMesh::GetWorldTransform()
{
	if (UpdateWorldTransform)
	{
		ZEMatrix4x4::Multiply(WorldTransform, GetLocalTransform(), Owner->GetWorldTransform());
		UpdateWorldTransform = false;
	}
	return WorldTransform;	
}
	
void ZEModelMesh::SetLocalPosition(const ZEVector3& LocalPosition)
{
	UpdateWorldBoundingBox = true;
	UpdateModelBoundingBox = true;
	UpdateModelTransform = true;
	UpdateWorldTransform = true;
	UpdateModelTransform = true;
	UpdateLocalTransform = true;
	Owner->UpdateBoundingBox();

	Position = LocalPosition;
}

const ZEVector3& ZEModelMesh::GetLocalPostion()
{
	return Position;
}

void ZEModelMesh::SetLocalRotation(const ZEQuaternion& LocalRotation)
{
	UpdateWorldBoundingBox = true;
	UpdateWorldTransform = true;
	UpdateModelBoundingBox = true;
	UpdateModelTransform = true;
	UpdateLocalTransform = true;
	Owner->UpdateBoundingBox();

	Rotation = LocalRotation;
}

const ZEQuaternion& ZEModelMesh::GetLocalRotation()
{
	return Rotation;
}

void ZEModelMesh::SetLocalScale(const ZEVector3& LocalScale)
{
	UpdateWorldBoundingBox = true;
	UpdateModelTransform = true;
	UpdateModelTransform = true;
	UpdateModelBoundingBox = true;
	UpdateLocalTransform = true;
	Owner->UpdateBoundingBox();

	Scale = LocalScale;
}

const ZEVector3& ZEModelMesh::GetLocalScale()
{
	return Scale;
}

void ZEModelMesh::SetAnimationType(ZEModelAnimationType AnimationType)
{
	this-> AnimationType = AnimationType;
}

ZEModelAnimationType ZEModelMesh::GetAnimationType()
{
	return AnimationType;
}

ZEPhysicalRigidBody* ZEModelMesh::GetPhysicalBody()
{
	return PhysicalBody;
}

void ZEModelMesh::SetPhysicsEnabled(bool Enabled)
{
	PhysicsEnabled = Enabled;
}

bool ZEModelMesh::GetPhysicsEnabled()
{
	return PhysicsEnabled;
}

void ZEModelMesh::Initialize(ZEModel* Model,  const ZEModelResourceMesh* MeshResource)
{
	Owner = Model;
	this->MeshResource = MeshResource;
	ActiveLOD = 0;
	AutoLOD = true;
	PhysicsEnabled = false;
	AnimationType = ZE_MAT_PREDEFINED;
	Position = MeshResource->Position;
	Rotation = MeshResource->Rotation;
	Scale = MeshResource->Scale;
	LocalBoundingBox = MeshResource->BoundingBox;

	UpdateWorldBoundingBox = true;
	UpdateWorldTransform = true;
	UpdateModelBoundingBox = true;
	UpdateModelTransform = true;
	UpdateLocalTransform = true;

	LODs.SetCount(MeshResource->LODs.GetCount());
	for (size_t I = 0; I < MeshResource->LODs.GetCount(); I++)
		LODs[I].Initialize(Owner, this, &MeshResource->LODs[I]);
}

void ZEModelMesh::Deinitialize()
{
	Owner = NULL;
	LODs.Clear();
}

void ZEModelMesh::ModelTransformChanged()
{
	UpdateWorldBoundingBox = true;
	UpdateWorldTransform = true;
	UpdateModelBoundingBox = true;
	UpdateModelTransform = true;
}

void ZEModelMesh::ModelWorldTransformChanged()
{
	UpdateWorldBoundingBox = true;
	UpdateWorldTransform = true;
}

void ZEModelMesh::Draw(ZEDrawParameters* DrawParameters)
{
	LODs[0].Draw(DrawParameters);	
}


ZEModelMesh::ZEModelMesh()
{
	Owner = NULL;
	MeshResource = NULL;
}

ZEModelMesh::~ZEModelMesh()
{
	Deinitialize();
}


