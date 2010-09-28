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

#include "ZEGame\ZEGame.h"
#include "ZEGame\ZEPlayer.h"
#include "ZEGame\ZEScene.h"
#include "ZEGraphics\ZEGraphicsModule.h"
#include "ZEGraphics\ZEPointLight.h"
#include "ZEGraphics\ZEDirectionalLight.h"
#include "ZEGraphics\ZEOmniProjectiveLight.h"
#include "ZEGraphics\ZEProjectiveLight.h"
#include "ZEGraphics\ZETextureCubeResource.h"
#include "ZEGraphics\ZETexture2DResource.h"
#include "ZEGame\ZECanvasBrush.h"
#include "ZEGraphics\ZESimpleMaterial.h"

#include "ZEModel/ZEModel.h"
bool ZEGraphicsDebugModule::Initialize()
{
	ZEScene* Scene = zeGame->GetScene();

	// Create the player
	if (Player == NULL)
	{
		Player = (ZEPlayer*)zeGame->CreateEntityInstance("ZEPlayer");
		Player->SetPosition(ZEVector3(0.0f, 0.0f, -1.0f));
		Player->SetRotation(ZEQuaternion::Identity);
		Player->GetCamera()->SetNearZ(zeGraphics->GetNearZ());
		Player->GetCamera()->SetFarZ(zeGraphics->GetFarZ());
		Scene->SetActiveCamera(Player->GetCamera());
		Scene->AddEntity(Player);
	}

	Scene->LoadMap("PerfTest.zemap");

	ZEModel* Model = new ZEModel();
	Model->SetModelResource(ZEModelResource::LoadResource("test.zemodel"));
	Scene->AddEntity(Model);



	PointLight1 = new ZEPointLight();
	PointLight1->SetPosition(ZEVector3(-6.0f, 3.0f, -2.0f));
	PointLight1->SetColor(ZEVector3::One);
	PointLight1->SetAttenuation(0.01f, 0.0f, 1.0f);
	PointLight1->SetIntensity(1.0f);
	PointLight1->SetRange(55.0f);
	PointLight1->SetCastShadows(false);
	Scene->AddEntity(PointLight1);

	PointLight2 = new ZEPointLight();
	PointLight2->SetPosition(ZEVector3(-15.0f, 3.0f, -15.0f));
	PointLight2->SetColor(ZEVector3(0.5f, 0.5f, 1.0f));
	PointLight2->SetAttenuation(0.01f, 0.0f, 1.0f);
	PointLight2->SetIntensity(1.0f);
	PointLight2->SetRange(40.0f);
	PointLight2->SetCastShadows(false);
	Scene->AddEntity(PointLight2);

	PointLight3 = new ZEPointLight();
	PointLight3->SetPosition(ZEVector3(-33.0f, 3.0f, -1.0f));
	PointLight3->SetColor(ZEVector3(0.5f, 1.0f, 1.0f));
	PointLight3->SetAttenuation(0.01f, 0.0f, 1.0f);
	PointLight3->SetIntensity(1.0f);
	PointLight3->SetRange(40.0f);
	PointLight3->SetCastShadows(false);
	Scene->AddEntity(PointLight3);

	PointLight4 = new ZEPointLight();
	PointLight4->SetPosition(ZEVector3(-17.0f, 4.0f, 37.0f));
	PointLight4->SetColor(ZEVector3(0.25f, 0.0f, 0.75f));
	PointLight4->SetAttenuation(0.03f, 0.0f, 1.0f);
	PointLight4->SetIntensity(2.0f);
	PointLight4->SetRange(40.0f);
	PointLight4->SetCastShadows(false);
	Scene->AddEntity(PointLight4);

	PointLight5 = new ZEPointLight();
	PointLight5->SetPosition(ZEVector3(-32.0f, 3.0f, 24.0f));
	PointLight5->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	PointLight5->SetAttenuation(0.03f, 0.0f, 1.0f);
	PointLight5->SetIntensity(2.0f);
	PointLight5->SetRange(40.0f);
	PointLight5->SetCastShadows(false);
	Scene->AddEntity(PointLight5);

	OmniProjectiveLight0 = new ZEOmniProjectiveLight();
	OmniProjectiveLight0->SetPosition(ZEVector3(-32.0f, 3.0f, 24.0f));
	OmniProjectiveLight0->SetColor(ZEVector3(1.0f, 0.0f, 0.0f));
	OmniProjectiveLight0->SetAttenuation(0.03f, 0.0f, 1.0f);
	OmniProjectiveLight0->SetIntensity(5.0f);
	OmniProjectiveLight0->SetRange(40.0f);
	OmniProjectiveLight0->SetProjectionTexture(ZETextureCubeResource::LoadResource("alarm.tga")->GetTexture());
	OmniProjectiveLight0->SetCastsShadow(false);
	Scene->AddEntity(OmniProjectiveLight0);

	ProjectiveLight0 = new ZEProjectiveLight();
	ProjectiveLight0->SetPosition(ZEVector3(-32.0f, 3.0f, 2.0f));
	ProjectiveLight0->SetRotation(ZEQuaternion(ZE_PI_4, ZEVector3::UnitY));
	ProjectiveLight0->SetScale(ZEVector3(1.0f, 1.0f, 1.0f));
	ProjectiveLight0->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	ProjectiveLight0->SetAttenuation(0.01f, 0.0f, 1.0f);
	ProjectiveLight0->SetIntensity(5.0f);
	ProjectiveLight0->SetRange(55.0f);
	ProjectiveLight0->SetCastsShadow(false);
	ProjectiveLight0->SetFOV(ZE_PI_2);
	ProjectiveLight0->SetAspectRatio(1.0f);
	ProjectiveLight0->SetProjectionTexture(ZETexture2DResource::LoadResource("test\\pavyon.png")->GetTexture());
	Scene->AddEntity(ProjectiveLight0);
	
	// Transformation
	ZEMatrix4x4 WorldTransform;
	float TanFovRange = tanf(ProjectiveLight0->GetFOV() * 0.5f) * ProjectiveLight0->GetRange();
	ZEMatrix4x4::CreateOrientation(WorldTransform, ProjectiveLight0->GetWorldPosition(), ProjectiveLight0->GetWorldRotation(), ZEVector3(TanFovRange * ProjectiveLight0->GetAspectRatio() * 2.0f, TanFovRange * 2.0f, ProjectiveLight0->GetRange()));

	ZECanvasBrush* Brush = new ZECanvasBrush();
	Brush->Canvas.SetRotation(ZEQuaternion(ZE_PI_2, ZEVector3::UnitX));
	Brush->Canvas.SetTranslation(-ZEVector3::UnitZ);
	Brush->Canvas.ApplyTransformationAfter(WorldTransform);
	Brush->Canvas.SetColor(ZEVector4(ProjectiveLight0->GetColor(), 1.0f));
	Brush->Canvas.AddPyramid(1, 1, 1);

	ZESimpleMaterial* Material = ZESimpleMaterial::CreateInstance();
	
	Brush->Material = Material;
	Material->SetTwoSided(true);
	Material->SetVertexColor(true);
	Material->SetWireframe(true);
	Brush->UpdateCanvas();
	Scene->AddEntity(Brush);
	DirectionalLight0 = new ZEDirectionalLight();
	DirectionalLight0->SetEnabled(false);
	DirectionalLight0->SetRotation(ZEQuaternion(-ZE_PI_4, ZEVector3::UnitX));
	DirectionalLight0->SetColor(ZEVector3(1.0f, 1.0f, 0.8f));
	DirectionalLight0->SetIntensity(2.0f);
	DirectionalLight0->SetCastsShadow(false);
	Scene->AddEntity(DirectionalLight0);

	PointLight1->SetEnabled(true);
	PointLight2->SetEnabled(false);
	PointLight3->SetEnabled(false);
	PointLight4->SetEnabled(false);
	PointLight5->SetEnabled(false);
	OmniProjectiveLight0->SetEnabled(true);
	ProjectiveLight0->SetEnabled(true);
	DirectionalLight0->SetEnabled(false);
	return true;
}

void ZEGraphicsDebugModule::Deinitialize()
{
	if (Player != NULL)
	{
		Player->Destroy();
		Player = NULL;
	}

	delete PointLight1;
	delete PointLight2;
	delete PointLight3;
	delete PointLight4;
	delete PointLight5;

	delete OmniProjectiveLight0;
	delete ProjectiveLight0;
	delete DirectionalLight0;
}

void ZEGraphicsDebugModule::Process(float ElapsedTime)
{
	static ZEQuaternion Rotation = ZEQuaternion::Identity;
	
	Rotation = Rotation * ZEQuaternion(ZE_PIx2 * ElapsedTime, ZEVector3::UnitY);
	OmniProjectiveLight0->SetRotation(Rotation);
}


ZEGraphicsDebugModule::ZEGraphicsDebugModule()
{
	Player = NULL;
}

ZEGraphicsDebugModule::~ZEGraphicsDebugModule()
{
	Deinitialize();
}





