//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleEffect.cpp
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

#include "ZEParticleEffect.h"
#include "ZEGame/ZEEntityProvider.h"
#include "ZEGame/ZEDrawParameters.h"

ZE_META_REGISTER_CLASS(ZEEntityProvider, ZEParticleEffect);
		
ZEDrawFlags ZEParticleEffect::GetDrawFlags() const
{
	return ZE_DF_DRAW | ZE_DF_CULL;
}

bool ZEParticleEffect::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	return true;
}

bool ZEParticleEffect::DeinitializeSelf()
{
	return ZEEntity::DeinitializeSelf();
}

void ZEParticleEffect::Draw(ZEDrawParameters* DrawParameters)
{
	if(!GetVisible())
		return;

	if (DrawParameters->Pass == ZE_RP_COLOR)
	{
		memset(&Statistics, 0, sizeof(ZEParticleStatistics));

		Statistics.EmitterCount = Emitters.GetCount();
	}

	for(ZESize I = 0; I < Emitters.GetCount(); I++)
		Emitters[I]->Draw(DrawParameters);

	for(ZESize I = 0; I < Systems.GetCount(); I++)
		Systems[I]->Draw(DrawParameters);

	if (DrawParameters->Pass == ZE_RP_COLOR)
	{
		DrawParameters->Statistics.ParticleStatistics.EmitterCount += Statistics.EmitterCount;
		DrawParameters->Statistics.ParticleStatistics.TotalParticleCount += Statistics.TotalParticleCount;
		DrawParameters->Statistics.ParticleStatistics.DrawedParticleCount += Statistics.DrawedParticleCount;
	}
}

void ZEParticleEffect::Tick(float TimeElapsed)
{
	for(ZESize I = 0; I < Emitters.GetCount(); I++)
		Emitters[I]->Tick(TimeElapsed);

	for(ZESize I = 0; I < Systems.GetCount(); I++)
		Systems[I]->Tick(TimeElapsed);
}

void ZEParticleEffect::AddEmitter(ZEParticleEmitter* Emitter)
{
	Emitter->SetOwner(this);
	Emitters.Add(Emitter);
}

void ZEParticleEffect::RemoveEmitter(ZEParticleEmitter* Emitter)
{
	Emitters.DeleteValue(Emitter);
}

const ZEArray<ZEParticleEmitter*>& ZEParticleEffect::GetEmitters()
{
	return Emitters;
}

void ZEParticleEffect::ResetEmitters()
{
	for (ZESize I = 0; I < Emitters.GetCount(); I++)
		Emitters[I]->Reset();
}

const ZEArray<ZEParticleSystem*>& ZEParticleEffect::GetSystems()
{
	return Systems;
}

void ZEParticleEffect::AddSystem(ZEParticleSystem* System)
{
	System->Owner = this;
	Systems.Add(System);
}

void ZEParticleEffect::RemoveSystem(ZEParticleSystem* System)
{
	Systems.DeleteValue(System);
}

const ZEParticleStatistics& ZEParticleEffect::GetStatistics()
{
	return Statistics;
}

ZEParticleEffect::ZEParticleEffect()
{
	SetBoundingBox(ZEAABBox(-ZEVector3::One * 10, ZEVector3::One * 10));

	memset(&Statistics, 0, sizeof(ZEParticleStatistics));
}

ZEParticleEffect::~ZEParticleEffect()
{
}

ZEParticleEffect* ZEParticleEffect::CreateInstance()
{
	return new ZEParticleEffect();
}
