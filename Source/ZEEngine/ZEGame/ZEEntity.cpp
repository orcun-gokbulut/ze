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
	EntityDirtyFlags.RaiseFlags(ZE_EDF_ALL & ~ZE_EDF_LOCAL_TRANSFORM);

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

	if (!(Entity->OwnerScene == NULL || Entity->OwnerScene == this->OwnerScene))
	{
		zeError("Component already has been entitled to another scene. Can not register component.");
		return false;
	}

	if (State == ZE_ES_DEINITIALIZING)
		return false;

	Entity->Owner = this;
	Entity->SetOwnerScene(this->OwnerScene);

	Components.Add(Entity);

	if (State == ZE_ES_INITIALIZING || State == ZE_ES_INITIALIZED)
		Entity->Initialize();

	if(Entity->GetName().GetLength() == 0)
		Entity->SetName(GetName() + "->" +  Entity->GetDescription()->GetName());

	return true;
}

void ZEEntity::RemoveComponent(ZEEntity* Entity)
{
	if (Entity->Owner != this)
	{
		zeError("Can not remove non-component entity.");
		return;
	}

	Components.DeleteValue(Entity);

	Entity->Owner = NULL;
	Entity->SetOwnerScene(NULL);
}

bool ZEEntity::AddChildEntity(ZEEntity* Entity)
{
	if (Entity->Owner != NULL)
	{
		zeError("Entity already has an owner. Can not register entity.");
		return false;
	}

	if (!(Entity->OwnerScene == NULL || Entity->OwnerScene == this->OwnerScene))
	{
		zeError("Child entity already has been entitled to another scene. Can not register child entity.");
		return false;
	}

	if (State == ZE_ES_DEINITIALIZING)
		return false;

	Entity->Owner = this;
	Entity->SetOwnerScene(this->OwnerScene);

	ChildEntities.Add(Entity);

	if (State == ZE_ES_INITIALIZED)
		Entity->Initialize();

	if(Entity->GetName().GetLength() == 0)
		Entity->SetName(GetName() + "->" +  Entity->GetDescription()->GetName());

	return true;
}

void ZEEntity::RemoveChildEntity(ZEEntity* Entity)
{
	if (Entity->Owner != this)
	{
		zeError("Can not remove non-child entity.");
		return;
	}

	ChildEntities.DeleteValue(Entity);

	Entity->Owner = NULL;
	Entity->SetOwnerScene(NULL);
}

void ZEEntity::SetBoundingBox(const ZEAABBox& BoundingBox)
{
	this->BoundingBox = BoundingBox;
	EntityDirtyFlags.RaiseFlags(ZE_EDF_WORLD_BOUNDING_BOX);
}

bool ZEEntity::SetOwner(ZEEntity* Owner)
{
	this->Owner = Owner;
	OnTransformChanged();

	return true;
}

void ZEEntity::SetOwnerScene(ZEScene* Scene)
{
	this->OwnerScene = Scene;

	ZESize SubItemCount = Components.GetCount();

	for (ZESize I = 0; I < SubItemCount; I++)
		Components[I]->SetOwnerScene(Scene);

	SubItemCount = ChildEntities.GetCount();

	for (ZESize I = 0; I < SubItemCount; I++)
		ChildEntities[I]->SetOwnerScene(Scene);
}

void ZEEntity::SetBoundingBox(const ZEAABBox& BoundingBox) const
{
	this->BoundingBox = BoundingBox;
	EntityDirtyFlags.RaiseFlags(ZE_EDF_WORLD_BOUNDING_BOX);
}

bool ZEEntity::InitializeSelf()
{
	State = ZE_ES_INITIALIZING;
	return true;
}

bool ZEEntity::DeinitializeSelf()
{
	State = ZE_ES_NOT_INITIALIZED;
	return true;
}

ZEEntity::ZEEntity()
{
	Owner = NULL;
	OwnerScene = NULL;
	EntityDirtyFlags.RaiseFlags(ZE_EDF_ALL);
	Position = ZEVector3(0.0f, 0.0f, 0.0f);
	Rotation = ZEQuaternion::Identity;
	Scale = ZEVector3::One;
	Enabled = true;
	Visible = true;
	State = ZE_ES_NOT_INITIALIZED;
}

ZEEntity::~ZEEntity()
{
	Deinitialize();

	for (ZESize I = 0; I < Components.GetCount(); I++)
		Components[I]->Destroy();

	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		ChildEntities[I]->Destroy();
}

ZEDrawFlags ZEEntity::GetDrawFlags() const
{
	return ZE_DF_NONE;
}

ZEEntity* ZEEntity::GetOwner() const
{
	return Owner;
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

const ZEAABBox& ZEEntity::GetWorldBoundingBox() const
{
	if (EntityDirtyFlags.GetFlags(ZE_EDF_WORLD_BOUNDING_BOX))
	{
		ZEAABBox::Transform(WorldBoundingBox, GetBoundingBox(), GetWorldTransform());
		EntityDirtyFlags.UnraiseFlags(ZE_EDF_WORLD_BOUNDING_BOX);
	}

	return WorldBoundingBox;
}

const ZEMatrix4x4& ZEEntity::GetTransform() const
{
	if (EntityDirtyFlags.GetFlags(ZE_EDF_LOCAL_TRANSFORM))
	{
		ZEMatrix4x4::CreateOrientation(Transform, GetPosition(), GetRotation(), GetScale());
		EntityDirtyFlags.UnraiseFlags(ZE_EDF_LOCAL_TRANSFORM);
	}

	return Transform;
}

const ZEMatrix4x4& ZEEntity::GetWorldTransform() const
{
	if (Owner == NULL)
	{
		EntityDirtyFlags.UnraiseFlags(ZE_EDF_WORLD_TRANSFORM);
		return GetTransform();
	}

	if (EntityDirtyFlags.GetFlags(ZE_EDF_WORLD_TRANSFORM))
	{
		ZEMatrix4x4::Multiply(WorldTransform, Owner->GetWorldTransform(), GetTransform());
		EntityDirtyFlags.UnraiseFlags(ZE_EDF_WORLD_TRANSFORM);
	}

	return WorldTransform;

}

const ZEMatrix4x4& ZEEntity::GetInvWorldTransform() const
{
	if (EntityDirtyFlags.GetFlags(ZE_EDF_INV_WORLD_TRANSFORM))
	{
		ZEMatrix4x4::Inverse(InvWorldTransform, GetWorldTransform());
		EntityDirtyFlags.UnraiseFlags(ZE_EDF_INV_WORLD_TRANSFORM);
	}

	return InvWorldTransform;
}

bool ZEEntity::IsInitialized()
{
	return (State == ZE_ES_INITIALIZED);
}

ZEEntityState ZEEntity::GetState()
{
	return State;
}

void ZEEntity::SetEntityId(ZEInt EntityId)
{
	this->EntityId = EntityId;
}

ZEInt ZEEntity::GetEntityId() const
{
	return EntityId;
}

void ZEEntity::SetName(ZEString NewName)
{
	Name = NewName;
}

ZEString ZEEntity::GetName() const
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

	EntityDirtyFlags.RaiseFlags(ZE_EDF_LOCAL_TRANSFORM);
	OnTransformChanged();
}

const ZEVector3& ZEEntity::GetPosition() const
{
	return Position;
}

void ZEEntity::SetWorldPosition(const ZEVector3& NewPosition)
{
	if (Owner != NULL)
	{
		ZEVector3 Result;
		ZEMatrix4x4::Transform(Result, Owner->GetInvWorldTransform(), NewPosition);
		SetPosition(Result);
	}
	else
		SetPosition(NewPosition);
}

const ZEVector3 ZEEntity::GetWorldPosition() const
{
	if (Owner != NULL)
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, Owner->GetWorldTransform(), GetPosition());
		return Temp;
	}

	return GetPosition();
}

void ZEEntity::SetRotation(const ZEQuaternion& NewRotation)
{
	Rotation = NewRotation;

	EntityDirtyFlags.RaiseFlags(ZE_EDF_LOCAL_TRANSFORM);
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
		ZEQuaternion::Product(Result, Temp, NewRotation);
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

	EntityDirtyFlags.RaiseFlags(ZE_EDF_LOCAL_TRANSFORM);
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
	if (IsInitialized())
		return false;

	//Owner Scene mevzusu addChild/Comp da ve AddtoScene de olcak

	for (ZESize I = 0; I < Components.GetCount(); I++)
		if(!Components[I]->Initialize())
			return false;

	if (!InitializeSelf())
		return false;

	if (State != ZE_ES_INITIALIZING)
		return false;

	State = ZE_ES_INITIALIZED;

	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		if(!ChildEntities[I]->Initialize())
			return false;

	return true;
}

bool ZEEntity::Deinitialize()
{
	if (!IsInitialized())
		return true;
	
	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
		if(!ChildEntities[I]->Deinitialize())
			return false;

	State = ZE_ES_DEINITIALIZING;

	if (!DeinitializeSelf())
		return false;

	if (State != ZE_ES_NOT_INITIALIZED)
		return false;

	for (ZESize I = 0; I < Components.GetCount(); I++)
		if(!Components[I]->Deinitialize())
			return false;

	return true;
}

void ZEEntity::Destroy()
{
	delete this;
}

void ZEEntity::Tick(float Time)
{

}

void ZEEntity::Draw(ZEDrawParameters* DrawParameters)
{

}

bool ZEEntity::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	if (!ZEAABBox::IntersectionTest(GetWorldBoundingBox(), Parameters.Ray))
		return false;

	ZERay LocalRay;
	ZEMatrix4x4::Transform(LocalRay.p, GetInvWorldTransform(), Parameters.Ray.p);
	ZEMatrix4x4::Transform3x3(LocalRay.v, GetInvWorldTransform(), Parameters.Ray.v);
	LocalRay.v.NormalizeSelf();

	float TMin;
	if (!ZEAABBox::IntersectionTest(BoundingBox, LocalRay, TMin))
		return false;

	ZEVector3 IntersectionPoint;
	ZEMatrix4x4::Transform(IntersectionPoint, GetWorldTransform(), LocalRay.GetPointOn(TMin));
	float DistanceSquare = IntersectionPoint.LengthSquare();
	if (Report.Distance * Report.Distance > DistanceSquare && Report.Distance * Report.Distance < Parameters.MaximumDistance)
	{
		Report.Distance = ZEMath::Sqrt(DistanceSquare);
		Report.Entity = this;
		Report.SubComponent = NULL;
		Report.PoligonIndex = 0;
		Report.Normal = Report.Binormal = ZEVector3::Zero;
		ZEMatrix4x4::Transform(Report.Position, WorldTransform, IntersectionPoint);
	}
}

ZEEntityRunAt ZEEntityDescription::GetRunAt() const
{
	return ZE_ERA_NONE;
}
