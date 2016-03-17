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

#include "ZERNRenderer.h"
#include "ZERNCuller.h"
#include "ZERNRenderParameters.h"

#define ZE_LDF_VIEW_TRANSFORM			1
#define ZE_LDF_PROJECTION_TRANSFORM		2
#define ZE_LDF_SHADOW_MAP				4
#define ZE_LDF_VIEW_VOLUME				8

float ZELight::AttenuationFunction(float RootToTry)
{
	float Result = 0.0f;
	Result = Intensity / (Attenuation.x + (Attenuation.y * RootToTry) + (Attenuation.z * RootToTry * RootToTry));

	return Result;
}

void ZELight::LocalTransformChanged()
{
	ZEEntity::LocalTransformChanged();
	DirtyFlags.RaiseFlags(ZE_LDF_VIEW_TRANSFORM | ZE_LDF_VIEW_VOLUME);
}

void ZELight::ParentTransformChanged()
{
	ZEEntity::ParentTransformChanged();
	DirtyFlags.RaiseFlags(ZE_LDF_VIEW_TRANSFORM | ZE_LDF_VIEW_VOLUME);
}

void ZELight::SetRange(float NewValue)
{
	if (Range == NewValue)
		return;

	Range = NewValue;

	ZEVector3 Extent(Range, Range, Range);
	SetBoundingBox(ZEAABBox(-Extent, Extent));

	DirtyFlags.RaiseFlags(ZE_LDF_PROJECTION_TRANSFORM | ZE_LDF_VIEW_VOLUME);
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
	SetAttenuation(Attenuation.x, Attenuation.y, Attenuation.z);
}

void ZELight::SetAttenuation(float Constant, float Distance, float DistanceSquare)
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
	return ZE_DF_DRAW | ZE_DF_LIGHT_SOURCE | ZE_DF_CULL;
}

void ZELight::SetCastsShadow(bool NewValue)
{
	CastsShadows = NewValue;
}

bool ZELight::GetCastsShadow() const
{
	return CastsShadows;
}

void ZELight::SetShadowResolution(ZELightShadowResolution ShadowResolution)
{
	if(this->ShadowResolution == ShadowResolution)
		return;

	this->ShadowResolution = ShadowResolution;

	DirtyFlags.RaiseFlags(ZE_LDF_SHADOW_MAP);
}

ZELightShadowResolution ZELight::GetShadowResolution() const
{
	return ShadowResolution;
}

void ZELight::SetShadowSampleCount(ZELightShadowSampleCount ShadowSampleCount)
{
	this->ShadowSampleCount = ShadowSampleCount;
}

ZELightShadowSampleCount ZELight::GetShadowSampleCount() const
{
	return ShadowSampleCount;
}

void ZELight::SetShadowSampleLength(float ShadowSampleLength)
{
	this->ShadowSampleLength = ShadowSampleLength;
}

float ZELight::GetShadowSampleLength() const
{
	return ShadowSampleLength;
}

bool ZELight::PreRender(const ZERNCullParameters* CullParameters)
{
	if(CastsShadows)
	{
		Command.StageMask |= ZERN_STAGE_SHADOWING;
	}

	Command.StageMask |= ZERN_STAGE_LIGHTING;

	CullParameters->Renderer->AddCommand(&Command);

	return true;
}

void ZELight::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	ShadowRenderer.SetContext(Parameters->Context);
	ShadowRenderer.SetScene(Parameters->Scene);
	ShadowRenderer.Render(0.0f);
}

ZELight::ZELight()
{
	DirtyFlags.RaiseAll();

	CastsShadows = false;
	ShadowResolution = ZE_LSR_MEDIUM;
	ShadowSampleCount = ZE_LSC_MEDIUM;
	ShadowSampleLength = 1.0f;

	Range = 100.0f;
	Intensity = 1.0f;
	Color = ZEVector3(1.0f, 1.0f, 1.0f);
	Attenuation = ZEVector3(1.0f, 0.0f, 0.0f);

	ViewTransform = ZEMatrix4x4::Identity;
	ProjectionTransform = ZEMatrix4x4::Identity;
}

ZELight::~ZELight()
{

}

ZEUInt ZELight::ConvertShadowResolution(ZELightShadowResolution ShadowResolution)
{
	switch (ShadowResolution)
	{
	default:
	case ZE_LSR_LOW:
		return 256;
	case ZE_LSR_MEDIUM:
		return 512;
	case ZE_LSR_HIGH:
		return 1024;
	case ZE_LSR_VERY_HIGH:
		return 2048;
	}
}

ZEUInt ZELight::ConvertShadowSampleCount(ZELightShadowSampleCount ShadowSampleCount)
{
	switch (ShadowSampleCount)
	{
	default:
	case ZE_LSC_LOW:
		return 4;
	case ZE_LSC_MEDIUM:
		return 8;
	case ZE_LSC_HIGH:
		return 16;
	case ZE_LSC_VERY_HIGH:
		return 16;
	}
}
