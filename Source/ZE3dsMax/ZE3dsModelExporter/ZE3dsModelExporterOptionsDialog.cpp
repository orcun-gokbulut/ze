//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsModelExporterOptionsDialog.cpp
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

#include "ZE3dsModelExporterOptionsDialog.h"
#include "QtGui\QFileDialog"
#include "ZEML\ZEMLProperty.h"

ZE3dsModelExporterOptionsDialogNew::ZE3dsModelExporterOptionsDialogNew(QWidget* Parent, ZEMLNode* Options) : QDialog(Parent)
{
	Form = new Ui::Form();
	Form->setupUi(this);
	Options = Options;

	connect(Form->btnBrowseEngineDir, SIGNAL(clicked()), this, SLOT(ShowEngineDirectoryDialog()));
	connect(Form->btnBrowseLogPath, SIGNAL(clicked()), this, SLOT(ShowLoggingFilePathDialog()));
	connect(Form->btnBrowseApplicationFolder, SIGNAL(clicked()), this, SLOT(ShowApplicationPathDialog()));
	connect(Form->ckbFileLoggingEnabled, SIGNAL(stateChanged(int)), this, SLOT(SetFileLoggingEnabled(int)));
	connect(Form->cmbRelativeTo, SIGNAL(currentIndexChanged(int)), this, SLOT(SetApplicationPathOptionsVisibility(int)));
	connect(Form->ckbCopyResources, SIGNAL(stateChanged(int)), this, SLOT(SetResourceCopyEnabled(int)));

	SetOptions(Options);
}

void ZE3dsModelExporterOptionsDialogNew::SetOptions(ZEMLNode* Options)
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

	CurrentProperty = Options->GetProperty("ApplicationPath");
	if(CurrentProperty != NULL)
		Form->txtApplicationFolder->setText(((ZEMLProperty*)CurrentProperty)->GetValue().GetString().ToCString());

	CurrentProperty = Options->GetProperty("IsFileLoggingEnabled");
	if(CurrentProperty != NULL)
		ToggleFileLogging(((ZEMLProperty*)CurrentProperty)->GetValue().GetBoolean());

	CurrentProperty = Options->GetProperty("IsResourceCopyingEnabled");
	if(CurrentProperty != NULL)
		Form->ckbCopyResources->setChecked(((ZEMLProperty*)CurrentProperty)->GetValue().GetBoolean());

	CurrentProperty = Options->GetProperty("RelativeTo");
	if(CurrentProperty != NULL)
	{
		ZEInt32 OptionValue = ((ZEMLProperty*)CurrentProperty)->GetValue().GetInt32();

		if(OptionValue == 1)
			ToggleApplicationPathOptions(true);
		else
			ToggleApplicationPathOptions(false);

		Form->cmbRelativeTo->setCurrentIndex(OptionValue);
	}
}

ZEMLNode* ZE3dsModelExporterOptionsDialogNew::GetOptions()
{
	CollectOptionsFromForm();
	return Options;
}

void ZE3dsModelExporterOptionsDialogNew::ToggleFileLogging(bool IsEnabled)
{
	Form->btnBrowseLogPath->setEnabled(IsEnabled);
	Form->txtLogFilePath->setEnabled(IsEnabled);
	Form->ckbFileLoggingEnabled->setChecked(IsEnabled);
}

void ZE3dsModelExporterOptionsDialogNew::ToggleApplicationPathOptions(bool IsEnabled)
{
	Form->lblApplicationPath->setVisible(IsEnabled);
	Form->txtApplicationFolder->setVisible(IsEnabled);
	Form->btnBrowseApplicationFolder->setVisible(IsEnabled);
}

void ZE3dsModelExporterOptionsDialogNew::ShowEngineDirectoryDialog()
{
	QString SelectedDirectory = QFileDialog::getExistingDirectory(this, QString("Select Directory"), Form->txtEngineWorkingDirectory->text());

	if(SelectedDirectory.length() == 0)
		return;

	Form->txtEngineWorkingDirectory->setText(SelectedDirectory);
}

void ZE3dsModelExporterOptionsDialogNew::ShowLoggingFilePathDialog()
{
	QString SelectedDirectory = QFileDialog::getExistingDirectory(this, QString("Select Directory"), Form->txtEngineWorkingDirectory->text());

	if(SelectedDirectory.length() == 0)
		return;

	Form->txtLogFilePath->setText(SelectedDirectory);
}

void ZE3dsModelExporterOptionsDialogNew::ShowApplicationPathDialog()
{
	QString SelectedDirectory = QFileDialog::getExistingDirectory(this, QString("Select Directory"), Form->txtApplicationFolder->text());

	if(SelectedDirectory.length() == 0)
		return;

	Form->txtApplicationFolder->setText(SelectedDirectory);
}

void ZE3dsModelExporterOptionsDialogNew::SetFileLoggingEnabled(int CheckBoxState)
{
	if(CheckBoxState == Qt::CheckState::Checked)
		ToggleFileLogging(true);
	else
		ToggleFileLogging(false);
}

void ZE3dsModelExporterOptionsDialogNew::SetResourceCopyEnabled(int CheckBoxState)
{

}

void ZE3dsModelExporterOptionsDialogNew::SetApplicationPathOptionsVisibility(int Index)
{
	if(Index == 1)
		ToggleApplicationPathOptions(true);
	else
		ToggleApplicationPathOptions(false);
}

void ZE3dsModelExporterOptionsDialogNew::CollectOptionsFromForm()
{
	if(Options == NULL)
	{
		Options = new ZEMLNode("Options");
		Options->AddProperty("ZinekEngineWorkingDirectory", ZEVariant(ZEString((const char*)Form->txtEngineWorkingDirectory->text().toLatin1())));
		Options->AddProperty("IsFileLoggingEnabled", ZEVariant(Form->ckbFileLoggingEnabled->isChecked()));
		Options->AddProperty("LogFilePath", ZEVariant(ZEString((const char*)Form->txtLogFilePath->text().toLatin1())));
		Options->AddProperty("RelativeTo", ZEVariant((ZEInt32)Form->cmbRelativeTo->currentIndex()));
		Options->AddProperty("ApplicationPath", ZEVariant(ZEString((const char*)Form->txtApplicationFolder->text().toLatin1())));
		Options->AddProperty("IsResourceCopyingEnabled", ZEVariant(Form->ckbCopyResources->isChecked()));
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

		if(Options->GetProperty("RelativeTo") != NULL)
			((ZEMLProperty*)(Options->GetProperty("RelativeTo")))->SetValue(ZEVariant((ZEInt32)Form->cmbRelativeTo->currentIndex()));
		else
			Options->AddProperty("RelativeTo", ZEVariant((ZEInt32)Form->cmbRelativeTo->currentIndex()));

		if(Options->GetProperty("ApplicationPath") != NULL)
			((ZEMLProperty*)(Options->GetProperty("ApplicationPath")))->SetValue(ZEVariant(ZEString((const char*)Form->txtApplicationFolder->text().toLatin1())));
		else
			Options->AddProperty("ApplicationPath", ZEVariant(ZEString((const char*)Form->txtApplicationFolder->text().toLatin1())));

		if(Options->GetProperty("IsResourceCopyingEnabled") != NULL)
			((ZEMLProperty*)(Options->GetProperty("IsResourceCopyingEnabled")))->SetValue(ZEVariant(Form->ckbCopyResources->isChecked()));
		else
			Options->AddProperty("IsResourceCopyingEnabled", ZEVariant(Form->ckbCopyResources->isChecked()));
	}
}
