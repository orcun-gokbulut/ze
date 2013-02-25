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
#include "ZERenderCommand.h"
#include "ZEGame\ZEDrawParameters.h"
#include "ZERenderer.h"

ZE_OBJECT_IMPL(ZELight)

float ZELight::AttenuationFunction(float RootToTry)
{
	float Result = 0.0f;
	Result = Intensity / (((RootToTry * RootToTry) * Attenuation.z) + (RootToTry * Attenuation.y) + Attenuation.x);

	return Result;
}

void ZELight::OnTransformChanged()
{
	UpdateViewVolume = true;
	ZEEntity::OnTransformChanged();
}

void ZELight::SetRange(float NewValue)
{
	Range = NewValue;
}

float ZELight::GetRange() const
{
	return Range;
}

void ZELight::SetIntensity(float NewValue)
{
	Intensity = NewValue;
}

float ZELight::GetIntensity() const
{
	return Intensity;
}

void ZELight::SetPenumbraScale(float NewValue)
{
	PenumbraScale = NewValue;
}

float ZELight::GetPenumbraScale() const
{
	return PenumbraScale;
}

void ZELight::SetDepthScaledBias(float NewValue)
{
	DepthScaledBias = NewValue;
}

float ZELight::GetDepthScaledBias() const
{
	return DepthScaledBias;
}

void ZELight::SetSlopeScaledBias(float NewValue)
{
	SlopeScaledBias = NewValue;
}

float ZELight::GetSlopeScaledBias() const
{
	return SlopeScaledBias;
}

void ZELight::SetColor(const ZEVector3& NewColor)
{
	Color = NewColor;
}

const ZEVector3& ZELight::GetColor() const
{
	return Color;
}

void ZELight::SetAttenuation(const ZEVector3& Attenuation)
{
	SetAttenuation(Attenuation.z, Attenuation.y, Attenuation.x);
}

void ZELight::SetAttenuation(float DistanceSquare, float Distance, float Constant)
{
	Attenuation.x = Constant;
	Attenuation.y = Distance;
	Attenuation.z = DistanceSquare;

	if(GetLightType() != ZE_LT_DIRECTIONAL)
	{
		float MinRange = 0.0f;
		float MaxRange = 1000.0f;
		float ApproximateRoot = 0.0f;
		float FunctionResult = 0.0f;
		ZESize IterationCounter = 0;

		do 
		{
			IterationCounter++;

			ApproximateRoot = (MaxRange + MinRange) / 2.0f;

			FunctionResult = AttenuationFunction(ApproximateRoot);

			if (FunctionResult < 0) 
			{
				MinRange = ApproximateRoot;
			} 
			else if (FunctionResult > 0) 
			{
				MaxRange = ApproximateRoot;
			}

			if(IterationCounter >= 100)
				break;
		}
		while(FunctionResult < Intensity / 256.0f);

		SetRange(ApproximateRoot);
	}
}

const ZEVector3& ZELight::GetAttenuation() const
{
	return Attenuation;
}

ZEDrawFlags ZELight::GetDrawFlags() const
{
	return ZE_DF_DRAW | ZE_DF_LIGHT_SOURCE;
}

void ZELight::SetCastsShadow(bool NewValue)
{
	CastsShadows = NewValue;
}

bool ZELight::GetCastsShadow() const
{
	return CastsShadows;
}

void ZELight::SetPosition(const ZEVector3& NewPosition)
{
	if (GetPosition() != NewPosition)
	{
		UpdateViewVolume = true;
		ZEEntity::SetPosition(NewPosition);
	}
}

void ZELight::SetRotation(const ZEQuaternion& NewRotation)
{
	if (GetRotation() != NewRotation)
	{
		UpdateViewVolume = true;
		ZEEntity::SetRotation(NewRotation);
	}
}

void ZELight::Draw(ZEDrawParameters* DrawParameters)
{
	DrawParameters->Lights.Add(this);
}

ZELight::ZELight()
{
	Enabled = true;
	CastsShadows = false;
	UpdateViewVolume = true;

	PenumbraScale = 1.0f;

	DepthScaledBias = 0.0f;
	SlopeScaledBias = 0.0f;

	Range = 100.0f;
	Intensity = 1.0f;
	Color = ZEVector3(1.0f, 1.0f, 1.0f);
	Attenuation = ZEVector3(0.0f, 0.0f, 1.0f);
}

ZELight::~ZELight()
{
	
}
