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

#include <QDir>
#include <QChar>
#include "ZEDSceneEditor/ZEDSceneEditor.h"
#include "ZEDViewPort.h"
#include "ZESDK.h"
#include "ZEGame\ZEGame.h"
#include "ZEGame\ZEScene.h"
#include "ZEGraphics\ZECamera.h"
#include "ZEMath\ZEPlane.h"
#include "ZEMath\ZERay.h"
#include "ZECore\ZEWindow.h"
#include <float.h>
#include "ZEMath/ZEMathDefinitions.h"

using namespace Qt;

void ZEDViewPort::mousePressEvent(QMouseEvent* Event)
{
	setFocus(MouseFocusReason);

	if(Event->button() == RightButton)
	{
		this->SendInputs = true;
		this->setFocus(MouseFocusReason);
		this->OldMousePositionX = Event->x();
		this->OldMousePositionY = Event->y();
	}
	else if (Event->button() == LeftButton && !(Event->buttons() & RightButton))
	{

		ZERay Ray;
		this->Scene->GetActiveCamera()->GetScreenRay(Ray, Event->pos().x(), Event->pos().y());
		float TRay = FLT_MAX;
		SelectedAxis = ZED_GA_NONE;

		for(int I = 0; I < this->SelectedItems->GetCount(); I++)
		{
			if(this->SelectedItems->GetItem(I)->GetGizmo() != NULL)
				this->SelectedAxis = this->SelectedItems->GetItem(I)->GetGizmo()->PickAxis(Ray, TRay);
			
			if(SelectedAxis != ZED_GA_NONE)
				break;
		}

		ZEDGizmo* SelectedGizmo = NULL;
		for(int I = 0; I < this->SelectedItems->GetCount(); I++)
		{
			if(this->SelectedItems->GetItem(I)->GetGizmo() != NULL)
			{
				if(this->SelectedItems->GetItem(I)->GetGizmo()->GetHoveredAxis() != ZED_GA_NONE)
				{
					SelectedGizmo = this->SelectedItems->GetItem(I)->GetGizmo();
					break;
				}
			}
		}

		if (SelectedAxis != ZED_GA_NONE)
		{
			if(SelectedGizmo->GetMode() == ZED_GM_MOVE)
				SelectedGizmo->StartMoveProjection(SelectedAxis, Ray);
			if(SelectedGizmo->GetMode() == ZED_GM_SCALE)
				SelectedGizmo->StartScaleProjection(SelectedAxis, Ray);
			if(SelectedGizmo->GetMode() == ZED_GM_SCALE)
				SelectedGizmo->StartRotationProjection(SelectedAxis, Ray);

			this->SendInputs = true;
		}
		else
		{
			ZEEntity* PickedEntity = NULL;
			PickedEntity = RayCaster.PickEntity(Event->pos().x(), Event->pos().y());

			for (int I = 0; I < SelectedItems->GetCount(); I++)
				delete SelectedItems->GetItem(I);

			SelectedItems->Clear();

			if (PickedEntity == NULL)
			{
				ParentEditor->SelectionDirtyFlag = true;
				return;
			}
			
			SelectedItems->Add(ParentEditor->CreateSelectionItem(PickedEntity));
			ParentEditor->SelectionDirtyFlag = true;
		}
	}
}

void ZEDViewPort::mouseMoveEvent(QMouseEvent* Event)
{
	if(this->SendInputs)
	{	
		if(Event->buttons() & RightButton && AltPressed)
		{
			ZEVector3 CameraDirection = Player->GetDirection();
			ZEVector3::Scale(CameraDirection,CameraDirection, 0.1f * (OldMousePositionY - Event->y()));
			Player->SetPosition(Player->GetPosition() + CameraDirection);
		}

		/*else if(Event->buttons() & RightButton && Event->buttons() & LeftButton)
		{
			ZEVector3 NewDirection = -Player->GetPosition();
			ZEVector3::Normalize(NewDirection, NewDirection);
			ZEQuaternion NewRotation;
			ZEQuaternion::CreateFromDirection(NewRotation, NewDirection);
			Player->SetRotation(NewRotation);

		}*/

		else if(Event->buttons() & RightButton)
		{
			RotatePlayer(Event->pos());		
		}
		else if(Event->buttons() & LeftButton && !(Event->buttons() & RightButton))
		{	
			ZEDGizmo* SelectedGizmo = NULL;
			for(int I = 0; I < this->SelectedItems->GetCount(); I++)
			{
				if(this->SelectedItems->GetItem(I)->GetGizmo() != NULL)
				{
					if(this->SelectedItems->GetItem(I)->GetGizmo()->GetHoveredAxis() != ZED_GA_NONE)
					{
						SelectedGizmo = this->SelectedItems->GetItem(I)->GetGizmo();
						break;
					}
				}
			}

			ZERay Ray;
			this->Scene->GetActiveCamera()->GetScreenRay(Ray, Event->pos().x(), Event->pos().y());
	
			if (SelectedGizmo != NULL)
			{
				if (SelectedGizmo->GetMode() == ZED_GM_MOVE)
				{
					if (UndoRedoStarted == false)
					{					
						UndoRedoStarted = true;
						ZEArray<ZEClass*> SelectedItemClasses;
						ZEArray<ZEVariant> SelectedItemOldValues;

						for(int I = 0; I < SelectedItems->GetCount(); I++)
						{
							SelectedItemClasses.Add(SelectedItems->GetItem(I)->GetClass());
							SelectedItemOldValues.Add(SelectedItems->GetItem(I)->GetPosition());
						}

						GizmoUndoRedo = new ZEDGizmoUndoRedoOperation(SelectedItemClasses, SelectedGizmo->GetMode());
						GizmoUndoRedo->SetOldValues(SelectedItemOldValues);
					}

					ZEVector3 NewGizmoPosition = SelectedGizmo->MoveProjection(Ray);
					MoveSelections(NewGizmoPosition - SelectedGizmo->GetPosition());
					ParentEditor->UpdatePropertyWidgetValues();
				}

				if (SelectedGizmo->GetMode() == ZED_GM_ROTATE)
				{
					//Fill Rotation
				}

				if (SelectedGizmo->GetMode() == ZED_GM_SCALE)
				{	
					if (UndoRedoStarted == false)
					{					
						UndoRedoStarted = true;
						ZEArray<ZEClass*> SelectedItemClasses;
						ZEArray<ZEVariant> SelectedItemOldValues;

						for(int I = 0; I < SelectedItems->GetCount(); I++)
						{
							SelectedItemClasses.Add(SelectedItems->GetItem(I)->GetClass());
							SelectedItemOldValues.Add(SelectedItems->GetItem(I)->GetScale());
						}

						GizmoUndoRedo = new ZEDGizmoUndoRedoOperation(SelectedItemClasses, SelectedGizmo->GetMode());
						GizmoUndoRedo->SetOldValues(SelectedItemOldValues);
					}

					ZEVector3 NewGizmoScale = SelectedGizmo->ScaleProjection(Ray);
					ScaleSelections(NewGizmoScale - SelectedGizmo->GetPosition());
					ParentEditor->UpdatePropertyWidgetValues();
				}
			}
		}

		this->OldMousePositionX = Event->x();
		this->OldMousePositionY = Event->y();
	}
	else
	{
		ZERay Ray;
		this->Scene->GetActiveCamera()->GetScreenRay(Ray, Event->pos().x(), Event->pos().y());
		float TRay = FLT_MAX;
		ZEDGizmo* CurrentGizmo = NULL;

		for(int I = 0; I < SelectedItems->GetCount(); I++)
		{
			CurrentGizmo = this->SelectedItems->GetItem(I)->GetGizmo();
			
			if(CurrentGizmo == NULL)
				continue;

			switch(CurrentGizmo->PickAxis(Ray, TRay))
			{
				case ZED_GA_NONE:
					CurrentGizmo->SetHoveredAxis(ZED_GA_NONE);
					break;
				case ZED_GA_X_AXIS:
					CurrentGizmo->SetHoveredAxis(ZED_GA_X_AXIS);
					break;
				case ZED_GA_Y_AXIS:
					CurrentGizmo->SetHoveredAxis(ZED_GA_Y_AXIS);
					break;
				case ZED_GA_Z_AXIS:
					CurrentGizmo->SetHoveredAxis(ZED_GA_Z_AXIS);
					break;
				case ZED_GA_XY_AXIS:
					CurrentGizmo->SetHoveredAxis(ZED_GA_XY_AXIS);
					break;
				case ZED_GA_XZ_AXIS:
					CurrentGizmo->SetHoveredAxis(ZED_GA_XZ_AXIS);
					break;
				case ZED_GA_YZ_AXIS:
					CurrentGizmo->SetHoveredAxis(ZED_GA_YZ_AXIS);
					break;
				case ZED_GA_XYZ_AXIS:
					CurrentGizmo->SetHoveredAxis(ZED_GA_XYZ_AXIS);
					break;
			}
		}
	}
}

void ZEDViewPort::mouseReleaseEvent(QMouseEvent* Event)
{
	if (Event->button() == LeftButton)
	{
		this->SendInputs = false;

		if (UndoRedoStarted)
		{
			ZEArray<ZEVariant> SelectedItemNewValues;

			for(int I = 0; I < SelectedItems->GetCount(); I++)
			{
				if (GizmoUndoRedo->GetGizmoMode() == ZED_GM_MOVE)
					SelectedItemNewValues.Add(SelectedItems->GetItem(I)->GetPosition());
				if (GizmoUndoRedo->GetGizmoMode() == ZED_GM_SCALE)
					SelectedItemNewValues.Add(SelectedItems->GetItem(I)->GetScale());
				if (GizmoUndoRedo->GetGizmoMode() == ZED_GM_ROTATE)
					SelectedItemNewValues.Add(SelectedItems->GetItem(I)->GetRotation());
			}

			GizmoUndoRedo->SetNewValues(SelectedItemNewValues);

			ZEDUndoRedoManager::RegisterOperation(GizmoUndoRedo);
			UndoRedoStarted = false;
		}
	}
	if (Event->button() == RightButton)
	{
		this->SendInputs = false;
	}
}

void ZEDViewPort::keyPressEvent(QKeyEvent* Event)
{
	if(Event->key() == Key_W || Event->key() == Key_A || Event->key() == Key_S || Event->key() == Key_D)
	{
		if(MovementTimer.isActive() == false)
		{
			PressedKey = Event->key();
			MovementTimer.start(10);
		}
	}

	if (Event->key() == Key_Alt)
	{
		AltPressed = true;
	}

	if(Event->key() == Key_Z)
		ParentEditor->GoToEntityActionTriggered();

	if(Event->key() == Key_1)
		ParentEditor->SelectActionTriggered();

	if(Event->key() == Key_2)
		ParentEditor->MoveActionTriggered();

	if(Event->key() == Key_3)
		ParentEditor->RotateActionTriggered();

	if(Event->key() == Key_4)
		ParentEditor->ScaleActionTriggered();

	if(Event->key() == Key_Delete)
		ParentEditor->DeleteActionTriggered();
}

void ZEDViewPort::MovePlayer()
{
	ZEVector3 Position = Player->GetPosition();
	ZEQuaternion Rotation = Player->GetRotation();
	ZEVector3 PositionChange;
	ZEQuaternion RotationChange;
	float MetersPerSecond = 0.1f;

	if(PressedKey == Key_W)
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3(0, 0, 1));
		ZEVector3::Scale(PositionChange, PositionChange, MetersPerSecond * StepSize);
		ZEVector3::Add(Position, Position, PositionChange);
		Player->SetPosition(Position);		
	}

	if(PressedKey == Key_S)
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3(0, 0, -1));
		ZEVector3::Scale(PositionChange, PositionChange, MetersPerSecond * StepSize);
		ZEVector3::Add(Position, Position, PositionChange);
		Player->SetPosition(Position);		
	}

	if(PressedKey == Key_A)
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3(-1, 0, 0));
		ZEVector3::Scale(PositionChange, PositionChange, MetersPerSecond * StepSize);
		ZEVector3::Add(Position, Position, PositionChange);
		Player->SetPosition(Position);		
	}

	if(PressedKey == Key_D)
	{
		ZEQuaternion::VectorProduct(PositionChange, Rotation, ZEVector3(1, 0, 0));
		ZEVector3::Scale(PositionChange, PositionChange, MetersPerSecond * StepSize);
		ZEVector3::Add(Position, Position, PositionChange);
		Player->SetPosition(Position);		
	}
}

void ZEDViewPort::RotatePlayer(QPoint MousePosition)
{
	float XDiff = MousePosition.x() - OldMousePositionX;
	float YDiff = MousePosition.y() - OldMousePositionY;

	Yawn = Yawn + 0.005f * XDiff;
	Pitch = Pitch + 0.005f * YDiff;

	if (Yawn < -ZE_PI)
		Yawn = ZE_PI;
	else if (Yawn > ZE_PI)
		Yawn = -ZE_PI;

	if (Pitch < -ZE_PI_2)
		Pitch = -ZE_PI_2;
	else if (Pitch > ZE_PI_2)
		Pitch = ZE_PI_2;

	ZEQuaternion Temp, Rotation;
	ZEQuaternion::CreateFromEuler(Temp, Pitch, Yawn, Roll);
	ZEQuaternion::Normalize(Rotation, Temp);
	Player->SetRotation(Rotation);
}

void ZEDViewPort::MoveSelections(ZEVector3 MoveAmount)
{
	for(int I = 0; I < this->SelectedItems->GetCount(); I++)
		SelectedItems->GetItem(I)->MoveUsingGizmo(MoveAmount);
}

void ZEDViewPort::ScaleSelections(ZEVector3 ScaleAmount)
{
	ZEVector3::Scale(ScaleAmount, ScaleAmount, 1.0f/3.0f);//Fix

	for(int I = 0; I < this->SelectedItems->GetCount(); I++)
		SelectedItems->GetItem(I)->ScaleUsingGizmo(ScaleAmount);
}

void ZEDViewPort::RotateSelections(ZEQuaternion RotateAmount)
{
	for(int I = 0; I < this->SelectedItems->GetCount(); I++)
		SelectedItems->GetItem(I)->RotateUsingGizmo(RotateAmount);
}

void ZEDViewPort::keyReleaseEvent(QKeyEvent* Event)
{
	if(Event->key() == Key_W || Event->key() == Key_A || Event->key() == Key_S || Event->key() == Key_D)
	{
		MovementTimer.stop();
	}

	if(Event->key() == Key_Alt)
	{
		AltPressed = false;
	}
}

void ZEDViewPort::resizeEvent(QResizeEvent* ResizeEvent)
{
	zeCore->GetWindow()->SetWindowSize(this->size().width(), this->size().height());
}

void ZEDViewPort::UpdateStepSize(int StepSize)
{
	this->StepSize = StepSize;
}

void ZEDViewPort::dragEnterEvent(QDragEnterEvent* Event)
{
	if(Event->mimeData()->hasText())
		Event->acceptProposedAction();
}

void ZEDViewPort::dropEvent(QDropEvent* Event)
{
	QString DropFile = Event->mimeData()->text();
	QString DropFileExtension;

	//PickNormalAndPosition(Event->pos().x(), Event->pos().y(), this->PickedPosition, this->PickedNormal);

	for(int I = DropFile.count() - 1; I >= 0; I--)
	{
		if(DropFile.at(I).toLatin1() == '.')
			break;
		
		DropFileExtension.prepend(DropFile.at(I));
	}

	DropFile = DropFile.remove(WorkingDirectory);

	/*ZEDPlugin* CurrentPlugIn;
	ZEDExtension* CurrentExtension;
	ZEEntity*	DropEntity;

	for(int I = 0; I < this->PlugIns->GetCount(); I++)
	{
		CurrentPlugIn = this->PlugIns->GetItem(I);

		for(int J = 0; J < CurrentPlugIn->GetExtensions().GetCount(); J++ )
		{
			CurrentExtension = CurrentPlugIn->GetExtensions().GetItem(J);

			if(CurrentExtension->GetExtensionType() == ZED_ET_BROWSER)
			{
				if( QString(((ZEDBrowserExtension*)CurrentExtension)->GetFileExtensions()) == DropFileExtension )
				{
					 DropEntity = ((ZEDBrowserExtension*)CurrentExtension)->CreateCorrespondingEntity(((const char*)DropFile.toLatin1()));
	//				 DropEntity->SetPosition(*(this->PickedPosition));
	//				 this->SelectedEntity->SetCount(0);
	//				 this->SelectedEntity->Add(DropEntity);
	//				 this->SelectionChanged = true;
				}
			}
		}
	}*/
}


void ZEDViewPort::focusInEvent(QFocusEvent* Event)
{
	this->setMouseTracking(true);
}

void ZEDViewPort::focusOutEvent(QFocusEvent* Event)
{
	this->setMouseTracking(false);
}

QPaintEngine* ZEDViewPort::paintEngine()
{
	return NULL;
}

void ZEDViewPort::SetPlayerHandle(ZEPlayer* Player)
{
	this->Player = Player;
}
void ZEDViewPort::SetSceneHandle(ZEScene* Scene)
{
	this->Scene = Scene;
}

ZEDViewPort::ZEDViewPort(MapEditor* ParentEditor ,ZEArray<ZEDSelectionItem*>* SelectedItems/*, ZEArray<ZEDPlugin*>*	PlugIns*/, QString WorkingDirectory, QWidget *parent):QFrame(parent)
{
	//this->PlugIns = PlugIns;
	SendInputs = false;
	OldMousePositionX = 0;
	OldMousePositionY = 0;
	this->StepSize = 1;
	this->setAcceptDrops(true);
	this->WorkingDirectory = WorkingDirectory;
	this->ParentEditor = ParentEditor;
	this->setAttribute(WA_PaintOnScreen, true);
	this->SelectedItems = SelectedItems;
	this->AltPressed = false;

	QObject::connect(&MovementTimer, SIGNAL(timeout()), this, SLOT(MovePlayer()));
	MovementTimer.stop();
	MovementTimer.setSingleShot(false);
	Pitch = Yawn = Roll = 0.0f;
	UndoRedoStarted = false;
}
