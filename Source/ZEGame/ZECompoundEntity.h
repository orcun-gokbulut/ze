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
#include "ZEDS\ZEArray.h"
#include "ZEDS\ZEVariant.h"
#include "ZEMath\ZEMatrix.h"
#include "ZEMath\ZEVector.h"
#include "ZEMath\ZEQuaternion.h"
#include "ZEMath\ZEAABoundingBox.h"
#include "ZEMath\ZEOBoundingBox.h"
#include "ZEMath\ZEBoundingSphere.h"
#include "ZEDefinitions.h"
#include "ZEMeta\ZEClass.h"

class ZEComponent;
struct ZEDrawParameters;

enum ZEBoundingVolumeMechnism
{
	ZE_BVM_NO_BOUNDING_VOLUME	= 0,
	ZE_BVM_USE_LOCAL_ONLY		= 1,
	ZE_BVM_USE_COMPONENTS		= 2,
	ZE_BVM_USE_BOTH				= 3
};

ZE_META_ENTITY_DESCRIPTION(ZECompoundEntity);

class ZECompoundEntity : public ZEEntity
{
	ZE_META_ENTITY()
	private:
		ZEDrawFlags								DrawFlags;
		ZERayCastFlags							RayCastFlags;
		ZEBoundingVolumeMechnism				BoundingVolumeMechanism;

	protected:
		ZEArray<ZEComponent*>					Components;

		void									RegisterComponent(ZEComponent* Component);
		void									UnregisterComponent(ZEComponent* Component);	

		void									SetBoundingVolumeMechanism(ZEBoundingVolumeMechnism Mechanism);

		void									UpdateComponents();

	public:
		virtual ZEEntityType					GetEntityType();

		const ZEArray<ZEComponent *>&			GetComponents() const;

		virtual const ZEAABoundingBox&			GetWorldBoundingBox();

		virtual ZEDWORD							GetDrawFlags() const;
		virtual ZEDWORD							GetRayCastFlags() const;

		virtual void							SetPosition(const ZEVector3& NewPosition);
		virtual void							SetRotation(const ZEQuaternion& NewRotation);
		virtual void							SetScale(const ZEVector3& NewScale);
		virtual void							SetVelocity(const ZEVector3& NewVelocity);

		virtual bool							Initialize();
		virtual void							Deinitialize();
		
		virtual void							Tick(float Time);
		virtual void							Draw(ZEDrawParameters* DrawParameters);
		
		void									UpdateBoundingVolumes();

												ZECompoundEntity();
		virtual									~ZECompoundEntity();
};

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta> 
		<class name="ZECompoundEntity" parent="ZEEntity">
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





