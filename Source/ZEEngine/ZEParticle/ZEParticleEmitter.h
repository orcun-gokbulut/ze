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
	ZE_PET_PLANE,
	ZE_PET_BOX,
	ZE_PET_TORUS,
	ZE_PET_SPHERE
};


enum ZEParticleBillboardType
{
	ZE_PBT_AXIS_ORIENTED		= 0,
	ZE_PBT_SCREEN_ALIGNED		= 1,
	ZE_PBT_VIEW_POINT_ORIENTED	= 2,
};

class ZEStaticVertexBuffer;
class ZEParticleEffect;
class ZEParticleModifier;
class ZEDrawParameters;

class ZEParticleEmitter : public ZEObject
{
	ZE_META_OBJECT(ZEParticleEmitter)

		friend class ZEParticleModifier;

	private:

		ZEString						Name;
		ZEParticleEffect*				Owner;

		ZEArray<ZEParticle>				ParticlePool;
		ZEArray<ZEParticle>				SortArray;
		ZEArray<ZEParticleModifier*>	Modifiers;

		ZEMaterial*						Material;
		ZEStaticVertexBuffer*			VertexBuffer;
		ZERenderCommand					RenderCommand;

		ZEParticleBillboardType			BillboardType;

		ZEVector3						AxisOrientation;


		ZEUInt							MaxParticleCount;
		ZEUInt							ParticlesPerSecond;
		float							ParticlesRemaining;
		ZEUInt							EmittedParticleCount;
		ZEUInt							AliveParticleCount;

		bool							IsEmmiterLocal;
		bool							IsSortingEnabled;
		bool							IsParticleFixedAspectRatio;

		ZEAABBox						BoundingBox;		
		ZEVector3						Position;
		ZEParticleEmitterType			Type;

		ZEVector3						BoxSize;
		float							SphereRadius;
		ZEVector2						TorusSize;
		ZEVector2						PlaneSize;

		ZEVector4						MinColor;
		ZEVector4						MaxColor;
		ZEVector2						MinSize;
		ZEVector2						MaxSize;
		float							MinLife;
		float							MaxLife;

		bool							IsContinuous;

		void 							InitializeParticlePool();
		void							GenerateParticle(ZEParticle &Particle);
		void							UpdateVertexBuffer(ZEDrawParameters* DrawParameters);

		void							SortParticles();

										ZEParticleEmitter();
										~ZEParticleEmitter();

	protected:

		ZEArray<ZEParticle>&			GetParticlePool();

	public:

		void							SetName(const ZEString& Name);
		const ZEString&					GetName() const;	

		void							SetOwner(ZEParticleEffect* Owner);
		ZEParticleEffect*				GetOwner() const;

		void							SetEmmiterLocal(bool IsLocal);
		bool							GetIsEmmiterLocal() const;

		void							SetSortingEnabled(bool IsSorted);
		bool							GetSortingEnabled() const;

		void							SetParticleFixedAspectRatio(bool IsParticleFixedAspectRatio);
		bool							GetParticleFixedAspectRatio() const;

		void							SetPosition(const ZEVector3& Position);
		const ZEVector3&				GetPosition() const;

		const ZEAABBox&					GetBoundingBox();

		const ZEArray<ZEParticleModifier*>&	GetModifiers();
		void							AddModifier(ZEParticleModifier* Modifier);
		void							RemoveModifier(ZEParticleModifier* Modifier);

		void							SetParticlesPerSecond(ZEUInt Value);
		ZEUInt							GetParticlesPerSecond() const;

		ZEUInt							GetAliveParticleCount() const;

		void							SetPlaneSize(const ZEVector2& PlaneSize);
		const ZEVector2&				GetPlaneSize() const;

		void							SetBoxSize(const ZEVector3& BoxSize);
		const ZEVector3&				GetBoxSize() const;

		void							SetSphereRadius(float SphereRadius);
		float							GetSphereRadius() const;

		void							SetTorusSize(const ZEVector2& TorusRadius);
		const ZEVector2&				GetTorusSize() const;

		void							SetType(ZEParticleEmitterType EmitterType);
		ZEParticleEmitterType			GetType() const;

		void							SetMaxParticleCount(ZEUInt Value);
		ZEUInt							GetMaxParticleCount() const;

		void							SetContinuity(bool Value);
		bool							GetContinuity() const;

		void							SetMinSize(const ZEVector2& Size);
		const ZEVector2&				GetMinSize() const;
		void 							SetMaxSize(const ZEVector2& Size);
		const ZEVector2&				GetMaxSize() const;

		void 							SetMinLife(float Life);
		float 							GetMinLife() const;
		void 							SetMaxLife(float Life);
		float 							GetMaxLife() const;

		void 							SetMinColor(const ZEVector4& Color);
		const ZEVector4&				GetMinColor() const;
		void 							SetMaxColor(const ZEVector4& Color);
		const ZEVector4&				GetMaxColor() const;

		void							SetBillboardType(ZEParticleBillboardType Type);
		ZEParticleBillboardType			GetBillboardType() const;

		void							SetAxisOrientation(const ZEVector3& Orientation);
		const ZEVector3&				GetAxisOrientation() const;

		void							SetMaterial(ZEMaterial *Material);
		ZEMaterial*						GetMaterial() const;

		void							Tick(float TimeElapsed);
		void							Draw(ZEDrawParameters* DrawParameters);

		static ZEParticleEmitter*		CreateInstance();

		void							LogSort();
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
					<item name="Plane" value="ZE_PET_PLANE"/> 
					<item name="Box" value="ZE_PET_BOX"/> 
					<item name="Torus" value="ZE_PET_TORUS"/>
					<item name="Sphere" value="ZE_PET_SPHERE"/>
				</enumurator>
			</property>
			<property name="Position" type="ZEVector3" autogetset="yes" description="Position of the particle emitter."/>
			<property name="PlaneSize" groupname="Emitter Sizes" type="ZEVector2" autogetset="true" description="Size of plane emitter."/>
			<property name="BoxSize" groupname="Emitter Sizes" type="ZEVector3" autogetset="true" description="Size of box emitter."/>
			<property name="TorusSize" groupname="Emitter Sizes" type="ZEVector2" autogetset="true" description="Size of torus emitter."/>
			<property name="SphereRadius" groupname="Emitter Sizes" type="float" autogetset="true" description="Size of sphere emitter."/>
			<property name="MaxParticleCount" groupname="Particle Generation" type="integer" autogetset="true" description="Maximum number of particles will be emitted from this emitter."/>
			<property name="ParticlesPerSecond" groupname="Particle Generation" type="integer" autogetset="true" description="Paritcles per secon emitted from this source."/>
			<property name="Continuity" groupname="Particle Generation" type="boolean" autogetset="true" description="Continuity of particles emitted from this emitter."/>			
			<property name="MinSize" groupname="Particle Generation" type="ZEVector2" autogetset="true" description="Minimum edge lenght of particles."/>
			<property name="MaxSize" groupname="Particle Generation" type="ZEVector2" autogetset="true" description="Maximum edge lenght of particles."/>			
			<property name="MinLife" groupname="Particle Generation" type="float" autogetset="true" description="Minimum life of particles."/>
			<property name="MaxLife" groupname="Particle Generation" type="float" autogetset="true" description="Maximum life of particles."/>			
			<property name="MinColor" groupname="Particle Generation" type="ZEVector4" semantic="ZE_PS_COLOR" autogetset="true" description="Minimum color of particles."/>
			<property name="MaxColor" groupname="Particle Generation" type="ZEVector4" semantic="ZE_PS_COLOR" autogetset="true" description="Maximum color of particles."/>				
			<property name="BillboardType" type="integer" autogetset="true" description="Billboarding type of the particles used in this system.">
				<enumurator name="ZEParticleBillboardType">
					<item name="None" value="ZE_PBT_AXIS_ORIENTED"/>
					<item name="Screen Aligned" value="ZE_PBT_SCREEN_ALIGNED"/>
					<item name="ViewPoint Oriented" value="ZE_PBT_VIEW_POINT_ORIENTED"/>
				</enumurator>
			</property>
			</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/


#endif
