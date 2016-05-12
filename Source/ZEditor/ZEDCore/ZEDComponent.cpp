//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDComponent.cpp
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

#include "ZEDComponent.h"

#include "ZEDSelectionEvent.h"
#include "ZEDViewportEvent.h"
#include "ZEDModule.h"

void ZEDComponent::EventReceived(const ZEDEvent* Event)
{
	if(ZEClass::IsDerivedFrom(ZEDTickEvent::Class(), Event->GetClass()))
		TickEvent(static_cast<const ZEDTickEvent*>(Event));
	else if (ZEClass::IsDerivedFrom(ZEDSelectionEvent::Class(), Event->GetClass()))
		SelectionEvent(static_cast<const ZEDSelectionEvent*>(Event));
	else if(ZEClass::IsDerivedFrom(ZEDViewportKeyboardEvent::Class(), Event->GetClass()))
		ViewportKeyboardEvent(static_cast<const ZEDViewportKeyboardEvent*>(Event));
	else if(ZEClass::IsDerivedFrom(ZEDViewportMouseEvent::Class(), Event->GetClass()))
		ViewportMouseEvent(static_cast<const ZEDViewportMouseEvent*>(Event));
	else if(ZEClass::IsDerivedFrom(ZEDViewportChangedEvent::Class(), Event->GetClass()))
		ViewportChangedEvent(static_cast<const ZEDViewportChangedEvent*>(Event));
	else if(ZEClass::IsDerivedFrom(ZEDViewportRenderEvent::Class(), Event->GetClass()))
		ViewportRenderEvent(static_cast<const ZEDViewportRenderEvent*>(Event));
}

void ZEDComponent::ObjectModifiedEvent(const ZEDEvent* Event)
{

}

void ZEDComponent::SelectionEvent(const ZEDSelectionEvent* Event)
{

}

void ZEDComponent::TransformationEvent(const ZEDTransformationEvent* Event)
{

}

void ZEDComponent::TickEvent(const ZEDTickEvent* Event)
{

}

void ZEDComponent::ViewportKeyboardEvent(const ZEDViewportKeyboardEvent* Event)
{

}

void ZEDComponent::ViewportMouseEvent(const ZEDViewportMouseEvent* Event)
{

}

void ZEDComponent::ViewportChangedEvent(const ZEDViewportChangedEvent* Event)
{

}

void ZEDComponent::ViewportRenderEvent(const ZEDViewportRenderEvent* Event)
{

}

void ZEDComponent::RaiseEvent(const ZEDEvent* Event)
{
	if (Module == NULL)
		return;

	Module->DistributeEvent(Event);
}

ZEDComponent::ZEDComponent()
{
	Module = NULL;
}

ZEDComponent::~ZEDComponent()
{
	
}

ZEDModule* ZEDComponent::GetModule()
{
	return Module;
}

void ZEDComponent::Destroy()
{
	delete this;
}
