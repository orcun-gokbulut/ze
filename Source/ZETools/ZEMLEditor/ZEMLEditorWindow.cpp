//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLEditorWindow.cpp
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

#include "ZEMLEditorWindow.h"
#include "ui_ZEMLEditorWindow.h"

#include <QtGui\QTreeWidget>
#include <QtGui\QMessageBox>
#include <QtGui\QFileDialog>

void ZEMLEditorWindow::LoadNode(QTreeWidgetItem* Item, ZEMLNode* Node)
{
	Item->setText(0, Node->GetName().ToCString());
	Item->setText(1, "Node");

	const ZEList<ZEMLElement>& Elements = Node->GetElements();
	ZEList<ZEMLElement>::Iterator Iterator = Elements.GetConstIterator();
	while (!Iterator.IsEnd())
	{
		QTreeWidgetItem* SubItem = new QTreeWidgetItem(Item);
		SubItem->setText(0, Iterator->GetName().ToCString());
		switch (Iterator->GetType())
		{
			case ZEML_ET1_NODE:
				LoadNode(SubItem, (ZEMLNode*)Iterator.GetItem());
				break;

			case ZEML_ET_PROPERTY:
				SubItem->setText(1, "Property");
				break;

			case ZEML_ET_DATA:
				SubItem->setText(1, "Data");
				break;

			default:
				SubItem->setText(1, "Unknown");
				break;

		}

		Iterator++;
	}
}

void ZEMLEditorWindow::LoadTree()
{
	Form->trwElementTree->clear();

	if (RootNode == NULL)
		return;

	QTreeWidgetItem* TopLevelItem = new QTreeWidgetItem();
	Form->trwElementTree->addTopLevelItem(TopLevelItem);
	TopLevelItem->setData(0, Qt::UserRole, QVariant((qlonglong)RootNode));
	LoadNode(TopLevelItem, RootNode);

	Form->wgtElementEditor->SetElement(NULL);
}

void ZEMLEditorWindow::ValueChanged(ZEMLProperty* Property, const ZEValue& NewValue, const ZEValue& OldValue)
{

}

void ZEMLEditorWindow::CurrentItemChanged(QTreeWidgetItem* Current, QTreeWidgetItem* Previous)
{

}


void ZEMLEditorWindow::Select()
{

}

void ZEMLEditorWindow::Deselect()
{

}

void ZEMLEditorWindow::New()
{
	if (RootNode != NULL)
		delete RootNode;

	RootNode = new ZEMLNode();
	RootNode->SetName("ZEML");
	Root.SetRootNode(RootNode);

	LoadTree();

	Form->trwElementTree->setEnabled(true);
}

void ZEMLEditorWindow::Open()
{
	QString NewFileName = QFileDialog::getOpenFileName(this, 
		"Open ZEML File", "", 
		"ZEML Files (*.ZEML);;ZEML Based Files (*.ZEML *.ZEModel *.ZEIterior *.ZEMaterial);;All Files (*.*)");
	
	if (NewFileName.isNull())
		return;

	ZEMLNode* NewNode = new ZEMLNode();
	ZEMLRoot Root;
	Root.SetRootNode(NewNode);

	ZEString Temp = NewFileName.toUtf8().constData();

	if (!Root.Read(Temp))
	{
		QMessageBox::critical(this, "ZEML Editor", "Cannot load ZEML file.", QMessageBox::Ok);
		return;
	}

	FileName = Temp;
	if (RootNode != NULL)
		delete RootNode;

	RootNode = NewNode;

	LoadTree();

	Form->trwElementTree->setEnabled(true);
}

void ZEMLEditorWindow::Save()
{
	if (RootNode == NULL)
		return;

	if (FileName == "")
	{
		SaveAs();
		return;
	}

	ZEMLRoot Root;
	Root.SetRootNode(RootNode);
	if (!Root.Write(FileName))
	{
		QMessageBox::critical(this, "ZEML Editor", "Cannot save ZEML file.", QMessageBox::Ok);
		return;
	}
}

void ZEMLEditorWindow::SaveAs()
{
	if (RootNode == NULL)
		return;

	QString NewFileName = QFileDialog::getSaveFileName(this, 
		"Open ZEML File", "", 
		"ZEML Files (*.ZEML);;ZEML Based Files (*.ZEML *.ZEModel *.ZEIterior *.ZEMaterial);;All Files (*.*)");

	if (NewFileName.isNull())
		return;

	if (!Root.Write(NewFileName.toStdString().c_str()))
	{
		QMessageBox::critical(this, "ZEML Editor", "Cannot save ZEML file.", QMessageBox::Ok);
		return;
	}

	FileName = NewFileName.toStdString();
}

void ZEMLEditorWindow::Close()
{
	Form->trwElementTree->clear();
	Select();

	if (RootNode != NULL)
	{
		delete RootNode;
		RootNode = NULL;
	}

	Form->trwElementTree->setEnabled(false);

	FileName = "";
}

void ZEMLEditorWindow::Quit()
{
	if (RootNode != NULL)
	{
		int Result = QMessageBox::question(this, "ZEML Editor", 
			"Do you want to save your changes before you quit ?", 
			QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
		
		if (Result == QMessageBox::Yes)
		{
			Save();
			QApplication::exit(EXIT_SUCCESS);
		} 
		else if (Result == QMessageBox::No)
		{
			QApplication::exit(EXIT_SUCCESS);
		}
	}
}

ZEMLEditorWindow::ZEMLEditorWindow()
{
	Form = new Ui_ZEMLEditorWindow();
	Form->setupUi(this);

	Form->trwElementTree->setEnabled(false);
	Form->trwElementTree->setAlternatingRowColors(true);
	Form->trwElementTree->header()->setStretchLastSection(false);
	Form->trwElementTree->header()->setResizeMode(0, QHeaderView::Stretch);
	Form->trwElementTree->header()->setResizeMode(1, QHeaderView::Interactive);

	connect(Form->actNew, SIGNAL(triggered()), this, SLOT(New()));
	connect(Form->actOpen, SIGNAL(triggered()), this, SLOT(Open()));
	connect(Form->actSave, SIGNAL(triggered()), this, SLOT(Save()));
	connect(Form->actSaveAs, SIGNAL(triggered()), this, SLOT(SaveAs()));
	connect(Form->actClose, SIGNAL(triggered()), this, SLOT(Close()));
	connect(Form->actQuit, SIGNAL(triggered()), this, SLOT(Quit()));

	connect(Form->trwElementTree, SIGNAL(CurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem)), this, SLOT(CurrentItemChanged(QTreeWidget*, QTreeWidget*)));
	connect(Form->wgtElementEditor, SIGNAL(ValueChanged(ZEMLProperty*, const ZEValue& NewValue, const ZEValue& OldValue)), this, SLOT(ValueChanged(ZEMLProperty*, const ZEValue& NewValue, const ZEValue& OldValue)));

	RootNode = NULL;

	LoadTree();
}

ZEMLEditorWindow::~ZEMLEditorWindow()
{
	delete Form;
}
