//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Model.h
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

#pragma once
#ifndef	__ZE_MODEL_H__
#define __ZE_MODEL_H__

#include "ZEDS/ZEDS.h"
#include "ZEMath/ZEMath.h"
#include "Core/Component.h"
#include "Graphics/RenderList.h"
#include "ModelResource.h"
#include "Physics/PhysicsBody.h"

enum ZEModelAnimationState
{
	ZE_MAS_STATICFRAME,
	ZE_MAS_PLAYING,
	ZE_MAS_PAUSED,
	ZE_MAS_STOPPED
};

enum ZEModelAnimationType
{
	ZE_MAT_NOANIMATION,
	ZE_MAT_PREDEFINED,
	ZE_MAT_PHYSICAL
};

class ZEModel;
class ZEModelMesh;
class ZEModelMeshLOD
{
	private:
		ZEModel*							Owner;
		ZEModelMesh*						OwnerMesh;
		const ZEModelResourceMeshLOD*		LODResource;
		ZEVertexBuffer*						VertexBuffer;
		ZERenderList						RenderList;
		const ZEMaterial*					Material;
		bool								Skinned;

	public:
		void								ResetMaterial();
		void								SetMaterial(const ZEMaterial* Material);
		const ZEMaterial*					GetMaterial();

		bool								IsSkinned();

		void								Draw(ZERenderer* Renderer, const ZESmartArray<const ZERLLight*>& Lights);
		
		void								Initialize(ZEModel* Model, ZEModelMesh* Mesh,  const ZEModelResourceMeshLOD* LODResource);
		void								Deinitialize();

											ZEModelMeshLOD();
											~ZEModelMeshLOD();
};

class ZEModelMesh
{
	private:
		ZEModel*							Owner;
		const ZEModelResourceMesh*			MeshResource;
		ZEAABoundingBox						LocalBoundingBox;
		ZEAABoundingBox						ModelBoundingBox;
		ZEAABoundingBox						WorldBoundingBox;

		ZEVector3							Position;
		ZEVector3							Scale;
		ZEQuaternion						Rotation;

		ZEMatrix4x4							LocalTransform;
		ZEMatrix4x4							ModelTransform;
		ZEMatrix4x4							WorldTransform;

		bool								PhysicsEnabled;
		ZEPhysicsBody*						PhysicalBody;

		bool								AutoLOD;
		size_t								ActiveLOD;

		bool								UpdateLocalTransform;
		bool								UpdateModelTransform;
		bool								UpdateWorldTransform;
		bool								UpdateModelBoundingBox;
		bool								UpdateWorldBoundingBox;

		ZEModelAnimationType				AnimationType;
		bool								Visible;		

		ZEArray<ZEModelMeshLOD>				LODs;

	public:
		const char*							GetName();

		const ZEAABoundingBox&				GetLocalBoundingBox();
		const ZEAABoundingBox&				GetModelBoundingBox();
		const ZEAABoundingBox&				GetWorldBoundingBox();

		const ZEMatrix4x4&					GetLocalTransform();
		const ZEMatrix4x4&					GetModelTransform();
		const ZEMatrix4x4&					GetWorldTransform();
				
		void								SetLocalPosition(const ZEVector3& LocalPosition);
		const ZEVector3&					GetLocalPostion();

		void								SetLocalRotation(const ZEQuaternion& LocalRotation);
		const ZEQuaternion&					GetLocalRotation();

		void								SetLocalScale(const ZEVector3& LocalScale);
		const ZEVector3&					GetLocalScale();

		void								SetAnimationType(ZEModelAnimationType AnimationType);
		ZEModelAnimationType				GetAnimationType();

		void								ModelTransformChanged();
		void								ModelWorldTransformChanged();

		void								SetActiveLOD(size_t LOD);
		size_t								GetActiveLOD();

		void								SetAutoLOD(bool Enabled);
		bool								GetAutoLOD();

		void								SetVisible(bool Visible);
		bool								GetVisible();

		void								SetPhysicsEnabled(bool Enabled);
		bool								GetPhysicsEnabled();

		void								Initialize(ZEModel* Model,  const ZEModelResourceMesh* MeshResource);
		void								Deinitialize();

		void								Draw(ZERenderer* Renderer,  const ZESmartArray<const ZERLLight*>& Lights);

											ZEModelMesh();
											~ZEModelMesh();
};

class ZEModelBone
{
	private:
		ZEModel*							Owner;
		const ZEModelResourceBone*			BoneResource;
		ZEModelBone*						ParentBone;
		ZEArray<ZEModelBone*>				ChildBones;

		ZEVector3							RelativePosition;
		ZEQuaternion						RelativeRotation;

		ZEMatrix4x4							ModelTransform;
		ZEMatrix4x4							WorldTransform;
		ZEMatrix4x4							RelativeTransform;
		ZEMatrix4x4							LocalTransform;

		ZEMatrix4x4							VertexTransform;

		ZEAABoundingBox						ModelBoundingBox;
		ZEAABoundingBox						WorldBoundingBox;

		ZEDWORD								DirtyFlags;

		bool								UpdateModelBoundingBox;
		bool								UpdateWorldBoundingBox;
		bool								UpdateRelativeTransform;

		bool								UpdateLocalTransform;
		bool								UpdateVertexTransform;
		bool								UpdateModelTransform;
		bool								UpdateWorldTransform;

		void*								PhysicalBody;
		bool								PhysicsEnabled;

		ZEModelAnimationType				AnimationType;

	public:
		const ZEModelBone*					GetParentBone();
		const ZEArray<ZEModelBone*>			GetChildBones();
		const char*							GetName();

		const ZEAABoundingBox&				GetLocalBoundingBox();
		const ZEAABoundingBox&				GetModelBoundingBox();
		const ZEAABoundingBox&				GetWorldBoundingBox();

		const ZEMatrix4x4&					GetLocalTransform();
		const ZEMatrix4x4&					GetWorldTransform();		
		const ZEMatrix4x4&					GetModelTransform();

		const ZEMatrix4x4&					GetVertexTransform();
		const ZEMatrix4x4&					GetRelativeTransform();
		const ZEMatrix4x4&					GetInverseTransform();
		const ZEMatrix4x4&					GetForwardTransform();

		const ZEVector3&					GetAbsolutePostion();
		const ZEQuaternion&					GetAbsoluteRotation();

		const ZEVector3&					GetRelativePosition();
		void								SetRelativePosition(const ZEVector3& Position);

		const ZEQuaternion&					GetRelativeRotation();
		void								SetRelativeRotation(const ZEQuaternion& Rotation);

		void								SetAnimationType(ZEModelAnimationType AnimationType);
		ZEModelAnimationType				GetAnimationType();

		void								AddChild(ZEModelBone* Bone);
		void								RemoveChild(ZEModelBone* Bone);

		void								SetPhysicsEnabled(bool Enabled);
		bool								GetPhysicsEnabled();

		void								Initialize(ZEModel* Model, const ZEModelResourceBone* BoneResource);
		void								Deinitialize();

		void								ModelTransformChanged();
		void								ModelWorldTransformChanged();

											ZEModelBone();
											~ZEModelBone();
};

typedef ZEModelResourceAnimation ZEModelAnimation;

class ZEModel : public ZEComponent
{	
	friend class ZEPhysicalEnvironment;
	private:
		const ZEModelResource*				ModelResource;
		ZEArray<ZEMatrix4x4>				BoneTransforms;
		ZEArray<ZEModelBone>				Skeleton;
		ZEArray<ZERenderList>				LODRenderLists;
		
		ZEArray<ZEModelMesh>				Meshes;
		ZEArray<ZEModelBone>				Bones;

		bool								Visibility;
		bool								AutoLOD;
		size_t								ActiveLOD;
		bool								PhysicsEnabled;

		ZEModelAnimationType				AnimationType;
		ZEModelAnimationState				AnimationState;
		const ZEModelAnimation*				Animation;	

		float								AnimationFrame;
		float								AnimationStartFrame;
		float								AnimationEndFrame;
		bool								AnimationLooping;
		float								AnimationSpeed;

		ZEAABoundingBox						BoundingBox;

		bool								UpdateBoundingBox_;
		bool								UpdateBoneTransforms_;

		void								CalculateBoundingBox();		
		void								LocalTransformChanged();

		bool								DrawSkeleton;
		bool								DrawPhysicalBodies;
		bool								DrawPhysicalJoints;

		ZERenderList						SkeletonRenderList;
		ZEDefaultMaterial					SkeletonMaterial;
		ZECanvas							SkeletonVertexBuffer;

		ZERenderList						SkeletonPointsRenderList;
		ZEDefaultMaterial					SkeletonPointsMaterial;
		ZECanvas							SkeletonPointsVertexBuffer;
		
		static ZERenderList					PhysicalBodiesRenderList;
		static ZEDefaultMaterial			PhysicalBodiesMaterial;
		ZECanvas							PhysicalBodiesVertexBuffer;

		static ZERenderList					PhysicalJointsRenderList;
		static ZEDefaultMaterial			PhysicalJointsMaterial;
		ZECanvas							PhysicalJointsVertexBuffer;

	public:
		virtual	bool						IsDrawable();

		virtual const ZEAABoundingBox&		GetLocalBoundingBox();

		void								SetModelResource(const ZEModelResource* ModelResource);	
		const ZEModelResource*				GetModelResource();

		
		const ZEArray<ZEModelBone>&			GetSkeleton();
		ZEArray<ZEModelBone>&				GetBones();
		const ZEArray<ZEMatrix4x4>&			GetBoneTransforms();
		const ZEArray<ZEModelMesh>&			GetMeshes();
		const ZEArray<ZEModelAnimation>*	GetAnimations();

		void								SetAnimation(const ZEModelAnimation* Animation);
		void								SetAnimationById(size_t AnimationIndex);
		void								SetAnimationByName(const char* AnimationName);

		const ZEModelAnimation*				GetAnimation();

		void								SetAnimationState(ZEModelAnimationState State);
		ZEModelAnimationState				GetAnimationState();

		void								SetAnimationFrame(unsigned int Frame);	
		size_t								GetAnimationFrame();

		void								SetAnimationType(ZEModelAnimationType AnimationType);
		ZEModelAnimationType				GetAnimationType();

		void								SetAnimationSpeed(float Factor);
		float								GetAnimationSpeed();

		void								SetAnimationLooping(bool Looping);
		bool								GetAnimationLooping();

		void								SetAutoLOD(bool Enabled);
		bool								GetAutoLOD();

		void								SetActiveLOD(unsigned int LOD);
		int									GetActiveLOD();

		void								SetPhysicsEnabled(bool Enabled);
		bool								GetPhysicsEnabled();

		void								SetStaticPose(const ZEModelAnimation* Animation, unsigned int Frame);
		void								SetStaticPoseByIndex(size_t AnimationIndex, unsigned int Frame);
		void								SetStaticPoseByName(const char* AnimationName, unsigned int Frame);

		void								PlayAnimation(const ZEModelAnimation* Animation, unsigned int StartFrame = 0, unsigned int EndFrame = 0);
		void								PlayAnimationByName(const char* AnimationName, unsigned int StartFrame = 0, unsigned int EndFrame = 0);
		void								PlayAnimationByIndex(size_t AnimationIndex, unsigned int StartFrame = 0, unsigned int EndFrame = 0);
		void								ResumeAnimation();
		void								PauseAnimation();
		void								StopAnimation();

		virtual void						SetLocalPosition(const ZEVector3& NewPosition);
		virtual void						SetLocalRotation(const ZEQuaternion& NewRotation);
		virtual void						SetLocalScale(const ZEVector3& NewScale);

		void								Tick(float ElapsedTime);
		void								Draw(ZERenderer* Renderer,  const ZESmartArray<const ZERLLight*>& Lights);

		virtual void						OwnerWorldTransformChanged();
		void								UpdateBoundingBox();
		void								UpdateBoneTransforms();

		void								SetDrawSkeleton(bool Enabled);
		bool								GetDrawSkeleton();

		void								SetDrawPhysicalBodies(bool Enabled);
		bool								GetDrawPhysicalBodies();

		void								SetDrawPhysicalJoints(bool Enabled);
		bool								GetDrawPhysicalJoints();

											ZEModel();
											~ZEModel();
};
#endif

