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

#define ZE_ENTITY_DESCRIPTION(EntityName, EntityParentType)\
	class EntityName##Description : public EntityParentType##Description\
	{\
		public:\
			virtual ZEEntityDescription*		GetParentType();\
			virtual const char*					GetTypeName();\
			virtual const ZEEntityAttribute*	GetAttributes(size_t* Count);\
			virtual ZEEntity*					CreateInstance();\
			virtual ZEEntityRunAt				GetEntityRunAt();\
			virtual const char*					GetEditorIcon();\
			virtual const char*					GetEditorDescription();\
	};	

#define ZE_ENTITY_DESCRIPTION_START(EntityName, EntityParentType, EntityRunAt, EntityEditorIcon, EntityEditorDescription)\
	ZEEntityDescription* EntityName##Description::GetParentType()\
	{\
		static EntityParentType##Description Parent;\
		return &Parent;\
	}\
	const char* EntityName##Description::GetTypeName()\
	{\
		return #EntityName;\
	}\
	ZEEntity* EntityName##Description::CreateInstance()\
	{\
		return new EntityName();\
	}\
	ZEEntityRunAt EntityName##Description::GetEntityRunAt()\
	{\
		return EntityRunAt;\
	}\
	const char* EntityName##Description::GetEditorIcon()\
	{\
		return EntityEditorIcon;\
	}\
	const char* EntityName##Description::GetEditorDescription()\
	{\
		return EntityEditorDescription;\
	}\
	const ZEEntityAttribute* EntityName##Description::GetAttributes(size_t* Count)\
	{

	#define ZE_ENTITY_AATTRIBUTE(Name, SetFunction, GetFunction) \
		if (strcmp(AttributeName, Name) == 0)\
			if (Write) \
				SetFunction(Value);\
			else\
				GetFunction(Value);

	#define ZE_ENTITY_ATTRIBUTE_ENUMURATOR_START(EnumuratorName)\
		static ZEEntityAttributeEnumurator EnumuratorName##Description[] = {
	#define ZE_ENTITY_ATTRIBUTE_ENUMURATOR_ELEMENT(EnumuratorName, Value) {EnumuratorName, Value},
	#define ZE_ENTITY_ATTRIBUTE_ENUMURATOR_END() }; 

	#define ZE_ENTITY_NOATTRIBUTE() *Count = 0; return NULL;

	#define ZE_ENTITY_ATTRIBUTES_START() static const ZEEntityAttribute Attributes[] = {
	#define ZE_ENTITY_ATTRIBUTE(AttributeName, AttributeType, AttributeFlag, AttributeDefaultValue, AttributeDescription)\
		{AttributeName, AttributeType, ZE_EAS_NONE, AttributeFlag, AttributeDefaultValue, NULL, NULL, NULL, 0, AttributeDescription},
	#define ZE_ENTITY_ATTRIBUTE_SEMANTIC(AttributeName, AttributeType,  AttributeFlag, AttributeSemantic, AttributeDefaultValue, AttributeDescription)\
		{AttributeName, AttributeType, AttributeSemantic, AttributeFlag, AttributeDefaultValue, NULL, NULL, 0, NULL, AttributeDescription},
	#define ZE_ENTITY_ATTRIBUTE_FILENAME(AttributeName, AttributeFlag, FileExtensionFilter, AttributeDefaultValue, AttributeDescription)\
		{AttributeName, ZEVARIANTTYPE_STRING, ZE_EAS_FILENAME, AttributeFlag, AttributeDefaultValue, FileExtensionFilter, NULL, 0, NULL, AttributeDescription},
	#define ZE_ENTITY_ATTRIBUTE_ENTITY(AttributeName, AttributeFlag, EntityTypeFilter, AttributeDefaultValue, AttributeDescription)\
		{AttributeName, ZEVARIANTTYPE_INTEGER, ZE_AS_ENTITY, AttributeFlag, AttributeDefaultValue, 0, NULL, NULL, NULL, AttributeDescription},
	#define ZE_ENTITY_ATTRIBUTE_ENUMURATOR(AttributeName, AttributeFlag, EnumuratorName, AttributeDefaultValue, AttributeDescription)\
		{AttributeName, ZEVARIANTTYPE_INTEGER, ZE_EAS_ENUMURATOR, AttributeFlag, AttributeDefaultValue, NULL, NULL, sizeof(EnumuratorName##Description) / sizeof(ZEEntityAttributeEnumurator), EnumuratorName##Description, AttributeDescription},

	#define ZE_ENTITY_ATTRIBUTES_END() }; *Count = sizeof(Attributes) / sizeof(ZEEntityAttribute); return Attributes; 
#define ZE_ENTITY_DESCRIPTION_END(EntityName) }\
	ZEEntityDescription* EntityName::EntityDescription()\
	{\
		static EntityName##Description Desc;\
		return &Desc;\
	}\
	ZEEntityDescription* EntityName::GetEntityDescription()\
	{\
		return EntityName::EntityDescription();\
	}

#define ZE_ENTITY_CLASS(EntityName) public: virtual ZEEntityDescription* GetEntityDescription(); static ZEEntityDescription* EntityDescription(); private:

#define ZE_EAF_NONE				0
#define ZE_EAF_HIDDEN			1
#define ZE_EAF_READONLY			2
#define ZE_EAF_UNIQUE			4

enum ZEEntityRunAt
{
	ZE_ERA_NONE			= 0,
	ZE_ERA_CLIENT		= 1,
	ZE_ERA_SERVER		= 2,
	ZE_ERA_BOTH			= 3,
};

enum ZEBoundingVolumeMechnism
{
	ZE_BVM_USELOCALONLY,
	ZE_BVM_USECOMPONENTS,
	ZE_BVM_USEBOTH
};

enum ZEEntityAttributeSemantic
{
	ZE_EAS_NONE				= 0,
	ZE_EAS_POSITION			= 1,
	ZE_EAS_DIRECTION		= 2,
	ZE_EAS_COLORVALUE		= 3,
	ZE_EAS_ENUMURATOR		= 4,
	ZE_EAS_ENTITY			= 5,
	ZE_EAS_FILENAME			= 6
};


class ZEEntity;
class ZEEntityDescription;

struct ZEEntityAttributeEnumurator
{
	char*									Name;
	int										Value;
};

struct ZEEntityAttribute
{
	const char*								Name;
	ZEVariantType							Type;
	ZEEntityAttributeSemantic				Semantic;
	unsigned int							Flags;
	ZEVariant								Value;
	const char*								FileExtensionFilter;
	const char*								EntityTypeFilter;
	size_t									EnumuratorCount;
	ZEEntityAttributeEnumurator*			Enumurator;
	const char*								Description;
};

class ZEEntityData : public ZESerializable
{
	public:
		char								EntityType[ZE_MAX_NAME_SIZE];
		ZEArray<ZEEntityAttribute>			Attributes;

		void								Generate(ZEEntityDescription* Description);
		void								Fill(ZEEntity* Entity);

		virtual	bool						Serialize(ZESerializer* Serializer);
		virtual bool						Unserialize(ZEUnserializer* Unserializer);
};


class ZEEntityDescription
{
	public:
		virtual ZEEntityDescription*		GetParentType();
		virtual const char*					GetTypeName();
		virtual const ZEEntityAttribute*	GetAttributes(size_t* Count);
		virtual ZEEntityRunAt				GetEntityRunAt();
		virtual ZEEntity*					CreateInstance();
		virtual const char*					GetEditorIcon();
		virtual const char*					GetEditorDescription();
};

class ZEEntity 
{
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
		static ZEEntityDescription*			EntityDescription();
		virtual ZEEntityDescription*		GetEntityDescription();

		void								SetEntityId(int EntityId);
		int									GetEntityId();

		const ZEArray<ZEComponent *>&		GetComponents();

		virtual const ZEAABoundingBox&		GetLocalBoundingBox();
		virtual const ZEAABoundingBox&		GetWorldBoundingBox();
		const ZEBoundingSphere&				GetWorldBoundingSphere();

		virtual bool						IsDrawable();
		virtual bool						IsLight();
		virtual bool						AllwaysDraw();

		void								SetName(const char* NewName);
		const char*							GetName();

		virtual bool						IsVisible();
		virtual void						SetVisible(bool Enabled);

		virtual bool						IsEnabled();
		virtual void						SetEnabled(bool Enabled);

		virtual void						Tick(float Time);
		virtual void						Update();

		virtual void						Draw(ZERenderer* Renderer, const ZESmartArray<const ZERLLight*>& Lights);

		virtual void						SetPosition(const ZEPoint3& NewPosition);
		const ZEPoint3&						GetPosition();

		virtual void						SetRotation(const ZEQuaternion& NewRotation);
		const ZEQuaternion&					GetRotation();

		virtual void						SetScale(const ZEPoint3& NewScale);
		const ZEPoint3&						GetScale();

		virtual void						SetVelocity(const ZEVector3& NewVelocity);
		const ZEVector3&					GetVelocity();

		const ZEMatrix4x4&					GetWorldTransform();

		virtual void						Initialize();
		virtual void						Deinitialize();
		virtual void						Destroy();
		virtual void						Reset(ZEEntityData* EntityData);
		
		void								UpdateBoundingVolumes();
		
		virtual bool						SetAttribute(const char* AttributeName, const ZEVariant& Value);
		virtual bool						GetAttribute(const char* AttributeName, ZEVariant& Value);

		virtual bool						Serialize(ZESerializer* Serializer);
		virtual bool						Unserialize(ZEUnserializer* Unserializer);

											ZEEntity();
		virtual								~ZEEntity();
};

#endif
