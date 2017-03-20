//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEInteriorHelper.cpp
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

#include "ZEInteriorHelper.h"
#include "ZEInteriorResource.h"
#include "ZEInterior.h"
#include "ZEInteriorRoom.h"

const char* ZEInteriorHelper::GetName()
{
	return HelperResource->Name;
}

ZEInteriorHelperOwnerType ZEInteriorHelper::GetOwnerType()
{
	return OwnerType;
}

ZEInteriorRoom* ZEInteriorHelper::GetRoomOwner()
{
	return OwnerRoom;
}

void ZEInteriorHelper::SetPosition(const ZEVector3& Position)
{
	this->Position = Position;
}

const ZEVector3& ZEInteriorHelper::GetPosition()
{
	return Position;
}

ZEVector3 ZEInteriorHelper::GetInteriorPosition()
{
	ZEVector3 Temp;

	if (OwnerType == ZE_IHOT_ROOM)
	{
		ZEMatrix4x4 OwnerRoomLocalTransform;
		ZEMatrix4x4::CreateOrientation(OwnerRoomLocalTransform, OwnerRoom->GetPosition(), OwnerRoom->GetRotation(), OwnerRoom->GetScale());
		ZEMatrix4x4::Transform(Temp, OwnerRoomLocalTransform, GetPosition());
	}
		
	else
		Temp = GetPosition();

	return Temp;
}

ZEVector3 ZEInteriorHelper::GetWorldPosition()
{
	ZEVector3 Temp;

	if (OwnerType == ZE_IHOT_ROOM)
	{
		ZEMatrix4x4 OwnerRoomLocalTransform;
		ZEMatrix4x4::CreateOrientation(OwnerRoomLocalTransform, OwnerRoom->GetPosition(), OwnerRoom->GetRotation(), OwnerRoom->GetScale());
		ZEMatrix4x4 OwnerRoomWorldTransform;
		ZEMatrix4x4::Multiply(OwnerRoomWorldTransform, OwnerInterior->GetWorldTransform(), OwnerRoomLocalTransform);

		ZEMatrix4x4::Transform(Temp, OwnerRoomWorldTransform, GetPosition());
	}
	else
		ZEMatrix4x4::Transform(Temp, OwnerInterior->GetWorldTransform(), GetPosition());

	return Temp;
}

void ZEInteriorHelper::SetRotation(const ZEQuaternion& Rotation)
{
	this->Rotation = Rotation;
}

const ZEQuaternion& ZEInteriorHelper::GetRotation()
{
	return Rotation;
}

ZEQuaternion ZEInteriorHelper::GetInteriorRotation()
{
	ZEQuaternion Temp;

	if (OwnerType == ZE_IHOT_ROOM)
	{
		ZEQuaternion::Product(Temp, OwnerRoom->GetRotation(), GetRotation());
	}
	else
		Temp = GetRotation();

	ZEQuaternion::Normalize(Temp, Temp);
	return Temp;
}

ZEQuaternion ZEInteriorHelper::GetWorldRotation()
{
	ZEQuaternion Temp;

	if (OwnerType == ZE_IHOT_ROOM)
	{
		ZEQuaternion OwnerRotation;
		ZEQuaternion::Product(OwnerRotation, OwnerInterior->GetWorldRotation(), OwnerRoom->GetRotation());
		ZEQuaternion::Product(Temp, OwnerRotation, GetRotation());
	}
	else
	{
		ZEQuaternion::Product(Temp, OwnerInterior->GetWorldRotation(), GetRotation());
	}

	ZEQuaternion::Normalize(Temp, Temp);
	return Temp;
}

void ZEInteriorHelper::SetScale(const ZEVector3& Scale)
{
	this->Scale = Scale;
}

const ZEVector3& ZEInteriorHelper::GetScale()
{
	return Scale;
}

ZEVector3 ZEInteriorHelper::GetInteriorScale()
{
	ZEVector3 Temp;

	if (OwnerType == ZE_IHOT_ROOM)
		ZEVector3::Multiply(Temp, OwnerRoom->GetScale(), GetScale());
	else
		Temp = GetScale();

	return Temp;
}

ZEVector3 ZEInteriorHelper::GetWorldScale()
{
	ZEVector3 Temp;

	if (OwnerType == ZE_IHOT_ROOM)
	{
		ZEVector3 OwnerScale;
		ZEVector3::Multiply(OwnerScale, OwnerInterior->GetWorldScale(), OwnerRoom->GetScale());
		ZEVector3::Multiply(Temp, OwnerScale, GetScale());
	}
	else
	{
		ZEVector3::Multiply(Temp, OwnerInterior->GetWorldScale(), GetScale());
	}

	return Temp;
}

ZEVector3 ZEInteriorHelper::GetFront()
{
	return Rotation * ZEVector3::UnitZ;
}

ZEVector3 ZEInteriorHelper::GetRight()
{
	return Rotation * ZEVector3::UnitX;
}

ZEVector3 ZEInteriorHelper::GetUp()
{
	return Rotation * ZEVector3::UnitY;
}

ZEVector3 ZEInteriorHelper::GetInteriorFront()
{
	return GetInteriorRotation() * ZEVector3::UnitZ;
}

ZEVector3 ZEInteriorHelper::GetInteriorRight()
{
	return GetInteriorRotation() * ZEVector3::UnitX;
}

ZEVector3 ZEInteriorHelper::GetInteriorUp()
{
	return GetInteriorRotation() * ZEVector3::UnitY;
}

ZEVector3 ZEInteriorHelper::GetWorldFront()
{
	return GetWorldRotation() * ZEVector3::UnitZ;
}

ZEVector3 ZEInteriorHelper::GetWorldRight()
{
	return GetWorldRotation() * ZEVector3::UnitX;
}

ZEVector3 ZEInteriorHelper::GetWorldUp()
{
	return GetWorldRotation() * ZEVector3::UnitY;
}

void ZEInteriorHelper::Initialize(ZEInterior* Interior, const ZEInteriorResourceHelper* HelperResource)
{
	OwnerInterior = Interior;
	this->HelperResource = HelperResource;
	OwnerType = (ZEInteriorHelperOwnerType)HelperResource->OwnerType;
	Position = HelperResource->Position;
	Rotation = HelperResource->Rotation;
	Scale = HelperResource->Scale;

	if (OwnerType == ZE_IHOT_ROOM)
		OwnerRoom = Interior->Rooms[HelperResource->OwnerIndex];
}

void ZEInteriorHelper::Deinitialize()
{
	OwnerInterior = NULL;
	OwnerRoom = NULL;
}

ZEInteriorHelper::ZEInteriorHelper()
{
	OwnerInterior = NULL;

	OwnerType = ZE_IHOT_INTERIOR;
	OwnerRoom = NULL;

	HelperResource = NULL;

	Position = ZEVector3::Zero;
	Rotation = ZEQuaternion::Identity;
	Scale = ZEVector3::One;
}

ZEInteriorHelper* ZEInteriorHelper::CreateInstance()
{
	return new ZEInteriorHelper();
}




