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

ZEUInt ZEParticleUVModifier::GetFlags() const
{
	return ZE_PEF_TEXCOORDS_PER_PARTICLE;
}

void ZEParticleUVModifier::SetTextureSize(const ZEVector2& TextureSize)
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

	UVFrameSize.y = 1.0f / RowCount;
}

ZEInt ZEParticleUVModifier::GetRowCount() const
{
	return RowCount;
}

void ZEParticleUVModifier::SetColumnCount(ZEInt ColumnCount)
{
	this->ColumnCount = ColumnCount;

	UVFrameSize.x = 1.0f / ColumnCount;
}

ZEInt ZEParticleUVModifier::GetColumnCount() const
{
	return ColumnCount;
}

void ZEParticleUVModifier::Tick(float ElapsedTime)
{
	ZEParticlePool& ParticlePool = GetPool();
	
	const ZEArray<ZEUInt>& AliveParticleIndices = GetEmitter()->GetAliveParticleIndices();
	const ZEArray<ZEUInt>& DeadParticleIndices = GetEmitter()->GetDeadParticleIndices();

	ZEUInt AliveParticleCount = GetEmitter()->GetAliveParticleCount();
	ZEUInt DeadParticleStartIndex = GetEmitter()->GetDeadParticleStartIndex();
	ZEUInt DeadParticleCount = GetEmitter()->GetDeadParticleCount();
	ZEUInt SpawnedParticleCount = GetEmitter()->GetLastSpawnedParticleCount();

	for (ZEUInt I = DeadParticleStartIndex; I < (DeadParticleStartIndex + SpawnedParticleCount); I++)
	{
		ZEUInt Index = DeadParticleIndices[I % DeadParticleIndices.GetCount()];
		ParticlePool.TexCoords[Index].x = 0.0f;
		ParticlePool.TexCoords[Index].y = 0.0f;
		ParticlePool.TexCoords[Index].z = UVFrameSize.x;
		ParticlePool.TexCoords[Index].w = UVFrameSize.y;
	}

	for (ZEUInt I = 0; I < AliveParticleCount; I++)
	{
		ZEUInt Index = AliveParticleIndices[I];

		CurrentUVFrame = (ParticlePool.TotalLifes[Index] - ParticlePool.Lifes[Index]) * (((float)RowCount * (float)ColumnCount) / ParticlePool.TotalLifes[Index]);
		
		ParticlePool.TexCoords[Index].x = (CurrentUVFrame % ColumnCount) * UVFrameSize.x;
		ParticlePool.TexCoords[Index].y = (CurrentUVFrame / ColumnCount) * UVFrameSize.y;
		ParticlePool.TexCoords[Index].z = ((CurrentUVFrame % ColumnCount) + 1) * UVFrameSize.x;
		ParticlePool.TexCoords[Index].w = ((CurrentUVFrame / ColumnCount) + 1) * UVFrameSize.y;
	}
}

ZEParticleUVModifier::ZEParticleUVModifier()
{
	ColumnCount = 1;
	RowCount = 1;
	UVFrameSize = ZEVector2::One;
	CurrentUVFrame = 0;
}

ZEParticleUVModifier::~ZEParticleUVModifier()
{

}
