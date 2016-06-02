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

#include "ZECore/ZEConsole.h"
#include "ZEInput/ZEInputModule.h"
#include "ZEInput/ZEInputDefinitions.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGame.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZERay.h"
#include "ZESound/ZEListener.h"
#include "ZEEntityProvider.h"
#include "ZERenderer/ZECamera.h"
#include "ZERenderer/ZELightProjective.h"
#include "ZEMath/ZEMath.h"

#define ACTIONID_FORWARD			0
#define ACTIONID_BACKWARD			1
#define ACTIONID_STRAFELEFT			2
#define ACTIONID_STRAFERIGHT		3
#define ACTIONID_TURNLEFT			4
#define ACTIONID_TURNRIGHT			5
#define ACTIONID_TURNUP				6
#define ACTIONID_TURNDOWN			7
#define ACTIONID_TURN_CW			8
#define ACTIONID_TURN_CCW			9
#define ACTIONID_CONSOLE			10

#define ACTIONID_UP					11
#define ACTIONID_DOWN				12

ZESteeringPlayerFree::~ZESteeringPlayerFree()
{

}

ZESteeringPlayerFree::ZESteeringPlayerFree()
{
	Rx = Ry = Rz = 0.0f;

	Friction = 30.0f;

	InputMap.AddButtonAction("Keyboard", ZE_IKB_W,								ZE_IS_PRESSING,		ACTIONID_FORWARD);
	InputMap.AddButtonAction("Keyboard", ZE_IKB_S,								ZE_IS_PRESSING,		ACTIONID_BACKWARD);
	InputMap.AddButtonAction("Keyboard", ZE_IKB_D,								ZE_IS_PRESSING,		ACTIONID_STRAFERIGHT);
	InputMap.AddButtonAction("Keyboard", ZE_IKB_A,								ZE_IS_PRESSING,		ACTIONID_STRAFELEFT);
	InputMap.AddButtonAction("Keyboard", ZE_IKB_R,								ZE_IS_PRESSING,		ACTIONID_UP);
	InputMap.AddButtonAction("Keyboard", ZE_IKB_F,								ZE_IS_PRESSING,		ACTIONID_DOWN);
	InputMap.AddButtonAction("Keyboard", ZE_IKB_Q,								ZE_IS_PRESSING,		ACTIONID_TURN_CW);
	InputMap.AddButtonAction("Keyboard", ZE_IKB_E,								ZE_IS_PRESSING,		ACTIONID_TURN_CCW);
	InputMap.AddAxisAction("Mouse", ZE_IMA_VERTICAL_AXIS, ZE_IAS_POSITIVE,		ZE_IS_CHANGED,		ACTIONID_TURNUP);
	InputMap.AddAxisAction("Mouse", ZE_IMA_VERTICAL_AXIS, ZE_IAS_NEGATIVE,		ZE_IS_CHANGED,		ACTIONID_TURNDOWN);
	InputMap.AddAxisAction("Mouse", ZE_IMA_HORIZANTAL_AXIS, ZE_IAS_POSITIVE,	ZE_IS_CHANGED,		ACTIONID_TURNRIGHT);
	InputMap.AddAxisAction("Mouse", ZE_IMA_HORIZANTAL_AXIS, ZE_IAS_NEGATIVE,	ZE_IS_CHANGED,		ACTIONID_TURNLEFT);
}

ZESteeringOutput ZESteeringPlayerFree::Process(float ElapsedTime)
{
	ZESteeringOutput Output;
	Output.SetZero();

	bool Moved = false;

	ZEQuaternion OwnerCameraRotation = ((ZEPlayer*)GetOwner())->GetCamera()->GetWorldRotation();
	ZEVector3 MovementDirection = ZEVector3::Zero;

	InputMap.Update();

	for (ZESize I = 0; I < InputMap.GetActionCount(); I++)
	{
		const ZEInputAction* Current = &InputMap.GetActions()[I];
		switch(Current->Id)
		{
			case ACTIONID_FORWARD:
				ZEQuaternion::VectorProduct(MovementDirection, OwnerCameraRotation, ZEVector3(0.0f, 0.0f, 1.0f));
				Output.LinearAcceleration += MovementDirection;
				Moved = true;
				break;
			case ACTIONID_BACKWARD:
				ZEQuaternion::VectorProduct(MovementDirection, OwnerCameraRotation, ZEVector3(0.0f, 0.0f, -1.0f));
				Output.LinearAcceleration += MovementDirection;
				Moved = true;
				break;
			case ACTIONID_STRAFELEFT:
				ZEQuaternion::VectorProduct(MovementDirection, OwnerCameraRotation, ZEVector3(-1.0f, 0.0f, 0.0f));
				Output.LinearAcceleration += MovementDirection;
				Moved = true;
				break;
			case ACTIONID_STRAFERIGHT:
				ZEQuaternion::VectorProduct(MovementDirection, OwnerCameraRotation, ZEVector3(1.0f, 0.0f, 0.0f));
				Output.LinearAcceleration += MovementDirection;
				Moved = true;
				break;
			case ACTIONID_UP:
				MovementDirection = ZEVector3(0.0f, 1.0f, 0.0f);
				Output.LinearAcceleration += MovementDirection;
				Moved = true;
				break;
			case ACTIONID_DOWN:
				MovementDirection = ZEVector3(0.0f, -1.0f, 0.0f);
				Output.LinearAcceleration += MovementDirection;
				Moved = true;
				break;
			case ACTIONID_TURNLEFT:
				Ry = Ry - 0.001f * Current->AxisValue;
				break;
			case ACTIONID_TURNRIGHT:
				Ry = Ry + 0.001f * Current->AxisValue;
				break;
			case ACTIONID_TURNUP:
				Rx = Rx + 0.001f * Current->AxisValue;
				break;
			case ACTIONID_TURNDOWN:
				Rx = Rx - 0.001f * Current->AxisValue;
				break;
			case ACTIONID_TURN_CW:
				Rz = Rz + 0.001f;
				break;
			case ACTIONID_TURN_CCW:
				Rz = Rz - 0.001f;
				break;
		}
	}

	if (Ry < -ZE_PI)
	{
		Ry = ZE_PI;
	}
	else if (Ry > ZE_PI)
	{
		Ry = -ZE_PI;
	}

	if (Rx < -ZE_PI_2)
	{
		Rx = -ZE_PI_2;
	}
	else if (Rx > ZE_PI_2)
	{
		Rx = ZE_PI_2;
	}

	if (Rz < -ZE_PI_2)
	{
		Rz = -ZE_PI_2;
	}
	else if (Rz > ZE_PI_2)
	{
		Rz = ZE_PI_2;
	}

	ZEQuaternion::CreateFromEuler(OwnerCameraRotation, Rx, Ry, Rz);
	((ZEPlayer*)GetOwner())->GetCamera()->SetRotation(OwnerCameraRotation.Normalize());

	if(Moved)
	{
		Output.LinearAcceleration = MovementDirection.Normalize() * GetOwner()->GetMaxLinearAcceleration();
	}
	else
	{
		if (fabs(Output.LinearAcceleration.LengthSquare()) < 0.1f)
		{
			Output.LinearAcceleration = -GetOwner()->GetLinearVelocity() * Friction;
		}

		//Output.AngularAcceleration = ZEMath::Sign(GetOwner()->GetAngularVelocity()) * Friction * GetOwner()->GetMaxAngularAcceleration();

		return Output;
	}

	return Output;
}



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
	Camera->SetHorizontalFOV(FOV);
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
	ZEQuaternion RotationChange;

	InputMap.Update();

	ZEVector3 RayDirection, HitPosition, HitNormal;

	for (size_t I = 0; I < InputMap.GetActionCount(); I++)
	{
		const ZEInputAction* Current = &InputMap.GetActions()[I];
		switch(Current->Id)
		{
			case ACTIONID_CONSOLE:
				if (ZEConsole::GetInstance()->IsVisible())
					ZEConsole::GetInstance()->HideConsole();
				else
					ZEConsole::GetInstance()->ShowConsole();
				break;
		}
	}

	ZEActor::Tick(Time);
}

bool ZEPlayer::InitializeSelf()
{
	if (!ZEActor::InitializeSelf())
		return false;

	zeScene->SetActiveCamera(Camera);
	zeScene->SetActiveListener(Listener);

	return true;
}

bool ZEPlayer::DeinitializeSelf()
{
	return ZEActor::DeinitializeSelf();
}

#include "ZETexture/ZETexture2DResource.h"
ZEPlayer::ZEPlayer()
{

	FOV = ZEAngle::ToRadian(50);

	InputMap.AddButtonAction("Keyboard", ZE_IKB_GRAVE, ZE_IS_PRESSED, ACTIONID_CONSOLE);

	Camera = ZECamera::CreateInstance();
	Camera->SetPosition(ZEVector3(0.0f, 0.0f, 0.0f));
	Camera->SetRotation(ZEQuaternion::Identity);
	Camera->SetNearZ(1);
	Camera->SetFarZ(10000);
	Camera->SetHorizontalFOV(FOV);
	Camera->SetAutoAspectRatio(true);
	AddComponent(Camera);

	Listener = ZEListener::CreateInstance();
	AddComponent(Listener);
}

ZEPlayer::~ZEPlayer()
{
	Deinitialize();
}

ZEPlayer* ZEPlayer::CreateInstance()
{
	return new ZEPlayer();
}
