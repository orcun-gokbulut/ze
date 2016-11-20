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

void ZEDCustomizeUIWindow::UpdateItems()
{
	QSignalBlocker lstItemsBlocker(Form->lstItems);

	Form->lstItems->clear();
	if (Form->radMenus->isChecked())
	{
		const ZEArray<ZEDMenu*>& Menus = MainWindow->GetMenuManager()->GetMenus();
		for (ZESize I = 0; I < Menus.GetCount(); I++)
		{
			ZEDMenu* Menu = Menus[I];
			if (Menu->GetName() != Form->cmbName->currentText())
				continue;

			const ZEArray<ZEDMenuItem*>& Items = Menu->GetItems();
			for (ZESize I = 0; I < Items.GetCount(); I++)
			{
				if (Items[I]->GetType() == ZED_MIT_SEPERATOR)
					Form->lstItems->addItem("<Seperator>");
				else
					Form->lstItems->addItem(Items[I]->GetTargetName().ToCString());
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
			if (Toolbar->GetName() != Form->cmbName->currentText())
				continue;

			const ZEArray<ZEDToolbarItem*>& Items = Toolbar->GetItems();
			for (ZESize I = 0; I < Items.GetCount(); I++)
			{
				if (Items[I]->GetType() == ZED_MIT_SEPERATOR)
					Form->lstItems->addItem("<Seperator>");
				else
					Form->lstItems->addItem(Items[I]->GetTargetName().ToCString());
			}

			break;
		}
	}
}

void ZEDCustomizeUIWindow::UpdateCommands()
{
	QSignalBlocker lstCommandsBlocker(Form->lstCommands);

	ZEArray<ZEDCommand*> Commands;
	if (Form->cmbCategory->currentText() == "<All>")
		Commands = ZEDCommandManager::GetInstance()->GetCommands();
	else
		Commands = ZEDCommandManager::GetInstance()->GetCommands(Form->cmbCategory->currentText().toStdString());

	Form->lstCommands->clear();
	Form->lstCommands->addItem("<Seperator>");
	for (ZESize I = 0; I < Commands.GetCount(); I++)
		Form->lstCommands->addItem(Commands[I]->GetName().ToCString());
}

void ZEDCustomizeUIWindow::UpdateNames()
{
	QSignalBlocker cmbNameBlocker(Form->cmbName);

	if (Form->radMenus->isChecked())
	{
		Form->cmbName->clear();
		const ZEArray<ZEDMenu*>& Menus = MainWindow->GetMenuManager()->GetMenus();
		for (ZESize I = 0; I < Menus.GetCount(); I++)
			Form->cmbName->addItem(Menus[I]->GetName().ToCString());
	}
	else
	{
		Form->cmbName->clear();
		const ZEArray<ZEDToolbar*>& Toolbars = MainWindow->GetToolbarManager()->GetToolbars();
		for (ZESize I = 0; I < Toolbars.GetCount(); I++)
			Form->cmbName->addItem(Toolbars[I]->GetName().ToCString());
	}
}

void ZEDCustomizeUIWindow::UpdateCategories()
{
	QSignalBlocker cmbCategoryBlocker(Form->cmbCategory);

	ZEArray<ZEString> Categories = ZEDCommandManager::GetInstance()->GetCatagories();
	Form->cmbCategory->clear();
	Form->cmbCategory->addItem("<All>");
	for (ZESize I = 0; I < Categories.GetCount(); I++)
		Form->cmbCategory->addItem(Categories[I].ToCString());

}

void ZEDCustomizeUIWindow::radSelection_toggled(bool)
{
	UpdateNames();
	UpdateItems();
}

void ZEDCustomizeUIWindow::btnNew_clicked()
{

}

void ZEDCustomizeUIWindow::btnDelete_clicked()
{

}

void ZEDCustomizeUIWindow::cmbCategory_currentIndexChanged(int)
{
	UpdateCommands();
}

void ZEDCustomizeUIWindow::cmbName_currentIndexChanged(int)
{
	UpdateItems();
}

void ZEDCustomizeUIWindow::txtName_textChanged(const QString&)
{
	if (Form->radMenus->isChecked())
	{
		ZEDMenu* Menu = MainWindow->GetMenuManager()->GetMenus()[Form->cmbName->currentIndex()];
		Menu->SetName(Form->txtName->text().toStdString());
	}
	else
	{
		ZEDToolbar* Toolbar = MainWindow->GetToolbarManager()->GetToolbars()[Form->cmbName->currentIndex()];
		Toolbar->SetName(Form->txtName->text().toStdString());
	}
}

void ZEDCustomizeUIWindow::txtTitle_textChanged(const QString&)
{
	if (Form->radMenus->isChecked())
	{
		ZEDMenu* Menu = MainWindow->GetMenuManager()->GetMenus()[Form->cmbName->currentIndex()];
		Menu->SetName(Form->txtName->text().toStdString());
	}
	else
	{
		ZEDToolbar* Toolbar = MainWindow->GetToolbarManager()->GetToolbars()[Form->cmbName->currentIndex()];
		Toolbar->SetName(Form->txtName->text().toStdString());
	}
}

void ZEDCustomizeUIWindow::btnAddItem_clicked()
{

}

void ZEDCustomizeUIWindow::btnRemoveItem_clicked()
{

}

void ZEDCustomizeUIWindow::btnMoveUpItem_clicked()
{

}

void ZEDCustomizeUIWindow::btnMoveDownItem_clicked()
{

}

ZEDCustomizeUIWindow::ZEDCustomizeUIWindow(ZEDMainWindow* MainWindow)
{
	this->MainWindow = MainWindow;
	Form = new Ui_ZEDCustomizeUIWindow();
	Form->setupUi(this);

	UpdateCategories();
	UpdateCommands();
	UpdateNames();
	UpdateItems();

	connect(Form->radMenus, SIGNAL(toggled(bool)), this, SLOT(radSelection_toggled(bool)));
	connect(Form->radToolbars, SIGNAL(toggled(bool)), this, SLOT(radSelection_toggled(bool)));
	connect(Form->btnNew, SIGNAL(clicked()), this, SLOT(btnNew_clicked()));
	connect(Form->btnDelete, SIGNAL(clicked()), this, SLOT(btnDelete_clicked()));
	connect(Form->cmbName, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbName_currentIndexChanged(int)));
	connect(Form->txtName, SIGNAL(textChanged(const QString&)), this, SLOT(txtName_textChanged(const QString&)));
	connect(Form->txtTitle, SIGNAL(textChanged(const QString&)), this, SLOT(txtTitle_textChanged(const QString&)));
	connect(Form->btnAddItem, SIGNAL(clicked()), this, SLOT(btnAddItem_clicked()));
	connect(Form->btnRemoveItem, SIGNAL(clicked()), this, SLOT(btnRemoveItem_clicked()));
	connect(Form->btnMoveUpItem, SIGNAL(clicked()), this, SLOT(btnMoveUpItem_clicked()));
	connect(Form->btnMoveDownItem, SIGNAL(clicked()), this, SLOT(btnMoveDownItem_clicked()));
	connect(Form->cmbCategory, SIGNAL(currentIndexChanged(int)), this, SLOT(cmbCategory_currentIndexChanged(int)));
}

ZEDCustomizeUIWindow::~ZEDCustomizeUIWindow()
{
	delete Form;
}
