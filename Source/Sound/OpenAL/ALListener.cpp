//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ALListener.cpp
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

#include "ALListener.h"
#define ZE_LISTENER_UPDATE_TRESHOLD 0.5f

void ZEALListener::ResetParameters()
{
	if (IsActiveListener())
	{
		const ZEVector3& WorldPosition = GetWorldPosition();
		alListener3f(AL_POSITION, WorldPosition.x, WorldPosition.y, WorldPosition.z);

		float Orientation[6];
		const ZEQuaternion& WorldRotation = GetWorldRotation();
		ZEVector3 Temp;
		ZEQuaternion::VectorProduct(Temp, WorldRotation, ZEVector3::UnitZ);
		Orientation[0] = Temp.x;
		Orientation[1] = Temp.y;
		Orientation[2] = Temp.z;
		ZEQuaternion::VectorProduct(Temp, WorldRotation, ZEVector3::UnitY);
		Orientation[3] = Temp.x;
		Orientation[4] = Temp.y;
		Orientation[5] = Temp.z;
		alListenerfv(AL_ORIENTATION, Orientation);

		const ZEVector3& WorldVelocity = GetWorldVelocity();
		alListener3f(AL_VELOCITY, WorldVelocity.x, WorldVelocity.y, WorldVelocity.z);

		alSpeedOfSound(DistanceFactor * 343.3f);
		alDopplerFactor(DopplerFactor);
		// Roll off factor
		// Update all sound sources roll off factor
	}
}

ZEALListener::ZEALListener()
{

}

ZEALListener::~ZEALListener()
{

}

void ZEALListener::SetActiveListener()
{
	GetModule()->SetActiveListener(this);
}

bool ZEALListener::IsActiveListener()
{
	return (GetModule()->GetActiveListener() == this);
}

void ZEALListener::SetPosition(const ZEVector3& Position)
{
	ZEEntityComponent::SetPosition(Position);

	if (IsActiveListener())
	{
		const ZEVector3& WorldPosition = GetWorldPosition();
		alListener3f(AL_POSITION, WorldPosition.x, WorldPosition.y, WorldPosition.z);
	}
}

void ZEALListener::SetRotation(const ZEQuaternion& Rotation)
{
	ZEEntityComponent::SetRotation(Rotation);

	if (IsActiveListener())
	{
		float Orientation[6];
		const ZEQuaternion& WorldRotation = GetWorldRotation();
		ZEVector3 Temp;
		ZEQuaternion::VectorProduct(Temp, WorldRotation, ZEVector3::UnitZ);
		Orientation[0] = Temp.x;
		Orientation[1] = Temp.y;
		Orientation[2] = Temp.z;
		ZEQuaternion::VectorProduct(Temp, WorldRotation, ZEVector3::UnitY);
		Orientation[3] = Temp.x;
		Orientation[4] = Temp.y;
		Orientation[5] = Temp.z;
		
		alListenerfv(AL_ORIENTATION, Orientation);
	}
}

void ZEALListener::SetDistanceFactor(float Value)
{
	DistanceFactor = Value;
	if(IsActiveListener())
		alSpeedOfSound(Value * 343.3f);
}

void ZEALListener::SetDopplerFactor(float Value)
{
	DopplerFactor = Value;
	ListenerDirtyFlag = true;

	if (IsActiveListener())
		alDopplerFactor(Value);
}

void ZEALListener::SetRollOffFactor(float Value)
{
	RollOffFactor = Value;
	ListenerDirtyFlag = true;

	if (IsActiveListener())
	{
		// Not Supported
	}
}

void ZEALListener::OwnerWorldTransformChanged()
{
	if (IsActiveListener())
	{
		const ZEVector3& WorldPosition = GetWorldPosition();
		alListener3f(AL_POSITION, WorldPosition.x, WorldPosition.y, WorldPosition.z);

		float Orientation[6];
		const ZEQuaternion& WorldRotation = GetWorldRotation();
		ZEVector3 Temp;
		ZEQuaternion::VectorProduct(Temp, WorldRotation, ZEVector3::UnitZ);
		Orientation[0] = Temp.x;
		Orientation[1] = Temp.y;
		Orientation[2] = Temp.z;
		ZEQuaternion::VectorProduct(Temp, WorldRotation, ZEVector3::UnitY);
		Orientation[3] = Temp.x;
		Orientation[4] = Temp.y;
		Orientation[5] = Temp.z;
		alListenerfv(AL_ORIENTATION, Orientation);

		const ZEVector3& WorldVelocity = GetWorldVelocity();
		alListener3f(AL_VELOCITY, WorldVelocity.x, WorldVelocity.y, WorldVelocity.z);
	}
}

void ZEALListener::Tick(float ElapsedTime)
{
	ZEEntityComponent::Tick(ElapsedTime);

	const ZEVector3& WorldVelocity = GetWorldVelocity();
	alListener3f(AL_VELOCITY, WorldVelocity.x, WorldVelocity.y, WorldVelocity.z);
}
