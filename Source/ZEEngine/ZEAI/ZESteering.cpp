//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESteering.cpp
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

#include "ZESteering.h"
#include "ZEActor.h"

#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEAngle.h"
#include "ZEInput/ZEInputDefinitions.h"
#include "ZEInput/ZEInputModule.h"
#include "ZERandom.h"

void ZESteeringOutput::SetZero()
{
	LinearAcceleration = ZEVector3::Zero;
	AngularAcceleration = 0.0f;
}

ZEActor* ZESteering::GetOwner()
{
	return Owner;
}

void ZESteering::SetOwner(ZEActor*	Owner)
{
	this->Owner = Owner;
}

ZEUInt ZESteering::GetPriority()
{
	return Priority;
}

void ZESteering::SetPriority(ZEUInt Priority)
{
	this->Priority = Priority;
}

float ZESteering::GetWeight()
{
	return Weight;
}

void ZESteering::SetWeight(float Weight)
{
	this->Weight = Weight;
}

bool ZESteering::GetEnabled()
{
	return Enabled;
}

void ZESteering::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

ZEActor* ZESteering::GetTarget()
{
	return Target;
}

void ZESteering::SetTarget(ZEActor* Target)
{
	this->Target = Target;
}

ZESteering::ZESteering()
{
	Target = NULL;
	Owner = NULL;

	Weight = 1.0f;
	Priority = 3;
	Enabled = true;
}

ZESteering::~ZESteering()
{

}

ZESteeringOutput ZESeekSteering::Seek(const ZEVector3& Target)
{
	ZESteeringOutput Output;
	Output.SetZero();

	Output.LinearAcceleration = Target - GetOwner()->GetPosition();
	if (Output.LinearAcceleration.LengthSquare() == 0)
		return Output;
	Output.LinearAcceleration.NormalizeSelf();
	Output.LinearAcceleration *= GetOwner()->GetMaxLinearAcceleration();

	return Output;
}

ZESteeringOutput ZESeekSteering::Process(float ElapsedTime)
{
	return Seek(GetTarget()->GetPosition());
}

ZESeekSteering::ZESeekSteering()
{
	SetPriority(4);
}


ZESteeringOutput ZEFleeSteering::Flee(const ZEVector3& Target)
{
	ZESteeringOutput Output;
	Output.SetZero();

	Output.LinearAcceleration = GetOwner()->GetPosition() - Target;
	if (Output.LinearAcceleration.LengthSquare() == 0)
		return Output;

	Output.LinearAcceleration.NormalizeSelf();
	Output.LinearAcceleration *= GetOwner()->GetMaxLinearAcceleration();

	return Output;
}

ZESteeringOutput ZEFleeSteering::Process(float ElapsedTime)
{
	return Flee(GetTarget()->GetPosition());
}

ZEFleeSteering::ZEFleeSteering()
{
	SetPriority(3);
}

ZESteeringOutput ZEArriveSteering::Arrive(const ZEVector3& Target)
{
	ZESteeringOutput Output;
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

ZESteeringOutput ZEArriveSteering::Process(float ElapsedTime)
{
	return Arrive(GetTarget()->GetPosition());
}

ZEArriveSteering::ZEArriveSteering()
{
	TimeToTarget = 0.1f;
	SlowRadius = 10.0f;
	SetPriority(3);
}

ZESteeringOutput ZEAlignSteering::Align(float Target_Rotation)
{
	ZESteeringOutput Output;
	Output.SetZero();

	float Rotation = ZEAngle::Range(Target_Rotation - GetOwner()->GetRotation2D());
	float RotationSize = ZEMath::Abs(Rotation);
	
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

	if (ZEMath::Abs(Output.AngularAcceleration) > GetOwner()->GetMaxAngularAcceleration())
	{
		Output.AngularAcceleration /= ZEMath::Abs(Output.AngularAcceleration);
		Output.AngularAcceleration *= GetOwner()->GetMaxAngularAcceleration();
	}

	return Output;
}

ZESteeringOutput ZEAlignSteering::Process(float ElapsedTime)
{
	return Align(GetTarget()->GetRotation2D());
}

ZEAlignSteering::ZEAlignSteering()
{
	TargetRadius = ZE_PI_12;
	SlowRadius = ZE_PI_8;
	TimeToTarget = 0.1f;
	SetPriority(3);
}

ZESteeringOutput ZEVelocityMatchingSteering::MatchVelocity(const ZEVector3& TargetVelocity)
{
	ZESteeringOutput Output;
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

ZESteeringOutput ZEVelocityMatchingSteering::Process(float ElapsedTime)
{
	return MatchVelocity(GetTarget()->GetLinearVelocity());
}

ZEVelocityMatchingSteering::ZEVelocityMatchingSteering()
{
	SetPriority(3);
	TimeToTarget = 0.1f;
}

ZESteeringOutput ZEFaceSteering::Face(const ZEVector3& TargetDirection)
{
	if (TargetDirection.LengthSquare() == 0)
	{
		ZESteeringOutput Output;
		Output.SetZero();
		return Output;
	}

	return Align(ZEAngle::ArcTan2(TargetDirection.x, TargetDirection.z));
}

ZESteeringOutput ZEFaceSteering::Process(float ElapsedTime)
{
	return Face(GetTarget()->GetPosition() - GetOwner()->GetPosition());
}

ZEFaceSteering::ZEFaceSteering()
{
	SetPriority(3);
}

ZESteeringOutput ZEFaceVelocitySteering::Process(float ElapsedTime)
{
	return Face(GetOwner()->GetLinearVelocity());
}

ZEFaceVelocitySteering::ZEFaceVelocitySteering()
{
	SetPriority(3);
}

ZESteeringOutput ZEWanderSteering::Process(float ElapsedTime)
{
	ZESteeringOutput Output;

	WanderRotation += ZERandom::GetFloat() * ZE_PI * WanderRate * ElapsedTime;
	ZEVector2 WanderDirection = ZEAngle::ToVector(WanderRotation);
	
	Output = Face(GetOwner()->GetLinearVelocity());
	Output.LinearAcceleration = ZEVector3(WanderDirection, 0.0f) *  GetOwner()->GetMaxLinearAcceleration();

	return Output;
}

ZEWanderSteering::ZEWanderSteering()
{
	SetPriority(4);
	WanderRate = 0.5f;
	WanderRotation = 0.0f;
}

void ZEPathFollowingSteering::SetOwner(ZEActor* Owner)
{
	Seek.SetOwner(Owner);
	Arrive.SetOwner(Owner);
	Face.SetOwner(Owner);

	ZESteering::SetOwner(Owner);
}
ZESteeringOutput ZEPathFollowingSteering::Process(float ElapsedTime)
{
	if ((PathNodes[CurrentPathNode] - GetOwner()->GetPosition()).LengthSquare() < PathNodeRadius * PathNodeRadius)
	{
		CurrentPathNode++;

		if (PathNodes.GetCount() == CurrentPathNode)
			CurrentPathNode--;
	}

	ZESteeringOutput Output;
	if (CurrentPathNode + 1 == PathNodes.GetCount())
		Output = Arrive.Arrive(PathNodes[CurrentPathNode]);
	else
		Output = Seek.Seek(PathNodes[CurrentPathNode]);

	ZESteeringOutput FaceOutput = Face.Process(ElapsedTime);
	Output.AngularAcceleration = FaceOutput.AngularAcceleration;

	return Output;
}

ZEPathFollowingSteering::ZEPathFollowingSteering()
{
	CurrentPathNode = 0;
	PathNodeRadius = 3.0f;
	SetPriority(3);
}

ZESteeringOutput ZESeperateSteering::Process(float ElapsedTime)
{
	ZESteeringOutput Output;
	Output.SetZero();

	if (GetTarget() != NULL)
	{
		ZEVector3 Direction = GetOwner()->GetPosition() - GetTarget()->GetPosition();
		float Distance = Direction.Length();

		if (Distance < Treshold + GetOwner()->GetRadius() + GetTarget()->GetRadius())
		{
			Direction /= Distance + 0.0001f;

			Distance -= GetOwner()->GetRadius() + GetTarget()->GetRadius();
			if (Distance < 0.0f)
			{
				Distance = 0.0001f;
			}

			Output.LinearAcceleration = Direction * ZEMath::Min(DecayCoefficient / (Distance * Distance), GetOwner()->GetMaxLinearAcceleration());
		}
	}

	for (ZESize I = 0; I < AvoidedActors.GetCount(); I++)
	{
		ZEVector3 Direction = GetOwner()->GetPosition() - AvoidedActors[I]->GetPosition();
		float Distance = Direction.Length();
		
		if (Distance < Treshold + GetOwner()->GetRadius() + AvoidedActors[I]->GetRadius())
		{
			Direction /= Distance + 0.0001f;
			
			Distance -= GetOwner()->GetRadius() + AvoidedActors[I]->GetRadius();
			if (Distance < 0.0f)
			{
				Distance = 0.0001f;
			}

			Output.LinearAcceleration += Direction * ZEMath::Min(DecayCoefficient / (Distance * Distance), GetOwner()->GetMaxLinearAcceleration());
		}
	}

	if (Output.LinearAcceleration.LengthSquare() > GetOwner()->GetMaxLinearAcceleration() * GetOwner()->GetMaxLinearAcceleration())
	{
		Output.LinearAcceleration.NormalizeSelf();
		Output.LinearAcceleration *= GetOwner()->GetMaxLinearAcceleration();
	}

	return Output;
}

ZESeperateSteering::ZESeperateSteering()
{
	SetPriority(1);
	Treshold = 4.0f;
	DecayCoefficient = 1.0f;
	SetPriority(2);
}

ZESteeringOutput ZEFrictionSteering::Process(float ElapsedTime)
{
	ZESteeringOutput Output;

	if (GetOwner()->GetLinearVelocity().LengthSquare() != 0)
	{
		Output.LinearAcceleration = -GetOwner()->GetLinearVelocity().Normalize() * Friction * GetOwner()->GetMaxLinearAcceleration();
	}

	Output.AngularAcceleration = ZEMath::Sign(GetOwner()->GetAngularVelocity()) * Friction * GetOwner()->GetMaxAngularAcceleration();
	
	return Output;
}

ZEFrictionSteering::ZEFrictionSteering()
{
	Friction = 0.1f;
	SetPriority(4);
}

ZESteeringOutput ZECollisionAvoidanceSteering::Process(float ElapsedTime)
{
	ZESteeringOutput Output;
	Output.SetZero();

	float ShortestTime = ZE_FLOAT_MAX;
	
	ZEActor* FirstTarget = NULL;
	float FirstMinSeperation;
	float FirstDistance;
	ZEVector3 FirstRelativePosition;
	ZEVector3 FirstRelativeVelocity;

	for (ZESize I = 0; I < AvoidedActors.GetCount(); I++)
	{
		ZEActor* CurrentTarget = AvoidedActors[I];

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

ZECollisionAvoidanceSteering::ZECollisionAvoidanceSteering()
{
	SetPriority(1);
}

#define ACTIONID_FORWARD		0
#define ACTIONID_BACKWARD		1
#define ACTIONID_STRAFELEFT		2
#define ACTIONID_STRAFERIGHT	3
#define ACTIONID_TURNLEFT		4
#define ACTIONID_TURNRIGHT		5
#define ACTIONID_TURNUP			6
#define ACTIONID_TURNDOWN		7

ZEUserTankControlStreering::ZEUserTankControlStreering()
{
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_FORWARD,			 ZEInputEvent("Keyboard", ZE_IKB_W, ZE_IBS_PRESSING)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_BACKWARD,		 ZEInputEvent("Keyboard", ZE_IKB_S, ZE_IBS_PRESSING)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_STRAFERIGHT,		 ZEInputEvent("Keyboard", ZE_IKB_D, ZE_IBS_PRESSING)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_STRAFELEFT,		 ZEInputEvent("Keyboard", ZE_IKB_A, ZE_IBS_PRESSING)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_TURNUP,			 ZEInputEvent("Mouse", ZE_IMA_VERTICAL_AXIS, ZE_IAS_POSITIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_TURNDOWN,		 ZEInputEvent("Mouse", ZE_IMA_VERTICAL_AXIS, ZE_IAS_NEGATIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_TURNRIGHT,		 ZEInputEvent("Mouse", ZE_IMA_HORIZANTAL_AXIS, ZE_IAS_POSITIVE)));
	InputMap.InputBindings.Add(ZEInputBinding(ACTIONID_TURNLEFT,		 ZEInputEvent("Mouse", ZE_IMA_HORIZANTAL_AXIS, ZE_IAS_NEGATIVE)));
}

ZESteeringOutput ZEUserTankControlStreering::Process(float ElapsedTime)
{
	ZESteeringOutput Output;
	Output.SetZero();

	ZEInputAction* Current;
	zeInput->ProcessInputMap(&InputMap);

	float Friction = 1.0f;

	ZEQuaternion OwnerRotation = GetOwner()->GetRotation();

	bool Linear = false;
	bool Angular = false;
	for (size_t I = 0; I < InputMap.InputActionCount; I++)
	{
		Current = &InputMap.InputActions[I];
		switch(Current->Id)
		{
		case ACTIONID_FORWARD:
			Output.LinearAcceleration = GetOwner()->GetMaxLinearAcceleration() * GetOwner()->GetFront();
			Linear = true;
			break;
		case ACTIONID_BACKWARD:
			Output.LinearAcceleration = -GetOwner()->GetMaxLinearAcceleration() * GetOwner()->GetFront();
			Linear = true;
			break;
		case ACTIONID_STRAFELEFT:
			Output.AngularAcceleration = -GetOwner()->GetMaxAngularAcceleration();
			Angular = true;
			break;
		case ACTIONID_STRAFERIGHT:
			Output.AngularAcceleration = GetOwner()->GetMaxAngularAcceleration();
			Angular = true;
			break;
		}
	}

	if (!Linear && GetOwner()->GetLinearVelocity().LengthSquare() != 0)
		Output.LinearAcceleration = -GetOwner()->GetLinearVelocity().Normalize() * Friction * GetOwner()->GetMaxLinearAcceleration();

	if (!Angular)
		Output.AngularAcceleration = -ZEMath::Sign(GetOwner()->GetAngularVelocity()) * Friction * GetOwner()->GetMaxAngularAcceleration();

	return Output;
}
