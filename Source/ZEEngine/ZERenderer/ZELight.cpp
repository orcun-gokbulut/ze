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

#include "ZEMath\ZEViewFrustum.h"
#include "ZERNRenderer.h"
#include "ZERNCuller.h"
#include "ZERNRenderParameters.h"
#include "ZERNStageShadowmapGeneration.h"
#include "ZEGraphics\ZEGRGraphicsModule.h"

#define ZE_LDF_VIEW_TRANSFORM			1
#define ZE_LDF_PROJECTION_TRANSFORM		2
#define ZE_LDF_SHADOW_MAP				4
#define ZE_LDF_VIEW_VOLUME				8
#define ZE_LDF_BOUNDING_BOX				16

float ZELight::AttenuationFunction(float RootToTry)
{
	float Result = 0.0f;
	Result = Intensity / (Attenuation.z + (Attenuation.y * RootToTry) + (Attenuation.x * RootToTry * RootToTry));

	return Result;
}

bool ZELight::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	ShadowRenderer.AddStage(new ZERNStageShadowmapGeneration());

	ShadowRenderer.SetContext(ZEGRGraphicsModule::GetInstance()->GetMainContext());

	return ShadowRenderer.Initialize();
}

bool ZELight::DeinitializeSelf()
{
	ShadowRenderer.Deinitialize();

	return ZEEntity::DeinitializeSelf();
}

void ZELight::CalculateBoundingBox() const
{
	ZEAABBox AABB;

	const ZEViewVolume& ViewVolume = GetViewVolume();
	if (ViewVolume.GetViewVolumeType() == ZE_VVT_FRUSTUM)
	{
		const ZEViewFrustum& ViewFrustum = static_cast<const ZEViewFrustum&>(ViewVolume);
		for (ZEUInt I = 0; I < 8; I++)
		{
			ZEVector3 VertexLocal = GetInvWorldTransform() * ViewFrustum.GetVertex((ZEViewFrustumVertex)I);

			ZEVector3::Min(AABB.Min, AABB.Min, VertexLocal);
			ZEVector3::Max(AABB.Max, AABB.Max, VertexLocal);
		}
	}
	else if (ViewVolume.GetViewVolumeType() == ZE_VVT_SPHERE)
	{
		ZEVector3 Extent(Range, Range, Range);
		AABB.Min = -Extent;
		AABB.Max = Extent;
	}
	
	const_cast<ZELight*>(this)->SetBoundingBox(AABB);
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

void ZELight::BoundingBoxChanged()
{
	ZEEntity::BoundingBoxChanged();
	DirtyFlags.RaiseFlags(ZE_LDF_BOUNDING_BOX);
}

ZELight::ZELight()
{
	DirtyFlags.RaiseAll();

	CastsShadows = false;
	ShadowResolution = ZE_LSR_MEDIUM;
	ShadowSampleCount = ZE_LSC_MEDIUM;
	ShadowSampleLength = 1.0f;
	ShadowDepthBias = 0.005f;
	ShadowNormalBias = 0.1f;

	Range = 100.0f;
	Intensity = 1.0f;
	Color = ZEVector3::One;
	Attenuation = ZEVector3(0.0f, 0.0f, 1.0f);

	ViewTransform = ZEMatrix4x4::Identity;
	ProjectionTransform = ZEMatrix4x4::Identity;
}

ZELight::~ZELight()
{

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

void ZELight::SetRange(float NewValue)
{
	if (Range == NewValue)
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

void ZELight::SetShadowDepthBias(float ShadowDepthBias)
{
	this->ShadowDepthBias = ShadowDepthBias;
}

float ZELight::GetShadowDepthBias() const
{
	return ShadowDepthBias;
}

void ZELight::SetShadowNormalBias(float ShadowNormalBias)
{
	this->ShadowNormalBias = ShadowNormalBias;
}

float ZELight::GetShadowNormalBias() const
{
	return ShadowNormalBias;
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

void ZELight::SetAttenuation(float DistanceSquare, float Distance, float Constant)
{
	Attenuation.x = DistanceSquare;
	Attenuation.y = Distance;
	Attenuation.z = Constant;

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

const ZEAABBox& ZELight::GetBoundingBox() const
{
	if (DirtyFlags.GetFlags(ZE_LDF_BOUNDING_BOX))
	{
		CalculateBoundingBox();
		DirtyFlags.UnraiseFlags(ZE_LDF_BOUNDING_BOX);
	}

	return ZEEntity::GetBoundingBox();
}

bool ZELight::PreRender(const ZERNCullParameters* CullParameters)
{
	if (!ZEEntity::PreRender(CullParameters))
		return false;

	Command.StageMask = ZERN_STAGE_LIGHTING | ZERN_STAGE_DEBUG;
	if (CastsShadows)
		Command.StageMask |= ZERN_STAGE_SHADOWING;

	CullParameters->Renderer->AddCommand(&Command);

	return true;
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
