//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDTransformationManagerToolbar.cpp
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

#include "ZEDTransformationManagerToolbar.h"

#include "ui_ZEDTransformationManagerToolbar.h"
#include "ZEDTransformationManager.h"

void ZEDTransformationManagerToolbar::UpdateUI()
{
	setEnabled(TransformationManager != NULL);

	if (TransformationManager == NULL)
		return;
	
	Form->btnSelect->blockSignals(true);
	Form->btnMove->blockSignals(true);
	Form->btnRotate->blockSignals(true);
	Form->btnScale->blockSignals(true);
	Form->cmbSpace->blockSignals(true);
	Form->cmbPivot->blockSignals(true);
	Form->txtX->blockSignals(true);
	Form->txtY->blockSignals(true);
	Form->txtZ->blockSignals(true);

	ZEDTransformType TransformType = TransformationManager->GetTransformType();
	Form->btnSelect->setChecked(TransformType == ZED_TT_NONE);
	Form->btnMove->setChecked(TransformType == ZED_TT_TRANSLATE);
	Form->btnRotate->setChecked(TransformType == ZED_TT_ROTATE);
	Form->btnScale->setChecked(TransformType == ZED_TT_SCALE);
	

	switch (TransformationManager->GetTransformSpace())
	{
		default:
		case ZED_TS_WORLD:
			Form->cmbSpace->setCurrentText("World Space");
			break;

		case ZED_TS_LOCAL:
			Form->cmbSpace->setCurrentText("Local Space");
			break;


		case ZED_TS_PARENT:
			Form->cmbSpace->setCurrentText("Parent Space");
			break;

		case ZED_TS_VIEW:
			Form->cmbSpace->setCurrentText("View Space");
			break;
	}

	switch (TransformationManager->GetTransformPivot())
	{
		default:
		case ZED_TP_OBJECT:
			Form->cmbPivot->setCurrentText("Object Pivot");
			break;

		case ZED_TP_FOCUSED_OBJECT:
			Form->cmbPivot->setCurrentText("Focused Object Pivot");
			break;

		case ZED_TP_FIRST_OBJECT:
			Form->cmbPivot->setCurrentText("First Object Pivot");
			break;

		case ZED_TP_LAST_OBJECT:
			Form->cmbPivot->setCurrentText("Last Object Pivot");
			break;

		case ZED_TP_CENTER:
			Form->cmbPivot->setCurrentText("Center Pivot");
			break;

		case ZED_TP_WORLD:
			Form->cmbPivot->setCurrentText("World Pivot");
			break;
	}

	Form->txtX->setValue(TransformationManager->GetX());
	Form->txtY->setValue(TransformationManager->GetY());
	Form->txtZ->setValue(TransformationManager->GetZ());

	Form->btnSelect->blockSignals(false);
	Form->btnMove->blockSignals(false);
	Form->btnRotate->blockSignals(false);
	Form->btnScale->blockSignals(false);
	Form->cmbSpace->blockSignals(false);
	Form->cmbPivot->blockSignals(false);
	Form->txtX->blockSignals(false);
	Form->txtY->blockSignals(false);
	Form->txtZ->blockSignals(false);
}

void ZEDTransformationManagerToolbar::btnSelect_clicked()
{
	TransformationManager->SetTransformType(ZED_TT_NONE);
}

void ZEDTransformationManagerToolbar::btnMove_clicked()
{
	TransformationManager->SetTransformType(ZED_TT_TRANSLATE);
}

void ZEDTransformationManagerToolbar::btnRotate_clicked()
{
	TransformationManager->SetTransformType(ZED_TT_ROTATE);
}

void ZEDTransformationManagerToolbar::btnScale_clicked()
{
	TransformationManager->SetTransformType(ZED_TT_SCALE);
}

void ZEDTransformationManagerToolbar::cmbSpace_currentIndexChanged(const QString & text)
{
	if (text == "World Space")
		TransformationManager->SetTransformSpace(ZED_TS_WORLD);
	else if (text == "Local Space")
		TransformationManager->SetTransformSpace(ZED_TS_LOCAL);
	else if (text == "Parent Space")
		TransformationManager->SetTransformSpace(ZED_TS_PARENT);
	else if (text == "View Space")
		TransformationManager->SetTransformSpace(ZED_TS_VIEW);
	else if (text == "Object Pivot")
		TransformationManager->SetTransformPivot(ZED_TP_OBJECT);
	else
		TransformationManager->SetTransformSpace(ZED_TS_WORLD);
}

void ZEDTransformationManagerToolbar::cmbPivot_currentIndexChanged(const QString & text)
{

	if (text == "Object Pivot")
		TransformationManager->SetTransformPivot(ZED_TP_OBJECT);
	else if (text == "Focused Object Pivot")
		TransformationManager->SetTransformPivot(ZED_TP_FOCUSED_OBJECT);
	else if (text == "First Object Pivot")
		TransformationManager->SetTransformPivot(ZED_TP_FIRST_OBJECT);
	else if (text == "Last Object Pivot")
		TransformationManager->SetTransformPivot(ZED_TP_LAST_OBJECT);
	else if (text == "Center Pivot")
		TransformationManager->SetTransformPivot(ZED_TP_CENTER);
	else if (text == "World Pivot")
		TransformationManager->SetTransformPivot(ZED_TP_WORLD);
	else
		TransformationManager->SetTransformPivot(ZED_TP_OBJECT);
}

void ZEDTransformationManagerToolbar::txtX_valueChanged(double d)
{
	TransformationManager->SetX(d);
}

void ZEDTransformationManagerToolbar::txtY_valueChanged(double d)
{
	TransformationManager->SetY(d);
}

void ZEDTransformationManagerToolbar::txtZ_valueChanged(double d)
{
	TransformationManager->SetZ(d);
}

void ZEDTransformationManagerToolbar::SetTransformManager(ZEDTransformationManager* Manager)
{
	if (TransformationManager == Manager)
		return;
	
	TransformationManager = Manager;
	
	UpdateUI();
}

ZEDTransformationManager* ZEDTransformationManagerToolbar::GetTransformManager()
{
	return TransformationManager;
}

ZEDTransformationManagerToolbar::ZEDTransformationManagerToolbar(QWidget* Parent) : QToolBar(Parent)
{
	Form = new Ui_ZEDTransformManagerToolbar();
	Form->setupUi(this);

	TransformationManager = NULL;

	connect(Form->btnSelect, SIGNAL(clicked()), this, SLOT(btnSelect_clicked()));
	connect(Form->btnMove, SIGNAL(clicked()), this, SLOT(btnMove_clicked()));
	connect(Form->btnRotate, SIGNAL(clicked()), this, SLOT(btnRotate_clicked()));
	connect(Form->btnScale, SIGNAL(clicked()), this, SLOT(btnScale_clicked()));
	connect(Form->cmbSpace, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(cmbSpace_currentIndexChanged(const QString&)));
	connect(Form->cmbPivot, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(cmbPivot_currentIndexChanged(const QString&)));
	connect(Form->txtX, SIGNAL(valueChanged(double)), this, SLOT(txtX_valueChanged(double)));
	connect(Form->txtY, SIGNAL(valueChanged(double)), this, SLOT(txtY_valueChanged(double)));
	connect(Form->txtZ, SIGNAL(valueChanged(double)), this, SLOT(txtZ_valueChanged(double)));
}

ZEDTransformationManagerToolbar::~ZEDTransformationManagerToolbar()
{
	delete Form;
}
