//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELNLicenseEditorDialog.cpp
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

#include "ZELNLicenseEditorDialog.h"
#include "ui_ZELNLicenseEditorDialog.h"

#include "ZELNLauncher.h"
#include "ZELNLicenseModule.h"
#include "ZEProtect/ZELCLicenseManager.h"

#include <QtCore/QMimeData>
#include <QtGui/QClipboard>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QDialogButtonBox>

void ZELNLicenseEditorDialog::UpdateWidget()
{
	Form->txtGUID->setText(License.GetGUID().ToString().ToCString());
	Form->txtApplicationName->setText(License.GetProductName().ToCString());
	Form->txtLicensee->setText(License.GetLicenseeName().ToCString());
	Form->txtSerialKey->setText(License.GetSerialKey().ToCString());
	Form->txtActivationCode->setPlainText(License.GetActivationCode().ToCString());
	Form->chkEnabled->setChecked(License.GetEnabled());
}

void ZELNLicenseEditorDialog::btnGeneratePreActivationCode_clicked()
{
	License.SetProductName(Form->txtApplicationName->text().toLocal8Bit().begin());
	License.SetLicenseeName(Form->txtLicensee->text().toLocal8Bit().begin());
	License.SetSerialKey(Form->txtSerialKey->text().toLocal8Bit().begin());

	QDialog Dialog;
	Dialog.setModal(true);

	QPlainTextEdit* txtPreActivationCode = new QPlainTextEdit();
	txtPreActivationCode->setPlainText(License.GeneratePreActivationCode().ToCString());
	txtPreActivationCode->setReadOnly(true);

	QVBoxLayout* Layout = new QVBoxLayout();
	Layout->addWidget(txtPreActivationCode);

	Dialog.setLayout(Layout);
	Dialog.exec();
}

void ZELNLicenseEditorDialog::btnPasteActivationCode_clicked()
{
	QClipboard* Clipboard = QApplication::clipboard();
	if (!Clipboard->mimeData()->hasText())
		return;

	Form->txtActivationCode->setPlainText(Clipboard->text());
}

void ZELNLicenseEditorDialog::btnOpenActivationCode_clicked()
{
	QString FileName = QFileDialog::getOpenFileName(this, "Open Activation Code File", QString(), "Text files (*.txt);;All files (*.*)");
	if (FileName.isEmpty())
		return;

	QFile File(FileName);
	bool Result = File.open(QIODevice::ReadOnly | QIODevice::Text);
	if (!Result)
	{
		QMessageBox::critical(this, "Open Activation Code File", "Cannot read file.", QDialogButtonBox::Ok, QDialogButtonBox::NoButton);
		return;
	}

	Form->txtActivationCode->setPlainText(File.readAll());

	File.close();
}

void ZELNLicenseEditorDialog::btnSave_clicked()
{
	License.SetProductName(Form->txtApplicationName->text().toLocal8Bit().begin());
	License.SetLicenseeName(Form->txtLicensee->text().toLocal8Bit().begin());
	License.SetSerialKey(Form->txtSerialKey->text().toLocal8Bit().begin());
	License.SetActivationCode(Form->txtActivationCode->toPlainText().toLocal8Bit().begin());
	License.SetEnabled(Form->chkEnabled->isChecked());

	ZELCLicenseManager Manager;
	Manager.LoadLicenses();
	Manager.RegisterLicense(License);
	Manager.SaveLicenses();
	accept();
}

void ZELNLicenseEditorDialog::SetLicense(const ZELCLicense& License)
{
	this->License = License;
	UpdateWidget();
}

const ZELCLicense& ZELNLicenseEditorDialog::GetLicense()
{
	return License;
}

ZELNLicenseEditorDialog::ZELNLicenseEditorDialog(QWidget* Parent) : QDialog(Parent)
{
	Form = new Ui_ZELNLicenseEditorDialog();
	Form->setupUi(this);
	connect(Form->btnSave, SIGNAL(clicked()), this, SLOT(btnSave_clicked()));
	connect(Form->btnGeneratePreActivationCode, SIGNAL(clicked()), this, SLOT(btnGeneratePreActivationCode_clicked()));
	connect(Form->btnClose, SIGNAL(clicked()), this, SLOT(reject()));
	connect(Form->btnPasteActivationCode, SIGNAL(clicked()), this, SLOT(btnPasteActivationCode_clicked()));
	connect(Form->btnOpenActivationCode, SIGNAL(clicked()), this, SLOT(btnOpenActivationCode_clicked()));
}

ZELNLicenseEditorDialog::~ZELNLicenseEditorDialog()
{
	delete Form;
}

