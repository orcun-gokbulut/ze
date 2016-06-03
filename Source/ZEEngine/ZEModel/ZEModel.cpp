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

#include "ZERenderer/ZERNRenderer.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEGRResource.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"

void ZEModel::CalculateBoundingBox() const
{
	if (Meshes.GetCount() == 0 && Bones.GetCount() == 0)
	{
		const_cast<ZEModel*>(this)->SetBoundingBox(ZEAABBox(ZEVector3::Zero, ZEVector3::Zero));
		return;
	}

	ZEAABBox BoundingBox(ZEVector3(FLT_MAX, FLT_MAX, FLT_MAX), ZEVector3(-FLT_MAX, -FLT_MAX, -FLT_MAX));

	for (ZESize I = 0; I < Meshes.GetCount(); I++)
	{
		if (!Meshes[I].MeshResource->IsSkinned)
		{
			const ZEAABBox& CurrentBoundingBox = Meshes[I].GetModelBoundingBox();

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
		const ZEAABBox& CurrentBoundingBox = Bones[I].GetModelBoundingBox();

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

	const_cast<ZEModel*>(this)->SetBoundingBox(BoundingBox);
}

ZEDrawFlags ZEModel::GetDrawFlags() const
{
	return ZE_DF_CULL | ZE_DF_DRAW | ZE_DF_LIGHT_RECEIVER;
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

	for (ZESize I = 0; I < Helpers.GetCount(); I++)
		Helpers[I].Deinitialize();
	Helpers.SetCount(0);

	if (ModelResource == NULL)
		return;

	if (ModelResource->GetUserDefinedBoundingBoxEnabled())
	{
		BoundingBoxIsUserDefined = true;
		SetBoundingBox(ModelResource->GetUserDefinedBoundingBox());
	}

	Meshes.SetCount(ModelResource->GetMeshes().GetCount());
	for (ZESize I = 0; I < ModelResource->GetMeshes().GetCount(); I++)
	{
		if (ModelResource->GetMeshes()[I].ParentMesh != -1)
		{
			Meshes[(ZESize)ModelResource->GetMeshes()[I].ParentMesh].AddChild(&Meshes[I]);
		}
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

	for (ZESize I = 0; I < ModelResource->GetMeshes().GetCount(); I++)
	{
		Meshes[I].Initialize(this, &ModelResource->GetMeshes()[I]);
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

	Helpers.SetCount(ModelResource->GetHelpers().GetCount());
	for (ZESize I = 0; I < ModelResource->GetHelpers().GetCount(); I++)
	{
		Helpers[I].Initialize(this, &ModelResource->GetHelpers()[I]);
	}

	DirtyConstantBufferSkin = true;
	DirtyBoundingBox = true;
}


void ZEModel::UpdateConstantBufferBoneTransforms()
{
	if (!DirtyConstantBufferSkin)
		return;

	void* Buffer;
	ConstantBufferBoneTransforms->Lock(&Buffer);

		ZESize BoneCount = Bones.GetCount();
		for (ZESize I = 0; I < BoneCount; I++)
			static_cast<ZEMatrix4x4*>(Buffer)[I] = Bones[I].GetVertexTransform();

	ConstantBufferBoneTransforms->Unlock();

	DirtyConstantBufferSkin = false;
}

void ZEModel::ChildBoundingBoxChanged()
{
	BoundingBoxChanged();
	DirtyBoundingBox = true;
}

void ZEModel::LocalTransformChanged()
{
	ZEEntity::LocalTransformChanged();

	if (ParentlessBoneBody != NULL && Bones.GetCount() > 0)
		ParentlessBoneBody->SetPosition(GetWorldPosition() + ParentlessBoneBodyPosition);

	for (ZESize I = 0; I < Skeleton.GetCount(); I++)
		Skeleton[I]->ParentTransformChanged();
	
	for (ZESize I = 0; I < Meshes.GetCount(); I++)
		Meshes[I].ParentTransformChanged();

	DirtyBoundingBox = true;
}

void ZEModel::ParentTransformChanged()
{
	ZEEntity::ParentTransformChanged();

	for (ZESize I = 0; I < Skeleton.GetCount(); I++)
		Skeleton[I]->ParentTransformChanged();

	for (ZESize I = 0; I < Meshes.GetCount(); I++)
		Meshes[I].ParentTransformChanged();

	DirtyBoundingBox = true;
}

void ZEModel::SetModelFile(const ZEString& ModelFile)
{
	ZEModelResource* ModelResource = ZEModelResource::LoadSharedResource(ModelFile);

	if (ModelResource == NULL)
	{
		zeError("Can not load model file. File Name : \"%s\"", ModelFile.ToCString());
		return;
	}

	SetModelResource(ModelResource);
}

const ZEString& ZEModel::GetModelFile() const
{
	if (ModelResource != NULL)
		return ModelResource->GetFileName();
	else
		return ZEString::Empty;
}

void ZEModel::SetModelResource(const ZEModelResource* ModelResource)
{
	if (this->ModelResource != NULL)
		((ZEModelResource*)this->ModelResource)->Release();

	ModelResource->AddReferance();

	this->ModelResource = ModelResource;

	if (IsInitialized())
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

const ZEArray<ZEModelHelper>& ZEModel::GetHelpers()
{
	return Helpers;
}

const ZEArray<ZEModelAnimation>* ZEModel::GetAnimations()
{
	if (ModelResource == NULL)
		return  NULL;

	return &ModelResource->GetAnimations();
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

ZEModelMesh* ZEModel::GetMesh(ZESize Index)
{
	return &Meshes[Index];
}

ZEModelMesh* ZEModel::GetMesh(const char* Name)
{
	for (ZESize I = 0; I < Meshes.GetCount(); I++)
		if (strcmp(Meshes[I].GetName(), Name) == 0)
			return &Meshes[I];

	return NULL;
}

ZEModelHelper* ZEModel::GetHelper(const char* Name)
{
	for (ZESize I = 0; I < Helpers.GetCount(); I++)
		if (strcmp(Helpers[I].GetName(), Name) == 0)
			return &Helpers[I];

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

void ZEModel::SetAnimationUpdateMode(ZEModelAnimationUpdateMode AnimationUpdateMode)
{
	this->AnimationUpdateMode = AnimationUpdateMode;
}

ZEModelAnimationUpdateMode ZEModel::GetAnimationUpdateMode()
{
	return AnimationUpdateMode;
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

void ZEModel::SetUserDefinedBoundingBoxEnabled(bool Value)
{
	BoundingBoxIsUserDefined = Value;
}

const ZEAABBox& ZEModel::GetBoundingBox() const
{
	if (DirtyBoundingBox && !BoundingBoxIsUserDefined)
	{
		CalculateBoundingBox();
		DirtyBoundingBox = false;
	}

	return ZEEntity::GetBoundingBox();
}

const ZEAABBox& ZEModel::GetWorldBoundingBox() const
{
	if (DirtyBoundingBox && !BoundingBoxIsUserDefined)
	{
		CalculateBoundingBox();
		DirtyBoundingBox = false;
	}

	return ZEEntity::GetWorldBoundingBox();
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

bool ZEModel::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (!ZEEntity::PreRender(Parameters))
		return false;

	if (AnimationUpdateMode == ZE_MAUM_VISUAL)
	{
		for(ZESize I = 0; I < AnimationTracks.GetCount(); I++)
			AnimationTracks[I].UpdateAnimation();
	}

	bool Result = false;
	for (ZESize I = 0; I < Meshes.GetCount(); I++)
	{
		if (Meshes[I].PreRender(Parameters))
			Result = true;
	}

	return Result;
}

void ZEModel::Render(const ZERNRenderParameters* RenderParameters, const ZERNCommand* Command)
{
	ZEModelMeshLOD* MeshLOD = (ZEModelMeshLOD*)Command->ExtraParameters;
	MeshLOD->Render(RenderParameters, Command);
}

void ZEModel::Tick(float ElapsedTime)
{
	for (ZESize I = 0; I < AnimationTracks.GetCount(); I++)
	{
		AnimationTracks[I].Tick(ElapsedTime);

		if (AnimationUpdateMode == ZE_MAUM_LOGICAL)
			AnimationTracks[I].UpdateAnimation();
	}

	for (ZESize I = 0; I < IKChains.GetCount(); I++)
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

			Bones[I].SetPosition(Position);
			Inverse.NormalizeSelf();
			Bones[I].SetRotation(Inverse * Bones[I].PhysicalBody->GetRotation());
		}
		else
		{
			ZEQuaternion Inverse;
			ZEQuaternion::Conjugate(Inverse, this->GetWorldRotation());
			Bones[I].SetPosition(Bones[I].PhysicalBody->GetPosition() - GetWorldPosition());
			Inverse.NormalizeSelf();
			Bones[I].SetRotation(Inverse * Bones[I].PhysicalBody->GetRotation());
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
	ParentlessBoneBody = NULL;
	AnimationUpdateMode = ZE_MAUM_LOGICAL;
	BoundingBoxIsUserDefined = false;
	DirtyBoundingBox = true;
	DirtyConstantBufferSkin = true;
}

ZEModel::~ZEModel()
{
	Deinitialize();
}

bool ZEModel::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	LoadModelResource();

	ConstantBufferBoneTransforms = ZEGRConstantBuffer::Create(150 * sizeof(ZEMatrix4x4));

	return true;
}

bool ZEModel::DeinitializeSelf()
{
	if (ModelResource != NULL)
		const_cast<ZEModelResource*>(ModelResource)->Release();

	Skeleton.Clear();
	LODRenderCommands.Clear();
	Meshes.Clear();
	Bones.Clear();
	Helpers.Clear();

	ConstantBufferBoneTransforms.Release();

	AnimationTracks.Clear();

	return ZEEntity::DeinitializeSelf();
}

void ZEModel::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	for (ZESize I = 0; I < Meshes.GetCount(); I++)
	{
		Meshes[I].RayCast(Report, Parameters);
		if (Report.CheckDone())
			return;
	}

	for (ZESize I = 0; I < Bones.GetCount(); I++)
	{
		Bones[I].RayCast(Report, Parameters);
		if (Report.CheckDone())
			return;
	}
}

ZEModel* ZEModel::CreateInstance()
{
	return new ZEModel();
}

