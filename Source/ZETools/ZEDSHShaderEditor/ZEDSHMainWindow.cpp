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

#include "ZEFile/ZEFile.h"
#include "ZEFile/ZEPathManager.h"

#include "ui_ZEDSHCompileOptionsWindow.h"
#include "ui_ZEDSHMainWindow.h"
#include "ZEDSHErrorsWindow.h"
#include "ZEDSHOutputWindow.h"
#include "ZEDSHCompileOptionsWindow.h"
#include "ZEGraphics/ZEGRShaderCompileOptions.h"
#include "ZEModules/ZEDirect3D11/ZED11ShaderCompiler.h"

#include <QPlainTextEdit>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QSettings>


class ZEDSHShaderCompiler : public ZED11ShaderCompiler
{
	public:
						ZEDSHShaderCompiler() {}
		virtual			~ZEDSHShaderCompiler() {}
};

void ZEDSHMainWindow::UpdateUI()
{
	bool Selection = Loaded && Editor->textCursor().hasSelection();
	Editor->setEnabled(Loaded);
	Form->actNew->setEnabled(!Engine);
	Form->actOpen->setEnabled(!Engine);
	Form->mnuRecentFiles->setEnabled(!Engine);
	Form->actLoadShader->setEnabled(Engine);
	Form->actUploadToEngine->setEnabled(Engine);
	Form->actSave->setEnabled(Loaded);
	Form->actSaveAs->setEnabled(Loaded);
	Form->actClose->setEnabled(Loaded);

	Form->actSelectAll->setEnabled(Loaded);
	Form->actDeselect->setEnabled(Selection);
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

	Form->actUndo->setEnabled(false);
	Form->actRedo->setEnabled(false);

	UpdateUI();
}

void ZEDSHMainWindow::OpenDocument(const QString& FileName)
{
	if (!CloseDocument())
		return;

	ZEFile File;
	if (!File.Open(FileName.toLocal8Bit().begin(), ZE_FOM_READ, ZE_FCM_NONE))
	{
		QMessageBox::critical(this, "Zinek Shader Editor", "Cannot open file for reading.", QMessageBox::Ok);
		return;
	}
	
	ZESize FileSize = File.GetSize();
	ZEPointer<ZEBYTE> Buffer = new ZEBYTE[FileSize + 1];
	if (FileSize != 0 && File.Read(Buffer.GetPointer(), FileSize, 1) != 1)
	{
		QMessageBox::critical(this, "Zinek Shader Editor", "Cannot read from file.", QMessageBox::Ok);
		return;
	}
	File.Close();

	Buffer[FileSize] = '\0';
	Editor->setPlainText((char*)Buffer.GetPointer());

	Loaded = true;
	Compiled = false;
	HasChanges = false;
	this->FileName = FileName;

	ZEGRShaderCompileOptions Options = CompileOptionsWindow->GetOptions();
	Options.FileName = FileName.toLocal8Bit().begin();
	CompileOptionsWindow->SetOptions(Options);

	Form->actUndo->setEnabled(false);
	Form->actRedo->setEnabled(false);

	RegisterRecentFile(FileName);
	UpdateUI();
}

void ZEDSHMainWindow::SaveDocument(const QString& FileName)
{
	if (!Loaded)
		return;

	ZEPathManager* PathManager = ZEPathManager::GetInstance();
	bool AccessControl = PathManager->GetAccessControl();
	PathManager->SetAccessControl(false);

	ZEFile File;
	if (!File.Open(FileName.toLocal8Bit().begin(), ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
	{
		QMessageBox::critical(this, "Zinek Shader Editor", "Cannot open file for writing.", QMessageBox::Ok);
		return;
	}

	QString Text = Editor->toPlainText();
	QByteArray TextUTF8 = Text.toLocal8Bit();
	if (TextUTF8.count() != 0 && File.Write(TextUTF8.begin(), TextUTF8.count(), 1) != 1)
	{
		QMessageBox::critical(this, "Zinek Shader Editor", "Cannot write to file.", QMessageBox::Ok);
		return;
	}
	File.Close();

	PathManager->SetAccessControl(AccessControl);

	ZEGRShaderCompileOptions TempOptions = GetCompileOptionsWindow()->GetOptions();
	TempOptions.SourceData.AddMultiple((ZEBYTE*)Editor->toPlainText().toLocal8Bit().data(), Editor->toPlainText().toLocal8Bit().count());
	GetCompileOptionsWindow()->SetOptions(TempOptions);

	this->statusBar()->showMessage("Changes Saved!");

	HasChanges = false;
	this->FileName = FileName;
	
	RegisterRecentFile(FileName);
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
	UploadedToEngine = false;
	this->FileName = QString();
	Editor->setPlainText("");

	UpdateUI();

	return true;
}


void ZEDSHMainWindow::RegisterRecentFile(const QString& FileName)
{
	QSettings Settings("Zinek", "ZESHShaderEditor");
	QStringList RecentFiles = Settings.value("RecentFiles", QStringList()).toStringList();

	int Index = RecentFiles.indexOf(FileName);
	if (Index != -1)
		RecentFiles.removeAt(Index);

	if (RecentFiles.size() > 10)
		RecentFiles.removeLast();

	RecentFiles.insert(0, FileName);

	Settings.setValue("RecentFiles", RecentFiles);

	LoadRecentFiles();

	Form->mnuRecentFiles->setVisible(false);
}

void ZEDSHMainWindow::LoadRecentFiles()
{
	QSettings Settings("Zinek", "ZESHShaderEditor");
	QStringList RecentFiles = Settings.value("RecentFiles", QStringList()).toStringList();

	Form->mnuRecentFiles->setVisible(!RecentFiles.isEmpty());

	Form->mnuRecentFiles->clear();
	for (int I = 0; I < RecentFiles.size(); I++)
	{
		QAction* RecentFileAction = new QAction(this);
		RecentFileAction->setText(RecentFiles[I]);
		connect(RecentFileAction, SIGNAL(triggered()), this, SLOT(actRecentFile_OnTrigger()));
		Form->mnuRecentFiles->addAction(RecentFileAction);	
	}
}

void ZEDSHMainWindow::Editor_OnTextChanged()
{
	if (HasChanges)
		return;

	HasChanges = true;
	Compiled = false;
	UploadedToEngine = false;

	UpdateUI();
}

void ZEDSHMainWindow::Editor_OnSelectionChanged()
{
	UpdateUI();
}

void ZEDSHMainWindow::Editor_OnCursorPositionChanged()
{

}

void ZEDSHMainWindow::actNew_OnTrigger()
{
	NewDocument();
}

void ZEDSHMainWindow::actOpen_OnTrigger()
{
	QString FileName = QFileDialog::getOpenFileName(this, tr("Open File"), this->FileName, tr("Shader Files (*.zeShader *.hlsl);;Any Files (*.*)"));
	if (FileName.isNull())
		return;

	OpenDocument(FileName);
}

void ZEDSHMainWindow::actRecentFile_OnTrigger()
{
	OpenDocument(((QAction*)sender())->text().toUtf8().constData());
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
	QString FileName = QFileDialog::getSaveFileName(this, tr("Open File"), this->FileName, tr("Shader files (*.zeShader *.hlsl);;Any Files (*.*)"));
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
	Editor->document()->undo();
}

void ZEDSHMainWindow::actRedo_OnTrigger()
{
	Editor->document()->redo();
}

void ZEDSHMainWindow::actSelectAll_OnTrigger()
{
	Editor->selectAll();
}

void ZEDSHMainWindow::actDeselect_OnTrigger()
{
	Editor->textCursor().clearSelection();
}

void ZEDSHMainWindow::actCut_OnTrigger()
{
	Editor->cut();
}

void ZEDSHMainWindow::actCopy_OnTrigger()
{
	Editor->copy();
}

void ZEDSHMainWindow::actPaste_OnTrigger()
{
	Editor->paste();
}

void ZEDSHMainWindow::actDelete_OnTrigger()
{
	Editor->textCursor().removeSelectedText();
}

void ZEDSHMainWindow::actFind_OnTrigger()
{

}

void ZEDSHMainWindow::actReplace_OnTrigger()
{

}

void ZEDSHMainWindow::actCompile_OnTrigger()
{
	OutputWindow->Clear();
	OutputWindow->Print(QString("Compiling... (%1)\n").arg(QDateTime::currentDateTime().toString()));
	OutputWindow->Print("\n");

	ZEGRShaderCompileOptions Options = CompileOptionsWindow->GetOptions();
	Options.SourceData.AddMultiple((ZEBYTE*)Editor->toPlainText().toLocal8Bit().data(), Editor->toPlainText().toLocal8Bit().count());

	ZEPointer<ZEDSHShaderCompiler> Compiler = new ZEDSHShaderCompiler();

	ZEArray<ZEBYTE> ShaderBinary;
	ZEString Output;
	bool BreakOnError = ZEError::GetInstance()->GetBreakOnErrorEnabled();
	
	ZEError::GetInstance()->SetBreakOnErrorEnabled(false);
	ZEError::GetInstance()->SetBreakOnDebugCheckEnabled(false);
	bool Result = Compiler->Compile(ShaderBinary, Options, NULL, &Output, true);
	ZEError::GetInstance()->SetBreakOnDebugCheckEnabled(true);
	ZEError::GetInstance()->SetBreakOnErrorEnabled(BreakOnError);

	ErrorsWindow->ParseCompilerOutput(QString(Output));

	OutputWindow->Print(Output.ToCString());
	OutputWindow->Print("\n");
	OutputWindow->Print(QString("Compile %1.\n").arg(Result ? "suceeded" : "failed"));
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

void ZEDSHMainWindow::actUploadToEngine_OnTrigger()
{
	UploadToEngine();
}

void ZEDSHMainWindow::closeEvent(QCloseEvent* e)
{
	if(Engine && !UploadedToEngine)
	{
		int Result = QMessageBox::question(this, "Zinek Shader Editor", "Do you want to upload to engine " + FileName + " ?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		if(Result == QMessageBox::Yes)
		{
			UploadToEngine();
		}
		else if(Result == QMessageBox::Cancel)
		{
			e->ignore();
			return;
		}
	}

	e->accept();
}

ZEDSHEditorWidget* ZEDSHMainWindow::GetEditor()
{
	return Editor;
}

ZEDSHOutputWindow* ZEDSHMainWindow::GetOutputWindow()
{
	return OutputWindow;
}

ZEDSHErrorsWindow* ZEDSHMainWindow::GetErrorsWindow()
{
	return ErrorsWindow;
}

ZEDSHCompileOptionsWindow* ZEDSHMainWindow::GetCompileOptionsWindow()
{
	return CompileOptionsWindow;
}

void ZEDSHMainWindow::Load(ZEGRShaderCompileOptions& Options)
{
	CompileOptionsWindow->SetOptions(Options);
	Editor->setPlainText((char*)Options.SourceData.GetCArray());

	Loaded = true;
	Engine = true;
	Compiled = false;
	HasChanges = false;
	UploadedToEngine = false;

	FileName = Options.FileName.ToCString();

	EngineCompileOptions = &Options;

	actCompile_OnTrigger();

	RegisterRecentFile(FileName);
	UpdateUI();
}

void ZEDSHMainWindow::UploadToEngine()
{
	*EngineCompileOptions = GetCompileOptionsWindow()->GetOptions();

	UploadedToEngine = true;

	this->statusBar()->showMessage("Shader Uploaded To Engine!");
}

void ZEDSHMainWindow::UpdateRowColNums()
{
	ZEUInt16 RowNum = Editor->textCursor().blockNumber() + 1;
	ZEUInt16 ColNum = Editor->textCursor().columnNumber() + 1;

	lblRowColNum->setText(QString("Row:%1   Col:%2").arg(RowNum, 3).arg(ColNum, 3));
}

ZEDSHMainWindow::ZEDSHMainWindow(QWidget* Parent) : QMainWindow(Parent)
{
	Form = new Ui_ZEDSHMainWindow();
	Form->setupUi(this);

	Editor = Form->textEdit;

	OutputWindow = new ZEDSHOutputWindow(this);
	ErrorsWindow = new ZEDSHErrorsWindow(this);
	CompileOptionsWindow = new ZEDSHCompileOptionsWindow(this);

	addDockWidget(Qt::BottomDockWidgetArea, OutputWindow);
	addDockWidget(Qt::BottomDockWidgetArea, ErrorsWindow);
	addDockWidget(Qt::RightDockWidgetArea, CompileOptionsWindow);

	lblRowColNum = new QLabel("Row:     Col:     ");
	lblRowColNum->setFixedWidth(100);

	statusBar()->addPermanentWidget(lblRowColNum);

	Loaded = false;
	Engine = false;
	Compiled = false;
	HasChanges = false;
	UploadedToEngine = false;

	UpdateUI();

	Form->actUndo->setEnabled(false);
	Form->actRedo->setEnabled(false);

	connect(Form->actNew,				SIGNAL(triggered()), this, SLOT(actNew_OnTrigger()));
	connect(Form->actOpen,				SIGNAL(triggered()), this, SLOT(actOpen_OnTrigger()));
	connect(Form->actSave,				SIGNAL(triggered()), this, SLOT(actSave_OnTrigger()));
	connect(Form->actSaveAs,			SIGNAL(triggered()), this, SLOT(actSaveAs_OnTrigger()));
	connect(Form->actClose,				SIGNAL(triggered()), this, SLOT(actClose_OnTrigger()));
	connect(Form->actQuit,				SIGNAL(triggered()), this, SLOT(actQuit_OnTrigger()));
	connect(Form->actUndo,				SIGNAL(triggered()), this, SLOT(actUndo_OnTrigger()));
	connect(Form->actRedo,				SIGNAL(triggered()), this, SLOT(actRedo_OnTrigger()));
	connect(Form->actSelectAll,			SIGNAL(triggered()), this, SLOT(actSelectAll_OnTrigger()));
	connect(Form->actDeselect,			SIGNAL(triggered()), this, SLOT(actDeselect_OnTrigger()));
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
	connect(Form->actUploadToEngine,	SIGNAL(triggered()), this, SLOT(actUploadToEngine_OnTrigger()));
	connect(Editor,						SIGNAL(undoAvailable(bool)), Form->actUndo, SLOT(setEnabled(bool)));
	connect(Editor,						SIGNAL(redoAvailable(bool)), Form->actRedo, SLOT(setEnabled(bool)));
	connect(Editor,						SIGNAL(cursorPositionChanged()), this, SLOT(UpdateRowColNums()));

	LoadRecentFiles();
}
