//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAssertBrowser.h
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

#ifndef __ZED_ASSERT_BROWSER_H__
#define __ZED_ASSERT_BROWSER_H__

#include <QWidget>
#include <QDir>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QMenuBar>
#include <QMenu>
#include <QFrame>
#include <QtGui>
#include <QGridLayout>
#include <QScrollArea>
#include <QToolBar>
#include <QPixmap>

#include "ZEDAssertBrowserViewPortItem.h"
#include "ZEDPlugIns\ZEDPlugIn.h"
#include "ZEDAssertBrowserPlugIns\ZEDAssertBrowserPlugIn.h"

class ZEDAssertBrowser : public QWidget
{
	Q_OBJECT

	friend class ZEDAssertBrowserViewPortItem;

	private:

		QHBoxLayout*									HMainLayout;
		QVBoxLayout*									VMainLayout;
		QGridLayout*									GViewPortLayout;
		QTreeView*										DirectoryBrowser;
		QMenuBar*										MainMenu;
		QFrame*											CentralFrame;
		QScrollArea*									BrowserViewPort;
		QMenu*											CreateResourceMenu;
		QMenu*											PreviewOptionsMenu;
		QMenu*											OperationsMenu;
		QDirModel*										MainModel;
		QDir*											MainDir;
		QWidget*										BrowserViewPortIn;
		QVBoxLayout*									VBoxLayout;
		QWidget*										ToolWidgetArea;
		QLineEdit*										SearchLineEdit;
		QComboBox*										FilterComboBox;
		QLabel*											SearchLabel;
		QLabel*											FilterLabel;
		QPushButton*									RefreshButton;
		QPushButton*									BackButton;
		QHBoxLayout*									ToolBarLayout;
		QString											WorkingDirectory;

		
		int												PreviewSize;

		QList<ZEDAssertBrowserViewPortItem*>			CurrentViewPortContents;
		ZEArray<QString>								ItemsToCopy;
		QList<ZEDFilePlugIn*>							PlugIns;

	public:

		ZEDFilePlugIn*									RegisterFilePlugIn(ZEDFilePlugIn* PlugIn);
														ZEDAssertBrowser(QString WorkingDirectory);

		public slots:

		void											GenerateBrowserViewPortItems();
		void											LargeSelected();
		void											MediumSelected();
		void											SmallSelected();
		void											SelectAll();
		void											DeselectAll();
		void											MassDelete();
		void											MassCopy();
		void											CopyFile(QString Source, QString DestinationFolder);
		void											Paste();
		void											RefreshDirectories();
		void											BackButtonClicked();

	protected:

		void											resizeEvent(QResizeEvent* Event);

};

#endif
