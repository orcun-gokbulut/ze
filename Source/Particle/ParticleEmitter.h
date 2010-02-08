//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ParticleEmitter.h
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

#pragma once
#ifndef __ZE_PARTICLE_EMITTER_H__
#define __ZE_PARTICLE_EMITTER_H__

#include "Definitions.h"
#include "ZEDS/Array.h"
#include "ZEMath/Vector.h"
#include "ParticleSystem.h"
#include "Particle.h"
#include "ParticleController.h"

enum ZEParticleEmitterType
{
	ZE_PET_POINT,						// Particles are emitted from a single point
	ZE_PET_BOX,							// Particles are emitted from inside of a 3D shaped box
	ZE_PET_TORUS,						// Particles are emitted from inside of a torus
	ZE_PET_SPHERE						// Particles are emitted from inside of a sphere
};

enum ZEParticleEmitterCollisionType
{
	ZE_PECT_NO_COLLISION,				// No collision effect - can be used in weather effects - uses GPU
	ZE_PECT_STATIC_COLLISION,			// Particles can collide to static objects - uses SIMD
	ZE_PECT_DYNAMIC_COLLISION			// Particles can collide to both static and dynamic objects - uses SIMD
};

class ZEParticleEmitter
{
	friend class ZEParticleSystem;

	private:
		ZEArray<ZEParticle>				ParticlePool;				// An array of 'MaxParticleCount' elements
		ZEArray<ZEParticleController*>	ControllerArray;			// These elements controls the particles after they are emitted from an emitter
		unsigned int					EmittedParticleCount;
		float							LastCreation;

		char							Name[ZE_MAX_NAME_SIZE];			// Name of the emitter - used in editor
		ZEParticleSystem*				Owner;					// System that this emitter belongs
		ZEVector3						Acceleration;					// This is acceleration of emitter NOT acceleration of particles
		unsigned int					ParticlesPerSecond;				// Number of particles that the emitter gets from the particle pool in a second.
		ZEVector3						Position;						// This is position of emitter NOT position of particles - Relative to the system
		ZEParticleEmitterType			Type;							// Type from 4 available types
		ZEVector3						UpVector;						// This is up vector of emitter NOT up vector of particles - Relative to the system
		ZEVector3						Velocity;						// This is velocity of emitter NOT velocity of particles
		struct 			// Since there's 3 different types, a structure is needed for the size variable.
		{
			ZEVector3 Box;
			float SphereRadius;
			ZEVector2 Torus;		
		} Size;

		// Following variables are used to create the Particle Pool
		ZEVector3						MinUpVector;				// Orientation of-
		ZEVector3						MaxUpVector;				//particles
		ZEVector3						MinAcceleration;			// Acceleration of-
		ZEVector3						MaxAcceleration;			//particles
		ZEVector3						MinVelocity;				// Velocity of-
		ZEVector3						MaxVelocity;				//particles
		ZEVector3						MinAngularAcceleration;		// Angular acceleration of-
		ZEVector3						MaxAngularAcceleration;		//particles
		ZEVector3						MinAngularVelocity;			// Angular velocity of-
		ZEVector3						MaxAngularVelocity;			//particles
		ZEVector4						MinColor;					// Color value of-
		ZEVector4						MaxColor;					//particles in RGBA
		float							MinSize;					// Size of an edge of-
		float							MaxSize;					//particles
		float							MinLife;					// Life of the-
		float							MaxLife;					//particles
		float							MinBounceFactor;			// Bounce factor of the-
		float							MaxBounceFactor;			//particles	
		unsigned int					MaxParticleCount;			// Maximum number of particles of this emitter
		bool							IsContinuous;					// Repeats the animation if true

		// These are the controllers for manipulating emitter values
		ZEParticleController			ParticlesPerSecondController;
		ZEParticleController			AccelerationController;
		ZEParticleController			VelocityController;
		ZEParticleController			AngularAccelerationController;
		ZEParticleController			AngularVelocityController;
		ZEParticleController			ColorController;
		ZEParticleController			SizeController;
		ZEParticleController			LifeController;


		//	Required functions for emitter
		void 							InitializeParticlePool();
		void							GenerateParticle(ZEParticle &Particle);

	public:
		void							Tick(float TimeElapsed);
		
		void							AddParticleController(ZEParticleController* &ParticleController);

		void							SetName(const char* EmitterName);
		const char*						GetName();	

		void							SetOwner(ZEParticleSystem* EmitterOwner);
		ZEParticleSystem*				GetOwner();

		void							SetPosition(const ZEVector3& EmitterPosition);
		const ZEVector3&				GetPosition();

		void							SetParticlesPerSecond(unsigned int Value);
		unsigned int					GetParticlesPerSecond();

		void							SetBoxEmitterSize(const ZEVector3& BoxSize);
		const ZEVector3&				GetBoxEmitterSize();

		void							SetSphereEmitterSize(float SphereRadius);
		float							GetSphereEmitterSize();

		void							SetTorusEmitterSize(const ZEVector2& TorusRadii);
		const ZEVector2&				GetTorusEmitterSize();

		void							SetType(ZEParticleEmitterType EmitterType);
		ZEParticleEmitterType			GetType();

		void							SetAcceleration(const ZEVector3& EmitterAcceleration);
		const ZEVector3&				GetAcceleration();

		void							SetUpVector(const ZEVector3& EmitterUpVector);
		const ZEVector3&				GetUpVector();

		void							SetVelocity(const ZEVector3& EmitterVelocity);
		const ZEVector3&				GetVelocity() ;

		void							SetMaxParticleCount(unsigned int Value);
		unsigned int					GetMaxParticleCount();

		void							SetContinuity(bool Value);
		bool							GetContinuity();

		void							SetMinUpVector(const ZEVector3& UpVector);
		const ZEVector3&				GetMinUpVector();
		void							SetMaxUpVector(const ZEVector3& UpVector);
		const ZEVector3&				GetMaxUpVector();

		void							SetMinAcceleration(const ZEVector3& Acceleration);
		const ZEVector3&				GetMinAcceleration();
		void							SetMaxAcceleration(const ZEVector3& Acceleration);
		const ZEVector3&				GetMaxAcceleration();

		void							SetMinVelocity(const ZEVector3& Velocity);
		const ZEVector3&				GetMinVelocity();
		void							SetMaxVelocity(const ZEVector3& Velocity);
		const ZEVector3&				GetMaxVelocity();

		void 							SetMinAngularAcceleration(const ZEVector3& AngularAcceleration);
		const ZEVector3& 				GetMinAngularAcceleration();
		void 							SetMaxAngularAcceleration(const ZEVector3& AngularAcceleration);
		const ZEVector3& 				GetMaxAngularAcceleration();

		void 							SetMinAngularVelocity(const ZEVector3& AngularVelocity);
		const ZEVector3&				GetMinAngularVelocity();
		void 							SetMaxAngularVelocity(const ZEVector3& AngularVelocity);
		const ZEVector3&				GetMaxAngularVelocity();

		void							SetMinSize(float Size);
		float 							GetMinSize();
		void 							SetMaxSize(float Size);
		float 							GetMaxSize();

		void 							SetMinLife(float Life);
		float 							GetMinLife();
		void 							SetMaxLife(float Life);
		float 							GetMaxLife();

		void 							SetMinColor(const ZEVector4& Color);
		const ZEVector4&				GetMinColor();
		void 							SetMaxColor(const ZEVector4& Color);
		const ZEVector4&				GetMaxColor();

		void 							SetMinBounceFactor(float BounceFactor);
		float 							GetMinBounceFactor();
		void 							SetMaxBounceFactor(float BounceFactor);
		float 							GetMaxBounceFactor();

										ZEParticleEmitter();
										~ZEParticleEmitter();
};

#endif
