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

ZEAISteeringOutput ZEAISeekSteering::Process(float ElapsedTime)
{
	ZEAISteeringOutput Output;

	Output.LinearAcceleration = GetTarget()->GetPosition() - GetOwner()->GetPosition();
	Output.LinearAcceleration.NormalizeSelf();
	Output.LinearAcceleration *= GetOwner()->GetMaxLinearAcceleration();
	Output.AngularAcceleration = 0.0f;

	return Output;
}

ZEAISteeringOutput ZEAIFleeSteering::Process(float ElapsedTime)
{
	ZEAISteeringOutput Output;

	Output.LinearAcceleration = GetOwner()->GetPosition() - GetTarget()->GetPosition();
	Output.LinearAcceleration.NormalizeSelf();
	Output.LinearAcceleration *= GetOwner()->GetMaxLinearAcceleration();
	Output.AngularAcceleration = 0.0f;

	return Output;
}


ZEAISteeringOutput ZEAIArriveSteering::Process(float ElapsedTime)
{
	ZEAISteeringOutput Output;
	Output.SetZero();

	ZEVector3 Direction = GetTarget()->GetPosition() - GetOwner()->GetPosition();
	float Distance = Direction.Length();

	float TargetSpeed;
	if (Distance > SlowRadius)
		TargetSpeed = GetOwner()->GetMaxLinearSpeed();
	else
		TargetSpeed = GetOwner()->GetMaxLinearSpeed() * Distance / SlowRadius;

	if (Distance == 0)
		return Output;

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

ZEAIArriveSteering::ZEAIArriveSteering()
{
	TimeToTarget = 0.1f;
	SlowRadius = 10.0f;
}

ZEAISteeringOutput ZEAIAlignSteering::Process(float ElapsedTime)
{
	ZEAISteeringOutput Output;
	Output.SetZero();

	float Rotation = ZEAngle::RangeRadian(GetTarget()->GetRotation() - GetOwner()->GetRotation());
	float RotationSize = abs(Rotation);

	/*if (RotationSize < TargetRadius)
		return Output;*/

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

ZEAIAlignSteering::ZEAIAlignSteering()
{
	TargetRadius = ZE_PI_12;
	SlowRadius = ZE_PI_8;
	TimeToTarget = 0.1f;
}

