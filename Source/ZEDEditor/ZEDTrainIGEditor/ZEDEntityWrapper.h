//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDEntityWrapper.h
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

#include "ZEDCore/ZEDObjectWrapper3D.h"

class ZERayCastCollision;

class ZEDEntityWrapper : public ZEDObjectWrapper3D
{
	ZE_OBJECT
	private:
		bool								AlterRaycast(ZERayCastCollision& Collision);
		ZEAABBox							CalculateBoundingBox(ZEEntity* Entity, bool& BoundingBoxAvailable) const;

	protected:
		bool								RayCastModifier(ZERayCastCollision& Collision, const void* Parameter);

	public:
		virtual void						SetObject(ZEObject* Object);
		ZEEntity*							GetEntity() const;

		virtual void						SetId(ZEInt Id);
		virtual ZEInt						GetId() const;

		virtual void						SetName(const ZEString& Name);
		virtual ZEString					GetName() const;

		virtual ZEAABBox					GetBoundingBox() const;
		virtual ZEMatrix4x4					GetWorldTransform() const;

		virtual void						SetPosition(const ZEVector3& NewPosition);
		virtual ZEVector3					GetPosition() const;
		virtual void						SetRotation(const ZEQuaternion& NewRotation);
		virtual ZEQuaternion				GetRotation() const;
		virtual void						SetScale(const ZEVector3& NewScale);
		virtual ZEVector3					GetScale() const;

		virtual void						SetVisible(bool Value);
		virtual bool						GetVisible() const;

		virtual bool						CheckChildrenClass(ZEClass* Class);

		virtual bool						AddChildWrapper(ZEDObjectWrapper* Wrapper, bool Update = false);
		virtual bool						RemoveChildWrapper(ZEDObjectWrapper* Wrapper, bool Update = false);

		virtual void						Tick(float ElapsedTime);
		virtual void						RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters);

		virtual void						Update();

		virtual void						Clean();

		virtual void						LockWrapper();
		virtual void						UnlockWrapper();

		static ZEDEntityWrapper*			CreateInstance();
}
ZEMT_ATTRIBUTE(ZEDObjectWrapper.TargetClass, ZEEntity);
