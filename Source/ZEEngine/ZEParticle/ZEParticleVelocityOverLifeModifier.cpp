//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleVelocityOverLifeModifier.cpp
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

#include "ZEParticleVelocityOverLifeModifier.h"

#include "ZEMath\ZEMath.h"

ZEUInt ZEParticleVelocityOverLifeModifier::GetFlags() const
{
	return ZE_PEF_VELOCITY_PER_PARTICLE | ZE_PEF_TOTAL_LIFE_PER_PARTICLE;
}

void ZEParticleVelocityOverLifeModifier::SetFromVelocity(const ZEVector3& Velocity)
{
	this->FromVelocity = Velocity;
}

const ZEVector3& ZEParticleVelocityOverLifeModifier::GetFromVelocity() const
{
	return FromVelocity;
}

void ZEParticleVelocityOverLifeModifier::SetToVelocity(const ZEVector3& Velocity)
{
	this->ToVelocity = Velocity;
}

const ZEVector3& ZEParticleVelocityOverLifeModifier::GetToVelocity() const
{
	return ToVelocity;
}

void ZEParticleVelocityOverLifeModifier::Tick(float ElapsedTime)
{
	ZEParticlePool& ParticlePool = GetPool();
	const ZEArray<ZEUInt>& AliveParticleIndices = GetEmitter()->GetAliveParticleIndices();
	ZEUInt AliveParticleCount = GetEmitter()->GetAliveParticleCount();

	for (ZEUInt I = 0; I < AliveParticleCount; I++)
	{
		ZEUInt Index = AliveParticleIndices[I];
		float Weight = 1.0f - (ParticlePool.Lifes[Index] / ParticlePool.TotalLifes[Index]);

		ParticlePool.Velocities[Index].x = ZEMath::Lerp(FromVelocity.x, ToVelocity.x, Weight);
		ParticlePool.Velocities[Index].y = ZEMath::Lerp(FromVelocity.y, ToVelocity.y, Weight);
		ParticlePool.Velocities[Index].z = ZEMath::Lerp(FromVelocity.z, ToVelocity.z, Weight);
	}
}

ZEParticleVelocityOverLifeModifier::ZEParticleVelocityOverLifeModifier()
{
	ToVelocity = ZEVector3::One;
}

ZEParticleVelocityOverLifeModifier::~ZEParticleVelocityOverLifeModifier()
{

}
