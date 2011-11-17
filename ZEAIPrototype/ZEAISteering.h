//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEAISteering.h
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
#ifndef __ZE_AI_STEERING_H__
#define __ZE_AI_STEERING_H__

#include "ZEDS/ZEFlags.h"
#include "ZEMath/ZEVector.h"

typedef ZEFlags ZEAISteeringElements;
#define ZE_SE_STATIC		1
#define ZE_SE_KINEMATIC		2
#define ZE_SE_DYNAMIC		4

struct ZEAISteeringOutput
{
	ZEVector3						LinearAcceleration;
	float							AngularAcceleration;

	void							SetZero();
};

class ZEAIActor;

class ZEAISteering
{
	friend class ZEAIActor;
	private:
		ZEAIActor*					Owner;
		ZEAIActor*					Target;

		bool						Enabled;

	public:
		ZEAIActor*					GetOwner();

		bool						GetEnabled();
		void						SetEnabled(bool Enabled);

		ZEAIActor*					GetTarget();
		void						SetTarget(ZEAIActor* Target);

		virtual 
		ZEAISteeringOutput			Process(float ElapsedTime) = 0;
};

class ZEAISeekSteering : public ZEAISteering
{	
	protected:
		ZEAISteeringOutput			Seek(const ZEVector3& TargetPosition);

	public:
		virtual ZEAISteeringOutput	Process(float ElapsedTime);
};

class ZEAIFleeSteering : public ZEAISteering
{	
	protected:
		ZEAISteeringOutput			Flee(const ZEVector3& TargetPosition);

	public:
		virtual ZEAISteeringOutput	Process(float ElapsedTime);
};

class ZEAIArriveSteering : public ZEAISteering
{
	protected:
		ZEAISteeringOutput			Arrive(const ZEVector3& TargetPosition);

	public:
		float SlowRadius;
		float TimeToTarget;

		virtual ZEAISteeringOutput	Process(float ElapsedTime);

									ZEAIArriveSteering();
};

class ZEAIAlignSteering : public ZEAISteering
{
	protected:
		ZEAISteeringOutput			Align(float TargetRotation);

	public:
		float TargetRadius;
		float SlowRadius;
		float TimeToTarget;

		virtual ZEAISteeringOutput	Process(float ElapsedTime);

									ZEAIAlignSteering();
};

class ZEAIVelocityMatchingSteering : public ZEAISteering
{
	protected:
		ZEAISteeringOutput			MatchVelocity(const ZEVector3& TargetVelocity);

	public:
		float TimeToTarget;

		virtual ZEAISteeringOutput	Process(float ElapsedTime);

									ZEAIVelocityMatchingSteering();
};

class ZEAIFaceSteering : public ZEAIAlignSteering
{
	protected:
		ZEAISteeringOutput			Face(const ZEVector3& TargetDirection);

	public:
		virtual ZEAISteeringOutput	Process(float ElapsedTime);
};

class ZEAIFaceVelocitySteering : public ZEAIFaceSteering
{
	public:
		virtual ZEAISteeringOutput	Process(float ElapsedTime);
};

class ZEAIWanderSteering : public ZEAIFaceVelocitySteering
{
	public:
		float WanderRate;	
		float WanderRotation;
		float WanderRadius;
		float WanderOffset;

		virtual ZEAISteeringOutput	Process(float ElapsedTime);

		ZEAIWanderSteering();
};

#endif
