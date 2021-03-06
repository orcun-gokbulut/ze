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

#include "ZEMeta/ZEObject.h"
#include "ZEDestroyable.h"

#include "ZEEntityMacros.h"

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
#include "ZEMeta/ZEEnumerator.h"
#include "ZESpatial/ZEOctree.h"
#include "ZEThread/ZETask.h"
#include "ZERayCast.h"
#include "ZEVolumeCast.h"
#include "ZESpatial/ZEOctree.h"

class ZEDObjectWrapper;
class ZEMLWriterNode;
class ZEMLReaderNode;

ZEMT_FORWARD_DECLARE(ZEScene);
ZEMT_FORWARD_DECLARE(ZERNPreRenderParameters);
ZEMT_FORWARD_DECLARE(ZERNRenderParameters);
ZEMT_FORWARD_DECLARE(ZERNCommand);

typedef ZEFlags ZEEntityFlags;
ZE_ENUM(ZEEntityFlag)
{
	ZE_EF_NONE						= 0x000,
	ZE_EF_TICKABLE					= 0x001,
	ZE_EF_TICKABLE_CUSTOM			= 0x002,
	ZE_EF_RENDERABLE				= 0x004,
	ZE_EF_RENDERABLE_CUSTOM			= 0x008,
	ZE_EF_CULLABLE					= 0x010,
	ZE_EF_STATIC_SUPPORT			= 0x020,
	ZE_EF_RAY_CASTABLE				= 0x040,
	ZE_EF_RAY_CASTABLE_CUSTOM		= 0x080,
	ZE_EF_VOLUME_CASTABLE			= 0x100,
	ZE_EF_VOLUME_CASTABLE_CUSTOM	= 0x200
};

ZE_ENUM(ZEEntityResult)
{
	ZE_ER_DONE,
	ZE_ER_WAIT,
	ZE_ER_FAILED,
	ZE_ER_FAILED_CLEANUP
};

ZE_ENUM(ZEEntityState)
{
	ZE_ES_ERROR_DESTRUCTION				= -5,
	ZE_ES_ERROR_DEINITIALIZATION		= -4,
	ZE_ES_ERROR_INITIALIZATION			= -3,
	ZE_ES_ERROR_UNLOADING				= -2,
	ZE_ES_ERROR_LOADING					= -1,
	ZE_ES_DESTROYED						= 0,
	ZE_ES_DESTROYING					= 1,
	ZE_ES_NONE							= 2,
	ZE_ES_UNLOADING						= 3,
	ZE_ES_LOADING						= 4,
	ZE_ES_LOADED						= 5,
	ZE_ES_DEINITIALIZING				= 6,
	ZE_ES_INITIALIZING					= 7,
	ZE_ES_INITIALIZED					= 8

};

class ZEEntityLoadingScore
{
	public:
		ZESize Score;
		ZESize Count;
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

		volatile ZEEntityState					State;
		volatile ZEEntityState					TargetState;
		volatile bool							ReloadFlag;
		volatile bool							ReinitializeFlag;
		bool									SerialOperation;
		volatile ZEUInt							LocalLoadingPercentage;
		ZETask									UpdateStateTask;

		ZEString								Name;
		ZEInt									EntityId;
		ZEVector3								Position;
		ZEQuaternion							Rotation;
		ZEVector3								Scale;
		bool									Enabled;
		bool									EnabledFlattened;
		bool									Visible;
		bool									VisibleFlattened;
		bool									Static;
		ZEEntityFlags							EntityFlags;
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

		ZELink<ZEEntity>						TickListLink;
		ZELink<ZEEntity>						RenderListLink;
		ZEOctree<ZEEntity*>*					RenderListOctree;

		#ifdef ZE_DEBUG_ENABLE
		bool									LoadInternalChainCheck;
		bool									UnloadInternalChainCheck;
		bool									InitializeInternalChainCheck;
		bool									DeinitializeInternalChainCheck;
		bool									DestroyInternalChainCheck;
		#endif

		void 									GetComponentsInternal(ZEClass* Class, ZEArray<ZEEntity*>& Output) const;
		void 									GetComponentsInternal(const ZEString& Name, ZEArray<ZEEntity*>& Output) const;

		void 									GetChildEntitiesInternal(ZEClass* Class, ZEArray<ZEEntity*>& Output) const;
		void									GetChildEntitiesInternal(const ZEString& Name, ZEArray<ZEEntity*>& Output) const;

		ZETaskResult							UpdateStateTaskFunction(ZETaskThread* Thread, ZESize InstanceIndex, void* Parameters);
		void									UpdateStateSerial();
		void									UpdateState();

		void									SetParent(ZEEntity* Parent);
		void									SetWrapper(ZEDObjectWrapper* Wrapper);

		void									UpdateRenderabilityState(bool Forced = false);
		void									UpdateTickabilityState();
		void									UpdateOctree();

		void									ParentEnabledChanged();
	protected:
		void									SetEntityFlags(ZEEntityFlags Flags);

		void									SetSerialOperation(bool SerialOperation);
		bool									GetSerialOperation() const;

		virtual ZEEntityResult					LoadInternal();
		virtual ZEEntityResult					UnloadInternal();
		virtual ZEEntityResult					InitializeInternal();
		virtual ZEEntityResult					DeinitializeInternal();

		virtual void							PreDestroyInternal();
		virtual void							PostDestroyInternal();

		ZEEntity*								GetComponent(ZEClass* Class, bool Recursive = false);
		ZEEntity*								GetComponent(const ZEString& Name, bool Recursive = false);
		const ZEArray<ZEEntity*>&				GetComponents() const;
		const ZEArray<ZEEntity*>				GetComponents(ZEClass* Class, bool Recursive = false) const;
		const ZEArray<ZEEntity*>				GetComponents(const ZEString& Name, bool Recursive = false) const;
		bool									AddComponent(ZEEntity* Entity); 
		void									RemoveComponent(ZEEntity* Entity);
		void									ClearComponents();

		virtual void							SetScene(ZEScene* Scene);
		virtual void							ParentVisibleChanged();

		virtual void							LocalTransformChanged();
		virtual void							ParentTransformChanged();
		virtual void							BoundingBoxChanged();

		virtual bool							CheckParent(ZEEntity* Parent);
		virtual bool							CheckComponent(ZEEntity* Parent);
		virtual bool							CheckChildEntity(ZEEntity* Parent);

		void									SetLocalLoadingPercentage(ZEUInt Percentage);
		ZEUInt									GetLocalLoadingPercentage();

												ZEEntity();
		virtual									~ZEEntity();

	public:
		ZEEntity*								GetParent() const;
		ZEScene*								GetScene() const;
		
		void									SetEntityId(ZEInt EntityId);
		ZEInt									GetEntityId() const;

		void									SetName(const ZEString& NewName);
		const ZEString&							GetName() const;

		virtual ZEEntityFlags					GetEntityFlags() const final;

		ZEEntity*								GetChildEntity(ZEClass* Class, bool Recursive = false);
		ZEEntity*								GetChildEntity(const ZEString& Name, bool Recursive = false);
		const ZEArray<ZEEntity*>&				GetChildEntities() const;
		const ZEArray<ZEEntity*>				GetChildEntities(ZEClass* Class, bool Recursive = false) const;
		const ZEArray<ZEEntity*>				GetChildEntities(const ZEString& Name, bool Recursive = false) const;
		virtual bool							AddChildEntity(ZEEntity* Entity);
		virtual void							RemoveChildEntity(ZEEntity* Entity);
		void									ClearChildEntities();

		void									SetBoundingBox(const ZEAABBox& BoundingBox, bool NoEvent = false);
		virtual const ZEAABBox&					GetBoundingBox() const;
		virtual const ZEAABBox&					GetWorldBoundingBox() const;

		const ZEMatrix4x4&						GetTransform() const;
		const ZEMatrix4x4&						GetInvTransform() const;
		const ZEMatrix4x4&						GetWorldTransform() const;
		const ZEMatrix4x4&						GetInvWorldTransform() const;

		virtual void							SetPosition(const ZEVector3& NewPosition);
		const ZEVector3&						GetPosition() const;

		void									SetWorldPosition(const ZEVector3& NewPosition);
		ZEVector3								GetWorldPosition() const;

		virtual void							SetRotation(const ZEQuaternion& NewRotation);
		const ZEQuaternion&						GetRotation() const;

		void									SetWorldRotation(const ZEQuaternion& NewRotation);
		ZEQuaternion							GetWorldRotation() const;

		virtual void							SetScale(const ZEVector3& NewScale);
		const ZEVector3&						GetScale() const;

		void									SetWorldScale(const ZEVector3& NewScale);
		ZEVector3								GetWorldScale() const;

		void									SetTransform(const ZEMatrix4x4& Transform);
		void									SetWorldTransform(const ZEMatrix4x4& Transform);

		ZEVector3								GetFront() const;
		ZEVector3								GetRight() const;
		ZEVector3								GetUp() const;

		ZEVector3								GetWorldFront() const;
		ZEVector3								GetWorldRight() const;
		ZEVector3								GetWorldUp() const;

		ZEEntityState							GetState() const;
		ZEEntityState							GetTargetState() const;
		ZEUInt									GetLoadingPercentage();
		ZEEntityLoadingScore					GetLoadingScore();

		bool									IsLoaded() const;
		bool									IsLoadedOrLoading() const;
		bool									IsInitialized() const;
		bool									IsInitializedOrInitializing() const;
		bool									IsFailed() const;
		bool									IsDestroyed() const;

		virtual void							SetEnabled(bool Enabled);
		bool									GetEnabled() const;
		
		virtual void							SetVisible(bool Visibility);
		bool									GetVisible() const;

		virtual void							SetStatic(bool Static);
		bool									GetStatic();

		void									Initialize();
		void									Deinitialize();
		void									Reinitialize();

		void									Load();
		void									Unload();
		void									Reload();

		void									Destroy();

		virtual void							Tick(float Time);

		virtual bool							PreRender(const ZERNPreRenderParameters* Parameters);
		virtual void							Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command);
		virtual void							RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters);
		virtual void							VolumeCast(ZEVolumeCastReport& Report, const ZEVolumeCastParameters& Parameters);

		virtual bool							Serialize(ZEMLWriterNode* Serializer);
		virtual bool							Unserialize(ZEMLReaderNode* Unserializer);

		void									LockEntity();
		void									UnlockEntity();

		ZEDObjectWrapper*						GetWrapper() const;
}
ZEMT_ATTRIBUTE(ZEDEditor.ObjectWrapper.Icon, "#R:/ZEDEditor/Icons/ZEDObjectWrapper/ZEEntity.png")
ZEMT_ATTRIBUTE(ZEDEditor.ObjectWrapper3D.Pickable, true)
ZEMT_ATTRIBUTE(ZEDEditor.ObjectWrapper3D.Transformable, true)
ZEMT_ATTRIBUTE(ZEDEditor.ObjectWrapper3D.BoundingBoxVisible, true)
ZEMT_ATTRIBUTE(ZEDEditor.ObjectWrapper3D.NameplateVisible, true)
ZEMT_ATTRIBUTE_PROPERTY(WorldPosition,		ZEMeta.Serialization, false)
ZEMT_ATTRIBUTE_PROPERTY(WorldRotation,		ZEMeta.Serialization, false)
ZEMT_ATTRIBUTE_PROPERTY(WorldScale,			ZEMeta.Serialization, false)
ZEMT_ATTRIBUTE_PROPERTY(Transform,			ZEDEditor.PropertyEditor.Display, false)
ZEMT_ATTRIBUTE_PROPERTY(WorldTransform,		ZEDEditor.PropertyEditor.Display, false)
ZEMT_ATTRIBUTE_PROPERTY(InvWorldTransform,	ZEDEditor.PropertyEditor.Display, false)
//ZE_META_ATTRIBUTE_PROPERTY(State,			ZEDEditor.PropertyEditor.Display, false)
ZEMT_ATTRIBUTE_PROPERTY(Right,				ZEDEditor.PropertyEditor.Display, false)
ZEMT_ATTRIBUTE_PROPERTY(Up,					ZEDEditor.PropertyEditor.Display, false)
ZEMT_ATTRIBUTE_PROPERTY(Front,				ZEDEditor.PropertyEditor.Display, false)
ZEMT_ATTRIBUTE_PROPERTY(WorldRight,			ZEDEditor.PropertyEditor.Display, false)
ZEMT_ATTRIBUTE_PROPERTY(WorldUp,			ZEDEditor.PropertyEditor.Display, false)
ZEMT_ATTRIBUTE_PROPERTY(WorldFront,			ZEDEditor.PropertyEditor.Display, false);
