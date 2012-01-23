//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDNumberedTextEditor.h
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
// -*- c++ -*-
#ifndef NUMBERED_TEXT_VIEW_H
#define NUMBERED_TEXT_VIEW_H

#include <QFrame>
#include <QPixmap>
#include <QTextCursor>
#include "ZETypes.h"

class QTextEdit;
class QHBoxLayout;

/**
* @internal Used to display the numbers.
*/
class NumberBar : public QWidget
{
	Q_OBJECT

public:
	NumberBar( QWidget *parent );
	~NumberBar();

	void setCurrentLine( ZEInt lineno );
	void setStopLine( ZEInt lineno );
	void setBugLine( ZEInt lineno );

	void setTextEdit( QTextEdit *edit );
	void paintEvent( QPaintEvent *ev );

protected:
	bool event( QEvent *ev );

private:
	QTextEdit *edit;
	QPixmap stopMarker;
	QPixmap currentMarker;
	QPixmap bugMarker;
	ZEInt stopLine;
	ZEInt currentLine;
	ZEInt bugLine;
	QRect stopRect;
	QRect currentRect;
	QRect bugRect;
};

/**
* Displays a QTextEdit with line numbers.
*/
class NumberedTextView : public QFrame
{
	Q_OBJECT

public:
	NumberedTextView( QWidget *parent = 0 );
	~NumberedTextView();

	/** Returns the QTextEdit of the main view. */
	QTextEdit *textEdit() const { return view; }

	/**
	* Sets the line that should have the current line indicator.
	* A value of -1 indicates no line should show the indicator.
	*/
	void setCurrentLine( ZEInt lineno );

	/**
	* Sets the line that should have the stop line indicator.
	* A value of -1 indicates no line should show the indicator.
	*/
	void setStopLine( ZEInt lineno );

	/**
	* Sets the line that should have the bug line indicator.
	* A value of -1 indicates no line should show the indicator.
	*/
	void setBugLine( ZEInt lineno );

	/** @internal Used to get tooltip events from the view for the hover signal. */
	bool eventFilter( QObject *obj, QEvent *event );

signals:
	/**
	* Emitted when the mouse is hovered over the text edit component.
	* @param word The word under the mouse pointer
	*/
	void mouseHover( const QString &word );

	/**
	* Emitted when the mouse is hovered over the text edit component.
	* @param pos The position of the mouse pointer.
	* @param word The word under the mouse pointer
	*/
	void mouseHover( const QPoint &pos, const QString &word );

	protected slots:
		/** @internal Used to update the highlight on the current line. */
		void textChanged( ZEInt pos, ZEInt added, ZEInt removed );

private:
	QTextEdit *view;
	NumberBar *numbers;
	QHBoxLayout *box;
	ZEInt currentLine;
	QTextCursor highlight;
};


#endif // NUMBERED_TEXT_VIEW_H
