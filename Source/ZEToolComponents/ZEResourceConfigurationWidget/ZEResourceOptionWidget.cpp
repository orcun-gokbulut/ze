//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEResourceOptionWidget.cpp
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

#include "ZEResourceOptionWidget.h"
#include "QtGui\qfiledialog.h"
#include "zeresourceoption.h"
#include "ui_ZEResourceOptionWidget.h"

ZEResourceOptionWidget::ZEResourceOptionWidget(QWidget* Parent) : QWidget(Parent)
{
	Form = new Ui::ZEResourceOptionUI();
	Form->setupUi(this);

	Form->cmbAction->addItem("Do Nothing", QVariant((ZEInt)ZE_ROAA_DO_NOTHING));
	Form->cmbAction->addItem("Overwrite", QVariant((ZEInt)ZE_ROAA_COPY_OVERWRITE));
	Form->cmbAction->addItem("Copy If Newer", QVariant((ZEInt)ZE_ROAA_COPY_IF_NEWER));
	Form->cmbAction->setCurrentIndex(0);

	connect(Form->chkAutoGenerated, SIGNAL(stateChanged(int)), this, SLOT(AutoGeneratedChanged()));
	connect(Form->btnBrowsePhsyicalPath, SIGNAL(clicked()), this, SLOT(ShowPhysicalPathDialog()));
	connect(Form->btnBrowseExportPath, SIGNAL(clicked()), this, SLOT(ShowExportPathDialog()));
 	connect(Form->cmbAction, SIGNAL(currentIndexChanged(int)), this, SLOT(EmitOptionChanged()));
	connect(Form->txtPhsyicalPath, SIGNAL(editingFinished()), this, SLOT(EmitOptionChanged()));
	connect(Form->txtExportPath, SIGNAL(editingFinished()), this, SLOT(EmitOptionChanged()));
}

ZEResourceOptionWidget::~ZEResourceOptionWidget()
{
	delete Form;
	Form = NULL;
}
void ZEResourceOptionWidget::SetPhysicalPath(const ZEString& Path)
{
	if(Path.GetLength() == 0)
		Form->txtPhsyicalPath->setText("");

	Form->txtPhsyicalPath->setText(Path.ToCString());
	Form->txtPhsyicalPath->setToolTip(Path.ToCString());
}

ZEString ZEResourceOptionWidget::GetPhysicalPath()
{
	return ZEString((const char*)Form->txtPhsyicalPath->text().toLatin1());
}

void ZEResourceOptionWidget::SetExportPath(const ZEString& Path)
{
	if(Path.GetLength() == 0)
		Form->txtExportPath->setText("");

	Form->txtExportPath->setText(Path.ToCString());
	Form->txtExportPath->setToolTip(Path.ToCString());
}

ZEString ZEResourceOptionWidget::GetExportPath()
{
	return ZEString((const char*)Form->txtExportPath->text().toLatin1());
}

void ZEResourceOptionWidget::SetAutoGenerated(bool IsAutoGenerated)
{
	disconnect(Form->chkAutoGenerated, SIGNAL(stateChanged(int)), this, SLOT(AutoGeneratedChanged()));
	Form->chkAutoGenerated->setChecked(IsAutoGenerated);
	Form->btnBrowsePhsyicalPath->setEnabled(!IsAutoGenerated);
	Form->txtPhsyicalPath->setEnabled(!IsAutoGenerated);
	connect(Form->chkAutoGenerated, SIGNAL(stateChanged(int)), this, SLOT(AutoGeneratedChanged()));
}

bool ZEResourceOptionWidget::GetAutoGenerated() const
{
	return Form->chkAutoGenerated->isChecked();
}

void ZEResourceOptionWidget::SetActionOnAvailable(ZEResourceOptionAvailableAction Action)
{
	disconnect(Form->cmbAction, SIGNAL(currentIndexChanged(int)), this, SLOT(EmitOptionChanged()));
	Form->cmbAction->setCurrentIndex((ZEInt)Action);
	connect(Form->cmbAction, SIGNAL(currentIndexChanged(int)), this, SLOT(EmitOptionChanged()));
}

ZEResourceOptionAvailableAction	ZEResourceOptionWidget::GetActionOnAvailable() const
{
	return (ZEResourceOptionAvailableAction)(Form->cmbAction->itemData(Form->cmbAction->currentIndex()).toInt());
}

void ZEResourceOptionWidget::SetOptions(ZEResourceOption* Options)
{
	SetPhysicalPath(Options->PhysicalPath);
	SetExportPath(Options->ExportPath);
	SetActionOnAvailable(Options->Action);
	SetAutoGenerated(Options->IsAutoGenerated);
	Form->txtIdentifier->setText(Options->Identifier.ToCString());
}

void ZEResourceOptionWidget::AutoGeneratedChanged()
{
	SetAutoGenerated(Form->chkAutoGenerated->isChecked());
	EmitOptionChanged();
}

void ZEResourceOptionWidget::ShowPhysicalPathDialog()
{
	QString SelectedDirectory = QFileDialog::getOpenFileName(this, QString("Select Directory"), Form->txtPhsyicalPath->text());

	if(SelectedDirectory.length() == 0)
		return;

	SetPhysicalPath((const char*)SelectedDirectory.toLatin1());
	EmitOptionChanged();
}

void ZEResourceOptionWidget::ShowExportPathDialog()
{
	QString SelectedDirectory = QFileDialog::getExistingDirectory(this, QString("Select Directory"), Form->txtExportPath->text());

	if(SelectedDirectory.length() == 0)
		return;

	SetExportPath((const char*)SelectedDirectory.toLatin1());
	EmitOptionChanged();
}

void ZEResourceOptionWidget::EmitOptionChanged()
{
	emit OptionChanged(this);
}
