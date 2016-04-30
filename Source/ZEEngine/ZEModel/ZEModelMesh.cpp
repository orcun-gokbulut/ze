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

#include "ZEMath/ZETriangle.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEViewVolume.h"

#include "ZEModel.h"
#include "ZEModelMeshLOD.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZERenderer/ZERNView.h"
#include "ZERenderer/ZECamera.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNMaterial.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZEPhysics/ZEPhysicalCloth.h"

#define ZEMD_MDF_LOCAL_TRANSFORM			0x0001
#define ZEMD_MDF_INV_LOCAL_TRANSFORM		0x0002
#define ZEMD_MDF_MODEL_TRANSFORM			0x0004
#define ZEMD_MDF_INV_MODEL_TRANSFORM		0x0008
#define ZEMD_MDF_WORLD_TRANSFORM			0x0010
#define ZEMD_MDF_INV_WORLD_TRANSFORM		0x0020
#define ZEMD_MDF_MODEL_BOUNDING_BOX			0x0040
#define ZEMD_MDF_WORLD_BOUNDING_BOX			0x0080
#define ZEMD_MDF_CONSTANT_BUFFER			0x0100

struct ZEModelMeshConstants
{
	ZEMatrix4x4						WorldTransform;
	ZEMatrix4x4						PreSkinTransform;
	ZEVector4						ClippingPlane0;
	ZEVector4						ClippingPlane1;
	ZEVector4						ClippingPlane2;
	ZEVector4						ClippingPlane3;
};

void ZEModelMesh::UpdateConstantBuffer()
{
	if (!DirtyFlags.GetFlags(ZEMD_MDF_CONSTANT_BUFFER))
		return;

	ZEModelMeshConstants Constants;

	Constants.WorldTransform = GetWorldTransform();
	Constants.PreSkinTransform = GetModelTransform();

	if (ClippingPlanes.GetCount() > 0)
		Constants.ClippingPlane0 = GetInvWorldTransform().Transpose() * ClippingPlanes[0].ToABCD();
	else
		Constants.ClippingPlane0 = ZEVector4::Zero;

	if (ClippingPlanes.GetCount() > 1)
		Constants.ClippingPlane1 = GetWorldTransform() * ClippingPlanes[1].ToABCD();
	else
		Constants.ClippingPlane1 = ZEVector4::Zero;

	if (ClippingPlanes.GetCount() > 2)
		Constants.ClippingPlane2 = GetWorldTransform() * ClippingPlanes[2].ToABCD();
	else
		Constants.ClippingPlane2 = ZEVector4::Zero;

	if (ClippingPlanes.GetCount() > 3)
		Constants.ClippingPlane3 = GetWorldTransform() * ClippingPlanes[3].ToABCD();
	else
		Constants.ClippingPlane3 = ZEVector4::Zero;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZEMD_MDF_CONSTANT_BUFFER);
}

void ZEModelMesh::LocalTransformChanged()
{
	DirtyFlags.RaiseFlags(
		ZEMD_MDF_LOCAL_TRANSFORM | ZEMD_MDF_INV_LOCAL_TRANSFORM |
		ZEMD_MDF_MODEL_TRANSFORM | ZEMD_MDF_INV_MODEL_TRANSFORM |
		ZEMD_MDF_WORLD_TRANSFORM | ZEMD_MDF_INV_WORLD_TRANSFORM |
		ZEMD_MDF_MODEL_BOUNDING_BOX | ZEMD_MDF_WORLD_BOUNDING_BOX |
		ZEMD_MDF_CONSTANT_BUFFER);

	for (ZESize I = 0; I < ChildMeshes.GetCount(); I++)
		ChildMeshes[I]->ParentTransformChanged();

	if (Owner != NULL)
		Owner->ChildBoundingBoxChanged();
}

void ZEModelMesh::ParentTransformChanged()
{
	if (PhysicalBody != NULL)
	{
		PhysicalBody->SetPosition(Owner->GetWorldPosition());
		PhysicalBody->SetRotation(Owner->GetWorldRotation());
	}

	DirtyFlags.RaiseFlags(
		ZEMD_MDF_MODEL_TRANSFORM | ZEMD_MDF_INV_MODEL_TRANSFORM |
		ZEMD_MDF_WORLD_TRANSFORM | ZEMD_MDF_INV_WORLD_TRANSFORM |
		ZEMD_MDF_MODEL_BOUNDING_BOX | ZEMD_MDF_WORLD_BOUNDING_BOX |
		ZEMD_MDF_CONSTANT_BUFFER);

	for (ZESize I = 0; I < ChildMeshes.GetCount(); I++)
		ChildMeshes[I]->ParentTransformChanged();
}

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

ZEArray<ZEModelMeshLOD>& ZEModelMesh::GetLODs()
{
	return LODs;
}

void ZEModelMesh::SetVisible(bool Visible)
{
	this->Visible = Visible;
}

bool ZEModelMesh::GetVisible()
{
	return Visible;
}

ZEModelMesh* ZEModelMesh::GetParentMesh()
{
	return ParentMesh;
}

const ZEArray<ZEModelMesh*>& ZEModelMesh::GetChildMeshes()
{
	return ChildMeshes;
}

const char* ZEModelMesh::GetName()
{
	return MeshResource->Name;
}

ZEPhysicalRigidBody* ZEModelMesh::GetPhysicalBody()
{
	return PhysicalBody;
}

ZEPhysicalCloth* ZEModelMesh::GetPhysicalCloth()
{
	return PhysicalCloth;
}

const ZEAABBox& ZEModelMesh::GetLocalBoundingBox() const
{
	return LocalBoundingBox;
}

const ZEAABBox& ZEModelMesh::GetModelBoundingBox() const
{
	if (DirtyFlags.GetFlags(ZEMD_MDF_MODEL_BOUNDING_BOX))
	{
		ZEAABBox::Transform(ModelBoundingBox, LocalBoundingBox, GetModelTransform());
		DirtyFlags.UnraiseFlags(ZEMD_MDF_MODEL_BOUNDING_BOX);
	}

	return ModelBoundingBox;
}

const ZEAABBox& ZEModelMesh::GetWorldBoundingBox() const
{
	if (DirtyFlags.GetFlags(ZEMD_MDF_WORLD_BOUNDING_BOX))
	{
		ZEAABBox::Transform(WorldBoundingBox, LocalBoundingBox, GetWorldTransform());
		DirtyFlags.UnraiseFlags(ZEMD_MDF_WORLD_BOUNDING_BOX);
	}

	return WorldBoundingBox;
}

const ZEMatrix4x4& ZEModelMesh::GetLocalTransform() const
{
	if (DirtyFlags.GetFlags(ZEMD_MDF_LOCAL_TRANSFORM))
	{
		ZEMatrix4x4::CreateOrientation(LocalTransform, Position, Rotation, Scale);
		DirtyFlags.UnraiseFlags(ZEMD_MDF_LOCAL_TRANSFORM);
	}

	return LocalTransform;
}

const ZEMatrix4x4& ZEModelMesh::GetModelTransform() const
{
	if (DirtyFlags.GetFlags(ZEMD_MDF_MODEL_TRANSFORM))
	{
		if (ParentMesh == NULL)
			return GetLocalTransform();
		else
			ZEMatrix4x4::Multiply(ModelTransform, ParentMesh->GetModelTransform(), GetLocalTransform());

		DirtyFlags.UnraiseFlags(ZEMD_MDF_MODEL_TRANSFORM);
	}
	
	return ModelTransform;
}

const ZEMatrix4x4& ZEModelMesh::GetWorldTransform() const
{
	if (DirtyFlags.GetFlags(ZEMD_MDF_WORLD_TRANSFORM))
	{
		if (Owner != NULL)
			ZEMatrix4x4::Multiply(WorldTransform, Owner->GetWorldTransform(), GetModelTransform());
		else
			WorldTransform = GetModelTransform();

		DirtyFlags.UnraiseFlags(ZEMD_MDF_WORLD_TRANSFORM);
	}

	return WorldTransform;
}

const ZEMatrix4x4& ZEModelMesh::GetInvWorldTransform() const
{
	if (DirtyFlags.GetFlags(ZEMD_MDF_INV_WORLD_TRANSFORM))
	{
		ZEMatrix4x4::Inverse(InvWorldTransform, GetWorldTransform());
		DirtyFlags.UnraiseFlags(ZEMD_MDF_INV_WORLD_TRANSFORM);
	}

	return InvWorldTransform;
}

void ZEModelMesh::SetLocalPosition(const ZEVector3& LocalPosition)
{
	Position = LocalPosition;
	LocalTransformChanged();
}

const ZEVector3& ZEModelMesh::GetLocalPosition() const
{
	return Position;
}

void ZEModelMesh::SetLocalRotation(const ZEQuaternion& LocalRotation)
{
	Rotation = LocalRotation;
	LocalTransformChanged();
}

const ZEQuaternion& ZEModelMesh::GetLocalRotation() const
{
	return Rotation;
}

void ZEModelMesh::SetLocalScale(const ZEVector3& LocalScale)
{
	Scale = LocalScale;
	LocalTransformChanged();
}

const ZEVector3& ZEModelMesh::GetLocalScale() const
{
	return Scale;
}

void ZEModelMesh::SetModelPosition(const ZEVector3& ModelPosition)
{
	if (ParentMesh == NULL)
		SetLocalPosition(ModelPosition);
	else
	{
		ZEVector3 Result;
		ZEMatrix4x4::Transform(Result, ParentMesh->GetModelTransform().Inverse(), ModelPosition);
		SetLocalPosition(Result);
	}
}

const ZEVector3 ZEModelMesh::GetModelPosition() const
{
	if (ParentMesh == NULL)
		return Position;
	else
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, ParentMesh->GetModelTransform(), Position);
		return Temp;
	}
}

void ZEModelMesh::SetModelRotation(const ZEQuaternion& ModelRotation)
{
	if (ParentMesh == NULL)
		SetLocalRotation(ModelRotation);
	else
	{
		ZEQuaternion Temp, Result;
		ZEQuaternion::Conjugate(Temp, ParentMesh->GetModelRotation());
		ZEQuaternion::Product(Result, Temp, ModelRotation);
		SetLocalRotation(Result);
	}
}

const ZEQuaternion ZEModelMesh::GetModelRotation() const
{
	if (ParentMesh == NULL)
		return Rotation;
	else
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, ParentMesh->GetModelRotation(), Rotation);
		ZEQuaternion::Normalize(Temp, Temp);
		return Temp;
	}
}

void ZEModelMesh::SetModelScale(const ZEVector3& ModelScale)
{
	if (ParentMesh == NULL)
		SetLocalScale(ModelScale);
	else
	{
		ZEVector3 Temp;
		ZEVector3::Divide(Temp, ModelScale, ParentMesh->GetModelScale());
		SetLocalScale(Temp);
	}		
}

const ZEVector3 ZEModelMesh::GetModelScale() const
{
	if (ParentMesh == NULL)
		return Scale;
	else
	{
		ZEVector3 Temp;
		ZEVector3::Multiply(Temp, ParentMesh->GetModelScale(), Scale);
		return Temp;
	}
}

void ZEModelMesh::SetWorldPosition(const ZEVector3& WorldPosition)
{
	if (ParentMesh == NULL)
	{
		ZEVector3 Result;
		ZEMatrix4x4::Transform(Result, Owner->GetWorldTransform().Inverse(), WorldPosition);
		SetLocalPosition(Result);
	}
	else
	{
		ZEVector3 Result;
		ZEMatrix4x4::Transform(Result, ParentMesh->GetWorldTransform().Inverse(), WorldPosition);
		SetLocalPosition(Result);
	}
}

const ZEVector3 ZEModelMesh::GetWorldPosition() const
{
	if (ParentMesh == NULL)
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, Owner->GetWorldTransform(), Position);
		return Temp;
	}
	else
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, ParentMesh->GetWorldTransform(), Position);
		return Temp;
	}
}

void ZEModelMesh::SetWorldRotation(const ZEQuaternion& WorldRotation)
{
	if (ParentMesh == NULL)
	{	
		ZEQuaternion Temp, Result;
		ZEQuaternion::Conjugate(Temp, Owner->GetWorldRotation());
		ZEQuaternion::Product(Result, Temp, WorldRotation);
		SetLocalRotation(Result);
	}
	else
	{
		ZEQuaternion Temp, Result;
		ZEQuaternion::Conjugate(Temp, ParentMesh->GetWorldRotation());
		ZEQuaternion::Product(Result, Temp, WorldRotation);
		SetLocalRotation(Result);
	}
}

const ZEQuaternion ZEModelMesh::GetWorldRotation() const
{
	if (ParentMesh == NULL)
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, Owner->GetWorldRotation(), Rotation);
		ZEQuaternion::Normalize(Temp, Temp);
		return Temp;
	}
	else
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, ParentMesh->GetWorldRotation(), Rotation);
		ZEQuaternion::Normalize(Temp, Temp);
		return Temp;
	}
}

void ZEModelMesh::SetWorldScale(const ZEVector3& WorldScale)
{
	if (ParentMesh == NULL)
	{
		ZEVector3 Temp;
		ZEVector3::Divide(Temp, WorldScale, Owner->GetWorldScale());
		SetLocalScale(Temp);
	}
	else
	{
		ZEVector3 Temp;
		ZEVector3::Divide(Temp, WorldScale, ParentMesh->GetWorldScale());
		SetLocalScale(Temp);
	}
}

const ZEVector3 ZEModelMesh::GetWorldScale() const
{
	if (ParentMesh == NULL)
	{
		ZEVector3 Temp;
		ZEVector3::Multiply(Temp, Owner->GetWorldScale(), Scale);
		return Temp;
	}
	else
	{
		ZEVector3 Temp;
		ZEVector3::Multiply(Temp, ParentMesh->GetWorldScale(), Scale);
		return Temp;
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

void ZEModelMesh::AddChild(ZEModelMesh* Mesh)
{
	Mesh->ParentMesh = this;
	ChildMeshes.Add(Mesh);
}

void ZEModelMesh::RemoveChild(ZEModelMesh* Mesh)
{
	Mesh->ParentMesh = NULL;
	ChildMeshes.RemoveValue(Mesh);
}

void ZEModelMesh::SetPhysicsEnabled(bool Enabled)
{
	PhysicsEnabled = Enabled;
}

bool ZEModelMesh::GetPhysicsEnabled() const
{
	return PhysicsEnabled;
}

void ZEModelMesh::SetCustomDrawOrderEnabled(bool Enabled)
{
	DrawOrderIsUserDefined = Enabled;
}

void ZEModelMesh::SetCustomDrawOrder(ZEUInt8 DrawOrder)
{
	UserDefinedDrawOrder = DrawOrder;
}

ZEUInt8 ZEModelMesh::GetCustomDrawOrder()
{
	return UserDefinedDrawOrder;
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
	Visible = MeshResource->IsVisible;
	LocalBoundingBox = MeshResource->BoundingBox;
	PhysicsEnabled = false;
	LocalTransformChanged();
	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(ZEModelMeshConstants));

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
			PhysicalBody->SetTransformChangeEvent(ZEDelegate<void (ZEPhysicalObject*, ZEVector3, ZEQuaternion)>::Create<ZEModel, &ZEModel::TransformChangeEvent>(this->Owner));

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
		else if(MeshResource->PhysicalBody.Type == ZE_MRPBT_CLOTH)
		{
// 			PhysicalCloth = ZEPhysicalCloth::CreateInstance();
// 
// 			ZESize VertexCount = MeshResource->LODs[0].Vertices.GetCount();
// 			ZEArray<ZEVector3>& ClothVertices = PhysicalCloth->GetVertices();
// 			ClothVertices.SetCount(VertexCount);
// 
// 			for(ZESize I = 0; I < VertexCount; I++)
// 				ClothVertices[I] = MeshResource->LODs[0].Vertices[I].Position;
// 
// 			PhysicalCloth->SetPosition(Owner->GetWorldTransform() * Position);
// 			ZEQuaternion TempRotation;
// 			ZEQuaternion::CreateFromMatrix(TempRotation, Owner->GetWorldTransform() * GetLocalTransform());
// 			PhysicalCloth->SetRotation(TempRotation);
// 
// 			PhysicalCloth->SetEnabled(true);
// 			PhysicalCloth->SetThickness(0.5f);
// 			PhysicalCloth->SetBendingMode(true);
// 			PhysicalCloth->SetBendingStiffness(1.0f);
// 			PhysicalCloth->SetStretchingStiffness(1.0f);
// 			PhysicalCloth->SetPhysicalWorld(zeScene->GetPhysicalWorld());
// 			PhysicalCloth->Initialize();
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
	ParentMesh = NULL;

	if (PhysicalBody != NULL)
	{
		PhysicalBody->Destroy();
		PhysicalBody = NULL;
	}

	ChildMeshes.Clear();
	LODs.Clear();

	DirtyFlags.RaiseAll();
}

bool ZEModelMesh::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (!Visible)
		return false;

	if (Parameters->View->ViewVolume != NULL && Parameters->View->ViewVolume->CullTest(GetWorldBoundingBox()))
		return false;

	ZEInt CurrentLOD = 0;
	float DrawOrder = 0.0f;
	float LODDistanceSquare = 0.0f;

	float EntityDistanceSquare = ZEVector3::DistanceSquare(Parameters->View->Position, GetWorldPosition());	
	if (!DrawOrderIsUserDefined)
		DrawOrder = EntityDistanceSquare;
	else
		DrawOrder = EntityDistanceSquare * (UserDefinedDrawOrder + 1);
 	
	float CurrentDistanceSquare = 0.0f;
	for (ZESize I = 0; I < LODs.GetCount(); I++)
	{
		LODDistanceSquare = (float)(LODs[I].GetDrawStartDistance() * LODs[I].GetDrawStartDistance());

		if (LODDistanceSquare < EntityDistanceSquare)
		{
			if (CurrentDistanceSquare <= LODDistanceSquare)
			{
				CurrentDistanceSquare = LODDistanceSquare;
				CurrentLOD = (ZEInt)I;
			}
		}
	}

	/*if (EntityDistanceSquare > (LODs[CurrentLOD].GetDrawEndDistance() * LODs[CurrentLOD].GetDrawEndDistance()))
		return false;*/

	ZEModelMeshLOD* MeshLOD = &LODs[(ZESize)CurrentLOD];
	RenderCommand.Priority = 0;
	RenderCommand.Order = DrawOrder;
	RenderCommand.StageMask = MeshLOD->GetMaterial()->GetStageMask();
	RenderCommand.Entity = Owner;
	RenderCommand.ExtraParameters = MeshLOD;

	Parameters->Renderer->AddCommand(&RenderCommand);

	return true;
}

void ZEModelMesh::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	ZERayCastHelper Helper;
	Helper.SetReport(&Report);
	Helper.SetWorldTransform(&GetWorldTransform());
	Helper.SetInvWorldTransform(&GetInvWorldTransform());
	Helper.SetObject(Owner);
	Helper.SetSubObject(this);

	if (!Helper.RayCastBoundingBox(GetWorldBoundingBox(), GetLocalBoundingBox()))
		return;

	Helper.RayCastMesh(MeshResource->Geometry.GetConstCArray(), MeshResource->Geometry.GetCount(), sizeof(ZEVector3));
}

ZEModelMesh::ZEModelMesh()
{
	DirtyFlags.RaiseAll();
	Owner = NULL;
	ParentMesh = NULL;
	MeshResource = NULL;
	PhysicalBody = NULL;
	PhysicalCloth = NULL;
	Visible = true;
	PhysicsEnabled = false;
	AutoLOD = false;
	ActiveLOD = 0;
	AnimationType = ZE_MAT_NOANIMATION;
	DrawOrderIsUserDefined = false;
	UserDefinedDrawOrder = 0;
	LocalTransformChanged();
}

ZEModelMesh::~ZEModelMesh()
{
	Deinitialize();
}
