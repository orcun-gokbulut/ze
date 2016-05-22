//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDConsoleWidget.cpp
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

#pragma once

#include "ZEDConsoleWidget.h"

#include "ui_ZEDConsoleWidget.h"
#include <qevent.h>
#include "QFileDialog"
#include "QMessageBox"
#include "QTextStream"
#include "QDialogButtonBox"
#include "ZEDS\ZEString.h"
#include "ZEDS\ZEArray.h"

void ZEDConsoleWidget::Execute()
{
	if (Form->txtCommand->text().isEmpty())
		return;

	Form->txtOutput->setPlainText(Form->txtOutput->toPlainText() + Form->txtCommand->text() + "\n");
	Form->txtCommand->setText("");

	ZEString Output = Engine.Execute(Form->txtCommand->text().toUtf8().cbegin());
	Form->txtOutput->setPlainText(Form->txtOutput->toPlainText() + Output + "\n");
}

bool ZEDConsoleWidget::eventFilter(QObject* Object, QEvent* Event)
{
	if (Object != Form->txtCommand && Event->type() == QEvent::KeyPress)
		return false;

	QKeyEvent* KeyEvent = static_cast<QKeyEvent*>(Event);
	if (KeyEvent->key() == Qt::Key_Enter || KeyEvent->key() == Qt::Key_Return)
	{
		Execute();
		return true;
	}

	return false;
}

void ZEDConsoleWidget::btnExecute_clicked(bool)
{
	Execute();
}

void ZEDConsoleWidget::btnClear_clicked(bool)
{
	Form->txtOutput->setPlainText("");
}

void ZEDConsoleWidget::btnSave_clicked(bool)
{
	QString FileName = QFileDialog::getSaveFileName(this, "Save Console Output", QString(), "Text files (*.txt);;All files (*.*)");
	if (FileName.isEmpty())
		return;

	if (QFile::exists(FileName))
	{
		int Result = QMessageBox::question(this, "Save Console Output", "Are you sure you want to overwrite this file ?", QDialogButtonBox::Yes, QDialogButtonBox::No);
		if (Result == QDialogButtonBox::No || Result == -1)
			return;
	}

	QFile File(FileName);
	bool Result = File.open(QIODevice::WriteOnly | QIODevice::Text);
	if (!Result)
	{
		QMessageBox::critical(this, "Save Console Output", "Cannot open file.", QDialogButtonBox::Ok, QDialogButtonBox::NoButton);
		return;
	}

	QTextStream Stream(&File);
	Stream << Form->txtOutput->toPlainText();

	File.close();
}

ZEDConsoleWidget::ZEDConsoleWidget(QWidget* Parent) : QWidget(Parent)
{
	Form = new Ui_ZEDConsoleWidget();
	Form->setupUi(this);
	Form->txtCommand->installEventFilter(this);
	connect(Form->btnExecute, SIGNAL(clicked(bool)), this, SLOT(btnExecute_clicked(bool)));
	connect(Form->btnClear, SIGNAL(clicked(bool)), this, SLOT(btnClear_clicked(bool)));
	connect(Form->btnSave, SIGNAL(clicked(bool)), this, SLOT(btnSave_clicked(bool)));
}

ZEDConsoleWidget::~ZEDConsoleWidget()
{
	delete Form;
}
