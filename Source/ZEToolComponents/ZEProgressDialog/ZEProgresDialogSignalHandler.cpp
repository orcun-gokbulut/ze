//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEProgresDialogSignalHandler.cpp
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

#include "ZEProgresDialogSignalHandler.h"
#include "ui_ZEProgressDialog.h"
#include "ZEProgressDialogTask.h"
#include "QtGui\QFileDialog"
#include "QtCore\QTextStream"
#include "QtGui\QApplication"
#include "QtGui\QClipboard"

ZEProgressDialogSignalHandler::ZEProgressDialogSignalHandler(QObject* Parent) : QObject(Parent)
{

}

ZEProgressDialogSignalHandler::ZEProgressDialogSignalHandler(ZEProgressDialog* ProgressDialog, QObject* Parent) : QObject(Parent)
{
	this->ProgressDialog = ProgressDialog;
	connect((const QObject*)ProgressDialog->TasksTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(TreeItemDoubleClicled(QTreeWidgetItem*, int)));
	connect(ProgressDialog->Form->btnCopyAll, SIGNAL(clicked(bool)), this, SLOT(CopyAllLog()));
	connect(ProgressDialog->Form->btnSave, SIGNAL(clicked(bool)), this, SLOT(SaveLog()));
}

void ZEProgressDialogSignalHandler::TreeItemDoubleClicled(QTreeWidgetItem* Item, int Column)
{
	for (ZESize I = 0; I < ProgressDialog->Tasks.GetCount(); I++)
	{
		if(Item == ProgressDialog->Tasks[I]->TreeItem)
		{
			if(ProgressDialog->Tasks[I]->GetLogVisible())
				ProgressDialog->Tasks[I]->SetLogVisible(false);
			else
				ProgressDialog->Tasks[I]->SetLogVisible(true);
		}
	}			
}

void ZEProgressDialogSignalHandler::CopyAllLog()
{
	QApplication::clipboard()->setText(ProgressDialog->Form->txtLog->toPlainText());
}

void ZEProgressDialogSignalHandler::SaveLog()
{
	QString FileName = QFileDialog::getSaveFileName(ProgressDialog->Dialog, "Save", QString(), QString("*.txt"));

	if(FileName.length() == 0)
		return;

	QFile File(FileName);
	File.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream Out(&File);
	Out << ProgressDialog->Form->txtLog->toPlainText();
	File.close(); 
}
