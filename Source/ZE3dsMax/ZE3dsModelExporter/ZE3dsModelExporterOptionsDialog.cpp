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
#include "ZEToolComponents\ZEResourceConfigurationWidget\ZEResourceConfigurationWidget.h"

ZE3dsModelExporterOptionsDialogNew::ZE3dsModelExporterOptionsDialogNew(QWidget* Parent) : QDialog(Parent)
{
	Form = new Ui::ZEModelExporterOptionsDialogUI();
	Form->setupUi(this);

	connect(Form->btnBrowseEngineDir, SIGNAL(clicked()), this, SLOT(ShowEngineDirectoryDialog()));
	connect(Form->btnBrowseLogPath, SIGNAL(clicked()), this, SLOT(ShowLoggingFilePathDialog()));
	connect(Form->btnBrowseApplicationFolder, SIGNAL(clicked()), this, SLOT(ShowApplicationPathDialog()));
	connect(Form->ckbFileLoggingEnabled, SIGNAL(stateChanged(int)), this, SLOT(SetFileLoggingEnabled(int)));
	connect(Form->cmbRelativeTo, SIGNAL(currentIndexChanged(int)), this, SLOT(SetApplicationPathOptionsVisibility(int)));
	connect(Form->ckbCopyResources, SIGNAL(stateChanged(int)), this, SLOT(SetResourceCopyEnabled(int)));
	connect(Form->btnNewAnimation, SIGNAL(clicked()), this, SLOT(AddAnimation()));
	connect(Form->btnDeleteAnimation, SIGNAL(clicked()), this, SLOT(RemoveAnimation()));
	connect(Form->grpExportBones, SIGNAL(toggled(bool)), this, SLOT(SetExportBonesEnabled(bool)));
	connect(Form->grpExportMeshes, SIGNAL(toggled(bool)), this, SLOT(SetExportMeshesEnabled(bool)));
	Form->AnimationTreeWidget->setSortingEnabled(false);

	SetApplicationPathOptionsVisibility(Form->cmbRelativeTo->currentIndex());
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

	CurrentProperty = Options->GetProperty("IsBoneExportEnabled");
	if(CurrentProperty != NULL)
		Form->grpExportBones->setChecked(((ZEMLProperty*)CurrentProperty)->GetValue().GetBoolean());

	CurrentProperty = Options->GetProperty("IsBonePhysicalBodyExportEnabled");
	if(CurrentProperty != NULL)
		Form->ckbExportBonePhysicalBodies->setChecked(((ZEMLProperty*)CurrentProperty)->GetValue().GetBoolean());

	CurrentProperty = Options->GetProperty("IsMeshExportEnabled");
	if(CurrentProperty != NULL)
		Form->grpExportMeshes->setChecked(((ZEMLProperty*)CurrentProperty)->GetValue().GetBoolean());

	CurrentProperty = Options->GetProperty("IsMeshPhysicalBodyExportEnabled");
	if(CurrentProperty != NULL)
		Form->ckbExportMeshPhysicalBodies->setChecked(((ZEMLProperty*)CurrentProperty)->GetValue().GetBoolean());

	CurrentProperty = Options->GetProperty("IsAnimationExportEnabled");
	if (CurrentProperty != NULL)
		Form->grpExportAnimations->setChecked(((ZEMLProperty*)CurrentProperty)->GetValue().GetBoolean());


	if (Options->GetSubNodes("Animations").GetCount() > 0)
	{
		CurrentProperty = Options->GetSubNodes("Animations").GetFirstItem();

		ZEArray<ZEMLNode*> Animations = ((ZEMLNode*)CurrentProperty)->GetSubNodes("Animation");

		for (ZESize I = 0; I < Animations.GetCount(); I++)
		{
			ZEInt ItemCount = Form->AnimationTreeWidget->topLevelItemCount();

			QStringList List;
			List << ((ZEMLProperty*)Animations[I]->GetProperty("Name"))->GetValue().GetString().ToCString() 
				 << ((ZEMLProperty*)Animations[I]->GetProperty("StartFrame"))->GetValue().GetString().ToCString() 
				 << ((ZEMLProperty*)Animations[I]->GetProperty("EndFrame"))->GetValue().GetString().ToCString();

			QTreeWidgetItem* Item = new QTreeWidgetItem(List);
			Item->setFlags(Item->flags() | Qt::ItemFlag::ItemIsEditable);
			Form->AnimationTreeWidget->insertTopLevelItem(ItemCount, Item);
		}
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

void ZE3dsModelExporterOptionsDialogNew::SetExportBonesEnabled(bool IsChecked)
{
	if(!IsChecked)
		Form->ckbExportBonePhysicalBodies->setChecked(false);
}

void ZE3dsModelExporterOptionsDialogNew::SetExportMeshesEnabled(bool IsChecked)
{
	if(!IsChecked)
		Form->ckbExportMeshPhysicalBodies->setChecked(false);
}

void ZE3dsModelExporterOptionsDialogNew::AddAnimation()
{

	ZEInt ItemCount = Form->AnimationTreeWidget->topLevelItemCount();

	for (ZEInt I = 0; I < ItemCount; I++)
		if (Form->AnimationTreeWidget->topLevelItem(I)->text(0) == "Default")
			return;

	QStringList List;
	List << "Default" << "0" << "0";
	QTreeWidgetItem* Item = new QTreeWidgetItem(List);
	Item->setFlags(Item->flags() | Qt::ItemFlag::ItemIsEditable);
	Form->AnimationTreeWidget->insertTopLevelItem(ItemCount, Item);
	Form->AnimationTreeWidget->editItem(Item, 0);
}

void ZE3dsModelExporterOptionsDialogNew::RemoveAnimation()
{
	if(Form->AnimationTreeWidget->currentItem() == NULL)
		return;

	ZEInt IndexOfItem = Form->AnimationTreeWidget->indexOfTopLevelItem(Form->AnimationTreeWidget->currentItem());
	Form->AnimationTreeWidget->takeTopLevelItem(IndexOfItem);
}

void ZE3dsModelExporterOptionsDialogNew::CollectOptionsFromForm()
{
	if(Options == NULL)
	{
		Options = new ZEMLNode("Options");
		Options->AddProperty("ZinekEngineWorkingDirectory", (const char*)Form->txtEngineWorkingDirectory->text().toUtf8());
		Options->AddProperty("IsFileLoggingEnabled", Form->ckbFileLoggingEnabled->isChecked());
		Options->AddProperty("LogFilePath", (const char*)Form->txtLogFilePath->text().toUtf8());
		Options->AddProperty("RelativeTo", (ZEInt32)Form->cmbRelativeTo->currentIndex());
		Options->AddProperty("ApplicationPath", (const char*)Form->txtApplicationFolder->text().toUtf8());
		Options->AddProperty("IsResourceCopyingEnabled", Form->ckbCopyResources->isChecked());
		Options->AddProperty("IsBoneExportEnabled", Form->grpExportBones->isChecked());
		Options->AddProperty("IsBonePhysicalBodyExportEnabled", Form->ckbExportBonePhysicalBodies->isChecked());
		Options->AddProperty("IsMeshExportEnabled", Form->grpExportMeshes->isChecked());
		Options->AddProperty("IsMeshPhysicalBodyExportEnabled", Form->ckbExportMeshPhysicalBodies->isChecked());
		Options->AddProperty("IsAnimationExportEnabled", Form->grpExportAnimations->isChecked());
		
		ZEInt ItemCount = Form->AnimationTreeWidget->topLevelItemCount();

		if (ItemCount > 0)
		{
			ZEMLNode* AnimationsNode = Options->AddSubNode("Animations");

			for (ZEInt I = 0; I < ItemCount; I++)
			{
				ZEMLNode* Animation = AnimationsNode->AddSubNode("Animation");
				Animation->AddProperty("Name", (const char*)Form->AnimationTreeWidget->topLevelItem(I)->text(0).toUtf8());
				Animation->AddProperty("StartFrame", (const char*)Form->AnimationTreeWidget->topLevelItem(I)->text(1).toUtf8());
				Animation->AddProperty("EndFrame", (const char*)Form->AnimationTreeWidget->topLevelItem(I)->text(2).toUtf8());
			}
		}
	}
	else
	{
		if (Options->GetProperty("ZinekEngineWorkingDirectory") !=  NULL)
			((ZEMLProperty*)(Options->GetProperty("ZinekEngineWorkingDirectory")))->SetValue((const char*)Form->txtEngineWorkingDirectory->text().toUtf8());
		else
			Options->AddProperty("ZinekEngineWorkingDirectory", (const char*)Form->txtEngineWorkingDirectory->text().toUtf8());

		if (Options->GetProperty("IsFileLoggingEnabled") != NULL)
			((ZEMLProperty*)(Options->GetProperty("IsFileLoggingEnabled")))->SetValue(Form->ckbFileLoggingEnabled->isChecked());
		else
			Options->AddProperty("IsFileLoggingEnabled", Form->ckbFileLoggingEnabled->isChecked());

		if (Options->GetProperty("LogFilePath") != NULL)
			((ZEMLProperty*)(Options->GetProperty("LogFilePath")))->SetValue((const char*)Form->txtLogFilePath->text().toUtf8());
		else
			Options->AddProperty("LogFilePath", (const char*)Form->txtLogFilePath->text().toUtf8());

		if (Options->GetProperty("RelativeTo") != NULL)
			((ZEMLProperty*)(Options->GetProperty("RelativeTo")))->SetValue((ZEInt32)Form->cmbRelativeTo->currentIndex());
		else
			Options->AddProperty("RelativeTo", (ZEInt32)Form->cmbRelativeTo->currentIndex());

		if (Options->GetProperty("ApplicationPath") != NULL)
			((ZEMLProperty*)(Options->GetProperty("ApplicationPath")))->SetValue((const char*)Form->txtApplicationFolder->text().toUtf8());
		else
			Options->AddProperty("ApplicationPath", (const char*)Form->txtApplicationFolder->text().toUtf8());

		if (Options->GetProperty("IsResourceCopyingEnabled") != NULL)
			((ZEMLProperty*)(Options->GetProperty("IsResourceCopyingEnabled")))->SetValue(Form->ckbCopyResources->isChecked());
		else
			Options->AddProperty("IsResourceCopyingEnabled", Form->ckbCopyResources->isChecked());

		if (Options->GetProperty("IsBoneExportEnabled") != NULL)
			((ZEMLProperty*)(Options->GetProperty("IsBoneExportEnabled")))->SetValue(Form->grpExportBones->isChecked());
		else
			Options->AddProperty("IsBoneExportEnabled", Form->grpExportBones->isChecked());

		if (Options->GetProperty("IsBonePhysicalBodyExportEnabled") != NULL)
			((ZEMLProperty*)(Options->GetProperty("IsBonePhysicalBodyExportEnabled")))->SetValue(Form->ckbExportBonePhysicalBodies->isChecked());
		else
			Options->AddProperty("IsBonePhysicalBodyExportEnabled", Form->ckbExportBonePhysicalBodies->isChecked());

		if (Options->GetProperty("IsMeshExportEnabled") != NULL)
			((ZEMLProperty*)(Options->GetProperty("IsMeshExportEnabled")))->SetValue(Form->grpExportMeshes->isChecked());
		else
			Options->AddProperty("IsMeshExportEnabled", Form->grpExportMeshes->isChecked());

		if (Options->GetProperty("IsMeshPhysicalBodyExportEnabled") != NULL)
			((ZEMLProperty*)(Options->GetProperty("IsMeshPhysicalBodyExportEnabled")))->SetValue(Form->ckbExportMeshPhysicalBodies->isChecked());
		else
			Options->AddProperty("IsMeshPhysicalBodyExportEnabled", Form->ckbExportMeshPhysicalBodies->isChecked());

		if (Options->GetProperty("IsAnimationExportEnabled") != NULL)
			((ZEMLProperty*)(Options->GetProperty("IsAnimationExportEnabled")))->SetValue(Form->grpExportAnimations->isChecked());
		else
			Options->AddProperty("IsAnimationExportEnabled", Form->grpExportAnimations->isChecked());

		if (Options->GetSubNodes("Animations").GetCount() != 0)
			Options->RemoveSubNode(Options->GetSubNodes("Animations").GetFirstItem());

		ZEInt ItemCount = Form->AnimationTreeWidget->topLevelItemCount();

		if (ItemCount > 0)
		{
			ZEMLNode* AnimationsNode = Options->AddSubNode("Animations");

			for (ZEInt I = 0; I < ItemCount; I++)
			{
				ZEMLNode* Animation = AnimationsNode->AddSubNode("Animation");
				Animation->AddProperty("Name", (const char*)Form->AnimationTreeWidget->topLevelItem(I)->text(0).toUtf8());
				Animation->AddProperty("StartFrame", (const char*)Form->AnimationTreeWidget->topLevelItem(I)->text(1).toUtf8());
				Animation->AddProperty("EndFrame", (const char*)Form->AnimationTreeWidget->topLevelItem(I)->text(2).toUtf8());
			}
		}
	}
}
