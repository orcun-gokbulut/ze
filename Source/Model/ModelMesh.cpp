//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ModelMesh.cpp
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

#include "ModelMesh.h"
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

	//init phy
	ZEPhysicsBodyInfo BodyInfo;
	BodyInfo.Mass = MeshResource->PhysicalBody.Mass;
	BodyInfo.Position = MeshResource->PhysicalBody.Position;
	BodyInfo.Orientation = MeshResource->PhysicalBody.Orientation;
	BodyInfo.Kinematic = MeshResource->PhysicalBody.Kinematic;
	BodyInfo.LinearDamp = MeshResource->PhysicalBody.LinearDamping;
	BodyInfo.AngularDamp = MeshResource->PhysicalBody.AngularDamping;
	
	//have local copy
	ZEPhysicsPlaneShapeInfo PlaneShapeInfo;
	ZEPhysicsBoxShapeInfo BoxShapeInfo;
	ZEPhysicsSphereShapeInfo SphereShapeInfo;
	ZEPhysicsCapsuleShapeInfo CapsuleShapeInfo;
	ZEPhysicsConvexShapeInfo ConvexShapeInfo;
	ZEPhysicsTrimeshShapeInfo TrimeshShapeInfo;

	for (size_t j = 0; j < MeshResource->PhysicalBody.Shapes.GetCount(); j++)
	{
		ZEModelResourcePhysicalShape Shape = MeshResource->PhysicalBody.Shapes[j];
		switch (Shape.Type)
		{
			case ZE_PST_PLANE:
			{
				PlaneShapeInfo.CollisionMask.Mask1 = Shape.CollisionMask1;
				PlaneShapeInfo.CollisionMask.Mask2 = Shape.CollisionMask2;
				PlaneShapeInfo.CollisionMask.Mask3 = Shape.CollisionMask3;
				PlaneShapeInfo.CollisionMask.Mask4 = Shape.CollisionMask4;
				//PlaneShapeInfo.Material->Initialize(Shape.Material.Friction,Shape.Material.Restitution);
				PlaneShapeInfo.LocalPosition = Shape.Position;
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
				BoxShapeInfo.CollisionMask.Mask1 = Shape.CollisionMask1;
				BoxShapeInfo.CollisionMask.Mask2 = Shape.CollisionMask2;
				BoxShapeInfo.CollisionMask.Mask3 = Shape.CollisionMask3;
				BoxShapeInfo.CollisionMask.Mask4 = Shape.CollisionMask4;
				//BoxShapeInfo.Material->Initialize(Shape.Material.Friction,Shape.Material.Restitution);
				BoxShapeInfo.LocalPosition = Shape.Position;
				BoxShapeInfo.Trigger = Shape.Trigger;

				BoxShapeInfo.Dimensions.x = Shape.Box.Width;
				BoxShapeInfo.Dimensions.y = Shape.Box.Height;
				BoxShapeInfo.Dimensions.z = Shape.Box.Length;
				BodyInfo.ShapeInfos.Add(&BoxShapeInfo);
				break;
			}
			case ZE_PST_SPHERE:
			{
				SphereShapeInfo.CollisionMask.Mask1 = Shape.CollisionMask1;
				SphereShapeInfo.CollisionMask.Mask2 = Shape.CollisionMask2;
				SphereShapeInfo.CollisionMask.Mask3 = Shape.CollisionMask3;
				SphereShapeInfo.CollisionMask.Mask4 = Shape.CollisionMask4;
				//SphereShapeInfo.Material->Initialize(Shape.Material.Friction,Shape.Material.Restitution);
				SphereShapeInfo.LocalPosition = Shape.Position;
				SphereShapeInfo.Trigger = Shape.Trigger;

				SphereShapeInfo.Radius = Shape.Sphere.Radius;
				BodyInfo.ShapeInfos.Add(&SphereShapeInfo);
				break;
			}
			case ZE_PST_CAPSULE:
			{
				CapsuleShapeInfo.CollisionMask.Mask1 = Shape.CollisionMask1;
				CapsuleShapeInfo.CollisionMask.Mask2 = Shape.CollisionMask2;
				CapsuleShapeInfo.CollisionMask.Mask3 = Shape.CollisionMask3;
				CapsuleShapeInfo.CollisionMask.Mask4 = Shape.CollisionMask4;
				//CapsuleShapeInfo.Material->Initialize(Shape.Material.Friction,Shape.Material.Restitution);
				CapsuleShapeInfo.LocalPosition = Shape.Position;
				CapsuleShapeInfo.Trigger = Shape.Trigger;

				CapsuleShapeInfo.Height = Shape.Capsule.Height;
				CapsuleShapeInfo.Radius = Shape.Capsule.Radius;
				BodyInfo.ShapeInfos.Add(&CapsuleShapeInfo);
				break;
			}
			case ZE_PST_CONVEX:
			{
				ConvexShapeInfo.CollisionMask.Mask1 = Shape.CollisionMask1;
				ConvexShapeInfo.CollisionMask.Mask2 = Shape.CollisionMask2;
				ConvexShapeInfo.CollisionMask.Mask3 = Shape.CollisionMask3;
				ConvexShapeInfo.CollisionMask.Mask4 = Shape.CollisionMask4;
				//ConvexShapeInfo.Material->Initialize(Shape.Material.Friction,Shape.Material.Restitution);
				ConvexShapeInfo.LocalPosition = Shape.Position;
				ConvexShapeInfo.Trigger = Shape.Trigger;

				ConvexShapeInfo.Vertices = Shape.Convex.Vertices;
				BodyInfo.ShapeInfos.Add(&ConvexShapeInfo);
				break;
			}
			case ZE_PST_TRIMESH:
			{
				TrimeshShapeInfo.CollisionMask.Mask1 = Shape.CollisionMask1;
				TrimeshShapeInfo.CollisionMask.Mask2 = Shape.CollisionMask2;
				TrimeshShapeInfo.CollisionMask.Mask3 = Shape.CollisionMask3;
				TrimeshShapeInfo.CollisionMask.Mask4 = Shape.CollisionMask4;
				//TrimeshShapeInfo.Material->Initialize(Shape.Material.Friction,Shape.Material.Restitution);
				TrimeshShapeInfo.LocalPosition = Shape.Position;
				TrimeshShapeInfo.Trigger = Shape.Trigger;

				TrimeshShapeInfo.Vertices = Shape.TriMesh.Vertices;
				for (int i=0;i<Shape.TriMesh.Indices.GetCount();i++)
				{
					TrimeshShapeInfo.Indexes.Add(Shape.TriMesh.Indices[i].VertexIndexes[0]);
					TrimeshShapeInfo.Indexes.Add(Shape.TriMesh.Indices[i].VertexIndexes[1]);
					TrimeshShapeInfo.Indexes.Add(Shape.TriMesh.Indices[i].VertexIndexes[2]);
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
