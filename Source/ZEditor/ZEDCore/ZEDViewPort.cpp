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

void ZEDViewPort::mousePressEvent(QMouseEvent* MouseEvent)
{
	ClickedMouseKeys.insert(MouseEvent->button());
}

void ZEDViewPort::mouseMoveEvent(QMouseEvent* MouseEvent)
{

}

void ZEDViewPort::mouseReleaseEvent(QMouseEvent* MouseEvent)
{
	ClickedMouseKeys.remove(MouseEvent->button());
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

}

ZEDViewPort::ZEDViewPort(QWidget* Parent) : QFrame(Parent)
{
	ViewMode = ZED_VM_PERSPECTIVE;
	StepSize = 1;
}
