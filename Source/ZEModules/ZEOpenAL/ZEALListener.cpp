//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEALListener.cpp
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

#include "ZEALListener.h"

void ZEALListener::LocalTransformChanged()
{
	UpdateOrientation();

	return ZEListener::LocalTransformChanged();
}

void ZEALListener::ParentTransformChanged()
{
	UpdateOrientation();

	ZEListener::ParentTransformChanged();
}

void ZEALListener::UpdateOrientation()
{
	if (!IsActiveListener())
		return;

	const ZEVector3& WorldPosition = GetWorldPosition();
	alListener3f(AL_POSITION, WorldPosition.x, WorldPosition.y, WorldPosition.z);

	ZEVector3 Orientation[2];
	Orientation[0] = GetWorldFront();
	Orientation[1] = GetWorldUp();
	alListenerfv(AL_ORIENTATION, (ALfloat*)Orientation);

	const ZEVector3& WorldVelocity = ZEVector3::Zero; /*GetWorldVelocity();*/
	alListener3f(AL_VELOCITY, WorldVelocity.x, WorldVelocity.y, WorldVelocity.z);
}

void ZEALListener::UpdateParameters()
{
	if (!IsActiveListener())
		return;

	alSpeedOfSound(DistanceFactor * 343.3f);
	alDopplerFactor(DopplerFactor);
}

ZEALListener::ZEALListener()
{
	DistanceFactor = 1.0f;
	RollOffFactor = 1.0f;
	DopplerFactor = 1.0f;

	UpdateParameters();

	SetEntityFlags(ZE_EF_TICKABLE);
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

void ZEALListener::SetDistanceFactor(float Value)
{
	if (DistanceFactor == Value)
		return;

	DistanceFactor = Value;

	UpdateParameters();
}

void ZEALListener::SetDopplerFactor(float Value)
{
	if (DopplerFactor == Value)
		return;

	DopplerFactor = Value;

	UpdateParameters();
}

void ZEALListener::SetRollOffFactor(float Value)
{
	if (RollOffFactor == Value)
		return;

	RollOffFactor = Value;

	UpdateOrientation();
}

void ZEALListener::Tick(float ElapsedTime)
{


	ZEEntity::Tick(ElapsedTime);
}
