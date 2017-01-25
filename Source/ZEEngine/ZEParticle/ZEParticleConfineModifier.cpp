//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleConfineModifier.cpp
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

#include "ZEParticleConfineModifier.h"

#include "ZEParticleEmitter.h"
#include "ZEParticleEffect.h"

void ZEParticleConfineModifier::SetBoundingBox(const ZEAABBox& BoundingBox)
{
	this->BoundingBox = BoundingBox;
}

const ZEAABBox ZEParticleConfineModifier::GetBoundingBox() const
{
	return BoundingBox;
}

void ZEParticleConfineModifier::Tick(float ElapsedTime)
{
	ZEArray<ZEParticle>& Particles = GetPool();
	ZESize ParticleCount = Particles.GetCount();

	ZEVector3 Displacement = GetBoundingBox().Max - GetBoundingBox().Min;
	for (ZESize I = 0; I < ParticleCount; I++)
	{
		if (Particles[I].State != ZE_PAS_ALIVE)
			continue;

		if (!GetEmitter()->GetLocalSpace())
			Particles[I].Position = GetEffect()->GetInvWorldTransform() * Particles[I].Position;

		if (Particles[I].Position.x < BoundingBox.Min.x)
			Particles[I].Position.x += Displacement.x;
		else if (Particles[I].Position.x > BoundingBox.Max.x)
			Particles[I].Position.x -= Displacement.x;

		if (Particles[I].Position.y < BoundingBox.Min.y)
			Particles[I].Position.y += Displacement.y;
		else if (Particles[I].Position.y > BoundingBox.Max.y)
			Particles[I].Position.y -= Displacement.y;

		if (Particles[I].Position.z < BoundingBox.Min.z)
			Particles[I].Position.z += Displacement.z;
		else if (Particles[I].Position.z > BoundingBox.Max.z)
			Particles[I].Position.z -= Displacement.z;

		if (!GetEmitter()->GetLocalSpace())
			Particles[I].Position = GetEffect()->GetWorldTransform() * Particles[I].Position;
	}
}


ZEParticleConfineModifier::ZEParticleConfineModifier()
{
	BoundingBox = ZEAABBox::Maximum;
}

ZEParticleConfineModifier::~ZEParticleConfineModifier()
{

}
