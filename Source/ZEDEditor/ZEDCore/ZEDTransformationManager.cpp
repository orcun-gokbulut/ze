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

#include "ZEError.h"
#include "ZEMath\ZEAngle.h"
#include "ZEMath\ZEMath.h"
#include "ZEDEditor.h"
#include "ZEDGizmo.h"
#include "ZEDInputDefinitions.h"
#include "ZEDObjectWrapper.h"
#include "ZEDObjectWrapper3D.h"
#include "ZEDOperationManager.h"
#include "ZEDSelectionManager.h"
#include "ZEDViewport.h"
#include "ZEDViewportManager.h"
#include "ZEDTransformationOperation.h"
#include "ZEDSelectionManager.h"
#include "ZEDObjectManager.h"
#include "ZEDUserInterface/ZEDCommandManager.h"
#include "ZERenderer/ZERNScreenUtilities.h"
#include "ZERenderer/ZERNRenderParameters.h"


// ZEDTransformationState
//////////////////////////////////////////////////////////////////////////////////////

ZEDTransformationState::ZEDTransformationState()
{
	Wrapper = NULL;
	Pivot = ZEVector3::Zero;
	OriginalPosition = ZEVector3::Zero;
	OriginalRotation = ZEQuaternion::Identity;
	OriginalScale = ZEVector3::Zero;
	Gizmo = NULL;
}

ZEDTransformationState::~ZEDTransformationState()
{
	if (Gizmo != NULL)
	{
		Gizmo->Destroy();
		Gizmo = NULL;
	}
}


// ZEDTransformationManager
//////////////////////////////////////////////////////////////////////////////////////

void ZEDTransformationManager::UpdateGizmo(ZEDGizmo* Gizmo, const ZEVector3& WorldPosition, const ZEQuaternion& WorldRotation)
{
	if (Gizmo == NULL)
		return;

	Gizmo->Initialize();
	Gizmo->SetPosition(WorldPosition);	

	switch (TransformSpace)
	{
		default:
		case ZED_TS_WORLD:
			Gizmo->SetRotation(ZEQuaternion::Identity);
			break;

		case ZED_TS_LOCAL:
			Gizmo->SetRotation(WorldRotation);
			break;

		case ZED_TS_PARENT:
			Gizmo->SetRotation(WorldRotation);
			break;

		case ZED_TS_VIEW:
			Gizmo->SetRotation(ZEQuaternion::Identity);
			break;
	}

	switch (TransformType)
	{
		case ZED_TT_NONE:
			Gizmo->SetMode(ZED_GM_HELPER);
			break;

		case ZED_TT_TRANSLATE:
			Gizmo->SetMode(ZED_GM_MOVE);
			break;

		case ZED_TT_ROTATE:
			Gizmo->SetMode(ZED_GM_ROTATE);
			break;

		case ZED_TT_SCALE:
			Gizmo->SetMode(ZED_GM_SCALE);
			break;

		default:
			Gizmo->SetMode(ZED_GM_NONE);
			break;
	}
}

void ZEDTransformationManager::UpdateGizmos()
{
	if (TransformStates.GetCount() == 0)
		return;

	if (GetTransformPivot() == ZED_TP_OBJECT || GetTransformPivot() == ZED_TP_FOCUSED_OBJECT)
	{
		for (ZESize I = 0; I < TransformStates.GetCount(); I++)
		{
			ZEDObjectWrapper3D* Wrapper = TransformStates[I].Wrapper;

			if (GetTransformSpace() == ZED_TS_PARENT && Wrapper->GetParent() != NULL)
				UpdateGizmo(TransformStates[I].Gizmo, Wrapper->GetParent3D()->GetPosition(), Wrapper->GetParent3D()->GetRotation());
			else
				UpdateGizmo(TransformStates[I].Gizmo, Wrapper->GetPosition(), Wrapper->GetRotation());
		}
	}
	else if (GetTransformPivot() == ZED_TP_SELECTION_CENTER)
	{
		ZEVector3 Pivot = ZEVector3::Zero;
		for (ZESize I = 0; I < TransformStates.GetCount(); I++)
			Pivot += TransformStates[I].Wrapper->GetPosition();

		Pivot /= TransformStates.GetCount();

		UpdateGizmo(TransformStates[0].Gizmo, Pivot, ZEQuaternion::Identity);
	}
	else if (GetTransformPivot() == ZED_TP_WORLD)
	{
		UpdateGizmo(TransformStates[0].Gizmo, ZEVector3::Zero, ZEQuaternion::Identity);
	}
}

void ZEDTransformationManager::UpdateTransformStates()
{
	const ZEArray<ZEDObjectWrapper*>& Selection = GetEditor()->GetSelectionManager()->GetSelection();

	TransformStates.Clear();
	TransformFocused = NULL;

	ZESize TransformableCount = 0;

	ze_for_each(Item, Selection)
	{
		zeDebugCheck(!ZEClass::IsDerivedFrom(ZEDObjectWrapper3D::Class(), Item.GetItem()->GetClass()), "Wrapper is not derived from ZEDObjectWrapper3D");

		ZEDObjectWrapper3D* CurrentWrapper = static_cast<ZEDObjectWrapper3D*>(Item.GetItem());

		if (CurrentWrapper->GetTransformable())
			TransformableCount++;
	}

	TransformStates.SetCount(TransformableCount);

	if (TransformStates.GetCount() != 0)
	{
		ZESize TransformStateIndex = 0;

		for (ZESize I = 0; I < Selection.GetCount(); I++)
		{
			zeDebugCheck(!ZEClass::IsDerivedFrom(ZEDObjectWrapper3D::Class(), Selection[I]->GetClass()), "Wrapper is not derived from ZEDObjectWrapper3D");

			ZEDObjectWrapper3D* CurrentWrapper = static_cast<ZEDObjectWrapper3D*>(Selection[I]);

			if (!CurrentWrapper->GetTransformable())
				continue;

			TransformStates[TransformStateIndex].Wrapper = static_cast<ZEDObjectWrapper3D*>(Selection[I]);
			TransformStates[TransformStateIndex].OriginalPosition = TransformStates[TransformStateIndex].Wrapper->GetPosition();
			TransformStates[TransformStateIndex].OriginalRotation = TransformStates[TransformStateIndex].Wrapper->GetRotation();
			TransformStates[TransformStateIndex].OriginalScale = TransformStates[TransformStateIndex].Wrapper->GetScale();

			if (TransformStates[TransformStateIndex].Wrapper->GetFocused())
				TransformFocused = &TransformStates[TransformStateIndex];

			TransformStateIndex++;
		}

		if (TransformPivot == ZED_TP_OBJECT)
		{
			for (ZESize I = 0; I < TransformStates.GetCount(); I++)
			{
				TransformStates[I].Pivot = TransformStates[I].Wrapper->GetPosition();
				TransformStates[I].Gizmo = ZEDGizmo::CreateInstance();
				TransformStates[I].Gizmo->Initialize();
			}
		}
		else if (TransformPivot == ZED_TP_FOCUSED_OBJECT)
		{
			if (TransformFocused != NULL)
			{
				TransformFocused->Gizmo = ZEDGizmo::CreateInstance();
				TransformFocused->Gizmo->Initialize();
				for (ZESize I = 0; I < TransformStates.GetCount(); I++)
					TransformStates[I].Pivot = TransformFocused->Wrapper->GetPosition();
			}
		}
		else if (TransformPivot == ZED_TP_SELECTION_CENTER)
		{
			TransformStates.GetFirstItem().Gizmo = ZEDGizmo::CreateInstance();
			TransformStates.GetFirstItem().Gizmo->Initialize();

			ZEVector3 Pivot = ZEVector3::Zero;
			for (ZESize I = 0; I < TransformStates.GetCount(); I++)
				Pivot += TransformStates[I].OriginalPosition;

			Pivot /= TransformStates.GetCount();

			for (ZESize I = 0; I < TransformStates.GetCount(); I++)
				TransformStates[I].Pivot = Pivot;
		}
		else if (TransformPivot == ZED_TP_WORLD)
		{
			TransformStates.GetFirstItem().Gizmo = ZEDGizmo::CreateInstance();
			for (ZESize I = 0; I < TransformStates.GetCount(); I++)
				TransformStates[I].Pivot = ZEVector3::Zero;
		}
	}

	bool Valid;
	PivotPosition = GetPosition(Valid);

	UpdateGizmos();
	UpdateCommands();
}

bool ZEDTransformationManager::InitializeInternal()
{
	if (!ZEDComponent::InitializeInternal())
		return false;

	GetEditor()->GetObjectManager()->OnObjectPropertyChanged.AddDelegate<ZEDTransformationManager, &ZEDTransformationManager::ObjectManager_OnObjectPropertyChanged>(this);
	GetEditor()->GetSelectionManager()->OnSelectionChanged.AddDelegate<ZEDTransformationManager, &ZEDTransformationManager::SelectionManager_OnSelectionChanged>(this);
	GetEditor()->GetViewportManager()->OnViewportKeyboardKeyPressed.AddDelegate<ZEDTransformationManager, &ZEDTransformationManager::Viewport_OnKeyboardKeyPressed>(this);
	GetEditor()->GetViewportManager()->OnViewportMouseButtonPressed.AddDelegate<ZEDTransformationManager, &ZEDTransformationManager::Viewport_OnMouseButtonPressed>(this);
	GetEditor()->GetViewportManager()->OnViewportMouseButtonReleased.AddDelegate<ZEDTransformationManager, &ZEDTransformationManager::Viewport_OnMouseButtonRelease>(this);
	GetEditor()->GetViewportManager()->OnViewportMouseMoved.AddDelegate<ZEDTransformationManager, &ZEDTransformationManager::Viewport_OnMouseMoved>(this);
	GetEditor()->GetViewportManager()->OnViewportPreRender.AddDelegate<ZEDTransformationManager, &ZEDTransformationManager::Viewport_OnPreRender>(this);

	RegisterCommands();

	return true;
}

bool ZEDTransformationManager::DeinitializeInternal()
{
	ResetTransform();
	TransformStates.Clear();
	TransformFocused = NULL;
	TransformGizmo = NULL;

	GetEditor()->GetObjectManager()->OnObjectPropertyChanged.DisconnectObject(this);
	GetEditor()->GetSelectionManager()->OnSelectionChanged.DisconnectObject(this);
	GetEditor()->GetViewportManager()->OnViewportViewChangedEvent.DisconnectObject(this);
	GetEditor()->GetViewportManager()->OnViewportKeyboardKeyPressed.DisconnectObject(this);
	GetEditor()->GetViewportManager()->OnViewportMouseButtonPressed.DisconnectObject(this);
	GetEditor()->GetViewportManager()->OnViewportMouseButtonReleased.DisconnectObject(this);
	GetEditor()->GetViewportManager()->OnViewportMouseMoved.DisconnectObject(this);
	GetEditor()->GetViewportManager()->OnViewportPreRender.DisconnectObject(this);

	return ZEDComponent::DeinitializeInternal();
}

void ZEDTransformationManager::StartTransform(ZEDGizmo* TransformGizmo)
{
	TransformActive = true;
	this->TransformGizmo = TransformGizmo;

	OnTransformationStarted(this);

	UpdateCommands();
}

void ZEDTransformationManager::EndTransform()
{
	if (!TransformActive)
		return;

	ZEDTransformationOperation* Operation = ZEDTransformationOperation::Create(TransformType, TransformStates);
	GetEditor()->GetOperationManager()->Do(Operation);

	for (ZESize I = 0; I < TransformStates.GetCount(); I++)
	{
		ZEDTransformationState& TransformState = TransformStates[I];
		TransformState.OriginalPosition = TransformState.Wrapper->GetPosition();
		TransformState.OriginalRotation = TransformState.Wrapper->GetRotation();
		TransformState.OriginalScale = TransformState.Wrapper->GetScale();
	}

	if (TransformGizmo != NULL)
	{
		TransformGizmo->SetSelectedAxis(ZED_GA_NONE);
		TransformGizmo = NULL;
	}

	TransformActive = false;

	UpdateTransformStates();

	OnTransformationEnded(this);

	UpdateCommands();
}

void ZEDTransformationManager::ResetTransform()
{
	if (!TransformActive)
		return;

	for (ZESize I = 0; I < TransformStates.GetCount(); I++)
	{
		ZEDTransformationState& TransformState = TransformStates[I];
		switch (TransformType)
		{
			case ZED_TT_TRANSLATE:
				TransformState.Wrapper->SetPosition(TransformState.OriginalPosition);
				break;

			case ZED_TT_ROTATE:
				TransformState.Wrapper->SetRotation(TransformState.OriginalRotation);
				break;

			case ZED_TT_SCALE:
				TransformState.Wrapper->SetScale(TransformState.OriginalScale);
				break;
		}
	}

	TransformGizmo = NULL;
	TransformActive = false;

	UpdateTransformStates();

	OnTransformationReset(this);

	UpdateCommands();
}

void ZEDTransformationManager::ApplyTranslation(const ZEVector3& Translation)
{
	if (!TransformActive)
		return;

	if (TransformType != ZED_TT_TRANSLATE)
		return;

	for (ZESize I = 0; I < TransformStates.GetCount(); I++)
	{
		ZEDTransformationState& TransformState = TransformStates[I];
		TransformState.Wrapper->SetPosition(TransformState.OriginalPosition + Translation);
	}

	UpdateGizmos();

	OnTransforming(this);

	UpdateCommands();
}

void ZEDTransformationManager::ApplyRotation(const ZEQuaternion& Rotation)
{
	if (!TransformActive)
		return;

	if (TransformType != ZED_TT_ROTATE)
		return;

	for (ZESize I = 0; I < TransformStates.GetCount(); I++)
	{
		ZEDTransformationState& TransformState = TransformStates[I];

		ZEVector3 Position;
		ZEQuaternion::VectorProduct(Position, Rotation, TransformState.OriginalPosition - TransformState.Pivot);
		Position += TransformState.Pivot;
		TransformState.Wrapper->SetPosition(Position);
		TransformState.Wrapper->SetRotation(Rotation * TransformState.OriginalRotation);
	}

	UpdateGizmos();

	OnTransforming(this);

	UpdateCommands();
}

void ZEDTransformationManager::ApplyScale(const ZEVector3& Scale)
{
	if (!TransformActive)
		return;

	if (TransformType != ZED_TT_SCALE)
		return;

	for (ZESize I = 0; I < TransformStates.GetCount(); I++)
	{
		ZEDTransformationState& TransformState = TransformStates[I];

		ZEVector3 Position;
		ZEVector3::Multiply(Position, Scale, TransformState.OriginalPosition - TransformState.Pivot);
		Position += TransformState.Pivot;
		TransformState.Wrapper->SetPosition(Position);
		TransformState.Wrapper->SetScale(Scale * TransformState.OriginalScale);
	}

	OnTransforming(this);

	UpdateCommands();
}

ZEVector3 ZEDTransformationManager::GetPosition(bool& Valid)
{
	Valid = true;

	ZEVector3 PivotPosition = ZEVector3::Zero;
	ZEDTransformationState* TargetObject = NULL;
	switch (TransformPivot)
	{
		case ZED_TP_OBJECT:
			if (TransformStates.GetCount() != 1)
			{
				Valid = false;
				return ZEVector3::Zero;
			}
			
			TargetObject = &TransformStates[0];
			PivotPosition = TransformStates[0].Wrapper->GetPosition();
			break;

		case ZED_TP_FOCUSED_OBJECT:
			if (TransformFocused == NULL)
			{
				Valid = false;
				return ZEVector3::Zero;
			}

			TargetObject = TransformFocused;
			PivotPosition = TransformFocused->Wrapper->GetPosition();
			break;
			

		case ZED_TP_SELECTION_CENTER:
			if (TransformStates.GetCount() == 0)
			{
				Valid = false;
				return ZEVector3::Zero;
			}

			for (ZESize I = 0; I < TransformStates.GetCount(); I++)
				PivotPosition += TransformStates[I].OriginalPosition;

			PivotPosition /= TransformStates.GetCount();
			break;

		case ZED_TP_WORLD:
			return ZEVector3::Zero;

		default:
			return ZEVector3::Zero;
	}

	switch(TransformSpace)
	{
		default:
		case ZED_TS_WORLD:
			return PivotPosition;

		case ZED_TS_LOCAL:
			if (TargetObject == NULL)
			{
				Valid = false;
				return ZEVector3::Zero;
			}
			
			return PivotPosition - TargetObject->Wrapper->GetPosition();

		case ZED_TS_PARENT:
			if (TargetObject == NULL)
			{
				Valid = false;
				return ZEVector3::Zero;
			}

			if (TargetObject->Wrapper->GetParent() != NULL)
				return PivotPosition - TargetObject->Wrapper->GetParent3D()->GetPosition();
			else
				return PivotPosition;

		case ZED_TS_VIEW:
			return PivotPosition;
	}
}

ZEVector3 ZEDTransformationManager::GetRotation(bool& Valid)
{
	return ZEVector3::Zero;
}

ZEVector3 ZEDTransformationManager::GetScale(bool& Valid)
{
	return ZEVector3::Zero;
}

void ZEDTransformationManager::ObjectManager_OnObjectPropertyChanged(ZEDObjectManager* Manager, ZEDObjectWrapper* Object, ZEMTProperty* Property)
{
	if (TransformActive)
		return;

	UpdateTransformStates();
}

void ZEDTransformationManager::SelectionManager_OnSelectionChanged(ZEDSelectionManager* Manager, const ZEArray<ZEDObjectWrapper*>& Objects)
{
	zeDebugCheck(TransformActive, "Selection changed while a transformation is active");

	UpdateCommands();
	ResetTransform();
	UpdateTransformStates();
}

void ZEDTransformationManager::Viewport_OnKeyboardKeyPressed(ZEDViewport* Viewport, ZEDKeyboardKey Key)
{
	if (TransformActive && Key == ZED_VKK_ESCAPE)
	{
		ResetTransform();
		ZEMTEventBase::Acquire();
	}
}

void ZEDTransformationManager::Viewport_OnMouseButtonPressed(ZEDViewport* Viewport, ZEDMouseButton Button)
{
	if (Button != ZED_VMB_LEFT)
		return;

	if (GetTransformType() == ZED_TT_NONE)
		return;

	if (TransformStates.GetCount() == 0)
		return;

	const ZERNView& View = Viewport->GetView();
	ZERay Ray = ZERNScreenUtilities::ScreenToWorld(View, Viewport->GetMousePosition());

	MouseStartPosition = Viewport->GetMousePosition();

	ZEDGizmoAxis GizmoAxis = ZED_GA_NONE;
	ZEDTransformationState* TransformState = NULL;

	if (!TransformActive)
	{
		float GizmoAxisRayT = ZE_FLOAT_MAX;	
		for (ZESize I = 0; I < TransformStates.GetCount(); I++)
		{
			if (TransformStates[I].Gizmo == NULL)
				continue;

			ZEDGizmoAxis Axis = TransformStates[I].Gizmo->PickAxis(View, Ray, GizmoAxisRayT);
			if (Axis != ZED_GA_NONE)
			{
				TransformState = &TransformStates[I];
				GizmoAxis = Axis;
			}
			else
			{
				TransformStates[I].Gizmo->SetHoveredAxis(ZED_GA_NONE);
			}
		}

		if (TransformState != NULL)
			TransformState->Gizmo->SetHoveredAxis(GizmoAxis);
	}

	if (TransformState == NULL)
		return;

	TransformState->Gizmo->SetSelectedAxis(GizmoAxis);
	switch (GetTransformType())
	{
		case ZED_TT_TRANSLATE:
			StartTransform(TransformState->Gizmo);
				
			TransformGizmo->SetHoveredAxis(TransformState->Gizmo->GetSelectedAxis());
			TransformGizmo->StartMoveProjection(View, Ray);
				
			ZEMTEventBase::Acquire();
			break;

		case ZED_TT_ROTATE:
			StartTransform(TransformState->Gizmo);
				
			TransformGizmo->SetHoveredAxis(TransformState->Gizmo->GetSelectedAxis());
			TransformGizmo->StartRotationProjection();
				
			ZEMTEventBase::Acquire();
			break;

		case ZED_TT_SCALE:
			StartTransform(TransformState->Gizmo);
				
			TransformGizmo->SetHoveredAxis(TransformState->Gizmo->GetSelectedAxis());
			TransformGizmo->StartScaleProjection();
				
			ZEMTEventBase::Acquire();
			break;


		default:
			return;
	}
}

void ZEDTransformationManager::Viewport_OnMouseButtonRelease(ZEDViewport* Viewport, ZEDMouseButton Button)
{
	if (Button != ZED_VMB_LEFT)
		return;

	if (GetTransformType() == ZED_TT_NONE)
		return;

	if (TransformStates.GetCount() == 0)
		return;

	EndTransform();
}

void ZEDTransformationManager::Viewport_OnMouseMoved(ZEDViewport* Viewport, const ZEVector2& Position)
{
	if (GetTransformType() == ZED_TT_NONE)
		return;

	if (TransformStates.GetCount() == 0)
		return;

	const ZERNView& View = Viewport->GetView();
	ZERay Ray = ZERNScreenUtilities::ScreenToWorld(View, Position);

	ZEDGizmoAxis GizmoAxis = ZED_GA_NONE;
	ZEDTransformationState* TransformState = NULL;

	if (!TransformActive)
	{
		float GizmoAxisRayT = ZE_FLOAT_MAX;	
		for (ZESize I = 0; I < TransformStates.GetCount(); I++)
		{
			if (TransformStates[I].Gizmo == NULL)
				continue;

			ZEDGizmoAxis Axis = TransformStates[I].Gizmo->PickAxis(View, Ray, GizmoAxisRayT);
			if (Axis != ZED_GA_NONE)
			{
				TransformState = &TransformStates[I];
				GizmoAxis = Axis;
			}
			else
			{
				TransformStates[I].Gizmo->SetHoveredAxis(ZED_GA_NONE);
			}
		}

		if (TransformState != NULL)
			TransformState->Gizmo->SetHoveredAxis(GizmoAxis);
	}
	else
	{
		if (GetTransformType() == ZED_TT_TRANSLATE)
		{
			ZEVector3 TranslationDiff = TransformGizmo->MoveProjection(View, Ray);
			ApplyTranslation(TranslationDiff);

			ZEMTEventBase::Acquire();
		}
		else if (GetTransformType() == ZED_TT_ROTATE)
		{
			float MouseDisplacement = (MouseStartPosition - Position).x;

			ZEQuaternion RotationDiff = TransformGizmo->RotationProjection(View, MouseDisplacement * 0.01f);
			ApplyRotation(RotationDiff);

			ZEMTEventBase::Acquire();
		}
		else if (GetTransformType() == ZED_TT_SCALE)
		{
			float MouseDisplacement = (MouseStartPosition - Position).x;

			ZEVector3 ScaleDiff = TransformGizmo->ScaleProjection(MouseDisplacement * 0.001f);
			ApplyScale(ScaleDiff);

			ZEMTEventBase::Acquire();
		}
	}
}

void ZEDTransformationManager::Viewport_OnPreRender(ZEDViewport* Viewport, const ZERNPreRenderParameters& Parameters)
{
	//PreRenderParameters.Renderer->StartScene(NULL);
	for (ZESize I = 0; I < TransformStates.GetCount(); I++)
	{
		if (TransformStates[I].Gizmo == NULL)
			continue;

		if (!TransformStates[I].Gizmo->IsInitialized())
		{
			TransformStates[I].Gizmo->Initialize();
			continue;
		}

		TransformStates[I].Gizmo->PreRender(&Parameters);
	}
	//PreRenderParameters.Renderer->EndScene();
}

ZEDTransformationManager::ZEDTransformationManager()
{
	TransformType = ZED_TT_NONE;
	TransformSpace = ZED_TS_WORLD;
	TransformPivot = ZED_TP_OBJECT;
	TransformActive = false;
	TransformGizmo = NULL;
}

ZEDTransformationManager::~ZEDTransformationManager()
{
	Deinitialize();
}

void ZEDTransformationManager::RegisterCommands()
{
	SelectCommand.SetName("ZEDTransformationManager::SelectCommand");
	SelectCommand.SetCategory("Transformations");
	SelectCommand.SetText("Select");
	SelectCommand.SetType(ZED_CT_TOGGLE);
	SelectCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/Select.png");
	SelectCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_Q));
	SelectCommand.OnAction.AddDelegate<ZEDTransformationManager, &ZEDTransformationManager::SelectCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&SelectCommand);

	MoveCommand.SetName("ZEDTransformationManager::MoveCommand");
	MoveCommand.SetCategory("Transformations");
	MoveCommand.SetText("Move");
	MoveCommand.SetType(ZED_CT_TOGGLE);
	MoveCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/Move.png");
	MoveCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_W));
	MoveCommand.OnAction.AddDelegate<ZEDTransformationManager, &ZEDTransformationManager::MoveCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&MoveCommand);

	RotateCommand.SetName("ZEDTransformationManager::RotateCommand");
	RotateCommand.SetCategory("Transformations");
	RotateCommand.SetText("Rotate");
	RotateCommand.SetType(ZED_CT_TOGGLE);
	RotateCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/Rotate.png");
	RotateCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_E));
	RotateCommand.OnAction.AddDelegate<ZEDTransformationManager, &ZEDTransformationManager::RotateCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&RotateCommand);

	AxisRotateCommand.SetName("ZEDTransformationManager::RotateAxisCommand");
	AxisRotateCommand.SetCategory("Transformations");
	AxisRotateCommand.SetText("Axis Rotate");
	AxisRotateCommand.SetType(ZED_CT_TOGGLE);
	AxisRotateCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL | ZED_VKM_ALT, ZED_VKK_E));
	AxisRotateCommand.OnAction.AddDelegate<ZEDTransformationManager, &ZEDTransformationManager::AxisRotateCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&AxisRotateCommand);

	ScaleCommand.SetName("ZEDTransformationManager::ScaleCommand");
	ScaleCommand.SetCategory("Transformations");
	ScaleCommand.SetText("Scale");
	ScaleCommand.SetType(ZED_CT_TOGGLE);
	ScaleCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/Scale.png");
	ScaleCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_R));
	ScaleCommand.OnAction.AddDelegate<ZEDTransformationManager, &ZEDTransformationManager::ScaleCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&ScaleCommand);

	SnapCommand.SetName("ZEDTransformationManager::SnapCommand");
	SnapCommand.SetCategory("Transformations");
	SnapCommand.SetText("Snap");
	SnapCommand.SetType(ZED_CT_TOGGLE);
	SnapCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_T));
	SnapCommand.OnAction.AddDelegate<ZEDTransformationManager, &ZEDTransformationManager::TransformPivotCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&SnapCommand);

	TransformSpaceCommand.SetName("ZEDTransformationManager::TransformSpaceCommand");
	TransformSpaceCommand.SetCategory("Transformations");
	TransformSpaceCommand.SetText("Space");
	TransformSpaceCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/Space.png");
	TransformSpaceCommand.SetType(ZED_CT_LIST);
	ZEArray<ZEString> TransformSpaceCommandItems;
	TransformSpaceCommandItems.Add("Local");
	TransformSpaceCommandItems.Add("Parent");
	TransformSpaceCommandItems.Add("World");
	TransformSpaceCommandItems.Add("View");
	TransformSpaceCommand.SetListItems(TransformSpaceCommandItems);
	TransformSpaceCommand.OnAction.AddDelegate<ZEDTransformationManager, &ZEDTransformationManager::TransformSpaceCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&TransformSpaceCommand);

	TransformPivotCommand.SetName("ZEDTransformationManager::TransformPivotCommand");
	TransformPivotCommand.SetCategory("Transformations");
	TransformPivotCommand.SetText("Pivot");
	TransformPivotCommand.SetType(ZED_CT_LIST);
	TransformPivotCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/Pivot.png");
	ZEArray<ZEString> TransformPivotCommandItems;
	TransformPivotCommandItems.Add("Object");
	TransformPivotCommandItems.Add("Focused Object");
	TransformPivotCommandItems.Add("Selection Center");
	TransformPivotCommandItems.Add("World");
	TransformPivotCommand.SetListItems(TransformPivotCommandItems);
	TransformPivotCommand.OnAction.AddDelegate<ZEDTransformationManager, &ZEDTransformationManager::TransformPivotCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&TransformPivotCommand);

	XCommand.SetName("ZEDTransformationManager::XCommand");
	XCommand.SetCategory("Transformations");
	XCommand.SetText("X");
	XCommand.SetType(ZED_CT_INPUT_FLOAT);
	XCommand.OnAction.AddDelegate<ZEDTransformationManager, &ZEDTransformationManager::ZCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&XCommand);

	YCommand.SetName("ZEDTransformationManager::YCommand");
	YCommand.SetCategory("Transformations");
	YCommand.SetText("Y");
	YCommand.SetType(ZED_CT_INPUT_FLOAT);
	YCommand.OnAction.AddDelegate<ZEDTransformationManager, &ZEDTransformationManager::YCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&YCommand);

	ZCommand.SetName("ZEDTransformationManager::ZCommand");
	ZCommand.SetCategory("Transformations");
	ZCommand.SetText("Z");
	ZCommand.SetType(ZED_CT_INPUT_FLOAT);
	ZCommand.OnAction.AddDelegate<ZEDTransformationManager, &ZEDTransformationManager::ZCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&ZCommand);

	UpdateCommands();
}

void ZEDTransformationManager::UpdateCommands()
{
	MoveCommand.SetEnabled(TransformStates.GetCount() != 0);
	RotateCommand.SetEnabled(TransformStates.GetCount() != 0);
	ScaleCommand.SetEnabled(TransformStates.GetCount() != 0);
	AxisRotateCommand.SetEnabled(TransformStates.GetCount() != 0);

	SelectCommand.SetValueChecked(GetTransformType() == ZED_TT_NONE);
	MoveCommand.SetValueChecked(GetTransformType() == ZED_TT_TRANSLATE);
	RotateCommand.SetValueChecked(GetTransformType() == ZED_TT_ROTATE);
	ScaleCommand.SetValueChecked(GetTransformType() == ZED_TT_SCALE);
	AxisRotateCommand.SetValueChecked(GetTransformType() == ZED_TT_AXIS_ROTATE);
	SnapCommand.SetValueChecked(GetTransformType() == ZED_TT_SNAP);

	TransformSpaceCommand.SetValueIndex(GetTransformSpace());
	TransformPivotCommand.SetValueIndex(GetTransformPivot());

	bool AllowChangeCoords = 
		(TransformStates.GetCount() == 1) || 
		(TransformStates.GetCount() > 1 && GetTransformPivot() == ZED_TP_SELECTION_CENTER || GetTransformPivot() == ZED_TP_FOCUSED_OBJECT);

	XCommand.SetEnabled(AllowChangeCoords);
	YCommand.SetEnabled(AllowChangeCoords);
	ZCommand.SetEnabled(AllowChangeCoords);

	bool Valid;
	float X = GetX(Valid);
	if (Valid)
		XCommand.SetValueFloat(X);

	float Y = GetY(Valid);
	if (Valid)
		YCommand.SetValueFloat(Y);

	float Z = GetY(Valid);
	if (Valid)
		ZCommand.SetValueFloat(Z);
}

void ZEDTransformationManager::SelectCommand_OnAction(const ZEDCommand* Command)
{
	SetTransformType(ZED_TT_NONE);
}

void ZEDTransformationManager::MoveCommand_OnAction(const ZEDCommand* Command)
{
	SetTransformType(ZED_TT_TRANSLATE);
}

void ZEDTransformationManager::RotateCommand_OnAction(const ZEDCommand* Command)
{
	SetTransformType(ZED_TT_ROTATE);
}

void ZEDTransformationManager::AxisRotateCommand_OnAction(const ZEDCommand* Command)
{
	SetTransformType(ZED_TT_AXIS_ROTATE);
}

void ZEDTransformationManager::ScaleCommand_OnAction(const ZEDCommand* Command)
{
	SetTransformType(ZED_TT_SCALE);
}

void ZEDTransformationManager::SnapCommand_OnAction(const ZEDCommand* Command)
{
	SetTransformType(ZED_TT_SNAP);
}

void ZEDTransformationManager::TransformSpaceCommand_OnAction(const ZEDCommand* Command)
{
	SetTransformSpace((ZEDTransformSpace)Command->GetValueIndex());
}

void ZEDTransformationManager::TransformPivotCommand_OnAction(const ZEDCommand* Command)
{
	SetTransformPivot((ZEDTransformPivot)Command->GetValueIndex());
}

void ZEDTransformationManager::XCommand_OnAction(const ZEDCommand* Command)
{
	SetX(Command->GetValueFloat());
}

void ZEDTransformationManager::YCommand_OnAction(const ZEDCommand* Command)
{
	SetY(Command->GetValueFloat());
}

void ZEDTransformationManager::ZCommand_OnAction(const ZEDCommand* Command)
{
	SetZ(Command->GetValueFloat());
}

void ZEDTransformationManager::SetTransformType(ZEDTransformType Type)
{
	if (TransformType == Type)
		return;

	TransformType = Type;

	UpdateTransformStates();

	OnTransformationStateChanged(this);

	UpdateCommands();
}

ZEDTransformType ZEDTransformationManager::GetTransformType()
{
	return TransformType;
}

void ZEDTransformationManager::SetTransformSpace(ZEDTransformSpace Space)
{
	if (TransformSpace == Space)
		return;

	TransformSpace = Space;

	UpdateTransformStates();

	OnTransformationStateChanged(this);

	UpdateCommands();
}

ZEDTransformSpace ZEDTransformationManager::GetTransformSpace()
{
	return TransformSpace;
}

void ZEDTransformationManager::SetTransformPivot(ZEDTransformPivot Pivot)
{
	if (TransformPivot == Pivot)
		return;

	TransformPivot = Pivot;

	UpdateTransformStates();

	OnTransformationStateChanged(this);

	UpdateCommands();
}

ZEDTransformPivot ZEDTransformationManager::GetTransformPivot()
{
	return TransformPivot;
}

void ZEDTransformationManager::SetX(float Value)
{
	switch (TransformType)
	{
		default:
		case ZED_TT_NONE:
			break;

		case ZED_TT_TRANSLATE:
		{
			StartTransform(NULL);
			ApplyTranslation(ZEVector3(Value - PivotPosition.x, 0.0f, 0.0f));
			EndTransform();
			break;
		}

		case ZED_TT_ROTATE:
		{
			StartTransform(NULL);
			ApplyRotation(ZEQuaternion(ZEAngle::ToRadian(Value), ZEVector3::UnitX));
			EndTransform();
			break;
		}

		case ZED_TT_SCALE:
		{
			StartTransform(NULL);
			ApplyScale(ZEVector3(Value / 100.0f, 0.0f, 0.0f));
			EndTransform();
			break;
		}
	}

	UpdateCommands();
}

float ZEDTransformationManager::GetX(bool& Valid)
{
	switch (TransformType)
	{
		default:
		case ZED_TT_NONE:
			return 0.0f;

		case ZED_TT_TRANSLATE:
			return GetPosition(Valid).x;

		case ZED_TT_ROTATE:
			return GetRotation(Valid).x;

		case ZED_TT_SCALE:
			return GetScale(Valid).x;
	}
}

void ZEDTransformationManager::SetY(float Value)
{
	switch (TransformType)
	{
		default:
		case ZED_TT_NONE:
			break;

		case ZED_TT_TRANSLATE:
		{
			StartTransform(NULL);
			ApplyTranslation(ZEVector3(0.0f, Value - PivotPosition.y, 0.0f));
			EndTransform();
			break;
		}

		case ZED_TT_ROTATE:
		{
			StartTransform(NULL);
			ApplyRotation(ZEQuaternion(ZEAngle::ToRadian(Value), ZEVector3::UnitY));
			EndTransform();
			break;
		}

		case ZED_TT_SCALE:
		{
			StartTransform(NULL);
			ApplyScale(ZEVector3(0.0f, Value / 100.0f, 0.0f));
			EndTransform();
			break;
		}
	}

	UpdateCommands();
}

float ZEDTransformationManager::GetY(bool& Valid)
{
	switch (TransformType)
	{
		default:
		case ZED_TT_NONE:
			return 0.0f;

		case ZED_TT_TRANSLATE:
			return GetPosition(Valid).y;

		case ZED_TT_ROTATE:
			return GetRotation(Valid).y;

		case ZED_TT_SCALE:
			return GetScale(Valid).y;
	}
}

void ZEDTransformationManager::SetZ(float Value)
{
	switch (TransformType)
	{
		default:
		case ZED_TT_NONE:
			break;

		case ZED_TT_TRANSLATE:
		{
			StartTransform(NULL);
			ApplyTranslation(ZEVector3(0.0f, 0.0f, Value - PivotPosition.z));
			EndTransform();
			break;
		}

		case ZED_TT_ROTATE:
		{
			StartTransform(NULL);
			ApplyRotation(ZEQuaternion(ZEAngle::ToRadian(Value), ZEVector3::UnitZ));
			EndTransform();
			break;
		}

		case ZED_TT_SCALE:
		{
			StartTransform(NULL);
			ApplyScale(ZEVector3(0.0f, 0.0f, Value / 100.0f));
			EndTransform();
			break;
		}
	}

	UpdateCommands();
}

float ZEDTransformationManager::GetZ(bool& Valid)
{
	switch (TransformType)
	{
		default:
		case ZED_TT_NONE:
			return 0.0f;

		case ZED_TT_TRANSLATE:
			return GetPosition(Valid).z;

		case ZED_TT_ROTATE:
			return GetRotation(Valid).z;

		case ZED_TT_SCALE:
			return GetScale(Valid).z;
	}
}

ZEDTransformationManager* ZEDTransformationManager::CreateInstance()
{
	return new ZEDTransformationManager();
}
