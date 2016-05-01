//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDTransformationManager.cpp
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

#include "ZEDTransformationManager.h"
#include "ZEDCore.h"
#include "ZEDGizmo.h"
#include "ZEDTransformationOperation.h"
#include "ZEDObjectWrapper.h"
#include "ZEDSelectionManager.h"
#include "ZEDOperationManager.h"
#include "ZERenderer\ZERNScreenUtilities.h"
#include "ZEDModule.h"
#include "ZEDViewportInput.h"
#include "ZEDViewport.h"

ZEDTransformationManager::ZEDTransformationManager()
{
	Module = NULL;
	Gizmo = ZEDGizmo::CreateInstance();
	TransformType = ZED_TT_NONE;
	TransformSpace = ZED_TS_WORLD;
	Transform = ZEMatrix4x4::Identity;
}

ZEDModule* ZEDTransformationManager::GetModule()
{
	return Module;
}

void ZEDTransformationManager::SetTransformType(ZEDTransformType Type)
{
	TransformType = Type;
}

ZEDTransformType ZEDTransformationManager::GetTransformType()
{
	return TransformType;
}

void ZEDTransformationManager::SetTransformSpace(ZEDTransformSpace Space)
{
	TransformSpace = Space;
}

ZEDTransformSpace ZEDTransformationManager::GetTransformSpace()
{
	return TransformSpace;
}

ZEDGizmo* ZEDTransformationManager::GetGizmo()
{
	return Gizmo;
}

void ZEDTransformationManager::BeginTransform(ZEDTransformType Type)
{
	if (Type == ZED_TT_NONE)
		return;

	if (TransformType != ZED_TT_NONE)
		return;

	SetTransformType(Type);
}

void ZEDTransformationManager::ResetTransform()
{
	if (TransformType == ZED_TT_NONE)
		return;

	const ZEArray<ZEDObjectWrapper*>& Selection = ZEDSelectionManager::GetInstance()->GetSelectedObjects();

	for (ZESize I = 0; I < Selection.GetCount(); I++)
	{
		switch (TransformType)
		{
			case ZED_TT_TRANSLATE:
				Selection[I]->SetPosition(Selection[I]->GetPosition() + (Transform.Inverse()).GetTranslation());
				break;
			case ZED_TT_ROTATE:
			{
				ZEQuaternion Temp;
				ZEQuaternion::CreateFromMatrix(Temp, Transform);
				Selection[I]->SetRotation(Temp.Conjugate() * Selection[I]->GetRotation());
				break;
			}
			case ZED_TT_SCALE:
				Selection[I]->SetScale(Selection[I]->GetScale() * (Transform.Inverse()).GetScale());
				break;
		}
	}
}

void ZEDTransformationManager::ApplyTransform(ZEMatrix4x4 Transform)
{
	if (TransformType == ZED_TT_NONE)
		return;

	ZEDSelectionManager* SelectionManager = ZEDSelectionManager::GetInstance();
	const ZEArray<ZEDObjectWrapper*>& Selection = SelectionManager->GetSelectedObjects();
	ZEDSelectionPivotMode PivotMode = SelectionManager->GetSelectionPivotMode();

	if (TransformType == ZED_TT_TRANSLATE)
	{
		for (ZESize I = 0; I < Selection.GetCount(); I++)
			Selection[I]->SetPosition(Selection[I]->GetPosition() + Transform.GetTranslation());
	}
	else if (TransformType == ZED_TT_ROTATE)
	{
		if (SelectionManager->GetSelectionPivotMode() == ZED_SCM_ENTITY_PIVOT)
		{
			for (ZESize I = 0; I < Selection.GetCount(); I++)
				Selection[I]->SetRotation(Transform.GetRotation() * Selection[I]->GetRotation());
		}
		else if (SelectionManager->GetSelectionPivotMode() == ZED_SCM_SELECTION_CENTER)
		{
			ZEMatrix4x4 ResultTransform;
			ZEMatrix4x4 PivotTransform = SelectionManager->GetSelectionPivot();
			ZEQuaternion Rotation = Transform.GetRotation();

			for (ZESize I = 0; I < Selection.GetCount(); I++)
			{
				ZEVector3 ObjectPositionInPivotSpace = PivotTransform.Inverse() * Selection[I]->GetPosition();
				ZEVector3 RotatedPositionInPivotSpace = Rotation * ObjectPositionInPivotSpace;
				Selection[I]->SetPosition(PivotTransform * RotatedPositionInPivotSpace);
				Selection[I]->SetRotation(Rotation * Selection[I]->GetRotation());
			}
		}
		else if (SelectionManager->GetSelectionPivotMode() == ZED_SCM_SPACE_CENTER)
		{
			ZEQuaternion Rotation = Transform.GetRotation();

			for (ZESize I = 0; I < Selection.GetCount(); I++)
			{
				Selection[I]->SetPosition(Rotation * Selection[I]->GetPosition());
				Selection[I]->SetRotation(Rotation * Selection[I]->GetRotation());
			}
		}
		else
		{
			return;
		}
	}
	else if (TransformType == ZED_TT_SCALE)
	{
		if (SelectionManager->GetSelectionPivotMode() == ZED_SCM_ENTITY_PIVOT)
		{
			for (ZESize I = 0; I < Selection.GetCount(); I++)
				Selection[I]->SetScale(Transform.GetScale() * Selection[I]->GetScale());
		}
		else if (SelectionManager->GetSelectionPivotMode() == ZED_SCM_SELECTION_CENTER)
		{
			ZEMatrix4x4 ResultTransform;
			ZEMatrix4x4 PivotTransform = SelectionManager->GetSelectionPivot();
			ZEVector3 Scale = Transform.GetScale();

			for (ZESize I = 0; I < Selection.GetCount(); I++)
			{
				ZEVector3 ObjectPositionInPivotSpace = PivotTransform.Inverse() * Selection[I]->GetPosition();
				ZEVector3 ScaledPositionInPivotSpace = Scale * ObjectPositionInPivotSpace;
				Selection[I]->SetPosition(PivotTransform * ScaledPositionInPivotSpace);
				Selection[I]->SetScale(Scale * Selection[I]->GetScale());
			}

		}
		else if (SelectionManager->GetSelectionPivotMode() == ZED_SCM_SPACE_CENTER)
		{
			ZEVector3 Scale = Transform.GetScale();

			for (ZESize I = 0; I < Selection.GetCount(); I++)
			{
				Selection[I]->SetPosition(Scale * Selection[I]->GetPosition());
				Selection[I]->SetScale(Scale * Selection[I]->GetScale());
			}
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}


	 ZEMatrix4x4 CurrentTransform = this->Transform;
	 ZEMatrix4x4::Multiply(this->Transform, Transform, CurrentTransform);
}

void ZEDTransformationManager::EndTransform()
{
	ZEDSelectionManager* SelectionManager = ZEDSelectionManager::GetInstance();
	ZEDTransformationOperation* Operation = new ZEDTransformationOperation(TransformType, SelectionManager->GetSelectionPivotMode(), Transform, SelectionManager->GetSelectedObjects());
	Operation->SetApplyEnabled(false);
	ZEDOperationManager::GetInstance()->DoOperation(Operation);
	Operation->SetApplyEnabled(true);

	TransformType = ZED_TT_NONE;
	Transform = ZEMatrix4x4::Identity;
}

bool ZEDTransformationManager::KeyboardEventHandler(const ZEDViewportKeyboardEvent& Event)
{
	return false;
}

bool ZEDTransformationManager::MouseEventHandler(const ZEDViewportMouseEvent& Event)
{
	if (!Gizmo->GetVisible() || Gizmo->GetMode() == ZED_GM_NONE)
		return false;

	if (ZEDSelectionManager::GetInstance()->GetSelectedObjects().GetCount() == 0)
		return false;

	const ZERNView& View = Event.Viewport->GetView();
	ZERay Ray = ZERNScreenUtilities::ScreenToWorld(Event.Viewport->GetView(), Event.Position);
	float TRay = FLT_MAX;

	if (Event.Button == ZED_MB_NONE)
	{
		// Axis Highlight
		ZEDGizmoAxis Axis = Gizmo->PickAxis(View, Ray, TRay);
		Gizmo->SetHoveredAxis(Axis);
		return false;
	}
	else if (Event.Button != ZED_MB_LEFT && Event.Type == ZED_ET_BUTTON_PRESSED)
	{
		// Transformation Start
		ZEDGizmoAxis Axis = Gizmo->PickAxis(View, Ray, TRay);
		Gizmo->SetSelectedAxis(Axis);

		if (Gizmo->GetSelectedAxis() == ZED_GA_NONE)
			return false;

		switch (Gizmo->GetMode())
		{
			case ZED_GM_MOVE:
				Gizmo->StartMoveProjection(View, Ray);
				return true;

			case ZED_GM_ROTATE:
				Gizmo->StartRotationProjection(View, Ray);
				return true;

			case ZED_GM_SCALE:
				Gizmo->StartScaleProjection(View, Ray);
				return true;

			default:
				return false;
		}
	}
	else if (Event.Type == ZED_ET_BUTTON_PRESSING)
	{

	}
	else if (Event.Type == ZED_ET_BUTTON_RELEASED)
	{
		// End	
		ZEMatrix4x4 NewTransform = ZEMatrix4x4::Identity;
		if (Gizmo->GetMode() == ZED_GM_MOVE)
		{
			if (ZEDTransformationManager::GetInstance()->GetTransformType() == ZED_TT_NONE)
				ZEDTransformationManager::GetInstance()->BeginTransform(ZED_TT_TRANSLATE);

			ZEVector3 NewTranslation = (Gizmo->MoveProjection(View, Ray) - Gizmo->GetPosition());
			ZEMatrix4x4::CreateTranslation(NewTransform, NewTranslation);
			ZEDTransformationManager::GetInstance()->ApplyTransform(NewTransform);
			Gizmo->SetPosition(Gizmo->GetPosition() + NewTranslation);
			return true;
		}
		else if (Gizmo->GetMode() == ZED_GM_ROTATE)
		{
			if (ZEDTransformationManager::GetInstance()->GetTransformType() == ZED_TT_NONE)
				ZEDTransformationManager::GetInstance()->BeginTransform(ZED_TT_ROTATE);

			ZEQuaternion NewOrientation = Gizmo->RotationProjection(View, Ray);
			ZEMatrix4x4::CreateRotation(NewTransform, NewOrientation);
			ZEDTransformationManager::GetInstance()->ApplyTransform(NewTransform);
			return true;
		}
		else if (Gizmo->GetMode() == ZED_GM_SCALE)
		{
			if (ZEDTransformationManager::GetInstance()->GetTransformType() == ZED_TT_NONE)
				ZEDTransformationManager::GetInstance()->BeginTransform(ZED_TT_SCALE);

			ZEVector3 NewScale = Gizmo->ScaleProjection(View, Ray);
			ZEMatrix4x4::CreateScale(NewTransform, NewScale);
			ZEDTransformationManager::GetInstance()->ApplyTransform(NewTransform);

			return true;
		}
	}
}

void ZEDTransformationManager::Destroy()
{
	if (Gizmo != NULL)
	{
		Gizmo->Destroy();
	}

	delete this;
}

ZEDTransformationManager* ZEDTransformationManager::GetInstance()
{
	return ZEDCore::GetInstance()->GetTransformationManager();
}
