//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESteering.h
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
#ifndef __ZE_STEERING_H__
#define __ZE_STEERING_H__

#include "ZETypes.h"
#include "ZEDS/ZEFlags.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEVector.h"



typedef ZEFlags ZESteeringElements;
#define ZE_SE_STATIC		1
#define ZE_SE_KINEMATIC		2
#define ZE_SE_DYNAMIC		4

struct ZESteeringOutput
{
	ZEVector3						LinearAcceleration;
	float							AngularAcceleration;

	void							SetZero();
};

class ZEActor;

class ZESteering
{
	friend class ZEActor;
	private:
		ZEActor*					Owner;
		ZEActor*					Target;

		ZEInt							Priority;
		bool						Enabled;

		float						Weight;

	public:
		ZEActor*					GetOwner();
		virtual void				SetOwner(ZEActor*	Owner);

		ZEUInt				GetPriority();
		void						SetPriority(ZEUInt Priority);

		float						GetWeight();
		virtual void				SetWeight(float Weight);

		bool						GetEnabled();
		virtual void				SetEnabled(bool Enabled);

		ZEActor*					GetTarget();
		virtual void				SetTarget(ZEActor* Target);

		virtual 
		ZESteeringOutput			Process(float ElapsedTime) = 0;
		
									ZESteering();
		virtual						~ZESteering();
};

class ZESeekSteering : public ZESteering
{	
	public:
		ZESteeringOutput			Seek(const ZEVector3& TargetPosition);
		virtual ZESteeringOutput	Process(float ElapsedTime);

		ZESeekSteering();
};

class ZEFleeSteering : public ZESteering
{	
	public:
		ZESteeringOutput			Flee(const ZEVector3& TargetPosition);
		virtual ZESteeringOutput	Process(float ElapsedTime);

									ZEFleeSteering();
};

class ZEArriveSteering : public ZESteering
{
	public:
		float SlowRadius;
		float TimeToTarget;

		ZESteeringOutput			Arrive(const ZEVector3& TargetPosition);
		virtual ZESteeringOutput	Process(float ElapsedTime);

									ZEArriveSteering();
};

class ZEAlignSteering : public ZESteering
{
	public:
		float TargetRadius;
		float SlowRadius;
		float TimeToTarget;

		ZESteeringOutput			Align(float TargetRotation);
		virtual ZESteeringOutput	Process(float ElapsedTime);

									ZEAlignSteering();
};

class ZEVelocityMatchingSteering : public ZESteering
{
	public:
		float TimeToTarget;

		ZESteeringOutput			MatchVelocity(const ZEVector3& TargetVelocity);
		virtual ZESteeringOutput	Process(float ElapsedTime);

									ZEVelocityMatchingSteering();
};

class ZEFaceSteering : public ZEAlignSteering
{
	public:
		ZESteeringOutput			Face(const ZEVector3& TargetDirection);
		virtual ZESteeringOutput	Process(float ElapsedTime);

									ZEFaceSteering();
};

class ZEFaceVelocitySteering : public ZEFaceSteering
{
	public:
		virtual ZESteeringOutput	Process(float ElapsedTime);

									ZEFaceVelocitySteering();
};

class ZEWanderSteering : public ZEFaceVelocitySteering
{
	public:
		float WanderRate;	
		float WanderRotation;
		float WanderRadius;
		float WanderOffset;

		virtual ZESteeringOutput	Process(float ElapsedTime);

									ZEWanderSteering();
};

class ZEPathFollowingSteering : public ZESteering
{
	public:
		ZESeekSteering				Seek;
		ZEArriveSteering			Arrive;
		ZEFaceVelocitySteering		Face;

		ZEArray<ZEVector3>			PathNodes;
		float						PathNodeRadius;
		ZESize						CurrentPathNode;

		virtual void				SetOwner(ZEActor*	Owner);
		virtual ZESteeringOutput	Process(float ElapsedTime);

									ZEPathFollowingSteering();
};


class ZESeperateSteering : public ZESteering
{
	public:
		ZEArray<ZEActor*>			AvoidedActors;
		float						Treshold;
		float						DecayCoefficient;

		virtual ZESteeringOutput	Process(float ElapsedTime);

									ZESeperateSteering();
};

class ZEFrictionSteering : public ZESteering
{
	public:
		float						Friction;

		virtual ZESteeringOutput	Process(float ElapsedTime);

									ZEFrictionSteering();
};

class ZECollisionAvoidanceSteering : public ZESteering
{
	public:
		ZEArray<ZEActor*>			AvoidedActors;

		virtual ZESteeringOutput	Process(float ElapsedTime);

									ZECollisionAvoidanceSteering();
};

#endif
