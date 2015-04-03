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

#include <QtCore\QCoreApplication>
#include <QtCore\QSettings>
#include <QtCore\QUrl>
#include <QtGui\QTreeWidget>
#include <QtGui\QMessageBox>
#include <QtGui\QFileDialog>
#include <QtGui\QDesktopServices>

#include "ZEDOperation.h"
#include "ZEOperationAddElement.h"
#include "ZEDOperationManager.h"
#include "ZEOperationChangePropertyValue.h"
#include "ZEOperationChangeElementName.h"
#include "ZEOperationDeleteElements.h"
#include "ZEML\ZEMLProperty.h"
#include "ZEML\ZEMLData.h"
#include "ZEOperationSelection.h"

ZEMLEditorWindow* ZEMLEditorWindow::Instance = NULL;

void ZEMLEditorWindow::LoadNode(QTreeWidgetItem* Item, ZEMLNode* Node)
{
	Item->setText(0, Node->GetName().ToCString());
	Item->setText(1, "Node");
	Item->setData(0, Qt::UserRole, QVariant((qlonglong)Node));

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
				Iterator->SetUserData(SubItem);
				break;

			case ZEML_ET_PROPERTY:
				SubItem->setText(1, "Property");
				SubItem->setData(0, Qt::UserRole, QVariant((qlonglong)Iterator.GetItem()));
				Iterator->SetUserData(SubItem);
				break;

			case ZEML_ET_DATA:
				SubItem->setText(1, "Data");
				SubItem->setData(0, Qt::UserRole, QVariant((qlonglong)Iterator.GetItem()));
				Iterator->SetUserData(SubItem);
				break;

			default:
				SubItem->setText(1, "Unknown");
				SubItem->setData(0, Qt::UserRole, QVariant((qlonglong)Iterator.GetItem()));
				Iterator->SetUserData(SubItem);
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
	LoadNode(TopLevelItem, RootNode);

	Form->wgtElementEditor->SetElement(NULL);
}

void ZEMLEditorWindow::RegisterRecentFile(const ZEString& FileName)
{
	QSettings Settings("Zinek", "ZEMLEditor");
	QStringList RecentFiles = Settings.value("RecentFiles", QStringList()).toStringList();

	int Index = RecentFiles.indexOf(FileName.ToCString());
	if (Index != -1)
		RecentFiles.removeAt(Index);

	if (RecentFiles.size() > 10)
		RecentFiles.removeLast();

	RecentFiles.insert(0, FileName.ToCString());

	Settings.setValue("RecentFiles", RecentFiles);

	LoadRecentFiles();
}

void ZEMLEditorWindow::LoadRecentFiles()
{
	QSettings Settings("Zinek", "ZEMLEditor");
	QStringList RecentFiles = Settings.value("RecentFiles", QStringList()).toStringList();

	Form->mnuRecentFiles->setVisible(!RecentFiles.isEmpty());

	Form->mnuRecentFiles->clear();
	for (int I = 0; I < RecentFiles.size(); I++)
	{
		QAction* RecentFileAction = new QAction(this);
		RecentFileAction->setText(RecentFiles[I]);
		connect(RecentFileAction, SIGNAL(triggered()), this, SLOT(OpenRecentFile()));
		Form->mnuRecentFiles->addAction(RecentFileAction);	
	}
}

void ZEMLEditorWindow::OpenFile(const ZEString& FileName)
{
	if (FileName == "")
		return;

	ZEMLNode* NewNode = new ZEMLNode();
	ZEMLRoot Root;
	Root.SetRootNode(NewNode);
	if (!Root.Read(FileName))
	{
		QMessageBox::critical(this, "ZEML Editor", "Cannot load ZEML file.", QMessageBox::Ok);
		return;
	}

	this->FileName = FileName;
	if (RootNode != NULL)
		delete RootNode;

	RootNode = NewNode;

	ZEDOperationManager::GetInstance()->Clear();

	LoadTree();
	ConfigureUI();
	RegisterRecentFile(FileName);
}

void ZEMLEditorWindow::SaveFile(const ZEString& FileName)
{
	if (RootNode == NULL)
		return;

	if (FileName == NULL)
		return;

	ZEMLRoot Root;
	Root.SetRootNode(RootNode);
	if (!Root.Write(FileName))
	{
		QMessageBox::critical(this, "ZEML Editor", "Cannot save ZEML file.", QMessageBox::Ok);
		return;
	}

	this->FileName = FileName;

	RegisterRecentFile(FileName);
}

void ZEMLEditorWindow::ConfigureUI()
{
	Form->actUndo->setEnabled(ZEDOperationManager::GetInstance()->CanUndo());
	Form->actRedo->setEnabled(ZEDOperationManager::GetInstance()->CanRedo());

	Form->actSave->setEnabled(RootNode != NULL);
	Form->actSaveAs->setEnabled(RootNode != NULL);
	Form->trwElementTree->setEnabled(RootNode != NULL);
	
	int SelectedItem = Form->trwElementTree->selectedItems().count();
	Form->actAddNode->setEnabled(SelectedItem == 1);
	Form->actAddProperty->setEnabled(SelectedItem == 1);
	Form->actAddData->setEnabled(SelectedItem == 1);
	Form->actDelete->setEnabled(SelectedItem > 0);

	Form->actCut->setEnabled(SelectedItem > 0);
	Form->actCopy->setEnabled(SelectedItem > 0);
	Form->actPaste->setEnabled(SelectedItem == 1);
	Form->actDeselect->setEnabled(SelectedItem > 0);

	if (SelectedItem == 1)
	{
		ZEMLElement* Element = (ZEMLElement*)Form->trwElementTree->selectedItems().first()->data(0, Qt::UserRole).toULongLong();
		Form->wgtElementEditor->SetElement(Element);
	}
	else
	{
		Form->wgtElementEditor->SetElement(NULL);
	}

}

void ZEMLEditorWindow::NameChanged(ZEMLElement* Element, const ZEString& NewName, const ZEString& OldName)
{
	ZEDOperationManager::GetInstance()->DoOperation(new ZEOperationChangeElementName(Element, NewName, OldName));
	Update();
}

void ZEMLEditorWindow::ValueChanged(ZEMLProperty* Property, const ZEValue& NewValue, const ZEValue& OldValue)
{
	ZEOperationChangePropertyValue* Operation = new ZEOperationChangePropertyValue(Property, OldValue, NewValue);
	ZEDOperationManager::GetInstance()->DoOperation(Operation);
	Update();
}

void ZEMLEditorWindow::Select()
{
	ZEDOperationManager::GetInstance()->DoOperation(new ZEOperationSelection(Form->trwElementTree->selectedItems()));
	Update();
}

void ZEMLEditorWindow::Deselect()
{
	ZEDOperationManager::GetInstance()->DoOperation(new ZEOperationSelection(QList<QTreeWidgetItem*>()));
	Update();
}

void ZEMLEditorWindow::New()
{
	if (RootNode != NULL)
		delete RootNode;

	RootNode = new ZEMLNode();
	RootNode->SetName("ZEML");
	Root.SetRootNode(RootNode);

	ZEDOperationManager::GetInstance()->Clear();

	ConfigureUI();
	LoadTree();
}

void ZEMLEditorWindow::Open()
{
	QString NewFileName = QFileDialog::getOpenFileName(this, 
		"Open ZEML File", FileName.ToCString(), 
		"ZEML Based Files (*.ZEML *.ZE*);;ZEML Files (*.ZEML);;All Files (*.*)");
	
	OpenFile(NewFileName.toUtf8().constData());

}

void ZEMLEditorWindow::OpenRecentFile()
{
	OpenFile(((QAction*)sender())->text().toUtf8().constData());
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
	QString NewFileName = QFileDialog::getSaveFileName(this, 
		"Open ZEML File", FileName.ToCString(), 
		"ZEML Based Files (*.ZEML *.ZE*);;ZEML Files (*.ZEML);;All Files (*.*)");

	SaveFile(NewFileName.toUtf8().constData());
}

void ZEMLEditorWindow::Close()
{
	if (RootNode != NULL)
		delete RootNode;

	RootNode = NULL;
	FileName = "";

	ZEDOperationManager::GetInstance()->Clear();
	
	LoadTree();
	ConfigureUI();
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

void ZEMLEditorWindow::Undo()
{
	ZEDOperationManager::GetInstance()->Undo();
	Update();
}

void ZEMLEditorWindow::Redo()
{
	ZEDOperationManager::GetInstance()->Redo();
	Update();
}

void ZEMLEditorWindow::AddNewNode()
{
	if (Form->trwElementTree->selectedItems().count() != 1)
		return;

	ZEMLElement* SelectedElement = (ZEMLElement*)Form->trwElementTree->selectedItems()[0]->data(0, Qt::UserRole).toULongLong();
	if (SelectedElement->GetType() != ZEML_ET1_NODE)
		return;

	ZEDOperationManager::GetInstance()->DoOperation(new ZEOperationAddElement((ZEMLNode*)SelectedElement, new ZEMLNode()));
	Update();
}

void ZEMLEditorWindow::AddNewProperty()
{
	if (Form->trwElementTree->selectedItems().count() != 1)
		return;

	ZEMLElement* SelectedElement = (ZEMLElement*)Form->trwElementTree->selectedItems()[0]->data(0, Qt::UserRole).toULongLong();
	if (SelectedElement->GetType() != ZEML_ET1_NODE)
		return;

	ZEDOperationManager::GetInstance()->DoOperation(new ZEOperationAddElement((ZEMLNode*)SelectedElement, new ZEMLProperty()));
	Update();
}

void ZEMLEditorWindow::AddNewData()
{
	if (Form->trwElementTree->selectedItems().count() != 1)
		return;

	ZEMLElement* SelectedElement = (ZEMLElement*)Form->trwElementTree->selectedItems()[0]->data(0, Qt::UserRole).toULongLong();
	if (SelectedElement->GetType() != ZEML_ET1_NODE)
		return;

	ZEDOperationManager::GetInstance()->DoOperation(new ZEOperationAddElement((ZEMLNode*)SelectedElement, new ZEMLData()));
	Update();
}

void ZEMLEditorWindow::Delete()
{
	if (Form->trwElementTree->selectedItems().count() == 0)
		return;

	ZEOperationDeleteElements* Operation = new ZEOperationDeleteElements();
	QList<QTreeWidgetItem*> Items = Form->trwElementTree->selectedItems();
	for (ZESize I = 0; Items.count(); I++)
	{
		ZEMLElement* Element = (ZEMLElement*)Items[I]->data(0, Qt::UserRole).toULongLong();
		Operation->AddDeletedElement(Element);
	}

	ZEDOperationManager::GetInstance()->DoOperation(Operation);
	Update();
}

void ZEMLEditorWindow::UserGuide()
{
	QMessageBox::information(this, "ZEML Editor", "Sorry, there is no user guide available right now.\nHowever this is not Matlab or 3ds Max so figure out yourself.", QMessageBox::Ok);
}

void ZEMLEditorWindow::BugReport()
{
	QDesktopServices::openUrl(QUrl("http://www.zinekengine.com/bugreport"));
}

void ZEMLEditorWindow::Website()
{
	QDesktopServices::openUrl(QUrl("http://www.zinekengine.com"));
}

void ZEMLEditorWindow::About()
{
	QMessageBox::about(this, "ZEML Editor", "Version: 1.0");
}

Ui_ZEMLEditorWindow* ZEMLEditorWindow::GetForm()
{
	return Form;
}

void ZEMLEditorWindow::Update()
{
	ConfigureUI();
	Form->wgtElementEditor->Update();
}

ZEMLEditorWindow::ZEMLEditorWindow()
{
	Instance = this;

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

	connect(Form->actUndo, SIGNAL(triggered()), this, SLOT(Undo()));
	connect(Form->actRedo, SIGNAL(triggered()), this, SLOT(Redo()));
	connect(Form->actDeselect, SIGNAL(triggered()), this, SLOT(Deselect()));

	connect(Form->actAddNode, SIGNAL(triggered()), this, SLOT(AddNewNode()));
	connect(Form->actAddProperty, SIGNAL(triggered()), this, SLOT(AddNewProperty()));
	connect(Form->actAddData, SIGNAL(triggered()), this, SLOT(AddNewData()));
	connect(Form->actDelete, SIGNAL(triggered()), this, SLOT(Delete()));

	connect(Form->actUserGuide, SIGNAL(triggered()), this, SLOT(UserGuide()));
	connect(Form->actBugReport, SIGNAL(triggered()), this, SLOT(BugReport()));
	connect(Form->actWebsite, SIGNAL(triggered()), this, SLOT(Website()));
	connect(Form->actAbout, SIGNAL(triggered()), this, SLOT(About()));

	connect(Form->trwElementTree, SIGNAL(itemSelectionChanged()), this, SLOT(Select()));
	connect(Form->wgtElementEditor, SIGNAL(ValueChanged(ZEMLProperty*, const ZEValue&, const ZEValue&)), this, SLOT(ValueChanged(ZEMLProperty*, const ZEValue&, const ZEValue&)));
	connect(Form->wgtElementEditor, SIGNAL(NameChanged(ZEMLElement*, const ZEString&, const ZEString&)), this, SLOT(NameChanged(ZEMLElement*, const ZEString&, const ZEString&)));

	RootNode = NULL;

	LoadTree();
	LoadRecentFiles();
	ConfigureUI();
}

ZEMLEditorWindow::~ZEMLEditorWindow()
{
	delete Form;
}

ZEMLEditorWindow* ZEMLEditorWindow::GetInstance()
{
	return Instance;
}
