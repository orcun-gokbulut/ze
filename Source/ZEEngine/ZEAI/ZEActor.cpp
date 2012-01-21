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


void ZEActor::SetRotation2D(float Rotation)
{
	this->Rotation2D = ZEAngle::Range(Rotation);
	SetRotation(ZEQuaternion(Rotation2D,ZEVector3::UnitY));
}

float ZEActor::GetRotation2D()
{
	return Rotation2D;
}

void ZEActor::SetLinearVelocity(const ZEVector3& Velocity)
{
	LinearVelocity = Velocity;
}

const ZEVector3& ZEActor::GetLinearVelocity()
{
	return LinearVelocity;
}

void ZEActor::SetAngularVelocity(float Velocity)
{
	AngularVelocity = Velocity;
}

float ZEActor::GetAngularVelocity()
{
	return AngularVelocity;
}

const ZEVector3& ZEActor::GetLinearAcceleration()
{
	return LinearAcceleration;
}

float ZEActor::GetAngularAcceleration()
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
	Steerings.DeleteValue(Steering);
}

void ZEActor::Tick(float ElapsedTime)
{
	bool PriorityLinearSteeringDone = false;
	bool PriorityAngularSteeringDone = false;

	for (ZESize Priority = 1; Priority <= 5; Priority++)
	{
		ZEVector3 PriorityLinearAcceleration = ZEVector3::Zero;
		float PriorityAngularAcceleration = 0.0f;

		for (ZESize I = 0; I < Steerings.GetCount(); I++)
		{
			if (Steerings[I]->GetPriority() == Priority && Steerings[I]->GetEnabled() && Steerings[I]->GetWeight() != 0.0f)
			{
				ZESteeringOutput Output = Steerings[I]->Process(ElapsedTime);

				LinearAcceleration += Steerings[I]->GetWeight() * Output.LinearAcceleration;
				AngularAcceleration += Steerings[I]->GetWeight() * Output.AngularAcceleration;
			}
		}
		if (LinearAcceleration.LengthSquare() < MinLinearAcceleration * MinLinearAcceleration)
			LinearAcceleration = ZEVector3::Zero;

		if (fabs(AngularAcceleration) < MinAngularAcceleration)
			AngularAcceleration = 0.0f;

		if (!PriorityLinearSteeringDone && PriorityLinearAcceleration.LengthSquare() > GetMaxLinearAcceleration() * GetMaxLinearAcceleration() * 0.1f)
		{
			LinearAcceleration = PriorityLinearAcceleration;
			PriorityLinearSteeringDone = true;
		}

		if (!PriorityAngularSteeringDone && PriorityAngularAcceleration > GetMaxAngularAcceleration() * 0.1f)
		{
			AngularAcceleration = PriorityAngularAcceleration;
			PriorityAngularSteeringDone = true;
		}

		if (PriorityLinearSteeringDone && PriorityAngularSteeringDone)
			break;
	}	

	if (LinearAcceleration.LengthSquare() > GetMaxLinearAcceleration() * GetMaxLinearAcceleration())
	{
		LinearAcceleration.NormalizeSelf();
		LinearAcceleration *= GetMaxLinearAcceleration();
	}

	if (AngularAcceleration > GetMaxAngularAcceleration())
		AngularAcceleration = ZEMath::Sign(AngularAcceleration) * GetMaxAngularAcceleration();

	ZEVector3 LinearVelocity = GetLinearVelocity();
	float AngularVelocity = GetAngularVelocity();

	LinearVelocity += LinearAcceleration * ElapsedTime;
	AngularVelocity += AngularAcceleration * ElapsedTime;

	if (LinearVelocity.LengthSquare() > MaxLinearVelocity * MaxLinearVelocity)
	{
		LinearVelocity.NormalizeSelf();
		LinearVelocity *= MaxLinearVelocity;
	}
	
	if (AngularVelocity > MaxAngularVelocity)
		AngularVelocity = MaxAngularVelocity;
	else if (AngularVelocity < -MaxAngularVelocity)
		AngularVelocity = -MaxAngularVelocity;


	ZEVector3 Position = GetPosition();
	float Rotation = GetRotation2D();

	Position += LinearVelocity * ElapsedTime;
	Rotation += AngularVelocity * ElapsedTime;

	Rotation = ZEAngle::Range(Rotation);

	SetPosition(Position);
	SetRotation2D(Rotation);
	SetLinearVelocity(LinearVelocity);
	SetAngularVelocity(AngularVelocity);

	ZECompoundEntity::Tick(ElapsedTime);
}

ZEActor::ZEActor()
{
	static ZEInt Index = 0;
	Rotation2D = 0.0f;
	MinLinearAcceleration = 0.01;
	MinAngularAcceleration = 0.01;
	MaxAngularVelocity = ZE_PI_2;
	MaxLinearVelocity = 1.0f;
	LinearVelocity = ZEVector3::Zero;
	AngularVelocity = 0.0f;
	LinearAcceleration = ZEVector3::Zero;
	AngularAcceleration = 0.0f;

	MaxAngularAcceleration = 1.0f;
	MaxLinearAcceleration = 1.0f;
	Radius = 1.0f;
}

ZEActor::~ZEActor()
{
}
