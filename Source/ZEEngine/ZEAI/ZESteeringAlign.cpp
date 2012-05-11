//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESteeringAlign.cpp
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

#include "ZESteeringAlign.h"
#include "ZEActor.h"

#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEAngle.h"

void ZESteeringAlign::SetSlowRadius(float Radius)
{
	SlowRadius = Radius;
}

float ZESteeringAlign::GetSlowRadius()
{
	return SlowRadius;
}

ZESteeringOutput ZESteeringAlign::Align(const ZEQuaternion& TargetRotation)
{
	ZESteeringOutput Output;
	Output.SetZero();

	ZEQuaternion Rotation  = TargetRotation * GetOwner()->GetRotation().Conjugate();
	Rotation.NormalizeSelf();

	float RotationAngle;
	ZEVector3 RotatationAxis;
	ZEQuaternion::ConvertToAngleAxis(RotationAngle, RotatationAxis, Rotation);
 
	float RotationSize = ZEMath::Abs(RotationAngle);
	
	if (RotationSize == 0)
		return Output;

	float TargetSpeed;
	if (RotationSize > SlowRadius)
		TargetSpeed = GetOwner()->GetMaxAngularAcceleration();
	else
		TargetSpeed = GetOwner()->GetMaxAngularAcceleration() * RotationSize / SlowRadius;
	
	RotationAngle = (RotationAngle / RotationSize) * TargetSpeed;
	ZEQuaternion::CreateFromAngleAxis(Output.AngularAcceleration, RotationAngle, RotatationAxis);
	Output.AngularAcceleration = Output.AngularAcceleration * GetOwner()->GetAngularVelocity().Conjugate();
	
	return Output;
}

ZESteeringOutput ZESteeringAlign::Process(float ElapsedTime)
{
	return Align(GetTarget()->GetRotation());
}

ZESteeringAlign::ZESteeringAlign()
{
	SlowRadius = ZE_PI_8;
	SetPriority(3);
}
