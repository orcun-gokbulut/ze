//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDViewport.h
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

#pragma once

#include "ZEDComponent.h"

#include "ZETypes.h"
#include "ZEMeta/ZEEvent.h"
#include "ZEDInputDefinitions.h"
#include "ZERenderer/ZECamera.h"
#include "ZEGraphics/ZEGRWindow.h"

#include <QWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDropEvent>
#include <QFocusEvent>
#include <QTimer>

class ZEDViewportManager;
class ZEDGrid;
enum ZEDKeyboardKey;
enum ZEDMouseButton;

class ZEDViewport : public QWidget, public ZEDComponent
{
	Q_OBJECT
	Q_DISABLE_COPY(ZEDViewport)
	friend ZEDViewportManager;
	private:
		ZEFlags								DirtyFlags;

		ZEDViewportManager*					ViewportManager;

		ZERNView							View;
		ZEViewFrustum						ViewFrustum;

		ZEVector3							Position;
		ZEQuaternion						Rotation;
		float								VerticalFOV;

		ZEGRWindow*							Window;
		ZERNRenderer						Renderer;

		ZEUInt								Modifiers;
		ZEArray<ZEDKeyboardKey>				KeyboardKeyBuffer;
		ZEArray<ZEDMouseButton>				MouseButtonBuffer;

		ZEVector2							MousePositionDelta;
		ZEVector2							LastMousePosition;

		ZEDGrid*							Grid;


		bool								UpdateView();
		bool								UpdateRenderTarget();
		bool								Update();

		virtual bool						InitializeInternal();
		virtual bool						DeinitializeInternal();

		virtual void						TickEvent(const ZEDTickEvent* Tick);

		virtual QPaintEngine*				paintEngine() const override;
		virtual void						paintEvent(QPaintEvent* PaintEvent) override;
		virtual void						mousePressEvent(QMouseEvent* MouseEvent) override;
		virtual void						mouseReleaseEvent(QMouseEvent* MouseEvent) override;
		virtual void						mouseMoveEvent(QMouseEvent* MouseEvent) override;
		virtual void						enterEvent(QEvent* MouseEvent) override;
		virtual void						leaveEvent(QEvent* MouseEvent) override;
		virtual void						keyPressEvent(QKeyEvent* KeyEvent) override;
		virtual void						keyReleaseEvent(QKeyEvent* KeyEvent) override;
		virtual void						resizeEvent(QResizeEvent* ResizeEvent) override;
		virtual void						focusInEvent(QFocusEvent* Event) override;
		virtual void						focusOutEvent(QFocusEvent* Event) override;

	public:
		ZEDViewportManager*					GetViewportManager() const;
		ZERNRenderer*						GetRenderer() const;
		const ZERNView&						GetView() const;

		void								SetPosition(const ZEVector3& Position);
		const ZEVector3&					GetPosition() const;

		void								SetRotation(const ZEQuaternion& Quaternion);
		const ZEQuaternion&					GetRotation() const;

		void								SetVerticalFOV(float FOV);
		float								GetVerticalFOV() const;

		ZEDKeyModifiers						GetKeyModifiers() const;
		const ZEVector2&					GetMousePosition() const;
		const ZEVector2&					GetMousePositionDelta() const;

		bool								PreRender();
		void								Render();
		void								Present();

		ZE_EVENT(							OnFocusGained,(ZEDViewport* Viewport));
		ZE_EVENT(							OnFocusLost,(ZEDViewport* Viewport));
		ZE_EVENT(							OnDestroying,(ZEDViewport* Viewport));
		ZE_EVENT(							OnDestroyed,(ZEDViewport* Viewport));
		ZE_EVENT(							OnViewChangedEvent,(ZEDViewport* Viewport, const ZERNView& View));
		ZE_EVENT(							OnKeyboardKeyPressed,(ZEDViewport* Viewport, ZEDKeyboardKey Key));
		ZE_EVENT(							OnKeyboardKeyPressing,(ZEDViewport* Viewport, ZEDKeyboardKey Key));
		ZE_EVENT(							OnKeyboardKeyReleased,(ZEDViewport* Viewport, ZEDKeyboardKey Key));
		ZE_EVENT(							OnMouseClick,(ZEDViewport* Viewport, ZEDMouseButton Button));
		ZE_EVENT(							OnMouseDoubleClick,(ZEDViewport* Viewport, ZEDMouseButton Button));
		ZE_EVENT(							OnMouseMoved,(ZEDViewport* Viewport, const ZEVector2& Position));
		ZE_EVENT(							OnMouseWheelMoved,(ZEDViewport* Viewport, ZEInt Value));
		ZE_EVENT(							OnMouseButtonPressed,(ZEDViewport* Viewport, ZEDMouseButton Button));
		ZE_EVENT(							OnMouseButtonPressing,(ZEDViewport* Viewport, ZEDMouseButton Button));
		ZE_EVENT(							OnMouseButtonReleased,(ZEDViewport* Viewport, ZEDMouseButton Button));

		ZE_EVENT(							OnPreRender,(ZEDViewport* Viewport, const ZERNPreRenderParameters& Parameters));

											ZEDViewport(QWidget* Parent = NULL);
		virtual								~ZEDViewport();
};
