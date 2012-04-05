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
#include "ZEGraphics/ZECamera.h"
#include "ZEGraphics/ZERenderer.h"
#include "ZEGame/ZEScene.h"
#include "ZEGame/ZEDrawParameters.h"

void ZEModelMesh::SetActiveLOD(ZEUInt LOD)
{
	AutoLOD = false;
	this->ActiveLOD = LOD;
}

ZEUInt ZEModelMesh::GetActiveLOD()
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

const ZEAABBox& ZEModelMesh::GetLocalBoundingBox()
{
	return LocalBoundingBox;
}

const ZEAABBox& ZEModelMesh::GetModelBoundingBox()
{
	ZEAABBox::Transform(ModelBoundingBox, LocalBoundingBox, GetModelTransform());

	return ModelBoundingBox;
}

const ZEAABBox& ZEModelMesh::GetWorldBoundingBox()
{

	ZEAABBox::Transform(WorldBoundingBox, GetLocalBoundingBox(), GetWorldTransform());

	return WorldBoundingBox;
}

const ZEMatrix4x4& ZEModelMesh::GetLocalTransform()
{
	ZEMatrix4x4::CreateOrientation(LocalTransform, Position, Rotation, Scale);

	return LocalTransform;
}

const ZEMatrix4x4& ZEModelMesh::GetModelTransform()
{
	ZEMatrix4x4::Multiply(ModelTransform, Owner->GetTransform(), GetLocalTransform());

	return ModelTransform;	
}

const ZEMatrix4x4& ZEModelMesh::GetWorldTransform()
{
	ZEMatrix4x4::Multiply(WorldTransform, Owner->GetWorldTransform(), GetLocalTransform());

	return WorldTransform;	
}
	
void ZEModelMesh::SetLocalPosition(const ZEVector3& LocalPosition)
{
	Position = LocalPosition;
}

const ZEVector3& ZEModelMesh::GetLocalPosition()
{
	return Position;
}

void ZEModelMesh::SetLocalRotation(const ZEQuaternion& LocalRotation)
{
	Rotation = LocalRotation;
}

const ZEQuaternion& ZEModelMesh::GetLocalRotation()
{
	return Rotation;
}

void ZEModelMesh::SetLocalScale(const ZEVector3& LocalScale)
{
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
	AnimationType = ZE_MAT_PREDEFINED;
	Position = MeshResource->Position;
	Rotation = MeshResource->Rotation;
	Scale = MeshResource->Scale;
	LocalBoundingBox = MeshResource->BoundingBox;
	PhysicsEnabled = false;

	ZEArray<ZEPhysicalShape*> ShapeList;

	if(PhysicalBody == NULL)
	{
		if (MeshResource->PhysicalBody.Type == ZE_MRPBT_RIGID)
		{
			PhysicalBody = ZEPhysicalRigidBody::CreateInstance();

			PhysicalBody->SetEnabled(MeshResource->PhysicalBody.Enabled);
			PhysicalBody->SetMass(MeshResource->PhysicalBody.Mass);
			PhysicalBody->SetLinearDamping(MeshResource->PhysicalBody.LinearDamping);
			PhysicalBody->SetAngularDamping(MeshResource->PhysicalBody.AngularDamping);
			PhysicalBody->SetPosition(Owner->GetWorldPosition());
			PhysicalBody->SetRotation(Owner->GetWorldRotation());
			PhysicalBody->SetMassCenterPosition(MeshResource->PhysicalBody.MassCenter);
			PhysicalBody->SetTransformChangeEvent(ZEPhysicalTransformChangeEvent(this->Owner, &ZEModel::TransformChangeEvent));

			for (ZESize I = 0; I < MeshResource->PhysicalBody.Shapes.GetCount(); I++)
			{
				const ZEModelResourcePhysicalShape* Shape = &MeshResource->PhysicalBody.Shapes[I];
				switch(Shape->Type)
				{
					case ZE_PST_BOX:
					{
						ZEPhysicalBoxShape* BoxShape = new ZEPhysicalBoxShape();
						BoxShape->SetWidth(Shape->Box.Width);
						BoxShape->SetHeight(Shape->Box.Height);
						BoxShape->SetLength(Shape->Box.Length);
						BoxShape->SetPosition(Shape->Position);
						BoxShape->SetRotation(Shape->Rotation);
						ShapeList.Add(BoxShape);
						PhysicalBody->AddPhysicalShape(BoxShape);
						break;
					}

					case ZE_PST_SPHERE:
					{
						ZEPhysicalSphereShape* SphereShape = new ZEPhysicalSphereShape();
						SphereShape->SetRadius(Shape->Sphere.Radius);
						SphereShape->SetPosition(Shape->Position);
						SphereShape->SetRotation(Shape->Rotation);
						ShapeList.Add(SphereShape);
						PhysicalBody->AddPhysicalShape(SphereShape);
						break;
					}
					case ZE_PST_CYLINDER:
					{
						// Problematic
						break;
					}

					case ZE_PST_CAPSULE:
					{
						ZEPhysicalCapsuleShape* CapsuleShape = new ZEPhysicalCapsuleShape();
						CapsuleShape->SetRadius(Shape->Capsule.Radius);
						CapsuleShape->SetHeight(Shape->Capsule.Height);
						CapsuleShape->SetPosition(Shape->Position);
						CapsuleShape->SetRotation(Shape->Rotation);
						ShapeList.Add(CapsuleShape);
						PhysicalBody->AddPhysicalShape(CapsuleShape);
						break;
					}

					case ZE_PST_CONVEX:
						// Problematic
						break;
				}
			}

			PhysicalBody->SetPhysicalWorld(zeScene->GetPhysicalWorld());
			PhysicalBody->Initialize();
		}
	}

	LODs.SetCount(MeshResource->LODs.GetCount());
	for (ZESize I = 0; I < MeshResource->LODs.GetCount(); I++)
		LODs[I].Initialize(Owner, this, &MeshResource->LODs[I]);

	for (ZESize I = 0; I < ShapeList.GetCount(); I++)
		delete ShapeList[I];
	ShapeList.Clear();


}

void ZEModelMesh::Deinitialize()
{
	Owner = NULL;
	LODs.Clear();
}

void ZEModelMesh::OnTransformChanged()
{
	if (PhysicalBody != NULL)
	{
		PhysicalBody->SetPosition(Owner->GetWorldPosition());
		PhysicalBody->SetRotation(Owner->GetWorldRotation());
	}
}

void ZEModelMesh::Draw(ZEDrawParameters* DrawParameters)
{
	if (!Visible)
		return;

	ZEVector3 WorldPosition;
	ZEMatrix4x4::Transform(WorldPosition, GetWorldTransform(), ZEVector3::Zero);
	float DistanceSquare = ZEVector3::DistanceSquare(DrawParameters->View->Camera->GetWorldPosition(), WorldPosition);

	ZEInt Lod = 0;
	/*ZEInt LastLod = LODs.GetCount() - 1;

	if (DistanceSquare > 40 * 40) 
		Lod = -1;
	else if (DistanceSquare > 30 * 30)
		Lod = 2;
	else if (DistanceSquare > 20 * 20)
		Lod = 1;
	else
		Lod = 0;

	if (Lod == -1)
		return;

	if (Lod > LastLod)
		Lod = LastLod;*/

	LODs[(ZESize)Lod].Draw(DrawParameters, DistanceSquare);
}


ZEModelMesh::ZEModelMesh()
{
	Owner = NULL;
	MeshResource = NULL;
	PhysicalBody = NULL;
	Visible = true;
	PhysicsEnabled = false;
	AutoLOD = false;
	ActiveLOD = 0;
	AnimationType = ZE_MAT_NOANIMATION;
}

ZEModelMesh::~ZEModelMesh()
{
	Deinitialize();
}
