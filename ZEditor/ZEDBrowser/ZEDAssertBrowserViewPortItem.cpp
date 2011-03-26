//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAssertBrowserViewPortItem.cpp
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

#include <QContextMenuEvent>
#include <QDrag>
#include <QMimeData>
#include <QMessageBox>
#include <QDir>
#include <QString>
#include <QMouseEvent>
#include "ZEDAssertBrowser.h"
#include "ZEDAssertBrowserViewPortItem.h"
#include <FreeImage.h>

#define DDS QString("DDS")
#define TGA QString("TGA")
#define BMP QString("BMP")
#define ZEMAP QString("ZEMAP")
#define ZEMODEL QString("ZEMODEL")
#define ZESCENE QString("ZESCENE")
#define ZEFONT QString("ZEFONT")

ZEDAssertBrowserViewPortItem::ZEDAssertBrowserViewPortItem(QString FilePath, QString FileName, QLabel* PreviewWidget, ZEDAssertBrowser* ParentBrowser)
{
	this->ParentBrowser = ParentBrowser;

	this->IsSelected = false;
	this->FilePath = FilePath;
	this->FileName = FileName;
	this->PreviewWidget = PreviewWidget;

	for (int I = this->FileName.length() - 1; I >= 0 ; I--)
	{
		if (FileName[I] != QChar('.'))
			this->FileExtension.prepend(FileName[I]);
		else
			break;
	}	 

	FileExtension = FileExtension.toUpper();

	this->ContextMenu = new QMenu(this);
	this->PreviewWidget->setParent(this);
	this->Layout = new QVBoxLayout(this);
	this->setLayout(this->Layout);
	this->Layout->addWidget(this->PreviewWidget);
	this->Layout->setContentsMargins(0,0,0,0);
	this->Layout->setDirection(QBoxLayout::TopToBottom);
	this->setFrameStyle(3);
	this->setFrameShadow(QFrame::Plain);
	this->ContextMenu->addAction(QString("Edit"));
	this->ContextMenu->addAction(QString("Copy"));
	this->ContextMenu->addAction(QString("Delete"));

	QList<QAction*> List;

	List = this->ContextMenu->actions();

	QObject::connect(this->ContextMenu->actions().at(0), SIGNAL(triggered(bool)), this, SLOT(OnContextMenuEditAction()));
	QObject::connect(this->ContextMenu->actions().at(1), SIGNAL(triggered(bool)), this, SLOT(OnContextMenuCopyAction()));
	QObject::connect(this->ContextMenu->actions().at(2), SIGNAL(triggered(bool)), this, SLOT(OnContextMenuDeleteAction()));
}

ZEDAssertBrowserViewPortItem::ZEDAssertBrowserViewPortItem()
{

}

void ZEDAssertBrowserViewPortItem::mouseDoubleClickEvent(QMouseEvent* MouseEvent)		//NeedsRevision
{	
	int ExtensionCount = 0;
	QString* Extensions;

	for (int I = 0; I < ParentBrowser->PlugIns.count(); I++)
	{
		Extensions = ParentBrowser->PlugIns[I]->GetSupportedFileExtension(ExtensionCount);

		for (int J = 0; J < ExtensionCount; J++)
		{
			if(Extensions[J] == FileExtension)
			{
				ParentBrowser->PlugIns[I]->GetEditor(FilePath, ParentBrowser->WorkingDirectory, 0, 0);
				break;
			}
		}
	}

	//ParentBrowser->BrowserViewPort->setVisible(!EditorOpenedFlag);
}

void ZEDAssertBrowserViewPortItem::contextMenuEvent(QContextMenuEvent* MenuEvent)
{
	this->ContextMenu->exec(MenuEvent->globalPos());
}

void ZEDAssertBrowserViewPortItem::mousePressEvent(QMouseEvent* MouseEvent)
{
	if(MouseEvent->button() == Qt::LeftButton)
	{	
		this->setFocus(Qt::MouseFocusReason);
		if(!this->IsSelected)
		{
			this->SetSelected(true);
		}

		else
		{
			this->SetSelected(false);
		}
	}
}

void ZEDAssertBrowserViewPortItem::focusOutEvent(QFocusEvent* FocusEvent)
{

}

void ZEDAssertBrowserViewPortItem::mouseMoveEvent(QMouseEvent* Event)
{
	if(Event->buttons() & Qt::LeftButton)
	{
		this->SetSelected(true);
		QDrag*			Drag = new QDrag(this);
		QMimeData*		MimeData = new QMimeData();

		MimeData->setText(this->FilePath);
		Drag->setMimeData(MimeData);
		Qt::DropAction DropAction = Drag->exec();
	}
}

void ZEDAssertBrowserViewPortItem::OnContextMenuDeleteAction()
{
	QMessageBox		DeleteMessageBox;
	DeleteMessageBox.setText(QString("Are you sure you want to delete ") + this->FileName + QString(" ?"));
	DeleteMessageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	DeleteMessageBox.setWindowTitle(QString("Delete"));
	DeleteMessageBox.exec();

	if(DeleteMessageBox.buttonRole(DeleteMessageBox.clickedButton()) == QMessageBox::YesRole)
	{
		QDir DirToDelete;
		QString Temp = this->FilePath;
		DirToDelete.setPath(Temp.remove(this->FileName));
		DirToDelete.remove(this->FileName);
		ParentBrowser->CurrentViewPortContents.removeAt(ParentBrowser->CurrentViewPortContents.indexOf(this));
		this->hide();
		ParentBrowser->GenerateBrowserViewPortItems();
	}
}

void ZEDAssertBrowserViewPortItem::OnContextMenuEditAction()
{

}

void ZEDAssertBrowserViewPortItem::OnContextMenuCopyAction()
{
	this->ParentBrowser->MassCopy();
}

void ZEDAssertBrowserViewPortItem::SetSelected(bool Selected)
{
	if(Selected)
	{
		this->IsSelected = true;
		this->setFrameStyle(6);
		this->setFrameShadow(QFrame::Plain);
	}

	else
	{
		this->IsSelected = false;
		this->setFrameStyle(3);
		this->setFrameShadow(QFrame::Plain);
	}
}

bool ZEDAssertBrowserViewPortItem::GetSelected()
{
	return this->IsSelected;
}
