//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDNumberedTextEditor.cpp
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

/* This file is part of the KDE libraries
Copyright (C) 2005, 2006 KJSEmbed Authors
See included AUTHORS file.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.
*/
#include <QTextDocument>
#include <QTextBlock>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QPainter>
#include <QAbstractTextDocumentLayout>
#include <QToolTip>
#include <QDebug>

#include "ZEDNumberedTextEditor.h"

NumberBar::NumberBar( QWidget *parent )
: QWidget( parent ), edit(0), stopLine(-1), currentLine(-1), bugLine(-1)
{
	// Make room for 4 digits and the breakpoint icon
	setFixedWidth( fontMetrics().width( QString("0000") + 10 + 32 ) );
	stopMarker = QPixmap( "images/no.png" );
	currentMarker = QPixmap( "images/next.png" );
	bugMarker = QPixmap( "images/bug.png" );
}

NumberBar::~NumberBar()
{
}

void NumberBar::setCurrentLine( int lineno )
{
	currentLine = lineno;
}

void NumberBar::setStopLine( int lineno )
{
	stopLine = lineno;
}

void NumberBar::setBugLine( int lineno )
{
	bugLine = lineno;
}

void NumberBar::setTextEdit( QTextEdit *edit )
{
	this->edit = edit;
	connect( edit->document()->documentLayout(), SIGNAL( update(const QRectF &) ),
		this, SLOT( update() ) );
	connect( edit->verticalScrollBar(), SIGNAL(valueChanged(int) ),
		this, SLOT( update() ) );
}

void NumberBar::paintEvent( QPaintEvent * )
{
	QAbstractTextDocumentLayout *layout = edit->document()->documentLayout();
	int contentsY = edit->verticalScrollBar()->value();
	qreal pageBottom = contentsY + edit->viewport()->height();
	const QFontMetrics fm = fontMetrics();
	const int ascent = fontMetrics().ascent() + 1; // height = ascent + descent + 1
	int lineCount = 1;

	QPainter p(this);

	bugRect = QRect();
	stopRect = QRect();
	currentRect = QRect();

	for ( QTextBlock block = edit->document()->begin();
		block.isValid(); block = block.next(), ++lineCount ) {

			const QRectF boundingRect = layout->blockBoundingRect( block );

			QPointF position = boundingRect.topLeft();
			if ( position.y() + boundingRect.height() < contentsY )
				continue;
			if ( position.y() > pageBottom )
				break;

			const QString txt = QString::number( lineCount );
			p.drawText( width() - fm.width(txt), qRound( position.y() ) - contentsY + ascent, txt );

			// Bug marker
			if ( bugLine == lineCount ) {
				p.drawPixmap( 1, qRound( position.y() ) - contentsY, bugMarker );
				bugRect = QRect( 1, qRound( position.y() ) - contentsY, bugMarker.width(), bugMarker.height() );
			}

			// Stop marker
			if ( stopLine == lineCount ) {
				p.drawPixmap( 19, qRound( position.y() ) - contentsY, stopMarker );
				stopRect = QRect( 19, qRound( position.y() ) - contentsY, stopMarker.width(), stopMarker.height() );
			}

			// Current line marker
			if ( currentLine == lineCount ) {
				p.drawPixmap( 19, qRound( position.y() ) - contentsY, currentMarker );
				currentRect = QRect( 19, qRound( position.y() ) - contentsY, currentMarker.width(), currentMarker.height() );
			}
	}
}

bool NumberBar::event( QEvent *event )
{
	if ( event->type() == QEvent::ToolTip ) {
		QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

		if ( stopRect.contains( helpEvent->pos() ) ) {
			QToolTip::showText( helpEvent->globalPos(), "Stop Here" );
		}
		else if ( currentRect.contains( helpEvent->pos() ) ) {
			QToolTip::showText( helpEvent->globalPos(), "Current Line" );
		}
		else if ( bugRect.contains( helpEvent->pos() ) ) {
			QToolTip::showText( helpEvent->globalPos(), "Error Line" );
		}
	}

	return QWidget::event(event);
}

NumberedTextView::NumberedTextView( QWidget *parent )
: QFrame( parent )
{
	setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	setLineWidth( 2 );

	// Setup the main view
	view = new QTextEdit( this );
	view->setFontFamily( "Courier" );
	view->setLineWrapMode( QTextEdit::NoWrap );
	view->setFrameStyle( QFrame::NoFrame );
	view->installEventFilter( this );

	connect( view->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(textChanged(int,int,int)) );

	// Setup the line number pane
	numbers = new NumberBar( this );
	numbers->setTextEdit( view );
	// Testing...
	numbers->setStopLine( 3 );
	numbers->setBugLine( 1 );
	setCurrentLine( 5 );

	box = new QHBoxLayout( this );
	box->setSpacing( 0 );
	box->setMargin( 0 );
	box->addWidget( numbers );
	box->addWidget( view );
}

NumberedTextView::~NumberedTextView()
{
}

void NumberedTextView::setCurrentLine( int lineno )
{
	currentLine = lineno;
	numbers->setCurrentLine( lineno );
	textChanged( 0, 0, 1 );
}

void NumberedTextView::setStopLine( int lineno )
{
	numbers->setStopLine( lineno );
}

void NumberedTextView::setBugLine( int lineno )
{
	numbers->setBugLine( lineno );
}

void NumberedTextView::textChanged( int pos, int removed, int added )
{
	Q_UNUSED( pos );

	if ( removed == 0 && added == 0 )
		return;

	QTextBlock block = highlight.block();
	QTextBlockFormat fmt = block.blockFormat();
	QColor bg = view->palette().base().color();
	fmt.setBackground( bg );
	highlight.setBlockFormat( fmt );

	int lineCount = 1;
	for ( QTextBlock block = view->document()->begin();
		block.isValid(); block = block.next(), ++lineCount ) {

			if ( lineCount == currentLine ) {
				fmt = block.blockFormat();
				QColor bg = view->palette().highlight().color().light( 175 );
				fmt.setBackground( bg );

				highlight = QTextCursor( block );
				highlight.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor );
				highlight.setBlockFormat( fmt );

				break;
			}
	}
}

bool NumberedTextView::eventFilter( QObject *obj, QEvent *event )
{
	if ( obj != view )
		return QFrame::eventFilter(obj, event);

	if ( event->type() == QEvent::ToolTip ) {
		QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

		QTextCursor cursor = view->cursorForPosition( helpEvent->pos() );
		cursor.movePosition( QTextCursor::StartOfWord, QTextCursor::MoveAnchor );
		cursor.movePosition( QTextCursor::EndOfWord, QTextCursor::KeepAnchor );

		QString word = cursor.selectedText();
		emit mouseHover( word );
		emit mouseHover( helpEvent->pos(), word );

		// QToolTip::showText( helpEvent->globalPos(), word ); // For testing
	}

	return false;
}
