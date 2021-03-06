//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSelectionManager.cpp
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

#include "ZEDSelectionManager.h"

#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEViewVolume.h"
#include "ZEMeta/ZEEventSuppressor.h"
#include "ZEDEditor.h"
#include "ZEDObjectWrapper.h"
#include "ZEDObjectManager.h"
#include "ZEDTransformationManager.h"
#include "ZEDUserInterface/ZEDCommandManager.h"


bool ZEDSelectionManager::FilterSelection(ZEObject* Object, void* Class)
{
	return ZEClass::IsDerivedFrom((ZEClass*)Class, Object->GetClass());
}

void ZEDSelectionManager::UnfrezeeAllInternal(ZEDObjectWrapper* Object)
{
	Object->SetFrozen(false);

	const ZEArray<ZEDObjectWrapper*>& ChildObjects =  Object->GetChildWrappers();
	for (ZESize I = 0; I < ChildObjects.GetCount(); I++)
		UnfrezeeAllInternal(ChildObjects[I]);
}

bool ZEDSelectionManager::InitializeInternal()
{
	if (!ZEInitializable::InitializeInternal())
		return false;

	GetEditor()->OnClosed.AddDelegate<ZEDSelectionManager, &ZEDSelectionManager::Editor_OnClosed>(this);
	
	RegisterCommands();

	return true;
}

bool ZEDSelectionManager::DeinitializeInternal()
{
	GetEditor()->OnClosed.DisconnectObject(this);
	return ZEInitializable::DeinitializeInternal();
}

void ZEDSelectionManager::Editor_OnClosed(ZEDEditor* Editor)
{
	ClearSelection();
}

ZEDSelectionManager::ZEDSelectionManager()
{
	LockSelection = false;
	FocusedObject = NULL;
	Filter = NULL;
}

ZEDSelectionManager::~ZEDSelectionManager()
{
	Deinitialize();
}

void ZEDSelectionManager::RegisterCommands()
{
	SelectAllCommand.SetCategory("Selection");
	SelectAllCommand.SetName("ZEDSelectionManager::SelectAllCommand");
	SelectAllCommand.SetText("Select All");
	SelectAllCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_A));
	SelectAllCommand.OnAction.AddDelegate<ZEDSelectionManager, &ZEDSelectionManager::SelectAllCommand_OnAction>(this);
	SelectAllCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/SelectAll.png");
	ZEDCommandManager::GetInstance()->RegisterCommand(&SelectAllCommand);
	
	ClearSelectionCommand.SetCategory("Selection");
	ClearSelectionCommand.SetName("ZEDSelectionManager::ClearSelectionCommand");
	ClearSelectionCommand.SetText("Clear Selection");
	ClearSelectionCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_D));
	ClearSelectionCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/ClearSelection.png");
	ClearSelectionCommand.OnAction.AddDelegate<ZEDSelectionManager, &ZEDSelectionManager::ClearSelectionCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&ClearSelectionCommand);

	LockSelectionCommand.SetCategory("Selection");
	LockSelectionCommand.SetName("ZEDSelectionManager::LockSelectionCommand");
	LockSelectionCommand.SetText("Lock Selection");
	LockSelectionCommand.SetType(ZED_CT_TOGGLE);
	LockSelectionCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_L));
	LockSelectionCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/Lock.png");
	LockSelectionCommand.OnAction.AddDelegate<ZEDSelectionManager, &ZEDSelectionManager::LockSelectionCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&LockSelectionCommand);

	FreezeObjectsCommand.SetCategory("Selection");
	FreezeObjectsCommand.SetName("ZEDSelectionManager::FreezeObjectsCommand");
	FreezeObjectsCommand.SetText("Freeze Objects");
	FreezeObjectsCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/Freeze.png");
	FreezeObjectsCommand.OnAction.AddDelegate<ZEDSelectionManager, &ZEDSelectionManager::FreezeObjectsCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&FreezeObjectsCommand);

	UnfreezeObjectsCommand.SetCategory("Selection");
	UnfreezeObjectsCommand.SetName("ZEDSelectionManager::UnfreezeObjectsCommand");
	UnfreezeObjectsCommand.SetText("Unfreeze Objects");
	UnfreezeObjectsCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/Unfreeze.png");
	UnfreezeObjectsCommand.OnAction.AddDelegate<ZEDSelectionManager, &ZEDSelectionManager::UnfreezeObjectsCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&UnfreezeObjectsCommand);
}

void ZEDSelectionManager::UpdateCommands()
{
	ClearSelectionCommand.SetEnabled(GetSelection().GetCount() != 0);
	LockSelectionCommand.SetValueChecked(GetLockSelection());
	FreezeObjectsCommand.SetEnabled(GetSelection().GetCount() != 0);
	UnfreezeObjectsCommand.SetEnabled(GetFrozenObjects().GetCount() != 0);
}

void ZEDSelectionManager::SelectAllCommand_OnAction(const ZEDCommand* Command)
{
	ZEDEditor* Editor = GetEditor();

	if (Editor == NULL)
		return;

	ZEDObjectManager* ObjectManager = Editor->GetObjectManager();

	if (ObjectManager == NULL)
		return;

	ZEDObjectWrapper* RootWrapper = ObjectManager->GetRootWrapper();

	if (RootWrapper == NULL)
		return;

	ZEArray<ZEDObjectWrapper*> Stack;
	ZEDObjectWrapper* CurrentWrapper = NULL;
	ZEDObjectWrapper* ParentWrapper = NULL;
	ZESmartArray<ZEDObjectWrapper*> Selection;

	CurrentWrapper = RootWrapper;
	/*Stack.Push(CurrentWrapper);*/

	while (!(CurrentWrapper == NULL && Stack.GetCount() == 0))
	{
		if (CurrentWrapper == NULL)
		{
			CurrentWrapper = Stack.Pop();
			Selection.Add(CurrentWrapper);

			ParentWrapper = CurrentWrapper->GetParent();

			if (ParentWrapper == NULL)
			{
				CurrentWrapper = NULL;
				continue;
			}

			ZESSize Count = ParentWrapper->GetChildWrappers().GetCount();
			ZESSize Index = ParentWrapper->GetChildWrappers().FindIndex(CurrentWrapper);

			if (Index < Count - 1)
				CurrentWrapper = ParentWrapper->GetChildWrappers().GetItem(++Index);
			else
				CurrentWrapper = NULL;

		}
		else
		{
			Stack.Push(CurrentWrapper);

			if (CurrentWrapper->GetChildWrappers().GetCount() != 0)
				CurrentWrapper = CurrentWrapper->GetChildWrappers().GetFirstItem();
			else
				CurrentWrapper = NULL;
		}

	}

	SetSelection(Selection);

}

void ZEDSelectionManager::ClearSelectionCommand_OnAction(const ZEDCommand* Command)
{
	ClearSelection();
}

void ZEDSelectionManager::LockSelectionCommand_OnAction(const ZEDCommand* Command)
{
	SetLockSelection(Command->GetValueChecked());
}

void ZEDSelectionManager::FreezeObjectsCommand_OnAction(const ZEDCommand* Command)
{
	FreezeObjects(GetSelection());
}

void ZEDSelectionManager::UnfreezeObjectsCommand_OnAction(const ZEDCommand* Command)
{
	UnfreezeObjects(GetFrozenObjects());
}

void ZEDSelectionManager::SetSelectionFilter(ZEClass* Class)
{
	Filter = Class;
}

ZEClass* ZEDSelectionManager::GetSelectionFilter()
{
	return Filter;
}

void ZEDSelectionManager::SetLockSelection(bool Lock)
{
	if (LockSelection == Lock)
		return;

	LockSelection = Lock;

	UpdateCommands();

	if (LockSelection)
		OnSelectionLocked(this);
	else
		OnSelectionUnlocked(this);
}

bool ZEDSelectionManager::GetLockSelection()
{
	return LockSelection;
}

void ZEDSelectionManager::SetSelection(const ZEArray<ZEDObjectWrapper*>& NewSelection)
{
	if (LockSelection)
		return;

	ZEArray<ZEDObjectWrapper*> DeselectedObjects;
	for (ZESize I = 0; I < Selection.GetCount(); I++)
	{
		if (NewSelection.Exists(Selection[I]))
			continue;

		DeselectedObjects.Add(Selection[I]);
	}

	if (DeselectedObjects.GetCount() != 0)
	{
		ZE_SUPPRESS_EVENT(OnSelectionChanged)
		{
			DeselectObjects(DeselectedObjects);
		}
	}

	ZEArray<ZEDObjectWrapper*> SelectedObjects;
	for (ZESize I = 0; I < NewSelection.GetCount(); I++)
	{
		if (Selection.Exists(NewSelection[I]))
			continue;

		SelectedObjects.Add(NewSelection[I]);
	}

	if (SelectedObjects.GetCount() != 0)
	{
		ZE_SUPPRESS_EVENT(OnSelectionChanged)
		{
			SelectObjects(SelectedObjects);
		}
	}

	if (SelectedObjects.GetCount() != 0 || DeselectedObjects.GetCount() != 0)
		OnSelectionChanged(this, Selection);
}

const ZEArray<ZEDObjectWrapper*>& ZEDSelectionManager::GetSelection()
{
	return Selection;
}

ZEDObjectWrapper* ZEDSelectionManager::GetFocusedObject()
{
	return FocusedObject;
}

void ZEDSelectionManager::SelectObject(ZEDObjectWrapper* Object)
{
	if (LockSelection)
		return;

	if (Object == NULL)
		return;

	if (!Object->GetSelectable())
		return;

	if (Object->GetFrozen())
		return;

	if (Filter != NULL)
	{
		if (!ZEClass::IsDerivedFrom(Filter, Object->GetObject()->GetClass()))
			return;
	}

	if (Selection.Exists(Object))
		return;

	ZEArray<ZEDObjectWrapper*> OldSelection = GetSelection();

	Object->SetSelected(true);
	Selection.Add(Object);

	UpdateCommands();

	ZEArray<ZEDObjectWrapper*> ObjectList;
	ObjectList.Add(Object);
	OnObjectsSelected(this, ObjectList);
	OnSelectionChanged(this, Selection);
}

void ZEDSelectionManager::SelectObjects(const ZEArray<ZEDObjectWrapper*>& Objects)
{
	if (LockSelection)
		return;

	if (Objects.GetCount() == 0)
		return;

	ZEArray<ZEDObjectWrapper*> SelectedObjects;
	for (ZESize I = 0; I < Objects.GetCount(); I++)
	{
		if (Objects[I] == NULL)
			continue;

		if (!Objects[I]->GetSelectable()) 
			continue;

		if (Objects[I]->GetFrozen())
			continue;

		if (Selection.Exists(Objects[I]))
			continue;

		Objects[I]->SetSelected(true);

		SelectedObjects.Add(Objects[I]);
		Selection.Add(Objects[I]);
	}

	UpdateCommands();

	OnObjectsSelected(this, SelectedObjects);
	OnSelectionChanged(this, Selection);
}

void ZEDSelectionManager::DeselectObject(ZEDObjectWrapper* Object)
{
	if (LockSelection)
		return;

	if (!Selection.Exists(Object))
		return;

	if (FocusedObject == Object)
		ClearFocus();

	Object->SetSelected(false);
	Selection.RemoveValue(Object);

	UpdateCommands();

	ZEArray<ZEDObjectWrapper*> DeselectedObjects;
	DeselectedObjects.Add(Object);
	OnObjectsDeselected(this, DeselectedObjects);
	OnSelectionChanged(this, Selection);
}

void ZEDSelectionManager::DeselectObjects(const ZEArray<ZEDObjectWrapper*>& Objects)
{
	if (LockSelection)
		return;

	if (Objects.GetCount() == 0)
		return;

	ZEArray<ZEDObjectWrapper*> DeselectedObjects;
	for (ZESize I = 0; I < Objects.GetCount(); I++)
	{
		if (!Selection.Exists(Objects[I]))
			continue;

		if (FocusedObject == Objects[I])
			ClearFocus();

		Objects[I]->SetSelected(false);
		DeselectedObjects.Add(Objects[I]);
		Selection.RemoveValue(Objects[I]);
		I--;
	}

	UpdateCommands();

	OnObjectsDeselected(this, DeselectedObjects);
	OnSelectionChanged(this, Selection);
}

void ZEDSelectionManager::FocusObject(ZEDObjectWrapper* Object)
{
	if (LockSelection)
		return;

	if (Object == NULL)
		return;

	if (FocusedObject == Object)
		return;

	if (!Selection.Exists(Object))
		return;

	ZEDObjectWrapper* OldFocusedObject = FocusedObject;

	ClearFocus();

	Object->SetFocused(true);
	FocusedObject = Object;

	UpdateCommands();

	OnFocusChanged(this, FocusedObject);
}

void ZEDSelectionManager::ClearFocus()
{
	if (LockSelection)
		return;

	if (FocusedObject == NULL)
		return;

	ZEDObjectWrapper* OldFocusedObject = FocusedObject;

	FocusedObject->SetFocused(false);
	FocusedObject = NULL;

	UpdateCommands();

	OnFocusChanged(this, NULL);
}

const ZEArray<ZEDObjectWrapper*> ZEDSelectionManager::GetFrozenObjects()
{
	return FrozenObjects;
}

void ZEDSelectionManager::ClearSelection()
{
	if (LockSelection)
		return;

	ClearFocus();

	const ZEArray<ZEDObjectWrapper*> DeselectedObjects = Selection;
	for (ZESize I = 0; I < Selection.GetCount(); I++)
		Selection[I]->SetSelected(false);

	Selection.Clear();

	UpdateCommands();

	OnObjectsDeselected(this, DeselectedObjects);
	OnSelectionChanged(this, Selection);
}

void ZEDSelectionManager::FreezeObject(ZEDObjectWrapper* Object)
{
	if (FrozenObjects.Exists(Object))
		return;
	
	Object->SetFrozen(false);
	FrozenObjects.Add(Object);

	UpdateCommands();

	ZEArray<ZEDObjectWrapper*> FrozenObjectsList;
	FrozenObjectsList.Add(Object);
	OnObjectsFrozen(this, FrozenObjectsList);

	DeselectObject(Object);
}

void ZEDSelectionManager::FreezeObjects(const ZEArray<ZEDObjectWrapper*>& Objects)
{
	ZEArray<ZEDObjectWrapper*> FrozenObjectList;
	for (ZESize I = 0; I < Objects.GetCount(); I++)
	{
		if (FrozenObjects.Exists(Objects[I]))
			continue;

		Objects[I]->SetFrozen(true);
		FrozenObjects.Add(Objects[I]);
		FrozenObjectList.Add(Objects[I]);
	}
	
	OnObjectsUnfrozen(this, FrozenObjectList);

	UpdateCommands();

	DeselectObjects(Objects);
}

void ZEDSelectionManager::UnfreezeObject(ZEDObjectWrapper* Object)
{
	if (!FrozenObjects.Exists(Object))
		return;

	Object->SetFrozen(false);
	FrozenObjects.RemoveValue(Object);

	UpdateCommands();

	ZEArray<ZEDObjectWrapper*> UnfrozenObjectList;
	UnfrozenObjectList.Add(Object);
	OnObjectsUnfrozen(this, UnfrozenObjectList);
}

void ZEDSelectionManager::UnfreezeObjects(const ZEArray<ZEDObjectWrapper*>& Objects)
{
	ZEArray<ZEDObjectWrapper*> UnfrozenObjectList;
	for (ZESize I = 0; I < Objects.GetCount(); I++)
	{
		if (!FrozenObjects.Exists(Objects[I]))
			continue;

		FrozenObjects.RemoveValue(Objects[I]);
		UnfrozenObjectList.Add(Objects[I]);
		Objects[I]->SetFrozen(false);
	}

	UpdateCommands();

	OnObjectsUnfrozen(this, UnfrozenObjectList);
}

ZEDSelectionManager* ZEDSelectionManager::CreateInstance()
{
	return new ZEDSelectionManager();
}
