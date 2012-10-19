//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMapExporterOptionsDialog.cpp
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

#include "ZEMapExporterOptionsDialog.h"
#include "QtGui\QFileDialog"
#include "ZEML\ZEMLProperty.h"
#include "ZEToolComponents\ZEResourceConfigurationWidget\ZEResourceConfigurationWidget.h"

ZEMapExporterOptionsDialog::ZEMapExporterOptionsDialog(QWidget* Parent) : QDialog(Parent)
{
	Form = new Ui::ZEMapExporterOptionsDialogUI();
	Form->setupUi(this);

	Options = NULL;

	connect(Form->btnBrowseEngineDir, SIGNAL(clicked()), this, SLOT(ShowEngineDirectoryDialog()));
	connect(Form->btnBrowseLogPath, SIGNAL(clicked()), this, SLOT(ShowLoggingFilePathDialog()));
	connect(Form->ckbFileLoggingEnabled, SIGNAL(stateChanged(int)), this, SLOT(SetFileLoggingEnabled(int)));
}

ZEMapExporterOptionsDialog::~ZEMapExporterOptionsDialog()
{
	delete Form;
	Form = NULL;
}

void ZEMapExporterOptionsDialog::SetOptions(ZEMLNode* Options)
{
	if(Options == NULL)
		return;

	this->Options = Options;

	const ZEMLItem* CurrentProperty = NULL;
	CurrentProperty = Options->GetProperty("ZinekEngineWorkingDirectory");
	if(CurrentProperty != NULL)
		Form->txtEngineWorkingDirectory->setText(((ZEMLProperty*)CurrentProperty)->GetValue().GetString().ToCString());

	CurrentProperty = Options->GetProperty("LogFilePath");
	if(CurrentProperty != NULL)
		Form->txtLogFilePath->setText(((ZEMLProperty*)CurrentProperty)->GetValue().GetString().ToCString());

	CurrentProperty = Options->GetProperty("IsFileLoggingEnabled");
	if(CurrentProperty != NULL)
		ToggleFileLogging(((ZEMLProperty*)CurrentProperty)->GetValue().GetBoolean());
}

ZEMLNode*	ZEMapExporterOptionsDialog::GetOptions()
{
	CollectOptionsFromForm();
	return Options;
}

void ZEMapExporterOptionsDialog::ToggleFileLogging(bool IsEnabled)
{
	Form->btnBrowseLogPath->setEnabled(IsEnabled);
	Form->txtLogFilePath->setEnabled(IsEnabled);
	Form->ckbFileLoggingEnabled->setChecked(IsEnabled);
}

void ZEMapExporterOptionsDialog::ShowEngineDirectoryDialog()
{
	QString SelectedDirectory = QFileDialog::getExistingDirectory(this, QString("Select Directory"), Form->txtEngineWorkingDirectory->text());

	if(SelectedDirectory.length() == 0)
		return;

	Form->txtEngineWorkingDirectory->setText(SelectedDirectory);
}

void ZEMapExporterOptionsDialog::ShowLoggingFilePathDialog()
{
	QString SaveFilePath = QFileDialog::getSaveFileName(this, QString("Select Directory"), Form->txtEngineWorkingDirectory->text(), QString("*.txt"));

	if(SaveFilePath.length() == 0)
		return;

	Form->txtLogFilePath->setText(SaveFilePath);
}

void ZEMapExporterOptionsDialog::SetFileLoggingEnabled(int CheckBoxState)
{
	if(CheckBoxState == Qt::Checked)
		ToggleFileLogging(true);
	else
		ToggleFileLogging(false);
}

void ZEMapExporterOptionsDialog::CollectOptionsFromForm()
{
	if(Options == NULL)
	{
		Options = new ZEMLNode("Options");
		Options->AddProperty("ZinekEngineWorkingDirectory", ZEVariant(ZEString((const char*)Form->txtEngineWorkingDirectory->text().toLatin1())));
		Options->AddProperty("IsFileLoggingEnabled", ZEVariant(Form->ckbFileLoggingEnabled->isChecked()));
		Options->AddProperty("LogFilePath", ZEVariant(ZEString((const char*)Form->txtLogFilePath->text().toLatin1())));
	}
	else
	{
		if(Options->GetProperty("ZinekEngineWorkingDirectory") !=  NULL)
			((ZEMLProperty*)(Options->GetProperty("ZinekEngineWorkingDirectory")))->SetValue(ZEVariant(ZEString((const char*)Form->txtEngineWorkingDirectory->text().toLatin1())));
		else
			Options->AddProperty("ZinekEngineWorkingDirectory", ZEVariant(ZEString((const char*)Form->txtEngineWorkingDirectory->text().toLatin1())));

		if(Options->GetProperty("IsFileLoggingEnabled") != NULL)
			((ZEMLProperty*)(Options->GetProperty("IsFileLoggingEnabled")))->SetValue(ZEVariant(Form->ckbFileLoggingEnabled->isChecked()));
		else
			Options->AddProperty("IsFileLoggingEnabled", ZEVariant(Form->ckbFileLoggingEnabled->isChecked()));

		if(Options->GetProperty("LogFilePath") != NULL)
			((ZEMLProperty*)(Options->GetProperty("LogFilePath")))->SetValue(ZEVariant(ZEString((const char*)Form->txtLogFilePath->text().toLatin1())));
		else
			Options->AddProperty("LogFilePath", ZEVariant(ZEString((const char*)Form->txtLogFilePath->text().toLatin1())));
	}
}

bool ZEMapExporterOptionsDialog::GetFileLoggingEnabled()
{
	return ((ZEMLProperty*)Options->GetProperty("IsFileLoggingEnabled"))->GetValue().GetBoolean();
}

ZEString ZEMapExporterOptionsDialog::GetLogFilePath()
{
	return ((ZEMLProperty*)Options->GetProperty("LogFilePath"))->GetValue().GetString();
}
