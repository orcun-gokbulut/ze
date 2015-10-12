//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELCLicenseToolWindow.cpp
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

#include "ZELCLicenseToolWindow.h"
#include "ui_ZELCLicenseToolWindow.h"

#include "ZEProtect\ZELCSerialKeyGenerator.h"
#include "ZEProtect\ZELCLicense.h"
#include "ZEProtect\ZELCActivationCodeGenerator.h"
#include "ZEVersion.h"
#include "ZERandom.h"

#include <QtGui\QMessageBox>
#include <QtGui\QInputDialog>

void ZELCLicenseToolWindow::btnGenerateSerial_clicked()
{
	ZELCSerialKeyGenerator Generator;
	Generator.SetApplicationName(Form->txtName->text().toLocal8Bit().begin());
	Generator.SetApplicationVersionMajor(Form->txtVersionMajor->value());
	Generator.SetApplicationVersionMinor(Form->txtVersionMinor->value());
	Generator.SetApplicationEdition(Form->txtEdition->value());
	Generator.SetLicenseeName(Form->txtLicensee->text().toLocal8Bit().begin());
	Generator.SetSerialNumber(Form->txtSerialNumber->value());

	ZEString Output = Generator.GenerateSerialKey();
	
	if (Output.IsEmpty())
		QMessageBox::critical(this, "Zinek Engine Crack", "Cannot generate serial key.");

	Form->txtSerialKey->setText(Output.ToCString());
}

void ZELCLicenseToolWindow::btnGenerateActivationCode_clicked()
{
	bool Ok = false;
	QString Password = QInputDialog::getText(this, "Activation Keys Password", "Password:", QLineEdit::Password, "", &Ok);
	if (!Ok || Password.isEmpty())
		return;

	ZELCActivationCodeGenerator Generator;
	Generator.LoadKeys(Password.toLocal8Bit().begin());
	Generator.SetSerialKey(Form->txtSerialKeyActivation->text().toLocal8Bit().begin());
	Generator.SetPreActivationCode(Form->txtPreActivationCode->toPlainText().toLocal8Bit().begin());
	
	ZEString Output;
	if (!Generator.GenerateActivationCode(Output))
	{
		QMessageBox::critical(this, "Zinek Licensing Tool", "Code generation failed !\nPrivate Key Password, Serial Key or Pre-Activation Code is not valid.", QMessageBox::Ok);
		return;
	}
	Form->txtActivationCode->setPlainText(Output.ToCString());
}

ZELCLicenseToolWindow::ZELCLicenseToolWindow()
{
	Form = new Ui_ZELCLicenseToolWindow();
	Form->setupUi(this);
	Form->txtName->setText("Zinek Engine");
	Form->txtVersionMajor->setValue(ZEVersion::GetZinekVersion().Major);
	Form->txtVersionMinor->setValue(ZEVersion::GetZinekVersion().Minor);
	Form->txtSerialNumber->setValue(ZERandom::GetUInt16());

	connect(Form->btnGenerateSerial, SIGNAL(clicked()), this, SLOT(btnGenerateSerial_clicked()));
	connect(Form->btnGenerateActivationCode, SIGNAL(clicked()), this, SLOT(btnGenerateActivationCode_clicked()));
}

ZELCLicenseToolWindow::~ZELCLicenseToolWindow()
{
	delete Form;
}
