//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESteeringCollisionAvoidance.cpp
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

#include "ZESteeringCollisionAvoidance.h"
#include "ZEActor.h"
#include "ZEMath\ZEMath.h"

ZESteeringOutput ZESteeringCollisionAvoidance::Process(float ElapsedTime)
{
	ZESteeringOutput Output;
	Output.SetZero();

	float ShortestTime = ZE_FLOAT_MAX;
	
	ZEActor* FirstTarget = NULL;
	float FirstMinSeperation;
	float FirstDistance;
	ZEVector3 FirstRelativePosition;
	ZEVector3 FirstRelativeVelocity;

	for (ZESize I = 0; I < AvoidedActors.GetCount(); I++)
	{
		ZEActor* CurrentTarget = AvoidedActors[I];

		ZEVector3 RelativePosition = CurrentTarget->GetPosition() - GetOwner()->GetPosition();
		ZEVector3 RelativeVelocity = CurrentTarget->GetLinearVelocity() - GetOwner()->GetLinearVelocity();
		float RelativeSpeed = RelativeVelocity.Length();

		float TimeToCollision = (ZEVector3::DotProduct(RelativePosition, RelativeVelocity) * RelativeSpeed * RelativeSpeed);

		float Distance = RelativePosition.Length();
		float MinSeparation = Distance - RelativeSpeed * ShortestTime;

		if (MinSeparation > GetOwner()->GetRadius() + CurrentTarget->GetRadius())
			continue;

		if (TimeToCollision > 0.0f && TimeToCollision < ShortestTime)
		{
			ShortestTime = TimeToCollision;
			FirstTarget = CurrentTarget;
			FirstMinSeperation = MinSeparation;
			FirstDistance = Distance;
			FirstRelativePosition = RelativePosition;
			FirstRelativeVelocity = RelativeVelocity;
		}
	}

	if (FirstTarget == NULL)
		return Output;

	if (FirstMinSeperation <= 0 || FirstDistance < GetOwner()->GetRadius() + FirstTarget->GetRadius())
		Output.LinearAcceleration = FirstTarget->GetPosition() - GetOwner()->GetPosition();
	else
		Output.LinearAcceleration = FirstRelativePosition + FirstRelativeVelocity * ShortestTime;

	Output.LinearAcceleration.NormalizeSelf();
	Output.LinearAcceleration = Output.LinearAcceleration * GetOwner()->GetMaxLinearAcceleration();

	return Output;
}

ZESteeringCollisionAvoidance::ZESteeringCollisionAvoidance()
{
	SetPriority(1);
}
