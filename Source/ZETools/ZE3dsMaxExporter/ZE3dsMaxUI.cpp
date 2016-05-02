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
#include "ZE3dsMaxInteriorActions.h"
#include "ZE3dsMaxModelActions.h"
#include "ZE3dsMaxScriptCommonUtilities.ms.h"
#include "ZE3dsMaxScriptHelper.ms.h"
#include "ZE3dsMaxScriptRemove.ms.h"
#include "ZEDS/ZEString.h"
#include <imenus.h>
#include <imenuman.h>
#include <maxscript/maxscript.h>

DWORD ZE3dsMaxUI::Start()
{
	// Define Actions
	// Menus

	IMenuManager* MenuManager = GetCOREInterface()->GetMenuManager();
	ZEString ZinekEngineName("Zinek Engine");

	ActionTable* ZinekActionTable = GetCOREInterface()->GetActionManager()->FindTable(ZE3dsMax_Action_Table_ID.PartA());

	int kZinekEngineMenuBar = ZE3dsMax_Menu_Context_ID.PartA();

	bool IsMenuDefined = !(MenuManager->RegisterMenuBarContext(kZinekEngineMenuBar, ZinekEngineName));
	
	IMenu* ZinekMenu = MenuManager->FindMenu(ZinekEngineName);

	if (!IsMenuDefined || ZinekMenu == NULL)
	{
		if (ZinekMenu != NULL)
		{
			bool Result = MenuManager->UnRegisterMenu(ZinekMenu);
			ZinekMenu=NULL;
		}

		if (ZinekMenu == NULL)
		{
			ZinekMenu = GetIMenu();
			ZinekMenu->SetTitle(ZinekEngineName);
			MenuManager->RegisterMenu(ZinekMenu);

			IMenu* ModelSubMenu = GetIMenu();
			ModelSubMenu->SetTitle(ZEString("ZEModel Attributes"));

			IMenu* InteriorSubMenu = GetIMenu();
			InteriorSubMenu->SetTitle(ZEString("ZEInterior Attributes"));

			IMenu* HelperSubMenu = GetIMenu();
			HelperSubMenu->SetTitle(ZEString("ZEHelper Attributes"));

			for (ZESize I = 0; I < 4; I++)
			{
				IMenuItem* ModelSubMenuSubItem = GetIMenuItem();
				ActionItem* ModelMenuAction = ZinekActionTable->GetActionByIndex(I);

				if (ModelMenuAction != NULL)
				{
					ModelSubMenuSubItem->SetActionItem(ModelMenuAction);
					ModelSubMenu->AddItem(ModelSubMenuSubItem);
				}
			}

			IMenuItem* ModelSubMenuItem =  GetIMenuItem();
			ModelSubMenuItem->SetSubMenu(ModelSubMenu);
			ZinekMenu->AddItem(ModelSubMenuItem);

			for (ZESize I = 4; I < 6; I++)
			{
				IMenuItem* InteriorSubMenuSubItem = GetIMenuItem();
				ActionItem* InteriorMenuAction = ZinekActionTable->GetActionByIndex(I);

				if (InteriorMenuAction != NULL)
				{
					InteriorSubMenuSubItem->SetActionItem(InteriorMenuAction);
					InteriorSubMenu->AddItem(InteriorSubMenuSubItem);
				}
			}

			IMenuItem* InteriorSubMenuItem =  GetIMenuItem();
			InteriorSubMenuItem->SetSubMenu(InteriorSubMenu);
			ZinekMenu->AddItem(InteriorSubMenuItem);

			IMenuItem* HelperSubMenuSubItem = GetIMenuItem();
			HelperSubMenuSubItem->SetActionItem(ZinekActionTable->GetAction(ZECommonUtilsHelperAttributesAdd_Action_ID.PartA()));
			HelperSubMenu->AddItem(HelperSubMenuSubItem);

			IMenuItem* HelperSubMenuItem =  GetIMenuItem();
			HelperSubMenuItem->SetSubMenu(HelperSubMenu);
			ZinekMenu->AddItem(HelperSubMenuItem);

			IMenuItem* SeparatorItem = GetIMenuItem();
			SeparatorItem->ActAsSeparator();
			ZinekMenu->AddItem(SeparatorItem);

			IMenuItem* RemoveItem = GetIMenuItem();
			RemoveItem->SetActionItem(ZinekActionTable->GetAction(ZECommonUtilsRemoveAttributes_Action_ID.PartA()));
			ZinekMenu->AddItem(RemoveItem);
		}

		IMenuBarContext* ZinekMenuContext = (IMenuBarContext*)MenuManager->GetContext(kZinekEngineMenuBar);
		ZinekMenuContext->SetMenu(ZinekMenu);
		
		IMenuItem* ZinekSubMenuItem =  GetIMenuItem();
		ZinekSubMenuItem->SetSubMenu(ZinekMenu);
		MenuManager->GetMainMenuBar()->AddItem(ZinekSubMenuItem, (MenuManager->GetMainMenuBar()->NumItems() - 1));
		MenuManager->UpdateMenuBar();

// 		IQuadMenu* TranformMenu = MenuManager->FindQuadMenu(L"transform");
// 		TranformMenu->AddMenu(ZinekMenu);
	}

	ZE3dsMaxScriptCommonUtilities_ms CommonUtilitiesScript;
	ExecuteMAXScriptScript(ZEString((const char*)CommonUtilitiesScript.GetData()));

	return GUPRESULT_KEEP;
}

void ZE3dsMaxUI::Stop()
{
	IMenuManager* MenuManager = GetCOREInterface()->GetMenuManager();
	IMenu* ZinekMenu = MenuManager->FindMenu(ZEString("Zinek Engine"));

	if (ZinekMenu != NULL)
	{
		bool Result = MenuManager->UnRegisterMenu(ZinekMenu);
		MenuManager->UpdateMenuBar();
	}
}

void ZE3dsMaxUI::DeleteThis()
{
	delete this;
}

ZE3dsMaxUI::ZE3dsMaxUI()
{

}

ZE3dsMaxUI::~ZE3dsMaxUI()
{

}

int ZE3dsMaxCommonUtilsActionAddHelperAttributes::GetId()
{
	return ZECommonUtilsHelperAttributesAdd_Action_ID.PartA();
}

BOOL ZE3dsMaxCommonUtilsActionAddHelperAttributes::ExecuteAction()
{
 	ZE3dsMaxScriptHelper_ms HelperScript;
 	return ExecuteMAXScriptScript(ZEString((const char*)HelperScript.GetData()));
}

void ZE3dsMaxCommonUtilsActionAddHelperAttributes::GetButtonText(MSTR& buttonText)
{
	buttonText = ZEString("Add ZEHelper Attributes");
}

void ZE3dsMaxCommonUtilsActionAddHelperAttributes::GetMenuText(MSTR& menuText)
{
	menuText = ZEString("Add ZEHelper Attributes");
}

void ZE3dsMaxCommonUtilsActionAddHelperAttributes::GetDescriptionText(MSTR& descText)
{
	descText = ZEString("Add ZEHelper Attributes");
}

void ZE3dsMaxCommonUtilsActionAddHelperAttributes::GetCategoryText(MSTR& catText)
{
	catText = ZEString("Zinek Engine");
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
		HelperActionIcon = new MaxBmpFileIcon(ZEString("ZEToolbarIcons"), 7);

	return HelperActionIcon;
}

void ZE3dsMaxCommonUtilsActionAddHelperAttributes::DeleteThis()
{
	//delete HelperActionIcon;
	delete this;
}

ZE3dsMaxCommonUtilsActionAddHelperAttributes::ZE3dsMaxCommonUtilsActionAddHelperAttributes()
{
	HelperActionIcon = NULL;
}

int ZE3dsMaxCommonUtilsRemoveAttributes::GetId()
{
	return ZECommonUtilsRemoveAttributes_Action_ID.PartA();
}

BOOL ZE3dsMaxCommonUtilsRemoveAttributes::ExecuteAction()
{
	ZE3dsMaxScriptRemove_ms RemoveScript;
	return ExecuteMAXScriptScript(ZEString((const char*)RemoveScript.GetData()));
}

void ZE3dsMaxCommonUtilsRemoveAttributes::GetButtonText(MSTR& buttonText)
{
	buttonText = ZEString("Remove Zinek Engine Attributes");
}

void ZE3dsMaxCommonUtilsRemoveAttributes::GetMenuText(MSTR& menuText)
{
	menuText = ZEString("Remove Zinek Engine Attributes");
}

void ZE3dsMaxCommonUtilsRemoveAttributes::GetDescriptionText(MSTR& descText)
{
	descText = ZEString("Remove Zinek Engine Attributes");
}

void ZE3dsMaxCommonUtilsRemoveAttributes::GetCategoryText(MSTR& catText)
{
	catText = ZEString("Zinek Engine");
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
		RemoveActionIcon = new MaxBmpFileIcon(ZEString("ZEToolbarIcons"), 6);

	return RemoveActionIcon;
}

void ZE3dsMaxCommonUtilsRemoveAttributes::DeleteThis()
{
	//delete RemoveActionIcon;
	delete this;
}

ZE3dsMaxCommonUtilsRemoveAttributes::ZE3dsMaxCommonUtilsRemoveAttributes()
{
	RemoveActionIcon = NULL;
}

BOOL ZE3dsMaxActionCallback::ExecuteAction(int id)
{
	ActionItem* Action = ZinekActionTable->GetAction(id);

	if (Action == NULL)
		return FALSE;

	return Action->ExecuteAction();
}

ZE3dsMaxActionCallback::ZE3dsMaxActionCallback(ActionTable* Table)
{
	ZinekActionTable = Table;
}
