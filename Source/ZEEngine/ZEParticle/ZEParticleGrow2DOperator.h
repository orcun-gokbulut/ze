//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleGrow2DOperator.h
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
#ifndef __ZE_PARTICLE_GROW_OPERATOR_H__
#define __ZE_PARTICLE_GROW_OPERATOR_H__

#include "ZEParticleOperator.h"

ZE_META_OBJECT_DESCRIPTION(ZEParticleGrow2DOperator)

class ZEParticleGrow2DOperator : public ZEParticleOperator
{
	ZE_META_OBJECT(ZEParticleGrow2DOperator)

	private: 

		ZEVector2			GrowFactor;

	public:

		virtual void		Tick(float ElapsedTime, ZEArray<ZEParticle>& OwnerParticlePool);
		virtual void		ResizeCustomDataPool(ZESize NewPoolSize);

		void				SetGrowFactor(const ZEVector2& Factor);
		const ZEVector2&	GetGrowFactor() const;

							ZEParticleGrow2DOperator();
							~ZEParticleGrow2DOperator();
};

#endif

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta> 
		<class name="ZEParticleGrow2DOperator" parent="ZEParticleOperator">		
			<noinstance>true</noinstance>
			<description>2D grow and shrink operator.</description>
			<property name="GrowFactor" type="ZEVector2" autogetset="yes" description="Ammount of grow or shirink in x and y related with time."/>	
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/
