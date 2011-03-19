//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAssertBrowserViewPortItem.h
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

#ifndef __ZED_ASSERT_BROWSER_VIEWPORT_ITEM_H__
#define __ZED_ASSERT_BROWSER_VIEWPORT_ITEM_H__

#include <QFrame>
#include <QVBoxLayout>
#include <QLabel>
#include <QMenu>
#include "../../../../ZETools/ZEDFontEditor/trunk/Source/ZEDFontEditor.h"

class ZEDAssertBrowser;

class ZEDAssertBrowserViewPortItem : public QFrame
{
	Q_OBJECT

	private:

		bool				IsSelected;

	public:

		QVBoxLayout*		Layout;
		QString				FilePath;
		QString				FileName;
		QString				FileExtension;
		QLabel*				FileNameChecBox;
		QLabel*				PreviewWidget;
		QMenu*				ContextMenu;
		ZEDAssertBrowser*	ParentBrowser;

		void				SetSelected(bool Selected);
		bool				GetSelected();

							ZEDAssertBrowserViewPortItem(QString FilePath, QString FileName, QLabel* PreviewWidget, ZEDAssertBrowser*	ParentBrowser);
							ZEDAssertBrowserViewPortItem();

	protected:

		virtual void		mouseDoubleClickEvent(QMouseEvent* MouseEvent);
		virtual void		mousePressEvent(QMouseEvent* MouseEvent);
		virtual void		focusOutEvent(QFocusEvent* FocusEvent);
		virtual void		contextMenuEvent(QContextMenuEvent* MenuEvent);
		virtual void		mouseMoveEvent(QMouseEvent* Event);

	public slots:

		void				OnContextMenuDeleteAction();
		void				OnContextMenuEditAction();
		void				OnContextMenuCopyAction();
};

#endif
