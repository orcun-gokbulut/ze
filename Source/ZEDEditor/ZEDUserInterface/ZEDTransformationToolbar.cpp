//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDTransformationToolbar.cpp
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

#include "ZEDTransformationToolbar.h"

#include "ZEDCore/ZEDEditor.h"
#include "ZEDCore/ZEDTransformationManager.h"
#include "ZEDCore/ZEDTransformationEvent.h"

#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>


void ZEDTransformationToolbar::SetupUI()
{
	Toolbar = new QToolBar();

	ActionGroup = new QActionGroup(this);

	actSelect = new QAction(this);
	actSelect->setText("Select");
	actSelect->setCheckable(true);
	ActionGroup->addAction(actSelect);
	Toolbar->addAction(actSelect);

	actMove = new QAction(this);
	actMove->setText("Move");
	actMove->setCheckable(true);
	ActionGroup->addAction(actMove);
	Toolbar->addAction(actMove);
	
	actRotate = new QAction(this);
	actRotate->setText("Rotate");
	actRotate->setCheckable(true);
	ActionGroup->addAction(actRotate);
	Toolbar->addAction(actRotate);

	actScale = new QAction(this);
	actScale->setText("Scale");
	actScale->setCheckable(true);
	ActionGroup->addAction(actScale);
	Toolbar->addAction(actScale);

	Toolbar->addSeparator();

	cmbSpace = new QComboBox();
	cmbSpace->clear();
	cmbSpace->addItem("World Space");
	cmbSpace->addItem("Local Space");
	cmbSpace->addItem("Parent Space");
	cmbSpace->addItem("View Space");
	Toolbar->addWidget(cmbSpace);

	cmbPivot = new QComboBox();
	cmbPivot->clear();
	cmbPivot->addItem("Object Pivot");
	cmbPivot->addItem("Focused Object Pivot");
	cmbPivot->addItem("Center Pivot");
	cmbPivot->addItem("World Pivot");
	Toolbar->addWidget(cmbPivot);
	
	Toolbar->addSeparator();

	QLabel* lblX = new QLabel();
	lblX->setText("x:");
	Toolbar->addWidget(lblX);

	txtX = new QDoubleSpinBox();
	Toolbar->addWidget(txtX);

	QLabel* lblY = new QLabel();
	lblY->setText("y:");
	Toolbar->addWidget(lblY);

	txtY = new QDoubleSpinBox();
	Toolbar->addWidget(txtY);

	QLabel* lblZ = new QLabel();
	lblZ->setText("z:");
	Toolbar->addWidget(lblZ);

	txtZ = new QDoubleSpinBox();
	Toolbar->addWidget(txtZ);
}

void ZEDTransformationToolbar::UpdateUI()
{
	Toolbar->setEnabled(GetTransformationManager() != NULL);

	if (GetTransformationManager() == NULL)
		return;
	
	QSignalBlocker btnSelectBlocker(actSelect);
	QSignalBlocker btnMoveBlocker(actMove);
	QSignalBlocker btnRotatBlocker(actRotate);
	QSignalBlocker btnScaleBlocker(actScale);
	QSignalBlocker cmbSpaceBlocker(cmbSpace);
	QSignalBlocker cmbPivotBlocker(cmbPivot);
	QSignalBlocker txtXBlocker(txtX);
	QSignalBlocker txtYBlocker(txtY);
	QSignalBlocker txtZBlocker(txtZ);

	ZEDTransformType TransformType = GetTransformationManager()->GetTransformType();
	actSelect->setChecked(TransformType == ZED_TT_NONE);
	actMove->setChecked(TransformType == ZED_TT_TRANSLATE);
	actRotate->setChecked(TransformType == ZED_TT_ROTATE);
	actScale->setChecked(TransformType == ZED_TT_SCALE);
	
	switch (GetTransformationManager()->GetTransformSpace())
	{
		default:
		case ZED_TS_WORLD:
			cmbSpace->setCurrentText("World Space");
			break;

		case ZED_TS_LOCAL:
			cmbSpace->setCurrentText("Local Space");
			break;


		case ZED_TS_PARENT:
			cmbSpace->setCurrentText("Parent Space");
			break;

		case ZED_TS_VIEW:
			cmbSpace->setCurrentText("View Space");
			break;
	}

	switch (GetTransformationManager()->GetTransformPivot())
	{
		default:
		case ZED_TP_OBJECT:
			cmbPivot->setCurrentText("Object Pivot");
			break;

		case ZED_TP_FOCUSED_OBJECT:
			cmbPivot->setCurrentText("Focused Object Pivot");
			break;

		case ZED_TP_CENTER:
			cmbPivot->setCurrentText("Center Pivot");
			break;

		case ZED_TP_WORLD:
			cmbPivot->setCurrentText("World Pivot");
			break;
	}

	bool Result = true;
	bool Valid = false;
	txtX->setValue(GetTransformationManager()->GetX(Valid));
	Result &= Valid;
	txtY->setValue(GetTransformationManager()->GetY(Valid));
	Result &= Valid;
	txtZ->setValue(GetTransformationManager()->GetZ(Valid));
	Result &= Valid;

	txtX->setEnabled(Result);
	txtY->setEnabled(Result);
	txtZ->setEnabled(Result);
}

void ZEDTransformationToolbar::TransformationEvent(const ZEDTransformationEvent* Event)
{
	if (Event->GetType() == ZED_TET_MANAGER_STATE_CHANGED)
		UpdateUI();
}

ZEDTransformationToolbar::ZEDTransformationToolbar()
{
	SetName("Transformation");
	SetupUI();

	connect(actSelect, SIGNAL(triggered()),	this, SLOT(actSelect_triggered()));
	connect(actMove, SIGNAL(triggered()),	this, SLOT(actMove_triggered()));
	connect(actRotate, SIGNAL(triggered()),	this, SLOT(actRotate_triggered()));
	connect(actScale, SIGNAL(triggered()),	this, SLOT(actScale_triggered()));
	connect(cmbSpace, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(cmbSpace_currentIndexChanged(const QString&)));
	connect(cmbPivot, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(cmbPivot_currentIndexChanged(const QString&)));
	connect(txtX, SIGNAL(valueChanged(double)), this, SLOT(txtX_valueChanged(double)));
	connect(txtY, SIGNAL(valueChanged(double)), this, SLOT(txtY_valueChanged(double)));
	connect(txtZ, SIGNAL(valueChanged(double)), this, SLOT(txtZ_valueChanged(double)));
}

ZEDTransformationToolbar::~ZEDTransformationToolbar()
{
	delete Toolbar;
}

void ZEDTransformationToolbar::actSelect_triggered()
{
	if (GetTransformationManager() == NULL)
		return;

	GetTransformationManager()->SetTransformType(ZED_TT_NONE);
}

void ZEDTransformationToolbar::actMove_triggered()
{
	if (GetTransformationManager() == NULL)
		return;

	GetTransformationManager()->SetTransformType(ZED_TT_TRANSLATE);
}

void ZEDTransformationToolbar::actRotate_triggered()
{
	if (GetTransformationManager() == NULL)
		return;

	GetTransformationManager()->SetTransformType(ZED_TT_ROTATE);
}

void ZEDTransformationToolbar::actScale_triggered()
{
	GetTransformationManager()->SetTransformType(ZED_TT_SCALE);
}

void ZEDTransformationToolbar::cmbSpace_currentIndexChanged(const QString & text)
{
	if (GetTransformationManager() == NULL)
		return;

	if (text == "World Space")
		GetTransformationManager()->SetTransformSpace(ZED_TS_WORLD);
	else if (text == "Local Space")
		GetTransformationManager()->SetTransformSpace(ZED_TS_LOCAL);
	else if (text == "Parent Space")
		GetTransformationManager()->SetTransformSpace(ZED_TS_PARENT);
	else if (text == "View Space")
		GetTransformationManager()->SetTransformSpace(ZED_TS_VIEW);
	else if (text == "Object Pivot")
		GetTransformationManager()->SetTransformPivot(ZED_TP_OBJECT);
	else
		GetTransformationManager()->SetTransformSpace(ZED_TS_WORLD);
}

void ZEDTransformationToolbar::cmbPivot_currentIndexChanged(const QString & text)
{
	if (GetTransformationManager() == NULL)
		return;

	if (text == "Object Pivot")
		GetTransformationManager()->SetTransformPivot(ZED_TP_OBJECT);
	else if (text == "Focused Object Pivot")
		GetTransformationManager()->SetTransformPivot(ZED_TP_FOCUSED_OBJECT);
	else if (text == "Center Pivot")
		GetTransformationManager()->SetTransformPivot(ZED_TP_CENTER);
	else if (text == "World Pivot")
		GetTransformationManager()->SetTransformPivot(ZED_TP_WORLD);
	else
		GetTransformationManager()->SetTransformPivot(ZED_TP_OBJECT);
}

void ZEDTransformationToolbar::txtX_valueChanged(double d)
{
	if (GetTransformationManager() == NULL)
		return;

	GetTransformationManager()->SetX(d);
}

void ZEDTransformationToolbar::txtY_valueChanged(double d)
{
	if (GetTransformationManager() == NULL)
		return;

	GetTransformationManager()->SetY(d);
}

void ZEDTransformationToolbar::txtZ_valueChanged(double d)
{
	if (GetTransformationManager() == NULL)
		return;

	GetTransformationManager()->SetZ(d);
}

QToolBar* ZEDTransformationToolbar::GetToolbar()
{
	return Toolbar;
}

ZEDTransformationManager* ZEDTransformationToolbar::GetTransformationManager()
{
	if (GetEditor() == NULL)
		return false;

	return GetEditor()->GetTransformationManager();
}

ZEDTransformationToolbar* ZEDTransformationToolbar::CreateInstance()
{
	return new ZEDTransformationToolbar();
}
