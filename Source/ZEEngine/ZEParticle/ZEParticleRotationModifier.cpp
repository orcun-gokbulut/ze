//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleRotationModifier.cpp
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

#include "ZEParticleRotationModifier.h"

void ZEParticleRotationModifier::SetMinAngularAcceleration(float AngularAcceleration)
{
	MinAngularAcceleration = AngularAcceleration;
}

float ZEParticleRotationModifier::GetMinAngularAcceleration() const
{
	return MinAngularAcceleration;
}

void ZEParticleRotationModifier::SetMaxAngularAcceleration(float AngularAcceleration)
{
	MaxAngularAcceleration = AngularAcceleration;
}

float ZEParticleRotationModifier::GetMaxAngularAcceleration() const
{
	return MaxAngularAcceleration;
}

void ZEParticleRotationModifier::SetMinAngularVelocity(float AngularVelocity)
{
	MinAngularVelocity = AngularVelocity;
}

float ZEParticleRotationModifier::GetMinAngularVelocity() const
{
	return MinAngularVelocity;
}

void ZEParticleRotationModifier::SetMaxAngularVelocity(float AngularVelocity)
{
	MaxAngularVelocity = AngularVelocity;
}

float ZEParticleRotationModifier::GetMaxAngularVelocity() const
{
	return MaxAngularVelocity;
}

void ZEParticleRotationModifier::SetMaxRotation(float MaxRotation)
{
	this->MaxRotation = MaxRotation;
}

float ZEParticleRotationModifier::GetMaxRotation() const
{
	return MaxRotation;
}

void ZEParticleRotationModifier::SetMinRotation(float MinRotation)
{
	this->MinRotation = MinRotation;
}

float ZEParticleRotationModifier::GetMinRotation() const
{
	return MinRotation;
}

void ZEParticleRotationModifier::Tick(float ElapsedTime)
{
	ZEArray<ZEParticle>& Particles = GetPool();
	ZESize ParticlCount = Particles.GetCount();

	for (ZESize I = 0; I < ParticlCount; I++)
	{
		switch (Particles[I].State)
		{
			case ZE_PAS_NEW:
				Particles[I].Rotation = ZERandom::GetFloatRange(MinRotation, MaxRotation);
				Particles[I].Cos_NegSin.x = cos(Particles[I].Rotation);
				Particles[I].Cos_NegSin.y = -sin(Particles[I].Rotation);
				Particles[I].AngularAcceleration = ZERandom::GetFloatRange(MinAngularAcceleration, MaxAngularAcceleration);
				Particles[I].AngularVelocity = ZERandom::GetFloatRange(MinAngularVelocity, MaxAngularVelocity);
				break;

			case ZE_PAS_ALIVE:
				Particles[I].AngularVelocity += Particles[I].AngularAcceleration * ElapsedTime;
				Particles[I].Rotation += Particles[I].AngularVelocity * ElapsedTime;

				if(Particles[I].AngularVelocity != 0.0f)
				{
					Particles[I].Cos_NegSin.x = cos(Particles[I].Rotation);
					Particles[I].Cos_NegSin.y = -sin(Particles[I].Rotation);
				}
				break;

			case ZE_PAS_DEAD:
				break;
		}
	}
}

ZEParticleRotationModifier::ZEParticleRotationModifier()
{
	MinAngularAcceleration = 0.0f;		
	MaxAngularAcceleration = 0.0f;		
	MinAngularVelocity = 0.0f;			
	MaxAngularVelocity = 0.0f;
	MaxRotation = 0.0f;
	MinRotation = 0.0f;
}

ZEParticleRotationModifier::~ZEParticleRotationModifier()
{

}
