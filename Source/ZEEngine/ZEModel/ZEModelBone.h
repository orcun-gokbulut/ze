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
#include "ZEModelAnimation.h"

class ZEModel;
class ZEModelBone
{
	friend class ZEModel;
	friend class ZEModelIKChain;
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
		ZEMatrix4x4							InitialRelativeTransform;				

		ZEMatrix4x4							VertexTransform;

		ZEAABBox							ModelBoundingBox;
		ZEAABBox							WorldBoundingBox;

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

		const ZEAABBox&						GetLocalBoundingBox();
		const ZEAABBox&						GetModelBoundingBox();
		const ZEAABBox&						GetWorldBoundingBox();

		const ZEMatrix4x4&					GetLocalTransform();
		const ZEMatrix4x4&					GetWorldTransform();		
		const ZEMatrix4x4&					GetModelTransform();

		const ZEVector3&					GetInitialRelativePosition();
		const ZEQuaternion&					GetInitialRelativeRotation();
		
		const ZEVector3&					GetInitialLocalPosition();
		const ZEQuaternion&					GetInitialLocalRotation();

		const ZEMatrix4x4&					GetInitialRelativeTransform();
		const ZEMatrix4x4&					GetInitialLocalTransform();

		const ZEMatrix4x4&					GetInverseTransform();
		const ZEMatrix4x4&					GetRelativeTransform();
		const ZEMatrix4x4&					GetForwardTransform();
		const ZEMatrix4x4&					GetVertexTransform();

		const ZEVector3&					GetRelativePosition();
		void								SetRelativePosition(const ZEVector3& Position);

		const ZEQuaternion&					GetRelativeRotation();
		void								SetRelativeRotation(const ZEQuaternion& Rotation);

		const ZEVector3						GetModelPosition();
		const ZEQuaternion					GetModelRotation();

		const ZEVector3						GetLocalPosition();
		const ZEQuaternion					GetLocalRotation();

		void								SetLocalRotation(const ZEQuaternion& Rotation);

		const ZEVector3						GetWorldPosition();
		const ZEQuaternion					GetWorldRotation();

		void								SetAnimationType(ZEModelAnimationType AnimationType);
		ZEModelAnimationType				GetAnimationType();

		void								AddChild(ZEModelBone* Bone);
		void								RemoveChild(ZEModelBone* Bone);

		void								SetPhysicsEnabled(bool Enabled);
		bool								GetPhysicsEnabled();

		void								Initialize(ZEModel* Model, const ZEModelResourceBone* BoneResource);
		void								Deinitialize();

											ZEModelBone();
											~ZEModelBone();
};
#endif
