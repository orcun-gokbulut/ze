//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - NewZEDAssertBrowser.cpp
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

#include "NewZEDAssertBrowser.h"
#include <stdlib.h>

#define SmallPreviewSize	128
#define MediumPreviewSize	256
#define LargePreviewSize	512

#define PreviewSpacing		1

#define ALL					"All	"
NewZEDAssertBrowser::NewZEDAssertBrowser(QString WorkingDirectory, QWidget *Parent, Qt::WFlags Flags) : QMainWindow(Parent, Flags)
{
	AssertBrowserUI.setupUi(this);
	InitializeSearchToolBar();
	DirectoryModel = new QDirModel(AssertBrowserUI.DirectoryBrowser);
	DirectoryModel->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
	AssertBrowserUI.DirectoryBrowser->setModel(DirectoryModel);
	AssertBrowserUI.DirectoryBrowser->setRootIndex(DirectoryModel->index(QString(WorkingDirectory)));
	PreviewsLayout = new QGridLayout(AssertBrowserUI.PreviewsViewport->widget());
	PreviewsLayout->setContentsMargins(PreviewSpacing,PreviewSpacing,PreviewSpacing,PreviewSpacing);
	AssertBrowserUI.PreviewsViewport->setMinimumWidth(600);
	PreviewSize = MediumPreviewSize;
	PreviewsLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
	SelectedViewportItems.Clear();
	MutipleSelectionEnabled = false;
	CutOperationFlag = false;

	QObject::connect(AssertBrowserUI.DirectoryBrowser,	SIGNAL(clicked(const QModelIndex &)),	this, SLOT(GeneratePreviewItems()));
	QObject::connect(FilterComboBox,					SIGNAL(currentIndexChanged(int)),		this, SLOT(GeneratePreviewItems()));
	QObject::connect(SearchLineEdit,					SIGNAL(textChanged(const QString&)),	this, SLOT(GeneratePreviewItems()));
	QObject::connect(AssertBrowserUI.SmallAction,		SIGNAL(triggered(bool)),				this, SLOT(SmallClicked()));
	QObject::connect(AssertBrowserUI.MediumAction,		SIGNAL(triggered(bool)),				this, SLOT(MediumClicked()));
	QObject::connect(AssertBrowserUI.LargeAction,		SIGNAL(triggered(bool)),				this, SLOT(LargeClicked()));
	QObject::connect(AssertBrowserUI.SelectAllAciton,	SIGNAL(triggered(bool)),				this, SLOT(SelectAllActionTriggered()));
	QObject::connect(AssertBrowserUI.DesellectAction,	SIGNAL(triggered(bool)),				this, SLOT(DesellectActionTriggered()));

	QObject::connect(AssertBrowserUI.CopyAction,		SIGNAL(triggered(bool)),				this, SLOT(CopyActionTriggered()));
	QObject::connect(AssertBrowserUI.CutAction,			SIGNAL(triggered(bool)),				this, SLOT(CutActionTriggered()));
	QObject::connect(AssertBrowserUI.PasteAction,		SIGNAL(triggered(bool)),				this, SLOT(PasteActionTriggered()));
	QObject::connect(AssertBrowserUI.DeleteAction,		SIGNAL(triggered(bool)),				this, SLOT(DeleteActionTriggered()));
	QObject::connect(AssertBrowserUI.EditAction,		SIGNAL(triggered(bool)),				this, SLOT(EditActionTriggered()));
}

void NewZEDAssertBrowser::InitializeSearchToolBar()
{
	SearchLabel		 = new QLabel(QString(" Search "), AssertBrowserUI.SearchToolBar);
	SearchLineEdit	 = new QLineEdit(AssertBrowserUI.SearchToolBar);
	SearchLineEdit->setMinimumHeight(20);
	FilterLabel		 = new QLabel(QString(" Filter "), AssertBrowserUI.SearchToolBar);
	FilterComboBox	 = new QComboBox(AssertBrowserUI.SearchToolBar);

	AssertBrowserUI.SearchToolBar->addWidget(SearchLabel);
	AssertBrowserUI.SearchToolBar->addWidget(SearchLineEdit);
	AssertBrowserUI.SearchToolBar->addSeparator();
	AssertBrowserUI.SearchToolBar->addWidget(FilterLabel);
	AssertBrowserUI.SearchToolBar->addWidget(FilterComboBox);

	FilterComboBox->addItem(QString(ALL));
}

void NewZEDAssertBrowser::RegisterPlugIn(ZEDFilePlugIn* PlugIn)
{
	this->PlugIns.Add(PlugIn);
	int ExtensionCount = 0;

	QString* Extensions = PlugIn->GetSupportedFileExtension(ExtensionCount);

	for (int I = 0; I < ExtensionCount; I++)
		FilterComboBox->addItem(Extensions[I]);
}

void NewZEDAssertBrowser::GeneratePreviewItems()
{
	for (int I = 0; I < ViewportContents.GetCount(); I++)
	{
		if(ViewportContents[I] != NULL)
		{
			ViewportContents[I]->close();
			delete ViewportContents[I];
		}
	}

	ViewportContents.Clear();
	SelectedViewportItems.Clear();

	int ViewportWidth = AssertBrowserUI.PreviewsViewport->width();

	QDirModel* SelectedDirectoryModel = NULL;
	SelectedDirectoryModel = ((QDirModel*)(AssertBrowserUI.DirectoryBrowser->currentIndex().model()));

	if(SelectedDirectoryModel == NULL)
		return;

	QString SelectedPath = SelectedDirectoryModel->filePath(AssertBrowserUI.DirectoryBrowser->currentIndex());
	QDir SelectedDirectory = QDir(SelectedPath);

	QFileInfoList SelectedDirectoryFileList = SelectedDirectory.entryInfoList(QDir::Files);

	int ExpansionRow = 0;
	int ExpansionColumn = 0;

	int RowItemCapacity = floor((float)ViewportWidth / (float)PreviewSize);
	int SpacePixexls = 6 * RowItemCapacity;
	
	if(SpacePixexls + PreviewSize * RowItemCapacity > ViewportWidth)
		RowItemCapacity--;

	NewZEDAssertBrowserItem* CurrentItem = NULL;

	for (int I = 0; I < SelectedDirectoryFileList.count(); I++)
	{
		CurrentItem = new NewZEDAssertBrowserItem(SelectedDirectoryFileList[I].filePath(), this, AssertBrowserUI.PreviewsViewport->widget());
		
		if(FilterComboBox->currentText() != ALL)
		{
			if(CurrentItem->GetExtension() != FilterComboBox->currentText())
			{
				CurrentItem->close();
				delete CurrentItem;
				continue;
			}
		}

		if(SearchLineEdit->text().length() != 0)
		{
			if(!CurrentItem->GetFileName().contains(SearchLineEdit->text(), Qt::CaseInsensitive))
			{
				CurrentItem->close();
				delete CurrentItem;
				continue;
			}
		}

		ViewportContents.Add(CurrentItem);
		CurrentItem->setFixedHeight(PreviewSize);
		CurrentItem->setFixedWidth(PreviewSize);

		for (int I = 0; I < PlugIns.GetCount(); I++)
		{
			int		 ExtensionCount = 0;
			QString* Extensions = PlugIns[I]->GetSupportedFileExtension(ExtensionCount);

			for (int J = 0; J < ExtensionCount; J++)
			{
				if(Extensions[J] == CurrentItem->GetExtension())
					CurrentItem->setPixmap(PlugIns[I]->GetPreview(CurrentItem->GetPath(), PreviewSize));
			}
		}

		PreviewsLayout->addWidget(CurrentItem,ExpansionRow,ExpansionColumn);
		ExpansionColumn++;

		if(ExpansionColumn == RowItemCapacity)
		{
			ExpansionRow++;
			ExpansionColumn = 0;
		}
	}

	AssertBrowserUI.statusbar->showMessage(QString().setNum(ViewportContents.GetCount()) + QString(" File(s)"));
}

void NewZEDAssertBrowser::SmallClicked()
{
	PreviewSize = SmallPreviewSize;
	GeneratePreviewItems();
}

void NewZEDAssertBrowser::MediumClicked()
{
	PreviewSize = MediumPreviewSize;
	GeneratePreviewItems();
}

void NewZEDAssertBrowser::LargeClicked()
{
	PreviewSize = LargePreviewSize;
	GeneratePreviewItems();
}

void NewZEDAssertBrowser::AddSelectedItem(NewZEDAssertBrowserItem* SelectedItem)
{
	for (int I = 0; I < SelectedViewportItems.GetCount(); I++)
	{
		if(SelectedViewportItems[I] == SelectedItem)
			return;
	}

	if(MutipleSelectionEnabled)
	{
		SelectedItem->SetIsSelected(true);
		SelectedViewportItems.Add(SelectedItem);
	}

	else
	{
		for (int I = 0; I < SelectedViewportItems.GetCount(); I++)
		{
			SelectedViewportItems[I]->SetIsSelected(false);
		}
		SelectedViewportItems.Clear();
		SelectedItem->SetIsSelected(true);
		//SelectedViewportItems.Add(SelectedItem);
	}
}

void NewZEDAssertBrowser::RemoveSelectedItem(NewZEDAssertBrowserItem* SelectedItem)
{
	SelectedItem->SetIsSelected(false);
	SelectedViewportItems.DeleteValue(SelectedItem);

	if (SelectedViewportItems.GetCount() == 0)
	{
		MutipleSelectionEnabled = false;
	}
}

void NewZEDAssertBrowser::SetMultipleSelection(bool Enabled)
{
	if (Enabled == false)
	{
		if(SelectedViewportItems.GetCount() == 0)
		{
			this->MutipleSelectionEnabled = Enabled;
			return;
		}
	}

	else
		MutipleSelectionEnabled = Enabled;

}

void NewZEDAssertBrowser::SelectAllActionTriggered()
{
	SelectedViewportItems.Clear();

	MutipleSelectionEnabled = true;

	for (int I = 0; I < ViewportContents.GetCount(); I++)
	{
		ViewportContents[I]->SetIsSelected(true);
	}

	MutipleSelectionEnabled = false;
}

void NewZEDAssertBrowser::DesellectActionTriggered()
{
	QList<NewZEDAssertBrowserItem*> TestList;

	for (int I = 0; I < SelectedViewportItems.GetCount(); I++)
	{
		TestList.append(SelectedViewportItems[I]);
	}

	for (int I = 0; I < TestList.count(); I++)
	{
		TestList[I]->SetIsSelected(false);
	}

	SelectedViewportItems.Clear();
}

void NewZEDAssertBrowser::CutActionTriggered()
{
	for (int I = 0; I < FilesToManipulate.count(); I++)
	{
		FilesToManipulate[I]->close();
		delete FilesToManipulate[I];
	}

	FilesToManipulate.clear();

	for (int I = 0; I < SelectedViewportItems.GetCount(); I++)
	{
		FilesToManipulate.append(new QFile(SelectedViewportItems[I]->GetPath()));
	}

	CutOperationFlag = true;
}

void NewZEDAssertBrowser::CopyActionTriggered()
{
	for (int I = 0; I < FilesToManipulate.count(); I++)
	{
		FilesToManipulate[I]->close();
		delete FilesToManipulate[I];
	}

	FilesToManipulate.clear();

	for (int I = 0; I < SelectedViewportItems.GetCount(); I++)
	{
		FilesToManipulate.append(new QFile(SelectedViewportItems[I]->GetPath()));
	}
}

void NewZEDAssertBrowser::PasteActionTriggered()
{
	QDirModel* SelectedDirectoryModel = NULL;
	SelectedDirectoryModel = ((QDirModel*)(AssertBrowserUI.DirectoryBrowser->currentIndex().model()));

	if(SelectedDirectoryModel == NULL)
		return;

	QString SelectedPath = SelectedDirectoryModel->filePath(AssertBrowserUI.DirectoryBrowser->currentIndex());

	for (int I = 0; I < FilesToManipulate.count(); I++)
	{
		QFileInfo CurrentFileInfo = QFileInfo(FilesToManipulate[I]->fileName());
		FilesToManipulate[I]->copy(SelectedPath + QString("/") +CurrentFileInfo.fileName());

		if(CutOperationFlag == true)
		{
			FilesToManipulate[I]->remove();
			CutOperationFlag = false;
		}
	}

	GeneratePreviewItems();
}

void NewZEDAssertBrowser::DeleteActionTriggered()
{
	for (int I = 0; I < SelectedViewportItems.GetCount(); I++)
	{
		QFile* File = new QFile(SelectedViewportItems[I]->GetPath());
		File->remove();
		delete File;
	}

	SelectedViewportItems.Clear();
	GeneratePreviewItems();
}

void NewZEDAssertBrowser::EditActionTriggered()
{
	for (int I = 0; I < SelectedViewportItems.GetCount(); I++)
	{
		for (int J = 0; J < PlugIns.GetCount(); J++)
		{
			int ExtensionCount = 0;
			QString* Extensions = NULL;
			Extensions = PlugIns[J]->GetSupportedFileExtension(ExtensionCount);

			for (int K = 0; K < ExtensionCount; K++)
			{
				if(Extensions[K] == SelectedViewportItems[I]->GetExtension())
				{
					PlugIns[J]->GetEditor(SelectedViewportItems[I]->GetPath(), QDir::currentPath());
				}
			}
		}
	}
}
