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

#include "ZETypes.h"

#include "ZEDViewportInput.h"
#include "ZERenderer/ZECamera.h"
#include "ZEGraphics/ZEGRWindow.h"

#include <QFrame>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDropEvent>
#include <QFocusEvent>
#include <QTimer>

class ZEScene;

class ZEDViewport : public QFrame, public ZEInitializable
{
	Q_OBJECT
	private:
		ZEFlags								DirtyFlags;

		ZERNView							View;
		ZEViewFrustum						ViewFrustum;
		ZEScene*							Scene;

		ZEVector3							Position;
		ZEQuaternion						Rotation;
		float								VerticalFOV;

		ZEGRWindow*							Window;
		ZERNRenderer						Renderer;

		ZEVector2							MouseDelta;
		ZEVector2							LastMousePosition;

		ZEUInt								Modifiers;
		ZEArray<ZEDViewportKeyboardEvent>	KeyboardEvents;
		ZEArray<ZEDViewportMouseEvent>		MouseEvents;

		bool								UpdateView();
		bool								UpdateRenderTarget();
		bool								Update();

		virtual bool						InitializeSelf();
		virtual void						DeinitializeSelf();

		ZEDViewportKeyboardEvent			Convert(QKeyEvent* KeyEvent);
		ZEDViewportMouseEvent				Convert(QMouseEvent* MouseEvent);

		virtual void						mousePressEvent(QMouseEvent* MouseEvent);
		virtual void						mouseMoveEvent(QMouseEvent* MouseEvent);
		virtual void						mouseReleaseEvent(QMouseEvent* MouseEvent);
		virtual void						enterEvent(QEvent* MouseEvent);
		virtual void						leaveEvent(QEvent* MouseEvent);
		virtual void						keyPressEvent(QKeyEvent* KeyEvent);
		virtual void						keyReleaseEvent(QKeyEvent* KeyEvent);
		virtual void						resizeEvent(QResizeEvent* ResizeEvent);
		virtual void						focusInEvent(QFocusEvent* Event);
		virtual void						focusOutEvent(QFocusEvent* Event);

	public:
		ZERNRenderer*						GetRenderer();
		const ZERNView&						GetView();

		void								SetPosition(const ZEVector3& Position);
		const ZEVector3&					GetPosition();

		void								SetRotation(const ZEQuaternion& Quaternion);
		const ZEQuaternion&					GetRotation();

		void								SetVerticalFOV(float FOV);
		float								GetVerticalFOV();

		void								SetScene(ZEScene* Scene);
		ZEScene*							GetScene();

		void								Tick();
		bool								PreRender();
		void								Render();
		void								Present();

											ZEDViewport(QWidget* Parent = NULL);
};
