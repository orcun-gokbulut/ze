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

#include "ZEMeta/ZEObject.h"

#include "ZETypes.h"
#include "ZEModelAnimation.h"
#include "ZEModelMeshLOD.h"
#include "ZEModelResource.h"
#include "ZEGame/ZERayCast.h"
#include "ZERenderer/ZERNCommand.h"

class ZERNPreRenderParameters;
class ZEPhysicalCloth;

ZE_META_FORWARD_DECLARE(ZEModel, "ZEModel.h")

class ZEModelMesh : public ZEObject
{
	ZE_OBJECT
	friend class ZEModel;
	friend class ZEModelMeshLOD;

	private:
		ZEModel*							Owner;

		const ZEModelResourceMesh*			MeshResource;
		ZEModelMesh*						ParentMesh;
		ZEArray<ZEModelMesh*>				ChildMeshes;

		ZEVector3							Position;
		ZEVector3							Scale;
		ZEQuaternion						Rotation;

		mutable ZEFlags						DirtyFlags;
		mutable ZEAABBox					LocalBoundingBox;
		mutable ZEAABBox					ModelBoundingBox;
		mutable ZEAABBox					WorldBoundingBox;
		mutable ZEMatrix4x4					LocalTransform;
		mutable ZEMatrix4x4					ModelTransform;
		mutable ZEMatrix4x4					WorldTransform;
		mutable ZEMatrix4x4					InvWorldTransform;

		ZEHolder<ZEGRConstantBuffer>		ConstantBuffer;

		bool								PhysicsEnabled;
		ZEPhysicalRigidBody*				PhysicalBody;
		ZEPhysicalCloth*					PhysicalCloth;

		bool								AutoLOD;
		ZEUInt								ActiveLOD;

		bool								DrawOrderIsUserDefined;
		ZEUInt8								UserDefinedDrawOrder;

		ZEModelAnimationType				AnimationType;
		bool								Visible;		

		ZEArray<ZEModelMeshLOD>				LODs;

		ZEArray<ZEPlane>					ClippingPlanes;
		ZERNCommand							RenderCommand;
		
		void								UpdateConstantBuffer();

		void								LocalTransformChanged();
		void								ParentTransformChanged();

	public:
		ZEModelMesh*						GetParentMesh();
		const ZEArray<ZEModelMesh*>&		GetChildMeshes();
		const char*							GetName();

		ZEPhysicalRigidBody*				GetPhysicalBody();
		ZEPhysicalCloth*					GetPhysicalCloth();

		const ZEAABBox&						GetLocalBoundingBox() const;
		const ZEAABBox&						GetModelBoundingBox() const;
		const ZEAABBox&						GetWorldBoundingBox() const;
		const ZEMatrix4x4&					GetLocalTransform() const;
		const ZEMatrix4x4&					GetModelTransform() const;
		const ZEMatrix4x4&					GetWorldTransform() const;
		const ZEMatrix4x4&					GetInvWorldTransform() const;

		void								SetLocalPosition(const ZEVector3& LocalPosition);
		const ZEVector3&					GetLocalPosition() const;

		void								SetLocalRotation(const ZEQuaternion& LocalRotation);
		const ZEQuaternion&					GetLocalRotation() const;

		void								SetLocalScale(const ZEVector3& LocalScale);
		const ZEVector3&					GetLocalScale() const;

		void								SetModelPosition(const ZEVector3& ModelPosition);
		const ZEVector3						GetModelPosition() const;

		void								SetModelRotation(const ZEQuaternion& ModelRotation);
		const ZEQuaternion					GetModelRotation() const;

		void								SetModelScale(const ZEVector3& ModelScale);
		const ZEVector3						GetModelScale() const;

		void								SetWorldPosition(const ZEVector3& WorldPosition);
		const ZEVector3						GetWorldPosition() const;

		void								SetWorldRotation(const ZEQuaternion& WorldRotation);
		const ZEQuaternion					GetWorldRotation() const;

		void								SetWorldScale(const ZEVector3& WorldScale);
		const ZEVector3						GetWorldScale() const;

		void								SetAnimationType(ZEModelAnimationType AnimationType);
		ZEModelAnimationType				GetAnimationType();

		void								SetActiveLOD(ZEUInt LOD);
		ZEUInt								GetActiveLOD();

		void								SetAutoLOD(bool Enabled);
		bool								GetAutoLOD();

		ZEArray<ZEModelMeshLOD>&			GetLODs();

		void								SetVisible(bool Visible);
		bool								GetVisible();

		void								AddChild(ZEModelMesh* Mesh);
		void								RemoveChild(ZEModelMesh* Mesh);

		void								SetPhysicsEnabled(bool Enabled);
		bool								GetPhysicsEnabled() const;

		void								SetClippingPlaneCount(ZESize Count);
		ZESize								GetClippingPlaneCount();
		void								SetClippingPlane(ZESize Index, const ZEPlane& Plane);
		const ZEPlane&						GetClippingPlane(ZESize Index);

		void								SetCustomDrawOrderEnabled(bool Enabled);
		void								SetCustomDrawOrder(ZEUInt8 DrawOrder);
		ZEUInt8								GetCustomDrawOrder();

		void								Initialize(ZEModel* Model, const ZEModelResourceMesh* MeshResource);
		void								Deinitialize();

		bool								PreRender(const ZERNPreRenderParameters* Parameters);

		void								RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters);

											ZEModelMesh();
											~ZEModelMesh();
};
