//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysicalJoint.h
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
#ifndef	__ZE_PHYSICAL_JOINT_H__
#define __ZE_PHYSICAL_JOINT_H__

#include "ZEPhysicsModule.h"
#include "ZEPhysicalObject.h"

#include "ZEDS\ZEArray.h"
#include "ZEMath\ZEVector.h"
#include "ZEMath\ZEQuaternion.h"
#include "ZEMath\ZEPlane.h"

#include "NxJoint.h"

class	ZEPhysicalWorld;
class	ZEPhysicalObject;		

enum ZEPhysicalJointType
{
	ZE_PJT_NONE					= 0,
	ZE_PJT_SPHERICAL			= 1,
	ZE_PJT_REVOLUTE				= 2,
	ZE_PJT_PRISMATIC			= 3,
	ZE_PJT_CYLINDRIC			= 4,
	ZE_PJT_FIXED				= 5,
	ZE_PJT_DISTANCE				= 6,
	ZE_PJT_POINT_IN_PLANE		= 7,
	ZE_PJT_POINT_ON_LINE		= 8,
	ZE_PJT_PULLEY				= 9,
	ZE_PJT_FREE					= 10,
};

enum ZEPhysicalJointState
{
	ZE_PJS_UNBOUND				= 0,
	ZE_PJS_SIMULATING			= 1,
	ZE_PJS_BROKEN				= 2,
};

enum ZEPhysicalJointMotion
{
	ZE_PJMOTION_FREE			= 0,
	ZE_PJMOTION_LIMITED			= 1,
	ZE_PJMOTION_LOCKED			= 2,
};

enum ZEPhysicalJointProjectionMode
{
	ZE_PJPM_NONE				= 0,
	ZE_PJPM_POINT_MINDIST		= 1,
	ZE_PJPM_LINEAR_MINDIST		= 2,
};

enum ZEPhysicalJointFlags
{
	ZE_PJF_COLLISION_ENABLED	= 0,
	ZE_PJF_VISUALIZATION		= 1,
};

enum ZEPhysicalJointVisualizations
{
	ZE_PJV_LOCAL_AXES			= 0,
	ZE_PJV_WORLD_AXES			= 1,
	ZE_PJV_LIMITS				= 2,
};

enum ZEPhysicalJointMotorType
{
	ZE_PJMT_POSITION			= 1<<0,
	ZE_PJMT_VELOCITY			= 1<<1,
};

struct ZEPhysicalLimitPlane : ZEPlane
{
	float Restitution;
};

class ZEPhysicalJoint : public ZEPhysicalObject
{
	protected:
												ZEPhysicalJoint();
		virtual									~ZEPhysicalJoint();

	public:
		virtual ZEPhysicalWorld*				GetPhysicalWorld() = 0;

		//virtual ZEPhysicalJointState			GetJointState() const = 0;

		virtual void							SetPosition(const ZEVector3& NewPosition) = 0;
		virtual ZEVector3						GetPosition()= 0;

		virtual void							SetRotation(const ZEQuaternion& NewRotation) = 0;
		virtual ZEQuaternion					GetRotation() = 0;

		virtual void							SetBodyA(ZEPhysicalRigidBody* Object) = 0;
		virtual ZEPhysicalRigidBody*			GetBodyA() const = 0;

		virtual void							SetBodyAConnectionPosition(const ZEVector3& Position) = 0;
		virtual ZEVector3						GetBodyAConnectionPosition() const = 0;

		virtual void							SetBodyAConnectionOrientation(const ZEQuaternion& Orientation) = 0;
		virtual ZEQuaternion					GetBodyAConnectionOrientation() const = 0;

		virtual void							SetBodyB(ZEPhysicalRigidBody* Object) = 0;
		virtual ZEPhysicalRigidBody*			GetBodyB() const = 0;

		virtual void							SetBodyBConnectionPosition(const ZEVector3& Position) = 0;
		virtual ZEVector3						GetBodyBConnectionPosition() const = 0;

		virtual void							SetBodyBConnectionOrientation(const ZEQuaternion& Orientation) = 0;
		virtual ZEQuaternion					GetBodyBConnectionOrientation() const = 0;

		virtual void							SetBreakForce(float BreakForce) = 0;
		virtual float							GetBreakForce() const = 0;
	
		virtual void							SetBreakTorque(float BreakTorque) = 0;
		virtual float							GetBreakTorque() const = 0;

		virtual void							SetDrivePosition(const ZEVector3& Position) = 0;
		virtual ZEVector3						GetDrivePosition() const = 0;

		virtual void							SetDriveOrientation(const ZEQuaternion& Orientation) = 0;
		virtual ZEQuaternion					GetDriveOrientation() const = 0;

		virtual void							SetDriveLinearVelocity(const ZEVector3& LinearVelocity) = 0;
		virtual ZEVector3						GetDriveLinearVelocity() const = 0;

		virtual void							SetDriveAngularVelocity(const ZEVector3& AngularVelocity) = 0;
		virtual ZEVector3						GetDriveAngularVelocity() const = 0;

		virtual const 
		ZEArray<ZEPhysicalLimitPlane>&			GetLimitPlanes() const = 0;
		virtual void							AddLimitPlane(const ZEPhysicalLimitPlane& Plane) = 0;
		virtual void							RemoveLimitPlane(size_t Index) = 0;

		//Constraints

		virtual void							SetXMotion(ZEPhysicalJointMotion XMotion) = 0;
		virtual ZEPhysicalJointMotion			GetXMotion() const = 0;

		virtual void							SetYMotion(ZEPhysicalJointMotion YMotion) = 0;
		virtual ZEPhysicalJointMotion			GetYMotion() const = 0;

		virtual void							SetZMotion(ZEPhysicalJointMotion ZMotion) = 0;
		virtual ZEPhysicalJointMotion			GetZMotion() const = 0;

		virtual void							SetSwing1Motion(ZEPhysicalJointMotion Swing1Motion) = 0;
		virtual ZEPhysicalJointMotion			GetSwing1Motion() const = 0;

		virtual void							SetSwing2Motion(ZEPhysicalJointMotion Swing2Motion) = 0;
		virtual ZEPhysicalJointMotion			GetSwing2Motion() const = 0;

		virtual void							SetTwistMotion(ZEPhysicalJointMotion TwistMotion) = 0;
		virtual ZEPhysicalJointMotion			GetTwistMotion() const = 0;

		//Linear Limit

		virtual void							SetLinearLimitValue(float LinearLimitValue) = 0;
		virtual float							GetLinearLimitValue() const = 0;

		virtual void							SetLinearLimitRestitution(float LinearLimitRestitution) = 0;
		virtual float							GetLinearLimitRestitution() const = 0;

		virtual void							SetLinearLimitSpring(float LinearLimitSpring) = 0;
		virtual float							GetLinearLimitSpring() const = 0;

		virtual void							SetLinearLimitDamping(float LinearLimitDamping) = 0;
		virtual float							GetLinearLimitDamping() const = 0;

		//Swing1 Limit

		virtual void							SetSwing1LimitValue(float Swing1LimitValue) = 0;
		virtual float							GetSwing1LimitValue() const = 0;

		virtual void							SetSwing1LimitRestitution(float Swing1LimitRestitution) = 0;
		virtual float							GetSwing1LimitRestitution() const = 0;

		virtual void							SetSwing1LimitSpring(float Swing1LimitSpring) = 0;
		virtual float							GetSwing1LimitSpring() const = 0;

		virtual void							SetSwing1LimitDamping(float Swing1LimitDamping) = 0;
		virtual float							GetSwing1LimitDamping() const = 0;

		//Swing2 Limit

		virtual void							SetSwing2LimitValue(float Swing2LimitValue) = 0;
		virtual float							GetSwing2LimitValue() const = 0;

		virtual void							SetSwing2LimitRestitution(float Swing2LimitRestitution) = 0;
		virtual float							GetSwing2LimitRestitution() const = 0;

		virtual void							SetSwing2LimitSpring(float Swing2LimitSpring) = 0;
		virtual float							GetSwing2LimitSpring() const = 0;

		virtual void							SetSwing2LimitDamping(float Swing2LimitDamping) = 0;
		virtual float							GetSwing2LimitDamping() const = 0;

		//Twist Low Limit

		virtual void							SetTwistLowLimitValue(float TwistLowLimitValue) = 0;
		virtual float							GetTwistLowLimitValue() const = 0;

		virtual void							SetTwistLowLimitRestitution(float TwistLowLimitRestitution) = 0;
		virtual float							GetTwistLowLimitRestitution() const = 0;

		virtual void							SetTwistLowLimitSpring(float TwistLowLimitSpring) = 0;
		virtual float							GetTwistLowLimitSpring() const = 0;

		virtual void							SetTwistLowLimitDamping(float TwistLowLimitDamping) = 0;
		virtual float							GetTwistLowLimitDamping() const = 0;

		//Twist High Limit

		virtual void							SetTwistHighLimitValue(float TwistHighLimitValue) = 0;
		virtual float							GetTwistHighLimitValue() const = 0;

		virtual void							SetTwistHighLimitRestitution(float TwistHighLimitRestitution) = 0;
		virtual float							GetTwistHighLimitRestitution() const = 0;

		virtual void							SetTwistHighLimitSpring(float TwistHighLimitSpring) = 0;
		virtual float							GetTwistHighLimitSpring() const = 0;

		virtual void							SetTwistHighLimitDamping(float TwistHighLimitDamping) = 0;
		virtual float							GetTwistHighLimitDamping() const = 0;

		//Joint Motors : Currently NVidia PhysX supporting D6, Revolute and Pulley Joint Motors

		virtual void							SetMotorTargetPosition(ZEVector3 MotorTargetPosition) = 0;
		virtual ZEVector3						GetMotorTargetPosition() const = 0;

		virtual void							SetMotorTargetOrientation(ZEQuaternion MotorTargetOrientation) = 0;
		virtual ZEQuaternion					GetMotorTargetOrientation() const = 0;

		virtual void							SetMotorTargetVelocity(ZEVector3 MotorTargetVelocity) = 0;
		virtual ZEVector3						GetMotorTargetVelocity() const = 0;

		virtual void							SetMotorTargetAngularVelocity(ZEVector3 MotorTargetAngularVelocity) = 0;
		virtual ZEVector3						GetMotorTargetAngularVelocity() const = 0;

		virtual void							SetLinearXMotor(ZEPhysicalJointMotorType LinearXMotor) = 0;
		virtual ZEPhysicalJointMotorType		GetLinearXMotor() const = 0;

		virtual void							SetLinearXMotorForce(float LinearXMotorForce) = 0;
		virtual float							GetLinearXMotorForce() const = 0;

		virtual void							SetLinearXMotorSpring(float LinearXMotorSpring) = 0;
		virtual float							GetLinearXMotorSpring() const = 0;

		virtual void							SetLinearXMotorDamper(float LinearXMotorDamper) = 0;
		virtual float							GetLinearXMotorDamper() const = 0;

		virtual void							SetLinearYMotor(ZEPhysicalJointMotorType LinearYMotor) = 0;
		virtual ZEPhysicalJointMotorType		GetLinearYMotor() const = 0;

		virtual void							SetLinearYMotorForce(float LinearYMotorForce) = 0;
		virtual float							GetLinearYMotorForce() const = 0;

		virtual void							SetLinearYMotorSpring(float LinearYMotorSpring) = 0;
		virtual float							GetLinearYMotorSpring() const = 0;

		virtual void							SetLinearYMotorDamper(float LinearYMotorDamper) = 0;
		virtual float							GetLinearYMotorDamper() const = 0;

		virtual void							SetLinearZMotor(ZEPhysicalJointMotorType LinearZMotor) = 0;
		virtual ZEPhysicalJointMotorType		GetLinearZMotor() const = 0;

		virtual void							SetLinearZMotorForce(float LinearZMotorForce) = 0;
		virtual float							GetLinearZMotorForce() const = 0;

		virtual void							SetLinearZMotorSpring(float LinearZMotorSpring) = 0;
		virtual float							GetLinearZMotorSpring() const = 0;

		virtual void							SetLinearZMotorDamper(float LinearZMotorDamper) = 0;
		virtual float							GetLinearZMotorDamper() const = 0;

		virtual void							SetAngularSwingMotor(ZEPhysicalJointMotorType AngularSwingMotor) = 0;
		virtual ZEPhysicalJointMotorType		GetAngularSwingMotor() const = 0;

		virtual void							SetAngularSwingMotorForce(float AngularSwingMotorForce) = 0;
		virtual float							GetAngularSwingMotorForce() const = 0;

		virtual void							SetAngularSwingMotorSpring(float AngularSwingMotorSpring) = 0;
		virtual float							GetAngularSwingMotorSpring() const = 0;

		virtual void							SetAngularSwingMotorDamper(float AngularSwingMotorDamper) = 0;
		virtual float							GetAngularSwingMotorDamper() const = 0;

		virtual void							SetAngularTwistMotor(ZEPhysicalJointMotorType AngularTwistMotor) = 0;
		virtual ZEPhysicalJointMotorType		GetAngularTwistMotor() const = 0;

		virtual void							SetAngularTwistMotorForce(float AngularTwistMotorForce) = 0;
		virtual float							GetAngularTwistMotorForce() const = 0;

		virtual void							SetAngularTwistMotorSpring(float AngularTwistMotorSpring) = 0;
		virtual float							GetAngularTwistMotorSpring() const = 0;

		virtual void							SetAngularTwistMotorDamper(float AngularTwistMotorDamper) = 0;
		virtual float							GetAngularTwistMotorDamper() const = 0;

		virtual void							SetAngularSlerpMotor(ZEPhysicalJointMotorType AngularSlerpMotor) = 0;
		virtual ZEPhysicalJointMotorType		GetAngularSlerpMotor() const = 0;

		virtual void							SetAngularSlerpMotorForce(float AngularSlerpMotorForce) = 0;
		virtual float							GetAngularSlerpMotorForce() const = 0;

		virtual void							SetAngularSlerpMotorSpring(float AngularSlerpMotorSpring) = 0;
		virtual float							GetAngularSlerpMotorSpring() const = 0;

		virtual void							SetAngularSlerpMotorDamper(float AngularSlerpMotorDamper) = 0;
		virtual float							GetAngularSlerpMotorDamper() const = 0;

//		virtual void							SetDefaultValues(ZEPhysicalJointType Type = ZE_PJT_NONE) = 0;

		virtual bool							Initialize() = 0;
		virtual void							Deinitialize() = 0;

		virtual void							ReCreate() = 0;

		virtual void							Destroy();
		
		static ZEPhysicalJoint*					CreateInstance();
};

#endif
