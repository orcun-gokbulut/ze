//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDMenuOptionsPage.cpp
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

#include "ZEDMenuOptionsPage.h"
#include "ui_ZEDMenuOptionsPageWidget.h"

#include "ZEDCore/ZEDEditor.h"
#include "ZEDOptionsManager.h"
#include "ZEDCommandManager.h"
#include "ZEDMainWindow.h"
#include "ZEDMenu.h"
#include "ZEDMenuItem.h"
#include "ZEDMenuManager.h"
#include "ZEDUIUtils.h"

#include <QMessageBox>

#define ZED_ROW_SIZE 24

void ZEDMenuOptionsPageWidget::UpdateElements()
{
	QSignalBlocker cmbNameBlocker(Form->cmbElements);

	Form->cmbElements->clear();
	const ZEArray<ZEDMenu*>& Menus = MainWindow->GetMenuManager()->GetMenus();
	for (ZESize I = 0; I < Menus.GetCount(); I++)
		Form->cmbElements->addItem(Menus[I]->GetName().ToCString(), QVariant((ZEUInt64)Menus[I]));

	UpdateElement();
}

void ZEDMenuOptionsPageWidget::UpdateElement()
{
	QSignalBlocker txtElementNameBlocker(Form->txtElementName);
	QSignalBlocker txtElementTextBlocker(Form->txtElementText);

	if (GetMenu() == NULL)
		return;

	Form->txtElementName->setText(GetMenu()->GetName().ToCString());
	Form->txtElementText->setText(GetMenu()->GetText().ToCString());
}

void ZEDMenuOptionsPageWidget::UpdateItems()
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

void ZEDMenuOptionsPageWidget::UpdateElementItems()
{
	QSignalBlocker lstItemsBlocker(Form->lstElementItems);

	ZEDCommandManager* Manager = ZEDCommandManager::GetInstance();

	Form->lstElementItems->clear();
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
		else if (Items[I]->GetType() == ZED_MIT_COMMAND)
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
		else
		{
			ListItem->setText(1, "<Error: Unknown Type>");
			ListItem->setTextAlignment(1, Qt::AlignLeft | Qt::AlignVCenter);
			Form->lstElementItems->addTopLevelItem(ListItem);
		}
	}

	UpdateUI();
}

void ZEDMenuOptionsPageWidget::UpdateItemCategories()
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

void ZEDMenuOptionsPageWidget::UpdateUI()
{
	bool ElementSelected = Form->cmbElements->currentIndex() != -1;
	Form->btnElementDelete->setEnabled(ElementSelected);
	Form->txtElementName->setEnabled(ElementSelected);
	Form->txtElementText->setEnabled(ElementSelected);
	Form->lstElementItems->setEnabled(ElementSelected);
	Form->btnElementItemAdd->setEnabled(ElementSelected && Form->lstItems->selectedItems().count() != 0);
	Form->btnElementItemRemove->setEnabled(ElementSelected && Form->lstElementItems->selectedItems().count() != 0);

	int Count = Form->lstElementItems->topLevelItemCount();
	int Index = GetElementItemSelectionIndex();

	Form->btnElementItemUp->setEnabled(ElementSelected && Count > 1 && Index > 0);
	Form->btnElementItemDown->setEnabled(ElementSelected && Count > 1 && Index != -1 && Index + 1 != Count);

	Form->cmbItemCategory->setEnabled(Form->radItemTypeCommand->isChecked());

	if (ElementSelected && GetMenu()->GetProtected())
	{
		Form->btnElementDelete->setEnabled(false);
		Form->txtElementName->setEnabled(false);
		Form->txtElementText->setEnabled(false);
	}
}

ZEDMenu* ZEDMenuOptionsPageWidget::GetMenu()
{
	return MainWindow->GetMenuManager()->GetMenus()[Form->cmbElements->currentIndex()];
}

ZEInt ZEDMenuOptionsPageWidget::GetElementItemSelectionIndex()
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

void ZEDMenuOptionsPageWidget::btnElementNew_clicked()
{
	ZEDMenu* NewMenu = ZEDMenu::CreateInstance();
	NewMenu->SetName("NewMenu");
	NewMenu->SetText("New Menu");
	MainWindow->GetMenuManager()->AddMenu(NewMenu);
		
	UpdateElements();
	Form->cmbElements->setCurrentIndex(Form->cmbElements->count() - 1);

	UpdateUI();
}

void ZEDMenuOptionsPageWidget::btnElementDelete_clicked()
{
	QMessageBox::StandardButton Result = QMessageBox::question(this, "Menu Options", "Are you sure that you want to delete this element ?");
	if (Result == QMessageBox::No)
		return;

	ZEDMenu* Menu = reinterpret_cast<ZEDMenu*>(Form->cmbElements->currentData(Qt::UserRole).toULongLong());
	Menu->Destroy();


	UpdateElements();
	UpdateElementItems();
	UpdateItems();
}

void ZEDMenuOptionsPageWidget::cmbElements_currentIndexChanged(int)
{
	UpdateElement();
	UpdateElementItems();
}

void ZEDMenuOptionsPageWidget::txtElementName_textChanged(const QString&)
{
	GetMenu()->SetName(Form->txtElementName->text().toStdString());
	Form->cmbElements->setItemText(Form->cmbElements->currentIndex(), Form->txtElementName->text());
}

void ZEDMenuOptionsPageWidget::txtElementText_textChanged(const QString&)
{
	GetMenu()->SetText(Form->txtElementText->text().toStdString());
}

void ZEDMenuOptionsPageWidget::btnElementItemAdd_clicked()
{
	int ItemIndex = Form->lstItems->currentIndex().row() - 1;
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

	UpdateElementItems();

	Page->MarkAsModified();
}

void ZEDMenuOptionsPageWidget::btnElementItemRemove_clicked()
{
	ZEDMenuItem* MenuItem = reinterpret_cast<ZEDMenuItem*>(Form->lstElementItems->selectedItems()[0]->data(0, Qt::UserRole).toULongLong());
	MenuItem->Destroy();
	UpdateElementItems();

	Page->MarkAsModified();
}

void ZEDMenuOptionsPageWidget::btnElementItemUp_clicked()
{
	QTreeWidgetItem* ListItem = Form->lstElementItems->selectedItems()[0];

	int ListItemIndex = 0;
	for (int I = 0; I < Form->lstElementItems->topLevelItemCount(); I++)
	{
		if (Form->lstElementItems->topLevelItem(I) == ListItem)
			ListItemIndex = I;
	}

	ZEDMenuItem* MenuItem = reinterpret_cast<ZEDMenuItem*>(ListItem->data(0, Qt::UserRole).toULongLong());
	ZEDMenu* Menu = MenuItem->GetMenu();
	ZESize Index = Menu->GetItems().FindIndex(MenuItem);
	Menu->RemoveItem(MenuItem);
	Index--;
	Menu->InsertItem(Index, MenuItem);

	UpdateElementItems();
	Form->lstElementItems->topLevelItem(ListItemIndex - 1)->setSelected(true);
	UpdateUI();

	Page->MarkAsModified();
}

void ZEDMenuOptionsPageWidget::btnElementItemDown_clicked()
{
	QTreeWidgetItem* ListItem = Form->lstElementItems->selectedItems()[0];
	
	int ListItemIndex = 0;
	for (int I = 0; I < Form->lstElementItems->topLevelItemCount(); I++)
	{
		if (Form->lstElementItems->topLevelItem(I) == ListItem)
			ListItemIndex = I;
	}

	ZEDMenuItem* MenuItem = reinterpret_cast<ZEDMenuItem*>(ListItem->data(0, Qt::UserRole).toULongLong());
	ZEDMenu* Menu = MenuItem->GetMenu();
	ZESize Index = Menu->GetItems().FindIndex(MenuItem);
	Menu->RemoveItem(MenuItem);
	Index++;
	Menu->InsertItem(Index, MenuItem);

	UpdateElementItems();
	Form->lstElementItems->topLevelItem(ListItemIndex + 1)->setSelected(true);
	UpdateUI();

	Page->MarkAsModified();
}

void ZEDMenuOptionsPageWidget::lstElementItems_itemSelectionChanged()
{
	UpdateUI();
}

void ZEDMenuOptionsPageWidget::radItemType_toggled(bool)
{
	UpdateItems();
}

void ZEDMenuOptionsPageWidget::cmbItemCategory_currentIndexChanged(int)
{
	UpdateItems();
}

void ZEDMenuOptionsPageWidget::lstItems_itemSelectionChanged()
{
	UpdateUI();
}

void ZEDMenuOptionsPageWidget::Default()
{
	MainWindow->GetMenuManager()->Load("#R:/ZEDEditor/Configurations/MenuDefault.ZEDConfig");
	UpdateElements();
	UpdateElementItems();
	MainWindow->UpdateMainMenu();
}

void ZEDMenuOptionsPageWidget::Save()
{
	MainWindow->GetMenuManager()->Save("#S:/Configurations/ZEDEditor/Menu.ZEDConfig");
	MainWindow->UpdateMainMenu();
}

ZEDMenuOptionsPageWidget::ZEDMenuOptionsPageWidget(QWidget* Parent, ZEDMainWindow* MainWindow, ZEDMenuOptionsPage* Page) : QWidget(Parent)
{
	this->MainWindow = MainWindow;
	this->Page = Page;

	Form = new Ui_ZEDMenuOptionsPageWidget();
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

	connect(Form->cmbElements, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbElements_currentIndexChanged(int)));
	connect(Form->btnElementNew, SIGNAL(clicked()), this, SLOT(btnElementNew_clicked()));
	connect(Form->btnElementDelete, SIGNAL(clicked()), this, SLOT(btnElementDelete_clicked()));
	connect(Form->txtElementName, SIGNAL(textChanged(const QString&)), this, SLOT(txtElementName_textChanged(const QString&)));
	connect(Form->txtElementText, SIGNAL(textChanged(const QString&)), this, SLOT(txtElementText_textChanged(const QString&)));
	connect(Form->btnElementItemAdd, SIGNAL(clicked()), this, SLOT(btnElementItemAdd_clicked()));
	connect(Form->btnElementItemRemove, SIGNAL(clicked()), this, SLOT(btnElementItemRemove_clicked()));
	connect(Form->btnElementItemUp, SIGNAL(clicked()), this, SLOT(btnElementItemUp_clicked()));
	connect(Form->btnElementItemDown, SIGNAL(clicked()), this, SLOT(btnElementItemDown_clicked()));
	connect(Form->lstElementItems, SIGNAL(itemSelectionChanged()), this, SLOT(lstElementItems_itemSelectionChanged()));

	connect(Form->radItemTypeCommand, SIGNAL(toggled(bool)), this, SLOT(radItemType_toggled(bool)));
	connect(Form->radItemTypeMenu, SIGNAL(toggled(bool)), this, SLOT(radItemType_toggled(bool)));
	connect(Form->cmbItemCategory, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbItemCategory_currentIndexChanged(int)));
	connect(Form->lstItems, SIGNAL(itemSelectionChanged()), this, SLOT(lstItems_itemSelectionChanged()));
}

ZEDMenuOptionsPageWidget::~ZEDMenuOptionsPageWidget()
{
	delete Form;
}

const char* ZEDMenuOptionsPage::GetName() const
{
	return "ZEDMenuOptionsPage";
}

const char* ZEDMenuOptionsPage::GetText() const 
{
	return "Menus";
}

const char* ZEDMenuOptionsPage::GetPath() const
{
	return "User Interface";
}

QWidget* ZEDMenuOptionsPage::CreateWidget(QWidget* Parent)
{
	return new ZEDMenuOptionsPageWidget(Parent, GetManager()->GetEditor()->GetMainWindow(), this);
}

void ZEDMenuOptionsPage::Save(QWidget* Widget)
{
	static_cast<ZEDMenuOptionsPageWidget*>(Widget)->Save();
	ZEDOptionsPage::Save(Widget);
}

void ZEDMenuOptionsPage::Default(QWidget* Widget)
{
	static_cast<ZEDMenuOptionsPageWidget*>(Widget)->Default();
	ZEDOptionsPage::Default(Widget);
}

void ZEDMenuOptionsPage::Close(QWidget* Widget)
{
	Widget->close();
	ZEDOptionsPage::Close(Widget);
}
