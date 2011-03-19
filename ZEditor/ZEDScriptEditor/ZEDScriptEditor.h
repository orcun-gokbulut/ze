//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDScriptEditor.h
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
#ifndef __ZED_SCRIPT_EDITOR_H__
#define __ZED_SCRIPT_EDITOR_H__

#include <QWidget>
#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QTextCharFormat>
#include <QStringList>
#include <QRegExp>
#include <QList>
#include <QAction>
#include <QToolBar>
#include "ZEDNumberedTextEditor.h"

struct ZEDSyntaxHighLightRule
{
	QRegExp			Expresion;
	QTextCharFormat	FontFormat;
};

class ZEDLuaScriptHighLighter : public QSyntaxHighlighter
{
	private:

		QTextCharFormat				DoubleCotationFormat;
		QTextCharFormat				KeyWordFormat;
		QTextCharFormat				MultiLineCommandFormat;
		QTextCharFormat				SingleLineCommentFormat;

		QStringList					KeyWords;
		QList<ZEDSyntaxHighLightRule>	Rules;

	protected:

		virtual void				highlightBlock(const QString &Text);

	public:

									ZEDLuaScriptHighLighter(QTextEdit* Parent);
};

class ZEDScriptEditor : public QWidget
{
	Q_OBJECT

	private:

		bool					IsDocumentModified;

		QTextEdit*				TextEditor;
		QVBoxLayout*			MainLayout;
		QSyntaxHighlighter*		HighLighter;
		QWidget*				OldParent;
		QToolBar*				ToolBar;
		NumberedTextView*		TextView;

		QAction*				OpenEdiorAction;
		QAction*				UndoAction;
		QAction*				RedoAction;
		QAction*				CopyAction;
		QAction*				CutAction;
		QAction*				PasteAction;
		QAction*				SaveAction;
		QAction*				LoadAction;

	private slots:

		void					OpenEditorButtonTriggered();

	public:

		bool					GetIsDocumentModified() const;
		QTextEdit*				GetTextEditor() const;
		void					SetOpenEditorButtonVisibility(bool Visibility);

								ZEDScriptEditor(QWidget* Parent = 0);
};

#endif
