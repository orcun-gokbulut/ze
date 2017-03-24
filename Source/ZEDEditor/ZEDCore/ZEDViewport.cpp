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
#include "ZEDInputDefinitions.h"

#include "ZEError.h"
#include "ZEMath\ZEAngle.h"
#include "ZECore\ZECore.h"
#include "ZEGame\ZEScene.h"
#include "ZEGraphics\ZEGROutput.h"
#include "ZEGraphics\ZEGRTexture.h"
#include "ZEGraphics\ZEGRRenderTarget.h"
#include "ZEGraphics\ZEGRGraphicsModule.h"
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
#include "ZEDGrid.h"
#include "ZERenderer\ZERNStageAO.h"

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
	View.HorizontalFOV = ZEAngle::ArcTan(ZEAngle::Tan(View.VerticalFOV) * View.AspectRatio);
	View.HorizontalFOVLeft = View.HorizontalFOV / 2.0f;
	View.HorizontalFOVRight = View.HorizontalFOV / 2.0f;
	View.ProjectionType = ZERN_PT_PERSPECTIVE;
	View.NearZ = 0.1f;
	View.FarZ = 2000.0f;

	ZEMatrix4x4::CreateViewTransform(View.ViewTransform, View.Position, View.Rotation);
	ZEMatrix4x4::CreatePerspectiveProjection(View.ProjectionTransform, View.VerticalFOV, View.AspectRatio, View.NearZ, View.FarZ);
	ZEMatrix4x4::Multiply(View.ViewProjectionTransform, View.ProjectionTransform, View.ViewTransform);
	ZEMatrix4x4::Inverse(View.InvViewTransform, View.ViewTransform);
	ZEMatrix4x4::Inverse(View.InvProjectionTransform, View.ProjectionTransform);
	ZEMatrix4x4::Inverse(View.InvViewProjectionTransform, View.ViewProjectionTransform);

	ZEViewFrustum::Create(ViewFrustum, View.Position, View.Rotation, View.VerticalFOV, View.AspectRatio, View.NearZ, View.FarZ);
	View.ViewVolume = &ViewFrustum;

	View.ShadowDistance = 100.0f;
	View.ShadowFadeDistance = View.ShadowDistance * 0.1f;

	Renderer.SetView(View);

	DirtyFlags.UnraiseFlags(ZED_VDF_VIEW);

	OnViewChangedEvent(this, View);

	return true;
}

bool ZEDViewport::UpdateRenderTarget()
{
	if (Window == NULL)
		return false;

	ZEGROutput* Output = Window->GetOutput();
	if (Output == NULL)
		return false;

	ZEGRTexture* OutputTexture = Output->GetTexture();
	if (OutputTexture == NULL)
		return false;

	Renderer.SetOutputTexture(OutputTexture);

	if (View.Viewport.GetWidth() == OutputTexture->GetWidth() &&
		View.Viewport.GetHeight() == OutputTexture->GetHeight())
	{
		return true;
	}

	View.Viewport.SetX(0.0f);
	View.Viewport.SetY(0.0f);
	View.Viewport.SetWidth((float)OutputTexture->GetWidth());
	View.Viewport.SetHeight((float)OutputTexture->GetHeight());

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
	Renderer.SetOutputTexture(Window->GetOutput()->GetTexture());
	Renderer.SetContext(ZEGRGraphicsModule::GetInstance()->GetMainContext());

	ZERNStageRenderDepth* StageRenderDepth = new ZERNStageRenderDepth();
	Renderer.AddStage(StageRenderDepth);

	ZERNStageGBuffer* StageGBuffer = new ZERNStageGBuffer();
	Renderer.AddStage(StageGBuffer);

	ZERNStageAO* StageAO = new ZERNStageAO();
	StageAO->SetSampleCount(ZERN_AOSC_MEDIUM);
	StageAO->SetIntensity(18.0f);
	StageAO->SetNormalBias(0.05f);
	StageAO->SetOcclusionRadius(0.5f);
	StageAO->SetDistanceThreshold(100.0f);
	Renderer.AddStage(StageAO);

	ZERNStageShadowing* StageShadowing = new ZERNStageShadowing();
	Renderer.AddStage(StageShadowing);
	
	ZERNStageLighting* StageLighting = new ZERNStageLighting();
	Renderer.AddStage(StageLighting);

	ZERNStageForward* StageForward = new ZERNStageForward();
	Renderer.AddStage(StageForward);

	ZERNStageAtmosphere* StageAtmosphere = new ZERNStageAtmosphere();
	Renderer.AddStage(StageAtmosphere);

	ZERNStageForwardTransparent* StageForwardTransparent = new ZERNStageForwardTransparent();
	Renderer.AddStage(StageForwardTransparent);

	ZERNStageParticleRendering* StageParticleRendering = new ZERNStageParticleRendering();
	Renderer.AddStage(StageParticleRendering);

	ZERNStageHDR* StageHDR = new ZERNStageHDR();
	StageHDR->SetToneMapOperator(ZERN_HTMO_UNCHARTED);
	StageHDR->SetKey(0.18f);
	StageHDR->SetAutoKey(false);
	StageHDR->SetWhiteLevel(2.0f);
	StageHDR->SetBlurTextureSize(ZERN_HBTS_HALF);
	StageHDR->SetBloomEnabled(true);
	StageHDR->SetBloomFactor(5.0f);
	StageHDR->SetBloomThreshold(10.0f);
	StageHDR->SetLuminanceMin(0.1f);
	StageHDR->SetLuminanceMax(10.0f);
	StageHDR->SetSaturation(0.7f);
	Renderer.AddStage(StageHDR);

	ZERNStageResolving* StageResolving = new ZERNStageResolving();
	Renderer.AddStage(StageResolving);

	ZERNStageForwardPostHDR* StagePostHDRForward = new ZERNStageForwardPostHDR();
	Renderer.AddStage(StagePostHDRForward);

	ZERNStageAntiAliasing* StageAntiAliasing = new ZERNStageAntiAliasing();
	Renderer.AddStage(StageAntiAliasing);

	ZERNStagePostProcess* StagePostProcess = new ZERNStagePostProcess();
	StagePostProcess->SetEnabled(false);
	Renderer.AddStage(StagePostProcess);

	ZERNStage2D* Stage2D = new ZERNStage2D();
	Renderer.AddStage(Stage2D);

	Grid = ZEDGrid::CreateInstance();
	Grid->Initialize();

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
	Grid->Destroy();

	return ZEDComponent::DeinitializeInternal();
}

void ZEDViewport::TickEvent(const ZEDTickEvent* Event)
{
	for (ZESize I = 0; I < KeyboardKeyBuffer.GetCount(); I++)
	{
		OnKeyboardKeyPressing(this, KeyboardKeyBuffer[I]);
		if (GetViewportManager() != NULL)
			GetViewportManager()->OnViewportKeyboardKeyPressing(this, KeyboardKeyBuffer[I]);
	}

	for (ZESize I = 0; I < MouseButtonBuffer.GetCount(); I++)
	{
		OnMouseButtonPressing(this, MouseButtonBuffer[I]);
		if (GetViewportManager() != NULL)
			GetViewportManager()->OnViewportMouseButtonPressing(this, MouseButtonBuffer[I]);
	}

	MousePositionDelta = ZEVector2::Zero;
}


QPaintEngine* ZEDViewport::paintEngine() const
{
	return NULL;
}

void ZEDViewport::mousePressEvent(QMouseEvent* Event)
{
	OnMouseButtonPressed(this, (ZEDMouseButton)Event->button());
	if (GetViewportManager() != NULL)
		GetViewportManager()->OnViewportMouseButtonPressed(this, (ZEDMouseButton)Event->button());

	MouseButtonBuffer.Add((ZEDMouseButton)Event->button());

	QWidget::mousePressEvent(Event);
}

void ZEDViewport::mouseReleaseEvent(QMouseEvent* Event)
{
	MouseButtonBuffer.RemoveValue((ZEDMouseButton)Event->button());
	
	OnMouseButtonReleased(this, (ZEDMouseButton)Event->button());
	if (GetViewportManager() != NULL)
		GetViewportManager()->OnViewportMouseButtonReleased(this, (ZEDMouseButton)Event->button());

	QWidget::mouseReleaseEvent(Event);
}

void ZEDViewport::mouseMoveEvent(QMouseEvent* Event)
{

	ZEVector2 Position(Event->pos().x(), Event->pos().y());

	if (LastMousePosition.x == -1)
		MousePositionDelta = ZEVector2::Zero;
	else
		MousePositionDelta = Position - LastMousePosition;

	LastMousePosition = Position;

	OnMouseMoved(this, Position);
	if (GetViewportManager() != NULL)
		GetViewportManager()->OnViewportMouseMoved(this, Position);

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

	KeyboardKeyBuffer.Add((ZEDKeyboardKey)Event->key());

	OnKeyboardKeyPressed(this, (ZEDKeyboardKey)Event->key());
	if (GetViewportManager() != NULL)
		GetViewportManager()->OnViewportKeyboardKeyPressed(this, (ZEDKeyboardKey)Event->key());

	QWidget::keyPressEvent(Event);
}

void ZEDViewport::keyReleaseEvent(QKeyEvent* Event)
{
	if (Event->isAutoRepeat())
	{
		QWidget::keyReleaseEvent(Event);
		return;
	}

	KeyboardKeyBuffer.RemoveValue((ZEDKeyboardKey)Event->key());

	OnKeyboardKeyReleased(this, (ZEDKeyboardKey)Event->key());
	if (GetViewportManager() != NULL)
		GetViewportManager()->OnViewportKeyboardKeyReleased(this, (ZEDKeyboardKey)Event->key());

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
	OnFocusGained(this);
	if (GetViewportManager() != NULL)
		GetViewportManager()->OnViewportFocusGained(this);

	QWidget::focusInEvent(Event);
}

void ZEDViewport::focusOutEvent(QFocusEvent* Event)
{
	LastMousePosition = ZEVector2(-1.0f, -1.0f);
	MousePositionDelta = ZEVector2::Zero;
	Modifiers = ZED_VKM_NONE;

	for (ZESize I = 0; I < KeyboardKeyBuffer.GetCount(); I++)
	{
		OnKeyboardKeyReleased(this, KeyboardKeyBuffer[I]);
		if (GetViewportManager() != NULL)
			GetViewportManager()->OnViewportKeyboardKeyReleased(this, KeyboardKeyBuffer[I]);
	}
	KeyboardKeyBuffer.Clear();

	for (ZESize I = 0; I < MouseButtonBuffer.GetCount(); I++)
	{
		OnMouseButtonReleased(this, MouseButtonBuffer[I]);
		if (GetViewportManager() != NULL)
			GetViewportManager()->OnViewportMouseButtonReleased(this, MouseButtonBuffer[I]);
	}
	MouseButtonBuffer.Clear();

	OnFocusLost(this);
	if (GetViewportManager() != NULL)
		GetViewportManager()->OnViewportFocusLost(this);

	QWidget::focusOutEvent(Event);
}

ZEDViewportManager* ZEDViewport::GetViewportManager() const
{
	return ViewportManager;
}

ZERNRenderer* ZEDViewport::GetRenderer() const
{
	return const_cast<ZERNRenderer*>(&Renderer);
}

const ZERNView& ZEDViewport::GetView() const
{
	return View;
}

void ZEDViewport::SetPosition(const ZEVector3& Position)
{
	this->Position = Position;
	DirtyFlags.RaiseFlags(ZED_VDF_VIEW);
	Update();
}

const ZEVector3& ZEDViewport::GetPosition() const
{
	return Position;
}

void ZEDViewport::SetRotation(const ZEQuaternion& Quaternion)
{
	Rotation = Quaternion;
	DirtyFlags.RaiseFlags(ZED_VDF_VIEW);
	Update();
}

const ZEQuaternion& ZEDViewport::GetRotation() const
{
	return Rotation;
}

void ZEDViewport::SetVerticalFOV(float FOV)
{
	VerticalFOV = FOV;
	DirtyFlags.RaiseFlags(ZED_VDF_VIEW);
	Update();
}

float ZEDViewport::GetVerticalFOV() const
{
	return VerticalFOV;
}

ZEDKeyModifiers ZEDViewport::GetKeyModifiers() const
{
	return Modifiers;
}

const ZEVector2& ZEDViewport::GetMousePosition() const
{
	return LastMousePosition;
}

const ZEVector2& ZEDViewport::GetMousePositionDelta() const
{
	return MousePositionDelta;
}

bool ZEDViewport::PreRender()
{
	if (!isVisible())
		return false;

	if (!Update())
		return false;

	ZERNPreRenderParameters Parameters;
	Parameters.Renderer = &Renderer;
	Parameters.View = &View;

	Grid->PreRender(&Parameters);
	
	OnPreRender(this, Parameters);
	if (GetViewportManager() != NULL)
		GetViewportManager()->OnViewportPreRender(this, Parameters);

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
	setAttribute(Qt::WA_PaintOnScreen, true);
	setAttribute(Qt::WA_NativeWindow, true);

	DirtyFlags.RaiseAll();
	ViewportManager = NULL;
	Window = NULL;
	Position = ZEVector3::Zero;
	Rotation = ZEQuaternion::Identity;
	Modifiers = ZED_VKM_NONE;
	VerticalFOV = ZE_PI_3;
	LastMousePosition = ZEVector2(-1.0f, -1.0f);
	MousePositionDelta = ZEVector2::Zero;
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
}

ZEDViewport::~ZEDViewport()
{
	Deinitialize();
}

void ZEDViewport::paintEvent(QPaintEvent* PaintEvent)
{

}
