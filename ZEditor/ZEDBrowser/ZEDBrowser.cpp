//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDBrowser.cpp
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

#include <ui_ZEDBrowser.h>
#include "ZEDBrowser.h"
#include "ZEDCore/ZEDFileExtension.h"
#include "ZEDBrowserItem.h"

#define WorkingDir "C:/Users/Can/Desktop/ZE/trunk/RunDir"

ZEDBrowser::ZEDBrowser(QWidget *Parent, Qt::WFlags Flags) : QMainWindow(Parent, Flags)
{
	AssertBrowserUI = new Ui::ZEDBrowserUI();
	AssertBrowserUI->setupUi(this);
	MultipleSelectionEnabled = false;
	DirectoryTree = new ZEDDirectoryTreeWidget(QDir(WorkingDir), this);
	AssertBrowserUI->LeftLayout->addWidget(DirectoryTree);	
	BrowserItemsLayout = new QGridLayout(AssertBrowserUI->BrowserScrollArea->widget());
	BrowserItemsLayout->setContentsMargins(1, 1, 1, 1);
	BrowserItemsLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
	showMaximized();
	QList<int> WidgetSizes;
	WidgetSizes.append(300);
	WidgetSizes.append(1600);
	AssertBrowserUI->splitter->setSizes(WidgetSizes);

	SeperatorAction = new QAction(this);
	SeperatorAction->setSeparator(true);

	ContextMenuActions.append(SeperatorAction);
	ContextMenuActions.append(new QAction("Copy", this));
	ContextMenuActions.append(new QAction("Cut", this));
	ContextMenuActions.append(new QAction("Paste", this));
	ContextMenuActions.append(new QAction("Delete", this));

	QObject::connect(DirectoryTree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(DirectorySelected(QTreeWidgetItem*, QTreeWidgetItem*)));

	QObject::connect(ContextMenuActions[0], SIGNAL(triggered(bool)), this, SLOT(CopyActionTriggered()));
	QObject::connect(ContextMenuActions[1], SIGNAL(triggered(bool)), this, SLOT(CutActionTriggered()));
	QObject::connect(ContextMenuActions[2], SIGNAL(triggered(bool)), this, SLOT(PasteActionTriggered()));
	QObject::connect(ContextMenuActions[3], SIGNAL(triggered(bool)), this, SLOT(DeleteActionTriggered()));
}

void ZEDBrowser::DirectorySelected(QTreeWidgetItem* Current, QTreeWidgetItem* Previous)
{
	ClearBrowserItems();
	GenerateBrowserItems((ZEDDirectoryTreeWidgetItem*)Current);
}

void ZEDBrowser::GenerateBrowserItems(ZEDDirectoryTreeWidgetItem* Current)
{
	int ExpansionRow = 0;
	int ExpansionColumn = 0;

	int RowItemCapacity = floor((float)AssertBrowserUI->BrowserScrollArea->width() / 256.0f);
	int SpacePixexls = 6 * RowItemCapacity;

	if(SpacePixexls + 256 * RowItemCapacity > AssertBrowserUI->BrowserScrollArea->width())
		RowItemCapacity--;

	QStringList Files = Current->GetDirectory().entryList(QStringList(), QDir.Files | QDir.NoDotAndDotDot, QDir.Name);

	for (int I = 0; I < Files.count(); I++)
	{
		ZEDBrowserItem* NewItem = new ZEDBrowserItem(this, AssertBrowserUI->BrowserScrollArea->widget() ,WorkingDir + Current->GetDirectory().path().remove(0,1) + "/" + Files[I]);
		BrowserItems.append(NewItem);
		BrowserItemsLayout->addWidget(NewItem, ExpansionRow, ExpansionColumn);

		ExpansionColumn++;

		if(ExpansionColumn == RowItemCapacity)
		{
			ExpansionRow++;
			ExpansionColumn = 0;
		}
	}
	
	AssertBrowserUI->StatusBar->showMessage(QString::number(Files.count()) + " File(s)");
}

QList<QAction*> ZEDBrowser::GetBrowserContextMenuActions()
{
	return ContextMenuActions;
}

void ZEDBrowser::ClearBrowserItems()
{
	for (int I = 0; I < BrowserItems.count(); I++)
	{
		BrowserItems.at(I)->close();
		delete BrowserItems.at(I);
		BrowserItems[I] = NULL;
	}

	BrowserItems.clear();
}

void ZEDBrowser::ItemSelected(ZEDBrowserItem* SelectedItem)
{
	if(!MultipleSelectionEnabled)
		ClearSelectedItems();

	if(!SelectedBrowserItems.contains(SelectedItem))
		SelectedBrowserItems.append(SelectedItem);
}

void ZEDBrowser::ItemDeselected(ZEDBrowserItem* SelectedItem)
{
	SelectedBrowserItems.removeAt(SelectedBrowserItems.indexOf(SelectedItem));
}

void ZEDBrowser::ClearSelectedItems()
{
	for (int I = 0; I < SelectedBrowserItems.count(); I++)
	{
		SelectedBrowserItems.at(I)->SetSelected(false);
	}

	SelectedBrowserItems.clear();
}

void ZEDBrowser::CopyActionTriggered()
{

}

void ZEDBrowser::CutActionTriggered()
{

}

void ZEDBrowser::PasteActionTriggered()
{

}

void ZEDBrowser::DeleteActionTriggered()
{

}
