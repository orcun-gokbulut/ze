//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGeographicEntity.cpp
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

#include "ZEGeographicEntity.h"

bool ZEGeographicEntity::SetOwner(ZEEntity* Owner)
{
	if (!ZEClass::IsDerivedFrom(ZEGeographicEntity::Class(), Owner->GetClass()))
		return false;

	return ZEEntity::SetOwner(Owner);
}

ZEGeographicEntity::ZEGeographicEntity()
{
	GeographicPosition = ZEVector3d::Zero;
	GeographicRotation = ZEQuaternion::Identity;
	GeographicScale = ZEVector3d::One;
	GeographicTransform = ZEMatrix4x4d::Identity;
}

const ZEMatrix4x4d& ZEGeographicEntity::GetGeographicTransform() const
{
	ZEMatrix4x4d TempGeographicTransform;
	ZEMatrix4x4d::CreateOrientation(TempGeographicTransform, GeographicPosition, GeographicRotation, GeographicScale);

	if (GetOwner() != NULL)
	{
		ZEMatrix4x4d::Multiply(GeographicTransform, ((ZEGeographicEntity*)GetOwner())->GetGeographicTransform(), TempGeographicTransform);
	}
	else
	{
		GeographicTransform = TempGeographicTransform;
	}

	return GeographicTransform;
}

const ZEMatrix4x4d& ZEGeographicEntity::GetInvGeographicTransform() const
{
	ZEMatrix4x4d::Inverse(InvGeographicTransform, GetGeographicTransform());
	return InvGeographicTransform;
}

void ZEGeographicEntity::SetGeographicPosition(const ZEVector3d& Position)
{
	ZEGeographicEntity* Owner = (ZEGeographicEntity*)GetOwner();

	if (Owner != NULL)
	{
		ZEVector3d Result = Owner->GetInvGeographicTransform() * Position;
		SetPosition(Result.ToVector3());
		GeographicPosition = Result;
	}
	else
	{
		SetPosition(Position.ToVector3());
		GeographicPosition = Position;
	}
}

ZEVector3d ZEGeographicEntity::GetGeographicPosition() const
{
	ZEGeographicEntity* Owner = (ZEGeographicEntity*)GetOwner();

	if (Owner != NULL)
	{
		ZEVector3d Temp;
		ZEMatrix4x4d::Transform(Temp, Owner->GetGeographicTransform(), GeographicPosition);
		return Temp;
	}

	return GeographicPosition;
}

void ZEGeographicEntity::SetGeographicRotation(const ZEQuaternion& Rotation)
{
	ZEGeographicEntity* Owner = (ZEGeographicEntity*)GetOwner();

	if (Owner != NULL)
	{
		ZEQuaternion Temp, Result;
		ZEQuaternion::Conjugate(Temp, Owner->GetGeographicRotation());
		ZEQuaternion::Product(Result, Temp, Rotation);
		SetRotation(Result);
		GeographicRotation = Result;
	}
	else
	{
		SetRotation(Rotation);
		GeographicRotation = Rotation;
	}
}

ZEQuaternion ZEGeographicEntity::GetGeographicRotation() const
{
	ZEGeographicEntity* Owner = (ZEGeographicEntity*)GetOwner();

	if (Owner != NULL)
	{
		ZEQuaternion Temp;
		ZEQuaternion::Product(Temp, Owner->GetGeographicRotation(), GeographicRotation);
		ZEQuaternion::Normalize(Temp, Temp);
		return Temp;
	}

	return GeographicRotation;
}

void ZEGeographicEntity::SetGeographicScale(const ZEVector3d& Scale)
{
	ZEGeographicEntity* Owner = (ZEGeographicEntity*)GetOwner();

	if (Owner != NULL)
	{
		ZEVector3d Temp;
		ZEVector3d::Divide(Temp, Scale, Owner->GetGeographicScale());
		SetScale(Temp.ToVector3());
		GeographicScale = Temp;
	}
	else
	{
		SetScale(Scale.ToVector3());
		GeographicScale = Scale;
	}

}

ZEVector3d ZEGeographicEntity::GetGeographicScale() const
{
	ZEGeographicEntity* Owner = (ZEGeographicEntity*)GetOwner();

	if (Owner != NULL)
	{
		ZEVector3d Temp;
		ZEVector3d::Multiply(Temp, Owner->GetGeographicScale(), GeographicScale);
		return Temp;
	}

	return GeographicScale;
}

ZEGeographicEntity* ZEGeographicEntity::CreateInstance()
{
	return new ZEGeographicEntity();
}
