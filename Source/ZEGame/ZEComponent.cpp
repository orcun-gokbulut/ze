//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEComponent.cpp
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

#include "ZEComponent.h"
#include "ZEGame\ZECompoundEntity.h"

// ZEComponentDirtyTransfromFlags
#define ZE_CDF_ALL							0xFFFFFFFF
#define ZE_CDF_LOCAL_TRANSFORM				1
#define ZE_CDF_WORLD_TRANSFORM				2
#define ZE_CDF_WORLD_BOUNDING_SPHERE		4
#define ZE_CDF_WORLD_BOUNDING_BOX			8

void ZEComponent::UpdateBoundingVolumes()
{
	DirtyFlags = ZE_CDF_ALL;

	if (Owner != NULL)
		Owner->UpdateBoundingVolumes();
}

void ZEComponent::SetLocalBoundingBox(const ZEAABoundingBox& BoundingBox)
{
	LocalBoundingBox = BoundingBox;
	DirtyFlags |= ZE_CDF_WORLD_BOUNDING_BOX | ZE_CDF_WORLD_BOUNDING_SPHERE;

	if (Owner != NULL)
		Owner->UpdateBoundingVolumes();
}

ZEDWORD ZEComponent::GetDrawFlags() const
{
	return ZE_DF_NONE;
}

ZEEntityType ZEComponent::GetEntityType()
{
	return ZE_ET_COMPONENT;
}

ZECompoundEntity* ZEComponent::GetOwner() const
{
	return Owner;
}

const ZEVector3& ZEComponent::GetWorldVelocity()
{
	return ZEVector3::Zero;// LocalVelocity;
}

ZEVector3 ZEComponent::GetWorldDirection() const
{
	return GetWorldRotation() * ZEVector3::UnitZ;
}

ZEVector3 ZEComponent::GetWorldRight() const
{
	return GetWorldRotation() * ZEVector3::UnitX;
}

ZEVector3 ZEComponent::GetWorldUp() const
{
	return GetWorldRotation() * ZEVector3::UnitY;
}

const ZEMatrix4x4& ZEComponent::GetWorldTransform() const
{
	if (Owner != NULL)
	{
		//if (DirtyFlags & ZE_CDF_WORLD_TRANSFORM)
		{
			ZEMatrix4x4::Multiply(((ZEComponent*)this)->WorldTransform, GetLocalTransform(), Owner->GetWorldTransform());
			((ZEComponent*)this)->DirtyFlags &= ~ZE_CDF_WORLD_TRANSFORM;
		}

		return WorldTransform;
	}
	else
		return GetLocalTransform();
}

const ZEMatrix4x4& ZEComponent::GetLocalTransform() const 
{
	//if (DirtyFlags & ZE_CDF_LOCAL_TRANSFORM)
	{
		ZEMatrix4x4::CreateOrientation(((ZEComponent*)this)->LocalTransform, GetPosition(), GetRotation(), GetScale());
		((ZEComponent*)this)->DirtyFlags &= ~ZE_CDF_LOCAL_TRANSFORM;
	}

	return LocalTransform;
}

const ZEAABoundingBox& ZEComponent::GetLocalBoundingBox() const
{
	return LocalBoundingBox;
}

const ZEAABoundingBox& ZEComponent::GetWorldBoundingBox() const
{
	//if (DirtyFlags & ZE_CDF_WORLD_BOUNDING_BOX)
	{
		ZEAABoundingBox::Transform(((ZEComponent*)this)->WorldBoundingBox, GetLocalBoundingBox(), GetWorldTransform());
		((ZEComponent*)this)->DirtyFlags &= ~ZE_CDF_WORLD_BOUNDING_BOX;
	}

	return WorldBoundingBox;
}

const ZEBoundingSphere& ZEComponent::GetWorldBoundingSphere() const
{
	//if (DirtyFlags & ZE_CDF_WORLD_BOUNDING_SPHERE)
	{
		GetWorldBoundingBox().GenerateBoundingSphere(((ZEComponent*)this)->WorldBoundingSphere);
		((ZEComponent*)this)->DirtyFlags &= ~ZE_CDF_WORLD_BOUNDING_SPHERE;
	}

	return WorldBoundingSphere;
}

void ZEComponent::SetPosition(const ZEVector3& NewPosition)
{
	ZEEntity::SetPosition(NewPosition);

	if (GetDrawFlags() | ZE_DF_CULL && Owner != NULL)
		Owner->UpdateBoundingVolumes();
}

void ZEComponent::SetRotation(const ZEQuaternion& NewRotation)
{
	ZEEntity::SetRotation(NewRotation);

	if (GetDrawFlags() | ZE_DF_CULL && Owner != NULL)
		Owner->UpdateBoundingVolumes();
}

void ZEComponent::SetScale(const ZEVector3& NewScale)
{
	ZEEntity::SetScale(NewScale);

	if (GetDrawFlags() | ZE_DF_CULL && Owner != NULL)
		Owner->UpdateBoundingVolumes();
}


const ZEVector3 ZEComponent::GetWorldPosition() const
{
	if (Owner != NULL)
	{
		ZEVector3 Temp;
		ZEVector3::Add(Temp, Owner->GetPosition(), GetPosition());
		return Temp;
	}
	else
		return GetPosition();
}

const ZEQuaternion ZEComponent::GetWorldRotation() const
{
	if (Owner != NULL)
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, Owner->GetRotation(), GetRotation());
		ZEQuaternion::Normalize(Temp, Temp);
		return Temp;
	}
	else
		return GetRotation();
}

bool ZEComponent::CastRay(const ZERay& Ray, ZEVector3& Position, ZEVector3& Normal, float& TEnterance, float &TExit)
{
	return false;
}

void ZEComponent::Tick(float Time)
{
	const ZEVector3& WorldPosition = GetWorldPosition();

/*	ZEVector3::Sub(LocalVelocity, WorldPosition, LocalOldPosition);
	ZEVector3::Scale(LocalVelocity, LocalVelocity, Time);*/
}

void ZEComponent::Draw(ZEDrawParameters* DrawParameters)
{
}

bool ZEComponent::Initialize()
{
	return true;
}

void ZEComponent::Deinitialize()
{

}

void ZEComponent::Destroy()
{
	Deinitialize();
	delete this;
}

void ZEComponent::OwnerWorldTransformChanged()
{
	DirtyFlags |= ZE_CDF_WORLD_TRANSFORM | ZE_CDF_WORLD_BOUNDING_BOX | ZE_CDF_WORLD_BOUNDING_BOX;
}


ZEComponent::ZEComponent()
{
	Owner = NULL;

	DirtyFlags = ZE_CDF_ALL;
	Visible = true;
	Enabled = true;
}

#include "ZEComponent.h.zpp"

ZEEntityRunAt ZEComponentDescription::GetRunAt() const
{
	return ZE_ERA_NONE;
}





