//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelAnimationDebugModule.cpp
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


#include "ZEModelAnimationDebugModule.h"

#include "ZECore/ZEConsole.h"
#include "ZEGame/ZEGame.h"
#include "ZEGame/ZEScene.h"
#include "ZEGame/ZEGrid.h"
#include "ZEGame/ZECharacter.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEPointLight.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEInput/ZEInputModule.h"
#include "ZEInput/ZEInputDefinitions.h"
#include "ZEInput/ZEInputMap.h"
#include "ZEGame/ZESkyBrush.h"

#define ZE_ACTIONID_CAMERA_TURN_LEFT		100
#define ZE_ACTIONID_CAMERA_TURN_RIGHT		101
#define ZE_ACTIONID_CAMERA_TURN_UP			102
#define ZE_ACTIONID_CAMERA_TURN_DOWN		103
#define ZE_ACTIONID_CAMERA_ZOOM_IN			104
#define ZE_ACTIONID_CAMERA_ZOOM_OUT			105
#define ZE_ACTIONID_CAMERA_MOVE_FORWARD		106	
#define ZE_ACTIONID_CAMERA_MOVE_BACKWARD	107	
#define ZE_ACTIONID_CAMERA_MOVE_RIGHT		108
#define ZE_ACTIONID_CAMERA_MOVE_LEFT		109	
#define ZE_ACTIONID_CAMERA_TURN_UP			110	

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

	//Scene->LoadMap("triplex.zemap");

	//Create the player
	Camera = new ZECamera();
	Camera->SetNearZ(zeGraphics->GetNearZ());
	Camera->SetFarZ(zeGraphics->GetFarZ());
	Scene->SetActiveCamera(Camera);
	Scene->AddEntity(Camera);
	
	Grid = new ZEGrid();
	Grid->SetVisible(true);
	Scene->AddEntity(Grid);

	Sky = new ZESkyBrush();
	Sky->SetSkyTexture("night.tga");
	Scene->AddEntity(Sky);

	//FlashLight = new ZEProjectiveLight();
	
	ZEPointLight* PointLight = NULL;

	PointLight = new ZEPointLight();
	PointLight->SetIntensity(6.0f);
	PointLight->SetAttenuation(2.0f, 0.0f, 6.0f);
	PointLight->SetColor(ZEVector3::One);
	PointLight->SetPosition(ZEVector3(-2.42f, 1.72f, -1.85f));
	Scene->AddEntity(PointLight);

	PointLight = new ZEPointLight();
	PointLight->SetIntensity(5.0f);
	PointLight->SetAttenuation(20.0f, 0.0f, 1.0f);
	PointLight->SetColor(ZEVector3::One);
	PointLight->SetPosition(ZEVector3(-4.33f, 1.10f, -3.44f));
	Scene->AddEntity(PointLight);

	PointLight = new ZEPointLight();
	PointLight->SetIntensity(7.0f);
	PointLight->SetAttenuation(1.0f, 0.0f, 10.0f);
	PointLight->SetColor(ZEVector3::One);
	PointLight->SetPosition(ZEVector3(-6.24f, 1.88f, -0.58f));
	Scene->AddEntity(PointLight);

	PointLight = new ZEPointLight();
	PointLight->SetIntensity(3.0f);
	PointLight->SetAttenuation(1.0f, 0.0f, 3.0f);
	PointLight->SetColor(ZEVector3::One);
	PointLight->SetPosition(ZEVector3(3.91f, 0.66f, -2.89f));
	Scene->AddEntity(PointLight);


	PointLight = new ZEPointLight();
	PointLight->SetIntensity(1.0f);
	PointLight->SetAttenuation(1.0f, 0.0f, 2.0f);
	PointLight->SetColor(ZEVector3::One);
	PointLight->SetPosition(ZEVector3(1.52f, 0.64f, -0.25f));
	Scene->AddEntity(PointLight);

	PointLight = new ZEPointLight();
	PointLight->SetIntensity(3.0f);
	PointLight->SetAttenuation(1.0f, 0.0f, 3.0f);
	PointLight->SetColor(ZEVector3::One);
	PointLight->SetPosition(ZEVector3(7.02f, 1.22f, 0.82f));
	Scene->AddEntity(PointLight);

	PointLight = new ZEPointLight();
	PointLight->SetIntensity(2.0f);
	PointLight->SetAttenuation(1.0f, 0.0f, 3.0f);
	PointLight->SetColor(ZEVector3::One);
	PointLight->SetPosition(ZEVector3(5.10f, 1.93f, 2.95f));
	Scene->AddEntity(PointLight);

	PointLight = new ZEPointLight();
	PointLight->SetIntensity(2.0f);
	PointLight->SetAttenuation(1.0f, 0.0f, 3.0f);
	PointLight->SetColor(ZEVector3::One);
	PointLight->SetPosition(ZEVector3(-6.43f, 2.16f, 3.50f));
	Scene->AddEntity(PointLight);

	PointLight = new ZEPointLight();
	PointLight->SetIntensity(2.0f);
	PointLight->SetAttenuation(1.0f, 0.0f, 3.0f);
	PointLight->SetColor(ZEVector3::One);
	PointLight->SetPosition(ZEVector3(-9.80f, 2.16f, 3.50f));
	Scene->AddEntity(PointLight);

	PointLight = new ZEPointLight();
	PointLight->SetIntensity(2.0f);
	PointLight->SetAttenuation(1.0f, 0.0f, 3.0f);
	PointLight->SetColor(ZEVector3::One);
	PointLight->SetPosition(ZEVector3(-13.67f, 2.16f, 3.50f));
	Scene->AddEntity(PointLight);

	PointLight = new ZEPointLight();
	PointLight->SetIntensity(2.0f);
	PointLight->SetAttenuation(1.0f, 0.0f, 1.0f);
	PointLight->SetColor(ZEVector3::One);
	PointLight->SetPosition(ZEVector3(1.74f, -3.76f, -1.91f));
	Scene->AddEntity(PointLight);

	PointLight = new ZEPointLight();
	PointLight->SetIntensity(2.0f);
	PointLight->SetAttenuation(1.0f, 0.0f, 1.0f);
	PointLight->SetColor(ZEVector3::One);
	PointLight->SetPosition(ZEVector3(-6.41f, -3.76f, -1.91f));
	Scene->AddEntity(PointLight);


	Character = new ZECharacter();
	Scene->AddEntity(Character);

	InputMap.InputBindings.Clear();
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_MOVE_FORWARD,		"",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_UP, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_MOVE_BACKWARD,		"",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_DOWN, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_MOVE_RIGHT,		"",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_RIGHT, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_MOVE_LEFT,			"",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_LEFT, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_TURN_UP,			"",	ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_VERTICAL_AXIS, ZE_IAS_POSITIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_TURN_DOWN,			"",	ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_VERTICAL_AXIS, ZE_IAS_NEGATIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_TURN_RIGHT,		"",	ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_HORIZANTAL_AXIS, ZE_IAS_POSITIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_TURN_LEFT,			"",	ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_HORIZANTAL_AXIS, ZE_IAS_NEGATIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_ZOOM_IN,			"",	ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_SCROLL_AXIS, ZE_IAS_POSITIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_ZOOM_OUT,			"",	ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_SCROLL_AXIS, ZE_IAS_NEGATIVE)));
	
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_RUN,			"",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_LSHIFT, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_MOVE_FORWARD,	"",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_W, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_MOVE_BACKWARD,	"",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_S, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_TURN_RIGHT,		"",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_D, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_TURN_LEFT,		"",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_A, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_STRAFE_RIGHT,	"",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_RIGHT, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_STRAFE_LEFT,	"",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_LEFT, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_RECORD_START,	"",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_F1, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_RECORD_PLAY,	"",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_F12, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_RECORD_STOP,	"",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_F2, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_RECORD_SAVE,	"",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_F5, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_RECORD_LOAD,	"",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_F7, ZE_IBS_PRESSED)));

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
	for (size_t I = 0; I < InputMap.InputActionCount; I++)
	{
		ZEInputAction* CurrentAction = &InputMap.InputActions[I];
		switch (CurrentAction->Id)
		{	
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

			case ZE_ACTIONID_CAMERA_MOVE_FORWARD:
				//TargetPosition += Camera->GetDirection() * (ElapsedTime * 5.0f);
				break;

			case ZE_ACTIONID_CAMERA_MOVE_BACKWARD:
				//TargetPosition -= Camera->GetDirection() * (ElapsedTime * 5.0f);
				break;

			case ZE_ACTIONID_CAMERA_MOVE_RIGHT:
				//TargetPosition += Camera->GetRight() * (ElapsedTime * 5.0f);
				break;

			case ZE_ACTIONID_CAMERA_MOVE_LEFT:
				//TargetPosition -= Camera->GetRight() * (ElapsedTime * 5.0f);
				break;

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

			case ZE_ACTIONID_CHARACTER_STRAFE_LEFT:
				Character->StrafeLeft();
				break;

			case ZE_ACTIONID_CHARACTER_STRAFE_RIGHT:
				Character->StrafeRight();
				break;

			case ZE_ACTIONID_CHARACTER_RECORD_START:
				Character->StartRecording();
				break;

			case ZE_ACTIONID_CHARACTER_RECORD_PLAY:
				Character->PlayRecording();
				break;

			case ZE_ACTIONID_CHARACTER_RECORD_STOP:
				Character->StopRecording();
				break;

			case ZE_ACTIONID_CHARACTER_RECORD_SAVE:
				Character->SaveRecording("c:\\Record.zerec");
				break;

			case ZE_ACTIONID_CHARACTER_RECORD_LOAD:
				Character->LoadRecording("c:\\Record.zerec");
				break;
		}
	}

	if (Elevation < -ZE_PI_4 - ZE_PI_8)
		Elevation = -ZE_PI_4 - ZE_PI_8;
	else if (Elevation > 0)
		Elevation = 0;

	Rotation = -Character->TurnAngle + ZE_PI_2;
	/*if (Rotation < -ZE_PI)
		Rotation = ZE_PI;
	else if (Rotation > ZE_PI)
		Rotation = -ZE_PI;*/

	if (Radious < 1.25f)
		Radious = 1.25f;
	else if (Radious > 7.0f)
		Radious = 7.0f;


	TargetPosition = Character->GetPosition();

	ZEVector3 CameraPosition;
	ZEVector3::CreateFromSpherical(CameraPosition, Radious, Elevation, Rotation);
	ZEVector3::Add(CameraPosition, ZEVector3(TargetPosition.x, 1.0f, TargetPosition.z), CameraPosition);
	Camera->SetPosition(CameraPosition);
	
	//Light->SetPosition(Character->GetPosition() + ZEVector3(0.0f, 2.1f, 0.0f));
	
	ZEQuaternion CameraRotation;
	ZEQuaternion::CreateFromEuler(CameraRotation, ZE_PI_2 + Elevation, ZE_PI_2 - Rotation, 0.0f);
	Camera->SetRotation(CameraRotation);

}

ZEModelAnimationDebugModule::ZEModelAnimationDebugModule()
{
	Camera = NULL;
	Grid = NULL;
	Light = NULL;
	Character = NULL;
	TargetPosition = ZEVector3::Zero;
}

ZEModelAnimationDebugModule::~ZEModelAnimationDebugModule()
{
	Deinitialize();
}
