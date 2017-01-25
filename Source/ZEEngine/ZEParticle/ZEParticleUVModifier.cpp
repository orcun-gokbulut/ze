//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleUVModifier.cpp
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

#include "ZEParticleUVModifier.h"

void ZEParticleUVModifier::SetTextureSize(ZEVector2 TextureSize)
{
	this->TextureSize = TextureSize;
}

const ZEVector2& ZEParticleUVModifier::GetTextureSize() const
{
	return TextureSize;
}

void ZEParticleUVModifier::SetRowCount(ZEInt RowCount)
{
	this->RowCount = RowCount;
}

ZEInt ZEParticleUVModifier::GetRowCount() const
{
	return RowCount;
}

void ZEParticleUVModifier::SetColumnCount(ZEInt ColumnCount)
{
	this->ColumnCount = ColumnCount;
}

ZEInt ZEParticleUVModifier::GetColumnCount() const
{
	return ColumnCount;
}

void ZEParticleUVModifier::Tick(float ElapsedTime)
{
	if (!DoOnce)
	{
		UVFrameSize.x = 1.0f / (float)ColumnCount;
		UVFrameSize.y = 1.0f / (float)RowCount;
		DoOnce = true;
	}

	ZEArray<ZEParticle>& Particles = GetPool();
	ZESize ParticleCount = Particles.GetCount();

	for (ZESize I = 0; I < ParticleCount; I++)
	{
		switch (Particles[I].State)
		{
			case ZE_PAS_NEW:
				Particles[I].MinTexCoord = ZEVector2::Zero;
				Particles[I].MaxTexCoord = UVFrameSize;
				break;

			case ZE_PAS_ALIVE:
				CurrentUVFrame = (Particles[I].TotalLife - Particles[I].Life) * (((float)RowCount * (float)ColumnCount) / Particles[I].TotalLife);
				Particles[I].MinTexCoord.x = (CurrentUVFrame % ColumnCount) * UVFrameSize.x;
				Particles[I].MinTexCoord.y = (CurrentUVFrame / ColumnCount) * UVFrameSize.y;
				Particles[I].MaxTexCoord.x = ((CurrentUVFrame % ColumnCount) + 1) * UVFrameSize.x;
				Particles[I].MaxTexCoord.y = ((CurrentUVFrame / ColumnCount) + 1) * UVFrameSize.y;
				break;

			case ZE_PAS_DEAD:
				break;
		}
	}
}

ZEParticleUVModifier::ZEParticleUVModifier()
{
	DoOnce = false;
}

ZEParticleUVModifier::~ZEParticleUVModifier()
{

}
