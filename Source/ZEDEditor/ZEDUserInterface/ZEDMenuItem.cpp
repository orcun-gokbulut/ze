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
#include <QMenu>

void ZEDMenuItem::SetupCommand()
{
	TargetCommand = ZEDCommandManager::GetInstance()->GetCommand(GetTargetName());
	if (TargetCommand == NULL)
		return;

	TargetCommand->OnUpdated += ZEDCommandDelegate::Create<ZEDMenuItem, &ZEDMenuItem::TargetCommand_OnUpdate>(this);

	if (TargetCommand->GetType() == ZED_CT_COMMAND || TargetCommand->GetType() == ZED_CT_TOGGLE)
	{
		Action = new QAction(this);
		Action->setCheckable(TargetCommand->GetType() == ZED_CT_TOGGLE);
		connect(Action, &QAction::triggered, this, &ZEDMenuItem::Action_triggered);
		Menu->GetNativeMenu()->addAction(Action);
	}
	else if (TargetCommand->GetType() == ZED_CT_LIST || TargetCommand->GetType() == ZED_CT_LIST_COMMAND)
	{
		Action = new QAction(this);
		SubMenu = new QMenu();
		const ZEArray<ZEString>& Items = TargetCommand->GetListItems();
		for (ZESize I = 0; I < Items.GetCount(); I++)
		{
			QAction* SubAction = SubMenu->addAction(Items[I].ToCString());
			if (TargetCommand->GetType() == ZED_CT_LIST)
			{
				SubAction->setCheckable(true);
				SubAction->setChecked(TargetCommand->GetValueIndex() == I);
			}
			QObject::connect(SubAction, &QAction::triggered, this, &ZEDMenuItem::Action_triggered);					
		}

		Action->setMenu(SubMenu);
		Menu->GetNativeMenu()->addAction(Action);
	}

	TargetCommand_OnUpdate(TargetCommand);
}

void ZEDMenuItem::SetupMenu()
{
	TargetMenu = GetMenu()->GetManager()->GetMenu(TargetName);
	if (TargetMenu == NULL)
		return;

	TargetMenu->OnUpdated += ZEEventDelegate<void (const ZEDMenu*)>::Create<ZEDMenuItem, &ZEDMenuItem::TargetMenu_OnUpdate>(this);

	Action = new QAction(this);
	Action->setMenu(TargetMenu->GetNativeMenu());
	connect(Action, &QAction::triggered, this, &ZEDMenuItem::Action_triggered);
	GetMenu()->GetNativeMenu()->addAction(Action);

	TargetMenu_OnUpdate(TargetMenu);
}

void ZEDMenuItem::SetupSeperator()
{
	if (Action == NULL)
	{
		Action = new QAction(this);
		Action->setSeparator(true);
		GetMenu()->GetNativeMenu()->addAction(Action);
	}
}

void ZEDMenuItem::Setup()
{
	if (Type != ZED_MIT_COMMAND && TargetCommand != NULL)
	{
		TargetCommand->OnUpdated.DisconnectObject(this);
		TargetCommand = NULL;
	}

	if (Type != ZED_MIT_MENU_POINTER && TargetMenu != NULL)
	{
		TargetMenu->OnUpdated.DisconnectObject(this);
		TargetMenu = NULL;
	}

	if (GetMenu() == NULL || GetMenu()->GetManager() == NULL)
		return;

	switch (Type)
	{
		default:
		case ZED_MIT_NONE:
			break;

		case ZED_MIT_COMMAND:
			SetupCommand();
			break;

		case ZED_MIT_MENU_POINTER:
			SetupMenu();
			break;

		case ZED_MIT_SEPERATOR:
			SetupSeperator();
			break;
	}
}

void ZEDMenuItem::CleanUp()
{
	if (TargetCommand != NULL)
	{
		TargetCommand->OnUpdated.DisconnectObject(this);
		TargetCommand = NULL;
	}

	if (TargetMenu != NULL)
	{
		TargetMenu->OnUpdated.DisconnectObject(this);
		TargetMenu = NULL;
	}

	if (Action != NULL)
	{
		delete Action;
		Action = NULL;
	}

	if (SubMenu != NULL)
	{
		delete SubMenu;
		SubMenu = NULL;
	}
}

void ZEDMenuItem::Action_triggered(bool)
{
	if (GetType() != ZED_MIT_COMMAND || TargetCommand == NULL)
		return;

	if (TargetCommand->GetType() == ZED_CT_COMMAND)
	{
		TargetCommand->OnAction(TargetCommand);
	}
	else if (TargetCommand->GetType() == ZED_CT_TOGGLE)
	{
		QSignalBlocker Blocker(Action);
		TargetCommand->SetValueChecked(Action->isChecked());
		TargetCommand->OnAction(TargetCommand);
	}
	else if (TargetCommand->GetType() == ZED_CT_LIST || TargetCommand->GetType() == ZED_CT_LIST_COMMAND)
	{
		QList<QAction*> SubActions = SubMenu->actions();
		for (int I = 0; I < SubActions.count(); I++)
		{
			if (TargetCommand->GetType() == ZED_CT_LIST_COMMAND)
			{
				if (sender() == SubActions[I])
				{
					TargetCommand->SetValueIndex(I);
					TargetCommand->OnAction(TargetCommand);
					break;
				}
			}
			else
			{
				QSignalBlocker SubActionBlocker(SubActions[I]);
				if (sender() == SubActions[I])
				{
					TargetCommand->SetValueIndex(I);
					TargetCommand->OnAction(TargetCommand);
					break;
				}
			}
		}
	}
}

void ZEDMenuItem::TargetCommand_OnUpdate(const ZEDCommand* Command)
{
	if (TargetCommand == NULL || Action == NULL)
		return;

	Action->setText(TargetCommand->GetText().ToCString());
	Action->setIcon(ZEDUIUtils::GetIcon(TargetCommand->GetIcon()));
	Action->setToolTip(TargetCommand->GetTooltip().ToCString());
	Action->setVisible(TargetCommand->GetVisible());
	Action->setEnabled(TargetCommand->GetEnabled());

	int Squence = 0;
	if (Command->GetShortcut().Modifiers.GetFlags(ZED_VKM_CTRL))
		Squence += Qt::CTRL;
	if (Command->GetShortcut().Modifiers.GetFlags(ZED_VKM_ALT))
		Squence += Qt::ALT;
	if (Command->GetShortcut().Modifiers.GetFlags(ZED_VKM_SHIFT))
		Squence += Qt::SHIFT;

	Squence += Command->GetShortcut().Key;
	
	Action->setShortcut(QKeySequence(Squence));

	if (TargetCommand->GetType() == ZED_CT_COMMAND || TargetCommand->GetType() == ZED_CT_TOGGLE)
	{
		Action->setCheckable(TargetCommand->GetType() == ZED_CT_TOGGLE);

		if (TargetCommand->GetType() == ZED_CT_TOGGLE)
			Action->setChecked(TargetCommand->GetValueChecked());
	}
	else if (TargetCommand->GetType() == ZED_CT_LIST || TargetCommand->GetType() == ZED_CT_LIST_COMMAND)
	{
		SubMenu->clear();
		const ZEArray<ZEString>& Items = TargetCommand->GetListItems();
		for (ZESize I = 0; I < Items.GetCount(); I++)
		{
			QAction* SubAction = SubMenu->addAction(Items[I].ToCString());
			if (TargetCommand->GetType() == ZED_CT_LIST)
			{
				SubAction->setCheckable(true);
				SubAction->setChecked(TargetCommand->GetValueIndex() == I);
			}
			QObject::connect(SubAction, &QAction::triggered, this, &ZEDMenuItem::Action_triggered);					
		}

		Action->setMenu(SubMenu);
	}
}

void ZEDMenuItem::TargetMenu_OnUpdate(const ZEDMenu* Menu)
{
	if (TargetMenu == NULL || Action == NULL)
		return;

	Action->setMenu(TargetMenu->GetNativeMenu());
	Action->setText(TargetMenu->GetText().ToCString());
	Action->setIcon(ZEDUIUtils::GetIcon(TargetMenu->GetIcon()));
	Action->setEnabled(true);
	Action->setVisible(true);
}

ZEDMenuItem::ZEDMenuItem()
{
	Action = NULL;
	Menu = NULL;
	Type = ZED_MIT_NONE;
	SubMenu = NULL;
	TargetCommand = NULL;
	TargetMenu = NULL;
}

ZEDMenuItem::~ZEDMenuItem()
{
	if (GetMenu() != NULL)
		GetMenu()->RemoveItem(this);

	CleanUp();
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

	if (GetMenu() != NULL)
		GetMenu()->Setup();
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

	if (GetMenu() != NULL)
		GetMenu()->Setup();
}

const ZEString& ZEDMenuItem::GetTargetName() const
{
	return TargetName;
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

bool ZEDMenuItem::Save(ZEMLWriterNode* ItemsNode)
{
	zeCheckError(ItemsNode == NULL, false, "Cannot save Menu Item. ItemsNode is NULL.");
	
	ZEMLWriterNode ItemNode;
	ItemsNode->OpenNode("Item", ItemNode);

	ItemNode.WriteUInt8("Type", Type);
	if (Type != ZED_MIT_SEPERATOR)
		ItemNode.WriteString("TargetName", TargetName);

	ItemNode.CloseNode();

	return true;
}

void ZEDMenuItem::Destroy()
{
	delete this;
}

ZEDMenuItem* ZEDMenuItem::CreateInstance()
{
	return new ZEDMenuItem();
}
