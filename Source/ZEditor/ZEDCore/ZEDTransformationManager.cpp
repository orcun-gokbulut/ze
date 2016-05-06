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
#include "ZEDModule.h"
#include "ZEDGizmo.h"
#include "ZEDTransformationOperation.h"
#include "ZEDObjectWrapper.h"
#include "ZEDOperationManager.h"
#include "ZEDSelectionManager.h"
#include "ZEDViewport.h"
#include "ZEDViewportInput.h"

#include "ZERenderer\ZERNScreenUtilities.h"
#include "ZERenderer\ZERNRenderParameters.h"


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

void ZEDTransformationManager::UpdateGizmos()
{
	for (ZESize I = 0; I < TransformStates.GetCount(); I++)
	{
		ZEDGizmo* Gizmo = TransformStates[I].Gizmo;
		if (Gizmo == NULL)
			continue;


		Gizmo->SetPosition(TransformStates[I].Wrapper->GetPosition());	
		switch (TransformSpace)
		{
			default:
			case ZED_TS_WORLD:
				Gizmo->SetRotation(ZEQuaternion::Identity);
				break;

			case ZED_TS_LOCAL:
				Gizmo->SetRotation(TransformStates[I].Wrapper->GetRotation());
				break;

			case ZED_TS_PARENT:
				if (TransformStates[I].Wrapper->GetParent() != NULL)
					Gizmo->SetRotation(TransformStates[I].Wrapper->GetParent()->GetRotation());
				else
					Gizmo->SetRotation(TransformStates[I].Wrapper->GetRotation());
				break;

			case ZED_TS_VIEW:
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

		TransformStates[I].Gizmo->Initialize();
	}
}

void ZEDTransformationManager::UpdateTransformStates()
{
	const ZEArray<ZEDObjectWrapper*>& Selection = ZEDCore::GetInstance()->GetSelectionManager()->GetSelectedObjects();

	TransformStates.SetCount(Selection.GetCount());
	for (ZESize I = 0; I < Selection.GetCount(); I++)
	{
		TransformStates[I].Wrapper = Selection[I];
		TransformStates[I].OriginalPosition = Selection[I]->GetPosition();
		TransformStates[I].OriginalRotation = Selection[I]->GetRotation();
		TransformStates[I].OriginalScale = Selection[I]->GetScale();
	}

	if (TransformStates.GetCount())
	{
		if (TransformPivot == ZED_TP_OBJECT)
		{
			for (ZESize I = 0; I < TransformStates.GetCount(); I++)
			{
				TransformStates[I].Pivot = TransformStates[I].Wrapper->GetPosition();
				TransformStates[I].Gizmo = ZEDGizmo::CreateInstance();
			}
		}
		else if (TransformPivot == ZED_TP_FIRST_OBJECT)
		{
			TransformStates.GetFirstItem().Gizmo = ZEDGizmo::CreateInstance();

			const ZEVector3& Pivot = TransformStates.GetFirstItem().OriginalPosition;
			for (ZESize I = 0; I < TransformStates.GetCount(); I++)
				TransformStates[I].Pivot = Pivot;
		}
		else if (TransformPivot == ZED_TP_LAST_OBJECT)
		{
			TransformStates.GetLastItem().Gizmo = ZEDGizmo::CreateInstance();

			const ZEVector3& Pivot = TransformStates.GetLastItem().OriginalPosition;
			for (ZESize I = 0; I < TransformStates.GetCount(); I++)
				TransformStates[I].Pivot = Pivot;
		}
		else if (TransformPivot == ZED_TP_CENTER)
		{
			TransformStates.GetFirstItem().Gizmo = ZEDGizmo::CreateInstance();

			ZEVector3 Pivot;
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

	UpdateGizmos();
}

bool ZEDTransformationManager::InitializeSelf()
{
	if (!ZEInitializable::InitializeSelf())
		return false;

	return true;
}

void ZEDTransformationManager::DeinitializeSelf()
{
	ResetTransform();
	TransformStates.Clear();

	ZEInitializable::DeinitializeSelf();
}


void ZEDTransformationManager::StartTransform(ZEDGizmo* TransformGizmo)
{
	TransformActive = true;
	this->TransformGizmo = TransformGizmo;
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
}

void ZEDTransformationManager::EndTransform()
{
	if (!TransformActive)
		return;

	ZEDSelectionManager* SelectionManager = ZEDSelectionManager::GetInstance();
	ZEMatrix4x4 Transform;

	ZEDTransformationOperation* Operation = new ZEDTransformationOperation(TransformType, TransformStates);
	Operation->SetApplyEnabled(false);
	ZEDOperationManager::GetInstance()->DoOperation(Operation);
	Operation->SetApplyEnabled(true);

	for (ZESize I = 0; I < TransformStates.GetCount(); I++)
	{
		ZEDTransformationState& TransformState = TransformStates[I];
		TransformState.OriginalPosition = TransformState.Wrapper->GetPosition();
		TransformState.OriginalRotation = TransformState.Wrapper->GetRotation();
		TransformState.OriginalScale = TransformState.Wrapper->GetScale();
	}

	TransformGizmo = NULL;
	TransformActive = false;
}

ZEDTransformationManager::ZEDTransformationManager()
{
	Module = NULL;
	TransformType = ZED_TT_NONE;
	TransformSpace = ZED_TS_WORLD;
	TransformPivot = ZED_TP_OBJECT;
	TransformActive = false;
	TransformGizmo = NULL;
}

ZEDModule* ZEDTransformationManager::GetModule()
{
	return Module;
}

void ZEDTransformationManager::SetTransformType(ZEDTransformType Type)
{
	if (TransformType == Type)
		return;

	TransformType = Type;

	UpdateGizmos();
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
}

ZEDTransformPivot ZEDTransformationManager::GetTransformPivot()
{
	return TransformPivot;
}

void ZEDTransformationManager::SetX(float Value)
{

}

float ZEDTransformationManager::GetX()
{
	return 0.0f;
}

void ZEDTransformationManager::SetY(float Value)
{

}

float ZEDTransformationManager::GetY()
{
	return 0.0f;
}

void ZEDTransformationManager::SetZ(float Value)
{

}

float ZEDTransformationManager::GetZ()
{
	return 0.0f;
}

void ZEDTransformationManager::SelectionEvent(const ZEDSelectionEvent& Event)
{
	ResetTransform();
	UpdateTransformStates();
}

void ZEDTransformationManager::ViewportChangedEvent(const ZEDViewportChangedEvent& Event)
{

}

bool ZEDTransformationManager::ViewportKeyboardEvent(const ZEDViewportKeyboardEvent& Event)
{
	if (TransformActive && Event.GetKey() == ZED_IKK_KEY_ESCAPE)
	{
		ResetTransform();
		return true;
	}

	return false;
}

bool ZEDTransformationManager::ViewportMouseEvent(const ZEDViewportMouseEvent& Event)
{
	if (GetTransformType() == ZED_TT_NONE)
		return false;

	if (TransformStates.GetCount() == 0)
		return false;

	const ZERNView& View = Event.GetViewport()->GetView();
	ZERay Ray = ZERNScreenUtilities::ScreenToWorld(Event.GetViewport()->GetView(), Event.GetPosition());

	if (!TransformActive)
	{
		float TRay = FLT_MAX;
		for (ZESize I = 0; I < TransformStates.GetCount(); I++)
		{
			ZEDGizmo* Gizmo = TransformStates[I].Gizmo;
			if (Gizmo == NULL)
				continue;

			ZEDGizmoAxis Axis = Gizmo->PickAxis(View, Ray, TRay);
			Gizmo->SetHoveredAxis(Axis);
		}
	}

	if (Event.GetButton() == ZED_MB_LEFT && Event.GetType() == ZED_ET_BUTTON_PRESSED)
	{
		MouseStartPosition = Event.GetPosition();

		float TRay = FLT_MAX;
		for (ZESize I = 0; I < TransformStates.GetCount(); I++)
		{
			ZEDGizmo* Gizmo = TransformStates[I].Gizmo;
			if (Gizmo == NULL)
				continue;

			ZEDGizmoAxis Axis = Gizmo->PickAxis(View, Ray, TRay);
			Gizmo->SetSelectedAxis(Axis);

			if (Gizmo->GetSelectedAxis() == ZED_GA_NONE)
				return false;

			switch (Gizmo->GetMode())
			{
				case ZED_GM_MOVE:
					StartTransform(Gizmo);
					TransformGizmo->SetPosition(TransformStates[I].OriginalPosition);
					TransformGizmo->SetHoveredAxis(Gizmo->GetSelectedAxis());
					TransformGizmo->StartMoveProjection(View, Ray);
					return true;

				case ZED_GM_ROTATE:
					StartTransform(Gizmo);
					TransformGizmo->SetPosition(TransformStates[I].OriginalPosition);
					TransformGizmo->SetHoveredAxis(Gizmo->GetSelectedAxis());
					TransformGizmo->StartRotationProjection();
					return true;

				case ZED_GM_SCALE:
					StartTransform(Gizmo);
					TransformGizmo->SetPosition(TransformStates[I].OriginalPosition);
					TransformGizmo->SetHoveredAxis(Gizmo->GetSelectedAxis());
					TransformGizmo->StartScaleProjection();
					return true;

				default:
					return false;
			}
		}
	}
	else if (Event.GetType() == ZED_ET_MOUSE_MOVED)
	{
		if (!TransformActive)
			return false;

		// End	
		ZEMatrix4x4 NewTransform = ZEMatrix4x4::Identity;
		if (GetTransformType() == ZED_TT_TRANSLATE)
		{
			ZEVector3 NewTranslation = TransformGizmo->MoveProjection(View, Ray);
			ApplyTranslation(NewTranslation);
			
			return true;
		}
		else if (GetTransformType() == ZED_TT_ROTATE)
		{

			float MouseDisplacement = (MouseStartPosition - Event.GetPosition()).x;

			ZEQuaternion NewOrientation = TransformGizmo->RotationProjection(View, MouseDisplacement * 0.01f);
			ApplyRotation(NewOrientation);
			
			return true;
		}
		else if (GetTransformType() == ZED_TT_SCALE)
		{
			float MouseDisplacement = (MouseStartPosition - Event.GetPosition()).x;

			ZEVector3 NewScale = TransformGizmo->ScaleProjection(MouseDisplacement * 0.001f);
			ApplyScale(NewScale);

			return true;
		}
	}
	else if (Event.GetButton() == ZED_MB_LEFT && Event.GetType() == ZED_ET_BUTTON_RELEASED)
	{
		EndTransform();
	}

	return false;
}

void ZEDTransformationManager::PreRender(ZERNRenderer* Renderer)
{
	ZERNPreRenderParameters Parameters;
	Parameters.Renderer = Renderer;
	Parameters.View = &Renderer->GetView();

	Renderer->StartScene(NULL);
	for (ZESize I = 0; I < TransformStates.GetCount(); I++)
	{
		if (TransformStates[I].Gizmo == NULL)
			continue;

		TransformStates[I].Gizmo->PreRender(&Parameters);
	}
	Renderer->EndScene();
}

void ZEDTransformationManager::Destroy()
{
	delete this;
}

ZEDTransformationManager* ZEDTransformationManager::GetInstance()
{
	return ZEDCore::GetInstance()->GetTransformationManager();
}
