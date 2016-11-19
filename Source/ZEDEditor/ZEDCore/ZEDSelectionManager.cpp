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

#include "ZEMath/ZEViewVolume.h"
#include "ZEDEditor.h"
#include "ZEDObjectWrapper.h"
#include "ZEDObjectManager.h"
#include "ZEDGizmo.h"
#include "ZEDViewPort.h"
#include "ZEDViewportEvent.h"
#include "ZEDTransformationManager.h"
#include "ZEDSelectionEvent.h"
#include "ZEDEditorEvent.h"
#include "ZERenderer/ZERNScreenUtilities.h"
#include "ZEUI/ZEUIFrameControl.h"
#include "ZEMath/ZEMath.h"
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


void ZEDSelectionManager::ViewportKeyboardEvent(const ZEDViewportKeyboardEvent* Event)
{
	if (Event->GetKey() == ZED_VKK_ESCAPE && Event->GetType() == ZED_VIET_BUTTON_PRESSED)
	{
		ClearSelection();
		Event->Acquire();
	}
}

void ZEDSelectionManager::ViewportMouseEvent(const ZEDViewportMouseEvent* Event)
{
	if (Event->GetButton() == ZED_VMB_LEFT && Event->GetType() == ZED_VIET_BUTTON_PRESSED)
	{
		MultiSelection = true;
		MultiSelectionStartPosition = Event->GetPosition();

		// Single Selection	
		ZERayCastParameters Parameters;
		Parameters.Ray = ZERNScreenUtilities::ScreenToWorld(Event->GetViewport()->GetView(), Event->GetPosition());
		Parameters.Components |= ZE_RCRE_MULTI_COLLISIONS;

		ZERayCastReport Report;
		Report.SetParameters(&Parameters);
		GetEditor()->GetObjectManager()->GetRootWrapper()->RayCast(Report, Parameters);	

		const ZEArray<ZERayCastCollision>& Collisions = Report.GetCollisions();
		bool HasSelection = false;
		for (ZESize I = 0; I < Collisions.GetCount(); I++)
		{
			ZEDObjectWrapper* Wrapper = static_cast<ZEDObjectWrapper*>(Collisions[I].Object);
			
			if (!Wrapper->GetSelectable())
				continue;
			
			if (!Wrapper->GetPickable())
				continue;

			if (Wrapper->GetFrozen())
				continue;

			HasSelection = true;

			if ((Event->GetModifiers() & ZED_VKM_CTRL) != 0 || (Event->GetModifiers() & ZED_VKM_SHIFT) != 0)
			{
				SelectObject(Wrapper);
			}
			else if (Event->GetModifiers() & ZED_VKM_ALT)
			{
				DeselectObject(Wrapper);
			}
			else
			{
				if (Selection.Exists(Wrapper))
				{
					FocusObject(Wrapper);
				}
				else
				{
					ClearSelection();
					SelectObject(Wrapper);
				}
			}

			break;
		}
	
		if (!HasSelection)
			ClearSelection();

		Event->Acquire();
	}
	else if (Event->GetType() == ZED_VIET_MOUSE_MOVED)
	{
		if (MultiSelection)
		{
			ZEVector2 Size;
			Size.x = ZEMath::Abs(MultiSelectionStartPosition.x - Event->GetPosition().x);
			Size.y = ZEMath::Abs(MultiSelectionStartPosition.y - Event->GetPosition().y);

			if (Size.Min() != 0)
			{
				ZEVector2 Position;
				Position.x = MultiSelectionStartPosition.x < Event->GetPosition().x ? MultiSelectionStartPosition.x : Event->GetPosition().x;
				Position.y = MultiSelectionStartPosition.y < Event->GetPosition().y ? MultiSelectionStartPosition.y : Event->GetPosition().y;

				MultiSelectionBox->SetPosition(Position);
				MultiSelectionBox->SetSize(Size);
				MultiSelectionBox->SetVisiblity(true);
			}
		}
		else
		{
			MultiSelectionBox->SetVisiblity(false);
		}
	}
	else if (Event->GetButton() == ZED_VMB_LEFT && Event->GetType() == ZED_VIET_BUTTON_RELEASED)
	{
		if (MultiSelection)
		{
			ZEVector2 Size;
			Size.x = ZEMath::Abs(MultiSelectionStartPosition.x - Event->GetPosition().x);
			Size.y = ZEMath::Abs(MultiSelectionStartPosition.y - Event->GetPosition().y);

			ZEArray<ZEDObjectWrapper*> List;
			if (Size.Min() != 0)
			{
				ZEVector2 Position;
				Position.x = MultiSelectionStartPosition.x < Event->GetPosition().x ? MultiSelectionStartPosition.x : Event->GetPosition().x;
				Position.y = MultiSelectionStartPosition.y < Event->GetPosition().y ? MultiSelectionStartPosition.y : Event->GetPosition().y;

				const ZERNView& View = Event->GetViewport()->GetView();
				ZERay LeftBottomRay =  ZERNScreenUtilities::ScreenToWorld(View, Position + ZEVector2(0.0f, Size.y));
				ZERay RightBottomRay = ZERNScreenUtilities::ScreenToWorld(View, Position + ZEVector2(Size.x, Size.y));
				ZERay LeftTopRay =	ZERNScreenUtilities::ScreenToWorld(View, Position);
				ZERay RightTopRay = ZERNScreenUtilities::ScreenToWorld(View, Position + ZEVector2(Size.x, 0.0f));

				ZEFrustum Frustum;
				ZEPlane::Create(Frustum.LeftPlane,	LeftBottomRay.GetPointOn(1.0f),	 LeftBottomRay.p,  LeftTopRay.GetPointOn(1.0f));
				ZEPlane::Create(Frustum.RightPlane,	RightTopRay.GetPointOn(1.0f),    RightBottomRay.p, RightBottomRay.GetPointOn(1.0f));
				ZEPlane::Create(Frustum.BottomPlane,	RightBottomRay.GetPointOn(1.0f), LeftBottomRay.p,  LeftBottomRay.GetPointOn(1.0f));
				ZEPlane::Create(Frustum.TopPlane,	LeftTopRay.GetPointOn(1.0f),	 LeftTopRay.p,	   RightTopRay.GetPointOn(1.0f));

				Frustum.NearPlane.p = View.Position + View.Direction * View.NearZ;
				Frustum.NearPlane.n = View.Direction;
				Frustum.FarPlane.p = View.Position + View.Direction * View.FarZ;
				Frustum.FarPlane.n = -View.Direction;

				ZEDObjectWrapper* RootWrapper = GetEditor()->GetObjectManager()->GetRootWrapper();

				const ZEArray<ZEDObjectWrapper*> ChildWrappers = RootWrapper->GetChildWrappers();
				for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
				{
					ZEDObjectWrapper* ChildWrapper = ChildWrappers[I];

					if (!ChildWrapper->GetSelectable())
						continue;

					if (!ChildWrapper->GetPickable())
						continue;

					if (ChildWrapper->GetFrozen())
						continue;

					ZEOBBox BoundingBox;
					ZEAABBox::GenerateOBoundingBox(BoundingBox, ChildWrapper->GetBoundingBox());
					ZEOBBox WorldBoundingBox;
					ZEOBBox::Transform(WorldBoundingBox, ChildWrapper->GetWorldTransform(), BoundingBox);

					if (GetSelectionMode() == ZE_SM_FULLY_COVERS)
					{
						if (Frustum.IntersectionTestExact(Frustum, WorldBoundingBox) != ZE_IR_COVERS)
							continue;
					}
					else
					{
						if (Frustum.IntersectionTestExact(Frustum, WorldBoundingBox) == ZE_IR_NONE)
							continue;
					}

					List.Add(ChildWrapper);
				}

				if ((Event->GetModifiers() & ZED_VKM_CTRL) != 0 || (Event->GetModifiers() & ZED_VKM_SHIFT) != 0)
					SelectObjects(List);
				else if (Event->GetModifiers() & ZED_VKM_ALT)
					DeselectObjects(List);
				else
					SetSelection(List);
			}
		}

		MultiSelection = false;
		MultiSelectionBox->SetVisiblity(false);
	}
	else if (Event->GetButton() == ZED_VMB_RIGHT && Event->GetType() == ZED_VIET_BUTTON_PRESSED)
	{
		MultiSelection = false;
		MultiSelectionBox->SetVisiblity(false);
	}
}

void ZEDSelectionManager::EditorEvent(const ZEDEditorEvent* Event)
{
	if (Event->GetType() == ZED_EET_FILE_CLOSED)
		ClearSelection();
}

bool ZEDSelectionManager::InitializeInternal()
{
	if (!ZEInitializable::InitializeInternal())
		return false;

	MultiSelectionBox = new ZEUIFrameControl();
	MultiSelectionBox->SetBackgroundColor(ZEVector4(0.17f, 0.5f, 1.0f, 0.5f));
	MultiSelectionBox->SetVisiblity(false);
	MultiSelectionBox->SetZOrder(1000);
	GetEditor()->GetUIManager()->AddControl(MultiSelectionBox);

	RegisterCommands();

	return true;
}

bool ZEDSelectionManager::DeinitializeInternal()
{
	GetEditor()->GetUIManager()->RemoveControl(MultiSelectionBox);
	MultiSelectionBox->Destroy();

	return ZEInitializable::DeinitializeInternal();
}

ZEDSelectionManager::ZEDSelectionManager()
{
	LockSelection = false;
	MultiSelection = false;
	FocusedObject = NULL;
	Filter = NULL;
}

ZEDSelectionManager::~ZEDSelectionManager()
{

}

void ZEDSelectionManager::RegisterCommands()
{
	SelectAllCommand.SetCategory("Selection");
	SelectAllCommand.SetName("ZEDSelectionManager::SelectAllCommand");
	SelectAllCommand.SetText("Select All");
	SelectAllCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_A));
	SelectAllCommand.OnAction += ZEDCommandDelegate::Create<ZEDSelectionManager, &ZEDSelectionManager::SelectionModeCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&SelectAllCommand);
	
	ClearSelectionCommand.SetCategory("Selection");
	ClearSelectionCommand.SetName("ZEDSelectionManager::ClearSelectionCommand");
	ClearSelectionCommand.SetText("Clear Selection");
	ClearSelectionCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_D));
	ClearSelectionCommand.OnAction += ZEDCommandDelegate::Create<ZEDSelectionManager, &ZEDSelectionManager::ClearSelectionCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&ClearSelectionCommand);

	LockSelectionCommand.SetCategory("Selection");
	LockSelectionCommand.SetName("ZEDSelectionManager::LockSelectionCommand");
	LockSelectionCommand.SetText("Lock Selection");
	LockSelectionCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_L));
	LockSelectionCommand.OnAction += ZEDCommandDelegate::Create<ZEDSelectionManager, &ZEDSelectionManager::LockSelectionCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&LockSelectionCommand);

	FreezeObjectsCommand.SetCategory("Selection");
	FreezeObjectsCommand.SetName("ZEDSelectionManager::FreezeObjectsCommand");
	FreezeObjectsCommand.SetText("Freeze Objects");
	FreezeObjectsCommand.OnAction += ZEDCommandDelegate::Create<ZEDSelectionManager, &ZEDSelectionManager::FreezeObjectsCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&FreezeObjectsCommand);

	UnfreezeObjectCommand.SetCategory("Selection");
	UnfreezeObjectCommand.SetName("ZEDSelectionManager::UnfreezeObjectsCommand");
	UnfreezeObjectCommand.SetText("Unfreeze Objects");
	UnfreezeObjectCommand.OnAction += ZEDCommandDelegate::Create<ZEDSelectionManager, &ZEDSelectionManager::UnfreezeObjectsCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&UnfreezeObjectCommand);

	SelectionModeCommand.SetCategory("Selection");
	SelectionModeCommand.SetName("ZEDSelectionManager::SelectionModeCommand");
	SelectionModeCommand.SetText("Intersects");
	SelectionModeCommand.SetType(ZED_CT_CHECK);
	SelectionModeCommand.OnAction += ZEDCommandDelegate::Create<ZEDSelectionManager, &ZEDSelectionManager::SelectionModeCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&SelectionModeCommand);

	SelectionShapeCommand.SetCategory("Selection");
	SelectionShapeCommand.SetName("ZEDSelectionManager::SelectionShapeCommand");
	SelectionShapeCommand.SetText("Shape");
	SelectionShapeCommand.SetType(ZED_CT_LIST);
	ZEArray<ZEString> Items;
	Items.Add("Rectangle");
	Items.Add("Circle");
	Items.Add("Brush");
	SelectionShapeCommand.SetListItems(Items);
	SelectionShapeCommand.OnAction += ZEDCommandDelegate::Create<ZEDSelectionManager, &ZEDSelectionManager::SelectionShapeCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&SelectionShapeCommand);
}

void ZEDSelectionManager::SelectAllCommand_OnAction(const ZEDCommand* Command)
{

}

void ZEDSelectionManager::ClearSelectionCommand_OnAction(const ZEDCommand* Command)
{
	ClearSelection();
}

void ZEDSelectionManager::LockSelectionCommand_OnAction(const ZEDCommand* Command)
{
	SetLockSelection(Command->GetValue().GetBoolean());
}

void ZEDSelectionManager::FreezeObjectsCommand_OnAction(const ZEDCommand* Command)
{
	FreezeObjects(GetSelection());
}

void ZEDSelectionManager::UnfreezeObjectsCommand_OnAction(const ZEDCommand* Command)
{
	UnfreezeObjects(GetFrozonObjects());
}

void ZEDSelectionManager::SelectionModeCommand_OnAction(const ZEDCommand* Command)
{
	if (Command->GetValue().GetBoolean())
	{
		SetSelectionMode(ZE_SM_FULLY_COVERS);
		SelectionModeCommand.SetText("Covers");
	}
	else
	{
		SetSelectionMode(ZE_SM_INTERSECTS);
		SelectionModeCommand.SetText("Intersects");
	}
}

void ZEDSelectionManager::SelectionShapeCommand_OnAction(const ZEDCommand* Command)
{
	const ZEString& Value = Command->GetValue().GetString();
	
	if (Value == "Circle")
		SetSelectionShape(ZED_SS_CIRCLE);
	else if (Value == "Brush")
		SetSelectionShape(ZED_SS_BRUSH);
	else
		SetSelectionShape(ZED_SS_RECTANGLE);
}

void ZEDSelectionManager::SetSelectionMode(ZEDSelectionMode Mode)
{
	SelectionMode = Mode;

	ZEDSelectionEvent Event;
	Event.SetManager(this);
	Event.SetType(ZED_SET_MANAGER_STATE_CHANGED);
	RaiseEvent(&Event);
}

ZEDSelectionMode ZEDSelectionManager::GetSelectionMode()
{
	return SelectionMode;
}

void ZEDSelectionManager::SetSelectionShape(ZEDSelectionShape Shape)
{
	SelectionShape = Shape;

	ZEDSelectionEvent Event;
	Event.SetManager(this);
	Event.SetType(ZED_SET_MANAGER_STATE_CHANGED);
	RaiseEvent(&Event);
}

ZEDSelectionShape ZEDSelectionManager::GetSelectionShape()
{
	return SelectionShape;
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

	ZEDSelectionEvent Event;
	Event.SetManager(this);
	Event.SetType(ZED_SET_MANAGER_STATE_CHANGED);
	RaiseEvent(&Event);
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
		DeselectObjects(DeselectedObjects);

	ZEArray<ZEDObjectWrapper*> SelectedObjects;
	for (ZESize I = 0; I < NewSelection.GetCount(); I++)
	{
		if (Selection.Exists(NewSelection[I]))
			continue;

		SelectedObjects.Add(NewSelection[I]);
	}

	if (SelectedObjects.GetCount() != 0)
		SelectObjects(SelectedObjects);
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

	ZEDSelectionEvent Event;
	Event.SetManager(this);
	Event.SetType(ZED_SET_SELECTED);
	Event.SetList(&Selection);
	Event.SetOldList(&OldSelection);
	RaiseEvent(&Event);
}

void ZEDSelectionManager::SelectObjects(const ZEArray<ZEDObjectWrapper*>& Objects)
{
	if (LockSelection)
		return;

	ZEArray<ZEDObjectWrapper*> OldSelection = Selection;

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
		Selection.Add(Objects[I]);
	}
	
	ZEDSelectionEvent Event;
	Event.SetManager(this);
	Event.SetType(ZED_SET_SELECTED);
	Event.SetList(&Selection);
	Event.SetOldList(&OldSelection);
	RaiseEvent(&Event);
}

void ZEDSelectionManager::DeselectObject(ZEDObjectWrapper* Object)
{
	if (LockSelection)
		return;

	if (!Selection.Exists(Object))
		return;

	ZEArray<ZEDObjectWrapper*> OldSelection = GetSelection();

	if (FocusedObject == Object)
		ClearFocus();

	Object->SetSelected(false);
	Selection.RemoveValue(Object);

	ZEDSelectionEvent Event;
	Event.SetManager(this);
	Event.SetType(ZED_SET_DESELECTED);
	Event.SetList(&Selection);
	Event.SetOldList(&OldSelection);
	RaiseEvent(&Event);
}

void ZEDSelectionManager::DeselectObjects(const ZEArray<ZEDObjectWrapper*>& Objects)
{
	if (LockSelection)
		return;

	ZEArray<ZEDObjectWrapper*> OldSelection = Selection;

	for (ZESize I = 0; I < Objects.GetCount(); I++)
	{
		if (!Selection.Exists(Objects[I]))
			continue;

		if (FocusedObject == Objects[I])
			ClearFocus();

		Objects[I]->SetSelected(false);
		Selection.RemoveValue(Objects[I]);
		I--;
	}

	ZEDSelectionEvent Event;
	Event.SetManager(this);
	Event.SetType(ZED_SET_DESELECTED);
	Event.SetList(&Selection);
	Event.SetOldList(&OldSelection);
	RaiseEvent(&Event);
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

	ZEDSelectionEvent Event;
	Event.SetManager(this);
	Event.SetType(ZED_SET_FOCUS_CHANGED);
	Event.SetFocusedObject(FocusedObject);
	Event.SetOldFocusedObject(OldFocusedObject);
	RaiseEvent(&Event);
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

	ZEDSelectionEvent Event;
	Event.SetManager(this);
	Event.SetType(ZED_SET_FOCUS_CHANGED);
	Event.SetFocusedObject(NULL);
	Event.SetOldFocusedObject(OldFocusedObject);
	RaiseEvent(&Event);
}

const ZEArray<ZEDObjectWrapper*> ZEDSelectionManager::GetFrozonObjects()
{
	return FrozenObjects;
}

void ZEDSelectionManager::ClearSelection()
{
	if (LockSelection)
		return;

	const ZEArray<ZEDObjectWrapper*> OldSelection = Selection;

	ClearFocus();

	for (ZESize I = 0; I < Selection.GetCount(); I++)
		Selection[I]->SetSelected(false);

	Selection.Clear();

	ZEDSelectionEvent Event;
	Event.SetManager(this);
	Event.SetType(ZED_SET_DESELECTED);
	Event.SetList(&Selection);
	Event.SetOldList(&OldSelection);
	RaiseEvent(&Event);
}

void ZEDSelectionManager::FreezeObject(ZEDObjectWrapper* Object)
{
	if (FrozenObjects.Exists(Object))
		return;

	ZEArray<ZEDObjectWrapper*> OldFrozenObjects = FrozenObjects;

	Object->SetFrozen(false);
	FrozenObjects.Add(Object);

	ZEDSelectionEvent Event;
	Event.SetManager(this);
	Event.SetType(ZED_SET_OBJECTS_FREEZED);
	Event.SetList(&FrozenObjects);
	Event.SetOldList(&OldFrozenObjects);
	RaiseEvent(&Event);

	DeselectObject(Object);
}

void ZEDSelectionManager::FreezeObjects(const ZEArray<ZEDObjectWrapper*>& Objects)
{
	ZEArray<ZEDObjectWrapper*> OldFrozenObjects = FrozenObjects;
	
	for (ZESize I = 0; I < Objects.GetCount(); I++)
	{
		if (FrozenObjects.Exists(Objects[I]))
			continue;

		Objects[I]->SetFrozen(true);
		FrozenObjects.Add(Objects[I]);
	}

	ZEDSelectionEvent Event;
	Event.SetManager(this);
	Event.SetType(ZED_SET_OBJECTS_FREEZED);
	Event.SetList(&FrozenObjects);
	Event.SetOldList(&OldFrozenObjects);
	RaiseEvent(&Event);

	DeselectObjects(Objects);
}

void ZEDSelectionManager::UnfreezeObject(ZEDObjectWrapper* Object)
{
	if (!FrozenObjects.Exists(Object))
		return;

	ZEArray<ZEDObjectWrapper*> OldList = FrozenObjects;
	
	Object->SetFrozen(false);
	FrozenObjects.RemoveValue(Object);

	ZEDSelectionEvent Event;
	Event.SetManager(this);
	Event.SetType(ZED_SET_OBJECTS_UNFREEZED);
	Event.SetList(&FrozenObjects);
	Event.SetOldList(&OldList);
	RaiseEvent(&Event);
}

void ZEDSelectionManager::UnfreezeObjects(const ZEArray<ZEDObjectWrapper*>& Objects)
{
	ZEArray<ZEDObjectWrapper*> OldList = FrozenObjects;

	for (ZESize I = 0; I < Objects.GetCount(); I++)
	{
		if (!FrozenObjects.Exists(Objects[I]))
			continue;

		FrozenObjects.RemoveValue(Objects[I]);
		Objects[I]->SetFrozen(false);
	}

	ZEDSelectionEvent Event;
	Event.SetManager(this);
	Event.SetType(ZED_SET_OBJECTS_UNFREEZED);
	Event.SetList(&FrozenObjects);
	Event.SetOldList(&OldList);
	RaiseEvent(&Event);
}

ZEDSelectionManager* ZEDSelectionManager::CreateInstance()
{
	return new ZEDSelectionManager();
}
