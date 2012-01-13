//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysXPhysicalJoint.h
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
#ifndef	__ZE_PHYSX_PHYSICAL_JOINT_H__
#define __ZE_PHYSX_PHYSICAL_JOINT_H__

#include "ZEPhysics/ZEPhysicalJoint.h"
#include "ZEPhysXPhysicalRigidBody.h"
#include "ZEPhysXConversion.h"

#include <NxD6Joint.h>
#include <NxD6JointDesc.h>

class ZEPhysicalObject;
class ZEPhysXPhysicalRigidBody;
class ZEPhysicalJoint;

class ZEPhysXPhysicalJoint: public ZEPhysicalJoint
{
	friend class ZEPhysXModule;

	private:
		ZEPhysXPhysicalWorld*					PhysicalWorld;
		NxD6Joint*								Joint;
		NxD6JointDesc							JointDesc;

		ZEQuaternion							BodyAConnectionOrientation;
		ZEQuaternion							BodyBConnectionOrientation;

		ZEQuaternion							GlobalOrientation;

		ZEVector3*								MassInertiaTensor;

		ZEArray<ZEPhysicalLimitPlane>			LimitPlanes;

		virtual void							ReCreate();

												ZEPhysXPhysicalJoint();										
		virtual									~ZEPhysXPhysicalJoint();

	public:
		virtual ZEPhysicalObjectType			GetPhysicalObjectType();

		virtual void							SetPhysicalWorld(ZEPhysicalWorld* World);
		virtual ZEPhysicalWorld*				GetPhysicalWorld();

		virtual void							SetEnabled(bool Enabled);
		virtual bool							GetEnabled();

		virtual ZEPhysicalJointState			GetJointState() const;

		virtual void							SetPosition(const ZEVector3& NewPosition);
		virtual ZEVector3						GetPosition();

		virtual void							SetRotation(const ZEQuaternion& NewRotation);
		virtual ZEQuaternion					GetRotation();

		virtual void							SetBodyA(ZEPhysicalRigidBody* Object);
		virtual ZEPhysicalRigidBody*			GetBodyA() const;

		virtual void							SetBodyAConnectionPosition(const ZEVector3& BodyPosition);
		virtual ZEVector3						GetBodyAConnectionPosition() const;

		virtual void							SetBodyAConnectionOrientation(const ZEQuaternion& BodyOrientation);
		virtual ZEQuaternion					GetBodyAConnectionOrientation() const;

		virtual void							SetBodyB(ZEPhysicalRigidBody* Object);
		virtual ZEPhysicalRigidBody*			GetBodyB() const;

		virtual void							SetBodyBConnectionPosition(const ZEVector3& BodyPosition);
		virtual ZEVector3						GetBodyBConnectionPosition() const;

		virtual void							SetBodyBConnectionOrientation(const ZEQuaternion& BodyOrientation);
		virtual ZEQuaternion					GetBodyBConnectionOrientation() const;

		virtual void							SetBreakForce(float BreakForce);
		virtual float							GetBreakForce() const;

		virtual void							SetBreakTorque(float BreakTorque);
		virtual float							GetBreakTorque() const;

		virtual void							SetBodiesCollide(bool BodiesCollide);
		virtual bool							GetBodiesCollide() const;

		virtual void							SetDrivePosition(const ZEVector3& DrivePosition);
		virtual ZEVector3						GetDrivePosition() const;

		virtual void							SetDriveOrientation(const ZEQuaternion& DriveOrientation);
		virtual ZEQuaternion					GetDriveOrientation() const;

		virtual void							SetDriveLinearVelocity(const ZEVector3& DriveLinearVelocity);
		virtual ZEVector3						GetDriveLinearVelocity() const;

		virtual void							SetDriveAngularVelocity(const ZEVector3& DriveAngularVelocity);
		virtual ZEVector3						GetDriveAngularVelocity() const;

		virtual void							SetMassInertiaTensor(const ZEVector3& InertiaTensor);
		virtual ZEVector3						GetMassInertiaTensor() const;

		virtual void							SetSolverExtrapolationFactor(float SolverExtrapolationFactor);
		virtual float							GetSolverExtrapolationFactor() const;

		virtual const 
		ZEArray<ZEPhysicalLimitPlane>&			GetLimitPlanes() const;
		virtual void							AddLimitPlane(const ZEPhysicalLimitPlane& Plane);
		virtual void							RemoveLimitPlane(size_t Index);

		//Constraints

		virtual void							SetXMotion(ZEPhysicalJointMotion XMotion);
		virtual ZEPhysicalJointMotion			GetXMotion() const;

		virtual void							SetYMotion(ZEPhysicalJointMotion YMotion);
		virtual ZEPhysicalJointMotion			GetYMotion() const;

		virtual void							SetZMotion(ZEPhysicalJointMotion ZMotion);
		virtual ZEPhysicalJointMotion			GetZMotion() const;

		virtual void							SetSwing1Motion(ZEPhysicalJointMotion Swing1Motion);
		virtual ZEPhysicalJointMotion			GetSwing1Motion() const;

		virtual void							SetSwing2Motion(ZEPhysicalJointMotion Swing2Motion);
		virtual ZEPhysicalJointMotion			GetSwing2Motion() const;

		virtual void							SetTwistMotion(ZEPhysicalJointMotion TwistMotion);
		virtual ZEPhysicalJointMotion			GetTwistMotion() const;

		//Linear Limit

		virtual void							SetLinearLimitValue(float LinearLimitValue);
		virtual float							GetLinearLimitValue() const;

		virtual void							SetLinearLimitRestitution(float LinearLimitRestitution);
		virtual float							GetLinearLimitRestitution() const;

		virtual void							SetLinearLimitSpring(float LinearLimitSpring);
		virtual float							GetLinearLimitSpring() const;

		virtual void							SetLinearLimitDamping(float LinearLimitDamping);
		virtual float							GetLinearLimitDamping() const;

		//Swing1 Limit

		virtual void							SetSwing1LimitValue(float Swing1LimitValue);
		virtual float							GetSwing1LimitValue() const;

		virtual void							SetSwing1LimitRestitution(float Swing1LimitRestitution);
		virtual float							GetSwing1LimitRestitution() const;

		virtual void							SetSwing1LimitSpring(float Swing1LimitSpring);
		virtual float							GetSwing1LimitSpring() const;

		virtual void							SetSwing1LimitDamping(float Swing1LimitDamping);
		virtual float							GetSwing1LimitDamping() const;

		//Swing2 Limit

		virtual void							SetSwing2LimitValue(float Swing2LimitValue);
		virtual float							GetSwing2LimitValue() const;

		virtual void							SetSwing2LimitRestitution(float Swing2LimitRestitution);
		virtual float							GetSwing2LimitRestitution() const;

		virtual void							SetSwing2LimitSpring(float Swing2LimitSpring);
		virtual float							GetSwing2LimitSpring() const;

		virtual void							SetSwing2LimitDamping(float Swing2LimitDamping);
		virtual float							GetSwing2LimitDamping() const;

		//Twist Low Limit

		virtual void							SetTwistLowLimitValue(float TwistLowLimitValue);
		virtual float							GetTwistLowLimitValue() const;

		virtual void							SetTwistLowLimitRestitution(float TwistLowLimitRestitution);
		virtual float							GetTwistLowLimitRestitution() const;

		virtual void							SetTwistLowLimitSpring(float TwistLowLimitSpring);
		virtual float							GetTwistLowLimitSpring() const;

		virtual void							SetTwistLowLimitDamping(float TwistLowLimitDamping);
		virtual float							GetTwistLowLimitDamping() const;

		//Twist High Limit

		virtual void							SetTwistHighLimitValue(float TwistHighLimitValue);
		virtual float							GetTwistHighLimitValue() const;

		virtual void							SetTwistHighLimitRestitution(float TwistHighLimitRestitution);
		virtual float							GetTwistHighLimitRestitution() const;

		virtual void							SetTwistHighLimitSpring(float TwistHighLimitSpring);
		virtual float							GetTwistHighLimitSpring() const;

		virtual void							SetTwistHighLimitDamping(float TwistHighLimitDamping);
		virtual float							GetTwistHighLimitDamping() const;

		//Joint Motors

		virtual void							SetMotorTargetPosition(ZEVector3 MotorTargetPosition);
		virtual ZEVector3						GetMotorTargetPosition() const;

		virtual void							SetMotorTargetOrientation(ZEQuaternion MotorTargetOrientation);
		virtual ZEQuaternion					GetMotorTargetOrientation() const;

		virtual void							SetMotorTargetVelocity(ZEVector3 MotorTargetVelocity);
		virtual ZEVector3						GetMotorTargetVelocity() const;

		virtual void							SetMotorTargetAngularVelocity(ZEVector3 MotorTargetAngularVelocity);
		virtual ZEVector3						GetMotorTargetAngularVelocity() const;

		virtual void							SetLinearXMotor(ZEPhysicalJointMotorType LinearXMotor);
		virtual ZEPhysicalJointMotorType		GetLinearXMotor() const;

		virtual void							SetLinearXMotorForce(float LinearXMotorForce);
		virtual float							GetLinearXMotorForce() const;

		virtual void							SetLinearXMotorSpring(float LinearXMotorSpring);
		virtual float							GetLinearXMotorSpring() const;

		virtual void							SetLinearXMotorDamper(float LinearXMotorDamper);
		virtual float							GetLinearXMotorDamper() const;

		virtual void							SetLinearYMotor(ZEPhysicalJointMotorType LinearYMotor);
		virtual ZEPhysicalJointMotorType		GetLinearYMotor() const;

		virtual void							SetLinearYMotorForce(float LinearYMotorForce);
		virtual float							GetLinearYMotorForce() const;

		virtual void							SetLinearYMotorSpring(float LinearYMotorSpring);
		virtual float							GetLinearYMotorSpring() const;

		virtual void							SetLinearYMotorDamper(float LinearYMotorDamper);
		virtual float							GetLinearYMotorDamper() const;

		virtual void							SetLinearZMotor(ZEPhysicalJointMotorType LinearZMotor);
		virtual ZEPhysicalJointMotorType		GetLinearZMotor() const;

		virtual void							SetLinearZMotorForce(float LinearZMotorForce);
		virtual float							GetLinearZMotorForce() const;

		virtual void							SetLinearZMotorSpring(float LinearZMotorSpring);
		virtual float							GetLinearZMotorSpring() const;

		virtual void							SetLinearZMotorDamper(float LinearZMotorDamper);
		virtual float							GetLinearZMotorDamper() const;

		virtual void							SetAngularSwingMotor(ZEPhysicalJointMotorType AngularSwingMotor);
		virtual ZEPhysicalJointMotorType		GetAngularSwingMotor() const;

		virtual void							SetAngularSwingMotorForce(float AngularSwingMotorForce);
		virtual float							GetAngularSwingMotorForce() const;

		virtual void							SetAngularSwingMotorSpring(float AngularSwingMotorSpring);
		virtual float							GetAngularSwingMotorSpring() const;

		virtual void							SetAngularSwingMotorDamper(float AngularSwingMotorDamper);
		virtual float							GetAngularSwingMotorDamper() const;

		virtual void							SetAngularTwistMotor(ZEPhysicalJointMotorType AngularTwistMotor);
		virtual ZEPhysicalJointMotorType		GetAngularTwistMotor() const;

		virtual void							SetAngularTwistMotorForce(float AngularTwistMotorForce);
		virtual float							GetAngularTwistMotorForce() const;

		virtual void							SetAngularTwistMotorSpring(float AngularTwistMotorSpring);
		virtual float							GetAngularTwistMotorSpring() const;

		virtual void							SetAngularTwistMotorDamper(float AngularTwistMotorDamper);
		virtual float							GetAngularTwistMotorDamper() const;

		virtual void							SetAngularSlerpMotor(ZEPhysicalJointMotorType AngularSlerpMotor);
		virtual ZEPhysicalJointMotorType		GetAngularSlerpMotor() const;

		virtual void							SetAngularSlerpMotorForce(float AngularSlerpMotorForce);
		virtual float							GetAngularSlerpMotorForce() const;

		virtual void							SetAngularSlerpMotorSpring(float AngularSlerpMotorSpring);
		virtual float							GetAngularSlerpMotorSpring() const;

		virtual void							SetAngularSlerpMotorDamper(float AngularSlerpMotorDamper);
		virtual float							GetAngularSlerpMotorDamper() const;

		///// Physical Object Abstraction

		virtual void							SetScale(const ZEVector3& NewScale);
		virtual ZEVector3						GetScale();

		virtual void							SetCollisionEventFlags(ZEUInt32 CollisionEventFlags);
		virtual ZEUInt32							GetCollisionEventFlags();

		//virtual void							SetDefaultValues(ZEPhysicalJointType Type);

		virtual bool							Initialize();
		virtual void							Deinitialize();
};

#endif
