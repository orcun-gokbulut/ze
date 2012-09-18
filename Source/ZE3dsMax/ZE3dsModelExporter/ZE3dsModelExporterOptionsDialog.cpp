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

	ZEArray<ZEMLItem*> CurrentProperties = Options->GetProperties("ZinekEngineWorkingDirectory");
	if(CurrentProperties.GetCount() != 0)
		Form->txtEngineWorkingDirectory->setText(((ZEMLProperty*)CurrentProperties[0])->GetValue().GetString().ToCString());

	CurrentProperties = Options->GetProperties("LogFilePath");
	if(CurrentProperties.GetCount() != 0)
		Form->txtLogFilePath->setText(((ZEMLProperty*)CurrentProperties[0])->GetValue().GetString().ToCString());

	CurrentProperties = Options->GetProperties("ApplicationPath");
	if(CurrentProperties.GetCount() != 0)
		Form->txtApplicationFolder->setText(((ZEMLProperty*)CurrentProperties[0])->GetValue().GetString().ToCString());

	CurrentProperties = Options->GetProperties("IsFileLoggingEnabled");
	if(CurrentProperties.GetCount() != 0)
		ToggleFileLogging(((ZEMLProperty*)CurrentProperties[0])->GetValue().GetBoolean());

	CurrentProperties = Options->GetProperties("IsResourceCopyingEnabled");
	if(CurrentProperties.GetCount() != 0)
		Form->ckbCopyResources->setChecked(((ZEMLProperty*)CurrentProperties[0])->GetValue().GetBoolean());

	CurrentProperties = Options->GetProperties("RelativeTo");
	if(CurrentProperties.GetCount() != 0)
	{
		ZEInt32 OptionValue = ((ZEMLProperty*)CurrentProperties[0])->GetValue().GetInt32();

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
		Options->AddProperty("ZinekEngineWorkingDirectory", ZEVariant(ZEString((const char*)Form->txtEngineWorkingDirectory->text().toUtf8())));
		Options->AddProperty("IsFileLoggingEnabled", ZEVariant(Form->ckbFileLoggingEnabled->isChecked()));
		Options->AddProperty("LogFilePath", ZEVariant(ZEString((const char*)Form->txtLogFilePath->text().toUtf8())));
		Options->AddProperty("RelativeTo", ZEVariant((ZEInt32)Form->cmbRelativeTo->currentIndex()));
		Options->AddProperty("ApplicationPath", ZEVariant(ZEString((const char*)Form->txtApplicationFolder->text().toUtf8())));
		Options->AddProperty("IsResourceCopyingEnabled", ZEVariant(Form->ckbCopyResources->isChecked()));
	}
	else
	{
		if(Options->GetProperties("ZinekEngineWorkingDirectory").GetCount() != 0)
			((ZEMLProperty*)(Options->GetProperties("ZinekEngineWorkingDirectory")[0]))->SetValue(ZEVariant(ZEString((const char*)Form->txtEngineWorkingDirectory->text().toUtf8())));
		else
			Options->AddProperty("ZinekEngineWorkingDirectory", ZEVariant(ZEString((const char*)Form->txtEngineWorkingDirectory->text().toUtf8())));

		if(Options->GetProperties("IsFileLoggingEnabled").GetCount() != 0)
			((ZEMLProperty*)(Options->GetProperties("IsFileLoggingEnabled")[0]))->SetValue(ZEVariant(Form->ckbFileLoggingEnabled->isChecked()));
		else
			Options->AddProperty("IsFileLoggingEnabled", ZEVariant(Form->ckbFileLoggingEnabled->isChecked()));

		if(Options->GetProperties("LogFilePath").GetCount() != 0)
			((ZEMLProperty*)(Options->GetProperties("LogFilePath")[0]))->SetValue(ZEVariant(ZEString((const char*)Form->txtLogFilePath->text().toUtf8())));
		else
			Options->AddProperty("LogFilePath", ZEVariant(ZEString((const char*)Form->txtLogFilePath->text().toUtf8())));

		if(Options->GetProperties("RelativeTo").GetCount() != 0)
			((ZEMLProperty*)(Options->GetProperties("RelativeTo")[0]))->SetValue(ZEVariant((ZEInt32)Form->cmbRelativeTo->currentIndex()));
		else
			Options->AddProperty("RelativeTo", ZEVariant((ZEInt32)Form->cmbRelativeTo->currentIndex()));

		if(Options->GetProperties("ApplicationPath").GetCount() != 0)
			((ZEMLProperty*)(Options->GetProperties("ApplicationPath")[0]))->SetValue(ZEVariant(ZEString((const char*)Form->txtApplicationFolder->text().toUtf8())));
		else
			Options->AddProperty("ApplicationPath", ZEVariant(ZEString((const char*)Form->txtApplicationFolder->text().toUtf8())));

		if(Options->GetProperties("IsResourceCopyingEnabled").GetCount() != 0)
			((ZEMLProperty*)(Options->GetProperties("IsResourceCopyingEnabled")[0]))->SetValue(ZEVariant(Form->ckbCopyResources->isChecked()));
		else
			Options->AddProperty("IsResourceCopyingEnabled", ZEVariant(Form->ckbCopyResources->isChecked()));
	}
}
