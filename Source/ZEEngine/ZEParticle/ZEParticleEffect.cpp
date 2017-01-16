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

#include "ZEMath/ZEViewVolume.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNCommand.h"
#include "ZERenderer/ZERNView.h"


void ZEParticleEffect::LocalTransformChanged()
{
	ZEEntity::LocalTransformChanged();

	ze_for_each(Emitter, Emitters)
		Emitter.GetItem()->EffectTransformChanged();
}

void ZEParticleEffect::ParentTransformChanged()
{
	ZEEntity::ParentTransformChanged();

	ze_for_each(Emitter, Emitters)
		Emitter.GetItem()->EffectTransformChanged();
}

ZEEntityResult ZEParticleEffect::InitializeInternal()
{
	ZE_ENTITY_INITIALIZE_CHAIN(ZEEntity);

	if (AutoStart)
		Start();

	return ZE_ER_DONE;
}

ZEEntityResult ZEParticleEffect::LoadInternal()
{
	ZE_ENTITY_LOAD_CHAIN(ZEEntity);

	for (ZESize I = 0; I < Emitters.GetCount(); I++)
		Emitters[I]->Initialize();

	return ZE_ER_DONE;
}

ZEEntityResult ZEParticleEffect::UnloadInternal()
{
	for (ZESize I = 0; I < Emitters.GetCount(); I++)
		Emitters[I]->Deinitialize();

	ZE_ENTITY_UNLOAD_CHAIN(ZEEntity);

	return ZE_ER_DONE;
}


ZEParticleEffect::ZEParticleEffect()
{
	SetEntityFlags(ZE_EF_RENDERABLE | ZE_EF_TICKABLE);
}

ZEParticleEffect::~ZEParticleEffect()
{

}

const ZEArray<ZEParticleEmitter*>& ZEParticleEffect::GetEmitters()
{
	return Emitters;
}

void ZEParticleEffect::AddEmitter(ZEParticleEmitter* Emitter)
{
	zeCheckError(Emitter == NULL, ZE_VOID, "Emitter is NULL.");
	zeCheckError(Emitter->Effect != NULL, ZE_VOID, "Emitter is already registered to a Particle Effect.");

	Emitter->Effect = this;
	Emitter->ResetPool();
	if (IsLoadedOrLoading())
		Emitter->Initialize();
	Emitters.Add(Emitter);
}

void ZEParticleEffect::RemoveEmitter(ZEParticleEmitter* Emitter)
{
	zeCheckError(Emitter == NULL, ZE_VOID, "Emitter is NULL.");
	zeCheckError(Emitter->Effect != this, ZE_VOID, "Emitter is not registered to this Particle Effect.");

	Emitter->Effect = NULL;
	Emitter->Deinitialize();
	Emitters.RemoveValue(Emitter);
}

void ZEParticleEffect::SetAutoStart(bool Enabled)
{
	AutoStart = Enabled;
	
	if (IsLoaded())
		Start();
}

bool ZEParticleEffect::GetAutoStart()
{
	return AutoStart;
}

void ZEParticleEffect::SetEffectState(ZEParticleEffectState State)
{
	if (EffectState == State)
		return;

	EffectState = State;

	if (EffectState == ZE_PES_STOPPED)
		Reset();
}

ZEParticleEffectState ZEParticleEffect::GetEffectState() const
{
	return EffectState;
}

bool ZEParticleEffect::IsRunning()
{
	return (EffectState == ZE_PES_RUNNING);
}

void ZEParticleEffect::Start()
{
	SetEffectState(ZE_PES_RUNNING);
}

void ZEParticleEffect::Stop()
{
	SetEffectState(ZE_PES_STOPPED);
}

void ZEParticleEffect::Pause()
{
	SetEffectState(ZE_PES_PAUSED);
}

void ZEParticleEffect::Reset()
{
	for (ZESize I = 0; I < Emitters.GetCount(); I++)
		Emitters[I]->ResetPool();
}

void ZEParticleEffect::Tick(float TimeElapsed)
{
	if (GetState() != ZE_PES_RUNNING)
		return;

	for (ZESize I = 0; I < Emitters.GetCount(); I++)
		Emitters[I]->Tick(TimeElapsed);
}

bool ZEParticleEffect::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (!ZEEntity::PreRender(Parameters))
		return false;

	for (ZESize I = 0; I < Emitters.GetCount(); I++)
		Emitters[I]->PreRender(Parameters);

	return true;
}

ZEParticleEffect* ZEParticleEffect::CreateInstance()
{
	return new ZEParticleEffect();
}
