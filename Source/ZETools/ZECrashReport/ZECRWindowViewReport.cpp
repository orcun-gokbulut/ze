//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRWindowViewReport.cpp
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

#include "ZECRWindowViewReport.h"

#include "ZECRWindow.h"
#include "Ui_ZECRWindowViewReport.h"

#include "ZETypes.h"
#include "ZEFile/ZEFile.h"
#include "ZECrashReport/ZECRProvider.h"
#include "ZECrashReport/ZECRProviderFile.h"

#include <QFileDialog>
#include <QFile>
#include "QMessageBox"
#include "ZEDS/ZEArray.h"
#include "ZECRCrashReport.h"

class ZECRWindow;

ZECRWindowViewReport::ZECRWindowViewReport(ZECRCrashReport* Report, QWidget* Parent) : QDialog(Parent)
{
	Form = new Ui_ZECRWindowViewReport();
	Form->setupUi(this);
	this->Report = Report;

	PopulateProviders();

	Form->radASCII->setChecked(true);

	trwProviders_treeItemSelected();

	QObject::connect(Form->btnSaveAs, SIGNAL(clicked()), this, SLOT(btnSaveAs_clicked()));
	QObject::connect(Form->radHex, SIGNAL(clicked()), this, SLOT(trwProviders_treeItemSelected()));
	QObject::connect(Form->radASCII, SIGNAL(clicked()), this, SLOT(trwProviders_treeItemSelected()));
	QObject::connect(Form->trwProviders, SIGNAL(itemSelectionChanged()), this, SLOT(trwProviders_treeItemSelected()));
}

ZECRWindowViewReport::~ZECRWindowViewReport()
{
	delete Form;
}

void ZECRWindowViewReport::btnSaveAs_clicked()
{
	QList<QTreeWidgetItem*> Itemlist = Form->trwProviders->selectedItems();

	if (Itemlist.count() == 0)
		return;

	ZESize Index = Form->trwProviders->selectedItems()[0]->data(0, Qt::UserRole).toUInt();
	const ZEArray<ZECRProvider*>& Providers = Report->GetProviders();
	ZECRProvider* Provider = Providers[Index];

	QString Filter = QString("%1 (*%2);All files (*.*)").arg(Provider->GetName()).arg(Provider->GetExtension());
	QString FileName = QFileDialog::getSaveFileName(this, "Save As", QString(), Filter);
	
	ZEArray<ZEBYTE> SelectedData;
	SelectedData.SetCount(Provider->GetSize());
	Provider->GetData(SelectedData.GetCArray(), 0, SelectedData.GetCount());

	ZEFile FileToSave;
	if (!FileToSave.Open(FileName.toStdString(), ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
	{
		QMessageBox::critical(this, "ZECRCrashReporter", "Cannot open file for writing", QMessageBox::Ok);
		return;
	}

	if (FileToSave.Write(SelectedData.GetCArray(), 1, SelectedData.GetCount()) != SelectedData.GetCount())
	{
		QMessageBox::critical(this, "ZECRCrashReporter", "Cannot write to file", QMessageBox::Ok);
		FileToSave.Close();
		return;
	}

	FileToSave.Close();
}

void ZECRWindowViewReport::PopulateProviders()
{
	const ZEArray<ZECRProvider*>& Providers = Report->GetProviders();

	Form->trwProviders->setHeaderLabel("Items");
	for (ZESize I = 0; I < Providers.GetCount(); I++)
	{		
		QTreeWidgetItem* Item = new QTreeWidgetItem();
		Item->setText(Form->trwProviders->topLevelItemCount(), Providers[I]->GetName());
		Item->setData(0, Qt::UserRole, (ZEUInt32)I);		
		Form->trwProviders->addTopLevelItem(Item);
	}
}

void ZECRWindowViewReport::trwProviders_treeItemSelected()
{
	if (Form->trwProviders->selectedItems().count() == 0)
	{
		Form->txtContent->setEnabled(false);
		Form->txtContent->setText("");
		Form->btnSaveAs->setEnabled(false);
		Form->radHex->setEnabled(false);
		Form->radASCII->setEnabled(false);
	}
	else
	{
		Form->txtContent->setEnabled(true);
		Form->txtContent->setText("");
		Form->btnSaveAs->setEnabled(true);
		Form->radHex->setEnabled(true);
		Form->radASCII->setEnabled(true);

		ZESize ItemIndex = Form->trwProviders->selectedItems()[0]->data(0, Qt::UserRole).toUInt();
		const ZEArray<ZECRProvider*>& Providers = Report->GetProviders();

		ZEArray<ZEBYTE> SelectedData;
		SelectedData.SetCount(Providers[ItemIndex]->GetSize());
		Providers[ItemIndex]->GetData(SelectedData.GetCArray(), 0, SelectedData.GetCount());

		if (Providers[ItemIndex]->GetProviderType() == ZECR_DPT_BINARY)
		{
			Form->txtContent->setEnabled(false);
			Form->txtContent->setText("Does not support to display binary files.");
			return;
		}

		Form->txtContent->setEnabled(true);

		if (Form->radHex->isChecked())
			Form->txtContent->setText(QByteArray((char*)SelectedData.GetCArray()).toHex().toUpper());
		else	
			Form->txtContent->setText((char*)SelectedData.GetCArray());
	}
}
