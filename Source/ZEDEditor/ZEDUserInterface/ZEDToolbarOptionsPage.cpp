//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDToolbarOptionsPage.cpp
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

#include "ZEDToolbarOptionsPage.h"
#include "ui_ZEDToolbarOptionsPageWidget.h"

#include "ZEDOptionsManager.h"
#include "ZEDCommandManager.h"
#include "ZEDCore/ZEDEditor.h"
#include "ZEDMainWindow.h"
#include "ZEDMenu.h"
#include "ZEDMenuItem.h"
#include "ZEDMenuManager.h"
#include "ZEDToolbar.h"
#include "ZEDToolbarItem.h"
#include "ZEDToolbarManager.h"
#include "ZEDUIUtils.h"

#include <QMessageBox>

#define ZED_ROW_SIZE 24

void ZEDToolbarOptionsPageWidget::UpdateElements()
{
	QSignalBlocker cmbNameBlocker(Form->cmbElements);

	Form->cmbElements->clear();
	const ZEArray<ZEDToolbar*>& Toolbars = MainWindow->GetToolbarManager()->GetToolbars();
	for (ZESize I = 0; I < Toolbars.GetCount(); I++)
		Form->cmbElements->addItem(Toolbars[I]->GetName().ToCString(), QVariant((ZEUInt64)Toolbars[I]));

	UpdateElement();
}

void ZEDToolbarOptionsPageWidget::UpdateElement()
{
	QSignalBlocker txtElementNameBlocker(Form->txtElementName);
	QSignalBlocker txtElementTextBlocker(Form->txtElementText);
	QSignalBlocker chkElementVisibleBlocker(Form->chkElementVisible);

	if (GetToolbar() == NULL)
		return;

	Form->txtElementName->setText(GetToolbar()->GetName().ToCString());
	Form->txtElementText->setText(GetToolbar()->GetText().ToCString());
	Form->chkElementVisible->setChecked(GetToolbar()->GetVisible());
}

void ZEDToolbarOptionsPageWidget::UpdateItems()
{
	QSignalBlocker lstCommandsBlocker(Form->lstItems);

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

	UpdateUI();
}

void ZEDToolbarOptionsPageWidget::UpdateElementItems()
{
	QSignalBlocker lstItemsBlocker(Form->lstElementItems);

	ZEDCommandManager* Manager = ZEDCommandManager::GetInstance();

	Form->lstElementItems->clear();

	ZEDToolbar* Toolbar = GetToolbar();
	const ZEArray<ZEDToolbarItem*>& Items = Toolbar->GetItems();
	for (ZESize I = 0; I < Items.GetCount(); I++)
	{
		QTreeWidgetItem* ListItem = new QTreeWidgetItem();
		ListItem->setSizeHint(0, QSize(ZED_ROW_SIZE, ZED_ROW_SIZE));
		ListItem->setData(0, Qt::UserRole, QVariant((ZEUInt64)Items[I]));

		if (Items[I]->GetType() == ZED_TIT_SEPERATOR)
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
		else
		{
			ListItem->setText(1, "<Error: Unknown Type>");
			ListItem->setTextAlignment(1, Qt::AlignLeft | Qt::AlignVCenter);
			Form->lstElementItems->addTopLevelItem(ListItem);
		}
	}

	UpdateUI();
}

void ZEDToolbarOptionsPageWidget::UpdateItemCategories()
{
	QSignalBlocker cmbCategoryBlocker(Form->cmbItemCategory);

	ZEArray<ZEString> Categories = ZEDCommandManager::GetInstance()->GetCatagories();
	Form->cmbItemCategory->clear();
	Form->cmbItemCategory->addItem("<All>");
	for (ZESize I = 0; I < Categories.GetCount(); I++)
		Form->cmbItemCategory->addItem(Categories[I].ToCString());
}

void ZEDToolbarOptionsPageWidget::UpdateUI()
{
	bool ElementSelected = Form->cmbElements->currentIndex() != -1;
	Form->btnElementDelete->setEnabled(ElementSelected);
	Form->txtElementName->setEnabled(ElementSelected);
	Form->txtElementText->setEnabled(ElementSelected);
	Form->chkElementVisible->setEnabled(ElementSelected);
	Form->lstElementItems->setEnabled(ElementSelected);
	Form->btnElementItemAdd->setEnabled(ElementSelected && Form->lstItems->selectedItems().count() != 0);
	Form->btnElementItemRemove->setEnabled(ElementSelected && Form->lstElementItems->selectedItems().count() != 0);

	int Count = Form->lstElementItems->topLevelItemCount();
	int Index = GetElementItemSelectionIndex();

	Form->btnElementItemUp->setEnabled(ElementSelected && Count > 1 && Index > 0);
	Form->btnElementItemDown->setEnabled(ElementSelected && Count > 1 && Index != -1 && Index + 1 != Count);
}

ZEDToolbar* ZEDToolbarOptionsPageWidget::GetToolbar()
{
	return MainWindow->GetToolbarManager()->GetToolbars()[Form->cmbElements->currentIndex()];
}

ZEInt ZEDToolbarOptionsPageWidget::GetElementItemSelectionIndex()
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

void ZEDToolbarOptionsPageWidget::btnElementNew_clicked()
{

	ZEDToolbar* NewToolbar = ZEDToolbar::CreateInstance();
	NewToolbar->SetName("NewToolbar");
	NewToolbar->SetText("New Toolbar");
	MainWindow->GetToolbarManager()->AddToolbar(NewToolbar);

	UpdateElements();
	Form->cmbElements->setCurrentIndex(Form->cmbElements->count() - 1);

	UpdateUI();

	Page->MarkAsModified();
}

void ZEDToolbarOptionsPageWidget::btnElementDelete_clicked()
{
	QMessageBox::StandardButton Result = QMessageBox::question(this, "Toolbar Options", "Are you sure that you want to delete this element ?");
	if (Result == QMessageBox::No)
		return;

	ZEDToolbar* Toolbar = reinterpret_cast<ZEDToolbar*>(Form->cmbElements->currentData(Qt::UserRole).toULongLong());
	Toolbar->Destroy();

	UpdateElements();
	UpdateElementItems();
	UpdateItems();

	Page->MarkAsModified();
}

void ZEDToolbarOptionsPageWidget::cmbElements_currentIndexChanged(int)
{
	UpdateElement();
	UpdateElementItems();
}

void ZEDToolbarOptionsPageWidget::txtElementName_textChanged(const QString&)
{
	GetToolbar()->SetName(Form->txtElementName->text().toStdString());
	Form->cmbElements->setItemText(Form->cmbElements->currentIndex(), Form->txtElementName->text());

	Page->MarkAsModified();
}

void ZEDToolbarOptionsPageWidget::txtElementText_textChanged(const QString&)
{
	GetToolbar()->SetText(Form->txtElementText->text().toStdString());

	Page->MarkAsModified();
}

void ZEDToolbarOptionsPageWidget::chkElementVisible_toggled(bool)
{
	GetToolbar()->SetVisible(Form->chkElementVisible->isChecked());

	Page->MarkAsModified();
}

void ZEDToolbarOptionsPageWidget::btnElementItemAdd_clicked()
{
	int ItemIndex = Form->lstItems->currentIndex().row() - 1;
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

	UpdateElementItems();

	Page->MarkAsModified();
}

void ZEDToolbarOptionsPageWidget::btnElementItemRemove_clicked()
{

	ZEDToolbarItem* ToolbarItem = reinterpret_cast<ZEDToolbarItem*>(Form->lstElementItems->selectedItems()[0]->data(0, Qt::UserRole).toULongLong());
	ToolbarItem->Destroy();
	
	UpdateElementItems();

	Page->MarkAsModified();
}

void ZEDToolbarOptionsPageWidget::btnElementItemUp_clicked()
{
	QTreeWidgetItem* ListItem = Form->lstElementItems->selectedItems()[0];

	int ListItemIndex = 0;
	for (int I = 0; I < Form->lstElementItems->topLevelItemCount(); I++)
	{
		if (Form->lstElementItems->topLevelItem(I) == ListItem)
			ListItemIndex = I;
	}

	ZEDToolbarItem* ToolbarItem = reinterpret_cast<ZEDToolbarItem*>(ListItem->data(0, Qt::UserRole).toULongLong());
	ZEDToolbar* Toolbar = ToolbarItem->GetToolbar();
	ZESize Index = Toolbar->GetItems().FindIndex(ToolbarItem);
	Toolbar->RemoveItem(ToolbarItem);
	Index--;
	Toolbar->InsertItem(Index, ToolbarItem);

	UpdateElementItems();
	Form->lstElementItems->topLevelItem(ListItemIndex - 1)->setSelected(true);
	UpdateUI();

	Page->MarkAsModified();
}

void ZEDToolbarOptionsPageWidget::btnElementItemDown_clicked()
{
	QTreeWidgetItem* ListItem = Form->lstElementItems->selectedItems()[0];
	
	int ListItemIndex = 0;
	for (int I = 0; I < Form->lstElementItems->topLevelItemCount(); I++)
	{
		if (Form->lstElementItems->topLevelItem(I) == ListItem)
			ListItemIndex = I;
	}

	ZEDToolbarItem* ToolbarItem = reinterpret_cast<ZEDToolbarItem*>(ListItem->data(0, Qt::UserRole).toULongLong());
	ZEDToolbar* Toolbar = ToolbarItem->GetToolbar();
	ZESize Index = Toolbar->GetItems().FindIndex(ToolbarItem);
	Toolbar->RemoveItem(ToolbarItem);
	Index++;
	Toolbar->InsertItem(Index, ToolbarItem);

	UpdateElementItems();
	Form->lstElementItems->topLevelItem(ListItemIndex + 1)->setSelected(true);
	UpdateUI();

	Page->MarkAsModified();
}

void ZEDToolbarOptionsPageWidget::lstElementItems_itemSelectionChanged()
{
	UpdateUI();
}

void ZEDToolbarOptionsPageWidget::cmbItemCategory_currentIndexChanged(int)
{
	UpdateItems();
}

void ZEDToolbarOptionsPageWidget::lstItems_itemSelectionChanged()
{
	UpdateUI();
}

void ZEDToolbarOptionsPageWidget::Default()
{
	MainWindow->GetToolbarManager()->Load("#R:/ZEDEditor/Configurations/ToolbarDefault.ZEDConfig");
	UpdateElements();
	UpdateElementItems();
	MainWindow->UpdateToolbars();
}

void ZEDToolbarOptionsPageWidget::Save()
{
	MainWindow->GetToolbarManager()->Save("#S:/Configurations/ZEDEditor/Toolbar.ZEDConfig");
	MainWindow->UpdateToolbars();
}

ZEDToolbarOptionsPageWidget::ZEDToolbarOptionsPageWidget(QWidget* Parent, ZEDMainWindow* MainWindow, ZEDToolbarOptionsPage* Page) : QWidget(Parent)
{
	this->MainWindow = MainWindow;
	this->Page = Page;

	Form = new Ui_ZEDToolbarOptionsPageWidget();
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
	connect(Form->chkElementVisible, SIGNAL(toggled(bool)), this, SLOT(chkElementVisible_toggled(bool)));
	connect(Form->btnElementItemAdd, SIGNAL(clicked()), this, SLOT(btnElementItemAdd_clicked()));
	connect(Form->btnElementItemRemove, SIGNAL(clicked()), this, SLOT(btnElementItemRemove_clicked()));
	connect(Form->btnElementItemUp, SIGNAL(clicked()), this, SLOT(btnElementItemUp_clicked()));
	connect(Form->btnElementItemDown, SIGNAL(clicked()), this, SLOT(btnElementItemDown_clicked()));
	connect(Form->lstElementItems, SIGNAL(itemSelectionChanged()), this, SLOT(lstElementItems_itemSelectionChanged()));
	connect(Form->cmbItemCategory, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbItemCategory_currentIndexChanged(int)));
	connect(Form->lstItems, SIGNAL(itemSelectionChanged()), this, SLOT(lstItems_itemSelectionChanged()));
}

ZEDToolbarOptionsPageWidget::~ZEDToolbarOptionsPageWidget()
{
	delete Form;
}


// ZEToolbarOptionsPage
//////////////////////////////////////////////////////////////////////////////////////

const char* ZEDToolbarOptionsPage::GetName() const
{
	return "ZEDToolbarOptionsPage";
}

const char* ZEDToolbarOptionsPage::GetText() const
{
	return "Toolbars";
}


const char* ZEDToolbarOptionsPage::GetPath() const
{
	return "User Interface";
}

QWidget* ZEDToolbarOptionsPage::CreateWidget(QWidget* Parent)
{
	return new ZEDToolbarOptionsPageWidget(Parent, GetManager()->GetEditor()->GetMainWindow(), this);
}

void ZEDToolbarOptionsPage::Save(QWidget* Widget)
{
	static_cast<ZEDToolbarOptionsPageWidget*>(Widget)->Save();
	ZEDOptionsPage::Save(Widget);
}

void ZEDToolbarOptionsPage::Default(QWidget* Widget)
{
	static_cast<ZEDToolbarOptionsPageWidget*>(Widget)->Default();
	ZEDOptionsPage::Default(Widget);
}

void ZEDToolbarOptionsPage::Close(QWidget* Widget)
{
	Widget->close();
	ZEDOptionsPage::Close(Widget);
}
