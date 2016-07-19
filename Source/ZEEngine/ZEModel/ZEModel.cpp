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

#include "ZEModelBone.h"
#include "ZEMDResourceMesh.h"
#include "ZEMDResourceBone.h"
#include "ZEMDResourceHelper.h"

#include "ZEError.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEGRResource.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZERenderer/ZERNRenderer.h"


void ZEModel::CalculateBoundingBox() const
{
	if (Meshes.GetCount() == 0 && Bones.GetCount() == 0)
	{
		const_cast<ZEModel*>(this)->SetBoundingBox(ZEAABBox(ZEVector3::Zero, ZEVector3::Zero));
		return;
	}

	ZEAABBox BoundingBox(ZEVector3(FLT_MAX, FLT_MAX, FLT_MAX), ZEVector3(-FLT_MAX, -FLT_MAX, -FLT_MAX));

	ze_for_each(Mesh, Meshes)
	{
		ze_for_each(LOD, Mesh->GetLODs())
		{
			if (!LOD->GetVertexType() != ZEMD_VT_SKINNED)
			{
				const ZEAABBox& CurrentBoundingBox = Mesh->GetModelBoundingBox();

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
	}

	ze_for_each(Bone, Bones)
	{
		const ZEAABBox& CurrentBoundingBox = Bone->GetModelBoundingBox();

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

void ZEModel::UpdateConstantBufferBoneTransforms()
{
	if (!DirtyConstantBufferSkin)
		return;

	void* Buffer;
	ConstantBufferBoneTransforms->Lock(&Buffer);

		ZESize BufferIndex = 0;

		ze_for_each(Bone, Bones)
		{
			static_cast<ZEMatrix4x4*>(Buffer)[BufferIndex] = Bone->GetVertexTransform();
			BufferIndex++;
		}

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

	ze_for_each(Bone, Bones)
		Bone->TransformChangedModel();

	ze_for_each(Mesh, Meshes)
		Mesh->TransformChangedModel();
	
	DirtyBoundingBox = true;
}

void ZEModel::ParentTransformChanged()
{
	ZEEntity::ParentTransformChanged();

	ze_for_each(Bone, Bones)
		Bone->TransformChangedModel();

	ze_for_each(Mesh, Meshes)
		Mesh->TransformChangedModel();

	DirtyBoundingBox = true;
}

ZEEntityResult ZEModel::LoadInternal()
{
	if (Resource == NULL)
		return ZE_ER_DONE;

	if (!Resource->IsLoaded())
		return ZE_ER_WAIT;
	else if (Resource->IsFailed())
		return ZE_ER_FAILED;

	BoundingBoxIsUserDefined = Resource->GetUserDefinedBoundingBoxEnabled();
	const_cast<ZEModel*>(this)->SetBoundingBox(Resource->GetUserDefinedBoundingBox());

	ze_for_each(ResourceMesh, Resource->GetMeshes())
	{
		ZEModelMesh* Mesh = new ZEModelMesh();
		AddMesh(Mesh);
		Mesh->SetResouce(ResourceMesh.GetPointer());
		Mesh->Load();
	}

	for (ZESize I = 0; I < Resource->GetMeshes().GetCount(); I++)
	{
		if (Resource->GetMeshes()[I]->GetParentMesh() != -1 || Resource->GetMeshes()[I]->GetParentMesh() >= Meshes.GetCount())
			continue;

		Meshes[(ZESize)Resource->GetMeshes()[I]->GetParentMesh()]->AddChildMesh(Meshes[I]);
	}

	ze_for_each(ResourceBone, Resource->GetBones())
	{
		ZEModelBone* Bone = new ZEModelBone();	
		AddBone(Bone);
		Bone->SetResource(ResourceBone.GetPointer());
		Bone->Load();
	}

	for (ZESize I = 0; I < Resource->GetBones().GetCount(); I++)
	{
		if (Resource->GetBones()[I]->GetParentBone() != -1 || Resource->GetBones()[I]->GetParentBone() >= Bones.GetCount())
			continue;

		Bones[(ZESize)Resource->GetBones()[I]->GetParentBone()]->AddChildBone(Bones[I]);
	}

	ze_for_each(ResourceHelper, Resource->GetHelpers())
	{
		ZEModelHelper* Helper = new ZEModelHelper();
		AddHelper(Helper);
		Helper->SetResource(ResourceHelper.GetPointer());
		Helper->Load();
	}

	DirtyConstantBufferSkin = true;
	DirtyBoundingBox = true;

	ConstantBufferBoneTransforms = ZEGRConstantBuffer::Create(150 * sizeof(ZEMatrix4x4));

	return ZE_ER_DONE;
}

ZEEntityResult ZEModel::UnloadInternal()
{
// 	if (ModelResource != NULL)
// 		const_cast<ZEMDResource*>(ModelResource)->Release();

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

// 	ze_for_each(IKChain, IKChains)
// 	{
// 		RemoveIKChain(IKChain.GetPointer());
// 		delete IKChain.GetPointer();
// 	}

	ze_for_each(AnimationTrack, AnimationTracks)
	{
		RemoveAnimationTrack(AnimationTrack.GetPointer());
		delete AnimationTrack.GetPointer();
	}

	AnimationTrack = NULL;

	ConstantBufferBoneTransforms.Release();

	return ZE_ER_DONE;
}

ZEDrawFlags ZEModel::GetDrawFlags() const
{
	return ZE_DF_CULL | ZE_DF_DRAW | ZE_DF_LIGHT_RECEIVER;
}

const ZEList2<ZEModelMesh>& ZEModel::GetMeshes()
{
	return Meshes;
}

ZEModelMesh* ZEModel::GetMesh(ZEUInt32 Id)
{
	zeCheckError(Id >= Meshes.GetCount(), NULL, "Cannot get mesh by id. Index out of range.");
	return Meshes[Id];
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
	zeCheckError(Mesh == NULL, ZE_VOID, "Cannot add mesh. Mesh is NULL.");
	zeCheckError(Mesh->GetParent() != NULL, ZE_VOID, 
		"Can not add mesh. Mesh belongs to another Mesh. Parent Mesh Name: \"%s\", Mesh Name: \"%s\".", 
		Mesh->GetParent()->GetName().ToCString(), Mesh->GetName().ToCString());
	zeCheckError((Mesh->GetModel() != NULL) && (Mesh->GetModel() != this), ZE_VOID, 
		"Can not add mesh. Mesh already added to another Model. Model Name: \"%s\", Mesh Name: \"%s\".",
		Mesh->GetModel()->GetName().ToCString(), Mesh->GetName().ToCString());
	zeCheckError(Meshes.Exists(&Mesh->ModelLink), ZE_VOID, 
		"Can not add mesh. Mesh already added to this Model. Model Name: \"%s\", Mesh Name: \"%s\".",
		GetName().ToCString(), Mesh->GetName().ToCString());

	Mesh->SetModel(this);
	Meshes.AddEnd(&Mesh->ModelLink);
}

void ZEModel::RemoveMesh(ZEModelMesh* Mesh)
{
	zeCheckError(Mesh == NULL, ZE_VOID, "Cannot remove mesh. Mesh is NULL.");
	zeCheckError(Mesh->GetModel() != this, ZE_VOID, "Cannot remove mesh. Mesh does not belong this Model. Parent Model Name: \"%s\", Mesh Name: \"%s\".",
		Mesh->GetModel()->GetName().ToCString(), Mesh->GetName().ToCString());

	Mesh->SetModel(NULL);
	Meshes.Remove(&Mesh->ModelLink);
}

const ZEList2<ZEModelBone>& ZEModel::GetBones()
{
	return Bones;
}

ZEModelBone* ZEModel::GetBone(ZEUInt32 Id)
{
	zeCheckError(Id >= Bones.GetCount(), NULL, "Cannot get bone by id. Index out of range.");
	return Bones[Id];
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

void ZEModel::AddBone(ZEModelBone* Bone)
{
	zeCheckError(Bone == NULL, ZE_VOID, "Cannot add bone. Bone is NULL.");
	zeCheckError(Bone->GetParent() != NULL, ZE_VOID, 
		"Can not add bone. Bone belongs to another Bone. Parent Bone Name: \"%s\", Bone Name: \"%s\".", 
		Bone->GetParent()->GetName().ToCString(), Bone->GetName().ToCString());
	zeCheckError((Bone->GetModel() != NULL) && (Bone->GetModel() != this), ZE_VOID, 
		"Can not add bone. Bone already added to another Model. Model Name: \"%s\", Bone Name: \"%s\".",
		Bone->GetModel()->GetName().ToCString(), Bone->GetName().ToCString());
	zeCheckError(Bones.Exists(&Bone->ModelLink), ZE_VOID, 
		"Can not add bone. Bone already added to this Model. Model Name: \"%s\", Bone Name: \"%s\".",
		GetName().ToCString(), Bone->GetName().ToCString());

	Bone->SetModel(this);
	Bones.AddEnd(&Bone->ModelLink);
}

void ZEModel::RemoveBone(ZEModelBone* Bone)
{
	zeCheckError(Bone == NULL, ZE_VOID, "Cannot remove bone. Bone is NULL.");
	zeCheckError(Bone->GetModel() != this, ZE_VOID, "Cannot remove bone. Bone does not belong this Model. Parent Model Name: \"%s\", Bone Name: \"%s\".",
		Bone->GetModel()->GetName().ToCString(), Bone->GetName().ToCString());

	Bone->SetModel(NULL);
	Bones.Remove(&Bone->ModelLink);
}

const ZEList2<ZEModelHelper>& ZEModel::GetHelpers()
{
	return Helpers;
}

ZEModelHelper* ZEModel::GetHelper(ZEUInt32 Id)
{
	zeCheckError(Id >= Helpers.GetCount(), NULL, "Cannot get helper by id. Index out of range.");
	return Helpers[Id];
}

ZEModelHelper* ZEModel::GetHelper(const char* Name)
{
	ze_for_each(Helper, GetHelpers())
	{
		if (Helper->GetName() == Name)
			return Helper.GetPointer();
	}

	return NULL;
}

void ZEModel::AddHelper(ZEModelHelper* Helper)
{
	zeCheckError(Helper == NULL, ZE_VOID, "Cannot add helper. Helper is NULL.");
	zeCheckError(Helper->GetParentMesh() != NULL, ZE_VOID, "Can not add helper. Helper belongs to another Mesh. Parent Mesh Name: \"%s\", Helper Name: \"%s\".", 
		Helper->GetParentMesh()->GetName().ToCString(), Helper->GetName());
	zeCheckError(Helper->GetParentBone() != NULL, ZE_VOID, "Can not add helper. Helper belongs to another Bone. Parent Bone Name: \"%s\", Helper Name: \"%s\".", 
		Helper->GetParentBone()->GetName().ToCString(), Helper->GetName());
	zeCheckError(Helper->GetModel() != NULL, ZE_VOID,  "Can not add helper. Helper already added to a Model. Model Name: \"%s\", Helper Name: \"%s\".",
		Helper->GetModel()->GetName().ToCString(), Helper->GetName());

	Helper->Model = this;
	Helpers.AddEnd(&Helper->ModelLink);
}

void ZEModel::RemoveHelper(ZEModelHelper* Helper)
{
	zeCheckError(Helper == NULL, ZE_VOID, "Cannot remove helper. Helper is NULL.");
	zeCheckError(Helper->GetModel() != this, ZE_VOID, "Cannot remove helper. Helper does not belong this Model. Helper Name: \"%s\".", Helper->GetName());

	Helper->Model = NULL;
	Helpers.Remove(&Helper->ModelLink);
}

const ZEList2<ZEModelAnimationTrack>& ZEModel::GetAnimationTracks()
{
	return AnimationTracks;
}

void ZEModel::AddAnimationTrack(ZEModelAnimationTrack* Track)
{
	zeCheckError(Track == NULL, ZE_VOID, "Cannot add animation track. Track is NULL.");
	zeCheckError(Track->GetModel() != NULL, ZE_VOID, "Can not add animation track. Track is already added to a model.");

	AnimationTrack->Model = this;
	AnimationTracks.AddEnd(&Track->ModelLink);
}

void ZEModel::RemoveAnimationTrack(ZEModelAnimationTrack* Track)
{
	zeCheckError(Track == NULL, ZE_VOID, "Cannot remove animation track. Track is NULL.");
	zeCheckError(Track->GetModel() != this, ZE_VOID, "Can not remove animation track. Track belongs to another model.");

	AnimationTrack->Model = NULL;
	AnimationTracks.Remove(&Track->ModelLink);
}

void ZEModel::SetAnimationTrack(ZEModelAnimationTrack* Track)
{
	AnimationTrack = Track;
}

ZEModelAnimationTrack* ZEModel::GetAnimationTrack()
{
	return AnimationTrack;
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

void ZEModel::SetModelFile(const ZEString& FileName)
{
	ZERSHolder<const ZEMDResource> ModelResource = ZEMDResource::LoadResourceShared(FileName);

	if (ModelResource.IsNull())
	{
		zeError("Can not load model file. File Name : \"%s\"", FileName.ToCString());
		return;
	}

	SetModelResource(ModelResource);
}

const ZEString& ZEModel::GetModelFile() const
{
	if (Resource.IsNull())
		return ZEString::Empty;

	return Resource->GetFileName();
}

void ZEModel::SetModelResource(ZERSHolder<const ZEMDResource> ModelResource)
{
	bool ReloadingRequired = IsLoaded();

	if (ReloadingRequired)
		Unload();

	Resource = ModelResource;

	if (ReloadingRequired)
		Load();
}

ZERSHolder<const ZEMDResource> ZEModel::GetModelResource() const
{
	return Resource;
}

void ZEModel::SetAnimationType(ZEModelAnimationType AnimationType)
{
	this->AnimationType = AnimationType;

	ze_for_each(Mesh, Meshes)
		Mesh->SetAnimationType(AnimationType);

	ze_for_each(Bone, Bones)
		Bone->SetAnimationType(AnimationType);
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

bool ZEModel::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (!ZEEntity::PreRender(Parameters))
		return false;

	if (AnimationUpdateMode == ZE_MAUM_VISUAL)
	{
		if (AnimationTrack != NULL)
			AnimationTrack->UpdateAnimation();

		ze_for_each(AnimationTrack, GetAnimationTracks())
			AnimationTrack->UpdateAnimation();
	}

	bool Result = false;
	ze_for_each(Mesh, Meshes)
	{
		if (Mesh->PreRender(Parameters))
			Result = true;
	}

	return Result;
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
	Resource = NULL;
	AnimationUpdateMode = ZE_MAUM_LOGICAL;
	BoundingBoxIsUserDefined = false;
	DirtyBoundingBox = true;
	DirtyConstantBufferSkin = true;
}

ZEModel::~ZEModel()
{
	Unload();
}

void ZEModel::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	ze_for_each(Mesh, Meshes)
	{
		Mesh->RayCast(Report, Parameters);
		if (Report.CheckDone())
			return;
	}

	ze_for_each(Bone, Bones)
	{
		Bone->RayCast(Report, Parameters);
		if (Report.CheckDone())
			return;
	}
}

ZEModel* ZEModel::CreateInstance()
{
	return new ZEModel();
}

