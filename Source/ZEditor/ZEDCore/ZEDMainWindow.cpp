//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDMainWindow.cpp
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

#include "ZEDMainWindow.h"

#include "ui_ZEDMainWindow.h"

#include "ZEMath/ZEAngle.h"
#include "ZEDCore/ZEDCore.h"
#include "ZEDCore/ZEDModule.h"
#include "ZEDCore/ZEDViewport.h"
#include "ZEDCore/ZEDOperationManager.h"
#include "ZEDCore/ZEDTransformationManager.h"
#include "ZEDCore/ZEDGizmo.h"
#include "ZEDCore/ZEDTransformationToolbar.h"
#include "ZEDCore/ZEDSelectionToolbar.h"
#include "ZEDCore/ZEDObjectBrowser.h"

void ZEDMainWindow::closeEvent(QCloseEvent* Event)
{
	actExit_onTriggered();
}

bool ZEDMainWindow::InitializeSelf()
{
	connect(Form->actNew, SIGNAL(triggered(bool)), this, SLOT(actNew_onTriggered()));
	connect(Form->actOpen, SIGNAL(triggered(bool)), this, SLOT(actOpen_onTriggered()));
	connect(Form->actClose, SIGNAL(triggered(bool)), this, SLOT(actClose_onTriggered()));
	connect(Form->actSave, SIGNAL(triggered(bool)), this, SLOT(actSave_onTriggered()));
	connect(Form->actSaveAs, SIGNAL(triggered(bool)), this, SLOT(actSaveAs_onTriggered()));
	connect(Form->actExit, SIGNAL(triggered(bool)), this, SLOT(actExit_onTriggered()));
	connect(Form->actSelect, SIGNAL(triggered(bool)), this, SLOT(actSelect_onTriggered()));
	connect(Form->actMove, SIGNAL(triggered(bool)), this, SLOT(actMove_onTriggered()));
	connect(Form->actRotate, SIGNAL(triggered(bool)), this, SLOT(actRotate_onTriggered()));
	connect(Form->actScale, SIGNAL(triggered(bool)), this, SLOT(actScale_onTriggered()));
	connect(Form->actUndo, SIGNAL(triggered(bool)), this, SLOT(actUndo_onTriggered()));
	connect(Form->actRedo, SIGNAL(triggered(bool)), this, SLOT(actRedo_onTriggered()));

	return true;
}

void ZEDMainWindow::DeinitializeSelf()
{

}

void ZEDMainWindow::actNew_onTriggered()
{

}

void ZEDMainWindow::actOpen_onTriggered()
{

}

void ZEDMainWindow::actClose_onTriggered()
{

}

void ZEDMainWindow::actSave_onTriggered()
{

}

void ZEDMainWindow::actSaveAs_onTriggered()
{

}

void ZEDMainWindow::actExit_onTriggered()
{
	exit(0);
}

void ZEDMainWindow::actUndo_onTriggered()
{
	GetModule()->GetOperationManager()->Undo();
}

void ZEDMainWindow::actRedo_onTriggered()
{
	GetModule()->GetOperationManager()->Redo();
}

void ZEDMainWindow::actClone_onTriggered()
{

}

void ZEDMainWindow::actDelete_onTriggered()
{

}

void ZEDMainWindow::actSelect_onTriggered()
{
	Form->actMove->setChecked(false);
	Form->actRotate->setChecked(false);
	Form->actScale->setChecked(false);

	if(!Form->actSelect->isChecked())
		Form->actSelect->setChecked(true);

	GetModule()->GetTransformManager()->SetTransformType(ZED_TT_NONE);
}

void ZEDMainWindow::actMove_onTriggered()
{
	Form->actSelect->setChecked(false);
	Form->actRotate->setChecked(false);
	Form->actScale->setChecked(false);

	if(!Form->actMove->isChecked())
		Form->actMove->setChecked(true);

	GetModule()->GetTransformManager()->SetTransformType(ZED_TT_TRANSLATE);
}

void ZEDMainWindow::actRotate_onTriggered()
{
	Form->actSelect->setChecked(false);
	Form->actMove->setChecked(false);
	Form->actScale->setChecked(false);

	if(!Form->actRotate->isChecked())
		Form->actRotate->setChecked(true);

	GetModule()->GetTransformManager()->SetTransformType(ZED_TT_ROTATE);
}

void ZEDMainWindow::actScale_onTriggered()
{
	Form->actSelect->setChecked(false);
	Form->actMove->setChecked(false);
	Form->actRotate->setChecked(false);

	if(!Form->actScale->isChecked())
		Form->actScale->setChecked(true);

	GetModule()->GetTransformManager()->SetTransformType(ZED_TT_SCALE);
}

void ZEDMainWindow::MainTimer_onTimeout()
{
	ZEDCore::GetInstance()->ProcessEngine();

	Form->actUndo->setEnabled(GetModule()->GetOperationManager()->CanUndo());
	Form->actRedo->setEnabled(GetModule()->GetOperationManager()->CanRedo());
}

void ZEDMainWindow::SetViewport(ZEDViewport* Viewport)
{
	this->Viewport = Viewport;
	setCentralWidget(Viewport);
}

ZEDViewport* ZEDMainWindow::GetViewport()
{
	return Viewport;
}

ZEDMainWindow::ZEDMainWindow(QWidget* Parent, Qt::WindowFlags Flags) : QMainWindow(Parent, Flags)
{
	Form = new Ui_ZEDMainWindow();
	Form->setupUi(this);
	
	showMaximized();
	
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
}

ZEDMainWindow::~ZEDMainWindow()
{

}
