//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECharacter.h
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

#pragma once
#ifndef	__ZE_CHARACTER_H__
#define __ZE_CHARACTER_H__
 
#include "ZECompoundEntity.h"
#include "ZEInput/ZEInputMap.h"
#include <stdio.h>

class ZEModel;
class ZEModelAnimationTrack;

ZE_META_ENTITY_DESCRIPTION(ZECharacter);

enum ZECharacterMovementStatus
{
	ZE_CMS_IDLE,
	ZE_CMS_WALKING_FORWARD,
	ZE_CMS_RUNNING_FORWARD,
	ZE_CMS_SPRINT_FORWARD,
	ZE_CMS_WALKING_BACKWARD,
	ZE_CMS_RUNNING_BACKWARD
};

enum ZECharacterTurnStatus
{
	ZE_CTS_NO_TURN,
	ZE_CTS_TURN_RIGHT,
	ZE_CTS_TURN_LEFT
};

enum ZECharacterStrafeStatus
{
	ZE_CSS_NO_STRAFE,
	ZE_CSS_STRAFE_RIGHT,
	ZE_CSS_STRAFE_LEFT
};

struct ZECharacterRecordingKey
{
	float Time;
	ZEDWORD Event;
	ZEVector3 Position;
	ZEQuaternion Rotation;
	float TurnAngle;
};

enum ZECharacterRecordingStatus
{
	ZE_CRS_STOPPED,
	ZE_CRS_RECORDING,
	ZE_CRS_PLAYING
};

class ZEProjectiveLight;

class ZECharacter : public ZECompoundEntity
{
	ZE_META_ENTITY(ZECharacter)
	private:
		ZEChunkArray<ZECharacterRecordingKey, 29127> 
									Records;
		ZECharacterRecordingStatus	RecordingStatus;
		float						RecordingTime;
		int							RecordingFrame;

		ZEModel*					Model;
		ZEInputMap					InputMap;

		ZEModelAnimationTrack*		IdleAnimationTrack;


		ZEVector3					MovementVelocity;
		float						MovementRatio;
		ZECharacterMovementStatus	MovementStatus;

		ZEModelAnimationTrack*		WalkAnimationTrack;
		float						WalkTransitionTime;
		float						WalkSpeed;

		ZEModelAnimationTrack*		WalkBackwardAnimationTrack;
		float						WalkBackwardTransitionTime;
		float						WalkBackwardSpeed;

		ZEModelAnimationTrack*		RunAnimationTrack;
		float						RunTransitionTime;
		float						RunSpeed;

		ZEProjectiveLight*			FlashLight;

	protected:

									ZECharacter();
									~ZECharacter();

	public:
		float						TurnAngle;
		float						TurnRatio;
		ZECharacterTurnStatus		TurnStatus;
		float						TurnTransitionTime;
		float						TurnSpeed;
		float						MovementTurnSpeed;

		ZEModelAnimationTrack*		TurnRightAnimationTrack;
		ZEModelAnimationTrack*		TurnLeftAnimationTrack;


		ZEVector3					StrafeVelocity;
		float						StrafeRatio;
		float						StrafeSpeed;
		ZECharacterStrafeStatus		StrafeStatus;
		float						StrafeTransitionTime;

		ZEModelAnimationTrack*		StrafeRightAnimationTrack;
		ZEModelAnimationTrack*		StrafeLeftAnimationTrack;

		void						Movement(float ElapsedTime);
		void						Strafe(float ElapsedTime);
		void						Turning(float ElapsedTime);
		void						RecordEvent(ZEDWORD Event);
	public:
		void						AdvanceRecording(float ElapsedTime);


		virtual ZEDrawFlags			GetDrawFlags();

		virtual bool				Initialize();
		virtual void				Deinitialize();

		void						WalkForward();
		void						RunForward();
		void						WalkBackward();
		void						SprintForward();
		void						StrafeRight();
		void						StrafeLeft();
		void						TurnRight();
		void						TurnLeft();
		void						Stop();
		
		void						StartRecording();
		void						StopRecording();
		void						PlayRecording();
		void						SaveRecording(const char* FileName);
		void						LoadRecording(const char* FileName);

		virtual void				Tick(float ElapsedTime);

		static ZECharacter*			CreateInstance();

};

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta>
		<class name="ZECharacter" parent="ZECompoundEntity" description="Player spawn point"/>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/
#endif
