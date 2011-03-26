//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - NewZEDAssertBrowserItem.cpp
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

#include "NewZEDAssertBrowserItem.h"
#include "NewZEDAssertBrowser.h"
#include <QFileInfo>
#include <QMouseEvent>
#include <QMenu>

void NewZEDAssertBrowserItem::SetPixmap(QPixmap PreviewPixmap)
{
	this->setPixmap(PreviewPixmap);
}

QString NewZEDAssertBrowserItem::GetFileName() const
{
	return FileName;
}

QString NewZEDAssertBrowserItem::GetExtension() const
{
	return FileExtension;
}

QString NewZEDAssertBrowserItem::GetPath() const
{
	return FilePath;
}

QString NewZEDAssertBrowserItem::GetDirectory() const
{
	return FileDirectory;
}

bool NewZEDAssertBrowserItem::GetIsSelected() const
{
	return IsSelected;
}

void NewZEDAssertBrowserItem::mousePressEvent(QMouseEvent* Event)
{
	QLabel::mousePressEvent(Event);

	if(Event->button() == Qt::LeftButton || Event->button() == Qt::RightButton)
	{
		if(IsSelected == false)
			ParentAssertBrowser->AddSelectedItem(this);
		else if(!(Event->button() == Qt::RightButton))
			ParentAssertBrowser->RemoveSelectedItem(this);
	}
}

void NewZEDAssertBrowserItem::contextMenuEvent(QContextMenuEvent* Event)
{
	QMenu*	ContextMenu = new QMenu(this);

	ContextMenu->addAction(QString("Edit"));
	ContextMenu->addAction(QString("Cut"));
	ContextMenu->addAction(QString("Copy"));
	ContextMenu->addAction(QString("Paste"));
	ContextMenu->addAction(QString("Delete"));
	QAction* SelectedAction = ContextMenu->exec(Event->globalPos());

	if (SelectedAction == NULL)
		return;

	if(SelectedAction->text() == "Edit")
		ParentAssertBrowser->EditActionTriggered();
	else if(SelectedAction->text() == "Cut")
		ParentAssertBrowser->CutActionTriggered();
	else if(SelectedAction->text() == "Copy")
		ParentAssertBrowser->CopyActionTriggered();
	else if(SelectedAction->text() == "Paste")
		ParentAssertBrowser->PasteActionTriggered();
	else if(SelectedAction->text() == "Delete")
		ParentAssertBrowser->DeleteActionTriggered();
}

void NewZEDAssertBrowserItem::SetIsSelected(bool IsSelected)
{

	if(!this->IsSelected)
	{
		setFrameStyle(6);
		setFrameShadow(QFrame::Plain);
		SelectionCheckBox->setChecked(true);
		
		for (int I = 0; I < ParentAssertBrowser->SelectedViewportItems.GetCount(); I++)
		{
			if (ParentAssertBrowser->SelectedViewportItems[I] == this)
				return;
		}
		
		this->IsSelected = IsSelected;
		ParentAssertBrowser->AddSelectedItem(this);
	}

	else
	{
		setFrameStyle(3);
		setFrameShadow(QFrame::Plain);
		SelectionCheckBox->setChecked(false);
		ParentAssertBrowser->SelectedViewportItems.DeleteValue(this);
	}
}

void NewZEDAssertBrowserItem::CheckBoxToogled()
{
	if(SelectionCheckBox->isChecked())
	{
		ParentAssertBrowser->AddSelectedItem(this);
		ParentAssertBrowser->SetMultipleSelection(true);
	}
	else
	{
		ParentAssertBrowser->RemoveSelectedItem(this);
		ParentAssertBrowser->SetMultipleSelection(false);
	}
}

NewZEDAssertBrowserItem::NewZEDAssertBrowserItem(QString& FilePath, NewZEDAssertBrowser* ParentAssertBrowser, QWidget* Parent, Qt::WindowFlags Flags) : QLabel(Parent, Flags)
{
	this->ParentAssertBrowser = ParentAssertBrowser;
	QFileInfo Collector(FilePath);

	IsSelected = false;
	this->FilePath = FilePath;
	this->FileName = Collector.fileName();
	this->FileExtension = Collector.completeSuffix().toUpper();
	FileDirectory = FilePath.remove(FileName, Qt::CaseInsensitive);
	setToolTip(this->FilePath);
	SelectionCheckBox = new QCheckBox(this);
	SelectionCheckBox->setGeometry(ParentAssertBrowser->PreviewSize - 20, 0, 20, 20);

	setFrameStyle(3);
	setFrameShadow(QFrame::Plain);

	QObject::connect(SelectionCheckBox, SIGNAL(clicked(bool)), this, SLOT(CheckBoxToogled()));
}
