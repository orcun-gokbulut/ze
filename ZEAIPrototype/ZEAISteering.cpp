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

#include "ZEMath/ZEMath.h"
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

void ZEAISteering::SetOwner(ZEAIActor*	Owner)
{
	this->Owner = Owner;
}

unsigned int ZEAISteering::GetPriority()
{
	return Priority;
}

void ZEAISteering::SetPriority(unsigned int Priority)
{
	this->Priority = Priority;
}

float ZEAISteering::GetWeight()
{
	return Weight;
}

void ZEAISteering::SetWeight(float Weight)
{
	this->Weight = Weight;
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

ZEAISteering::ZEAISteering()
{
	Target = NULL;
	Owner = NULL;

	Weight = 1.0f;
	Priority = 3;
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

ZEAISeekSteering::ZEAISeekSteering()
{
	SetPriority(4);
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

ZEAIFleeSteering::ZEAIFleeSteering()
{
	SetPriority(3);
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
	SetPriority(3);
}

ZEAISteeringOutput ZEAIAlignSteering::Align(float Target_Rotation)
{
	ZEAISteeringOutput Output;
	Output.SetZero();

	float Rotation = ZEAngle::Range(Target_Rotation - GetOwner()->GetRotation());
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
	SetPriority(3);
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
	SetPriority(3);
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

ZEAIFaceSteering::ZEAIFaceSteering()
{
	SetPriority(3);
}

ZEAISteeringOutput ZEAIFaceVelocitySteering::Process(float ElapsedTime)
{
	return Face(GetOwner()->GetLinearVelocity());
}

ZEAIFaceVelocitySteering::ZEAIFaceVelocitySteering()
{
	SetPriority(3);
}

ZEAISteeringOutput ZEAIWanderSteering::Process(float ElapsedTime)
{
	ZEAISteeringOutput Output;

	WanderRotation += frand(ZE_PI) * WanderRate * ElapsedTime;
	ZEVector2 WanderDirection = ZEAngle::ToVector(WanderRotation);
	
	Output = Face(GetOwner()->GetLinearVelocity());
	Output.LinearAcceleration = ZEVector3(WanderDirection, 0.0f) *  GetOwner()->GetMaxLinearAcceleration();

	return Output;
}

ZEAIWanderSteering::ZEAIWanderSteering()
{
	SetPriority(4);
	WanderRate = 0.5f;
	WanderRotation = 0.0f;
}

void ZEAIPathFollowingSteering::SetOwner(ZEAIActor* Owner)
{
	Seek.SetOwner(Owner);
	Arrive.SetOwner(Owner);
	Face.SetOwner(Owner);

	ZEAISteering::SetOwner(Owner);
}
ZEAISteeringOutput ZEAIPathFollowingSteering::Process(float ElapsedTime)
{
	if ((PathNodes[CurrentPathNode] - GetOwner()->GetPosition()).LengthSquare() < PathNodeRadius * PathNodeRadius)
	{
		CurrentPathNode++;

		if (PathNodes.GetCount() == CurrentPathNode)
			CurrentPathNode--;
	}

	ZEAISteeringOutput Output;
	if (CurrentPathNode + 1 == PathNodes.GetCount())
		Output = Arrive.Arrive(PathNodes[CurrentPathNode]);
	else
		Output = Seek.Seek(PathNodes[CurrentPathNode]);

	ZEAISteeringOutput FaceOutput = Face.Process(ElapsedTime);
	Output.AngularAcceleration = FaceOutput.AngularAcceleration;

	return Output;
}

ZEAIPathFollowingSteering::ZEAIPathFollowingSteering()
{
	CurrentPathNode = 0;
	PathNodeRadius = 3.0f;
	SetPriority(3);
}

ZEAISteeringOutput ZEAISeperateSteering::Process(float ElapsedTime)
{
	ZEAISteeringOutput Output;
	Output.SetZero();

	if (GetTarget() != NULL)
	{
		ZEVector3 Direction = GetOwner()->GetPosition() - GetTarget()->GetPosition();
		float Distance = Direction.Length();

		if (Distance < Treshold + GetOwner()->GetRadius() + GetTarget()->GetRadius())
		{
			Direction /= Distance + 0.0001;

			Distance -= GetOwner()->GetRadius() + GetTarget()->GetRadius();
			if (Distance < 0.0f)
				Distance = 0.0001;

			Output.LinearAcceleration = Direction * ZEMath::Min(DecayCoefficient / (Distance * Distance), GetOwner()->GetMaxLinearAcceleration());
		}
	}

	for (size_t I = 0; I < AvoidedActors.GetCount(); I++)
	{
		ZEVector3 Direction = GetOwner()->GetPosition() - AvoidedActors[I]->GetPosition();
		float Distance = Direction.Length();
		
		if (Distance < Treshold + GetOwner()->GetRadius() + AvoidedActors[I]->GetRadius())
		{
			Direction /= Distance + 0.0001;
			
			Distance -= GetOwner()->GetRadius() + AvoidedActors[I]->GetRadius();
			if (Distance < 0.0f)
				Distance = 0.0001;

			Output.LinearAcceleration = Direction * ZEMath::Min(DecayCoefficient / (Distance * Distance), GetOwner()->GetMaxLinearAcceleration());
		}
	}

	if (Output.LinearAcceleration.LengthSquare() > GetOwner()->GetMaxLinearAcceleration() * GetOwner()->GetMaxLinearAcceleration())
	{
		Output.LinearAcceleration.NormalizeSelf();
		Output.LinearAcceleration *= GetOwner()->GetMaxLinearAcceleration();
	}

	return Output;
}

ZEAISeperateSteering::ZEAISeperateSteering()
{
	SetPriority(1);
	Treshold = 4.0f;
	DecayCoefficient = 1.0f;
	SetPriority(2);
}

ZEAISteeringOutput ZEAIFrictionSteering::Process(float ElapsedTime)
{
	ZEAISteeringOutput Output;

	if (GetOwner()->GetLinearVelocity().LengthSquare() != 0)
		Output.LinearAcceleration = -GetOwner()->GetLinearVelocity().Normalize() * Friction * GetOwner()->GetMaxLinearAcceleration();

	Output.AngularAcceleration = ZEMath::Sign(GetOwner()->GetAngularVelocity()) * Friction * GetOwner()->GetMaxAngularAcceleration();
	
	return Output;
}

ZEAIFrictionSteering::ZEAIFrictionSteering()
{
	Friction = 0.1;
	SetPriority(4);
}

ZEAISteeringOutput ZEAICollisionAvoidanceSteering::Process(float ElapsedTime)
{
	ZEAISteeringOutput Output;
	Output.SetZero();

	float ShortestTime = ZE_FLOAT_MAX;
	
	ZEAIActor* FirstTarget = NULL;
	float FirstMinSeperation;
	float FirstDistance;
	ZEVector3 FirstRelativePosition;
	ZEVector3 FirstRelativeVelocity;

	for (size_t I = 0; I < AvoidedActors.GetCount(); I++)
	{
		ZEAIActor* CurrentTarget = AvoidedActors[I];

		ZEVector3 RelativePosition = CurrentTarget->GetPosition() - GetOwner()->GetPosition();
		ZEVector3 RelativeVelocity = CurrentTarget->GetLinearVelocity() - GetOwner()->GetLinearVelocity();
		float RelativeSpeed = RelativeVelocity.Length();

		float TimeToCollision = (ZEVector3::DotProduct(RelativePosition, RelativeVelocity) * RelativeSpeed * RelativeSpeed);

		float Distance = RelativePosition.Length();
		float MinSeparation = Distance - RelativeSpeed * ShortestTime;

		if (MinSeparation > GetOwner()->GetRadius() + CurrentTarget->GetRadius())
			continue;

		if (TimeToCollision > 0.0f && TimeToCollision < ShortestTime)
		{
			ShortestTime = TimeToCollision;
			FirstTarget = CurrentTarget;
			FirstMinSeperation = MinSeparation;
			FirstDistance = Distance;
			FirstRelativePosition = RelativePosition;
			FirstRelativeVelocity = RelativeVelocity;
		}
	}

	if (FirstTarget == NULL)
		return Output;

	if (FirstMinSeperation <= 0 || FirstDistance < GetOwner()->GetRadius() + FirstTarget->GetRadius())
		Output.LinearAcceleration = FirstTarget->GetPosition() - GetOwner()->GetPosition();
	else
		Output.LinearAcceleration = FirstRelativePosition + FirstRelativeVelocity * ShortestTime;

	Output.LinearAcceleration.NormalizeSelf();
	Output.LinearAcceleration = Output.LinearAcceleration * GetOwner()->GetMaxLinearAcceleration();

	return Output;
}

ZEAICollisionAvoidanceSteering::ZEAICollisionAvoidanceSteering()
{
	SetPriority(1);
}
