//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxModelExporterOptionsDialog.cpp
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

#include "ZE3dsMaxModelExporterOptionsDialog.h"

#include "ZEML\ZEMLProperty.h"
#include "ZEToolComponents\ZEResourceConfigurator\ZEResourceConfiguratorWidget.h"

#include <QFileDialog>

ZE3dsMaxModelExporterOptionsDialog::ZE3dsMaxModelExporterOptionsDialog(QWidget* Parent) : QDialog(Parent)
{
	Form = new Ui::ZE3dsMaxModelExporterOptionsDialogUI();
	Form->setupUi(this);

	Options = NULL;

	connect(Form->btnBrowseEngineDir, SIGNAL(clicked()), this, SLOT(ShowEngineDirectoryDialog()));
	connect(Form->btnBrowseLogPath, SIGNAL(clicked()), this, SLOT(ShowLoggingFilePathDialog()));
	connect(Form->ckbFileLoggingEnabled, SIGNAL(stateChanged(int)), this, SLOT(SetFileLoggingEnabled(int)));
	connect(Form->grpExportBones, SIGNAL(toggled(bool)), this, SLOT(SetExportBonesEnabled(bool)));
	connect(Form->grpExportMeshes, SIGNAL(toggled(bool)), this, SLOT(SetExportMeshesEnabled(bool)));
	connect(Form->btnNewAnimation, SIGNAL(clicked()), this, SLOT(AddAnimation()));
	connect(Form->btnDeleteAnimation, SIGNAL(clicked()), this, SLOT(RemoveAnimation()));
	Form->AnimationTreeWidget->setSortingEnabled(false);
}

void ZE3dsMaxModelExporterOptionsDialog::SetOptions(ZEMLNode* Options)
{
	if (Options == NULL)
		return;

	this->Options = Options;

	const ZEMLElement* CurrentProperty = NULL;
	CurrentProperty = Options->GetProperty("ZinekEngineWorkingDirectory");
	if (CurrentProperty != NULL)
		Form->txtEngineWorkingDirectory->setText(((ZEMLProperty*)CurrentProperty)->GetValue().GetString().ToCString());

	CurrentProperty = Options->GetProperty("LogFilePath");
	if (CurrentProperty != NULL)
		Form->txtLogFilePath->setText(((ZEMLProperty*)CurrentProperty)->GetValue().GetString().ToCString());

	CurrentProperty = Options->GetProperty("IsFileLoggingEnabled");
	if (CurrentProperty != NULL)
		ToggleFileLogging(((ZEMLProperty*)CurrentProperty)->GetValue().GetBoolean());

	CurrentProperty = Options->GetProperty("IsBoneExportEnabled");
	if (CurrentProperty != NULL)
		Form->grpExportBones->setChecked(((ZEMLProperty*)CurrentProperty)->GetValue().GetBoolean());

	CurrentProperty = Options->GetProperty("IsBonePhysicalBodyExportEnabled");
	if (CurrentProperty != NULL)
		Form->ckbExportBonePhysicalBodies->setChecked(((ZEMLProperty*)CurrentProperty)->GetValue().GetBoolean());

	CurrentProperty = Options->GetProperty("IsMeshExportEnabled");
	if (CurrentProperty != NULL)
		Form->grpExportMeshes->setChecked(((ZEMLProperty*)CurrentProperty)->GetValue().GetBoolean());

	CurrentProperty = Options->GetProperty("IsMeshPhysicalBodyExportEnabled");
	if (CurrentProperty != NULL)
		Form->ckbExportMeshPhysicalBodies->setChecked(((ZEMLProperty*)CurrentProperty)->GetValue().GetBoolean());

	CurrentProperty = Options->GetProperty("IsAnimationExportEnabled");
	if (CurrentProperty != NULL)
		Form->grpExportAnimations->setChecked(((ZEMLProperty*)CurrentProperty)->GetValue().GetBoolean());


	if (Options->GetNodes("Animations").GetCount() > 0)
	{
		CurrentProperty = Options->GetNodes("Animations").GetFirstItem();

		ZEArray<ZEMLNode*> Animations = ((ZEMLNode*)CurrentProperty)->GetNodes("Animation");

		for (ZESize I = 0; I < Animations.GetCount(); I++)
		{
			ZEInt32 ItemCount = Form->AnimationTreeWidget->topLevelItemCount();

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

ZEMLNode* ZE3dsMaxModelExporterOptionsDialog::GetOptions()
{
	CollectOptionsFromForm();
	return Options;
}

void ZE3dsMaxModelExporterOptionsDialog::ToggleFileLogging(bool IsEnabled)
{
	Form->btnBrowseLogPath->setEnabled(IsEnabled);
	Form->txtLogFilePath->setEnabled(IsEnabled);
	Form->ckbFileLoggingEnabled->setChecked(IsEnabled);
}

void ZE3dsMaxModelExporterOptionsDialog::ShowEngineDirectoryDialog()
{
	QString SelectedDirectory = QFileDialog::getExistingDirectory(this, QString("Select Directory"), Form->txtEngineWorkingDirectory->text());

	if (SelectedDirectory.length() == 0)
		return;

	Form->txtEngineWorkingDirectory->setText(SelectedDirectory);
}

void ZE3dsMaxModelExporterOptionsDialog::ShowLoggingFilePathDialog()
{
	QString SelectedDirectory = QFileDialog::getExistingDirectory(this, QString("Select Directory"), Form->txtEngineWorkingDirectory->text());

	if (SelectedDirectory.length() == 0)
		return;

	Form->txtLogFilePath->setText(SelectedDirectory);
}

void ZE3dsMaxModelExporterOptionsDialog::SetFileLoggingEnabled(int CheckBoxState)
{
	if (CheckBoxState == Qt::CheckState::Checked)
		ToggleFileLogging(true);
	else
		ToggleFileLogging(false);
}

void ZE3dsMaxModelExporterOptionsDialog::SetExportBonesEnabled(bool IsChecked)
{
	if (!IsChecked)
		Form->ckbExportBonePhysicalBodies->setChecked(false);
}

void ZE3dsMaxModelExporterOptionsDialog::SetExportMeshesEnabled(bool IsChecked)
{
	if (!IsChecked)
		Form->ckbExportMeshPhysicalBodies->setChecked(false);
}

void ZE3dsMaxModelExporterOptionsDialog::AddAnimation()
{

	ZEInt32 ItemCount = Form->AnimationTreeWidget->topLevelItemCount();

	for (ZEInt32 I = 0; I < ItemCount; I++)
		if (Form->AnimationTreeWidget->topLevelItem(I)->text(0) == "Default")
			return;

	QStringList List;
	List << "Default" << "0" << "0";
	QTreeWidgetItem* Item = new QTreeWidgetItem(List);
	Item->setFlags(Item->flags() | Qt::ItemFlag::ItemIsEditable);
	Form->AnimationTreeWidget->insertTopLevelItem(ItemCount, Item);
	Form->AnimationTreeWidget->editItem(Item, 0);
}

void ZE3dsMaxModelExporterOptionsDialog::RemoveAnimation()
{
	if (Form->AnimationTreeWidget->currentItem() == NULL)
		return;

	ZEInt32 IndexOfItem = Form->AnimationTreeWidget->indexOfTopLevelItem(Form->AnimationTreeWidget->currentItem());
	Form->AnimationTreeWidget->takeTopLevelItem(IndexOfItem);
}

void ZE3dsMaxModelExporterOptionsDialog::CollectOptionsFromForm()
{
	if (Options == NULL)
	{
		Options = new ZEMLNode("Options");
		Options->AddProperty("ZinekEngineWorkingDirectory")->SetString(Form->txtEngineWorkingDirectory->text().toUtf8().constData());
		Options->AddProperty("IsFileLoggingEnabled")->SetBool(Form->ckbFileLoggingEnabled->isChecked());
		Options->AddProperty("LogFilePath")->SetString(Form->txtLogFilePath->text().toUtf8().constData());
		Options->AddProperty("IsBoneExportEnabled")->SetBool(Form->grpExportBones->isChecked());
		Options->AddProperty("IsBonePhysicalBodyExportEnabled")->SetBool(Form->ckbExportBonePhysicalBodies->isChecked());
		Options->AddProperty("IsMeshExportEnabled")->SetBool(Form->grpExportMeshes->isChecked());
		Options->AddProperty("IsMeshPhysicalBodyExportEnabled")->SetBool(Form->ckbExportMeshPhysicalBodies->isChecked());
		Options->AddProperty("IsAnimationExportEnabled")->SetBool(Form->grpExportAnimations->isChecked());
		
		ZEInt32 ItemCount = Form->AnimationTreeWidget->topLevelItemCount();

		if (ItemCount > 0)
		{
			ZEMLNode* AnimationsNode = Options->AddNode("Animations");

			for (ZEInt32 I = 0; I < ItemCount; I++)
			{
				ZEMLNode* Animation = AnimationsNode->AddNode("Animation");
				Animation->AddProperty("Name")->SetString(Form->AnimationTreeWidget->topLevelItem(I)->text(0).toUtf8().constData());
				Animation->AddProperty("StartFrame")->SetString(Form->AnimationTreeWidget->topLevelItem(I)->text(1).toUtf8().constData());
				Animation->AddProperty("EndFrame")->SetString(Form->AnimationTreeWidget->topLevelItem(I)->text(2).toUtf8().constData());
			}
		}
	}
	else
	{
		if (Options->GetProperty("ZinekEngineWorkingDirectory") !=  NULL)
			((ZEMLProperty*)(Options->GetProperty("ZinekEngineWorkingDirectory")))->SetString(Form->txtEngineWorkingDirectory->text().toUtf8().constData());
		else
			Options->AddProperty("ZinekEngineWorkingDirectory")->SetString(Form->txtEngineWorkingDirectory->text().toUtf8().constData());

		if (Options->GetProperty("IsFileLoggingEnabled") != NULL)
			((ZEMLProperty*)(Options->GetProperty("IsFileLoggingEnabled")))->SetBool(Form->ckbFileLoggingEnabled->isChecked());
		else
			Options->AddProperty("IsFileLoggingEnabled")->SetBool(Form->ckbFileLoggingEnabled->isChecked());

		if (Options->GetProperty("LogFilePath") != NULL)
			((ZEMLProperty*)(Options->GetProperty("LogFilePath")))->SetString(Form->txtLogFilePath->text().toUtf8().constData());
		else
			Options->AddProperty("LogFilePath")->SetString(Form->txtLogFilePath->text().toUtf8().constData());

		if (Options->GetProperty("IsBoneExportEnabled") != NULL)
			((ZEMLProperty*)(Options->GetProperty("IsBoneExportEnabled")))->SetBool(Form->grpExportBones->isChecked());
		else
			Options->AddProperty("IsBoneExportEnabled")->SetBool(Form->grpExportBones->isChecked());

		if (Options->GetProperty("IsBonePhysicalBodyExportEnabled") != NULL)
			((ZEMLProperty*)(Options->GetProperty("IsBonePhysicalBodyExportEnabled")))->SetBool(Form->ckbExportBonePhysicalBodies->isChecked());
		else
			Options->AddProperty("IsBonePhysicalBodyExportEnabled")->SetBool(Form->ckbExportBonePhysicalBodies->isChecked());

		if (Options->GetProperty("IsMeshExportEnabled") != NULL)
			((ZEMLProperty*)(Options->GetProperty("IsMeshExportEnabled")))->SetBool(Form->grpExportMeshes->isChecked());
		else
			Options->AddProperty("IsMeshExportEnabled")->SetBool(Form->grpExportMeshes->isChecked());

		if (Options->GetProperty("IsMeshPhysicalBodyExportEnabled") != NULL)
			((ZEMLProperty*)(Options->GetProperty("IsMeshPhysicalBodyExportEnabled")))->SetBool(Form->ckbExportMeshPhysicalBodies->isChecked());
		else
			Options->AddProperty("IsMeshPhysicalBodyExportEnabled")->SetBool(Form->ckbExportMeshPhysicalBodies->isChecked());

		if (Options->GetProperty("IsAnimationExportEnabled") != NULL)
			((ZEMLProperty*)(Options->GetProperty("IsAnimationExportEnabled")))->SetBool(Form->grpExportAnimations->isChecked());
		else
			Options->AddProperty("IsAnimationExportEnabled")->SetBool(Form->grpExportAnimations->isChecked());

		if (Options->GetNodes("Animations").GetCount() != 0)
			Options->RemoveElement(Options->GetNodes("Animations").GetFirstItem());

		ZEInt32 ItemCount = Form->AnimationTreeWidget->topLevelItemCount();

		if (ItemCount > 0)
		{
			ZEMLNode* AnimationsNode = Options->AddNode("Animations");

			for (ZEInt32 I = 0; I < ItemCount; I++)
			{
				ZEMLNode* Animation = AnimationsNode->AddNode("Animation");
				Animation->AddProperty("Name")->SetString(Form->AnimationTreeWidget->topLevelItem(I)->text(0).toUtf8().constData());
				Animation->AddProperty("StartFrame")->SetString(Form->AnimationTreeWidget->topLevelItem(I)->text(1).toUtf8().constData());
				Animation->AddProperty("EndFrame")->SetString(Form->AnimationTreeWidget->topLevelItem(I)->text(2).toUtf8().constData());
			}
		}
	}
}
