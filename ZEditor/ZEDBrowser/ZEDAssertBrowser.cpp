//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAssertBrowser.cpp
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

#include <QFile>
#include "ZEDAssertBrowser.h"
#include <FreeImage.h>
#include "ZEDAssertBrowserPlugIns\ZEDAssertBrowserFontFilePlugIn.h"

ZEDAssertBrowser::ZEDAssertBrowser(QString WorkingDirectory)
{
	FreeImage_Initialise(true);
	
	this->WorkingDirectory = WorkingDirectory;

	this->CentralFrame = new QFrame(this);
	this->MainMenu = new QMenuBar(this);
	this->BrowserViewPort = new QScrollArea(this->CentralFrame);
	this->DirectoryBrowser = new QTreeView(this->CentralFrame);
	this->VMainLayout = new QVBoxLayout(this);
	this->HMainLayout = new QHBoxLayout(this->CentralFrame);

	this->CreateResourceMenu = new QMenu(QString("CreateResource"), this->MainMenu);
	this->PreviewOptionsMenu = new QMenu(QString("Preview Options"), this->MainMenu);
	this->OperationsMenu = new QMenu(QString("Operations"), this->MainMenu);

	this->PreviewOptionsMenu->addAction(QString("Large"));
	this->PreviewOptionsMenu->addAction(QString("Medium"));
	this->PreviewOptionsMenu->addAction(QString("Small"));

	this->OperationsMenu->addAction(QString("Copy"));
	this->OperationsMenu->addAction(QString("Paste"));
	this->OperationsMenu->addAction(QString("Edit"));
	this->OperationsMenu->addAction(QString("Delete"));
	this->OperationsMenu->addSeparator();
	this->OperationsMenu->addAction(QString("Select All"));
	this->OperationsMenu->addAction(QString("Deselect All"));


	this->MainModel = new QDirModel(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name, 0);
	this->MainDir = new QDir(WorkingDirectory);
	GViewPortLayout = new QGridLayout();
	BrowserViewPortIn = new QWidget(this->BrowserViewPort);
	VBoxLayout = new QVBoxLayout(this->BrowserViewPort);
	this->ToolWidgetArea = new QWidget(this);
	this->PreviewSize = 150;

	this->VMainLayout->setMenuBar(this->MainMenu);
	this->setLayout(this->VMainLayout);
	this->VMainLayout->addWidget(this->ToolWidgetArea);
	this->ToolWidgetArea->setMinimumHeight(20);
	this->CentralFrame->setLayout(this->HMainLayout);
	this->VMainLayout->addWidget(this->CentralFrame);
	this->HMainLayout->addWidget(this->DirectoryBrowser);
	this->HMainLayout->addWidget(this->BrowserViewPort);
	this->MainMenu->addMenu(this->CreateResourceMenu);
	this->MainMenu->addMenu(this->PreviewOptionsMenu);
	this->MainMenu->addMenu(this->OperationsMenu);
	this->BrowserViewPortIn->setLayout(GViewPortLayout);
	this->BrowserViewPort->setLayout(VBoxLayout);
	this->HMainLayout->setMargin(1);
	this->VMainLayout->setMargin(1);
	this->GViewPortLayout->setContentsMargins(1,1,1,1);	

	this->resize(800,600);

	this->DirectoryBrowser->setMaximumWidth(300);

	this->DirectoryBrowser->setModel(MainModel);

	this->DirectoryBrowser->setRootIndex(MainModel->index(MainDir->currentPath()));

	this->BrowserViewPort->setWidget(this->BrowserViewPortIn);

	this->BrowserViewPort->setWidgetResizable(false);

	this->GViewPortLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

	this->HMainLayout->setDirection(QBoxLayout::LeftToRight);

	QObject::connect(this->DirectoryBrowser, SIGNAL(clicked(const QModelIndex &)), this, SLOT(GenerateBrowserViewPortItems()));

	QObject::connect(this->PreviewOptionsMenu->actions()[0], SIGNAL(triggered(bool)), this, SLOT(LargeSelected()));
	QObject::connect(this->PreviewOptionsMenu->actions()[1], SIGNAL(triggered(bool)), this, SLOT(MediumSelected()));
	QObject::connect(this->PreviewOptionsMenu->actions()[2], SIGNAL(triggered(bool)), this, SLOT(SmallSelected()));

	QObject::connect(this->OperationsMenu->actions()[0], SIGNAL(triggered(bool)), this, SLOT(MassCopy()));
	QObject::connect(this->OperationsMenu->actions()[1], SIGNAL(triggered(bool)), this, SLOT(Paste()));
	QObject::connect(this->OperationsMenu->actions()[3], SIGNAL(triggered(bool)), this, SLOT(MassDelete()));
	QObject::connect(this->OperationsMenu->actions()[5], SIGNAL(triggered(bool)), this, SLOT(SelectAll()));
	QObject::connect(this->OperationsMenu->actions()[6], SIGNAL(triggered(bool)), this, SLOT(DeselectAll()));

	this->BrowserViewPort->setContextMenuPolicy(Qt::ActionsContextMenu);
	this->BrowserViewPort->addAction(this->OperationsMenu->actions().at(0));
	this->BrowserViewPort->addAction(this->OperationsMenu->actions().at(1));

	//Tree view context menu implementation.
	//this->DirectoryBrowser->setContextMenuPolicy(Qt::ActionsContextMenu);
	//this->addAction(new QAction(QString("New Folder")));

	SearchLineEdit = new QLineEdit(this->ToolWidgetArea);
	SearchLabel = new QLabel(QString("Search"),this->ToolWidgetArea);
	FilterLabel = new QLabel(QString("Filter"),this->ToolWidgetArea);
	FilterComboBox = new QComboBox(this->ToolWidgetArea);
	RefreshButton = new QPushButton(QString("Refresh"),this->ToolWidgetArea);
	BackButton = new QPushButton(QString("Back"), this->ToolWidgetArea);
	ToolBarLayout = new QHBoxLayout(this->ToolWidgetArea);
	ToolBarLayout->setContentsMargins(1,1,1,1);

	this->ToolWidgetArea->setLayout(this->ToolBarLayout);
	this->ToolBarLayout->addWidget(this->BackButton);
	this->ToolBarLayout->addWidget(this->RefreshButton);
	this->ToolBarLayout->addWidget(this->SearchLabel);
	this->ToolBarLayout->addWidget(this->SearchLineEdit);
	this->ToolBarLayout->addWidget(this->FilterLabel);
	this->ToolBarLayout->addWidget(this->FilterComboBox);

	this->FilterComboBox->addItem(QString("All"));

	QObject::connect(this->SearchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(GenerateBrowserViewPortItems()));
	QObject::connect(this->FilterComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(GenerateBrowserViewPortItems()));
	QObject::connect(this->RefreshButton, SIGNAL(clicked(bool)), this, SLOT(RefreshDirectories()));
	QObject::connect(this->BackButton, SIGNAL(clicked(bool)), this, SLOT(BackButtonClicked()));
}

void ZEDAssertBrowser::GenerateBrowserViewPortItems()
{
	QString CurrentDir;

	ZEDAssertBrowserViewPortItem* CurrentLabel;

	QDirModel* CurrentModel = (QDirModel*)this->DirectoryBrowser->currentIndex().model();

	if(CurrentModel == NULL)
		return;

	CurrentDir = CurrentModel->filePath(this->DirectoryBrowser->currentIndex());

	QDir NewDir(CurrentDir);

	QFileInfoList NewList = NewDir.entryInfoList(QDir::Files);

	for(int I = 0; I < CurrentViewPortContents.count(); I++)
	{
		CurrentViewPortContents[I]->close();
		delete CurrentViewPortContents[I];
	}

	CurrentViewPortContents.clear();

	QString Filter = this->FilterComboBox->currentText();
	int ExtensionCount = 0;

	if(Filter == QString("All"))
	{
		for(int I = 0; I < NewList.count(); I++)
		{
			if(NewList[I].fileName().contains(this->SearchLineEdit->text(),Qt::CaseInsensitive))
			{
				CurrentLabel = new ZEDAssertBrowserViewPortItem(NewList[I].filePath(), NewList[I].fileName(), new QLabel(), this);
				for (int I = 0; I < PlugIns.count(); I++)
				{
					QString*	Extensions;
					Extensions = PlugIns[I]->GetSupportedFileExtension(ExtensionCount);

					for(int J = 0; J < ExtensionCount; J++)
					{
						if (Extensions[J] == CurrentLabel->FileExtension)
						{
							CurrentLabel->PreviewWidget->setPixmap(PlugIns[I]->GetPreview(CurrentLabel->FilePath, PreviewSize));
							break;
						}
					}						
				}
				
				CurrentLabel->setFixedSize(this->PreviewSize, this->PreviewSize);
				CurrentViewPortContents.append(CurrentLabel);
			}
		}
	}

	else
	{
		for(int I = 0; I < NewList.count(); I++)
		{
			if(NewList[I].fileName().contains(this->SearchLineEdit->text(),Qt::CaseInsensitive) && NewList[I].fileName().contains(this->FilterComboBox->currentText(), Qt::CaseInsensitive))
			{
				CurrentLabel = new ZEDAssertBrowserViewPortItem(NewList[I].filePath(), NewList[I].fileName(), new QLabel(), this);
				for (int I = 0; I < PlugIns.count(); I++)
				{
					QString*	Extensions;
					Extensions = PlugIns[I]->GetSupportedFileExtension(ExtensionCount);

					for(int J = 0; J < ExtensionCount; J++)
					{
						if (Extensions[J] == CurrentLabel->FileExtension)
						{
							CurrentLabel->PreviewWidget->setPixmap(PlugIns[I]->GetPreview(CurrentLabel->FilePath, PreviewSize));
							break;
						}
					}	
				}
				CurrentLabel->setFixedSize(this->PreviewSize, this->PreviewSize);
				CurrentViewPortContents.append(CurrentLabel);
			}
		}
	}

	int ContentCount = CurrentViewPortContents.count();

	QSize CurrentViewPortSize = this->BrowserViewPort->size();

	int ColumnCount = (CurrentViewPortSize.width() / PreviewSize);
	int ColumnIndex = 0;
	int RowIndex = 0;

	for(int I = 0; I < CurrentViewPortContents.count() ; I++)
	{
		GViewPortLayout->addWidget(CurrentViewPortContents[I], RowIndex, ColumnIndex);
		ColumnIndex++;

		if(ColumnIndex == ColumnCount)
		{
			ColumnIndex = 0;
			RowIndex++;
		}
	}
}

void ZEDAssertBrowser::LargeSelected()
{
	this->PreviewSize = 512;
	this->GenerateBrowserViewPortItems();
}

void ZEDAssertBrowser::MediumSelected()
{
	this->PreviewSize = 256;
	this->GenerateBrowserViewPortItems();
}

void ZEDAssertBrowser::SmallSelected()
{
	this->PreviewSize = 128;
	this->GenerateBrowserViewPortItems();
}

void ZEDAssertBrowser::BackButtonClicked()
{
	/*for (int I = 0; I < AssertBrowserPlugIns.count(); I++)
	{
		AssertBrowserPlugIns[I]->CloseEditor();
	}

	BrowserViewPort->show();*/
}
	
void ZEDAssertBrowser::SelectAll()
{
	for(int I = 0; I  < this->CurrentViewPortContents.count(); I++)
	{
		this->CurrentViewPortContents[I]->SetSelected(true);
	}
}

void ZEDAssertBrowser::DeselectAll()
{
	for(int I = 0; I  < this->CurrentViewPortContents.count(); I++)
	{
		this->CurrentViewPortContents[I]->SetSelected(true);
	}
}

void ZEDAssertBrowser::CopyFile(QString Source, QString DestinationFolder)
{
	QFile	FileToCopy;
	QString FileName;
	DestinationFolder.append(QString("/"));

	for(int I = Source.count() - 1; I >= 0; I--)
	{
		if(Source.at(I).toLatin1() == '/')
			break;
		
		FileName.prepend(Source.at(I));
	}


	FileToCopy.copy(Source, DestinationFolder + FileName);
}

void ZEDAssertBrowser::MassDelete()
{
	QMessageBox		DeleteMessageBox;
	QString			CountString;
	int				SelectedItemCount = 0;


	for(int I = 0; I < this->CurrentViewPortContents.count(); I++)
	{
		if(this->CurrentViewPortContents[I]->GetSelected())
			SelectedItemCount++;
	}

	CountString.setNum(SelectedItemCount);

	DeleteMessageBox.setText(QString("Are you sure you want to delete ") + CountString + QString(" items ?"));
	DeleteMessageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	DeleteMessageBox.setWindowTitle(QString("Mass Delete"));
	DeleteMessageBox.exec();
	
	QDir DirToDelete;
	QString Temp;

	if(DeleteMessageBox.buttonRole(DeleteMessageBox.clickedButton()) == QMessageBox::YesRole)
	{
		for(int I = 0; I < this->CurrentViewPortContents.count(); I++)
		{
			if(CurrentViewPortContents[I]->GetSelected())
			{
				Temp = this->CurrentViewPortContents[I]->FilePath;
				DirToDelete.setPath(Temp.remove(this->CurrentViewPortContents[I]->FileName));
				DirToDelete.remove(CurrentViewPortContents[I]->FileName);
				this->CurrentViewPortContents[I]->hide();
				this->CurrentViewPortContents.removeAt(I);	
				I--;
			}
		}

		this->GenerateBrowserViewPortItems();
	}
}

void ZEDAssertBrowser::MassCopy()
{
	this->ItemsToCopy.SetCount(0);

	for(int I  = 0; I < this->CurrentViewPortContents.count(); I++)
	{
		if(this->CurrentViewPortContents.at(I)->GetSelected())
		{
			this->ItemsToCopy.Add(this->CurrentViewPortContents.at(I)->FilePath);
		}
	}
}

void ZEDAssertBrowser::Paste()
{
	QDirModel*	CurrentModel = (QDirModel*)this->DirectoryBrowser->currentIndex().model();

	if(CurrentModel == NULL)
		return;

	QString ModelFilePath = CurrentModel->filePath(this->DirectoryBrowser->currentIndex());

	for(int I = 0; I < this->ItemsToCopy.GetCount(); I++)
	{ 
		this->CopyFile(this->ItemsToCopy.GetItem(I), ModelFilePath);
	}

	this->GenerateBrowserViewPortItems();
}

void ZEDAssertBrowser::resizeEvent(QResizeEvent* Event)
{
	this->GenerateBrowserViewPortItems();
}

void ZEDAssertBrowser::RefreshDirectories()
{
	((QDirModel*)(this->DirectoryBrowser->model()))->refresh();
	this->GenerateBrowserViewPortItems();
}

ZEDFilePlugIn* ZEDAssertBrowser::RegisterFilePlugIn(ZEDFilePlugIn* PlugIn)
{
	this->PlugIns.append(PlugIn);

	int ExtensionCount = 0;
	QString* Extensions;
	Extensions = PlugIn->GetSupportedFileExtension(ExtensionCount);

	for (int I = 0; I < ExtensionCount; I++)
	{
		this->FilterComboBox->addItem(Extensions[I]);
	}

	return PlugIn;
}
