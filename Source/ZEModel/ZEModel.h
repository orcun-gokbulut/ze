//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModel.h
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

#include "ZEDS\ZEArray.h"
#include "ZEGame\ZEComponent.h"
#include "ZEGraphics\ZERenderOrder.h"
#include "ZEGraphics\ZECanvas.h"
#include "ZEModelResource.h"
#include "ZEModelBone.h"
#include "ZEModelMesh.h"
#include "ZEModelAnimation.h"

#define ZE_MDF
class ZEQuaternion;
class ZEMatrix4x4;
class ZEFixedMaterial;
class ZESimpleMaterial;

class ZEPhysicalRigidBody;
class ZEPhysicalJoint;

struct ZEDrawParameters;

class ZEModel : public ZEComponent
{	
	friend class ZEPhysicalEnvironment;
	private:
		const ZEModelResource*				ModelResource;
		ZEArray<ZEModelBone*>				Skeleton;
		ZEArray<ZERenderOrder>				LODRenderOrders;
		
		ZEArray<ZEModelMesh>				Meshes;
		ZEArray<ZEModelBone>				Bones;

		ZEPhysicalRigidBody*				ParentlessBoneBody;
		ZEPhysicalBoxShape*					ParentlessBoneShape;

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

		bool								BoundingBoxDirtyFlag;
		bool								BoneTransformsDirtyFlag;

		void								CalculateBoundingBox();		
		void								LocalTransformChanged();

		bool								DrawSkeleton;
		bool								DrawPhysicalBodies;
		bool								DrawPhysicalJoints;

		struct
		{
			ZESimpleMaterial*				Material;
			ZECanvas						BonesCanvas;
			ZECanvas						BonePositionsCanvas;
			ZERenderOrder					BonesRenderOrder;
			ZERenderOrder					BonePositionsRenderOrder;
		} DebugDrawComponents;

		void								DebugDraw(ZERenderer* Renderer);

	public:
		virtual	ZEDWORD						GetDrawFlags() const;

		virtual const ZEAABoundingBox&		GetLocalBoundingBox() const;

		void								SetModelResource(const ZEModelResource* ModelResource);	
		const ZEModelResource*				GetModelResource();
		
		const ZEArray<ZEModelBone*>&		GetSkeleton();

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

		virtual void						SetPosition(const ZEVector3& NewPosition);
		virtual void						SetRotation(const ZEQuaternion& NewRotation);
		virtual void						SetScale(const ZEVector3& NewScale);

		void								Tick(float ElapsedTime);
		void								Draw(ZEDrawParameters* DrawParameters);
		void								BoneTransformChangeEvent(const ZEPhysicalTransformChangeEventArgument& TransformChange);

		void								LinkParentlessBones(ZEModelBone* ParentlessBone);

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





