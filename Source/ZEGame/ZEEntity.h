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

#include "ZEDS\ZEArray.h"
#include "ZEDS\ZEVariant.h"
#include "ZEMath\ZEMatrix.h"
#include "ZEMath\ZEVector.h"
#include "ZEMath\ZEQuaternion.h"
#include "ZEMath\ZEAABoundingBox.h"
#include "ZEMath\ZEOBoundingBox.h"
#include "ZEMath\ZEBoundingSphere.h"
#include "ZEDefinitions.h"
#include "ZEMeta\ZEClass.h"

class ZEComponent;
class ZECompoundEntity;
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

#define ZE_META_ENTITY_DESCRIPTION(ClassName) ZE_META_EXTENDED_CLASS_DESCRIPTION(ClassName, ZEEntityDescription, ZE_META_ENTITY_CLASS_EXTENSION)
#define ZE_META_ENTITY() ZE_META_EXTENDED_CLASS(ZEEntityDescription, )

class ZEEntityDescription : public ZEClassDescription
{
	public:
		virtual const char*						GetName() const;
		virtual ZEClassDescription*				GetParent() const;
		virtual const char*						GetType() const;
		virtual const char*						GetDescription() const;
		virtual const char*						GetIcon() const;
		virtual const ZEPropertyDescription*	GetProperties() const;
		virtual size_t							GetPropertyCount() const;
		virtual size_t							GetPropertyOffset() const;
		virtual const ZEContainerDescription*	GetContainers() const;
		virtual size_t							GetContainerCount() const;
		virtual size_t							GetContainerOffset() const;
		virtual const ZEMethodDescription*		GetMethods() const;
		virtual size_t							GetMethodCount() const;
		virtual size_t							GetMethodOffset() const;
		virtual ZEClassProvider*				GetProvider() const;
		virtual ZEClass*						CreateInstance() const;
		virtual ZEEntityRunAt					GetRunAt() const;
};

enum ZEEntityType
{
	ZE_ET_REGULAR,
	ZE_ET_COMPOUND,
	ZE_ET_COMPONENT
};

// ZEDrawFlags
typedef ZEDWORD ZEDrawFlags;
#define ZE_DF_NONE								0
#define ZE_DF_DRAW								1
#define ZE_DF_DRAW_COMPONENTS					2
#define ZE_DF_LIGHT_SOURCE						4
#define ZE_DF_LIGHT_RECIVER						8
#define ZE_DF_CULL								16
#define ZE_DF_CULL_COMPONENTS					32
#define ZE_DF_AUTO								64

// ZERayCastFlags
typedef ZEDWORD ZERayCastFlags;
#define ZE_RC_NONE								0
#define ZE_RC_CAST_TO_ENTITY					1
#define ZE_RC_CAST_TO_COMPONENTS				2

// Entity Dirty Flags
typedef ZEDWORD ZEEntityDirtyFlags;
#define ZE_EDF_ALL								0xFFFFFFFF
#define ZE_EDF_LOCAL_TRANSFORM					1
#define ZE_EDF_WORLD_TRANSFORM					2
#define ZE_EDF_WORLD_BOUNDING_SPHERE			4
#define ZE_EDF_WORLD_BOUNDING_BOX				8

class ZEEntity : public ZEClass
{
	ZE_META_ENTITY()
	friend class ZECompoundEntity;
	private: 
		char									Name[ZE_MAX_NAME_SIZE];
		int										EntityId;
		ZEVector3								Position;
		ZEQuaternion							Rotation;
		ZEVector3								Scale;
		ZEMatrix4x4								WorldTransform;
		ZEVector3								Velocity;
		ZEVector3								OldPosition;

		bool									Enabled;
		bool									Visible;

		ZEAABoundingBox							LocalBoundingBox;
		ZEAABoundingBox							WorldBoundingBox;
		ZEBoundingSphere						WorldBoundingSphere;

	protected:
		ZEEntityDirtyFlags						DirtyFlags;	

		void									SetLocalBoundingBox(const ZEAABoundingBox& BoundingBox);
	
	public:
		virtual ZEEntityType					GetEntityType();

		virtual const ZEAABoundingBox&			GetLocalBoundingBox() const;
		virtual const ZEAABoundingBox&			GetWorldBoundingBox();
		const ZEBoundingSphere&					GetWorldBoundingSphere();

		virtual ZEDWORD							GetDrawFlags() const;
		virtual ZEDWORD							GetRayCastFlags() const;

		void									SetEntityId(int EntityId);
		int										GetEntityId() const;

		void									SetName(const char* NewName);
		const char*								GetName() const;

		virtual void							SetVisible(bool Enabled);
		virtual bool							GetVisible() const;

		virtual void							SetEnabled(bool Enabled);
		virtual bool							GetEnabled() const;

		virtual void							SetPosition(const ZEVector3& NewPosition);
		const ZEVector3&						GetPosition() const;

		virtual void							SetRotation(const ZEQuaternion& NewRotation);
		const ZEQuaternion&						GetRotation() const;

		virtual void							SetScale(const ZEVector3& NewScale);
		const ZEVector3&						GetScale() const;

		virtual void							SetVelocity(const ZEVector3& NewVelocity);
		const ZEVector3&						GetVelocity() const;

		ZEVector3								GetDirection();
		ZEVector3								GetRight();
		ZEVector3								GetUp();

		const ZEMatrix4x4&						GetWorldTransform();

		virtual bool							Initialize();
		virtual void							Deinitialize();
		virtual void							Destroy();
		virtual void							Reset();
		
		virtual void							Tick(float Time);
		virtual void							Draw(ZEDrawParameters* DrawParameters);

												ZEEntity();
		virtual									~ZEEntity();
};

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta> 
		<class name="ZEEntity">
			<description>Base Entity Type</description>
			<property name="EntityId" type="integer" autogetset="yes" description="Unique number that indentifes entity"/>
			<property name="Name" type="string" autogetset="yes" description="Name of the entity"/>
			<property name="Position" type="ZEVector3" autogetset="yes" description="World position of the entity"/>
			<property name="Rotation" type="ZEQuaternion" autogetset="yes" description="World rotation of the entity"/>
			<property name="Scale" type="ZEVector3" autogetset="yes" description="World scale of the entity"/>
			<property name="Enabled" type="boolean" autogetset="yes" description="If entity is disabled it will not recive Ticks so it will not interact with player. However this property does not affect entity physical interactions. A entity can be disabled but physically active."/>
			<property name="Visible" type="boolean" autogetset="yes" description="Is entity visible"/>
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/
#endif





