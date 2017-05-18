//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECRReportViewer.cpp
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

#include "ZECRReportViewer.h"

#include "ZETypes.h"
#include "ZEML/ZEMLReader.h"
#include "ZEFile/ZEPathManager.h"
#include "ZECRWindow.h"
#include "Ui_ZECRReportViewer.h"

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

class ZECRWindow;

bool ZECRReportViewer::LoadReport(const ZEString& FileName)
{
	if (!Reader.Open(FileName))
		return false;

	ZEMLReaderNode ReportNode = Reader.GetRootNode();
	if (ReportNode.GetName() != "ZECRReport")
		return false;

	ZEMLReaderNode CollectorsNode = ReportNode.GetNode("Collectors");
	ZESize CollectorCount = CollectorsNode.GetNodeCount("Collector");
	ReportEntries.SetCount(CollectorCount);
	for (ZESize I = 0; I < CollectorCount; I++)
	{
		ZEMLReaderNode CollectorNode = CollectorsNode.GetNode("Collector", I);
		ZECRReportEntry* Entry = &ReportEntries[I];
		Entry->Name = CollectorNode.ReadString("Name");
		Entry->Extention = CollectorNode.ReadString("Extension");
		Entry->Binary = CollectorNode.ReadBoolean("Binary");
		Entry->Node = CollectorNode;

		QListWidgetItem* Item = new QListWidgetItem();
		Item->setText(Entry->Name.ToCString());
		Item->setData(Qt::UserRole, (ZEUInt32)I);		
		Form->lstCollectors->addItem(Item);
	}

	return true;
}

void ZECRReportViewer::btnSaveAs_clicked()
{
	QList<QListWidgetItem*> Itemlist = Form->lstCollectors->selectedItems();

	if (Itemlist.count() == 0)
		return;

	ZESize ItemIndex = Form->lstCollectors->selectedItems()[0]->data(Qt::UserRole).toUInt();
	const ZECRReportEntry& Entry = ReportEntries[ItemIndex];

	QString Filter = QString("%1 (*%2);;All files (*.*)").arg(Entry.Name.ToCString()).arg(Entry.Extention.ToCString());
	QString FileName = QFileDialog::getSaveFileName(this, "Save As", QString(), Filter);
	
	ZEArray<ZEBYTE> SelectedData;
	SelectedData.SetCount(Entry.Node.ReadDataSize("Data"));
	Entry.Node.ReadData("Data", SelectedData.GetCArray(), SelectedData.GetCount(), 0);

	bool AccessControl = ZEPathManager::GetInstance()->GetAccessControl();
	ZEPathManager::GetInstance()->SetAccessControl(false);
	ZEFile FileToSave;
	if (!FileToSave.Open(FileName.toStdString(), ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
	{
		QMessageBox::critical(this, "ZECRCrashReporter", "Cannot open file for writing", QMessageBox::Ok);
		return;
	}

	if (FileToSave.Write(SelectedData.GetCArray(), SelectedData.GetCount(), 1) != 1)
	{
		QMessageBox::critical(this, "ZECRCrashReporter", "Cannot write to file", QMessageBox::Ok);
		FileToSave.Close();
		return;
	}

	FileToSave.Close();
	ZEPathManager::GetInstance()->SetAccessControl(AccessControl);
}

void ZECRReportViewer::lstCollectors_itemSelectionChanged()
{
	if (Form->lstCollectors->selectedItems().count() == 0)
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

		ZESize ItemIndex = Form->lstCollectors->selectedItems()[0]->data(Qt::UserRole).toUInt();
		const ZECRReportEntry& Entry = ReportEntries[ItemIndex];

		ZESize SelectedDataSize = Entry.Node.ReadDataSize("Data");
		if (SelectedDataSize > 2 * ZE_SIZE_MB)
		{
			Form->txtContent->setEnabled(false);
			Form->txtContent->setText("File is too big to be displayed.\nPlease use save as command to save it to your computer and open it with another program to view it.");
			return;
		}

		ZEArray<ZEBYTE> SelectedData;
		SelectedData.SetCount(SelectedDataSize);
		Entry.Node.ReadData("Data", SelectedData.GetCArray(), SelectedData.GetCount(), 0);

		if (Entry.Binary)
		{
			Form->txtContent->setEnabled(false);
			Form->txtContent->setText("Does not support to display binary files.");
			return;
		}

		Form->txtContent->setEnabled(true);

		if (Form->radHex->isChecked())
		{
			Form->txtContent->setText(QByteArray((char*)SelectedData.GetCArray()).toHex().toUpper());
		}
		else	
		{
			if (SelectedData.GetCount() >= 2 && SelectedData[0] == 0xFF && SelectedData[1] == 0xFE) // UTF-16 BOM
				Form->txtContent->setText(QString::fromWCharArray((wchar_t*)(SelectedData.GetCArray() + 2), SelectedData.GetCount() - 4));
			else if (SelectedData.GetCount() >= 3 && SelectedData[0] == 0xEF && SelectedData[1] == 0xBB && SelectedData[1] == 0xBF) // UTF-8 BOM
				Form->txtContent->setText(QString::fromUtf8((char*)SelectedData.GetCArray() + 3, SelectedData.GetCount() - 3));
			else
				Form->txtContent->setText(QString::fromUtf8((char*)SelectedData.GetCArray(), SelectedData.GetCount()));
		}
	}
}

ZECRReportViewer::ZECRReportViewer(QWidget* Parent) : QDialog(Parent)
{
	Form = new Ui_ZECRReportViewer();
	Form->setupUi(this);

	Form->radASCII->setChecked(true);

	lstCollectors_itemSelectionChanged();

	QObject::connect(Form->btnSaveAs, SIGNAL(clicked()), this, SLOT(btnSaveAs_clicked()));
	QObject::connect(Form->radHex, SIGNAL(clicked()), this, SLOT(lstCollectors_itemSelectionChanged()));
	QObject::connect(Form->radASCII, SIGNAL(clicked()), this, SLOT(lstCollectors_itemSelectionChanged()));
	QObject::connect(Form->lstCollectors, SIGNAL(itemSelectionChanged()), this, SLOT(lstCollectors_itemSelectionChanged()));
	QObject::connect(Form->btnClose, SIGNAL(clicked()), this, SLOT(close()));
}

ZECRReportViewer::~ZECRReportViewer()
{
	delete Form;
}
