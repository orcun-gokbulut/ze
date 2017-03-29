//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDOptionsWindowMultiple.cpp
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

#include "ZEDOptionsWindowMultiple.h"

#include "ZEDOptionsPage.h"
#include "ui_ZEDOptionsWindowMultiple.h"

#include <QMessageBox>

void ZEDOptionsWindowMultiple::btnDefault_clicked()
{
	for (ZESize I = 0; I < OptionsPages.GetCount(); I++)
		OptionsPages[I]->Default(OptionsPageWidgets[I]);
}

void ZEDOptionsWindowMultiple::btnSave_clicked()
{
	for (ZESize I = 0; I < OptionsPages.GetCount(); I++)
		OptionsPages[I]->Save(OptionsPageWidgets[I]);
}

void ZEDOptionsWindowMultiple::btnClose_clicked()
{
	bool Modified = false;
	for (ZESize I = 0; I < OptionsPages.GetCount(); I++)
	{
		if (OptionsPages[I]->IsModified())
		{
			Modified = true;
			break;
		}
	}

	if (Modified)
	{
		int Result = QMessageBox::question(this, "Options",
			"There are some unsaved modifications in the options.\n"
			"Are you sure you want to close without saving them ?");

		if (Result == QMessageBox::No)
			return;
	}

	for (ZESize I = 0; I < OptionsPages.GetCount(); I++)
		OptionsPages[I]->Close(OptionsPageWidgets[I]);

	close();
}

void ZEDOptionsWindowMultiple::Setup(const ZEArray<ZEDOptionsPage*>& Pages)
{
	OptionsPages = Pages;
	OptionsPageWidgets.SetCount(Pages.GetCount());
	for (ZESize I = 0; I < Pages.GetCount(); I++)
	{
		QWidget* Wrapper = new QWidget(Form->tabOptionPages);
		Form->tabOptionPages->addTab(Wrapper, QIcon(), Pages[I]->GetText());

		QVBoxLayout* Layout = new QVBoxLayout(Wrapper);
		setLayout(Layout);

		OptionsPageWidgets[I] = Pages[I]->CreateWidget(Wrapper);
		Layout->addWidget(OptionsPageWidgets[I]);
	}
}

ZEDOptionsWindowMultiple::ZEDOptionsWindowMultiple(QWidget* Parent) : QDialog(Parent)
{
	Form = new Ui_ZEDOptionsWindowMultiple();
	Form->setupUi(this);

	connect(Form->btnDefault, SIGNAL(clicked()), this, SLOT(btnDefault_clicked()));
	connect(Form->btnSave, SIGNAL(clicked()), this, SLOT(btnSave_clicked()));
	connect(Form->btnClose, SIGNAL(clicked()), this, SLOT(btnClose_clicked()));
}

ZEDOptionsWindowMultiple::~ZEDOptionsWindowMultiple()
{
	delete Form;
}
