//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Model.cpp
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

#include "Model.h"
#include "ModelFileFormat.h"
#include <stdio.h>
#include "Core/Core.h"
#include "Physics/PhysicsModule.h"
#include "Physics/PhysicsBodyInfo.h"
#include "Physics/PhysicsShapeInfo.h"
#include "Physics/PhysicsBoxShapeInfo.h"
#include "Physics/PhysicsCapsuleShapeInfo.h"
#include "Physics/PhysicsConvexShapeInfo.h"
#include "Physics/PhysicsPlaneShapeInfo.h"
#include "Physics/PhysicsSphereShapeInfo.h"
#include "Physics/PhysicsTrimeshShapeInfo.h"
#include "Physics/PhysicsMaterial.h"

void ZEModelMeshLOD::ResetMaterial()
{
	RenderList.Material = &Owner->GetModelResource()->Materials[LODResource->MaterialId];
}

void ZEModelMeshLOD::SetMaterial(const ZEMaterial* Material)
{
	RenderList.Material = Material;
}

const ZEMaterial* ZEModelMeshLOD::GetMaterial()
{
	return RenderList.Material;
}

bool ZEModelMeshLOD::IsSkinned()
{
	return Skinned;
}

void ZEModelMeshLOD::Draw(ZERenderer* Renderer, const ZESmartArray<const ZERLLight*>& Lights)
{
	if (VertexBuffer == NULL)
		return;

	if (Skinned)
	{
		const ZEArray<ZEMatrix4x4>& BoneVertexTransforms = Owner->GetBoneTransforms();
		ZEMatrix4x4 OwnerMeshModelTransform = OwnerMesh->GetModelTransform();

		RenderList.BoneTransforms.SetCount(BoneVertexTransforms.GetCount());
		for (size_t I = 0; I < RenderList.BoneTransforms.GetCount(); I++)
			ZEMatrix4x4::Multiply(RenderList.BoneTransforms[I], OwnerMeshModelTransform, BoneVertexTransforms[I]);

		RenderList.WorldMatrix = Owner->GetWorldTransform();
	}
	else
		RenderList.WorldMatrix = OwnerMesh->GetWorldTransform();

	RenderList.Lights.Clear();
	RenderList.Lights.MassAdd(Lights.GetConstCArray(), Lights.GetCount());
	Renderer->AddToRenderList(&RenderList);
}

void ZEModelMeshLOD::Initialize(ZEModel* Model, ZEModelMesh* Mesh,  const ZEModelResourceMeshLOD* LODResource)
{
	Owner = Model;
	OwnerMesh = Mesh;
	this->LODResource = LODResource;

	Skinned = LODResource->Vertices.GetCount() == 0 ? true : false;

	RenderList.SetZero();
	RenderList.Flags = ZE_RLF_ENABLE_VIEWPROJECTION_TRANSFORM | ZE_RLF_ENABLE_WORLD_TRANSFORM | ZE_RLF_ENABLE_ZCULLING | (Skinned ? ZE_RLF_SKINNED : 0);
	RenderList.PrimitiveType = ZE_RLPT_TRIANGLE;
	RenderList.VertexBuffer = VertexBuffer = LODResource->GetSharedVertexBuffer();
	RenderList.PrimitiveCount = Skinned ? LODResource->SkinnedVertices.GetCount() / 3: LODResource->Vertices.GetCount() / 3;
	RenderList.VertexType = Skinned ? ZE_VT_SKINNEDMODELVERTEX : ZE_VT_MODELVERTEX;
	RenderList.Material = &Owner->GetModelResource()->Materials[LODResource->MaterialId];
}

void ZEModelMeshLOD::Deinitialize()
{
	Owner = NULL;
	OwnerMesh = NULL;
	RenderList.SetZero();
	VertexBuffer = NULL;
	LODResource = NULL;
	Material = NULL;
	Skinned = false;
}

ZEModelMeshLOD::ZEModelMeshLOD()
{
	Skinned = false;
	Owner = NULL;
	OwnerMesh = NULL;
	RenderList.SetZero();
	VertexBuffer = NULL;
	LODResource = NULL;
	Material = NULL;
}

ZEModelMeshLOD::~ZEModelMeshLOD()
{
	Deinitialize();
}

// MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH 
// MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH 
// MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH MESH 

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
	Rotation = MeshResource->Orientation;
	Scale = ZEVector3(1.0f, 1.0f, 1.0f);
	LocalBoundingBox = MeshResource->BoundingBox;

	UpdateWorldBoundingBox = true;
	UpdateWorldTransform = true;
	UpdateModelBoundingBox = true;
	UpdateModelTransform = true;
	UpdateLocalTransform = true;

	LODs.SetCount(MeshResource->LODs.GetCount());
	for (size_t I = 0; I < MeshResource->LODs.GetCount(); I++)
		LODs[I].Initialize(Owner, this, &MeshResource->LODs[I]);

	//init phy
	ZEPhysicsBodyInfo BodyInfo;
	BodyInfo.Mass = MeshResource->PhysicalBody.Mass;
	BodyInfo.Position = MeshResource->PhysicalBody.Position;
	BodyInfo.Orientation = MeshResource->PhysicalBody.Orientation;
	BodyInfo.Kinematic = MeshResource->PhysicalBody.Kinematic;
	BodyInfo.LinearDamp = MeshResource->PhysicalBody.LinearDamp;
	BodyInfo.AngularDamp = MeshResource->PhysicalBody.AngularDamp;
	for (size_t j = 0; j < MeshResource->PhysicalBody.Shapes.GetCount(); j++)
	{
		ZEModelResourcePhysicalShape Shape = MeshResource->PhysicalBody.Shapes[j];
		switch (Shape.Type)
		{
			case ZE_PST_PLANE:
			{
				ZEPhysicsPlaneShapeInfo PlaneShapeInfo;
				PlaneShapeInfo.CollisionMask.Mask1 = Shape.Mask1;
				PlaneShapeInfo.CollisionMask.Mask2 = Shape.Mask2;
				PlaneShapeInfo.CollisionMask.Mask3 = Shape.Mask3;
				PlaneShapeInfo.CollisionMask.Mask4 = Shape.Mask4;
				//PlaneShapeInfo.Material->Initialize(Shape.Material.Friction,Shape.Material.Restitution);
				PlaneShapeInfo.LocalPosition = Shape.LocalPosition;
				PlaneShapeInfo.Trigger = Shape.Trigger;

				PlaneShapeInfo.Height = Shape.Plane.Height;
				PlaneShapeInfo.Normal.x = Shape.Plane.NormalX;
				PlaneShapeInfo.Normal.y = Shape.Plane.NormalY;
				PlaneShapeInfo.Normal.z = Shape.Plane.NormalZ;

				BodyInfo.ShapeInfos.Add(&PlaneShapeInfo);
				break;
			}
			case ZE_PST_BOX:
			{
				ZEPhysicsBoxShapeInfo BoxShapeInfo;
				BoxShapeInfo.CollisionMask.Mask1 = Shape.Mask1;
				BoxShapeInfo.CollisionMask.Mask2 = Shape.Mask2;
				BoxShapeInfo.CollisionMask.Mask3 = Shape.Mask3;
				BoxShapeInfo.CollisionMask.Mask4 = Shape.Mask4;
				//BoxShapeInfo.Material->Initialize(Shape.Material.Friction,Shape.Material.Restitution);
				BoxShapeInfo.LocalPosition = Shape.LocalPosition;
				BoxShapeInfo.Trigger = Shape.Trigger;

				BoxShapeInfo.Dimensions.x = Shape.Box.Width;
				BoxShapeInfo.Dimensions.y = Shape.Box.Height;
				BoxShapeInfo.Dimensions.z = Shape.Box.Length;
				BodyInfo.ShapeInfos.Add(&BoxShapeInfo);
				break;
			}
			case ZE_PST_SPHERE:
			{
				ZEPhysicsSphereShapeInfo SphereShapeInfo;
				SphereShapeInfo.CollisionMask.Mask1 = Shape.Mask1;
				SphereShapeInfo.CollisionMask.Mask2 = Shape.Mask2;
				SphereShapeInfo.CollisionMask.Mask3 = Shape.Mask3;
				SphereShapeInfo.CollisionMask.Mask4 = Shape.Mask4;
				//SphereShapeInfo.Material->Initialize(Shape.Material.Friction,Shape.Material.Restitution);
				SphereShapeInfo.LocalPosition = Shape.LocalPosition;
				SphereShapeInfo.Trigger = Shape.Trigger;

				SphereShapeInfo.Radius = Shape.Sphere.Radius;
				BodyInfo.ShapeInfos.Add(&SphereShapeInfo);
				break;
			}
			case ZE_PST_CAPSULE:
			{
				ZEPhysicsCapsuleShapeInfo CapsuleShapeInfo;
				CapsuleShapeInfo.CollisionMask.Mask1 = Shape.Mask1;
				CapsuleShapeInfo.CollisionMask.Mask2 = Shape.Mask2;
				CapsuleShapeInfo.CollisionMask.Mask3 = Shape.Mask3;
				CapsuleShapeInfo.CollisionMask.Mask4 = Shape.Mask4;
				//CapsuleShapeInfo.Material->Initialize(Shape.Material.Friction,Shape.Material.Restitution);
				CapsuleShapeInfo.LocalPosition = Shape.LocalPosition;
				CapsuleShapeInfo.Trigger = Shape.Trigger;

				CapsuleShapeInfo.Height = Shape.Capsule.Height;
				CapsuleShapeInfo.Radius = Shape.Capsule.Radius;
				BodyInfo.ShapeInfos.Add(&CapsuleShapeInfo);
				break;
			}
			case ZE_PST_CONVEX:
			{
				ZEPhysicsConvexShapeInfo ConvexShapeInfo;
				ConvexShapeInfo.CollisionMask.Mask1 = Shape.Mask1;
				ConvexShapeInfo.CollisionMask.Mask2 = Shape.Mask2;
				ConvexShapeInfo.CollisionMask.Mask3 = Shape.Mask3;
				ConvexShapeInfo.CollisionMask.Mask4 = Shape.Mask4;
				//ConvexShapeInfo.Material->Initialize(Shape.Material.Friction,Shape.Material.Restitution);
				ConvexShapeInfo.LocalPosition = Shape.LocalPosition;
				ConvexShapeInfo.Trigger = Shape.Trigger;

				ConvexShapeInfo.Vertices = Shape.Convex.Vertices;
				BodyInfo.ShapeInfos.Add(&ConvexShapeInfo);
				break;
			}
			case ZE_PST_TRIMESH:
			{
				ZEPhysicsTrimeshShapeInfo TrimeshShapeInfo;
				TrimeshShapeInfo.CollisionMask.Mask1 = Shape.Mask1;
				TrimeshShapeInfo.CollisionMask.Mask2 = Shape.Mask2;
				TrimeshShapeInfo.CollisionMask.Mask3 = Shape.Mask3;
				TrimeshShapeInfo.CollisionMask.Mask4 = Shape.Mask4;
				//TrimeshShapeInfo.Material->Initialize(Shape.Material.Friction,Shape.Material.Restitution);
				TrimeshShapeInfo.LocalPosition = Shape.LocalPosition;
				TrimeshShapeInfo.Trigger = Shape.Trigger;

				TrimeshShapeInfo.Vertices = Shape.Trimesh.Vertices;
				for (int i=0;i<Shape.Trimesh.Indices.GetCount();i++)
				{
					TrimeshShapeInfo.Indexes.Add(Shape.Trimesh.Indices[i].VertexIndexes[0]);
					TrimeshShapeInfo.Indexes.Add(Shape.Trimesh.Indices[i].VertexIndexes[1]);
					TrimeshShapeInfo.Indexes.Add(Shape.Trimesh.Indices[i].VertexIndexes[2]);
				}
				BodyInfo.ShapeInfos.Add(&TrimeshShapeInfo);
				break;
			}
		}
	}
	PhysicalBody = zePhysics->CreateBody();
	PhysicalBody->Initialize(BodyInfo);
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

void ZEModelMesh::Draw(ZERenderer* Renderer, const ZESmartArray<const ZERLLight*>& Lights)
{
	LODs[0].Draw(Renderer, Lights);	
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

// BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE
// BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE
// BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE BONE

const char* ZEModelBone::GetName()
{
	return BoneResource->Name;
}

const ZEModelBone* ZEModelBone::GetParentBone()
{
	return ParentBone;
}

const ZEArray<ZEModelBone*> ZEModelBone::GetChildBones()
{
	return ChildBones;
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
	if (UpdateLocalTransform)
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
	if (UpdateWorldTransform)
	{
		ZEMatrix4x4::Multiply(WorldTransform, GetLocalTransform(), Owner->GetWorldTransform());
		UpdateWorldTransform = false;
	}

	return WorldTransform;
}

const ZEMatrix4x4& ZEModelBone::GetModelTransform()
{
	if (UpdateWorldTransform)
	{
		ZEMatrix4x4::Multiply(WorldTransform, GetLocalTransform(), Owner->GetLocalTransform());
		UpdateWorldTransform = false;
	}

	return WorldTransform;
}

const ZEMatrix4x4& ZEModelBone::GetVertexTransform()
{
	if (UpdateVertexTransform)
	{
		ZEMatrix4x4::Multiply(VertexTransform, BoneResource->InverseTransform, GetModelTransform());
		//VertexTransform = GetModelTransform();
		//ZEMatrix4x4::CreateIdentity(VertexTransform);
		UpdateVertexTransform = false;
	}

	return VertexTransform;
}

const ZEMatrix4x4& ZEModelBone::GetRelativeTransform()
{
	if (UpdateRelativeTransform)
	{
		ZEMatrix4x4 Temp1, Temp2;
		ZEMatrix4x4::CreateTranslation(Temp1, RelativePosition);
		ZEMatrix4x4::CreateRotation(Temp2, RelativeRotation);
		ZEMatrix4x4::Multiply(RelativeTransform, Temp2, Temp1);

		ZEMatrix4x4::CreateOffset(Temp1, RelativePosition, RelativeRotation);
		UpdateRelativeTransform = false;
	}
	return RelativeTransform;
}

const ZEVector3& ZEModelBone::GetAbsolutePostion()
{
	return BoneResource->AbsolutePosition;
}

const ZEQuaternion& ZEModelBone::GetAbsoluteRotation()
{
	return BoneResource->AbsoluteOrientation;
}

const ZEVector3& ZEModelBone::GetRelativePosition()
{
	return RelativePosition;
}

void ZEModelBone::SetRelativePosition(const ZEPosition3& Position)
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
	RelativeRotation = BoneResource->RelativeOrientation;

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

// MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL
// MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL
// MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL MODEL
void ZEModel::CalculateBoundingBox()
{
	bool NotInitialized = true;

	for (size_t I = 0; I < Meshes.GetCount(); I++)
	{
		const ZEAABoundingBox& CurrentBoundingBox = Meshes[I].GetModelBoundingBox();

		if (NotInitialized)
		{
			BoundingBox.Min = BoundingBox.Max = CurrentBoundingBox.GetVertex(0);
			NotInitialized = false;
		}

		for (int N = 0; N < 8; N++)
		{
			ZEVector3 Point = CurrentBoundingBox.GetVertex(N);
			if (Point.x < BoundingBox.Min.x) BoundingBox.Min.x = Point.x;
			if (Point.y < BoundingBox.Min.y) BoundingBox.Min.y = Point.y;
			if (Point.z < BoundingBox.Min.z) BoundingBox.Min.z = Point.z;

			if (Point.x > BoundingBox.Max.x) BoundingBox.Max.x = Point.x;
			if (Point.y > BoundingBox.Max.y) BoundingBox.Max.y = Point.y;
			if (Point.z > BoundingBox.Max.z) BoundingBox.Max.z = Point.z;
		}

	}
/*
	for (size_t I = 0; I < Bones.GetCount(); I++)
	{
		const ZEAABoundingBox& CurrentBoundingBox = Bones[I].GetModelBoundingBox();

		if (NotInitialized)
		{
			BoundingBox.Min = BoundingBox.Max = CurrentBoundingBox.GetVertex(0);
			NotInitialized = false;
		}

		for (int N = 0; N < 8; N++)
		{
			ZEVector3 Point = CurrentBoundingBox.GetVertex(N);
			if (Point.x < BoundingBox.Min.x) BoundingBox.Min.x = Point.x;
			if (Point.y < BoundingBox.Min.y) BoundingBox.Min.y = Point.y;
			if (Point.z < BoundingBox.Min.z) BoundingBox.Min.z = Point.z;

			if (Point.x > BoundingBox.Max.x) BoundingBox.Max.x = Point.x;
			if (Point.y > BoundingBox.Max.y) BoundingBox.Max.y = Point.y;
			if (Point.z > BoundingBox.Max.z) BoundingBox.Max.z = Point.z;
		}

	}*/

	if (NotInitialized )
		BoundingBox.Max = BoundingBox.Max = ZEPoint3(0.0f, 0.0f, 0.0f);
}

void ZEModel::LocalTransformChanged()
{
	for (size_t I = 0; I < Meshes.GetCount(); I++)
		Meshes[I].ModelTransformChanged();

	for (size_t I = 0; I < Bones.GetCount(); I++)
		Bones[I].ModelTransformChanged();

}

bool ZEModel::IsDrawable()
{
	return true;
}

void ZEModel::SetModelResource(const ZEModelResource* ModelResource)
{
	if (this->ModelResource != NULL)
		this->ModelResource->Release();
	this->ModelResource = ModelResource;

	if (ModelResource == NULL)
		return;

	Meshes.SetCount(ModelResource->Meshes.GetCount());
	for (size_t I = 0; I < ModelResource->Meshes.GetCount(); I++)
	{
		Meshes[I].Initialize(this, &ModelResource->Meshes[I]);
	}

	BoneTransforms.SetCount(ModelResource->Bones.GetCount());
	Bones.SetCount(ModelResource->Bones.GetCount());
	for (size_t I = 0; I < ModelResource->Bones.GetCount(); I++)
		Bones[I].Initialize(this, &ModelResource->Bones[I]);

	for (size_t I = 0; I < ModelResource->Bones.GetCount(); I++)
	{
		if (ModelResource->Bones[I].ParentBone != -1)
			Bones[ModelResource->Bones[I].ParentBone].AddChild(&Bones[I]);
	}
	UpdateBoundingBox();
	UpdateBoneTransforms();
}

const ZEModelResource* ZEModel::GetModelResource()
{
	return ModelResource;
}


const ZEArray<ZEModelBone>& ZEModel::GetSkeleton()
{
	return Skeleton;
}

ZEArray<ZEModelBone>& ZEModel::GetBones()
{
	return Bones;
}

const ZEArray<ZEMatrix4x4>& ZEModel::GetBoneTransforms()
{
	if (UpdateBoneTransforms_)
	{
		for (size_t I = 0; I < Bones.GetCount(); I++)
			BoneTransforms[I] = Bones[I].GetVertexTransform();
		UpdateBoneTransforms_ = false;
	}

	return BoneTransforms;
}

const ZEArray<ZEModelMesh>& ZEModel::GetMeshes()
{
	return Meshes;
}

const ZEArray<ZEModelAnimation>* ZEModel::GetAnimations()
{
	if (ModelResource == NULL)
		return  NULL;
	else 
		return &ModelResource->Animations;
}

void ZEModel::SetActiveLOD(unsigned int LOD)
{
	ActiveLOD = LOD;
	for (size_t I = 0; I < Meshes.GetCount(); I++)
		Meshes[I].SetActiveLOD(LOD);
}

int ZEModel::GetActiveLOD()
{
	return ActiveLOD;
}

void ZEModel::SetPhysicsEnabled(bool Enabled)
{
	PhysicsEnabled = Enabled;
	for (size_t I = 0; I < Bones.GetCount(); I++)
		Bones[I].SetPhysicsEnabled(Enabled);
	for (size_t I = 0; I < Meshes.GetCount(); I++)
		Meshes[I].SetPhysicsEnabled(Enabled);
}

bool ZEModel::GetPhysicsEnabled()
{
	return PhysicsEnabled;
}

void ZEModel::SetAnimationState(ZEModelAnimationState State)
{
	if (Animation == NULL)
		AnimationState = ZE_MAS_STOPPED;
	else
		AnimationState = State;
}

ZEModelAnimationState ZEModel::GetAnimationState()
{
	return AnimationState;
}

void ZEModel::SetAnimation(const ZEModelAnimation* Animation)
{
	this->Animation = Animation;
}

void ZEModel::SetAnimationById(size_t AnimationIndex)
{
	Animation = NULL;

	if (ModelResource == NULL)
		return;

	Animation = &ModelResource->Animations[AnimationIndex];
}

void ZEModel::SetAnimationByName(const char* AnimationName)
{
	Animation = NULL;

	if (ModelResource == NULL)
		return;

	for (size_t I = 0; I < ModelResource->Animations.GetCount(); I++)
		if (strnicmp(Animation->Name, ModelResource->Animations[I].Name, ZE_MDLF_MAX_NAME_SIZE) == 0)
		{
			Animation = &ModelResource->Animations[I];
			return;
		}
}

const ZEModelAnimation* ZEModel::GetAnimation()
{
	return Animation;
}

void ZEModel::SetAnimationFrame(unsigned int Frame)
{
	AnimationFrame = Frame;
}

size_t ZEModel::GetAnimationFrame()
{
	return AnimationFrame;
}

void ZEModel::SetAnimationType(ZEModelAnimationType AnimationType)
{
	this->AnimationType = AnimationType;
	for (size_t I = 0; I < Meshes.GetCount(); I++)
		Meshes[I].SetAnimationType(AnimationType);
	for (size_t I = 0; I < Bones.GetCount(); I++)
		Bones[I].SetAnimationType(AnimationType);
}

ZEModelAnimationType ZEModel::GetAnimationType()
{
	return AnimationType;
}

void ZEModel::SetAnimationSpeed(float Factor)
{
	AnimationSpeed = Factor;
}

float ZEModel::GetAnimationSpeed()
{
	return AnimationSpeed;
}

void ZEModel::SetAutoLOD(bool Enabled)
{
	AutoLOD = Enabled;
	for (size_t I = 0; I < Meshes.GetCount(); I++)
		Meshes[I].SetAutoLOD(Enabled);
}

bool ZEModel::GetAutoLOD()
{
	return AutoLOD;
}

void ZEModel::SetAnimationLooping(bool Looping)
{
	AnimationLooping = Looping;
}

bool ZEModel::GetAnimationLooping()
{
	return AnimationLooping;
}

void ZEModel::SetStaticPoseByName(const char* AnimationName, unsigned int Frame)
{
	Animation = NULL;
	AnimationState = ZE_MAS_STOPPED;

	if (ModelResource == NULL)
		return;

	for (size_t I = 0; I < ModelResource->Animations.GetCount(); I++)
		if (strnicmp(Animation->Name, ModelResource->Animations[I].Name, ZE_MDLF_MAX_NAME_SIZE) == 0)
		{
			SetStaticPose(&ModelResource->Animations[I], Frame);
			return;
		}
}

void ZEModel::SetStaticPoseByIndex(size_t AnimationIndex, unsigned int Frame)
{
	Animation = NULL;
	AnimationState = ZE_MAS_STOPPED;

	if (ModelResource == NULL)
		return;
	
	SetStaticPose(&ModelResource->Animations[AnimationIndex], Frame);
}


void ZEModel::SetStaticPose(const ZEModelAnimation* Animation, unsigned int Frame)
{
	if (Animation == NULL)
		AnimationState = ZE_MAS_STOPPED;
	else
	{
		size_t FrameId = Frame % Animation->Frames.GetCount();

		const ZEModelResourceAnimationFrame* Frame = &Animation->Frames[FrameId];

		const ZEModelResourceAnimationKey* Key, NextKey;
		for (size_t I = 0; I < Frame->BoneKeys.GetCount(); I++)
		{
			Key = &Frame->BoneKeys[I];

			Bones[Key->ItemId].SetRelativeRotation(Key->Orientation);
			//Bones[Key->ItemId].SetRelativeRotation(Key->Orientation);
		}
		
		for (size_t I = 0; I < Frame->MeshKeys.GetCount(); I++)
		{
			Key = &Frame->MeshKeys[I];
			
			Meshes[Key->ItemId].SetLocalPosition(Key->Position);
			Bones[Key->ItemId].SetRelativeRotation(Key->Orientation);
		}
	}
}

void ZEModel::PlayAnimationByName(const char* AnimationName, unsigned int StartFrame, unsigned int EndFrame)
{
	Animation = NULL;
	AnimationState = ZE_MAS_STOPPED;

	if (ModelResource == NULL)
		return;

	for (size_t I = 0; I < ModelResource->Animations.GetCount(); I++)
		if (strnicmp(Animation->Name, ModelResource->Animations[I].Name, ZE_MDLF_MAX_NAME_SIZE) == 0)
		{
			PlayAnimation(&ModelResource->Animations[I], StartFrame, EndFrame);
			return;
		}
}

void ZEModel::PlayAnimationByIndex(size_t AnimationIndex, unsigned int StartFrame, unsigned int EndFrame)
{
	Animation = NULL;
	AnimationState = ZE_MAS_STOPPED;

	if (ModelResource == NULL)
		return;
	
	PlayAnimation(&ModelResource->Animations[AnimationIndex], StartFrame, EndFrame);
}

void ZEModel::PlayAnimation(const ZEModelAnimation* Animation, unsigned int StartFrame, unsigned int EndFrame)
{
	this->Animation = Animation;

	if (Animation == NULL)
	{
		AnimationState = ZE_MAS_STOPPED;
		return;
	}

	if (StartFrame > Animation->Frames.GetCount())
	{
		AnimationStartFrame = Animation->Frames.GetCount() - 1;
		AnimationFrame = AnimationStartFrame;
	}
	else
	{
		AnimationFrame = StartFrame;
		AnimationStartFrame = StartFrame;
	}

	if (AnimationEndFrame > Animation->Frames.GetCount())
		AnimationEndFrame = Animation->Frames.GetCount() - 1;
	else
		AnimationEndFrame = (EndFrame == 0 ? Animation->Frames.GetCount() - 1 : EndFrame);
	
	AnimationState = ZE_MAS_PLAYING;
}

void ZEModel::ResumeAnimation()
{
	AnimationState = ZE_MAS_PLAYING;
}

void ZEModel::PauseAnimation()
{
	AnimationState = ZE_MAS_PAUSED;
}

void ZEModel::StopAnimation()
{
	AnimationState = ZE_MAS_STOPPED;
}

const ZEAABoundingBox& ZEModel::GetLocalBoundingBox()
{
	if (UpdateBoundingBox_)
	{
		CalculateBoundingBox();
		UpdateBoundingBox_ = false;
	}
	return BoundingBox;
}

void ZEModel::SetLocalPosition(const ZEVector3& NewPosition)
{
	ZEComponent::SetLocalPosition(NewPosition);
	LocalTransformChanged();
}

void ZEModel::SetLocalRotation(const ZEQuaternion& NewRotation)
{
	ZEComponent::SetLocalRotation(NewRotation);
	LocalTransformChanged();
}

void ZEModel::SetLocalScale(const ZEVector3& NewScale)
{
	ZEComponent::SetLocalScale(NewScale);
	LocalTransformChanged();
}

void ZEModel::OwnerWorldTransformChanged()
{
	ZEComponent::OwnerWorldTransformChanged();
	for (size_t I = 0; I < Meshes.GetCount(); I++)
		Meshes[I].ModelWorldTransformChanged();

	for (size_t I = 0; I < Bones.GetCount(); I++)
		Bones[I].ModelWorldTransformChanged();
}

void ZEModel::UpdateBoundingBox()
{
	UpdateBoundingBox_ = true;
	UpdateBoundingVolumes();
	GetOwner()->UpdateBoundingVolumes();
}

void ZEModel::UpdateBoneTransforms()
{
	UpdateBoneTransforms_ = true;
}

void ZEModel::Draw(ZERenderer* Renderer, const ZESmartArray<const ZERLLight*>& Lights)
{
	for (size_t I = 0; I < Meshes.GetCount(); I++)
		Meshes[I].Draw(Renderer, Lights);

	SkeletonPointsVertexBuffer.Clean();
	SkeletonVertexBuffer.Clean();
	if (DrawSkeleton)
	{
		ZEVector3 BonePosition1, BonePosition2;
		for (size_t I = 0; I < Bones.GetCount(); I++)
		{
			ZEMatrix4x4::Transform(BonePosition1, Bones[I].GetModelTransform(), ZEVector3(0.0f, 0.0f, 0.0f));
			SkeletonPointsVertexBuffer.AddPoint(BonePosition1);
			if (Bones[I].GetChildBones().GetCount() != 0)	
			{
				ZEMatrix4x4::Transform(BonePosition2, Bones[I].GetModelTransform(), Bones[I].GetChildBones()[0]->GetRelativePosition());
				SkeletonVertexBuffer.AddLine(BonePosition1, BonePosition2);
			}
		}
		Renderer->AddToRenderList(&SkeletonPointsRenderList);
		Renderer->AddToRenderList(&SkeletonRenderList);
	}

	if (DrawPhysicalBodies)
	{
		/*for (size_t I = 0; I < Meshes.GetCount(); I++)
		{
			if (Meshes[I].PhysicsEnabled)
			{
				switch(Meshes[I].PhysicalBody.
			}
		}*/
	}

	if (DrawPhysicalJoints)
	{
	}
}

void ZEModel::Tick(float ElapsedTime)
{
	if (AnimationState == ZE_MAS_PLAYING)
	{
		if (AnimationFrame > AnimationEndFrame)
			if (AnimationLooping)
				AnimationFrame = AnimationStartFrame + fmodf(AnimationFrame, AnimationEndFrame - AnimationStartFrame);
			else
			{
				AnimationFrame = AnimationEndFrame;
				AnimationState = ZE_MAS_STOPPED;
			}
			
		float Interpolation = AnimationFrame - floorf(AnimationFrame);

		const ZEModelResourceAnimationFrame* Frame = &Animation->Frames[(int)floorf(AnimationFrame)];
		const ZEModelResourceAnimationFrame* NextFrame = &Animation->Frames[(int)ceilf(AnimationFrame)];

		for (size_t I = 0; I < Frame->BoneKeys.GetCount(); I++)
		{
			const ZEModelResourceAnimationKey* Key = &Frame->BoneKeys[I];
			const ZEModelResourceAnimationKey* NextKey = &NextFrame->BoneKeys[I];
			
			ZEQuaternion Rotation;
			ZEQuaternion::Slerp(Rotation, Key->Orientation, NextKey->Orientation, Interpolation);

			Bones[Key->ItemId].SetRelativeRotation(Rotation);
			//Bones[Key->ItemId].SetRelativeRotation(Key->Orientation);
		}
		
		for (size_t I = 0; I < Frame->MeshKeys.GetCount(); I++)
		{
			const ZEModelResourceAnimationKey* Key = &Frame->MeshKeys[I];
			const ZEModelResourceAnimationKey* NextKey = &NextFrame->MeshKeys[I];
			
			ZEVector3 Position;
			ZEVector3::Lerp(Position, Key->Position, NextKey->Position, Interpolation);
			Meshes[Key->ItemId].SetLocalPosition(Position);

			ZEQuaternion Rotation;
			ZEQuaternion::Slerp(Rotation, Key->Orientation, NextKey->Orientation, Interpolation);
			Bones[Key->ItemId].SetRelativeRotation(Rotation);

		}
		AnimationFrame += AnimationSpeed * ElapsedTime;
	}
}


ZEModel::ZEModel()
{
	ModelResource = NULL;
	AnimationSpeed = 1.0f;
	Visibility = true;
	AutoLOD = true;
	ActiveLOD = 0;
	UpdateBoundingBox_ = true;
	AnimationState = ZE_MAS_STOPPED;
	Animation = NULL;	
	AnimationFrame = 0;
	AnimationLooping = false;
}

ZEModel::~ZEModel()
{
	if (ModelResource != NULL)
		ModelResource->Release();
}
