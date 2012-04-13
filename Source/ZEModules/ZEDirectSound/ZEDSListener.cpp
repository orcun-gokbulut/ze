//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSListener.cpp
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

#include "ZEDSListener.h"
#define ZE_LISTENER_UPDATE_TRESHOLD 0.5f

void ZEDSListener::ResetParameters()
{
	if (IsActiveListener())
	{
		DS3DLISTENER Params;
		Params.dwSize = sizeof(DS3DLISTENER);
		
		const ZEVector3& WorldPosition = GetWorldPosition();
		Params.vPosition.x = WorldPosition.x;
		Params.vPosition.y = WorldPosition.y;
		Params.vPosition.z = WorldPosition.z;

		/*const ZEVector3& WorldVelocity = GetWorldVelocity();
		Params.vVelocity.x = WorldVelocity.x;
		Params.vVelocity.y = WorldVelocity.y;
		Params.vVelocity.z = WorldVelocity.z;*/
		
		Params.vVelocity.x = 0.0f;
		Params.vVelocity.y = 0.0f;
		Params.vVelocity.z = 0.0f;
		
		const ZEQuaternion& WorldRotation = GetWorldRotation();
		ZEVector3 Temp;
		ZEQuaternion::VectorProduct(Temp, WorldRotation, ZEVector3::UnitZ);
		Params.vOrientFront.x = Temp.x;
		Params.vOrientFront.y = Temp.y;
		Params.vOrientFront.z = Temp.z;

		ZEQuaternion::VectorProduct(Temp, WorldRotation, ZEVector3::UnitY);
		Params.vOrientTop.x = Temp.x;
		Params.vOrientTop.y = Temp.y;
		Params.vOrientTop.z = Temp.z;

		Params.flDistanceFactor = DistanceFactor;
		Params.flRolloffFactor = RollOffFactor;
		Params.flDopplerFactor = DopplerFactor;

		GetListener()->SetAllParameters(&Params, DS3D_DEFERRED);
	}
}

ZEDSListener::ZEDSListener()
{
}

ZEDSListener::~ZEDSListener()
{
	GetModule()->Listeners.DeleteValue(this);
}

void ZEDSListener::SetActiveListener()
{
	GetModule()->SetActiveListener(this);
}

bool ZEDSListener::IsActiveListener()
{
	return (GetModule()->GetActiveListener() == this);
}

void ZEDSListener::SetPosition(const ZEVector3& Position)
{
	ZEEntity::SetPosition(Position);

	if (IsActiveListener())
	{
		const ZEVector3& WorldPosition = GetWorldPosition();
		GetListener()->SetPosition(WorldPosition.x, WorldPosition.y, WorldPosition.z, DS3D_DEFERRED);
	}
}

void ZEDSListener::SetRotation(const ZEQuaternion& Rotation)
{
	ZEEntity::SetRotation(Rotation);

	if (IsActiveListener())
	{
		const ZEQuaternion& WorldRotation = GetWorldRotation();
		ZEVector3 Front, Top;
		ZEQuaternion::VectorProduct(Front, WorldRotation, ZEVector3::UnitZ);
		ZEQuaternion::VectorProduct(Top, WorldRotation, ZEVector3::UnitY);

		GetListener()->SetOrientation(Front.x, Front.y, Front.z, Top.x, Top.y, Top.z, DS3D_DEFERRED);
	}
}

void ZEDSListener::SetDistanceFactor(float Value)
{
	DistanceFactor = Value;

	if (IsActiveListener())
		GetListener()->SetDistanceFactor(Value, DS3D_DEFERRED);
}

void ZEDSListener::SetDopplerFactor(float Value)
{
	DopplerFactor = Value;

	if (IsActiveListener())
		GetListener()->SetDopplerFactor(Value, DS3D_DEFERRED);
}

void ZEDSListener::SetRollOffFactor(float Value)
{
	RollOffFactor = Value;

	if (IsActiveListener())
		GetModule()->GetListener()->SetRolloffFactor(Value,  DS3D_DEFERRED);
}

void ZEDSListener::OwnerWorldTransformChanged()
{
	if (IsActiveListener())
	{
		SetPosition(GetPosition());
		SetRotation(GetRotation());
	}
}

void ZEDSListener::Tick(float ElapsedTime)
{
	ZEEntity::Tick(ElapsedTime);
// 	if (IsActiveListener())
// 	{
// 		const ZEVector3& WorldVelocity = GetWorldVelocity();
// 		GetModule()->GetListener()->SetVelocity(WorldVelocity.x, WorldVelocity.y, WorldVelocity.z, DS3D_DEFERRED);	
// 	}
}




