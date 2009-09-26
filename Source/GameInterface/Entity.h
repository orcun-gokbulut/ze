//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Entity.h
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

#include "ZEDS/Array.h"
#include "ZEDS/Variant.h"
#include "ZEMath/Matrix.h"
#include "ZEMath/Vector.h"
#include "ZEMath/Quaternion.h"
#include "ZEMath/AABoundingBox.h"
#include "ZEMath/OBoundingBox.h"
#include "ZEMath/BoundingSphere.h"
#include "Definitions.h"
#include "Core/Component.h"
#include "Graphics/Renderer.h"
#include "Meta/Class.h"

enum ZEBoundingVolumeMechnism
{
	ZE_BVM_USELOCALONLY,
	ZE_BVM_USECOMPONENTS,
	ZE_BVM_USEBOTH
};
class ZEEntityDescription;
class ZEEntity;
class ZEEntityData : public ZESerializable
{
	public:
		char								EntityType[ZE_MAX_NAME_SIZE];
		ZEArray<ZEPropertyDescription>		Properties;

		void								Generate(ZEEntityDescription* Description);
		void								Fill(ZEEntity* Entity);

		virtual	bool						Serialize(ZESerializer* Serializer);
		virtual bool						Unserialize(ZEUnserializer* Unserializer);
};

ZE_META_CLASS_DESCRIPTION(ZEEntity)
class ZEEntity : public ZEClass
{
	ZE_META_CLASS()
	private: 
		char								Name[ZE_MAX_NAME_SIZE];
		int									EntityId;
		ZEPoint3							Position;
		ZEQuaternion						Rotation;
		ZEVector3							Scale;
		ZEMatrix4x4							WorldTransform;
		ZEVector3							Velocity;
		ZEVector3							OldPosition;
		bool								Enabled;
		bool								Visible;
		bool								Drawable;
		bool								HasLight;

		ZEBoundingVolumeMechnism			BoundingVolumeMechanism;
		ZEAABoundingBox						LocalBoundingBox;
		ZEAABoundingBox						WorldBoundingBox;
		ZEBoundingSphere					WorldBoundingSphere;

	protected:
		void								SetBoundingVolumeMechanism(ZEBoundingVolumeMechnism Mechanism);
		void								SetLocalBoundingBox(const ZEAABoundingBox& BoundingBox);

		// States
		void								UpdateComponents();
		bool								UpdateBoundingBox;
		bool								UpdateBoundingSphere;
		bool								UpdateWorldTransform;

		ZEArray<ZEComponent*>				Components;

		void								RegisterComponent(ZEComponent* Component);
		void								UnregisterComponent(ZEComponent* Component);

	public:
		const ZEArray<ZEComponent *>&		GetComponents();

		virtual const ZEAABoundingBox&		GetLocalBoundingBox();
		virtual const ZEAABoundingBox&		GetWorldBoundingBox();
		const ZEBoundingSphere&				GetWorldBoundingSphere();

		virtual bool						IsDrawable();
		virtual bool						IsLight();
		virtual bool						AllwaysDraw();

		void								SetEntityId(int EntityId);
		int									GetEntityId() const;

		void								SetName(const char* NewName);
		const char*							GetName() const;

		virtual void						SetVisible(bool Enabled);
		virtual bool						GetVisible() const;

		virtual void						SetEnabled(bool Enabled);
		virtual bool						GetEnabled() const;

		virtual void						SetPosition(const ZEPoint3& NewPosition);
		const ZEPoint3&						GetPosition() const;

		virtual void						SetRotation(const ZEQuaternion& NewRotation);
		const ZEQuaternion&					GetRotation() const;

		virtual void						SetScale(const ZEPoint3& NewScale);
		const ZEPoint3&						GetScale() const;

		virtual void						SetVelocity(const ZEVector3& NewVelocity);
		const ZEVector3&					GetVelocity() const;

		const ZEMatrix4x4&					GetWorldTransform();

		virtual void						Initialize();
		virtual void						Deinitialize();
		virtual void						Destroy();
		virtual void						Reset();
		
		virtual void						Tick(float Time);
		virtual void						Draw(ZERenderer* Renderer, const ZESmartArray<const ZERLLight*>& Lights);
		virtual void						Update();

		void								UpdateBoundingVolumes();

											ZEEntity();
		virtual								~ZEEntity();
};

/*
ZE_POSTPROCESSOR_START(Meta)
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
ZE_POSTPROCESSOR_END()
*/
#endif
