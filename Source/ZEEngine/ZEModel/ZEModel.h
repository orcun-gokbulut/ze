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

#include "ZEGame/ZEEntity.h"

#include "ZEModelResource.h"
#include "ZEModelBone.h"
#include "ZEModelMesh.h"
#include "ZEModelHelper.h"
#include "ZEModelAnimation.h"
#include "ZEModelAnimationTrack.h"
#include "ZEModelIKChain.h"

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"

class ZEPhysicalRigidBody;
class ZEPhysicalJoint;

class ZEModel : public ZEEntity
{	
	ZE_OBJECT
	friend class ZEPhysicalEnvironment;
	friend class ZEModelBone;
	friend class ZEModelMesh;
	friend class ZEModelAnimationTrack;
	friend class ZEModelHelper;
	friend class ZEModelDebugDrawer;
	friend class ZEModelMeshLOD;

	private:
		const ZEModelResource*				ModelResource;
		ZEArray<ZEModelBone*>				Skeleton;
		ZEArray<ZERNCommand>				LODRenderCommands;
		
		ZEArray<ZEModelMesh>				Meshes;
		ZEArray<ZEModelBone>				Bones;
		ZEArray<ZEModelHelper>				Helpers;

		ZEPhysicalRigidBody*				ParentlessBoneBody;
		ZEPhysicalBoxShape*					ParentlessBoneShape;
		ZEVector3							ParentlessBoneBodyPosition;

		bool								DirtyConstantBufferSkin;
		ZEHolder<ZEGRConstantBuffer>		ConstantBufferBoneTransforms;

		ZEUInt								ActiveLOD;
		bool								Visibility;
		bool								AutoLOD;
		bool								PhysicsEnabled;

		ZEModelAnimationType				AnimationType;
		ZEModelAnimationUpdateMode			AnimationUpdateMode;

		ZEArray<ZEModelAnimationTrack>		AnimationTracks;

		mutable bool						DirtyBoundingBox;
		bool								BoundingBoxIsUserDefined;

		ZERNCommand							RenderCommand;

		void								CalculateBoundingBox() const;
		void								UpdateTransforms();
	
		void								LoadModelResource();

		virtual void						UpdateConstantBufferBoneTransforms();

		virtual void						ChildBoundingBoxChanged();
		virtual void						LocalTransformChanged();
		virtual void						ParentTransformChanged();

	protected:
		virtual bool						InitializeSelf();
		virtual bool						DeinitializeSelf();

											ZEModel();
		virtual								~ZEModel();

	public:
		ZEArray<ZEModelIKChain>				IKChains;

		virtual ZEDrawFlags					GetDrawFlags() const;
		void								SetUserDefinedBoundingBoxEnabled(bool Value);
		virtual const ZEAABBox&				GetBoundingBox() const;
		virtual const ZEAABBox&				GetWorldBoundingBox() const;

		void								SetModelFile(const ZEString& ModelFile);
		const ZEString&						GetModelFile() const;

		void								SetModelResource(const ZEModelResource* ModelResource);	
		const ZEModelResource*				GetModelResource();
		
		const ZEArray<ZEModelBone*>&		GetSkeleton();

		ZEArray<ZEModelBone>&				GetBones();
		const ZEArray<ZEModelMesh>&			GetMeshes();
		const ZEArray<ZEModelHelper>&		GetHelpers();

		ZEModelBone*						GetBone(const char* Name);
		ZEModelMesh*						GetMesh(ZESize Index);
		ZEModelMesh*						GetMesh(const char* Name);
		ZEModelHelper*						GetHelper(const char* Name);

		void								SetAnimationType(ZEModelAnimationType AnimationType);
		ZEModelAnimationType				GetAnimationType();

		void								SetAnimationUpdateMode(ZEModelAnimationUpdateMode AnimationUpdateMode);
		ZEModelAnimationUpdateMode			GetAnimationUpdateMode();

		ZEArray<ZEModelAnimationTrack>&		GetAnimationTracks();

		void								SetAutoLOD(bool Enabled);
		bool								GetAutoLOD();

		void								SetActiveLOD(ZEUInt LOD);
		ZEUInt								GetActiveLOD();

		void								SetPhysicsEnabled(bool Enabled);
		bool								GetPhysicsEnabled();

		void								Tick(float ElapsedTime);
		
		virtual bool						PreRender(const ZERNPreRenderParameters* Parameters);
		virtual void						Render(const ZERNRenderParameters* RenderParameters, const ZERNCommand* Command);

		void								TransformChangeEvent(ZEPhysicalObject* PhysicalObject, ZEVector3 NewPosition, ZEQuaternion NewRotation);	
		void								LinkParentlessBones(ZEModelBone* ParentlessBone);
		virtual void						RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters);
		
		static ZEModel*						CreateInstance();
}
ZE_META_ATTRIBUTE_PROPERTY("ModelFile", "ZEDEditor.PropertyEditor.Semantic", "Asset", "ZEModelAsset");
