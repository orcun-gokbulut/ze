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

#include "ZEError.h"
#include "ZEML/ZEMLReader.h"
#include "ZELNIntegrityWorker.h"
#include "ui_ZELNIntegrityWidget.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDialogButtonBox>

ZELN_MODULE_DECRIPTION(ZELNIntegrityModule, "Integrity");

void ZELNIntegrityModule::CheckerWorker_StateChanged()
{
	if (CheckerWorker->GetState() != ZELN_IWS_DONE && CheckerWorker->GetState() != ZELN_IWS_CANCELED)
		return;

	const char* StatusText;
	switch(Checker.GetResult())
	{
		default:
		case ZEIT_CR_NOT_CHECKED:
			StatusText = "Unknown";
			break;

		case ZEIT_CR_WARNING:
			StatusText = "<font color=\"#D24726\">Warning</font>";
			break;

		case ZEIT_CR_ERROR:
			StatusText = "<font color=\"Red\">Failed !</font>";
			break;

		case ZEIT_CR_SUCCESS:
			StatusText = "<font color=\"#008A00\">Success</font>";
			break;
	}

	Form->btnCheckIntegrity->setText("Check Integrity");

	if (CheckerWorker->GetState() != ZELN_IWS_CANCELED)
	{
		Form->prgProgress->setValue(Checker.GetRecords().GetCount());
		Form->lblPath->setText("");
		Form->lblStatus->setText(StatusText);

		if (Checker.GetResult() == ZEIT_CR_ERROR)
			Form->prgProgress->setStyleSheet(Form->prgProgress->property("defaultStyleSheet").toString() + " QProgressBar::chunk { background: red; }");
		else
			Form->prgProgress->setStyleSheet(Form->prgProgress->property("defaultStyleSheet").toString() + " QProgressBar::chunk { background: gray; }");
	}
	else
	{
		Form->prgProgress->setEnabled(false);
		Form->lblStatus->setText(QString("Canceled (%1)").arg(StatusText));
		Form->prgProgress->setStyleSheet(Form->prgProgress->property("defaultStyleSheet").toString() + " QProgressBar::chunk { background: gray; }");
	}

}

void ZELNIntegrityModule::CheckerWorker_RecordUpdated(unsigned int RecordIndex)
{
	const ZEArray<ZEITRecord>& Records = Checker.GetRecords();
	Form->lblPath->setText(Records[RecordIndex].GetPath().ToCString());
	Form->prgProgress->setValue(RecordIndex - 1);

	UpdateRecord(RecordIndex);

	const char* StatusText;
	switch(Checker.GetResult())
	{
		default:
		case ZEIT_CR_NOT_CHECKED:
			StatusText = "Unknown";
			break;

		case ZEIT_CR_WARNING:
			StatusText = "<font color=\"#D24726\">Warning</font>";
			break;

		case ZEIT_CR_ERROR:
			StatusText = "<font color=\"Red\">Error</font>";
			break;

		case ZEIT_CR_SUCCESS:
			StatusText = "<font color=\"#008A00\">OK</font>";
			break;
	}

	Form->lblStatus->setText(QString("Checking... (%1)").arg(StatusText));

	if (Checker.GetResult() == ZEIT_CR_ERROR)
		Form->prgProgress->setStyleSheet(Form->prgProgress->property("defaultStyleSheet").toString() + " QProgressBar::chunk { background: red; }");
}

void ZELNIntegrityModule::UpdateRecord(ZESize Index)
{
	const ZEITRecord& Record = Checker.GetRecords()[Index];

	if (Form->tblRecords->item(Index, 0) == NULL)
	{
		QTableWidgetItem* StatusItem = new QTableWidgetItem();
		StatusItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		StatusItem->setFlags(StatusItem->flags() & ~Qt::ItemIsEditable);
		Form->tblRecords->setItem(Index, 0, StatusItem);

		QTableWidgetItem* PathItem = new QTableWidgetItem();
		PathItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PathItem->setFlags(PathItem->flags() & ~Qt::ItemIsEditable);
		Form->tblRecords->setItem(Index, 1, PathItem);
	}
	  
	switch (Record.GetResult())
	{
		default:
		case ZEIT_CR_NOT_CHECKED:
			Form->tblRecords->item(Index, 0)->setText("Not Checked");
			Form->tblRecords->item(Index, 0)->setBackground(Qt::lightGray);
			Form->tblRecords->item(Index, 0)->setTextColor(Qt::black);
			break;

		case ZEIT_CR_WARNING:
			Form->tblRecords->item(Index, 0)->setBackground(Qt::yellow);
			Form->tblRecords->item(Index, 0)->setTextColor(Qt::black);
			break;

		case ZEIT_CR_ERROR:
			Form->tblRecords->item(Index, 0)->setBackgroundColor(Qt::red);
			Form->tblRecords->item(Index, 0)->setTextColor(Qt::white);
			break;

		case ZEIT_CR_SUCCESS:
			Form->tblRecords->item(Index, 0)->setBackgroundColor(0x008A00);
			Form->tblRecords->item(Index, 0)->setTextColor(Qt::white);
			break;
	}

	switch (Record.GetProblem())
	{
		default:
		case ZEIT_CP_NONE:
			if (Record.GetResult() == ZEIT_CR_NOT_CHECKED)
				Form->tblRecords->item(Index, 0)->setText("Not Checked");
			else
				Form->tblRecords->item(Index, 0)->setText("OK");
			break;

		case ZEIT_CP_MISSING:
			Form->tblRecords->item(Index, 0)->setText("Missing");
			break;

		case ZEIT_CP_FILE_SIZE:
			Form->tblRecords->item(Index, 0)->setText("Size Mismatch");
			break;

		case ZEIT_CP_CHECKSUM:
			Form->tblRecords->item(Index, 0)->setText("Checksum Failed");
			break;
	}

	Form->tblRecords->item(Index, 1)->setText(Record.GetPath().ToCString());
	Form->tblRecords->item(Index, 1)->setToolTip(Record.GetPath().ToCString());
}

void ZELNIntegrityModule::Update()
{
	Form->tblRecords->setRowCount(Checker.GetRecords().GetCount());

	for (ZESize I = 0; I < Form->tblRecords->rowCount(); I++)
		UpdateRecord(I);
}

bool ZELNIntegrityModule::InitializeInternal()
{
	if (!ZELNModule::InitializeInternal())
		return false;
	
	if (!Checker.Load())
	{
		Form->lblStatus->setText("ERROR: Cannot load Integrity file.");
		zeWarning("Cannot load Integrity file.");
		Widget->setEnabled(false);
	}

	Update();

	return true;
}

ZELNIntegrityModule::ZELNIntegrityModule()
{
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

	Form->tblRecords->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	Form->tblRecords->verticalHeader()->setVisible(false);

	QHeaderView* verticalHeader = Form->tblRecords->verticalHeader();
	verticalHeader->sectionResizeMode(QHeaderView::Fixed);
	verticalHeader->setDefaultSectionSize(24);

	CheckerWorker = new ZELNIntegrityWorker();
	CheckerWorker->SetChecker(&Checker);

	Form->tblRecords->setSelectionBehavior(QAbstractItemView::SelectRows);
	Form->tblRecords->setSelectionMode(QAbstractItemView::NoSelection);

	Form->tblRecords->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	Form->tblRecords->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

	connect(Form->btnCheckIntegrity, SIGNAL(clicked()), this, SLOT(btnCheckIntegrity_clicked()));
	connect(Form->chkFilterAll,		SIGNAL(toggled(bool)), this, SLOT(chkFilter_toggled(bool)));
	connect(Form->chkFilterFailed,	SIGNAL(toggled(bool)), this, SLOT(chkFilter_toggled(bool)));
	connect(Form->btnSave,			SIGNAL(clicked()), this, SLOT(btnSave_clicked()));
	connect(CheckerWorker,			SIGNAL(RecordUpdated(unsigned int)), this, SLOT(CheckerWorker_RecordUpdated(unsigned int)));
	connect(CheckerWorker,			SIGNAL(StateChanged()), this, SLOT(CheckerWorker_StateChanged()));
}

void ZELNIntegrityModule::btnCheckIntegrity_clicked()
{
	if (CheckerWorker->GetState() != ZELN_IWS_RUNNING)
	{
		Checker.CheckStart();
		for (ZESize I = 0; I < Checker.GetRecords().GetCount(); I++)
			UpdateRecord(I);

		chkFilter_toggled(true);

		Form->prgProgress->setEnabled(true);
		Form->prgProgress->setValue(0);
		Form->prgProgress->setMaximum(Checker.GetRecords().GetCount());
		Form->prgProgress->setStyleSheet("");
		Form->lblStatus->setText("Checking...");
		Form->btnCheckIntegrity->setText("Cancel");

		CheckerWorker->start();
	}
	else
	{
		CheckerWorker->Cancel();
	}
}

void ZELNIntegrityModule::chkFilter_toggled(bool)
{
	const ZEArray<ZEITRecord>& Records = Checker.GetRecords();
	if (Form->chkFilterAll->isChecked())
	{
		for (ZESize I = 0; I < Records.GetCount(); I++)
			Form->tblRecords->setRowHidden(I, false);
	}
	else
	{
		for (ZESize I = 0; I < Records.GetCount(); I++)
		{
			bool Hide = (Records[I].GetResult() == ZEIT_CR_SUCCESS || Records[I].GetResult() == ZEIT_CR_NOT_CHECKED);
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

	const ZEArray<ZEITRecord>& Records = Checker.GetRecords();
	for(ZESize I = 0; I < Records.GetCount(); I++)
	{
		const char* ResultText;
		switch(Records[I].GetResult())
		{
			default:
			case ZEIT_CR_NOT_CHECKED:
				ResultText = "Unknown";
				break;

			case ZEIT_CR_WARNING:
				ResultText = "Warning";
				break;

			case ZEIT_CR_ERROR:
				ResultText = "Error";
				break;

			case ZEIT_CR_SUCCESS:
				ResultText = "Success";
				break;
		}

		const char* ProblemText;
		switch (Records[I].GetProblem())
		{
			default:
			case ZEIT_CP_NONE:
				ProblemText = "No Problem";
				break;

			case ZEIT_CP_MISSING:
				ProblemText = "Missing";
				break;

			case ZEIT_CP_FILE_SIZE:
				ProblemText = "Size Mismatch";
				break;

			case ZEIT_CP_CHECKSUM:
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
