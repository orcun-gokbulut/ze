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
#include "ZEDModule.h"


ZEDObjectEvent::ZEDObjectEvent()
{
	Type = ZED_OCED_NONE;
	Wrapper = NULL;
}

void ZEDObjectEvent::SetType(ZEDObjectChangedEventType Type)
{
	this->Type = Type;
}

ZEDObjectChangedEventType ZEDObjectEvent::GetType() const
{
	return Type;
}

void ZEDObjectEvent::SetWrapper(ZEDObjectWrapper* Wrapper)
{
	this->Wrapper = Wrapper;
}

ZEDObjectWrapper* ZEDObjectEvent::GetWrapper() const
{
	return Wrapper;
}

void ZEDObjectWrapper::SetModule(ZEDModule* Module)
{
	if (this->Module == Module)
		return;

	for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
		ChildWrappers[I]->SetModule(Module);

	this->Module = Module;
}

void ZEDObjectWrapper::ClearChildWrappers()
{
	for (ZESSize I = ChildWrappers.GetCount() -1; I >= 0; I--)
		ChildWrappers[I]->Destroy();
}

ZEDObjectWrapper::ZEDObjectWrapper()
{
	Object = NULL;
	Parent = NULL;
	Module = NULL;
	Selectable = true;
	Selected = false;
	Locked = false;
	Focused = false;
}

ZEDObjectWrapper::~ZEDObjectWrapper()
{
	ClearChildWrappers();

	if (Parent != NULL)
		Parent->RemoveChildWrapper(this, true);
}

void ZEDObjectWrapper::SetObject(ZEObject* Object)
{
	if (this->Object == Object)
		return;

	this->Object = Object;

	if (Object == NULL)
		ClearChildWrappers();

	Update();
}

ZEObject* ZEDObjectWrapper::GetObject() const
{
	return Object;
}


void ZEDObjectWrapper::SetParent(ZEDObjectWrapper* Wrapper)
{
	Parent = Wrapper;
}

ZEDObjectWrapper* ZEDObjectWrapper::GetParent() const
{
	return Parent;
}

ZEDModule* ZEDObjectWrapper::GetModule() const
{
	return Module;
}

void ZEDObjectWrapper::SetId(ZEInt Id)
{

}

ZEInt ZEDObjectWrapper::GetId() const
{
	return 0;
}

void ZEDObjectWrapper::SetName(const ZEString& Name)
{

}

ZEString ZEDObjectWrapper::GetName() const
{
	return ZEString::Empty;
}

void ZEDObjectWrapper::SetIcon(const ZEString& Icon)
{

}

const ZEString& ZEDObjectWrapper::GetIcon() const
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

ZEVector3 ZEDObjectWrapper::GetPosition() const
{
	return ZEVector3::Zero;
}

void ZEDObjectWrapper::SetRotation(const ZEQuaternion& NewRotation)
{

}

ZEQuaternion ZEDObjectWrapper::GetRotation() const
{
	return ZEQuaternion::Identity;
}

void ZEDObjectWrapper::SetScale(const ZEVector3& NewScale)
{
	
}

ZEVector3 ZEDObjectWrapper::GetScale() const
{
	return ZEVector3::One;
}


void ZEDObjectWrapper::SetLocked(bool Value)
{
	Locked = Value;
}

bool ZEDObjectWrapper::GetLocked() const
{
	return Locked;
}

void ZEDObjectWrapper::SetSelectable(bool Value)
{
	this->Selectable = Selectable;
}

bool ZEDObjectWrapper::GetSelectable() const
{
	return Selectable;
}

void ZEDObjectWrapper::SetSelected(bool Selected)
{
	this->Selected = Selected;

	if (!Selected)
		Focused = false;
}

bool ZEDObjectWrapper::GetSelected() const
{
	return Selected;
}

void ZEDObjectWrapper::SetFocused(bool Focused)
{
	this->Focused = Focused;
}

bool ZEDObjectWrapper::GetFocused() const
{
	return Focused;
}


void ZEDObjectWrapper::SetVisible(bool Value)
{
	
}

bool ZEDObjectWrapper::GetVisible() const
{
	return true;
}

QWidget* ZEDObjectWrapper::GetCustomWidget() const
{
	return NULL;
}

QMenu* ZEDObjectWrapper::GetPopupMenu() const
{
	return NULL;
}

const ZEArray<ZEDObjectWrapper*>& ZEDObjectWrapper::GetChildWrappers()
{
	return ChildWrappers;
}

bool ZEDObjectWrapper::AddChildWrapper(ZEDObjectWrapper* Wrapper, bool Update)
{
	if (Wrapper == NULL)
		return false;

	if (ChildWrappers.Exists(Wrapper))
		return false;

	ChildWrappers.Add(Wrapper);
	Wrapper->SetParent(this);
	Wrapper->SetModule(Module);

	ZEDObjectEvent Event;
	Event.Wrapper = Wrapper;
	Event.Type = ZED_OCET_ADDED;
	if (Module != NULL)
		Module->DistributeEvent(&Event);

	return true;
}

bool ZEDObjectWrapper::RemoveChildWrapper(ZEDObjectWrapper* Wrapper, bool Update)
{
	if (Wrapper == NULL)
		return false;

	if (!ChildWrappers.Exists(Wrapper))
		return false;

	ChildWrappers.RemoveValue(Wrapper);
	Wrapper->SetParent(NULL);
	Wrapper->SetModule(NULL);

	ZEDObjectEvent Event;
	Event.Wrapper = Wrapper;
	Event.Type = ZED_OCET_REMOVED;
	if (Module != NULL)
		Module->DistributeEvent(&Event);

	return true;
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

void ZEDObjectWrapper::Update()
{

}

void ZEDObjectWrapper::Destroy()
{
	delete this;
}
