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

#include "ZEDEditor.h"
#include "ZEDObjectManager.h"
#include "ZEFile/ZEPathInfo.h"

void ZEDObjectWrapper::SetManager(ZEDObjectManager* Manager)
{
	if (this->Manager == Manager)
		return;

	for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
		ChildWrappers[I]->SetManager(Manager);

	this->Manager = Manager;
}

bool ZEDObjectWrapper::InitializeInternal()
{
	if (!ZEInitializable::InitializeInternal())
		return false;

	for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
	{
		if (!ChildWrappers[I]->Initialize())
			return false;
	}

	Update();

	return true;
}

bool ZEDObjectWrapper::DeinitializeInternal()
{
	for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
		ChildWrappers[I]->Deinitialize();

	return ZEInitializable::DeinitializeInternal();
}

void ZEDObjectWrapper::SyncronizeChildWrappers(ZEObject*const*  TargetList, ZESize TargetListSize)
{
	// Remove
	const ZEArray<ZEDObjectWrapper*>& Wrappers = GetChildWrappers();

	for (ZESSize I = Wrappers.GetCount() - 1; I >= 0; I--)
	{
		bool Found = false;
		for (ZESize N = 0; N < TargetListSize; N++)
		{
			if (Wrappers[I]->GetObject() == TargetList[N])
			{
				Found = true;
				break;
			}
		}

		if (!Found)
			RemoveChildWrapper(Wrappers[I]);
		else
			Wrappers[I]->Update();
	}

	// Add
	for (ZESize I = 0; I < TargetListSize; I++)
	{
		ZEDObjectWrapper* Wrapper = NULL;
		const ZEArray<ZEDObjectWrapper*>& Wrappers = GetChildWrappers();
		for (ZESize N = 0; N < Wrappers.GetCount(); N++)
		{
			if (Wrappers[N]->GetObject() == TargetList[I])
			{
				Wrapper = Wrappers[N];
				break;
			}
		}

		if (Wrapper == NULL)
		{
			Wrapper = GetManager()->WrapObject(TargetList[I]);
			if (Wrapper != NULL)
				AddChildWrapper(Wrapper, true);
		}
	}
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
	Manager = NULL;
	Selectable = true;
	Selected = false;
	Frozen = false;
	Focused = false;
}

ZEDObjectWrapper::~ZEDObjectWrapper()
{
	Deinitialize();

	ClearChildWrappers();

	if (GetParent() != NULL)
		GetParent()->RemoveChildWrapper(this, true);
}

void ZEDObjectWrapper::SetObject(ZEObject* Object)
{
	if (this->Object == Object)
		return;

	Selectable =  Object->GetClass()->CheckAttributeHasValue("ZEDEditor.ObjectWrapper.Selectable", "true");
	IconFileName =  Object->GetClass()->GetAttributeValue("ZEDEditor.ObjectWrapper.Icon");

	ClearChildWrappers();
	this->Object = Object;

	if (IsInitialized())
		Update();
}

ZEObject* ZEDObjectWrapper::GetObject() const
{
	return Object;
}

ZEClass* ZEDObjectWrapper::GetObjectClass()
{
	if (Object == NULL)
		return NULL;

	return Object->GetClass();
}

void ZEDObjectWrapper::SetParent(ZEDObjectWrapper* Parent)
{
	if (this->Parent == Parent)
		return;

	this->Parent = Parent;

	OnParentChanged(this, Parent);
}

ZEDObjectWrapper* ZEDObjectWrapper::GetParent() const
{
	return Parent;
}

ZEDObjectManager* ZEDObjectWrapper::GetManager() const
{
	return Manager;
}

void ZEDObjectWrapper::SetId(ZEInt Id)
{
	UpdateLocal();
}

ZEInt ZEDObjectWrapper::GetId() const
{
	return 0;
}

void ZEDObjectWrapper::SetName(const ZEString& Name)
{
	UpdateLocal();
}

ZEString ZEDObjectWrapper::GetName() const
{
	return ZEString::Empty;
}

void ZEDObjectWrapper::SetFrozen(bool Value)
{
	if (Frozen == Value)
		return;

	Frozen = Value;
	UpdateLocal();

	if (Frozen)
		OnFrozen(this);
	else
		OnUnfrozen(this);
}

bool ZEDObjectWrapper::GetFrozen() const
{
	return Frozen;
}

void ZEDObjectWrapper::SetSelectable(bool Value)
{
	if (this->Selectable != Value)
		return;

	Selectable = Value;

	UpdateLocal();
}

bool ZEDObjectWrapper::GetSelectable() const
{
	return Selectable;
}

void ZEDObjectWrapper::SetSelected(bool Selected)
{
	if (this->Selected == Selected)
		return;

	if (!Selected)
		SetFocused(false);

	this->Selected = Selected;
	UpdateLocal();

	if (Selected)
		OnSelected(this);
	else
		OnDeselected(this);
}

bool ZEDObjectWrapper::GetSelected() const
{
	return Selected;
}

void ZEDObjectWrapper::SetFocused(bool Focused)
{
	if (this->Focused == Focused)
		return;

	this->Focused = Focused;
	UpdateLocal();

	if (Focused)
		OnFocused(this);
	else
		OnUnfocused(this);
}

bool ZEDObjectWrapper::GetFocused() const
{
	return Focused;
}

QWidget* ZEDObjectWrapper::GetCustomWidget() const
{
	return NULL;
}

QMenu* ZEDObjectWrapper::GetPopupMenu() const
{
	return NULL;
}

void ZEDObjectWrapper::SetIconFileName(const ZEString& FileName)
{
	if (ZEPathInfo::Compare(IconFileName, FileName))
		return;

	IconFileName = FileName;

	UpdateLocal();
}

const ZEString& ZEDObjectWrapper::GetIconFileName() const
{
	return IconFileName;
}

const ZEArray<ZEDObjectWrapper*>& ZEDObjectWrapper::GetChildWrappers()
{
	return ChildWrappers;
}

bool ZEDObjectWrapper::AddChildWrapper(ZEDObjectWrapper* ChildObject, bool Update)
{
	if (ChildObject == NULL)
		return false;

	if (ChildWrappers.Exists(ChildObject))
		return false;

	ChildObject->SetParent(this);
	ChildObject->SetManager(Manager);

	OnChildObjectAdding(this, ChildObject);
	if (GetManager() != NULL)
		GetManager()->OnObjectChildObjectAdding(GetManager(), this, ChildObject);

	ChildWrappers.Add(ChildObject);

	OnChildObjectAdded(this, ChildObject);
	if (GetManager() != NULL)
		GetManager()->OnObjectChildObjectAdded(GetManager(), this, ChildObject);

	if (IsInitializedOrInitializing())
	{
		if (!ChildObject->Initialize())
			return false;

		ChildObject->Update();
	}

	return true;
}

bool ZEDObjectWrapper::RemoveChildWrapper(ZEDObjectWrapper* ChildObject, bool Update)
{
	if (ChildObject == NULL)
		return false;

	if (!ChildWrappers.Exists(ChildObject))
		return false;

	OnChildObjectRemoving(this, ChildObject);
	if (GetManager() != NULL)
		GetManager()->OnObjectChildObjectRemoving(GetManager(), this, ChildObject);

	ChildObject->Deinitialize();

	ChildWrappers.RemoveValue(ChildObject);
	ChildObject->SetParent(NULL);
	ChildObject->SetManager(NULL);

	OnChildObjectRemoved(this, ChildObject);
	if (GetManager() != NULL)
		GetManager()->OnObjectChildObjectRemoved(GetManager(), this, ChildObject);

	return true;
}

bool ZEDObjectWrapper::CheckChildrenClass(ZEClass* Class)
{
	return true;
}

void ZEDObjectWrapper::Tick(float ElapsedTime)
{
	for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
		ChildWrappers[I]->Tick(ElapsedTime);
}

void ZEDObjectWrapper::SendChangedEvent()
{
	Update();

	OnPropertyChanged(this, NULL);
	if (GetManager() != NULL)
		GetManager()->OnObjectPropertyChanged(GetManager(), this, NULL);
}

void ZEDObjectWrapper::LockWrapper()
{

}

void ZEDObjectWrapper::UnlockWrapper()
{

}

bool ZEDObjectWrapper::Load(const ZEString& FileName)
{
	return false;
}

bool ZEDObjectWrapper::Save(const ZEString& FileName)
{
	return false;
}

void ZEDObjectWrapper::Clean()
{

}

void ZEDObjectWrapper::UpdateLocal()
{

}

void ZEDObjectWrapper::Update()
{
	UpdateLocal();

	for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
		ChildWrappers[I]->Update();
}

ZEDObjectWrapper* ZEDObjectWrapper::Clone()
{
	ZEDObjectWrapper* CloneWrapper = static_cast<ZEDObjectWrapper*>(GetClass()->CreateInstance());
	if (CloneWrapper == NULL)
		return false;

	ZEObject* CloneObject = Object->GetClass()->CreateInstance();
	if (CloneObject == NULL)
		return NULL;

	const ZEMTProperty* Properties = CloneObject->GetClass()->GetProperties();
	ZESize PropertyCount = CloneObject->GetClass()->GetPropertyCount();
	for (ZESize I = 0; I < PropertyCount; I++)
	{
		if (Properties[I].Access != ZE_PA_READ_WRITE)
			continue;

		ZEVariant Value;
		if (!Object->GetClass()->GetProperty(Object, Properties[I].ID, Value))
			continue;
		
		CloneObject->GetClass()->SetProperty(CloneObject, Properties[I].ID, Value);
	}

	CloneWrapper->SetObject(CloneObject);

	return CloneWrapper;
}
