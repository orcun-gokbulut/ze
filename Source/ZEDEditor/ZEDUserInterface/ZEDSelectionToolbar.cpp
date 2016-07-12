//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSelectionToolbar.cpp
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

#include "ZEDSelectionToolbar.h"

#include "ZEDCore/ZEDEditor.h"
#include "ZEDCore/ZEDSelectionManager.h"
#include "ZEDUserInterfaceComponent.h"
#include "ZEDMainWindow.h"

#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QComboBox>
#include <QMessageBox>
#include <QMainWindow>

void ZEDSelectionToolbar::SetupUI()
{
	actSelectionList = new QAction(this);
	actSelectionList->setText("Selections");
	GetToolbar()->addAction(actSelectionList);

	GetToolbar()->addSeparator();

	cmbShape = new QComboBox();
	cmbShape->addItem("Rectangle");
	cmbShape->addItem("Circle");
	cmbShape->addItem("Brush");
	GetToolbar()->addWidget(cmbShape);

	actFreeze = new QAction(this);
	actFreeze->setText("Freeze");
	actFreeze->setCheckable(true);
	GetToolbar()->addAction(actFreeze);

	actUnfreezeAll = new QAction(this);
	actUnfreezeAll->setText("Unfreeze All");
	GetToolbar()->addAction(actUnfreezeAll);

	cmbMode = new QComboBox();
	cmbMode->clear();
	cmbMode->addItem("Fully Inside");
	cmbMode->addItem("Partially Inside");
	GetToolbar()->addWidget(cmbMode);
}

void ZEDSelectionToolbar::UpdateUI()
{
	GetToolbar()->setEnabled(GetSelectionManager() != NULL && GetEditor()->GetFileState() != ZED_ES_NONE);
	
	if (GetSelectionManager() == NULL)
		return;

	QSignalBlocker cmbShapeBlocker(cmbShape);
	QSignalBlocker cmbModeBlocker(cmbMode);

	switch (GetSelectionManager()->GetSelectionMode())
	{
		default:
		case ZED_SS_RECTANGLE:
			cmbMode->setCurrentText("Rectangle");
			break;

		case ZED_SS_CIRCLE:
			cmbMode->setCurrentText("Circle");
			break;

		case ZED_SS_BRUSH:
			cmbMode->setCurrentText("Brush");
			break;
	}

	switch (GetSelectionManager()->GetSelectionMode())
	{
		default:
		case ZE_SM_FULLY_INSIDE:
			cmbMode->setCurrentText("Fully Inside");
			break;

		case ZE_SM_PARTIALY_INSIDE:
			cmbMode->setCurrentText("Partially Inside");
			break;
	}
}

ZEDSelectionToolbar::ZEDSelectionToolbar()
{
	SetName("Selection");
	SetupUI();

	connect(actSelectionList, SIGNAL(triggered()), this, SLOT(actSelectionList_triggered()));
	connect(cmbShape, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(cmbShape_currentIndexChanged(const QString&)));
	connect(cmbMode, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(cmbMode_currentIndexChanged(const QString&)));
	connect(actFreeze, SIGNAL(triggered()), this, SLOT(actFreeze_triggered()));
	connect(actUnfreezeAll, SIGNAL(triggered()), this, SLOT(actUnfreezeAll_triggered()));
}

ZEDSelectionToolbar::~ZEDSelectionToolbar()
{

}

void ZEDSelectionToolbar::actSelectionList_triggered()
{
	QMessageBox::information(GetMainWindow()->GetMainWindow(), "Zinek", "Not implemented", QMessageBox::Ok);
}

void ZEDSelectionToolbar::cmbShape_currentIndexChanged(const QString & text)
{
	if (GetSelectionManager() == NULL)
		return;

	if (text == "Rectangle")
		GetSelectionManager()->SetSelectionShape(ZED_SS_RECTANGLE);
	else if (text == "Circle")
		GetSelectionManager()->SetSelectionShape(ZED_SS_CIRCLE);
	else if (text == "Brush")
		GetSelectionManager()->SetSelectionShape(ZED_SS_BRUSH);
	else
		GetSelectionManager()->SetSelectionShape(ZED_SS_RECTANGLE);
}

void ZEDSelectionToolbar::cmbMode_currentIndexChanged(const QString & text)
{
	if (GetSelectionManager() == NULL)
		return;

	if (text == "Fully Inside")
		GetSelectionManager()->SetSelectionMode(ZE_SM_FULLY_INSIDE);
	else if (text == "Partially Inside")
		GetSelectionManager()->SetSelectionMode(ZE_SM_PARTIALY_INSIDE);
	else
		GetSelectionManager()->SetSelectionMode(ZE_SM_PARTIALY_INSIDE);
}

void ZEDSelectionToolbar::actFreeze_triggered()
{
	QMessageBox::information(GetMainWindow()->GetMainWindow(), "Zinek", "Not implemented", QMessageBox::Ok);
}

void ZEDSelectionToolbar::actUnfreezeAll_triggered()
{
	QMessageBox::information(GetMainWindow()->GetMainWindow(), "Zinek", "Not implemented", QMessageBox::Ok);
}

ZEDSelectionManager* ZEDSelectionToolbar::GetSelectionManager()
{
	if (GetEditor() == NULL)
		return NULL;

	return GetEditor()->GetSelectionManager();
}

ZEDSelectionToolbar* ZEDSelectionToolbar::CreateInstance()
{
	return new ZEDSelectionToolbar();
}
