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

void ZELight::OnTransformChanged()
{
	ZEEntity::OnTransformChanged();

	DirtyFlags.RaiseFlags(ZE_LDF_VIEW_TRANSFORM | ZE_LDF_VIEW_VOLUME);
}

void ZELight::SetRange(float NewValue)
{
	if(Range == NewValue)
		return;

	Range = NewValue;
	
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
		ZEEntity::SetPosition(NewPosition);

		DirtyFlags.RaiseFlags(ZE_LDF_VIEW_TRANSFORM | ZE_LDF_VIEW_VOLUME);
	}
}

void ZELight::SetRotation(const ZEQuaternion& NewRotation)
{
	if (GetRotation() != NewRotation)
	{
		ZEEntity::SetRotation(NewRotation);

		DirtyFlags.RaiseFlags(ZE_LDF_VIEW_TRANSFORM | ZE_LDF_VIEW_VOLUME);
	}
}

bool ZELight::PreRender(const ZERNCullParameters* CullParameters)
{
	if(CastsShadows)
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
	CastsShadows = false;

	Range = 100.0f;
	Intensity = 1.0f;
	Color = ZEVector3(1.0f, 1.0f, 1.0f);
	Attenuation = ZEVector3(1.0f, 0.0f, 0.0f);

	ViewTransform = ZEMatrix4x4::Identity;
	ProjectionTransform = ZEMatrix4x4::Identity;

	DirtyFlags.RaiseAll();
}

ZELight::~ZELight()
{

}
