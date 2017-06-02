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

#include "ZEMDResource.h"
#include "ZEModelBone.h"
#include "ZEModelMesh.h"
#include "ZEModelHelper.h"
#include "ZEModelAnimation.h"
#include "ZEModelAnimationTrack.h"
#include "ZEModelIKChain.h"
#include "ZETypes.h"
#include "ZECommon.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEPointer/ZEPointer.h"
#include "ZERenderer/ZERNCommand.h"
#include "ZEGraphics/ZEGRCanvas.h"
#include "ZEExport.ZEEngine.h"

class ZEPhysicalRigidBody;
class ZEPhysicalJoint;
class ZEPhysicalBoxShape;
class ZERNDrawParameters;
class ZERNStandardMaterial;
class ZERNSimpleMaterial;

class ZE_EXPORT_ZEENGINE ZEModel : public ZEEntity
{	
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZEModel)
	friend class ZEPhysicalEnvironment;
	friend class ZEModelBone;
	friend class ZEModelMesh;
	friend class ZEModelAnimationTrack;
	friend class ZEModelHelper;
	friend class ZEModelDraw;
	private:
		ZEString								ModelFileName;

		ZEList2<ZEModelMesh>					Meshes;
		ZEList2<ZEModelBone>					Bones;
		ZEList2<ZEModelHelper>					Helpers;
		ZEList2<ZEModelIKChain>					IKChains;

		ZEModelAnimationType					AnimationType;
		ZEModelAnimationUpdateMode				AnimationUpdateMode;
		ZEList2<ZEModelAnimationTrack>			AnimationTracks;
		ZEModelAnimationTrack*					AnimationTrack;

		bool									DirtyConstantBufferSkin;
		ZEHolder<ZEGRBuffer>					ConstantBufferBoneTransforms;

		ZEHolder<const ZEGRBuffer>				VertexBuffers[ZEMD_VT_COUNT];
		ZEHolder<const ZEGRBuffer>				IndexBuffers[ZEMD_VIT_COUNT];

		mutable bool							DirtyBoundingBox;
		bool									BoundingBoxIsUserDefined;

		bool									LODTransitionOnVisible;

		ZERSHolder<const ZEMDResource>			Resource;

		void									CalculateBoundingBox() const;

		virtual void							UpdateConstantBufferBoneTransforms();
		virtual void							ParentVisibleChanged();
		virtual void							SetScene(ZEScene* Scene);

		virtual void							ChildBoundingBoxChanged();
		virtual void							LocalTransformChanged();
		virtual void							ParentTransformChanged();
		void									AnimationStateChanged();
		virtual void							BoundingBoxChanged();

		virtual ZEEntityResult					LoadInternal();
		virtual ZEEntityResult					UnloadInternal();

												ZEModel();
		virtual									~ZEModel();

	public:
		const ZEList2<ZEModelMesh>&				GetMeshes();
		ZEModelMesh*							GetMesh(ZEUInt32 Id);
		ZEModelMesh*							GetMesh(const char* Name);
		void									AddMesh(ZEModelMesh* Mesh);
		void									RemoveMesh(ZEModelMesh* Mesh);

		const ZEList2<ZEModelBone>&				GetBones();
		ZEModelBone*							GetBone(ZEUInt32 Id);
		ZEModelBone*							GetBone(const char* Name);
		void									AddBone(ZEModelBone* Bone);
		void									RemoveBone(ZEModelBone* Bone);

		const ZEList2<ZEModelHelper>&			GetHelpers();
		ZEModelHelper*							GetHelper(ZEUInt32 Id);
		ZEModelHelper*							GetHelper(const char* Name);
		void									AddHelper(ZEModelHelper* Helper);
		void									RemoveHelper(ZEModelHelper* Helper);

		const ZEList2<ZEModelAnimationTrack>&	GetAnimationTracks();
		void									AddAnimationTrack(ZEModelAnimationTrack* Track);
		void									RemoveAnimationTrack(ZEModelAnimationTrack* Track);

		void									SetAnimationTrack(ZEModelAnimationTrack* Track);
		ZEModelAnimationTrack*					GetAnimationTrack();

		void									SetUserDefinedBoundingBoxEnabled(bool Value);
		virtual const ZEAABBox&					GetBoundingBox() const;
		virtual const ZEAABBox&					GetWorldBoundingBox() const;

		void									SetModelFile(const ZEString& ModelFile);
		const ZEString&							GetModelFile() const;

		void									SetModelResource(ZERSHolder<const ZEMDResource> ModelResource);	
		ZERSHolder<const ZEMDResource>			GetModelResource() const;

		void									SetAnimationType(ZEModelAnimationType AnimationType);
		ZEModelAnimationType					GetAnimationType();

		void									SetAnimationUpdateMode(ZEModelAnimationUpdateMode AnimationUpdateMode);
		ZEModelAnimationUpdateMode				GetAnimationUpdateMode();

		void									SetVertexBuffer(const ZEGRBuffer* VertexBuffer, ZEMDVertexType VertexType);
		const ZEGRBuffer*						GetVertexBuffer(ZEMDVertexType VertexType) const;
		
		void									SetIndexBuffer(const ZEGRBuffer* IndexBuffer, ZEMDVertexIndexType IndexType);
		const ZEGRBuffer*						GetIndexBuffer(ZEMDVertexIndexType IndexType) const;

		void									SetLODTransitionOnVisible(bool Enabled);
		bool									GetLODTransitionOnVisible() const;

		virtual void							Tick(float ElapsedTime);

		virtual bool							PreRender(const ZERNPreRenderParameters* Parameters);
		virtual void							RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters);

		static ZEModel*							CreateInstance();
}
ZEMT_ATTRIBUTE(ZEDEditor.ObjectWrapper.Icon, "#R:/ZEDEditor/Icons/ZEDObjectWrapper/ZEModel.png")
ZEMT_ATTRIBUTE_PROPERTY("ModelFile", "ZEDEditor.PropertyEditor.Semantic", "Asset", "ZEDModelAsset");
