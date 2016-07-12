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
#include "ZEThread/ZETask.h"

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

ZE_ENUM(ZEEntityResult)
{
	ZE_ER_DONE,
	ZE_ER_WAIT,
	ZE_ER_FAILED
};

ZE_ENUM(ZEEntityState)
{
	ZE_ES_ERROR_DEINITIALIZATION		= -4,
	ZE_ES_ERROR_INITIALIZATION			= -3,
	ZE_ES_ERROR_UNLOADING				= -2,
	ZE_ES_ERROR_LOADING					= -1,
	ZE_ES_NOT_INITIALIZED				= 0,
	ZE_ES_NOT_LOADED					= 0,
	ZE_ES_NONE							= 0,
	ZE_ES_UNLOADING						= 1,
	ZE_ES_DEINITIALIZED					= 2,
	ZE_ES_DEINITIALIZING				= 4,
	ZE_ES_LOADING						= 5,
	ZE_ES_LOADED						= 6,
	ZE_ES_INITIALIZING					= 7,
	ZE_ES_INITIALIZED					= 8
};

class ZEEntity : public ZEObject
{
	ZE_OBJECT
	friend class ZEScene;
	friend class ZEDEntityWrapper;
	private: 
		ZEEntity*								Parent;
		ZEScene*								Scene;
		ZEDObjectWrapper*						Wrapper;

		ZEString								Name;
		ZEEntityState							State;
		ZEEntityState							TargetState;
		ZEInt									EntityId;
		ZEVector3								Position;
		ZEQuaternion							Rotation;
		ZEVector3								Scale;
		bool									Enabled;
		bool									Visible;
		bool									SerialOperation;
		mutable ZELock							EntityLock;
		
		mutable ZEFlags							EntityDirtyFlags;
		mutable ZEMatrix4x4						Transform;
		mutable ZEMatrix4x4						InvTransform;
		mutable ZEMatrix4x4						WorldTransform;
		mutable ZEMatrix4x4						InvWorldTransform;
		mutable ZEAABBox						BoundingBox;
		mutable ZEAABBox						WorldBoundingBox;
		
		ZEArray<ZEEntity*>						Components;
		ZEArray<ZEEntity*>						ChildEntities;

		ZETask									ManageTask;
		ZETaskResult							ManageStates(ZETaskThread* Thread, void* Parameters);
		void									ManagetStatesSerial();

		void									SetParent(ZEEntity* Parent);
		void									SetScene(ZEScene* Scene);
		void									SetWrapper(ZEDObjectWrapper* Wrapper);

	protected:
		void									SetSerialOperation(bool SerialOperation);
		bool									GetSerialOperation() const;

		virtual ZEEntityResult					LoadInternal();
		virtual ZEEntityResult					UnloadInternal();
		virtual ZEEntityResult					InitializeInternal();
		virtual ZEEntityResult					DeinitializeInternal();

		const ZEArray<ZEEntity*>&				GetComponents() const;
		bool									AddComponent(ZEEntity* Entity); 
		void									RemoveComponent(ZEEntity* Entity);
		void									ClearComponents();

		virtual void							LocalTransformChanged();
		virtual void							ParentTransformChanged();
		virtual void							BoundingBoxChanged();

		virtual bool							CheckParent(ZEEntity* Parent);
		virtual bool							CheckComponent(ZEEntity* Parent);
		virtual bool							CheckChildEntity(ZEEntity* Parent);

		virtual bool							InitializeSelf();
		virtual bool							DeinitializeSelf();

												ZEEntity();
		virtual									~ZEEntity();

	public:
		ZEEntity*								GetParent() const;
		ZEScene*								GetScene() const;
		
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

		ZEVector3								GetFront() const;
		ZEVector3								GetRight() const;
		ZEVector3								GetUp() const;

		ZEVector3								GetWorldFront() const;
		ZEVector3								GetWorldRight() const;
		ZEVector3								GetWorldUp() const;

		ZEEntityState							GetState() const;
		bool									IsLoaded() const;
		bool									IsInitialized() const;

		void									Initialize();
		void									Deinitialize();

		void									Load();
		void									Unload();
			
		virtual bool							Serialize(ZEMLWriterNode* Serializer);
		virtual bool							Unserialize(ZEMLReaderNode* Unserializer);

		virtual void							Tick(float Time);

		virtual bool							PreRender(const ZERNPreRenderParameters* Parameters);
		virtual void							Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command);
		virtual void							RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters);

		virtual void							Destroy();

		ZEDObjectWrapper*						GetWrapper() const;
}
ZE_META_ATTRIBUTE_PROPERTY(WorldPosition,		ZEMeta.Serialization, false)
ZE_META_ATTRIBUTE_PROPERTY(WorldRotation,		ZEMeta.Serialization, false)
ZE_META_ATTRIBUTE_PROPERTY(WorldScale,			ZEMeta.Serialization, false)
ZE_META_ATTRIBUTE_PROPERTY(Transform,			ZEDEditor.PropertyEditor.Display, false)
ZE_META_ATTRIBUTE_PROPERTY(WorldTransform,		ZEDEditor.PropertyEditor.Display, false)
ZE_META_ATTRIBUTE_PROPERTY(InvWorldTransform,	ZEDEditor.PropertyEditor.Display, false)
ZE_META_ATTRIBUTE_PROPERTY(State,				ZEDEditor.PropertyEditor.Display, false)
ZE_META_ATTRIBUTE_PROPERTY(Right,				ZEDEditor.PropertyEditor.Display, false)
ZE_META_ATTRIBUTE_PROPERTY(Up,					ZEDEditor.PropertyEditor.Display, false)
ZE_META_ATTRIBUTE_PROPERTY(Front,				ZEDEditor.PropertyEditor.Display, false)
ZE_META_ATTRIBUTE_PROPERTY(Right,				ZEDEditor.PropertyEditor.Display, false)
ZE_META_ATTRIBUTE_PROPERTY(Up,					ZEDEditor.PropertyEditor.Display, false)
ZE_META_ATTRIBUTE_PROPERTY(Front,				ZEDEditor.PropertyEditor.Display, false);
