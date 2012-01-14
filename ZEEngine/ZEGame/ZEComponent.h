//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEComponent.h
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
#ifndef __ZE_COMPONENT_H__
#define __ZE_COMPONENT_H__

#include "ZEEntity.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZEOBBox.h"
#include "ZEMath/ZEBSphere.h"

class ZERenderer;
class ZECompoundEntity;
class ZELight;

ZE_META_ENTITY_DESCRIPTION(ZEComponent);

#define ZE_CDF_USER_FLAG_START		16

class ZEComponent : public ZEEntity
{
	ZE_META_ENTITY(ZEComponent)
	friend class ZECompoundEntity;
	private:
		ZECompoundEntity*				Owner;

		ZEMatrix4x4						LocalTransform;
		ZEMatrix4x4						WorldTransform;

		ZEAABBox						LocalBoundingBox;
		ZEAABBox						WorldBoundingBox;

	protected: 
		void							SetLocalBoundingBox(const ZEAABBox& BoundingBox);
		virtual	void					OnTransformChanged();

	public:
		ZEEntityType					GetEntityType();

		virtual ZECompoundEntity*		GetOwner() const;
		virtual	ZEUInt32					GetDrawFlags() const;

		const ZEMatrix4x4&				GetWorldTransform() const;
		const ZEMatrix4x4&				GetLocalTransform() const;

		virtual const ZEAABBox&	GetLocalBoundingBox() const;
		virtual const ZEAABBox&	GetWorldBoundingBox();

		virtual void					SetPosition(const ZEVector3& NewPosition);
		virtual void					SetRotation(const ZEQuaternion& NewRotation);
		virtual void					SetScale(const ZEVector3& NewScale);

		const ZEVector3					GetWorldPosition() const;
		const ZEQuaternion				GetWorldRotation() const;

		ZEVector3						GetWorldDirection() const;
		ZEVector3						GetWorldRight() const;
		ZEVector3						GetWorldUp() const;

		const ZEVector3&				GetWorldVelocity();

		virtual bool					CastRay(const ZERay& Ray, ZEVector3& Position, ZEVector3& Normal, float& TEnterance, float &TExit);

										ZEComponent();
};
/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta> 
		<class name="ZEComponent" parent="ZEEntity">
			<noinstance>true</noinstance>
			<description>Base Component Type</description>
			<proptery name="WorldVelocity" type="ZEVector3" getfunction="GetWorldVelocty"/>
			<proptery name="LocalTransform" type="ZEMatrix4x4" getfunction="GetLocalTransform"/>
			<proptery name="WorldPosition" type="ZEVector3" getfunction="GetWorldPosition"/>
			<proptery name="WorldRotation" type="ZEQuaternion" getfunction="GetWorldRotation"/>
			<proptery name="WorldDirection" type="ZEVector3" getfunction="GetWorldDirection"/>
			<proptery name="WorldRight" type="ZEVector3" getfunction="GetWorldRight"/>
			<proptery name="WorldUp"	type="ZEVector3" getfunction="GetWorldUp"/>

		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/
#endif





