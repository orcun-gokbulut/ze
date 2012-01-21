//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDScriptEditor.cpp
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

#include "ZEDScriptEditor.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPaintEvent>
#include <QPainter>
#include <QRect>
#include <QPaintEngine>

/************************************************************************/
/*                            LUA HighLighter                           */
/************************************************************************/

ZEDLuaScriptHighLighter::ZEDLuaScriptHighLighter(QTextEdit* Parent) : QSyntaxHighlighter(Parent)
{
	DoubleCotationFormat.setFontWeight(QFont::Normal);
	DoubleCotationFormat.setForeground(Qt::darkRed);

	KeyWordFormat.setFontWeight(QFont::Normal);
	KeyWordFormat.setForeground(Qt::blue);

	//MultiLineCommandFormat.setFontWeight(QFont::Normal);
	//MultiLineCommandFormat.setForeground(Qt::darkGreen);

	SingleLineCommentFormat.setFontItalic(true);
	SingleLineCommentFormat.setForeground(Qt::darkGreen);

	KeyWords << "\\band\\b" << "\\bend\\b" << "\\bin\\b" << "\\brepeat\\b" << "\\bbreak\\b" << "\\bfalse\\b" << "\\blocal\\b" << "\\breturn\\b" << "\\bdo\\b" 
			 << "\\bfor\\b" << "\\bnil\\b" << "\\bthen\\b" << "\\belse\\b" << "\\bfunction\\b" << "\\bnot\\b" << "\\btrue\\b" << "\\belseif\\b" << "\\bif\\b" 
			 << "\\bor\\b" << "\\buntil\\b" << "\\bwhile\\b";

	for (ZEInt I = 0; I < KeyWords.count(); I++)
	{
		ZEDSyntaxHighLightRule Rule;
		Rule.Expresion = QRegExp(KeyWords[I]);
		Rule.FontFormat = KeyWordFormat;
		Rules.append(Rule);
	}

	ZEDSyntaxHighLightRule Rule;

	Rule.Expresion = QRegExp(QString("\".*\""));
	Rule.FontFormat = DoubleCotationFormat;
	Rules.append(Rule);

	Rule.Expresion = QRegExp(QString("--[^|]*"));
	Rule.FontFormat = SingleLineCommentFormat;
	Rules.append(Rule);
}

void ZEDLuaScriptHighLighter::highlightBlock(const QString &Text)
{
	for (ZEInt I = 0; I < Rules.count(); I++)
	{
		QRegExp Expression = Rules[I].Expresion;
		ZEInt Index = Expression.indexIn(Text);

		while (Index >= 0)
		{
			ZEInt Length = Expression.matchedLength();
			setFormat(Index, Length, Rules[I].FontFormat);
			Index = Expression.indexIn(Text, Index + Length);
		}
	}

	((QTextEdit*)(parent()))->setFontFamily(QString("Courier New"));
	((QTextEdit*)(parent()))->setFontPointSize(8);
}

/************************************************************************/
/*                            Script Editor                             */
/************************************************************************/

ZEDScriptEditor::ZEDScriptEditor(QWidget *Parent) : QWidget(Parent)
{
	MainLayout = new QVBoxLayout(this);
	MainLayout->setContentsMargins(0,0,0,0);
	this->setLayout(MainLayout);
	TextView = new NumberedTextView(Parent);
	ToolBar = new QToolBar(this);
	TextEditor = TextView->textEdit();
	TextEditor->setFontFamily(QString("Courier New"));
	TextEditor->setFontPointSize(8);
	TextEditor->setWordWrapMode(QTextOption::NoWrap);
	HighLighter = new ZEDLuaScriptHighLighter(TextEditor);
	this->setLayout(MainLayout);
	MainLayout->addWidget(ToolBar);
	MainLayout->addWidget(TextView);
	IsDocumentModified = false;
 
	OpenEdiorAction = new QAction(QString("Open Editor"), ToolBar);
	OpenEdiorAction->setIcon(QIcon(":/ToolBarIcons/ScriptEditor.png"));

	LoadAction = new QAction(QString("Load"), ToolBar);
	LoadAction->setIcon(QIcon(":/ToolBarIcons/Load.png"));

	SaveAction = new QAction(QString("Save"), ToolBar);
	SaveAction->setIcon(QIcon(":/ToolBarIcons/Save.png"));
	SaveAction->setShortcut(QKeySequence::Save);

	UndoAction = new QAction(QString("Undo"), ToolBar);
	UndoAction->setIcon(QIcon(":/ToolBarIcons/Undo.png"));
	//UndoAction->setShortcut(QKeySequence::Undo);

	RedoAction = new QAction(QString("Redo"), ToolBar);
	RedoAction->setIcon(QIcon(":/ToolBarIcons/Redo.png"));
//	RedoAction->setShortcut(QKeySequence::Redo);

	CopyAction = new QAction(QString("Copy"), ToolBar);
	CopyAction->setIcon(QIcon(":/ToolBarIcons/Copy.png"));
	CopyAction->setShortcut(QKeySequence::Copy);

	CutAction = new QAction(QString("Cut"), ToolBar);
	CutAction->setIcon(QIcon(":/ToolBarIcons/Cut.png"));
	CutAction->setShortcut(QKeySequence::Cut);

	PasteAction = new QAction(QString("Paste"), ToolBar);
	PasteAction->setIcon(QIcon(":/ToolBarIcons/Paste.png"));
	PasteAction->setShortcut(QKeySequence::Paste);

	ToolBar->addAction(OpenEdiorAction);
	ToolBar->addAction(LoadAction);
	ToolBar->addAction(SaveAction);
	ToolBar->addAction(UndoAction);
	ToolBar->addAction(RedoAction);
	ToolBar->addAction(CopyAction);
	ToolBar->addAction(CutAction);
	ToolBar->addAction(PasteAction);

	this->TextEditor->setTabStopWidth(32);
	this->show();

	QObject::connect(OpenEdiorAction,	SIGNAL(triggered(bool)), this,				SLOT(OpenEditorButtonTriggered()));
	QObject::connect(CopyAction,		SIGNAL(triggered(bool)), this->TextEditor,	SLOT(copy()));
	QObject::connect(CutAction,			SIGNAL(triggered(bool)), this->TextEditor,	SLOT(cut()));
	QObject::connect(PasteAction,		SIGNAL(triggered(bool)), this->TextEditor,	SLOT(paste()));
	QObject::connect(UndoAction,		SIGNAL(triggered(bool)), this->TextEditor,	SLOT(undo()));
	QObject::connect(RedoAction,		SIGNAL(triggered(bool)), this->TextEditor,	SLOT(redo()));
}

void ZEDScriptEditor::SetOpenEditorButtonVisibility(bool Visibility)
{
	OpenEdiorAction->setVisible(Visibility);
}

bool ZEDScriptEditor::GetIsDocumentModified() const
{
	return IsDocumentModified;
}

QTextEdit* ZEDScriptEditor::GetTextEditor() const
{
	return TextEditor;
}

void ZEDScriptEditor::OpenEditorButtonTriggered()
{
	TextView->setDisabled(true);

	QDialog* ScriptEditorDialog;
	ScriptEditorDialog = new QDialog(0);
	ScriptEditorDialog->setWindowTitle("Script Edior");
	ScriptEditorDialog->setModal(true);
	QDialogButtonBox* ButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,Qt::Horizontal ,ScriptEditorDialog);
	QVBoxLayout* DialogLayout = new QVBoxLayout(ScriptEditorDialog);
	ScriptEditorDialog->setLayout(DialogLayout);
	DialogLayout->setContentsMargins(0,0,0,0);
	ZEDScriptEditor* ScriptEdior = new ZEDScriptEditor(ScriptEditorDialog);
	DialogLayout->addWidget(ScriptEdior);
	DialogLayout->addWidget(ButtonBox);
	ScriptEdior->SetOpenEditorButtonVisibility(false);

	QObject* OkButton = ((QObject*)(ButtonBox->button(QDialogButtonBox::Ok)));
	QObject* CancelButton = ((QObject*)(ButtonBox->button(QDialogButtonBox::Cancel)));

	QObject::connect(OkButton,		SIGNAL(clicked(bool)), ScriptEditorDialog, SLOT(accept()));
	QObject::connect(CancelButton,	SIGNAL(clicked(bool)), ScriptEditorDialog, SLOT(reject()));

	ScriptEdior->GetTextEditor()->setText(this->TextEditor->toPlainText());
	ScriptEditorDialog->setBaseSize(QSize(800,600));
	
	ZEInt Result = ScriptEditorDialog->exec();

	if (Result == QDialog::Accepted)
	{
		this->TextEditor->setText(ScriptEdior->TextEditor->toPlainText());
	}

	else
	{
		QMessageBox MessageBox;
		MessageBox.setText("Document has been modified.");
		MessageBox.setInformativeText("Do you want to save your changes?");
		MessageBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
		MessageBox.setDefaultButton(QMessageBox::Save);
		ZEInt MessageResult = MessageBox.exec();

		if (MessageResult == QMessageBox::Save)
		{
			this->TextEditor->setText(ScriptEdior->GetTextEditor()->toPlainText());
		}
	}

	TextView->setEnabled(true);
}
