//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - NewZEDAssertBrowser.h
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

#ifndef __NEW_ZED_ASSERT_BROWSER_H__
#define __NEW_ZED_ASSERT_BROWSER_H__

#include "ui_ZEDAssertBrowser.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDirModel>
#include <QGridLayout>
#include <QFile>

#include <zeds/ZEArray.h>

#include "../ZEDAssertBrowserPlugIns/ZEDAssertBrowserPlugIn.h"
#include "NewZEDAssertBrowserItem.h"

class NewZEDAssertBrowser : public QMainWindow
{
	Q_OBJECT

	friend class NewZEDAssertBrowserItem;

	private:

		Ui::ZEDAssertBrowserUI				AssertBrowserUI;

		ZEArray<ZEDFilePlugIn*>				PlugIns;
		ZEArray<NewZEDAssertBrowserItem*>	ViewportContents;
		ZEArray<NewZEDAssertBrowserItem*>	SelectedViewportItems;
		QList<QFile*>						FilesToManipulate;

		bool								CutOperationFlag;

		QLabel*								FilterLabel;
		QLabel*								SearchLabel;
		QLineEdit*							SearchLineEdit;
		QComboBox*							FilterComboBox;
		QGridLayout*						PreviewsLayout;

		QDirModel*							DirectoryModel;	

		int									PreviewSize;
		bool								MutipleSelectionEnabled;


		void								InitializeSearchToolBar();
		void								AddSelectedItem(NewZEDAssertBrowserItem* SelectedItem);
		void								RemoveSelectedItem(NewZEDAssertBrowserItem* SelectedItem);

	public slots:

		void								GeneratePreviewItems();
		void								SmallClicked();
		void								MediumClicked();
		void								LargeClicked();

		void								CutActionTriggered();
		void								CopyActionTriggered();
		void								PasteActionTriggered();
		void								DeleteActionTriggered();
		void								EditActionTriggered();

		void								SelectAllActionTriggered();
		void								DesellectActionTriggered();

	public:

		void								RegisterPlugIn(ZEDFilePlugIn* PlugIn);
		void								SetMultipleSelection(bool Enabled);

											NewZEDAssertBrowser(QString WorkingDirectory, QWidget *Parent = 0, Qt::WFlags Flags = 0);
};

#endif
