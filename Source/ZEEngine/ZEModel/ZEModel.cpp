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
#include "ZEGraphics/ZEGRVertexBuffer.h"
#include "ZERenderer/ZERNSimpleMaterial.h"
#include "ZEGame/ZEScene.h"
#include "ZEError.h"

#include <stdio.h>
#include <string.h>
#include <float.h>

void ZEModel::CalculateBoundingBox() const
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

	SetBoundingBox(BoundingBox);
}

void ZEModel::UpdateTransforms()
{
	for (ZESize I = 0; I < Bones.GetCount(); I++)
		Bones[I].OnTransformChanged();

	for (ZESize I = 0; I < Meshes.GetCount(); I++)
		Meshes[I].OnTransformChanged();
}

ZEDrawFlags ZEModel::GetDrawFlags() const
{
	return /*ZE_DF_CULL | */ZE_DF_DRAW | ZE_DF_LIGHT_RECEIVER;
}

void ZEModel::CleanUp()
{
	ze_for_each(Mesh, Meshes)
	{
		RemoveMesh(Mesh.GetPointer());
		delete Mesh.GetPointer();
	}

	ze_for_each(Bone, Bones)
	{
		RemoveBone(Bone.GetPointer());
		delete Bone.GetPointer();
	}


	ze_for_each(Helper, Helpers)
	{
		RemoveHelper(Helper.GetPointer());
		delete Helper.GetPointer();
	}

	ze_for_each(IKChain, IKChains)
	{
		RemoveIKChain(IKChain.GetPointer());
		delete IKChain.GetPointer();
	}

	ze_for_each(AnimationTrack, AnimationTracks)
	{
		RemoveAnimationTrack(AnimationTrack.GetPointer());
		delete AnimationTrack.GetPointer();
	}

	AnimationTrack = NULL;

	Skeleton.Clear();
}

void ZEModel::LoadResource()
{
	if (Resource == NULL)
		return;

	CleanUp();

	BoundingBoxIsUserDefined = Resource->GetUserDefinedBoundingBoxEnabled())
	SetBoundingBox(Resource->GetUserDefinedBoundingBox());

	ze_for_each(ResourceMesh, Resource->GetMeshes())
	{
		ZEPointer<ZEModelMesh> Mesh = new ZEModelMesh();
		Mesh->Initialize(this, ResourceMesh.GetPointer());
		AddMesh(Mesh.Transfer());
	}

	ze_for_each(ResorceMesh, Resource->GetMeshes())
	{
		if (ResorceMesh->GetParentMesh() != -1)
		{
			Meshes[(ZESize)ResorceMesh->GetParentMesh()]->AddChild(ResorceMesh.GetPointer());
		}
	}

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

	Helpers.SetCount(ModelResource->GetHelpers().GetCount());
	for (ZESize I = 0; I < ModelResource->GetHelpers().GetCount(); I++)
	{
		Helpers[I].Initialize(this, &ModelResource->GetHelpers()[I]);
	}
}

void ZEModel::SetModelFile(const ZEString& ModelFile)
{
	ZEHolder<const ZEModelResource> ModelResource = ZEModelResource::Load(ModelFile);
	SetModelResource(ModelResource);
}

const ZEString& ZEModel::GetModelFile() const
{
	if (Resource != NULL)
		return Resource->GetFileName();
	else
		return ZEString::Empty;
}

void ZEModel::SetModelResource(const ZEModelResource* ModelResource)
{
	Resource = ModelResource;

	if (IsInitialized())
		LoadModelResource();
}

ZEHolder<const ZEModelResource> ZEModel::GetModelResource() const
{
	return Resource;
}




const ZEList2<ZEModelMesh>& ZEModel::GetMeshes()
{
	return Meshes;
}

ZEModelMesh* ZEModel::GetMesh(const char* Name)
{
	ze_for_each(Mesh, GetMeshes())
	{
		if (Mesh->GetName() == Name)
			return Mesh.GetPointer();
	}

	return NULL;
}

void ZEModel::AddMesh(ZEModelMesh* Mesh)
{
	if (Mesh->GetModelPosition())
	Meshes.AddEnd(&Mesh->Link);
}

void ZEModel::RemoveMesh(ZEModelMesh* Mesh);
const ZEList2<ZEModelBone>& ZEModel::GetBones()
{
	return Bones;
}

ZEModelBone* ZEModel::GetBone(const char* Name)
{
	ze_for_each(Bone, GetBones())
	{
		if (Bone->GetName() == Name)
			return Bone.GetPointer();
	}

	return NULL;
}


const ZEList2<ZEModelHelper>& ZEModel::GetHelpers()
{
	return Helpers;
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

void ZEModel::SetUserDefinedBoundingBoxEnabled(bool Value)
{
	BoundingBoxIsUserDefined = Value;
}

const ZEAABBox& ZEModel::GetWorldBoundingBox() const
{
	if (!IsStaticModel || !StaticCalculationsDone)
	{
		if (!BoundingBoxIsUserDefined)
			CalculateBoundingBox();

		StaticCalculationsDone = IsStaticModel;
	}

	return ZEEntity::GetWorldBoundingBox();
}

void ZEModel::SetStaticModel(bool Value)
{
	IsStaticModel = Value;
}

bool ZEModel::GetStaticModel() const
{
	return IsStaticModel;
}

void ZEModel::SetPosition(const ZEVector3& NewPosition)
{
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

bool ZEModel::PreRender(const ZERNCullParameters* CullParameters)
{
	if (!ZEEntity::PreRender(CullParameters))
		return false;

	if (AnimationUpdateMode == ZE_MAUM_VISUAL)
	{
		if (AnimationTrack != NULL)
			AnimationTrack->UpdateAnimation();

		ze_for_each(AnimationTrack, GetAnimationTracks())
			AnimationTrack->UpdateAnimation();
	}

	bool Result = false;
	ze_for_each(Mesh, GetMeshes())
	{
		if (Mesh->PreRender(CullParameters))
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
	if (AnimationTrack != NULL)
	{
		AnimationTrack->Tick(ElapsedTime);
		if (AnimationUpdateMode == ZE_MAUM_LOGICAL)
			AnimationTrack->UpdateAnimation();
	}

	ze_for_each(AnimationTrack, AnimationTracks)
	{

		AnimationTrack->Tick(ElapsedTime);
		if (AnimationUpdateMode == ZE_MAUM_LOGICAL)
			AnimationTrack->UpdateAnimation();
	}

	ze_for_each(IKChain, IKChains)
		IKChain->Process();
}

ZEModel::ZEModel()
{
	ParentlessBoneBody = NULL;
	AnimationUpdateMode = ZE_MAUM_LOGICAL;
	BoundingBoxIsUserDefined = false;
	IsStaticModel = false;
	StaticCalculationsDone = false;
}

ZEModel::~ZEModel()
{
	CleanUp();
}

bool ZEModel::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	LoadResource();

	return true;
}

bool ZEModel::DeinitializeSelf()
{
	return ZEEntity::DeinitializeSelf();
}

bool ZEModel::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	bool Result = false;
	ze_for_each(Mesh, GetMeshes())
		Result |= Mesh->RayCast(Report, Parameters);

	ze_for_each(Bone, GetBones())
		Result |= Bone->RayCast(Report, Parameters);
	
	if (Result)
		Report.Entity = this;

	return Result;
}

ZEModel* ZEModel::CreateInstance()
{
	return new ZEModel();
}

