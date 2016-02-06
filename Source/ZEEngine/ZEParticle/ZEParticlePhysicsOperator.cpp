//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticlePhysicsOperator.cpp
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

#include "ZEParticlePhysicsOperator.h"

#include "ZEParticle.h"

void ZEParticlePhysicsOperator::SetMinAcceleration(const ZEVector3& Acceleration)
{
	MinAcceleration = Acceleration;
}

const ZEVector3& ZEParticlePhysicsOperator::GetMinAcceleration() const
{
	return MinAcceleration;
}

void ZEParticlePhysicsOperator::SetMaxAcceleration(const ZEVector3& Acceleration)
{
	MaxAcceleration = Acceleration;
}

const ZEVector3& ZEParticlePhysicsOperator::GetMaxAcceleration() const
{
	return MaxAcceleration;
}

void ZEParticlePhysicsOperator::SetMinVelocity(const ZEVector3& Velocity)
{
	MinVelocity = Velocity;
}

const ZEVector3& ZEParticlePhysicsOperator::GetMinVelocity() const
{
	return MinVelocity;
}

void ZEParticlePhysicsOperator::SetMaxVelocity(const ZEVector3& Velocity)
{
	MaxVelocity = Velocity;
}

const ZEVector3& ZEParticlePhysicsOperator::GetMaxVelocity() const
{
	return MaxVelocity;
}

void ZEParticlePhysicsOperator::Tick(float ElapsedTime, ZEArray<ZEParticle>& OwnerParticlePool)
{
	ZESize ParticlCount = OwnerParticlePool.GetCount();

	for (ZESize I = 0; I < ParticlCount; I++)
	{
		switch(OwnerParticlePool[I].State)
		{
		case ZE_PAS_NEW:

			CustomData[I].Acceleration.x				= RAND_BETWEEN_TWO_FLOAT(MinAcceleration.x, MaxAcceleration.x);
			CustomData[I].Acceleration.y				= RAND_BETWEEN_TWO_FLOAT(MinAcceleration.y, MaxAcceleration.y);
			CustomData[I].Acceleration.z				= RAND_BETWEEN_TWO_FLOAT(MinAcceleration.z, MaxAcceleration.z);

			CustomData[I].Velocity.x					= RAND_BETWEEN_TWO_FLOAT(MinVelocity.x, MaxVelocity.x);
			CustomData[I].Velocity.y					= RAND_BETWEEN_TWO_FLOAT(MinVelocity.y, MaxVelocity.y);
			CustomData[I].Velocity.z					= RAND_BETWEEN_TWO_FLOAT(MinVelocity.z, MaxVelocity.z);
			break;

		case ZE_PAS_ALIVE:

			OwnerParticlePool[I].Position += CustomData[I].Velocity * ElapsedTime;
			CustomData[I].Velocity += CustomData[I].Acceleration * ElapsedTime;

			break;

		case ZE_PAS_DEAD:
			break;
		}
	}
}

void ZEParticlePhysicsOperator::ResizeCustomDataPool(ZESize NewPoolSize)
{
	CustomData.Resize(NewPoolSize);
}

ZEParticlePhysicsOperator::ZEParticlePhysicsOperator()
{
	CustomData.Clear();
}

ZEParticlePhysicsOperator::~ZEParticlePhysicsOperator()
{

}
