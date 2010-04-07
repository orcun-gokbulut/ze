//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Player.cpp
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

#include "Player.h"

#include "Core/Console.h"
#include "Input/InputModule.h"
#include "Input/InputDefinitions.h"
#include "Graphics/GraphicsModule.h"
#include "Game.h"
#include "ZEMath/Ray.h"
#include "ZEMath/Ray.h"
#include "Sound/Listener.h"

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

ZECamera* ZEPlayer::GetCamera()
{
	return &Camera;
}

ZEListener* ZEPlayer::GetListener()
{
	return Listener;
}

void ZEPlayer::SetFOV(float FOV)
{
	this->FOV = FOV;
}

float ZEPlayer::GetFOV()
{
	return FOV;
}

void ZEPlayer::Tick(float Time)
{
	ZEVector3 Position = GetPosition();
	ZEQuaternion Rotation = GetRotation();
	ZEVector3 PositionChange;
	ZEQuaternion RotationChange;
	ZEInputAction* Current;
	zeInput->ProcessInputMap(&InputMap);
	
	float MetersPerSecond = 80.0f;

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
				ZEVector3::Scale(PositionChange, PositionChange, 80.0f * Time);
				ZEVector3::Add(Position, Position, PositionChange);
				SetPosition(Position);
				break;
			case ACTIONID_BACKWARD:
				ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3(0, 0, -1));
				ZEVector3::Scale(PositionChange, PositionChange, 80.0f * Time);
				ZEVector3::Add(Position, Position, PositionChange);
				SetPosition(Position);
				break;
			case ACTIONID_STRAFELEFT:
				ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3(-1, 0, 0));
				ZEVector3::Scale(PositionChange, PositionChange, 80.0f * Time);
				ZEVector3::Add(Position, Position, PositionChange);
				SetPosition(Position);
				break;
			case ACTIONID_STRAFERIGHT:
				ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3(1, 0, 0));
				ZEVector3::Scale(PositionChange, PositionChange, 80.0f * Time);
				ZEVector3::Add(Position, Position, PositionChange);
				SetPosition(Position);
				break;
			case ACTIONID_ZOOMIN:
				if (FOV > ZE_PI_8 * 0.5)
					FOV -= Current->AxisValue * 0.0005f;
				Camera.SetFOV(FOV);
				break;
			case ACTIONID_ZOOMOUT:
				if (FOV < ZE_PI - ZE_PI_8)
					FOV += Current->AxisValue * 0.0005f;
				Camera.SetFOV(FOV);
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
				/*ZEQuaternion::VectorProduct(RayDirection, Camera.GetWorldRotation(), ZEVector3(0.0f, 0.0f, 1.0f));
				if (zeCore->GetGame()->GetScene()->CastRay(ZERay(RayDirection, Camera.GetWorldPosition()), 100000000000000.0f) != NULL)
					continue;
				if (zeGame->GetScene()->CastRay(ZERay(RayDirection, Camera.GetWorldPosition()), 100000000000000.0f, &HitEntity, HitPosition, HitNormal) != NULL)
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

		ZEQuaternion::Create(Rotation, Pitch, Yawn, Roll);
		ZEQuaternion Temp;
		ZEQuaternion::Normalize(Temp,Rotation);
		Rotation = Temp;
		SetRotation(Rotation);
	}
	ZEEntity::Tick(Time);
}

bool ZEPlayer::Initialize()
{
	FOV = ZE_PI_2;
	Yawn = Pitch = Roll = 0;

	Camera.SetLocalPosition(ZEVector3(0.0f, 0.0f, 0.0f));
	Camera.SetLocalRotation(ZEQuaternion::Identity);
	Camera.SetNearZ(zeGraphics->GetNearZ());
	Camera.SetFarZ(zeGraphics->GetFarZ());
	Camera.SetFOV(FOV);
	Camera.SetAspectRatio(zeGraphics->GetAspectRatio());

	Camera.Initialize();
	if (Listener == NULL)
		Listener = ZEListener::CreateInstance();

	Listener->Initialize();
	PointLight.SetLocalPosition(ZEVector3(0.0f, 5.0f, -5.0f));
	PointLight.SetAttenuation(0.00000, 0.0000, 1.0f);
	PointLight.SetRange(10000.0f);
	PointLight.SetIntensity(5.0f);
	PointLight.SetColor(ZEVector3::One);
	PointLight.SetEnabled(true);

	//RegisterComponent(&PointLight);
	RegisterComponent(&Camera);
	RegisterComponent(Listener);

	zeScene->SetActiveCamera(&Camera);
	zeScene->SetActiveListener(Listener);

	return true;
}

void ZEPlayer::Deinitialize()
{
	Camera.Deinitialize();
	Listener->Deinitialize();
}

void ZEPlayer::Draw(ZERenderer * Renderer)
{

}

ZEPlayer::ZEPlayer()
{
	Listener = NULL;
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
}

ZEPlayer::~ZEPlayer()
{
	Deinitialize();
}

#include "Player.h.zpp"
