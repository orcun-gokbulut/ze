//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSHCompileOptionsWindow.cpp
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

#include "ZEDSHCompileOptionsWindow.h"
#include "ui_ZEDSHCompileOptionsWindow.h"

#include <QtGui/qdialog.h>
#include <QtGui/qtextedit.h>
#include <QtGui/qpushbutton.h>
#include <QtGui/qdialogbuttonbox.h>

void ZEDSHCompileOptionsWindow::btnAddDefinition_OnClick()
{
	ZEGRShaderDefinition Definition;
	Definition.Name = "New Definition";
	Definition.Value = ZEString::Empty;

	Options.Definitions.AddByRef(Definition);

	QListWidgetItem* Item = new QListWidgetItem(Form->DefinitionList);
	Item->setText("New Definition");
	Item->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
}

void ZEDSHCompileOptionsWindow::btnRemoveDefinition_OnClick()
{
	if(Form->DefinitionList->currentItem() == NULL)
		return;

	Options.Definitions.Remove(Form->DefinitionList->currentRow());

	delete Form->DefinitionList->currentItem();
}

void ZEDSHCompileOptionsWindow::btnAddIncludeDirection_OnClick()
{
	Options.IncludeDirectories.AddByRef("New Include Dir");

	QListWidgetItem* Item = new QListWidgetItem(Form->IncludeDirList);
	Item->setText("New Include Dir");
	Item->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
}

void ZEDSHCompileOptionsWindow::btnRemoveIncludeDirection_OnClick()
{
	if(Form->IncludeDirList->currentItem() == NULL)
		return;

	Options.IncludeDirectories.Remove(Form->IncludeDirList->currentRow());

	delete Form->IncludeDirList->currentItem();
}

void ZEDSHCompileOptionsWindow::DefinitionItem_OnDoubleClick(QListWidgetItem* item)
{
	QLabel* lblDefinitionName = new QLabel("Definition Name");
	QLineEdit* txtDefinitionName = new QLineEdit();
	QTextEdit* txtDefinitionValue = new QTextEdit();

	ZEGRShaderDefinition& Definition = Options.Definitions[Form->DefinitionList->currentRow()];

	txtDefinitionName->setText(Definition.Name.ToCString());
	txtDefinitionValue->setText(Definition.Value.ToCString());

	QDialog Dialog;
	Dialog.setWindowTitle("Edit Definition");

	QDialogButtonBox* btnSaveClose = new QDialogButtonBox(&Dialog);
	btnSaveClose->setOrientation(Qt::Horizontal);
	btnSaveClose->setStandardButtons(QDialogButtonBox::Save | QDialogButtonBox::Close);

	QGridLayout* GridLayout = new QGridLayout();

	GridLayout->addWidget(lblDefinitionName, 0, 0, Qt::AlignLeft | Qt::AlignTop);
	GridLayout->addWidget(txtDefinitionName, 0, 1);
	GridLayout->addWidget(txtDefinitionValue, 1, 0, 1, 2);
	GridLayout->addWidget(btnSaveClose, 2, 1);

	Dialog.setLayout(GridLayout);

	connect(btnSaveClose, SIGNAL(accepted()), &Dialog, SLOT(accept()));
	connect(btnSaveClose, SIGNAL(rejected()), &Dialog, SLOT(reject()));

	if(Dialog.exec() == QDialog::Accepted)
	{
		item->setText(txtDefinitionName->text());

		Definition.Name = txtDefinitionName->text().toLocal8Bit().begin();
		Definition.Value = txtDefinitionValue->toPlainText().toLocal8Bit().begin();
	}
}

void ZEDSHCompileOptionsWindow::IncludeDirectoryItem_OnDoubleClick(QListWidgetItem* item)
{
	QLabel* lblIncludeDirectory = new QLabel("Include Directory");
	QLineEdit* txtIncludeDirectory = new QLineEdit();

	ZEString& IncludeDirectory = Options.IncludeDirectories[Form->IncludeDirList->currentRow()];

	txtIncludeDirectory->setText(IncludeDirectory.ToCString());

	QDialog Dialog;
	Dialog.setWindowTitle("Edit Include Directory");

	QDialogButtonBox* btnSaveClose = new QDialogButtonBox(&Dialog);
	btnSaveClose->setOrientation(Qt::Horizontal);
	btnSaveClose->setStandardButtons(QDialogButtonBox::Save | QDialogButtonBox::Close);

	QGridLayout* gridLayout = new QGridLayout();
	gridLayout->addWidget(lblIncludeDirectory, 0, 0, Qt::AlignLeft | Qt::AlignTop);
	gridLayout->addWidget(txtIncludeDirectory, 0, 1);
	gridLayout->addWidget(btnSaveClose, 1, 1);

	Dialog.setLayout(gridLayout);

	connect(btnSaveClose, SIGNAL(accepted()), &Dialog, SLOT(accept()));
	connect(btnSaveClose, SIGNAL(rejected()), &Dialog, SLOT(reject()));

	if(Dialog.exec() == QDialog::Accepted)
	{
		item->setText(txtIncludeDirectory->text());

		IncludeDirectory = txtIncludeDirectory->text().toLocal8Bit().begin();
	}
}

void ZEDSHCompileOptionsWindow::SetOptions(const ZEGRShaderCompileOptions& ParamOptions)
{
	Form->IsDebug->setChecked(ParamOptions.Debug);
	Form->EntryPointValue->setText(ParamOptions.EntryPoint.ToCString());
	Form->FilenameValue->setText(ParamOptions.FileName.ToCString());
	Form->ModelValue->setCurrentIndex(ParamOptions.Model);
	Form->TypeValue->setCurrentIndex(ParamOptions.Type);
	Form->OptimizationLevelValue->setCurrentIndex(ParamOptions.OptimizationLevel - 1);

	Form->DefinitionList->clear();
	ZESize size = ParamOptions.Definitions.GetCount();
	for (int i = 0; i < size; i++)
	{
		QListWidgetItem* Item = new QListWidgetItem(Form->DefinitionList);
		Item->setText(ParamOptions.Definitions[i].Name.ToCString());
		Item->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
	}

	Form->IncludeDirList->clear();
	size = ParamOptions.IncludeDirectories.GetCount();
	for (int i = 0; i < size; i++)
	{
		QListWidgetItem* Item = new QListWidgetItem(Form->IncludeDirList);
		Item->setText(ParamOptions.IncludeDirectories[i].ToCString());
		Item->setFlags(Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled);
	}

	Options = ParamOptions;
}

const ZEGRShaderCompileOptions& ZEDSHCompileOptionsWindow::GetOptions()
{
	Options.Debug = Form->IsDebug->isChecked();
	Options.EntryPoint = Form->EntryPointValue->text().toLocal8Bit().begin();
	Options.FileName = Form->FilenameValue->text().toLocal8Bit().begin();
	Options.Model = (ZEGRShaderModel)Form->ModelValue->currentIndex();
	Options.Type = (ZEGRShaderType)Form->TypeValue->currentIndex();
	Options.OptimizationLevel = Form->OptimizationLevelValue->currentIndex() + 1;

	return Options;
}

ZEDSHCompileOptionsWindow::ZEDSHCompileOptionsWindow(QWidget* parent) : QDockWidget(parent)
{
	Form = new Ui_ZEDSHCompileOptionsWindow();
	Form->setupUi(this);

	this->setWindowTitle("Compile Options");

	this->setWidget(Form->scrollArea);

	connect(Form->btnAddDefinition,		SIGNAL(clicked()), this, SLOT(btnAddDefinition_OnClick()));
	connect(Form->btnRemoveDefinition,	SIGNAL(clicked()), this, SLOT(btnRemoveDefinition_OnClick()));

	connect(Form->btnAddIncludeDir,		SIGNAL(clicked()), this, SLOT(btnAddIncludeDirection_OnClick()));
	connect(Form->btnRemoveIncludeDir,	SIGNAL(clicked()), this, SLOT(btnRemoveIncludeDirection_OnClick()));

	connect(Form->DefinitionList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(DefinitionItem_OnDoubleClick(QListWidgetItem*)));
	connect(Form->IncludeDirList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(IncludeDirectoryItem_OnDoubleClick(QListWidgetItem*)));

}
