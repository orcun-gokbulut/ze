//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Main.cpp
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

#include "Core/Core.h"

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#include <math.h>

extern HINSTANCE ApplicationInstance;

#include "Graphics/RenderList.h"
#include "Graphics/Renderer.h"
#include "GameInterface/Player.h"
#include "GameInterface/LightBrush.h"
#include "GameInterface/CanvasBrush.h"
#include "GameInterface/SkyBrush.h"
#include "GameInterface/Scene.h"
#include "GameInterface/ModelBrush.h"
#include "Core/ConsoleWindow.h"
#include "Core/Core.h"

#include "Physics/PhysicsModule.h"
#include "Physics/Aegia/AegiaPhysicsModule.h"
#include "Physics/PhysicsWorld.h"
#include "Physics/PhysicsWorldInfo.h"
#include "Physics/PhysicsBody.h"
#include "Physics/PhysicsBodyInfo.h"
#include "Physics/PhysicsCollisionMask.h"

#include "Physics/PhysicsCharacterController.h"
#include "Physics/PhysicsCharacterControllerInfo.h"

#include "Physics/PhysicsShapeInfo.h"
#include "Physics/PhysicsPlaneShapeInfo.h"
#include "box.h"

ZEPhysicsWorld* World;
ZEPhysicsBody* StaticBody;
ZEPhysicsCollisionMask all;
ZEPhysicsCharacterController* cont1;

ZECanvasBrush* ccapsule;
box* boxes[64];

#include "GameInterface/CameraManager.h"
CameraManager CamMgr;

void initPhy()
{
	//physics
	zePhysics->Initialize();

	//world
	ZEPhysicsWorldInfo WorldInfo;
	WorldInfo.Gravity = ZEVector3(0,-10,0);
	WorldInfo.DebugView = false;
	World = zePhysics->CreateWorld();
	World->Initialize(WorldInfo);

	all.Full();
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	_set_SSE2_enable(1);
	ApplicationInstance = hInstance;
	zeCore->GetOptions()->Load("options.ini");
	zeCore->GetOptions()->ResetChanges();
	zeCore->SetGraphicsModule(zeCore->GetModuleManager()->CreateModule(ZE_MT_GRAPHICS));
	zeCore->SetSoundModule(zeCore->GetModuleManager()->CreateModule(ZE_MT_SOUND));
	zeCore->SetInputModule(zeCore->GetModuleManager()->CreateModule(ZE_MT_INPUT));
	zeCore->SetPhysicsModule((ZEModule*)new ZEAegiaPhysicsModule());
	ZEConsoleWindow ConsoleWindow;
	zeCore->GetConsole()->SetConsoleInterface(&ConsoleWindow);
	zeCore->GetWindow()->SetWindowType(zeCore->GetOptions()->GetOption("Graphics", "Fullscreen")->GetValue().GetBoolean() ? ZE_WT_FULLSCREEN : ZE_WT_RESIZABLE);
	zeCore->GetWindow()->SetWindowSize(zeCore->GetOptions()->GetOption("Graphics", "ScreenWidth")->GetValue().GetInteger(), zeCore->GetOptions()->GetOption("Graphics", "ScreenHeight")->GetValue().GetInteger());

	if (zeCore->StartUp())
	{
		initPhy();
	
		ZEScene* Scene = zeCore->GetGame()->GetScene();
		Scene->LoadEnvironment("BoxMap.ZEMAP");
		Scene->SetVisualDebugElements(ZE_VDE_NONE);

		ZEModelBrush* ModelBrush = (ZEModelBrush*)zeCore->GetGame()->CreateEntityInstance("ZEModelBrush");
		Scene->AddEntity(ModelBrush);
		ModelBrush->SetScale(ZEVector3(1.0f, 1.0f, 1.0f));
		ModelBrush->SetPosition(ZEVector3(1.0f, 0.0f, 0.0f));
		ModelBrush->SetModelFile("skin.zeModel");
		ModelBrush->GetModel()->SetAnimationLooping(true);
		ModelBrush->GetModel()->SetAnimationSpeed(5.0f);
		//ModelBrush->GetModel()->GetBones()[4].SetRelativeRotation(ZEQuaternion(ZE_PI_4, ZEVector3(0.0f, 1.0f, 0.0f)));
		ModelBrush->GetModel()->PlayAnimationByName("test",0, 100);
		//boxes
		for (int i=0;i<5;i++)
		{
			boxes[i] = new box(Scene,"textures/greek_roman0001.tga",10,ZEVector3(0,0+i*1,0),ZEQuaternion(), ZEVector3(0.5,0.5,0.5), false, false, all, 0.25, 0.25);
		}

		//character
		ccapsule = new ZECanvasBrush();
		ccapsule->Canvas.LoadCanvasFile("Test/Capsule.zeCanvas");
		ccapsule->SetPosition(ZEPoint3(0,0,0));
		ccapsule->SetScale(ZEPoint3(0.25,0.60,0.25));
		ccapsule->UpdateCanvas();
		ccapsule->Material.SetZero();
		ccapsule->Material.LightningEnabled = true;
		ccapsule->Material.SetShaderComponents(ZESHADER_DIFFUSEMAP);
		ccapsule->Material.AmbientColor = ZEVector3(0.0f, 0.0f, 0.0f);
		ccapsule->Material.DiffuseColor = ZEVector3(1.0f, 1.0f, 1.0f);
		ccapsule->Material.SpecularColor = ZEVector3(1.0f, 1.0f, 1.0f);
		ccapsule->Material.SpecularFactor = 64.0f;
		ccapsule->Material.DiffuseMap = ZETextureResource::LoadResource("Test/blue.jpg")->GetTexture();
		Scene->AddEntity(ccapsule);

		ZEPhysicsCharacterControllerInfo ChrInfo;
		ChrInfo.Height = 1.25;
		ChrInfo.Radius = 0.125;
		ChrInfo.SlopeLimit = 60;
		ChrInfo.StepLimit = 0.125;
		ChrInfo.Position = ZEVector3(2,1,0);
		cont1 = zePhysics->CreateController();
		cont1->Initialize(ChrInfo);

		ZERenderer* Renderer;
		ZERenderer* ShadowRenderer;

		ZEPlayer* Player = (ZEPlayer*)zeCore->GetGame()->CreateEntityInstance("ZEPlayer");
		Player->SetPosition(ZEVector3(0.0f, 5.0f, 0.0f));
		Player->SetRotation(ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f));
		Scene->AddEntity(Player);

		ZELightBrush* Light = new ZELightBrush();
		Light->SetLightType(ZE_LT_POINT);
		Light->SetPosition(ZEPoint3(1.0f, 5.0f, 1.0f));
		Light->SetScale(ZEPoint3(1.0f, 1.0f, 1.0f));
		Light->GetLight()->SetRange(1500);
		Light->SetRotation(ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f));
		Light->GetLight()->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
		Light->GetLight()->SetAttenuation(0.0f, 0.1f, 0.5f);
		Light->GetLight()->SetIntensity(1.0f);
		Light->GetLight()->SetCastShadows(false);
		Light->SetScale(ZEVector3(0.01,0.01,0.01));
		Scene->AddEntity(Light);

		ZEFreeCameraController* cont1 = new ZEFreeCameraController(Scene->CurrentCamera,ZE_PI_2);
		CamMgr.SetFreeController(cont1);
		ZEFpsCameraController* cont2 = new ZEFpsCameraController(Scene->CurrentCamera,ZE_PI_2);
		CamMgr.SetFpsController(cont2);
		ZETpsCameraController* cont3 = new ZETpsCameraController(Scene->CurrentCamera,ZE_PI / 3, 1000, 100);
		CamMgr.SetTpsController(cont3);

		zeGraphics->SetNearZ(0.1);
		zeGraphics->SetFarZ(1000.0);
		zeCore->Run();
	}
}
