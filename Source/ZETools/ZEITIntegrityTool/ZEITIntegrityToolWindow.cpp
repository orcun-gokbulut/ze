//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEITIntegrityToolWindow.cpp
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

#include "ZEITIntegrityToolWindow.h"

#include "ZEFile/ZEPathManager.h"
#include "ZEITIntegrity/ZEITGenerator.h"
#include "ZEITGeneratorWorker.h"
#include "Ui_ZEITIntegrityToolWindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTreeWidget>
#include <QSettings>

void ZEITIntegrityToolWindow::UpdateRecord(ZESize Index)
{
	QSignalBlocker tblRecordsBlocker(Form->tblRecords);

	if (Form->tblRecords->item(Index, 0) == NULL)
	{
		Form->tblRecords->setItem(Index, 0, new QTableWidgetItem());
		Form->tblRecords->setItem(Index, 1, new QTableWidgetItem());
		Form->tblRecords->setItem(Index, 2, new QTableWidgetItem());
		Form->tblRecords->setItem(Index, 3, new QTableWidgetItem());
		Form->tblRecords->setItem(Index, 4, new QTableWidgetItem());
		Form->tblRecords->setItem(Index, 5, new QTableWidgetItem());

		Qt::ItemFlags Flags = Form->tblRecords->item(Index, 0)->flags();
		Form->tblRecords->item(Index, 0)->setFlags((Flags | Qt::ItemIsUserCheckable) & ~Qt::ItemIsEditable);
		Form->tblRecords->item(Index, 0)->setText("");
		Form->tblRecords->item(Index, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		Form->tblRecords->item(Index, 1)->setFlags(Flags & ~Qt::ItemIsEditable);
		Form->tblRecords->item(Index, 1)->setText("Status");
		Form->tblRecords->item(Index, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		Form->tblRecords->item(Index, 2)->setFlags(Flags | Qt::ItemIsEditable);
		Form->tblRecords->item(Index, 2)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		Form->tblRecords->item(Index, 3)->setFlags(Flags & ~Qt::ItemIsEditable);
		Form->tblRecords->item(Index, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		Form->tblRecords->item(Index, 4)->setFlags(Flags & ~Qt::ItemIsEditable);
		Form->tblRecords->item(Index, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		Form->tblRecords->item(Index, 5)->setFlags((Flags | Qt::ItemIsUserCheckable) & ~Qt::ItemIsEditable);
		Form->tblRecords->item(Index, 5)->setText("");
		Form->tblRecords->item(Index, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	}

	Form->tblRecords->item(Index, 0)->setCheckState(Generator.GetRecord(Index).GetEnabled() ? Qt::Checked : Qt::Unchecked);
	Form->tblRecords->item(Index, 2)->setText(Generator.GetRecord(Index).GetPath().ToCString());
	Form->tblRecords->item(Index, 3)->setText(QString::number(Generator.GetRecord(Index).GetFileSize()));
	Form->tblRecords->item(Index, 4)->setText(Generator.GetRecord(Index).GetChecksum().ToCString());
	Form->tblRecords->item(Index, 5)->setCheckState(Generator.GetRecord(Index).GetRequired() ? Qt::Checked : Qt::Unchecked);

	switch (Generator.GetRecord(Index).GetResult())
	{
		default:
		case ZEIT_CR_NOT_CHECKED:
			Form->tblRecords->item(Index, 1)->setText("Not Checked");
			Form->tblRecords->item(Index, 1)->setBackground(Qt::lightGray);
			Form->tblRecords->item(Index, 1)->setTextColor(Qt::black);
			break;

		case ZEIT_CR_WARNING:
			Form->tblRecords->item(Index, 1)->setBackground(Qt::yellow);
			Form->tblRecords->item(Index, 1)->setTextColor(Qt::black);
			break;

		case ZEIT_CR_ERROR:
			Form->tblRecords->item(Index, 1)->setBackgroundColor(Qt::red);
			Form->tblRecords->item(Index, 1)->setTextColor(Qt::white);
			break;

		case ZEIT_CR_SUCCESS:
			Form->tblRecords->item(Index, 1)->setBackgroundColor(Qt::darkGreen);
			Form->tblRecords->item(Index, 1)->setTextColor(Qt::white);
			break;
	}

	switch (Generator.GetRecord(Index).GetProblem())
	{
		default:
		case ZEIT_CP_NONE:
			if (Generator.GetRecord(Index).GetResult() == ZEIT_CR_NOT_CHECKED)
				Form->tblRecords->item(Index, 1)->setText("Not Checked");
			else
				Form->tblRecords->item(Index, 1)->setText("OK");
			break;

		case ZEIT_CP_MISSING:
			Form->tblRecords->item(Index, 1)->setText("Missing");
			break;

		case ZEIT_CP_FILE_SIZE:
			Form->tblRecords->item(Index, 1)->setText("Size Mismatch");
			break;

		case ZEIT_CP_CHECKSUM:
			Form->tblRecords->item(Index, 1)->setText("Checksum Failed");
			break;
	}
}

void ZEITIntegrityToolWindow::UpdateLists()
{
	Form->lstIncludes->clear();
	const ZEArray<ZEITScannerEntry>& Includes = Generator.GetIncludes();
	for (ZESize I = 0; I < Includes.GetCount(); I++)
	{
		QListWidgetItem* Item = new QListWidgetItem();
		Item->setFlags(Item->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
		Item->setText(Includes[I].GetPath().ToCString());
		Item->setCheckState(Includes[I].GetEnabled() ? Qt::Checked : Qt::Unchecked);
		Form->lstIncludes->addItem(Item);
	}

	Form->lstExludes->clear();
	const ZEArray<ZEITScannerEntry>& Excludes = Generator.GetExcludes();
	for (ZESize I = 0; I < Excludes.GetCount(); I++)
	{
		QListWidgetItem* Item = new QListWidgetItem();
		Item->setFlags(Item->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
		Item->setText(Excludes[I].GetPath().ToCString());
		Item->setCheckState(Excludes[I].GetEnabled() ? Qt::Checked : Qt::Unchecked);
		Form->lstExludes->addItem(Item);
	}

	QSignalBlocker tblRecordsBlocker(Form->tblRecords);
	const ZEArray<ZEITRecord>& Records = Generator.GetRecords();
	Form->tblRecords->setRowCount(Records.GetCount());
	for (ZESize I = 0; I < Records.GetCount(); I++)
		UpdateRecord(I);
}

void ZEITIntegrityToolWindow::UpdateUI()
{
	bool GeneratorWorkerRunning = GeneratorWorker->GetState() == ZEIT_GWS_RUNNING;
	Form->mnuRecentFiles->setEnabled(!GeneratorWorkerRunning);
	Form->btnIncludeAdd->setEnabled(!GeneratorWorkerRunning);
	Form->btnIncludeRemove->setEnabled(!GeneratorWorkerRunning && Form->lstIncludes->selectedItems().count() != 0);
	Form->btnExcludeAdd->setEnabled(!GeneratorWorkerRunning);
	Form->btnExcludeRemove->setEnabled(!GeneratorWorkerRunning && Form->lstExludes->selectedItems().count() != 0);
	Form->txtEnginePath->setEnabled(!GeneratorWorkerRunning);
	Form->btnBrowseEnginePath->setEnabled(!GeneratorWorkerRunning);
	Form->txtIntegrityFile->setEnabled(!GeneratorWorkerRunning);
	Form->btnBrowseIntegrityFile->setEnabled(!GeneratorWorkerRunning);
	Form->btnScan->setEnabled(!GeneratorWorkerRunning);
	Form->btnRecordAdd->setEnabled(!GeneratorWorkerRunning);
	Form->btnRecordRemove->setEnabled(!GeneratorWorkerRunning && Form->tblRecords->selectedItems().count() != 0);
	Form->btnRecordExclude->setEnabled(!GeneratorWorkerRunning && Form->tblRecords->selectedItems().count() != 0);
	Form->btnRecordUpdate->setEnabled(!GeneratorWorkerRunning && Form->tblRecords->selectedItems().count() != 0);
	Form->btnRecordClear->setEnabled(!GeneratorWorkerRunning && Generator.GetRecords().GetCount());
	Form->actNew->setEnabled(!GeneratorWorkerRunning);
	Form->actOpen->setEnabled(!GeneratorWorkerRunning);
	Form->actSave->setEnabled(!GeneratorWorkerRunning);
	Form->actSaveAs->setEnabled(!GeneratorWorkerRunning);
	Form->actQuit->setEnabled(!GeneratorWorkerRunning);
	Form->actIntegritySave->setEnabled(!GeneratorWorkerRunning);
	Form->actIntegritySaveAs->setEnabled(!GeneratorWorkerRunning);
	
	if (GeneratorWorkerRunning)
		Form->btnGenerate->setText("Cancel");
	else
		Form->btnGenerate->setText("Generate");
}

void ZEITIntegrityToolWindow::RegisterRecentFile(const QString& FileName)
{
	QSettings Settings("Zinek", "ZEITIntegrityTool");
	QStringList RecentFiles = Settings.value("RecentFiles", QStringList()).toStringList();

	int Index = RecentFiles.indexOf(FileName);
	if (Index != -1)
		RecentFiles.removeAt(Index);

	if (RecentFiles.size() > 10)
		RecentFiles.removeLast();

	RecentFiles.insert(0, FileName);
	Settings.setValue("RecentFiles", RecentFiles);

	LoadRecentFiles();
}

bool ZEITIntegrityToolWindow::Open(const QString& FileName)
{
	Form->stbStatusBar->showMessage(QString("Opening Integrity Generator file \"%1\".").arg(FileName));
	ZEPathManager::GetInstance()->SetAccessControl(false);
	if (!Generator.LoadGeneratorFile(FileName.toUtf8().begin()))
	{
		QMessageBox::critical(this, "Open Integrity Generator File", "Cannot load file.", QMessageBox::Ok, QMessageBox::NoButton);
		Form->stbStatusBar->showMessage(QString("Opening Integrity Generator file \"%1\" failed.").arg(Generator.GetIntegrityFileName().ToCString()), 5000);
		return false;
	}
	ZEPathManager::GetInstance()->SetAccessControl(true);
	Form->stbStatusBar->showMessage(QString("Integrity Generator file \"%1\" opened.").arg(FileName), 5000);

	Form->txtEnginePath->setText(Generator.GetEnginePath().ToCString());
	Form->txtIntegrityFile->setText(Generator.GetIntegrityFileName().ToCString());
	GeneratorFileName = FileName;

	RegisterRecentFile(FileName);

	UpdateLists();
	UpdateUI();

	return true;
}

bool ZEITIntegrityToolWindow::Save(const QString& FileName)
{
	Form->stbStatusBar->showMessage(QString("Saving Integrity Generator file \"%1\".").arg(FileName));
	ZEPathManager::GetInstance()->SetAccessControl(false);
	if (!Generator.SaveGeneratorFile(FileName.toUtf8().begin()))
	{
		QMessageBox::critical(this, "Save Integrity Generator File", "Cannot save file.", QMessageBox::Ok, QMessageBox::NoButton);
		Form->stbStatusBar->showMessage(QString("Saving Integrity Generator file \"%1\" failed.").arg(FileName), 5000);
		return false;
	}
	ZEPathManager::GetInstance()->SetAccessControl(true);
	Form->stbStatusBar->showMessage(QString("Integrity Generator file \"%1\" saved.").arg(FileName), 5000);

	GeneratorFileName = FileName;
	RegisterRecentFile(FileName);

	return true;
}

void ZEITIntegrityToolWindow::LoadRecentFiles()
{
	QSettings Settings("Zinek", "ZEITIntegrityTool");
	QStringList RecentFiles = Settings.value("RecentFiles", QStringList()).toStringList();

	if (RecentFiles.isEmpty())
	{
		Form->mnuRecentFiles->setVisible(false);
		return;
	}
	else
	{
		Form->mnuRecentFiles->setVisible(true);
	}

	Form->mnuRecentFiles->clear();
	for (int I = 0; I < RecentFiles.size(); I++)
	{
		QAction* RecentFileAction = new QAction(this);
		RecentFileAction->setText(RecentFiles[I]);
		connect(RecentFileAction, SIGNAL(triggered()), this, SLOT(mnuRecentFiles_triggered()));
		Form->mnuRecentFiles->addAction(RecentFileAction);	
	}
}

void ZEITIntegrityToolWindow::btnScan_clicked()
{
	Form->stbStatusBar->showMessage("Scanning...");
	Generator.Scan();
	UpdateLists();
	UpdateUI();
	Form->stbStatusBar->showMessage("Scanning done.", 5000);
}

void ZEITIntegrityToolWindow::btnGenerate_clicked()
{
	if (GeneratorWorker->GetState() != ZEIT_GWS_RUNNING)
	{
		Form->prgGenerate->setEnabled(true);
		Form->prgGenerate->setValue(0);
		Form->prgGenerate->setMaximum(Generator.GetRecords().GetCount());
		Form->stbStatusBar->showMessage("Generating...");
		GeneratorWorker->start();
	}
	else
	{
		Form->stbStatusBar->showMessage("Generate canceled.");
		GeneratorWorker->Cancel();
	}
}

void ZEITIntegrityToolWindow::GeneratorWorker_RecordUpdated(unsigned int RecordIndex)
{
	Form->lblGenerateFile->setText(Generator.GetRecord(RecordIndex).GetPath().ToCString());
	UpdateRecord(RecordIndex);
	Form->prgGenerate->setValue(RecordIndex);
}

void ZEITIntegrityToolWindow::GeneratorWorker_StateChanged()
{
	if (GeneratorWorker->GetState() == ZEIT_GWS_DONE)
	{
		Form->prgGenerate->setValue(Form->prgGenerate->maximum());
		Form->lblGenerateFile->setText("");
		Form->stbStatusBar->showMessage("Generate done.", 5000);

		QString IntegrityFileName = Form->txtIntegrityFile->text();
		Form->stbStatusBar->showMessage(QString("Saving Integrity file \"%1\".").arg(Generator.GetIntegrityFileName().ToCString()));
		ZEPathManager::GetInstance()->SetAccessControl(false);
		if (!Generator.GenerateIntegrityFile())
		{
			QMessageBox::critical(this, "Save Integrity File", "Cannot save file.", QMessageBox::Ok, QMessageBox::NoButton);
			Form->stbStatusBar->showMessage(QString("Saving Integrity file \"%1\" failed.").arg(Generator.GetIntegrityFileName().ToCString()), 5000);
			return;
		}
		Form->txtIntegrityFile->setText(Generator.GetIntegrityFileName().ToCString());
		Form->stbStatusBar->showMessage(QString("Integrity file \"%1\" saved.").arg(Generator.GetIntegrityFileName().ToCString()), 5000);
	}
	else if (GeneratorWorker->GetState() == ZEIT_GWS_ERROR)
	{
		static bool Fence = false;
		if (Fence)
			return;
		Fence = true;
		QMessageBox::critical(this, "Save Integrity File", "There are problems in Integrity file. Please correct them.", QMessageBox::Ok, QMessageBox::NoButton);
		Form->stbStatusBar->showMessage("Generation failed.", 5000);
		Fence = false;
	}

	UpdateUI();
}

void ZEITIntegrityToolWindow::actNew_triggered()
{
	Generator = ZEITGenerator();
	GeneratorFileName = "";
	Form->txtIntegrityFile->clear();
	Form->txtEnginePath->clear();
	Form->lstIncludes->clear();
	Form->lstExludes->clear();
	Form->tblRecords->clearContents();
	Form->tblRecords->setRowCount(0);
}

void ZEITIntegrityToolWindow::actOpen_triggered()
{
	QString FileName = QFileDialog::getOpenFileName(this, "Open Integrity Generator File", QString(), "Integrity Generator files (*.ZEITGenerator);;All files (*.*)");
	if (FileName.isEmpty())
		return;

	Open(FileName);
}

void ZEITIntegrityToolWindow::mnuRecentFiles_triggered()
{
	QString FileName = ((QAction*)sender())->text().toUtf8().constData();
	Open(FileName);
}

void ZEITIntegrityToolWindow::actSave_triggered()
{
	if (GeneratorFileName.isEmpty())
	{
		actSaveAs_triggered();
		return;
	}

	Save(GeneratorFileName);
}

void ZEITIntegrityToolWindow::actSaveAs_triggered()
{
	QString FileName = QFileDialog::getSaveFileName(this, "Save Integrity Generator File", QString(), "Integrity Generator files (*.ZEITGenerator);;All files (*.*)");
	if (FileName.isEmpty())
		return;

	Save(FileName);
}

void ZEITIntegrityToolWindow::actQuit_triggered()
{
	int Result = QMessageBox::question(this, "ZEITIntegrity Tool", "Are you sure that you want to quit ?", QMessageBox::Yes, QMessageBox::No);
	if (Result == QMessageBox::Yes)
		qApp->exit(EXIT_SUCCESS);
}


void ZEITIntegrityToolWindow::txtEnginePath_textEdited(const QString&)
{
	Generator.SetEnginePath(Form->txtEnginePath->text().toUtf8().begin());
}

void ZEITIntegrityToolWindow::btnBrowseEnginePath_clicked()
{
	QString FileName = QFileDialog::getExistingDirectory(this, "Set Engine Path", Form->txtEnginePath->text());
	if (FileName.isEmpty())
		return;

	Form->txtEnginePath->setText(FileName);
	Generator.SetEnginePath(FileName.toUtf8().begin());
}

void ZEITIntegrityToolWindow::txtIntegrityFile_textEdited(const QString&)
{
	Generator.SetIntegrityFileName(Form->txtIntegrityFile->text().toUtf8().begin());
}

void ZEITIntegrityToolWindow::btnBrowseIntegrityFile_clicked()
{
	QString FileName = QFileDialog::getSaveFileName(this, "Save Integrity File", Form->txtIntegrityFile->text(), "Integrity Generator files (*.ZEITIntegrity);;All files (*.*)");
	if (FileName.isEmpty())
		return;

	Form->txtIntegrityFile->setText(FileName);
	Generator.SetIntegrityFileName(FileName.toUtf8().begin());
}


void ZEITIntegrityToolWindow::lstIncludes_itemSelectionChanged()
{
	UpdateUI();
}

void ZEITIntegrityToolWindow::lstIncludes_itemChanged(QListWidgetItem* Item)
{
	for (ZESize I = 0; I < Form->lstIncludes->count(); I++)
	{
		if (Form->lstIncludes->item(I) != Item)
			continue;

		ZEITScannerEntry& Entry = Generator.GetInclude(I);
		Entry.SetPath(Item->text().toUtf8().begin());
		Entry.SetEnabled(Item->checkState() == Qt::Checked);
	}
}

void ZEITIntegrityToolWindow::btnIncludeAdd_clicked()
{
	QListWidgetItem* Item = new QListWidgetItem();
	Item->setFlags(Item->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
	Item->setCheckState(Qt::Checked);
	Form->lstIncludes->addItem(Item);

	ZEITScannerEntry Entry;
	Generator.AddInclude(Entry);
	
	UpdateUI();
}

void ZEITIntegrityToolWindow::btnIncludeRemove_clicked()
{
	QList<QListWidgetItem*> Items = Form->lstIncludes->selectedItems();
	for (int I = Items.count() - 1; I >= 0; I--)
	{
		Generator.RemoveInclude(I);
		delete Items[I];
	}

	UpdateUI();
}

void ZEITIntegrityToolWindow::lstExcludes_itemSelectionChanged()
{
	UpdateUI();
}

void ZEITIntegrityToolWindow::lstExcludes_itemChanged(QListWidgetItem* Item)
{
	for (ZESize I = 0; I < Form->lstExludes->count(); I++)
	{
		if (Form->lstExludes->item(I) != Item)
			continue;

		ZEITScannerEntry& Entry = Generator.GetExclude(I);
		Entry.SetPath(Item->text().toUtf8().begin());
		Entry.SetEnabled(Item->checkState() == Qt::Checked);
	}
}

void ZEITIntegrityToolWindow::btnExcludeAdd_clicked()
{
	QListWidgetItem* Item = new QListWidgetItem();
	Item->setFlags(Item->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
	Item->setCheckState(Qt::Checked);
	Form->lstExludes->addItem(Item);

	ZEITScannerEntry Entry;
	Generator.AddExclude(Entry);

	UpdateUI();
}

void ZEITIntegrityToolWindow::btnExcludeRemove_clicked()
{
	QList<QListWidgetItem*> Items = Form->lstExludes->selectedItems();
	for (int I = Items.count() - 1; I >= 0; I--)
	{
		Generator.RemoveExclude(I);
		delete Items[I];
	}

	UpdateUI();
}

void ZEITIntegrityToolWindow::tblRecords_itemChanged(QTableWidgetItem* Item)
{
	int Index = Item->row();
	ZEITRecord& Record = Generator.GetRecord(Index);

	switch (Item->column())
	{
		case 0:
			Record.SetEnabled(Item->checkState() == Qt::Checked);
			break;

		case 2:
			Record.SetPath(Item->text().toUtf8().begin());
			break;

		case 5:
			Record.SetRequired(Item->checkState() == Qt::Checked);
			break;

		default:
			break;
	}
}

void ZEITIntegrityToolWindow::tblRecords_itemSelectionChanged()
{
	UpdateUI();
}

void ZEITIntegrityToolWindow::btnRecordAdd_clicked()
{
	QSignalBlocker tblRecordsBlocker(Form->tblRecords);

	ZEITRecord Record;
	Generator.AddRecord(Record);

	Form->tblRecords->setRowCount(Form->tblRecords->rowCount() + 1);
	UpdateRecord(Form->tblRecords->rowCount() - 1);
	
	UpdateUI();
}

void ZEITIntegrityToolWindow::btnRecordRemove_clicked()
{
	QSignalBlocker tblRecordsBlocker(Form->tblRecords);

	QModelIndexList List = Form->tblRecords->selectionModel()->selectedRows();
	for (int I = List.count() - 1; I >= 0; I--)
	{
		Generator.RemoveRecord(List[I].row());
		Form->tblRecords->removeRow(List[I].row());
	}

	UpdateUI();
}

void ZEITIntegrityToolWindow::btnRecordExclude_clicked()
{
	QSignalBlocker tblRecordsBlocker(Form->tblRecords);

	QModelIndexList List = Form->tblRecords->selectionModel()->selectedRows();
	for (int I = List.count() - 1; I >= 0; I--)
	{
		ZEITScannerEntry Entry;
		Entry.SetPath(Form->tblRecords->item(List[I].row(), 2)->text().toUtf8().begin());
		Generator.AddExclude(Entry);

		QListWidgetItem* Item = new QListWidgetItem();
		Item->setFlags(Item->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
		Item->setCheckState(Qt::Checked);
		Item->setText(Entry.GetPath().ToCString());
		Form->lstExludes->addItem(Item);

		Generator.RemoveRecord(List[I].row());
		Form->tblRecords->removeRow(List[I].row());
	}

	UpdateUI();
}

void ZEITIntegrityToolWindow::btnRecordUpdate_clicked()
{
	QSignalBlocker tblRecordsBlocker(Form->tblRecords);

	QModelIndexList List = Form->tblRecords->selectionModel()->selectedRows();
	for (int I = 0; I < List.count(); I++)
	{
		Generator.Generate(List[I].row());
		UpdateRecord(List[I].row());
	}

	UpdateUI();
}

void ZEITIntegrityToolWindow::btnRecordClear_clicked()
{
	QSignalBlocker tblRecordsBlocker(Form->tblRecords);

	Generator.ClearRecords();
	UpdateLists();
	UpdateUI();
}

ZEITIntegrityToolWindow::ZEITIntegrityToolWindow()
{
	GeneratorWorker = new ZEITGeneratorWorker();
	GeneratorWorker->SetGenerator(&Generator);

	Form = new Ui_ZEITIntegrityToolWindow();
	Form->setupUi(this);

	Form->lstIncludes->setSelectionMode(QAbstractItemView::ExtendedSelection);
	Form->lstExludes->setSelectionMode(QAbstractItemView::ExtendedSelection);

	Form->tblRecords->setColumnCount(6);
	Form->tblRecords->setSelectionBehavior(QAbstractItemView::SelectRows);
	Form->tblRecords->setSelectionMode(QAbstractItemView::ExtendedSelection);
	Form->tblRecords->setHorizontalHeaderItem(0, new QTableWidgetItem());
	Form->tblRecords->horizontalHeaderItem(0)->setText("Enabled");
	Form->tblRecords->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	Form->tblRecords->setHorizontalHeaderItem(1, new QTableWidgetItem());
	Form->tblRecords->horizontalHeaderItem(1)->setText("Status");
	Form->tblRecords->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	Form->tblRecords->setHorizontalHeaderItem(2, new QTableWidgetItem());
	Form->tblRecords->horizontalHeaderItem(2)->setText("Path");
	Form->tblRecords->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	Form->tblRecords->setHorizontalHeaderItem(3, new QTableWidgetItem());
	Form->tblRecords->horizontalHeaderItem(3)->setText("Size");
	Form->tblRecords->horizontalHeaderItem(3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	Form->tblRecords->setHorizontalHeaderItem(4, new QTableWidgetItem());
	Form->tblRecords->horizontalHeaderItem(4)->setText("Checksum");
	Form->tblRecords->horizontalHeaderItem(4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	Form->tblRecords->setHorizontalHeaderItem(5, new QTableWidgetItem());
	Form->tblRecords->horizontalHeaderItem(5)->setText("Required");
	Form->tblRecords->horizontalHeaderItem(5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	Form->tblRecords->setSortingEnabled(true);

	Form->tblRecords->horizontalHeader()->setMinimumSectionSize(0);
	Form->tblRecords->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	Form->tblRecords->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
	Form->tblRecords->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
	Form->tblRecords->setStyleSheet(
		"QCheckBox"
		"{"
			"margin-left:50%;"
			"margin-right:50%;"
		"}");

	setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint | Qt::WindowMinMaxButtonsHint);

	connect(Form->actNew,				SIGNAL(triggered()), this, SLOT(actNew_triggered()));
	connect(Form->actOpen,				SIGNAL(triggered()), this, SLOT(actOpen_triggered()));
	connect(Form->actSave,				SIGNAL(triggered()), this, SLOT(actSave_triggered()));
	connect(Form->actSaveAs,			SIGNAL(triggered()), this, SLOT(actSaveAs_triggered()));
	connect(Form->actQuit,				SIGNAL(triggered()), this, SLOT(actQuit_triggered()));

	connect(Form->btnBrowseEnginePath,	SIGNAL(clicked()), this, SLOT(btnBrowseEnginePath_clicked()));
	connect(Form->txtEnginePath,		SIGNAL(textEdited(const QString&)), this, SLOT(txtEnginePath_textEdited(const QString&)));

	connect(Form->btnBrowseIntegrityFile,SIGNAL(clicked()), this, SLOT(btnBrowseIntegrityFile_clicked()));
	connect(Form->txtIntegrityFile,		SIGNAL(textEdited(const QString&)), this, SLOT(txtIntegrityFile_textEdited(const QString&)));

	connect(Form->btnScan,				SIGNAL(clicked()), this, SLOT(btnScan_clicked()));
	connect(Form->btnGenerate,			SIGNAL(clicked()), this, SLOT(btnGenerate_clicked()));
	connect(GeneratorWorker,			SIGNAL(RecordUpdated(unsigned int)), this, SLOT(GeneratorWorker_RecordUpdated(unsigned int)));
	connect(GeneratorWorker,			SIGNAL(StateChanged()), this, SLOT(GeneratorWorker_StateChanged()));

	connect(Form->lstIncludes,			SIGNAL(itemSelectionChanged()), this, SLOT(lstIncludes_itemSelectionChanged()));
	connect(Form->lstIncludes,			SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(lstIncludes_itemChanged(QListWidgetItem*)));
	connect(Form->btnIncludeAdd,		SIGNAL(clicked()), this, SLOT(btnIncludeAdd_clicked()));
	connect(Form->btnIncludeRemove,		SIGNAL(clicked()), this, SLOT(btnIncludeRemove_clicked()));

	connect(Form->lstExludes,			SIGNAL(itemSelectionChanged()), this, SLOT(lstExcludes_itemSelectionChanged()));
	connect(Form->lstExludes,			SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(lstExcludes_itemChanged(QListWidgetItem*)));
	connect(Form->btnExcludeAdd,		SIGNAL(clicked()), this, SLOT(btnExcludeAdd_clicked()));
	connect(Form->btnExcludeRemove,		SIGNAL(clicked()), this, SLOT(btnExcludeRemove_clicked()));


	connect(Form->tblRecords,			SIGNAL(itemSelectionChanged()), this, SLOT(lstIncludes_itemSelectionChanged()));
	connect(Form->tblRecords,			SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(tblRecords_itemChanged(QTableWidgetItem*)));
	connect(Form->btnRecordAdd,			SIGNAL(clicked()), this, SLOT(btnRecordAdd_clicked()));
	connect(Form->btnRecordRemove,		SIGNAL(clicked()), this, SLOT(btnRecordRemove_clicked()));
	connect(Form->btnRecordExclude,		SIGNAL(clicked()), this, SLOT(btnRecordExclude_clicked()));
	connect(Form->btnRecordUpdate,		SIGNAL(clicked()), this, SLOT(btnRecordUpdate_clicked()));
	connect(Form->btnRecordClear,		SIGNAL(clicked()), this, SLOT(btnRecordClear_clicked()));

	LoadRecentFiles();
	UpdateUI();
}

ZEITIntegrityToolWindow::~ZEITIntegrityToolWindow()
{
	delete Form;
	delete GeneratorWorker;
}
