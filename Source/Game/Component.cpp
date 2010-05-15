//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Component.cpp
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

#include "Component.h"
#include "Game/Entity.h"

// ZEComponentDirtyTransfromFlags
#define ZE_CDF_ALL							0xFFFFFFFF
#define ZE_CDF_LOCAL_TRANSFORM				1
#define ZE_CDF_WORLD_TRANSFORM				2
#define ZE_CDF_WORLD_BOUNDING_SPHERE		4
#define ZE_CDF_WORLD_BOUNDING_BOX			8

void ZEComponent::UpdateBoundingVolumes()
{
	DirtyFlags = ZE_CDF_ALL;
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

void ZEComponent::SetOwner(ZEEntity* NewOwner)
{
	Owner = NewOwner;

	DirtyFlags |= ZE_CDF_WORLD_TRANSFORM | ZE_CDF_WORLD_BOUNDING_BOX | ZE_CDF_WORLD_BOUNDING_SPHERE;

	if (Visible && Owner != NULL)
		Owner->UpdateBoundingVolumes();
}

ZEEntity* ZEComponent::GetOwner() const
{
	return Owner;
}

const ZEVector3& ZEComponent::GetWorldVelocity()
{
	return LocalVelocity;
}


const ZEMatrix4x4& ZEComponent::GetWorldTransform() const
{
	if (Owner != NULL)
	{
		if (DirtyFlags & ZE_CDF_WORLD_TRANSFORM)
		{
			ZEMatrix4x4::Multiply(((ZEComponent*)this)->WorldTransform, GetLocalTransform(), Owner->GetWorldTransform());
			((ZEComponent*)this)->DirtyFlags &= ~ZE_CDF_WORLD_TRANSFORM;
		}

		return WorldTransform;
	}
	else
		return LocalTransform;
}

const ZEMatrix4x4& ZEComponent::GetLocalTransform() const 
{
	if (DirtyFlags & ZE_CDF_LOCAL_TRANSFORM)
	{
		ZEMatrix4x4::CreateOrientation(((ZEComponent*)this)->LocalTransform, LocalPosition, LocalRotation, LocalScale);
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
	if (DirtyFlags & ZE_CDF_WORLD_BOUNDING_BOX)
	{
		ZEAABoundingBox::Transform(((ZEComponent*)this)->WorldBoundingBox, GetLocalBoundingBox(), GetWorldTransform());
		((ZEComponent*)this)->DirtyFlags &= ~ZE_CDF_WORLD_BOUNDING_BOX;
	}

	return WorldBoundingBox;
}

const ZEBoundingSphere& ZEComponent::GetWorldBoundingSphere() const
{
	if (DirtyFlags & ZE_CDF_WORLD_BOUNDING_SPHERE)
	{
		GetWorldBoundingBox().GenerateBoundingSphere(((ZEComponent*)this)->WorldBoundingSphere);
		((ZEComponent*)this)->DirtyFlags &= ~ZE_CDF_WORLD_BOUNDING_SPHERE;
	}

	return WorldBoundingSphere;
}

bool ZEComponent::GetEnabled() const
{
	return Enabled;
}

void ZEComponent::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEComponent::GetVisible() const
{
	return Visible;
}

void ZEComponent::SetVisible(bool Visible)
{
	this->Visible = Visible;
}

const ZEVector3& ZEComponent::GetLocalPosition() const
{
	return LocalPosition;
}

void ZEComponent::SetLocalPosition(const ZEVector3& NewPosition)
{
	DirtyFlags = ZE_CDF_ALL;

	LocalPosition = NewPosition;
	if (Owner != NULL)
		Owner->UpdateBoundingVolumes();
}

const ZEQuaternion& ZEComponent::GetLocalRotation() const
{
	return LocalRotation;
}

void ZEComponent::SetLocalRotation(const ZEQuaternion& NewRotation)
{
	DirtyFlags = ZE_CDF_ALL;

	LocalRotation = NewRotation;
	if (Owner != NULL)
		Owner->UpdateBoundingVolumes();
}

const ZEVector3& ZEComponent::GetLocalScale() const
{
	return LocalScale;
}

void ZEComponent::SetLocalScale(const ZEVector3& NewScale)
{
	DirtyFlags = ZE_CDF_ALL;

	LocalScale = NewScale;
	if (Owner != NULL)
		Owner->UpdateBoundingVolumes();
}


const ZEVector3 ZEComponent::GetWorldPosition() const
{
	if (Owner != NULL)
	{
		ZEVector3 Temp;
		ZEVector3::Add(Temp, Owner->GetPosition(), LocalPosition);
		return Temp;
	}
	else
		return LocalPosition;
}

void ZEComponent::SetWorldPosition(const ZEVector3& NewPosition)
{
	DirtyFlags = ZE_CDF_ALL;

	if (Owner != NULL)
		ZEVector3::Sub(LocalPosition,NewPosition,Owner->GetPosition());
	else
		LocalPosition = NewPosition;
}

const ZEQuaternion ZEComponent::GetWorldRotation() const
{
	if (Owner != NULL)
	{
		ZEQuaternion Temp, Temp1;
		ZEQuaternion::Product(Temp, Owner->GetRotation(), LocalRotation);
		ZEQuaternion::Normalize(Temp1, Temp);
		return Temp1;
	}
	else
		return LocalRotation;
}

void ZEComponent::SetWorldRotation(const ZEQuaternion& NewRotation)
{
	DirtyFlags = ZE_CDF_ALL;

	if (Owner != NULL)
	{
		ZEQuaternion Temp, InvWorldRotation;
		ZEQuaternion::Conjugate(InvWorldRotation, Owner->GetRotation());
		ZEQuaternion::Product(Temp, NewRotation, InvWorldRotation);
		ZEQuaternion::Normalize(LocalRotation, Temp);
	}
	else
		LocalRotation = NewRotation;
}

bool ZEComponent::CastRay(const ZERay& Ray, ZEVector3& Position, ZEVector3& Normal, float& TEnterance, float &TExit)
{
	return false;
}

void ZEComponent::Tick(float Time)
{
	const ZEVector3& WorldPosition = GetWorldPosition();

	ZEVector3::Sub(LocalVelocity, WorldPosition, LocalOldPosition);
	ZEVector3::Scale(LocalVelocity, LocalVelocity, Time);
}

void ZEComponent::Draw(ZERenderer* Renderer, const ZESmartArray<const ZERLLight*>& Lights)
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
	LocalPosition = ZEVector3::Zero;
	LocalOldPosition = ZEVector3::Zero;
	LocalVelocity = ZEVector3::Zero;
	LocalRotation = ZEQuaternion::Identity;
	LocalScale = ZEVector3::One;
	Visible = true;
	Enabled = true;
}

#include "Component.h.zpp"
