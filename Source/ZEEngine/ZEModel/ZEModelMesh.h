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
#include "ZEDestroyable.h"

#include "ZEModelMeshLOD.h"
#include "ZEModelAnimation.h"

#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEPointer/ZEHolder.h"

class ZERNPreRenderParameters;
class ZEGRBuffer;
class ZERayCastParameters;
class ZERayCastReport;

ZEMT_FORWARD_DECLARE(ZEModel);

class ZEModelMesh : public ZEObject, public ZEDestroyable
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZEModelMesh)
	friend class ZEModel;
	friend class ZEModelDraw;
	private:
		ZEModel*								Model;
		ZELink<ZEModelMesh>						ModelLink;

		ZEString								Name;
		ZEModelMesh*							Parent;
		ZELink<ZEModelMesh>						ParentLink;

		ZEVector3								Position;
		ZEQuaternion							Rotation;
		ZEVector3								Scale;

		mutable ZEFlags							DirtyFlags;

		mutable ZEAABBox						BoundingBox;
		mutable ZEAABBox						ModelBoundingBox;
		mutable ZEAABBox						WorldBoundingBox;

		mutable ZEMatrix4x4						Transform;
		mutable ZEMatrix4x4						InvTransform;
		mutable ZEMatrix4x4						ModelTransform;
		mutable ZEMatrix4x4						InvModelTransform;
		mutable ZEMatrix4x4						WorldTransform;
		mutable ZEMatrix4x4						InvWorldTransform;

		ZEModelAnimationType					AnimationType;
		bool									Visible;

		ZEList2<ZEModelMesh>					ChildMeshes;
		ZEList2<ZEModelMeshLOD>					LODs;

		float									LODTransitionTime;
		float									LODTransitionElapsedTime;
		float									LODTransitionSpeed;
		bool									LODTransitionPlaying;
		ZEModelMeshLOD*							PrevLOD;
		ZEModelMeshLOD*							NextLOD;

		float									DrawOrder;
		bool									CustomDrawOrderEnabled;
		ZEUInt8									CustomDrawOrder;
		ZEArray<ZEPlane>						ClippingPlanes;
		ZEHolder<ZEGRBuffer>					ConstantBuffer;

		const ZEMDResourceMesh*					Resource;

		void									SetModel(ZEModel* Model);
		void									SetParent(ZEModelMesh* Mesh);
		
		void									ParentChanged();
		void									TransformChangedLocal();
		void									TransformChangedModel();
		void									TransformChangedWorld();
		void									VisibilityChanged();

		void									UpdateConstantBuffer();

		bool									Load(const ZEMDResourceMesh* Resource);
		bool									Unload();

												ZEModelMesh();
		virtual									~ZEModelMesh();

	public:
		ZEModel*								GetModel() const;
		ZEModelMesh*							GetParent() const;

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

		void									SetPosition(const ZEVector3& Position);
		const ZEVector3&						GetPosition() const;

		void									SetRotation(const ZEQuaternion& Rotation);
		const ZEQuaternion&						GetRotation() const;

		void									SetScale(const ZEVector3& Scale);
		const ZEVector3&						GetScale() const;

		void									SetModelPosition(const ZEVector3& ModelPosition);
		ZEVector3								GetModelPosition() const;

		void									SetModelRotation(const ZEQuaternion& ModelRotation);
		ZEQuaternion							GetModelRotation() const;

		void									SetModelScale(const ZEVector3& ModelScale);
		ZEVector3								GetModelScale() const;

		void									SetWorldPosition(const ZEVector3& WorldPosition);
		ZEVector3								GetWorldPosition() const;

		void									SetWorldRotation(const ZEQuaternion& WorldRotation);
		ZEQuaternion							GetWorldRotation() const;

		void									SetWorldScale(const ZEVector3& WorldScale);
		ZEVector3								GetWorldScale() const;

		void									SetAnimationType(ZEModelAnimationType AnimationType);
		ZEModelAnimationType					GetAnimationType();

		void									SetVisible(bool Visible);
		bool									GetVisible() const;

		const ZEList2<ZEModelMeshLOD>&			GetLODs();
		void									AddLOD(ZEModelMeshLOD* LOD);
		void									RemoveLOD(ZEModelMeshLOD* LOD);

		const ZEList2<ZEModelMesh>&				GetChildMeshes();
		void									AddChildMesh(ZEModelMesh* Mesh);
		void									RemoveChildMesh(ZEModelMesh* Mesh);

		void									SetClippingPlaneCount(ZESize Count);
		ZESize									GetClippingPlaneCount();
		void									SetClippingPlane(ZESize Index, const ZEPlane& Plane);
		const ZEPlane&							GetClippingPlane(ZESize Index);

		void									SetCustomDrawOrderEnabled(bool Enabled);
		bool									GetCustomDrawOrderEnabled() const;

		void									SetCustomDrawOrder(ZEUInt8 DrawOrder);
		ZEUInt8									GetCustomDrawOrder() const;

		bool									PreRender(const ZERNPreRenderParameters* Parameters);
		void									RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters);

		static ZEModelMesh*						CreateInstance();
};
