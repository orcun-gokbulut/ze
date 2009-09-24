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

#include "Graphics/RenderOrder.h"
#include "Graphics/Renderer.h"
#include "Graphics/FixedMaterial.h"
#include "GameInterface/Player.h"
#include "GameInterface/LightBrush.h"
#include "GameInterface/CanvasBrush.h"
#include "GameInterface/SkyBrush.h"
#include "GameInterface/Scene.h"
#include "Core/ConsoleWindow.h"
#include "Core/Core.h"
#include "GameInterface/ModelBrush.h"


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	// Bismillahirahmanirahim

	_set_SSE2_enable(1);
	ApplicationInstance = hInstance;
	zeCore->GetOptions()->Load("options.ini");
	zeCore->GetOptions()->ResetChanges();
	zeCore->SetGraphicsModule(zeCore->GetModuleManager()->CreateModule(ZE_MT_GRAPHICS));
	zeCore->SetSoundModule(zeCore->GetModuleManager()->CreateModule(ZE_MT_SOUND));
	zeCore->SetInputModule(zeCore->GetModuleManager()->CreateModule(ZE_MT_INPUT));
	ZEConsoleWindow ConsoleWindow;
	zeCore->GetConsole()->SetConsoleInterface(&ConsoleWindow);
	zeCore->GetWindow()->SetWindowType(zeCore->GetOptions()->GetOption("Graphics", "Fullscreen")->GetValue().GetBoolean() ? ZE_WT_FULLSCREEN : ZE_WT_RESIZABLE);
	zeCore->GetWindow()->SetWindowSize(zeCore->GetOptions()->GetOption("Graphics", "ScreenWidth")->GetValue().GetInteger(), zeCore->GetOptions()->GetOption("Graphics", "ScreenHeight")->GetValue().GetInteger());
//	zeCore->GetWindow()->SetWindowPosition(0, 0);
 	if (zeCore->StartUp())
	{
		ZEFixedMaterial* Material = ZEFixedMaterial::CreateInstance();
		Material->SetProperty("AmbientFactor", 5.0f);
		int PropId = Material->GetPropertyId("AmbientColor");

		ZEScene* Scene = zeCore->GetGame()->GetScene();

		ZERenderer* Renderer;
		ZERenderer* ShadowRenderer;

		ZEPlayer* Player = (ZEPlayer*)zeCore->GetGame()->CreateEntityInstance("ZEPlayer");

		Player->SetPosition(ZEVector3(0.0f, 5.0f, 0.0f));
		Player->SetRotation(ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f));
		Player->GetCamera()->SetNearZ(zeGraphics->GetNearZ());
		Player->GetCamera()->SetFarZ(zeGraphics->GetFarZ());

		Scene->AddEntity(Player);
		Scene->SetActiveCamera(Player->GetCamera());

		ZELightBrush* Light = new ZELightBrush();
		Light->SetLightType(ZE_LT_PROJECTIVE);
		Light->SetPosition(ZEPoint3(0.0f, 45.0f, 0.0f));
		Light->SetScale(ZEPoint3(1.0f, 1.0f, 1.0f));
		Light->GetLight()->SetRange(150);
		Light->SetRotation(ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f));
		Light->GetLight()->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
		Light->GetLight()->SetAttenuation(0.001f, 0.0f, 2.0f);
		Light->GetLight()->SetIntensity(5.0f);
		Light->GetLight()->SetCastShadows(false);
		Light->SetProjectionFOV(ZE_PI_2);
		Light->SetProjectionAspectRatio(1.0f);
		Light->SetProjectionTexture("test/pavyon.bmp");
		Scene->AddEntity(Light);

		ZECanvasBrush* CanvasBrush = new ZECanvasBrush();
		CanvasBrush->SetRotation(ZEQuaternion(ZE_PI_8, ZEVector3(0.0f, 1.0f, 0.0f)));
		CanvasBrush->SetScale(ZEVector3(0.5f, 0.5f, 0.5f));
		CanvasBrush->Canvas.AddPlane(1000.0f, 1000);
		CanvasBrush->SetPosition(ZEVector3(0.0f, 0.0f, 0.0f));
		CanvasBrush->Canvas.LoadCanvasFile("Test\\test.zeCanvas");
		CanvasBrush->UpdateCanvas();

		ZEFixedMaterial* CanvasMaterial = ZEFixedMaterial::CreateInstance();
		
		CanvasBrush->Material = CanvasMaterial;
		CanvasMaterial->SetZero();
		CanvasMaterial->SetLightningEnabled(true);
		CanvasMaterial->SetTransparancyMode(ZE_MTM_NOTRANSPARACY);
		CanvasMaterial->SetTwoSided(false);
		CanvasMaterial->SetRecivesShadow(false);
		CanvasMaterial->SetAmbientEnabled(true);
		CanvasMaterial->SetAmbientColor(ZEVector3(0.1f, 0.1f, 0.1f));
		CanvasMaterial->SetDiffuseEnabled(true);
		CanvasMaterial->SetDiffuseColor(ZEVector3(1.0f, 1.0f, 1.0f));
		CanvasMaterial->SetDiffuseMap(ZETexture2DResource::LoadResource("Test\\Diffuse.tga")->GetTexture());
		CanvasMaterial->SetSpecularEnabled(true);
		CanvasMaterial->SetSpecularColor(ZEVector3(1.0f, 1.0f, 1.0f));
		CanvasMaterial->SetSpecularShininess(64.0f);
		CanvasMaterial->UpdateMaterial();
		Scene->AddEntity(CanvasBrush);

		/*
		ZESkyBrush* Brush = (ZESkyBrush*)zeCore->GetGame()->CreateEntityInstance("ZESkyBrush");
		Brush->SetSkyTexture("cubetest.tga");
		Brush->SetSkyColor(ZEVector3(1.0f, 1.0f, 1.0f));
		Scene->AddEntity(Brush);
		*/
	
		zeCore->Run();
	}
}
