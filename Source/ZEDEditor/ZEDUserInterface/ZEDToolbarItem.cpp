//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDToolbarItem.cpp
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

#include "ZEDToolbarItem.h"

#include "ZEDToolbar.h"
#include "ZEDToolbarManager.h"
#include "ZEDMenu.h"
#include "ZEDCommand.h"
#include "ZEDCommandManager.h"
#include "ZEDUIUtils.h"

#include "ZEMeta/ZEEventDelegate.h"
#include "ZEML/ZEMLReader.h"
#include "ZEML/ZEMLWriter.h"

#include <QAction>
#include <QMenu>

void ZEDToolbarAction::Action_triggered(bool Triggered)
{
	Item->Action_Triggered();
}

void ZEDToolbarAction::SubAction_triggered(bool Triggered)
{
	Item->SubAction_Triggered(static_cast<QAction*>(sender()));
}

ZEDToolbarAction::ZEDToolbarAction(ZEDToolbarItem* Item) : QAction(NULL)
{
	this->Item = Item;
	connect(this, &QAction::triggered, this, &ZEDToolbarAction::Action_triggered);
}

void ZEDToolbarItem::Action_Triggered()
{
	QSignalBlocker Blocker(Action);

	if (GetType() == ZED_TIT_COMMAND && TargetCommand != NULL)
	{
		if (TargetCommand->GetType() == ZED_CT_COMMAND)
		{
			TargetCommand->OnAction(TargetCommand);
		}
		else if (TargetCommand->GetType() == ZED_CT_TOGGLE)
		{
			TargetCommand->SetValue(Action->isChecked());
			TargetCommand->OnAction(TargetCommand);
		}
	}
}

void ZEDToolbarItem::SubAction_Triggered(QAction* Action)
{
	TargetCommand->SetValue(ZEString(Action->text().toStdString()));

	if (TargetCommand->GetType() == ZED_CT_LIST)
	{
		const ZEArray<ZEString>& Items = TargetCommand->GetListItems();
		for (ZESize I = 0; I < Items.GetCount(); I++)
		{
			QAction* SubAction = SubMenu->actions().at(I);
			QSignalBlocker Blocker(SubAction);
			SubAction->setChecked(TargetCommand->GetValue().GetString() == Items[I]);
		}
	}

	TargetCommand->OnAction(TargetCommand);
}

void ZEDToolbarItem::TargetCommand_OnUpdate(const ZEDCommand* Command)
{
	Update();
}

void ZEDToolbarItem::TargetMenu_OnUpdate(const ZEDMenu* Menu)
{
	TargetName = Menu->GetName();
	Update();
}

ZEDToolbar* ZEDToolbarItem::GetToolbar()
{
	return Toolbar;
}

void ZEDToolbarItem::SetType(ZEDToolbarItemType Type)
{
	if (this->Type == Type)
		return;

	this->Type = Type;

	Update();
}

ZEDToolbarItemType ZEDToolbarItem::GetType() const
{
	return Type;
}

void ZEDToolbarItem::SetTargetName(const ZEString& Name)
{
	if (TargetName == Name)
		return;

	TargetName = Name;

	Update();
}

const ZEString& ZEDToolbarItem::GetTargetName() const
{
	return TargetName;
}

void ZEDToolbarItem::Update()
{
	if (Action == NULL)
		return;

	if (Type != ZED_TIT_COMMAND && TargetCommand != NULL)
	{
		TargetCommand->OnUpdated.DisconnectObject(this);
		TargetCommand = NULL;
	}

	switch (Type)
	{
		default:
		case ZED_TIT_NONE:
		{
			Action->setMenu(NULL);
			Action->setSeparator(true);
			Action->setVisible(false);
			break;
		}

		case ZED_TIT_COMMAND:
		{
			Action->setSeparator(false);
			Action->setMenu(NULL);

			if (GetToolbar()->GetManager() == NULL)
				break;

			ZEDCommand* NewCommand = ZEDCommandManager::GetInstance()->GetCommand(GetTargetName());
			if (NewCommand != TargetCommand)
			{
				if (TargetCommand != NULL)
					TargetCommand->OnUpdated.DisconnectObject(this);

				TargetCommand = NewCommand;
				TargetCommand->OnUpdated += ZEEventDelegate<void (const ZEDCommand*)>::Create<ZEDToolbarItem, &ZEDToolbarItem::TargetCommand_OnUpdate>(this);
			}

			if (TargetCommand != NULL)
			{
				Action->setText(TargetCommand->GetText().ToCString());
				Action->setIcon(ZEDUIUtils::GetIcon(TargetCommand->GetIcon()));
				Action->setToolTip(TargetCommand->GetTooltip().ToCString());
				Action->setVisible(TargetCommand->GetVisible());
				Action->setEnabled(TargetCommand->GetEnabled());

				if (TargetCommand->GetType() == ZED_CT_COMMAND)
				{
					Action->setCheckable(false);
					Action->setChecked(false);
				}
				else if (TargetCommand->GetType() == ZED_CT_TOGGLE)
				{
					Action->setCheckable(true);
					Action->setChecked(TargetCommand->GetValue().GetBoolean());
				}
				else if (TargetCommand->GetType() == ZED_CT_LIST || TargetCommand->GetType() == ZED_CT_LIST_COMMAND)
				{
					Action->setCheckable(false);
					Action->setChecked(false);

					if (SubMenu == NULL)
						SubMenu = new QMenu();
					
					SubMenu->clear();

					const ZEArray<ZEString>& Items = TargetCommand->GetListItems();
					for (ZESize I = 0; I < Items.GetCount(); I++)
					{
						QAction* SubAction = SubMenu->addAction(Items[I].ToCString());
						QObject::connect(SubAction, &QAction::triggered, Action, &ZEDToolbarAction::SubAction_triggered);
						
						if (TargetCommand->GetType() == ZED_CT_LIST)
						{
							SubAction->setCheckable(true);
							if (TargetCommand->GetValue().GetString() == Items[I])
								SubAction->setChecked(true);
						}
					}

					Action->setMenu(SubMenu);				
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

		case ZED_TIT_SEPERATOR:
		{
			Action->setMenu(NULL);
			Action->setSeparator(true);
			Action->setVisible(true);
			Action->setEnabled(true);
			break;
		}
	}
}

bool ZEDToolbarItem::Load(ZEMLReaderNode* ItemNode)
{
	zeCheckError(ItemNode == NULL, false, "Cannot load Menu Item. ItemNode is NULL.");
	zeCheckError(ItemNode->IsValid() == NULL, false, "Cannot load Menu Item. ItemNode is not valid.");

	Type = (ZEDToolbarItemType)ItemNode->ReadUInt8("Type");
	if (Type != ZED_TIT_SEPERATOR)
		TargetName = ItemNode->ReadString("TargetName");
	else
		TargetName = "";

	return true;
}

bool ZEDToolbarItem::Save(ZEMLWriterNode* ItemNode)
{
	zeCheckError(ItemNode == NULL, false, "Cannot save Menu Item. ItemsNode is NULL.");
	
	ItemNode->WriteUInt8("Type", Type);
	if (Type == ZED_TIT_SEPERATOR)
		ItemNode->WriteString("TargetName", TargetName);

	return true;
}

ZEDToolbarItem::ZEDToolbarItem()
{
	Action = NULL;
	Toolbar = NULL;
	SubMenu = NULL;
	Type = ZED_TIT_NONE;
	TargetCommand = NULL;
	TargetMenu = NULL;
}

ZEDToolbarItem::~ZEDToolbarItem()
{
	if (SubMenu != NULL)
		delete SubMenu;
}
