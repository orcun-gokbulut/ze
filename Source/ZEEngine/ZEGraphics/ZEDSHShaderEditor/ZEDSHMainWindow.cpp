//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSHMainWindow.cpp
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

#include "ZEDSHMainWindow.h"

#include "ui_ZEDSHMainWindow.h"
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include "ZEFile/ZEFile.h"
#include "ZEDSHErrorsWindow.h"
#include "ZEDSHOutputWindow.h"

void ZEDSHMainWindow::UpdateUI()
{
	bool Selection = Editor->textCursor().hasSelection();
	Editor->setEnabled(Loaded);
	Form->actNew->setEnabled(!Engine);
	Form->actOpen->setEnabled(!Engine);
	Form->actRecent->setEnabled(!Engine);
	Form->actSave->setEnabled(Loaded);
	Form->actSaveAs->setEnabled(Loaded);
	Form->actUndo->setEnabled(Loaded);
	Form->actRedo->setEnabled(Loaded);
	Form->actCut->setEnabled(Selection);
	Form->actCopy->setEnabled(Selection);
	Form->actPaste->setEnabled(Selection);
	Form->actDelete->setEnabled(Selection);
	Form->actFind->setEnabled(Loaded);
	Form->actReplace->setEnabled(Loaded);
	Form->actCompile->setEnabled(Loaded);
	this->setWindowTitle(QString("Zinek Shader Editor %1").arg(FileName != "" ? QString(" - %1").arg(FileName) : QString("")));
}

void ZEDSHMainWindow::NewDocument()
{
	if (!CloseDocument())
		return;

	FileName = "";
	Loaded = true;
	HasChanges = false;
	Compiled = false;
	this->FileName = QString();

	UpdateUI();
}

void ZEDSHMainWindow::OpenDocument(const QString& FileName)
{
	if (!CloseDocument())
		return;

	ZEFile File;
	if (!File.Open(FileName.toLocal8Bit().begin(), ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
	{
		QMessageBox::critical(this, "Zinek Shader Editor", "Cannot open file for reading.", QMessageBox::Ok);
		return;
	}

	File.Seek(0, ZE_SF_END);
	ZESize FileSize = File.Tell();
	File.Seek(0, ZE_SF_BEGINING);

	ZEPointer<ZEBYTE> Buffer = new ZEBYTE[FileSize];

	if (File.Read(Buffer.GetPointer(), FileSize, 1) != 1)
	{
		QMessageBox::critical(this, "Zinek Shader Editor", "Cannot read from file.", QMessageBox::Ok);
		return;
	}
	File.Close();

	Loaded = true;
	Compiled = false;
	HasChanges = false;
	this->FileName = FileName;

	UpdateUI();
}

void ZEDSHMainWindow::SaveDocument(const QString& FileName)
{
	if (!Loaded)
		return;

	ZEFile File;
	if (!File.Open(FileName.toLocal8Bit().begin(), ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
	{
		QMessageBox::critical(this, "Zinek Shader Editor", "Cannot open file for writing.", QMessageBox::Ok);
		return;
	}

	QString Text = Editor->toPlainText();
	QByteArray TextUTF8 = Text.toUtf8();
	if (File.Write(TextUTF8.begin(), TextUTF8.count(), 1) != 1)
	{
		QMessageBox::critical(this, "Zinek Shader Editor", "Cannot write to file.", QMessageBox::Ok);
		return;
	}
	File.Close();

	HasChanges = false;
	this->FileName = FileName;

	UpdateUI();
}

bool ZEDSHMainWindow::CloseDocument()
{
	if (HasChanges)
	{
		int Result = QMessageBox::question(this, "Zinek Shader Editor", "You have unsaved changes. Are you sure you want to quit ?", QMessageBox::Yes, QMessageBox::No);
		if (Result == QMessageBox::Yes)
			return false;
	}

	FileName = "";
	Loaded = false;
	Compiled = false;
	HasChanges = false;
	this->FileName = QString();

	return true;
}

void ZEDSHMainWindow::actNew_OnTrigger()
{
	NewDocument();
}

void ZEDSHMainWindow::actOpen_OnTrigger()
{
	QString FileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), tr("Shader files (*.zeShader, *.hlsl);;Any files (*)"));
	if (FileName.isNull())
		return;

	OpenDocument(FileName);
}

void ZEDSHMainWindow::actRecent_OnTrigger()
{

}

void ZEDSHMainWindow::actSave_OnTrigger()
{
	if (FileName == "")
	{
		actSaveAs_OnTrigger();
		return;
	}

	SaveDocument(FileName);
}

void ZEDSHMainWindow::actSaveAs_OnTrigger()
{
	QString FileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), 
		tr("Shader files (*.zeShader, *.hlsl);;Any files (*)"));

	if (FileName.isNull())
		return;

	SaveDocument(FileName);
}

void ZEDSHMainWindow::actClose_OnTrigger()
{
	CloseDocument();
}

void ZEDSHMainWindow::actQuit_OnTrigger()
{
	if (CloseDocument())
		this->close();
}

void ZEDSHMainWindow::actUndo_OnTrigger()
{

}

void ZEDSHMainWindow::actRedo_OnTrigger()
{

}

void ZEDSHMainWindow::actCut_OnTrigger()
{

}

void ZEDSHMainWindow::actCopy_OnTrigger()
{

}

void ZEDSHMainWindow::actPaste_OnTrigger()
{

}

void ZEDSHMainWindow::actDelete_OnTrigger()
{

}

void ZEDSHMainWindow::actFind_OnTrigger()
{

}

void ZEDSHMainWindow::actReplace_OnTrigger()
{

}

void ZEDSHMainWindow::actCompile_OnTrigger()
{

}

void ZEDSHMainWindow::actPreference_OnTrigger()
{

}

void ZEDSHMainWindow::actOutput_OnTrigger()
{

}

void ZEDSHMainWindow::actError_OnTrigger()
{

}

void ZEDSHMainWindow::actDefinitions_OnTrigger()
{

}

void ZEDSHMainWindow::actCompileParameters_OnTrigger()
{

}

void ZEDSHMainWindow::actReflection_OnTrigger()
{

}

void ZEDSHMainWindow::actAbout_OnTrigger()
{

}

ZEDSHMainWindow::ZEDSHMainWindow(QWidget* Parent) : QMainWindow(Parent)
{
	Form = new Ui_ZEDSHMainWindow();
	Form->setupUi(this);

	Editor = Form->textEdit;

	OutputWindow = new ZEDSHOutputWindow(this);
	ErrorsWindow = new ZEDSHErrorsWindow(this);

	addDockWidget(Qt::BottomDockWidgetArea, OutputWindow);
	addDockWidget(Qt::BottomDockWidgetArea, ErrorsWindow);

	Loaded = false;
	Engine = false;
	Compiled = false;
	HasChanges = false;

	UpdateUI();

	connect(Form->actNew,				SIGNAL(triggered()), this, SLOT(actNew_OnTrigger()));
	connect(Form->actOpen,				SIGNAL(triggered()), this, SLOT(actOpen_OnTrigger()));
	connect(Form->actRecent,			SIGNAL(triggered()), this, SLOT(actRecent_OnTrigger()));
	connect(Form->actSave,				SIGNAL(triggered()), this, SLOT(actSave_OnTrigger()));
	connect(Form->actSaveAs,			SIGNAL(triggered()), this, SLOT(actSaveAs_OnTrigger()));
	connect(Form->actClose,				SIGNAL(triggered()), this, SLOT(actClose_OnTrigger()));
	connect(Form->actQuit,				SIGNAL(triggered()), this, SLOT(actQuit_OnTrigger()));
	connect(Form->actUndo,				SIGNAL(triggered()), this, SLOT(actUndo_OnTrigger()));
	connect(Form->actRedo,				SIGNAL(triggered()), this, SLOT(actRedo_OnTrigger()));
	connect(Form->actCut,				SIGNAL(triggered()), this, SLOT(actCut_OnTrigger()));
	connect(Form->actCopy,				SIGNAL(triggered()), this, SLOT(actCopy_OnTrigger()));
	connect(Form->actPaste,				SIGNAL(triggered()), this, SLOT(actPaste_OnTrigger()));
	connect(Form->actDelete,			SIGNAL(triggered()), this, SLOT(actDelete_OnTrigger()));
	connect(Form->actFind,				SIGNAL(triggered()), this, SLOT(actFind_OnTrigger()));
	connect(Form->actReplace,			SIGNAL(triggered()), this, SLOT(actReplace_OnTrigger()));
	connect(Form->actCompile,			SIGNAL(triggered()), this, SLOT(actCompile_OnTrigger()));
	connect(Form->actPreference,		SIGNAL(triggered()), this, SLOT(actPreference_OnTrigger()));
	connect(Form->actOutput,			SIGNAL(triggered()), this, SLOT(actOutput_OnTrigger()));
	connect(Form->actError,				SIGNAL(triggered()), this, SLOT(actError_OnTrigger()));
	connect(Form->actDefinitions,		SIGNAL(triggered()), this, SLOT(actDefinitions_OnTrigger()));
	connect(Form->actCompileParameters,	SIGNAL(triggered()), this, SLOT(actCompileParameters_OnTrigger()));
	connect(Form->actReflection,		SIGNAL(triggered()), this, SLOT(actReflection_OnTrigger()));
	connect(Form->actAbout,				SIGNAL(triggered()), this, SLOT(actAbout_OnTrigger()));
}
