//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Component.h
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

#include "ZEDS/Array.h"
#include "ZEMath/Matrix.h"
#include "ZEMath/Vector.h"
#include "ZEMath/Quaternion.h"
#include "ZEMath/AABoundingBox.h"
#include "ZEMath/OBoundingBox.h"
#include "ZEMath/BoundingSphere.h"

class ZERenderer;
class ZEEntity;
class ZERLLight;
class ZEComponent
{
	friend class ZEEntity;
	private:
		ZEEntity*						Owner;
		ZEVector3						Position;
		ZEQuaternion					Rotation;
		ZEVector3						Scale;
		ZEMatrix4x4						LocalTransform;
		ZEMatrix4x4						WorldTransform;

		bool							UpdateLocalTransform;
		bool							UpdateWorldTransform;
		bool							UpdateWorldBoundingSphere;
		bool							UpdateWorldBoundingBox;

		ZEAABoundingBox					LocalBoundingBox;
		ZEAABoundingBox					WorldBoundingBox;
		ZEBoundingSphere				WorldBoundingSphere;

		bool							Enabled;
		bool							Visible;

	protected:
		void							UpdateBoundingVolumes();
		void							SetLocalBoundingBox(const ZEAABoundingBox& BoundingBox);

	public:
		bool							IsEnabled();
		virtual void					SetEnabled(bool Enabled);

		bool							IsVisible();
		virtual void					SetVisible(bool Visible);

		virtual	bool					IsLight();
		virtual	bool					IsDrawable();

		virtual void					SetOwner(ZEEntity* NewOwner);
		virtual ZEEntity*				GetOwner();
		virtual void					OwnerWorldTransformChanged();

		virtual const ZEAABoundingBox&	GetLocalBoundingBox();
		virtual const ZEAABoundingBox&	GetWorldBoundingBox();
		const ZEBoundingSphere&			GetWorldBoundingSphere();

		const ZEVector3&				GetLocalPosition();
		virtual void					SetLocalPosition(const ZEVector3& NewPosition);

		const ZEQuaternion&				GetLocalRotation();
		virtual void					SetLocalRotation(const ZEQuaternion& NewRotation);

		const ZEVector3&				GetLocalScale();
		virtual void					SetLocalScale(const ZEVector3& NewScale);

		const ZEMatrix4x4&				GetWorldTransform();
		const ZEMatrix4x4&				GetLocalTransform();

		virtual const ZEVector3			GetWorldPosition();
		virtual void					SetWorldPosition(const ZEVector3& NewWorldPosition);

		virtual const ZEQuaternion		GetWorldRotation();
		virtual void					SetWorldRotation(const ZEQuaternion& NewWorldRotation);

		virtual bool					CastRay(const ZERay& Ray, ZEVector3& Position, ZEVector3& Normal, float& TEnterance, float &TExit);
		virtual void					Draw(ZERenderer* Renderer, const ZESmartArray<const ZERLLight*>& Lights);
		virtual void					Tick(float TimeElapsed);

		virtual bool					Initialize();
		virtual bool					Deinitialize();
								
										ZEComponent();
};

#endif
