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
	ZEArray<ZEParticle>& Particles = GetPool();
	ZESize ParticleCount = Particles.GetCount();
	ZEVector3 EmitterWorldPosition = GetEmitter()->GetEffect()->GetWorldPosition() + GetEmitter()->GetPosition();
	ZEVector3 EmitterUp = GetEmitter()->GetEffect()->GetWorldRotation() * -ZEVector3::UnitZ;
	ZEParticleEmitterType EmitterType = GetEmitter()->GetType();

	for (ZESize I = 0; I < ParticleCount; I++)
	{
		switch (Particles[I].State)
		{
			case ZE_PAS_NEW:
				if (IsRadialMovement)
				{
					Particles[I].InitialVelocity = ((Particles[I].Position - EmitterWorldPosition).Normalize()) * ZERandom::GetFloatRange(MinRadialSpeed, MaxRadialSpeed);
				
					if (EmitterType == ZE_PET_TORUS)
					{
						Particles[I].InitialVelocity.x -= ZEMath::Abs(EmitterUp.x) * Particles[I].InitialVelocity.x;
						Particles[I].InitialVelocity.y -= ZEMath::Abs(EmitterUp.y) * Particles[I].InitialVelocity.y;
						Particles[I].InitialVelocity.z -= ZEMath::Abs(EmitterUp.z) * Particles[I].InitialVelocity.z;
					}
				}
				else
				{
					Particles[I].InitialVelocity.x = ZERandom::GetFloatRange(MinVelocity.x, MaxVelocity.x);
					Particles[I].InitialVelocity.y = ZERandom::GetFloatRange(MinVelocity.y, MaxVelocity.y);
					Particles[I].InitialVelocity.z = ZERandom::GetFloatRange(MinVelocity.z, MaxVelocity.z);
				}

				Particles[I].Acceleration.x = ZERandom::GetFloatRange(MinAcceleration.x, MaxAcceleration.x);
				Particles[I].Acceleration.y = ZERandom::GetFloatRange(MinAcceleration.y, MaxAcceleration.y);
				Particles[I].Acceleration.z = ZERandom::GetFloatRange(MinAcceleration.z, MaxAcceleration.z);
				Particles[I].Velocity = Particles[I].InitialVelocity;
				break;

			case ZE_PAS_ALIVE:
				{
					float TotalElapsedTime = Particles[I].TotalLife - Particles[I].Life;
					Particles[I].Velocity = Particles[I].InitialVelocity + Particles[I].Acceleration * TotalElapsedTime;
					Particles[I].Position = Particles[I].InitialPositionWorld + ((Particles[I].InitialVelocity + Particles[I].Velocity) / 2) * TotalElapsedTime;
					
					break;
				}

			case ZE_PAS_DEAD:
				break;
		}
	}
}
