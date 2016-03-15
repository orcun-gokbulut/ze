//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEATAtmosphere.cpp
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

#include "ZEATAtmosphere.h"

#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEViewVolume.h"
#include "ZEGame/ZEScene.h"
#include "ZERenderer/ZECamera.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNCuller.h"
#include "ZERenderer/ZERNStageGBuffer.h"
#include "ZERenderer/ZELightDirectional.h"
#include "ZEGraphics/ZEGRTexture2D.h"

#include "ZEATSun.h"
#include "ZEATMoon.h"
#include "ZEATAstronomy.h"
#include "ZEMath/ZEAngle.h"

#define EARTH_RADIUS	6371000.0f	//In meters

bool ZEATAtmosphere::InitializeSelf()
{
	SunLight = ZELightDirectional::CreateInstance();
	SunLight->SetName("SunLight");
	SunLight->SetCastsShadow(true);
	SunLight->SetCascadeCount(4);
	SunLight->SetShadowResolution(ZE_LSR_VERY_HIGH);
	SunLight->SetShadowSampleCount(ZE_LSC_VERY_HIGH);
	//zeScene->AddEntity(SunLight);

	MoonLight = ZELightDirectional::CreateInstance();
	MoonLight->SetName("MoonLight");
	MoonLight->SetCastsShadow(true);
	MoonLight->SetCascadeCount(4);
	MoonLight->SetShadowResolution(ZE_LSR_VERY_HIGH);
	MoonLight->SetShadowSampleCount(ZE_LSC_VERY_HIGH);
	//zeScene->AddEntity(MoonLight);

	Sun = new ZEATSun();
	zeScene->AddEntity(Sun);

	Moon = new ZEATMoon();
	zeScene->AddEntity(Moon);
	Moon->SetTextureFile("#R:/ZEEngine/ZEAtmosphere/Textures/MoonFrame.png", 53, 1);

	AtmosphericScattering.SetMultipleScattering(true);
	AtmosphericScattering.SetOrderCount(5);
	AtmosphericScattering.Initialize();

	return true;
}

bool ZEATAtmosphere::DeinitializeSelf()
{
	AtmosphericScattering.Deinitialize();

	return true;
}

void ZEATAtmosphere::SetObserver(const ZEATObserver& Observer)
{
	this->Observer = Observer;
}

const ZEATObserver& ZEATAtmosphere::GetObserver() const
{
	return Observer;
}

void ZEATAtmosphere::SetMultipleScattering(bool MultipleScattering)
{
	AtmosphericScattering.SetMultipleScattering(MultipleScattering);
}

bool ZEATAtmosphere::GetMultipleScattering()
{
	return AtmosphericScattering.GetMultipleScattering();
}

ZEATAtmosphere::ZEATAtmosphere()
{
	Command.Entity = this;
	Command.StageMask = ZERN_STAGE_PRE_EFFECT | ZERN_STAGE_POST_EFFECT;
	Command.Priority = 2;
}

ZEATAtmosphere::~ZEATAtmosphere()
{
}

ZEDrawFlags ZEATAtmosphere::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

void ZEATAtmosphere::Tick(float Time)
{
	ZEVector3 SunDirection = ZEATAstronomy::GetSunDirection(Observer);
	float SunAngularAttenuation = ZEVector3::DotProduct(-SunDirection, ZEVector3::UnitY);
	bool Day = SunAngularAttenuation > 0.0f;

	ZEVector3 MoonDirection = ZEATAstronomy::GetMoonDirection(Observer);
	float MoonAngularAttenuation = ZEVector3::DotProduct(-MoonDirection, ZEVector3::UnitY);
	bool MoonVisible = MoonAngularAttenuation > 0.0f;

	ZEQuaternion SunRotation;
	ZEQuaternion::CreateFromDirection(SunRotation, SunDirection);

	ZEQuaternion MoonRotation;
	ZEQuaternion::CreateFromDirection(MoonRotation, MoonDirection);

	float HeightFromEarthCenter = (Observer.Space.Elevation + EARTH_RADIUS) * 1e-6f;

	float SunDiskRadiusDegree = ZEATAstronomy::GetSunDiskRadius(Observer);
	float SunDiskRadiusFromObserver = ZEAngle::Tan(ZEAngle::ToRadian(SunDiskRadiusDegree)) * HeightFromEarthCenter;

	Sun->SetDirection(SunDirection);
	Sun->SetDiskRadius(SunDiskRadiusFromObserver);
	SunLight->SetWorldRotation(SunRotation);
	SunLight->SetIntensity(2.0f);
	SunLight->SetColor(ZEVector3::One);
	SunLight->SetVisible(Day);

	float MoonDiskRadiusDegree = ZEATAstronomy::GetMoonDiskRadius(Observer);
	float MoonDiskRadiusFromObserver = ZEAngle::Tan(ZEAngle::ToRadian(MoonDiskRadiusDegree)) * HeightFromEarthCenter;

	Moon->SetDirection(MoonDirection);
	Moon->SetDiskRadius(MoonDiskRadiusFromObserver);
	MoonLight->SetWorldRotation(MoonRotation);
	MoonLight->SetColor(ZEVector3::One);
	MoonLight->SetIntensity(0.3f);
	MoonLight->SetVisible(!Day && MoonVisible);

	double SunTransit, Sunrise, Sunset;
	ZEATAstronomy::GetSunTransitSunriseSunsetTime(SunTransit, Sunrise, Sunset, Observer);

	ZEUInt SunTransitHour = (ZEUInt)SunTransit % 24;
	ZEUInt SunTransitMinute = (ZEUInt)(SunTransit * 60.0) % 60;

	ZEUInt SunriseHour = (ZEUInt)Sunrise % 24;
	ZEUInt SunriseMinute = (ZEUInt)(Sunrise * 60.0) % 60;

	ZEUInt SunsetHour = (ZEUInt)Sunset % 24;
	ZEUInt SunsetMinute = (ZEUInt)(Sunset * 60.0) % 60;

	if (Day)
	{
		AtmosphericScattering.SetLightDirection(SunDirection);
		AtmosphericScattering.SetLightIntensity(10.0f);
		AtmosphericScattering.SetLightColor(ZEVector3::One);
	}
	else if (MoonVisible)
	{
		AtmosphericScattering.SetLightDirection(MoonDirection);
		AtmosphericScattering.SetLightIntensity(0.1f);
		AtmosphericScattering.SetLightColor(ZEVector3::One);
	}
	else
	{
		AtmosphericScattering.SetLightIntensity(0.0f);
		AtmosphericScattering.SetLightColor(ZEVector3::One);
	}
}

bool ZEATAtmosphere::PreRender(const ZERNCullParameters* CullParameters)
{
	CullParameters->Renderer->AddCommand(&Command);

	return true;
}

void ZEATAtmosphere::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	ZEUInt StageID = (Parameters->Stage->GetId() & Command->StageMask);

	if(StageID == ZERN_STAGE_PRE_EFFECT)
	{
		AtmosphericScattering.PreProcess(Parameters->Renderer, Parameters->Context);
	}
	else if(StageID == ZERN_STAGE_POST_EFFECT)
	{
		AtmosphericScattering.PostProcess(Parameters->Renderer, Parameters->Context);
	}
}

ZEATAtmosphere* ZEATAtmosphere::CreateInstance()
{
	return new ZEATAtmosphere();
}
