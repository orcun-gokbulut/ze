//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELNIntegrityModule.cpp
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

#include "ZELNIntegrityModule.h"
#include "ui_ZELNIntegrityWidget.h"

#include "ZEError.h"
#include "ZEML/ZEMLReader.h"

#include <md5.h>

ZELN_MODULE_DECRIPTION(ZELNIntegrityModule, "Integrity");

void ZELNIntegrityModule::UpdateRecord(ZESize Index)
{
	const ZEITIntegrityRecord& Record = Checker.GetRecords()[Index];

	if (Form->tblRecords->item(Index, 0) == NULL)
	{
		Form->tblRecords->setItem(Index, 0, new QTableWidgetItem());
		Form->tblRecords->item(Index, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		Form->tblRecords->setItem(Index, 1, new QTableWidgetItem());
		Form->tblRecords->item(Index, 1)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	}

	switch (Record.GetResult())
	{
		default:
		case ZETI_CR_NOT_CHECKED:
			Form->tblRecords->item(Index, 0)->setText("Not Checked");
			Form->tblRecords->item(Index, 0)->setTextColor(Qt::black);
			break;

		case ZEIT_CR_SUCCESS:
			Form->tblRecords->item(Index, 0)->setText("Success");
			Form->tblRecords->item(Index, 0)->setTextColor(Qt::green);
			break;

		case ZEIT_CR_MISSING:
			Form->tblRecords->item(Index, 0)->setText("Missing");
			Form->tblRecords->item(Index, 0)->setTextColor(Qt::red);
			break;

		case ZEIT_CR_CHECKSUM_FAILED:
			Form->tblRecords->item(Index, 0)->setText("Checksum Failed");
			Form->tblRecords->item(Index, 0)->setTextColor(Qt::red);
			break;
	}

	Form->tblRecords->item(Index, 1)->setText(Record.GetPath().ToCString());

	switch(Checker.GetResult())
	{
		default:
		case ZEIT_ICR_NONE:
			Form->lblStatus->setText("Unknown");
			break;

		case ZEIT_ICR_SUCESS:
			Form->lblStatus->setText("Success");
			break;

		case ZEIT_ICR_PARTIAL_SUCESS:
			Form->lblStatus->setText("Partial Success");
			break;

		case ZEIT_ICR_FAILED:
			Form->lblStatus->setText("Failed");
			break;
	}
}

void ZELNIntegrityModule::Update()
{
	Form->tblRecords->setRowCount(Checker.GetRecords().GetCount());

	for (ZESize I = 0; I < Form->tblRecords->rowCount(); I++)
		UpdateRecord(I);
}

bool ZELNIntegrityModule::InitializeSelf()
{
	if (!ZEInitializable::InitializeSelf())
		return false;

	Widget = new QWidget();
	Form = new Ui_ZELNIntegrityWidget();
	Form->setupUi(Widget);
	
	connect(Form->btnCheckIntegrity, SIGNAL(clicked()), this, SLOT(btnCheckIntegrity_clicked()));

	Checker.SetIntegrityFile("#S:/Integrity.ZEIntegrity");
	Checker.Load();
	Update();

	QTableWidgetItem* StatusHeader = new QTableWidgetItem("Status");
	StatusHeader->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	QTableWidgetItem* FileNameHeader = new QTableWidgetItem("Status");
	FileNameHeader->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	
	//Form->tblRecords->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

	return true;
}

void ZELNIntegrityModule::btnCheckIntegrity_clicked()
{
	if (!State)
	{
		Checker.CheckStart();
		Form->prgProgress->setValue(0);
		Form->prgProgress->setMaximum(Checker.GetRecords().GetCount());

		for (ZESize I = 0; I < Checker.GetRecords().GetCount(); I++)
		{
			ZESSize Index = Checker.CheckProgress();
			UpdateRecord(Index);
			qApp->processEvents();
		}
	}
}

QWidget* ZELNIntegrityModule::GetWidget()
{
	return Widget;
}
