//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleEmitter.h
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

#include "ZETypes.h"
#include "ZEMeta/ZEObject.h"
#include "ZEMath/ZEVector.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEArray.h"
#include "ZEGraphics/ZERenderCommand.h"
#include "ZEParticle.h"
#include "ZEMath/ZEAABBox.h"


ZE_META_OBJECT_DESCRIPTION(ZEParticleEmitter);

enum ZEParticleEmitterType
{
	ZE_PET_POINT,
	ZE_PET_BOX,
	ZE_PET_TORUS,
	ZE_PET_SPHERE
};


enum ZEParticleBillboardType
{
	ZE_PBT_NONE					= 0,
	ZE_PBT_SCREEN_ALIGNED		= 1,
	ZE_PBT_VIEW_PLANE_ALIGNED	= 2,
	ZE_PBT_VIEW_POINT_ORIENTED	= 3
};

class ZEStaticVertexBuffer;
class ZEParticleEffect;
class ZEParticleModifier;
class ZEDrawParameters;

class ZEParticleEmitter : public ZEObject
{
	ZE_META_OBJECT(ZEParticleEmitter)
	private:
		ZEString						Name;
		ZEParticleEffect*				Owner;

		ZEAABBox						BoundingBox;

		ZEVector3						Acceleration;
		ZEUInt							ParticlesPerSecond;
		ZEVector3						Position;
		ZEParticleEmitterType			Type;
		ZEVector3						UpVector;
		ZEVector3						Velocity;

		ZEVector3						BoxSize;
		float							SphereRadius;
		ZEVector2						TorusSize;

		ZEVector3						MinUpVector;	
		ZEVector3						MaxUpVector;
		ZEVector3						MinAcceleration;
		ZEVector3						MaxAcceleration;
		ZEVector3						MinVelocity;
		ZEVector3						MaxVelocity;
		ZEVector3						MinAngularAcceleration;
		ZEVector3						MaxAngularAcceleration;
		ZEVector3						MinAngularVelocity;
		ZEVector3						MaxAngularVelocity;
		ZEVector4						MinColor;
		ZEVector4						MaxColor;
		float							MinSize;
		float							MaxSize;
		float							MinLife;
		float							MaxLife;
		float							MinBounceFactor;
		float							MaxBounceFactor;
		ZEUInt							MaxParticleCount;
		bool							IsContinuous;
		
		ZEArray<ZEParticle>				ParticlePool;
		ZEArray<ZEParticleModifier*>	Modifiers;
		ZEUInt							EmittedParticleCount;
		float							LastCreation;

		ZEParticleBillboardType			BillboardType;
		ZEMaterial*						Material;
		ZEStaticVertexBuffer*			VertexBuffer;
		ZERenderCommand					RenderCommand;

		void 							InitializeParticlePool();
		void							GenerateParticle(ZEParticle &Particle);
		void							UpdateVertexBuffer(ZEDrawParameters* DrawParameters);

										ZEParticleEmitter();
										~ZEParticleEmitter();

	public:
		void							SetName(const ZEString& Name);
		const ZEString&					GetName() const;	

		void							SetOwner(ZEParticleEffect* Owner);
		ZEParticleEffect*				GetOwner() const;

		void							SetPosition(const ZEVector3& Position);
		const ZEVector3&				GetPosition() const;

		const ZEAABBox&					GetBoundingBox();

		const ZEArray<ZEParticleModifier*>&	GetModifiers();
		void							AddModifier(ZEParticleModifier* Modifier);
		void							RemoveModifier(ZEParticleModifier* Modifier);

		void							SetParticlesPerSecond(ZEUInt Value);
		ZEUInt							GetParticlesPerSecond() const;

		void							SetBoxSize(const ZEVector3& BoxSize);
		const ZEVector3&				GetBoxSize() const;

		void							SetSphereRadius(float SphereRadius);
		float							GetSphereRadius() const;

		void							SetTorusSize(const ZEVector2& TorusRadius);
		const ZEVector2&				GetTorusSize() const;

		void							SetType(ZEParticleEmitterType EmitterType);
		ZEParticleEmitterType			GetType() const;

		void							SetAcceleration(const ZEVector3& EmitterAcceleration);
		const ZEVector3&				GetAcceleration() const;

		void							SetUpVector(const ZEVector3& EmitterUpVector);
		const ZEVector3&				GetUpVector() const;

		void							SetVelocity(const ZEVector3& EmitterVelocity);
		const ZEVector3&				GetVelocity() const;

		void							SetMaxParticleCount(ZEUInt Value);
		ZEUInt							GetMaxParticleCount() const;

		void							SetContinuity(bool Value);
		bool							GetContinuity() const;

		void							SetMinUpVector(const ZEVector3& UpVector);
		const ZEVector3&				GetMinUpVector() const;
		void							SetMaxUpVector(const ZEVector3& UpVector);
		const ZEVector3&				GetMaxUpVector() const;

		void							SetMinAcceleration(const ZEVector3& Acceleration);
		const ZEVector3&				GetMinAcceleration() const;
		void							SetMaxAcceleration(const ZEVector3& Acceleration);
		const ZEVector3&				GetMaxAcceleration() const;

		void							SetMinVelocity(const ZEVector3& Velocity);
		const ZEVector3&				GetMinVelocity() const;
		void							SetMaxVelocity(const ZEVector3& Velocity);
		const ZEVector3&				GetMaxVelocity() const;

		void 							SetMinAngularAcceleration(const ZEVector3& AngularAcceleration);
		const ZEVector3& 				GetMinAngularAcceleration() const;
		void 							SetMaxAngularAcceleration(const ZEVector3& AngularAcceleration);
		const ZEVector3& 				GetMaxAngularAcceleration() const;

		void 							SetMinAngularVelocity(const ZEVector3& AngularVelocity);
		const ZEVector3&				GetMinAngularVelocity() const;
		void 							SetMaxAngularVelocity(const ZEVector3& AngularVelocity);
		const ZEVector3&				GetMaxAngularVelocity() const;

		void							SetMinSize(float Size);
		float 							GetMinSize() const;
		void 							SetMaxSize(float Size);
		float 							GetMaxSize() const;

		void 							SetMinLife(float Life);
		float 							GetMinLife() const;
		void 							SetMaxLife(float Life);
		float 							GetMaxLife() const;

		void 							SetMinColor(const ZEVector4& Color);
		const ZEVector4&				GetMinColor() const;
		void 							SetMaxColor(const ZEVector4& Color);
		const ZEVector4&				GetMaxColor() const;

		void 							SetMinBounceFactor(float BounceFactor);
		float 							GetMinBounceFactor() const;
		void 							SetMaxBounceFactor(float BounceFactor);
		float 							GetMaxBounceFactor() const;

		void							SetBillboardType(ZEParticleBillboardType Type);
		ZEParticleBillboardType			GetBillboardType() const;

		void							SetMaterial(ZEMaterial *Material);
		ZEMaterial*						GetMaterial() const;

		void							Tick(float TimeElapsed);
		void							Draw(ZEDrawParameters* DrawParameters);

		static ZEParticleEmitter*		CreateInstance();
};

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta> 
		<class name="ZEParticleEmitter">
			<description>Particle Emitter Class</description>
			<property name="Name" type="string" autogetset="yes" description="Name of the particle emitter."/>
			<property name="Type" type="integer" autogetset="true" description="Type of the emitter.">
				<enumurator name="ZEParticleEmitterType">
					<item name="Point" value="ZE_PET_POINT"/>
					<item name="Box" value="ZE_PET_BOX"/> 
					<item name="Torus" value="ZE_PET_TORUS"/>
					<item name="Sphere" value="ZE_PET_SPHERE"/>
				</enumurator>
			</property>
			<property name="Position" type="ZEVector3" autogetset="yes" description="Position of the particle emitter."/>
			<property name="Acceleration" type="ZEVector3" autogetset="true" description="Acceleration of particles emitted from this emitter."/>
			<property name="Velocity" type="ZEVector3" autogetset="true" description="Velocity of particles emitted from this emitter."/>

			<property name="BoxSize" groupname="Emitter Sizes" type="ZEVector3" autogetset="true" description="Size of box emitter."/>
			<property name="TorusSize" groupname="Emitter Sizes" type="ZEVector2" autogetset="true" description="Size of torus emitter."/>
			<property name="SphereRadius" groupname="Emitter Sizes" type="float" autogetset="true" description="Size of sphere emitter."/>

			<property name="MaxParticleCount" groupname="Particle Generation" type="integer" autogetset="true" description="Maximum number of particles will be emitted from this emitter."/>
			<property name="ParticlesPerSecond" groupname="Particle Generation" type="integer" autogetset="true" description="Paritcles per secon emitted from this source."/>
			<property name="Continuity" groupname="Particle Generation" type="boolean" autogetset="true" description="Continuity of particles emitted from this emitter."/>
			<property name="MinAcceleration" groupname="Acceleration" type="ZEVector3" autogetset="true" description="Minimum acceleration of particles emitted from this emitter."/>
			<property name="MaxAcceleration" groupname="Acceleration" type="ZEVector3" autogetset="true" description="Maximum acceleration of particles emitted from this emitter."/>
			<property name="MaxVelocity" groupname="Velocity" type="ZEVector3" autogetset="true" description="Maximum velocity of particles emitted from this emitter."/>
			<property name="MinVelocity" groupname="Velocity" type="ZEVector3" autogetset="true" description="Minimum velocity of particles emitted from this emitter."/>	
			<property name="MinAngularAcceleration" groupname="Acceleration" type="ZEVector3" autogetset="true" description="Minimum angular acceleration of particals."/>
			<property name="MaxAngularAcceleration" groupname="Acceleration" type="ZEVector3" autogetset="true" description="Maximum angular acceleration of particals."/>
			<property name="MinAngularVelocity" groupname="Velocity" type="ZEVector3" autogetset="true" description="Minimum angular velocity of particals."/>
			<property name="MaxAngularVelocity" groupname="Velocity" type="ZEVector3" autogetset="true" description="Maximum angular velocity of particals."/>		
			<property name="MinSize" groupname="Particle Generation" type="float" autogetset="true" description="Minimum edge lenght of particles."/>
			<property name="MaxSize" groupname="Particle Generation" type="float" autogetset="true" description="Maximum edge lenght of particles."/>			
			<property name="MinLife" groupname="Particle Generation" type="float" autogetset="true" description="Minimum life of particles."/>
			<property name="MaxLife" groupname="Particle Generation" type="float" autogetset="true" description="Maximum life of particles."/>			
			<property name="MinColor" groupname="Particle Generation" type="ZEVector4" semantic="ZE_PS_COLOR" autogetset="true" description="Minimum color of particles."/>
			<property name="MaxColor" groupname="Particle Generation" type="ZEVector4" semantic="ZE_PS_COLOR" autogetset="true" description="Maximum color of particles."/>			
			<property name="MinBounceFactor" groupname="Physics" type="float" autogetset="true" description="Minimum  bounce factor of particles."/>
			<property name="MaxBounceFactor" groupname="Physics" type="float" autogetset="true" description="Maximum bounce factor of particles."/>
			<property name="BillboardType" type="integer" autogetset="true" description="Billboarding type of the particles used in this system.">
				<enumurator name="ZEParticleBillboardType">
					<item name="None" value="ZE_PBT_NONE"/>
					<item name="Screen Aligned" value="ZE_PBT_SCREEN_ALIGNED"/>
					<item name="ViewPlane Aligned" value="ZE_PBT_VIEW_PLANE_ALIGNED"/>
					<item name="ViewPoint Oriented" value="ZE_PBT_VIEW_POINT_ORIENTED"/>
				</enumurator>
			</property>
			</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/


#endif
