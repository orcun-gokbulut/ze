//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleRenderer.cpp
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

#include "ZEParticleRenderer.h"
#include "ZEGame\ZEDrawParameters.h"
#include "ZEGame\ZEScene.h"
#include "ZERenderer\ZECamera.h"
#include "ZEParticle\ZEParticleSystem.h"

void ZEParticleRenderer::SortParticles() // GetSystem Pool Solve!!!
{
// 	ZEVector3 CamPos = zeScene->GetActiveCamera()->GetWorldPosition();
// 	Particles = Owner->GetParticlePool();
// 	float DistanceSqr;
// 
// 	for (ZEInt I = 0; I < 32; I++)
// 	{
// 		ZEUInt Radix=(1 << I);
// 
// 		ZESize ParticleCount = Particles.GetCount();
// 
// 		int Count0=0;
// 		int Count1=0;
// 
// 		for (ZESize J = 0; J < ParticleCount; J++)
// 		{
// 			DistanceSqr =	(CamPos.x - Particles[J].Position.x) * (CamPos.x - Particles[J].Position.x) +
// 				(CamPos.y - Particles[J].Position.y) * (CamPos.y - Particles[J].Position.y) +
// 				(CamPos.z - Particles[J].Position.z) * (CamPos.z - Particles[J].Position.z);
// 
// 			if (!((*(ZEUInt*)&(DistanceSqr))&Radix))
// 				++Count1;
// 		}
// 
// 		for (ZESize J = 0; J < ParticleCount; J++)
// 		{
// 			DistanceSqr =	(CamPos.x - Particles[J].Position.x) * (CamPos.x - Particles[J].Position.x) +
// 				(CamPos.y - Particles[J].Position.y) * (CamPos.y - Particles[J].Position.y) +
// 				(CamPos.z - Particles[J].Position.z) * (CamPos.z - Particles[J].Position.z);
// 
// 			if ((*(ZEUInt*)&(DistanceSqr))&Radix)
// 			{
// 				SortTempArray[Count1]=Particles[J];
// 				++Count1;
// 			}
// 			else 
// 			{
// 				SortTempArray[Count0]=Particles[J];
// 				++Count0;
// 			}
// 		}
// 
// 		for (ZESize J = 0; J < ParticleCount; J++)
// 			Particles[J] = SortTempArray[J];
// 	}
}

void ZEParticleRenderer::SetParticlesLocal(bool AreParticlesLocal)
{
	this->AreParticlesLocal = AreParticlesLocal;
}

bool ZEParticleRenderer::GetParticlesLocal() const
{
	return AreParticlesLocal;
}

void ZEParticleRenderer::SetParticleCount(ZEUInt Count)
{
	Particles.Resize((ZESize)Count);
	SortTempArray.Resize((ZESize)Count);
}

void ZEParticleRenderer::SetMaterial(ZERNMaterial* Material)
{
	this->Material = Material;
}

const ZERNMaterial* ZEParticleRenderer::GetMaterial() const
{
	return Material;
}

void ZEParticleRenderer::SetSortingEnabled(bool Enabled)
{
	IsSortingEnabled = Enabled;
}

bool ZEParticleRenderer::GetSortingEnabled() const
{
	return IsSortingEnabled;
}

const ZEParticleSystem* ZEParticleRenderer::GetOwner() const
{
	return Owner;
}

void ZEParticleRenderer::Draw(ZEDrawParameters* DrawParameters)
{
	if(IsSortingEnabled)
		SortParticles();
}

ZEParticleRenderer::ZEParticleRenderer()
{
	IsSortingEnabled = true;
	AreParticlesLocal = false;
	Material = NULL;
	Owner = NULL;
}

ZEParticleRenderer::~ZEParticleRenderer()
{
	Particles.Clear();
}
