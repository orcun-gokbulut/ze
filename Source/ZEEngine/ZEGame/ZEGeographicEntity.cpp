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
#include "ZESectorManager.h"

#define ZE_GEDF_GEOGRAPHIC_TRANSFORM			0x0001
#define ZE_GEDF_INV_GEOGRAPHIC_TRANSFORM		0x0002
#define ZE_GEDF_LOCAL_TRANSFORM					0x0004 

void ZEGeographicEntity::GeographicTransformChanged()
{
	GeographicEntityDirtyFlags.RaiseFlags(ZE_GEDF_GEOGRAPHIC_TRANSFORM | ZE_GEDF_INV_GEOGRAPHIC_TRANSFORM | ZE_GEDF_LOCAL_TRANSFORM);
}

ZEGeographicEntity::ZEGeographicEntity() : GeoLink(this)
{
	GeographicPosition = ZEVector3d::Zero;
	GeographicRotation = ZEQuaternion::Identity;
	GeographicScale = ZEVector3d::One;
	GeographicTransform = ZEMatrix4x4d::Identity;
	InvGeographicTransform = ZEMatrix4x4d::Identity;
}

const ZEMatrix4x4d& ZEGeographicEntity::GetGeographicTransform() const
{
	if (GeographicEntityDirtyFlags.GetFlags(ZE_GEDF_GEOGRAPHIC_TRANSFORM))
	{
		ZEMatrix4x4d::CreateOrientation(GeographicTransform, GeographicPosition, GeographicRotation, GeographicScale);
		GeographicEntityDirtyFlags.UnraiseFlags(ZE_GEDF_GEOGRAPHIC_TRANSFORM);
	}

	return GeographicTransform;
}

const ZEMatrix4x4d& ZEGeographicEntity::GetInvGeographicTransform() const
{
	if (GeographicEntityDirtyFlags.GetFlags(ZE_GEDF_INV_GEOGRAPHIC_TRANSFORM))
	{
		ZEMatrix4x4d::CreateInvOrientation(InvGeographicTransform, GeographicPosition, GeographicRotation, GeographicScale);
		GeographicEntityDirtyFlags.UnraiseFlags(ZE_GEDF_INV_GEOGRAPHIC_TRANSFORM);
	}

	return InvGeographicTransform;
}

void ZEGeographicEntity::SetGeographicPosition(const ZEVector3d& Position)
{
	GeographicPosition = Position;
	GeographicTransformChanged();
}

ZEVector3d ZEGeographicEntity::GetGeographicPosition() const
{
	return GeographicPosition;
}

void ZEGeographicEntity::SetGeographicRotation(const ZEQuaternion& Rotation)
{
	GeographicRotation = Rotation;
	GeographicTransformChanged();
}

ZEQuaternion ZEGeographicEntity::GetGeographicRotation() const
{
	return GeographicRotation;
}

void ZEGeographicEntity::SetGeographicScale(const ZEVector3d& Scale)
{
	GeographicScale = Scale;
	GeographicTransformChanged();
}

ZEVector3d ZEGeographicEntity::GetGeographicScale() const
{
	return GeographicScale;
}

ZEGeographicEntity* ZEGeographicEntity::CreateInstance()
{
	return new ZEGeographicEntity();
}
