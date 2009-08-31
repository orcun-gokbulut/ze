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
#include "Core/Core.h"

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

#define ACTIONID_VFORWARD		12
#define ACTIONID_VBACKWARD		13
#define ACTIONID_VSTRAFELEFT	14
#define ACTIONID_VSTRAFERIGHT	15
#define ACTIONID_FIRE1          16
#define ACTIONID_FIRE2          17

#define ACTIONID_CAMERA         18
#define ACTIONID_JUMP           19
#define ACTIONID_TESTUP         20
#define ACTIONID_TESTDN         21

ZE_ENTITY_DESCRIPTION_START(ZEPlayer, ZEEntity, ZE_ERA_BOTH, "", "Player spawn point")
	ZE_ENTITY_NOATTRIBUTE(ZEPlayer)
ZE_ENTITY_DESCRIPTION_END(ZEPlayer)


ZEListener* ZEPlayer::GetListener()
{
	return &Listener;
}

#include "Physics/PhysicsWorld.h"
#include "Physics/Aegia/AegiaPhysicsWorld.h"
#include "CameraManager.h"
extern CameraManager CamMgr;

#include "GameInterface/CanvasBrush.h"
#include "box.h"
extern box* boxes[64];

#include "Physics/PhysicsCharacterController.h"
extern ZEPhysicsCharacterController* cont1;
extern ZECanvasBrush* ccapsule;

#include "windows.h"
#include "stdio.h"
#include <iostream>
void ZEPlayer::Tick(float Time)
{
	if (CamMgr.GetCurrentCamera() > 0)Sleep(10);
	static float ttt = 0;
	ttt += Time;
	if (ttt > 1)
	{
		ttt = 0;
		char str[64];
		sprintf(str,"->%f\n",Time);
		zeLog(str);
	}

	ZEVector3 Position = GetPosition();
	ZEQuaternion Rotation = GetRotation();
	ZEVector3 PositionChange;
	ZEQuaternion RotationChange;
	ZEInputAction* Current;
	zeInput->ProcessInputMap(&InputMap);
	float walk = 0,strafe = 0,pitch = 0,yaw = 0;
	static float char_y = -10;
	ZEVector3 cVelocity = ZEVector3(0,0,0);
	static float tps_dist = 4.0;
	
	ZEVector3 RayDirection, HitPosition, HitNormal;
	ZEComponent* HitComponent;
	ZEEntity* HitEntity;
	for (size_t I = 0; I < InputMap.InputActionCount; I++)
	{
		Current = &InputMap.InputActions[I];
		switch(Current->Id)
		{
			case ACTIONID_CAMERA:
			{
				static float ctime = 0;
				ctime += Time;
				if (ctime > 0.5)
				{
					ctime = 0;
					CamMgr.ChangeCamera();
					if (CamMgr.GetCurrentCamera() != 0)
					{
						cont1->SetPosition(CamMgr.GetCamera()->GetLocalPosition());
					}
				}
				break;
			}
			case ACTIONID_TESTUP:
			{
				if (CamMgr.GetCurrentCamera() == 1)
				{
					CamMgr.GetFpsCamera()->Shake();
				}
				else if (CamMgr.GetCurrentCamera() == 2)
				{
					CamMgr.GetTpsCamera()->Shake();
				}
				break;
			}
			case ACTIONID_TESTDN:
			{
				break;
			}
			case ACTIONID_ZOOMIN:
				tps_dist -= Time * 100;
				break;
			case ACTIONID_ZOOMOUT:
				tps_dist += Time * 100;
				break;
			case ACTIONID_TURNLEFT:
				//setCrossair(GetPosition(), GetRotation());
				yaw = -0.0025 * Current->AxisValue / Time;
				break;
			case ACTIONID_TURNRIGHT:
				//setCrossair(GetPosition(), GetRotation());
				yaw = 0.0025 * Current->AxisValue / Time;
				break;
			case ACTIONID_TURNUP:
				//setCrossair(GetPosition(), GetRotation());
				pitch = 0.0025 * Current->AxisValue / Time;
				break;
			case ACTIONID_TURNDOWN:
				//setCrossair(GetPosition(), GetRotation());
				pitch = -0.0025 * Current->AxisValue / Time;
				break;
			case ACTIONID_CONSOLE:
				if (zeConsole->IsVisible())
					zeConsole->HideConsole();
				else
					zeConsole->ShowConsole();
				break;
			case ACTIONID_RAYCAST:
			{
				static bool cas = false;
				cas = !cas;
				ZEAegiaPhysicsWorld::getSingletonPtr()->ShowDebugView(cas);
				break;
			}
			case ACTIONID_FIRE2:
			{
					ZEVector3 dir(0,0,1);
					dir = this->GetRotation() * dir;
					static float ctime = 0;
					ctime += Time;
					if (ctime > 0.2)
					{
						ctime = 0;
						static int ct = 0;
						if (ct > 3)ct = 0;
						boxes[ct]->actor->SetPosition(this->GetPosition());
						boxes[ct]->actor->SetLinearVelocity(ZEVector3(0,0,0));
						boxes[ct]->actor->SetAngularVelocity(ZEVector3(0,0,0));
						boxes[ct]->actor->ApplyImpulse(dir * 250);
						ct++;
					}
					break;
			}
			case ACTIONID_JUMP:
			{
				if (cont1->IsOnGround())
				{
					char_y = 5;
				}
				break;
			}
			case ACTIONID_FORWARD:
			{
				if (CamMgr.GetCurrentCamera() == 0)
				{
					walk = 40;
				}
				else
				{
					ZEQuaternion::VectorProduct(PositionChange, CamMgr.GetCamera()->GetLocalRotation(), ZEVector3(0, 0, 1));
					PositionChange.y = 0;
					cVelocity += PositionChange;
				}
				break;
			}
			case ACTIONID_BACKWARD:
			{
				if (CamMgr.GetCurrentCamera() == 0)
				{
					walk = -40;
				}
				else
				{
					ZEQuaternion::VectorProduct(PositionChange, CamMgr.GetCamera()->GetLocalRotation(), ZEVector3(0, 0, -1));
					PositionChange.y = 0;
					cVelocity += PositionChange;
				}
				break;
			}
			case ACTIONID_STRAFELEFT:
			{
				if (CamMgr.GetCurrentCamera() == 0)
				{
					strafe = -40;
				}
				else
				{
					ZEQuaternion::VectorProduct(PositionChange, CamMgr.GetCamera()->GetLocalRotation(), ZEVector3(-1, 0, 0));
					PositionChange.y = 0;
					cVelocity += PositionChange;
				}
				break;
			}
			case ACTIONID_STRAFERIGHT:
			{
				if (CamMgr.GetCurrentCamera() == 0)
				{
					strafe = 40;
				}
				else
				{
					ZEQuaternion::VectorProduct(PositionChange, CamMgr.GetCamera()->GetLocalRotation(), ZEVector3(1, 0, 0));
					PositionChange.y = 0;
					cVelocity += PositionChange;
				}
				break;
			}
		}
	}

	if (CamMgr.GetCurrentCamera() == 0)
	{
		CamMgr.GetFreeCamera()->setParams(walk, strafe, pitch, yaw);
	}
	else if (CamMgr.GetCurrentCamera() == 1)
	{
		//apply velocity to char controller
		ZEVector3::Scale(cVelocity, cVelocity, 4);
		char_y -= 10 * Time;
		if (cont1->IsOnGround() && char_y < -10)char_y = -10;
		cVelocity.y = char_y;
		cont1->SetVelocity(cVelocity);
		//
		ZEVector3 npos = cont1->GetPosition();
		ZEVector3 offset(0,1,0);
		CamMgr.GetFpsCamera()->SetParams(npos, offset,pitch, yaw);
	}
	else if (CamMgr.GetCurrentCamera() == 2)
	{
		//apply velocity to char controller
		ZEVector3::Scale(cVelocity, cVelocity, 4);
		char_y -= 10 * Time;
		if (cont1->IsOnGround() && char_y < -10)char_y = -10;
		cVelocity.y = char_y;
		cont1->SetVelocity(cVelocity);

		ZEVector3 npos = cont1->GetPosition();
		ZEVector3 offset(0,1,-5);
		ZEVector3::Normalize(offset,offset);
		CamMgr.GetTpsCamera()->SetParams(npos, offset * tps_dist, pitch, yaw);
	}
	CamMgr.Update(Time);

	ZEEntity::Tick(Time);
}

void ZEPlayer::Initialize()
{
	Listener.Initialize();
	Listener.SetLocalPosition(ZEVector3(0.0f, 0.0f, 0.0f));
	Listener.SetLocalRotation(ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f));

	Light.SetRange(200);
	Light.SetLocalPosition(ZEVector3(0.0f, 10.0f, 0.0f));
	Light.SetLocalPosition(ZEVector3(0.0f, 10.0f, 0.0f));
	Light.SetIntensity(3.0f);
	Light.SetAttenuation(0.0001f, 0.01f, 1.0f);
	Light.SetColor(ZEVector3(1.0f, 0.0f, 1.0f));

	RegisterComponent(&Listener);
}
void ZEPlayer::Deinitialize()
{
	Listener.Deinitialize();
	Light.Deinitialize();
}

void ZEPlayer::Draw(ZERenderer * Renderer)
{

}

ZEPlayer::ZEPlayer()
{
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_FORWARD,		"Move Forward",		ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_UP, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_BACKWARD,	"Move Backward",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_DOWN, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_STRAFERIGHT, "Strafe Right",		ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_RIGHT, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_STRAFELEFT,	"Strafe Left",		ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_LEFT, ZE_IBS_PRESSED)));

	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_FIRE1,		"Fire1",			ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMB_LEFTBUTTON, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_FIRE2,		"Fire2",			ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMB_RIGHTBUTTON, ZE_IBS_PRESSED)));

	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_VFORWARD,	"vMove Forward",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_W, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_VBACKWARD,	"vMove Backward",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_S, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_VSTRAFERIGHT,"vStrafe Right",	ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_D, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_VSTRAFELEFT,	"vStrafe Left",		ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_A, ZE_IBS_PRESSED)));

	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_CAMERA,		"Cam",		        ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_C, ZE_IBS_PRESSED)));
	
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_JUMP,		"Jump",		        ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMB_MIDDLEBUTTON, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_TESTUP,		"Testup",		     ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_PGUP, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_TESTDN,		"Testdn",		     ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_PGDN, ZE_IBS_PRESSED)));

	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_TURNUP,		"Turn Up",			ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_VERTICAL_AXIS, ZE_IAS_POSITIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_TURNDOWN,	"Turn Down",		ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_VERTICAL_AXIS, ZE_IAS_NEGATIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_TURNRIGHT,	"Turn Right",		ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_HORIZANTAL_AXIS, ZE_IAS_POSITIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_TURNLEFT,	"Turn Left",		ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_HORIZANTAL_AXIS, ZE_IAS_NEGATIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_ZOOMIN,		"Zoom In",			ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_SCROLL_AXIS, ZE_IAS_POSITIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_ZOOMOUT,		"Zoom Out",			ZEInputEvent(ZE_IDT_MOUSE, ZE_IDK_DEFAULT_MOUSE, ZE_IMA_SCROLL_AXIS, ZE_IAS_NEGATIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_RAYCAST,		"Ray Cast",			ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_R, ZE_IBS_PRESSED)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_CONSOLE,		"Console",			ZEInputEvent(ZE_IDT_KEYBOARD, ZE_IDK_DEFAULT_KEYBOARD, ZE_IKB_GRAVE, ZE_IBS_PRESSED)));
}

ZEPlayer::~ZEPlayer()
{
	Deinitialize();
}
