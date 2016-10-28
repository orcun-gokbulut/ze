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

/*void ZEDSelectionManager::SelectObject(ZEViewVolume* ViewVolume)
{
	if (ViewVolume == NULL)
		return;
	
	if (ViewVolume->GetViewVolumeType() == ZE_VVT_NONE)
		return;

	const ZEArray<ZEDObjectWrapper*>& Wrappers = GetModule()->GetRootWrapper()->GetChildWrappers();
	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{
		if (!ViewVolume->CullTest(Wrappers[I]->GetLocalBoundingBox()))
			SelectObject(Wrappers[I]);
	}
}

void ZEDSelectionManager::SelectObject(const ZERNView& View, const ZEVector2& ScreenPoint1, const ZEVector2& ScreenPoint2)
{
	ZECamera* ActiveCamera = ZEDCore::GetInstance()->GetEditorModule()->GetScene()->GetActiveCamera();

	if (ActiveCamera == NULL)
		return;

	if (ScreenPoint1.Equals(ScreenPoint2))
	{
		ZERay Ray;
		ActiveCamera->GetScreenRay(Ray, ScreenPoint1.x, ScreenPoint1.y);
		SelectObject(Ray);
		return;
	}

	if (ScreenPoint1.x == ScreenPoint2.x || ScreenPoint1.y == ScreenPoint2.y)
		return;

	ZEVector2 LeftUp, RightDown;

	if (ScreenPoint1.x >= ScreenPoint2.x)
	{
		LeftUp.x = ScreenPoint2.x;
		RightDown.x = ScreenPoint1.x;
	}
	else
	{
		LeftUp.x = ScreenPoint1.x;
		RightDown.x = ScreenPoint2.x;
	}

	if (ScreenPoint1.y >= ScreenPoint2.y)
	{
		LeftUp.y = ScreenPoint2.y;
		RightDown.y = ScreenPoint1.y;
	}
	else
	{
		LeftUp.y = ScreenPoint1.y;
		RightDown.y = ScreenPoint2.y;
	}

	ZERay RayLU, RayRU, RayLD, RayRD;
	ActiveCamera->GetScreenRay(RayLU, LeftUp.x, LeftUp.y);
	ActiveCamera->GetScreenRay(RayRU, RightDown.x, LeftUp.y);
	ActiveCamera->GetScreenRay(RayLD, LeftUp.x, RightDown.y);
	ActiveCamera->GetScreenRay(RayRD, RightDown.x, RightDown.y);
	float FarZ = ActiveCamera->GetFarZ();

	ZEPlane LeftClipPlane, TopClipPlane, RightClipPlane, BottomClipPlane, NearClipPlane, FarClipPlane;
	ZEViewFrustum SelectionFrustum;

	ZEPlane::Create(LeftClipPlane, RayLD.GetPointOn(1.0f), RayLU.GetPointOn(1.0f), RayLU.GetPointOn(10.0f));
	ZEPlane::Create(TopClipPlane, RayLU.GetPointOn(1.0f), RayRU.GetPointOn(1.0f), RayRU.GetPointOn(10.0f));
	ZEPlane::Create(BottomClipPlane, RayRD.GetPointOn(1.0f), RayLD.GetPointOn(1.0f), RayLD.GetPointOn(10.0f));
	ZEPlane::Create(RightClipPlane, RayRU.GetPointOn(1.0f), RayRD.GetPointOn(1.0f), RayRD.GetPointOn(10.0f));
	ZEPlane::Create(NearClipPlane, RayLU.GetPointOn(1.0f), RayLD.GetPointOn(1.0f), RayRD.GetPointOn(1.0f));
	ZEPlane::Create(FarClipPlane, RayRD.GetPointOn(FarZ), RayLD.GetPointOn(FarZ), RayLU.GetPointOn(FarZ));

	SelectionFrustum.Create(RightClipPlane, BottomClipPlane, LeftClipPlane, TopClipPlane, FarClipPlane, NearClipPlane);

	SelectObject(&SelectionFrustum);
}*/


/*void ZEDSelectionManager::DeselectObject(ZEViewVolume* ViewVolume)
{
	if (ViewVolume == NULL)
		return;

	if (ViewVolume->GetViewVolumeType() == ZE_VVT_NONE)
		return;

	const ZEArray<ZEDObjectWrapper*>& Wrappers = GetModule()->GetRootWrapper()->GetChildWrappers();
	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{
		if (!ViewVolume->CullTest(Wrappers[I]->GetLocalBoundingBox()))
			DeselectObject(Wrappers[I]);
	}
}


void ZEDSelectionManager::DeselectObject(const ZERNView& View, const ZEVector2& ScreenPoint1, const ZEVector2& ScreenPoint2)
{
	ZECamera* ActiveCamera = ZEDCore::GetInstance()->GetEditorModule()->GetScene()->GetActiveCamera();

	if (ActiveCamera == NULL)
		return;

	if (ScreenPoint1.Equals(ScreenPoint2))
	{
		ZERay Ray;
		ActiveCamera->GetScreenRay(Ray, ScreenPoint1.x, ScreenPoint1.y);
		DeselectObject(Ray);
	}

	ZEVector2 LeftUp, RightDown;

	if (ScreenPoint1.x >= ScreenPoint2.x)
	{
		LeftUp.x = ScreenPoint2.x;
		RightDown.x = ScreenPoint1.x;
	}
	else
	{
		LeftUp.x = ScreenPoint1.x;
		RightDown.x = ScreenPoint2.x;
	}

	if (ScreenPoint1.y >= ScreenPoint2.y)
	{
		LeftUp.y = ScreenPoint2.y;
		RightDown.y = ScreenPoint1.y;
	}
	else
	{
		LeftUp.y = ScreenPoint1.y;
		RightDown.y = ScreenPoint2.y;
	}

	ZERay RayLU, RayRU, RayLD, RayRD;
	ActiveCamera->GetScreenRay(RayLU, LeftUp.x, LeftUp.y);
	ActiveCamera->GetScreenRay(RayRU, RightDown.x, LeftUp.y);
	ActiveCamera->GetScreenRay(RayLD, LeftUp.x, RightDown.y);
	ActiveCamera->GetScreenRay(RayRD, RightDown.x, RightDown.y);
	float FarZ = ActiveCamera->GetFarZ();

	ZEPlane LeftClipPlane, TopClipPlane, RightClipPlane, BottomClipPlane, NearClipPlane, FarClipPlane;
	ZEViewFrustum SelectionFrustum;

	ZEPlane::Create(LeftClipPlane, RayLD.p, RayLU.p, RayLU.GetPointOn(1.0f));
	ZEPlane::Create(TopClipPlane, RayLU.p, RayRU.p, RayRU.GetPointOn(1.0f));
	ZEPlane::Create(BottomClipPlane, RayRD.p, RayLD.p, RayLD.GetPointOn(1.0f));
	ZEPlane::Create(RightClipPlane, RayRU.p, RayRD.p, RayRD.GetPointOn(1.0f));
	ZEPlane::Create(NearClipPlane, RayLU.p, RayLD.p, RayRD.p);
	ZEPlane::Create(FarClipPlane, RayRD.GetPointOn(FarZ), RayLD.GetPointOn(FarZ), RayLU.GetPointOn(FarZ));

	SelectionFrustum.Create(RightClipPlane, BottomClipPlane, LeftClipPlane, TopClipPlane, FarClipPlane, NearClipPlane);

	DeselectObject(&SelectionFrustum);
}*/


ZEDSelectionManager::ZEDSelectionManager()
{
	LockSelection = false;
	FocusedObject = NULL;
	Filter = NULL;
}

ZEDSelectionManager::~ZEDSelectionManager()
{

}

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

void ZEDSelectionManager::EditorEvent(const ZEDEditorEvent* Event)
{
	if (Event->GetType() == ZED_EET_FILE_CLOSED)
		ClearSelection();
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
	if (Event->GetButton() != ZED_VMB_LEFT)
		return;

	if (Event->GetType() == ZED_VIET_BUTTON_PRESSED)
	{
		SelectionStartPosition = Event->GetPosition();

		// Single Selection	
		ZERayCastParameters Parameters;
		Parameters.Ray = ZERNScreenUtilities::ScreenToWorld(Event->GetViewport()->GetView(), Event->GetPosition());

		ZERayCastReport Report;
		Report.SetParameters(&Parameters);

		GetEditor()->GetObjectManager()->GetRootWrapper()->RayCast(Report, Parameters);	
		if (Report.GetResult())
		{
			ZEDObjectWrapper* Wrapper = static_cast<ZEDObjectWrapper*>(Report.GetCollision().Object);
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
		}
		else
		{
			ClearSelection();
		}

		Event->Acquire();
	}
	else if (Event->GetType() == ZED_VIET_BUTTON_PRESSED)
	{
		if (SelectionStartPosition - Event->GetPosition() == ZEVector2::Zero)
		{
		}
	}
}

ZEDSelectionManager* ZEDSelectionManager::CreateInstance()
{
	return new ZEDSelectionManager();
}
