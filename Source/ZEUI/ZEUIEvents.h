//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIEvents.h
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
#ifndef __ZE_UI_EVENTS_H__
#define __ZE_UI_EVENTS_H__

#include "ZEMath\ZEVector.h"
#include "ZEFastDelegate.h"

enum ZEUIMouseKey
{
	ZE_UI_MOUSE_BUTTON_NONE		= 0,
	ZE_UI_MOUSE_BUTTON_LEFT		= 1,
	ZE_UI_MOUSE_BUTTON_RIGHT	= 2,
	ZE_UI_MOUSE_BUTTON_MIDDLE	= 3,
};

// Mouse Events
typedef fastdelegate::FastDelegate2<ZEUIMouseKey, const ZEVector2&, void>	ZEUIEventMouseClicked;
typedef fastdelegate::FastDelegate1<const ZEVector2&, void>					ZEUIEventMouseDoubleClicked;
typedef fastdelegate::FastDelegate2<ZEUIMouseKey, const ZEVector2&, void>	ZEUIEventMouseButtonPressed;
typedef fastdelegate::FastDelegate2<ZEUIMouseKey, const ZEVector2&, void>	ZEUIEventMouseButtonReleased;
typedef fastdelegate::FastDelegate1<const ZEVector2&, void>					ZEUIEventMouseHovered;
typedef fastdelegate::FastDelegate1<const ZEVector2&, void>					ZEUIEventMouseEntered;
typedef fastdelegate::FastDelegate1<const ZEVector2&, void>					ZEUIEventMouseLeft;
typedef fastdelegate::FastDelegate1<const ZEVector2&, void>					ZEUIEventMouseMoved;

// Keyboard Events
typedef fastdelegate::FastDelegate1<unsigned char, void>								ZEUIEventKeyPressed;
typedef fastdelegate::FastDelegate1<unsigned char, void>								ZEUIEventKeyReleased;

// Component Event
typedef fastdelegate::FastDelegate0<void>									ZEUIEventDestroyed;
typedef fastdelegate::FastDelegate0<void>									ZEUIEventCreated;
typedef fastdelegate::FastDelegate0<void>									ZEUIEventFocusLost;
typedef fastdelegate::FastDelegate0<void>									ZEUIEventFocusGained;

// Windows
typedef fastdelegate::FastDelegate1<const ZEVector2&, void>					ZEUIEventWindowMoved;
typedef fastdelegate::FastDelegate2<const ZEVector2&, void>					ZEUIEventWindowResized;
typedef fastdelegate::FastDelegate1<bool, void>								ZEUIEventWindowClosed;
typedef fastdelegate::FastDelegate0<void>									ZEUIEventWindowMinimized;

#endif




