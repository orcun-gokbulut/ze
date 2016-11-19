//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDMenuItem.cpp
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

#include "ZEDMenuItem.h"

#include "ZEDMenu.h"
#include "ZEDMenuManager.h"
#include "ZEDCommand.h"
#include "ZEDUIUtils.h"
#include "ZEDCommandManager.h"

#include "ZEMeta\ZEEventDelegate.h"
#include "ZEML\ZEMLReader.h"
#include "ZEML\ZEMLWriter.h"

#include <QAction>

void ZEDMenuAction::Action_triggered(bool Triggered)
{
	Item->Action_Triggered();
}

ZEDMenuAction::ZEDMenuAction(ZEDMenuItem* Item) : QAction(NULL)
{
	this->Item = Item;
	connect(this, &QAction::triggered, this, &ZEDMenuAction::Action_triggered);
}

void ZEDMenuItem::Action_Triggered()
{
	QSignalBlocker Blocker(Action);

	if (GetType() == ZED_MIT_COMMAND && TargetCommand != NULL)
	{
		if (TargetCommand->GetType() == ZED_CT_BUTTON)
			TargetCommand->OnAction(TargetCommand);
		else if (TargetCommand->GetType() == ZED_CT_CHECK)
			TargetCommand->SetValue(Action->isChecked());
	}
}

void ZEDMenuItem::TargetCommand_OnUpdate(const ZEDCommand* Command)
{
	Update();
}

void ZEDMenuItem::TargetMenu_OnUpdate(const ZEDMenu* Menu)
{
	TargetName = Menu->GetName();
	Update();
}

ZEDMenu* ZEDMenuItem::GetMenu()
{
	return Menu;
}

void ZEDMenuItem::SetType(ZEDMenuItemType Type)
{
	if (this->Type == Type)
		return;

	this->Type = Type;

	Update();
}

ZEDMenuItemType ZEDMenuItem::GetType() const
{
	return Type;
}

void ZEDMenuItem::SetTargetName(const ZEString& Name)
{
	if (TargetName == Name)
		return;

	TargetName = Name;

	Update();
}

const ZEString& ZEDMenuItem::GetTargetName() const
{
	return TargetName;
}

void ZEDMenuItem::Update()
{
	if (Action == NULL)
		return;

	if (Type != ZED_MIT_COMMAND && TargetCommand != NULL)
	{
		TargetCommand->OnUpdated.DisconnectObject(this);
		TargetCommand = NULL;
	}

	switch (Type)
	{
		default:
		case ZED_MIT_NONE:
		{
			Action->setMenu(NULL);
			Action->setSeparator(true);
			Action->setVisible(false);
			break;
		}

		case ZED_MIT_COMMAND:
		{
			Action->setSeparator(false);
			Action->setMenu(NULL);

			if (GetMenu()->GetManager() == NULL)
				break;

			ZEDCommand* NewCommand = ZEDCommandManager::GetInstance()->GetCommand(GetTargetName());
			if (NewCommand != TargetCommand)
			{
				if (TargetCommand != NULL)
					TargetCommand->OnUpdated.DisconnectObject(this);

				TargetCommand = NewCommand;
				TargetCommand->OnUpdated += ZEEventDelegate<void (const ZEDCommand*)>::Create<ZEDMenuItem, &ZEDMenuItem::TargetCommand_OnUpdate>(this);
			}

			if (TargetCommand != NULL && 
				(TargetCommand->GetType() == ZED_CT_BUTTON || TargetCommand->GetType() == ZED_CT_CHECK))
			{
				Action->setText(TargetCommand->GetText().ToCString());
				Action->setIcon(ZEDUIUtils::GetIcon(TargetCommand->GetIcon()));
				Action->setToolTip(TargetCommand->GetTooltip().ToCString());
				Action->setVisible(TargetCommand->GetVisible());
				Action->setEnabled(TargetCommand->GetEnabled());
					
				if (TargetCommand->GetType() == ZED_CT_CHECK)
				{
					Action->setCheckable(true);
					Action->setChecked(TargetCommand->GetValue().GetBoolean());
				}
				else
				{
					Action->setCheckable(false);
					Action->setChecked(false);
				}
			}
			else
			{

				Action->setText(NULL);
				Action->setIcon(QIcon());
				Action->setToolTip("");
				Action->setEnabled(false);
				Action->setVisible(false);
			}

			break;
		}

		case ZED_MIT_MENU_POINTER:
		{
			Action->setSeparator(false);
			
			if (GetMenu()->GetManager() == NULL)
				break;

			ZEDMenu* NewMenu = GetMenu()->GetManager()->GetMenu(TargetName);
			if (TargetMenu != NewMenu)
			{
				if (TargetMenu != NULL)
					TargetMenu->OnUpdated.DisconnectObject(this);

				TargetMenu = NewMenu;
			}

			if (TargetMenu != NULL)
			{
				Action->setMenu(TargetMenu->GetNativeMenu());
				Action->setText(TargetMenu->GetText().ToCString());
				Action->setIcon(ZEDUIUtils::GetIcon(TargetMenu->GetIcon()));
				Action->setEnabled(true);
				Action->setVisible(true);
			}
			else
			{
				Action->setMenu(NULL);
				Action->setText(NULL);
				Action->setIcon(QIcon());
				Action->setEnabled(false);
				Action->setVisible(false);
			}

			break;
		}

		case ZED_MIT_SEPERATOR:
		{
			Action->setMenu(NULL);
			Action->setSeparator(true);
			Action->setVisible(true);
			Action->setEnabled(true);
			break;
		}
	}
}

bool ZEDMenuItem::Load(ZEMLReaderNode* ItemNode)
{
	zeCheckError(ItemNode == NULL, false, "Cannot load Menu Item. ItemNode is NULL.");
	zeCheckError(ItemNode->IsValid() == NULL, false, "Cannot load Menu Item. ItemNode is not valid.");

	Type = (ZEDMenuItemType)ItemNode->ReadUInt8("Type");
	if (Type != ZED_MIT_SEPERATOR)
		TargetName = ItemNode->ReadString("TargetName");
	else
		TargetName = "";

	return true;
}

bool ZEDMenuItem::Save(ZEMLWriterNode* ItemNode)
{
	zeCheckError(ItemNode == NULL, false, "Cannot save Menu Item. ItemsNode is NULL.");
	
	ItemNode->WriteUInt8("Type", Type);
	if (Type == ZED_MIT_SEPERATOR)
		ItemNode->WriteString("TargetName", TargetName);

	return true;
}

ZEDMenuItem::ZEDMenuItem()
{
	Action = NULL;
	Menu = NULL;
	Type = ZED_MIT_NONE;
	TargetCommand = NULL;
	TargetMenu = NULL;
}

ZEDMenuItem::~ZEDMenuItem()
{

}
