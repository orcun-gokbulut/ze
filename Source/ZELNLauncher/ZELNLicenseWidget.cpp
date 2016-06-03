//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELNLicenseWidget.cpp
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

#include "ZELNLicenseWidget.h"
#include "ui_ZELNLicenseWidget.h"

#include "ZELNLauncher.h"
#include "ZELNLicenseModule.h"
#include "ZELNLicenseManagerDialog.h"
#include "ZEProtect/ZELCLicenseManager.h"

#include <QtCore/QTextStream>
#include <QtCore/QMimeData>
#include <QtGui/QClipboard>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>

void ZELNLicenseWidget::UpdateWidget()
{
	Form->lblApplicationName->setText(License.GetApplicationName().ToCString());
	Form->lblApplicationVersion->setText(QString("%1.%2").arg(License.GetApplicationVersionMajor()).arg(License.GetApplicationVersionMinor()));
	Form->lblApplicationEdition->setText(QString::number(License.GetApplicationEdition()));
	Form->lblLicensee->setText(License.GetLicenseeName().ToCString());
	Form->lblSerialKey->setText(License.GetSerialKey().ToCString());

	if (License.GetSerialKey().IsEmpty())
	{
		Form->grpSerialKey->setVisible(true);
		Form->grpActivate->setVisible(false);
		Form->txtSerialKey->setText(License.GetSerialKey().ToCString());
		Form->txtLicensee->setText(License.GetLicenseeName().ToCString());
		Form->lblStatus->setText("<html><head/><body><p><span style=\"font-weight:600; color:#0000aa;\">Enter Serial Key</span></p></body></html>");
	}
	else if (!License.CheckSerialKeyValid())
	{
		Form->grpSerialKey->setVisible(true);
		Form->grpActivate->setVisible(false);
		Form->txtSerialKey->setText(License.GetSerialKey().ToCString());
		Form->txtLicensee->setText(License.GetLicenseeName().ToCString());
		Form->lblStatus->setText("<html><head/><body><p><span style=\"font-weight:600; color:#aa0000;\">Invalid Serial Key</span></p></body></html>");
	}
	else
	{
		Form->grpSerialKey->setVisible(false);
		Form->txtSerialKeyActivation->setText(License.GetSerialKey().ToCString());
		Form->txtActivationCode->setPlainText("");
		Form->txtPreActivationCode->setPlainText(License.GeneratePreActivationCode().ToCString());
		Form->grpActivate->setVisible(true);
		if (License.GetActivationCode().IsEmpty())
		{
			Form->lblStatus->setText("<html><head/><body><p><span style=\"font-weight:600; color:#0000aa;\">Enter Activation Code</span></p></body></html>");
		}
		else if (!License.CheckActivationCodeValid())
		{
			Form->lblStatus->setText("<html><head/><body><p><span style=\"font-weight:600; color:#aa0000;\">Invalid Activation Code</span></p></body></html>");
		}
		else
		{
			Form->lblStatus->setText("<html><head/><body><p><span style=\"font-weight:600; color:#00aa00;\">Valid License</span></p></body></html>");
			Form->grpActivate->setVisible(false);
		}
	}
}

void ZELNLicenseWidget::SaveLicense()
{
	ZELCLicenseManager Manager;
	Manager.LoadLicenses();
	Manager.RegisterLicense(License);
	Manager.SaveLicenses();
}

void ZELNLicenseWidget::btnEnter_clicked()
{
	License.SetApplicationName(ZELNLauncher::GetInstance()->GetApplicationName());
	License.SetLicenseeName(Form->txtLicensee->text().toLocal8Bit().begin());
	License.SetSerialKey(Form->txtSerialKey->text().toLocal8Bit().begin());

	if (!License.CheckSerialKeyValid())
	{
		QMessageBox::critical(this, "Zinek Launcher", "Invalid Serial Key !", QMessageBox::Ok);
		return;
	}

	ZELCLicenseManager Manager;
	Manager.LoadLicenses();
	Manager.RegisterLicense(License);
	Manager.SaveLicenses();

	UpdateWidget();
	SaveLicense();
}

void ZELNLicenseWidget::btnActivate_clicked()
{
	License.SetActivationCode(Form->txtActivationCode->toPlainText().toLocal8Bit().begin());

	if (!License.CheckActivationCodeValid())
	{
		QMessageBox::critical(this, "Zinek Launcher", "Invalid Activation Code !", QMessageBox::Ok);
		return;
	}

	UpdateWidget();
	SaveLicense();

	Module->LoadLicense();
}

void ZELNLicenseWidget::btnLicenseManager_clicked()
{
	ZELNLicenseManagerDialog LicenseManagerWindow;
	LicenseManagerWindow.setModal(true);
	LicenseManagerWindow.exec();
	
	Module->LoadLicense();
}

void ZELNLicenseWidget::btnCopyPreActivationCode_clicked()
{
	QClipboard* Clipboard = QApplication::clipboard();
	Clipboard->setText(Form->txtPreActivationCode->toPlainText());
}

void ZELNLicenseWidget::btnSavePreActivationCode_clicked()
{
	QString FileName = QFileDialog::getSaveFileName(this, "Save Pre-Activation File", QString(), "Text files (*.txt);;All files (*.*)");
	if (FileName.isEmpty())
		return;
	
	if (QFile::exists(FileName))
	{
		int Result = QMessageBox::question(this, "Save Pre-Activation Code", "Are you sure you want to overwrite this file ?", QDialogButtonBox::Yes, QDialogButtonBox::No);
		if (Result == QDialogButtonBox::No || Result == -1)
			return;
	}

	QFile File(FileName);
	bool Result = File.open(QIODevice::WriteOnly | QIODevice::Text);
	if (!Result)
	{
		QMessageBox::critical(this, "Save Pre-Activation Code", "Cannot open file.", QDialogButtonBox::Ok, QDialogButtonBox::NoButton);
		return;
	}

	QTextStream Stream(&File);
	Stream << Form->txtPreActivationCode->toPlainText();

	File.close();
}

void ZELNLicenseWidget::btnPasteActivationCode_clicked()
{
	QClipboard* Clipboard = QApplication::clipboard();
	if (!Clipboard->mimeData()->hasText())
		return;

	Form->txtActivationCode->setPlainText(Clipboard->text());
}

void ZELNLicenseWidget::btnOpenActivationCode_clicked()
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

void ZELNLicenseWidget::SetLicense(const ZELCLicense& License)
{
	this->License = License;
	UpdateWidget();
}

const ZELCLicense& ZELNLicenseWidget::GetLicense()
{
	return License;
}

ZELNLicenseWidget::ZELNLicenseWidget(QWidget* Parent) : QWidget(Parent)
{
	Form = new Ui_ZELNLicenseWidget();
	Form->setupUi(this);
	Form->grpSerialKey->setVisible(false);
	Form->grpActivate->setVisible(false);
	connect(Form->btnEnter, SIGNAL(clicked()), this, SLOT(btnEnter_clicked()));
	connect(Form->btnActivate, SIGNAL(clicked()), this, SLOT(btnActivate_clicked()));
	connect(Form->btnLicenseManager, SIGNAL(clicked()), this, SLOT(btnLicenseManager_clicked()));
	connect(Form->btnCopyPreActivationCode, SIGNAL(clicked()), this, SLOT(btnCopyPreActivationCode_clicked()));
	connect(Form->btnSavePreActivationCode, SIGNAL(clicked()), this, SLOT(btnSavePreActivationCode_clicked()));
	connect(Form->btnPasteActivationCode, SIGNAL(clicked()), this, SLOT(btnPasteActivationCode_clicked()));
	connect(Form->btnOpenActivationCode, SIGNAL(clicked()), this, SLOT(btnOpenActivationCode_clicked()));
}

ZELNLicenseWidget::~ZELNLicenseWidget()
{
	delete Form;
}

