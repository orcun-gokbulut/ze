//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSHErrorsWindow.cpp
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

#include "ZEDSHErrorsWindow.h"

#include "ZEFile/ZEFileInfo.h"
#include "ZERegEx/ZERegEx.h"
#include "ZEDSHMainWindow.h"
#include "ZEDSHEditorWidget.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QTextCursor>
#include <QTextBlock>

void ZEDSHErrorsWindow::tblErrors_onItemDoubleClicked(QTableWidgetItem * Item)
{
	int Row = Item->row();
	bool Result = false;
	int LineNumber = tblErrors->item(Row, 2)->text().toInt(&Result);
	if (!Result)
		return;

	QTextCursor TextCursor(MainWindow->GetEditor()->document()->findBlockByLineNumber(LineNumber - 1));
	TextCursor.select(QTextCursor::LineUnderCursor);
	MainWindow->GetEditor()->setTextCursor(TextCursor);

	MainWindow->GetEditor()->setFocus(Qt::OtherFocusReason);
}

QTableWidget* ZEDSHErrorsWindow::GetErrorTable()
{
	return tblErrors;
}

void ZEDSHErrorsWindow::AddError(ZEDSHErrorType Type, QString Code, QString Description, QString Line, QString File)
{
	int NewIndex = tblErrors->rowCount();
	tblErrors->insertRow(NewIndex);
	const char* TypeString;
	switch(Type)
	{
		case ZEDSG_ET_ERROR:
			TypeString = "E";
			break;

		case ZEDSG_ET_WARNING:
				TypeString = "W";
			break;

		default:
		case ZEDSG_ET_NOTICE:
			TypeString = "N";
			break;
	};

	QTableWidgetItem* Item0 = new QTableWidgetItem(TypeString);
	Item0->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	tblErrors->setItem(NewIndex, 0, Item0);

	QTableWidgetItem* Item1 = new QTableWidgetItem(QString("%1 %2: %3").arg(TypeString).arg(Code).arg(Description));
	Item1->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	tblErrors->setItem(NewIndex, 1, Item1);

	QTableWidgetItem* Item2 = new QTableWidgetItem(Line);
	Item2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	tblErrors->setItem(NewIndex, 2, Item2);

	QTableWidgetItem* Item3 = new QTableWidgetItem(File);
	Item3->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	tblErrors->setItem(NewIndex, 3, Item3);
}

void ZEDSHErrorsWindow::Clear()
{
	tblErrors->setRowCount(0);
}


void ZEDSHErrorsWindow::ParseCompilerOutput(const QString& Errors)
{
	Clear();
	QStringList Lines = Errors.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
	for (int I = 0; I < Lines.count(); I++)
	{
		ZERegExMatch Match;
		if (!CompilerOutputRegex.Match(Lines[I].toLocal8Bit().begin(), Match))
			return;

		ZEDSHErrorType ErrorType = ZEDSG_ET_ERROR;
		if (Match.SubMatches[0].String == "error")
			ErrorType = ZEDSG_ET_ERROR;
		else if (Match.SubMatches[0].String == "warning")
			ErrorType = ZEDSG_ET_WARNING;
		else if (Match.SubMatches[0].String == "notice")
			ErrorType = ZEDSG_ET_NOTICE;

		AddError(ErrorType, Match.SubMatches[5].String.ToCString(), Match.SubMatches[6].String.ToCString(), 
			Match.SubMatches[1].String.ToCString(), Match.SubMatches[0].String.ToCString());
	}
}

ZEDSHErrorsWindow::ZEDSHErrorsWindow(QWidget* Parent) : QDockWidget(Parent)
{
	MainWindow = (ZEDSHMainWindow*)Parent;

	this->setWindowTitle("Errors");

	tblErrors = new QTableWidget(this);
	tblErrors->setFont(QFont("Consolas", 10));
	tblErrors->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	
	tblErrors->setColumnCount(4);

	QTableWidgetItem* Column0 = new QTableWidgetItem("");
	tblErrors->setHorizontalHeaderItem(0, Column0);
	
	QTableWidgetItem* Column1 = new QTableWidgetItem("Description");
	tblErrors->setHorizontalHeaderItem(1, Column1);
	
	QTableWidgetItem* Column2 = new QTableWidgetItem("Line");
	tblErrors->setHorizontalHeaderItem(2, Column2);

	QTableWidgetItem* Column3 = new QTableWidgetItem("File");
	tblErrors->setHorizontalHeaderItem(3, Column3);
	setWidget(tblErrors);

	QHeaderView* HeaderView = new QHeaderView(Qt::Horizontal, tblErrors);
	tblErrors->setHorizontalHeader(HeaderView);
	HeaderView->setSectionResizeMode(0, QHeaderView::Interactive);
	HeaderView->setSectionResizeMode(1, QHeaderView::Interactive);
	HeaderView->setSectionResizeMode(2, QHeaderView::Interactive);
	HeaderView->setSectionResizeMode(3, QHeaderView::Interactive);

	tblErrors->setSelectionMode(QAbstractItemView::SingleSelection);
	tblErrors->setSelectionBehavior(QAbstractItemView::SelectRows);
	
	CompilerOutputRegex.Compile("\\s*(.*)\\s*\\(\\s*(.*)\\s*,\\s*(.*)\\s*\\-\\s*(.*)\\s*\\)\\s*:\\s*(.*)\\s+(.*)\\s*:\\s*(.*)\\s*");

	connect(tblErrors, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(tblErrors_onItemDoubleClicked(QTableWidgetItem*)));
}
