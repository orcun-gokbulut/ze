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
#ifndef	__ZE_ENTITY_H__
#define __ZE_ENTITY_H__

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEFlags.h"
#include "ZEDefinitions.h"
#include "ZEDS/ZEVariant.h"
#include "ZEMath/ZEOBBox.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEVector.h"
#include "ZEMeta/ZEObject.h"
#include "ZEMath/ZEBSphere.h"
#include "ZEMath/ZEQuaternion.h"

struct ZEDrawParameters;

enum ZEEntityRunAt
{
	ZE_ERA_NONE			= 0,
	ZE_ERA_CLIENT		= 1,
	ZE_ERA_SERVER		= 2,
	ZE_ERA_BOTH			= 3
};

#define ZE_META_ENTITY_CLASS_EXTENSION\
	virtual ZEEntityRunAt GetRunAt() const;

#define ZE_META_ENTITY_DESCRIPTION(ObjectName) ZE_META_EXTENDED_OBJECT_DESCRIPTION(ObjectName, ZEEntityDescription, ZE_META_ENTITY_CLASS_EXTENSION)
#define ZE_META_ENTITY(Object) ZE_META_EXTENDED_OBJECT(ZEEntityDescription, ,Object)

class ZEEntityDescription : public ZEObjectDescription
{
public:
	virtual const char*						GetName() const;
	virtual ZEObjectDescription*			GetParent() const;
	virtual const char*						GetType() const;
	virtual const char*						GetDescription() const;
	virtual const char*						GetIcon() const;
	virtual const ZEPropertyDescription*	GetProperties() const;
	virtual ZESSize							GetPropertyCount() const;
	virtual ZESSize							GetPropertyOffset() const;
	virtual const ZEContainerDescription*	GetContainers() const;
	virtual ZESSize							GetContainerCount() const;
	virtual ZESSize							GetContainerOffset() const;
	virtual const ZEMethodDescription*		GetMethods() const;
	virtual ZESSize							GetMethodCount() const;
	virtual ZESSize							GetMethodOffset() const;
	virtual ZEObjectProvider*				GetProvider() const;
	virtual ZEObject*						CreateInstance() const;
	virtual ZEEntityRunAt					GetRunAt() const;
};

// ZEDrawFlags
typedef ZEFlags ZEDrawFlags;
#define ZE_DF_NONE								0
#define ZE_DF_DRAW								1
#define ZE_DF_DRAW_COMPONENTS					2
#define ZE_DF_LIGHT_SOURCE						4
#define ZE_DF_LIGHT_RECIVER						8
#define ZE_DF_CULL								16
#define ZE_DF_CULL_COMPONENTS					32
#define ZE_DF_AUTO								64

// Entity Dirty Flags
typedef ZEFlags ZEEntityDirtyFlags;
#define ZE_EDF_NONE								0
#define ZE_EDF_LOCAL_TRANSFORM					1
#define ZE_EDF_WORLD_TRANSFORM					2
#define ZE_EDF_WORLD_BOUNDING_SPHERE			4
#define ZE_EDF_WORLD_BOUNDING_BOX				8
#define ZE_EDF_ALL								0xFFFFFFFF

enum ZEEntityState
{
	ZE_ES_NOT_INITIALIZED				= 0,
	ZE_ES_INITIALIZING					= 1,
	ZE_ES_INITIALIZED					= 2,
	ZE_ES_DEINITIALIZING				= 3
};

class ZEScene;

class ZEEntity : public ZEObject
{
	ZE_META_ENTITY(ZEEntity)
	friend class ZEScene;
	friend class ZESceneCuller;
	friend class ZEDebugDrawer;

	private: 
		ZEEntity*								Owner;
		ZEScene*								OwnerScene;

		ZEString								Name;
		ZEInt									EntityId;
		ZEVector3								Position;
		ZEQuaternion							Rotation;
		ZEVector3								Scale;

		ZEEntityState							State;

		bool									Enabled;
		bool									Visible;

		mutable ZEEntityDirtyFlags				DirtyFlags;

		mutable ZEMatrix4x4						Transform;
		mutable ZEMatrix4x4						WorldTransform;

		ZEAABBox								BoundingBox;
		ZEAABBox								WorldBoundingBox;
		
	protected:
		ZEArray<ZEEntity*>						Components;
		ZEArray<ZEEntity*>						ChildEntities;

		void									SetBoundingBox(const ZEAABBox& BoundingBox);
		virtual void							OnTransformChanged();

		bool									AddComponent(ZEEntity* Entity); 
		void									RemoveComponent(ZEEntity* Entity);
		const ZEArray<ZEEntity*>&				GetComponents() const;

		virtual bool							SetOwner(ZEEntity* Owner);
		void									SetOwnerScene(ZEScene* Scene);

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

		virtual const ZEAABBox&					GetBoundingBox() const;
		virtual const ZEAABBox&					GetWorldBoundingBox();

		virtual const ZEMatrix4x4&				GetTransform() const;
		virtual const ZEMatrix4x4&				GetWorldTransform() const;

		bool									IsInitialized();
		ZEEntityState							GetState();

		virtual void							SetVisible(bool Visibility);
		virtual bool							GetVisible() const;

		virtual void							SetEnabled(bool Enabled);
		virtual bool							GetEnabled() const;

		virtual void							SetPosition(const ZEVector3& NewPosition);
		const ZEVector3&						GetPosition() const;
		virtual void							SetWorldPosition(const ZEVector3& NewPosition);
		const ZEVector3							GetWorldPosition() const;

		virtual void							SetRotation(const ZEQuaternion& NewRotation);
		const ZEQuaternion&						GetRotation() const;
		virtual void							SetWorldRotation(const ZEQuaternion& NewRotation);
		const ZEQuaternion						GetWorldRotation() const;

		virtual void							SetScale(const ZEVector3& NewScale);
		const ZEVector3&						GetScale() const;
		virtual void							SetWorldScale(const ZEVector3& NewScale);
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
		
		virtual void							Tick(float Time);
		virtual void							Draw(ZEDrawParameters* DrawParameters);
};

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta> 
		<class name="ZEEntity">		
			<noinstance>true</noinstance>
			<description>Base Entity Type</description>
			<property name="EntityId" type="integer32" autogetset="yes" description="Unique number that indentifes entity"/>	
			<property name="Name" type="string" autogetset="yes" description="Name of the entity"/>
			<property name="Position" type="ZEVector3" setfunction="SetWorldPosition" getfunction="GetWorldPosition" description="World position of the entity"/>
			<property name="Rotation" type="ZEQuaternion" setfunction="SetWorldRotation" getfunction="GetWorldRotation" description="World rotation of the entity"/>
			<property name="Scale" type="ZEVector3" setfunction="SetWorldScale" getfunction="GetWorldScale" description="World scale of the entity"/>
			<property name="Enabled" type="boolean" autogetset="yes" description="If entity is disabled it will not recive Ticks so it will not interact with player. However this property does not affect entity physical interactions. A entity can be disabled but physically active."/>
			<property name="Visible" type="boolean" autogetset="yes" description="Is entity visible"/>
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/

#endif
