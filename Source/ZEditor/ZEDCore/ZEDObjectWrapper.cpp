//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDObjectWrapper.cpp
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

#include "ZEDObjectWrapper.h"

#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZEMath.h"

#include "ZERenderer/ZERNSimpleMaterial.h"

ZEDObjectWrapper::ZEDObjectWrapper()
{
	Object = NULL;
	Parent = NULL;
	Selectable = true;
	Selected = false;
	Locked = false;
	Focused = false;
}

ZEDObjectWrapper::~ZEDObjectWrapper()
{

}

void ZEDObjectWrapper::SetObject(ZEObject* Object)
{
	if (this->Object != NULL)
		return;

	this->Object = Object;
}

ZEObject* ZEDObjectWrapper::GetObject()
{
	return Object;
}


void ZEDObjectWrapper::SetParent(ZEDObjectWrapper* Wrapper)
{
	Parent = Wrapper;
}

ZEDObjectWrapper* ZEDObjectWrapper::GetParent()
{
	return Parent;
}

void ZEDObjectWrapper::SetId(ZEInt Id)
{

}

ZEInt ZEDObjectWrapper::GetId()
{
	return 0;
}

void ZEDObjectWrapper::SetName(const ZEString& Name)
{

}

ZEString ZEDObjectWrapper::GetName()
{
	return ZEString::Empty;
}

void ZEDObjectWrapper::SetIcon(const ZEString& Icon)
{

}

const ZEString& ZEDObjectWrapper::GetIcon()
{
	return ZEString::Empty;
}

ZEAABBox ZEDObjectWrapper::GetLocalBoundingBox()
{
	return ZEAABBox::Zero;
}

ZEMatrix4x4 ZEDObjectWrapper::GetWorldTransform()
{
	return ZEMatrix4x4::Identity;
}

void ZEDObjectWrapper::SetPosition(const ZEVector3& NewPosition)
{

}

ZEVector3 ZEDObjectWrapper::GetPosition()
{
	return ZEVector3::Zero;
}

void ZEDObjectWrapper::SetRotation(const ZEQuaternion& NewRotation)
{

}

ZEQuaternion ZEDObjectWrapper::GetRotation()
{
	return ZEQuaternion::Identity;
}

void ZEDObjectWrapper::SetScale(const ZEVector3& NewScale)
{
	
}

ZEVector3 ZEDObjectWrapper::GetScale()
{
	return ZEVector3::One;
}


void ZEDObjectWrapper::SetLocked(bool Value)
{
	Locked = Value;
}

bool ZEDObjectWrapper::GetLocked()
{
	return Locked;
}

void ZEDObjectWrapper::SetSelectable(bool Value)
{
	this->Selectable = Selectable;
}

bool ZEDObjectWrapper::GetSelectable()
{
	return Selectable;
}

void ZEDObjectWrapper::SetSelected(bool Selected)
{
	this->Selected = Selected;

	if (!Selected)
		Focused = false;
}

bool ZEDObjectWrapper::GetSelected()
{
	return Selected;
}

void ZEDObjectWrapper::SetFocused(bool Focused)
{
	this->Focused = Focused;
}

bool ZEDObjectWrapper::GetFocused()
{
	return Focused;
}


void ZEDObjectWrapper::SetVisible(bool Value)
{
	
}

bool ZEDObjectWrapper::GetVisible()
{
	return true;
}

QWidget* ZEDObjectWrapper::GetCustomWidget()
{
	return NULL;
}

QMenu* ZEDObjectWrapper::GetPopupMenu()
{
	return NULL;
}

const ZEArray<ZEDObjectWrapper*>& ZEDObjectWrapper::GetChildWrappers()
{
	return ChildWrappers;
}

void ZEDObjectWrapper::AddChildWrapper(ZEDObjectWrapper* Wrapper)
{
	if (Wrapper == NULL)
		return;

	if (ChildWrappers.Exists(Wrapper))
		return;

	if (!ZEClass::IsDerivedFrom(Object->GetClass(), Wrapper->GetObject()->GetClass()))
		return;

	ChildWrappers.Add(Wrapper);
	Wrapper->SetParent(this);
}

void ZEDObjectWrapper::RemoveChildWrapper(ZEDObjectWrapper* Wrapper)
{
	if (Wrapper == NULL)
		return;

	if (!ChildWrappers.Exists(Wrapper))
		return;

	ChildWrappers.RemoveValue(Wrapper);
	Wrapper->SetParent(NULL);
}

void ZEDObjectWrapper::PreRender(const ZERNPreRenderParameters* Parameters)
{

}

void ZEDObjectWrapper::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{

}

void ZEDObjectWrapper::Tick(float ElapsedTime)
{

}

void ZEDObjectWrapper::RayCast(ZERayCastReport& Report, const ZERayCastParameters& Parameters)
{

}

void ZEDObjectWrapper::Destroy()
{
	delete this;
}
