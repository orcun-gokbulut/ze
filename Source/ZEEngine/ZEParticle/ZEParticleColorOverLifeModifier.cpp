//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleColorOverLifeModifier.cpp
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

#include "ZEParticleColorOverLifeModifier.h"

#include "ZEMath/ZEMath.h"

ZEUInt ZEParticleColorOverLifeModifier::GetFlags() const
{
	return ZE_PEF_COLOR_PER_PARTICLE | ZE_PEF_TOTAL_LIFE_PER_PARTICLE;
}

void ZEParticleColorOverLifeModifier::SetFromColor(const ZEVector4& Color)
{
	this->FromColor = Color;
}

const ZEVector4& ZEParticleColorOverLifeModifier::GetFromColor() const
{
	return FromColor;
}

void ZEParticleColorOverLifeModifier::SetToColor(const ZEVector4& Color)
{
	this->ToColor = Color;
}

const ZEVector4& ZEParticleColorOverLifeModifier::GetToColor() const
{
	return ToColor;
}

void ZEParticleColorOverLifeModifier::Tick(float ElapsedTime)
{
	ZEParticlePool& ParticlePool = GetPool();
	const ZEArray<ZEUInt>& AliveParticleIndices = GetEmitter()->GetAliveParticleIndices();
	ZEUInt AliveParticleCount = GetEmitter()->GetAliveParticleCount();
	
	for (ZEUInt I = 0; I < AliveParticleCount; I++)
	{
		ZEUInt Index = AliveParticleIndices[I];
		float Weight = 1.0f - (ParticlePool.Lifes[Index] / ParticlePool.TotalLifes[Index]);

		ParticlePool.Colors[Index].x = ZEMath::Lerp(FromColor.x, ToColor.x, Weight);
		ParticlePool.Colors[Index].y = ZEMath::Lerp(FromColor.y, ToColor.y, Weight);
		ParticlePool.Colors[Index].z = ZEMath::Lerp(FromColor.z, ToColor.z, Weight);
		ParticlePool.Colors[Index].w = ZEMath::Lerp(FromColor.w, ToColor.w, Weight);
	}
}

ZEParticleColorOverLifeModifier::ZEParticleColorOverLifeModifier()
{
	FromColor = ZEVector4(0.0f, 0.0f, 0.0f, 1.0f);
	ToColor = ZEVector4::One;
}

ZEParticleColorOverLifeModifier::~ZEParticleColorOverLifeModifier()
{

}
