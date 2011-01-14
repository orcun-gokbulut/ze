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

#define ZE_ACTIONID_CAMERA_TURN_LEFT		0
#define ZE_ACTIONID_CAMERA_TURN_RIGHT		1
#define ZE_ACTIONID_CAMERA_TURN_UP			2
#define ZE_ACTIONID_CAMERA_TURN_DOWN		3
#define ZE_ACTIONID_CAMERA_ZOOM_IN			4
#define ZE_ACTIONID_CAMERA_ZOOM_OUT			5
#define ZE_ACTIONID_CHARACTER_MOVE_FORWARD	6
#define ZE_ACTIONID_CHARACTER_MOVE_BACKWARD	7
#define ZE_ACTIONID_CHARACTER_RUN			8
#define ZE_ACTIONID_CHARACTER_STRAFE_LEFT   9
#define ZE_ACTIONID_CHARACTER_STRAFE_RIGHT  10
#define ZE_ACTIONID_CHARACTER_TURN_LEFT		11
#define ZE_ACTIONID_CHARACTER_TURN_RIGHT	12


bool ZEModelAnimationDebugModule::Initialize()
{
	ZEScene* Scene = zeGame->GetScene();

	// Create the player
	if (Camera == NULL)
	{
		Camera = new ZECamera();
		Camera->SetNearZ(zeGraphics->GetNearZ());
		Camera->SetFarZ(zeGraphics->GetFarZ());
		Scene->SetActiveCamera(Camera);
		Scene->AddEntity(Camera);
	}
	
	if (Grid == NULL)
	{
		Grid = new ZEGrid();
		Scene->AddEntity(Grid);
		Grid->SetVisible(true);
	}

	if (Light == NULL)
	{
		Light = new ZEPointLight();
		Light->SetPosition(ZEVector3(-6.0f, 3.0f, -2.0f));
		Light->SetColor(ZEVector3::One);
		Light->SetAttenuation(0.01f, 0.0f, 1.0f);
		Light->SetIntensity(1.0f);
		Light->SetRange(55.0f);
		Light->SetCastShadows(false);
		Scene->AddEntity(Light);
	}

	if (Character == NULL)
	{
		Character = new ZECharacter();
		Scene->AddEntity(Character);
	}

	InputMap.InputBindings.Clear();

	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_TURN_UP,			"Turn Up",			ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_VERTICAL_AXIS, ZE_IAS_POSITIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_TURN_DOWN,			"Turn Down",		ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_VERTICAL_AXIS, ZE_IAS_NEGATIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_TURN_RIGHT,		"Turn Right",		ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_HORIZANTAL_AXIS, ZE_IAS_POSITIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_TURN_LEFT,			"Turn Left",		ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_HORIZANTAL_AXIS, ZE_IAS_NEGATIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_ZOOM_IN,			"Zoom In",			ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_SCROLL_AXIS, ZE_IAS_POSITIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CAMERA_ZOOM_OUT,			"Zoom Out",			ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_SCROLL_AXIS, ZE_IAS_NEGATIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_RUN,			"Run",				ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_LSHIFT, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_MOVE_FORWARD,	"Move Forward",		ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_W, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_MOVE_BACKWARD,	"Move Backward",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_S, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_TURN_RIGHT,		"Turn Right",		ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_D, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_TURN_LEFT,		"Turn Left",		ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_A, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_STRAFE_RIGHT,	"Turn Right",		ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_E, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ZE_ACTIONID_CHARACTER_STRAFE_LEFT,	"Turn Left",		ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_Q, ZE_IBS_ALL)));

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

	for (size_t I = 0; I < InputMap.InputActionCount; I++)
	{
		ZEInputAction* CurrentAction = &InputMap.InputActions[I];
		switch (CurrentAction->Id)
		{	
			case ZE_ACTIONID_CHARACTER_MOVE_FORWARD:
				if (InputMap.CheckInputAction(ZE_ACTIONID_CHARACTER_RUN) != NULL)
					Character->RunForward();
				else
					Character->WalkForward();
				break;

			case ZE_ACTIONID_CHARACTER_MOVE_BACKWARD:
				Character->Stop();
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

			case ZE_ACTIONID_CAMERA_TURN_UP:
				Elevation += 0.001f * CurrentAction->AxisValue;
				break;

			case ZE_ACTIONID_CAMERA_TURN_DOWN:
				Elevation -= 0.001f * CurrentAction->AxisValue;
				break;

			case ZE_ACTIONID_CAMERA_TURN_RIGHT:
				Rotation += 0.001f * CurrentAction->AxisValue;
				break;

			case ZE_ACTIONID_CAMERA_TURN_LEFT:
				Rotation -= 0.001f * CurrentAction->AxisValue;
				break;

			case ZE_ACTIONID_CAMERA_ZOOM_IN:
				Radious -= 0.001f * CurrentAction->AxisValue;
				break;
	
			case ZE_ACTIONID_CAMERA_ZOOM_OUT:
				Radious += 0.001f * CurrentAction->AxisValue;
				break;
		}
	}

	if (Elevation < -ZE_PI)
		Elevation = -ZE_PI;
	else if (Elevation > 0)
		Elevation = 0;

	if (Rotation < -ZE_PI)
		Rotation = ZE_PI;
	else if (Rotation > ZE_PI)
		Rotation = -ZE_PI;

	if (Radious < 0.0f)
		Radious = 0.0f;

	ZEVector3 CameraPosition;
	ZEVector3::CreateFromSpherical(CameraPosition, Radious, Elevation, Rotation);
	ZEVector3::Add(CameraPosition, ZEVector3(Character->GetPosition().x, 1.0f, Character->GetPosition().z), CameraPosition);
	Camera->SetPosition(CameraPosition);
	Light->SetPosition(CameraPosition);
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
}

ZEModelAnimationDebugModule::~ZEModelAnimationDebugModule()
{
	Deinitialize();
}
