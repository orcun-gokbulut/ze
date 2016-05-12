//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDViewportEvent.cpp
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

#include "ZEDViewportEvent.h"

// ZEDViewportEvent
//////////////////////////////////////////////////////////////////////////////////////

ZEDViewportEvent::ZEDViewportEvent()
{
	Viewport = NULL;
	Type = ZED_VIET_NONE;
}

ZEDViewportEvent::~ZEDViewportEvent()
{

}

ZEDViewport* ZEDViewportEvent::GetViewport() const
{
	return Viewport;
}


ZEDViewportEventType ZEDViewportEvent::GetType() const
{
	return Type;
}


// ZEDViewportMouseEvent
//////////////////////////////////////////////////////////////////////////////////////

ZEDViewportMouseEvent::ZEDViewportMouseEvent()
{
	Delta = ZEVector2::Zero;
	Position = ZEVector2::Zero;
	Modifiers = ZED_VKM_NONE;
	Button = ZED_VMB_NONE;
}

ZEDViewportMouseEvent::~ZEDViewportMouseEvent()
{

}


const ZEVector2& ZEDViewportMouseEvent::GetDelta() const
{
	return Delta;
}

const ZEVector2& ZEDViewportMouseEvent::GetPosition() const
{
	return Position;
}

ZEUInt ZEDViewportMouseEvent::GetModifiers() const
{
	return Modifiers;
}

ZEDIViewportMouseButton ZEDViewportMouseEvent::GetButton() const
{
	return Button;
}


// ZEDViewportKeyboardEvent
//////////////////////////////////////////////////////////////////////////////////////

ZEDViewportKeyboardEvent::ZEDViewportKeyboardEvent()
{
	Key = ZED_VKK_ESCAPE;
	Modifiers = ZED_VKM_NONE;
	VirtualKey = 0;
}

ZEDViewportKeyboardEvent::~ZEDViewportKeyboardEvent()
{

}

ZEDViewportKeyboardKey ZEDViewportKeyboardEvent::GetKey() const
{
	return Key;
}

ZEUInt ZEDViewportKeyboardEvent::GetModifiers() const
{
	return Modifiers;
}

ZEUInt ZEDViewportKeyboardEvent::GetVirtualKey() const
{
	return VirtualKey;
}

const ZEString& ZEDViewportKeyboardEvent::GetText() const
{
	return Text;
}


// ZEDViewportKeyboardEvent
//////////////////////////////////////////////////////////////////////////////////////

ZEDViewportChangedEvent::ZEDViewportChangedEvent()
{
	View = NULL;
	OldView = NULL;
}

ZEDViewportChangedEvent::~ZEDViewportChangedEvent()
{

}

const ZERNView& ZEDViewportChangedEvent::GetView() const
{
	return *View;
}

const ZERNView& ZEDViewportChangedEvent::GetOldView() const
{
	return *OldView;
}


// ZEDViewportKeyboardEvent
//////////////////////////////////////////////////////////////////////////////////////

ZEDViewportRenderEvent::ZEDViewportRenderEvent()
{
	PreRenderParameters = NULL;
}

ZEDViewportRenderEvent::~ZEDViewportRenderEvent()
{

}

const ZERNPreRenderParameters& ZEDViewportRenderEvent::GetPreRenderParameters() const
{
	return *PreRenderParameters;
}
