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

#include "ZEDCore.h"
#include "ZEDModule.h"
#include "ZEDObjectWrapper.h"
#include "ZEDGizmo.h"
#include "ZEDViewPort.h"
#include "ZEDViewportInput.h"
#include "ZEDTransformationManager.h"
#include "ZERenderer/ZERNScreenUtilities.h"

ZEDSelectionEvent::ZEDSelectionEvent()
{
	Type = ZED_SET_NONE;
	Selection = NULL;
	OldSelection = NULL;
	SelectedObjects = NULL;
	UnselectedObjects = NULL;
}

ZEDSelectionEventType ZEDSelectionEvent::GetType() const
{
	return Type;
}

const ZEArray<ZEDObjectWrapper*>& ZEDSelectionEvent::GetSelection() const
{
	return *Selection;
}

const ZEArray<ZEDObjectWrapper*>& ZEDSelectionEvent::GetOldSelection() const
{
	return *OldSelection;
}

const ZEArray<ZEDObjectWrapper*>& ZEDSelectionEvent::GetSelectedObjects() const
{
	return *SelectedObjects;
}

const ZEArray<ZEDObjectWrapper*>& ZEDSelectionEvent::GetUnselectedObjects() const
{
	return *UnselectedObjects;
}

void ZEDSelectionManager::CalculateSelectionPivot()
{
	if (Selection.GetCount() == 0)
		return;

	ZEVector3 ObjectCenter;
	ZEVector3 SelectionCenterPosition = ZEVector3::Zero;
	ZEQuaternion SelectionCenterRotation = ZEQuaternion::Identity;
	ZEDTransformSpace TargetSpace = ZEDTransformationManager::GetInstance()->GetTransformSpace();

	if (PivotMode == ZED_SCM_ENTITY_PIVOT)
	{
		SelectionCenterPosition = Selection.GetFirstItem()->GetPosition(); //GetPivotPosition

		if (TargetSpace == ZED_TS_LOCAL)
			SelectionCenterRotation = Selection.GetFirstItem()->GetRotation(); //GetPivotRotation
	}
	else if (PivotMode == ZED_SCM_SELECTION_CENTER)
	{
		if (TargetSpace == ZED_TS_LOCAL)
		{
			SelectionCenterPosition = Selection.GetFirstItem()->GetLocalBoundingBox().GetCenter();
			SelectionCenterRotation = Selection.GetFirstItem()->GetRotation(); //GetPivotRotation ? BBox rotation?
		}
		else if (TargetSpace == ZED_TS_WORLD)
		{
			for (ZESize I = 0; I < Selection.GetCount(); I++)
			{
				ObjectCenter = Selection[I]->GetLocalBoundingBox().GetCenter();
				SelectionCenterPosition += ObjectCenter;
			}

			SelectionCenterPosition /= Selection.GetCount();
		}
	}
	else if (PivotMode == ZED_SCM_SPACE_CENTER)
	{
		if (TargetSpace == ZED_TS_LOCAL)
		{
			SelectionCenterPosition = Selection.GetFirstItem()->GetPosition(); //GetPivotPosition
			SelectionCenterRotation = Selection.GetFirstItem()->GetRotation(); //GetPivotRotation
		}
		else if (TargetSpace == ZED_TS_WORLD)
		{
			SelectionCenterPosition = ZEVector3::Zero;
			SelectionCenterRotation = ZEQuaternion::Identity;
		}
	}

	ZEMatrix4x4::CreateOrientation(SelectionPivot, SelectionCenterPosition, SelectionCenterRotation, ZEVector3::One);
}

void ZEDSelectionManager::UpdateSelectionGizmo()
{
/*	const ZEMatrix4x4& Pivot = GetSelectionPivot();
	ZEDTransformSpace Space = ZEDTransformationManager::GetInstance()->GetTransformSpace();
	ZEDGizmo* Gizmo = ZEDTransformationManager::GetInstance()->GetGizmo();

	Gizmo->SetVisible(Selection.GetCount());
	Gizmo->SetPosition(Pivot.GetTranslation());

	if (Space == ZED_TS_LOCAL && (Gizmo->GetMode() == ZED_GM_MOVE || Gizmo->GetMode() == ZED_GM_HELPER))
	{
		Gizmo->SetRotation(Pivot.GetRotation());
	}*/
}

ZEDModule* ZEDSelectionManager::GetModule()
{
	return Module;
}

const ZEArray<ZEDObjectWrapper*>& ZEDSelectionManager::GetSelectedObjects()
{
	return Selection;
}

void ZEDSelectionManager::SelectObject(ZEDObjectWrapper* Object)
{
	if (Object == NULL)
		return;

	if (!Object->GetSelectable())
		return;

	if (Filter != NULL)
	{
		if (!ZEClass::IsDerivedFrom(Filter, Object->GetObject()->GetClass()))
			return;
	}

	if (Selection.Exists(Object))
		return;

	ZEArray<ZEDObjectWrapper*> OldSelection = GetSelectedObjects();

	Object->SetSelected(true);
	Selection.Add(Object);

	ZEArray<ZEDObjectWrapper*> SelectedObjects;
	ZEArray<ZEDObjectWrapper*> UnselectedObjects;
	SelectedObjects.Add(Object);

	ZEDSelectionEvent Event;
	Event.Type = ZED_SET_SELECTED;
	Event.Selection = &Selection;
	Event.OldSelection = &OldSelection;
	Event.SelectedObjects = &SelectedObjects;
	Event.UnselectedObjects = &UnselectedObjects;

	GetModule()->SelectionEvent(Event);
}

void ZEDSelectionManager::SelectObjects(const ZEArray<ZEDObjectWrapper*>& Objects)
{
	ZEArray<ZEDObjectWrapper*> OldSelection = Selection;
	ZEArray<ZEDObjectWrapper*> SelectedObjects;
	ZEArray<ZEDObjectWrapper*> UnselectedObjects;

	for (ZESize I = 0; I < Objects.GetCount(); I++)
	{
		if (Objects[I] == NULL)
			continue;

		if (Selection.Exists(Objects[I]))
			continue;;

		Objects[I]->SetSelected(true);
		SelectedObjects.Add(Objects[I]);
		Selection.Add(Objects[I]);
	}


	ZEDSelectionEvent Event;
	Event.Type = ZED_SET_SELECTED;
	Event.Selection = &Selection;
	Event.OldSelection = &OldSelection;
	Event.SelectedObjects = &SelectedObjects;
	Event.UnselectedObjects = &UnselectedObjects;

	GetModule()->SelectionEvent(Event);
}

/*
void ZEDSelectionManager::SelectObject(const ZERay& Ray)
{
	ZERayCastParameters Parameters;
	Parameters.Ray = Ray;
	Parameters.FilterFunction = ZERayCastFilterFunction::Create<ZEDSelectionManager, &ZEDSelectionManager::FilterSelection>(this);
	Parameters.FilterFunctionParameter = Filter;
	ZERayCastReport Report;
	ZEScene* Scene = ZEDCore::GetInstance()->GetEditorModule()->GetScene();
	if (!Scene->RayCast(Report, Parameters))
		return;

	SelectObject(Scene->GetWrapper(Report.Object));
}

void ZEDSelectionManager::SelectObject(ZEViewVolume* ViewVolume)
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
}

void ZEDSelectionManager::SelectObject(const ZEString& Name)
{
	if (Name.IsEmpty())
		return;

	const ZEArray<ZEDObjectWrapper*>& Wrappers = GetModule()->GetRootWrapper()->GetChildWrappers();
	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{
		if (Wrappers[I]->GetName() == Name)
			SelectObject(Wrappers[I]);
	}
}

void ZEDSelectionManager::SelectObject(ZESize Id)
{
	const ZEArray<ZEDObjectWrapper*>& Wrappers = GetModule()->GetRootWrapper()->GetChildWrappers();
	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{
		if (Wrappers[I]->GetId() == Id)
		{
			SelectObject(Wrappers[I]);
			return;
		}
	}
}*/

void ZEDSelectionManager::DeselectObject(ZEDObjectWrapper* Object)
{
	if (!Selection.Exists(Object))
		return;

	Object->SetSelected(false);
	Selection.RemoveValue(Object);

	ZEArray<ZEDObjectWrapper*> OldSelection = GetSelectedObjects();

	Object->SetSelected(true);
	Selection.Add(Object);

	ZEArray<ZEDObjectWrapper*> SelectedObjects;
	ZEArray<ZEDObjectWrapper*> UnselectedObjects;
	UnselectedObjects.Add(Object);

	ZEDSelectionEvent Event;
	Event.Type = ZED_SET_DESELECTED;
	Event.Selection = &Selection;
	Event.OldSelection = &OldSelection;
	Event.SelectedObjects = &SelectedObjects;
	Event.UnselectedObjects = &UnselectedObjects;

	GetModule()->SelectionEvent(Event);
}

void ZEDSelectionManager::DeselectObjects(const ZEArray<ZEDObjectWrapper*>& Objects)
{
	ZEArray<ZEDObjectWrapper*> OldSelection = Selection;
	ZEArray<ZEDObjectWrapper*> SelectedObjects;
	ZEArray<ZEDObjectWrapper*> UnselectedObjects;

	for (ZESize I = 0; I < Objects.GetCount(); I++)
	{
		if (!Selection.Exists(Objects[I]))
			continue;

		Objects[I]->SetSelected(false);
		UnselectedObjects.Add(Objects[I]);
		Selection.Remove(I);
	}

	ZEDSelectionEvent Event;
	Event.Type = ZED_SET_DESELECTED;
	Event.Selection = &Selection;
	Event.OldSelection = &OldSelection;
	Event.SelectedObjects = &SelectedObjects;
	Event.UnselectedObjects = &UnselectedObjects;

	GetModule()->SelectionEvent(Event);
}

void ZEDSelectionManager::ClearSelection()
{
	ZEArray<ZEDObjectWrapper*> OldSelection = Selection;
	ZEArray<ZEDObjectWrapper*> SelectedObjects;

	for (ZESize I = 0; I < Selection.GetCount(); I++)
		Selection[I]->SetSelected(false);

	Selection.Clear();

	ZEDSelectionEvent Event;
	Event.Type = ZED_SET_DESELECTED;
	Event.Selection = &Selection;
	Event.OldSelection = &OldSelection;
	Event.SelectedObjects = &SelectedObjects;
	Event.UnselectedObjects = &OldSelection;

	GetModule()->SelectionEvent(Event);
}
/*
void ZEDSelectionManager::DeselectObject(const ZERay& Ray)
{
	ZERayCastParameters Parameters;
	Parameters.Ray = Ray;
	Parameters.FilterFunction = ZERayCastFilterFunction::Create<ZEDSelectionManager, &ZEDSelectionManager::FilterSelection>(this);
	Parameters.FilterFunctionParameter = Filter;
	ZERayCastReport Report;

	ZEDSceneWrapper* SceneWrapper = static_cast<ZEDSceneWrapper*>(GetModule()->GetRootWrapper());
	if (!SceneWrapper->RayCast(Report, Parameters))
		return;

	DeselectObject(Report.Object);
}

void ZEDSelectionManager::DeselectObject(ZEViewVolume* ViewVolume)
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
}
*/

void ZEDSelectionManager::SetSelectionFilter(ZEClass* Class)
{
	Filter = Class;
}

ZEClass* ZEDSelectionManager::GetSelectionFilter()
{
	return Filter;
}

bool ZEDSelectionManager::FilterSelection(ZEObject* Object, void* Class)
{
	return ZEClass::IsDerivedFrom((ZEClass*)Class, Object->GetClass());
}

void ZEDSelectionManager::SetSelectionPivotMode(ZEDSelectionPivotMode Mode)
{
	PivotMode = Mode;

	CalculateSelectionPivot();
}

ZEDSelectionPivotMode ZEDSelectionManager::GetSelectionPivotMode()
{
	return PivotMode;
}

const ZEMatrix4x4& ZEDSelectionManager::GetSelectionPivot()
{
	CalculateSelectionPivot();

	return SelectionPivot;
}

bool ZEDSelectionManager::KeyboardEvent(const ZEDViewportKeyboardEvent& Event)
{
	return false;
}

bool ZEDSelectionManager::MouseEvent(const ZEDViewportMouseEvent& Event)
{
	if (Event.GetButton() != ZED_MB_LEFT)
		return false;

	if (Event.GetType() == ZED_ET_BUTTON_PRESSED)
	{
		SelectionStartPosition = Event.GetPosition();

		// Single Selection	
		ZERayCastParameters Parameters;
		Parameters.Ray = ZERNScreenUtilities::ScreenToWorld(Event.GetViewport()->GetView(), Event.GetPosition());

		ZERayCastReport Report;
		Report.SetParameters(&Parameters);

		GetModule()->GetRootWrapper()->RayCast(Report, Parameters);	
		if (Report.GetResult())
		{
			if ((Event.GetModifiers() & ZED_KKM_CTRL) != 0 || (Event.GetModifiers() & ZED_KKM_SHIFT) != 0)
			{
				SelectObject(static_cast<ZEEntity*>(Report.GetCollision().Object)->GetWrapper());
			}
			else if (Event.GetModifiers() & ZED_KKM_ALT)
			{
				DeselectObject(static_cast<ZEEntity*>(Report.GetCollision().Object)->GetWrapper());
			}
			else
			{
				ClearSelection();
				SelectObject(static_cast<ZEEntity*>(Report.GetCollision().Object)->GetWrapper());
			}
		}
		else
		{
			ClearSelection();
		}

		return true;
	}
	else if (Event.GetType() == ZED_ET_BUTTON_PRESSED)
	{
		if (SelectionStartPosition - Event.GetPosition() == ZEVector2::Zero)
		{
		}
	}


	return false;
}

void ZEDSelectionManager::Destroy()
{
	delete this;
}


ZEDSelectionManager* ZEDSelectionManager::GetInstance()
{
	return ZEDCore::GetInstance()->GetSelectionManager();
}

ZEDSelectionManager::ZEDSelectionManager()
{
	Module = NULL;
	Filter = ZEDObjectWrapper::Class();
	PivotMode = ZED_SCM_SELECTION_CENTER;
	SelectionPivot = ZEMatrix4x4::Zero;
}
