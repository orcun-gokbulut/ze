//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModel.cpp
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

#include "ZEModel.h"
#include "ZEGraphics/ZERenderer.h"
#include "ZEGraphics/ZEVertexBuffer.h"
#include "ZEGraphics/ZESimpleMaterial.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGame/ZEEntity.h"
#include "ZEGame/ZEScene.h"
#include "ZEError.h"

#include <stdio.h>
#include <string.h>
#include <float.h>

#include "ZEGame/ZEEntityProvider.h"

ZE_META_REGISTER_CLASS(ZEEntityProvider, ZEModel);

void ZEModel::CalculateBoundingBox()
{
	if (Meshes.GetCount() == 0 && Bones.GetCount() == 0)
	{
		SetBoundingBox(ZEAABBox(ZEVector3::Zero, ZEVector3::Zero));
		return;
	}

	ZEAABBox BoundingBox(ZEVector3(FLT_MAX, FLT_MAX, FLT_MAX), ZEVector3(-FLT_MAX, -FLT_MAX, -FLT_MAX));

	for (ZESize I = 0; I < Meshes.GetCount(); I++)
	{
		if (!Meshes[I].MeshResource->IsSkinned)
		{
			ZEAABBox CurrentBoundingBox = Meshes[I].GetModelBoundingBox();
			//ZEAABBox::Transform(CurrentBoundingBox, Meshes[I].GetLocalBoundingBox(), Meshes[I].GetLocalTransform());

			for (ZEInt N = 0; N < 8; N++)
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
	}

	for (ZESize I = 0; I < Bones.GetCount(); I++)
	{
		ZEVector3 BonePosition = Bones[I].GetLocalPosition();

		if (BonePosition.x < BoundingBox.Min.x) BoundingBox.Min.x = BonePosition.x;
		if (BonePosition.y < BoundingBox.Min.y) BoundingBox.Min.y = BonePosition.y;
		if (BonePosition.z < BoundingBox.Min.z) BoundingBox.Min.z = BonePosition.z;

		if (BonePosition.x > BoundingBox.Max.x) BoundingBox.Max.x = BonePosition.x;
		if (BonePosition.y > BoundingBox.Max.y) BoundingBox.Max.y = BonePosition.y;
		if (BonePosition.z > BoundingBox.Max.z) BoundingBox.Max.z = BonePosition.z;
	}

	SetBoundingBox(BoundingBox);
}

void ZEModel::UpdateTransforms()
{
	for (ZESize I = 0; I < Bones.GetCount(); I++)
		Bones[I].OnTransformChanged();

	for (ZESize I = 0; I < Meshes.GetCount(); I++)
		Meshes[I].OnTransformChanged();
}

void ZEModel::DebugDraw(ZERenderer* Renderer)
{
	if (DebugDrawComponents.Material == NULL)
	{
		DebugDrawComponents.Material = ZESimpleMaterial::CreateInstance();


		DebugDrawComponents.BonePositionsRenderCommand.SetZero();
		DebugDrawComponents.BonePositionsRenderCommand.Material = DebugDrawComponents.Material;
		DebugDrawComponents.BonePositionsRenderCommand.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_WORLD_TRANSFORM;
		DebugDrawComponents.BonePositionsRenderCommand.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
		DebugDrawComponents.BonePositionsRenderCommand.VertexBuffer = &DebugDrawComponents.BonePositionsCanvas;
		DebugDrawComponents.BonePositionsRenderCommand.PrimitiveType = ZE_ROPT_TRIANGLE;
		DebugDrawComponents.BonePositionsRenderCommand.Priority = 4;


		DebugDrawComponents.BonesRenderCommand.SetZero();
		DebugDrawComponents.BonesRenderCommand.Material = DebugDrawComponents.Material;
		DebugDrawComponents.BonesRenderCommand.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_WORLD_TRANSFORM;
		DebugDrawComponents.BonesRenderCommand.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
		DebugDrawComponents.BonesRenderCommand.VertexBuffer = &DebugDrawComponents.BonesCanvas;
		DebugDrawComponents.BonesRenderCommand.PrimitiveType = ZE_ROPT_LINE;
		DebugDrawComponents.BonesRenderCommand.Priority = 4;

		DebugDrawComponents.BoxRenderCommand.SetZero();
		DebugDrawComponents.BoxRenderCommand.Material = DebugDrawComponents.Material;
		DebugDrawComponents.BoxRenderCommand.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_NO_Z_WRITE;
		DebugDrawComponents.BoxRenderCommand.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
		DebugDrawComponents.BoxRenderCommand.VertexBuffer = &DebugDrawComponents.BoxCanvas;
		DebugDrawComponents.BoxRenderCommand.PrimitiveType = ZE_ROPT_LINE;
	}

	DebugDrawComponents.BonesCanvas.Clean();
	DebugDrawComponents.BonePositionsCanvas.Clean();
	DebugDrawComponents.BoxCanvas.Clean();
	DebugDrawComponents.BonePositionsCanvas.SetColor(ZEVector4(1.0f, 0.0f, 0.0f, 0.0f));
	DebugDrawComponents.BonesCanvas.SetColor(ZEVector4(0.0f, 1.0f, 1.0f, 0.0f));
	DebugDrawComponents.BoxCanvas.SetColor(ZEVector4(1.0f, 1.0f, 1.0f, 0.0f));

	if (DrawSkeleton)
	{
		ZEVector3 BonePosition1, BonePosition2;
		for (ZESize I = 0; I < Bones.GetCount(); I++)
		{
			DebugDrawComponents.BonePositionsCanvas.SetTranslation(Bones[I].GetWorldPosition());
			DebugDrawComponents.BonePositionsCanvas.AddSphere(0.02f, 8, 8);

			if (Bones[I].GetParentBone() != NULL)
				DebugDrawComponents.BonesCanvas.AddLine(Bones[I].GetWorldPosition(), Bones[I].GetParentBone()->GetWorldPosition());
		}
		//DebugDrawComponents.BonesRenderCommand.WorldMatrix = GetWorldTransform();
		DebugDrawComponents.BonesRenderCommand.WorldMatrix = ZEMatrix4x4::Identity;
		DebugDrawComponents.BonesRenderCommand.PrimitiveCount = DebugDrawComponents.BonesCanvas.Vertices.GetCount() / 2;

		//DebugDrawComponents.BonePositionsRenderCommand.WorldMatrix = GetWorldTransform();
		DebugDrawComponents.BonePositionsRenderCommand.WorldMatrix = ZEMatrix4x4::Identity;
		DebugDrawComponents.BonePositionsRenderCommand.PrimitiveCount = DebugDrawComponents.BonePositionsCanvas.Vertices.GetCount() / 3;

		Renderer->AddToRenderList(&DebugDrawComponents.BonesRenderCommand);
		Renderer->AddToRenderList(&DebugDrawComponents.BonePositionsRenderCommand);
	}

	ZEAABBox BoundingBox = GetWorldBoundingBox();
	DebugDrawComponents.BoxCanvas.SetRotation(ZEQuaternion::Identity);
	DebugDrawComponents.BoxCanvas.SetTranslation(BoundingBox.GetCenter());
	DebugDrawComponents.BoxCanvas.AddWireframeBox((BoundingBox.Max.x - BoundingBox.Min.x), (BoundingBox.Max.y - BoundingBox.Min.y), (BoundingBox.Max.z - BoundingBox.Min.z));
	DebugDrawComponents.BoxRenderCommand.WorldMatrix = ZEMatrix4x4::Identity;
	DebugDrawComponents.BoxRenderCommand.PrimitiveCount = DebugDrawComponents.BoxCanvas.Vertices.GetCount() / 2;
	DebugDrawComponents.BoxRenderCommand.Priority = 4;
	Renderer->AddToRenderList(&DebugDrawComponents.BoxRenderCommand);

}

ZEDrawFlags ZEModel::GetDrawFlags() const
{
	return ZE_DF_CULL | ZE_DF_DRAW | ZE_DF_LIGHT_RECIVER;
}

void ZEModel::LoadModelResource()
{
	for (ZESize I = 0; I < Meshes.GetCount(); I++)
		Meshes[I].Deinitialize();
	Meshes.SetCount(0);

	for (ZESize I = 0; I < Bones.GetCount(); I++)
		Bones[I].Deinitialize();
	Bones.SetCount(0);

	Skeleton.SetCount(0);

	if (ModelResource == NULL)
		return;

	Meshes.SetCount(ModelResource->GetMeshes().GetCount());
	for (ZESize I = 0; I < ModelResource->GetMeshes().GetCount(); I++)
	{
		Meshes[I].Initialize(this, &ModelResource->GetMeshes()[I]);
	}

	Bones.SetCount(ModelResource->GetBones().GetCount());

	for (ZESize I = 0; I < ModelResource->GetBones().GetCount(); I++)
	{
		if (ModelResource->GetBones()[I].ParentBone != -1)
		{
			Bones[(ZESize)ModelResource->GetBones()[I].ParentBone].AddChild(&Bones[I]);
		}
	}

	for (ZESize I = 0; I < ModelResource->GetBones().GetCount(); I++)
	{
		Bones[I].Initialize(this, &ModelResource->GetBones()[I]);
		if (Bones[I].GetParentBone() == NULL)
		{
			Skeleton.Add(&Bones[I]);
		}
	}

	if(Skeleton.GetCount() > 1)
	{
		ZEVector3 AveragePosition = ZEVector3::Zero;

		for(ZESize I = 0; I < Skeleton.GetCount(); I++)
		{
			AveragePosition += Skeleton[I]->PhysicalBody->GetPosition();
		}
		ZEVector3::Scale(AveragePosition, AveragePosition , 1.0f / Skeleton.GetCount());

		ParentlessBoneBodyPosition = AveragePosition;

		ParentlessBoneBody = ZEPhysicalRigidBody::CreateInstance();
		ParentlessBoneShape = new ZEPhysicalBoxShape();
		ParentlessBoneBody->SetPosition(ParentlessBoneBodyPosition);
		ParentlessBoneBody->SetEnabled(true);
		ParentlessBoneBody->SetMass(1.0f); //This must be average of other bone parts' mass
		ParentlessBoneShape->SetWidth(0.01f);
		ParentlessBoneShape->SetHeight(0.01f);
		ParentlessBoneShape->SetLength(0.01f);
		ParentlessBoneShape->SetPosition(ZEVector3::Zero);
		ParentlessBoneBody->AddPhysicalShape(ParentlessBoneShape);

		ParentlessBoneBody->SetPhysicalWorld(zeScene->GetPhysicalWorld());
		ParentlessBoneBody->Initialize();

		for(ZESize I = 0; I < Skeleton.GetCount(); I++)
		{
			LinkParentlessBones(Skeleton[I]);
		}
	}
}

void ZEModel::SetModelFile(const char* ModelFile)
{
	ZEModelResource* ModelResource = ZEModelResource::LoadSharedResource(ModelFile);

	if (ModelResource == NULL)
	{
		zeError("Can not load model file. File Name : \"%s\"", ModelFile);
		return;
	}

	SetModelResource(ModelResource);
}

const char* ZEModel::GetModelFile() const
{
	if (ModelResource != NULL)
		return ModelResource->GetFileName();
	else
		return "";
}

void ZEModel::SetModelResource(const ZEModelResource* ModelResource)
{
	if (this->ModelResource != NULL)
		((ZEModelResource*)this->ModelResource)->Release();

	ModelResource->AddReferance();

	this->ModelResource = ModelResource;

	if (GetInitialized())
		LoadModelResource();
}

const ZEModelResource* ZEModel::GetModelResource()
{
	return ModelResource;
}


const ZEArray<ZEModelBone*>& ZEModel::GetSkeleton()
{
	return Skeleton;
}

ZEArray<ZEModelBone>& ZEModel::GetBones()
{
	return Bones;
}

const ZEArray<ZEModelMesh>& ZEModel::GetMeshes()
{
	return Meshes;
}

const ZEArray<ZEModelAnimation>* ZEModel::GetAnimations()
{
	if (ModelResource == NULL)
	{
		return  NULL;
	}
	else 
	{
		return &ModelResource->GetAnimations();
	}
}

void ZEModel::SetActiveLOD(ZEUInt LOD)
{
	ActiveLOD = LOD;
	for (ZESize I = 0; I < Meshes.GetCount(); I++)
	{
		Meshes[I].SetActiveLOD(LOD);
	}
}

ZEUInt ZEModel::GetActiveLOD()
{
	return ActiveLOD;
}

void ZEModel::SetPhysicsEnabled(bool Enabled)
{
	PhysicsEnabled = Enabled;
	for (ZESize I = 0; I < Bones.GetCount(); I++)
	{
		Bones[I].SetPhysicsEnabled(Enabled);
	}

	for (ZESize I = 0; I < Meshes.GetCount(); I++)
	{
		Meshes[I].SetPhysicsEnabled(Enabled);
	}
}

bool ZEModel::GetPhysicsEnabled()
{
	return PhysicsEnabled;
}

ZEModelBone* ZEModel::GetBone(const char* Name)
{
	for (ZESize I = 0; I < Bones.GetCount(); I++)
		if (strcmp(Bones[I].GetName(), Name) == 0)
			return &Bones[I];

	return NULL;
}

ZEModelMesh* ZEModel::GetMesh(const char* Name)
{
	for (ZESize I = 0; I < Meshes.GetCount(); I++)
		if (strcmp(Meshes[I].GetName(), Name) == 0)
			return &Meshes[I];

	return NULL;
}

void ZEModel::SetAnimationType(ZEModelAnimationType AnimationType)
{
	this->AnimationType = AnimationType;
	for (ZESize I = 0; I < Meshes.GetCount(); I++)
		Meshes[I].SetAnimationType(AnimationType);
	for (ZESize I = 0; I < Bones.GetCount(); I++)
		Bones[I].SetAnimationType(AnimationType);
}

ZEModelAnimationType ZEModel::GetAnimationType()
{
	return AnimationType;
}

ZEArray<ZEModelAnimationTrack>& ZEModel::GetAnimationTracks()
{
	return AnimationTracks;
}

void ZEModel::SetAutoLOD(bool Enabled)
{
	AutoLOD = Enabled;
	for (ZESize I = 0; I < Meshes.GetCount(); I++)
		Meshes[I].SetAutoLOD(Enabled);
}

bool ZEModel::GetAutoLOD()
{
	return AutoLOD;
}

const ZEAABBox& ZEModel::GetWorldBoundingBox()
{
	((ZEModel*)this)->CalculateBoundingBox();
	return ZEEntity::GetWorldBoundingBox();
}

void ZEModel::SetPosition(const ZEVector3& NewPosition)
{
	if (ParentlessBoneBody != NULL && Bones.GetCount() > 0)
	{
		ParentlessBoneBody->SetPosition(NewPosition + ParentlessBoneBodyPosition);
	}

	ZEEntity::SetPosition(NewPosition);
	UpdateTransforms();
}

void ZEModel::SetRotation(const ZEQuaternion& NewRotation)
{
	ZEEntity::SetRotation(NewRotation);
	UpdateTransforms();
}

void ZEModel::SetScale(const ZEVector3& NewScale)
{
	ZEEntity::SetScale(NewScale);
	UpdateTransforms();
}

void ZEModel::LinkParentlessBones( ZEModelBone* ParentlessBone )
{
	ZEPhysicalJoint* ParentlessBoneJoint = ZEPhysicalJoint::CreateInstance();

	ParentlessBoneJoint->SetBodyA(ParentlessBone->PhysicalBody);
	ParentlessBoneJoint->SetBodyB(ParentlessBoneBody);

	ParentlessBoneJoint->SetPosition(ParentlessBoneBody->GetPosition());
	ParentlessBoneJoint->SetRotation(ParentlessBoneBody->GetRotation());

	ParentlessBoneJoint->SetXMotion(ZE_PJMOTION_LOCKED);
	ParentlessBoneJoint->SetYMotion(ZE_PJMOTION_LOCKED);
	ParentlessBoneJoint->SetZMotion(ZE_PJMOTION_LOCKED);

	ParentlessBoneJoint->SetSwing1Motion(ZE_PJMOTION_LOCKED);
	ParentlessBoneJoint->SetSwing2Motion(ZE_PJMOTION_LOCKED);
	ParentlessBoneJoint->SetTwistMotion(ZE_PJMOTION_LOCKED);

	ParentlessBoneJoint->SetMassInertiaTensor(ZEVector3(1,1,1)); // For solid joint connection

	ParentlessBoneJoint->SetPhysicalWorld(zeScene->GetPhysicalWorld());
	ParentlessBoneJoint->Initialize();
}

void ZEModel::Draw(ZEDrawParameters* DrawParameters)
{
	if(!GetVisible())
		return;

	for (ZESize I = 0; I < Meshes.GetCount(); I++)
		Meshes[I].Draw(DrawParameters);
	
	//DebugDraw(DrawParameters->Renderer);
}

void ZEModel::Tick(float ElapsedTime)
{
	for(ZESize I = 0; I < AnimationTracks.GetCount(); I++)
		AnimationTracks[I].Tick(ElapsedTime);

	for(ZESize I = 0; I < IKChains.GetCount(); I++)
		IKChains[I].Process();
	
}

void ZEModel::TransformChangeEvent(ZEPhysicalObject* PhysicalObject, ZEVector3 NewPosition, ZEQuaternion NewRotation)
{
	for (ZESize I = 0; I < Bones.GetCount(); I++)
	{
		if(Bones[I].GetParentBone() != NULL)
		{
			ZEQuaternion Inverse;
			ZEQuaternion::Conjugate(Inverse, Bones[I].GetParentBone()->GetWorldRotation());

			ZEMatrix4x4 InvParent;
			ZEMatrix4x4::Inverse(InvParent, Bones[I].GetParentBone()->GetWorldTransform());
			ZEVector3 Position;
			ZEMatrix4x4::Transform(Position, InvParent, Bones[I].PhysicalBody->GetPosition());

			Bones[I].SetRelativePosition(Position);
			Inverse.NormalizeSelf();
			Bones[I].SetRelativeRotation(Inverse * Bones[I].PhysicalBody->GetRotation());
		}
		else
		{
			ZEQuaternion Inverse;
			ZEQuaternion::Conjugate(Inverse, this->GetWorldRotation());
			Bones[I].SetRelativePosition(Bones[I].PhysicalBody->GetPosition() - GetWorldPosition());
			Inverse.NormalizeSelf();
			Bones[I].SetRelativeRotation(Inverse * Bones[I].PhysicalBody->GetRotation());
		}
	}

	for (ZESize I = 0; I < Meshes.GetCount(); I++)
	{
		if (Meshes[I].GetPhysicalBody() != NULL)
		{
			ZEQuaternion Inverse;
			ZEQuaternion::Conjugate(Inverse, this->GetWorldRotation());
			Meshes[I].SetLocalPosition(Meshes[I].GetPhysicalBody()->GetPosition() - GetWorldPosition());
			Inverse.NormalizeSelf();
			Meshes[I].SetLocalRotation(Inverse * Meshes[I].GetPhysicalBody()->GetRotation());
		}
	}
}

ZEModel::ZEModel()
{
	ModelResource = NULL;
	Visibility = true;
	AutoLOD = true;
	ActiveLOD = 0;
	DebugDrawComponents.Material = NULL;
	DrawSkeleton = true;
	ParentlessBoneBody = NULL;
}

ZEModel::~ZEModel()
{
	if (DebugDrawComponents.Material != NULL)
		DebugDrawComponents.Material->Release();

	if (ModelResource != NULL)
		((ZEModelResource*)ModelResource)->Release();
}

bool ZEModel::Initialize()
{
	LoadModelResource();

	return ZEEntity::Initialize();
}

void ZEModel::Deinitialize()
{

}

ZEModel* ZEModel::CreateInstance()
{
	return new ZEModel();
}

ZEEntityRunAt ZEModelDescription::GetRunAt() const
{
	return ZE_ERA_BOTH;
}
