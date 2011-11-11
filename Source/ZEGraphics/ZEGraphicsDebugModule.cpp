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

#include "ZEGraphicsDebugModule.h"

#include "ZEGame/ZEGame.h"
#include "ZEGame/ZEPlayer.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEPointLight.h"
#include "ZEGraphics/ZEDirectionalLight.h"
#include "ZEGraphics/ZEOmniProjectiveLight.h"
#include "ZEGraphics/ZEProjectiveLight.h"
#include "ZEGraphics/ZETextureCubeResource.h"
#include "ZEGraphics/ZETexture2DResource.h"
#include "ZEGame/ZECanvasBrush.h"
#include "ZEGraphics/ZESimpleMaterial.h"
#include "ZEGame/ZEEntityProvider.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEGraphics/ZEProjectiveLight.h"
#include "ZEModel/ZEModel.h"
#include "ZEMath/ZEMathDefinitions.h"
#include "ZEGame/ZESkyBrush.h"
#include "ZEMap/ZEPortalMap/ZEPortalMap.h"
#include <stdlib.h>

bool ZEGraphicsDebugModule::Initialize()
{
	ZEScene* Scene = zeGame->GetScene();
	
	Player = ZEPlayer::CreateInstance();
	Player->SetName("TestPlayer1");
	Player->SetPosition(ZEVector3(0.0f, 0.0f, -1.0f));
	Player->SetRotation(ZEQuaternion::Identity);
	Player->GetCamera()->SetNearZ(zeGraphics->GetNearZ());
	Player->GetCamera()->SetFarZ(zeGraphics->GetFarZ());
	Scene->SetActiveCamera(Player->GetCamera());
	Scene->AddEntity(Player);

	ZEPortalMap* Map = ZEPortalMap::CreateInstance();
	Map->SetMapFile("deneme.zemap");
	Map->SetPosition(ZEVector3(0.0f, 0.0f, 0.0f));
	Map->SetName("TestMap1");
	Scene->AddEntity(Map);

	/*
	ZEModel* Model = ZEModel::CreateInstance();
	Model->SetModelFile("Zombies.ZEMODEL");
	Model->SetPosition(ZEVector3(0.0f, 0.0f, 0.0f));
	Model->SetName("TestModel1");
	Model->SetVisible(true);
	Model->SetScale(ZEVector3(3.0f, 3.0f, 3.0f));
	Scene->AddEntity(Model);
	*/

	PointLight1 = ZEPointLight::CreateInstance();
	PointLight1->SetPosition(ZEVector3(-6.0f, 3.0f, -2.0f));
	PointLight1->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	PointLight1->SetAttenuation(0.01f, 0.0f, 1.0f);
	PointLight1->SetIntensity(4.0f);
	PointLight1->SetRange(55.0f);
	PointLight1->SetCastShadows(false);
	PointLight1->SetVisible(true);
	PointLight1->SetEnabled(true);
	PointLight1->SetName("TestLight1");
	Scene->AddEntity(PointLight1);

	PointLight2 = ZEPointLight::CreateInstance();
	PointLight2->SetPosition(ZEVector3(-15.0f, 3.0f, -15.0f));
	PointLight2->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	PointLight2->SetAttenuation(0.01f, 0.0f, 1.0f);
	PointLight2->SetIntensity(4.0f);
	PointLight2->SetRange(55.0f);
	PointLight2->SetCastShadows(false);
	PointLight2->SetVisible(true);
	PointLight2->SetEnabled(true);
	PointLight2->SetName("TestLight2");
	Scene->AddEntity(PointLight2);

	PointLight3 = ZEPointLight::CreateInstance();
	PointLight3->SetPosition(ZEVector3(-33.0f, 3.0f, -1.0f));
	PointLight3->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	PointLight3->SetAttenuation(0.01f, 0.0f, 1.0f);
	PointLight3->SetIntensity(4.0f);
	PointLight3->SetRange(55.0f);
	PointLight3->SetCastShadows(false);
	PointLight3->SetVisible(true);
	PointLight3->SetEnabled(true);
	PointLight3->SetName("TestLight3");
	Scene->AddEntity(PointLight3);

	PointLight4 = ZEPointLight::CreateInstance();
	PointLight4->SetPosition(ZEVector3(-17.0f, 4.0f, 37.0f));
	PointLight4->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	PointLight4->SetAttenuation(0.01f, 0.0f, 1.0f);
	PointLight4->SetIntensity(4.0f);
	PointLight4->SetRange(55.0f);
	PointLight4->SetCastShadows(false);
	PointLight4->SetVisible(true);
	PointLight4->SetEnabled(true);
	PointLight4->SetName("TestLight4");
	Scene->AddEntity(PointLight4);

	PointLight5 = ZEPointLight::CreateInstance();
	PointLight5->SetPosition(ZEVector3(-32.0f, 3.0f, 24.0f));
	PointLight5->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	PointLight5->SetAttenuation(0.01f, 0.0f, 1.0f);
	PointLight5->SetIntensity(4.0f);
	PointLight5->SetRange(55.0f);
	PointLight5->SetCastShadows(false);
	PointLight5->SetVisible(true);
	PointLight5->SetEnabled(true);
	PointLight5->SetName("TestLight5");
	Scene->AddEntity(PointLight5);

	PointLight6 = ZEPointLight::CreateInstance();
	PointLight6->SetPosition(ZEVector3(-50.0f, 15.0f, -10.0f));
	PointLight6->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	PointLight6->SetAttenuation(0.01f, 0.0f, 1.0f);
	PointLight6->SetIntensity(7.0f);
	PointLight6->SetRange(55.0f);
	PointLight6->SetCastShadows(false);
	PointLight6->SetVisible(true);
	PointLight6->SetEnabled(true);
	PointLight6->SetName("TestLight6");
	Scene->AddEntity(PointLight6);

	OmniProjectiveLight0 = ZEOmniProjectiveLight::CreateInstance();
	OmniProjectiveLight0->SetPosition(ZEVector3(-32.0f, 3.0f, 24.0f));
	OmniProjectiveLight0->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	OmniProjectiveLight0->SetAttenuation(0.01f, 0.0f, 1.0f);
	OmniProjectiveLight0->SetIntensity(5.0f);
	OmniProjectiveLight0->SetRange(40.0f);
	OmniProjectiveLight0->SetProjectionTexture(ZETextureCubeResource::LoadResource("alarm.tga")->GetTexture());
	OmniProjectiveLight0->SetCastsShadow(false);
	OmniProjectiveLight0->SetEnabled(true);
	OmniProjectiveLight0->SetName("TestOmniProjectiveLight0");
	Scene->AddEntity(OmniProjectiveLight0);

	ProjectiveLight0 = ZEProjectiveLight::CreateInstance();
	ProjectiveLight0->SetPosition(ZEVector3(-32.0f, 3.0f, -10.0f));
	//ProjectiveLight0->SetRotation(ZEQuaternion(ZE_PI_4 + ZE_PI_8, ZEVector3::UnitY));
	ProjectiveLight0->SetScale(ZEVector3(1.0f, 1.0f, 1.0f));
	ProjectiveLight0->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	ProjectiveLight0->SetAttenuation(0.01f, 0.0f, 1.0f);
	ProjectiveLight0->SetIntensity(5.0f);
	ProjectiveLight0->SetRange(55.0f);
	ProjectiveLight0->SetFOV(ZE_PI_4);
	ProjectiveLight0->SetAspectRatio(1.0f);
	ProjectiveLight0->SetProjectionTexture(ZETexture2DResource::LoadResource("test\\pavyon.png")->GetTexture());
	ProjectiveLight0->SetCastsShadow(false);
	ProjectiveLight0->SetVisible(false);
	ProjectiveLight0->SetName("TestProjectiveLight0");
	Scene->AddEntity(ProjectiveLight0);

	DirectionalLight0 = ZEDirectionalLight::CreateInstance();
	DirectionalLight0->SetEnabled(true);
	DirectionalLight0->SetRotation(ZEQuaternion(-ZE_PI_2, ZEVector3::UnitX));
	DirectionalLight0->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	DirectionalLight0->SetIntensity(3.0f);
	DirectionalLight0->SetCastsShadow(false);
	DirectionalLight0->SetVisible(false);
	DirectionalLight0->SetName("TestDirectionalLight0");
	Scene->AddEntity(DirectionalLight0);

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
	Map->Destroy();
}

void ZEGraphicsDebugModule::Process(float ElapsedTime)
{
	static ZEQuaternion Rotation = ZEQuaternion::Identity;
}


ZEGraphicsDebugModule::ZEGraphicsDebugModule()
{
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
