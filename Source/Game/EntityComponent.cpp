//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - EntityComponent.cpp
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

#include "EntityComponent.h"
#include "Game/CompoundEntity.h"

// ZEComponentDirtyTransfromFlags
#define ZE_CDF_ALL							0xFFFFFFFF
#define ZE_CDF_LOCAL_TRANSFORM				1
#define ZE_CDF_WORLD_TRANSFORM				2
#define ZE_CDF_WORLD_BOUNDING_SPHERE		4
#define ZE_CDF_WORLD_BOUNDING_BOX			8

void ZEEntityComponent::UpdateBoundingVolumes()
{
	DirtyFlags = ZE_CDF_ALL;

	if (Owner != NULL)
		Owner->UpdateBoundingVolumes();
}

void ZEEntityComponent::SetLocalBoundingBox(const ZEAABoundingBox& BoundingBox)
{
	LocalBoundingBox = BoundingBox;
	DirtyFlags |= ZE_CDF_WORLD_BOUNDING_BOX | ZE_CDF_WORLD_BOUNDING_SPHERE;

	if (Owner != NULL)
		Owner->UpdateBoundingVolumes();
}

ZEDWORD ZEEntityComponent::GetDrawFlags() const
{
	return ZE_DF_NONE;
}

ZEEntityType ZEEntityComponent::GetEntityType()
{
	return ZE_ET_COMPONENT;
}

ZECompoundEntity* ZEEntityComponent::GetOwner() const
{
	return Owner;
}

const ZEVector3& ZEEntityComponent::GetWorldVelocity()
{
	return GetVelocity();
}

ZEVector3 ZEEntityComponent::GetWorldDirection()
{
	if (Owner != NULL)
		return GetWorldRotation() * ZEVector3::UnitZ;
	else
		return GetWorldDirection();
}

ZEVector3 ZEEntityComponent::GetWorldRight()
{
	if (Owner != NULL)
		return GetWorldRotation() * ZEVector3::UnitX;
	else
		return GetRight();
}

ZEVector3 ZEEntityComponent::GetWorldUp()
{
	if (Owner != NULL)
		return GetWorldRotation() * ZEVector3::UnitY;
	else
		return GetWorldUp();
}

const ZEMatrix4x4& ZEEntityComponent::GetWorldTransform() const
{
	if (Owner != NULL)
	{
		if (DirtyFlags & ZE_CDF_WORLD_TRANSFORM)
		{
			ZEMatrix4x4::Multiply(((ZEEntityComponent*)this)->WorldTransform, GetLocalTransform(), Owner->GetWorldTransform());
			((ZEEntityComponent*)this)->DirtyFlags &= ~ZE_CDF_WORLD_TRANSFORM;
		}

		return WorldTransform;
	}
	else
		return LocalTransform;
}

const ZEMatrix4x4& ZEEntityComponent::GetLocalTransform() const 
{
	if (DirtyFlags & ZE_CDF_LOCAL_TRANSFORM)
	{
		ZEMatrix4x4::CreateOrientation(((ZEEntityComponent*)this)->LocalTransform, GetPosition(), GetRotation(), GetScale());
		((ZEEntityComponent*)this)->DirtyFlags &= ~ZE_CDF_LOCAL_TRANSFORM;
	}

	return LocalTransform;
}

const ZEAABoundingBox& ZEEntityComponent::GetWorldBoundingBox() const
{
	if (DirtyFlags & ZE_CDF_WORLD_BOUNDING_BOX)
	{
		ZEAABoundingBox::Transform(((ZEEntityComponent*)this)->WorldBoundingBox, GetLocalBoundingBox(), GetWorldTransform());
		((ZEEntityComponent*)this)->DirtyFlags &= ~ZE_CDF_WORLD_BOUNDING_BOX;
	}

	return WorldBoundingBox;
}

const ZEBoundingSphere& ZEEntityComponent::GetWorldBoundingSphere() const
{
	if (DirtyFlags & ZE_CDF_WORLD_BOUNDING_SPHERE)
	{
		GetWorldBoundingBox().GenerateBoundingSphere(((ZEEntityComponent*)this)->WorldBoundingSphere);
		((ZEEntityComponent*)this)->DirtyFlags &= ~ZE_CDF_WORLD_BOUNDING_SPHERE;
	}

	return WorldBoundingSphere;
}

void ZEEntityComponent::SetPosition(const ZEVector3& NewPosition)
{
	DirtyFlags = ZE_CDF_ALL;

	ZEEntity::SetPosition(NewPosition);

	if (GetDrawFlags() | ZE_DF_CULL && Owner != NULL)
		Owner->UpdateBoundingVolumes();
}

void ZEEntityComponent::SetRotation(const ZEQuaternion& NewRotation)
{
	DirtyFlags = ZE_CDF_ALL;

	ZEEntity::SetRotation(NewRotation);

	if (GetDrawFlags() | ZE_DF_CULL && Owner != NULL)
		Owner->UpdateBoundingVolumes();
}

void ZEEntityComponent::SetScale(const ZEVector3& NewScale)
{
	DirtyFlags = ZE_CDF_ALL;

	ZEEntity::SetScale(NewScale);

	if (GetDrawFlags() | ZE_DF_CULL && Owner != NULL)
		Owner->UpdateBoundingVolumes();
}


ZEVector3 ZEEntityComponent::GetWorldPosition() const
{
	if (Owner != NULL)
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, GetWorldTransform(), GetPosition());
		return Temp;
	}
	else
		return GetPosition();
}

ZEQuaternion ZEEntityComponent::GetWorldRotation() const
{
	if (Owner != NULL)
	{
		ZEQuaternion Temp, Temp1;
		ZEQuaternion::Product(Temp, Owner->GetRotation(), GetRotation());
		ZEQuaternion::Normalize(Temp1, Temp);
		return Temp1;
	}
	else
		return GetRotation();
}

bool ZEEntityComponent::CastRay(const ZERay& Ray, ZEVector3& Position, ZEVector3& Normal, float& TEnterance, float &TExit)
{
	return false;
}

void ZEEntityComponent::Tick(float Time)
{
	const ZEVector3& WorldPosition = GetWorldPosition();

/*	ZEVector3::Sub(LocalVelocity, WorldPosition, LocalOldPosition);
	ZEVector3::Scale(LocalVelocity, LocalVelocity, Time);*/
}

void ZEEntityComponent::Draw(ZEDrawParameters* DrawParameters)
{
}

bool ZEEntityComponent::Initialize()
{
	return true;
}

void ZEEntityComponent::Deinitialize()
{

}

void ZEEntityComponent::Destroy()
{
	delete this;
}

void ZEEntityComponent::OwnerWorldTransformChanged()
{
	DirtyFlags |= ZE_CDF_WORLD_TRANSFORM | ZE_CDF_WORLD_BOUNDING_BOX | ZE_CDF_WORLD_BOUNDING_BOX;
}


ZEEntityComponent::ZEEntityComponent()
{
	Owner = NULL;

	DirtyFlags = ZE_CDF_ALL;
}

#include "EntityComponent.h.zpp"

ZEEntityRunAt ZEEntityComponentDescription::GetRunAt() const
{
	return ZE_ERA_NONE;
}
