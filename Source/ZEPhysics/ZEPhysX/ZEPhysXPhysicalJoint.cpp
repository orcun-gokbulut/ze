//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysXPhysicalJoint.cpp
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

#include "ZEPhysXPhysicalJoint.h"
#include "ZEPhysXPhysicalWorld.h"
#include "ZEError.h"

static inline NxD6JointMotion ConvertZEMotionToNXMotion(ZEPhysicalJointMotion Motion)
{
	switch (Motion)
	{
		case ZE_PJMOTION_FREE:
			return NX_D6JOINT_MOTION_FREE;
			break;

		case ZE_PJMOTION_LIMITED:
			return NX_D6JOINT_MOTION_LIMITED;
			break;

		case ZE_PJMOTION_LOCKED:
			return NX_D6JOINT_MOTION_LOCKED;
			break;
		default:
			return NX_D6JOINT_MOTION_FREE;
			break;
	}
}

static inline ZEPhysicalJointMotion ConvertNXMotionToZEMotion(NxD6JointMotion Motion)
{
	switch (Motion)
	{
		case NX_D6JOINT_MOTION_FREE:
			return ZE_PJMOTION_FREE;
			break;

		case NX_D6JOINT_MOTION_LIMITED:
			return ZE_PJMOTION_LIMITED;
			break;

		case NX_D6JOINT_MOTION_LOCKED:
			return ZE_PJMOTION_LOCKED;
			break;
		default:
			return ZE_PJMOTION_FREE;
			break;
	}
}

static inline ZEPhysicalJointMotorType ConvertNxMotorToZEMotor(NxBitField32 MotorType)
{
	switch(MotorType)
	{
	case NX_D6JOINT_DRIVE_POSITION:
		return ZE_PJMT_POSITION;
		break;

	case NX_D6JOINT_DRIVE_VELOCITY:
		return ZE_PJMT_VELOCITY;
		break;
	default:
		return ZE_PJMT_NONE;
		break;
	}
}

static inline NxBitField32 ConvertZEMotorToNxMotor(ZEPhysicalJointMotorType MotorType)
{
	switch(MotorType)
	{
		case ZE_PJMT_POSITION:
			return NX_D6JOINT_DRIVE_POSITION;
			break;

		case ZE_PJMT_VELOCITY:
			return NX_D6JOINT_DRIVE_VELOCITY;
			break;
		default:
			return 0;
			break;
	}
}

void ZEPhysXPhysicalJoint::SetPhysicalWorld(ZEPhysicalWorld* World)
{
	PhysicalWorld = (ZEPhysXPhysicalWorld*)World;
	if (Joint != NULL)
		ReCreate();
}

ZEPhysicalWorld* ZEPhysXPhysicalJoint::GetPhysicalWorld()
{
	return PhysicalWorld;
}

ZEPhysicalJointState ZEPhysXPhysicalJoint::GetJointState() const
{
	switch (Joint->getState())
	{
	default:
		zeError("ZEPhysicalJointState is not valid");

		case NX_JS_UNBOUND:
			return ZE_PJS_UNBOUND;
			break;

		case NX_JS_SIMULATING:
			return ZE_PJS_SIMULATING;
			break;

		case NX_JS_BROKEN:
			return ZE_PJS_BROKEN;
			break;
	}
}

void ZEPhysXPhysicalJoint::ReCreate()
{
	Deinitialize();
	Initialize();
}

void ZEPhysXPhysicalJoint::SetPosition(const ZEVector3& NewPosition)
{
	JointDesc.setGlobalAnchor(ZE_TO_NX(NewPosition));
	if (Joint != NULL)
		Joint->setGlobalAnchor(ZE_TO_NX(NewPosition));
}

ZEVector3 ZEPhysXPhysicalJoint::GetPosition()
{
	if (Joint != NULL)
		return NX_TO_ZE(Joint->getGlobalAnchor());
	else
		return ZEVector3::Zero;
}

void ZEPhysXPhysicalJoint::SetRotation(const ZEQuaternion &NewRotation)
{
	ZEVector3 Temp;
	ZEQuaternion::VectorProduct(Temp, NewRotation, ZEVector3::UnitX);

	JointDesc.setGlobalAxis(ZE_TO_NX(Temp));
	if (Joint != NULL)
		Joint->setGlobalAxis(ZE_TO_NX(Temp));

	GlobalOrientation = NewRotation;
}

ZEQuaternion ZEPhysXPhysicalJoint::GetRotation()
{
	if (Joint != NULL)
		return GlobalOrientation;
	else
		return ZEQuaternion::Zero;
}

void ZEPhysXPhysicalJoint::SetBodyA(ZEPhysicalRigidBody* Object)
{
	if (Object != NULL)
		JointDesc.actor[0] = ((ZEPhysXPhysicalRigidBody*)Object)->Actor;
	else
		JointDesc.actor[0] = NULL;

	if (Joint != NULL)
		ReCreate();
}

ZEPhysicalRigidBody* ZEPhysXPhysicalJoint::GetBodyA() const
{
	return (ZEPhysicalRigidBody*)JointDesc.actor[0]->userData;
}

void ZEPhysXPhysicalJoint::SetBodyAConnectionPosition(const ZEVector3& Position)
{
	JointDesc.localAnchor[0] = ZE_TO_NX(Position);
	if (Joint != NULL)
		ReCreate();
}

ZEVector3 ZEPhysXPhysicalJoint::GetBodyAConnectionPosition() const
{
	return NX_TO_ZE(JointDesc.localAnchor[0]);
}

void ZEPhysXPhysicalJoint::SetBodyAConnectionOrientation(const ZEQuaternion& Orientation)
{
	ZEVector3 Temp;
	ZEQuaternion::VectorProduct(Temp, Orientation, ZEVector3::UnitX);
	JointDesc.localAxis[0] = ZE_TO_NX(Temp);
	ZEQuaternion::VectorProduct(Temp, Orientation, ZEVector3::UnitY);
	JointDesc.localNormal[0] = ZE_TO_NX(Temp);

	BodyAConnectionOrientation = Orientation;

	if (Joint != NULL)
		ReCreate();
}

ZEQuaternion ZEPhysXPhysicalJoint::GetBodyAConnectionOrientation() const
{
	return BodyAConnectionOrientation;
}

void ZEPhysXPhysicalJoint::SetBodyB(ZEPhysicalRigidBody* Object)
{
	if (Object != NULL)
		JointDesc.actor[1] = ((ZEPhysXPhysicalRigidBody*)Object)->Actor;
	else
		JointDesc.actor[1] = NULL;

	if (Joint != NULL)
		ReCreate();
}

ZEPhysicalRigidBody* ZEPhysXPhysicalJoint::GetBodyB() const
{
	return (ZEPhysicalRigidBody*)JointDesc.actor[1]->userData;
}

void ZEPhysXPhysicalJoint::SetBodyBConnectionPosition(const ZEVector3& Position)
{
	JointDesc.localAnchor[1] = ZE_TO_NX(Position);
	if (Joint != NULL)
		ReCreate();
}

ZEVector3 ZEPhysXPhysicalJoint::GetBodyBConnectionPosition() const
{
	return NX_TO_ZE(JointDesc.localAnchor[1]);
}

void ZEPhysXPhysicalJoint::SetBodyBConnectionOrientation(const ZEQuaternion& Orientation)
{
	ZEVector3 Temp;
	ZEQuaternion::VectorProduct(Temp, Orientation, ZEVector3::UnitX);
	JointDesc.localAxis[1] = ZE_TO_NX(Temp);
	ZEQuaternion::VectorProduct(Temp, Orientation, ZEVector3::UnitY);
	JointDesc.localNormal[1] = ZE_TO_NX(Temp);

	BodyBConnectionOrientation = Orientation;

	if (Joint != NULL)
		ReCreate();
}

ZEQuaternion ZEPhysXPhysicalJoint::GetBodyBConnectionOrientation() const
{
	return BodyBConnectionOrientation;
}


void ZEPhysXPhysicalJoint::SetBreakForce(float BreakForce)
{
	JointDesc.maxForce = BreakForce;

	if (Joint != NULL)
		Joint->setBreakable(JointDesc.maxForce, JointDesc.maxTorque);
}

float ZEPhysXPhysicalJoint::GetBreakForce() const
{
	return JointDesc.maxForce;
}

void ZEPhysXPhysicalJoint::SetBreakTorque(float BreakTorque)
{
	JointDesc.maxTorque = BreakTorque;

	if (Joint != NULL)
		Joint->setBreakable(JointDesc.maxForce, JointDesc.maxTorque);
}

float ZEPhysXPhysicalJoint::GetBreakTorque() const
{
	return JointDesc.maxTorque;
}

void ZEPhysXPhysicalJoint::SetBodiesCollide(bool BodiesCollide)
{
	if (BodiesCollide)
		JointDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
}

bool ZEPhysXPhysicalJoint::GetBodiesCollide() const
{
	if (JointDesc.jointFlags == NX_JF_COLLISION_ENABLED)
	{
		return true;
	}
	return false;
}

void ZEPhysXPhysicalJoint::SetDrivePosition(const ZEVector3& DrivePosition)
{
	Joint->setDrivePosition(ZE_TO_NX(DrivePosition));

	if (Joint != NULL)
		ReCreate();
}


ZEVector3 ZEPhysXPhysicalJoint::GetDrivePosition() const
{
	return NX_TO_ZE(JointDesc.drivePosition);
}

void ZEPhysXPhysicalJoint::SetDriveOrientation(const ZEQuaternion& DriveOrientation)
{
	Joint->setDriveOrientation(ZE_TO_NX(DriveOrientation));

	if (Joint != NULL)
		ReCreate();
}

ZEQuaternion ZEPhysXPhysicalJoint::GetDriveOrientation() const
{
	return NX_TO_ZE(JointDesc.driveOrientation);
}

void ZEPhysXPhysicalJoint::SetDriveLinearVelocity(const ZEVector3& DriveLinearVelocity)
{
	Joint->setDriveLinearVelocity(ZE_TO_NX(DriveLinearVelocity));

	if (Joint != NULL)
		ReCreate();
}

ZEVector3 ZEPhysXPhysicalJoint::GetDriveLinearVelocity() const
{
	return NX_TO_ZE(JointDesc.driveLinearVelocity);
}

void ZEPhysXPhysicalJoint::SetDriveAngularVelocity(const ZEVector3& DriveAngularVelocity)
{
	Joint->setDriveAngularVelocity(ZE_TO_NX(DriveAngularVelocity));

	if (Joint != NULL)
		ReCreate();
}

ZEVector3 ZEPhysXPhysicalJoint::GetDriveAngularVelocity() const
{
	return NX_TO_ZE(JointDesc.driveAngularVelocity);
}

void ZEPhysXPhysicalJoint::SetMassInertiaTensor(const ZEVector3& InertiaTensor)
{
	JointDesc.actor[0]->setMassSpaceInertiaTensor(ZE_TO_NX(InertiaTensor));
	JointDesc.actor[1]->setMassSpaceInertiaTensor(ZE_TO_NX(InertiaTensor));
}

ZEVector3 ZEPhysXPhysicalJoint::GetMassInertiaTensor() const
{
	return NX_TO_ZE(JointDesc.actor[0]->getMassSpaceInertiaTensor());
}

void ZEPhysXPhysicalJoint::SetSolverExtrapolationFactor(float SolverExtrapolationFactor)
{
	JointDesc.solverExtrapolationFactor = SolverExtrapolationFactor;
}

float ZEPhysXPhysicalJoint::GetSolverExtrapolationFactor() const
{
	return JointDesc.solverExtrapolationFactor;
}


const ZEArray<ZEPhysicalLimitPlane>& ZEPhysXPhysicalJoint::GetLimitPlanes() const
{
	return LimitPlanes;
}

void ZEPhysXPhysicalJoint::AddLimitPlane(const ZEPhysicalLimitPlane& Plane)
{
	LimitPlanes.Add(Plane);

	if (Joint != NULL)
		Joint->addLimitPlane(ZE_TO_NX(Plane.n), ZE_TO_NX(Plane.p), Plane.Restitution);
}

void ZEPhysXPhysicalJoint::RemoveLimitPlane(size_t Index)
{
	LimitPlanes.DeleteAt(Index);

	if (Joint != NULL)
	{
		Joint->purgeLimitPlanes();
		for (size_t I = 0; I < LimitPlanes.GetCount(); I++)
			Joint->addLimitPlane(ZE_TO_NX(LimitPlanes[I].n), ZE_TO_NX(LimitPlanes[I].p), LimitPlanes[I].Restitution);
	}
}

void ZEPhysXPhysicalJoint::SetXMotion(ZEPhysicalJointMotion XMotion)
{
	JointDesc.xMotion = ConvertZEMotionToNXMotion(XMotion);

	if (Joint != NULL)
		ReCreate();
}

ZEPhysicalJointMotion ZEPhysXPhysicalJoint::GetXMotion() const
{
	return ConvertNXMotionToZEMotion(JointDesc.xMotion);
}

void ZEPhysXPhysicalJoint::SetYMotion(ZEPhysicalJointMotion YMotion)
{
	JointDesc.yMotion = ConvertZEMotionToNXMotion(YMotion);

	if (Joint != NULL)
		ReCreate();
}

ZEPhysicalJointMotion ZEPhysXPhysicalJoint::GetYMotion() const
{
	return ConvertNXMotionToZEMotion(JointDesc.yMotion);
}

void ZEPhysXPhysicalJoint::SetZMotion(ZEPhysicalJointMotion ZMotion)
{
	JointDesc.zMotion = ConvertZEMotionToNXMotion(ZMotion);

	if (Joint != NULL)
		ReCreate();
}

ZEPhysicalJointMotion ZEPhysXPhysicalJoint::GetZMotion() const
{
	return ConvertNXMotionToZEMotion(JointDesc.zMotion);
}

void ZEPhysXPhysicalJoint::SetSwing1Motion(ZEPhysicalJointMotion Swing1Motion)
{
	JointDesc.swing1Motion = ConvertZEMotionToNXMotion(Swing1Motion);

	if (Joint != NULL)
		ReCreate();
}

ZEPhysicalJointMotion ZEPhysXPhysicalJoint::GetSwing1Motion() const
{
	return ConvertNXMotionToZEMotion(JointDesc.swing1Motion);
}

void ZEPhysXPhysicalJoint::SetSwing2Motion(ZEPhysicalJointMotion Swing2Motion)
{
	JointDesc.swing2Motion = ConvertZEMotionToNXMotion(Swing2Motion);

	if (Joint != NULL)
		ReCreate();
}

ZEPhysicalJointMotion ZEPhysXPhysicalJoint::GetSwing2Motion() const
{
	return ConvertNXMotionToZEMotion(JointDesc.swing2Motion);
}

void ZEPhysXPhysicalJoint::SetTwistMotion(ZEPhysicalJointMotion TwistMotion)
{
	JointDesc.twistMotion = ConvertZEMotionToNXMotion(TwistMotion);

	if (Joint != NULL)
		ReCreate();
}

ZEPhysicalJointMotion ZEPhysXPhysicalJoint::GetTwistMotion() const
{
	return ConvertNXMotionToZEMotion(JointDesc.twistMotion);
}

void ZEPhysXPhysicalJoint::SetLinearLimitValue(float LinearLimitValue)
{
	JointDesc.linearLimit.value = LinearLimitValue;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetLinearLimitValue() const
{
	return JointDesc.linearLimit.value;
}

void ZEPhysXPhysicalJoint::SetLinearLimitRestitution(float LinearLimitRestitution)
{
	JointDesc.linearLimit.restitution = LinearLimitRestitution;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetLinearLimitRestitution() const
{
	return JointDesc.linearLimit.restitution;
}

void ZEPhysXPhysicalJoint::SetLinearLimitSpring(float LinearLimitSpring)
{
	JointDesc.linearLimit.spring = LinearLimitSpring;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetLinearLimitSpring() const
{
	return JointDesc.linearLimit.spring;
}

void ZEPhysXPhysicalJoint::SetLinearLimitDamping(float LinearLimitDamping)
{
	JointDesc.linearLimit.damping = LinearLimitDamping;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetLinearLimitDamping() const
{
	return JointDesc.linearLimit.damping;
}

//////////////////////////////////////////////////////////////////////////

void ZEPhysXPhysicalJoint::SetSwing1LimitValue(float Swing1LimitValue)
{
	JointDesc.swing1Limit.value = Swing1LimitValue;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetSwing1LimitValue() const
{
	return JointDesc.swing1Limit.value;
}

void ZEPhysXPhysicalJoint::SetSwing1LimitRestitution(float Swing1LimitRestitution)
{
	JointDesc.swing1Limit.restitution = Swing1LimitRestitution;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetSwing1LimitRestitution() const
{
	return JointDesc.swing1Limit.restitution;
}

void ZEPhysXPhysicalJoint::SetSwing1LimitSpring(float Swing1LimitSpring)
{
	JointDesc.swing1Limit.spring = Swing1LimitSpring;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetSwing1LimitSpring() const
{
	return JointDesc.swing1Limit.spring;
}

void ZEPhysXPhysicalJoint::SetSwing1LimitDamping(float Swing1LimitDamping)
{
	JointDesc.swing1Limit.damping = Swing1LimitDamping;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetSwing1LimitDamping() const
{
	return JointDesc.swing1Limit.damping;
}

//////////////////////////////////////////////////////////////////////////

void ZEPhysXPhysicalJoint::SetSwing2LimitValue(float Swing2LimitValue)
{
	JointDesc.swing2Limit.value = Swing2LimitValue;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetSwing2LimitValue() const
{
	return JointDesc.swing2Limit.value;
}

void ZEPhysXPhysicalJoint::SetSwing2LimitRestitution(float Swing2LimitRestitution)
{
	JointDesc.swing2Limit.restitution = Swing2LimitRestitution;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetSwing2LimitRestitution() const
{
	return JointDesc.swing2Limit.restitution;
}

void ZEPhysXPhysicalJoint::SetSwing2LimitSpring(float Swing2LimitSpring)
{
	JointDesc.swing2Limit.spring = Swing2LimitSpring;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetSwing2LimitSpring() const
{
	return JointDesc.swing2Limit.spring;
}

void ZEPhysXPhysicalJoint::SetSwing2LimitDamping(float Swing2LimitDamping)
{
	JointDesc.swing2Limit.damping = Swing2LimitDamping;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetSwing2LimitDamping() const
{
	return JointDesc.swing2Limit.damping;
}

//////////////////////////////////////////////////////////////////////////


void ZEPhysXPhysicalJoint::SetTwistLowLimitValue( float TwistLowLimitValue )
{
	JointDesc.twistLimit.low.value = TwistLowLimitValue;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetTwistLowLimitValue() const
{
	return JointDesc.twistLimit.low.value;
}

void ZEPhysXPhysicalJoint::SetTwistLowLimitRestitution( float TwistLowLimitRestitution )
{
	JointDesc.twistLimit.low.restitution = TwistLowLimitRestitution;
	
	if (Joint != NULL)
			ReCreate();
}

float ZEPhysXPhysicalJoint::GetTwistLowLimitRestitution() const
{
	return JointDesc.twistLimit.low.restitution;
}

void ZEPhysXPhysicalJoint::SetTwistLowLimitSpring(float TwistLowLimitSpring)
{
	JointDesc.twistLimit.low.spring = TwistLowLimitSpring;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetTwistLowLimitSpring() const
{
	return JointDesc.twistLimit.low.spring;
}

void ZEPhysXPhysicalJoint::SetTwistLowLimitDamping( float TwistLowLimitDamping )
{
	JointDesc.twistLimit.low.damping = TwistLowLimitDamping;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetTwistLowLimitDamping() const
{
	return JointDesc.twistLimit.low.damping;
}

//////////////////////////////////////////////////////////////////////////

void ZEPhysXPhysicalJoint::SetTwistHighLimitValue( float TwistHighLimitValue )
{
	JointDesc.twistLimit.high.value = TwistHighLimitValue;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetTwistHighLimitValue() const
{
	return JointDesc.twistLimit.high.value;
}

void ZEPhysXPhysicalJoint::SetTwistHighLimitRestitution( float TwistHighLimitRestitution )
{
	JointDesc.twistLimit.high.restitution = TwistHighLimitRestitution;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetTwistHighLimitRestitution() const
{
	return JointDesc.twistLimit.high.restitution;
}

void ZEPhysXPhysicalJoint::SetTwistHighLimitSpring(float TwistHighLimitSpring)
{
	JointDesc.twistLimit.high.spring = TwistHighLimitSpring;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetTwistHighLimitSpring() const
{
	return JointDesc.twistLimit.high.spring;
}

void ZEPhysXPhysicalJoint::SetTwistHighLimitDamping( float TwistHighLimitDamping )
{
	JointDesc.twistLimit.high.damping = TwistHighLimitDamping;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetTwistHighLimitDamping() const
{
	return JointDesc.twistLimit.high.damping;
}

//////////////////////////////////////////////////////////////////////////
//								JOINT MOTORS
//////////////////////////////////////////////////////////////////////////

void ZEPhysXPhysicalJoint::SetMotorTargetPosition(ZEVector3 MotorTargetPosition)
{
	JointDesc.drivePosition = ZE_TO_NX(MotorTargetPosition);

	if (Joint != NULL)
		ReCreate();
}

ZEVector3 ZEPhysXPhysicalJoint::GetMotorTargetPosition() const
{
	return NX_TO_ZE(JointDesc.drivePosition);
}

void ZEPhysXPhysicalJoint::SetMotorTargetOrientation(ZEQuaternion MotorTargetOrientation)
{
	JointDesc.driveOrientation = ZE_TO_NX(MotorTargetOrientation);

	if (Joint != NULL)
		ReCreate();
}

ZEQuaternion ZEPhysXPhysicalJoint::GetMotorTargetOrientation() const
{
	return NX_TO_ZE(JointDesc.driveOrientation);
}

void ZEPhysXPhysicalJoint::SetMotorTargetVelocity(ZEVector3 MotorTargetVelocity)
{
	JointDesc.driveLinearVelocity = ZE_TO_NX(MotorTargetVelocity);

	if (Joint != NULL)
		ReCreate();
}

ZEVector3 ZEPhysXPhysicalJoint::GetMotorTargetVelocity() const
{
	return NX_TO_ZE(JointDesc.driveLinearVelocity);
}

void ZEPhysXPhysicalJoint::SetMotorTargetAngularVelocity(ZEVector3 MotorTargetAngularVelocity)
{
	JointDesc.driveAngularVelocity = ZE_TO_NX(MotorTargetAngularVelocity);

	if (Joint != NULL)
		ReCreate();
}

ZEVector3 ZEPhysXPhysicalJoint::GetMotorTargetAngularVelocity() const
{
	return NX_TO_ZE(JointDesc.driveAngularVelocity);
}

void ZEPhysXPhysicalJoint::SetLinearXMotor(ZEPhysicalJointMotorType LinearXMotor)
{
	JointDesc.xDrive.driveType = ConvertZEMotorToNxMotor(LinearXMotor);

	if (Joint != NULL)
		ReCreate();
}

ZEPhysicalJointMotorType ZEPhysXPhysicalJoint::GetLinearXMotor() const
{
	return ConvertNxMotorToZEMotor(JointDesc.xDrive.driveType);
}

void ZEPhysXPhysicalJoint::SetLinearXMotorForce(float LinearXMotorForce)
{
	JointDesc.xDrive.forceLimit = LinearXMotorForce;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetLinearXMotorForce() const
{
	return JointDesc.xDrive.forceLimit;
}

void ZEPhysXPhysicalJoint::SetLinearXMotorSpring(float LinearXMotorSpring)
{
	JointDesc.xDrive.spring = LinearXMotorSpring;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetLinearXMotorSpring() const
{
	return JointDesc.xDrive.spring;
}

void ZEPhysXPhysicalJoint::SetLinearXMotorDamper(float LinearXMotorDamper)
{
	JointDesc.xDrive.damping = LinearXMotorDamper;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetLinearXMotorDamper() const
{
	return JointDesc.xDrive.damping;
}

void ZEPhysXPhysicalJoint::SetLinearYMotor(ZEPhysicalJointMotorType LinearYMotor)
{
	JointDesc.yDrive.driveType = ConvertZEMotorToNxMotor(LinearYMotor);

	if (Joint != NULL)
		ReCreate();
}

ZEPhysicalJointMotorType ZEPhysXPhysicalJoint::GetLinearYMotor() const
{
	return ConvertNxMotorToZEMotor(JointDesc.yDrive.driveType);
}

void ZEPhysXPhysicalJoint::SetLinearYMotorForce(float LinearYMotorForce)
{
	JointDesc.yDrive.forceLimit = LinearYMotorForce;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetLinearYMotorForce() const
{
	return JointDesc.yDrive.forceLimit;
}

void ZEPhysXPhysicalJoint::SetLinearYMotorSpring(float LinearYMotorSpring)
{
	JointDesc.yDrive.spring = LinearYMotorSpring;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetLinearYMotorSpring() const
{
	return JointDesc.yDrive.spring;
}

void ZEPhysXPhysicalJoint::SetLinearYMotorDamper(float LinearYMotorDamper)
{
	JointDesc.yDrive.damping = LinearYMotorDamper;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetLinearYMotorDamper() const
{
	return JointDesc.yDrive.damping;
}

void ZEPhysXPhysicalJoint::SetLinearZMotor(ZEPhysicalJointMotorType LinearZMotor)
{
	JointDesc.zDrive.driveType = ConvertZEMotorToNxMotor(LinearZMotor);

	if (Joint != NULL)
		ReCreate();
}

ZEPhysicalJointMotorType ZEPhysXPhysicalJoint::GetLinearZMotor() const
{
	return ConvertNxMotorToZEMotor(JointDesc.zDrive.driveType);
}

void ZEPhysXPhysicalJoint::SetLinearZMotorForce(float LinearZMotorForce)
{
	JointDesc.zDrive.forceLimit = LinearZMotorForce;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetLinearZMotorForce() const
{
	return JointDesc.zDrive.forceLimit;
}

void ZEPhysXPhysicalJoint::SetLinearZMotorSpring(float LinearZMotorSpring)
{
	JointDesc.zDrive.spring = LinearZMotorSpring;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetLinearZMotorSpring() const
{
	return JointDesc.zDrive.spring;
}

void ZEPhysXPhysicalJoint::SetLinearZMotorDamper(float LinearZMotorDamper)
{
	JointDesc.zDrive.damping = LinearZMotorDamper;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetLinearZMotorDamper() const
{
	return JointDesc.zDrive.damping;
}

void ZEPhysXPhysicalJoint::SetAngularSwingMotor(ZEPhysicalJointMotorType AngularSwingMotor)
{
	JointDesc.swingDrive.driveType = ConvertZEMotorToNxMotor(AngularSwingMotor);

	if (Joint != NULL)
		ReCreate();
}

ZEPhysicalJointMotorType ZEPhysXPhysicalJoint::GetAngularSwingMotor() const
{
	return ConvertNxMotorToZEMotor(JointDesc.swingDrive.driveType);
}

void ZEPhysXPhysicalJoint::SetAngularSwingMotorForce(float AngularSwingMotorForce)
{
	JointDesc.swingDrive.forceLimit = AngularSwingMotorForce;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetAngularSwingMotorForce() const
{
	return JointDesc.swingDrive.forceLimit;
}

void ZEPhysXPhysicalJoint::SetAngularSwingMotorSpring(float AngularSwingMotorSpring)
{
	JointDesc.swingDrive.spring = AngularSwingMotorSpring;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetAngularSwingMotorSpring() const
{
	return JointDesc.swingDrive.spring;
}

void ZEPhysXPhysicalJoint::SetAngularSwingMotorDamper(float AngularSwingMotorDamper)
{
	JointDesc.swingDrive.damping = AngularSwingMotorDamper;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetAngularSwingMotorDamper() const
{
	return JointDesc.swingDrive.damping;
}

void ZEPhysXPhysicalJoint::SetAngularTwistMotor(ZEPhysicalJointMotorType AngularTwistMotor)
{
	JointDesc.twistDrive.driveType = ConvertZEMotorToNxMotor(AngularTwistMotor);

	if (Joint != NULL)
		ReCreate();
}

ZEPhysicalJointMotorType ZEPhysXPhysicalJoint::GetAngularTwistMotor() const
{
	return ConvertNxMotorToZEMotor(JointDesc.twistDrive.driveType);
}

void ZEPhysXPhysicalJoint::SetAngularTwistMotorForce(float AngularTwistMotorForce)
{
	JointDesc.twistDrive.forceLimit = AngularTwistMotorForce;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetAngularTwistMotorForce() const
{
	return JointDesc.twistDrive.forceLimit;
}

void ZEPhysXPhysicalJoint::SetAngularTwistMotorSpring(float AngularTwistMotorSpring)
{
	JointDesc.twistDrive.spring = AngularTwistMotorSpring;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetAngularTwistMotorSpring() const
{
	return JointDesc.twistDrive.spring;
}

void ZEPhysXPhysicalJoint::SetAngularTwistMotorDamper(float AngularTwistMotorDamper)
{
	JointDesc.twistDrive.damping = AngularTwistMotorDamper;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetAngularTwistMotorDamper() const
{
	return JointDesc.twistDrive.damping;
}

void ZEPhysXPhysicalJoint::SetAngularSlerpMotor(ZEPhysicalJointMotorType AngularSlerpMotor)
{
	JointDesc.slerpDrive.driveType = ConvertZEMotorToNxMotor(AngularSlerpMotor);

	if (Joint != NULL)
		ReCreate();
}

ZEPhysicalJointMotorType ZEPhysXPhysicalJoint::GetAngularSlerpMotor() const
{
	return ConvertNxMotorToZEMotor(JointDesc.slerpDrive.driveType);
}

void ZEPhysXPhysicalJoint::SetAngularSlerpMotorForce(float AngularSlerpMotorForce)
{
	JointDesc.slerpDrive.forceLimit = AngularSlerpMotorForce;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetAngularSlerpMotorForce() const
{
	return JointDesc.slerpDrive.forceLimit;
}

void ZEPhysXPhysicalJoint::SetAngularSlerpMotorSpring(float AngularSlerpMotorSpring)
{
	JointDesc.slerpDrive.spring = AngularSlerpMotorSpring;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetAngularSlerpMotorSpring() const
{
	return JointDesc.slerpDrive.spring;
}

void ZEPhysXPhysicalJoint::SetAngularSlerpMotorDamper(float AngularSlerpMotorDamper)
{
	JointDesc.slerpDrive.damping = AngularSlerpMotorDamper;

	if (Joint != NULL)
		ReCreate();
}

float ZEPhysXPhysicalJoint::GetAngularSlerpMotorDamper() const
{
	return JointDesc.slerpDrive.damping;
}

//////////////////////////////////////////////////////////////////////////

ZEPhysicalObjectType ZEPhysXPhysicalJoint::GetPhysicalObjectType()
{
	return ZE_POT_JOINT;
}

void ZEPhysXPhysicalJoint::SetEnabled(bool Enabled)
{

}

bool ZEPhysXPhysicalJoint::GetEnabled()
{
	return true;
}

void ZEPhysXPhysicalJoint::SetScale(const ZEVector3& NewScale)
{

}

ZEVector3 ZEPhysXPhysicalJoint::GetScale()
{
	return ZEVector3::Zero;
}

void ZEPhysXPhysicalJoint::SetCollisionEventFlags(ZEDWORD CollisionEventFlags)
{

}

ZEDWORD ZEPhysXPhysicalJoint::GetCollisionEventFlags()
{
	return NULL;
}

//////////////////////////////////////////////////////////////////////////


bool ZEPhysXPhysicalJoint::Initialize()
{
	if (Joint != NULL || PhysicalWorld == NULL || PhysicalWorld->GetScene() == NULL)
		return false;

	NxScene* Scene = PhysicalWorld->GetScene();
	Joint = (NxD6Joint*)Scene->createJoint(JointDesc);
	if(Joint == NULL)
	{
		zeError("Can not create joint.");
		return false;
	}

	return true;
}

void ZEPhysXPhysicalJoint::Deinitialize()
{
	if (Joint != NULL)
	{
		PhysicalWorld->GetScene()->releaseJoint(*Joint);	
		Joint = NULL;
	}
}

ZEPhysXPhysicalJoint::ZEPhysXPhysicalJoint()
{
	Joint = NULL;
	PhysicalWorld = NULL;
	//memset(&JointDesc, 0, sizeof(NxD6JointDesc));
	JointDesc.userData = this;
	JointDesc.projectionMode = NX_JPM_NONE;
}

ZEPhysXPhysicalJoint::~ZEPhysXPhysicalJoint()
{
	Deinitialize();
}
