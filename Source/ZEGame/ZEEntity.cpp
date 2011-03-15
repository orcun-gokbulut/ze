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
#include "ZEMath\ZEMatrix.h"
#include "ZEMath\ZERay.h"
#include "ZECore\ZEError.h"
#include "ZEComponent.h"
#include "ZEEntityProvider.h"
#include <string.h>

void ZEEntity::SetLocalBoundingBox(const ZEAABoundingBox& BoundingBox)
{
	LocalBoundingBox = BoundingBox;
}
		
ZEDWORD ZEEntity::GetDrawFlags() const
{
	return ZE_DF_CULL | ZE_DF_DRAW | ZE_DF_LIGHT_RECIVER;
}

ZEDWORD ZEEntity::GetRayCastFlags() const
{
	return ZE_RCF_BOUNDING_BOX;
}


ZEEntityType ZEEntity::GetEntityType()
{
	return ZE_ET_REGULAR;
}

const ZEAABoundingBox& ZEEntity::GetLocalBoundingBox() const
{
	return LocalBoundingBox;
}

const ZEAABoundingBox& ZEEntity::GetWorldBoundingBox()
{
	ZEAABoundingBox::Transform(WorldBoundingBox, GetLocalBoundingBox(), GetWorldTransform());
	return WorldBoundingBox;
}

const ZEMatrix4x4& ZEEntity::GetWorldTransform()
{
	ZEMatrix4x4::CreateOrientation(WorldTransform, Position, Rotation, Scale);
	return WorldTransform;
}

void ZEEntity::SetEntityId(int EntityId)
{
	this->EntityId = EntityId;
}

int ZEEntity::GetEntityId() const
{
	return EntityId;
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

void ZEEntity::SetPosition(const ZEVector3& NewPosition) 
{
	Position = NewPosition;
}

const ZEVector3& ZEEntity::GetPosition() const
{
	return Position;
}

void ZEEntity::SetRotation(const ZEQuaternion& NewRotation) 
{
	Rotation = NewRotation;
}

const ZEQuaternion& ZEEntity::GetRotation() const
{
	return Rotation;
}

void ZEEntity::SetScale(const ZEVector3& NewScale)
{
	Scale = NewScale;
}

const ZEVector3& ZEEntity::GetScale() const
{
	return Scale;
}

ZEVector3 ZEEntity::GetDirection()
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

void ZEEntity::SetVelocity(const ZEVector3& NewVelocity)
{
	Velocity = NewVelocity;
}

const ZEVector3& ZEEntity::GetVelocity() const
{
	return Velocity;
}

bool ZEEntity::CastRay(const ZERay& Ray, float& TRay, ZEVector3& Position, ZEVector3& Normal)
{
	return false;
}


bool ZEEntity::GetInitialized()
{
	return Initialized;
}

bool ZEEntity::Initialize()
{
	Initialized = true;
	return true;
}

void ZEEntity::Deinitialize()
{
	Initialized = false;
}

void ZEEntity::Destroy()
{
	Deinitialize();
	delete this;
}

void ZEEntity::Reset()
{
	Deinitialize();
	Initialize();
}

void ZEEntity::Tick(float Time)
{
	ZEVector3::Sub(Velocity, Position, OldPosition);
	ZEVector3::Scale(Velocity, Velocity, 1.0f / Time);
	OldPosition = Position;
}

void ZEEntity::Draw(ZEDrawParameters* DrawParameters)
{
}

ZEEntity::ZEEntity()
{
	Name[0] = '\0';

	OldPosition = Position = ZEVector3(0.0f, 0.0f, 0.0f);
	Rotation = ZEQuaternion::Identity;
	Scale = ZEVector3::One;
	Enabled = true;
	Visible = true;
	Initialized = false;
	Visible = true;
}

ZEEntity::~ZEEntity()
{
}

ZEEntity* ZEEntity::CreateInstance(const char* Name)
{
	return (ZEEntity*)ZEEntityProvider::GetInstance()->CreateInstance(Name);
}

#include "ZEEntity.h.zpp"

ZEEntityRunAt ZEEntityDescription::GetRunAt() const
{
	return ZE_ERA_NONE;
}
