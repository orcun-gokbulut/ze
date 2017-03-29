//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDToolbarOptionsPage.h
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

#include <QDialog>

#include "ZETypes.h"
#include "ZEDOptionsPage.h"

class Ui_ZEDToolbarOptionsPageWidget;
class ZEDToolbar;
class ZEDMainWindow;
class ZEDToolbarOptionsPage;

class ZEDToolbarOptionsPageWidget : public QWidget
{
	Q_OBJECT
	private:
		Ui_ZEDToolbarOptionsPageWidget*		Form;
		ZEDMainWindow*						MainWindow;
		ZEDToolbarOptionsPage*				Page;

		void								UpdateElements();
		void								UpdateElement();
		void								UpdateElementItems();
		void								UpdateItems();
		void								UpdateItemCategories();
		void								UpdateUI();

		ZEDToolbar*							GetToolbar();
		ZEInt								GetElementItemSelectionIndex();

	private slots:
		void								btnElementNew_clicked();
		void								btnElementDelete_clicked();
		void								cmbElements_currentIndexChanged(int);
		void								txtElementName_textChanged(const QString&);
		void								txtElementText_textChanged(const QString&);
		void								chkElementVisible_toggled(bool);
		void								btnElementItemAdd_clicked();
		void								btnElementItemRemove_clicked();
		void								btnElementItemUp_clicked();
		void								btnElementItemDown_clicked();
		void								lstElementItems_itemSelectionChanged();
		void								cmbItemCategory_currentIndexChanged(int);
		void								lstItems_itemSelectionChanged();

	public:
		void								Default();
		void								Save();

											ZEDToolbarOptionsPageWidget(QWidget* Parent, ZEDMainWindow* Window, ZEDToolbarOptionsPage* Page);
											~ZEDToolbarOptionsPageWidget();
};

class ZEDToolbarOptionsPage : public ZEDOptionsPage
{
	public:
		virtual const char*					GetName() const override;
		virtual const char*					GetText() const override;
		virtual const char*					GetPath() const override;

		virtual QWidget*					CreateWidget(QWidget* Parent) override;

		virtual void						Save(QWidget* Widget) override;
		virtual void						Default(QWidget* Widget) override;
		virtual void						Close(QWidget* Widget) override;
};
