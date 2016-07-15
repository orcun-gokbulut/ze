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
#include "ZEPointer/ZEPointer.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZERenderer/ZERNCommand.h"
#include "ZERenderer/ZECanvas.h"

class ZERNFixedMaterial;
class ZERNSimpleMaterial;

class ZEPhysicalRigidBody;
class ZEPhysicalJoint;
class ZEPhysicalBoxShape;
class ZERNDrawParameters;

class ZEModel : public ZEEntity
{	
	ZE_OBJECT
	friend class ZEPhysicalEnvironment;
	friend class ZEModelBone;
	friend class ZEModelMesh;
	friend class ZEModelAnimationTrack;
	friend class ZEModelHelper;
	friend class ZEModelDebugDrawer;

	private:
		ZEList2<ZEModelMesh>				Meshes;
		ZEList2<ZEModelBone>				Bones;
		ZEList2<ZEModelHelper>				Helpers;
		ZEList2<ZEModelIKChain>				IKChains;
		
		ZEModelAnimationType				AnimationType;
		ZEModelAnimationUpdateMode			AnimationUpdateMode;
		ZEList2<ZEModelAnimationTrack>		AnimationTracks;
		ZEPointer<ZEModelAnimationTrack>	AnimationTrack;

		bool								DirtyConstantBufferSkin;
		ZEHolder<ZEGRConstantBuffer>		ConstantBufferBoneTransforms;

		mutable bool						DirtyBoundingBox;
		bool								BoundingBoxIsUserDefined;

		ZEHolder<const ZEModelResource>		Resource;

		void								CalculateBoundingBox() const;

		virtual void						UpdateConstantBufferBoneTransforms();

		virtual void						ChildBoundingBoxChanged();
		virtual void						LocalTransformChanged();
		virtual void						ParentTransformChanged();

	protected:
		virtual ZEEntityResult				LoadInternal();
		virtual ZEEntityResult				UnloadInternal();

											ZEModel();
		virtual								~ZEModel();

	public:
		virtual ZEDrawFlags					GetDrawFlags() const;

		const ZEList2<ZEModelMesh>&			GetMeshes();
		ZEModelMesh*						GetMesh(ZEUInt32 Id);
		ZEModelMesh*						GetMesh(const char* Name);
		void								AddMesh(ZEModelMesh* Mesh);
		void								RemoveMesh(ZEModelMesh* Mesh);

		const ZEList2<ZEModelBone>&			GetBones();
		ZEModelBone*						GetBone(ZEUInt32 Id);
		ZEModelBone*						GetBone(const char* Name);
		void								AddBone(ZEModelBone* Bone);
		void								RemoveBone(ZEModelBone* Bone);

		const ZEList2<ZEModelHelper>&		GetHelpers();
		ZEModelHelper*						GetHelper(ZEUInt32 Id);
		ZEModelHelper*						GetHelper(const char* Name);
		void								AddHelper(ZEModelHelper* Helper);
		void								RemoveHelper(ZEModelHelper* Helper);

		const ZEList2<ZEModelAnimationTrack>& GetAnimationTracks();
		void								AddAnimationTrack(ZEModelAnimationTrack* Track);
		void								RemoveAnimationTrack(ZEModelAnimationTrack* Track);

		void								SetAnimationTrack(ZEModelAnimationTrack* Track);
		ZEModelAnimationTrack*				GetAnimationTrack();

		void								SetUserDefinedBoundingBoxEnabled(bool Value);
		virtual const ZEAABBox&				GetBoundingBox() const;
		virtual const ZEAABBox&				GetWorldBoundingBox() const;

		void								SetModelFile(const ZEString& ModelFile);
		const ZEString&						GetModelFile() const;

		void								SetModelResource(ZEHolder<const ZEModelResource> ModelResource);	
		ZEHolder<const ZEModelResource>		GetModelResource() const;

		void								SetAnimationType(ZEModelAnimationType AnimationType);
		ZEModelAnimationType				GetAnimationType();

		void								SetAnimationUpdateMode(ZEModelAnimationUpdateMode AnimationUpdateMode);
		ZEModelAnimationUpdateMode			GetAnimationUpdateMode();

		void								Tick(float ElapsedTime);
		
		virtual bool						PreRender(const ZERNPreRenderParameters* Parameters);
		virtual void						RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters);
		
		static ZEModel*						CreateInstance();
}
ZE_META_ATTRIBUTE_PROPERTY("ModelFile", "ZEDEditor.PropertyEditor.Semantic", "Asset", "ZEModelAsset");
