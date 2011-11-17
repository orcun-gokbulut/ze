//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEAISteering.cpp
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

#include "ZEAISteering.h"
#include "ZEAIActor.h"

#include "ZEMath/ZEAngle.h"

float frand(float Range);

void ZEAISteeringOutput::SetZero()
{
	LinearAcceleration = ZEVector3::Zero;
	AngularAcceleration = 0.0f;
}

ZEAIActor* ZEAISteering::GetOwner()
{
	return Owner;
}

bool ZEAISteering::GetEnabled()
{
	return Enabled;
}

void ZEAISteering::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

ZEAIActor* ZEAISteering::GetTarget()
{
	return Target;
}

void ZEAISteering::SetTarget(ZEAIActor* Target)
{
	this->Target = Target;
}

ZEAISteeringOutput ZEAISeekSteering::Seek(const ZEVector3& Target)
{
	ZEAISteeringOutput Output;
	Output.SetZero();

	Output.LinearAcceleration = Target - GetOwner()->GetPosition();
	if (Output.LinearAcceleration.LengthSquare() == 0)
		return Output;
	Output.LinearAcceleration.NormalizeSelf();
	Output.LinearAcceleration *= GetOwner()->GetMaxLinearAcceleration();

	return Output;
}

ZEAISteeringOutput ZEAISeekSteering::Process(float ElapsedTime)
{
	return Seek(GetTarget()->GetPosition());
}

ZEAISteeringOutput ZEAIFleeSteering::Flee(const ZEVector3& Target)
{
	ZEAISteeringOutput Output;
	Output.SetZero();

	Output.LinearAcceleration = GetOwner()->GetPosition() - Target;
	if (Output.LinearAcceleration.LengthSquare() == 0)
		return Output;

	Output.LinearAcceleration.NormalizeSelf();
	Output.LinearAcceleration *= GetOwner()->GetMaxLinearAcceleration();

	return Output;
}

ZEAISteeringOutput ZEAIFleeSteering::Process(float ElapsedTime)
{
	return Flee(GetTarget()->GetPosition());
}

ZEAISteeringOutput ZEAIArriveSteering::Arrive(const ZEVector3& Target)
{
	ZEAISteeringOutput Output;
	Output.SetZero();

	ZEVector3 Direction = Target - GetOwner()->GetPosition();
	float Distance = Direction.Length();

	if (Distance == 0)
		return Output;

	float TargetSpeed;
	if (Distance > SlowRadius)
		TargetSpeed = GetOwner()->GetMaxLinearSpeed();
	else
		TargetSpeed = GetOwner()->GetMaxLinearSpeed() * Distance / SlowRadius;

	ZEVector3 TargetVelocity = Direction / Distance;
	TargetVelocity *= TargetSpeed;

	Output.LinearAcceleration = TargetVelocity - GetOwner()->GetLinearVelocity();
	Output.LinearAcceleration /= TimeToTarget;

	if (Output.LinearAcceleration.LengthSquare() > GetOwner()->GetMaxLinearAcceleration() * GetOwner()->GetMaxLinearAcceleration())
	{
		Output.LinearAcceleration.NormalizeSelf();
		Output.LinearAcceleration *= GetOwner()->GetMaxLinearAcceleration();
	}

	return Output;
}

ZEAISteeringOutput ZEAIArriveSteering::Process(float ElapsedTime)
{
	return Arrive(GetTarget()->GetPosition());
}

ZEAIArriveSteering::ZEAIArriveSteering()
{
	TimeToTarget = 0.1f;
	SlowRadius = 10.0f;
}

ZEAISteeringOutput ZEAIAlignSteering::Align(float Target_Rotation)
{
	ZEAISteeringOutput Output;
	Output.SetZero();

	float Rotation = ZEAngle::Radian::Range(Target_Rotation - GetOwner()->GetRotation());
	float RotationSize = abs(Rotation);
	
	if (RotationSize == 0)
		return Output;

	float TargetRotation;
	if (RotationSize > SlowRadius)
		TargetRotation = GetOwner()->GetMaxAngularAcceleration();
	else
		TargetRotation = GetOwner()->GetMaxAngularAcceleration() * RotationSize / SlowRadius;
	TargetRotation *= Rotation / RotationSize;

	Output.AngularAcceleration = TargetRotation - GetOwner()->GetAngularVelocity();
	Output.AngularAcceleration /= TimeToTarget;

	if (abs(Output.AngularAcceleration) > GetOwner()->GetMaxAngularAcceleration())
	{
		Output.AngularAcceleration /= abs(Output.AngularAcceleration);
		Output.AngularAcceleration *= GetOwner()->GetMaxAngularAcceleration();
	}

	return Output;
}

ZEAISteeringOutput ZEAIAlignSteering::Process(float ElapsedTime)
{
	return Align(GetTarget()->GetRotation());
}

ZEAIAlignSteering::ZEAIAlignSteering()
{
	TargetRadius = ZE_PI_12;
	SlowRadius = ZE_PI_8;
	TimeToTarget = 0.1f;
}

ZEAISteeringOutput ZEAIVelocityMatchingSteering::MatchVelocity(const ZEVector3& TargetVelocity)
{
	ZEAISteeringOutput Output;
	Output.SetZero();

	Output.LinearAcceleration = TargetVelocity - GetOwner()->GetLinearVelocity();
	Output.LinearAcceleration /= TimeToTarget;

	if (Output.LinearAcceleration.LengthSquare() > GetOwner()->GetMaxLinearAcceleration() * GetOwner()->GetMaxLinearAcceleration())
	{
		Output.LinearAcceleration.NormalizeSelf();
		Output.LinearAcceleration *= GetOwner()->GetMaxLinearAcceleration();
	}

	return Output;
}

ZEAISteeringOutput ZEAIVelocityMatchingSteering::Process(float ElapsedTime)
{
	return MatchVelocity(GetTarget()->GetLinearVelocity());
}

ZEAIVelocityMatchingSteering::ZEAIVelocityMatchingSteering()
{
	TimeToTarget = 0.1f;
}

ZEAISteeringOutput ZEAIFaceSteering::Face(const ZEVector3& TargetDirection)
{
	if (TargetDirection.LengthSquare() == 0)
	{
		ZEAISteeringOutput Output;
		Output.SetZero();
		return Output;
	}

	return Align(atan2(TargetDirection.x, TargetDirection.y));
}

ZEAISteeringOutput ZEAIFaceSteering::Process(float ElapsedTime)
{
	return Face(GetTarget()->GetPosition() - GetOwner()->GetPosition());
}

ZEAISteeringOutput ZEAIFaceVelocitySteering::Process(float ElapsedTime)
{
	return Face(GetOwner()->GetLinearVelocity());
}

ZEAISteeringOutput ZEAIWanderSteering::Process(float ElapsedTime)
{
	ZEAISteeringOutput Output;

	WanderRotation += frand(ZE_PI) * WanderRate;
	ZEVector2 WanderDirection = ZEAngle::Radian::ToVector(WanderRotation);
	
	Output = Face(GetOwner()->GetLinearVelocity());
	Output.LinearAcceleration = ZEVector3(WanderDirection, 0.0f) *  GetOwner()->GetMaxLinearAcceleration();

	return Output;
}

ZEAIWanderSteering::ZEAIWanderSteering()
{
	WanderRate = 0.5f;
	WanderRotation = 0.0f;
}
