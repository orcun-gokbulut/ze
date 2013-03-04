//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelMesh.h
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
#ifndef	__ZE_MODEL_MESH_H__
#define __ZE_MODEL_MESH_H__

#include "ZETypes.h"
#include "ZEModelAnimation.h"
#include "ZEModelMeshLod.h"
#include "ZEGame/ZEEntity.h"
#include "ZEGraphics/ZERenderCommand.h"
#include "ZEModelResource.h"
#include "ZEMeta/ZEObject.h"

ZE_META_FORWARD_DECLARE(ZEModel, "ZEModel.h")
ZE_META_FORWARD_DECLARE(ZEPhysicalCloth, "ZEPhysics/ZEPhysicalCloth.h")

ZE_CLASS(ZEModelMesh)

class ZEModelMesh : public ZEObject
{
	friend class ZEModel;

	ZE_OBJECT

	private:
		ZEModel*							Owner;

		const ZEModelResourceMesh*			MeshResource;
		ZEAABBox							LocalBoundingBox;
		ZEAABBox							ModelBoundingBox;
		ZEAABBox							WorldBoundingBox;

		ZEVector3							Position;
		ZEVector3							Scale;
		ZEQuaternion						Rotation;

		ZEMatrix4x4							LocalTransform;
/*		ZEMatrix4x4							ModelTransform;*/
		ZEMatrix4x4							WorldTransform;

		bool								PhysicsEnabled;
		ZEPhysicalRigidBody*				PhysicalBody;
		ZEPhysicalCloth*					PhysicalCloth;

		bool								AutoLOD;
		ZEUInt								ActiveLOD;

		ZEModelAnimationType				AnimationType;
		bool								Visible;		

		ZEArray<ZEModelMeshLOD>				LODs;

		void								OnTransformChanged();

	public:
		const char*							GetName();
		ZEPhysicalRigidBody*				GetPhysicalBody() { return PhysicalBody; }
		ZEPhysicalCloth*					GetPhysicalCloth() { return PhysicalCloth; }
		ZEModelMeshLOD*						GetFirstLOD() { return &LODs[0]; }

		const ZEAABBox&						GetLocalBoundingBox();
		const ZEAABBox&						GetModelBoundingBox();
		const ZEAABBox&						GetWorldBoundingBox();

		const ZEMatrix4x4&					GetLocalTransform();
/*		const ZEMatrix4x4&					GetModelTransform();*/
		const ZEMatrix4x4&					GetWorldTransform();
				
		void								SetLocalPosition(const ZEVector3& LocalPosition);
		const ZEVector3&					GetLocalPosition();

		void								SetLocalRotation(const ZEQuaternion& LocalRotation);
		const ZEQuaternion&					GetLocalRotation();

		void								SetLocalScale(const ZEVector3& LocalScale);
		const ZEVector3&					GetLocalScale();

		void								SetAnimationType(ZEModelAnimationType AnimationType);
		ZEModelAnimationType				GetAnimationType();

		void								SetActiveLOD(ZEUInt LOD);
		ZEUInt								GetActiveLOD();

		void								SetAutoLOD(bool Enabled);
		bool								GetAutoLOD();

		void								SetVisible(bool Visible);
		bool								GetVisible();

		void								SetPhysicsEnabled(bool Enabled);
		bool								GetPhysicsEnabled();

		void								Initialize(ZEModel* Model, const ZEModelResourceMesh* MeshResource);
		void								Deinitialize();

		void								Draw(ZEDrawParameters* DrawParameters);

											ZEModelMesh();
											~ZEModelMesh();
};
#endif
