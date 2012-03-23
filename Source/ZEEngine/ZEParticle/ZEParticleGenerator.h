//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleGenerator.h
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
#ifndef __ZE_PARTICLE_GENERETOR_H__
#define __ZE_PARTICLE_GENERETOR_H__

#include "ZEParticle.h"
#include "ZEDS\ZEArray.h"
#include "ZEMeta\ZEObject.h"

ZE_META_OBJECT_DESCRIPTION(ZEParticleGenerator)

class ZEParticleGenerator : public ZEObject
{
	ZE_META_OBJECT(ZEParticleGenerator)

	friend class ZEParticleSystem;

	private:

		ZEParticleSystem*		Owner;
		ZEUInt					ParticlesPerSecond;

		bool					IsSingleBurst;
		ZEUInt					SingleBurstMaxParticleCount;

	protected:

		ZEUInt					SingleBurstParticleCounter;

		virtual void			Tick(float ElapsedTime, ZEArray<ZEParticle>& OwnerParticlePool) = 0;

								ZEParticleGenerator();

	public:

		void					SetParticlesPerSecond(ZEUInt ParticlesPerSecond);
		ZEUInt					GetParticlesPerSecond() const;

		void					SetSingleBurstMaxParticleCount(ZEUInt Count);
		ZEUInt					GetSingleBurstMaxParticleCount() const;

		void					SetIsSingleBurst(bool IsSingleBurst);
		bool					GetIsSingleBurst() const;

		const ZEParticleSystem*	GetOwner() const;
};

#endif

// Correct ParticlesPerSecond type in XML !!!

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta> 
		<class name="ZEParticleGenerator">		
			<noinstance>true</noinstance>
			<description>Base of particle generators.</description>
			<property name="ParticlesPerSecond" type="integer" autogetset="yes" description="Particle count that will be generated in one second."/>
			<property name="IsSingleBurst" type="boolean" autogetset="yes" description="Is a single use generator or a permenent generator."/>
			<property name="SingleBurstMaxParticleCount" type="integer" autogetset="yes" description="If generator is single burst, this number determines how many particles will it generate before stopping generation."/>
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/
