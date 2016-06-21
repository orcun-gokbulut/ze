//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEEntity.h
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

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEFlags.h"
#include "ZEDefinitions.h"
#include "ZEDS/ZEValue.h"
#include "ZEMath/ZEOBBox.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEBSphere.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZERayCast.h"

#include "ZEMeta/ZEObject.h"
#include "ZEMeta/ZEEnumerator.h"

class ZEScene;
class ZEDObjectWrapper;
class ZERNDrawParameters;
class ZERNPreRenderParameters;
class ZERNRenderParameters;
class ZERNCommand;
class ZEMLWriterNode;
class ZEMLReaderNode;


// ZEDrawFlags
typedef ZEFlags ZEDrawFlags;
#define ZE_DF_NONE								0
#define ZE_DF_DRAW								1
#define ZE_DF_DRAW_COMPONENTS					2
#define ZE_DF_LIGHT_SOURCE						4
#define ZE_DF_LIGHT_RECEIVER					8
#define ZE_DF_CULL								16
#define ZE_DF_CULL_COMPONENTS					32
#define ZE_DF_AUTO								64

ZE_ENUM(ZEEntityState)
{
	ZE_ES_NOT_INITIALIZED				= 0,
	ZE_ES_INITIALIZING					= 1,
	ZE_ES_INITIALIZED					= 2,
	ZE_ES_DEINITIALIZING				= 3
};

class ZEEntity : public ZEObject
{
	ZE_OBJECT
	friend class ZEScene;
	friend class ZEDEntityWrapper;
	private: 
		ZEEntity*								Owner;
		ZEScene*								OwnerScene;
		ZEDObjectWrapper*						Wrapper;

		ZEString								Name;
		ZEEntityState							State;
		ZEInt									EntityId;
		ZEVector3								Position;
		ZEQuaternion							Rotation;
		ZEVector3								Scale;
		bool									Enabled;
		bool									Visible;

		mutable ZEFlags							EntityDirtyFlags;
		mutable ZEMatrix4x4						Transform;
		mutable ZEMatrix4x4						InvTransform;
		mutable ZEMatrix4x4						WorldTransform;
		mutable ZEMatrix4x4						InvWorldTransform;
		mutable ZEAABBox						BoundingBox;
		mutable ZEAABBox						WorldBoundingBox;
		
		ZEArray<ZEEntity*>						Components;
		ZEArray<ZEEntity*>						ChildEntities;

		void									SetWrapper(ZEDObjectWrapper* Wrapper);

	protected:
		virtual bool							SetOwner(ZEEntity* Owner);
		void									SetOwnerScene(ZEScene* Scene);

		const ZEArray<ZEEntity*>&				GetComponents() const;
		bool									AddComponent(ZEEntity* Entity); 
		void									RemoveComponent(ZEEntity* Entity);
		void									ClearComponents();

		virtual void							LocalTransformChanged();
		virtual void							ParentTransformChanged();
		virtual void							BoundingBoxChanged();

		virtual bool							InitializeSelf();
		virtual bool							DeinitializeSelf();

												ZEEntity();
		virtual									~ZEEntity();

	public:
		virtual ZEEntity*						GetOwner() const;
		ZEScene*								GetOwnerScene() const;
		
		void									SetEntityId(ZEInt EntityId);
		ZEInt									GetEntityId() const;

		void									SetName(ZEString NewName);
		ZEString								GetName() const;

		virtual ZEDrawFlags						GetDrawFlags() const;

		const ZEArray<ZEEntity*>&				GetChildEntities() const;
		bool									AddChildEntity(ZEEntity* Entity);
		void									RemoveChildEntity(ZEEntity* Entity);
		void									ClearChildEntities();

		void									SetBoundingBox(const ZEAABBox& BoundingBox);
		virtual const ZEAABBox&					GetBoundingBox() const;
		virtual const ZEAABBox&					GetWorldBoundingBox() const;

		const ZEMatrix4x4&						GetTransform() const;
		const ZEMatrix4x4&						GetWorldTransform() const;
		const ZEMatrix4x4&						GetInvWorldTransform() const;

		bool									IsInitialized();
		ZEEntityState							GetState();

		virtual void							SetVisible(bool Visibility);
		bool									GetVisible() const;

		virtual void							SetEnabled(bool Enabled);
		bool									GetEnabled() const;

		virtual void							SetPosition(const ZEVector3& NewPosition);
		const ZEVector3&						GetPosition() const;
		void									SetWorldPosition(const ZEVector3& NewPosition);
		const ZEVector3							GetWorldPosition() const;

		virtual void							SetRotation(const ZEQuaternion& NewRotation);
		const ZEQuaternion&						GetRotation() const;
		void									SetWorldRotation(const ZEQuaternion& NewRotation);
		const ZEQuaternion						GetWorldRotation() const;

		virtual void							SetScale(const ZEVector3& NewScale);
		const ZEVector3&						GetScale() const;
		void									SetWorldScale(const ZEVector3& NewScale);
		const ZEVector3							GetWorldScale() const;

		ZEVector3								GetFront();
		ZEVector3								GetRight();
		ZEVector3								GetUp();

		ZEVector3								GetWorldFront() const;
		ZEVector3								GetWorldRight() const;
		ZEVector3								GetWorldUp() const;

		bool									Initialize();
		bool									Deinitialize();
		virtual void							Destroy();
		
		virtual bool							Save(ZEMLWriterNode* Serializer);
		virtual bool							Restore(ZEMLReaderNode* Unserializer);

		virtual void							Tick(float Time);

		virtual bool							PreRender(const ZERNPreRenderParameters* Parameters);
		virtual void							Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command);
		virtual void							RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters);

		ZEDObjectWrapper*						GetWrapper() const;
}
ZE_META_ATTRIBUTE(ZEDObjectWrapper.Icon, "#R:/ZEDEditor/Entity/Icons/Entity.png")
ZE_META_ATTRIBUTE(ZEDObjectWrapper.NamePlate, true)
ZE_META_ATTRIBUTE(ZEDObjectWrapper.Selectable, true)
ZE_META_ATTRIBUTE(ZEDObjectWrapper.Visualization, BoundingBox);
