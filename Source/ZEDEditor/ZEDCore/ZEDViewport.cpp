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

#include "ZEDEditorCore.h"
#include "ZEDEditor.h"
#include "ZEDViewportManager.h"
#include "ZEDViewportEvent.h"

#include "ZEError.h"
#include "ZEMath\ZEAngle.h"
#include "ZECore\ZECore.h"
#include "ZEGame\ZEScene.h"
#include "ZEGraphics\ZEGROutput.h"
#include "ZEGraphics\ZEGRRenderTarget.h"
#include "ZERenderer\ZERNStageGBuffer.h"
#include "ZERenderer\ZERNStageShadowing.h"
#include "ZERenderer\ZERNStageLighting.h"
#include "ZERenderer\ZERNStageParticleRendering.h"
#include "ZERenderer\ZERNStagePostProcess.h"
#include "ZERenderer\ZERNStageHDR.h"
#include "ZERenderer\ZERNStageAntiAliasing.h"
#include "ZERenderer\ZERNStage2D.h"
#include "ZERenderer\ZERNStageOutput.h"
#include "ZERenderer\ZERNStageForward.h"
#include "ZERenderer\ZERNStageAtmosphere.h"
#include "ZERenderer\ZERNStageRenderDepth.h"
#include "ZERenderer\ZERNStageResolving.h"
#include "ZERenderer\ZERNRenderParameters.h"
#include "ZEGraphics\ZEGRGraphicsModule.h"

#define ZED_VDF_VIEW			0x01
#define ZED_VDF_VIEW_PORT		0x02

bool ZEDViewport::UpdateView()
{
	if (!DirtyFlags.GetFlags(ZED_VDF_VIEW))
		return true;
	
	ZERNView OldView;
	ZEViewFrustum OldViewFrustum = ViewFrustum;
	OldView.ViewVolume = &OldViewFrustum;

	View.Type = ZERN_VT_VIEWPORT;
	View.Entity = NULL;

	View.Position = Position;
	View.Rotation = Rotation;
	View.Direction = View.Rotation * ZEVector3::UnitZ;
	View.U = View.Rotation * ZEVector3::UnitX;
	View.V = View.Rotation * ZEVector3::UnitY;
	View.N = View.Rotation * ZEVector3::UnitZ;

	View.AspectRatio = (float)Window->GetWidth() / (float)Window->GetHeight();
	View.VerticalFOV = GetVerticalFOV();
	View.VerticalFOVTop = View.VerticalFOV / 2.0f;
	View.VerticalFOVBottom = View.VerticalFOV / 2.0f;
	View.HorizontalFOV = View.AspectRatio * View.VerticalFOV;
	View.HorizontalFOVLeft = View.HorizontalFOV / 2.0f;
	View.HorizontalFOVRight = View.HorizontalFOV / 2.0f;
	View.ProjectionType = ZERN_PT_PERSPECTIVE;
	View.NearZ = 1.0f;
	View.FarZ = 2000.0f;

	ZEMatrix4x4::CreateViewTransform(View.ViewTransform, View.Position, View.Rotation);
	ZEMatrix4x4::CreatePerspectiveProjection(View.ProjectionTransform, View.VerticalFOV, View.AspectRatio, View.NearZ, View.FarZ);
	ZEMatrix4x4::Multiply(View.ViewProjectionTransform, View.ProjectionTransform, View.ViewTransform);
	ZEMatrix4x4::Inverse(View.InvViewTransform, View.ViewTransform);
	ZEMatrix4x4::Inverse(View.InvProjectionTransform, View.ProjectionTransform);
	ZEMatrix4x4::Inverse(View.InvViewProjectionTransform, View.ViewProjectionTransform);

	ViewFrustum.Create(View.Position, View.Rotation, View.VerticalFOV, View.AspectRatio, View.NearZ, View.FarZ);
	View.ViewVolume = &ViewFrustum;

	View.ShadowDistance = 100.0f;
	View.ShadowFadeDistance = View.ShadowDistance * 0.1f;

	Renderer.SetView(View);

	DirtyFlags.UnraiseFlags(ZED_VDF_VIEW);

	ZEDViewportChangedEvent Event;
	Event.Viewport = this;
	Event.OldView = &OldView;
	Event.View = &View;

	RaiseEvent(&Event);

	return true;
}

bool ZEDViewport::UpdateRenderTarget()
{
	if (Window == NULL)
		return false;

	ZEGROutput* Output = Window->GetOutput();
	if (Output == NULL)
		return false;

	ZEGRRenderTarget* RenderTarget = Output->GetRenderTarget();
	Renderer.SetOutputRenderTarget(RenderTarget);

	if (RenderTarget == NULL)
		return false;

	if (View.Viewport.GetWidth() == RenderTarget->GetWidth() &&
		View.Viewport.GetHeight() == RenderTarget->GetHeight())
	{
		return true;
	}

	View.Viewport.SetX(0.0f);
	View.Viewport.SetY(0.0f);
	View.Viewport.SetWidth((float)RenderTarget->GetWidth());
	View.Viewport.SetHeight((float)RenderTarget->GetHeight());

	DirtyFlags.RaiseFlags(ZED_VDF_VIEW);

	return true;
}

bool ZEDViewport::Update()
{
	if (!UpdateRenderTarget())
		return false;

	if (!UpdateView())
		return false;

	return true;
}

bool ZEDViewport::InitializeInternal()
{
	if (!ZEDComponent::InitializeInternal())
		return false;

	zeCheckError(ViewportManager == NULL, false, "Cannot initialize. Viewport is not registered with a Viewport Manager.");
	Window = ZEGRWindow::WrapHandle((void*)winId());
	Renderer.SetOutputRenderTarget(Window->GetOutput()->GetRenderTarget());
	Renderer.SetContext(ZEGRGraphicsModule::GetInstance()->GetMainContext());

	ZERNStageGBuffer* StageGBuffer = new ZERNStageGBuffer();
	Renderer.AddStage(StageGBuffer);

	ZERNStageShadowing* StageShadowing = new ZERNStageShadowing();
	Renderer.AddStage(StageShadowing);
	
	ZERNStageLighting* StageLighting = new ZERNStageLighting();
	Renderer.AddStage(StageLighting);

	ZERNStageAtmosphere* StageAtmosphere = new ZERNStageAtmosphere();
	Renderer.AddStage(StageAtmosphere);

	ZERNStageRenderDepth* StageRenderDepth = new ZERNStageRenderDepth();
	Renderer.AddStage(StageRenderDepth);

	ZERNStageForward* StageForward = new ZERNStageForward();
	Renderer.AddStage(StageForward);

	ZERNStageForwardTransparent* StageForwardTransparent = new ZERNStageForwardTransparent();
	Renderer.AddStage(StageForwardTransparent);

	ZERNStageParticleRendering* StageParticleRendering = new ZERNStageParticleRendering();
	Renderer.AddStage(StageParticleRendering);

	ZERNStagePostProcess* StagePostProcess = new ZERNStagePostProcess();
	Renderer.AddStage(StagePostProcess);

	ZERNStageHDR* StageHDR = new ZERNStageHDR();
	StageHDR->SetToneMapOperator(ZERN_HTMO_UNCHARTED);
	StageHDR->SetKey(0.18f);
	StageHDR->SetAutoKey(false);
	StageHDR->SetWhiteLevel(2.000000f);
	StageHDR->SetBlurTextureSize(ZERN_HBTS_HALF);
	StageHDR->SetBloomEnabled(true);
	StageHDR->SetBloomFactor(2.000000f);
	StageHDR->SetBloomThreshold(6.000000f);
	StageHDR->SetLuminanceMin(0.100000f);
	StageHDR->SetLuminanceMax(10.000000f);
	StageHDR->SetSaturation(0.700000f);
	Renderer.AddStage(StageHDR);

	ZERNStageResolving* StageResolving = new ZERNStageResolving();
	Renderer.AddStage(StageResolving);

	ZERNStageForwardPostHDR* StagePostHDRForward = new ZERNStageForwardPostHDR();
	Renderer.AddStage(StagePostHDRForward);

	ZERNStageAntiAliasing* StageAntiAliasing = new ZERNStageAntiAliasing();
	Renderer.AddStage(StageAntiAliasing);

	ZERNStage2D* Stage2D = new ZERNStage2D();
	Renderer.AddStage(Stage2D);

	ZERNStageOutput* StageOutput = new ZERNStageOutput();
	Renderer.AddStage(StageOutput);

	if (!Renderer.Initialize())
		return false;

	return true;
}


bool ZEDViewport::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();
	Renderer.Deinitialize();
	Window->Destroy();
	Window = NULL;

	return ZEDComponent::DeinitializeInternal();
}

void ZEDViewport::TickEvent(const ZEDTickEvent* Event)
{
	for (ZESize I = 0; I < KeyboardEvents.GetCount(); I++)
	{
		KeyboardEvents[I].Modifiers = Modifiers;
		KeyboardEvents[I].Unacuqire();
		RaiseEvent(&KeyboardEvents[I]);
	}

	for (ZESize I = 0; I < MouseEvents.GetCount(); I++)
	{
		MouseEvents[I].Delta = MouseDelta;
		MouseEvents[I].Modifiers = Modifiers;
		MouseEvents[I].Unacuqire();
		RaiseEvent(&MouseEvents[I]);
	}

	MouseDelta = ZEVector2::Zero;
}

void ZEDViewport::mousePressEvent(QMouseEvent* Event)
{
	ZEDViewportMouseEvent MouseEvent;
	MouseEvent.Viewport = this;
	MouseEvent.Position.x = Event->pos().x();// = LastMousePosition;
	MouseEvent.Position.y = Event->pos().y();
	MouseEvent.Delta = ZEVector2::Zero;
	MouseEvent.Button = (ZEDIViewportMouseButton)Event->button();
	MouseEvent.Modifiers = Modifiers;
	MouseEvent.Type = ZED_VIET_BUTTON_PRESSING;
	MouseEvents.Add(MouseEvent);
	MouseEvent.Type = ZED_VIET_BUTTON_PRESSED;

	RaiseEvent(&MouseEvent);

	QWidget::mousePressEvent(Event);
}

void ZEDViewport::mouseMoveEvent(QMouseEvent* Event)
{
	ZEDViewportMouseEvent MouseEvent;
	MouseEvent.Type = ZED_VIET_MOUSE_MOVED;
	MouseEvent.Viewport = this;
	MouseEvent.Position.x = Event->pos().x();
	MouseEvent.Position.y = Event->pos().y();

	if (LastMousePosition.x == -1)
	{
		MouseDelta = ZEVector2::Zero;
	}
	else
	{
		MouseDelta = MouseEvent.Position - LastMousePosition;
	}

	MouseEvent.Button = (ZEDIViewportMouseButton)Event->button();
	MouseEvent.Modifiers = Modifiers;
	MouseEvent.Delta = MouseDelta;

	LastMousePosition = MouseEvent.Position;

	RaiseEvent(&MouseEvent);

	QWidget::mouseMoveEvent(Event);
}

void ZEDViewport::mouseReleaseEvent(QMouseEvent* Event)
{
	ZEDViewportMouseEvent MouseEvent;
	MouseEvent.Type = ZED_VIET_BUTTON_RELEASED;
	MouseEvent.Viewport = this;
	MouseEvent.Position = LastMousePosition;
	MouseEvent.Delta = MouseDelta;
	MouseEvent.Button = (ZEDIViewportMouseButton)Event->button();
	MouseEvent.Modifiers = Modifiers;

	for (ZESize I = 0; I < MouseEvents.GetCount(); I++)
	{
		if (MouseEvents[I].Button == MouseEvent.Button)
		{
			MouseEvents.Remove(I);
			break;
		}
	}

	RaiseEvent(&MouseEvent);

	QWidget::mouseMoveEvent(Event);
}

void ZEDViewport::enterEvent(QEvent* Event)
{
	LastMousePosition = ZEVector2(-1.0f, -1.0f);
	QWidget::enterEvent(Event);
}

void ZEDViewport::leaveEvent(QEvent* Event)
{
	LastMousePosition = ZEVector2(-1.0f, -1.0f);
	QWidget::leaveEvent(Event);
}

void ZEDViewport::keyPressEvent(QKeyEvent* Event)
{
	if (Event->isAutoRepeat())
	{
		QWidget::keyReleaseEvent(Event);
		return;
	}

	if (Event->key() == Qt::Key_Shift)
		Modifiers |= ZED_VKM_SHIFT;

	if (Event->key() == Qt::Key_Control)
		Modifiers |= ZED_VKM_CTRL;

	if (Event->key() == Qt::Key_Alt)
		Modifiers |= ZED_VKM_ALT;

	if (Event->key() == Qt::Key_Meta)
		Modifiers |= ZED_VKM_WINDOWS;

	ZEDViewportKeyboardEvent KeyboardEvent;
	KeyboardEvent.Viewport = this;
	KeyboardEvent.Key = (ZEDViewportKeyboardKey)Event->key();
	KeyboardEvent.VirtualKey = Event->nativeVirtualKey();
	KeyboardEvent.Text = Event->text().toUtf8().begin();
	KeyboardEvent.Modifiers = Modifiers;
	KeyboardEvent.Type = ZED_VIET_BUTTON_PRESSING;
	KeyboardEvents.Add(KeyboardEvent);
	KeyboardEvent.Type = ZED_VIET_BUTTON_PRESSED;

	RaiseEvent(&KeyboardEvent);

	QWidget::keyPressEvent(Event);
}

void ZEDViewport::keyReleaseEvent(QKeyEvent* Event)
{
	if (Event->isAutoRepeat())
	{
		QWidget::keyReleaseEvent(Event);
		return;
	}

	ZEDViewportKeyboardEvent KeyboardEvent;
	KeyboardEvent.Viewport = this;
	KeyboardEvent.Type = ZED_VIET_BUTTON_RELEASED;
	KeyboardEvent.Key = (ZEDViewportKeyboardKey)Event->key();
	KeyboardEvent.VirtualKey = Event->nativeVirtualKey();
	KeyboardEvent.Text = Event->text().toUtf8().begin();
	KeyboardEvent.Modifiers = Modifiers;

	for (ZESize I = 0; I < KeyboardEvents.GetCount(); I++)
	{
		if (KeyboardEvents[I].Key == KeyboardEvent.Key)
		{
			KeyboardEvents.Remove(I);
			break;
		}
	}

	RaiseEvent(&KeyboardEvent);
	
	if (Event->key() == Qt::Key_Shift)
		Modifiers &= ~ZED_VKM_SHIFT;

	if (Event->key() == Qt::Key_Control)
		Modifiers &= ~ZED_VKM_CTRL;

	if (Event->key() == Qt::Key_Alt)
		Modifiers &= ~ZED_VKM_ALT;

	if (Event->key() == Qt::Key_Meta)
		Modifiers &= ~ZED_VKM_WINDOWS;

	QWidget::keyReleaseEvent(Event);
}

void ZEDViewport::resizeEvent(QResizeEvent* Event)
{
	QSize NewSize = Event->size();

	if (Window != NULL)
		Window->WrapperResized(NewSize.width(), NewSize.height());

	QWidget::resizeEvent(Event);
}

void ZEDViewport::focusInEvent(QFocusEvent* Event)
{
	QWidget::focusInEvent(Event);
}

void ZEDViewport::focusOutEvent(QFocusEvent* Event)
{
	LastMousePosition = ZEVector2(-1.0f, -1.0f);
	MouseDelta = ZEVector2::Zero;
	Modifiers = ZED_VKM_NONE;

	for (ZESize I = 0; I < KeyboardEvents.GetCount(); I++)
	{
		KeyboardEvents[I].Type = ZED_VIET_BUTTON_RELEASED;
		RaiseEvent(&KeyboardEvents[I]);
	}
	KeyboardEvents.Clear();

	for (ZESize I = 0; I < MouseEvents.GetCount(); I++)
	{
		MouseEvents[I].Type = ZED_VIET_BUTTON_RELEASED;
		RaiseEvent(&MouseEvents[I]);
	}

	MouseEvents.Clear();

	QWidget::focusOutEvent(Event);
}

ZEDViewportManager* ZEDViewport::GetViewportManager()
{
	return ViewportManager;
}

ZERNRenderer* ZEDViewport::GetRenderer()
{
	return &Renderer;
}

const ZERNView& ZEDViewport::GetView()
{
	return View;
}

void ZEDViewport::SetPosition(const ZEVector3& Position)
{
	this->Position = Position;
	DirtyFlags.RaiseFlags(ZED_VDF_VIEW);
	Update();
}

const ZEVector3& ZEDViewport::GetPosition()
{
	return Position;
}

void ZEDViewport::SetRotation(const ZEQuaternion& Quaternion)
{
	Rotation = Quaternion;
	DirtyFlags.RaiseFlags(ZED_VDF_VIEW);
	Update();
}

const ZEQuaternion& ZEDViewport::GetRotation()
{
	return Rotation;
}

void ZEDViewport::SetVerticalFOV(float FOV)
{
	VerticalFOV = FOV;
	DirtyFlags.RaiseFlags(ZED_VDF_VIEW);
	Update();
}

float ZEDViewport::GetVerticalFOV()
{
	return VerticalFOV;
}

bool ZEDViewport::PreRender()
{
	if (!isVisible())
		return false;

	if (Window == NULL)
		return false;

	ZEGROutput* Output = Window->GetOutput();
	if (Output == NULL)
		return false;

	ZEGRRenderTarget* RenderTarget = Output->GetRenderTarget();
	if (RenderTarget == NULL)
		return false;

	if (!Update())
		return false;

	Renderer.SetOutputRenderTarget(RenderTarget);
	Renderer.SetView(View);


	ZERNPreRenderParameters Parameters;
	Parameters.Renderer = &Renderer;
	Parameters.View = &View;

	ZEDViewportRenderEvent Event;
	Event.Viewport = this;
	Event.PreRenderParameters = &Parameters;

	RaiseEvent(&Event);

	return true;
}

void ZEDViewport::Render()
{
	Renderer.Render();
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

ZEDViewport::ZEDViewport(QWidget* Parent) : QWidget(Parent)
{
	DirtyFlags.RaiseAll();
	ViewportManager = NULL;
	Window = NULL;
	Position = ZEVector3::Zero;
	Rotation = ZEQuaternion::Identity;
	Modifiers = ZED_VKM_NONE;
	VerticalFOV = ZE_PI_3;
	LastMousePosition = ZEVector2(-1.0f, -1.0f);
	MouseDelta = ZEVector2::Zero;
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
}

void ZEDViewport::paintEvent(QPaintEvent* PaintEvent)
{

}
