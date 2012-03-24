//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleDisplacementOperator.h
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
#ifndef __ZE_PARTICLE_DISPLACEMENT_OPERATOR_H__
#define __ZE_PARTICLE_DISPLACEMENT_OPERATOR_H__

#include "ZEParticleOperator.h"

ZE_META_OBJECT_DESCRIPTION(ZEParticleDisplacementOperator)

class ZEParticleDisplacementOperator : public ZEParticleOperator
{
	ZE_META_OBJECT(ZEParticleDisplacementOperator)

	private: 

		ZEVector3			Displacement;

	public:

		virtual void		Tick(float ElapsedTime, ZEArray<ZEParticle>& OwnerParticlePool);
		virtual void		ResizeCustomDataPool(ZESize NewPoolSize);

		void				SetDisplacement(const ZEVector3& Value);
		const ZEVector3&	GetDisplacement() const;

							ZEParticleDisplacementOperator();
							~ZEParticleDisplacementOperator();
};

#endif

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta> 
		<class name="ZEParticleDisplacementOperator" parent="ZEParticleOperator">		
			<noinstance>true</noinstance>
			<description>Displacement operator.</description>
			<property name="Displacement" type="ZEVector3" autogetset="yes" description="Displacement in x,y and z related with time."/>	
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/