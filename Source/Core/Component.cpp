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
#include "GameInterface/Entity.h"

void ZEComponent::UpdateBoundingVolumes()
{
	UpdateWorldBoundingBox = true;
	UpdateWorldBoundingSphere = true;
	Owner->UpdateBoundingVolumes();
}

void ZEComponent::SetLocalBoundingBox(const ZEAABoundingBox& BoundingBox)
{
	LocalBoundingBox = BoundingBox;
	UpdateWorldBoundingBox = true;
	UpdateWorldBoundingSphere = true;
	if (Owner != NULL)
		Owner->UpdateBoundingVolumes();
}

bool ZEComponent::IsEnabled()
{
	return Enabled;
}

void ZEComponent::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEComponent::IsVisible()
{
	return Visible;
}

void ZEComponent::SetVisible(bool Visible)
{
	this->Visible = Visible;
	UpdateWorldBoundingSphere = true;
	UpdateWorldBoundingBox = true;
	Owner->UpdateBoundingVolumes();
}

bool  ZEComponent::IsLight()
{
	return false;
}

bool ZEComponent::IsDrawable()
{
	return false;
}

void ZEComponent::SetOwner(ZEEntity* NewOwner)
{
	Owner = NewOwner;
	UpdateWorldBoundingBox = true;
	UpdateWorldBoundingSphere = true;
	UpdateWorldTransform = true;

	if (Visible && Owner != NULL)
		Owner->UpdateBoundingVolumes();
}

ZEEntity* ZEComponent::GetOwner()
{
	return Owner;
}

void ZEComponent::OwnerWorldTransformChanged()
{
	UpdateWorldTransform = true;
	UpdateWorldBoundingSphere = true;
	UpdateWorldBoundingBox = true;	
}


const ZEAABoundingBox& ZEComponent::GetLocalBoundingBox()
{
	return LocalBoundingBox;
}

const ZEAABoundingBox& ZEComponent::GetWorldBoundingBox()
{
	if (!UpdateWorldBoundingBox)
		return WorldBoundingBox;

	ZEAABoundingBox::Transform(WorldBoundingBox, GetLocalBoundingBox(), GetWorldTransform());
	UpdateWorldBoundingBox = false;
	return WorldBoundingBox;
}

const ZEBoundingSphere& ZEComponent::GetWorldBoundingSphere()
{
	if (!UpdateWorldBoundingSphere)
		return WorldBoundingSphere;

	GetWorldBoundingBox().GenerateBoundingSphere(WorldBoundingSphere);

	UpdateWorldBoundingSphere = false;

	return WorldBoundingSphere;
}

const ZEVector3& ZEComponent::GetLocalPosition()
{
	return Position;
}

void ZEComponent::SetLocalPosition(const ZEVector3& NewPosition)
{
	UpdateLocalTransform = true;
	UpdateWorldTransform = true;
	UpdateWorldBoundingBox = true;
	UpdateWorldBoundingSphere = true;
	Position = NewPosition;
	if (Owner != NULL)
		Owner->UpdateBoundingVolumes();
}

const ZEQuaternion& ZEComponent::GetLocalRotation()
{
	return Rotation;
}

void ZEComponent::SetLocalRotation(const ZEQuaternion& NewRotation)
{
	UpdateLocalTransform = true;
	UpdateWorldTransform = true;
	UpdateWorldBoundingBox = true;
	UpdateWorldBoundingSphere = true;

	Rotation = NewRotation;
	if (Owner != NULL)
		Owner->UpdateBoundingVolumes();
}

const ZEVector3& ZEComponent::GetLocalScale()
{
	return Scale;
}

void ZEComponent::SetLocalScale(const ZEVector3& NewScale)
{
	UpdateLocalTransform = true;
	UpdateWorldTransform = true;
	UpdateWorldBoundingBox = true;
	UpdateWorldBoundingSphere = true;
	Scale = NewScale;
	if (Owner != NULL)
		Owner->UpdateBoundingVolumes();
}

const ZEMatrix4x4& ZEComponent::GetWorldTransform()
{
	if (Owner != NULL)
	{
		if (UpdateWorldTransform)
		{
			ZEMatrix4x4::Multiply(WorldTransform, GetLocalTransform(), Owner->GetWorldTransform());
			UpdateWorldTransform = false;
		}

		return WorldTransform;
	}
	else
		return LocalTransform;
}

const ZEMatrix4x4& ZEComponent::GetLocalTransform()
{
	if (UpdateLocalTransform)
	{
		ZEMatrix4x4::CreateOrientation(LocalTransform, Position, Rotation, Scale);
		UpdateLocalTransform = false;
	}

	return LocalTransform;
}


const ZEVector3 ZEComponent::GetWorldPosition()
{
	if (Owner != NULL)
	{
		ZEVector3 Temp;
		ZEVector3::Add(Temp, Owner->GetPosition(), Position);
		return Temp;
	}
	else
		return Position;
}

void ZEComponent::SetWorldPosition(const ZEVector3& NewPosition)
{
	if (Owner != NULL)
		ZEVector3::Sub(Position,NewPosition,Owner->GetPosition());
	else
		Position = NewPosition;
}

const ZEQuaternion ZEComponent::GetWorldRotation()
{
	if (Owner != NULL)
	{
		ZEQuaternion Temp, Temp1;
		ZEQuaternion::Product(Temp, Owner->GetRotation(), Rotation);
		ZEQuaternion::Normalize(Temp1, Temp);
		return Temp1;
	}
	else
		return Rotation;
}

void ZEComponent::SetWorldRotation(const ZEQuaternion& NewRotation)
{
	if (Owner != NULL)
	{
		ZEQuaternion Temp, InvWorldRotation;
		ZEQuaternion::Conjugate(InvWorldRotation, Owner->GetRotation());
		ZEQuaternion::Product(Temp, NewRotation, InvWorldRotation);
		ZEQuaternion::Normalize(Rotation, Temp);
	}
	else
		Rotation = NewRotation;
}

bool ZEComponent::CastRay(const ZERay& Ray, ZEVector3& Position, ZEVector3& Normal, float& TEnterance, float &TExit)
{
	return true;
}

void ZEComponent::Tick(float Time)
{
}

void ZEComponent::Draw(ZERenderer* Renderer, const ZESmartArray<const ZERLLight*>& Lights)
{
}

bool ZEComponent::Initialize()
{
	return true;
}

bool ZEComponent::Deinitialize()
{
	return true;
}

ZEComponent::ZEComponent()
{
	Position = ZEVector3(0.0f, 0.0f, 0.0f);
	Rotation = ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f);
	Scale = ZEVector3(1.0f, 1.0f, 1.0f);
	Owner = NULL;
	UpdateLocalTransform = true;
	UpdateWorldTransform = true;
	UpdateWorldBoundingSphere = true;
	UpdateWorldBoundingBox = true;
	Visible = true;
}
