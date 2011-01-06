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

#include "ZEGame\ZEGame.h"
#include "ZEGame\ZEPlayer.h"
#include "ZEGame\ZEScene.h"
#include "ZEGame\ZEGrid.h"
#include "ZEGraphics\ZEGraphicsModule.h"
#include "ZEGraphics\ZEPointLight.h"
#include "ZEModel\ZEModel.h"
#include "ZEInput\ZEInputModule.h"
#include "ZEInput\ZEInputDefinitions.h"

#define ACTIONID_FORWARD		0
#define ACTIONID_BACKWARD		1
#define ACTIONID_STRAFELEFT		2
#define ACTIONID_STRAFERIGHT	3
#define ACTIONID_TURNLEFT		4
#define ACTIONID_TURNRIGHT		5
#define ACTIONID_TURNUP			6
#define ACTIONID_TURNDOWN		7
#define ACTIONID_ZOOMIN			8
#define ACTIONID_ZOOMOUT		9
#define ACTIONID_CONSOLE		10
#define ACTIONID_RAYCAST		11

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
		Light->SetIntensity(0.3f);
		Light->SetRange(55.0f);
		Light->SetCastShadows(false);
		Scene->AddEntity(Light);
	}

	if (Model == NULL)
	{
		Model = new ZEModel();		
		Model->SetModelResource(ZEModelResource::LoadResource("soldier.zemodel"));
		
		IdleTrack = Model->GetAnimationTracks().Add();
		IdleTrack->SetOwner(Model);
		IdleTrack->SetAnimationByName("Test");
		IdleTrack->SetSpeed(30.0f);
		IdleTrack->SetStartFrame(5);
		IdleTrack->SetEndFrame(105);
		IdleTrack->SetLooping(true);
		IdleTrack->Play();


		// Walk
		WalkTrack = Model->GetAnimationTracks().Add();
		WalkTrack->SetOwner(Model);
		WalkTrack->SetAnimationByName("Test");
		WalkTrack->SetSpeed(30.0f);
		WalkTrack->SetStartFrame(315);
		WalkTrack->SetEndFrame(355);
		WalkTrack->SetLooping(true);
		WalkTrack->SetBlendFactor(0.0f);
		WalkTrack->Play();

		// Run
		RunTrack = Model->GetAnimationTracks().Add();
		RunTrack->SetOwner(Model);
		RunTrack->SetAnimationByName("Test");
		RunTrack->SetSpeed(30.0f);
		RunTrack->SetStartFrame(405);
		RunTrack->SetEndFrame(435);
		RunTrack->SetLooping(true);
		RunTrack->SetBlendFactor(0.0f);
		RunTrack->Play();

		Scene->AddEntity(Model);
	}

	IdleTrack = &Model->GetAnimationTracks()[0];
	WalkTrack = &Model->GetAnimationTracks()[1];
	RunTrack = &Model->GetAnimationTracks()[2];

	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_FORWARD,		"Move Forward",		ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_W, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_BACKWARD,	"Move Backward",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_S, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_STRAFERIGHT, "Strafe Right",		ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_D, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_STRAFELEFT,	"Strafe Left",		ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_A, ZE_IBS_ALL)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_TURNUP,		"Turn Up",			ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_VERTICAL_AXIS, ZE_IAS_POSITIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_TURNDOWN,	"Turn Down",		ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_VERTICAL_AXIS, ZE_IAS_NEGATIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_TURNRIGHT,	"Turn Right",		ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_HORIZANTAL_AXIS, ZE_IAS_POSITIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_TURNLEFT,	"Turn Left",		ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_HORIZANTAL_AXIS, ZE_IAS_NEGATIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_ZOOMIN,		"Zoom In",			ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_SCROLL_AXIS, ZE_IAS_POSITIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_ZOOMOUT,		"Zoom Out",			ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_SCROLL_AXIS, ZE_IAS_NEGATIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_RAYCAST,		"Ray Cast",			ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_R, ZE_IBS_RELEASED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_CONSOLE,		"Console",			ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_GRAVE, ZE_IBS_PRESSED)));

	return true;
}

void ZEModelAnimationDebugModule::Deinitialize()
{
	if (Camera != NULL)
	{
		Camera->Destroy();
		Camera = NULL;
	}

	if (Model != NULL)
	{
		Model->Destroy();
		Model = NULL;
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
	static float Movement = 0.0f;
	zeInput->ProcessInputMap(&InputMap);

	for (size_t I = 0; I < InputMap.InputActionCount; I++)
	{
		ZEInputAction* CurrentAction = &InputMap.InputActions[I];
		switch (CurrentAction->Id)
		{
			case ACTIONID_FORWARD:
				Movement += 0.1f * ElapsedTime;
				break;

			case ACTIONID_BACKWARD:
				Movement -= 0.1f * ElapsedTime;
				break;

			case ACTIONID_TURNUP:
				Elevation += 0.001f * CurrentAction->AxisValue;
				break;

			case ACTIONID_TURNDOWN:
				Elevation -= 0.001f * CurrentAction->AxisValue;
				break;

			case ACTIONID_TURNRIGHT:
				Rotation += 0.001f * CurrentAction->AxisValue;
				break;

			case ACTIONID_TURNLEFT:
				Rotation -= 0.001f * CurrentAction->AxisValue;
				break;

			case ACTIONID_ZOOMIN:
				Radious -= 0.001f * CurrentAction->AxisValue;
				break;
	
			case ACTIONID_ZOOMOUT:
				Radious += 0.001f * CurrentAction->AxisValue;
				break;
		}
	}

	/*if (Movement < 0.0f)
		Movement = 0.0f;
	else if (Movement > 1.0f)
		Movement = 1.0f;

	if (Movement > 0.5f)
	{
		RunTrack->SetSpeed((Movement - 0.5f) * 60.0f);
		WalkTrack->Stop();
		RunTrack->Play();

	}
	else if (Movement > 0.0f)
	{
		WalkTrack->SetSpeed(Movement * 60.0f);
		WalkTrack->Play();
		RunTrack->Stop();
		IdleTrack->Stop();
	}
	else
	{
		WalkTrack->Stop();
		RunTrack->Stop();
		IdleTrack->Play();
	}*/

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
	ZEVector3::Add(CameraPosition, Model->GetWorldPosition(), CameraPosition);
	Camera->SetPosition(CameraPosition);

	ZEQuaternion CameraRotation;

	ZEQuaternion::CreateFromEuler(CameraRotation, ZE_PI_2 + Elevation, ZE_PI_2 - Rotation, 0.0f);
	Camera->SetRotation(CameraRotation);


}

ZEModelAnimationDebugModule::ZEModelAnimationDebugModule()
{
	Camera = NULL;
	Player = NULL;
	Grid = NULL;
	Light = NULL;
	Model = NULL;
}

ZEModelAnimationDebugModule::~ZEModelAnimationDebugModule()
{
	Deinitialize();
}
