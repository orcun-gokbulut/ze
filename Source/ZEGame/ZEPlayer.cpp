//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPlayer.cpp
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

#include "ZEPlayer.h"

#include "ZECore\ZEConsole.h"
#include "ZEInput\ZEInputModule.h"
#include "ZEInput\ZEInputDefinitions.h"
#include "ZEGraphics\ZEGraphicsModule.h"
#include "ZEGame.h"
#include "ZEMath\ZERay.h"
#include "ZEMath\ZERay.h"
#include "ZESound\ZEListener.h"
#include "ZEEntityProvider.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEGraphics/ZEProjectiveLight.h"

ZE_META_REGISTER_CLASS(ZEEntityProvider, ZEPlayer);

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

ZEDrawFlags ZEPlayer::GetDrawFlags()
{
	return ZE_DF_NONE | ZE_DF_LIGHT_SOURCE;
}

ZECamera* ZEPlayer::GetCamera()
{
	return Camera;
}

ZEListener* ZEPlayer::GetListener()
{
	return Listener;
}

void ZEPlayer::SetFOV(float FOV)
{
	this->FOV = FOV;
	Camera->SetFOV(FOV);
}

float ZEPlayer::GetFOV()
{
	return FOV;
}

void ZEPlayer::Activate()
{
	zeScene->SetActiveCamera(Camera);
	zeScene->SetActiveListener(Listener);
}

void ZEPlayer::Tick(float Time)
{
	ZEVector3 Position = GetPosition();
	ZEQuaternion Rotation = GetRotation();
	ZEVector3 PositionChange;
	ZEQuaternion RotationChange;
	ZEInputAction* Current;
	zeInput->ProcessInputMap(&InputMap);
	
	float MetersPerSecond = 50.0f;

	ZEVector3 RayDirection, HitPosition, HitNormal;
	ZEComponent* HitComponent;
	ZEEntity* HitEntity;
	for (size_t I = 0; I < InputMap.InputActionCount; I++)
	{
		Current = &InputMap.InputActions[I];
		switch(Current->Id)
		{
			case ACTIONID_FORWARD:
				ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3(0, 0, 1));
				ZEVector3::Scale(PositionChange, PositionChange, MetersPerSecond * Time);
				ZEVector3::Add(Position, Position, PositionChange);
				SetPosition(Position);
				break;
			case ACTIONID_BACKWARD:
				ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3(0, 0, -1));
				ZEVector3::Scale(PositionChange, PositionChange, MetersPerSecond * Time);
				ZEVector3::Add(Position, Position, PositionChange);
				SetPosition(Position);
				break;
			case ACTIONID_STRAFELEFT:
				ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3(-1, 0, 0));
				ZEVector3::Scale(PositionChange, PositionChange, MetersPerSecond * Time);
				ZEVector3::Add(Position, Position, PositionChange);
				SetPosition(Position);
				break;
			case ACTIONID_STRAFERIGHT:
				ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3(1, 0, 0));
				ZEVector3::Scale(PositionChange, PositionChange, MetersPerSecond * Time);
				ZEVector3::Add(Position, Position, PositionChange);
				SetPosition(Position);
				break;
			case ACTIONID_ZOOMIN:
				if (FOV > ZE_PI_8 * 0.5)
					FOV -= Current->AxisValue * 0.0005f;
				Camera->SetFOV(FOV);
				break;
			case ACTIONID_ZOOMOUT:
				if (FOV < ZE_PI - ZE_PI_8)
					FOV += Current->AxisValue * 0.0005f;
				Camera->SetFOV(FOV);
				break;
			case ACTIONID_TURNLEFT:
				Yawn = Yawn - 0.005f * Current->AxisValue;
				break;
			case ACTIONID_TURNRIGHT:
				Yawn = Yawn + 0.005f * Current->AxisValue;
				break;
			case ACTIONID_TURNUP:
				Pitch = Pitch + 0.005f * Current->AxisValue;
				break;
			case ACTIONID_TURNDOWN:
				Pitch = Pitch - 0.005f * Current->AxisValue;
				break;
			case ACTIONID_CONSOLE:
				if (zeConsole->IsVisible())
					zeConsole->HideConsole();
				else
					zeConsole->ShowConsole();
				break;
			case ACTIONID_RAYCAST:
				/*ZEQuaternion::VectorProduct(RayDirection, Camera->GetWorldRotation(), ZEVector3(0.0f, 0.0f, 1.0f));
				if (zeCore->GetGame()->GetScene()->CastRay(ZERay(RayDirection, Camera->GetWorldPosition()), 100000000000000.0f) != NULL)
					continue;
				if (zeGame->GetScene()->CastRay(ZERay(RayDirection, Camera->GetWorldPosition()), 100000000000000.0f, &HitEntity, HitPosition, HitNormal) != NULL)
					continue;*/
				break;
		}
		
		if (Yawn < -ZE_PI)
			Yawn = ZE_PI;
		else if (Yawn > ZE_PI)
			Yawn = -ZE_PI;

		if (Pitch < -ZE_PI_2)
			Pitch = -ZE_PI_2;
		else if (Pitch > ZE_PI_2)
			Pitch = ZE_PI_2;

		if (Roll < -ZE_PI)
			Roll = ZE_PI_2;
		else if (Roll > ZE_PI)
			Roll = ZE_PI;

		ZEQuaternion::CreateFromEuler(Rotation, Pitch, Yawn, Roll);
		ZEQuaternion Temp;
		ZEQuaternion::Normalize(Temp,Rotation);
		Rotation = Temp;
		SetRotation(Rotation);
	}

	ZEEntity::Tick(Time);
}

bool ZEPlayer::Initialize()
{
	if (GetInitialized())
		return false;

	FOV = ZE_PI_2;
	Yawn = Pitch = Roll = 0;

	ZECompoundEntity::Initialize();

	zeScene->SetActiveCamera(Camera);
	zeScene->SetActiveListener(Listener);

	return true;
}

void ZEPlayer::Deinitialize()
{
	if (!GetInitialized())
		return;

	ZECompoundEntity::Deinitialize();
}

ZEPlayer::ZEPlayer()
{
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

	Camera = ZECamera::CreateInstance();
	Camera->SetPosition(ZEVector3(0.0f, 0.0f, 0.0f));
	Camera->SetLocalRotation(ZEQuaternion::Identity);
	Camera->SetNearZ(zeGraphics->GetNearZ());
	Camera->SetFarZ(zeGraphics->GetFarZ());
	Camera->SetFOV(FOV);
	Camera->SetAspectRatio(zeGraphics->GetAspectRatio());
	RegisterComponent(Camera);

	Listener = ZEListener::CreateInstance();
	RegisterComponent(Listener);
	/*
	Light = ZEProjectiveLight::CreateInstance();
	Light->SetProjectionTextureFile("flashlight.jpg");
	Light->SetAttenuation(0.01f, 0.0f, 1.0f);
	Light->SetIntensity(3.0f);
	Light->SetRange(55.0f);
	Light->SetFOV(ZE_PI_2);
	Light->SetAspectRatio(1.0f);
	Light->SetCastsShadow(true);
	Light->SetPosition(ZEVector3(0.0f, -2.0f, 0.0f));
	RegisterComponent(Light);
	*/
}

ZEPlayer::~ZEPlayer()
{
	Deinitialize();
}

ZEPlayer* ZEPlayer::CreateInstance()
{
	return new ZEPlayer();
}

#include "ZEPlayer.h.zpp"

ZEEntityRunAt ZEPlayerDescription::GetRunAt() const
{
	return ZE_ERA_BOTH;
}
