//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleSystem.cpp
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

#include "ZEParticleSystem.h"
#include "ZEGame\ZEDrawParameters.h"
#include "ZEParticleRenderer.h"
#include "ZEParticleOperator.h"
#include "ZEParticleGenerator.h"

void ZEParticleSystem::SetRenderer(ZEParticleRenderer* Renderer)
{
	if(Renderer == NULL)
		return;

	this->Renderer = Renderer;
	Renderer->Owner = this;
}

const ZEParticleRenderer* ZEParticleSystem::GetRenderer() const
{
	return Renderer;
}

const ZEArray<ZEParticle>& ZEParticleSystem::GetParticlePool() const
{
	return ParticlePool;
}

const ZEArray<ZEParticleOperator*>& ZEParticleSystem::GetOperators() const
{
	return Operators;
}

bool ZEParticleSystem::AddOperator(ZEParticleOperator* NewOperator)
{
	if(Operators.Exists(NewOperator))
		return false;

	Operators.Add(NewOperator);
	NewOperator->ResizeCustomDataPool(MaximumParticleCount);
	NewOperator->Owner = this;

	return true;
}

bool ZEParticleSystem::RemoveOperator(ZEParticleOperator* OperatorToRemove)
{
	if(!Operators.Exists(OperatorToRemove))
		return false;

	Operators.DeleteValue(OperatorToRemove);
	return true;
}

const ZEArray<ZEParticleGenerator*>& ZEParticleSystem::GetGenerators() const
{
	return Generators;
}

bool ZEParticleSystem::AddGenerator(ZEParticleGenerator* NewGenerator)
{
	if(Generators.Exists(NewGenerator))
		return false;

	Generators.Add(NewGenerator);
	NewGenerator->Owner = this;

	return true;
}

bool ZEParticleSystem::RemoveGenerator(ZEParticleGenerator* GeneratorToRemove)
{
	if(!Generators.Exists(GeneratorToRemove))
		return false;

	Generators.DeleteValue(GeneratorToRemove);
	return true;
}

void ZEParticleSystem::SetMaximumParticleCount(ZESize ParticleCount)
{
	if(MaximumParticleCount == ParticleCount)
		return;

	MaximumParticleCount = ParticleCount;	
	ParticlePool.Resize(MaximumParticleCount);

	for (ZESize I = 0; I < Operators.GetCount(); I++)
	{
		Operators[I]->ResizeCustomDataPool(MaximumParticleCount); 
	}

	if(Renderer != NULL)
		Renderer->SetParticleCount(MaximumParticleCount);
}

ZESize ZEParticleSystem::GetMaximumParticleCount() const
{
	return MaximumParticleCount;
}

const ZEParticleEffect* ZEParticleSystem::GetOwner() const
{
	return Owner;
}

void ZEParticleSystem::Draw(ZEDrawParameters* DrawParameters)
{
	if(Renderer == NULL)
		return;

	Renderer->Draw(DrawParameters);
}

void ZEParticleSystem::Tick(float ElapsedTime)
{
	for (ZESize I = 0; I < MaximumParticleCount; I++)
	{
		if(ParticlePool[I].State == ZE_PAS_NEW)
			ParticlePool[I].State = ZE_PAS_ALIVE;
	}

	for (ZESize I = 0; I < Generators.GetCount(); I++)
	{
		Generators[I]->Tick(ElapsedTime, ParticlePool);
	}

	for (ZESize I = 0; I < Operators.GetCount(); I++)
	{
		Operators[I]->Tick(ElapsedTime, ParticlePool);
	}
}

ZEParticleSystem::ZEParticleSystem()
{
	Renderer = NULL;
	ParticlePool.Clear();
	Operators.Clear();
	Owner = NULL;
}

ZEParticleSystem::~ZEParticleSystem()
{

}
