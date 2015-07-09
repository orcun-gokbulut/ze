//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleModifier.cpp
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

#include "ZEParticleModifier.h"
#include "ZEParticleEmitter.h"
#include "ZEParticleEffect.h"
#include "ZERenderer\ZEFixedMaterial.h"
#include "ZERandom.h"
#include "ZEMath\ZEAngle.h"
#include "ZEMath\ZEMath.h"

void ZEParticleModifier::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEParticleModifier::GetName()
{
	return Name;
}

void ZEParticleModifier::SetOwner(ZEParticleEmitter* Owner)
{
	this->Owner = Owner;
}

ZEParticleEmitter* ZEParticleModifier::GetOwner()
{
	return Owner;
}

ZEArray<ZEParticle>& ZEParticleModifier::GetOwnerParticlePool()
{
	return Owner->GetParticlePool();
}

ZEParticleModifier::ZEParticleModifier()
{
	Owner = NULL;
}

ZEParticleModifier::~ZEParticleModifier()
{

}

/************************************************************************/
/*							ZEParticleBasicBehaviourModifier            */
/************************************************************************/

void ZEParticlePhysicsModifier::SetRadialMovement(bool Enabled)
{
	IsRadialMovement = Enabled;
}

void ZEParticlePhysicsModifier::SetRadialSpeed(float Max, float Min)
{
	MaxRadialSpeed = Max;
	MinRadialSpeed = Min;
}

void ZEParticlePhysicsModifier::SetMinAcceleration(const ZEVector3& Acceleration)
{
	MinAcceleration = Acceleration;
}

const ZEVector3& ZEParticlePhysicsModifier::GetMinAcceleration() const
{
	return MinAcceleration;
}

void ZEParticlePhysicsModifier::SetMaxAcceleration(const ZEVector3& Acceleration)
{
	MaxAcceleration = Acceleration;
}

const ZEVector3& ZEParticlePhysicsModifier::GetMaxAcceleration() const
{
	return MaxAcceleration;
}

void ZEParticlePhysicsModifier::SetMinVelocity(const ZEVector3& Velocity)
{
	MinVelocity = Velocity;
}

const ZEVector3& ZEParticlePhysicsModifier::GetMinVelocity() const
{
	return MinVelocity;
}

void ZEParticlePhysicsModifier::SetMaxVelocity(const ZEVector3& Velocity)
{
	MaxVelocity = Velocity;
}

const ZEVector3& ZEParticlePhysicsModifier::GetMaxVelocity() const
{
	return MaxVelocity;
}

ZEParticlePhysicsModifier::ZEParticlePhysicsModifier()
{
	IsRadialMovement = false;
	MaxRadialSpeed = 0.0f;
	MinRadialSpeed = 0.0f;

	MinAcceleration = ZEVector3(0.0f, 0.0f, 0.0f);			
	MaxAcceleration = ZEVector3(0.0f, 0.0f, 0.0f);			
	MinVelocity = ZEVector3(0.0f, 0.0f, 0.0f);				
	MaxVelocity = ZEVector3(0.0f, 0.0f, 0.0f);				
}

ZEParticlePhysicsModifier::~ZEParticlePhysicsModifier()
{

}

void ZEParticlePhysicsModifier::Tick(float ElapsedTime)
{
	ZEArray<ZEParticle>& Particles = GetOwnerParticlePool();
	ZESize ParticleCount = Particles.GetCount();
	ZEVector3 EmitterWorldPosition = GetOwner()->GetOwner()->GetWorldPosition() + GetOwner()->GetPosition();
	ZEVector3 EmitterUp = GetOwner()->GetOwner()->GetWorldRotation() * -ZEVector3::UnitZ;
	ZEParticleEmitterType EmitterType = GetOwner()->GetType();

	for (ZESize I = 0; I < ParticleCount; I++)
	{
		switch(Particles[I].State)
		{
		case ZE_PAS_NEW:

			if (IsRadialMovement)
			{
				Particles[I].Velocity = ((Particles[I].Position - EmitterWorldPosition).Normalize()) * RAND_BETWEEN_TWO_FLOAT(MinRadialSpeed, MaxRadialSpeed);
				
				if(EmitterType == ZE_PET_TORUS)
				{
					Particles[I].Velocity.x -= ZEMath::Abs(EmitterUp.x) * Particles[I].Velocity.x;
					Particles[I].Velocity.y -= ZEMath::Abs(EmitterUp.y) * Particles[I].Velocity.y;
					Particles[I].Velocity.z -= ZEMath::Abs(EmitterUp.z) * Particles[I].Velocity.z;
				}
			}
			else
			{
				Particles[I].Velocity.x				= RAND_BETWEEN_TWO_FLOAT(MinVelocity.x, MaxVelocity.x);
				Particles[I].Velocity.y				= RAND_BETWEEN_TWO_FLOAT(MinVelocity.y, MaxVelocity.y);
				Particles[I].Velocity.z				= RAND_BETWEEN_TWO_FLOAT(MinVelocity.z, MaxVelocity.z);
			}

			Particles[I].Acceleration.x				= RAND_BETWEEN_TWO_FLOAT(MinAcceleration.x, MaxAcceleration.x);
			Particles[I].Acceleration.y				= RAND_BETWEEN_TWO_FLOAT(MinAcceleration.y, MaxAcceleration.y);
			Particles[I].Acceleration.z				= RAND_BETWEEN_TWO_FLOAT(MinAcceleration.z, MaxAcceleration.z);

			Particles[I].InitialVelocity			= Particles[I].Velocity;

			break;

		case ZE_PAS_ALIVE:

			Particles[I].Position			+= Particles[I].Velocity * ElapsedTime;
			Particles[I].Velocity			+= Particles[I].Acceleration * ElapsedTime;

			break;

		case ZE_PAS_DEAD:
			break;
		}
	}
}

/************************************************************************/
/*                        ZEParticleRotationModifier                    */
/************************************************************************/

void ZEParticleRotationModifier::SetMinAngularAcceleration(float AngularAcceleration)
{
	MinAngularAcceleration = AngularAcceleration;
}

float ZEParticleRotationModifier::GetMinAngularAcceleration() const
{
	return MinAngularAcceleration;
}

void ZEParticleRotationModifier::SetMaxAngularAcceleration(float AngularAcceleration)
{
	MaxAngularAcceleration = AngularAcceleration;
}

float ZEParticleRotationModifier::GetMaxAngularAcceleration() const
{
	return MaxAngularAcceleration;
}

void ZEParticleRotationModifier::SetMinAngularVelocity(float AngularVelocity)
{
	MinAngularVelocity = AngularVelocity;
}

float ZEParticleRotationModifier::GetMinAngularVelocity() const
{
	return MinAngularVelocity;
}

void ZEParticleRotationModifier::SetMaxAngularVelocity(float AngularVelocity)
{
	MaxAngularVelocity = AngularVelocity;
}

float ZEParticleRotationModifier::GetMaxAngularVelocity() const
{
	return MaxAngularVelocity;
}

void ZEParticleRotationModifier::SetMaxRotation(float MaxRotation)
{
	this->MaxRotation = MaxRotation;
}

float ZEParticleRotationModifier::GetMaxRotation() const
{
	return MaxRotation;
}

void ZEParticleRotationModifier::SetMinRotation(float MinRotation)
{
	this->MinRotation = MinRotation;
}

float ZEParticleRotationModifier::GetMinRotation() const
{
	return MinRotation;
}

void ZEParticleRotationModifier::Tick(float ElapsedTime)
{
	ZEArray<ZEParticle>& Particles = GetOwnerParticlePool();
	ZESize ParticlCount = Particles.GetCount();

	for (ZESize I = 0; I < ParticlCount; I++)
	{
		switch(Particles[I].State)
		{
		case ZE_PAS_NEW:

			Particles[I].Rotation					= RAND_BETWEEN_TWO_FLOAT(MinRotation, MaxRotation);
			Particles[I].Cos_NegSin.x				= cos(Particles[I].Rotation);
			Particles[I].Cos_NegSin.y				= -sin(Particles[I].Rotation);

			Particles[I].AngularAcceleration		= RAND_BETWEEN_TWO_FLOAT(MinAngularAcceleration, MaxAngularAcceleration);
			Particles[I].AngularVelocity			= RAND_BETWEEN_TWO_FLOAT(MinAngularVelocity, MaxAngularVelocity);
			break;

		case ZE_PAS_ALIVE:

			Particles[I].AngularVelocity	+= Particles[I].AngularAcceleration * ElapsedTime;
			Particles[I].Rotation			+= Particles[I].AngularVelocity * ElapsedTime;

			if(Particles[I].AngularVelocity != 0.0f)
			{
				Particles[I].Cos_NegSin.x			= cos(Particles[I].Rotation);
				Particles[I].Cos_NegSin.y			= -sin(Particles[I].Rotation);
			}
			break;

		case ZE_PAS_DEAD:
			break;
		}
	}
}

ZEParticleRotationModifier::ZEParticleRotationModifier()
{
	MinAngularAcceleration = 0.0f;		
	MaxAngularAcceleration = 0.0f;		
	MinAngularVelocity = 0.0f;			
	MaxAngularVelocity = 0.0f;
	MaxRotation = 0.0f;
	MinRotation = 0.0f;
}

ZEParticleRotationModifier::~ZEParticleRotationModifier()
{

}

/************************************************************************/
/*                        ZEParticleSizeOverLifeModifier                */
/************************************************************************/

void ZEParticleGrowModifier::SetGrowFactor(float Factor)
{
	GrowFactor = Factor;
}

float ZEParticleGrowModifier::GetGrowFactor() const
{
	return GrowFactor;
}

void ZEParticleGrowModifier::Tick(float ElapsedTime)
{
	ZESize ParticleCount = GetOwnerParticlePool().GetCount();
	ZEArray<ZEParticle>& Particles =  GetOwnerParticlePool();

	float GrowAmount = GrowFactor * ElapsedTime;

	for (ZESize I = 0; I < ParticleCount; I++)
	{
		Particles[I].Size2D += (Particles[I].Size2D * GrowAmount);
	}
}

ZEParticleGrowModifier::ZEParticleGrowModifier()
{
	GrowFactor = 0.2f;
}

ZEParticleGrowModifier::~ZEParticleGrowModifier()
{

}

/************************************************************************/
/*                        ZEParticleColorLerpModifier                   */
/************************************************************************/

void ZEParticleColorOverLifeModifier::SetToColor(const ZEVector4& ToColor)
{
	this->ToColor = ToColor;
}

const ZEVector4& ZEParticleColorOverLifeModifier::GetToColor() const
{
	return ToColor;
}

void ZEParticleColorOverLifeModifier::Tick(float ElapsedTime)
{
	ZESize ParticleCount = GetOwnerParticlePool().GetCount();
	ZEArray<ZEParticle>& Particles =  GetOwnerParticlePool();

	for (ZESize I = 0; I < ParticleCount; I++)
	{
		ZEParticle* CurrentParticle = &Particles[I];

		if(CurrentParticle->State == ZE_PAS_ALIVE)
		{
			float TotalLife_Life = CurrentParticle->Life / CurrentParticle->TotalLife;
			float LerpFactor = 1.0f - TotalLife_Life;

			CurrentParticle->Color.x = CurrentParticle->InitialColor.x + (ToColor.x - CurrentParticle->InitialColor.x) * LerpFactor;
			CurrentParticle->Color.y = CurrentParticle->InitialColor.y + (ToColor.y - CurrentParticle->InitialColor.y) * LerpFactor;
			CurrentParticle->Color.z = CurrentParticle->InitialColor.z + (ToColor.z - CurrentParticle->InitialColor.z) * LerpFactor;
			CurrentParticle->Color.w = CurrentParticle->InitialColor.w + (ToColor.w - CurrentParticle->InitialColor.w) * LerpFactor;
		}
	}
}

ZEParticleColorOverLifeModifier::ZEParticleColorOverLifeModifier()
{
	ToColor = ZEVector4::One;
}

ZEParticleColorOverLifeModifier::~ZEParticleColorOverLifeModifier()
{

}

/************************************************************************/
/*                        ZEParticleVelocityLerpModifier				*/
/************************************************************************/

void ZEParticleVelocityOverLifeModifier::SetToVelocity(const ZEVector3& ToVelocity)
{
	this->ToVelocity = ToVelocity;
}

const ZEVector3& ZEParticleVelocityOverLifeModifier::GetToVelocity() const
{
	return ToVelocity;
}

void ZEParticleVelocityOverLifeModifier::Tick(float ElapsedTime)
{
	ZESize ParticleCount = GetOwnerParticlePool().GetCount();
	ZEArray<ZEParticle>& Particles =  GetOwnerParticlePool();

	for (ZESize I = 0; I < ParticleCount; I++)
	{
		ZEParticle* CurrentParticle = &Particles[I];

		if(CurrentParticle->State == ZE_PAS_ALIVE)
		{
			float TotalLife_Life = CurrentParticle->Life / CurrentParticle->TotalLife;
			float LerpFactor = 1.0f - TotalLife_Life;

			CurrentParticle->Velocity.x = CurrentParticle->InitialVelocity.x + (ToVelocity.x - CurrentParticle->InitialVelocity.x) * LerpFactor;
			CurrentParticle->Velocity.y = CurrentParticle->InitialVelocity.y + (ToVelocity.y - CurrentParticle->InitialVelocity.y) * LerpFactor;
			CurrentParticle->Velocity.z = CurrentParticle->InitialVelocity.z + (ToVelocity.z - CurrentParticle->InitialVelocity.z) * LerpFactor;
		}
	}
}

ZEParticleVelocityOverLifeModifier::ZEParticleVelocityOverLifeModifier()
{
	ToVelocity = ZEVector3::One;
}

ZEParticleVelocityOverLifeModifier::~ZEParticleVelocityOverLifeModifier()
{

}

/************************************************************************/
/*                        ZEParticleDiffuseMapChangerModifier           */
/************************************************************************/

void ZEParticleDiffuseMapChangerModifier::SetInterval(float NewInterval)
{
	Interval = NewInterval;
}

float ZEParticleDiffuseMapChangerModifier::GetInterval()
{
	return Interval;
}

void ZEParticleDiffuseMapChangerModifier::AddTextureResource(ZETexture2DResource* Resource)
{
	Textures.Add(Resource);
}

void ZEParticleDiffuseMapChangerModifier::Tick(float ElapsedTime)
{
	((ZEFixedMaterial*)(GetOwner()->GetMaterial()))->SetBaseMap(Textures[Textures.Circular((ZESize)CurrentTextureIndex)]->GetTexture2D());

	if(TotalTime >= Interval)
	{
		CurrentTextureIndex++;
		TotalTime = 0.0f;
	}

	TotalTime += ElapsedTime;
}

ZEParticleDiffuseMapChangerModifier::ZEParticleDiffuseMapChangerModifier()
{
	Textures.Clear();
	TotalTime = 0.0f;
	CurrentTextureIndex = 0;
}

ZEParticleDiffuseMapChangerModifier::~ZEParticleDiffuseMapChangerModifier()
{

}

/************************************************************************/
/*                        ZEParticleDisplacementModifier	            */
/************************************************************************/

void ZEParticleDisplacementModifier::SetDisplacement(ZEVector3 NewDisplacement)
{
	Displacement = NewDisplacement;
}

ZEVector3 ZEParticleDisplacementModifier::GetDisplacement()
{
	return Displacement;
}

void ZEParticleDisplacementModifier::Tick(float ElapsedTime)
{
	ZEArray<ZEParticle>& Particles = GetOwnerParticlePool();
	ZESize ParticleCount = Particles.GetCount();

	for (ZESize I = 0; I < ParticleCount; I++)
	{
		ZEVector3 RandomedDisplacement;
		RandomedDisplacement.x = RAND_BETWEEN_TWO_FLOAT(Displacement.x * 0.9f, Displacement.x * 1.1f);
		RandomedDisplacement.y = RAND_BETWEEN_TWO_FLOAT(Displacement.y * 0.9f, Displacement.y * 1.1f);
		RandomedDisplacement.z = RAND_BETWEEN_TWO_FLOAT(Displacement.z * 0.9f, Displacement.z * 1.1f);
		Particles[I].Position += RandomedDisplacement * ElapsedTime;
	}
}

ZEParticleDisplacementModifier::ZEParticleDisplacementModifier()
{
	Displacement = ZEVector3::Zero;
}

ZEParticleDisplacementModifier::~ZEParticleDisplacementModifier()
{

}

/************************************************************************/
/*               ZEParticleRandomAccelerationModifier			        */
/************************************************************************/

void ZEParticleRandomAccelerationModifier::SetMaxStrength(float NewStrength)
{
	MaxStrength = NewStrength;
}

float ZEParticleRandomAccelerationModifier::GetMaxStrength() const
{
	return MaxStrength;
}

void ZEParticleRandomAccelerationModifier::Tick(float ElapsedTime)
{
	ZEArray<ZEParticle>& Particles = GetOwnerParticlePool();
	ZESize ParticleCount = Particles.GetCount();

	ZEVector3 RandomResult = ZEVector3::Zero;

	for (ZESize I = 0; I < ParticleCount; I++)
	{
		RandomResult.x = (ZERandom::GetFloat() / RAND_MAX) * MaxStrength;
		RandomResult.y = (ZERandom::GetFloat() / RAND_MAX) * MaxStrength;
		RandomResult.z = (ZERandom::GetFloat() / RAND_MAX) * MaxStrength;
		Particles[I].Velocity += RandomResult;
	}
}

ZEParticleRandomAccelerationModifier::ZEParticleRandomAccelerationModifier()
{
	MaxStrength = 1000.0f;
}

ZEParticleRandomAccelerationModifier::~ZEParticleRandomAccelerationModifier()
{

}

/************************************************************************/
/*							ZEParticleUVModifier						*/
/************************************************************************/


void ZEParticleUVModifier::SetTextureSize(ZEVector2 TextureSize)
{
	this->TextureSize = TextureSize;
}

void ZEParticleUVModifier::SetRowCount(ZEInt RowCount)
{
	this->RowCount = RowCount;
}

void ZEParticleUVModifier::SetColumnCount(ZEInt ColumnCount)
{
	this->ColumnCount = ColumnCount;
}

void ZEParticleUVModifier::Tick(float ElapsedTime)
{
	if(!DoOnce)
	{
		UVFrameSize.x = 1.0f / (float)ColumnCount;
		UVFrameSize.y = 1.0f / (float)RowCount;
		DoOnce = true;
	}

	ZEArray<ZEParticle>& Particles = GetOwnerParticlePool();
	ZESize ParticleCount = Particles.GetCount();

	for (ZESize I = 0; I < ParticleCount; I++)
	{
		switch(Particles[I].State)
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
