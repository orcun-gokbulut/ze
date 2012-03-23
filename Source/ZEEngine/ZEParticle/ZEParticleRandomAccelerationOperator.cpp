//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleRandomAccelerationOperator.cpp
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

#include "ZEParticleRandomAccelerationOperator.h"
#include "ZEParticleSystem.h"
#include "ZEParticlePhysicsOperator.h"

void ZEParticleRandomAccelerationOperator::SetMaxStrength(ZEVector3 NewStrength)
{
	MaxStrength = NewStrength;
}

const ZEVector3& ZEParticleRandomAccelerationOperator::GetMaxStrength() const
{
	return MaxStrength;
}

void ZEParticleRandomAccelerationOperator::SetMinStrength(ZEVector3 NewStrength)
{
	MinStrength = NewStrength;
}

const ZEVector3& ZEParticleRandomAccelerationOperator::GetMinStrength() const
{
	return MinStrength;
}

void ZEParticleRandomAccelerationOperator::Tick(float ElapsedTime, ZEArray<ZEParticle>& OwnerParticlePool)
{
	ZEParticlePhysicsOperator* PhysicsOperator = NULL;

	for(ZESize I = 0; I < GetOwner()->GetOperators().GetCount(); I++)
	{
		if(GetOwner()->GetOperators()[I]->GetDescription() == ZEParticlePhysicsOperator::Description())
			PhysicsOperator = (ZEParticlePhysicsOperator*)GetOwner()->GetOperators()[I];
	}

	if(PhysicsOperator == NULL)
		return;

	ZEVector3 RandomResult;

	for (ZESize I = 0; I < PhysicsOperator->CustomData.GetCount(); I++)
	{
		if(OwnerParticlePool[I].State != ZE_PAS_DEAD)
		{
			RandomResult.x = RAND_BETWEEN_TWO_FLOAT(MinStrength.x, MaxStrength.x);
			RandomResult.y = RAND_BETWEEN_TWO_FLOAT(MinStrength.y, MaxStrength.y);
			RandomResult.z = RAND_BETWEEN_TWO_FLOAT(MinStrength.z, MaxStrength.z);
			PhysicsOperator->CustomData[I].Velocity += RandomResult * ElapsedTime; 
		}
	}
}

void ZEParticleRandomAccelerationOperator::ResizeCustomDataPool(ZESize NewPoolSize)
{
	//Nothing to do with pool size
}

ZEParticleRandomAccelerationOperator::ZEParticleRandomAccelerationOperator()
{
	MaxStrength = ZEVector3::Zero;
	MinStrength = ZEVector3::Zero;
}

ZEParticleRandomAccelerationOperator::~ZEParticleRandomAccelerationOperator()
{

}
