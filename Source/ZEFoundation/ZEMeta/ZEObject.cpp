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

struct ZEEventConnection 
{
	ZEMTEventBase*					Event;
	ZESize							Count;
};

void ZEObject::CloneEventConnections(ZEObject* Other)
{
	if (EventConnections != NULL)
	{
		ZEArray<ZEEventConnection>& EventConnectionsCasted = *static_cast<ZEArray<ZEEventConnection>*>(EventConnections);

		EventConnectionsCasted.LockWrite();
		for (ZESize I = 0; I < EventConnectionsCasted.GetCount(); I++)
			EventConnectionsCasted[I].Event->DisconnectObject(this);
		EventConnectionsCasted.Clear();
		EventConnectionsCasted.UnlockWrite();
	}

	if (Other->EventConnections != NULL)
	{
		ZEArray<ZEEventConnection>& OtherEventConnectionsCasted = *static_cast<ZEArray<ZEEventConnection>*>(Other->EventConnections);
		OtherEventConnectionsCasted.LockRead();
		if (OtherEventConnectionsCasted.GetCount() != 0)
		{
			if (EventConnections == NULL)
				EventConnections = new ZEArray<ZEEventConnection>();

			ZEArray<ZEEventConnection>& EventConnectionsCasted = *static_cast<ZEArray<ZEEventConnection>*>(EventConnections);
			EventConnectionsCasted.LockWrite();
			{
				EventConnectionsCasted = OtherEventConnectionsCasted;
				for (ZESize I = 0; I < OtherEventConnectionsCasted.GetCount(); I++)
					EventConnectionsCasted[I].Event->CloneConnections(Other, this);
			}
			EventConnectionsCasted.UnlockWrite();
		}
		OtherEventConnectionsCasted.UnlockRead();
	}
}

void ZEObject::AddEventConnection(ZEMTEventBase* Event)
{
	zeDebugCheck(Event == NULL, "Cannot add event connection. Event is NULL.");
	
	if (EventConnections == NULL)
		EventConnections = new ZEArray<ZEEventConnection>();

	ZEArray<ZEEventConnection>& EventConnectionsCasted = *static_cast<ZEArray<ZEEventConnection>*>(EventConnections);
	EventConnectionsCasted.LockWrite();
	{
		for (ZESize I = 0; I < EventConnectionsCasted.GetCount(); I++)
		{
			if (EventConnectionsCasted[I].Event != Event)
				continue;

			EventConnectionsCasted[I].Count++;

			EventConnectionsCasted.UnlockWrite();
			return;
		}

		ZEEventConnection* Connection = EventConnectionsCasted.Add();
		Connection->Event = Event;
		Connection->Count = 1;
	}
	EventConnectionsCasted.UnlockWrite();
}

void ZEObject::RemoveEventConnection(ZEMTEventBase* Event)
{
	zeDebugCheck(Event == NULL, "Cannot remove event connection. Event is NULL.");
		
	ZEArray<ZEEventConnection>& EventConnectionsCasted = *static_cast<ZEArray<ZEEventConnection>*>(EventConnections);
	EventConnectionsCasted.LockWrite();
	{
		for (ZESize I = 0; I < EventConnectionsCasted.GetCount(); I++)
		{
			if (EventConnectionsCasted[I].Event != Event)
				continue;
			
			EventConnectionsCasted[I].Count--;

			if (EventConnectionsCasted[I].Count == 0)
				EventConnectionsCasted.Remove(I);
			
			break;
		}
	}
	EventConnectionsCasted.UnlockWrite();
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

ZEObject& ZEObject::operator=(const ZEObject& Object)
{
	CloneEventConnections(const_cast<ZEObject*>(&Object));
	return *this;
}

ZEObject::ZEObject()
{
	EventConnections = NULL;
}

ZEObject::ZEObject(const ZEObject& Object)
{
	EventConnections = NULL;
	CloneEventConnections(const_cast<ZEObject*>(&Object));
}

ZEObject::~ZEObject()
{
	if (EventConnections != NULL)
	{
		ZEArray<ZEEventConnection>& EventConnectionsCasted = *static_cast<ZEArray<ZEEventConnection>*>(EventConnections);
		EventConnectionsCasted.LockWrite();
		{
			for (ZESize I = 0; I < EventConnectionsCasted.GetCount(); I++)
				EventConnectionsCasted[I].Event->DisconnectObject(this);

			EventConnectionsCasted.Clear();
		}
		EventConnectionsCasted.UnlockWrite();

		delete static_cast<ZEArray<ZEEventConnection>*>(EventConnections);
	}
}
