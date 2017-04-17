//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelBone.h
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

#include "ZEMeta/ZEObject.h"
#include "ZEDestroyable.h"

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEFlags.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEModelAnimation.h"
#include "ZEGame/ZERayCast.h"
#include "ZEExport.ZEEngine.h"

ZEMT_FORWARD_DECLARE(ZEModel);
ZEMT_FORWARD_DECLARE(ZEAABBox);

class ZE_EXPORT_ZEENGINE ZEModelBone : public ZEObject, public ZEDestroyable
{
	ZE_OBJECT
	friend class ZEModel;
	friend class ZEModelIKChain;
	private:
		mutable ZEFlags							DirtyFlags;

		ZEModel*								Model;
		ZELink<ZEModelBone>						ModelLink;
		ZEString								Name;
		ZEModelBone*							Parent;
		ZELink<ZEModelBone>						ParentLink;
		
		ZEVector3								InitialPosition;
		ZEQuaternion							InitialRotation;

		ZEVector3								Position;
		ZEQuaternion							Rotation;

		mutable ZEAABBox						BoundingBox;
		mutable ZEAABBox						ModelBoundingBox;
		mutable ZEAABBox						WorldBoundingBox;
		
		mutable ZEMatrix4x4						InitialTransform;
		mutable ZEMatrix4x4						InvInitialTransform;
		mutable ZEMatrix4x4						InitialModelTransform;
		mutable ZEMatrix4x4						InvInitialModelTransform;
		mutable ZEMatrix4x4						Transform;
		mutable ZEMatrix4x4						InvTransform;
		mutable ZEMatrix4x4						ModelTransform;
		mutable ZEMatrix4x4						InvModelTransform;
		mutable ZEMatrix4x4						WorldTransform;
		mutable ZEMatrix4x4						InvWorldTransform;
		mutable ZEMatrix4x4						VertexTransform; // ModelTransform (ForwardTransfomr) * InvInitialModelTransform (InverseTransform) * Vertex

		ZEModelAnimationType					AnimationType;

		ZEList2<ZEModelBone>					ChildBones;
		
		void									SetModel(ZEModel* Model);
		void									SetParent(ZEModelBone* ParentBone);

		void									ParentChanged();
		void									TransformChangedInitialLocal();
		void									TransformChangedInitialModel();
		void									TransformChangedLocal();
		void									TransformChangedModel();
		void									TransformChangedWorld();

		//void									LocalTransformChanged();
		//void									ParentTransformChanged();

		bool									Load(const ZEMDResourceBone* Resource);
		bool									Unload();

												ZEModelBone();
		virtual									~ZEModelBone();

	public:
		ZEModel*								GetModel() const;
		ZEModelBone*							GetParent() const;

		void									SetName(const ZEString& Name);
		const ZEString&							GetName() const;

		void									SetBoundingBox(const ZEAABBox& BoundingBox);
		const ZEAABBox&							GetBoundingBox() const;

		const ZEAABBox&							GetModelBoundingBox() const;
		const ZEAABBox&							GetWorldBoundingBox() const;

		const ZEMatrix4x4&						GetTransform() const;
		const ZEMatrix4x4&						GetInvTransform() const;

		const ZEMatrix4x4&						GetModelTransform() const;
		const ZEMatrix4x4&						GetInvModelTransform() const;

		const ZEMatrix4x4&						GetWorldTransform() const;		
		const ZEMatrix4x4&						GetInvWorldTransform() const;		

		const ZEMatrix4x4&						GetInitialTransform() const;
		const ZEMatrix4x4&						GetInvInitialTransform() const;

		const ZEMatrix4x4&						GetInitialModelTransform() const;
		const ZEMatrix4x4&						GetInvInitialModelTransform() const;

		const ZEMatrix4x4&						GetVertexTransform() const;	

		void									SetInitialPosition(const ZEVector3& Position);
		const ZEVector3&						GetInitialPosition() const;

		void									SetInitialRotation(const ZEQuaternion& Rotation);
		const ZEQuaternion&						GetInitialRotation() const;

		void									SetPosition(const ZEVector3& Position);
		const ZEVector3&						GetPosition() const;

		void									SetRotation(const ZEQuaternion& Rotation);
		const ZEQuaternion&						GetRotation() const;

		void									SetModelPosition(const ZEVector3& ModelPosition);
		ZEVector3								GetModelPosition() const;

		void									SetModelRotation(const ZEQuaternion& ModelRotation);
		ZEQuaternion							GetModelRotation() const;

		void									SetWorldPosition(const ZEVector3& WorldPosition);
		ZEVector3								GetWorldPosition() const;

		void									SetWorldRotation(const ZEQuaternion& WorldRotation);
		ZEQuaternion							GetWorldRotation() const;

		void									SetAnimationType(ZEModelAnimationType AnimationType);
		ZEModelAnimationType					GetAnimationType() const;

		const ZEList2<ZEModelBone>&				GetChildBones() const;
		void									AddChildBone(ZEModelBone* Bone);
		void									RemoveChildBone(ZEModelBone* Bone);

		void									RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters);

		static ZEModelBone*						CreateInstance();
};
