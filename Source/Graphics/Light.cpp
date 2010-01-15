//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Light.cpp
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

#include "Light.h"
#include "RenderOrder.h"
#include "GameInterface/Entity.h"

ZEDWORD ZELight::GetDrawFlags() const
{
	return ZE_DF_LIGHT_SOURCE;
}

void ZELight::SetLocalPosition(const ZEVector3& NewPosition)
{
	UpdateViewVolume = true;
	ZEComponent::SetLocalPosition(NewPosition);
}

void ZELight::SetLocalRotation(const ZEQuaternion& NewRotation)
{
	UpdateViewVolume = true;
	ZEComponent::SetLocalRotation(NewRotation);
}


void ZELight::OwnerWorldTransformChanged()
{
	UpdateViewVolume = true;
	ZEComponent::OwnerWorldTransformChanged();
}


void ZELight::SetColor(const ZEVector3& NewColor)
{
	RenderOrderLight.Color = NewColor;
}

const ZEVector3& ZELight::GetColor() const
{
	return RenderOrderLight.Color;
}

void ZELight::SetIntensity(float NewValue)
{
	RenderOrderLight.Intensity = NewValue;
}

float ZELight::GetIntensity() const
{
	return RenderOrderLight.Intensity;
}

void ZELight::SetRange(float NewValue)
{
	RenderOrderLight.Range = NewValue;
}

float ZELight::GetRange() const
{
	return RenderOrderLight.Range;
}

void ZELight::SetAttenuation(float DistanceSquare, float Distance, float Constant)
{
	RenderOrderLight.Attenuation.x = Constant;
	RenderOrderLight.Attenuation.y = Distance;
	RenderOrderLight.Attenuation.z = DistanceSquare;
}

const ZEVector3& ZELight::GetAttenuation() const
{
	return RenderOrderLight.Attenuation;
}

void ZELight::SetEnabled(bool NewValue)
{
	Enabled = NewValue;
}

bool ZELight::GetEnabled() const
{
	return Enabled;
}

void ZELight::SetCastsShadows(bool NewValue)
{
	CastsShadows = NewValue;
}

bool ZELight::GetCastsShadows() const
{
	return CastsShadows;
}

const ZERLLight* ZELight::GetRenderOrderLight()
{
	RenderOrderLight.Position = ZEVector3(GetWorldPosition());
	ZEQuaternion::VectorProduct(RenderOrderLight.Direction, GetWorldRotation(), ZEVector3(0.0f, 0.0f, 1.0f));
	ZEQuaternion InvRotation;
	ZEQuaternion::Conjugate(InvRotation, GetWorldRotation());
	ZEMatrix4x4::CreateRotation(RenderOrderLight.LightRotationMatrix, InvRotation);
	return &RenderOrderLight;
}

ZELight::ZELight()
{
	RenderOrderLight.SetZero();
}
