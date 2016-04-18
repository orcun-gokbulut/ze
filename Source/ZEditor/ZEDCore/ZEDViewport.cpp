//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDViewport.cpp
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

#include "ZEDViewport.h"

#include "ZEMath\ZEAngle.h"
#include "ZEGraphics\ZEGROutput.h"
#include "ZEGraphics\ZEGRRenderTarget.h"
#include "ZERenderer\ZERNStageGBuffer.h"
#include "ZERenderer\ZERNStageShadowing.h"
#include "ZERenderer\ZERNStagePreProcess.h"
#include "ZERenderer\ZERNStageLighting.h"
#include "ZERenderer\ZERNStageParticleRendering.h"
#include "ZERenderer\ZERNStagePostProcess.h"
#include "ZERenderer\ZERNStageHDR.h"
#include "ZERenderer\ZERNStageAntiAliasing.h"
#include "ZERenderer\ZERNStage2D.h"
#include "ZERenderer\ZERNStageOutput.h"

#include "ZEDCore.h"
#include "ZEDModule.h"
#include "ZEDViewportManager.h"
#include "ZECore\ZECore.h"

void ZEDViewport::UpdateView()
{
	//if (CameraDirtyFlags.GetFlags(ZE_CDF_VIEW))
	{
		View.Position = Position;
		View.Rotation = Rotation;
		View.Direction = View.Rotation * ZEVector3::UnitZ;
		View.U = View.Rotation * ZEVector3::UnitX;
		View.V = View.Rotation * ZEVector3::UnitY;
		View.N = View.Rotation * ZEVector3::UnitZ;


		View.AspectRatio = (float)Window->GetWidth() / (float)Window->GetHeight();
		View.VerticalFOV = GetVerticalFOV();
		View.HorizontalFOV = View.AspectRatio * View.VerticalFOV;


		ZEMatrix4x4::CreateViewTransform(View.ViewTransform, View.Position, View.Rotation);
		ZEMatrix4x4::CreatePerspectiveProjection(View.ProjectionTransform, View.VerticalFOV, View.AspectRatio, View.NearZ, View.FarZ);
		ZEMatrix4x4::Multiply(View.ViewProjectionTransform, View.ProjectionTransform, View.ViewTransform);
		ZEMatrix4x4::Inverse(View.InvViewTransform, View.ViewTransform);
		ZEMatrix4x4::Inverse(View.InvProjectionTransform, View.ProjectionTransform);
		ZEMatrix4x4::Inverse(View.InvViewProjectionTransform, View.ViewProjectionTransform);

		View.NearZ = 1.0f;
		View.FarZ = 2000.0f;

		View.ProjectionType = ZERN_PT_PERSPECTIVE;
		View.Type = ZERN_VT_VIEWPORT;
		View.Viewport = &Viewport;
		View.ViewVolume = &ViewFrustum;

		View.ShadowDistance = 100.0f;
		View.ShadowFadeDistance = View.ShadowDistance * 0.1f;

		ViewFrustum.Create(View.Position, View.Rotation, View.VerticalFOV, View.AspectRatio, View.NearZ, View.FarZ);

		//CameraDirtyFlags.UnraiseFlags(ZE_CDF_VIEW);
	}

	//return View;
}

bool ZEDViewport::InitializeSelf()
{
	Window = ZEGRWindow::WrapHandle((void*)winId());

	ZERNStageGBuffer* StageGBuffer = new ZERNStageGBuffer();
	Renderer.AddStage(StageGBuffer);

	ZERNStageShadowing* StageShadowing = new ZERNStageShadowing();
	Renderer.AddStage(StageShadowing);
	
	ZERNStagePreProcess* StagePreProcess = new ZERNStagePreProcess();
	Renderer.AddStage(StagePreProcess);

	ZERNStageLighting* StageLighting = new ZERNStageLighting();
	Renderer.AddStage(StageLighting);

	ZERNStageParticleRendering* StageParticleRendering = new ZERNStageParticleRendering();
	Renderer.AddStage(StageParticleRendering);

	ZERNStagePostProcess* StagePostProcess = new ZERNStagePostProcess();
	Renderer.AddStage(StagePostProcess);

	ZERNStageHDR* StageHDR = new ZERNStageHDR();
	StageHDR->SetToneMapOperator(ZERN_HTMO_UNCHARTED);
	StageHDR->SetKey(0.180000);
	StageHDR->SetAutoKey(false);
	StageHDR->SetWhiteLevel(1.000000);
	StageHDR->SetBlurTextureSize(ZERN_HBTS_HALF);
	StageHDR->SetBloomEnabled(true);
	StageHDR->SetBloomFactor(2.000000);
	StageHDR->SetBloomThreshold(1.000000);
	StageHDR->SetLuminanceMin(0.100000);
	StageHDR->SetLuminanceMax(10.000000);
	StageHDR->SetSaturation(0.700000);
	Renderer.AddStage(StageHDR);

	ZERNStageAntiAliasing* StageAntiAliasing = new ZERNStageAntiAliasing();
	Renderer.AddStage(StageAntiAliasing);

	ZERNStage2D* Stage2D = new ZERNStage2D();
	Renderer.AddStage(Stage2D);

	ZERNStageOutput* StageOutput = new ZERNStageOutput();
	Renderer.AddStage(StageOutput);

	if (!Renderer.Initialize())
		return false;

	ZEDCore::GetInstance()->GetEditorModule()->GetViewportManager()->RegisterViewport(this);

	return true;
}

void ZEDViewport::DeinitializeSelf()
{
	ZEDCore::GetInstance()->GetEditorModule()->GetViewportManager()->UnregisterViewport(this);
	Renderer.Deinitialize();
	Window->Destroy();
}

/*
void ZEDViewport::MoveCamera(float ElapsedTime)
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

void ZEDViewport::RotateCamera(const ZEVector2& MousePosition)
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
	MouseStartPosition = MousePosition;
}

void ZEDViewport::mousePressEvent(QMouseEvent* MouseEvent)
{
	if (Scene == NULL)
		return;

	setFocus(Qt::MouseFocusReason);

	if(MouseEvent->button() == Qt::LeftButton || MouseEvent->button() == Qt::RightButton)
	{
		this->MouseStartPosition.x = MouseEvent->x();
		this->MouseStartPosition.y = MouseEvent->y();
	}
	
	if (MouseEvent->button() == Qt::LeftButton && !(MouseEvent->buttons() & Qt::RightButton))
	{
		ZERay Ray;
		Scene->GetActiveCamera()->GetScreenRay(Ray, MouseEvent->x(), MouseEvent->y());
		float TRay = FLT_MAX;

		ZEDGizmo* Gizmo = ZEDTransformationManager::GetInstance()->GetGizmo();

		if (!Gizmo->GetVisible() || Gizmo->GetMode() == ZED_GM_NONE)
			return;

		Gizmo->SetSelectedAxis(Gizmo->PickAxis(Camera->GetView(), Ray, TRay));

		if (Gizmo->GetSelectedAxis() == ZED_GA_NONE)
			return;

		switch (Gizmo->GetMode())
		{
			case ZED_GM_MOVE:
				Gizmo->StartMoveProjection(Camera->GetView(), Ray);
				return;
			case ZED_GM_ROTATE:
				Gizmo->StartRotationProjection(Camera->GetView(), Ray);
				return;
			case ZED_GM_SCALE:
				Gizmo->StartScaleProjection(Camera->GetView(), Ray);
				return;
		}
	}
}

void ZEDViewport::mouseMoveEvent(QMouseEvent* MouseEvent)
{	
	if (Scene == NULL)
		return;

	if((MouseEvent->buttons() & Qt::RightButton))
	{
		if (MouseEvent->modifiers() & Qt::AltModifier)
		{
			ZEVector3 CameraDirection = Camera->GetFront();
			ZEVector3::Scale(CameraDirection,CameraDirection, 0.1f * (MouseStartPosition.y - MouseEvent->y()));
			Camera->SetPosition(Camera->GetPosition() + CameraDirection);
			MouseStartPosition.x = MouseEvent->x();
			MouseStartPosition.y = MouseEvent->y();
		}
		else if (MouseEvent->modifiers() == Qt::NoModifier)
		{
			MouseCurrentPosition.x = MouseEvent->x();
			MouseCurrentPosition.y = MouseEvent->y();
			RotateCamera(MouseCurrentPosition);	
		}

		return;
	}

	if(MouseEvent->buttons() & Qt::LeftButton)
	{	
		ZERay Ray;
		Scene->GetActiveCamera()->GetScreenRay(Ray, MouseEvent->x(), MouseEvent->y());
	
		ZEDGizmo* Gizmo = ZEDTransformationManager::GetInstance()->GetGizmo();

		if (Gizmo->GetVisible() && Gizmo->GetSelectedAxis() != ZED_GA_NONE)
		{
			ZEMatrix4x4 NewTransform = ZEMatrix4x4::Identity;

			if (Gizmo->GetMode() == ZED_GM_MOVE)
			{
				if (ZEDTransformationManager::GetInstance()->GetTransformType() == ZED_TT_NONE)
					ZEDTransformationManager::GetInstance()->BeginTransform(ZED_TT_TRANSLATE);

				ZEVector3 NewTranslation = (Gizmo->MoveProjection(Camera->GetView(), Ray) - Gizmo->GetPosition());
				ZEMatrix4x4::CreateTranslation(NewTransform, NewTranslation);
				ZEDTransformationManager::GetInstance()->ApplyTransform(NewTransform);
				Gizmo->SetPosition(Gizmo->GetPosition() + NewTranslation);
			}
			else if (Gizmo->GetMode() == ZED_GM_ROTATE)
			{
				if (ZEDTransformationManager::GetInstance()->GetTransformType() == ZED_TT_NONE)
					ZEDTransformationManager::GetInstance()->BeginTransform(ZED_TT_ROTATE);

				ZEQuaternion NewOrientation = Gizmo->RotationProjection(Camera->GetView(), Ray);
				ZEMatrix4x4::CreateRotation(NewTransform, NewOrientation);
				ZEDTransformationManager::GetInstance()->ApplyTransform(NewTransform);
			}
			else if (Gizmo->GetMode() == ZED_GM_SCALE)
			{
				if (ZEDTransformationManager::GetInstance()->GetTransformType() == ZED_TT_NONE)
					ZEDTransformationManager::GetInstance()->BeginTransform(ZED_TT_SCALE);

				ZEVector3 NewScale = Gizmo->ScaleProjection(Camera->GetView(), Ray);
				ZEMatrix4x4::CreateScale(NewTransform, NewScale);
				ZEDTransformationManager::GetInstance()->ApplyTransform(NewTransform);
			}
		}
		else
		{
			MouseCurrentPosition.x = MouseEvent->x();
			MouseCurrentPosition.y = MouseEvent->y();

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
		ZERay Ray =	Scene->GetActiveCamera()->GetScreenRay(MouseEvent->x(), MouseEvent->y());
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

void ZEDViewport::mouseReleaseEvent(QMouseEvent* MouseEvent)
{
	if (Scene == NULL)
		return;

	if (MouseEvent->button() == Qt::LeftButton)
	{
		ZEDTransformationManager* TransformationManager = ZEDTransformationManager::GetInstance();

		ZERay Ray = Scene->GetActiveCamera()->GetScreenRay(MouseEvent->x(), MouseEvent->y());

		ZEDGizmo* Gizmo = TransformationManager->GetGizmo();

		if (Gizmo->GetVisible() && Gizmo->GetSelectedAxis() != ZED_GA_NONE)
		{
			ZEMatrix4x4 NewTransform = ZEMatrix4x4::Identity;

			if (Gizmo->GetMode() == ZED_GM_MOVE)
			{
				ZEVector3 NewTranslation = (Gizmo->MoveProjection(Camera->GetView(), Ray) - Gizmo->GetPosition());
				ZEMatrix4x4::CreateTranslation(NewTransform, NewTranslation);
				TransformationManager->ApplyTransform(NewTransform);
				TransformationManager->EndTransform();
				//ParentEditor->UpdatePropertyWidgetValues();
			}
			else if (Gizmo->GetMode() == ZED_GM_ROTATE)
			{
				ZEQuaternion NewOrientation = Gizmo->RotationProjection(Camera->GetView(), Ray);
				ZEMatrix4x4::CreateRotation(NewTransform, NewOrientation);
				TransformationManager->ApplyTransform(NewTransform);
				TransformationManager->EndTransform();
				//ParentEditor->UpdatePropertyWidgetValues();
			}
			else if (Gizmo->GetMode() == ZED_GM_SCALE)
			{
				ZEVector3 NewScale = Gizmo->ScaleProjection(Camera->GetView(), Ray);
				ZEMatrix4x4::CreateScale(NewTransform, NewScale);
				TransformationManager->ApplyTransform(NewTransform);
				TransformationManager->EndTransform();
				//ParentEditor->UpdatePropertyWidgetValues();
			}
		}
		else
		{
			MouseCurrentPosition.x = MouseEvent->x();
			MouseCurrentPosition.y = MouseEvent->y();

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

void ZEDViewport::keyPressEvent(QKeyEvent* KeyEvent)
{
	if (!KeyEvent->isAutoRepeat())
		PressedKeyboardKeys.insert(KeyEvent->key());
}

void ZEDViewport::keyReleaseEvent(QKeyEvent* KeyEvent)
{
	if (!KeyEvent->isAutoRepeat())
		PressedKeyboardKeys.remove(KeyEvent->key());
}*/

void ZEDViewport::resizeEvent(QResizeEvent* ResizeEvent)
{
	QSize NewSize = ResizeEvent->size();

	if (Window != NULL)
		Window->WrapperResized(NewSize.width(), NewSize.height());
}

void ZEDViewport::focusInEvent(QFocusEvent* Event)
{
	this->setMouseTracking(true);
}

void ZEDViewport::focusOutEvent(QFocusEvent* Event)
{
	this->setMouseTracking(false);
}

const ZERNView& ZEDViewport::GetView()
{
	return View;
}

void ZEDViewport::SetViewMode(ZEDViewMode Mode)
{
	this->ViewMode = Mode;
}

ZEDViewMode ZEDViewport::GetViewMode()
{
	return ViewMode;
}

void ZEDViewport::SetScene(ZEDScene* Scene)
{
	this->Scene = Scene;
}

ZEDScene* ZEDViewport::GetScene()
{
	return Scene;
}

void ZEDViewport::SetPosition(const ZEVector3& Position)
{
	this->Position = Position;
}

const ZEVector3& ZEDViewport::GetPosition()
{
	return Position;
}

void ZEDViewport::SetRotation(const ZEQuaternion& Quaternion)
{
	Rotation = Quaternion;
}

const ZEQuaternion& ZEDViewport::GetRotation()
{
	return Rotation;
}

void ZEDViewport::SetVerticalFOV(float FOV)
{
	VerticalFOV = FOV;
}

float ZEDViewport::GetVerticalFOV()
{
	return VerticalFOV;
}

void ZEDViewport::Render()
{
	if (!isVisible())
		return;

	ZESize FrameID = ZECore::GetInstance()->GetFrameId();
	ZEGROutput* Output = Window->GetOutput();
	if (Output == NULL)
		return;

	ZEGRRenderTarget* RenderTarget = Output->GetRenderTarget();
	Viewport.SetX(0.0f);
	Viewport.SetY(0.0f);
	Viewport.SetWidth((float)RenderTarget->GetWidth());
	Viewport.SetHeight((float)RenderTarget->GetHeight());

	UpdateView();

	Renderer.SetOutputRenderTarget(RenderTarget);
	Renderer.SetView(View);
	Renderer.Render(0.0f);
}

void ZEDViewport::Present()
{
	if (!isVisible())
		return;

	ZEGROutput* Output = Window->GetOutput();
	if(Output == NULL)
		return;

	Output->Present();
}

ZEDViewport::ZEDViewport(QWidget* Parent) : QFrame(Parent)
{
	Window = NULL;
	ViewMode = ZED_VM_FREE;
	Position = ZEVector3::Zero;
	Rotation = ZEQuaternion::Identity;
	VerticalFOV = ZE_PI_3;
}
