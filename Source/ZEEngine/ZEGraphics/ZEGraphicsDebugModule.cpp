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


bool ZEGraphicsDebugModule::Initialize()
{
	// Load "AreaMap32.tga"
	/*
	unsigned char* Buffer = (unsigned char*)malloc(sizeof(unsigned char) * 4 * 160 * 160);
	ZEBitmap Bibibitmap;
	Bibibitmap.Load("resources\\AreaMap32.tga");
	Bibibitmap.CopyTo(Buffer, 160 * 4, 160, 160);

	FILE* thefile = fopen("Color.txt", "a+");
	

	for (int i=0; i<160; i++)
	{
		for (int j=0; j<160; j++)
		{
			unsigned char* Pixel = Buffer + 160 * 4 * i + j * 4;
			fprintf(thefile, "%3d, %3d, %3d, %3d, ", (int)*(Pixel + 0), (int)*(Pixel + 1), (int)*(Pixel + 2), (int)*(Pixel + 3));
			
			if (j % 39 == 0 && j != 0)
				fprintf(thefile, "\n");

			fflush(thefile);
		}
		
	}
	*/

	ZEScene* Scene = zeGame->GetScene();
	
	

	PortalMap = ZEPortalMap::CreateInstance();
	PortalMap->SetMapFile("Ground.ZeMap");
	PortalMap->SetPosition(ZEVector3(0.0f, 0.0f, 0.0f));
	PortalMap->SetName("deneme.ZEMAP");
	Scene->AddEntity(PortalMap);
	
	Player = ZEPlayer::CreateInstance();
	Player->SetName("TestPlayer1");
	Player->SetPosition(ZEVector3(0.0f, 10.0f, 0.0f));
	Player->SetRotation(ZEQuaternion::Identity);
	Player->GetCamera()->SetNearZ(zeGraphics->GetNearZ());
	Player->GetCamera()->SetFarZ(zeGraphics->GetFarZ());
	Player->GetCamera()->SetFOV(ZE_PI / 3.0f);
	Player->SetMovementSpeed(100.0f);
	Scene->SetActiveCamera(Player->GetCamera());
	Scene->AddEntity(Player);

	

	ZEFontResource* Font = ZEFontResource::LoadSharedResource("Courier New.zeFont", NULL);
	
	Coordinates = new ZEUITextControl();
	Coordinates->SetPosition(ZEVector2(0.0f, 0.0f));
	Coordinates->SetSize(ZEVector2(400.0f, 25.0f));
	Coordinates->SetName("Coordinates");
	Coordinates->SetFont(Font);
	zeGame->UIManager->AddControl(Coordinates);

	CameraHeight = new ZEUITextControl();
	CameraHeight->SetPosition(ZEVector2(0.0f, 25.0f));
	CameraHeight->SetSize(ZEVector2(200.0f, 25.0f));
	CameraHeight->SetName("CameraHeight");
	CameraHeight->SetFont(Font);
	zeGame->UIManager->AddControl(CameraHeight);

	InOutRadius = new ZEUITextControl();
	InOutRadius->SetPosition(ZEVector2(0.0f, 50.0f));
	InOutRadius->SetSize(ZEVector2(400.0f, 25.0f));
	InOutRadius->SetName("Coordinates");
	InOutRadius->SetFont(Font);
	zeGame->UIManager->AddControl(InOutRadius);

	MovementSpeed = new ZEUITextControl();
	MovementSpeed->SetPosition(ZEVector2(0.0f, 75.0f));
	MovementSpeed->SetSize(ZEVector2(200.0f, 25.0f));
	MovementSpeed->SetName("MovementSpeed");
	MovementSpeed->SetFont(Font);
	zeGame->UIManager->AddControl(MovementSpeed);
	
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
	PointLight6->SetPosition(ZEVector3(-50.0f, 15.0f, -10.0f));
	PointLight6->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	PointLight6->SetAttenuation(0.01f, 0.0f, 1.0f);
	PointLight6->SetIntensity(1.0f);
	PointLight6->SetRange(55.0f);
	PointLight6->SetCastShadows(false);
	PointLight6->SetVisible(true);
	PointLight6->SetEnabled(true);
	PointLight6->SetName("TestLight6");
	//Scene->AddEntity(PointLight6);

	DirectionalLight0 = ZEDirectionalLight::CreateInstance();
	DirectionalLight0->SetEnabled(true);
	DirectionalLight0->SetRotation(ZEQuaternion(ZE_PI / 2.0f, ZEVector3::UnitX));
	DirectionalLight0->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	DirectionalLight0->SetIntensity(1.0f);
	DirectionalLight0->SetCastsShadow(false);
	DirectionalLight0->SetVisible(true);
	DirectionalLight0->SetName("TestDirectionalLight0");
	Scene->AddEntity(DirectionalLight0);
	
// 	StarMap = ZESkyBrush::CreateInstance();
// 	StarMap->SetSkyTexture("StarMap.png");
// 	StarMap->SetSkyColor(ZEVector3::One);
// 	StarMap->SetSkyBrightness(1.0f);
// 	Scene->AddEntity(StarMap);

	SkyDome = ZESkyDome::CreateInstance();
	SkyDome->SetName("TestSkyDome");
	SkyDome->SetEnabled(true);
	SkyDome->SetVisible(true);
	SkyDome->SetSunIntensity(30.0f);
	SkyDome->SetOuterRadius(61500.0f); // Try to maintain the Inner/Outer radius ratio
	SkyDome->SetInnerRadius(60000.0f); 
	SkyDome->SetCameraPositionOffset(ZEVector3(0.0f, 60000.0f, 0.0f)); // For camera on the ground case, it is advised to use offset as (0.0f, InnerRadius, 0.0f)
	SkyDome->SetSunPosition(ZEVector3(0.0f, 1.0f, 0.0f));
	Scene->AddEntity(SkyDome);

	Cloud = ZECloud::CreateInstance();
	Cloud->SetCamera(Scene->GetActiveCamera());
	Cloud->SetEnabled(true);
	Cloud->SetVisible(true);
	Cloud->SetSunLightDirection(ZEVector3(0.0f, -1.0f, 0.0f));
	Cloud->SetCloudCover(0.6f);
	Cloud->SetCloudPlaneHeight(2000.0f);
	Scene->AddEntity(Cloud);

	return true;
}

void ZEGraphicsDebugModule::Deinitialize()
{
	zeScene->SetActiveCamera(NULL);

	PortalMap->Destroy();
	Player->Destroy();
	Cloud->Destroy();
	PointLight1->Destroy();
	PointLight2->Destroy();
	PointLight3->Destroy();
	PointLight4->Destroy();
	PointLight5->Destroy();
	OmniProjectiveLight0->Destroy();
	ProjectiveLight0->Destroy();
	DirectionalLight0->Destroy();

	delete Coordinates;
	delete CameraHeight;
	delete InOutRadius;

	PortalMap->Destroy();
}

void ZEGraphicsDebugModule::Process(float ElapsedTime)
{
	// Print sky dome status
	ZEVector3 CamPos = zeScene->GetActiveCamera()->GetWorldPosition();
	
	char buffer0[100];
	char buffer1[100];
	char buffer2[100];
	char buffer3[100];

	sprintf(buffer0, "X: %.3lf, Y: %.3lf, Z: %.3lf", CamPos.x, CamPos.y, CamPos.z);
	Coordinates->SetText((ZEString)buffer0);

	float CamHeight = ZEVector3::Length(CamPos);
	sprintf(buffer1, "Camera Height: %.3lf", CamHeight);
	CameraHeight->SetText((ZEString)buffer1);

	float InnerRadius = SkyDome->GetInnerRadius();
	float OuterRadius = SkyDome->GetOuterRadius();
	sprintf(buffer2, "Inner Radius: %.3lf, Outer Radius: %.3lf", InnerRadius, OuterRadius);
	InOutRadius->SetText((ZEString)buffer2);

	sprintf(buffer3, "Movement Speed: %.3lf", ((ZEPlayer*)(zeScene->GetActiveCamera()->GetOwner()))->GetMovementSpeed());
	MovementSpeed->SetText((ZEString)buffer3);

	static float RotX = 0.0f;
	static float RotY = 0.0f;
	static float RotZ = 0.0f;

	RotX += SunRotationSpeed * ElapsedTime * 1.5f;
	// RotY += SunRotationSpeed * ElapsedTime * 1.5f;
	// RotZ += SunRotationSpeed * ElapsedTime * 2;

	if (RotX > ZE_PI * 2.0f) RotX = 0.0f;
	if (RotY > ZE_PI * 2.0f) RotY = 0.0f;
	if (RotZ > ZE_PI * 2.0f) RotZ = 0.0f;

	ZEMatrix4x4 Rot;
	ZEMatrix4x4::CreateRotation(Rot, RotX, RotY, RotZ);
	static ZEVector4 SunPos(SkyDome->GetSunPosition(), 0.0f);
	ZEVector4 RotatedSun;
	ZEMatrix4x4::Transform(RotatedSun, Rot, SunPos);
	RotatedSun.NormalizeSelf();
	
	SkyDome->SetSunPosition(ZEVector3(RotatedSun.x, RotatedSun.y, RotatedSun.z));
	// Cloud->SetSunLightDirection(SunPosTrans);
	

	ZEQuaternion Temp;
	ZEQuaternion::CreateFromMatrix(Temp, Rot);
	// DirectionalLight0->SetRotation(Temp);

}


ZEGraphicsDebugModule::ZEGraphicsDebugModule()
{
	SunRotationSpeed		= 0.1f;

	StarMap					= NULL;
	PortalMap				= NULL;
	Model					= NULL;
	Cloud					= NULL;
	Player					= NULL;
	PointLight1				= NULL;
	PointLight2				= NULL;
	PointLight3				= NULL;
	PointLight4				= NULL;
	PointLight5				= NULL;
	ProjectiveLight0		= NULL;
	DirectionalLight0		= NULL;
	OmniProjectiveLight0	= NULL;
	
}

ZEGraphicsDebugModule::~ZEGraphicsDebugModule()
{
	Deinitialize();
}
