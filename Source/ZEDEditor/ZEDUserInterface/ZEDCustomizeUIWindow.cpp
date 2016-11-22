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
#include "QMessageBox"

#define ZED_ROW_SIZE 24

void ZEDCustomizeUIWindow::UpdateElements()
{
	QSignalBlocker cmbNameBlocker(Form->cmbElements);

	if (Form->radElementTypeMenu->isChecked())
	{
		Form->cmbElements->clear();
		const ZEArray<ZEDMenu*>& Menus = MainWindow->GetMenuManager()->GetMenus();
		for (ZESize I = 0; I < Menus.GetCount(); I++)
			Form->cmbElements->addItem(Menus[I]->GetName().ToCString(), QVariant((ZEUInt64)Menus[I]));
	}
	else
	{
		Form->cmbElements->clear();
		const ZEArray<ZEDToolbar*>& Toolbars = MainWindow->GetToolbarManager()->GetToolbars();
		for (ZESize I = 0; I < Toolbars.GetCount(); I++)
			Form->cmbElements->addItem(Toolbars[I]->GetName().ToCString(), QVariant((ZEUInt64)Toolbars[I]));
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
		Form->txtElementText->setText(GetToolbar()->GetText().ToCString());
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

		QTreeWidgetItem* SeperatorItem = new QTreeWidgetItem();
		SeperatorItem->setSizeHint(0, QSize(ZED_ROW_SIZE, ZED_ROW_SIZE));
		Form->lstItems->addTopLevelItem(SeperatorItem);
		QFrame* Line = new QFrame();
		Line->setFrameShape(QFrame::HLine);
		Line->setFrameShadow(QFrame::Sunken);
		Form->lstItems->setItemWidget(SeperatorItem, 1, Line);

		for (ZESize I = 0; I < Commands.GetCount(); I++)
		{
			QTreeWidgetItem* ListItem = new QTreeWidgetItem();
			ListItem->setSizeHint(0, QSize(ZED_ROW_SIZE, ZED_ROW_SIZE));
			ListItem->setData(0, Qt::UserRole, QVariant((ZEUInt64)Commands[I]));
			ListItem->setIcon(0, QIcon(ZEDUIUtils::GetIcon(Commands[I]->GetIcon())));
			ListItem->setText(1, Commands[I]->GetText().ToCString());
			ListItem->setTextAlignment(1, Qt::AlignLeft | Qt::AlignVCenter);
			Form->lstItems->addTopLevelItem(ListItem);
		}
	}
	else
	{
		ZEArray<ZEDMenu*> Menus = MainWindow->GetMenuManager()->GetMenus();
		
		Form->lstItems->clear();

		QTreeWidgetItem* SeperatorItem = new QTreeWidgetItem();
		SeperatorItem->setSizeHint(0, QSize(ZED_ROW_SIZE, ZED_ROW_SIZE));
		Form->lstItems->addTopLevelItem(SeperatorItem);
		QFrame* Line = new QFrame();
		Line->setFrameShape(QFrame::HLine);
		Line->setFrameShadow(QFrame::Sunken);
		Form->lstItems->setItemWidget(SeperatorItem, 1, Line);

		for (ZESize I = 0; I < Menus.GetCount(); I++)
		{
			QTreeWidgetItem* ListItem = new QTreeWidgetItem();
			ListItem->setSizeHint(0, QSize(ZED_ROW_SIZE, ZED_ROW_SIZE));
			ListItem->setData(0, Qt::UserRole, QVariant((ZEUInt64)Menus[I]));
			ListItem->setIcon(0, QIcon(ZEDUIUtils::GetIcon(Menus[I]->GetIcon())));
			ListItem->setText(1, Menus[I]->GetText().ToCString());
			ListItem->setTextAlignment(1, Qt::AlignLeft | Qt::AlignVCenter);
			ListItem->setText(2, ">");
			ListItem->setTextAlignment(2, Qt::AlignCenter | Qt::AlignVCenter);
			Form->lstItems->addTopLevelItem(ListItem);
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
		ZEDMenu* Menu = GetMenu();

		const ZEArray<ZEDMenuItem*>& Items = Menu->GetItems();
		for (ZESize I = 0; I < Items.GetCount(); I++)
		{
			QTreeWidgetItem* ListItem = new QTreeWidgetItem();
			ListItem->setSizeHint(0, QSize(ZED_ROW_SIZE, ZED_ROW_SIZE));
			ListItem->setData(0, Qt::UserRole, QVariant((ZEUInt64)Items[I]));

			if (Items[I]->GetType() == ZED_MIT_SEPERATOR)
			{
				Form->lstElementItems->addTopLevelItem(ListItem);
				QFrame* Line = new QFrame();
				Line->setFrameShape(QFrame::HLine);
				Line->setFrameShadow(QFrame::Sunken);
				Form->lstElementItems->setItemWidget(ListItem, 1, Line);
			}
			else if (Items[I]->GetType() == ZED_TIT_COMMAND)
			{
				ZEDCommand* Command = Manager->GetCommand(Items[I]->GetTargetName());
				if (Command != NULL)
				{
					ListItem->setIcon(0, QIcon(ZEDUIUtils::GetIcon(Command->GetIcon())));
					ListItem->setText(1, Command->GetText().ToCString());
					ListItem->setTextAlignment(1, Qt::AlignLeft | Qt::AlignVCenter);
					Form->lstElementItems->addTopLevelItem(ListItem);
				}
				else
				{
					ListItem->setText(1, "<Error: Command Not Found>");
					ListItem->setTextAlignment(1, Qt::AlignLeft | Qt::AlignVCenter);
					Form->lstElementItems->addTopLevelItem(ListItem);
				}
			}
			else if (Items[I]->GetType() == ZED_MIT_MENU_POINTER)
			{
				ZEDMenu* Menu = MainWindow->GetMenuManager()->GetMenu(Items[I]->GetTargetName());
				if (Menu != NULL)
				{
					ListItem->setIcon(0, QIcon(ZEDUIUtils::GetIcon(Menu->GetIcon())));
					ListItem->setText(1, Menu->GetText().ToCString());
					ListItem->setTextAlignment(1, Qt::AlignLeft | Qt::AlignVCenter);
					ListItem->setText(2, ">");
					ListItem->setTextAlignment(2, Qt::AlignCenter | Qt::AlignVCenter);
					Form->lstElementItems->addTopLevelItem(ListItem);
				}
				else
				{
					ListItem->setText(1, "<Error: Menu Not Found>");
					ListItem->setTextAlignment(1, Qt::AlignLeft | Qt::AlignVCenter);
					Form->lstElementItems->addTopLevelItem(ListItem);
				}
			}
		}
	}
	else
	{
		ZEDToolbar* Toolbar = GetToolbar();

		const ZEArray<ZEDToolbarItem*>& Items = Toolbar->GetItems();
		for (ZESize I = 0; I < Items.GetCount(); I++)
		{
			QTreeWidgetItem* ListItem = new QTreeWidgetItem();
			ListItem->setSizeHint(0, QSize(ZED_ROW_SIZE, ZED_ROW_SIZE));
			ListItem->setData(0, Qt::UserRole, QVariant((ZEUInt64)Items[I]));

			if (Items[I]->GetType() == ZED_MIT_SEPERATOR)
			{
				Form->lstElementItems->addTopLevelItem(ListItem);
				QFrame* Line = new QFrame();
				Line->setFrameShape(QFrame::HLine);
				Line->setFrameShadow(QFrame::Sunken);
				Form->lstElementItems->setItemWidget(ListItem, 1, Line);
			}
			else if (Items[I]->GetType() == ZED_TIT_COMMAND)
			{
				ZEDCommand* Command = Manager->GetCommand(Items[I]->GetTargetName());
				if (Command != NULL)
				{
					ListItem->setIcon(0, QIcon(ZEDUIUtils::GetIcon(Command->GetIcon())));
					ListItem->setText(1, Command->GetText().ToCString());
					ListItem->setTextAlignment(1, Qt::AlignLeft | Qt::AlignVCenter);
					Form->lstElementItems->addTopLevelItem(ListItem);
				}
				else
				{
					ListItem->setText(1, "<Error: Command Not Found>");
					ListItem->setTextAlignment(1, Qt::AlignLeft | Qt::AlignVCenter);
					Form->lstElementItems->addTopLevelItem(ListItem);
				}
			}
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
	Form->btnElementDelete->setEnabled(ElementSelected);
	Form->txtElementName->setEnabled(ElementSelected);
	Form->txtElementText->setEnabled(ElementSelected);
	Form->chkElementVisible->setEnabled(ElementSelected && !IsElementTypeMenu());
	Form->lstElementItems->setEnabled(ElementSelected);
	Form->btnElementItemAdd->setEnabled(ElementSelected && Form->lstItems->selectedItems().count() != 0);
	Form->btnElementItemRemove->setEnabled(ElementSelected && Form->lstElementItems->selectedItems().count() != 0);

	int Count = Form->lstElementItems->topLevelItemCount();
	int Index = GetElementItemSelectionIndex();

	Form->btnElementItemUp->setEnabled(ElementSelected && Count > 1 && Index > 0);
	Form->btnElementItemDown->setEnabled(ElementSelected && Count > 1 && Index != -1 && Index + 1 != Count);

	Form->cmbItemCategory->setEnabled(Form->radItemTypeCommand->isChecked());
	Form->radItemTypeMenu->setEnabled(Form->radElementTypeMenu->isChecked());

	if (IsElementTypeMenu() && ElementSelected && GetMenu()->GetProtected())
	{
		Form->btnElementDelete->setEnabled(false);
		Form->txtElementName->setEnabled(false);
		Form->txtElementText->setEnabled(false);
	}
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

ZEInt ZEDCustomizeUIWindow::GetElementItemSelectionIndex()
{
	int Count = Form->lstElementItems->topLevelItemCount();
	int Index = -1;
	for (int I = 0; I < Count; I++)
	{
		if (Form->lstElementItems->topLevelItem(I)->isSelected())
			return I;
	}

	return -1;
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
	if (IsElementTypeMenu())
	{
		ZEDMenu* NewMenu = ZEDMenu::CreateInstance();
		NewMenu->SetName("NewMenu");
		NewMenu->SetText("New Menu");
		MainWindow->GetMenuManager()->AddMenu(NewMenu);
		
		UpdateElements();
		Form->cmbElements->setCurrentIndex(Form->cmbElements->count() - 1);
	}
	else
	{
		ZEDToolbar* NewToolbar = ZEDToolbar::CreateInstance();
		NewToolbar->SetName("NewToolbar");
		NewToolbar->SetText("New Toolbar");
		MainWindow->GetToolbarManager()->AddToolbar(NewToolbar);

		UpdateElements();
		Form->cmbElements->setCurrentIndex(Form->cmbElements->count() - 1);
	}

	UpdateUI();
}

void ZEDCustomizeUIWindow::btnElementDelete_clicked()
{
	QMessageBox::StandardButton Result = QMessageBox::question(this, "Customize", "Are you sure that you want to delete this element ?");
	if (Result == QMessageBox::No)
		return;

	if (IsElementTypeMenu())
	{
		ZEDMenu* Menu = reinterpret_cast<ZEDMenu*>(Form->cmbElements->currentData(Qt::UserRole).toULongLong());
		Menu->Destroy();
	}
	else
	{
		ZEDToolbar* Toolbar = reinterpret_cast<ZEDToolbar*>(Form->cmbElements->currentData(Qt::UserRole).toULongLong());
		Toolbar->Destroy();
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

	Form->cmbElements->setItemText(Form->cmbElements->currentIndex(), Form->txtElementName->text());
}

void ZEDCustomizeUIWindow::txtElementText_textChanged(const QString&)
{
	if (IsElementTypeMenu())
		GetMenu()->SetText(Form->txtElementText->text().toStdString());
	else
		GetToolbar()->SetText(Form->txtElementText->text().toStdString());
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
			ZEDMenu* TargetMenu = reinterpret_cast<ZEDMenu*>(Form->lstItems->selectedItems()[0]->data(0, Qt::UserRole).toULongLong());
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

			ZEInt Index = GetElementItemSelectionIndex();
			if (Index == -1)
				GetMenu()->AddItem(Item);
			else
				GetMenu()->InsertItem(Index, Item);
		}
		else
		{
			ZEDCommand* TargetCommand = reinterpret_cast<ZEDCommand*>(Form->lstItems->selectedItems()[0]->data(0, Qt::UserRole).toULongLong());
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

			ZEInt Index = GetElementItemSelectionIndex();
			if (Index == -1)
				GetMenu()->AddItem(Item);
			else
				GetMenu()->InsertItem(Index, Item);
		}
	}
	else
	{
		ZEDCommand* TargetCommand = reinterpret_cast<ZEDCommand*>(Form->lstItems->selectedItems()[0]->data(0, Qt::UserRole).toULongLong());
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

		ZEInt Index = GetElementItemSelectionIndex();
		if (Index == -1)
			GetToolbar()->AddItem(Item);
		else
			GetToolbar()->InsertItem(Index, Item);
	}

	UpdateElementItems();
}

void ZEDCustomizeUIWindow::btnElementItemRemove_clicked()
{
	if (IsElementTypeMenu())
	{
		ZEDMenuItem* MenuItem = reinterpret_cast<ZEDMenuItem*>(Form->lstElementItems->selectedItems()[0]->data(0, Qt::UserRole).toULongLong());
		MenuItem->Destroy();

	}
	else
	{
		ZEDToolbarItem* ToolbarItem = reinterpret_cast<ZEDToolbarItem*>(Form->lstElementItems->selectedItems()[0]->data(0, Qt::UserRole).toULongLong());
		ToolbarItem->Destroy();
	}
	
	UpdateElementItems();
}

void ZEDCustomizeUIWindow::btnElementItemUp_clicked()
{
	QTreeWidgetItem* ListItem = Form->lstElementItems->selectedItems()[0];

	int ListItemIndex = 0;
	for (int I = 0; I < Form->lstElementItems->topLevelItemCount(); I++)
	{
		if (Form->lstElementItems->topLevelItem(I) == ListItem)
			ListItemIndex = I;
	}

	if (IsElementTypeMenu())
	{
		ZEDMenuItem* MenuItem = reinterpret_cast<ZEDMenuItem*>(ListItem->data(0, Qt::UserRole).toULongLong());
		ZEDMenu* Menu = MenuItem->GetMenu();
		ZESize Index = Menu->GetItems().FindIndex(MenuItem);
		Menu->RemoveItem(MenuItem);
		Index--;
		Menu->InsertItem(Index, MenuItem);
	}
	else
	{
		ZEDToolbarItem* ToolbarItem = reinterpret_cast<ZEDToolbarItem*>(ListItem->data(0, Qt::UserRole).toULongLong());
		ZEDToolbar* Toolbar = ToolbarItem->GetToolbar();
		ZESize Index = Toolbar->GetItems().FindIndex(ToolbarItem);
		Toolbar->RemoveItem(ToolbarItem);
		Index--;
		Toolbar->InsertItem(Index, ToolbarItem);
	}

	UpdateElementItems();
	Form->lstElementItems->topLevelItem(ListItemIndex - 1)->setSelected(true);
	UpdateUI();
}

void ZEDCustomizeUIWindow::btnElementItemDown_clicked()
{
	QTreeWidgetItem* ListItem = Form->lstElementItems->selectedItems()[0];
	
	int ListItemIndex = 0;
	for (int I = 0; I < Form->lstElementItems->topLevelItemCount(); I++)
	{
		if (Form->lstElementItems->topLevelItem(I) == ListItem)
			ListItemIndex = I;
	}

	if (IsElementTypeMenu())
	{
		ZEDMenuItem* MenuItem = reinterpret_cast<ZEDMenuItem*>(ListItem->data(0, Qt::UserRole).toULongLong());
		ZEDMenu* Menu = MenuItem->GetMenu();
		ZESize Index = Menu->GetItems().FindIndex(MenuItem);
		Menu->RemoveItem(MenuItem);
		Index++;
		Menu->InsertItem(Index, MenuItem);
	}
	else
	{
		ZEDToolbarItem* ToolbarItem = reinterpret_cast<ZEDToolbarItem*>(ListItem->data(0, Qt::UserRole).toULongLong());
		ZEDToolbar* Toolbar = ToolbarItem->GetToolbar();
		ZESize Index = Toolbar->GetItems().FindIndex(ToolbarItem);
		Toolbar->RemoveItem(ToolbarItem);
		Index++;
		Toolbar->InsertItem(Index, ToolbarItem);
	}

	UpdateElementItems();
	Form->lstElementItems->topLevelItem(ListItemIndex + 1)->setSelected(true);
	UpdateUI();
}

void ZEDCustomizeUIWindow::lstElementItems_itemSelectionChanged()
{
	UpdateUI();
}

void ZEDCustomizeUIWindow::radItemType_toggled(bool)
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
	QMessageBox::StandardButton Result = QMessageBox::question(this, "Customize", "Are you sure that you want to revert back all your configuration to default ?\nAll of your customization will be reverted.");
	if (Result == QMessageBox::No)
		return;

	MainWindow->GetMenuManager()->Load("#R:/ZEDEditor/Configurations/MenuDefault.ZEDConfig");
	MainWindow->GetToolbarManager()->Load("#R:/ZEDEditor/Configurations/ToolbarDefault.ZEDConfig");
	UpdateElements();
	UpdateElementItems();

	MainWindow->PopulateMainMenu();
	MainWindow->PopulateToolbars();
}

void ZEDCustomizeUIWindow::btnSave_clicked()
{
	MainWindow->GetMenuManager()->Save("#S:/Configurations/ZEDEditor/Menu.ZEDConfig");
	MainWindow->GetToolbarManager()->Save("#S:/Configurations/ZEDEditor/Toolbar.ZEDConfig");
	
	close();
}

ZEDCustomizeUIWindow::ZEDCustomizeUIWindow(ZEDMainWindow* MainWindow)
{
	this->MainWindow = MainWindow;
	Form = new Ui_ZEDCustomizeUIWindow();
	Form->setupUi(this);

	Form->lstElementItems->setColumnCount(3);
	Form->lstElementItems->setAlternatingRowColors(true);
	Form->lstElementItems->setRootIsDecorated(false);
	Form->lstElementItems->header()->setStretchLastSection(false);
	Form->lstElementItems->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Custom);
	Form->lstElementItems->header()->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);
	Form->lstElementItems->header()->setSectionResizeMode(2, QHeaderView::ResizeMode::Fixed);
	Form->lstElementItems->header()->resizeSection(0, ZED_ROW_SIZE);
	Form->lstElementItems->header()->resizeSection(2, ZED_ROW_SIZE);

	Form->lstItems->setColumnCount(3);
	Form->lstItems->setAlternatingRowColors(true);
	Form->lstItems->setRootIsDecorated(false);
	Form->lstItems->header()->setStretchLastSection(false);
	Form->lstItems->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Fixed);
	Form->lstItems->header()->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);
	Form->lstItems->header()->setSectionResizeMode(2, QHeaderView::ResizeMode::Fixed);
	Form->lstItems->header()->resizeSection(0, ZED_ROW_SIZE);
	Form->lstItems->header()->resizeSection(2, ZED_ROW_SIZE);

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

	connect(Form->radItemTypeCommand, SIGNAL(toggled(bool)), this, SLOT(radItemType_toggled(bool)));
	connect(Form->radItemTypeMenu, SIGNAL(toggled(bool)), this, SLOT(radItemType_toggled(bool)));
	connect(Form->cmbItemCategory, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbItemCategory_currentIndexChanged(int)));
	connect(Form->lstItems, SIGNAL(itemSelectionChanged()), this, SLOT(lstItems_itemSelectionChanged()));

	connect(Form->btnDefault, SIGNAL(clicked()), this, SLOT(btnDefault_clicked()));
	connect(Form->btnSave, SIGNAL(clicked()), this, SLOT(btnSave_clicked()));
}

ZEDCustomizeUIWindow::~ZEDCustomizeUIWindow()
{
	delete Form;
}
