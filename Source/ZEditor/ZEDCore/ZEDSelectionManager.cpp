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
#include "ZEDCore.h"
#include "ZEDTransformationManager.h"
#include "ZEDModule.h"
#include "ZEDObjectWrapper.h"
#include "ZEDScene.h"
#include "ZEFoundation/ZEMath/ZEViewVolume.h"
#include "ZEGame/ZEEntity.h"
#include "ZEGraphics/ZECamera.h"

void ZEDSelectionManager::CalculateSelectionPivot()
{
	if (Selection.GetCount() == 0)
		return;

	if (!DirtyPivot)
		return;

	ZEVector3 ObjectCenter;
	ZEVector3 SelectionCenterPosition = ZEVector3::Zero;
	ZEQuaternion SelectionCenterRotation = ZEQuaternion::Identity;
	ZEDTransformSpace TargetSpace = ZEDTransformationManager::GetInstance()->GetTransformSpace();

	if (PivotMode == ZED_SCM_ENTITY_PIVOT)
	{
		SelectionCenterPosition = Selection.GetFirstItem()->GetWorldPosition(); //GetPivotPosition

		if (TargetSpace == ZED_TS_LOCAL)
			SelectionCenterRotation = Selection.GetFirstItem()->GetWorldRotation(); //GetPivotRotation
	}
	else if (PivotMode == ZED_SCM_SELECTION_CENTER)
	{
		if (TargetSpace == ZED_TS_LOCAL)
		{
			SelectionCenterPosition = Selection.GetFirstItem()->GetObjectBoundingBox().Center;
			SelectionCenterRotation = Selection.GetFirstItem()->GetWorldRotation(); //GetPivotRotation ? BBox rotation?
		}
		else if (TargetSpace == ZED_TS_WORLD)
		{
			for (ZESize I = 0; I < Selection.GetCount(); I++)
			{
				ObjectCenter = Selection[I]->GetObjectBoundingBox().Center;
				SelectionCenterPosition += ObjectCenter;
			}

			SelectionCenterPosition /= Selection.GetCount();
		}
	}
	else if (PivotMode == ZED_SCM_SPACE_CENTER)
	{
		if (TargetSpace == ZED_TS_LOCAL)
		{
			SelectionCenterPosition = Selection.GetFirstItem()->GetWorldPosition(); //GetPivotPosition
			SelectionCenterRotation = Selection.GetFirstItem()->GetWorldRotation(); //GetPivotRotation
		}
		else if (TargetSpace == ZED_TS_WORLD)
		{
			SelectionCenterPosition = ZEVector3::Zero;
			SelectionCenterRotation = ZEQuaternion::Identity;
		}
	}

	ZEMatrix4x4::CreateOrientation(SelectionPivot, SelectionCenterPosition, SelectionCenterRotation, ZEVector3::One);
	DirtyPivot = false;
}

const ZEArray<ZEDObjectWrapper*>& ZEDSelectionManager::GetSelectedObjects()
{
	return Selection;
}

void ZEDSelectionManager::SelectObject(ZEDObjectWrapper* Object)
{
	if (Object == NULL)
		return;

	if (Filter != NULL)
		if (!ZEClass::IsDerivedFrom(Filter, Object->GetObject()->GetClass()))
			return;

	Selection.Add(Object);

	DirtyPivot = true;
}

void ZEDSelectionManager::SelectObject(const ZERay& Ray)
{
	ZERayCastParameters Parameters;
	Parameters.Ray = Ray;
	Parameters.FilterFunction = ZERayCastFilterFunction::Create<ZEDSelectionManager, &ZEDSelectionManager::FilterSelection>(this);
	Parameters.FilterFunctionParameter = Filter;
	ZERayCastReport Report;

	if (!ZEDCore::GetInstance()->GetEditorModule()->GetScene()->RayCast(Report, Parameters))
		return;

	SelectObject((ZEDObjectWrapper*)Report.Entity);
}

void ZEDSelectionManager::SelectObject(ZEViewVolume* ViewVolume)
{
	if (ViewVolume == NULL)
		return;
	
	if (ViewVolume->GetViewVolumeType() == ZE_VVT_NONE)
		return;

	ZEArray<ZEEntity*> Wrappers = ZEDCore::GetInstance()->GetEditorModule()->GetScene()->GetEntities(Filter);
	
	ZEDObjectWrapper* TempWrapper = NULL;

	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{
		TempWrapper = (ZEDObjectWrapper*)Wrappers[I];

		if (!ViewVolume->CullTest(TempWrapper->GetWorldBoundingBox()))
			SelectObject(TempWrapper);
	}
}

void ZEDSelectionManager::SelectObject(const ZEVector2& ScreenPoint1, const ZEVector2& ScreenPoint2)
{
	ZECamera* ActiveCamera = ZEDCore::GetInstance()->GetEditorModule()->GetScene()->GetActiveCamera();

	if (ActiveCamera == NULL)
		return;

	if (ScreenPoint1.Equals(ScreenPoint2))
	{
		ZERay Ray;
		ActiveCamera->GetScreenRay(Ray, ScreenPoint1.x, ScreenPoint1.y);
		SelectObject(Ray);
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

	SelectObject(&SelectionFrustum);
}

void ZEDSelectionManager::SelectObject(const ZEString& Name)
{
	if (Name.IsEmpty())
		return;

	ZEArray<ZEEntity*> Wrappers = ZEDCore::GetInstance()->GetEditorModule()->GetScene()->GetEntities(Filter);

	ZEDObjectWrapper* TempWrapper = NULL;

	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{
		TempWrapper = (ZEDObjectWrapper*)Wrappers[I];

		if (TempWrapper->GetName() == Name)
			SelectObject(TempWrapper);
	}
}

void ZEDSelectionManager::DeselectObject(ZEDObjectWrapper* Object)
{
	if (Object = NULL)
		return;

	if (!Selection.Exists(Object))
		return;

	Selection.RemoveValue(Object);

	DirtyPivot = true;
}

void ZEDSelectionManager::DeselectObject(const ZERay& Ray)
{
	ZERayCastParameters Parameters;
	Parameters.Ray = Ray;
	Parameters.FilterFunction = ZERayCastFilterFunction::Create<ZEDSelectionManager, &ZEDSelectionManager::FilterSelection>(this);
	Parameters.FilterFunctionParameter = Filter;
	ZERayCastReport Report;

	if (!ZEDCore::GetInstance()->GetEditorModule()->GetScene()->RayCast(Report, Parameters))
		return;

	DeselectObject((ZEDObjectWrapper*)Report.Entity);
}

void ZEDSelectionManager::DeselectObject(ZEViewVolume* ViewVolume)
{
	if (ViewVolume == NULL)
		return;

	if (ViewVolume->GetViewVolumeType() == ZE_VVT_NONE)
		return;

	ZEArray<ZEEntity*> Wrappers = ZEDCore::GetInstance()->GetEditorModule()->GetScene()->GetEntities(Filter);

	ZEDObjectWrapper* TempWrapper = NULL;

	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{
		TempWrapper = (ZEDObjectWrapper*)Wrappers[I];

		if (!ViewVolume->CullTest(TempWrapper->GetWorldBoundingBox()))
			DeselectObject(TempWrapper);
	}
}

void ZEDSelectionManager::DeselectObject(const ZEVector2& ScreenPoint1, const ZEVector2& ScreenPoint2)
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

void ZEDSelectionManager::DeselectObject(const ZEString& Name)
{
	if (Name.IsEmpty())
		return;

	ZEArray<ZEEntity*> Wrappers = ZEDCore::GetInstance()->GetEditorModule()->GetScene()->GetEntities(Filter);

	ZEDObjectWrapper* TempWrapper = NULL;

	for (ZESize I = 0; I < Wrappers.GetCount(); I++)
	{
		TempWrapper = (ZEDObjectWrapper*)Wrappers[I];

		if (TempWrapper->GetName() == Name)
			DeselectObject(TempWrapper);
	}
}

void ZEDSelectionManager::ClearSelection()
{
	Selection.Clear(false);
	SelectionPivot = ZEMatrix4x4::Zero;
	DirtyPivot = true;
}

void ZEDSelectionManager::SetSelectionFilter(ZEClass* Class)
{
	Filter = Class;
}

ZEClass* ZEDSelectionManager::GetSelectionFilter()
{
	return Filter;
}

bool ZEDSelectionManager::FilterSelection(ZEEntity* Entity, void* Class)
{
	return ZEClass::IsDerivedFrom(Entity->GetClass(), (ZEClass*)Class);
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

ZEMatrix4x4 ZEDSelectionManager::GetSelectionPivot()
{
	CalculateSelectionPivot();

	return SelectionPivot;
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
	Filter = NULL;
	PivotMode = ZED_SCM_SELECTION_CENTER;
	SelectionPivot = ZEMatrix4x4::Zero;
	DirtyPivot = true;
}
