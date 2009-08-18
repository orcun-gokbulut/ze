//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysicsFreeJointInfo.h
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
#ifndef	__ZE_PHYSICS_FREE_JOINT_INFO_H__
#define __ZE_PHYSICS_FREE_JOINT_INFO_H__

class ZEPhysicsJointInfo;

class ZEPhysicsFreeJointInfo : public ZEPhysicsJointInfo
{
public:
	enum ZEPhysicsFreeJointMotion
	{
		ZE_PJM_FREE,
		ZE_PJM_LIMITED,
		ZE_PJM_LOCKED
	};

	enum ZEPhysicsFreeJointMotorTarget
	{
		ZE_PJMT_POSITION,
		ZE_PJMT_VELOCITY
	};

	ZEPhysicsFreeJointInfo();
	~ZEPhysicsFreeJointInfo(){}
	ZEPhysicsJointType GetType() { return ZEPhysicsJointInfo::ZE_PJT_FREE; }

	ZEPhysicsFreeJointMotion XMotion;
	ZEPhysicsFreeJointMotion YMotion;
	ZEPhysicsFreeJointMotion ZMotion;

	ZEPhysicsFreeJointMotion TwistMotion;
	ZEPhysicsFreeJointMotion Swing1Motion;
	ZEPhysicsFreeJointMotion Swing2Motion;

	float LinearLimitValue;
	float LinearLimitRestitution;
	float LinearLimitSpring;
	float LinearLimitDamper;

	float TwistLimitLowValue;
	float TwistLimitHighValue;
	float TwistLimitRestitution;
	float TwistLimitSpring;
	float TwistLimitDamper;

	float Swing1LimitValue;
	float Swing1LimitRestitution;
	float Swing1LimitSpring;
	float Swing1LimitDamper;

	float Swing2LimitValue;
	float Swing2LimitRestitution;
	float Swing2LimitSpring;
	float Swing2LimitDamper;

	ZEVector3 LinearMotorPosition;
	ZEVector3 LinearMotorVelocity;

	ZEPhysicsFreeJointMotorTarget LinearXMotor;
	float LinearXMotorForce;
	float LinearXMotorSpring;
	float LinearXMotorDamper;

	ZEPhysicsFreeJointMotorTarget LinearYMotor;
	float LinearYMotorForce;
	float LinearYMotorSpring;
	float LinearYMotorDamper;

	ZEPhysicsFreeJointMotorTarget LinearZMotor;
	float LinearZMotorForce;
	float LinearZMotorSpring;
	float LinearZMotorDamper;

	ZEQuaternion AngularMotorOrientation;
	ZEVector3 AngularMotorVelocity;
	ZEPhysicsFreeJointMotorTarget AngularMotor;
	float AngularMotorForce;
	float AngularMotorSpring;
	float AngularMotorDamper;
};

#endif
