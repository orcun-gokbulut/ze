//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticlePhysicsModifier.cpp
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

#include "ZEParticlePhysicsModifier.h"

#include "ZEMath/ZEMath.h"
#include "ZEParticleEffect.h"
#include "ZEParticleEmitter.h"

ZEUInt ZEParticlePhysicsModifier::GetFlags() const
{
	return ZE_PEF_VELOCITY_PER_PARTICLE | ZE_PEF_ACCELERATION_PER_PARTICLE;
}

void ZEParticlePhysicsModifier::SetRadialMovement(bool Enabled)
{
	IsRadialMovement = Enabled;
}

void ZEParticlePhysicsModifier::SetRadialSpeed(float Max, float Min)
{
	MaxRadialSpeed = Max;
	MinRadialSpeed = Min;
}

void ZEParticlePhysicsModifier::SetMinAcceleration(const ZEVector3& Acceleration)
{
	MinAcceleration = Acceleration;
}

const ZEVector3& ZEParticlePhysicsModifier::GetMinAcceleration() const
{
	return MinAcceleration;
}

void ZEParticlePhysicsModifier::SetMaxAcceleration(const ZEVector3& Acceleration)
{
	MaxAcceleration = Acceleration;
}

const ZEVector3& ZEParticlePhysicsModifier::GetMaxAcceleration() const
{
	return MaxAcceleration;
}

void ZEParticlePhysicsModifier::SetMinVelocity(const ZEVector3& Velocity)
{
	MinVelocity = Velocity;
}

const ZEVector3& ZEParticlePhysicsModifier::GetMinVelocity() const
{
	return MinVelocity;
}

void ZEParticlePhysicsModifier::SetMaxVelocity(const ZEVector3& Velocity)
{
	MaxVelocity = Velocity;
}

const ZEVector3& ZEParticlePhysicsModifier::GetMaxVelocity() const
{
	return MaxVelocity;
}

ZEParticlePhysicsModifier::ZEParticlePhysicsModifier()
{
	IsRadialMovement = false;
	MaxRadialSpeed = 0.0f;
	MinRadialSpeed = 0.0f;

	MinAcceleration = ZEVector3(0.0f, 0.0f, 0.0f);
	MaxAcceleration = ZEVector3(0.0f, 0.0f, 0.0f);
	MinVelocity = ZEVector3(0.0f, 0.0f, 0.0f);
	MaxVelocity = ZEVector3(0.0f, 0.0f, 0.0f);
}

ZEParticlePhysicsModifier::~ZEParticlePhysicsModifier()
{

}

void ZEParticlePhysicsModifier::Tick(float ElapsedTime)
{
	ZEParticlePool& ParticlePool = GetPool();

	ZEUInt AliveParticleCount = GetEmitter()->GetAliveParticleCount();
	ZEArray<ZEUInt> AliveParticleIndices = GetEmitter()->GetAliveParticleIndices();
	ZEArray<ZEUInt> DeadParticleIndices = GetEmitter()->GetDeadParticleIndices();

	ZEUInt DeadParticleStartIndex = GetEmitter()->GetDeadParticleStartIndex();
	ZEUInt SpawnedParticleCount = GetEmitter()->GetLastSpawnedParticleCount();
	for (ZEUInt I = DeadParticleStartIndex; I < (DeadParticleStartIndex + SpawnedParticleCount); I++)
	{
		ZEUInt Index = DeadParticleIndices[I % DeadParticleIndices.GetCount()];

		ParticlePool.Accelerations[Index].x = ZERandom::GetFloatRange(MinAcceleration.x, MaxAcceleration.x);
		ParticlePool.Accelerations[Index].y = ZERandom::GetFloatRange(MinAcceleration.y, MaxAcceleration.y);
		ParticlePool.Accelerations[Index].z = ZERandom::GetFloatRange(MinAcceleration.z, MaxAcceleration.z);

		ParticlePool.Velocities[Index].x = ZERandom::GetFloatRange(MinVelocity.x, MaxVelocity.x);
		ParticlePool.Velocities[Index].y = ZERandom::GetFloatRange(MinVelocity.y, MaxVelocity.y);
		ParticlePool.Velocities[Index].z = ZERandom::GetFloatRange(MinVelocity.z, MaxVelocity.z);
	}

	for (ZEUInt I = 0; I < AliveParticleCount; I++)
	{
		ZEUInt Index = AliveParticleIndices[I];
		ZEVector3 OldVelocity = ParticlePool.Velocities[Index];
		ParticlePool.Velocities[Index] += ParticlePool.Accelerations[Index] * ElapsedTime;
		ParticlePool.Positions[Index] += ((OldVelocity + ParticlePool.Velocities[Index]) / 2.0f) * ElapsedTime;
	}
}
