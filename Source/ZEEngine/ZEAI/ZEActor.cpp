//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEActor.cpp
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

#include "ZETypes.h"
#include "ZEActor.h"
#include "ZESteering.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMath.h"


ZEActor::ZEActor()
{
	MinLinearAcceleration	= 0.01f;
	MinAngularAcceleration	= 0.01f;
	MaxAngularVelocity		= ZE_PI_2;
	MaxLinearVelocity		= 1.0f;
	LinearVelocity			= ZEVector3::Zero;
	AngularVelocity			= ZEQuaternion::Identity;
	LinearAcceleration		= ZEVector3::Zero;
	AngularAcceleration		= ZEQuaternion::Identity;

	MaxAngularAcceleration	= 1.0f;
	MaxLinearAcceleration	= 1.0f;
	Radius					= 1.0f;

	SetEntityFlags(ZE_EF_TICKABLE);
}

ZEActor::~ZEActor()
{
}


void ZEActor::SetLinearVelocity(const ZEVector3& Velocity)
{
	LinearVelocity = Velocity;
}

const ZEVector3& ZEActor::GetLinearVelocity()
{
	return LinearVelocity;
}

void ZEActor::SetAngularVelocity(const ZEQuaternion& Velocity)
{
	AngularVelocity = Velocity;
}

const ZEQuaternion& ZEActor::GetAngularVelocity()
{
	return AngularVelocity;
}

const ZEVector3& ZEActor::GetLinearAcceleration()
{
	return LinearAcceleration;
}

const ZEQuaternion& ZEActor::GetAngularAcceleration()
{
	return AngularAcceleration;
}

void ZEActor::SetMaxAngularVelocity(float Velocity)
{
	MaxAngularVelocity = Velocity;
}

float ZEActor::GetMaxAngularVelocity()
{
	return MaxLinearVelocity;
}

void ZEActor::SetMinAngularAcceleration(float Acceleration)
{
MinAngularAcceleration = Acceleration;
}

float ZEActor::GetMinAngularAcceleration()
{
	return MinAngularAcceleration;
}

void ZEActor::SetMaxAngularAcceleration(float Acceleration)
{
	MaxAngularAcceleration = Acceleration;
}

float ZEActor::GetMaxAngularAcceleration()
{
	return MaxAngularAcceleration;
}

void ZEActor::SetMaxLinearVelocity(float Velocity)
{
	MaxLinearVelocity = Velocity;
}

float ZEActor::GetMaxLinearSpeed()
{
	return MaxLinearVelocity;
}

void ZEActor::SetMinLinearAcceleration(float Acceleration)
{
	MinLinearAcceleration = Acceleration;
}

float ZEActor::GetMinLinearAcceleration()
{
	return MinLinearAcceleration;
}

void ZEActor::SetMaxLinearAcceleration(float Acceleration)
{
	MaxLinearAcceleration = Acceleration;
}

float ZEActor::GetMaxLinearAcceleration()
{
	return MaxLinearAcceleration;
}

void ZEActor::SetRadius(float Radius)
{
	this->Radius = Radius;
}

float ZEActor::GetRadius()
{
	return Radius;
}

const ZEArray<ZESteering*>& ZEActor::GetSteerings()
{
	return Steerings;
}

void ZEActor::AddSteering(ZESteering* Steering)
{
	Steering->SetOwner(this);
	Steerings.Add(Steering);
}

void ZEActor::RemoveSteering(ZESteering* Steering)
{
	Steerings.RemoveValue(Steering);
}

void ZEActor::Tick(float ElapsedTime)
{
	LinearAcceleration = ZEVector3::Zero;
	AngularAcceleration = ZEQuaternion::Identity;

	bool PriorityLinearSteeringDone = false;
	bool PriorityAngularSteeringDone = false;

	// Walk through priority levels
	for (ZESize Priority = 1; Priority <= 5; Priority++)
	{
		ZEVector3 PriorityLinearAcceleration = ZEVector3::Zero;
		ZEQuaternion PriorityAngularAcceleration = ZEQuaternion::Identity;

		for (ZESize I = 0; I < Steerings.GetCount(); I++)
		{
			if (Steerings[I]->GetEnabled() && Steerings[I]->GetPriority() == Priority && Steerings[I]->GetEnabled() && Steerings[I]->GetWeight() != 0.0f)
			{
				ZESteeringOutput Output = Steerings[I]->Process(ElapsedTime);

				PriorityLinearAcceleration += Steerings[I]->GetWeight() * Output.LinearAcceleration;
				ZEQuaternion::ScaleRotation(Output.AngularAcceleration, Output.AngularAcceleration, Steerings[I]->GetWeight());
				PriorityAngularAcceleration = PriorityAngularAcceleration * Output.AngularAcceleration;
			}
		}

		// Check and clamp linear output
		if (!PriorityLinearSteeringDone)
		{
			float LengthSquare = PriorityLinearAcceleration.LengthSquare();
			if (LengthSquare > MinLinearAcceleration * MinLinearAcceleration)
			{
				if (LengthSquare < MaxLinearAcceleration * MaxLinearAcceleration)
					LinearAcceleration = PriorityLinearAcceleration;
				else
					LinearAcceleration = MaxLinearAcceleration * (PriorityLinearAcceleration / ZEMath::Sqrt(LengthSquare));

				PriorityLinearSteeringDone = true;
			}
		}

		// Check and clamp angular output
		if (!PriorityAngularSteeringDone)
		{
			float PriorityAngularAccelerationAngle;
			ZEVector3 PriorityAngularAccelerationAxis;
			ZEQuaternion::ConvertToAngleAxis(PriorityAngularAccelerationAngle, PriorityAngularAccelerationAxis, PriorityAngularAcceleration);
			
			if (ZEMath::Abs(PriorityAngularAccelerationAngle) > MinAngularAcceleration)
			{
				if (ZEMath::Abs(PriorityAngularAccelerationAngle) < MaxAngularAcceleration)
				{
					PriorityAngularAccelerationAngle = MaxAngularAcceleration * ZEMath::Sign(PriorityAngularAccelerationAngle);
					ZEQuaternion::CreateFromAngleAxis(AngularAcceleration, PriorityAngularAccelerationAngle, PriorityAngularAccelerationAxis);
				}	
				else
					AngularAcceleration = PriorityAngularAcceleration;
				PriorityAngularSteeringDone = true;
			}
		}

		if (PriorityLinearSteeringDone && PriorityAngularSteeringDone)
			break;
	}	

	ZEVector3 LinearVelocity = GetLinearVelocity();
	ZEQuaternion AngularVelocity = GetAngularVelocity();

	LinearVelocity += LinearAcceleration * ElapsedTime;

	ZEQuaternion TempAngularAcceleration;
	ZEQuaternion::ScaleRotation(TempAngularAcceleration, AngularAcceleration, ElapsedTime);
	AngularVelocity =  AngularVelocity * TempAngularAcceleration;

	// Clamp Linear Velocity
	if (LinearVelocity.LengthSquare() > MaxLinearVelocity * MaxLinearVelocity)
	{
		LinearVelocity.NormalizeSelf();
		LinearVelocity *= MaxLinearVelocity;
	}
	SetLinearVelocity(LinearVelocity);
	
	// Clamp Angular Velocity
	float AngularVelocityAngle;
	ZEVector3 AngularVelocityAxis;
	ZEQuaternion::ConvertToAngleAxis(AngularVelocityAngle, AngularVelocityAxis, AngularVelocity);
	if (ZEMath::Abs(AngularVelocityAngle) > MaxAngularVelocity)
	{
		AngularVelocityAngle = MaxAngularVelocity * ZEMath::Sign(AngularVelocityAngle);
		ZEQuaternion::CreateFromAngleAxis(AngularVelocity, AngularVelocityAngle, AngularVelocityAxis);
	}
	SetAngularVelocity(AngularVelocity);
	
	ZEVector3 Position = GetPosition();
	Position += LinearVelocity * ElapsedTime;
	SetPosition(Position);

	ZEQuaternion Rotation = GetRotation();
	ZEQuaternion TempAngularVelocity;
	ZEQuaternion::ScaleRotation(TempAngularVelocity, AngularVelocity, ElapsedTime);
	Rotation = Rotation * TempAngularVelocity;
	SetRotation(Rotation);


	ZEEntity::Tick(ElapsedTime);
}
