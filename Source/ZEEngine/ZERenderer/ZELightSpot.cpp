//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELightSpot.cpp
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

#include "ZELightSpot.h"

#include "ZEMath\ZEAngle.h"
#include "ZERNRenderParameters.h"

#define ZERN_LDF_VIEW_TRANSFORM			1
#define ZERN_LDF_PROJECTION_TRANSFORM	2
#define ZERN_LDF_SHADOW_MAP				4
#define ZERN_LDF_VIEW_VOLUME			8

ZELightSpot::ZELightSpot()
{
	Command.Entity = this;
	Command.Priority = 1;

	CommandShadow.Entity = this;
	CommandShadow.Priority = 1;

	InnerConeAngle = ZE_PI_8;
	OuterConeAngle = ZE_PI_4;
	FalloffExponent = 4.0f;
}

ZELightSpot::~ZELightSpot()
{

}

ZELightType ZELightSpot::GetLightType() const
{
	return ZE_LT_SPOT;
}

ZESize ZELightSpot::GetViewCount() const
{
	return 1;
}

void ZELightSpot::SetInnerConeAngle(float Angle)
{
	InnerConeAngle = Angle;
}

float ZELightSpot::GetInnerConeAngle() const
{
	return InnerConeAngle;
}

void ZELightSpot::SetOuterConeAngle(float Angle)
{
	if (OuterConeAngle == Angle)
		return;

	OuterConeAngle = Angle;

	DirtyFlags.RaiseFlags(ZERN_LDF_PROJECTION_TRANSFORM | ZERN_LDF_VIEW_VOLUME);
}

float ZELightSpot::GetOuterConeAngle() const
{
	return OuterConeAngle;
}

void ZELightSpot::SetFalloffExponent(float Exponent)
{
	FalloffExponent = Exponent;
}

float ZELightSpot::GetFalloffExponent() const
{
	return FalloffExponent;
}

const ZEViewVolume& ZELightSpot::GetViewVolume(ZESize Index) const
{
	if (DirtyFlags.GetFlags(ZERN_LDF_VIEW_VOLUME))
	{
		ZEViewFrustum::Create(ViewVolume, GetWorldPosition(), GetWorldRotation(), GetOuterConeAngle(), 1.0f, 0.1f, GetRange());
		DirtyFlags.UnraiseFlags(ZERN_LDF_VIEW_VOLUME);
	}
	return ViewVolume;
}

const ZEMatrix4x4& ZELightSpot::GetViewTransform(ZESize Index) const
{
	if (DirtyFlags.GetFlags(ZERN_LDF_VIEW_TRANSFORM))
	{
		ZEMatrix4x4::CreateViewTransform(ViewTransform, GetWorldPosition(), GetWorldRotation());
		DirtyFlags.UnraiseFlags(ZERN_LDF_VIEW_TRANSFORM);
	}

	return ViewTransform;
}

const ZEMatrix4x4& ZELightSpot::GetProjectionTransform(ZESize Index) const
{
	if (DirtyFlags.GetFlags(ZERN_LDF_PROJECTION_TRANSFORM))
	{
		ZEMatrix4x4::CreatePerspectiveProjection(ProjectionTransform, GetOuterConeAngle(), 1.0f, 0.1f, GetRange());
		DirtyFlags.UnraiseFlags(ZERN_LDF_PROJECTION_TRANSFORM);
	}

	return ProjectionTransform;
}

bool ZELightSpot::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (!ZELight::PreRender(Parameters))
		return false;

	if (!GetCastsShadow())
	{
		Command.Scene = GetScene();
		Command.PositionWorld = GetWorldPosition();
		Command.RotationWorld = GetWorldRotation();
		Command.Range = GetRange();
		Command.Color = GetColor() * GetIntensity();
		Command.FalloffExponent = GetFalloffExponent();
		Command.InnerConeAngle = GetInnerConeAngle();
		Command.OuterConeAngle = GetOuterConeAngle();
		Command.StageMask = ZERN_STAGE_LIGHTING;

		Parameters->Renderer->AddCommand(&Command);
	}
	else
	{
		CommandShadow.Scene = GetScene();
		CommandShadow.PositionWorld = GetWorldPosition();
		CommandShadow.RotationWorld = GetWorldRotation();
		CommandShadow.Range = GetRange();
		CommandShadow.Color = GetColor() * GetIntensity();
		CommandShadow.FalloffExponent = GetFalloffExponent();
		CommandShadow.InnerConeAngle = GetInnerConeAngle();
		CommandShadow.OuterConeAngle = GetOuterConeAngle();
		CommandShadow.ViewFrustum = static_cast<const ZEViewFrustum&>(GetViewVolume());
		CommandShadow.ViewTransform = GetViewTransform();
		CommandShadow.ProjectionTransform = GetProjectionTransform();
		CommandShadow.ShadowSampleCount = GetShadowSampleCount();
		CommandShadow.ShadowSampleLength = GetShadowSampleLength();
		CommandShadow.ShadowDepthBias = GetShadowDepthBias();
		CommandShadow.ShadowNormalBias = GetShadowNormalBias();
		CommandShadow.StageMask = ZERN_STAGE_LIGHTING | ZERN_STAGE_SHADOWING;

		Parameters->Renderer->AddCommand(&CommandShadow);
	}

	return true;
}

ZELightSpot* ZELightSpot::CreateInstance()
{
	return new ZELightSpot();
}
