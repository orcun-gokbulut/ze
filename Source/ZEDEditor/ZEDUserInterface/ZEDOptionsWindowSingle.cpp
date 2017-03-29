//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDOptionsWindowSingle.cpp
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

#include "ZEDOptionsWindowSingle.h"

#include "ZEDOptionsPage.h"
#include "ui_ZEDOptionsWindowSingle.h"

#include <QMessageBox>

void ZEDOptionsWindowSingle::btnDefault_clicked()
{
	OptionsPage->Default(OptionsPageWidget);
}

void ZEDOptionsWindowSingle::btnSave_clicked()
{
	OptionsPage->Save(OptionsPageWidget);
}

void ZEDOptionsWindowSingle::btnClose_clicked()
{
	if (OptionsPage->IsModified())
	{
		int Result = QMessageBox::question(this, "Options",
			"There are some unsaved modifications in the options.\n"
			"Are you sure you want to close without saving them ?");

		if (Result == QMessageBox::No)
			return;
	}

	OptionsPage->Close(OptionsPageWidget);
	close();
}

void ZEDOptionsWindowSingle::Setup(ZEDOptionsPage* Page)
{
	OptionsPage = Page;
	OptionsPageWidget = Page->CreateWidget(Form->widOptionsPage);
	QVBoxLayout* Layout = new QVBoxLayout(Form->widOptionsPage);
	Layout->setMargin(0);
	Layout->addWidget(OptionsPageWidget);
	setLayout(Layout);
}

ZEDOptionsWindowSingle::ZEDOptionsWindowSingle(QWidget* Parent) : QDialog(Parent)
{
	Form = new Ui_ZEDOptionsWindowSingle();
	Form->setupUi(this);

	OptionsPage = NULL;
	OptionsPageWidget = NULL;

	connect(Form->btnDefault, SIGNAL(clicked()), this, SLOT(btnDefault_clicked()));
	connect(Form->btnSave, SIGNAL(clicked()), this, SLOT(btnSave_clicked()));
	connect(Form->btnClose, SIGNAL(clicked()), this, SLOT(btnClose_clicked()));
}

ZEDOptionsWindowSingle::~ZEDOptionsWindowSingle()
{
	delete Form;
}
