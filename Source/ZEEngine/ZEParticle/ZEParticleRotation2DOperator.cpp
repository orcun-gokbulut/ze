//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleRotation2DOperator.cpp
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

#include "ZEParticleRotation2DOperator.h"
#include <math.h>

void ZEParticleRotation2DOperator::SetMaxRotation(float MaxRotation)
{
	this->MaxRotation = MaxRotation;
}

float ZEParticleRotation2DOperator::GetMaxRotation() const
{
	return MaxRotation;
}

void ZEParticleRotation2DOperator::SetMinRotation(float MinRotation)
{
	this->MinRotation = MinRotation;
}

float ZEParticleRotation2DOperator::GetMinRotation() const
{
	return MinRotation;
}

void ZEParticleRotation2DOperator::SetMinAngularAcceleration(float AngularAcceleration)
{
	MinAngularAcceleration = AngularAcceleration;
}

float ZEParticleRotation2DOperator::GetMinAngularAcceleration() const
{
	return MinAngularAcceleration;
}

void ZEParticleRotation2DOperator::SetMaxAngularAcceleration(float AngularAcceleration)
{
	MaxAngularAcceleration = AngularAcceleration;
}

float ZEParticleRotation2DOperator::GetMaxAngularAcceleration() const
{
	return MaxAngularAcceleration;
}

void ZEParticleRotation2DOperator::SetMinAngularVelocity(float AngularVelocity)
{
	MinAngularVelocity = AngularVelocity;
}

float ZEParticleRotation2DOperator::GetMinAngularVelocity() const
{
	return MinAngularVelocity;
}

void ZEParticleRotation2DOperator::SetMaxAngularVelocity(float AngularVelocity)
{
	MaxAngularVelocity = AngularVelocity;
}

float ZEParticleRotation2DOperator::GetMaxAngularVelocity() const
{
	return MaxAngularVelocity;
}

void ZEParticleRotation2DOperator::Tick(float ElapsedTime, ZEArray<ZEParticle>& OwnerParticlePool)
{
	ZESize ParticlCount = OwnerParticlePool.GetCount();

	for (ZESize I = 0; I < ParticlCount; I++)
	{
		switch(OwnerParticlePool[I].State)
		{
		case ZE_PAS_NEW:

			CustomData[I].Rotation				= RAND_BETWEEN_TWO_FLOAT(MinRotation, MaxRotation);
			OwnerParticlePool[I].Cos_NegSin.x	= cos(CustomData[I].Rotation);
			OwnerParticlePool[I].Cos_NegSin.y	= -sin(CustomData[I].Rotation);

			CustomData[I].AngularAcceleration	= RAND_BETWEEN_TWO_FLOAT(MinAngularAcceleration, MaxAngularAcceleration);
			CustomData[I].AngularVelocity		= RAND_BETWEEN_TWO_FLOAT(MinAngularVelocity, MaxAngularVelocity);
			break;

		case ZE_PAS_ALIVE:

			CustomData[I].AngularVelocity += CustomData[I].AngularAcceleration * ElapsedTime;
			CustomData[I].Rotation		  += CustomData[I].AngularVelocity * ElapsedTime;

			if(CustomData[I].AngularVelocity != 0.0f)
			{
				OwnerParticlePool[I].Cos_NegSin.x = cos(CustomData[I].Rotation);
				OwnerParticlePool[I].Cos_NegSin.y = -sin(CustomData[I].Rotation);
			}
			break;

		case ZE_PAS_DEAD:
			break;
		}
	}
}

void ZEParticleRotation2DOperator::ResizeCustomDataPool(ZESize NewPoolSize)
{
	CustomData.Resize(NewPoolSize);
}

ZEParticleRotation2DOperator::ZEParticleRotation2DOperator()
{
	CustomData.Clear();
}

ZEParticleRotation2DOperator::~ZEParticleRotation2DOperator()
{

}
