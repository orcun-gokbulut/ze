//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGraphicsDebugModule.cpp
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


#include "ZEBitmap.h"
#include "ZEGame/ZEGrid.h"
#include "ZEGame/ZEGame.h"
#include "ZEGame/ZEScene.h"
#include "ZEMath/ZEAngle.h"
#include "ZEModel/ZEModel.h"
#include "ZEGame/ZEPlayer.h"
#include "ZEUI/ZEUIManager.h"
#include "ZEGame/ZESkyDome.h"
#include "ZESkyDomeMaterial.h"
#include "ZEGame/ZESkyBrush.h"
#include "ZEUI/ZEFontResource.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEUI/ZEUITextControl.h"
#include "ZEGame/ZECanvasBrush.h"
#include "ZEGraphicsDebugModule.h"
#include "ZEGame/ZEEntityProvider.h"
#include "ZEGraphics/ZEPointLight.h"
#include "ZEGraphics/ZESimpleMaterial.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEProjectiveLight.h"
#include "ZEGraphics/ZEProjectiveLight.h"
#include "ZEGraphics/ZEDirectionalLight.h"
#include "ZEMap/ZEPortalMap/ZEPortalMap.h"
#include "ZEGraphics/ZETexture2DResource.h"
#include "ZEGraphics/ZEOmniProjectiveLight.h"
#include "ZEGraphics/ZETextureCubeResource.h"


#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include "ZEGame/ZECloud.h"
#include "ZEInput/ZEInputDefinitions.h"
#include "ZEInput/ZEInputModule.h"
#include "ZECore/ZECore.h"
#include "ZEGame/ZEMoon.h"


#define ACTIONID_INCREASE_DAYTIME		0
#define ACTIONID_DECREASE_DAYTIME		1

#define ACTIONID_INCREASE_CLOUDCOVER	2
#define ACTIONID_DECREASE_CLOUDCOVER	3

#define ACTIONID_INCREASE_MOONPHASE		4
#define ACTIONID_DECREASE_MOONPHASE		5


bool ZEGraphicsDebugModule::Initialize()
{
	ZEScene* Scene = zeGame->GetScene();
	
	PortalMap = ZEPortalMap::CreateInstance();
	PortalMap->SetName("deneme.ZEMAP");
	PortalMap->SetEnabled(true);
	PortalMap->SetVisible(true);
	PortalMap->SetMapFile("Ground.ZeMap");
	PortalMap->SetPosition(ZEVector3(0.0f, 0.0f, 0.0f));
	Scene->AddEntity(PortalMap);
	
	Steering = new ZEPlayerFreeSteering();

	Player = ZEPlayer::CreateInstance();
	Player->SetName("TestPlayer1");
	Player->SetEnabled(true);
	Player->SetVisible(true);
	Player->SetPosition(ZEVector3(0.0f, 10.0f, 0.0f));
	Player->SetRotation(ZEQuaternion::Identity);
	Player->GetCamera()->SetNearZ(zeGraphics->GetNearZ());
	Player->GetCamera()->SetFarZ(zeGraphics->GetFarZ());
	Player->GetCamera()->SetFOV(ZE_PI / 3.0f);
	Player->AddSteering(Steering);
	Player->SetMaxLinearVelocity(50.0f);
	Player->SetMaxLinearAcceleration(50.0f);
	Scene->SetActiveCamera(Player->GetCamera());
	Scene->AddEntity(Player);

	ZEFontResource* Font = ZEFontResource::LoadSharedResource("Courier New.zeFont", NULL);
	
	CoordinatesText = new ZEUITextControl();
	CoordinatesText->SetPosition(ZEVector2(0.0f, 0.0f));
	CoordinatesText->SetSize(ZEVector2(600.0f, 25.0f));
	CoordinatesText->SetName("Coordinates");
	CoordinatesText->SetFont(Font);
	zeGame->UIManager->AddControl(CoordinatesText);

	SunDirectionText = new ZEUITextControl();
	SunDirectionText->SetPosition(ZEVector2(0.0f, 25.0f));
	SunDirectionText->SetSize(ZEVector2(600.0f, 25.0f));
	SunDirectionText->SetName("CameraHeight");
	SunDirectionText->SetFont(Font);
	zeGame->UIManager->AddControl(SunDirectionText);

	CloudCoverText = new ZEUITextControl();
	CloudCoverText->SetPosition(ZEVector2(0.0f, 50.0f));
	CloudCoverText->SetSize(ZEVector2(600.0f, 25.0f));
	CloudCoverText->SetName("Coordinates");
	CloudCoverText->SetFont(Font);
	zeGame->UIManager->AddControl(CloudCoverText);

	WindSpeedText = new ZEUITextControl();
	WindSpeedText->SetPosition(ZEVector2(0.0f, 75.0f));
	WindSpeedText->SetSize(ZEVector2(600.0f, 25.0f));
	WindSpeedText->SetName("MovementSpeed");
	WindSpeedText->SetFont(Font);
	zeGame->UIManager->AddControl(WindSpeedText);
	
	PointLight1 = ZEPointLight::CreateInstance();
	PointLight1->SetPosition(ZEVector3(-6.0f, 3.0f, -2.0f));
	PointLight1->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	PointLight1->SetAttenuation(0.01f, 0.0f, 1.0f);
	PointLight1->SetIntensity(1.0f);
	PointLight1->SetRange(55.0f);
	PointLight1->SetCastShadows(false);
	PointLight1->SetVisible(true);
	PointLight1->SetEnabled(true);
	PointLight1->SetName("TestLight1");
	//Scene->AddEntity(PointLight1);

	PointLight2 = ZEPointLight::CreateInstance();
	PointLight2->SetPosition(ZEVector3(-15.0f, 3.0f, -15.0f));
	PointLight2->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	PointLight2->SetAttenuation(0.01f, 0.0f, 1.0f);
	PointLight2->SetIntensity(1.0f);
	PointLight2->SetRange(55.0f);
	PointLight2->SetCastShadows(false);
	PointLight2->SetVisible(true);
	PointLight2->SetEnabled(true);
	PointLight2->SetName("TestLight2");
	//Scene->AddEntity(PointLight2);

	PointLight3 = ZEPointLight::CreateInstance();
	PointLight3->SetPosition(ZEVector3(-33.0f, 3.0f, -1.0f));
	PointLight3->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	PointLight3->SetAttenuation(0.01f, 0.0f, 1.0f);
	PointLight3->SetIntensity(1.0f);
	PointLight3->SetRange(55.0f);
	PointLight3->SetCastShadows(false);
	PointLight3->SetVisible(true);
	PointLight3->SetEnabled(true);
	PointLight3->SetName("TestLight3");
	//Scene->AddEntity(PointLight3);

	PointLight4 = ZEPointLight::CreateInstance();
	PointLight4->SetPosition(ZEVector3(-17.0f, 4.0f, 37.0f));
	PointLight4->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	PointLight4->SetAttenuation(0.01f, 0.0f, 1.0f);
	PointLight4->SetIntensity(1.0f);
	PointLight4->SetRange(55.0f);
	PointLight4->SetCastShadows(false);
	PointLight4->SetVisible(true);
	PointLight4->SetEnabled(true);
	PointLight4->SetName("TestLight4");
	//Scene->AddEntity(PointLight4);

	PointLight5 = ZEPointLight::CreateInstance();
	PointLight5->SetPosition(ZEVector3(-32.0f, 3.0f, 24.0f));
	PointLight5->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	PointLight5->SetAttenuation(0.01f, 0.0f, 1.0f);
	PointLight5->SetIntensity(1.0f);
	PointLight5->SetRange(55.0f);
	PointLight5->SetCastShadows(false);
	PointLight5->SetVisible(true);
	PointLight5->SetEnabled(true);
	PointLight5->SetName("TestLight5");
	//Scene->AddEntity(PointLight5);

	PointLight6 = ZEPointLight::CreateInstance();
	PointLight6->SetName("TestLight6");
	PointLight6->SetVisible(true);
	PointLight6->SetEnabled(true);
	PointLight6->SetPosition(ZEVector3(-50.0f, 15.0f, -10.0f));
	PointLight6->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	PointLight6->SetAttenuation(0.01f, 0.0f, 1.0f);
	PointLight6->SetIntensity(1.0f);
	PointLight6->SetRange(55.0f);
	PointLight6->SetCastShadows(false);
	//Scene->AddEntity(PointLight6);

	// Sun Light
	SunLight = ZEDirectionalLight::CreateInstance();
	SunLight->SetEnabled(true);
	SunLight->SetRotation(ZEQuaternion(ZE_PI_2, ZEVector3::UnitX));
	SunLight->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	SunLight->SetIntensity(SunLightIntensity);
	SunLight->SetCastsShadow(false);
	SunLight->SetVisible(true);
	SunLight->SetName("TestSunLight");
	Scene->AddEntity(SunLight);
	
	// Moon Light
	MoonLight = ZEDirectionalLight::CreateInstance();
	MoonLight->SetName("TestMoonLight");
	MoonLight->SetEnabled(true);
	MoonLight->SetVisible(true);
	MoonLight->SetRotation(ZEQuaternion(ZE_PI_2, -ZEVector3::UnitX));
	MoonLight->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	MoonLight->SetIntensity(MoonLightIntensity);
	MoonLight->SetCastsShadow(false);
	Scene->AddEntity(MoonLight);

	// Star Map
	StarMap = ZESkyBrush::CreateInstance();
	StarMap->SetName("TestStarMap");
	StarMap->SetVisible(true);
	StarMap->SetEnabled(true);
	StarMap->SetSkyTexture("StarMap.png");
	StarMap->SetSkyColor(ZEVector3::One);
	StarMap->SetSkyBrightness(0.5f);
	Scene->AddEntity(StarMap);

	// Moon
	Moon = ZEMoon::CreateInstance();
	Moon->SetName("TestMoon");
	Moon->SetEnabled(true);
	Moon->SetVisible(true);
	// Moon->SetMoonTexture("VolumeTextureTest.tga", 4, 4);
	Moon->SetMoonTexture("MoonFrame.png", 53, 1);
	Moon->SetMoonAmbientColor(ZEVector3(1.0f, 0.99f, 0.82f));
	Moon->SetMoonDirection(MoonDirection);
	Moon->SetMoonPhase(MoonPhase);
	Moon->SetMoonAmbientFactor(0.7f);
	Moon->SetMoonScale(0.07f);
	Scene->AddEntity(Moon);

	// Sky Dome
	SkyDome = ZESkyDome::CreateInstance();
	SkyDome->SetName("TestSkyDome");
	SkyDome->SetEnabled(true);
	SkyDome->SetVisible(true);
	SkyDome->SetSunIntensity(15.0f);
	SkyDome->SetOuterRadius(61500.0f);
	SkyDome->SetInnerRadius(60000.0f);
	SkyDome->SetCameraPositionOffset(ZEVector3(0.0f, 60000.0f, 0.0f));
	SkyDome->SetSunLightDirection(SunDirection);
	SkyDome->SetAmbientFactor(0.5f);
	SkyDome->SetMiddayAmbientColor(ZEVector3(0.07451f, 0.10197f, 0.12549f));
	SkyDome->SetSunsetAmbientColor(ZEVector3(0.89020f, 0.60392f, 0.21177f));
	Scene->AddEntity(SkyDome);

	// Planar Cloud
	Cloud = ZECloud::CreateInstance();
	Cloud->SetName("CloudTest");
	Cloud->SetEnabled(true);
	Cloud->SetVisible(true);
	Cloud->SetCloudFormationTexture("Cloud.bmp");
	Cloud->SetCamera(Scene->GetActiveCamera());
	Cloud->SetCloudPlaneHeight(600.0f);
	Cloud->SetSunLightDirection(SunDirection);
	Cloud->SetSunLightColor(ZEVector3(0.650f, 0.570f, 0.475f));
	Cloud->SetCloudCover(CloudCover);
	Scene->AddEntity(Cloud);

	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_INCREASE_MOONPHASE,			ZEInputEvent("Keyboard", ZE_IKB_NUMPAD9, ZE_IBS_PRESSING)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_DECREASE_MOONPHASE,			ZEInputEvent("Keyboard", ZE_IKB_NUMPAD6, ZE_IBS_PRESSING)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_INCREASE_DAYTIME,			ZEInputEvent("Keyboard", ZE_IKB_NUMPAD8, ZE_IBS_PRESSING)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_DECREASE_DAYTIME,			ZEInputEvent("Keyboard", ZE_IKB_NUMPAD5, ZE_IBS_PRESSING)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_INCREASE_CLOUDCOVER,			ZEInputEvent("Keyboard", ZE_IKB_NUMPAD7, ZE_IBS_PRESSING)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_DECREASE_CLOUDCOVER,			ZEInputEvent("Keyboard", ZE_IKB_NUMPAD4, ZE_IBS_PRESSING)));

	return true;
}

void ZEGraphicsDebugModule::Deinitialize()
{
	zeScene->SetActiveCamera(NULL);

	Moon->Destroy();
	PortalMap->Destroy();
	Player->Destroy();
	Cloud->Destroy();
	StarMap->Destroy();
	PointLight1->Destroy();
	PointLight2->Destroy();
	PointLight3->Destroy();
	PointLight4->Destroy();
	PointLight5->Destroy();
	OmniProjectiveLight0->Destroy();
	ProjectiveLight0->Destroy();
	SunLight->Destroy();

	delete Steering;
	delete CoordinatesText;
	delete SunDirectionText;
	delete CloudCoverText;
}

void ZEGraphicsDebugModule::Process(float ElapsedTime)
{
	this->ProcessInputs(ElapsedTime);
	this->DisplayStatus();
}

void ZEGraphicsDebugModule::DisplayStatus()
{
	// Print sky dome status
	ZEVector3 CamPos = zeScene->GetActiveCamera()->GetWorldPosition();

	static char PositionBuffer[100];
	static char SunDirectionBuffer[100];
	static char WindDirrectionBuffer[100];
	static char CloudCoverBuffer[100];

	sprintf(PositionBuffer, "Position(X: %.3lf, Y: %.3lf, Z: %.3lf).", CamPos.x, CamPos.y, CamPos.z);
	CoordinatesText->SetText((ZEString)PositionBuffer);

	ZEVector3 Dir = Cloud->GetSunLightDirection();
	sprintf(SunDirectionBuffer, "Sunlight Direction(X: %.3lf, Y: %.3lf, Z: %.3lf).", Dir.x, Dir.y, Dir.z);
	SunDirectionText->SetText((ZEString)SunDirectionBuffer);

	sprintf(CloudCoverBuffer, "Cloud Cover:( %.3lf).", CloudCover);
	CloudCoverText->SetText((ZEString)CloudCoverBuffer);

	sprintf(WindDirrectionBuffer, "Wind Speed(X: %.3lf, Y: %.3lf)", Cloud->GetWindVelocity().x, Cloud->GetWindVelocity().y);
	WindSpeedText->SetText((ZEString)WindDirrectionBuffer);
}

void ZEGraphicsDebugModule::ProcessInputs(float ElapsedTime)
{
	ZEInputAction* Current;
	zeInput->ProcessInputMap(&InputMap);

	for (ZESize I = 0; I < InputMap.InputActionCount; I++)
	{
		Current = &InputMap.InputActions[I];
		
		switch(Current->Id)
		{
			case ACTIONID_INCREASE_CLOUDCOVER:
				this->IncreaseCloudCover(ElapsedTime);
				break;

			case ACTIONID_DECREASE_CLOUDCOVER:
				this->DecreaseCloudCover(ElapsedTime);
				break;

			case ACTIONID_INCREASE_DAYTIME:
				this->IncreaseDayTime(ElapsedTime);
				break;

			case ACTIONID_DECREASE_DAYTIME:
				this->DecreaseDayTime(ElapsedTime);
				break;

			case ACTIONID_INCREASE_MOONPHASE:
				this->IncreaseMoonPhase(ElapsedTime);
				break;

			case ACTIONID_DECREASE_MOONPHASE:
				this->DecreaseMoonPhase(ElapsedTime);
				break;
		}
	}
}

void ZEGraphicsDebugModule::UpdateCloudColor()
{
 	float SunDown = 1.0f - (-SkyDome->GetSunLightDirection().y);
 	
	ZEVector3 SunColor(0.650f, 0.570f, 0.475f);

	ZEVector3 NewSunColor;
	NewSunColor.x	= -(ZEMath::Power((SunDown / 2.5f), 2.0f)) + SunColor.x;
	NewSunColor.y	= -(ZEMath::Power((SunDown / 1.7f), 2.0f)) + SunColor.y;
	NewSunColor.z	= -(ZEMath::Power((SunDown / 1.4f), 2.0f)) + SunColor.z;

 	Cloud->SetSunLightColor(NewSunColor);
}

void ZEGraphicsDebugModule::IncreaseMoonPhase(float ElapsedTime)
{
	MoonPhase += MoonPhaseChangeMultiplier * ElapsedTime;
	
	if (MoonPhase > 1.0f)
		MoonPhase = 0.0f;

	//MoonPhase = ZEMath::ClampUpper(MoonPhase, 1.0f);
	Moon->SetMoonPhase(MoonPhase);
}

void ZEGraphicsDebugModule::DecreaseMoonPhase(float ElapsedTime)
{
	MoonPhase -= MoonPhaseChangeMultiplier * ElapsedTime;
	
	if (MoonPhase < 0.0f)
		MoonPhase = 1.0f;
	
	//MoonPhase = ZEMath::ClampLower(MoonPhase, 0.0f);
	Moon->SetMoonPhase(MoonPhase);
}

void ZEGraphicsDebugModule::IncreaseCloudCover(float ElapsedTime)
{
	CloudCover += CloudCoverChangeMultiplier * ElapsedTime;
	CloudCover = ZEMath::ClampUpper(CloudCover, 1.0f);
	Cloud->SetCloudCover(CloudCover);
}

void ZEGraphicsDebugModule::DecreaseCloudCover(float ElapsedTime)
{
	CloudCover -= CloudCoverChangeMultiplier * ElapsedTime;
	CloudCover = ZEMath::ClampLower(CloudCover, 0.0f);
	Cloud->SetCloudCover(CloudCover);
}

void ZEGraphicsDebugModule::IncreaseDayTime(float ElapsedTime)
{
	// Update rotation based on speed and elapsed time
	SunMoonRotation.x += SunMoonRotationMultiplier.x * ElapsedTime;
	SunMoonRotation.y += SunMoonRotationMultiplier.y * ElapsedTime;
	SunMoonRotation.z += SunMoonRotationMultiplier.z * ElapsedTime;

	// Limit New Angles
	if (SunMoonRotation.x >= ZE_PI * 2.0f)
		SunMoonRotation.x = 0.0f;
	if (SunMoonRotation.y >= ZE_PI * 2.0f)
		SunMoonRotation.y = 0.0f;
	if (SunMoonRotation.z >= ZE_PI * 2.0f)
		SunMoonRotation.z = 0.0f;

	// Create Rotation
	ZEVector4 TransformedSunDir;
	ZEMatrix4x4 SunDirRotationMatrix;
	ZEMatrix4x4::CreateRotation(SunDirRotationMatrix, SunMoonRotation.x, SunMoonRotation.y, SunMoonRotation.z);
	ZEMatrix4x4::Transform(TransformedSunDir, SunDirRotationMatrix, ZEVector4(SunDirection, 0.0f));

	SkyDome->SetSunLightDirection(ZEVector3(TransformedSunDir.x, TransformedSunDir.y, TransformedSunDir.z));
	Cloud->SetSunLightDirection(ZEVector3(TransformedSunDir.x, TransformedSunDir.y, TransformedSunDir.z));
	Moon->SetMoonDirection(ZEVector3(-TransformedSunDir.x, -TransformedSunDir.y, -TransformedSunDir.z));
	
	ZEQuaternion SunLightRotation;
	ZEQuaternion MoonLightRotation;
	ZEQuaternion::CreateFromDirection(SunLightRotation, ZEVector3(TransformedSunDir.x, TransformedSunDir.y, TransformedSunDir.z));
	ZEQuaternion::CreateFromDirection(MoonLightRotation, ZEVector3(-TransformedSunDir.x, -TransformedSunDir.y, -TransformedSunDir.z));
	SunLight->SetRotation(SunLightRotation);
	MoonLight->SetRotation(MoonLightRotation);

	this->UpdateCloudColor();
}

void ZEGraphicsDebugModule::DecreaseDayTime(float ElapsedTime)
{
	// Update rotation based on speed and elapsed time
	SunMoonRotation.x -= SunMoonRotationMultiplier.x * ElapsedTime;
	SunMoonRotation.y -= SunMoonRotationMultiplier.y * ElapsedTime;
	SunMoonRotation.z -= SunMoonRotationMultiplier.z * ElapsedTime;

	// Limit New Angles
	if (SunMoonRotation.x <= 0.0f) 
		SunMoonRotation.x = ZE_PI * 2.0f;
	if (SunMoonRotation.y <= 0.0f) 
		SunMoonRotation.y = ZE_PI * 2.0f;
	if (SunMoonRotation.z <= 0.0f) 
		SunMoonRotation.z = ZE_PI * 2.0f;

	// Create Rotation
	ZEVector4 TransformedSunDir;
	ZEMatrix4x4 SunDirRotationMatrix;
	ZEMatrix4x4::CreateRotation(SunDirRotationMatrix, SunMoonRotation.x, SunMoonRotation.y, SunMoonRotation.z);
	ZEMatrix4x4::Transform(TransformedSunDir, SunDirRotationMatrix, ZEVector4(SunDirection, 0.0f));

	SkyDome->SetSunLightDirection(ZEVector3(TransformedSunDir.x, TransformedSunDir.y, TransformedSunDir.z));
	Cloud->SetSunLightDirection(ZEVector3(TransformedSunDir.x, TransformedSunDir.y, TransformedSunDir.z));
	Moon->SetMoonDirection(ZEVector3(-TransformedSunDir.x, -TransformedSunDir.y, -TransformedSunDir.z));

	ZEQuaternion SunLightRotation;
	ZEQuaternion MoonLightRotation;
	ZEQuaternion::CreateFromDirection(SunLightRotation, ZEVector3(TransformedSunDir.x, TransformedSunDir.y, TransformedSunDir.z));
	ZEQuaternion::CreateFromDirection(MoonLightRotation, ZEVector3(-TransformedSunDir.x, -TransformedSunDir.y, -TransformedSunDir.z));
	SunLight->SetRotation(SunLightRotation);
	MoonLight->SetRotation(MoonLightRotation);

	this->UpdateCloudColor();
}

ZEGraphicsDebugModule::ZEGraphicsDebugModule()
{
	CoordinatesText				= NULL;
	SunDirectionText			= NULL;
	CloudCoverText				= NULL;
	WindSpeedText				= NULL;

	PointLight1					= NULL;
	PointLight2					= NULL;
	PointLight3					= NULL;
	PointLight4					= NULL;
	PointLight5					= NULL;
	ProjectiveLight0			= NULL;

	Player						= NULL;
	Moon						= NULL;
	StarMap						= NULL;
	PortalMap					= NULL;
	Model						= NULL;
	Cloud						= NULL;
	OmniProjectiveLight0		= NULL;
	SunLight					= NULL;
	MoonLight					= NULL;
	SkyDome						= NULL;
	Steering					= NULL;

	MoonLightIntensity			= 0.3f;
	SunLightIntensity			= 1.2f;

	MoonPhase					= 0.3f;
	CloudCover					= 0.3f;
	CloudCoverChangeMultiplier	= 0.2f;
	MoonPhaseChangeMultiplier	= 0.2f;
	SunDirection				= ZEVector3(0.00001f, -1.0f, 0.00001f);
	MoonDirection				= -SunDirection;
	SunMoonRotation				= ZEVector3(0.0f, 0.0f, 0.0f);
	SunMoonRotationMultiplier	= ZEVector3(0.7f, 0.01f, 0.01f);
}

ZEGraphicsDebugModule::~ZEGraphicsDebugModule()
{
	Deinitialize();
}
