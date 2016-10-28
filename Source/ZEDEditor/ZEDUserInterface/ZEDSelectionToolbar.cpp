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
#include "ZEDCore/ZEDObjectWrapper.h"
#include "ZEDCore/ZEDSelectionEvent.h"

void ZEDSelectionToolbar::SetupUI()
{
	actSelectionList = new QAction(this);
	actSelectionList->setText("Selections");
	GetToolbar()->addAction(actSelectionList);

	actLockSelection = new QAction(this);
	actLockSelection->setText("Lock Selection");
	actLockSelection->setCheckable(true);
	GetToolbar()->addAction(actLockSelection);

	GetToolbar()->addSeparator();

	cmbSelectionShape = new QComboBox();
	cmbSelectionShape->addItem("Rectangle");
	cmbSelectionShape->addItem("Circle");
	cmbSelectionShape->addItem("Brush");
	GetToolbar()->addWidget(cmbSelectionShape);


	actSelectionMode = new QAction(this);
	actSelectionMode->setText("Intersects");
	actSelectionMode->setCheckable(true);
	GetToolbar()->addAction(actSelectionMode);

	GetToolbar()->addSeparator();

	actFreezeSelection = new QAction(this);
	actFreezeSelection->setText("Freeze");
	GetToolbar()->addAction(actFreezeSelection);

	actUnfreezeAll = new QAction(this);
	actUnfreezeAll->setText("Unfreeze All");
	GetToolbar()->addAction(actUnfreezeAll);

}

void ZEDSelectionToolbar::UpdateUI()
{
	//GetToolbar()->setEnabled(GetSelectionManager() != NULL && GetEditor()->GetFileState() != ZED_ES_NONE);
	
	if (GetSelectionManager() == NULL)
		return;

	QSignalBlocker cmbSelectionShapeBlocker(cmbSelectionShape);
	QSignalBlocker actSelectionModeBlocker(actSelectionMode);

	switch (GetSelectionManager()->GetSelectionShape())
	{
		default:
		case ZED_SS_RECTANGLE:
			cmbSelectionShape->setCurrentText("Rectangle");
			break;

		case ZED_SS_CIRCLE:
			cmbSelectionShape->setCurrentText("Circle");
			break;

		case ZED_SS_BRUSH:
			cmbSelectionShape->setCurrentText("Brush");
			break;
	}

	switch (GetSelectionManager()->GetSelectionMode())
	{
		case ZE_SM_FULLY_COVERS:
			actSelectionMode->setText("Fully Inside");
			break;

		default:
		case ZE_SM_INTERSECTS:
			actSelectionMode->setText("Intersects");
			break;
	}

	if (GetSelectionManager()->GetLockSelection())
	{
		actFreezeSelection->setEnabled(false);
		actUnfreezeAll->setEnabled(false);
	}
	else
	{
		actFreezeSelection->setEnabled(GetSelectionManager()->GetSelection().GetCount());
		actUnfreezeAll->setEnabled(GetSelectionManager()->GetFrozonObjects().GetCount());
	}
}

void ZEDSelectionToolbar::SelectionEvent(const ZEDSelectionEvent* Event)
{
	UpdateUI();
}

ZEDSelectionToolbar::ZEDSelectionToolbar()
{
	SetName("Selection");
	SetupUI();

	connect(actSelectionList, SIGNAL(triggered()), this, SLOT(actSelectionList_triggered()));
	connect(cmbSelectionShape, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(cmbSelectionShape_currentIndexChanged(const QString&)));
	connect(actSelectionMode, SIGNAL(triggered()), this, SLOT(actSelectionMode_triggered()));
	connect(actLockSelection, SIGNAL(triggered()), this, SLOT(actLockSelection_triggered()));
	connect(actFreezeSelection, SIGNAL(triggered()), this, SLOT(actFreeze_triggered()));
	connect(actUnfreezeAll, SIGNAL(triggered()), this, SLOT(actUnfreezeAll_triggered()));
}

ZEDSelectionToolbar::~ZEDSelectionToolbar()
{

}

void ZEDSelectionToolbar::actSelectionList_triggered()
{
	QMessageBox::information(GetMainWindow()->GetMainWindow(), "Zinek", "Not implemented", QMessageBox::Ok);
}

void ZEDSelectionToolbar::cmbSelectionShape_currentIndexChanged(const QString & text)
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

void ZEDSelectionToolbar::actSelectionMode_triggered()
{
	if (actSelectionMode->isChecked())
		GetSelectionManager()->SetSelectionMode(ZE_SM_FULLY_COVERS);
	else
		GetSelectionManager()->SetSelectionMode(ZE_SM_INTERSECTS);
}

void ZEDSelectionToolbar::actLockSelection_triggered()
{
	GetSelectionManager()->SetLockSelection(actLockSelection->isChecked());
}

void ZEDSelectionToolbar::actFreeze_triggered()
{
	GetSelectionManager()->FreezeObjects(GetSelectionManager()->GetSelection());
}

void ZEDSelectionToolbar::actUnfreezeAll_triggered()
{
	GetSelectionManager()->UnfreezeObjects(GetSelectionManager()->GetFrozonObjects());
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
