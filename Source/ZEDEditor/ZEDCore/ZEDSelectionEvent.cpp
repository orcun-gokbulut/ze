//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSelectionEvent.cpp
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

#include "ZEDSelectionEvent.h"

#define ZED_SEF_ADDED_LIST_DIRTY		0x01
#define ZED_SEF_REMOVED_LIST_DIRTY		0x02

void ZEDSelectionEvent::SetManager(ZEDSelectionManager* Manager)
{
	this->Manager = Manager;
}

void ZEDSelectionEvent::SetType(ZEDSelectionEventType Type)
{
	this->Type = Type;
}

ZEDSelectionEventType ZEDSelectionEvent::GetType() const
{
	return Type;
}

void ZEDSelectionEvent::SetList(const ZEArray<ZEDObjectWrapper*>* List)
{
	DirtyFlags.RaiseAll();
	NewList = List;
}


void ZEDSelectionEvent::SetOldList(const ZEArray<ZEDObjectWrapper*>* List)
{
	DirtyFlags.RaiseAll();
	OldList = List;
}

void ZEDSelectionEvent::SetFocusedObject(ZEDObjectWrapper* FocusedObject)
{
	FocusedObject = FocusedObject;
}

void ZEDSelectionEvent::SetOldFocusedObject(ZEDObjectWrapper* UnfocusedObject)
{
	OldFocusedObject = UnfocusedObject;
}

const ZEArray<ZEDObjectWrapper*>& ZEDSelectionEvent::GetList() const
{
	return *NewList;
}

const ZEArray<ZEDObjectWrapper*>& ZEDSelectionEvent::GetOldList() const
{
	return *OldList;
}

const ZEArray<ZEDObjectWrapper*>& ZEDSelectionEvent::GetAddedlist() const
{
	if (NewList != NULL && OldList != NULL &&
		DirtyFlags.GetFlags(ZED_SEF_ADDED_LIST_DIRTY))
	{
		AddedList.Clear();
		for (ZESize I = 0; I < NewList->GetCount(); I++)
		{
			if (OldList->Exists((*NewList)[I]))
				continue;

			AddedList.Add((*NewList)[I]);
		}
	}

	DirtyFlags.UnraiseFlags(ZED_SEF_ADDED_LIST_DIRTY);

	return AddedList;
}

const ZEArray<ZEDObjectWrapper*>& ZEDSelectionEvent::GetRemovedlist() const
{
	if (NewList != NULL && OldList != NULL &&
		DirtyFlags.GetFlags(ZED_SEF_REMOVED_LIST_DIRTY))
	{
		RemovedList.Clear();
		for (ZESize I = 0; I < OldList->GetCount(); I++)
		{
			if (NewList->Exists((*OldList)[I]))
				continue;

			RemovedList.Add((*OldList)[I]);
		}
	}

	DirtyFlags.UnraiseFlags(ZED_SEF_REMOVED_LIST_DIRTY);

	return RemovedList;
}

ZEDObjectWrapper* ZEDSelectionEvent::GetFocusedObject()
{
	return FocusedObject;
}

ZEDObjectWrapper* ZEDSelectionEvent::GetOldFocusedObject()
{
	return OldFocusedObject;
}

ZEDSelectionEvent::ZEDSelectionEvent()
{
	Type = ZED_SET_NONE;
	DirtyFlags.RaiseAll();
	NewList = NULL;
	OldList = NULL;
	FocusedObject = NULL;
	OldFocusedObject = NULL;
}
