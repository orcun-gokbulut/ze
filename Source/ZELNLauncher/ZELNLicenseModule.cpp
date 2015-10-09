//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELNLicenseModule.cpp
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

#include "ZELNLicenseModule.h"

#include "ZELNLicenseWidget.h"
#include "ui_ZELNLicenseWidget.h"
#include "ZELog.h"
#include "ZELNLauncher.h"
#include "ZEProtect\ZELCLicenseManager.h"

#include <QtGui\QMessageBox>

void ZELNLicenseModule::UpdateWidget()
{
	GetForm()->lblApplicationName->setText(License.GetApplicationName().ToCString());
	GetForm()->lblApplicationVersion->setText(QString("%1.%2").arg(License.GetApplicationVersionMajor()).arg(License.GetApplicationVersionMinor()));
	GetForm()->lblApplicationEdition->setText(QString::number(License.GetApplicationEdition()));
	GetForm()->lblLicensee->setText(License.GetLicenseeName().ToCString());
	GetForm()->lblSerialKey->setText(License.GetSerialKey().ToCString());

	if (License.GetSerialKey().IsEmpty())
	{
		GetForm()->grpSerialKey->setVisible(true);
		GetForm()->grpActivate->setVisible(false);
		GetForm()->txtSerialKey->setText(License.GetSerialKey().ToCString());
		GetForm()->txtLicensee->setText(License.GetLicenseeName().ToCString());
		GetForm()->lblStatus->setText("<html><head/><body><p><span style=\"font-weight:600; color:#0000aa;\">Enter Serial Key</span></p></body></html>");
	}
	else if (!License.CheckSerialKeyValid())
	{
		GetForm()->grpSerialKey->setVisible(true);
		GetForm()->grpActivate->setVisible(false);
		GetForm()->txtSerialKey->setText(License.GetSerialKey().ToCString());
		GetForm()->txtLicensee->setText(License.GetLicenseeName().ToCString());
		GetForm()->lblStatus->setText("<html><head/><body><p><span style=\"font-weight:600; color:#aa0000;\">Invalid Serial Key</span></p></body></html>");
	}
	else
	{
		GetForm()->grpSerialKey->setVisible(false);
		if (License.GetActivationCode().IsEmpty())
		{
			GetForm()->lblStatus->setText("<html><head/><body><p><span style=\"font-weight:600; color:#0000aa;\">Enter Activation Code</span></p></body></html>");
			GetForm()->grpActivate->setVisible(true);
			GetForm()->txtPreActivationCode->setPlainText(License.GeneratePreActivationCode().ToCString());
			GetForm()->txtActivationCode->setPlainText("");

		}
		else if (!License.CheckActivationCodeValid())
		{
			GetForm()->lblStatus->setText("<html><head/><body><p><span style=\"font-weight:600; color:#aa0000;\">Invalid Activation Code</span></p></body></html>");
			GetForm()->grpActivate->setVisible(true);
			GetForm()->txtSerialKeyActivation->setText(License.GetSerialKey().ToCString());
			GetForm()->txtPreActivationCode->setPlainText(License.GeneratePreActivationCode().ToCString());
			GetForm()->txtActivationCode->setPlainText("");
		}
		else
		{
			GetForm()->lblStatus->setText("<html><head/><body><p><span style=\"font-weight:600; color:#00aa00;\">Valid License</span></p></body></html>");
			GetForm()->grpActivate->setVisible(false);
		}
	}
}

bool ZELNLicenseModule::InitializeSelf()
{
	if (!ZEInitializable::InitializeSelf())
		return false;

	Widget = new ZELNLicenseWidget();
	Widget->Module = this;
	ZELCLicenseManager LicenseManager;
	LicenseManager.LoadLicenses();
	const ZELCLicense* ResultLicense = LicenseManager.RequestLicense(ZELNLauncher::GetInstance()->GetApplicationName(), ZELNLauncher::GetInstance()->GetApplicationVersionMajor());
	if (ResultLicense != NULL)
	{
		License = *ResultLicense;
		LicenseValid = true;
	}
	else
	{
		ZEArray<ZELCLicense> Licenses = LicenseManager.GetLicenses(ZELNLauncher::GetInstance()->GetApplicationName(), ZELNLauncher::GetInstance()->GetApplicationVersionMajor());
		if (Licenses.GetCount() != 0)
		{
			Licenses.Sort2<ZELCLicenseManager::CompareLicenseOrder>();
			License = Licenses[0];
		}
		else
		{
			License = ZELCLicense();
			License.SetApplicationName(ZELNLauncher::GetInstance()->GetApplicationName());
			License.SetApplicationVersionMajor(ZELNLauncher::GetInstance()->GetApplicationVersionMajor());
			License.SetApplicationVersionMinor(ZELNLauncher::GetInstance()->GetApplicationVersionMinor());
		}
		LicenseValid = false;
	}

	UpdateWidget();

	return true;
}

const char* ZELNLicenseModule::GetName()
{
	return "License";
}

QWidget* ZELNLicenseModule::GetWidget()
{
	return Widget;
}

bool ZELNLicenseModule::GetAllowLaunch()
{
	return LicenseValid;
}

Ui_ZELNLicenseWidget* ZELNLicenseModule::GetForm()
{
	return Widget->Form;
}

void ZELNLicenseModule::EnterSerial()
{
	License.SetLicenseeName(GetForm()->txtLicensee->text().toLocal8Bit().begin());
	License.SetSerialKey(GetForm()->txtSerialKey->text().toLocal8Bit().begin());

	if (!License.CheckSerialKeyValid())
	{
		QMessageBox::critical(GetWidget(), "Zinek Launcher", "Invalid Serial Key !", QMessageBox::Ok);
		return;
	}

	ZELCLicenseManager Manager;
	Manager.LoadLicenses();
	Manager.RegisterLicense(License);
	Manager.SaveLicenses();

	UpdateWidget();
}

void ZELNLicenseModule::Activate()
{
	License.SetActivationCode(GetForm()->txtActivationCode->toPlainText().toLocal8Bit().begin());

	if (!License.CheckActivationCodeValid())
	{
		QMessageBox::critical(GetWidget(), "Zinek Launcher", "Invalid Activation Code !", QMessageBox::Ok);
		return;
	}

	ZELCLicenseManager Manager;
	Manager.LoadLicenses();
	Manager.RegisterLicense(License);
	Manager.SaveLicenses();

	LicenseValid = true;

	UpdateWidget();
}
