//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDViewPort.cpp
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

#include "ZEDViewPort.h"
#include "ZEDCore.h"
#include "ZEDGizmo.h"
#include "ZEDSelectionManager.h"
#include "ZEDTransformationManager.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZEWindow.h"
#include "ZEGame/ZEScene.h"
#include "ZEMath/ZEAngle.h"

void ZEDViewPort::MoveCamera(float ElapsedTime)
{	
	ZEVector3 Position = Camera->GetPosition();
	ZEQuaternion Rotation = Camera->GetRotation();
	ZEVector3 PositionChange;
	ZEQuaternion RotationChange;

	if (PressedKeyboardKeys.contains(Qt::Key_W))
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3(0, 0, 5));
		ZEVector3::Scale(PositionChange, PositionChange, StepSize * ElapsedTime);
		ZEVector3::Add(Position, Position, PositionChange);
		Camera->SetPosition(Position);
	}

	if (PressedKeyboardKeys.contains(Qt::Key_S))
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3(0, 0, -5));
		ZEVector3::Scale(PositionChange, PositionChange, StepSize * ElapsedTime);
		ZEVector3::Add(Position, Position, PositionChange);
		Camera->SetPosition(Position);
	}

	if (PressedKeyboardKeys.contains(Qt::Key_A))
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3(-5, 0, 0));
		ZEVector3::Scale(PositionChange, PositionChange, StepSize * ElapsedTime);
		ZEVector3::Add(Position, Position, PositionChange);
		Camera->SetPosition(Position);
	}

	if (PressedKeyboardKeys.contains(Qt::Key_D))
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3(5, 0, 0));
		ZEVector3::Scale(PositionChange, PositionChange, StepSize * ElapsedTime);
		ZEVector3::Add(Position, Position, PositionChange);
		Camera->SetPosition(Position);
	}
}

void ZEDViewPort::RotateCamera(const ZEVector2& MousePosition)
{
	float XDiff = MousePosition.x - MouseStartPosition.x;
	float YDiff = MousePosition.y - MouseStartPosition.y;

	Yaw = Yaw + 0.005f * XDiff;
	Pitch = Pitch + 0.005f * YDiff;

	if (Yaw < -ZE_PI)
		Yaw = ZE_PI;
	else if (Yaw > ZE_PI)
		Yaw = -ZE_PI;

	if (Pitch < -ZE_PI_2)
		Pitch = -ZE_PI_2;
	else if (Pitch > ZE_PI_2)
		Pitch = ZE_PI_2;

	ZEQuaternion Temp, Rotation;
	ZEQuaternion::CreateFromEuler(Temp, Pitch, Yaw, Roll);
	ZEQuaternion::Normalize(Rotation, Temp);
	Camera->SetRotation(Rotation);
}

void ZEDViewPort::mousePressEvent(QMouseEvent* MouseEvent)
{
	setFocus(Qt::MouseFocusReason);

	if(MouseEvent->button() == Qt::LeftButton || MouseEvent->button() == Qt::RightButton)
	{
		this->MouseStartPosition.x = MouseEvent->x();
		this->MouseStartPosition.y = MouseEvent->y();
	}
	
	if (MouseEvent->button() == Qt::LeftButton && !(MouseEvent->buttons() & Qt::RightButton))
	{
		ZERay Ray;
		ZEDCore::GetInstance()->GetEditorScene()->GetActiveCamera()->GetScreenRay(Ray, MouseEvent->pos().x(), MouseEvent->pos().y());
		float TRay = FLT_MAX;

		ZEDGizmo* Gizmo = ZEDTransformationManager::GetInstance()->GetGizmo();

		if (Gizmo->GetMode() == ZED_GM_NONE)
			return;

		Gizmo->SetSelectedAxis(Gizmo->PickAxis(Ray, TRay));

		if (Gizmo->GetSelectedAxis() == ZED_GA_NONE)
			return;

		switch (Gizmo->GetMode())
		{
			case ZED_GM_MOVE:
				Gizmo->StartMoveProjection(Ray);
				return;
			case ZED_GM_ROTATE:
				Gizmo->StartRotationProjection(Ray);
				return;
			case ZED_GM_SCALE:
				Gizmo->StartScaleProjection(Ray);
				return;
		}
	}
}

void ZEDViewPort::mouseMoveEvent(QMouseEvent* MouseEvent)
{	
	if((MouseEvent->buttons() & Qt::RightButton))
	{
		if (MouseEvent->modifiers() & Qt::AltModifier)
		{
			ZEVector3 CameraDirection = Camera->GetFront();
			ZEVector3::Scale(CameraDirection,CameraDirection, 0.1f * (MouseStartPosition.y - MouseEvent->y()));
			Camera->SetPosition(Camera->GetPosition() + CameraDirection);
		}
		else if (MouseEvent->modifiers() == Qt::NoModifier)
		{
			MouseCurrentPosition.x = MouseEvent->pos().x();
			MouseCurrentPosition.y = MouseEvent->pos().y();
			RotateCamera(MouseCurrentPosition);	
		}

		return;
	}

	if(MouseEvent->buttons() & Qt::LeftButton)
	{	
		ZERay Ray;
		ZEDCore::GetInstance()->GetEditorScene()->GetActiveCamera()->GetScreenRay(Ray, MouseEvent->pos().x(), MouseEvent->pos().y());
	
		ZEDGizmo* Gizmo = ZEDTransformationManager::GetInstance()->GetGizmo();

		if (Gizmo->GetSelectedAxis() != ZED_GA_NONE)
		{
			ZEMatrix4x4 NewTransform = ZEMatrix4x4::Identity;

			if (Gizmo->GetMode() == ZED_GM_MOVE)
			{
				if (ZEDTransformationManager::GetInstance()->GetTransformType() == ZED_TT_NONE)
					ZEDTransformationManager::GetInstance()->BeginTransform(ZED_TT_TRANSLATE);

				ZEVector3 NewTranslation = (Gizmo->MoveProjection(Ray) - Gizmo->GetPosition());
				ZEMatrix4x4::CreateTranslation(NewTransform, NewTranslation);
				ZEDTransformationManager::GetInstance()->ApplyTransform(NewTransform);
				//ParentEditor->UpdatePropertyWidgetValues();
			}
			else if (Gizmo->GetMode() == ZED_GM_ROTATE)
			{
				if (ZEDTransformationManager::GetInstance()->GetTransformType() == ZED_TT_NONE)
					ZEDTransformationManager::GetInstance()->BeginTransform(ZED_TT_ROTATE);

				ZEQuaternion NewOrientation = Gizmo->RotationProjection(Ray);
				ZEMatrix4x4::CreateRotation(NewTransform, NewOrientation);
				ZEDTransformationManager::GetInstance()->ApplyTransform(NewTransform);
				//ParentEditor->UpdatePropertyWidgetValues();
			}
			else if (Gizmo->GetMode() == ZED_GM_SCALE)
			{
				if (ZEDTransformationManager::GetInstance()->GetTransformType() == ZED_TT_NONE)
					ZEDTransformationManager::GetInstance()->BeginTransform(ZED_TT_SCALE);

				ZEVector3 NewScale = Gizmo->ScaleProjection(Ray);
				ZEMatrix4x4::CreateScale(NewTransform, NewScale);
				ZEDTransformationManager::GetInstance()->ApplyTransform(NewTransform);
				//ParentEditor->UpdatePropertyWidgetValues();
			}
		}
		else
		{
			MouseCurrentPosition.x = MouseEvent->pos().x();
			MouseCurrentPosition.y = MouseEvent->pos().y();

			if (MouseEvent->modifiers() & Qt::ControlModifier)
			{
				//Make add to selection display preps
			}
			else if (MouseEvent->modifiers() & Qt::AltModifier)
			{
				//Make remove from selection display preps
			}
		}

	}
	else
	{
		ZERay Ray;
		ZEDCore::GetInstance()->GetEditorScene()->GetActiveCamera()->GetScreenRay(Ray, MouseEvent->pos().x(), MouseEvent->pos().y());
		float TRay = FLT_MAX;

		ZEDGizmo* Gizmo = ZEDTransformationManager::GetInstance()->GetGizmo();

		if (Gizmo->GetMode() == ZED_GM_NONE)
			return;

		switch(Gizmo->PickAxis(Ray, TRay))
		{
			case ZED_GA_NONE:
				Gizmo->SetHoveredAxis(ZED_GA_NONE);
				break;
			case ZED_GA_X_AXIS:
				Gizmo->SetHoveredAxis(ZED_GA_X_AXIS);
				break;
			case ZED_GA_Y_AXIS:
				Gizmo->SetHoveredAxis(ZED_GA_Y_AXIS);
				break;
			case ZED_GA_Z_AXIS:
				Gizmo->SetHoveredAxis(ZED_GA_Z_AXIS);
				break;
			case ZED_GA_XY_AXIS:
				Gizmo->SetHoveredAxis(ZED_GA_XY_AXIS);
				break;
			case ZED_GA_XZ_AXIS:
				Gizmo->SetHoveredAxis(ZED_GA_XZ_AXIS);
				break;
			case ZED_GA_YZ_AXIS:
				Gizmo->SetHoveredAxis(ZED_GA_YZ_AXIS);
				break;
			case ZED_GA_XYZ_AXIS:
				Gizmo->SetHoveredAxis(ZED_GA_XYZ_AXIS);
				break;
		}	
	}
}

void ZEDViewPort::mouseReleaseEvent(QMouseEvent* MouseEvent)
{
	if (MouseEvent->button() == Qt::LeftButton)
	{
		ZEDTransformationManager* TransformationManager = ZEDTransformationManager::GetInstance();

		ZERay Ray;
		ZEDCore::GetInstance()->GetEditorScene()->GetActiveCamera()->GetScreenRay(Ray, MouseEvent->pos().x(), MouseEvent->pos().y());

		ZEDGizmo* Gizmo = TransformationManager->GetGizmo();

		if (Gizmo->GetSelectedAxis() != ZED_GA_NONE)
		{
			ZEMatrix4x4 NewTransform = ZEMatrix4x4::Identity;

			if (Gizmo->GetMode() == ZED_GM_MOVE)
			{
				ZEVector3 NewTranslation = (Gizmo->MoveProjection(Ray) - Gizmo->GetPosition());
				ZEMatrix4x4::CreateTranslation(NewTransform, NewTranslation);
				TransformationManager->ApplyTransform(NewTransform);
				TransformationManager->EndTransform();
				//ParentEditor->UpdatePropertyWidgetValues();
			}
			else if (Gizmo->GetMode() == ZED_GM_ROTATE)
			{
				ZEQuaternion NewOrientation = Gizmo->RotationProjection(Ray);
				ZEMatrix4x4::CreateRotation(NewTransform, NewOrientation);
				TransformationManager->ApplyTransform(NewTransform);
				TransformationManager->EndTransform();
				//ParentEditor->UpdatePropertyWidgetValues();
			}
			else if (Gizmo->GetMode() == ZED_GM_SCALE)
			{
				ZEVector3 NewScale = Gizmo->ScaleProjection(Ray);
				ZEMatrix4x4::CreateScale(NewTransform, NewScale);
				TransformationManager->ApplyTransform(NewTransform);
				TransformationManager->EndTransform();
				//ParentEditor->UpdatePropertyWidgetValues();
			}
		}
		else
		{
			MouseCurrentPosition.x = MouseEvent->pos().x();
			MouseCurrentPosition.y = MouseEvent->pos().y();

			if (MouseEvent->modifiers() & Qt::ControlModifier) //this should be equality but I'm not sure.
			{
				ZEDSelectionManager::GetInstance()->SelectObject(MouseStartPosition, MouseCurrentPosition);
			}
			else if (MouseEvent->modifiers() & Qt::AltModifier)
			{
				ZEDSelectionManager::GetInstance()->DeselectObject(MouseStartPosition, MouseCurrentPosition);
			}
			else
			{
				ZEDSelectionManager::GetInstance()->ClearSelection();
				ZEDSelectionManager::GetInstance()->SelectObject(MouseStartPosition, MouseCurrentPosition);
			}
		}

	}
	else if (MouseEvent->button() == Qt::RightButton)
	{
		//Nothing to do?
	}
}

void ZEDViewPort::keyPressEvent(QKeyEvent* KeyEvent)
{
	if (!KeyEvent->isAutoRepeat())
		PressedKeyboardKeys.insert(KeyEvent->key());
}

void ZEDViewPort::keyReleaseEvent(QKeyEvent* KeyEvent)
{
	if (!KeyEvent->isAutoRepeat())
		PressedKeyboardKeys.remove(KeyEvent->key());
}

void ZEDViewPort::resizeEvent(QResizeEvent* ResizeEvent)
{
	QSize NewSize = ResizeEvent->size();
	ZECore::GetInstance()->GetWindow()->SetWindowSize(NewSize.width(), NewSize.height());
}

void ZEDViewPort::dragEnterEvent(QDragEnterEvent* Event)
{

}

void ZEDViewPort::dragMoveEvent(QDragMoveEvent* Event)
{

}

void ZEDViewPort::dragLeaveEvent(QDragLeaveEvent* Event)
{

}

void ZEDViewPort::dropEvent(QDropEvent* Event)
{

}

void ZEDViewPort::focusInEvent(QFocusEvent* Event)
{
	this->setMouseTracking(true);
}

void ZEDViewPort::focusOutEvent(QFocusEvent* Event)
{
	this->setMouseTracking(false);
}

void ZEDViewPort::SetViewMode(ZEDViewMode Mode)
{
	this->ViewMode = Mode;
}

ZEDViewMode ZEDViewPort::GetViewMode()
{
	return ViewMode;
}

const ZEView& ZEDViewPort::GetView()
{
	return Camera->GetView();
}

void ZEDViewPort::SetStepSize(ZEInt StepSize)
{
	this->StepSize = StepSize;
}

ZEInt ZEDViewPort::GetStepSize()
{
	return StepSize;
}

void ZEDViewPort::Tick(float Time)
{
	if (!PressedKeyboardKeys.isEmpty())
		MoveCamera(Time);

	ZEDGizmo* Gizmo = ZEDTransformationManager::GetInstance()->GetGizmo();
	ZEDSelectionManager* SelectionManager = ZEDSelectionManager::GetInstance();

	if ((Gizmo->GetMode() != ZED_GM_NONE) && (SelectionManager->GetSelectedObjects().GetCount() != 0))
	{
		ZEMatrix4x4 Pivot = SelectionManager->GetSelectionPivot();
		Gizmo->SetWorldPosition(Pivot.GetTranslation());
		Gizmo->SetWorldRotation(Pivot.GetRotation());
		Gizmo->SetWorldScale(Pivot.GetScale());
	}

}

ZEDViewPort::ZEDViewPort(QWidget* Parent) : QFrame(Parent)
{
	ViewMode = ZED_VM_FREE;
	MouseStartPosition = ZEVector2::Zero;
	MouseCurrentPosition = ZEVector2::Zero;
	float Pitch = 0.0f;
	float Yaw = 0.0f;
	float Roll = 0.0f;
	StepSize = 1;
}
