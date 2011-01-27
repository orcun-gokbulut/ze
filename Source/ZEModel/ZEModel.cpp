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
#include "ZEModelFileFormat.h"
#include "ZEGraphics/ZERenderer.h"
#include "ZEGraphics/ZEVertexBuffer.h"
#include "ZEGraphics/ZESimpleMaterial.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGame/ZECompoundEntity.h"

#include <stdio.h>
#include <string.h>

#include "ZEGame/ZEEntityProvider.h"

ZE_META_REGISTER_CLASS(ZEEntityProvider, ZEModel);

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

	for (size_t I = 0; I < Bones.GetCount(); I++)
	{
		ZEVector3 BonePosition = Bones[I].GetModelPosition();

		if (NotInitialized)
		{
			BoundingBox.Min = BoundingBox.Max = BonePosition;
			NotInitialized = false;
		}

		if (BonePosition.x < BoundingBox.Min.x) BoundingBox.Min.x = BonePosition.x;
		if (BonePosition.y < BoundingBox.Min.y) BoundingBox.Min.y = BonePosition.y;
		if (BonePosition.z < BoundingBox.Min.z) BoundingBox.Min.z = BonePosition.z;

		if (BonePosition.x > BoundingBox.Max.x) BoundingBox.Max.x = BonePosition.x;
		if (BonePosition.y > BoundingBox.Max.y) BoundingBox.Max.y = BonePosition.y;
		if (BonePosition.z > BoundingBox.Max.z) BoundingBox.Max.z = BonePosition.z;
	}

	if (NotInitialized )
		BoundingBox.Max = BoundingBox.Max = ZEVector3(0.0f, 0.0f, 0.0f);
}

void ZEModel::DebugDraw(ZERenderer* Renderer)
{
	if (DebugDrawComponents.Material == NULL)
	{
		DebugDrawComponents.Material = ZESimpleMaterial::CreateInstance();

		DebugDrawComponents.BonePositionsRenderOrder.SetZero();
		DebugDrawComponents.BonePositionsRenderOrder.Material = DebugDrawComponents.Material;
		DebugDrawComponents.BonePositionsRenderOrder.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_IMPOSTER;
		DebugDrawComponents.BonePositionsRenderOrder.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
		DebugDrawComponents.BonePositionsRenderOrder.VertexBuffer = &DebugDrawComponents.BonePositionsCanvas;
		DebugDrawComponents.BonePositionsRenderOrder.PrimitiveType = ZE_ROPT_POINT;

		DebugDrawComponents.BonesRenderOrder.SetZero();
		DebugDrawComponents.BonesRenderOrder.Material = DebugDrawComponents.Material;
		DebugDrawComponents.BonesRenderOrder.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_IMPOSTER;
		DebugDrawComponents.BonesRenderOrder.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
		DebugDrawComponents.BonesRenderOrder.VertexBuffer = &DebugDrawComponents.BonesCanvas;
		DebugDrawComponents.BonesRenderOrder.PrimitiveType = ZE_ROPT_LINE;
	}

	DebugDrawComponents.BonesCanvas.Clean();
	DebugDrawComponents.BonePositionsCanvas.Clean();
	DebugDrawComponents.BonePositionsCanvas.SetColor(ZEVector4(1.0f, 0.0f, 0.0f, 0.0f));
	DebugDrawComponents.BonesCanvas.SetColor(ZEVector4(0.0f, 1.0f, 1.0f, 0.0f));

	if (DrawSkeleton)
	{
		ZEVector3 BonePosition1, BonePosition2;
		for (size_t I = 0; I < Bones.GetCount(); I++)
		{
			DebugDrawComponents.BonePositionsCanvas.AddPoint(Bones[I].GetWorldPosition());
		
			if (Bones[I].GetParentBone() != NULL)
				DebugDrawComponents.BonesCanvas.AddLine(Bones[I].GetWorldPosition(), Bones[I].GetParentBone()->GetWorldPosition());
		}
		DebugDrawComponents.BonesRenderOrder.WorldMatrix = GetWorldTransform();
		DebugDrawComponents.BonesRenderOrder.PrimitiveCount = DebugDrawComponents.BonesCanvas.Vertices.GetCount() / 2;

		DebugDrawComponents.BonePositionsRenderOrder.WorldMatrix = GetWorldTransform();
		DebugDrawComponents.BonePositionsRenderOrder.PrimitiveCount = DebugDrawComponents.BonePositionsCanvas.Vertices.GetCount();

		Renderer->AddToRenderList(&DebugDrawComponents.BonesRenderOrder);
		Renderer->AddToRenderList(&DebugDrawComponents.BonePositionsRenderOrder);
	}

}

void ZEModel::LocalTransformChanged()
{
	for (size_t I = 0; I < Meshes.GetCount(); I++)
		Meshes[I].ModelTransformChanged();

	for (size_t I = 0; I < Bones.GetCount(); I++)
		Bones[I].ModelTransformChanged();

}

ZEDWORD ZEModel::GetDrawFlags() const
{
	return ZE_DF_CULL | ZE_DF_DRAW | ZE_DF_LIGHT_RECIVER;
}

void ZEModel::SetModelFile(const char* ModelFile)
{
	ZEModelResource* ModelResource = ZEModelResource::LoadSharedResource(ModelFile);


	if (ModelResource == NULL)
	{
		zeError("Model", "Can not load model file.");
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
	this->ModelResource = ModelResource;


	if (ModelResource == NULL)
	{
		for (size_t I = 0; Meshes.GetCount(); I++)
			Meshes[I].Deinitialize();
		Meshes.SetCount(0);

		for (size_t I = 0; Meshes.GetCount(); I++)
			Bones[I].Deinitialize();
		Bones.SetCount(0);

		Skeleton.SetCount(0);
		return;
	}
	else
	{
		Meshes.SetCount(ModelResource->Meshes.GetCount());
		for (size_t I = 0; I < ModelResource->Meshes.GetCount(); I++)
		{
			Meshes[I].Initialize(this, &ModelResource->Meshes[I]);
		}

		Bones.SetCount(ModelResource->Bones.GetCount());
		for (size_t I = 0; I < ModelResource->Bones.GetCount(); I++)
		{
			Bones[I].Initialize(this, &ModelResource->Bones[I]);
			if (Bones[I].GetParentBone() == NULL)
				Skeleton.Add(&Bones[I]);
		}

		for (size_t I = 0; I < ModelResource->Bones.GetCount(); I++)
		{
			if (ModelResource->Bones[I].ParentBone != -1)
				Bones[ModelResource->Bones[I].ParentBone].AddChild(&Bones[I]);
		}
	}

	UpdateBoundingBox();
	UpdateBoneTransforms();
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


ZEArray<ZEModelAnimationTrack>& ZEModel::GetAnimationTracks()
{
	return AnimationTracks;
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

/*
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

			Bones[Key->ItemId].SetRelativePosition(Key->Position);
			Bones[Key->ItemId].SetRelativeRotation(Key->Rotation);
		}
		
		for (size_t I = 0; I < Frame->MeshKeys.GetCount(); I++)
		{
			Key = &Frame->MeshKeys[I];
			
			Meshes[Key->ItemId].SetLocalPosition(Key->Position);
			Meshes[Key->ItemId].SetLocalRotation(Key->Rotation);
			Meshes[Key->ItemId].SetLocalScale(Key->Scale);
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
		if (strnicmp(AnimationName, ModelResource->Animations[I].Name, ZE_MDLF_MAX_NAME_SIZE) == 0)
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
	//AnimationState = ZE_MAS_PLAYING;
}

void ZEModel::PauseAnimation()
{
	//AnimationState = ZE_MAS_PAUSED;
}

void ZEModel::StopAnimation()
{
	//AnimationState = ZE_MAS_STOPPED;
}
*/

const ZEAABoundingBox& ZEModel::GetLocalBoundingBox() const
{
	if (BoundingBoxDirtyFlag)
	{
		((ZEModel*)this)->CalculateBoundingBox();
		((ZEModel*)this)->BoundingBoxDirtyFlag = false;
	}
	return BoundingBox;
}

void ZEModel::SetPosition(const ZEVector3& NewPosition)
{
	ZEComponent::SetPosition(NewPosition);
	LocalTransformChanged();
}

void ZEModel::SetRotation(const ZEQuaternion& NewRotation)
{
	ZEComponent::SetRotation(NewRotation);
	LocalTransformChanged();
}

void ZEModel::SetScale(const ZEVector3& NewScale)
{
	ZEComponent::SetScale(NewScale);
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
	BoundingBoxDirtyFlag = true;
	UpdateBoundingVolumes();
	if (GetOwner() != NULL)
		GetOwner()->UpdateBoundingVolumes();
}

void ZEModel::UpdateBoneTransforms()
{
	BoneTransformsDirtyFlag = true;
}

void ZEModel::Draw(ZEDrawParameters* DrawParameters)
{
	for (size_t I = 0; I < Meshes.GetCount(); I++)
		Meshes[I].Draw(DrawParameters);
	
	DebugDraw(DrawParameters->Renderer);
}

void ZEModel::Tick(float ElapsedTime)
{
	for(size_t I = 0; I < AnimationTracks.GetCount(); I++)
		AnimationTracks[I].AdvanceAnimation(ElapsedTime);
}

ZEModel::ZEModel()
{
	ModelResource = NULL;
	Visibility = true;
	AutoLOD = true;
	ActiveLOD = 0;
	BoundingBoxDirtyFlag = true;
	BoneTransformsDirtyFlag = true;
	DebugDrawComponents.Material = NULL;
	DrawSkeleton = false;
}

ZEModel::~ZEModel()
{
	if (DebugDrawComponents.Material != NULL)
	{
		DebugDrawComponents.Material->Release();
	}

	if (ModelResource != NULL)
		((ZEModelResource*)ModelResource)->Release();
}

ZEModel* ZEModel::CreateInstance()
{
	return new ZEModel();
}

#include "ZEModel.h.zpp"

ZEEntityRunAt ZEModelDescription::GetRunAt() const
{
	return ZE_ERA_BOTH;
}
