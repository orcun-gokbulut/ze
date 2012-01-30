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

#include "ZEGame/ZEGame.h"
#include "ZEGame/ZEScene.h"
#include "ZEGame/ZEPlayer.h"
#include "ZEGame/ZESkyBrush.h"
#include "ZEGame/ZECanvasBrush.h"
#include "ZEGame/ZEEntityProvider.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEGraphicsDebugModule.h"
#include "ZEGraphics/ZEPointLight.h"
#include "ZEGraphics/ZESimpleMaterial.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEProjectiveLight.h"
#include "ZEGraphics/ZEProjectiveLight.h"
#include "ZEGraphics/ZEDirectionalLight.h"
#include "ZEGraphics/ZETexture2DResource.h"
#include "ZEGraphics/ZEOmniProjectiveLight.h"
#include "ZEGraphics/ZETextureCubeResource.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMap/ZEPortalMap/ZEPortalMap.h"
#include "ZEModel/ZEModel.h"
#include "ZEUI/ZEFontResource.h"
#include "ZEUI/ZEUIManager.h"

#include <stdlib.h>
#include "ZEGame/ZESkyDome.h"
#include "ZEUI/ZEUITextControl.h"
#include "ZEGame/ZEGrid.h"
#include "ZESkyDomeMaterial.h"



bool ZEGraphicsDebugModule::Initialize()
{
	ZEScene* Scene = zeGame->GetScene();
	
	ZEPortalMap* Map = ZEPortalMap::CreateInstance();
	Map->SetMapFile("Ground.ZEMAP");
	Map->SetPosition(ZEVector3(0.0f, 0.0f, 0.0f));
	Map->SetName("Ground.ZEMAP");
	Scene->AddEntity(Map);
	
	Player = ZEPlayer::CreateInstance();
	Player->SetName("TestPlayer1");
	Player->SetPosition(ZEVector3(0.0f, 10.0f, 0.0f));
	Player->SetRotation(ZEQuaternion::Identity);
	Player->GetCamera()->SetNearZ(zeGraphics->GetNearZ());
	Player->GetCamera()->SetFarZ(zeGraphics->GetFarZ());
	Player->GetCamera()->SetFOV(ZE_PI / 3.0f);
	Player->SetMovementSpeed(500.0f);
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

	DirectionalLight0 = ZEDirectionalLight::CreateInstance();
	DirectionalLight0->SetEnabled(true);
	DirectionalLight0->SetRotation(ZEQuaternion(ZE_PI, ZEVector3::UnitY));
	DirectionalLight0->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	DirectionalLight0->SetIntensity(2.0f);
	DirectionalLight0->SetCastsShadow(false);
	DirectionalLight0->SetVisible(true);
	DirectionalLight0->SetName("TestDirectionalLight0");
	Scene->AddEntity(DirectionalLight0);
	
	SkyDome = ZESkyDome::CreateInstance();
	SkyDome->SetName("TestSkyDome");
	SkyDome->SetEnabled(true);
	SkyDome->SetVisible(true);
	// Try to maintain the Inner/Outer radius ratio
	SkyDome->SetOuterRadius(61500.0f);
	SkyDome->SetInnerRadius(60000.0f);
	// For camera on the ground case, it is advised to use offset as (0.0f, InnerRadius, 0.0f) 
	SkyDome->SetCameraPositionOffset(ZEVector3(0.0f, 60000.0f, 0.0f));
	Scene->AddEntity(SkyDome);

	// Scene->AddEntity(ZEGrid::CreateInstance());

	return true;
}

void ZEGraphicsDebugModule::Deinitialize()
{

	zeScene->SetActiveCamera(NULL);

	Player->Destroy();

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

	Map->Destroy();
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

	// P = point on the circle
	// t = parameter of the parametric equation [0-1]
	// u = unit direction vector to sun
	// R = radius
	// n = unit vector perpendicular to circle plane
	// c = center of th circle
	// P = R * cos(t) * u + R * sin(t) * n * u + c


	static float RotX = 0.0f;
	static float RotY = 0.0f;
	static float RotZ = 0.0f;

	RotX += SunRotationSpeed * ElapsedTime;
	//RotY += SunRotationSpeed * ElapsedTime;
	//RotZ += SunRotationSpeed * ElapsedTime;

	if (RotX > ZE_PI * 2.0f) RotX = 0.0f;
	if (RotY > ZE_PI * 2.0f) RotY = 0.0f;
	if (RotZ > ZE_PI * 2.0f) RotZ = 0.0f;

	ZEMatrix4x4 Rot;
	ZEMatrix4x4::CreateRotation(Rot, RotX, RotY, RotZ);
	static ZEVector3 SunPos = SkyDome->GetSunPosition();
	ZEVector3 SunPosTrans;
	ZEMatrix4x4::Transform(SunPosTrans, Rot, SunPos);
	SunPosTrans.NormalizeSelf();
	SkyDome->SetSunPosition(SunPosTrans);

	ZEQuaternion Temp;
	ZEQuaternion::CreateFromMatrix(Temp, Rot);
	DirectionalLight0->SetRotation(Temp);

}


ZEGraphicsDebugModule::ZEGraphicsDebugModule()
{
	SunRotationSpeed = 0.1f;

	Player = NULL;
	PointLight1 = NULL;
	PointLight2 = NULL;
	PointLight3 = NULL;
	PointLight4 = NULL;
	PointLight5 = NULL;

	ProjectiveLight0 = NULL;
	OmniProjectiveLight0 = NULL;
	DirectionalLight0 = NULL;
	Map = NULL;
	Model = NULL;
}

ZEGraphicsDebugModule::~ZEGraphicsDebugModule()
{
	Deinitialize();
}
