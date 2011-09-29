//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleEmitter.cpp
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
#include <cmath>
#include "ZEParticleEffect.h"
#include "ZEParticleSystem.h"
#include "ZEParticleEmitter.h"
#include "ZEMath/ZEMathDefinitions.h"

#define RAND_BETWEEN_TWO_FLOAT(Min, Max) (((Max - Min) * ((float)rand() / RAND_MAX)) + Min)

void ZEParticleEmitter::Tick(float TimeElapsed)
{
	float ParticleCreationTime = 1.0f / (float)ParticlesPerSecond;	// Time in seconds for creation of a particle

	LastCreation += TimeElapsed;

	if(LastCreation > ParticleCreationTime)	// If more than 'ParticleCreation' seconds has passed since last creation...
	{
		for(unsigned int I = 0; I < ParticlePool.GetCount(); I++)
		{
			if(ParticlePool[I].IsAlive == false)	// ...look for a dead particle...
			{
				GenerateParticle(ParticlePool[I]);	//...create it...
				EmittedParticleCount++;				//... and add one to the list...
				break;								//...but not more than once
			}			
		}

		LastCreation = 0.0f;
	}

	for(unsigned int I = 0; I < ParticlePool.GetCount(); I++)
	{
		if(ParticlePool[I].IsAlive == true)
		{
			ParticlePool[I].Position += (ParticlePool[I].Velocity * TimeElapsed); //a*t^2 is not necessary
			ParticlePool[I].Velocity += ParticlePool[I].Acceleration;
			ParticlePool[I].Life -= TimeElapsed;	// Time is killing particles, like real people
		}

		if(ParticlePool[I].Life < 0.0f)			// If a particle's life is below zero...
		{
			ParticlePool[I].IsAlive = false;	//...flag it dead
		}
	}

	if(IsContinuous == false)	// If the emitter is not continuous...
	{
		if(EmittedParticleCount > MaxParticleCount)	//...and it reaches its limit...
			for(unsigned int I = 0; I < ParticlePool.GetCount(); I++)
			{
				if(ParticlePool[I].IsAlive == true)	//...wait for the pool's death...
					return;
				else
					Owner->DeleteParticleEmitter(this);			//...then destroy it
			}
	}
}

void ZEParticleEmitter::InitializeParticlePool()
{
	ParticlePool.SetCount(MaxParticleCount);

	for(unsigned int I = 0; I < MaxParticleCount; I++)
	{
		GenerateParticle(ParticlePool[I]);
	}
}

void ZEParticleEmitter::GenerateParticle(ZEParticle &Particle)
{
	switch(Type)
	{
	case ZE_PET_POINT:
		Particle.Position = Position;		// All particles come from a single point
		break;
	case ZE_PET_BOX:	// Particles come from inside of a box
		Particle.Position.x = Position.x + RAND_BETWEEN_TWO_FLOAT(-BoxSize.x / 2, BoxSize.x / 2);
		Particle.Position.y = Position.y + RAND_BETWEEN_TWO_FLOAT(-BoxSize.y / 2, BoxSize.y / 2);
		Particle.Position.z = Position.z + RAND_BETWEEN_TWO_FLOAT(-BoxSize.z / 2, BoxSize.z / 2);
		break;
	case ZE_PET_TORUS:		// Particles come from inside of a torus
		{
			float Theta = RAND_BETWEEN_TWO_FLOAT(0.0f, (float)ZE_PIx2);
			float Phi = RAND_BETWEEN_TWO_FLOAT(0.0f, (float)ZE_PIx2);
			float TubeRadius = RAND_BETWEEN_TWO_FLOAT(0.0f, TorusSize.y);
			Particle.Position.x = Position.x + (TorusSize.x + TubeRadius * cosf(Phi)) * cosf(Theta);
			Particle.Position.y = Position.y + (TorusSize.x + TubeRadius * cosf(Phi)) * sinf(Theta);
			Particle.Position.z = Position.z + TubeRadius * sinf(Phi);		
			break;
		}
	case ZE_PET_SPHERE:		//Particles come from inside of a sphere.
		{
			float Radius = RAND_BETWEEN_TWO_FLOAT(0.0f, SphereRadius);
			float Theta = RAND_BETWEEN_TWO_FLOAT(0.0f, (float)ZE_PIx2);
			float Phi = RAND_BETWEEN_TWO_FLOAT(0.0f, ZE_PI);
			Particle.Position.x = Position.x + Radius * cosf(Theta) * sinf(Phi);
			Particle.Position.y = Position.y + Radius * sinf(Theta) * sinf(Phi);
			Particle.Position.z = Position.z + Radius * cosf(Phi);		
			break;
		}
	}

	Particle.UpVector.x					= RAND_BETWEEN_TWO_FLOAT(MinUpVector.x, MaxUpVector.x);
	Particle.UpVector.y					= RAND_BETWEEN_TWO_FLOAT(MinUpVector.y, MaxUpVector.y);
	Particle.UpVector.z					= RAND_BETWEEN_TWO_FLOAT(MinUpVector.z, MaxUpVector.z);
	ZEVector3::Normalize(Particle.UpVector, Particle.UpVector);

	Particle.Acceleration.x				= RAND_BETWEEN_TWO_FLOAT(MinAcceleration.x, MaxAcceleration.x);
	Particle.Acceleration.y				= RAND_BETWEEN_TWO_FLOAT(MinAcceleration.y, MaxAcceleration.y);
	Particle.Acceleration.z				= RAND_BETWEEN_TWO_FLOAT(MinAcceleration.z, MaxAcceleration.z);
	Particle.Velocity.x					= RAND_BETWEEN_TWO_FLOAT(MinVelocity.x, MaxVelocity.x);
	Particle.Velocity.y					= RAND_BETWEEN_TWO_FLOAT(MinVelocity.y, MaxVelocity.y);
	Particle.Velocity.z					= RAND_BETWEEN_TWO_FLOAT(MinVelocity.z, MaxVelocity.z);
	Particle.AngularAcceleration.x		= RAND_BETWEEN_TWO_FLOAT(MinAcceleration.x, MaxAcceleration.x);
	Particle.AngularAcceleration.y		= RAND_BETWEEN_TWO_FLOAT(MinAcceleration.y, MaxAcceleration.y);
	Particle.AngularAcceleration.z		= RAND_BETWEEN_TWO_FLOAT(MinAcceleration.z, MaxAcceleration.z);
	Particle.AngularVelocity.x			= RAND_BETWEEN_TWO_FLOAT(MinVelocity.x, MaxVelocity.x);
	Particle.AngularVelocity.y			= RAND_BETWEEN_TWO_FLOAT(MinVelocity.y, MaxVelocity.y);
	Particle.AngularVelocity.z			= RAND_BETWEEN_TWO_FLOAT(MinVelocity.z, MaxVelocity.z);
	Particle.Life						= RAND_BETWEEN_TWO_FLOAT(MinLife, MaxLife);
	Particle.Size						= RAND_BETWEEN_TWO_FLOAT(MinSize, MaxSize);
	Particle.BounceFactor				= RAND_BETWEEN_TWO_FLOAT(MinBounceFactor, MaxBounceFactor);
	Particle.Color.x					= RAND_BETWEEN_TWO_FLOAT(MinColor.x, MaxColor.x);
	Particle.Color.y					= RAND_BETWEEN_TWO_FLOAT(MinColor.y, MaxColor.y);
	Particle.Color.z					= RAND_BETWEEN_TWO_FLOAT(MinColor.z, MaxColor.z);
	Particle.Color.w					= RAND_BETWEEN_TWO_FLOAT(MinColor.w, MaxColor.w);
	Particle.IsAlive					= true;
}

void ZEParticleEmitter::AddParticleController(ZEParticleController* &ParticleController)
{
	ParticleController->SetOwner(this);
	ControllerArray.Add(ParticleController);
}

void ZEParticleEmitter::SetAcceleration(const ZEVector3& EmitterAcceleration)
{
	Acceleration = EmitterAcceleration;
}

const ZEVector3& ZEParticleEmitter::GetAcceleration() const
{
	return Acceleration;
}

void ZEParticleEmitter::SetName(const char* EmitterName)
{
	strcpy(Name, EmitterName);
}

const char* ZEParticleEmitter::GetName() const
{
	return Name;
}

void ZEParticleEmitter::SetOwner(ZEParticleSystem *EmitterOwner)
{
	Owner = EmitterOwner;
}

ZEParticleSystem* ZEParticleEmitter::GetOwner() const
{
	return Owner;
}

void ZEParticleEmitter::SetParticlesPerSecond(unsigned int Value)
{
	ParticlesPerSecond = Value;
}

unsigned int ZEParticleEmitter::GetParticlesPerSecond() const
{
	return ParticlesPerSecond;
}

void ZEParticleEmitter::SetPosition(const ZEVector3& EmitterPosition)
{
	Position = EmitterPosition;
}

const ZEVector3& ZEParticleEmitter::GetPosition() const
{
	return Position;
}

void ZEParticleEmitter::SetSphereRadius(float SphereRadius)
{
	this->SphereRadius = SphereRadius;
}

float ZEParticleEmitter::GetSphereRadius() const
{
	return SphereRadius;
}

void ZEParticleEmitter::SetTorusSize(const ZEVector2& TorusRadius)
{
	this->TorusSize = TorusRadius;
}

const ZEVector2& ZEParticleEmitter::GetTorusSize() const
{
	return TorusSize;
}

void ZEParticleEmitter::SetBoxSize(const ZEVector3& BoxSize)
{
	this->BoxSize = BoxSize;
}

const ZEVector3& ZEParticleEmitter::GetBoxSize() const
{
	return BoxSize;
}

void ZEParticleEmitter::SetType(ZEParticleEmitterType EmitterType)
{
	Type = EmitterType;
}

ZEParticleEmitterType ZEParticleEmitter::GetType() const
{
	return Type;
}

void ZEParticleEmitter::SetUpVector(const ZEVector3& EmitterUpVector)
{
	UpVector = EmitterUpVector;
}

const ZEVector3& ZEParticleEmitter::GetUpVector() const
{
	return UpVector;
}

void ZEParticleEmitter::SetVelocity(const ZEVector3& EmitterVelocity)
{
	Velocity = EmitterVelocity;
}

const ZEVector3& ZEParticleEmitter::GetVelocity() const
{
	return Velocity;
}

void ZEParticleEmitter::SetMinAcceleration(const ZEVector3& Acceleration)
{
	MinAcceleration = Acceleration;
}

const ZEVector3& ZEParticleEmitter::GetMinAcceleration() const
{
	return MinAcceleration;
}

void ZEParticleEmitter::SetMaxAcceleration(const ZEVector3& Acceleration)
{
	MaxAcceleration = Acceleration;
}

const ZEVector3& ZEParticleEmitter::GetMaxAcceleration() const
{
	return MaxAcceleration;
}

void ZEParticleEmitter::SetMinAngularAcceleration(const ZEVector3& AngularAcceleration)
{
	MinAngularAcceleration = AngularAcceleration;
}

const ZEVector3& ZEParticleEmitter::GetMinAngularAcceleration() const
{
	return MinAngularAcceleration;
}

void ZEParticleEmitter::SetMaxAngularAcceleration(const ZEVector3& AngularAcceleration)
{
	MaxAngularAcceleration = AngularAcceleration;
}

const ZEVector3& ZEParticleEmitter::GetMaxAngularAcceleration() const
{
	return MaxAngularAcceleration;
}

void ZEParticleEmitter::SetMinVelocity(const ZEVector3& Velocity)
{
	MinVelocity = Velocity;
}

const ZEVector3& ZEParticleEmitter::GetMinVelocity() const
{
	return MinVelocity;
}

void ZEParticleEmitter::SetMaxVelocity(const ZEVector3& Velocity)
{
	MaxVelocity = Velocity;
}

const ZEVector3& ZEParticleEmitter::GetMaxVelocity() const
{
	return MaxVelocity;
}

void ZEParticleEmitter::SetMinAngularVelocity(const ZEVector3& AngularVelocity)
{
	MinAngularVelocity = AngularVelocity;
}

const ZEVector3& ZEParticleEmitter::GetMinAngularVelocity() const
{
	return MinAngularVelocity;
}

void ZEParticleEmitter::SetMaxAngularVelocity(const ZEVector3& AngularVelocity)
{
	MaxAngularVelocity = AngularVelocity;
}

const ZEVector3& ZEParticleEmitter::GetMaxAngularVelocity() const
{
	return MaxAngularVelocity;
}

void ZEParticleEmitter::SetMinColor(const ZEVector4& Color)
{
	MinColor = Color;
}

const ZEVector4& ZEParticleEmitter::GetMinColor() const
{
	return MinColor;
}

void ZEParticleEmitter::SetMaxColor(const ZEVector4& Color)
{
	MaxColor = Color;
}

const ZEVector4& ZEParticleEmitter::GetMaxColor() const
{
	return MaxColor;
}

void ZEParticleEmitter::SetMinBounceFactor(float BounceFactor)
{
	MinBounceFactor = BounceFactor;
}

float ZEParticleEmitter::GetMinBounceFactor() const
{
	return MinBounceFactor;
}

void ZEParticleEmitter::SetMaxBounceFactor(float BounceFactor)
{
	MaxBounceFactor = BounceFactor;
}

float ZEParticleEmitter::GetMaxBounceFactor() const
{
	return MaxBounceFactor;
}

void ZEParticleEmitter::SetContinuity(bool Value)
{
	IsContinuous = Value;
}

bool ZEParticleEmitter::GetContinuity() const
{
	return IsContinuous;
}

void ZEParticleEmitter::SetMinLife(float Life)
{
	MinLife = Life;
}

float ZEParticleEmitter::GetMinLife() const
{
	return MinLife;
}

void ZEParticleEmitter::SetMaxLife(float Life)
{
	MaxLife = Life;
}

float ZEParticleEmitter::GetMaxLife() const
{
	return MaxLife;
}

void ZEParticleEmitter::SetMaxParticleCount(unsigned int Value)
{
	MaxParticleCount = Value;

	size_t OldSize = ParticlePool.GetCount();
	ParticlePool.Resize(Value);
	for (size_t I = OldSize; I < MaxParticleCount; I++)
		GenerateParticle(ParticlePool[I]);

}

unsigned int ZEParticleEmitter::GetMaxParticleCount() const
{
	return MaxParticleCount;
}

void ZEParticleEmitter::SetMinSize(float Size)
{
	MinSize = Size;
}

float ZEParticleEmitter::GetMinSize() const
{
	return MinSize;
}

void ZEParticleEmitter::SetMaxSize(float Size)
{
	MaxSize = Size;
}

float ZEParticleEmitter::GetMaxSize() const
{
	return MaxSize;
}

void ZEParticleEmitter::SetMinUpVector(const ZEVector3& UpVector)
{
	MinUpVector = UpVector;
}

const ZEVector3& ZEParticleEmitter::GetMinUpVector() const
{
	return MinUpVector;
}

void ZEParticleEmitter::SetMaxUpVector(const ZEVector3& UpVector)
{
	MaxUpVector = UpVector;
}

const ZEVector3& ZEParticleEmitter::GetMaxUpVector() const
{
	return MinUpVector;
}

ZEParticleEmitter::ZEParticleEmitter()
{
	EmittedParticleCount = 0;
	MaxParticleCount = 0;
	LastCreation = 0.0f;
	Owner = NULL;

	Velocity = ZEVector3(0.0f, 0.0f, 0.0f);		
	Acceleration = ZEVector3(0.0f, 0.0f, 0.0f);					
	ParticlesPerSecond = 0;				
	Position = ZEVector3(0.0f, 0.0f, 0.0f);						
	Type = ZE_PET_POINT;	
	MinAcceleration = ZEVector3(0.0f, 0.0f, 0.0f);			
	MaxAcceleration = ZEVector3(0.0f, 0.0f, 0.0f);			
	MinVelocity = ZEVector3(0.0f, 0.0f, 0.0f);				
	MaxVelocity = ZEVector3(0.0f, 0.0f, 0.0f);				
	MinAngularAcceleration = ZEVector3(0.0f, 0.0f, 0.0f);		
	MaxAngularAcceleration = ZEVector3(0.0f, 0.0f, 0.0f);		
	MinAngularVelocity = ZEVector3(0.0f, 0.0f, 0.0f);			
	MaxAngularVelocity = ZEVector3(0.0f, 0.0f, 0.0f);			
	MinColor = ZEVector4(0.0f, 0.0f, 0.0f, 1.0f);					
	MaxColor = ZEVector4(0.0f, 0.0f, 0.0f, 1.0f);
	MinUpVector = ZEVector3(0.0f, 1.0f, 0.0f);
	MaxUpVector = ZEVector3(0.0f, 1.0f, 0.0f);
	UpVector = ZEVector3(0.0f, 1.0f, 0.0f);
	
	MinSize = 0.0f;					
	MaxSize = 0.0f;					
	MinLife = 0.0f;					
	MaxLife = 0.0f;					
	MinBounceFactor = 0.0f;			
	MaxBounceFactor = 0.0f;			
	MaxParticleCount = 0;			
	IsContinuous = true;		

	BoxSize = ZEVector3(1.0f, 1.0f, 1.0f);
	SphereRadius = 1.0f;
	TorusSize = ZEVector2(1.0f, 1.0f);
}

ZEParticleEmitter::~ZEParticleEmitter()
{
}

ZEParticleEmitter* ZEParticleEmitter::CreateInstance()
{
	return new ZEParticleEmitter();
}

#include "ZEParticleEmitter.h.zpp"
