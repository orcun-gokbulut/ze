//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDCustomizeUIWindow.cpp
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

#include "ZEDCustomizeUIWindow.h"
#include "ui_ZEDCustomizeUIWindow.h"

#include "ZEDCommandManager.h"
#include "ZEDMainWindow.h"
#include "ZEDMenu.h"
#include "ZEDMenuItem.h"
#include "ZEDMenuManager.h"
#include "ZEDToolbar.h"
#include "ZEDToolbarItem.h"
#include "ZEDToolbarManager.h"
#include "ZEDUIUtils.h"


void ZEDCustomizeUIWindow::UpdateElements()
{
	QSignalBlocker cmbNameBlocker(Form->cmbElements);

	if (Form->radElementTypeMenu->isChecked())
	{
		Form->cmbElements->clear();
		const ZEArray<ZEDMenu*>& Menus = MainWindow->GetMenuManager()->GetMenus();
		for (ZESize I = 0; I < Menus.GetCount(); I++)
			Form->cmbElements->addItem(Menus[I]->GetName().ToCString());
	}
	else
	{
		Form->cmbElements->clear();
		const ZEArray<ZEDToolbar*>& Toolbars = MainWindow->GetToolbarManager()->GetToolbars();
		for (ZESize I = 0; I < Toolbars.GetCount(); I++)
			Form->cmbElements->addItem(Toolbars[I]->GetName().ToCString());
	}

	UpdateElement();
}

void ZEDCustomizeUIWindow::UpdateElement()
{
	QSignalBlocker txtElementNameBlocker(Form->txtElementName);
	QSignalBlocker txtElementTextBlocker(Form->txtElementText);
	QSignalBlocker chkElementVisibleBlocker(Form->chkElementVisible);

	if (IsElementTypeMenu())
	{
		if (GetMenu() == NULL)
			return;

		Form->txtElementName->setText(GetMenu()->GetName().ToCString());
		Form->txtElementText->setText(GetMenu()->GetText().ToCString());
	}
	else
	{
		if (GetToolbar() == NULL)
			return;

		Form->txtElementName->setText(GetToolbar()->GetName().ToCString());
		Form->txtElementText->setText(GetMenu()->GetText().ToCString());
		Form->chkElementVisible->setChecked(GetToolbar()->GetVisible());
	}
}

void ZEDCustomizeUIWindow::UpdateItems()
{
	QSignalBlocker lstCommandsBlocker(Form->lstItems);

	if (Form->radItemTypeCommand->isChecked())
	{
		ZEArray<ZEDCommand*> Commands;
		if (Form->cmbItemCategory->currentText() == "<All>")
			Commands = ZEDCommandManager::GetInstance()->GetCommands();
		else
			Commands = ZEDCommandManager::GetInstance()->GetCommands(Form->cmbItemCategory->currentText().toStdString());

		Form->lstItems->clear();
		Form->lstItems->addItem("<Seperator>");
		for (ZESize I = 0; I < Commands.GetCount(); I++)
		{
			QListWidgetItem* ListItem = new QListWidgetItem();
			ListItem->setText(Commands[I]->GetText().ToCString());
			ListItem->setIcon(QIcon(ZEDUIUtils::GetIcon(Commands[I]->GetIcon())));
			ListItem->setData(Qt::UserRole, QVariant((ZESize)Commands[I]));
			Form->lstItems->addItem(ListItem);
		}
	}
	else
	{
		ZEArray<ZEDMenu*> Menus = MainWindow->GetMenuManager()->GetMenus();
		Form->lstItems->clear();
		Form->lstItems->addItem("<Seperator>");
		for (ZESize I = 0; I < Menus.GetCount(); I++)
		{
			QListWidgetItem* ListItem = new QListWidgetItem();
			ListItem->setText(Menus[I]->GetText().ToCString());
			ListItem->setIcon(QIcon(ZEDUIUtils::GetIcon(Menus[I]->GetIcon())));
			ListItem->setData(Qt::UserRole, QVariant((ZESize)Menus[I]));
			Form->lstItems->addItem(ListItem);
		}
	}

	UpdateUI();
}

void ZEDCustomizeUIWindow::UpdateElementItems()
{
	QSignalBlocker lstItemsBlocker(Form->lstElementItems);

	ZEDCommandManager* Manager = ZEDCommandManager::GetInstance();

	Form->lstElementItems->clear();
	if (Form->radElementTypeMenu->isChecked())
	{
		const ZEArray<ZEDMenu*>& Menus = MainWindow->GetMenuManager()->GetMenus();
		for (ZESize I = 0; I < Menus.GetCount(); I++)
		{
			ZEDMenu* Menu = Menus[I];
			if (Menu->GetName() != Form->cmbElements->currentText())
				continue;

			const ZEArray<ZEDMenuItem*>& Items = Menu->GetItems();
			for (ZESize I = 0; I < Items.GetCount(); I++)
			{
				QListWidgetItem* ListItem = new QListWidgetItem();
				ListItem->setData(Qt::UserRole, QVariant((ZESize)Items[I]));

				if (Items[I]->GetType() == ZED_MIT_SEPERATOR)
				{
					ListItem->setText("----------------");
				}
				else if (Items[I]->GetType() == ZED_TIT_COMMAND)
				{
					ZEDCommand* Command = Manager->GetCommand(Items[I]->GetTargetName());
					if (Command != NULL)
					{
						ListItem->setIcon(QIcon(ZEDUIUtils::GetIcon(Command->GetIcon())));
						ListItem->setText(Command->GetText().ToCString());
					}
					else
					{
						ListItem->setText("<Error: Command Not Found>");
					}
				}
				else if (Items[I]->GetType() == ZED_MIT_MENU_POINTER)
				{
					ZEDMenu* Menu = MainWindow->GetMenuManager()->GetMenu(Items[I]->GetTargetName());
					if (Menu != NULL)
					{
						ListItem->setIcon(QIcon(ZEDUIUtils::GetIcon(Menu->GetIcon())));
						ListItem->setText(Menu->GetText().ToCString());
					}
					else
					{
						ListItem->setText("<Error: Menu Not Found>");
					}
				}

				Form->lstElementItems->addItem(ListItem);
			}

			break;
		}
	}
	else
	{
		const ZEArray<ZEDToolbar*>& Toolbars = MainWindow->GetToolbarManager()->GetToolbars();
		for (ZESize I = 0; I < Toolbars.GetCount(); I++)
		{
			ZEDToolbar* Toolbar = Toolbars[I];
			if (Toolbar->GetName() != Form->cmbElements->currentText())
				continue;

			const ZEArray<ZEDToolbarItem*>& Items = Toolbar->GetItems();
			for (ZESize I = 0; I < Items.GetCount(); I++)
			{
				QListWidgetItem* ListItem = new QListWidgetItem();
				ListItem->setData(Qt::UserRole, QVariant((ZESize)Items[I]));

				if (Items[I]->GetType() == ZED_MIT_SEPERATOR)
				{
					ListItem->setText("----------------");
				}
				else if (Items[I]->GetType() == ZED_TIT_COMMAND)
				{
					ZEDCommand* Command = Manager->GetCommand(Items[I]->GetTargetName());
					if (Command == NULL)
					{
						ListItem->setIcon(QIcon(ZEDUIUtils::GetIcon(Command->GetIcon())));
						ListItem->setText(Command->GetText().ToCString());
					}
					else
					{
						ListItem->setText("<Error: Command Not Found>");
					}

				}
		
				Form->lstElementItems->addItem(ListItem);
			}

			break;
		}
	}

	UpdateUI();
}

void ZEDCustomizeUIWindow::UpdateItemCategories()
{
	if (Form->radItemTypeMenu->isChecked())
		return;

	QSignalBlocker cmbCategoryBlocker(Form->cmbItemCategory);

	ZEArray<ZEString> Categories = ZEDCommandManager::GetInstance()->GetCatagories();
	Form->cmbItemCategory->clear();
	Form->cmbItemCategory->addItem("<All>");
	for (ZESize I = 0; I < Categories.GetCount(); I++)
		Form->cmbItemCategory->addItem(Categories[I].ToCString());
}

void ZEDCustomizeUIWindow::UpdateUI()
{
	bool ElementSelected = Form->cmbElements->currentIndex() != -1;
	Form->txtElementName->setEnabled(ElementSelected);
	Form->txtElementText->setEnabled(ElementSelected);
	Form->chkElementVisible->setEnabled(ElementSelected && !IsElementTypeMenu());
	Form->lstElementItems->setEnabled(ElementSelected);
	Form->btnElementItemAdd->setEnabled(ElementSelected && Form->lstItems->selectedItems().count() != 0);
	Form->btnElementItemRemove->setEnabled(ElementSelected && Form->lstElementItems->selectedItems().count() != 0);

	int Index = Form->lstElementItems->currentIndex().row();
	int Count = Form->lstElementItems->count();

	Form->btnElementItemUp->setEnabled(ElementSelected && Count > 1 && Index > 0);
	Form->btnElementItemDown->setEnabled(ElementSelected && Count > 1 && Index != -1 && Index + 1 != Count);

	Form->cmbItemCategory->setEnabled(Form->radItemTypeCommand->isChecked());
	Form->radItemTypeMenu->setEnabled(Form->radElementTypeMenu->isChecked());
}

bool ZEDCustomizeUIWindow::IsElementTypeMenu()
{
	return Form->radElementTypeMenu->isChecked();
}

ZEDMenu* ZEDCustomizeUIWindow::GetMenu()
{
	return MainWindow->GetMenuManager()->GetMenus()[Form->cmbElements->currentIndex()];
}

ZEDToolbar* ZEDCustomizeUIWindow::GetToolbar()
{
	return MainWindow->GetToolbarManager()->GetToolbars()[Form->cmbElements->currentIndex()];
}

void ZEDCustomizeUIWindow::radElementType_toggled(bool)
{
	if (Form->radElementTypeToolbar->isChecked())
		Form->radItemTypeCommand->setChecked(true);

	UpdateElements();
	UpdateElementItems();
	UpdateItems();
}

void ZEDCustomizeUIWindow::btnElementNew_clicked()
{
	UpdateUI();
}

void ZEDCustomizeUIWindow::btnElementDelete_clicked()
{
	if (IsElementTypeMenu())
	{
		ZEDMenu* Menu = reinterpret_cast<ZEDMenu*>(Form->cmbElements->currentData(Qt::UserRole).toULongLong());
		Menu->Destroy();

	}
	UpdateElements();
	UpdateElementItems();
}

void ZEDCustomizeUIWindow::cmbElements_currentIndexChanged(int)
{
	UpdateElement();
	UpdateElementItems();
}

void ZEDCustomizeUIWindow::txtElementName_textChanged(const QString&)
{
	if (IsElementTypeMenu())
		GetMenu()->SetName(Form->txtElementName->text().toStdString());
	else
		GetToolbar()->SetName(Form->txtElementName->text().toStdString());

	UpdateElements();
	UpdateUI();
}

void ZEDCustomizeUIWindow::txtElementText_textChanged(const QString&)
{
	if (IsElementTypeMenu())
		GetMenu()->SetText(Form->txtElementText->text().toStdString());
	else
		GetToolbar()->SetText(Form->txtElementText->text().toStdString());

	UpdateUI();
}

void ZEDCustomizeUIWindow::chkElementVisible_toggled(bool)
{
	GetToolbar()->SetVisible(Form->chkElementVisible->isChecked());
}

void ZEDCustomizeUIWindow::btnElementItemAdd_clicked()
{
	int ItemIndex = Form->lstItems->currentIndex().row() - 1;
	if (IsElementTypeMenu())
	{
		if (Form->radItemTypeMenu->isChecked())
		{
			ZEDMenu* TargetMenu = reinterpret_cast<ZEDMenu*>(Form->lstItems->selectedItems()[0]->data(Qt::UserRole).toULongLong());
			ZEDMenuItem* Item = ZEDMenuItem::CreateInstance();
			if (TargetMenu == NULL)
			{
				Item->SetType(ZED_MIT_SEPERATOR);
			}
			else
			{
				Item->SetType(ZED_MIT_MENU_POINTER);
				Item->SetTargetName(TargetMenu->GetName());
			}
			GetMenu()->AddItem(Item);
		}
		else
		{
			ZEDCommand* TargetCommand = reinterpret_cast<ZEDCommand*>(Form->lstItems->selectedItems()[0]->data(Qt::UserRole).toULongLong());
			ZEDMenuItem* Item = ZEDMenuItem::CreateInstance();
			if (TargetCommand == NULL)
			{
				Item->SetType(ZED_MIT_SEPERATOR);
			}
			else
			{
				Item->SetType(ZED_MIT_COMMAND);
				Item->SetTargetName(TargetCommand->GetName());
			}
			GetMenu()->AddItem(Item);
		}
	}
	else
	{
		ZEDCommand* TargetCommand = reinterpret_cast<ZEDCommand*>(Form->lstItems->selectedItems()[0]->data(Qt::UserRole).toULongLong());
		ZEDToolbarItem* Item = ZEDToolbarItem::CreateInstance();
		if (TargetCommand == NULL)
		{
			Item->SetType(ZED_TIT_SEPERATOR);
		}
		else
		{
			Item->SetType(ZED_TIT_COMMAND);
			Item->SetTargetName(TargetCommand->GetName());
		}
		GetToolbar()->AddItem(Item);
	}

	UpdateElementItems();
}

void ZEDCustomizeUIWindow::btnElementItemRemove_clicked()
{
	if (IsElementTypeMenu())
	{
		ZEDMenuItem* MenuItem = reinterpret_cast<ZEDMenuItem*>(Form->lstElementItems->selectedItems()[0]->data(Qt::UserRole).toULongLong());
		MenuItem->Destroy();

	}
	else
	{
		ZEDToolbarItem* ToolbarItem = reinterpret_cast<ZEDToolbarItem*>(Form->lstElementItems->selectedItems()[0]->data(Qt::UserRole).toULongLong());
		ToolbarItem->Destroy();
	}
	
	UpdateElementItems();
}

void ZEDCustomizeUIWindow::btnElementItemUp_clicked()
{
	QListWidgetItem* ListItem = Form->lstElementItems->selectedItems()[0];
	int ListItemIndex = Form->lstElementItems->row(ListItem);

	if (IsElementTypeMenu())
	{
		ZEDMenuItem* MenuItem = reinterpret_cast<ZEDMenuItem*>(ListItem->data(Qt::UserRole).toULongLong());
		ZEDMenu* Menu = MenuItem->GetMenu();
		ZESize Index = Menu->GetItems().FindIndex(MenuItem);
		Menu->RemoveItem(MenuItem);
		Index--;
		Menu->InsertItem(Index, MenuItem);
	}
	else
	{
		ZEDToolbarItem* ToolbarItem = reinterpret_cast<ZEDToolbarItem*>(ListItem->data(Qt::UserRole).toULongLong());
		ZEDToolbar* Toolbar = ToolbarItem->GetToolbar();
		ZESize Index = Toolbar->GetItems().FindIndex(ToolbarItem);
		Toolbar->RemoveItem(ToolbarItem);
		Index--;
		Toolbar->InsertItem(Index, ToolbarItem);
	}

	UpdateElementItems();
	Form->lstElementItems->setCurrentRow(ListItemIndex - 1);
}

void ZEDCustomizeUIWindow::btnElementItemDown_clicked()
{
	QListWidgetItem* ListItem = Form->lstElementItems->selectedItems()[0];
	int ListItemIndex = Form->lstElementItems->row(ListItem);

	if (IsElementTypeMenu())
	{
		ZEDMenuItem* MenuItem = reinterpret_cast<ZEDMenuItem*>(ListItem->data(Qt::UserRole).toULongLong());
		ZEDMenu* Menu = MenuItem->GetMenu();
		ZESize Index = Menu->GetItems().FindIndex(MenuItem);
		Menu->RemoveItem(MenuItem);
		Index++;
		Menu->InsertItem(Index, MenuItem);
	}
	else
	{
		ZEDToolbarItem* ToolbarItem = reinterpret_cast<ZEDToolbarItem*>(ListItem->data(Qt::UserRole).toULongLong());
		ZEDToolbar* Toolbar = ToolbarItem->GetToolbar();
		ZESize Index = Toolbar->GetItems().FindIndex(ToolbarItem);
		Toolbar->RemoveItem(ToolbarItem);
		Index++;
		Toolbar->InsertItem(Index, ToolbarItem);
	}

	UpdateElementItems();
	Form->lstElementItems->setCurrentRow(ListItemIndex + 1);		
}

void ZEDCustomizeUIWindow::lstElementItems_itemSelectionChanged()
{
	UpdateUI();
}

void ZEDCustomizeUIWindow::radItemType_currentIndexChanged(int)
{
	UpdateItems();
}

void ZEDCustomizeUIWindow::cmbItemCategory_currentIndexChanged(int)
{
	UpdateItems();
}

void ZEDCustomizeUIWindow::lstItems_itemSelectionChanged()
{
	UpdateUI();
}

void ZEDCustomizeUIWindow::btnDefault_clicked()
{
	MainWindow->GetMenuManager()->Load("#R:/ZEDEditor/MenusDefault.ZEConfig");
	MainWindow->GetToolbarManager()->Load("#R:/ZEDEditor/ToolbarsDefault.ZEConfig");
	UpdateElements();
	UpdateElementItems();
}

void ZEDCustomizeUIWindow::btnSave_clicked()
{
	MainWindow->GetMenuManager()->Save("#S:/ZEDEditor/Menus.ZEConfig");
	MainWindow->GetToolbarManager()->Save("#S:/ZEDEditor/Toolbars.ZEConfig");
	close();
}

ZEDCustomizeUIWindow::ZEDCustomizeUIWindow(ZEDMainWindow* MainWindow)
{
	this->MainWindow = MainWindow;
	Form = new Ui_ZEDCustomizeUIWindow();
	Form->setupUi(this);

	UpdateItemCategories();
	UpdateItems();
	UpdateElements();
	UpdateElement();
	UpdateElementItems();
	UpdateUI();

	connect(Form->radElementTypeMenu, SIGNAL(toggled(bool)), this, SLOT(radElementType_toggled(bool)));
	connect(Form->radElementTypeToolbar, SIGNAL(toggled(bool)), this, SLOT(radElementType_toggled(bool)));
	connect(Form->cmbElements, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbElements_currentIndexChanged(int)));
	connect(Form->btnElementNew, SIGNAL(clicked()), this, SLOT(btnElementNew_clicked()));
	connect(Form->btnElementDelete, SIGNAL(clicked()), this, SLOT(btnElementDelete_clicked()));
	connect(Form->txtElementName, SIGNAL(textChanged(const QString&)), this, SLOT(txtElementName_textChanged(const QString&)));
	connect(Form->txtElementText, SIGNAL(textChanged(const QString&)), this, SLOT(txtElementText_textChanged(const QString&)));
	connect(Form->chkElementVisible, SIGNAL(toggled(bool)), this, SLOT(chkElementVisible_toggled(bool)));
	connect(Form->btnElementItemAdd, SIGNAL(clicked()), this, SLOT(btnElementItemAdd_clicked()));
	connect(Form->btnElementItemRemove, SIGNAL(clicked()), this, SLOT(btnElementItemRemove_clicked()));
	connect(Form->btnElementItemUp, SIGNAL(clicked()), this, SLOT(btnElementItemUp_clicked()));
	connect(Form->btnElementItemDown, SIGNAL(clicked()), this, SLOT(btnElementItemDown_clicked()));
	connect(Form->lstElementItems, SIGNAL(itemSelectionChanged()), this, SLOT(lstElementItems_itemSelectionChanged()));

	connect(Form->radItemTypeCommand, SIGNAL(toggled(bool)), this, SLOT(radElementType_toggled(bool)));
	connect(Form->radItemTypeMenu, SIGNAL(toggled(bool)), this, SLOT(radElementType_toggled(bool)));
	connect(Form->cmbItemCategory, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbItemCategory_currentIndexChanged(int)));
	connect(Form->lstItems, SIGNAL(itemSelectionChanged()), this, SLOT(lstItems_itemSelectionChanged()));

	connect(Form->btnDefault, SIGNAL(clicked()), this, SLOT(btnDefault_clicked()));
	connect(Form->btnSave, SIGNAL(clicked()), this, SLOT(btnSave_clicked()));
}

ZEDCustomizeUIWindow::~ZEDCustomizeUIWindow()
{
	delete Form;
}
