//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEObject.cpp
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

#include "ZEObject.h"

#include "ZEClass.h"
#include "ZEEvent.h"
#include "ZEDS\ZEArray.h"

void ZEObject::AddEventConnection(ZEEventBase* Event) const
{
	zeDebugCheck(Event == NULL, "Cannot add event connection. Event is NULL.");

	if (EventConnections == NULL)
		EventConnections = new ZEArray<ZEEventBase*>();

	static_cast<ZEArray<ZEEventBase*>*>(EventConnections)->Add(Event);
}

void ZEObject::RemoveEventConnection(ZEEventBase* Event) const
{
	zeDebugCheck(Event == NULL, "Cannot remove event connection. Event is NULL.");
	zeDebugCheck(EventConnections == NULL, "Cannot remove event connection. There are no event connections available.");
	zeDebugCheck(EventConnections == NULL, "Cannot remove event connection. Eveint connection does not belong to this object.");
	
	static_cast<ZEArray<ZEEventBase*>*>(EventConnections)->RemoveValue(Event);
}

ZEClass* ZEObject::GetClass() const
{
	return ZEObject::Class();
}

ZEClass* ZEObject::Class()
{
	static ZEClass Class;
	return &Class;
}

ZEObject::ZEObject()
{
	EventConnections = NULL;
}

ZEObject::~ZEObject()
{
	if (EventConnections != NULL)
	{
		ZEArray<ZEEventBase*>& EventConnections = *static_cast<ZEArray<ZEEventBase*>*>(this->EventConnections);

		EventConnections.LockWrite();
		{
			for (ZESize I = 0; I < EventConnections.GetCount(); I++)
				EventConnections[I]->DisconnectObject(this);

		}
		EventConnections.UnlockWrite();
	}
}
