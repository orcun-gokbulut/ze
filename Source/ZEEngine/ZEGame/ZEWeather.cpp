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
#include "ZERenderer/ZELightDirectional.h"
#include "ZEMath\ZEAngle.h"
#include "ZEScene.h"
#include "ZEGame.h"
#include "ZEMoon.h"
#include "ZESkyDome.h"
#include "ZERenderer/ZECamera.h"
#include "ZEGame/ZEEntityProvider.h"
#include "ZEMath/ZEMath.h"

void ZEWeather::AdjustHDR(float DayTime)
{

}

void ZEWeather::UpdateCloudColor()
{
	ZEVector3 NewSunColor, SunColor;
	float SunDown = 1 - SunHeight;

	if (SunDown > 1.0f)	// Night Time
	{
		SunDown -= 2.0f;
		SunColor = ZEVector3(0.650f, 0.570f, 0.475f); // FIX THIS: Hard coded right now

		NewSunColor.x	= -(ZEMath::Power((SunDown / 2.5f), 2.0f)) + SunColor.x;
		NewSunColor.y	= -(ZEMath::Power((SunDown / 1.7f), 2.0f)) + SunColor.y;
		NewSunColor.z	= -(ZEMath::Power((SunDown / 1.4f), 2.0f)) + SunColor.z;
	}
	else	// Daytime
	{
		SunColor = ZEVector3(0.850f, 0.750f, 0.655f); // FIX THIS: Hard coded right now

		NewSunColor.x	= -(ZEMath::Power((SunDown / 2.5f), 2.0f)) + SunColor.x;
		NewSunColor.y	= -(ZEMath::Power((SunDown / 1.7f), 2.0f)) + SunColor.y;
		NewSunColor.z	= -(ZEMath::Power((SunDown / 1.4f), 2.0f)) + SunColor.z;
	}

	SetSunLightColor(NewSunColor);
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

ZELightDirectional* ZEWeather::GetSunLight() const
{
	return SunLight;
}

ZELightDirectional* ZEWeather::GetMoonLight() const
{
	return MoonLight;
}

float ZEWeather::GetSunHeight() const
{
	return -SunHeight;
}

float ZEWeather::GetMoonHeight() const
{
	return -MoonHeight;
}

void ZEWeather::SetFogFactor(float Value)
{
	FogFactor = Value;
}

float ZEWeather::GetFogFactor() const
{
	return FogFactor;
}

void ZEWeather::SetMoonPhase(float Value)
{
	MoonPhase = Value;

	Moon->SetPhase(Value);
}

float ZEWeather::GetMoonPhase() const
{
	return MoonPhase;
}

void ZEWeather::SetCloudCover(float Value)
{
	CloudCover = Value;

	//Cloud->SetCloudCover(Value);
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
	MoonDirection = Value.Normalize();
	MoonHeight = MoonDirection.y;

	Moon->SetDirection(MoonDirection);

	// Set moon directional light
	ZEQuaternion MoonLightRotation;
	ZEQuaternion::CreateFromDirection(MoonLightRotation, MoonDirection);
	MoonLight->SetRotation(MoonLightRotation);

	this->UpdateCloudColor();
}

const ZEVector3& ZEWeather::GetMoonDirection() const
{
	return MoonDirection;
}

void ZEWeather::SetSunDirection(const ZEVector3& Value)
{
	SunDirection = Value.Normalize();
	SunHeight = SunDirection.y;
	
	//Cloud->SetSunLightDirection(SunDirection);
	SkyDome->SetSunLightDirection(SunDirection);

	// Also set the directional sun light
	ZEQuaternion SunLightRotation;
	ZEQuaternion::CreateFromDirection(SunLightRotation, SunDirection);
	SunLight->SetRotation(SunLightRotation);

	this->UpdateCloudColor();
}

void ZEWeather::SetSunCastsShadows(bool Value)
{
	SunLight->SetCastsShadow(Value);
}

bool ZEWeather::GetSunCastsShadows() const
{
	return SunLight->GetCastsShadow();
}

void ZEWeather::SetMoonCastsShadows(bool Value)
{
	MoonLight->SetCastsShadow(Value);
}

bool ZEWeather::GetMoonCastsShadows() const
{
	return MoonLight->GetCastsShadow();
}

const ZEVector3& ZEWeather::GetSunDirection() const
{
	return SunDirection;
}

void ZEWeather::SetSunLightColor(const ZEVector3& Value)
{
	SunLightColor = Value;

	//Cloud->SetSunLightColor(Value);
	SunLight->SetColor(Value);
}

const ZEVector3& ZEWeather::GetSunLightColor() const
{
	return SunLightColor;
}

void ZEWeather::SetMoonLightColor(const ZEVector3& Value)
{
	MoonLightColor = Value;

	//Cloud->SetSunLightColor(Value);
	
}

const ZEVector3& ZEWeather::GetMoonLightColor() const
{
	return MoonLightColor;
}

ZEDrawFlags ZEWeather::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

bool ZEWeather::DeinitializeSelf()
{
	MoonLight->Deinitialize();
	SunLight->Deinitialize();
	//StarMap->Deinitialize();
	SkyDome->Deinitialize();
	//Cloud->Deinitialize();
	Moon->Deinitialize();

	return ZEEntity::DeinitializeSelf();
}

void ZEWeather::Draw(ZERNDrawParameters* DrawParameters)
{
	
}

void ZEWeather::Tick(float Time)
{

}

ZEWeather* ZEWeather::CreateInstance()
{
	return new ZEWeather();
}

bool ZEWeather::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

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

	FogFactor			= 0.1f;
	MoonPhase			= 0.3f;
	CloudCover			= 0.3f;
	MoonLightIntensity	= 0.2f;
	SunLightIntensity	= 1.0f;

	SunDirection		= ZEVector3(0.0001f, -1.0f, 0.0001f);
	SunMoonRotation		= ZEVector3(ZE_PI_2, 0.0f, 0.0f);
	MoonDirection		= -SunDirection;

	SunHeight			= SunDirection.y;
	MoonHeight			= MoonDirection.y;

	SunLightColor		= ZEVector3(0.850f, 0.750f, 0.655f);
	MoonLightColor		= ZEVector3(ZEVector3::One);

	ZEQuaternion SunRotation;
	ZEQuaternion::CreateFromDirection(SunRotation, SunDirection);

	// Sun Light
	SunLight = ZELightDirectional::CreateInstance();
	SunLight->SetName("SunLight");
	SunLight->SetVisible(true);
	SunLight->SetEnabled(true);
	SunLight->SetRotation(SunRotation);
	SunLight->SetColor(SunLightColor);
	SunLight->SetIntensity(SunLightIntensity);
	SunLight->SetCastsShadow(false);
	SunLight->SetRange(600.0f);
	this->AddComponent(SunLight);

	ZEQuaternion MoonRotation;
	ZEQuaternion::CreateFromDirection(MoonRotation, -SunDirection);

	// Moon Light
	MoonLight = ZELightDirectional::CreateInstance();
	MoonLight->SetName("MoonLight");
	MoonLight->SetEnabled(true);
	MoonLight->SetVisible(true);
	MoonLight->SetRotation(MoonRotation);
	MoonLight->SetColor(MoonLightColor);
	MoonLight->SetCastsShadow(false);
	MoonLight->SetIntensity(MoonLightIntensity);
	MoonLight->SetRange(600.0f);
	this->AddComponent(MoonLight);

	// Star Map
	//StarMap = ZESkyBrush::CreateInstance();
	//StarMap->SetName("StarMap");
	//StarMap->SetVisible(true);
	//StarMap->SetEnabled(true);
	//StarMap->SetTexture("#R:\\ZEEngine\\ZEAtmosphere\\Textures\\StarMap.png");
	//StarMap->SetColor(ZEVector3::One);
	//StarMap->SetBrightness(1.0f);
	//this->AddComponent(StarMap);

	// Moon
	Moon = ZEMoon::CreateInstance();
	Moon->SetName("Moon");
	Moon->SetEnabled(true);
	Moon->SetVisible(true);
	Moon->SetTexture("#R:\\ZEEngine\\ZEAtmosphere\\Textures\\MoonFrame.png", 53, 1);
	Moon->SetAmbientColor(ZEVector3(1.0f, 0.99f, 0.92f));
	Moon->SetDirection(MoonDirection);
	Moon->SetPhase(MoonPhase);
	Moon->SetAmbientFactor(0.9f);
	Moon->SetMoonScale(0.1f);
	this->AddComponent(Moon);

	// Sky Dome
	SkyDome = ZESkyDome::CreateInstance();
	SkyDome->SetName("SkyDome");
	SkyDome->SetEnabled(true);
	SkyDome->SetVisible(true);
	SkyDome->SetSunIntensity(22.0f);
	SkyDome->SetOuterRadius(61500.0f);
	SkyDome->SetInnerRadius(60000.0f);
	SkyDome->SetCameraPositionOffset(ZEVector3(0.0f, 60000.0f, 0.0f));
	SkyDome->SetSunLightDirection(SunDirection);
	this->AddComponent(SkyDome);

	// Planar Cloud
	//Cloud = ZECloud::CreateInstance();
	//Cloud->SetName("PlanarCloud");
	//Cloud->SetEnabled(true);
	//Cloud->SetVisible(true);
	//Cloud->SetCloudFormationTexture("#R:/ZEEngine/ZEAtmosphere/Textures/Cloud.bmp");
	//Cloud->SetCamera(zeScene->GetActiveCamera());
	//Cloud->SetCloudPlaneHeight(600.0f);
	//Cloud->SetSunLightDirection(SunDirection);
	//Cloud->SetSunLightColor(SunLightColor);
	//Cloud->SetCloudCover(CloudCover);
	//this->AddComponent(Cloud);
}

ZEWeather::~ZEWeather()
{

}
