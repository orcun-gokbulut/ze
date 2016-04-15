//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSHEditorWidget.cpp
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

#include "ZEDSHEditorWidget.h"

#include "ZEDSHSyntaxHighlighter.h"
#include "ZEDSHEditorLineNumbersWidget.h"

#include <QtGui\QPainter>
#include <QtGui\qevent.h>
#include <QtCore\qglobal.h>

void ZEDSHEditorWidget::HighlightCurrentLine()
{
	QList<QTextEdit::ExtraSelection> extraSelections;

	if (!isReadOnly()) 
	{
		QTextEdit::ExtraSelection selection;

		QColor lineColor = QColor(255, 248, 239);

		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}

	setExtraSelections(extraSelections);
}

void ZEDSHEditorWidget::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	LineNumbers->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void ZEDSHEditorWidget::UpdateLineNumbers(const QRect &rect, int dy)
{
	if (dy)
		LineNumbers->scroll(0, dy);
	else
		LineNumbers->update(0, rect.y(), LineNumbers->width(), rect.height());

	if (rect.contains(viewport()->rect()))
		UpdateLineNumbersWidth(0);
}

void ZEDSHEditorWidget::UpdateLineNumbersWidth(int)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0 , 0);
}

int ZEDSHEditorWidget::lineNumberAreaWidth()
{
	int digits = 1;
	int max = qMax(1, blockCount());
	while (max >= 10) 
	{
		max /= 10;
		digits++;
	}

	int space = 25 + fontMetrics().width(QLatin1Char('9')) * digits;

	return space;
}

void ZEDSHEditorWidget::DrawLineNumbers(QPaintEvent* Event)
{
	QPainter painter(LineNumbers);
	painter.fillRect(Event->rect(), QColor(245, 245, 240));

	QPen Pen(QColor(43, 145, 175));
	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();

	while (block.isValid() && top <= Event->rect().bottom()) 
	{
		if (block.isVisible() && bottom >= Event->rect().top())
		{
			QString number = QString::number(blockNumber + 1);
			painter.setPen(Pen);
			painter.drawText(0, top, LineNumbers->width() - 10, fontMetrics().height(), Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		blockNumber++;
	}
}

ZEDSHEditorWidget::ZEDSHEditorWidget(QWidget* Parent) : QPlainTextEdit(Parent)
{
	new ZEDSHSyntaxHightlighter(document());
	setFont(QFont("Consolas", 10));

	const int tabStop = 4;
	QFontMetrics metrics(font());
	setTabStopWidth(tabStop * metrics.width(' '));

	setWordWrapMode(QTextOption::NoWrap);

	LineNumbers = new ZEDSHEditorLineNumbersWidget(this);

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(UpdateLineNumbersWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(UpdateLineNumbers(QRect,int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(HighlightCurrentLine()));

	UpdateLineNumbersWidth(0);
	HighlightCurrentLine();
}
