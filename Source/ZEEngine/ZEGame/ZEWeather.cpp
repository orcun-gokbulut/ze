//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEWeather.cpp
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

#include "ZEWeather.h"
#include "ZEGraphics\ZEDirectionalLight.h"
#include "ZEMath\ZEAngle.h"
#include "ZEScene.h"
#include "ZEGame.h"
#include "ZESkyBrush.h"
#include "ZEMoon.h"
#include "ZESkyDome.h"
#include "ZECloud.h"
#include "ZEModules\ZEDirect3D9\ZED3D9FrameRenderer.h"


void ZEWeather::AdjustHDR(float DayTime)
{

}

ZEMoon* ZEWeather::GetMoon() const
{
	return Moon;
}

ZECloud* ZEWeather::GetCloud() const
{
	return Cloud;
}

ZESkyDome* ZEWeather::GetSkyDome() const
{
	return SkyDome;
}

ZESkyBrush*	ZEWeather::GetStarMap() const
{
	return StarMap;
}

ZEDirectionalLight* ZEWeather::GetSunLight() const
{
	return SunLight;
}

ZEDirectionalLight* ZEWeather::GetMoonLight() const
{
	return MoonLight;
}

float ZEWeather::GetSunHeight() const
{
	return SunHeight;
}

float ZEWeather::GetMoonHeight() const
{
	return MoonHeight;
}

void ZEWeather::SetFogFactor(float Value)
{
	FogFactor = Value;

	((ZED3D9FrameRenderer*)zeScene->GetRenderer())->FogProcessor.SetFogVisibility(Value);
}

float ZEWeather::GetFogFactor() const
{
	return FogFactor;
}

void ZEWeather::SetMoonPhase(float Value)
{
	MoonPhase = Value;

	Moon->SetMoonPhase(Value);
}

float ZEWeather::GetMoonPhase() const
{
	return MoonPhase;
}

void ZEWeather::SetCloudCover(float Value)
{
	CloudCover = Value;

	Cloud->SetCloudCover(Value);
}

float ZEWeather::GetCloudCover() const
{
	return CloudCover;
}

void ZEWeather::SetMoonLightIntensity(float Value)
{
	MoonLightIntensity = Value;

	MoonLight->SetIntensity(Value);
}

float ZEWeather::GetMoonLightIntensity() const
{
	return MoonLightIntensity;
}

void ZEWeather::SetSunLightIntensity(float Value)
{
	SunLightIntensity = Value;

	SunLight->SetIntensity(Value);
}

float ZEWeather::GetSunLightIntensity() const
{
	return SunLightIntensity;
}

void ZEWeather::SetMoonDirection(const ZEVector3& Value)
{
	MoonHeight = Value.y;
	MoonDirection = Value;

	Moon->SetMoonDirection(Value);
}

const ZEVector3& ZEWeather::GetMoonDirection() const
{
	return MoonDirection;
}

void ZEWeather::SetSunDirection(const ZEVector3& Value)
{
	SunHeight = Value.y;
	SunDirection = Value;

	Cloud->SetSunLightDirection(Value);
	SkyDome->SetSunLightDirection(Value);
	
}

const ZEVector3& ZEWeather::GetSunDirection() const
{
	return SunDirection;
}

void ZEWeather::SetSunMoonRotation(const ZEVector3& Value)
{
	SunMoonRotation = Value;
}

const ZEVector3& ZEWeather::GetSunMoonRotation() const
{
	return SunMoonRotation;
}

void ZEWeather::SetSunLightColor(const ZEVector3& Value)
{
	SunLightColor = Value;
	Cloud->SetSunLightColor(Value);
}

const ZEVector3& ZEWeather::GetSunLightColor() const
{
	return SunLightColor;
}

void ZEWeather::SetMoonLightColor(const ZEVector3& Value)
{
	MoonLightColor = Value;
}

const ZEVector3& ZEWeather::GetMoonLightColor() const
{
	return MoonLightColor;
}

ZEDrawFlags ZEWeather::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

void ZEWeather::Deinitialize()
{
	MoonLight->Deinitialize();
	MoonLight = NULL;

	SunLight->Deinitialize();
	SunLight = NULL;

	StarMap->Deinitialize();
	StarMap = NULL;

	SkyDome->Deinitialize();
	SkyDome = NULL;

	Cloud->Deinitialize();
	Cloud = NULL;

	Moon->Deinitialize();
	Moon = NULL;

}

void ZEWeather::Draw(ZEDrawParameters* DrawParameters)
{
	
}

void ZEWeather::Tick(float Time)
{

}

ZEWeather* ZEWeather::CreateInstance()
{
	return new ZEWeather();
}

bool ZEWeather::Initialize()
{
	ZEEntity::Initialize();



	return true;
}

ZEWeather::ZEWeather()
{
	SunLight			= NULL;
	MoonLight			= NULL;
	StarMap				= NULL;
	SkyDome				= NULL;
	Cloud				= NULL;
	Moon				= NULL;

	FogFactor			= 0.0f;
	MoonPhase			= 0.0f;
	CloudCover			= 0.0f;
	MoonLightIntensity	= 0.3f;
	SunLightIntensity	= 1.2f;

	SunDirection		= ZEVector3(0.00001f, -1.0f, 0.00001f);
	SunMoonRotation		= ZEVector3(0.0f, 0.0f, 0.0f);
	MoonDirection		= -SunDirection;

	SunLightColor		= ZEVector3(0.650f, 0.570f, 0.475f);
	MoonLightColor		= ZEVector3(ZEVector3::One);

	// Sun Light
	SunLight = ZEDirectionalLight::CreateInstance();
	SunLight->SetEnabled(true);
	SunLight->SetRotation(ZEQuaternion(ZE_PI_2, ZEVector3::UnitX));
	SunLight->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	SunLight->SetIntensity(SunLightIntensity);
	SunLight->SetCastsShadow(false);
	SunLight->SetVisible(true);
	SunLight->SetName("TestSunLight");
	this->AddComponent(SunLight);


	// Moon Light
	MoonLight = ZEDirectionalLight::CreateInstance();
	MoonLight->SetName("TestMoonLight");
	MoonLight->SetEnabled(true);
	MoonLight->SetVisible(true);
	MoonLight->SetRotation(ZEQuaternion(ZE_PI_2, -ZEVector3::UnitX));
	MoonLight->SetColor(MoonLightColor);
	MoonLight->SetIntensity(MoonLightIntensity);
	MoonLight->SetCastsShadow(false);
	this->AddComponent(MoonLight);


	// Star Map
	StarMap = ZESkyBrush::CreateInstance();
	StarMap->SetName("TestStarMap");
	StarMap->SetVisible(true);
	StarMap->SetEnabled(true);
	StarMap->SetSkyTexture("StarMap.png");
	StarMap->SetSkyColor(ZEVector3::One);
	StarMap->SetSkyBrightness(0.5f);
	this->AddComponent(StarMap);

	// Moon
	Moon = ZEMoon::CreateInstance();
	Moon->SetName("TestMoon");
	Moon->SetEnabled(true);
	Moon->SetVisible(true);
	Moon->SetMoonTexture("MoonFrame.png", 53, 1);
	Moon->SetMoonAmbientColor(ZEVector3(1.0f, 0.99f, 0.82f));
	Moon->SetMoonDirection(MoonDirection);
	Moon->SetMoonPhase(MoonPhase);
	Moon->SetMoonAmbientFactor(0.7f);
	Moon->SetMoonScale(0.07f);
	this->AddComponent(Moon);

	// Sky Dome
	SkyDome = ZESkyDome::CreateInstance();
	SkyDome->SetName("TestSkyDome");
	SkyDome->SetEnabled(true);
	SkyDome->SetVisible(true);
	SkyDome->SetSunIntensity(10.0f);
	SkyDome->SetOuterRadius(61500.0f);
	SkyDome->SetInnerRadius(60000.0f);
	SkyDome->SetCameraPositionOffset(ZEVector3(0.0f, 60000.0f, 0.0f));
	SkyDome->SetSunLightDirection(SunDirection);
	SkyDome->SetAmbientFactor(0.5f);
	SkyDome->SetMiddayAmbientColor(ZEVector3(0.07451f, 0.10197f, 0.12549f));
	SkyDome->SetSunsetAmbientColor(ZEVector3(0.89020f, 0.60392f, 0.21177f));
	this->AddComponent(SkyDome);

	// Planar Cloud
	Cloud = ZECloud::CreateInstance();
	Cloud->SetName("CloudTest");
	Cloud->SetEnabled(true);
	Cloud->SetVisible(true);
	Cloud->SetCloudFormationTexture("Cloud.bmp");
	Cloud->SetCamera(zeScene->GetActiveCamera());
	Cloud->SetCloudPlaneHeight(600.0f);
	Cloud->SetSunLightDirection(SunDirection);
	Cloud->SetSunLightColor(SunLightColor);
	Cloud->SetCloudCover(CloudCover);
	this->AddComponent(Cloud);
}

ZEWeather::~ZEWeather()
{
	this->Deinitialize();
}
