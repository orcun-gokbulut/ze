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
#include "Graphics/Renderer.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/SimpleMaterial.h"
#include "Game/Entity.h"

#include <stdio.h>
#include <string.h>

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

void ZEModel::SetModelResource(const ZEModelResource* ModelResource)
{
	if (this->ModelResource != NULL)
		((ZEModelResource*)this->ModelResource)->Release();
	this->ModelResource = ModelResource;

	if (ModelResource == NULL)
		return;

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
	AnimationStartFrame = 0;
	AnimationEndFrame = Animation->Frames.GetCount() - 1;
}

void ZEModel::SetAnimationByName(const char* AnimationName)
{
	Animation = NULL;

	if (ModelResource == NULL)
		return;

	for (size_t I = 0; I < ModelResource->Animations.GetCount(); I++)
		if (strnicmp(AnimationName, ModelResource->Animations[I].Name, ZE_MDLF_MAX_NAME_SIZE) == 0)
		{
			Animation = &ModelResource->Animations[I];
			AnimationStartFrame = 0;
			AnimationEndFrame = Animation->Frames.GetCount() - 1;
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

const ZEAABoundingBox& ZEModel::GetLocalBoundingBox() const
{
	if (BoundingBoxDirtyFlag)
	{
		((ZEModel*)this)->CalculateBoundingBox();
		((ZEModel*)this)->BoundingBoxDirtyFlag = false;
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
	BoundingBoxDirtyFlag = true;
	UpdateBoundingVolumes();
	GetOwner()->UpdateBoundingVolumes();
}

void ZEModel::UpdateBoneTransforms()
{
	BoneTransformsDirtyFlag = true;
}

void ZEModel::Draw(ZERenderer* Renderer, const ZESmartArray<const ZERLLight*>& Lights)
{
	for (size_t I = 0; I < Meshes.GetCount(); I++)
		Meshes[I].Draw(Renderer, Lights);
	
	DebugDraw(Renderer);
}

void ZEModel::Tick(float ElapsedTime)
{
	if (AnimationState == ZE_MAS_PLAYING)
	{
		if (AnimationFrame >= AnimationEndFrame)
		{
			if (AnimationLooping)
				AnimationFrame = AnimationStartFrame + fmodf(AnimationFrame, AnimationEndFrame - AnimationStartFrame);
			else
			{
				AnimationFrame = AnimationEndFrame;
				AnimationState = ZE_MAS_STOPPED;
			}
		}
			
		float Interpolation = AnimationFrame - floorf(AnimationFrame);

		int NextFrameId = (int)ceilf(AnimationFrame);
		if (NextFrameId >= Animation->Frames.GetCount())
			NextFrameId = AnimationStartFrame + fmodf(AnimationFrame, AnimationEndFrame - AnimationStartFrame);

		const ZEModelResourceAnimationFrame* Frame = &Animation->Frames[(int)floorf(AnimationFrame)];
		const ZEModelResourceAnimationFrame* NextFrame = &Animation->Frames[NextFrameId];


		for (size_t I = 0; I < Frame->BoneKeys.GetCount(); I++)
		{
			const ZEModelResourceAnimationKey* Key = &Frame->BoneKeys[I];
			const ZEModelResourceAnimationKey* NextKey = &NextFrame->BoneKeys[I];
			
			ZEVector3 Position;
			ZEVector3::Lerp(Position, Key->Position, NextKey->Position, Interpolation);
			Bones[Key->ItemId].SetRelativePosition(Position);

			ZEQuaternion Rotation;
			ZEQuaternion::Slerp(Rotation, Key->Rotation, NextKey->Rotation, Interpolation);
			Bones[Key->ItemId].SetRelativeRotation(Rotation);
		}
		
		for (size_t I = 0; I < Frame->MeshKeys.GetCount(); I++)
		{
			const ZEModelResourceAnimationKey* Key = &Frame->MeshKeys[I];
			const ZEModelResourceAnimationKey* NextKey = &NextFrame->MeshKeys[I];
			
			ZEVector3 Position;
			ZEVector3::Lerp(Position, Key->Position, NextKey->Position, Interpolation);
			Meshes[Key->ItemId].SetLocalPosition(Position);

			ZEQuaternion Rotation;
			ZEQuaternion::Slerp(Rotation, Key->Rotation, NextKey->Rotation, Interpolation);
			Meshes[Key->ItemId].SetLocalRotation(Rotation);

			ZEVector3 Scale;
			ZEVector3::Lerp(Scale, Key->Scale, NextKey->Scale, Interpolation);
			Meshes[Key->ItemId].SetLocalScale(Scale);
		}
		AnimationFrame += AnimationSpeed * ElapsedTime;
	}

	/*if (Meshes.GetCount()>0)
	{
		//Meshes[0].SetLocalPosition(ZEVector3::Zero);
		//Meshes[0].GetPhysicalBody()->SetPosition(ZEVector3::Zero);
		//ZEVector3 ewrt = Meshes[0].GetPhysicalBody()->GetPosition();
		
		for (int i=0;i<Meshes.GetCount();i++)
		{
			Meshes[i].SetLocalPosition(Meshes[i].GetPhysicalBody()->GetPosition());
			Meshes[i].SetLocalRotation(Meshes[i].GetPhysicalBody()->GetOrientation());

			//Meshes[i].SetLocalPosition(ZEVector3::Zero);
			//ZEQuaternion q;
			//static float rrr;
			//rrr += ZE_PI * ElapsedTime;
			//ZEQuaternion::Create(q, rrr, ZEVector3::UnitX);
			//Meshes[i].SetLocalRotation(q);
		}

		//this->SetWorldPosition(ZEVector3::Zero);
		//this->SetLocalPosition(ZEVector3::Zero);
		/*this->SetWorldRotation(ZEQuaternion::Identity);
		this->SetLocalRotation(ZEQuaternion::Identity);

		//for (int i=0;i<Meshes.GetCount();i++)
		//{
		//	Meshes[i].SetLocalPosition(ZEVector3(0,0,0));
		//	Meshes[i].SetLocalRotation(Meshes[i].GetPhysicalBody()->GetOrientation());
		//}
		//ZEVector3 ewrt = Meshes[0].GetPhysicalBody()->GetPosition();
		//this->SetWorldPosition(Meshes[0].GetPhysicalBody()->GetPosition());
		//this->SetWorldRotation(Meshes[0].GetPhysicalBody()->GetOrientation());
		//this->SetLocalPosition(Meshes[0].GetPhysicalBody()->GetPosition());
		//this->SetLocalRotation(Meshes[0].GetPhysicalBody()->GetOrientation());

		//this->SetWorldPosition(ZEVector3::Zero);
		//this->SetLocalPosition(ZEVector3::Zero);
	}*/
}

ZEModel::ZEModel()
{
	AnimationFrame = 0.0f;
	AnimationStartFrame = 0.0f;
	AnimationEndFrame = 0.0f;
	ModelResource = NULL;
	AnimationSpeed = 1.0f;
	Visibility = true;
	AutoLOD = true;
	ActiveLOD = 0;
	BoundingBoxDirtyFlag = true;
	BoneTransformsDirtyFlag = true;
	AnimationState = ZE_MAS_STOPPED;
	Animation = NULL;	
	AnimationFrame = 0;
	AnimationLooping = false;
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
