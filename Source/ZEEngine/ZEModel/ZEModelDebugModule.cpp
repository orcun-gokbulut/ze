//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelDebugModule.cpp
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

#include "ZEModelDebugModule.h"

#include "ZECore/ZEConsole.h"
#include "ZEGame/ZEGame.h"
#include "ZEGame/ZEScene.h"
#include "ZEGame/ZEGrid.h"
#include "ZEGame/ZECharacter.h"
#include "ZEGame/ZEEntityProvider.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEPointLight.h"
#include "ZEGraphics/ZEDirectionalLight.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEGraphics/ZEProjectiveLight.h"
#include "ZEInput/ZEInputModule.h"
#include "ZEInput/ZEInputDefinitions.h"
#include "ZEInput/ZEInputMap.h"
#include "ZEGame/ZESkyBrush.h"
#include "ZEGame/ZEPlayer.h"
#include "ZESound/ZESoundSource.h"
#include "ZESound/ZESoundResource.h"
#include "ZEModel/ZEModel.h"
#include "ZEMath/ZEMathDefinitions.h"

#define ZE_ACTIONID_CAMERA_TURN_LEFT		100
#define ZE_ACTIONID_CAMERA_TURN_RIGHT		101
#define ZE_ACTIONID_CAMERA_TURN_UP			102
#define ZE_ACTIONID_CAMERA_TURN_DOWN		103
#define ZE_ACTIONID_CAMERA_ZOOM_IN			104
#define ZE_ACTIONID_CAMERA_ZOOM_OUT			105
#define ZE_ACTIONID_CHARACTER_MOVE_FORWARD	200
#define ZE_ACTIONID_CHARACTER_MOVE_BACKWARD	201
#define ZE_ACTIONID_CHARACTER_RUN			202
#define ZE_ACTIONID_CHARACTER_STRAFE_LEFT   203
#define ZE_ACTIONID_CHARACTER_STRAFE_RIGHT  204
#define ZE_ACTIONID_CHARACTER_TURN_LEFT		205
#define ZE_ACTIONID_CHARACTER_TURN_RIGHT	206
#define ZE_ACTIONID_CHARACTER_RECORD_START	207
#define ZE_ACTIONID_CHARACTER_RECORD_PLAY	208
#define ZE_ACTIONID_CHARACTER_RECORD_STOP	209
#define ZE_ACTIONID_CHARACTER_RECORD_SAVE	210
#define ZE_ACTIONID_CHARACTER_RECORD_LOAD	211


bool ZEModelAnimationDebugModule::Initialize()
{
	ZEScene* Scene = zeGame->GetScene();

	Camera = ZECamera::CreateInstance();
	Camera->SetNearZ(zeGraphics->GetNearZ());
	Camera->SetFarZ(zeGraphics->GetFarZ());
	Camera->SetFOV(ZE_PI_4 + ZE_PI_8);
	Scene->AddEntity(Camera);
	Scene->SetActiveCamera(Camera);
	
	Grid = ZEGrid::CreateInstance();
	Grid->SetVisible(true);
	Scene->AddEntity(Grid);

	PointLight0 = ZEPointLight::CreateInstance();
	PointLight0->SetIntensity(5.0f);
	PointLight0->SetAttenuation(0.02f, 0.0f, 1.0f);
	PointLight0->SetColor(ZEVector3(1.0f, 0.7f, 0.8f));
	Scene->AddEntity(PointLight0);

	PointLight1 = ZEPointLight::CreateInstance();
	PointLight1->SetIntensity(5.0f);
	PointLight1->SetAttenuation(0.02f, 0.0f, 1.0f);
	PointLight1->SetColor(ZEVector3(1.0f, 0.8f, 0.7f));
	Scene->AddEntity(PointLight1);

	PointLight2 = ZEPointLight::CreateInstance();
	PointLight2->SetIntensity(15.0f);
	PointLight2->SetAttenuation(0.02f, 0.0f, 1.0f);
	PointLight2->SetColor(ZEVector3(0.7f, 0.8f, 1.0f));
	Scene->AddEntity(PointLight2);

	DirectionalLight = ZEDirectionalLight::CreateInstance();
	DirectionalLight->SetIntensity(2.0f);
	DirectionalLight->SetAttenuation(0.02f, 0.0f, 1.0f);
	DirectionalLight->SetColor(ZEVector3(0.7f, 0.8f, 1.0f));
	DirectionalLight->SetEnabled(false);
	Scene->AddEntity(DirectionalLight);

	ProjectiveLight = ZEProjectiveLight::CreateInstance();
	ProjectiveLight->SetPosition(ZEVector3(0.0f, 0.0f, -5.0f));
	ProjectiveLight->SetIntensity(8.0f);
	ProjectiveLight->SetAttenuation(0.02f, 0.0f, 1.0f);
	ProjectiveLight->SetColor(ZEVector3(0.7f, 0.8f, 1.0f));
	ProjectiveLight->SetProjectionTextureFile("projtest.bmp");
	Scene->AddEntity(ProjectiveLight);

	PointLight0->SetEnabled(false);
	PointLight1->SetEnabled(false);
	PointLight2->SetEnabled(false);

	Character = ZECharacter::CreateInstance();
	Character->SetPosition(ZEVector3(0.0f, 0.0f, 0.0f));
	Scene->AddEntity(Character);

	ZESkyBrush* Sky = ZESkyBrush::CreateInstance();
	Sky->SetSkyTexture("Night.tga");
	Sky->SetSkyBrightness(5.5f);

	Scene->AddEntity(Sky);
	
	InputMap.InputBindings.Clear();
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_TURN_UP,			ZEInputEvent("Mouse", ZE_IMA_VERTICAL_AXIS, ZE_IAS_POSITIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_TURN_DOWN,			ZEInputEvent("Mouse", ZE_IMA_VERTICAL_AXIS, ZE_IAS_NEGATIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_TURN_RIGHT,		ZEInputEvent("Mouse", ZE_IMA_HORIZANTAL_AXIS, ZE_IAS_POSITIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_TURN_LEFT,			ZEInputEvent("Mouse", ZE_IMA_HORIZANTAL_AXIS, ZE_IAS_NEGATIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_ZOOM_IN,			ZEInputEvent("Mouse", ZE_IMA_SCROLL_AXIS, ZE_IAS_POSITIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_ZOOM_OUT,			ZEInputEvent("Mouse", ZE_IMA_SCROLL_AXIS, ZE_IAS_NEGATIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_RUN,			ZEInputEvent("Keyboard", ZE_IKB_LSHIFT, ZE_IBS_PRESSING)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_MOVE_FORWARD,	ZEInputEvent("Keyboard", ZE_IKB_W, ZE_IBS_PRESSING)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_MOVE_BACKWARD,	ZEInputEvent("Keyboard", ZE_IKB_S, ZE_IBS_PRESSING)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_TURN_RIGHT,		ZEInputEvent("Keyboard", ZE_IKB_D, ZE_IBS_PRESSING)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_TURN_LEFT,		ZEInputEvent("Keyboard", ZE_IKB_A, ZE_IBS_PRESSING)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_STRAFE_RIGHT,	ZEInputEvent("Keyboard", ZE_IKB_RIGHT, ZE_IBS_PRESSING)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_STRAFE_LEFT,	ZEInputEvent("Keyboard", ZE_IKB_LEFT, ZE_IBS_PRESSING)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_RECORD_START,	ZEInputEvent("Keyboard", ZE_IKB_F1, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_RECORD_PLAY,	ZEInputEvent("Keyboard", ZE_IKB_F12, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_RECORD_STOP,	ZEInputEvent("Keyboard", ZE_IKB_F2, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_RECORD_SAVE,	ZEInputEvent("Keyboard", ZE_IKB_F5, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_RECORD_LOAD,	ZEInputEvent("Keyboard", ZE_IKB_F7, ZE_IBS_PRESSED)));

	return true;
}

void ZEModelAnimationDebugModule::Deinitialize()
{
	if (Camera != NULL)
	{
		Camera->Destroy();
		Camera = NULL;
	}

	if (Character != NULL)
	{
		Character->Destroy();
		Character = NULL;
	}

	if (Grid != NULL)
	{
		Grid->Destroy();
		Grid = NULL;
	}

	if (Light != NULL)
	{
		Light->Destroy();
		Light = NULL;
	}
}

void ZEModelAnimationDebugModule::Process(float ElapsedTime)
{
	static float Elevation = 0.0f;
	static float Rotation = 0.0f;
	static float Radious = 5.0f;

	zeInput->ProcessInputMap(&InputMap);

	ZEVector3 CameraMovement;
	for (ZESize I = 0; I < InputMap.InputActionCount; I++)
	{
		ZEInputAction* CurrentAction = &InputMap.InputActions[I];
			switch (CurrentAction->Id)
			{	
				case ZE_ACTIONID_CAMERA_ZOOM_IN:
					Radious -= 0.001f * CurrentAction->AxisValue;
					break;

				case ZE_ACTIONID_CAMERA_ZOOM_OUT:
					Radious += 0.001f * CurrentAction->AxisValue;
					break;

				case ZE_ACTIONID_CHARACTER_MOVE_FORWARD:
					if (InputMap.CheckInputAction(ZE_ACTIONID_CHARACTER_RUN) != NULL)
						Character->RunForward();
					else
						Character->WalkForward();
					break;

				case ZE_ACTIONID_CHARACTER_MOVE_BACKWARD:
					Character->WalkBackward();
					break;

				case ZE_ACTIONID_CHARACTER_TURN_LEFT:
					Character->TurnLeft();
					break;

				case ZE_ACTIONID_CHARACTER_TURN_RIGHT:
					Character->TurnRight();
					break;

				case ZE_ACTIONID_CAMERA_TURN_UP:
					Elevation += 0.001f * CurrentAction->AxisValue;
					break;

				case ZE_ACTIONID_CAMERA_TURN_DOWN:
					Elevation -= 0.001f * CurrentAction->AxisValue;
					break;

				case ZE_ACTIONID_CAMERA_TURN_RIGHT:
					Rotation += 0.001f * CurrentAction->AxisValue;
					Character->TurnRight();
					break;

				case ZE_ACTIONID_CAMERA_TURN_LEFT:
					Rotation -= 0.001f * CurrentAction->AxisValue;
					Character->TurnLeft();
					break;
				case ZE_ACTIONID_CHARACTER_STRAFE_LEFT:
					Character->StrafeLeft();
					break;

				case ZE_ACTIONID_CHARACTER_STRAFE_RIGHT:
					Character->StrafeRight();
					break;

				case ZE_ACTIONID_CHARACTER_RECORD_START:
					Character->StartRecording();
					break;

				case ZE_ACTIONID_CHARACTER_RECORD_STOP:
					Character->StopRecording();
					break;

				case ZE_ACTIONID_CHARACTER_RECORD_PLAY:
					Character->PlayRecording();
					break;
	
				case ZE_ACTIONID_CHARACTER_RECORD_SAVE:
					Character->SaveRecording("c:\\Record.zerec");
					break;

				case ZE_ACTIONID_CHARACTER_RECORD_LOAD:
					Character->LoadRecording("c:\\Record.zerec");
					break;
			}
	}

	if (Elevation < -ZE_PI_2)
		Elevation = -ZE_PI_2;
	else if (Elevation > 0)
		Elevation = 0;

	if (Rotation < -ZE_PI)
		Rotation = ZE_PI;
	else if (Rotation > ZE_PI)
		Rotation = -ZE_PI;

	if (Radious < 1.25f)
		Radious = 1.25f;
	else if (Radious > 7.0f)
		Radious = 7.0f;


	ZEVector3 TargetPosition = Character->GetPosition();

	ZEVector3 CameraPosition;
	ZEVector3::CreateFromSpherical(CameraPosition, Radious, Elevation, Rotation);
	ZEVector3::Add(CameraPosition, ZEVector3(TargetPosition.x, 1.0f, TargetPosition.z), CameraPosition);
	Camera->SetPosition(CameraPosition);
	
	ZEQuaternion CameraRotation;
	ZEQuaternion::CreateFromEuler(CameraRotation, ZE_PI_2 + Elevation, ZE_PI_2 - Rotation, 0.0f);
	Camera->SetRotation(CameraRotation);

	PointLight0->SetPosition(TargetPosition + ZEVector3(1.5f, 2.5f, 0.0f));
	PointLight1->SetPosition(TargetPosition + ZEVector3(-1.5f, 0.0f, 1.5f));
	PointLight2->SetPosition(TargetPosition + ZEVector3(0.0f, -1.5f, -1.5f));
}

ZEModelAnimationDebugModule::ZEModelAnimationDebugModule()
{
	Camera = NULL;
	Grid = NULL;
	Light = NULL;
	Character = NULL;
	PointLight0 = NULL;
	PointLight1 = NULL;
	PointLight2 = NULL;
	DirectionalLight = NULL;
	ProjectiveLight = NULL;
}

ZEModelAnimationDebugModule::~ZEModelAnimationDebugModule()
{
	Deinitialize();
}
