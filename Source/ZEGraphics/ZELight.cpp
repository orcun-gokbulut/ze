//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELight.cpp
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

#include "ZELight.h"
#include "ZERenderOrder.h"
#include "ZEGame\ZEEntity.h"

ZEDWORD ZELight::GetDrawFlags() const
{
	return ZE_DF_LIGHT_SOURCE;
}

void ZELight::SetPosition(const ZEVector3& NewPosition)
{
	UpdateViewVolume = true;
	ZEComponent::SetPosition(NewPosition);
}

void ZELight::SetRotation(const ZEQuaternion& NewRotation)
{
	UpdateViewVolume = true;
	ZEComponent::SetRotation(NewRotation);
}


void ZELight::OwnerWorldTransformChanged()
{
	UpdateViewVolume = true;
	ZEComponent::OwnerWorldTransformChanged();
}


void ZELight::SetColor(const ZEVector3& NewColor)
{
	Color = NewColor;
}

const ZEVector3& ZELight::GetColor() const
{
	return Color;
}

void ZELight::SetIntensity(float NewValue)
{
	Intensity = NewValue;
}

float ZELight::GetIntensity() const
{
	return Intensity;
}

void ZELight::SetRange(float NewValue)
{
	Range = NewValue;
}

float ZELight::GetRange() const
{
	return Range;
}

void ZELight::SetAttenuation(const ZEVector3& Attenuation)
{
	this->Attenuation = Attenuation;
}

void ZELight::SetAttenuation(float DistanceSquare, float Distance, float Constant)
{
	Attenuation.x = Constant;
	Attenuation.y = Distance;
	Attenuation.z = DistanceSquare;
}

const ZEVector3& ZELight::GetAttenuation() const
{
	return Attenuation;
}

void ZELight::SetCastsShadow(bool NewValue)
{
	CastsShadows = NewValue;
}

bool ZELight::GetCastsShadow() const
{
	return CastsShadows;
}

ZELight::ZELight()
{
	Color = ZEVector3(1.0f, 1.0f, 1.0f);
	Intensity = 1.0f;
	Range = 100.0f;
	Attenuation = ZEVector3(0.0f, 0.0f, 1.0f);
	CastsShadows = false;
}

#include "ZELight.h.zpp"
