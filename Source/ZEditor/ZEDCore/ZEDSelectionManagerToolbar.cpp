//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSelectionManagerToolbar.cpp
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

#include "ZEDSelectionManagerToolbar.h"

#include "ui_ZEDSelectionManagerToolbar.h"
#include "ZEDSelectionManager.h"

#include <QMessageBox>

void ZEDSelectionManagerToolbar::UpdateUI()
{
	setEnabled(SelectionManager != NULL);

	if (SelectionManager == NULL)
		return;

	Form->cmbShape->blockSignals(true);
	Form->cmbMode->blockSignals(true);
	Form->btnFreeze->blockSignals(true);

	switch (SelectionManager->GetSelectionMode())
	{
		default:
		case ZED_SS_RECTANGLE:
			Form->cmbMode->setCurrentText("Rectangle");
			break;

		case ZED_SS_CIRCLE:
			Form->cmbMode->setCurrentText("Circle");
			break;

		case ZED_SS_BRUSH:
			Form->cmbMode->setCurrentText("Brush");
			break;
	}

	switch (SelectionManager->GetSelectionMode())
	{
		default:
		case ZE_SM_FULLY_INSIDE:
			Form->cmbMode->setCurrentText("Fully Inside");
			break;

		case ZE_SM_PARTIALY_INSIDE:
			Form->cmbMode->setCurrentText("Partially Inside");
			break;
	}

	Form->cmbShape->blockSignals(false);
	Form->cmbMode->blockSignals(false);
	Form->btnFreeze->blockSignals(false);
}

void ZEDSelectionManagerToolbar::btnSelectionList_clicked()
{
	QMessageBox::information(this, "Zinek", "Not implemented", QMessageBox::Ok);
}

void ZEDSelectionManagerToolbar::cmbShape_currentIndexChanged(const QString & text)
{
	if (text == "Rectangle")
		SelectionManager->SetSelectionShape(ZED_SS_RECTANGLE);
	else if (text == "Circle")
		SelectionManager->SetSelectionShape(ZED_SS_CIRCLE);
	else if (text == "Brush")
		SelectionManager->SetSelectionShape(ZED_SS_BRUSH);
	else
		SelectionManager->SetSelectionShape(ZED_SS_RECTANGLE);
}

void ZEDSelectionManagerToolbar::cmbMode_currentIndexChanged(const QString & text)
{
	if (text == "Fully Inside")
		SelectionManager->SetSelectionMode(ZE_SM_FULLY_INSIDE);
	else if (text == "Partially Inside")
		SelectionManager->SetSelectionMode(ZE_SM_PARTIALY_INSIDE);
	else
		SelectionManager->SetSelectionMode(ZE_SM_PARTIALY_INSIDE);
}

void ZEDSelectionManagerToolbar::btnFreeze_clicked()
{
	QMessageBox::information(this, "Zinek", "Not implemented", QMessageBox::Ok);
}

void ZEDSelectionManagerToolbar::btnUnfreezeAll_clicked()
{
	QMessageBox::information(this, "Zinek", "Not implemented", QMessageBox::Ok);
}

void ZEDSelectionManagerToolbar::SetSelectionManager(ZEDSelectionManager* Manager)
{
	if (SelectionManager == Manager)
		return;

	SelectionManager = Manager;

	UpdateUI();
}

ZEDSelectionManager* ZEDSelectionManagerToolbar::GetSelectionManager()
{
	return SelectionManager;
}

ZEDSelectionManagerToolbar::ZEDSelectionManagerToolbar(QWidget* Parent) : QToolBar(Parent)
{
	Form = new Ui_ZEDSelectionManagerToolbar();
	Form->setupUi(this);

	SelectionManager = NULL;

	connect(Form->btnSelectionList, SIGNAL(clicked()), this, SLOT(btnSelectionList_clicked()));
	connect(Form->cmbShape, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(cmbShape_currentIndexChanged(const QString&)));
	connect(Form->cmbMode, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(cmbMode_currentIndexChanged(const QString&)));
	connect(Form->btnFreeze, SIGNAL(clicked()), this, SLOT(btnFreeze_clicked()));
	connect(Form->btnUnfreezeAll, SIGNAL(clicked()), this, SLOT(btnUnfreezeAll_clicked()));
}

ZEDSelectionManagerToolbar::~ZEDSelectionManagerToolbar()
{
	delete Form;
}
