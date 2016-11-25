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
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QToolBar>


// ZEDToolbarItem
//////////////////////////////////////////////////////////////////////////////////////

void ZEDToolbarItem::SetupCommand()
{
	TargetCommand = ZEDCommandManager::GetInstance()->GetCommand(GetTargetName());
	if (TargetCommand == NULL)
		return;

	TargetCommand->OnUpdated += ZEDCommandDelegate::Create<ZEDToolbarItem, &ZEDToolbarItem::TargetCommand_OnUpdate>(this);

	if (TargetCommand->GetType() == ZED_CT_COMMAND || TargetCommand->GetType() == ZED_CT_TOGGLE)
	{
		Action = new QAction(this);
		Action->setCheckable(TargetCommand->GetType() == ZED_CT_TOGGLE);
		connect(Action, &QAction::triggered, this, &ZEDToolbarItem::Action_triggered);
		Toolbar->GetNativeToolbar()->addAction(Action);
	}
	else if (TargetCommand->GetType() == ZED_CT_LIST)
	{
		ComboBox = new QComboBox();
		ComboBox->setMaximumWidth(100);
		connect(ComboBox, (void (QComboBox::*)(int))&QComboBox::currentIndexChanged, this, &ZEDToolbarItem::ComboBox_currentIndexChanged);
		Action = GetToolbar()->Toolbar->insertWidget(Action, ComboBox);
	}
	else if (TargetCommand->GetType() == ZED_CT_INPUT_NUMBER || TargetCommand->GetType() == ZED_CT_INPUT_FLOAT || TargetCommand->GetType() == ZED_CT_INPUT_TEXT)
	{
		LineEdit = new QLineEdit();
		LineEdit->setMaximumWidth(100);
		connect(LineEdit, &QLineEdit::textChanged, this, &ZEDToolbarItem::LineEdit_textChanged);
		Action = GetToolbar()->Toolbar->insertWidget(Action, LineEdit);
	}

	TargetCommand_OnUpdate(TargetCommand);
}

void ZEDToolbarItem::SetupSeperator()
{
	if (Action == NULL)
	{
		Action = new QAction(this);
		Action->setSeparator(true);
		GetToolbar()->GetNativeToolbar()->addAction(Action);
	}
}

void ZEDToolbarItem::Setup()
{
	if (Type != ZED_TIT_COMMAND && TargetCommand != NULL)
	{
		TargetCommand->OnUpdated.DisconnectObject(this);
		TargetCommand = NULL;
	}

	if (GetToolbar() == NULL || GetToolbar()->GetManager() == NULL)
		return;

	switch (Type)
	{
		default:
		case ZED_TIT_NONE:
			break;

		case ZED_TIT_COMMAND:
			SetupCommand();
			break;

		case ZED_TIT_SEPERATOR:
			SetupSeperator();
			break;
	}
}

void ZEDToolbarItem::CleanUp()
{
	if (TargetCommand != NULL)
	{
		TargetCommand->OnUpdated.DisconnectObject(this);
		TargetCommand = NULL;
	}

	if (Action != NULL)
	{
		delete Action;
		Action = NULL;
	}

	if (Label != NULL)
	{
		delete Label;
		Label = NULL;
	}
}

void ZEDToolbarItem::Action_triggered()
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
			TargetCommand->SetValueChecked(Action->isChecked());
			TargetCommand->OnAction(TargetCommand);
		}
		else if (TargetCommand->GetType() == ZED_CT_LIST)
		{
			TargetCommand->SetValueIndex(ComboBox->currentIndex());
			TargetCommand->OnAction(TargetCommand);
		}
		else if (TargetCommand->GetType() == ZED_CT_INPUT_NUMBER)
		{
			TargetCommand->SetValueNumber(LineEdit->text().toInt());
			TargetCommand->OnAction(TargetCommand);
		}
		else if (TargetCommand->GetType() == ZED_CT_INPUT_FLOAT)
		{
			TargetCommand->SetValueFloat(LineEdit->text().toDouble());
			TargetCommand->OnAction(TargetCommand);
		}
		else if (TargetCommand->GetType() == ZED_CT_INPUT_TEXT)
		{
			TargetCommand->SetValueText((ZEString)LineEdit->text().toStdString());
			TargetCommand->OnAction(TargetCommand);
		}
	}
}

void ZEDToolbarItem::ComboBox_currentIndexChanged(int Index)
{
	TargetCommand->SetValueIndex(Index);
	TargetCommand->OnAction(TargetCommand);
}

void ZEDToolbarItem::LineEdit_textChanged(const QString&)
{
	if (TargetCommand->GetType() == ZED_CT_INPUT_NUMBER)
	{
		bool Correct = false;
		LineEdit->text().toInt(&Correct);
		if (Correct)
		{
			LineEdit->setStyleSheet("* { background-color: rgba(0, 0, 0, 0); }");
			Action_triggered();
		}
		else
		{
			LineEdit->setStyleSheet("* { background-color: rgba(255, 200, 210, 255); }");
		}
	}
	else if (TargetCommand->GetType() == ZED_CT_INPUT_FLOAT)
	{
		bool Correct;
		LineEdit->text().toDouble(&Correct);
		if (Correct)
		{
			LineEdit->setStyleSheet("* { background-color: rgba(0, 0, 0, 0); }");
			Action_triggered();
		}
		else
		{
			LineEdit->setStyleSheet("* { background-color: rgba(255, 200, 210, 255); }");
		}
	}
	else
	{
		Action_triggered();
	}
}

void ZEDToolbarItem::TargetCommand_OnUpdate(const ZEDCommand* Command)
{
	if (TargetCommand == NULL || Action == NULL)
		return;

	if (TargetCommand->GetType() == ZED_CT_COMMAND || TargetCommand->GetType() == ZED_CT_TOGGLE)
	{
		Action->setText(TargetCommand->GetText().ToCString());
		Action->setIcon(ZEDUIUtils::GetIcon(TargetCommand->GetIcon()));
		Action->setToolTip(TargetCommand->GetTooltip().ToCString());
		Action->setVisible(TargetCommand->GetVisible());
		Action->setEnabled(TargetCommand->GetEnabled());
		Action->setCheckable(TargetCommand->GetType() == ZED_CT_TOGGLE);

		if (TargetCommand->GetType() == ZED_CT_TOGGLE)
			Action->setChecked(TargetCommand->GetValueChecked());
	}
	else if (TargetCommand->GetType() == ZED_CT_LIST)
	{
		QSignalBlocker ComboBoxBlocker(ComboBox);

		ComboBox->clear();
		const ZEArray<ZEString>& Items = TargetCommand->GetListItems();
		for (ZESize I = 0; I < Items.GetCount(); I++)
			ComboBox->addItem(Items[I].ToCString());

		ComboBox->setCurrentIndex(TargetCommand->GetValueIndex());

		Action->setVisible(TargetCommand->GetVisible());
		Action->setEnabled(TargetCommand->GetEnabled());
	}
	else if (TargetCommand->GetType() == ZED_CT_INPUT_NUMBER || TargetCommand->GetType() == ZED_CT_INPUT_FLOAT || TargetCommand->GetType() == ZED_CT_INPUT_TEXT)
	{
		Action->setVisible(TargetCommand->GetVisible());
		Action->setEnabled(TargetCommand->GetEnabled());

		QSignalBlocker LineEditBlocker(LineEdit);
		if (TargetCommand->GetType() == ZED_CT_INPUT_NUMBER)
			LineEdit->setText(QString::number(TargetCommand->GetValueNumber()));
		else if (TargetCommand->GetType() == ZED_CT_INPUT_FLOAT)
			LineEdit->setText(QString::number(TargetCommand->GetValueFloat(), 'f', 2));
		else
			LineEdit->setText(TargetCommand->GetValueText().ToCString());
	}
}

ZEDToolbarItem::ZEDToolbarItem()
{
	Action = NULL;
	Toolbar = NULL;
	Label = NULL;
	ComboBox = NULL;
	LineEdit = NULL;
	Type = ZED_TIT_NONE;
	TargetCommand = NULL;
}

ZEDToolbarItem::~ZEDToolbarItem()
{
	CleanUp();

	if (GetToolbar() != NULL)
		GetToolbar()->RemoveItem(this);
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

	if (GetToolbar() != NULL)
		GetToolbar()->Setup();
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

	if (GetToolbar() != NULL)
		GetToolbar()->Setup();
}

const ZEString& ZEDToolbarItem::GetTargetName() const
{
	return TargetName;
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

bool ZEDToolbarItem::Save(ZEMLWriterNode* ItemsNode)
{
	zeCheckError(ItemsNode == NULL, false, "Cannot save Menu Item. ItemsNode is NULL.");
	
	ZEMLWriterNode ItemNode;
	ItemsNode->OpenNode("Item", ItemNode);

	ItemNode.WriteUInt8("Type", Type);
	if (Type != ZED_TIT_SEPERATOR)
		ItemNode.WriteString("TargetName", TargetName);

	ItemNode.CloseNode();

	return true;
}

void ZEDToolbarItem::Destroy()
{
	delete this;
}

ZEDToolbarItem* ZEDToolbarItem::CreateInstance()
{
	return new ZEDToolbarItem();
}
