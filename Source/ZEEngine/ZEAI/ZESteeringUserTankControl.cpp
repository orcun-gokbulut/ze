//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESteeringUserTankControl.cpp
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

#include "ZESteeringUserTankControl.h"
#include "ZEActor.h"
#include "ZEMath/ZEMath.h"
#include "ZEInput/ZEInputDefinitions.h"
#include "ZEInput/ZEInputModule.h"

#define ACTIONID_FORWARD		0
#define ACTIONID_BACKWARD		1
#define ACTIONID_STRAFELEFT		2
#define ACTIONID_STRAFERIGHT	3
#define ACTIONID_TURNLEFT		4
#define ACTIONID_TURNRIGHT		5
#define ACTIONID_TURNUP			6
#define ACTIONID_TURNDOWN		7

ZEStreeringUserTankControl::ZEStreeringUserTankControl()
{
	InputMap.AddButtonAction("Keyboard", ZE_IKB_W, ZE_IS_PRESSING, ACTIONID_FORWARD	);
	InputMap.AddButtonAction("Keyboard", ZE_IKB_S, ZE_IS_PRESSING, ACTIONID_BACKWARD);
	InputMap.AddButtonAction("Keyboard", ZE_IKB_D, ZE_IS_PRESSING, ACTIONID_STRAFELEFT);
	InputMap.AddButtonAction("Keyboard", ZE_IKB_A, ZE_IS_PRESSING, ACTIONID_STRAFERIGHT);
	InputMap.AddAxisAction("Mouse", ZE_IMA_VERTICAL_AXIS, ZE_IAS_POSITIVE, ZE_IS_CHANGED, ACTIONID_TURNLEFT);
	InputMap.AddAxisAction("Mouse", ZE_IMA_VERTICAL_AXIS, ZE_IAS_NEGATIVE, ZE_IS_CHANGED, ACTIONID_TURNRIGHT);
	InputMap.AddAxisAction("Mouse", ZE_IMA_HORIZANTAL_AXIS, ZE_IAS_POSITIVE, ZE_IS_CHANGED, ACTIONID_TURNUP);
	InputMap.AddAxisAction("Mouse", ZE_IMA_HORIZANTAL_AXIS, ZE_IAS_NEGATIVE, ZE_IS_CHANGED, ACTIONID_TURNDOWN);
}

ZESteeringOutput ZEStreeringUserTankControl::Process(float ElapsedTime)
{
	ZESteeringOutput Output;
	Output.SetZero();

	InputMap.Update();

	float Friction = 1.0f;

	ZEQuaternion OwnerRotation = GetOwner()->GetRotation();

	bool Linear = false;
	bool Angular = false;
	for (size_t I = 0; I < InputMap.GetActionCount(); I++)
	{
		const ZEInputAction* Current = &InputMap.GetActions()[I];
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
			Output.AngularAcceleration = ZEQuaternion(-GetOwner()->GetMaxAngularAcceleration(), ZEVector3::UnitY);
			Angular = true;
			break;
		case ACTIONID_STRAFERIGHT:
			Output.AngularAcceleration = ZEQuaternion(GetOwner()->GetMaxAngularAcceleration(), ZEVector3::UnitY);
			Angular = true;
			break;
		}
	}

	if (!Linear && GetOwner()->GetLinearVelocity().LengthSquare() != 0)
		Output.LinearAcceleration = -GetOwner()->GetLinearVelocity().Normalize() * Friction * GetOwner()->GetMaxLinearAcceleration();

	if (!Angular)
	{
		float RotationAngle;
		ZEVector3 RotationAxis;
		ZEQuaternion::ConvertToAngleAxis(RotationAngle, RotationAxis, GetOwner()->GetAngularVelocity());
		RotationAngle = -RotationAngle * Friction;
		ZEQuaternion::CreateFromAngleAxis(Output.AngularAcceleration, RotationAngle, RotationAxis);
	}

	return Output;
}
