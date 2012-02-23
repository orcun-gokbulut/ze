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
#include "ZEGame/ZECompoundEntity.h"

void ZEComponent::SetLocalBoundingBox(const ZEAABBox& BoundingBox)
{
	if (Owner != NULL)
	{
		Owner->DirtyFlags.RaiseFlags(ZE_EDF_WORLD_BOUNDING_BOX);
	}

	ZEEntity::SetLocalBoundingBox(BoundingBox);
}

void ZEComponent::OnTransformChanged()
{
	DirtyFlags.RaiseFlags(ZE_EDF_LOCAL_TRANSFORM | ZE_EDF_WORLD_TRANSFORM | ZE_EDF_WORLD_BOUNDING_BOX);
}

ZEDrawFlags ZEComponent::GetDrawFlags() const
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
		if (DirtyFlags.GetFlags(ZE_EDF_WORLD_TRANSFORM))
		{
			ZEMatrix4x4::Multiply(((ZEComponent*)this)->WorldTransform, Owner->GetWorldTransform(), GetLocalTransform());
			((ZEComponent*)this)->DirtyFlags.UnraiseFlags(ZE_EDF_WORLD_TRANSFORM);
		}
	}
	else
		return GetLocalTransform();
}

const ZEMatrix4x4& ZEComponent::GetLocalTransform() const 
{
	if (DirtyFlags.GetFlags(ZE_EDF_LOCAL_TRANSFORM))
	{
		ZEMatrix4x4::CreateOrientation(((ZEComponent*)this)->LocalTransform, GetPosition(), GetRotation(), GetScale());
		((ZEComponent*)this)->DirtyFlags.UnraiseFlags(ZE_EDF_LOCAL_TRANSFORM);
	}

	return LocalTransform;
}

const ZEAABBox& ZEComponent::GetLocalBoundingBox() const
{
	return LocalBoundingBox;
}

const ZEAABBox& ZEComponent::GetWorldBoundingBox()
{
	if (DirtyFlags.GetFlags(ZE_EDF_WORLD_BOUNDING_BOX))
	{
		ZEAABBox::Transform(((ZEComponent*)this)->WorldBoundingBox, GetLocalBoundingBox(), GetWorldTransform());
		((ZEComponent*)this)->DirtyFlags.UnraiseFlags(ZE_EDF_WORLD_BOUNDING_BOX);
	}
	return WorldBoundingBox;
}

void ZEComponent::SetPosition(const ZEVector3& NewPosition)
{
	DirtyFlags.RaiseFlags(ZE_EDF_LOCAL_TRANSFORM);
	ZEEntity::SetPosition(NewPosition);
}

void ZEComponent::SetRotation(const ZEQuaternion& NewRotation)
{
	DirtyFlags.RaiseFlags(ZE_EDF_LOCAL_TRANSFORM);
	ZEEntity::SetRotation(NewRotation);
}

void ZEComponent::SetScale(const ZEVector3& NewScale)
{
	DirtyFlags.RaiseFlags(ZE_EDF_LOCAL_TRANSFORM);
	ZEEntity::SetScale(NewScale);
}

const ZEVector3 ZEComponent::GetWorldPosition() const
{
	if (Owner != NULL)
	{
		ZEVector3 Temp;
		ZEMatrix4x4::Transform(Temp, Owner->GetWorldTransform(), GetPosition());
		//ZEVector3::Add(Temp, Owner->GetPosition(), GetPosition());
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

ZEComponent::ZEComponent()
{
	Owner = NULL;
	DirtyFlags.RaiseFlags(ZE_EDF_LOCAL_TRANSFORM);
}

ZEEntityRunAt ZEComponentDescription::GetRunAt() const
{
	return ZE_ERA_NONE;
}





