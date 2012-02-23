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

ZE_META_REGISTER_CLASS(ZEEntityProvider, ZEParticleEffect);
		
ZEDrawFlags ZEParticleEffect::GetDrawFlags() const
{
	return ZE_DF_DRAW | ZE_DF_CULL;
}

const ZEAABBox& ZEParticleEffect::GetWorldBoundingBox()
{
	ZEAABBox Box;
	Box.Max = ZEVector3::MinValue;
	Box.Min = ZEVector3::MaxValue;

	for (ZESize I = 0; I < Emitters.GetCount(); I++)
	{
		ZEVector3::Min(Box.Min, Box.Min, Emitters[I]->GetBoundingBox().Min);
		ZEVector3::Max(Box.Max, Box.Max, Emitters[I]->GetBoundingBox().Max);
	}

	return Box;
}

bool ZEParticleEffect::Initialize()
{
	return true;
}

void ZEParticleEffect::Deinitialize()
{
}

void ZEParticleEffect::Draw(ZEDrawParameters* DrawParameters)
{
	for(ZESize I = 0; I < Emitters.GetCount(); I++)
		Emitters[I]->Draw(DrawParameters);
}

void ZEParticleEffect::Tick(float TimeElapsed)
{
	for(ZESize I = 0; I < Emitters.GetCount(); I++)
		Emitters[I]->Tick(TimeElapsed);
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

ZEParticleEffect::ZEParticleEffect()
{
}

ZEParticleEffect::~ZEParticleEffect()
{
}

ZEParticleEffect* ZEParticleEffect::CreateInstance()
{
	return new ZEParticleEffect();
}
