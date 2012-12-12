//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxInteriorActions.cpp
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

#include "ZE3dsMaxInteriorActions.h"
#include <maxscrpt/maxscrpt.h>
#include "ZE3dsMaxInteriorScriptRoom_ms.h"
#include "ZE3dsMaxInteriorScriptDoor_ms.h"
#include <Commctrl.h>

int ZE3dsMaxInteriorActionAddRoomAttributes::GetId()
{
	return ZEInteriorRoomAttributesAdd_Action_ID;
}

BOOL ZE3dsMaxInteriorActionAddRoomAttributes::ExecuteAction()
{
	ZE3dsMaxInteriorScriptRoom_ms RoomScript;
	return ExecuteMAXScriptScript((char*)RoomScript.GetData());
}

void ZE3dsMaxInteriorActionAddRoomAttributes::GetButtonText(MSTR& buttonText)
{
	buttonText = "Add ZEInterior Room Attributes";
}

void ZE3dsMaxInteriorActionAddRoomAttributes::GetMenuText(MSTR& menuText)
{
	menuText = "Add ZEInterior Room Attributes";
}

void ZE3dsMaxInteriorActionAddRoomAttributes::GetDescriptionText(MSTR& descText)
{
	descText = "Add ZEInterior Room Attributes";
}

void ZE3dsMaxInteriorActionAddRoomAttributes::GetCategoryText(MSTR& catText)
{
	catText = "Zinek Engine";
}

BOOL ZE3dsMaxInteriorActionAddRoomAttributes::IsChecked()
{
	return FALSE;
}

BOOL ZE3dsMaxInteriorActionAddRoomAttributes::IsItemVisible()
{
	return TRUE;
}

BOOL ZE3dsMaxInteriorActionAddRoomAttributes::IsEnabled()
{
	return TRUE;
}

MaxIcon* ZE3dsMaxInteriorActionAddRoomAttributes::GetIcon()
{
	if (RoomActionIcon == NULL)
		RoomActionIcon = new MaxBmpFileIcon("ZEToolbarIcons", 4);

	return RoomActionIcon;
}

void ZE3dsMaxInteriorActionAddRoomAttributes::DeleteThis()
{
	delete RoomActionIcon;
}

ZE3dsMaxInteriorActionAddRoomAttributes::ZE3dsMaxInteriorActionAddRoomAttributes()
{
	RoomActionIcon = NULL;
}

int ZE3dsMaxInteriorActionAddDoorAttributes::GetId()
{
	return ZEInteriorDoorAttributesAdd_Action_ID;
}

BOOL ZE3dsMaxInteriorActionAddDoorAttributes::ExecuteAction()
{
	ZE3dsMaxInteriorScriptDoor_ms DoorScript;
	return ExecuteMAXScriptScript((char*)DoorScript.GetData());
}

void ZE3dsMaxInteriorActionAddDoorAttributes::GetButtonText(MSTR& buttonText)
{
	buttonText = "Add ZEInterior Door Attributes";
}

void ZE3dsMaxInteriorActionAddDoorAttributes::GetMenuText(MSTR& menuText)
{
	menuText = "Add ZEInterior Door Attributes";
}

void ZE3dsMaxInteriorActionAddDoorAttributes::GetDescriptionText(MSTR& descText)
{
	descText = "Add ZEInterior Door Attributes";
}

void ZE3dsMaxInteriorActionAddDoorAttributes::GetCategoryText(MSTR& catText)
{
	catText = "Zinek Engine";
}

BOOL ZE3dsMaxInteriorActionAddDoorAttributes::IsChecked()
{
	return FALSE;
}

BOOL ZE3dsMaxInteriorActionAddDoorAttributes::IsItemVisible()
{
	return TRUE;
}

BOOL ZE3dsMaxInteriorActionAddDoorAttributes::IsEnabled()
{
	return TRUE;
}

MaxIcon* ZE3dsMaxInteriorActionAddDoorAttributes::GetIcon()
{
	if (DoorActionIcon == NULL)
		DoorActionIcon = new MaxBmpFileIcon("ZEToolbarIcons", 5);

	return DoorActionIcon;
}

void ZE3dsMaxInteriorActionAddDoorAttributes::DeleteThis()
{
	delete DoorActionIcon;
}

ZE3dsMaxInteriorActionAddDoorAttributes::ZE3dsMaxInteriorActionAddDoorAttributes()
{
	DoorActionIcon = NULL;
}
