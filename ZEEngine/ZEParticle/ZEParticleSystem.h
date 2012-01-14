//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleSystem.h
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
#ifndef __ZE_PARTICLE_SYSTEM_H__
#define __ZE_PARTICLE_SYSTEM_H__

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEMath/ZEVector.h"
#include "ZEGraphics/ZERenderCommand.h"
#include "ZEMeta/ZEObject.h"

ZE_META_OBJECT_DESCRIPTION(ZEParticleSystem);

class ZEParticleEffect;
class ZEParticleEmitter;
class ZEParticleController;
class ZERenderer;

enum ZEParticleBillboardType
{
	ZE_PBT_WORLD_ORIENTED			= 0,
	ZE_PBT_VIEWPLANE_ORIENTED		= 1,
	ZE_PBT_VIEWPOINT_ORIENTED		= 2,
	ZE_PBT_CUSTOMAXIS_ORIENTED		= 3
};

class ZEStaricVertexBuffer;
class ZEMaterial;
class ZEStaticVertexBuffer;
struct ZEDrawParameters;

class ZEParticleSystem : public ZEObject
{
	ZE_META_OBJECT(ZEMaterial)

	private:
		ZEString							Name;						// Used in editor
		ZEParticleEffect*					Owner;						// Owner effect of the system
		bool								IsParticlePoolGenerated;
		ZEArray<ZEParticleEmitter*>			EmitterArray;				// These elements emits the particles from the particle pool
		ZEStaticVertexBuffer*				VertexBuffer;				// Holds the vertices
		ZERenderCommand						RenderCommand;					// Used for rendering
		ZEParticleBillboardType				BillboardType;				// Can be choosen from 3 types
		ZEMaterial*							ParticleMaterial;			// Material of the entire system
		bool								IsVertexBufferUpdated;		// A boolean for checking Vertex Buffer
		
		void								UpdateVertexBuffer(ZEDrawParameters* DrawParameters);

		unsigned int						GetTotalParticleCount();

	public:
		void								SetName(const ZEString& Name);
		const ZEString&						GetName() const;

		void								SetOwner(ZEParticleEffect* OwnerEffect);
		ZEParticleEffect*					GetOwner() const;

		void								SetBillboardType(ZEParticleBillboardType Type);
		ZEParticleBillboardType				GetBillboardType() const;

		void								SetMaterial(ZEMaterial *Material);
		ZEMaterial*							GetMaterial() const;

		void								Draw(ZEDrawParameters* DrawParameters);
		void								Tick(float TimeElapsed);	

		void								AddParticleEmitter(ZEParticleEmitter* ParticleEmitter);
		void								DeleteParticleEmitter(ZEParticleEmitter* ParticleEmitter);

		const ZEArray<ZEParticleEmitter*>&	GetEmitterArray();
	
											ZEParticleSystem();
											~ZEParticleSystem();

		static ZEParticleSystem*			CreateInstance();
};

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta> 
		<class name="ZEParticleSystem">
			<description>Particle System Class</description>
			<property name="Name" type="string" autogetset="yes" description="Name of the particle system."/>
			<property name="BillboardType" type="integer" autogetset="true" description="Billboarding type of the particles used in this system.">
				<enumurator name="ZEParticleBillboardType">
					<item name="World Oriented" value="ZE_PBT_WORLD_ORIENTED"/>
					<item name="ViewPlane Oriented" value="ZE_PBT_VIEWPLANE_ORIENTED"/>
					<item name="ViewPoint Oriented" value="ZE_PBT_VIEWPOINT_ORIENTED"/>
					<item name="Custom Axis Oriented" value="ZE_PBT_CUSTOMAXIS_ORIENTED"/>
				</enumurator>
			</property>
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/

#endif




