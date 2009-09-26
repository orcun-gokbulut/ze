//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Entity.cpp
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

#include "Entity.h"
#include "Core/Error.h"
#include <string.h>
/*
void ZEEntityData::Generate(ZEEntityDescription* Description)
{
	const ZEEntityAttribute* TempAttributes;
	size_t Count;

	Attributes.Clear();

	while (Description != NULL)
	{
		TempAttributes = Description->GetAttributes(&Count);
		if (Count != 0 && TempAttributes != NULL)
			Attributes.MassAdd(TempAttributes, Count);

		Description = Description->GetParentType();
	}
}

void ZEEntityData::Fill(ZEEntity* Entity)
{
	for (size_t I = 0; I < Attributes.GetCount(); I++)
		Entity->GetAttribute(Attributes[I].Name, Attributes[I].Value);
}

bool ZEEntityData::Serialize(ZESerializer* Serializer)
{
	Serializer->Write(EntityType, sizeof(char), ZE_MAX_NAME_SIZE);
	ZEDWORD AttributeCount = Attributes.GetCount();
	Serializer->Write(&AttributeCount, sizeof(ZEDWORD), 1);
	for (size_t I = 0; I < Attributes.GetCount(); I++)
	{
			Serializer->Write((void*)Attributes[I].Name, sizeof(char), ZE_MAX_NAME_SIZE);
			Attributes[I].Value.Serialize(Serializer);
	}
	return true;
}

bool ZEEntityData::Unserialize(ZEUnserializer* Unserializer)
{
	Unserializer->Read(EntityType, sizeof(char), ZE_MAX_NAME_SIZE);
	ZEDWORD AttributeCount;
	Unserializer->Read(&AttributeCount, sizeof(ZEDWORD), 1);
	Attributes.SetCount(AttributeCount);
	for (size_t I = 0; I < Attributes.GetCount(); I++)
	{
		Unserializer->Read((void*)Attributes[I].Name, sizeof(char), ZE_MAX_NAME_SIZE);
		Attributes[I].Value.Unserialize(Unserializer);
	}
	return true;
}*/
/*
ZEEntityDescription* ZEEntityDescription::GetParentType()
{
	return NULL;
}

const char* ZEEntityDescription::GetTypeName()
{
	return "ZEEntity";
}

const ZEEntityAttribute* ZEEntityDescription::GetAttributes(size_t* Count)
{

//	ZEEntityAttributeEnumurator* Enu = 	ZEEntityAttributeEnumurator()[] = {{"", 1}, {"", 1}};;

	static ZEEntityAttribute Attributes[] = 
	{
		{"EntityId", ZE_VRT_INTEGER, ZE_EAS_ENTITY, ZE_EAF_READONLY, 0, 0, NULL, NULL, NULL, "Unique number that indentifes entity"},
		{"EntityName", ZE_VRT_STRING, ZE_EAS_NONE, ZE_EAF_UNIQUE, "", 0, NULL, NULL, NULL, "Name of the entity"},
		{"Position", ZE_VRT_VECTOR3,  ZE_EAS_NONE, ZE_EAF_NONE, ZEVector3(0.0f, 0.0f, 0.0f), 0, NULL, NULL, NULL, "World position of the entity"},
		{"Rotation", ZE_VRT_QUATERNION,  ZE_EAS_NONE, ZE_EAF_NONE, ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f), 0, NULL, NULL, NULL, "World rotation of the entity"},
		{"Scale", ZE_VRT_VECTOR3,  ZE_EAS_NONE, ZE_EAF_NONE, ZEVector3(1.0f, 1.0f, 1.0f), 0, NULL, NULL, NULL, "World scale of the entity"},
		{"Enabled", ZE_VRT_BOOLEAN,  ZE_EAS_NONE, ZE_EAF_NONE, false, 0, NULL, NULL, NULL, "If entity is disabled it will not recive Ticks so it will not interact with player. However this property does not affect entity physical interactions. A entity can be disabled but physically active."},
		{"Visible", ZE_VRT_BOOLEAN,  ZE_EAS_NONE, ZE_EAF_NONE, false, 0, NULL, NULL, NULL, "Is entity visible"},
	};

	*Count = 7;
	return Attributes;
}

ZEEntity* ZEEntityDescription::CreateInstance()
{
	return NULL;
}

ZEEntityRunAt ZEEntityDescription::GetEntityRunAt()
{
	return ZE_ERA_NONE;
}

const char* ZEEntityDescription::GetEditorIcon()
{
	return "";
}

const char* ZEEntityDescription::GetEditorDescription()
{
	return "Base entity class";
}
*/

void ZEEntity::SetBoundingVolumeMechanism(ZEBoundingVolumeMechnism Mechanism)
{
	BoundingVolumeMechanism = Mechanism;
	UpdateBoundingBox = true;
	UpdateBoundingSphere = true;
}

void ZEEntity::SetLocalBoundingBox(const ZEAABoundingBox& BoundingBox)
{
	LocalBoundingBox = BoundingBox;
	UpdateBoundingBox = true;
	UpdateBoundingSphere = true;
}

const ZEAABoundingBox& ZEEntity::GetLocalBoundingBox()
{
	return LocalBoundingBox;
}


void ZEEntity::UpdateComponents()
{
	for (size_t I = 0; I < Components.GetCount(); I++)
		Components[I]->OwnerWorldTransformChanged();
}

void ZEEntity::RegisterComponent(ZEComponent* Component)
{
	if (Component->IsDrawable())
	{
		if (Component->IsVisible())
			UpdateBoundingVolumes();

		Drawable = true;
	}

	if (Component->IsLight())
		HasLight = true;



	Component->SetOwner(this);
	Component->Initialize();
	Components.Add(Component);
}

void ZEEntity::UnregisterComponent(ZEComponent* Component)
{
	Component->Deinitialize();
	Components.DeleteValue(Component);

	Drawable = false;
	if (Component->IsDrawable())
		for (size_t I = 0; I < Components.GetCount(); I++)
			if (Components[I]->IsDrawable())
			{
				Drawable = true;
				break;
			}

	HasLight = false;
	if (Component->IsLight())
		for (size_t I = 0; I < Components.GetCount(); I++)
			if (Components[I]->IsLight())
			{
				HasLight = true;
				break;
			}
}
/*
ZEEntityDescription* ZEEntity::EntityDescription()
{
	static  ZEEntityDescription EntityDescription;
	return &EntityDescription;
}

ZEEntityDescription* ZEEntity::GetEntityDescription()
{
	return EntityDescription();
}*/

void ZEEntity::SetEntityId(int EntityId)
{
	this->EntityId = EntityId;
}


int ZEEntity::GetEntityId() const
{
	return EntityId;
}

const ZEArray<ZEComponent *> &	ZEEntity::GetComponents()
{
	return Components; 
}

const ZEAABoundingBox &	 ZEEntity::GetWorldBoundingBox()
{
	if (UpdateBoundingBox)
	{
		bool NoBoundingBox = true;

		if (BoundingVolumeMechanism == ZE_BVM_USEBOTH || BoundingVolumeMechanism == ZE_BVM_USELOCALONLY)
		{
			ZEVector3 Point;
			ZEMatrix4x4 WorldTransform = GetWorldTransform();
			ZEMatrix4x4::Transform(Point, WorldTransform, LocalBoundingBox.GetVertex(0));
			WorldBoundingBox.Min = WorldBoundingBox.Max = Point;
			for (int I = 1; I < 8; I++)
			{
				ZEMatrix4x4::Transform(Point, WorldTransform, LocalBoundingBox.GetVertex(I));
				if (Point.x < WorldBoundingBox.Min.x) WorldBoundingBox.Min.x = Point.x;
				if (Point.y < WorldBoundingBox.Min.y) WorldBoundingBox.Min.y = Point.y;
				if (Point.z < WorldBoundingBox.Min.z) WorldBoundingBox.Min.z = Point.z;

				if (Point.x > WorldBoundingBox.Max.x) WorldBoundingBox.Max.x = Point.x;
				if (Point.y > WorldBoundingBox.Max.y) WorldBoundingBox.Max.y = Point.y;
				if (Point.z > WorldBoundingBox.Max.z) WorldBoundingBox.Max.z = Point.z;
			}
			NoBoundingBox = false;
		}

		if (BoundingVolumeMechanism == ZE_BVM_USEBOTH || BoundingVolumeMechanism == ZE_BVM_USECOMPONENTS)
			for (size_t I = 0; I < Components.GetCount(); I++)
			{
				if (Components[I]->IsDrawable() && Components[I]->IsVisible())
				{
					const ZEAABoundingBox& CompBoundingBox = Components[I]->GetWorldBoundingBox();
					if (NoBoundingBox == true)
					{
						WorldBoundingBox.Min = WorldBoundingBox.Max = CompBoundingBox.GetVertex(0);
						NoBoundingBox = false;
					}

					for (int N = 0; N < 8; N++)
					{
						ZEVector3 Point = CompBoundingBox.GetVertex(N);
						if (Point.x < WorldBoundingBox.Min.x) WorldBoundingBox.Min.x = Point.x;
						if (Point.y < WorldBoundingBox.Min.y) WorldBoundingBox.Min.y = Point.y;
						if (Point.z < WorldBoundingBox.Min.z) WorldBoundingBox.Min.z = Point.z;

						if (Point.x > WorldBoundingBox.Max.x) WorldBoundingBox.Max.x = Point.x;
						if (Point.y > WorldBoundingBox.Max.y) WorldBoundingBox.Max.y = Point.y;
						if (Point.z > WorldBoundingBox.Max.z) WorldBoundingBox.Max.z = Point.z;
					}
				}
			}

		if (NoBoundingBox == true)
			WorldBoundingBox.Max = WorldBoundingBox.Max = ZEPoint3(0.0f, 0.0f, 0.0f);

		UpdateBoundingBox = false;
	}

	return WorldBoundingBox;
}

const ZEBoundingSphere&	ZEEntity::GetWorldBoundingSphere()
{
	if (UpdateBoundingSphere)
	{
		GetWorldBoundingBox().GenerateBoundingSphere(WorldBoundingSphere);
		UpdateBoundingSphere = false;
	}

	return WorldBoundingSphere;
}

bool ZEEntity::IsDrawable()
{
	return Drawable;
}

bool ZEEntity::IsLight()
{
	return HasLight;	
}

bool ZEEntity::AllwaysDraw()
{
	return false;
}

void ZEEntity::SetName(const char* NewName)
{
	strncpy(Name, NewName, ZE_MAX_NAME_SIZE);
}

const char* ZEEntity::GetName() const
{
	return Name;
}

void ZEEntity::SetVisible(bool Visible)
{
	this->Visible = Visible;
}

bool ZEEntity::GetVisible() const
{
	return Visible;
}

void ZEEntity::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEEntity::GetEnabled() const
{
	return Enabled;
}

void ZEEntity::SetPosition(const ZEPoint3& NewPosition) 
{
	UpdateWorldTransform = true;
	UpdateBoundingBox = true;
	UpdateBoundingSphere = true;
	Position = NewPosition;
	UpdateComponents();
}

const ZEPoint3& ZEEntity::GetPosition() const
{
	return Position;
}

void ZEEntity::SetRotation(const ZEQuaternion& NewRotation) 
{
	UpdateWorldTransform = true;
	UpdateBoundingBox = true;
	UpdateBoundingSphere = true;
	Rotation = NewRotation;
	UpdateComponents();
}

const ZEQuaternion& ZEEntity::GetRotation() const
{
	return Rotation;
}

void ZEEntity::SetScale(const ZEVector3& NewScale)
{
	UpdateWorldTransform = true;
	UpdateBoundingBox = true;
	UpdateBoundingSphere = true;
	Scale = NewScale;
	UpdateComponents();
}

const ZEVector3& ZEEntity::GetScale() const
{
	return Scale;
}

const ZEMatrix4x4& ZEEntity::GetWorldTransform()
{
	if (UpdateWorldTransform)
	{
		ZEMatrix4x4::CreateOrientation(WorldTransform, Position, Rotation, Scale);
		UpdateWorldTransform = false;
	}

	return WorldTransform;
}

void ZEEntity::SetVelocity(const ZEVector3& NewVelocity)
{
	Velocity = NewVelocity;
}

const ZEVector3& ZEEntity::GetVelocity() const
{
	return Velocity;
}

void ZEEntity::Initialize()
{
}

void ZEEntity::Deinitialize()
{
}

void ZEEntity::Destroy()
{
	delete this;
}

void ZEEntity::Reset()
{
	Deinitialize();
	Initialize();
}
/*
bool ZEEntity::SetAttribute(const char* AttributeName, const ZEVariant& Value)
{
	if (strcmp(AttributeName, "EntityId") == 0)
		SetEntityId(Value.GetInteger());
	else if (strcmp(AttributeName, "Position") == 0)
		SetPosition(Value.GetVector3());
	else if (strcmp(AttributeName, "EntityName") == 0)
		SetName(Value.GetString());
	else if (strcmp(AttributeName, "Rotation") == 0)
		SetRotation(Value.GetQuaternion());
	else if (strcmp(AttributeName, "Scale") == 0)
		SetScale(Value.GetVector3());
	else if (strcmp(AttributeName, "Enabled") == 0)
		SetEnabled(Value.GetBoolean()) ;
	else if (strcmp(AttributeName, "Visible") == 0)
		SetVisible(Value.GetBoolean());
	else
		return false;

	return true;
}

bool ZEEntity::GetAttribute(const char* AttributeName, ZEVariant& Value)
{
	if (strcmp(AttributeName, "EntityId") == 0)
		Value = GetEntityId();
	else if (strcmp(AttributeName, "EntityName") == 0)
		Value = GetName();
	else if (strcmp(AttributeName, "Position") == 0)
		Value = GetPosition();
	else if (strcmp(AttributeName, "Rotation") == 0)
		Value = GetRotation();
	else if (strcmp(AttributeName, "Scale") == 0)
		Value = GetScale();
	else if (strcmp(AttributeName, "Enabled") == 0)
		Value = IsEnabled();
	else if (strcmp(AttributeName, "Visible") == 0)
		Value = IsVisible();
	else
	{
		Value.SetNull();
		return false;
	}

	return true;
}
*/
void ZEEntity::UpdateBoundingVolumes()
{
	if (BoundingVolumeMechanism == ZE_BVM_USEBOTH || BoundingVolumeMechanism == ZE_BVM_USECOMPONENTS)
	{
		UpdateBoundingSphere = true;
		UpdateBoundingBox = true;
	}
}

void ZEEntity::Tick(float Time)
{
	for (size_t I = 0; I < Components.GetCount(); I++)
		Components[I]->Tick(Time);

	ZEVector3::Sub(Velocity, Position, OldPosition);
	ZEVector3::Scale(Velocity, Velocity, 1.0f / Time);
	OldPosition = Position;
}

void ZEEntity::Update()
{

}

void ZEEntity::Draw(ZERenderer* Renderer, const ZESmartArray<const ZERLLight*>& Lights)
{
}
/*
bool ZEEntity::Serialize(ZESerializer* Serializer)
{
	ZEVariant Value;
	ZEEntityDescription* CurrDesc = GetEntityDescription();
	ZEDWORD TotalAttributeCount = 0;
	size_t AttributeCount;

	while (CurrDesc != NULL)
	{
		CurrDesc->GetAttributes(&AttributeCount);
		TotalAttributeCount += AttributeCount;
		CurrDesc = CurrDesc->GetParentType();
	}

	Serializer->Write(&TotalAttributeCount, sizeof(ZEDWORD), 1);

	CurrDesc = GetEntityDescription();
	while (CurrDesc != NULL)
	{

		const ZEEntityAttribute* Attributes = CurrDesc->GetAttributes(&AttributeCount);
		for (size_t I = 0; I < AttributeCount; I++)
		{
			Serializer->Write((void*)Attributes[I].Name, sizeof(char), ZE_MAX_NAME_SIZE);
			if (!GetAttribute(Attributes[I].Name, Value))
			{
				zeError("Entity Serialize", "Entity does not have specified attribute. (Entity Id: %d, Entity Name : \"%s\", Attribute Name : \"%s\")", GetEntityId(), GetName(), Attributes[I].Name);
				return false;
			}

			Value.Serialize(Serializer);
		}

		CurrDesc = CurrDesc->GetParentType();
	}

	return true;
}

bool ZEEntity::Unserialize(ZEUnserializer* Unserializer)
{	
	ZEDWORD AttributeCount;
	Unserializer->Read(&AttributeCount, sizeof(ZEDWORD), 1);

	for (size_t I = 0; I < AttributeCount; I++)
	{
		char AttributeName[ZE_MAX_NAME_SIZE];
		Unserializer->Read(AttributeName, sizeof(char), ZE_MAX_NAME_SIZE);

		ZEVariant AttributeValue;
		AttributeValue.Unserialize(Unserializer);

		if (!SetAttribute(AttributeName, AttributeValue))
		{
			zeError("Entity Unserialize", "Entity does not have specified attribute. (Entity Id: %d, Entity Name : \"%s\", Attribute Name : \"%s\")", GetEntityId(), GetName(), AttributeName);
			return false;
		}
	}

	return true;
}

 */
ZEEntity::ZEEntity()
{
	Name[0] = '\0';
	BoundingVolumeMechanism = ZE_BVM_USELOCALONLY;
	Position = ZEVector3(0.0f, 0.0f, 0.0f);
	Rotation = ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f);
	Scale = ZEVector3(1.0f, 1.0f ,1.0f);
	Enabled = true;
	UpdateBoundingBox = true;
	UpdateBoundingSphere = true;
	UpdateWorldTransform = true;
	HasLight = false;
	Drawable = false;
	Visible = true;
}

ZEEntity::~ZEEntity()
{
	ZEWARNINGASSERT(Components.GetCount() != 0, "Entity : %s. There are uninitialized component available.", GetClassDescription()->GetName());
}

#include "Entity.h.zpp"
