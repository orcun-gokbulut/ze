//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECharacter.cpp
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

#include "ZECharacter.h"

#include "ZECore/ZEConsole.h"
#include "ZEInput/ZEInputModule.h"
#include "ZEInput/ZEInputDefinitions.h"
#include "ZEModel/ZEModel.h"
#include "ZEGame/ZEGame.h"

#define ACTIONID_FORWARD		0
#define ACTIONID_BACKWARD		1
#define ACTIONID_RUN			2
#define ACTIONID_STRAFELEFT		3
#define ACTIONID_STRAFERIGHT	4
#define ACTIONID_TURNLEFT		5
#define ACTIONID_TURNRIGHT		6
#define ACTIONID_TURNUP			7
#define ACTIONID_TURNDOWN		8

ZEDrawFlags ZECharacter::GetDrawFlags()
{
	return ZE_DF_DRAW_COMPONENTS | ZE_DF_CULL_COMPONENTS;
}

void ZECharacter::Movement(float ElapsedTime)
{
	switch (MovementStatus)
	{
		case ZE_CMS_IDLE: 
			if (MovementRatio > 0.0f)
			{
				MovementRatio -= ElapsedTime / WalkTransitionTime;
				if (MovementRatio < 0.0f)
					MovementRatio = 0.0f;

				zeLog("Character", "Movement Status : %d, Movement Ratio : %f", MovementStatus, MovementRatio);
			}
			else if (MovementRatio < 0.0f)
			{
				MovementRatio += ElapsedTime / WalkTransitionTime;
				if (MovementRatio > 0.0f)
					MovementRatio = 0.0f;

				zeLog("Character", "Movement Status : %d, Movement Ratio : %f", MovementStatus, MovementRatio);		
			}
			break;

	case ZE_CMS_WALKING_FORWARD:
		if (MovementRatio > 1.0f)
		{
			MovementRatio -= ElapsedTime / WalkTransitionTime;
		}
		else
		{
			MovementRatio += ElapsedTime / WalkTransitionTime;
			if (MovementRatio > 1.0f)
				MovementRatio = 1.0f;
		}
		zeLog("Character", "Movement Status : %d, Movement Ratio : %f", MovementStatus, MovementRatio);
		break;

	case ZE_CMS_RUNNING_FORWARD:
		if (MovementRatio > 2.0f)
		{
			MovementRatio -= ElapsedTime / RunTransitionTime;
		}
		else
		{
			MovementRatio += ElapsedTime / RunTransitionTime;
			if (MovementRatio > 2.0f)
				MovementRatio = 2.0f;
		}

		zeLog("Character", "Movement Status : %d, Movement Ratio : %f", MovementStatus, MovementRatio);
		break;
	}

	float MovementSpeed = 0.0f;
	if (MovementRatio == 2.0f)
	{
		// Run
		if (RunAnimationTrack->GetState() == ZE_MAS_STOPPED)
			RunAnimationTrack->Play();

		RunAnimationTrack->SetBlendFactor(1.0f);
		IdleAnimationTrack->SetBlendFactor(0.0f);
		WalkAnimationTrack->SetBlendFactor(0.0f);

		MovementSpeed = RunSpeed;
	}
	else if (MovementRatio < 2.0f && MovementRatio > 1.0f)
	{
		// Run <-> Walk
		if (RunAnimationTrack->GetState() == ZE_MAS_STOPPED)
			RunAnimationTrack->Play();

		if (WalkAnimationTrack->GetState() == ZE_MAS_STOPPED)
			WalkAnimationTrack->Play();

		RunAnimationTrack->SetBlendFactor(MovementRatio - 1.0f);
		WalkAnimationTrack->SetBlendFactor(1.0f - (MovementRatio - 1.0f));
		IdleAnimationTrack->SetBlendFactor(0.0f);

		MovementSpeed = WalkSpeed + (RunSpeed - WalkSpeed) * (MovementRatio - 1.0f);
	}
	else if (MovementRatio == 1.0f)
	{
		// Walk
		if (WalkAnimationTrack->GetState() == ZE_MAS_STOPPED)
			WalkAnimationTrack->Play();

		if (IdleAnimationTrack->GetState() == ZE_MAS_STOPPED)
			IdleAnimationTrack->Play();

		RunAnimationTrack->SetBlendFactor(0.0f);
		WalkAnimationTrack->SetBlendFactor(MovementRatio);
		IdleAnimationTrack->SetBlendFactor(0.0f);

		IdleAnimationTrack->Resume();
		MovementSpeed = WalkSpeed;
	}
	else if (MovementRatio < 1.0f && MovementRatio > 0.0f)
	{
		// Walk <-> Idle
		if (WalkAnimationTrack->GetState() == ZE_MAS_STOPPED)
			WalkAnimationTrack->Play();

		RunAnimationTrack->SetBlendFactor(0.0f);
		WalkAnimationTrack->SetBlendFactor(MovementRatio);
		IdleAnimationTrack->SetBlendFactor(1.0f - MovementRatio);

		MovementSpeed = WalkSpeed * MovementRatio;
	}
	else if (MovementRatio == 0.0f)
	{
		// Idle
		if (IdleAnimationTrack->GetState() == ZE_MAS_STOPPED)
			IdleAnimationTrack->Play();
		IdleAnimationTrack->SetBlendFactor(1.0f);

		WalkAnimationTrack->SetBlendFactor(0.0f);
		WalkAnimationTrack->Stop();

		RunAnimationTrack->Stop();
		RunAnimationTrack->SetBlendFactor(0.0f);

		MovementSpeed = 0.0f;
	}
	else if (MovementRatio < 0.0f && MovementRatio > -1.0f)
	{
		// Idle <-> Backward
	}
	else if (MovementRatio == -1.0f)
	{
		// Walk Backward
	}
	else if (MovementRatio < -1.0f && MovementRatio > -2.0f)
	{
		// Idle <-> Backward
	}
	else if (MovementRatio == -2.0f)
	{
		// Run Backward
	}

	ZEQuaternion::VectorProduct(MovementVelocity, GetRotation(), ZEVector3::UnitZ);
	ZEVector3::Scale(MovementVelocity, MovementVelocity, MovementSpeed);
	SetPosition(GetPosition() + MovementVelocity * ElapsedTime);

	MovementStatus = ZE_CMS_IDLE;
}

void ZECharacter::Turning(float ElapsedTime)
{
	switch (TurnStatus)
	{
		case ZE_CTS_NO_TURN: 
			if (TurnRatio > 0.0f)
			{
				TurnRatio -= ElapsedTime / TurnTransitionTime;
				if (TurnRatio < 0.0f)
					TurnRatio = 0.0f;

				zeLog("Character", "Turn Status : %d, Movement Ratio : %f", TurnStatus, TurnRatio);
			}
			else if (TurnRatio < 0.0f)
			{
				TurnRatio += ElapsedTime / TurnTransitionTime;
				if (TurnRatio > 0.0f)
					TurnRatio = 0.0f;

				zeLog("Character", "Turn Status : %d, Movement Ratio : %f", TurnStatus, TurnRatio);		
			}
			break;

		case ZE_CTS_TURN_LEFT:
			TurnRatio -= ElapsedTime / TurnTransitionTime;
			if (TurnRatio < -1.0f)
				TurnRatio = -1.0f;

			zeLog("Character", "Turn Status : %d, Turn Ratio : %f", TurnStatus, TurnRatio);
			break;

		case ZE_CTS_TURN_RIGHT:
			TurnRatio += ElapsedTime / TurnTransitionTime;
			if (TurnRatio > 1.0f)
				TurnRatio = 1.0f;

			zeLog("Character", "Turn Status : %d, Turn Ratio : %f", TurnStatus, TurnRatio);
			break;
	}

	if (MovementRatio < 0.8f)
	{
		if (TurnRatio == 1.0f)
		{
			// Turn Right

			if (TurnRightAnimationTrack->GetState() == ZE_MAS_STOPPED)
				TurnRightAnimationTrack->Play();

			TurnRightAnimationTrack->SetBlendFactor(1.0f);
			TurnLeftAnimationTrack->SetBlendFactor(0.0f);

		}
		else if (TurnRatio < 1.0f && TurnRatio > 0.0f)
		{
			// Turn Right <-> Idle
			if (TurnRightAnimationTrack->GetState() == ZE_MAS_STOPPED)
				TurnRightAnimationTrack->Play();

			TurnRightAnimationTrack->SetBlendFactor(TurnRatio);
			TurnLeftAnimationTrack->SetBlendFactor(0.0f);
		}
		else if (TurnRatio == 0.0f)
		{
			// No Turning
			TurnLeftAnimationTrack->SetBlendFactor(0.0f);
			TurnLeftAnimationTrack->Stop();

			TurnRightAnimationTrack->SetBlendFactor(0.0f);
			TurnRightAnimationTrack->Stop();
		}
		else if (TurnRatio < 0.0f && TurnRatio > -1.0f)
		{
			// Turn Left <-> Idle
			if (TurnLeftAnimationTrack->GetState() == ZE_MAS_STOPPED)
				TurnLeftAnimationTrack->Play();

			TurnRightAnimationTrack->SetBlendFactor(0.0f);
			TurnLeftAnimationTrack->SetBlendFactor(-TurnRatio);
		}
		else if (TurnRatio == -1.0f)
		{
			// Turn Left
			if (TurnLeftAnimationTrack->GetState() == ZE_MAS_STOPPED)
				TurnLeftAnimationTrack->Play();

			TurnRightAnimationTrack->SetBlendFactor(0.0f);
			TurnLeftAnimationTrack->SetBlendFactor(1.0f);
		}
	}


	TurnAngle += TurnRatio * (MovementRatio < 1.0f ? TurnSpeed : MovementTurnSpeed) * ElapsedTime;

	if (TurnAngle > ZE_PIx2)
		TurnAngle = fmodf(TurnAngle, ZE_PIx2);
	
	if (TurnAngle < ZE_PIx2)
		TurnAngle = ZE_PIx2 - fmodf(TurnAngle, ZE_PIx2);

	ZEQuaternion Rotation(TurnAngle, ZEVector3(0.0f, 1.0f, 0.0f));
	SetRotation(Rotation);

	TurnStatus = ZE_CTS_NO_TURN;
}

void ZECharacter::Strafe(float ElapsedTime)
{
	switch (StrafeStatus)
	{
		case ZE_CSS_NO_STRAFE: 
			if (StrafeRatio > 0.0f)
			{
				StrafeRatio -= ElapsedTime / StrafeTransitionTime;
				if (StrafeRatio < 0.0f)
					StrafeRatio = 0.0f;

				zeLog("Character", "Strafe Status : %d, Movement Ratio : %f", StrafeStatus, StrafeRatio);
			}
			else if (StrafeRatio < 0.0f)
			{
				StrafeRatio += ElapsedTime / StrafeTransitionTime;
				if (StrafeRatio > 0.0f)
					StrafeRatio = 0.0f;

				zeLog("Character", "Strafe Status : %d, Movement Ratio : %f", StrafeStatus, StrafeRatio);		
			}
			break;

		case ZE_CSS_STRAFE_LEFT:
			StrafeRatio -= ElapsedTime / StrafeTransitionTime;
			if (StrafeRatio < -1.0f)
				StrafeRatio = -1.0f;

			zeLog("Character", "Strafe Status : %d, Strafe Ratio : %f", StrafeStatus, StrafeRatio);
			break;

		case ZE_CSS_STRAFE_RIGHT:
			StrafeRatio += ElapsedTime / StrafeTransitionTime;
			if (StrafeRatio > 1.0f)
				StrafeRatio = 1.0f;

			zeLog("Character", "Strafe Status : %d, Strafe Ratio : %f", StrafeStatus, StrafeRatio);
			break;
	}

	if (MovementRatio < 0.8f)
	{
		if (StrafeRatio == 1.0f)
		{
			// Strafe Right

			if (StrafeRightAnimationTrack->GetState() == ZE_MAS_STOPPED)
				StrafeRightAnimationTrack->Play();

			StrafeRightAnimationTrack->SetBlendFactor(1.0f);
			StrafeLeftAnimationTrack->SetBlendFactor(0.0f);

		}
		else if (StrafeRatio < 1.0f && StrafeRatio > 0.0f)
		{
			// Strafe Right <-> Idle
			if (StrafeRightAnimationTrack->GetState() == ZE_MAS_STOPPED)
				StrafeRightAnimationTrack->Play();

			StrafeRightAnimationTrack->SetBlendFactor(StrafeRatio);
			StrafeLeftAnimationTrack->SetBlendFactor(0.0f);
		}
		else if (StrafeRatio == 0.0f)
		{
			// No Strafeing
			StrafeLeftAnimationTrack->SetBlendFactor(0.0f);
			StrafeLeftAnimationTrack->Stop();

			StrafeRightAnimationTrack->SetBlendFactor(0.0f);
			StrafeRightAnimationTrack->Stop();
		}
		else if (StrafeRatio < 0.0f && StrafeRatio > -1.0f)
		{
			// Strafe Left <-> Idle
			if (StrafeLeftAnimationTrack->GetState() == ZE_MAS_STOPPED)
				StrafeLeftAnimationTrack->Play();

			StrafeRightAnimationTrack->SetBlendFactor(0.0f);
			StrafeLeftAnimationTrack->SetBlendFactor(-StrafeRatio);
		}
		else if (StrafeRatio == -1.0f)
		{
			// Strafe Left
			if (StrafeLeftAnimationTrack->GetState() == ZE_MAS_STOPPED)
				StrafeLeftAnimationTrack->Play();

			StrafeRightAnimationTrack->SetBlendFactor(0.0f);
			StrafeLeftAnimationTrack->SetBlendFactor(1.0f);
		}
	}


	ZEQuaternion::VectorProduct(StrafeVelocity, GetRotation(), ZEVector3::UnitX);
	ZEVector3::Scale(StrafeVelocity, StrafeVelocity, StrafeSpeed * StrafeRatio * ElapsedTime);
	SetPosition(GetPosition() + StrafeVelocity);

	StrafeStatus = ZE_CSS_NO_STRAFE;
}

void ZECharacter::Tick(float ElapsedTime)
{
	Movement(ElapsedTime);
	Strafe(ElapsedTime);
	Turning(ElapsedTime);

	Model->Tick(ElapsedTime);
	ZEEntity::Tick(ElapsedTime);


}

void ZECharacter::WalkForward()
{
	MovementStatus = ZE_CMS_WALKING_FORWARD;
}

void ZECharacter::RunForward()
{
	MovementStatus = ZE_CMS_RUNNING_FORWARD;
}

void ZECharacter::SprintForward()
{
	MovementStatus = ZE_CMS_SPRINT_FORWARD;
}

void ZECharacter::StrafeLeft()
{
	StrafeStatus = ZE_CSS_STRAFE_LEFT;
}

void ZECharacter::StrafeRight()
{
	StrafeStatus = ZE_CSS_STRAFE_RIGHT;
}

void ZECharacter::Stop()
{
	MovementStatus = ZE_CMS_IDLE;
}

void ZECharacter::TurnRight()
{
	TurnStatus = ZE_CTS_TURN_RIGHT;
}

void ZECharacter::TurnLeft()
{
	TurnStatus = ZE_CTS_TURN_LEFT;
}

bool ZECharacter::Initialize()
{
	if (Model == NULL)
		Model = new ZEModel();

	Model->SetModelResource(ZEModelResource::LoadResource("soldier.zemodel"));

	IdleAnimationTrack = Model->GetAnimationTracks().Add();
	IdleAnimationTrack->SetOwner(Model);
	IdleAnimationTrack->SetAnimationByName("Test");
	IdleAnimationTrack->SetSpeed(30.0f);
	IdleAnimationTrack->SetStartFrame(5);
	IdleAnimationTrack->SetEndFrame(105);
	IdleAnimationTrack->SetLooping(true);
	IdleAnimationTrack->SetBlendFactor(1.0f);
	//IdleAnimationTrack->Play();


	// Walk
	WalkAnimationTrack = Model->GetAnimationTracks().Add();
	WalkAnimationTrack->SetOwner(Model);
	WalkAnimationTrack->SetAnimationByName("Test");
	WalkAnimationTrack->SetSpeed(30.0f);
	WalkAnimationTrack->SetStartFrame(1960);
	WalkAnimationTrack->SetEndFrame(2000);
	WalkAnimationTrack->SetLooping(true);
	WalkAnimationTrack->SetBlendFactor(0.0f);
	//WalkAnimationTrack->Play();

	// Run
	RunAnimationTrack = Model->GetAnimationTracks().Add();
	RunAnimationTrack->SetOwner(Model);
	RunAnimationTrack->SetAnimationByName("Test");
	RunAnimationTrack->SetSpeed(30.0f);
	RunAnimationTrack->SetStartFrame(2050);
	RunAnimationTrack->SetEndFrame(2080);
	RunAnimationTrack->SetLooping(true);
	RunAnimationTrack->SetBlendFactor(0.0f);

	// Turn Left
	TurnLeftAnimationTrack = Model->GetAnimationTracks().Add();
	TurnLeftAnimationTrack->SetOwner(Model);
	TurnLeftAnimationTrack->SetAnimationByName("Test");
	TurnLeftAnimationTrack->SetSpeed(30.0f);
	TurnLeftAnimationTrack->SetStartFrame(2115);
	TurnLeftAnimationTrack->SetEndFrame(2145);
	TurnLeftAnimationTrack->SetLooping(true);
	TurnLeftAnimationTrack->SetBlendFactor(0.0f);

	// Turn Right
	TurnRightAnimationTrack = Model->GetAnimationTracks().Add();
	TurnRightAnimationTrack->SetOwner(Model);
	TurnRightAnimationTrack->SetAnimationByName("Test");
	TurnRightAnimationTrack->SetSpeed(30.0f);
	TurnRightAnimationTrack->SetStartFrame(2145);
	TurnRightAnimationTrack->SetEndFrame(2175);
	TurnRightAnimationTrack->SetLooping(true);
	TurnRightAnimationTrack->SetBlendFactor(0.0f);

	// Strafe Left
	StrafeLeftAnimationTrack = Model->GetAnimationTracks().Add();
	StrafeLeftAnimationTrack->SetOwner(Model);
	StrafeLeftAnimationTrack->SetAnimationByName("Test");
	StrafeLeftAnimationTrack->SetSpeed(30.0f);
	StrafeLeftAnimationTrack->SetStartFrame(2190);
	StrafeLeftAnimationTrack->SetEndFrame(2220);
	StrafeLeftAnimationTrack->SetLooping(true);
	StrafeLeftAnimationTrack->SetBlendFactor(0.0f);

	// Strafe Right
	StrafeRightAnimationTrack = Model->GetAnimationTracks().Add();
	StrafeRightAnimationTrack->SetOwner(Model);
	StrafeRightAnimationTrack->SetAnimationByName("Test");
	StrafeRightAnimationTrack->SetSpeed(30.0f);
	StrafeRightAnimationTrack->SetStartFrame(2225);
	StrafeRightAnimationTrack->SetEndFrame(2255);
	StrafeRightAnimationTrack->SetLooping(true);
	StrafeRightAnimationTrack->SetBlendFactor(0.0f);

	IdleAnimationTrack = &Model->GetAnimationTracks()[0];
	WalkAnimationTrack = &Model->GetAnimationTracks()[1];
	RunAnimationTrack = &Model->GetAnimationTracks()[2];
	TurnLeftAnimationTrack = &Model->GetAnimationTracks()[3];
	TurnRightAnimationTrack = &Model->GetAnimationTracks()[4];
	StrafeLeftAnimationTrack = &Model->GetAnimationTracks()[5];
	StrafeRightAnimationTrack = &Model->GetAnimationTracks()[6];

	Model->Initialize();

	RegisterComponent(Model);

	return true;
}

void ZECharacter::Deinitialize()
{
	Model->Destroy();
	Model = NULL;
}

ZECharacter::ZECharacter()
{
	Model = NULL;
	MovementVelocity = ZEVector3::Zero;
	MovementRatio = 0.0f;
	MovementStatus = ZE_CMS_IDLE;

	WalkTransitionTime = 0.3f;
	WalkSpeed = 1.0f;

	RunTransitionTime = 0.3f;
	RunSpeed = 3.0f;

	TurnStatus = ZE_CTS_NO_TURN;
	TurnAngle = 0.0f;
	TurnRatio = 0.0f;
	TurnTransitionTime = 0.3f;
	TurnSpeed = ZE_PI;
	MovementTurnSpeed = ZE_PI_2;

	StrafeRatio = 0.0f;
	StrafeSpeed = 1.0f;
	StrafeStatus = ZE_CSS_NO_STRAFE;
	StrafeTransitionTime = 0.3f;
}

ZECharacter::~ZECharacter()
{
	Deinitialize();
}

#include "ZECharacter.h.zpp"

ZEEntityRunAt ZECharacterDescription::GetRunAt() const
{
	return ZE_ERA_BOTH;
}
