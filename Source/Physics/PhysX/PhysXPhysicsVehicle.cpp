//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysXPhysicsVehicle.cpp
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

#include "NxPhysics.h"
//#include "NxWheel.h"
#include "ZEMath/Vector.h"
#include "ZEMath/Quaternion.h"
#include "Physics/PhysicsVehicle.h"
#include "Physics/PhysicsVehicleInfo.h"
#include "Physics/PhysicsBody.h"
#include "PhysXPhysicsBody.h"
#include "Physics/PhysicsWorld.h"
#include "PhysXPhysicsWorld.h"
#include "PhysXPhysicsUtility.h"
#include "PhysXPhysicsVehicle.h"
/*
ZEPhysXPhysicsVehicle::ZEPhysXPhysicsVehicle() : Body(NULL)
{
	Wheels.Clear();
	Contacts.Clear();
	RollValues.Clear();
}

ZEPhysXPhysicsVehicle::~ZEPhysXPhysicsVehicle()
{
	Deinitialize();
}

void ZEPhysXPhysicsVehicle::Initialize(ZEPhysicsVehicleInfo& Info)
{
	ZEPhysXPhysicsWorld* World = ZEPhysXPhysicsWorld::getSingletonPtr();

	if (World != NULL && Info.Body != NULL)
	{
		Body = Info.Body;
		ZEPhysXPhysicsBody* ABody = (ZEPhysXPhysicsBody*)Body;

		for (int i=0;i<Info.WheelInfo.GetCount();i++)
		{
			ZEPhysicsWheelAttribute Atr = Info.WheelInfo[i];
			//wheel desc
			NxWheelDesc wheelDesc;
			wheelDesc.wheelApproximation = 10;
			wheelDesc.wheelRadius = Atr.Radius;
			wheelDesc.wheelWidth = Atr.Width;
			wheelDesc.wheelSuspension = Atr.SuspensionLength;
			wheelDesc.springRestitution = Atr.SuspensionSpring;
			wheelDesc.springDamping = Atr.SuspensionDamper;
			wheelDesc.springBias = 0;
			wheelDesc.maxBrakeForce = Info.BrakeForce;

			//material
			NxWheelShapeDesc wheelShapeDesc;
			NxMaterial* wsm;
			NxMaterialDesc m;
			m.flags |= NX_MF_DISABLE_FRICTION;
			wsm = World->GetScene()->createMaterial(m);
			wheelShapeDesc.materialIndex = wsm->getMaterialIndex();

			//params
			NxReal heightModifier = (wheelDesc.wheelSuspension + wheelDesc.wheelRadius) / wheelDesc.wheelSuspension;
			wheelShapeDesc.suspension.spring = wheelDesc.springRestitution*heightModifier;
			wheelShapeDesc.suspension.damper = wheelDesc.springDamping*heightModifier;
			wheelShapeDesc.suspension.targetValue = wheelDesc.springBias*heightModifier;
			wheelShapeDesc.radius = wheelDesc.wheelRadius;
			wheelShapeDesc.suspensionTravel = wheelDesc.wheelSuspension;
			wheelShapeDesc.inverseWheelMass = 0.5;

			//Wheel
			wheelDesc.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF;
			if (Atr.Accelerated)
			{
				wheelDesc.wheelFlags |= NX_WF_ACCELERATED;
			}
			if (Atr.Steerable)
			{
				wheelDesc.wheelFlags |= NX_WF_STEERABLE_INPUT;
			}
			wheelDesc.position = TONX(Atr.Position);
			wheelShapeDesc.localPose.t = wheelDesc.position;
			if (wheelShapeDesc.isValid())
			{
				NxWheelShape* WShape = static_cast<NxWheelShape *>(ABody->GetActor()->createShape(wheelShapeDesc));
				ZEPhysicsCollisionMask FullMask;FullMask.Full();
				WShape->setGroupsMask(ZEPhysXPhysicsUtility::toNX(FullMask));
				WShape->setWheelFlags(wheelDesc.wheelFlags);
				Wheels.Add(WShape);
				//add contact data
				NxWheelContactData CData;
				Contacts.Add(CData);
				//add roll value
				RollValues.Add(0);
			}
		}

		//wake up actor
		ABody->GetActor()->wakeUp(1e30);
	}
}

void ZEPhysXPhysicsVehicle::Deinitialize()
{
	ZEPhysXPhysicsWorld* World = ZEPhysXPhysicsWorld::getSingletonPtr();

	if (World != NULL)
	{
		if (Body != NULL)
		{
			ZEPhysXPhysicsBody* ABody = (ZEPhysXPhysicsBody*)Body;
			for (int i=0;i<Wheels.GetCount();i++)
			{
				ABody->GetActor()->releaseShape(*Wheels[i]);
				Wheels[i] = NULL;
			}
			Wheels.Clear();
			Contacts.Clear();
			RollValues.Clear();
		}
	}
}

void ZEPhysXPhysicsVehicle::Update(float ElapsedTime)
{
	for (int i=0;i<Wheels.GetCount();i++)
	{
		Wheels[i]->getContact(Contacts[i]);
		RollValues[i] += Wheels[i]->getAxleSpeed() * ElapsedTime;
	}
}

void ZEPhysXPhysicsVehicle::SetSteeringAngle(float Angle)
{
	for (int i=0;i<Wheels.GetCount();i++)
	{
		if (Wheels[i]->getWheelFlags() & NX_WF_STEERABLE_INPUT)
		{
			Wheels[i]->setSteerAngle(Angle);
		}
	}
}

void ZEPhysXPhysicsVehicle::SetMotorTorque(float Torque)
{
	for (int i=0;i<Wheels.GetCount();i++)
	{
		if (Wheels[i]->getWheelFlags() & NX_WF_ACCELERATED)
		{
			Wheels[i]->setMotorTorque(Torque);
		}
	}
}

void ZEPhysXPhysicsVehicle::SetBrakeTorque(float Torque)
{
	for (int i=0;i<Wheels.GetCount();i++)
	{
		Wheels[i]->setBrakeTorque(Torque);
	}
}

ZEVector3 ZEPhysXPhysicsVehicle::GetWheelPosition(int Index)
{
	NxVec3 Pos = Wheels[Index]->getGlobalPosition();
	Pos.y -= Contacts[Index].contactPosition - Wheels[Index]->getRadius();
	return TOZE(Pos);
}

ZEQuaternion ZEPhysXPhysicsVehicle::GetWheelOrientation(int Index)
{
	ZEQuaternion QRet,QBody,QWheel;
	QBody = Body->GetOrientation();
	float Yaw = Wheels[Index]->getSteerAngle();
	ZEQuaternion::Create(QWheel, RollValues[Index], Yaw, 0);
	ZEQuaternion::Product(QRet, QBody, QWheel);
	return QRet;
}

float ZEPhysXPhysicsVehicle::GetWheelSpeed(int Index)
{
	return Wheels[Index]->getAxleSpeed();
}

float ZEPhysXPhysicsVehicle::GetAverageWheelSpeed()
{
	float Ret = 0;
	for (int i=0;i<Wheels.GetCount();i++)
	{
		Ret += Wheels[i]->getAxleSpeed();
	}
	return Ret / Wheels.GetCount();
}*/
