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

#include "ZELNLicenseModule.h"
#include "ZEProtect\ZELCLicenseManager.h"

#include <QtWidgets\QMessageBox>
#include "ZELNLicenseManagerWindow.h"

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

	emit LicenseUpdated();
}

void ZELNLicenseWidget::btnEnter_clicked()
{
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
}

void ZELNLicenseWidget::btnLicenseManager_clicked()
{
	ZELNLicenseManagerWindow LicenseManager;
	LicenseManager.setModal(true);
	LicenseManager.exec();
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

void ZELNLicenseWidget::SetLauncherMode(bool LauncherMode)
{
	Form->btnLicenseManager->setVisible(LauncherMode);
	Form->btnClose->setVisible(!LauncherMode);
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
	connect(Form->btnClose, SIGNAL(clicked()), parent(), SLOT(close()));
	SetLauncherMode(false);
}

ZELNLicenseWidget::~ZELNLicenseWidget()
{
	delete Form;
}

