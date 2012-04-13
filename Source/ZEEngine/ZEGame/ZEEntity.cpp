//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEEntity.cpp
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

#include "ZEEntity.h"
#include "ZEMath/ZERay.h"
#include "ZEError.h"
#include "ZEEntityProvider.h"
#include "ZEScene.h"
#include <string.h>


void ZEEntity::OnTransformChanged()
{
	DirtyFlags.RaiseFlags(ZE_EDF_LOCAL_TRANSFORM | ZE_EDF_WORLD_TRANSFORM | ZE_EDF_WORLD_BOUNDING_BOX);

	for (ZESize I = 0; I < Components.GetCount(); I++)
		Components[I]->OnTransformChanged();

	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		ChildEntities[I]->OnTransformChanged();
}

bool ZEEntity::AddComponent(ZEEntity* Entity)
{
	if (Entity->Owner != NULL)
	{
		zeError("Component already has an owner. Can not register component.");
		return false;
	}

	Entity->Owner = this;
	Entity->OwnerScene = OwnerScene;

	if (!GetInitialized())
		Entity->Initialize();

	Components.Add(Entity);
	return true;
}

void ZEEntity::RemoveComponent(ZEEntity* Entity)
{
	if (!Components.Exists(Entity))
	{
		zeError("Can not remove non-child entity.");
		return;
	}

	if (GetInitialized())
		Entity->Deinitialize();

	Components.DeleteValue(Entity);

	Entity->Owner = NULL;
	Entity->OwnerScene = NULL;
}

bool ZEEntity::AddChildEntity(ZEEntity* Entity)
{
	if (Entity->Owner != NULL)
	{
		zeError("Entity already has an owner. Can not register entity.");
		return false;
	}

	if (Entity->OwnerScene != NULL)
	{
		zeError("Target entity is active in a scene. Can not register target entity.");
		return false;
	}

	if (this->OwnerScene != NULL)
	{
		zeError("Parent entity is active in a scene. Can not register target entity.");
		return false;
	}

	Entity->OwnerScene = this->OwnerScene;
	Entity->Owner = this;

	if (!GetInitialized())
		Entity->Initialize();

	ChildEntities.Add(Entity);
	return true;
}

void ZEEntity::RemoveChildEntity(ZEEntity* Entity)
{
	if (!ChildEntities.Exists(Entity))
	{
		zeError("Can not remove non-child entity.");
		return;
	}

	ChildEntities.DeleteValue(Entity);

	if (GetInitialized())
		Entity->Deinitialize();
	
	Entity->Owner = NULL;
	Entity->OwnerScene = NULL;
}

void ZEEntity::SetBoundingBox(const ZEAABBox& BoundingBox)
{
	this->BoundingBox = BoundingBox;
	DirtyFlags.RaiseFlags(ZE_EDF_WORLD_BOUNDING_BOX);
}

ZEEntity::ZEEntity()
{
	Owner = NULL;
	OwnerScene = NULL;
	DirtyFlags.RaiseFlags(ZE_EDF_LOCAL_TRANSFORM | ZE_EDF_WORLD_BOUNDING_BOX | ZE_EDF_WORLD_TRANSFORM);
	Position = ZEVector3(0.0f, 0.0f, 0.0f);
	Rotation = ZEQuaternion::Identity;
	Scale = ZEVector3::One;
	Enabled = true;
	Visible = true;
	Initialized = false;
}

ZEEntity::~ZEEntity()
{
	Deinitialize();

	for (ZESize I = 0; I < Components.GetCount(); I++)
		delete Components[I];

	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		delete ChildEntities[I];
}

ZEDrawFlags ZEEntity::GetDrawFlags() const
{
	return ZE_DF_NONE;
}

ZEEntity* ZEEntity::GetOwner() const
{
	return Owner;
}

void ZEEntity::SetOwnerScene( ZEScene* Scene )
{
	this->OwnerScene = Scene;
}

ZEScene* ZEEntity::GetOwnerScene() const
{
	return OwnerScene;
}

const ZEArray<ZEEntity*>& ZEEntity::GetComponents() const
{
	return Components;
}

const ZEArray<ZEEntity*>& ZEEntity::GetChildEntities() const
{
	return ChildEntities; 
}

const ZEAABBox& ZEEntity::GetBoundingBox() const
{
	return BoundingBox;
}

const ZEAABBox& ZEEntity::GetWorldBoundingBox()
{
	if (DirtyFlags.GetFlags(ZE_EDF_WORLD_BOUNDING_BOX))
	{
		ZEAABBox::Transform(WorldBoundingBox, GetBoundingBox(), GetWorldTransform());
		DirtyFlags.UnraiseFlags(ZE_EDF_WORLD_BOUNDING_BOX);
	}

	return WorldBoundingBox;
}

const ZEMatrix4x4& ZEEntity::GetTransform() const
{
	if (DirtyFlags.GetFlags(ZE_EDF_LOCAL_TRANSFORM))
	{
		ZEMatrix4x4::CreateOrientation(Transform, GetPosition(), GetRotation(), GetScale());
		DirtyFlags.UnraiseFlags(ZE_EDF_LOCAL_TRANSFORM);
	}

	return Transform;
}

const ZEMatrix4x4& ZEEntity::GetWorldTransform() const
{
	if (Owner != NULL)
	{
		if (DirtyFlags.GetFlags(ZE_EDF_WORLD_TRANSFORM))
		{
			ZEMatrix4x4::Multiply(WorldTransform, Owner->GetWorldTransform(), GetTransform());
			DirtyFlags.UnraiseFlags(ZE_EDF_WORLD_TRANSFORM);
		}

		return WorldTransform;
	}

	return GetTransform();
}

bool ZEEntity::GetInitialized()
{
	return Initialized;
}

void ZEEntity::SetEntityId(ZEInt EntityId)
{
	this->EntityId = EntityId;
}

ZEInt ZEEntity::GetEntityId() const
{
	return EntityId;
}

void ZEEntity::SetName(const char* NewName)
{
	Name = NewName;
}

const char* ZEEntity::GetName() const
{
	return Name;
}

void ZEEntity::SetVisible(bool Visibility)
{
	this->Visible = Visibility;
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

void ZEEntity::SetPosition(const ZEVector3& NewPosition)
{
	Position = NewPosition;
	OnTransformChanged();
}

const ZEVector3& ZEEntity::GetPosition() const
{
	return Position;
}

void ZEEntity::SetWorldPosition(const ZEVector3& NewPosition)
{
	if (Owner != NULL)
		SetPosition(NewPosition - Owner->GetWorldPosition());
	else
		SetPosition(NewPosition);
}

const ZEVector3 ZEEntity::GetWorldPosition() const
{
	if (Owner != NULL)
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, Owner->GetWorldTransform(), GetPosition());
		//ZEVector3::Add(Temp, Owner->GetPosition(), GetPosition());
		return Temp;
	}

	return GetPosition();
}

void ZEEntity::SetRotation(const ZEQuaternion& NewRotation)
{
	Rotation = NewRotation;
	OnTransformChanged();
}

const ZEQuaternion& ZEEntity::GetRotation() const
{
	return Rotation;
}

void ZEEntity::SetWorldRotation(const ZEQuaternion& NewRotation)
{
	if (Owner != NULL)
	{
		ZEQuaternion Temp, Result;
		ZEQuaternion::Conjugate(Temp, Owner->GetWorldRotation());
		ZEQuaternion::Product(Result, NewRotation, Temp);
		SetRotation(Result);
	}
	else
		SetRotation(NewRotation);
}

const ZEQuaternion ZEEntity::GetWorldRotation() const
{
	if (Owner != NULL)
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, Owner->GetWorldRotation(), GetRotation());
		ZEQuaternion::Normalize(Temp, Temp);
		return Temp;
	}

	return GetRotation();
}

void ZEEntity::SetScale(const ZEVector3& NewScale)
{
	Scale = NewScale;
	OnTransformChanged();
}

const ZEVector3& ZEEntity::GetScale() const
{
	return Scale;
}

void ZEEntity::SetWorldScale(const ZEVector3& NewScale)
{
	if (Owner != NULL)
	{
		ZEVector3 Temp;
		ZEVector3::Divide(Temp, NewScale, Owner->GetWorldScale());
		SetScale(Temp);
	}
	else
		SetScale(NewScale);
}

const ZEVector3 ZEEntity::GetWorldScale() const
{
	if (Owner != NULL)
	{
		ZEVector3 Temp;
		ZEVector3::Multiply(Temp, Owner->GetWorldScale(), GetScale());
		return Temp;
	}

	return GetScale();
}

ZEVector3 ZEEntity::GetFront()
{
	return Rotation * ZEVector3::UnitZ;
}

ZEVector3 ZEEntity::GetRight()
{
	return Rotation * ZEVector3::UnitX;
}

ZEVector3 ZEEntity::GetUp()
{
	return Rotation * ZEVector3::UnitY;
}

ZEVector3 ZEEntity::GetWorldFront() const
{
	return GetWorldRotation() * ZEVector3::UnitZ;
}

ZEVector3 ZEEntity::GetWorldRight() const
{
	return GetWorldRotation() * ZEVector3::UnitX;
}

ZEVector3 ZEEntity::GetWorldUp() const
{
	return GetWorldRotation() * ZEVector3::UnitY;
}

bool ZEEntity::Initialize()
{
	if (GetInitialized())
		return false;

// 	if (OwnerScene == NULL)
// 		OwnerScene = zeScene;

	for (ZESize I = 0; I < Components.GetCount(); I++)
		if(!Components[I]->GetInitialized())
			Components[I]->Initialize();

	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		if(!ChildEntities[I]->GetInitialized())
			ChildEntities[I]->Initialize();

	Initialized = true;
	return true;
}

void ZEEntity::Deinitialize()
{
	if (!GetInitialized())
		return;
	
	for (ZESize I = 0; I < Components.GetCount(); I++)
		if(Components[I]->GetInitialized())
			Components[I]->Deinitialize();

	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		if(ChildEntities[I]->GetInitialized())
			ChildEntities[I]->Deinitialize();

	Initialized = false;
}

void ZEEntity::Destroy()
{
	Deinitialize();
	delete this;
}

void ZEEntity::Tick(float Time)
{

}

void ZEEntity::Draw(ZEDrawParameters* DrawParameters)
{

}

ZEEntityRunAt ZEEntityDescription::GetRunAt() const
{
	return ZE_ERA_NONE;
}
