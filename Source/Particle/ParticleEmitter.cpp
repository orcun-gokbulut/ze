//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ParticleEmitter.cpp
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
#include "ParticleEffect.h"
#include "ParticleSystem.h"
#include "ParticleEmitter.h"

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
		Particle.Position.x = Position.x + RAND_BETWEEN_TWO_FLOAT(-Size.Box.x / 2, Size.Box.x / 2);
		Particle.Position.y = Position.y + RAND_BETWEEN_TWO_FLOAT(-Size.Box.y / 2, Size.Box.y / 2);
		Particle.Position.z = Position.z + RAND_BETWEEN_TWO_FLOAT(-Size.Box.z / 2, Size.Box.z / 2);
		break;
	case ZE_PET_TORUS:		// Particles come from inside of a torus
		{
			float Theta = RAND_BETWEEN_TWO_FLOAT(0.0f, (float)ZE_PIx2);
			float Phi = RAND_BETWEEN_TWO_FLOAT(0.0f, (float)ZE_PIx2);
			float TubeRadius = RAND_BETWEEN_TWO_FLOAT(0.0f, Size.Torus.y);
			Particle.Position.x = Position.x + (Size.Torus.x + TubeRadius * cosf(Phi)) * cosf(Theta);
			Particle.Position.y = Position.y + (Size.Torus.x + TubeRadius * cosf(Phi)) * sinf(Theta);
			Particle.Position.z = Position.z + TubeRadius * sinf(Phi);		
			break;
		}
	case ZE_PET_SPHERE:		//Particles come from inside of a sphere.
		{
			float Radius = RAND_BETWEEN_TWO_FLOAT(0.0f, Size.SphereRadius);
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

const ZEVector3& ZEParticleEmitter::GetAcceleration()
{
	return Acceleration;
}

void ZEParticleEmitter::SetName(const char* EmitterName)
{
	strcpy(Name, EmitterName);
}

const char* ZEParticleEmitter::GetName()
{
	return Name;
}

void ZEParticleEmitter::SetOwner(ZEParticleSystem *EmitterOwner)
{
	Owner = EmitterOwner;
}

ZEParticleSystem* ZEParticleEmitter::GetOwner()
{
	return Owner;
}

void ZEParticleEmitter::SetParticlesPerSecond(unsigned int Value)
{
	ParticlesPerSecond = Value;
}

unsigned int ZEParticleEmitter::GetParticlesPerSecond()
{
	return ParticlesPerSecond;
}

void ZEParticleEmitter::SetPosition(const ZEVector3& EmitterPosition)
{
	Position = EmitterPosition;
}

const ZEVector3& ZEParticleEmitter::GetPosition()
{
	return Position;
}

void ZEParticleEmitter::SetSphereEmitterSize(float SphereRadius)
{
	Size.SphereRadius = SphereRadius;

	Size.Box.x = 0.0f;
	Size.Box.y = 0.0f;
	Size.Box.z = 0.0f;
	Size.Torus.x = 0.0f;
	Size.Torus.y = 0.0f;
}

float ZEParticleEmitter::GetSphereEmitterSize()
{
	return Size.SphereRadius;
}

void ZEParticleEmitter::SetTorusEmitterSize(const ZEVector2& TorusRadii)
{
	Size.Torus.x = TorusRadii.x;
	Size.Torus.y = TorusRadii.y;

	Size.SphereRadius = 0.0f;
	Size.Box.x = 0.0f;
	Size.Box.y = 0.0f;
	Size.Box.z = 0.0f;
}

const ZEVector2& ZEParticleEmitter::GetTorusEmitterSize()
{
	return Size.Torus;
}

void ZEParticleEmitter::SetBoxEmitterSize(const ZEVector3& BoxSize)
{
	Size.Box.x = BoxSize.x;
	Size.Box.y = BoxSize.y;
	Size.Box.z = BoxSize.z;

	Size.Torus.x = 0.0f;
	Size.Torus.y = 0.0f;
	Size.SphereRadius = 0.0f;
}

const ZEVector3& ZEParticleEmitter::GetBoxEmitterSize()
{
	return Size.Box;
}

void ZEParticleEmitter::SetType(ZEParticleEmitterType EmitterType)
{
	Type = EmitterType;
}

ZEParticleEmitterType ZEParticleEmitter::GetType()
{
	return Type;
}

void ZEParticleEmitter::SetUpVector(const ZEVector3& EmitterUpVector)
{
	UpVector = EmitterUpVector;
}

const ZEVector3& ZEParticleEmitter::GetUpVector()
{
	return UpVector;
}

void ZEParticleEmitter::SetVelocity(const ZEVector3& EmitterVelocity)
{
	Velocity = EmitterVelocity;
}

const ZEVector3& ZEParticleEmitter::GetVelocity()
{
	return Velocity;
}

void ZEParticleEmitter::SetMinAcceleration(const ZEVector3& Acceleration)
{
	MinAcceleration = Acceleration;
}

const ZEVector3& ZEParticleEmitter::GetMinAcceleration()
{
	return MinAcceleration;
}

void ZEParticleEmitter::SetMaxAcceleration(const ZEVector3& Acceleration)
{
	MaxAcceleration = Acceleration;
}

const ZEVector3& ZEParticleEmitter::GetMaxAcceleration()
{
	return MaxAcceleration;
}

void ZEParticleEmitter::SetMinAngularAcceleration(const ZEVector3& AngularAcceleration)
{
	MinAngularAcceleration = AngularAcceleration;
}

const ZEVector3& ZEParticleEmitter::GetMinAngularAcceleration()
{
	return MinAngularAcceleration;
}

void ZEParticleEmitter::SetMaxAngularAcceleration(const ZEVector3& AngularAcceleration)
{
	MaxAngularAcceleration = AngularAcceleration;
}

const ZEVector3& ZEParticleEmitter::GetMaxAngularAcceleration()
{
	return MaxAngularAcceleration;
}

void ZEParticleEmitter::SetMinVelocity(const ZEVector3& Velocity)
{
	MinVelocity = Velocity;
}

const ZEVector3& ZEParticleEmitter::GetMinVelocity()
{
	return MinVelocity;
}

void ZEParticleEmitter::SetMaxVelocity(const ZEVector3& Velocity)
{
	MaxVelocity = Velocity;
}

const ZEVector3& ZEParticleEmitter::GetMaxVelocity()
{
	return MaxVelocity;
}

void ZEParticleEmitter::SetMinAngularVelocity(const ZEVector3& AngularVelocity)
{
	MinAngularVelocity = AngularVelocity;
}

const ZEVector3& ZEParticleEmitter::GetMinAngularVelocity()
{
	return MinAngularVelocity;
}

void ZEParticleEmitter::SetMaxAngularVelocity(const ZEVector3& AngularVelocity)
{
	MaxAngularVelocity = AngularVelocity;
}

const ZEVector3& ZEParticleEmitter::GetMaxAngularVelocity()
{
	return MaxAngularVelocity;
}

void ZEParticleEmitter::SetMinColor(const ZEVector4& Color)
{
	MinColor = Color;
}

const ZEVector4& ZEParticleEmitter::GetMinColor()
{
	return MinColor;
}

void ZEParticleEmitter::SetMaxColor(const ZEVector4& Color)
{
	MaxColor = Color;
}

const ZEVector4& ZEParticleEmitter::GetMaxColor()
{
	return MaxColor;
}

void ZEParticleEmitter::SetMinBounceFactor(float BounceFactor)
{
	MinBounceFactor = BounceFactor;
}

float ZEParticleEmitter::GetMinBounceFactor()
{
	return MinBounceFactor;
}

void ZEParticleEmitter::SetMaxBounceFactor(float BounceFactor)
{
	MaxBounceFactor = BounceFactor;
}

float ZEParticleEmitter::GetMaxBounceFactor()
{
	return MaxBounceFactor;
}

void ZEParticleEmitter::SetContinuity(bool Value)
{
	IsContinuous = Value;
}

bool ZEParticleEmitter::GetContinuity()
{
	return IsContinuous;
}

void ZEParticleEmitter::SetMinLife(float Life)
{
	MinLife = Life;
}

float ZEParticleEmitter::GetMinLife()
{
	return MinLife;
}

void ZEParticleEmitter::SetMaxLife(float Life)
{
	MaxLife = Life;
}

float ZEParticleEmitter::GetMaxLife()
{
	return MaxLife;
}

void ZEParticleEmitter::SetMaxParticleCount(unsigned int Value)
{
	MaxParticleCount = Value;
}

unsigned int ZEParticleEmitter::GetMaxParticleCount()
{
	return MaxParticleCount;
}

void ZEParticleEmitter::SetMinSize(float Size)
{
	MinSize = Size;
}

float ZEParticleEmitter::GetMinSize()
{
	return MinSize;
}

void ZEParticleEmitter::SetMaxSize(float Size)
{
	MaxSize = Size;
}

float ZEParticleEmitter::GetMaxSize()
{
	return MaxSize;
}

void ZEParticleEmitter::SetMinUpVector(const ZEVector3& UpVector)
{
	MinUpVector = UpVector;
}

const ZEVector3& ZEParticleEmitter::GetMinUpVector()
{
	return MinUpVector;
}

void ZEParticleEmitter::SetMaxUpVector(const ZEVector3& UpVector)
{
	MaxUpVector = UpVector;
}

const ZEVector3& ZEParticleEmitter::GetMaxUpVector()
{
	return MinUpVector;
}

ZEParticleEmitter::ZEParticleEmitter()
{
	EmittedParticleCount = 0;
	LastCreation = 0.0f;
	Owner = NULL;
}

ZEParticleEmitter::~ZEParticleEmitter()
{
}
