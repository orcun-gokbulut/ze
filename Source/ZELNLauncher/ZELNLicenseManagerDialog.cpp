//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELNLicenseManagerDialog.cpp
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

#include "ZELNLicenseManagerDialog.h"
#include "Ui_ZELNLicenseManagerDialog.h"

#include "ZELNLauncher.h"
#include "ZELNLicenseWidget.h"
#include "ZEProtect\ZELCLicense.h"
#include "ZEProtect\ZELCLicenseManager.h"

#include <QtWidgets\QMessageBox>
#include <QtWidgets\QFileDialog>
#include "ZELNLicenseEditorDialog.h"

void ZELNLicenseManagerDialog::LoadLicenses()
{
	Form->tblLicenses->setRowCount((int)LicenseManager.GetLicenses().GetCount());
	for (int I = 0; I < LicenseManager.GetLicenses().GetCount(); I++)
	{
		const ZELCLicense& License = LicenseManager.GetLicenses()[I];
		Form->tblLicenses->setItem(I, 0, new QTableWidgetItem(License.GetGUID().ToString().ToCString()));
		Form->tblLicenses->setItem(I, 1, new QTableWidgetItem(License.CheckValid() ? "Yes" : "No"));
		Form->tblLicenses->setItem(I, 2, new QTableWidgetItem(License.GetLicenseeName().ToCString()));
		Form->tblLicenses->setItem(I, 3, new QTableWidgetItem(License.GetSerialKey().ToCString()));
		Form->tblLicenses->setItem(I, 4, new QTableWidgetItem(License.GetProductName().ToCString()));
		Form->tblLicenses->setItem(I, 5, new QTableWidgetItem(QString("%1.%2")
			.arg(License.GetProductVersionMajor())
			.arg(License.GetProductVersionMinor())));
		Form->tblLicenses->setItem(I, 6, new QTableWidgetItem(QString::number(License.GetProductEdition())));
		Form->tblLicenses->setItem(I, 7, new QTableWidgetItem(License.GetSystemWide() ? "System Wide" : "Instance"));
		Form->tblLicenses->setItem(I, 8, new QTableWidgetItem(License.GetEnabled() ? "Yes" : "No"));
	}

	UpdateGUI();
}

void ZELNLicenseManagerDialog::UpdateGUI()
{
	QModelIndexList Model = Form->tblLicenses->selectionModel()->selectedRows();
	int SelectedRowCount = Model.count();
	Form->btnViewEdit->setEnabled(SelectedRowCount == 1);
	Form->btnRemove->setEnabled(SelectedRowCount > 0);
	Form->btnExport->setEnabled(SelectedRowCount > 0);
}

void ZELNLicenseManagerDialog::tblLicenses_cellDoubleClicked(int, int)
{
	btnViewEdit_clicked();
}

void ZELNLicenseManagerDialog::tblLicenses_itemSelectionChanged()
{
	UpdateGUI();
}

void ZELNLicenseManagerDialog::btnViewEdit_clicked()
{
	if (Form->tblLicenses->selectionModel()->selectedRows().count() != 1)
		return;
	
	QModelIndexList SelectedRows = Form->tblLicenses->selectionModel()->selectedIndexes();
	ZEGUID GUID(Form->tblLicenses->item(SelectedRows[0].row(), 0)->text().toLocal8Bit().begin());
	const ZELCLicense* License = LicenseManager.GetLicense(GUID);
	if (License == NULL)
		return;

	ZELNLicenseEditorDialog Editor;
	Editor.SetLicense(*License);
	Editor.setModal(true);
	if (Editor.exec() == 1)
	{
		LicenseManager.LoadLicenses();
		LoadLicenses();
	}
}

void ZELNLicenseManagerDialog::btnAdd_clicked()
{
	ZELCLicense License;
	License.SetProductName(ZELNLauncher::GetInstance()->GetProductName());

	ZELNLicenseEditorDialog Editor;
	Editor.SetLicense(License);
	Editor.setModal(true);
	if (Editor.exec() == QDialog::Accepted)
	{
		LicenseManager.LoadLicenses();
		LoadLicenses();
	}
}

void ZELNLicenseManagerDialog::btnRemove_clicked()
{
	int Result = QMessageBox::question(this, "Remove License", 
		"Are you sure that you want to remove selected license(s) ? \nThis operation cannot be undone.");

	if (Result != QMessageBox::Yes)
		return;

	LicenseManager.LoadLicenses();
		
	QModelIndexList SelectedRows = Form->tblLicenses->selectionModel()->selectedRows();
	for (int I = 0; I < SelectedRows.count(); I++)
	{
		ZEGUID GUID(Form->tblLicenses->item(SelectedRows[I].row(), 0)->text().toLocal8Bit().begin());
		LicenseManager.UnregisterLicense(GUID);
	}

	LicenseManager.SaveLicenses();

	LoadLicenses();
}

void ZELNLicenseManagerDialog::btnRefresh_clicked()
{
	LicenseManager.LoadLicenses();
	LoadLicenses();
}

void ZELNLicenseManagerDialog::btnImport_clicked()
{
	QString FileName = QFileDialog::getOpenFileName(this, "Import License", QString(), "Zinek License File (*.ZELicense);;All Files (*.*");
	if (FileName.isEmpty())
		return;

	ZEArray<ZELCLicense> ImportedLicenses = ZELCLicenseManager::LoadLicenseFile(FileName.toLocal8Bit().begin());
	LicenseManager.LoadLicenses();
	for (ZESize I = 0; I < ImportedLicenses.GetCount(); I++)
		LicenseManager.RegisterLicense(ImportedLicenses[I]);
	LicenseManager.SaveLicenses();
	
	LoadLicenses();
}

void ZELNLicenseManagerDialog::btnExport_clicked()
{
	QString FileName = QFileDialog::getSaveFileName(this, "Export License", QString(), "Zinek License File (*.ZELicense);;All Files (*.*");
	if (FileName.isEmpty())
		return;

	ZEArray<ZELCLicense> ExportLicenses;
	QModelIndexList SelectedRows = Form->tblLicenses->selectionModel()->selectedRows();
	for (int I = 0; I < SelectedRows.count(); I++)
	{
		ZEGUID GUID(Form->tblLicenses->item(SelectedRows[I].row(), 0)->text().toLocal8Bit().begin());
		const ZELCLicense* License = LicenseManager.GetLicense(GUID);
		if (License == NULL)
			continue;

		ExportLicenses.Add(*License);
	}

	ZELCLicenseManager::SaveLicenseFile(FileName.toLocal8Bit().begin(), ExportLicenses);
}

ZELNLicenseManagerDialog::ZELNLicenseManagerDialog()
{
	Form = new Ui_ZELNLicenseManagerDialog();
	Form->setupUi(this);

	connect(Form->tblLicenses, SIGNAL(itemSelectionChanged()), this, SLOT(tblLicenses_itemSelectionChanged()));
	connect(Form->tblLicenses, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(tblLicenses_cellDoubleClicked(int, int)));
	connect(Form->btnViewEdit, SIGNAL(clicked()), this, SLOT(btnViewEdit_clicked()));
	connect(Form->btnAdd, SIGNAL(clicked()), this, SLOT(btnAdd_clicked()));
	connect(Form->btnRemove, SIGNAL(clicked()), this, SLOT(btnRemove_clicked()));
	connect(Form->btnImport, SIGNAL(clicked()), this, SLOT(btnImport_clicked()));
	connect(Form->btnExport, SIGNAL(clicked()), this, SLOT(btnExport_clicked()));

	LicenseManager.LoadLicenses();
	LoadLicenses();
	UpdateGUI();
}

ZELNLicenseManagerDialog::~ZELNLicenseManagerDialog()
{
	delete Form;
}

