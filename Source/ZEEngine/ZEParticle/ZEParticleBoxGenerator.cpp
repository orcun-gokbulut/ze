//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleBoxGenerator.cpp
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

#include "ZEParticleBoxGenerator.h"
#include "ZEParticleEffect.h"
#include "ZEParticleSystem.h"

ZE_OBJECT_IMPL(ZEParticleBoxGenerator)

void ZEParticleBoxGenerator::Tick(float ElapsedTime, ZEArray<ZEParticle>& OwnerParticlePool)
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

				OwnerParticlePool[I].Position.x = EffectPosition.x + RAND_BETWEEN_TWO_FLOAT(-BoxSize.x / 2.0f, BoxSize.x / 2.0f);
				OwnerParticlePool[I].Position.y = EffectPosition.y + RAND_BETWEEN_TWO_FLOAT(-BoxSize.y / 2.0f, BoxSize.y / 2.0f);
				OwnerParticlePool[I].Position.z = EffectPosition.z + RAND_BETWEEN_TWO_FLOAT(-BoxSize.z / 2.0f, BoxSize.z / 2.0f);
				OwnerParticlePool[I].State = ZE_PAS_NEW;

				SingleBurstParticleCounter++;
				ParticlesEmmitedThisFrame++;
			}

			if(ParticlesEmmitedThisFrame == ParticeCountForTimeElapsed)
			{
				break;
			}
		}
	}
}

void ZEParticleBoxGenerator::SetBoxSize(const ZEVector3& Size)
{
	BoxSize = Size;
}

const ZEVector3& ZEParticleBoxGenerator::GetBoxSize() const
{
	return BoxSize;
}

ZEParticleBoxGenerator::ZEParticleBoxGenerator()
{
	BoxSize = ZEVector3::One;
	ParticlesRemaining = 0.0f;
}

ZEParticleBoxGenerator::~ZEParticleBoxGenerator()
{

}
