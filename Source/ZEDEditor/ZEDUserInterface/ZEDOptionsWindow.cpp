//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDOptionsWindow.cpp
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

#include "ZEDOptionsWindow.h"

#include "ZEDOptionsPage.h"
#include "ui_ZEDOptionsWindow.h"
#include "ZEDOptionsManager.h"
#include "ZEFile/ZEPathTokenizer.h"

#include <QMessageBox>

void ZEDOptionsWindow::UpdateList()
{

}

void ZEDOptionsWindow::trwPages_itemSelectionChanged()
{
	if (Form->trwPages->selectedItems().count() == 0)
		return;

	QTreeWidgetItem* Item = Form->trwPages->selectedItems().at(0);
	ZEDOptionsPage* Page = (ZEDOptionsPage*)Item->data(0, Qt::UserRole).toULongLong();
	for (ZESize I = 0; I < OptionsPageWidgets.GetCount(); I++)
		OptionsPageWidgets[I]->setVisible(Manager->GetOptionsPages()[I] == Page);
}

void ZEDOptionsWindow::btnDefault_clicked()
{
	/*for (ZESize I = 0; I < OptionsPages.GetCount(); I++)
		OptionsPages[I]->Default(OptionsPageWidgets[I]);*/
}

void ZEDOptionsWindow::btnSave_clicked()
{
	/*for (ZESize I = 0; I < OptionsPages.GetCount(); I++)
		OptionsPages[I]->Save(OptionsPageWidgets[I]);*/
}

void ZEDOptionsWindow::btnClose_clicked()
{
	/*bool Modified = false;
	for (ZESize I = 0; I < OptionsPages.GetCount(); I++)
	{
		if (OptionsPages[I]->IsModified())
		{
			Modified = true;
			break;
		}
	}

	if (Modified)
	{
		int Result = QMessageBox::question(this, "Options",
			"There are some unsaved modifications in the options.\n"
			"Are you sure you want to close without saving them ?");

		if (Result == QMessageBox::No)
			return;
	}

	for (ZESize I = 0; I < OptionsPages.GetCount(); I++)
		OptionsPages[I]->Close(OptionsPageWidgets[I]);*/

	close();
}

void ZEDOptionsWindow::Setup(ZEDOptionsManager* Manager)
{
	this->Manager = Manager;

	const ZEArray<ZEDOptionsPage*>& Pages = Manager->GetOptionsPages();
	for (ZESize I = 0; I < Pages.GetCount(); I++)
	{
		ZEDOptionsPage* Page = Pages[I];
		ZEPathTokenizer Tokenizer;
		
		QTreeWidgetItem* ParentItem = NULL;
		Tokenizer.Tokenize(Page->GetPath());
		for (ZESize I = 0; I < Tokenizer.GetTokenCount(); I++)
		{
			if (I == 0)
			{
				for (ZESize I = 0; I < Form->trwPages->topLevelItemCount(); I++)
				{
					QTreeWidgetItem* CurrentItem = Form->trwPages->topLevelItem(I);
					if (CurrentItem->text(0).toStdString() == Tokenizer.GetToken(I))
					{
						ParentItem = CurrentItem;
						break;
					}
				}

				if (ParentItem == NULL)
				{
					QTreeWidgetItem* NewItem = new QTreeWidgetItem();
					NewItem->setText(0, Tokenizer.GetToken(I));
					NewItem->setData(0, Qt::UserRole, QVariant((unsigned long long)Page));
					Form->trwPages->addTopLevelItem(NewItem);
					ParentItem = NewItem;
				}
			}
			else
			{
				for (ZESize I = 0; I < ParentItem->childCount(); I++)
				{
					QTreeWidgetItem* CurrentTreeViewItem = ParentItem->child(I);
					if (CurrentTreeViewItem->text(0).toStdString() == Tokenizer.GetToken(I))
					{
						ParentItem = CurrentTreeViewItem;
						break;
					}
				}

				if (ParentItem == NULL)
				{
					QTreeWidgetItem* NewItem = new QTreeWidgetItem();
					NewItem->setText(0, Tokenizer.GetToken(I));
					NewItem->setData(0, Qt::UserRole, QVariant((unsigned long long)Page));
					ParentItem->addChild(NewItem);
					ParentItem = NewItem;
				}
			}	
		}

		QTreeWidgetItem* NewItem = new QTreeWidgetItem();
		NewItem->setText(0, Pages[I]->GetText());
		NewItem->setData(0, Qt::UserRole, QVariant((unsigned long long)Page));
		if (ParentItem == NULL)
		{
			Form->trwPages->addTopLevelItem(NewItem);
		}
		else
		{
			ParentItem->addChild(NewItem);
			ParentItem->setExpanded(true);
		}

		QWidget* PageWidget = Page->CreateWidget(Form->widOptions);
		PageWidget->setVisible(false);
		OptionsPageWidgets.Add(PageWidget);
		Layout->addWidget(PageWidget);
	}
}

ZEDOptionsWindow::ZEDOptionsWindow(QWidget* Parent) : QDialog(Parent)
{
	Form = new Ui_ZEDOptionsWindow();
	Form->setupUi(this);

	Layout = new QVBoxLayout(Form->widOptions);
	Layout->setMargin(0);
	setLayout(Layout);

	connect(Form->trwPages, SIGNAL(itemSelectionChanged()), this, SLOT(trwPages_itemSelectionChanged()));
	connect(Form->btnDefault, SIGNAL(clicked()), this, SLOT(btnDefault_clicked()));
	connect(Form->btnSave, SIGNAL(clicked()), this, SLOT(btnSave_clicked()));
	connect(Form->btnClose, SIGNAL(clicked()), this, SLOT(btnClose_clicked()));
}

ZEDOptionsWindow::~ZEDOptionsWindow()
{
	delete Form;
}
