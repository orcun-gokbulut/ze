//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDEntityWrapper.cpp
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

#include "ZEDEntityWrapper.h"

#include "ZEMath/ZEMath.h"
#include "ZEGame/ZEEntity.h"
#include "ZEDCore/ZEDEditor.h"
#include "ZEDCore/ZEDObjectManager.h"

ZEAABBox ZEDEntityWrapper::CalculateBoundingBox(ZEEntity* Entity) const
{
	if (Entity == NULL)
		return ZEAABBox();

	ZEAABBox CurrentBoundingBox = Entity->GetBoundingBox();

	ZEArray<ZEEntity*> Components = Entity->GetComponents();

	for (ZESize I = 0; I < Components.GetCount(); I++)
	{
		ZEAABBox ComponentBoundingBox = CalculateBoundingBox(Components[I]);

		for (ZEInt N = 0; N < 8; N++)
		{
			ZEVector3 Point = ComponentBoundingBox.GetVertex(N);
			if (Point.x < CurrentBoundingBox.Min.x) CurrentBoundingBox.Min.x = Point.x;
			if (Point.y < CurrentBoundingBox.Min.y) CurrentBoundingBox.Min.y = Point.y;
			if (Point.z < CurrentBoundingBox.Min.z) CurrentBoundingBox.Min.z = Point.z;

			if (Point.x > CurrentBoundingBox.Max.x) CurrentBoundingBox.Max.x = Point.x;
			if (Point.y > CurrentBoundingBox.Max.y) CurrentBoundingBox.Max.y = Point.y;
			if (Point.z > CurrentBoundingBox.Max.z) CurrentBoundingBox.Max.z = Point.z;
		}
	}

	ZEArray<ZEEntity*> ChildEntities = Entity->GetChildEntities();

	for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
	{
		const ZEAABBox& ChildEntityBoundingBox = CalculateBoundingBox(ChildEntities[I]);

		for (ZEInt N = 0; N < 8; N++)
		{
			ZEVector3 Point = ChildEntityBoundingBox.GetVertex(N);
			if (Point.x < CurrentBoundingBox.Min.x) CurrentBoundingBox.Min.x = Point.x;
			if (Point.y < CurrentBoundingBox.Min.y) CurrentBoundingBox.Min.y = Point.y;
			if (Point.z < CurrentBoundingBox.Min.z) CurrentBoundingBox.Min.z = Point.z;

			if (Point.x > CurrentBoundingBox.Max.x) CurrentBoundingBox.Max.x = Point.x;
			if (Point.y > CurrentBoundingBox.Max.y) CurrentBoundingBox.Max.y = Point.y;
			if (Point.z > CurrentBoundingBox.Max.z) CurrentBoundingBox.Max.z = Point.z;
		}
	}

	return CurrentBoundingBox;
}

bool ZEDEntityWrapper::RayCastModifier(ZERayCastCollision& Collision, const void* Parameter)
{
	Collision.Object = this;
	return true;
}

void ZEDEntityWrapper::SetId(ZEInt Id)
{
	if (GetEntity()->GetEntityId() == Id)
		return;

	GetEntity()->SetEntityId(Id);

	ZEDObjectWrapper3D::SetId(Id);
}

ZEInt ZEDEntityWrapper::GetId() const
{
	return GetEntity()->GetEntityId();
}

void ZEDEntityWrapper::SetName(const ZEString& Name)
{
	if (GetEntity() == NULL)
		return;

	if (GetEntity()->GetName() == Name)
		return;

	GetEntity()->SetName(Name);

	ZEDObjectWrapper3D::SetName(Name);
}

ZEString ZEDEntityWrapper::GetName() const
{
	return GetEntity()->GetName();
}

void ZEDEntityWrapper::SetObject(ZEObject* Object)
{
	if (this->GetObject() == Object)
		return;

	if (!ZEClass::IsDerivedFrom(ZEEntity::Class(), Object->GetClass()))
		return;

	static_cast<ZEEntity*>(Object)->SetWrapper(this);
	
	ZEDObjectWrapper3D::SetObject(Object);
}

ZEEntity* ZEDEntityWrapper::GetEntity() const
{
	return static_cast<ZEEntity*>(GetObject());
}

void ZEDEntityWrapper::SetVisible(bool Value)
{
	if (GetEntity() == NULL)
		return;

	GetEntity()->SetVisible(Value);
}

bool ZEDEntityWrapper::GetVisible() const
{
	return GetEntity()->GetVisible();
}

bool ZEDEntityWrapper::CheckChildrenClass(ZEClass* Class)
{
	return ZEClass::IsDerivedFrom(ZEEntity::Class(), Class);
}

bool ZEDEntityWrapper::AddChildWrapper(ZEDObjectWrapper* Wrapper, bool Update)
{
	if (Wrapper != NULL && !ZEClass::IsDerivedFrom(ZEDEntityWrapper::Class(), Wrapper->GetClass()))
		return false;

	if (!ZEDObjectWrapper3D::AddChildWrapper(Wrapper, Update))
		return false;

	if (!Update)
	{
		if (GetEntity() != NULL)
			GetEntity()->AddChildEntity(static_cast<ZEEntity*>(Wrapper->GetObject()));
	}

	return true;
}

bool ZEDEntityWrapper::RemoveChildWrapper(ZEDObjectWrapper* Wrapper, bool Update)
{
	if (Wrapper != NULL && !ZEClass::IsDerivedFrom(ZEDEntityWrapper::Class(), Wrapper->GetClass()))
		return false;

	if (!ZEDObjectWrapper3D::RemoveChildWrapper(Wrapper, Update))
		return false;

	if (!Update)
	{
		if (GetEntity() != NULL)
			GetEntity()->RemoveChildEntity(static_cast<ZEEntity*>(Wrapper->GetObject()));
	}

	return true;
}

ZEAABBox ZEDEntityWrapper::GetBoundingBox() const
{
	if (GetEntity() == NULL)
		return ZEAABBox();

	return CalculateBoundingBox(GetEntity());
}

ZEMatrix4x4 ZEDEntityWrapper::GetWorldTransform() const
{
	if (GetEntity() == NULL)
		return ZEMatrix4x4::Identity;

	return GetEntity()->GetWorldTransform();
}

void ZEDEntityWrapper::SetPosition(const ZEVector3& NewPosition)
{
	if (GetEntity() == NULL)
		return;

	GetEntity()->SetWorldPosition(NewPosition);
	
	ZEDObjectWrapper3D::SetPosition(NewPosition);
}

ZEVector3 ZEDEntityWrapper::GetPosition() const
{
	if (GetEntity() == NULL)
		return ZEVector3::Zero;

	return GetEntity()->GetWorldPosition();
}

void ZEDEntityWrapper::SetRotation(const ZEQuaternion& NewRotation)
{
	if (GetEntity() == NULL)
		return;

	GetEntity()->SetWorldRotation(NewRotation);

	ZEDObjectWrapper3D::SetRotation(NewRotation);
}

ZEQuaternion ZEDEntityWrapper::GetRotation() const
{
	if (GetEntity() == NULL)
		return ZEQuaternion::Identity;

	return GetEntity()->GetWorldRotation();
}

void ZEDEntityWrapper::SetScale(const ZEVector3& NewScale)
{
	if (GetObject() == NULL)
		return;

	GetEntity()->SetScale(NewScale);

	ZEDObjectWrapper3D::SetScale(NewScale);
}

ZEVector3 ZEDEntityWrapper::GetScale() const
{
	if (GetEntity() == NULL)
		return ZEVector3::One;

	return GetEntity()->GetWorldScale();
}

void ZEDEntityWrapper::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{
	Report.SetModifierFunction(ZEDelegateMethod(ZERayCastModifierFunction, ZEDEntityWrapper, RayCastModifier, this));
	GetEntity()->RayCast(Report, Parameters);
	Report.SetModifierFunction(ZERayCastModifierFunction());
}

void ZEDEntityWrapper::Tick(float ElapsedTime)
{
	ZEDObjectWrapper3D::Tick(ElapsedTime);

	if (GetEntity() != NULL && GetEntity()->GetState() == ZE_ES_LOADED)
		Update();
}

void ZEDEntityWrapper::Update()
{
	if (GetEntity() == NULL)
	{
		ClearChildWrappers();
		return;
	}
	
	LockWrapper();
	SyncronizeChildWrappers((ZEObject*const*)GetEntity()->GetChildEntities().GetConstCArray(), GetEntity()->GetChildEntities().GetCount());
	UnlockWrapper();

	ZEDObjectWrapper3D::Update();
}

void ZEDEntityWrapper::Clean()
{
	GetEntity()->Unload();
	GetEntity()->ClearChildEntities();
	Update();
}

void ZEDEntityWrapper::LockWrapper()
{
	if (GetEntity() != NULL)
		GetEntity()->LockEntity();
}

void ZEDEntityWrapper::UnlockWrapper()
{
	if (GetEntity() != NULL)
		GetEntity()->UnlockEntity();
}

ZEDEntityWrapper* ZEDEntityWrapper::CreateInstance()
{
	return new ZEDEntityWrapper();
}
