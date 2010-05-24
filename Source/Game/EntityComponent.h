//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - EntityComponent.h
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

#include "Entity.h"
#include "ZEDS/Array.h"
#include "ZEMath/Matrix.h"
#include "ZEMath/Vector.h"
#include "ZEMath/Quaternion.h"
#include "ZEMath/AABoundingBox.h"
#include "ZEMath/OBoundingBox.h"
#include "ZEMath/BoundingSphere.h"

class ZERenderer;
class ZECompoundEntity;
class ZELight;

ZE_META_ENTITY_DESCRIPTION(ZEEntityComponent);

#define ZE_CDF_USER_FLAG_START		16

class ZEEntityComponent : public ZEEntity
{
	ZE_META_CLASS()
	friend class ZECompoundEntity;
	private:
		ZECompoundEntity*				Owner;

		ZEDWORD							DirtyFlags;

		ZEMatrix4x4						LocalTransform;
		ZEMatrix4x4						WorldTransform;

		ZEAABoundingBox					LocalBoundingBox;
		ZEAABoundingBox					WorldBoundingBox;
		ZEBoundingSphere				WorldBoundingSphere;

		bool							Enabled;
		bool							Visible;

	protected: 
		void							UpdateBoundingVolumes();
		void							SetLocalBoundingBox(const ZEAABoundingBox& BoundingBox);

	public:

		ZEEntityType					GetEntityType();
		
		virtual ZECompoundEntity*		GetOwner() const;

		virtual	ZEDWORD					GetDrawFlags() const;

		const ZEVector3&				GetWorldVelocity();

		const ZEMatrix4x4&				GetWorldTransform() const;
		const ZEMatrix4x4&				GetLocalTransform() const;

		virtual const ZEAABoundingBox&	GetLocalBoundingBox() const;
		virtual const ZEAABoundingBox&	GetWorldBoundingBox() const;
		const ZEBoundingSphere&			GetWorldBoundingSphere() const;

		virtual void					SetPosition(const ZEVector3& NewPosition);
		virtual void					SetRotation(const ZEQuaternion& NewRotation);
		virtual void					SetScale(const ZEVector3& NewScale);

		const ZEVector3					GetWorldPosition() const;
		const ZEQuaternion				GetWorldRotation() const;

		ZEVector3						GetWorldDirectionVector() const;
		ZEVector3						GetWorldRightVector() const;
		ZEVector3						GetWorldUpVector() const;



		virtual bool					CastRay(const ZERay& Ray, ZEVector3& Position, ZEVector3& Normal, float& TEnterance, float &TExit);
		virtual void					Draw(ZEDrawParameters* DrawParameters);
		virtual void					Tick(float TimeElapsed);

		virtual bool					Initialize();
		virtual void					Deinitialize();
	
		virtual void					Destroy();

		virtual void					OwnerWorldTransformChanged();

										ZEEntityComponent();
};
/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta> 
		<class name="ZEEntityComponent">
			<description>Base Component Type</description>
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/
#endif
