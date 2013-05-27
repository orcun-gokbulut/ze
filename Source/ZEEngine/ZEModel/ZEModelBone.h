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
#ifndef	__ZE_MODEL_BONE_H__
#define __ZE_MODEL_BONE_H__

#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMeta/ZEObject.h"
#include "ZEModelAnimation.h"
#include "ZEGame/ZERayCast.h"

ZE_META_FORWARD_DECLARE(ZEModel, "ZEModel.h")
ZE_META_FORWARD_DECLARE(ZEAABBox, "ZEMath/ZEAABBox.h")

ZE_CLASS(ZEModelBone)

class ZEModelBone : public ZEObject
{
	friend class ZEModel;
	friend class ZEModelIKChain;

	ZE_OBJECT

	private:
		ZEModel*							Owner;
		const ZEModelResourceBone*			BoneResource;
		ZEModelBone*						ParentBone;
		ZEArray<ZEModelBone*>				ChildBones;

		ZEVector3							Position;
		ZEQuaternion						Rotation;

		mutable ZEMatrix4x4					InitialLocalTransform;
		mutable ZEMatrix4x4					LocalTransform;
		mutable ZEMatrix4x4					ModelTransform;
		mutable ZEMatrix4x4					WorldTransform;
		mutable ZEMatrix4x4					InvWorldTransform;

		mutable ZEMatrix4x4					VertexTransform;

		mutable ZEAABBox					LocalBoundingBox;
		mutable ZEAABBox					ModelBoundingBox;
		mutable ZEAABBox					WorldBoundingBox;

		ZEPhysicalRigidBody*				PhysicalBody;
		ZEPhysicalJoint*					PhysicalJoint;

		bool								PhysicsEnabled;

		ZEModelAnimationType				AnimationType;

		void								OnTransformChanged();

	public:
		ZEModelBone*						GetParentBone();
		const ZEArray<ZEModelBone*>&		GetChildBones();
		const char*							GetName();
		ZEPhysicalRigidBody*				GetPhysicalBody();

		bool								IsRootBone();

		const ZEAABBox&						GetBoundingBox() const;
		const ZEAABBox&						GetModelBoundingBox() const;
		const ZEAABBox&						GetWorldBoundingBox() const;

		const ZEMatrix4x4&					GetTransform() const;
		const ZEMatrix4x4&					GetModelTransform() const;
		const ZEMatrix4x4&					GetWorldTransform() const;		
		const ZEMatrix4x4&					GetInvWorldTransform() const;		


		const ZEVector3&					GetInitialPosition() const;
		const ZEQuaternion&					GetInitialRotation() const;

		const ZEMatrix4x4&					GetInitialTransform() const;
		const ZEMatrix4x4&					GetInverseTransform() const;
		const ZEMatrix4x4&					GetForwardTransform() const;
		const ZEMatrix4x4&					GetVertexTransform() const;

		void								SetPosition(const ZEVector3& Position);
		const ZEVector3&					GetPosition() const;

		void								SetRotation(const ZEQuaternion& Rotation);
		const ZEQuaternion&					GetRotation() const;

		void								SetModelPosition(const ZEVector3& ModelPosition);
		const ZEVector3						GetModelPosition() const;

		void								SetModelRotation(const ZEQuaternion& ModelRotation);
		const ZEQuaternion					GetModelRotation() const;

		void								SetWorldPosition(const ZEVector3& WorldPosition);
		const ZEVector3						GetWorldPosition() const;

		void								SetWorldRotation(const ZEQuaternion& WorldRotation);
		const ZEQuaternion					GetWorldRotation() const;

		void								SetAnimationType(ZEModelAnimationType AnimationType);
		ZEModelAnimationType				GetAnimationType() const;

		void								AddChild(ZEModelBone* Bone);
		void								RemoveChild(ZEModelBone* Bone);

		void								SetPhysicsEnabled(bool Enabled);
		bool								GetPhysicsEnabled() const;

		void								Initialize(ZEModel* Model, const ZEModelResourceBone* BoneResource);
		void								Deinitialize();

		bool								RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters);

											ZEModelBone();
											~ZEModelBone();
};
#endif
