//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECompoundEntity.h
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
#ifndef	__ZE_COMPOUND_ENTITY_H__
#define __ZE_COMPOUND_ENTITY_H__

#include "ZEEntity.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEVariant.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZEOBBox.h"
#include "ZEMath/ZEBSphere.h"
#include "ZEDefinitions.h"
#include "ZEMeta/ZEObject.h"
#include "ZEComponent.h"

struct ZEDrawParameters;

ZE_META_ENTITY_DESCRIPTION(ZECompoundEntity);

class ZECompoundEntity : public ZEEntity
{
	ZE_META_ENTITY(ZECompoundEntity)
	private:
		ZERayCastFlags							RayCastFlags;

	protected:
		ZEArray<ZEComponent*>					Components;

		void									UpdateComponentTransforms();

		void									RegisterComponent(ZEComponent* Component);
		void									UnregisterComponent(ZEComponent* Component);	
		
												ZECompoundEntity();
		virtual									~ZECompoundEntity();

	public:
		virtual ZEEntityType					GetEntityType();

		const ZEArray<ZEComponent*>&			GetComponents() const;

		virtual ZEUInt32						GetRayCastFlags() const;

		virtual void							SetPosition(const ZEVector3& NewPosition);
		virtual void							SetRotation(const ZEQuaternion& NewRotation);
		virtual void							SetScale(const ZEVector3& NewScale);
		virtual void							SetVelocity(const ZEVector3& NewVelocity);

		virtual bool							Initialize();
		virtual void							Deinitialize();
		
		virtual void							Tick(float Time);
};

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta> 
		<class name="ZECompoundEntity" parent="ZEEntity">
			<noinstance>true</noinstance>
			<container name="Components" baseclass="ZEComponent" allowderived="true"
				addfunction="RegisterComponent"
				removefunction="UnregisterComponent"
				getfunction="GetComponents().GetConstCArray"
				getcountfunction="GetComponents().GetCount"
				description="This container contains components that entity includes"/>
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/
#endif




