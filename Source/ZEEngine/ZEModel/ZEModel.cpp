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
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNRenderParameters.h"


void ZEModel::CalculateBoundingBox() const
{
	if (Meshes.GetCount() == 0 && Bones.GetCount() == 0)
	{
		DirtyBoundingBox = false;
		const_cast<ZEModel*>(this)->SetBoundingBox(ZEAABBox(ZEVector3::Zero, ZEVector3::Zero));
		return;
	}
	else
	{
		ZEAABBox BoundingBox(ZEVector3(FLT_MAX, FLT_MAX, FLT_MAX), ZEVector3(-FLT_MAX, -FLT_MAX, -FLT_MAX));

		ze_for_each(Mesh, Meshes)
		{
			ze_for_each(LOD, Mesh->GetLODs())
			{
				if (LOD->GetVertexType() != ZEMD_VT_SKINNED)
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

		DirtyBoundingBox = false;
		const_cast<ZEModel*>(this)->SetBoundingBox(BoundingBox, true);
	}
}

void ZEModel::UpdateConstantBufferBoneTransforms()
{
	if (!DirtyConstantBufferSkin)
		return;

	void* Buffer;
	ConstantBufferBoneTransforms->Map(ZEGR_RMT_WRITE_DISCARD, &Buffer);

		ZESize BufferIndex = 0;

		ze_for_each(Bone, Bones)
		{
			static_cast<ZEMatrix4x4*>(Buffer)[BufferIndex] = Bone->GetVertexTransform();
			BufferIndex++;
		}

	ConstantBufferBoneTransforms->Unmap();

	DirtyConstantBufferSkin = false;
}

void ZEModel::ChildBoundingBoxChanged()
{
	BoundingBoxChanged();
}

void ZEModel::LocalTransformChanged()
{
	ZEEntity::LocalTransformChanged();

	ze_for_each(Bone, Bones)
		Bone->TransformChangedModel();

	ze_for_each(Mesh, Meshes)
		Mesh->TransformChangedModel();
	
	BoundingBoxChanged();
}

void ZEModel::ParentTransformChanged()
{
	ZEEntity::ParentTransformChanged();

	ze_for_each(Bone, Bones)
		Bone->TransformChangedModel();

	ze_for_each(Mesh, Meshes)
		Mesh->TransformChangedModel();

	BoundingBoxChanged();
}

void ZEModel::AnimationStateChanged()
{
	if (AnimationTrack == NULL && AnimationTracks.GetCount() == 0)
	{
		SetEntityFlags(GetEntityFlags() & ~ZE_EF_TICKABLE);
	}
	else
	{
		if (AnimationTrack != NULL && AnimationTrack->GetState() == ZE_MAS_PLAYING)
		{
			SetEntityFlags((GetEntityFlags() | ZE_EF_TICKABLE) & ~ZE_EF_STATIC_SUPPORT);
			return;
		}

		for(ZESize I = 0; I < AnimationTracks.GetCount(); I++)
		{
			if (AnimationTracks[I]->GetState() == ZE_MAS_PLAYING)
			{
				SetEntityFlags((GetEntityFlags() | ZE_EF_TICKABLE) & ~ZE_EF_STATIC_SUPPORT);
				return;
			}
			
			SetEntityFlags(GetEntityFlags() & (~ZE_EF_TICKABLE));
		}
	}
}

void ZEModel::BoundingBoxChanged()
{
	DirtyBoundingBox = true;
	ZEEntity::BoundingBoxChanged();
}

ZEEntityResult ZEModel::LoadInternal()
{
	ZE_ENTITY_LOAD_CHAIN(ZEEntity);

	ZE_ENTITY_RESOURCE_FENCE_LOADED_PROGRESS(Resource, ZE_ER_FAILED, 5, 90)
	{
		if (ModelFileName.IsEmpty())
			return ZE_ER_DONE;
		
		Resource = ZEMDResource::LoadResourceShared(ModelFileName);
		return ZE_ER_WAIT;
	}

	BoundingBoxIsUserDefined = Resource->GetUserDefinedBoundingBoxEnabled();
	const_cast<ZEModel*>(this)->SetBoundingBox(Resource->GetUserDefinedBoundingBox());

	ze_for_each(ResourceMesh, Resource->GetMeshes())
	{
		ZEModelMesh* Mesh = ZEModelMesh::CreateInstance();
		AddMesh(Mesh);
		if (!Mesh->Load(ResourceMesh.GetPointer()))
			return ZE_ER_FAILED_CLEANUP;
	}

	for (ZESize I = 0; I < Resource->GetMeshes().GetCount(); I++)
	{
		if (Resource->GetMeshes()[I]->GetParentMeshId() < 0 || (ZESize)Resource->GetMeshes()[I]->GetParentMeshId() >= Meshes.GetCount())
			continue;

		Meshes[(ZESize)Resource->GetMeshes()[I]->GetParentMeshId()]->AddChildMesh(Meshes[I]);
	}

	ze_for_each(ResourceBone, Resource->GetBones())
	{
		ZEModelBone* Bone = ZEModelBone::CreateInstance();	
		AddBone(Bone);
		if (!Bone->Load(ResourceBone.GetPointer()))
			return ZE_ER_FAILED_CLEANUP;
	}

	for (ZESize I = 0; I < Resource->GetBones().GetCount(); I++)
	{
		if (Resource->GetBones()[I]->GetParentBoneId() < 0 || (ZESize)Resource->GetBones()[I]->GetParentBoneId() >= Bones.GetCount())
			continue;

		Bones[(ZESize)Resource->GetBones()[I]->GetParentBoneId()]->AddChildBone(Bones[I]);
	}

	ze_for_each(ResourceHelper, Resource->GetHelpers())
	{
		ZEModelHelper* Helper = ZEModelHelper::CreateInstance();
		AddHelper(Helper);
		if (!Helper->Load(ResourceHelper.GetPointer()))
			return ZE_ER_FAILED_CLEANUP;
	}

	ze_for_each(AnimationTrack, AnimationTracks)
		AnimationTrack->BindAnimation();

	DirtyConstantBufferSkin = true;
	DirtyBoundingBox = true;

	ConstantBufferBoneTransforms = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, 150 * sizeof(ZEMatrix4x4), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);

	return ZE_ER_DONE;
}

ZEEntityResult ZEModel::UnloadInternal()
{
	Resource = NULL;

	while (Meshes.GetFirst() != NULL)
	{
		ZEModelMesh* Mesh = Meshes.GetFirst()->GetItem();
		Mesh->Destroy();
	}

	while (Bones.GetFirst() != NULL)
	{
		ZEModelBone* Bone = Bones.GetFirst()->GetItem();
		Bone->Destroy();
	}

	while (Helpers.GetFirst() != NULL)
	{
		ZEModelHelper* Helper = Helpers.GetFirst()->GetItem();
		Helper->Destroy();
	}

	while (AnimationTracks.GetFirst() != NULL)
	{
		ZEModelAnimationTrack* AnimationTrack = AnimationTracks.GetFirst()->GetItem();
		AnimationTrack->Destroy();
	}

	if (AnimationTrack != NULL)
	{
		AnimationTrack->Destroy();
		AnimationTrack = NULL;
	}

	ConstantBufferBoneTransforms.Release();

	ZE_ENTITY_UNLOAD_CHAIN(ZEEntity);
	return ZE_ER_DONE;
}

ZEModel::ZEModel()
{
	Resource = NULL;
	AnimationUpdateMode = ZE_MAUM_LOGICAL;
	AnimationTrack = NULL;
	BoundingBoxIsUserDefined = false;
	DirtyBoundingBox = true;
	DirtyConstantBufferSkin = true;

	SetStatic(true);
	SetEntityFlags(ZE_EF_RENDERABLE | ZE_EF_CULLABLE | ZE_EF_STATIC_SUPPORT);
}

ZEModel::~ZEModel()
{
	Unload();
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

	DirtyBoundingBox = true;

	Mesh->SetModel(this);
	Meshes.AddEnd(&Mesh->ModelLink);
}

void ZEModel::RemoveMesh(ZEModelMesh* Mesh)
{
	zeCheckError(Mesh == NULL, ZE_VOID, "Cannot remove mesh. Mesh is NULL.");
	zeCheckError(Mesh->GetModel() != this, ZE_VOID, "Cannot remove mesh. Mesh does not belong this Model. Parent Model Name: \"%s\", Mesh Name: \"%s\".",
		Mesh->GetModel()->GetName().ToCString(), Mesh->GetName().ToCString());

	while (Mesh->ChildMeshes.GetFirst() != NULL)
	{
		ZEModelMesh* ChildMesh = Mesh->ChildMeshes.GetFirst()->GetItem();
		Mesh->RemoveChildMesh(ChildMesh);
	}

	if (Mesh->GetParent() != NULL)
		Mesh->GetParent()->RemoveChildMesh(Mesh);

	DirtyBoundingBox = true;

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

	DirtyConstantBufferSkin = true;
	DirtyBoundingBox = true;

	Bone->SetModel(this);
	Bones.AddEnd(&Bone->ModelLink);
}

void ZEModel::RemoveBone(ZEModelBone* Bone)
{
	zeCheckError(Bone == NULL, ZE_VOID, "Cannot remove bone. Bone is NULL.");
	zeCheckError(Bone->GetModel() != this, ZE_VOID, "Cannot remove bone. Bone does not belong this Model. Parent Model Name: \"%s\", Bone Name: \"%s\".",
		Bone->GetModel()->GetName().ToCString(), Bone->GetName().ToCString());

	while (Bone->ChildBones.GetFirst() != NULL)
	{
		ZEModelBone* ChildBone = Bone->ChildBones.GetFirst()->GetItem();
		Bone->RemoveChildBone(ChildBone);
	}

	if (Bone->GetParent() != NULL)
		Bone->GetParent()->RemoveChildBone(Bone);

	DirtyConstantBufferSkin = true;
	DirtyBoundingBox = true;

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

	Track->Model = this;
	AnimationTracks.AddEnd(&Track->ModelLink);
	Track->BindAnimation();
}

void ZEModel::RemoveAnimationTrack(ZEModelAnimationTrack* Track)
{
	zeCheckError(Track == NULL, ZE_VOID, "Cannot remove animation track. Track is NULL.");
	zeCheckError(Track->GetModel() != this, ZE_VOID, "Can not remove animation track. Track belongs to another model.");

	Track->Model = NULL;
	Track->ResourceAnimation = NULL;
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
	if (!BoundingBoxIsUserDefined && DirtyBoundingBox)
		CalculateBoundingBox();

	return ZEEntity::GetBoundingBox();
}

const ZEAABBox& ZEModel::GetWorldBoundingBox() const
{
	if (!BoundingBoxIsUserDefined && DirtyBoundingBox)
		CalculateBoundingBox();

	return ZEEntity::GetWorldBoundingBox();
}

void ZEModel::SetModelFile(const ZEString& FileName)
{
	ModelFileName = FileName;

	if (IsLoadedOrLoading())
	{
		ZERSHolder<const ZEMDResource> ModelResource = ZEMDResource::LoadResourceShared(FileName);
		SetModelResource(ModelResource);
	}
}

const ZEString& ZEModel::GetModelFile() const
{
	return ModelFileName;
}

void ZEModel::SetModelResource(ZERSHolder<const ZEMDResource> ModelResource)
{
	if (Resource == ModelResource)
		return;

	Resource = ModelResource;
	ModelFileName = Resource->GetFileName();
	
	if (IsLoaded())
		Reload();
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
	if (this->AnimationUpdateMode == AnimationUpdateMode)
		return;

	this->AnimationUpdateMode = AnimationUpdateMode;

	AnimationStateChanged();
}

ZEModelAnimationUpdateMode ZEModel::GetAnimationUpdateMode()
{
	return AnimationUpdateMode;
}

bool ZEModel::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (!ZEEntity::PreRender(Parameters))
		return false;

	if (Parameters->Type != ZERN_RT_SHADOW)
	{
		if (AnimationUpdateMode == ZE_MAUM_VISUAL)
		{
			if (AnimationTrack != NULL)
				AnimationTrack->UpdateAnimation();

			ze_for_each(AnimationTrack, GetAnimationTracks())
				AnimationTrack->UpdateAnimation();
		}
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

