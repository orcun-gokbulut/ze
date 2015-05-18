//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDViewPort.h
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
#ifndef __ZED_VIEWPORT_H__
#define __ZED_VIEWPORT_H__

#include <QtGui/QFrame>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QFocusEvent>
#include <QtCore/QTimer>

#include "ZETypes.h"
#include "ZEGraphics/ZECamera.h"

enum ZEDViewMode
{
	ZED_VM_PERSPECTIVE,
	ZED_VM_FRONT,
	ZED_VM_RIGHT,
	ZED_VM_TOP
};

class ZEDViewPort : public QFrame
{
	private:
		ZEDViewMode ViewMode;
		ZECamera* Camera;
		
		QSet<ZEInt> PressedKeyboardKeys;
		QSet<ZEInt> ClickedMouseKeys;

		ZEInt StepSize;

		float Rx;
		float Ry;
		float Rz;

	protected:
		virtual void mousePressEvent(QMouseEvent* MouseEvent);
		virtual void mouseMoveEvent(QMouseEvent* MouseEvent);
		virtual void mouseReleaseEvent(QMouseEvent* MouseEvent);		
				 
		virtual void keyPressEvent(QKeyEvent* KeyEvent);
		virtual void keyReleaseEvent(QKeyEvent* KeyEvent);
				 
		virtual void resizeEvent(QResizeEvent* ResizeEvent);
				 
		virtual void dragEnterEvent(QDragEnterEvent* Event);
		virtual void dragMoveEvent(QDragMoveEvent* Event);
		virtual void dragLeaveEvent(QDragLeaveEvent* Event);
		virtual void dropEvent(QDropEvent* Event);
				 
		virtual void focusInEvent(QFocusEvent* Event);
		virtual void focusOutEvent(QFocusEvent* Event);

	public:
		void SetViewMode(ZEDViewMode Mode);
		ZEDViewMode GetViewMode();
		const ZEView& GetView();

		void SetStepSize(ZEInt StepSize);
		ZEInt GetStepSize();

		void Tick(float Time);

		ZEDViewPort(QWidget* Parent = NULL);
};

#endif
