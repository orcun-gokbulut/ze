//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELNIntegrityModule.cpp
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

#include "ZELNIntegrityModule.h"
#include "ui_ZELNIntegrityWidget.h"

#include "ZEError.h"
#include "ZEML/ZEMLReader.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <md5.h>

ZELN_MODULE_DECRIPTION(ZELNIntegrityModule, "Integrity");

void ZELNIntegrityWorker::run()
{
	IsCanceled = false;
	Checker->CheckStart();
	const ZEArray<ZEITIntegrityRecord>& Records = Checker->GetRecords();
	for (ZESize I = 0; I < Records.GetCount(); I++)
	{
		if (IsCanceled)
		{
			emit Done(true);
			return;
		}

		emit RecordUpdated(I);
		Checker->CheckProgress(I);
		emit RecordUpdated(I);
	}

	emit Done(false);
}

void ZELNIntegrityWorker::Cancel()
{
	IsCanceled = true;
}

ZELNIntegrityWorker::ZELNIntegrityWorker()
{
	Checker = NULL;
	IsCanceled = false;
}

void ZELNIntegrityModule::Worker_Done(bool Canceled)
{
	const char* StatusText;
	switch(Checker.GetResult())
	{
		default:
		case ZEIT_IR_NOT_CHECKED:
			StatusText = "Unknown";
			break;

		case ZEIT_IR_WARNING:
			StatusText = "Warning";
			break;

		case ZEIT_IR_ERROR:
			StatusText = "Error";
			break;

		case ZEIT_IR_SUCCESS:
			StatusText = "OK";
			break;
	}

	if (!Canceled)
	{
		Form->prgProgress->setValue(Checker.GetRecords().GetCount());
		Form->lblPath->setText("");
		Form->lblStatus->setText(StatusText);
	}
	else
	{
		Form->prgProgress->setEnabled(false);
		Form->prgProgress->setStyleSheet(Form->prgProgress->property("defaultStyleSheet").toString() + " QProgressBar::chunk { background: gray; }");
		Form->lblStatus->setText(QString("Canceled (%1)").arg(StatusText));
	}

	Form->btnCheckIntegrity->setText("Check Integrity");
	State = false;
}

void ZELNIntegrityModule::Worker_RecordUpdated(unsigned int RecordIndex)
{
	const ZEArray<ZEITIntegrityRecord>& Records = Checker.GetRecords();
	Form->lblPath->setText(Records[RecordIndex].GetPath().ToCString());
	Form->prgProgress->setValue(RecordIndex - 1);

	UpdateRecord(RecordIndex);

	const char* StatusText;
	switch(Checker.GetResult())
	{
		default:
		case ZEIT_IR_NOT_CHECKED:
			StatusText = "Unknown";
			break;

		case ZEIT_IR_WARNING:
			StatusText = "Warning";
			break;

		case ZEIT_IR_ERROR:
			StatusText = "Error";
			break;

		case ZEIT_IR_SUCCESS:
			StatusText = "OK";
			break;
	}

	Form->lblStatus->setText(QString("Checking... (%1)").arg(StatusText));
}

void ZELNIntegrityModule::UpdateRecord(ZESize Index)
{
	const ZEITIntegrityRecord& Record = Checker.GetRecords()[Index];

	if (Form->tblRecords->item(Index, 0) == NULL)
	{
		QTableWidgetItem* StatusItem = new QTableWidgetItem();
		StatusItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		StatusItem->setFlags(StatusItem->flags() ^ Qt::ItemIsEditable);
		Form->tblRecords->setItem(Index, 0, StatusItem);

		QTableWidgetItem* PathItem = new QTableWidgetItem();
		PathItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PathItem->setFlags(PathItem->flags() ^ Qt::ItemIsEditable);
		Form->tblRecords->setItem(Index, 1, PathItem);
	}
	  
	switch (Record.GetResult())
	{
		default:
		case ZEIT_IR_NOT_CHECKED:
			Form->tblRecords->item(Index, 0)->setText("Not Checked");
			Form->tblRecords->item(Index, 0)->setBackground(Qt::lightGray);
			Form->tblRecords->item(Index, 0)->setTextColor(Qt::black);
			break;

		case ZEIT_IR_WARNING:
			Form->tblRecords->item(Index, 0)->setBackground(Qt::yellow);
			Form->tblRecords->item(Index, 0)->setTextColor(Qt::black);
			break;

		case ZEIT_IR_ERROR:
			Form->tblRecords->item(Index, 0)->setBackgroundColor(Qt::red);
			Form->tblRecords->item(Index, 0)->setTextColor(Qt::white);
			break;

		case ZEIT_IR_SUCCESS:
			Form->tblRecords->item(Index, 0)->setBackgroundColor(Qt::darkGreen);
			Form->tblRecords->item(Index, 0)->setTextColor(Qt::white);
			break;
	}

	switch (Record.GetProblem())
	{
		default:
		case ZEIT_IP_NONE:
			if (Record.GetResult() == ZEIT_IR_NOT_CHECKED)
				Form->tblRecords->item(Index, 0)->setText("Not Checked");
			else
				Form->tblRecords->item(Index, 0)->setText("OK");
			break;

		case ZEIT_IP_MISSING:
			Form->tblRecords->item(Index, 0)->setText("Missing");
			break;

		case ZEIT_IP_FILE_SIZE:
			Form->tblRecords->item(Index, 0)->setText("Size Mismatch");
			break;

		case ZEIT_IP_CHECKSUM:
			Form->tblRecords->item(Index, 0)->setText("Checksum Failed");
			break;
	}

	Form->tblRecords->item(Index, 1)->setText(Record.GetPath().ToCString());
}

void ZELNIntegrityModule::Update()
{
	Form->tblRecords->setRowCount(Checker.GetRecords().GetCount());

	for (ZESize I = 0; I < Form->tblRecords->rowCount(); I++)
		UpdateRecord(I);
}

bool ZELNIntegrityModule::InitializeSelf()
{
	if (!ZEInitializable::InitializeSelf())
		return false;
	
	if (!Checker.Load())
		zeWarning("Cannot load Integrity records file.");

	Update();

	return true;
}

ZELNIntegrityModule::ZELNIntegrityModule()
{
	State = false;
	Widget = new QWidget();
	Form = new Ui_ZELNIntegrityWidget();
	Form->setupUi(Widget);

	Form->tblRecords->setColumnCount(2);

	QTableWidgetItem* StatusHeader = new QTableWidgetItem();
	StatusHeader->setText("Status");
	StatusHeader->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	Form->tblRecords->setHorizontalHeaderItem(0, StatusHeader);

	QTableWidgetItem* PathHeader = new QTableWidgetItem();
	PathHeader->setText("Path");
	PathHeader->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	Form->tblRecords->setHorizontalHeaderItem(1, PathHeader);

	Form->tblRecords->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	Form->tblRecords->verticalHeader()->setVisible(false);

	QHeaderView* verticalHeader = Form->tblRecords->verticalHeader();
	verticalHeader->sectionResizeMode(QHeaderView::Fixed);
	verticalHeader->setDefaultSectionSize(24);

	Worker.Checker = &Checker;

	connect(Form->btnCheckIntegrity, SIGNAL(clicked()), this, SLOT(btnCheckIntegrity_clicked()));
	connect(Form->chkFilterAll, SIGNAL(toggled(bool)), this, SLOT(chkFilter_toggled(bool)));
	connect(Form->chkFilterFailed, SIGNAL(toggled(bool)), this, SLOT(chkFilter_toggled(bool)));
	connect(Form->btnSave, SIGNAL(clicked()), this, SLOT(btnSave_clicked()));
	connect(&Worker, SIGNAL(RecordUpdated(unsigned int)), this, SLOT(Worker_RecordUpdated(unsigned int)));
	connect(&Worker, SIGNAL(Done(bool)), this, SLOT(Worker_Done(bool)));
}

void ZELNIntegrityModule::btnCheckIntegrity_clicked()
{
	if (!State)
	{
		State = true;

		Form->prgProgress->setEnabled(true);
		Form->prgProgress->setValue(0);
		Form->prgProgress->setMaximum(Checker.GetRecords().GetCount());
		Form->prgProgress->setStyleSheet("");
		Form->lblStatus->setText("Checking...");
		Form->btnCheckIntegrity->setText("Cancel");

		Worker.start();
	}
	else
	{
		Worker.Cancel();
	}
}

void ZELNIntegrityModule::chkFilter_toggled(bool)
{
	const ZEArray<ZEITIntegrityRecord>& Records = Checker.GetRecords();
	if (Form->chkFilterAll->isChecked())
	{
		for (ZESize I = 0; I < Records.GetCount(); I++)
			Form->tblRecords->setRowHidden(I, false);
	}
	else
	{
		for (ZESize I = 0; I < Records.GetCount(); I++)
		{
			bool Hide = (Records[I].GetResult() == ZEIT_IR_SUCCESS || Records[I].GetResult() == ZEIT_IR_NOT_CHECKED);
			Form->tblRecords->setRowHidden(I, Hide);
		}
	}
}

void ZELNIntegrityModule::btnSave_clicked()
{
	QString FileName = QFileDialog::getSaveFileName(GetWidget(), "Save Integrity Report", QString(), "Text files (*.txt);;All files (*.*)");
	if (FileName.isEmpty())
		return;

	FILE* File = fopen(FileName.toLocal8Bit(), "w");
	if (File == NULL)
	{
		QMessageBox::critical(GetWidget(), "Save Integrity Report", "Cannot open file.", QDialogButtonBox::Ok, QDialogButtonBox::NoButton);
		return;
	}

	const ZEArray<ZEITIntegrityRecord>& Records = Checker.GetRecords();
	for(ZESize I = 0; I < Records.GetCount(); I++)
	{
		const char* ResultText;
		switch(Records[I].GetResult())
		{
			default:
			case ZEIT_IR_NOT_CHECKED:
				ResultText = "Unknown";
				break;

			case ZEIT_IR_WARNING:
				ResultText = "Warning";
				break;

			case ZEIT_IR_ERROR:
				ResultText = "Error";
				break;

			case ZEIT_IR_SUCCESS:
				ResultText = "Success";
				break;
		}

		const char* ProblemText;
		switch (Records[I].GetProblem())
		{
			default:
			case ZEIT_IP_NONE:
				ProblemText = "No Problem";
				break;

			case ZEIT_IP_MISSING:
				ProblemText = "Missing";
				break;

			case ZEIT_IP_FILE_SIZE:
				ProblemText = "Size Mismatch";
				break;

			case ZEIT_IP_CHECKSUM:
				ProblemText = "Checksum Failed";
				break;
		}

		fprintf(File, "%s, %s, \"%s\", %s, %llu, %s\n",
			ResultText,
			ProblemText,
			Records[I].GetPath().ToCString(),
			Records[I].GetRequired() ? "Required" : "NotRequired",
			Records[I].GetFileSize(),
			Records[I].GetChecksum().ToCString());
	}

	fclose(File);
}

QWidget* ZELNIntegrityModule::GetWidget()
{
	return Widget;
}

void ZELNIntegrityModule::LoadConfiguration(const ZEMLReaderNode& ConfigurationNode)
{
	Checker.SetIntegrityFile(ConfigurationNode.ReadString("IntegrityFile", "#E:/Zinek.ZEIntegrity"));
}
