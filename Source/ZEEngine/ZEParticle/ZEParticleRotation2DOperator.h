//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleRotation2DOperator.h
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
#ifndef __ZE_PARTICLE_ROTATION_2D_OPERATOR_H__
#define __ZE_PARTICLE_ROTATION_2D_OPERATOR_H__

#include "ZEParticleOperator.h"

struct ZEParticleRotation2DProperties
{
	float Rotation;
	float AngularVelocity;
	float AngularAcceleration;
};

ZE_META_OBJECT_DESCRIPTION(ZEParticleRotation2DOperator);

class ZEParticleRotation2DOperator : public ZEParticleOperator
{
	ZE_META_OBJECT(ZEParticleRotation2DOperator)

	private:

		float							MaxRotation;
		float							MinRotation;

		float							MinAngularAcceleration;
		float							MaxAngularAcceleration;
		float							MinAngularVelocity;
		float							MaxAngularVelocity;

	public:

		ZEArray<ZEParticleRotation2DProperties> CustomData;

		void							SetMaxRotation(float MaxRotation);
		float							GetMaxRotation() const;
		void							SetMinRotation(float MinRotation);
		float							GetMinRotation() const;

		void 							SetMinAngularAcceleration(float AngularAcceleration);
		float			 				GetMinAngularAcceleration() const;
		void 							SetMaxAngularAcceleration(float AngularAcceleration);
		float 							GetMaxAngularAcceleration() const;

		void 							SetMinAngularVelocity(float AngularVelocity);
		float							GetMinAngularVelocity() const;
		void 							SetMaxAngularVelocity(float AngularVelocity);
		float							GetMaxAngularVelocity() const;

		virtual	void					Tick(float ElapsedTime, ZEArray<ZEParticle>& OwnerParticlePool);
		virtual void					ResizeCustomDataPool(ZESize NewPoolSize);

										ZEParticleRotation2DOperator();
										~ZEParticleRotation2DOperator();
};

#endif

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta> 
		<class name="ZEParticleRotation2DOperator" parent="ZEParticleOperator">		
			<noinstance>true</noinstance>
			<description>2D rotation calculation modifier.</description>
			<property name="MinRotation" type="float" autogetset="yes" description="Minimum rotation that a particle will have just after creation in radians."/>
			<property name="MaxRotation" type="float" autogetset="yes" description="Maximum rotation that a particle will have just after creation in radians."/>
			<property name="MinAngularAcceleration" type="float" autogetset="yes" description="Minimum angular acceleration that a particle will have just after creation in radians."/>
			<property name="MaxAngularAcceleration" type="float" autogetset="yes" description="Maximum angular acceleration that a particle will have just after creation in radians."/>	
			<property name="MinAngularVelocity" type="float" autogetset="yes" description="Minimum angular velocity that a particle will have just after creation in radians."/>
			<property name="MaxAngularVelocity" type="float" autogetset="yes" description="Maximum angular velocity that a particle will have just after creation in radians."/>	
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/
