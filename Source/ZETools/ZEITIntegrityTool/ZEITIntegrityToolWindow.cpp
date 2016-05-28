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

#include "Ui_ZEITIntegrityToolWindow.h"
#include "ZEITIntegrity/ZEITGenerator.h"
#include "ZEFile/ZEPathManager.h"

#include <QFileDialog>
#include <QMessageBox>
#include "QTreeWidget"

void ZEITIntegrityToolWindow::UpdateRecord(ZESize Index)
{
	if (Form->tblRecords->item(Index, 0) == NULL)
	{
		Form->tblRecords->setItem(Index, 0, new QTableWidgetItem());
		Form->tblRecords->setItem(Index, 1, new QTableWidgetItem());
		Form->tblRecords->setItem(Index, 2, new QTableWidgetItem());
		Form->tblRecords->setItem(Index, 3, new QTableWidgetItem());
		Form->tblRecords->setItem(Index, 4, new QTableWidgetItem());
		Form->tblRecords->setItem(Index, 5, new QTableWidgetItem());
	}
	Qt::ItemFlags Flags = Form->tblRecords->item(Index, 0)->flags();

	Form->tblRecords->item(Index, 0)->setFlags((Flags | Qt::ItemIsUserCheckable) & ~Qt::ItemIsEditable);
	Form->tblRecords->item(Index, 0)->setText("");
	Form->tblRecords->item(Index, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	Form->tblRecords->item(Index, 0)->setCheckState(Generator.GetRecord(Index).GetEnabled() ? Qt::Checked : Qt::Unchecked);

	Form->tblRecords->item(Index, 1)->setFlags(Flags & ~Qt::ItemIsEditable);
	Form->tblRecords->item(Index, 1)->setText("Status");
	Form->tblRecords->item(Index, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	Form->tblRecords->item(Index, 2)->setFlags(Flags | Qt::ItemIsEditable);
	Form->tblRecords->item(Index, 2)->setText(Generator.GetRecord(Index).GetPath().ToCString());
	Form->tblRecords->item(Index, 2)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	Form->tblRecords->item(Index, 3)->setFlags(Flags & ~Qt::ItemIsEditable);
	Form->tblRecords->item(Index, 3)->setText(QString::number(Generator.GetRecord(Index).GetFileSize()));
	Form->tblRecords->item(Index, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	Form->tblRecords->item(Index, 4)->setFlags(Flags & ~Qt::ItemIsEditable);
	Form->tblRecords->item(Index, 4)->setText(Generator.GetRecord(Index).GetChecksum().ToCString());
	Form->tblRecords->item(Index, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	Form->tblRecords->item(Index, 5)->setFlags((Flags | Qt::ItemIsUserCheckable) & ~Qt::ItemIsEditable);
	Form->tblRecords->item(Index, 5)->setText("");
	Form->tblRecords->item(Index, 5)->setCheckState(Generator.GetRecord(Index).GetRequired() ? Qt::Checked : Qt::Unchecked);
	Form->tblRecords->item(Index, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

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

	const ZEArray<ZEITRecord>& Records = Generator.GetRecords();
	Form->tblRecords->clearContents();
	Form->tblRecords->setRowCount(Records.GetCount());
	for (ZESize I = 0; I < Records.GetCount(); I++)
		UpdateRecord(I);
}

void ZEITIntegrityToolWindow::UpdateUI()
{
	Form->btnIncludeRemove->setEnabled(Form->lstIncludes->selectedItems().count() != 0);
	Form->btnExcludeRemove->setEnabled(Form->lstExludes->selectedItems().count() != 0);
	Form->btnRecordRemove->setEnabled(Form->tblRecords->selectedItems().count() != 0);
	Form->btnRecordExclude->setEnabled(Form->tblRecords->selectedItems().count() != 0);
	Form->btnRecordUpdate->setEnabled(Form->tblRecords->selectedItems().count() != 0);
}

ZEITIntegrityToolWindow::ZEITIntegrityToolWindow()
{
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
		"QComboBox"
		"{"
			"margin-left:50%;"
			"margin-right:50%;"
		"}");

	setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint | Qt::WindowMinMaxButtonsHint);

	connect(Form->lstIncludes,			SIGNAL(itemSelectionChanged()), this, SLOT(lstIncludes_itemSelectionChanged()));
	connect(Form->lstIncludes,			SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(lstIncludes_itemChanged(QListWidgetItem*)));
	connect(Form->btnIncludeAdd,		SIGNAL(clicked()), this, SLOT(btnIncludeAdd_clicked()));
	connect(Form->btnIncludeRemove,		SIGNAL(clicked()), this, SLOT(btnIncludeRemove_clicked()));

	connect(Form->lstExludes,			SIGNAL(itemSelectionChanged()), this, SLOT(lstExcludes_itemSelectionChanged()));
	connect(Form->lstExludes,			SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(lstExcludes_itemChanged(QListWidgetItem*)));
	connect(Form->btnExcludeAdd,		SIGNAL(clicked()), this, SLOT(btnExcludeAdd_clicked()));
	connect(Form->btnExcludeRemove,		SIGNAL(clicked()), this, SLOT(btnExcludeRemove_clicked()));

	connect(Form->btnGeneratorLoad,		SIGNAL(clicked()), this, SLOT(btnGeneratorLoad_clicked()));
	connect(Form->btnGeneratorSave,		SIGNAL(clicked()), this, SLOT(btnGeneratorSave_clicked()));
	connect(Form->btnScan,				SIGNAL(clicked()), this, SLOT(btnScan_clicked()));

	connect(Form->tblRecords,			SIGNAL(itemSelectionChanged()), this, SLOT(lstIncludes_itemSelectionChanged()));
	connect(Form->btnRecordAdd,			SIGNAL(clicked()), this, SLOT(btnRecordAdd_clicked()));
	connect(Form->btnRecordRemove,		SIGNAL(clicked()), this, SLOT(btnRecordRemove_clicked()));
	connect(Form->btnRecordExclude,		SIGNAL(clicked()), this, SLOT(btnRecordExclude_clicked()));
	connect(Form->btnRecordUpdate,		SIGNAL(clicked()), this, SLOT(btnRecordUpdate_clicked()));
	connect(Form->btnRecordUpdateAll,	SIGNAL(clicked()), this, SLOT(btnRecordUpdateAll_clicked()));
	connect(Form->btnIntegritySave,		SIGNAL(clicked()), this, SLOT(btnIntegritySave_clicked()));
}

ZEITIntegrityToolWindow::~ZEITIntegrityToolWindow()
{
	delete Form;
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
	for (int I = 0; I < Items.count(); I++)
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
	for (int I = 0; I < Items.count(); I++)
	{
		Generator.RemoveExclude(I);
		delete Items[I];
	}

	UpdateUI();
}

void ZEITIntegrityToolWindow::btnScan_clicked()
{
	Generator.Scan();
	UpdateLists();
}

void ZEITIntegrityToolWindow::btnGeneratorLoad_clicked()
{
	QString FileName = QFileDialog::getOpenFileName(this, "Open Integrity Generator File", QString(), "Integrity Generator (*.ZEITGenerator);;All files (*.*)");
	if (FileName.isEmpty())
		return;
	
	ZEPathManager::GetInstance()->SetAccessControl(false);
	if (!Generator.LoadGeneratorFile(FileName.toUtf8().begin()))
	{
		QMessageBox::critical(this, "Open Integrity Generator File", "Cannot load file.", QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}
	ZEPathManager::GetInstance()->SetAccessControl(true);

	UpdateLists();
	UpdateUI();
}

void ZEITIntegrityToolWindow::btnGeneratorSave_clicked()
{
	QString FileName = QFileDialog::getSaveFileName(this, "Save Integrity Generator File", QString(), "Integrity Generator (*.ZEITGenerator);;All files (*.*)");
	if (FileName.isEmpty())
		return;

	if (QFile::exists(FileName))
	{
		int Result = QMessageBox::question(this, "Save Integrity Generator File", "Are you sure you want to overwrite this file ?", QMessageBox::Yes, QMessageBox::No);
		if (Result == QMessageBox::No || Result == -1)
			return;
	}

	ZEPathManager::GetInstance()->SetAccessControl(false);
	if (!Generator.SaveGeneratorFile(FileName.toUtf8().begin()))
	{
		QMessageBox::critical(this, "Save Integrity Generator File", "Cannot save file.", QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}
	ZEPathManager::GetInstance()->SetAccessControl(true);
}

void ZEITIntegrityToolWindow::tblRecords_itemSelectionChanged()
{
	UpdateUI();
}

void ZEITIntegrityToolWindow::btnRecordAdd_clicked()
{
	Form->tblRecords->setRowCount(Form->tblRecords->rowCount() + 1);
	UpdateRecord(Form->tblRecords->rowCount() - 1);
	
	UpdateUI();
}

void ZEITIntegrityToolWindow::btnRecordRemove_clicked()
{
	QItemSelectionModel* select = Form->tblRecords->selectionModel();

	QModelIndexList List = select->selectedRows();
	for (int I = 0; I < List.count(); I++)
		Form->tblRecords->removeRow(List[I].row());

	UpdateUI();
}

void ZEITIntegrityToolWindow::btnRecordExclude_clicked()
{
	QModelIndexList List = Form->tblRecords->selectionModel()->selectedRows();
	for (int I = 0; I < List.count(); I++)
	{
		QListWidgetItem* Item = new QListWidgetItem();
		Item->setFlags(Item->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
		Item->setCheckState(Qt::Checked);
		Form->lstIncludes->addItem(Item);

		ZEITScannerEntry Entry;
		Entry.SetPath(Form->tblRecords->item(List[I].row(), 3)->text().toUtf8().begin());
		Generator.AddInclude(Entry);

		Form->tblRecords->removeRow(List[I].row());
	}

	List.clear();

	UpdateUI();
}

void ZEITIntegrityToolWindow::btnRecordUpdate_clicked()
{
	QModelIndexList List = Form->tblRecords->selectionModel()->selectedRows();
	for (int I = 0; I < List.count(); I++)
	{
		Generator.GetRecord(List[I].row()).Check();
		UpdateRecord(List[I].row());
	}

	List.clear();

	UpdateUI();
}

void ZEITIntegrityToolWindow::btnRecordUpdateAll_clicked()
{
	Generator.Generate();
	UpdateLists();
	UpdateUI();
}

void ZEITIntegrityToolWindow::btnIntegritySave_clicked()
{

}
