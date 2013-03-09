//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxUI.cpp
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

#include "ZE3dsMaxUI.h"
#include "ZE3dsMaxScriptCommonUtilities.ms.h"
#include "ZE3dsMaxScriptHelper.ms.h"
#include "ZE3dsMaxScriptRemove.ms.h"
#include <imenus.h>
#include <imenuman.h>
#include <maxscrpt/maxscrpt.h>

DWORD ZE3dsMaxUI::Start()
{
	// Define Actions
	// Menus

	ZE3dsMaxScriptCommonUtilities_ms CommonUtilitiesScript;
	ExecuteMAXScriptScript((char*)CommonUtilitiesScript.GetData());

	return GUPRESULT_KEEP;
}

void ZE3dsMaxUI::Stop()
{
	
}

ZE3dsMaxUI::ZE3dsMaxUI()
{

}

ZE3dsMaxUI::~ZE3dsMaxUI()
{

}

int ZE3dsMaxCommonUtilsActionAddHelperAttributes::GetId()
{
	return ZECommonUtilsHelperAttributesAdd_Action_ID;
}

BOOL ZE3dsMaxCommonUtilsActionAddHelperAttributes::ExecuteAction()
{
 	ZE3dsMaxScriptHelper_ms HelperScript;
 	return ExecuteMAXScriptScript((char*)HelperScript.GetData());
}

void ZE3dsMaxCommonUtilsActionAddHelperAttributes::GetButtonText(MSTR& buttonText)
{
	buttonText = "Add ZEHelper Attributes";
}

void ZE3dsMaxCommonUtilsActionAddHelperAttributes::GetMenuText(MSTR& menuText)
{
	menuText = "Add ZEHelper Attributes";
}

void ZE3dsMaxCommonUtilsActionAddHelperAttributes::GetDescriptionText(MSTR& descText)
{
	descText = "Add ZEHelper Attributes";
}

void ZE3dsMaxCommonUtilsActionAddHelperAttributes::GetCategoryText(MSTR& catText)
{
	catText = "Zinek Engine";
}

BOOL ZE3dsMaxCommonUtilsActionAddHelperAttributes::IsChecked()
{
	return FALSE;
}

BOOL ZE3dsMaxCommonUtilsActionAddHelperAttributes::IsItemVisible()
{
	return TRUE;
}

BOOL ZE3dsMaxCommonUtilsActionAddHelperAttributes::IsEnabled()
{
	return TRUE;
}

MaxIcon* ZE3dsMaxCommonUtilsActionAddHelperAttributes::GetIcon()
{
	if (HelperActionIcon == NULL)
		HelperActionIcon = new MaxBmpFileIcon("ZEToolbarIcons", 7);

	return HelperActionIcon;
}

void ZE3dsMaxCommonUtilsActionAddHelperAttributes::DeleteThis()
{
	delete HelperActionIcon;
}

ZE3dsMaxCommonUtilsActionAddHelperAttributes::ZE3dsMaxCommonUtilsActionAddHelperAttributes()
{
	HelperActionIcon = NULL;
}

int ZE3dsMaxCommonUtilsRemoveAttributes::GetId()
{
	return ZECommonUtilsRemoveAttributes_Action_ID;
}

BOOL ZE3dsMaxCommonUtilsRemoveAttributes::ExecuteAction()
{
	ZE3dsMaxScriptRemove_ms RemoveScript;
	return ExecuteMAXScriptScript((char*)RemoveScript.GetData());
}

void ZE3dsMaxCommonUtilsRemoveAttributes::GetButtonText(MSTR& buttonText)
{
	buttonText = "Remove Zinek Engine Attributes";
}

void ZE3dsMaxCommonUtilsRemoveAttributes::GetMenuText(MSTR& menuText)
{
	menuText = "Remove Zinek Engine Attributes";
}

void ZE3dsMaxCommonUtilsRemoveAttributes::GetDescriptionText(MSTR& descText)
{
	descText = "Remove Zinek Engine Attributes";
}

void ZE3dsMaxCommonUtilsRemoveAttributes::GetCategoryText(MSTR& catText)
{
	catText = "Zinek Engine";
}

BOOL ZE3dsMaxCommonUtilsRemoveAttributes::IsChecked()
{
	return FALSE;
}

BOOL ZE3dsMaxCommonUtilsRemoveAttributes::IsItemVisible()
{
	return TRUE;
}

BOOL ZE3dsMaxCommonUtilsRemoveAttributes::IsEnabled()
{
	return TRUE;
}

MaxIcon* ZE3dsMaxCommonUtilsRemoveAttributes::GetIcon()
{
	if (RemoveActionIcon == NULL)
		RemoveActionIcon = new MaxBmpFileIcon("ZEToolbarIcons", 6);

	return RemoveActionIcon;
}

void ZE3dsMaxCommonUtilsRemoveAttributes::DeleteThis()
{
	delete RemoveActionIcon;
}

ZE3dsMaxCommonUtilsRemoveAttributes::ZE3dsMaxCommonUtilsRemoveAttributes()
{
	RemoveActionIcon = NULL;
}
