//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysicsFreeJointInfo.cpp
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

#include "ZEMath/Vector.h"
#include "ZEMath/Quaternion.h"
#include "Physics/PhysicsBody.h"
#include "Physics/PhysicsJointInfo.h"
#include "Physics/PhysicsFreeJointInfo.h"

ZEPhysicsFreeJointInfo::
ZEPhysicsFreeJointInfo() :

TwistMotion(ZE_PJM_LOCKED), Swing1Motion(ZE_PJM_LOCKED), Swing2Motion(ZE_PJM_LOCKED),

XMotion(ZE_PJM_LOCKED), YMotion(ZE_PJM_LOCKED), ZMotion(ZE_PJM_LOCKED),

LinearLimitValue(0), LinearLimitRestitution(0),
LinearLimitSpring(0), LinearLimitDamper(0),

TwistLimitLowValue(0), TwistLimitHighValue(0), TwistLimitRestitution(0),
TwistLimitSpring(0), TwistLimitDamper(0),

Swing1LimitValue(0), Swing1LimitRestitution(0),
Swing1LimitSpring(0), Swing1LimitDamper(0),

Swing2LimitValue(0), Swing2LimitRestitution(0),
Swing2LimitSpring(0), Swing2LimitDamper(0),

LinearMotorPosition(0,0,0), LinearMotorVelocity(0,0,0),

LinearXMotor(ZE_PJMT_VELOCITY), LinearXMotorForce(0),
LinearXMotorSpring(0), LinearXMotorDamper(0),

LinearYMotor(ZE_PJMT_VELOCITY), LinearYMotorForce(0),
LinearYMotorSpring(0), LinearYMotorDamper(0),

LinearZMotor(ZE_PJMT_VELOCITY), LinearZMotorForce(0),
LinearZMotorSpring(0), LinearZMotorDamper(0),

AngularMotorOrientation(1,0,0,0), AngularMotorVelocity(0,0,0),
AngularMotor(ZE_PJMT_VELOCITY), AngularMotorForce(0),
AngularMotorSpring(0),AngularMotorDamper(0)

{
}
