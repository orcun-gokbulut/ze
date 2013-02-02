//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleSphereGenerator.cpp
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

#include "ZEParticleSphereGenerator.h"
#include "ZEParticleEffect.h"
#include "ZEParticleSystem.h"
#include "ZEMath\ZEAngle.h"

ZE_OBJECT_IMPL(ZEParticleSphereGenerator)

void ZEParticleSphereGenerator::Tick(float ElapsedTime, ZEArray<ZEParticle>& OwnerParticlePool)
{
	if(GetIsSingleBurst() && SingleBurstParticleCounter >= GetSingleBurstMaxParticleCount())
		return;

	ParticlesRemaining += GetParticlesPerSecond() * ElapsedTime;
	ZEUInt ParticeCountForTimeElapsed = (ZEUInt)ParticlesRemaining;
	ParticlesRemaining -= ParticeCountForTimeElapsed;

	ZEUInt ParticlesEmmitedThisFrame = 0;

	if(ParticeCountForTimeElapsed != 0)
	{
		for (ZESize I = 0; I < OwnerParticlePool.GetCount(); I++)
		{
			if(OwnerParticlePool[I].State == ZE_PAS_DEAD)
			{
				ZEVector3 EffectPosition = GetOwner()->GetOwner()->GetWorldPosition();

				float Radius = RAND_BETWEEN_TWO_FLOAT(0.0f, this->Radius);
				float Theta = RAND_BETWEEN_TWO_FLOAT(0.0f, (float)ZE_PIx2);
				float Phi = RAND_BETWEEN_TWO_FLOAT(0.0f, ZE_PI);
				OwnerParticlePool[I].Position.x = EffectPosition.x + Radius * ZEAngle::Cos(Theta) * ZEAngle::Sin(Phi);
				OwnerParticlePool[I].Position.y = EffectPosition.y + Radius * ZEAngle::Sin(Theta) * ZEAngle::Sin(Phi);
				OwnerParticlePool[I].Position.z = EffectPosition.z + Radius * ZEAngle::Cos(Phi);	
				OwnerParticlePool[I].State = ZE_PAS_NEW;

				SingleBurstParticleCounter++;
				ParticlesEmmitedThisFrame++;
			}

			if(ParticlesEmmitedThisFrame == ParticeCountForTimeElapsed)
				break;
		}
	}
}

void ZEParticleSphereGenerator::SetRadius(float Size)
{
	Radius = Size;
}

float ZEParticleSphereGenerator::GetRadius() const
{
	return Radius;
}

ZEParticleSphereGenerator::ZEParticleSphereGenerator()
{
	Radius = 0.5f;
	ParticlesRemaining = 0.0f;
}

ZEParticleSphereGenerator::~ZEParticleSphereGenerator()
{

}
