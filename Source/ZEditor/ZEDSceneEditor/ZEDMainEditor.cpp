//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDMainEditor.cpp
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

#include "ZEDMainEditor.h"

#include "ui_ZEDMainEditor.h"
#include "ZEDMainBrowser.h"
#include "ZEDCore/ZEDCore.h"
#include "ZEDCore/ZEDModule.h"
#include "ZEDCore/ZEDViewport.h"
#include "ZEDCore/ZEDOperationManager.h"
#include "ZEDCore/ZEDTransformationManager.h"
#include "ZEDCore/ZEDGizmo.h"
#include "ZEMath/ZEAngle.h"

bool ZEDMainEditor::InitializeSelf()
{
	if (!Core->Initialize())
		return false;

	MainViewPort->SetScene(Core->GetEditorModule()->GetScene());
	MainViewPort->Initialize();

	connect(ui->actNew, SIGNAL(triggered(bool)), this, SLOT(actNew_onTriggered()));
	connect(ui->actOpen, SIGNAL(triggered(bool)), this, SLOT(actOpen_onTriggered()));
	connect(ui->actClose, SIGNAL(triggered(bool)), this, SLOT(actClose_onTriggered()));
	connect(ui->actSave, SIGNAL(triggered(bool)), this, SLOT(actSave_onTriggered()));
	connect(ui->actSaveAs, SIGNAL(triggered(bool)), this, SLOT(actSaveAs_onTriggered()));
	connect(ui->actExit, SIGNAL(triggered(bool)), this, SLOT(actExit_onTriggered()));
	connect(ui->actSelect, SIGNAL(triggered(bool)), this, SLOT(actSelect_onTriggered()));
	connect(ui->actMove, SIGNAL(triggered(bool)), this, SLOT(actMove_onTriggered()));
	connect(ui->actRotate, SIGNAL(triggered(bool)), this, SLOT(actRotate_onTriggered()));
	connect(ui->actScale, SIGNAL(triggered(bool)), this, SLOT(actScale_onTriggered()));
	connect(ui->actUndo, SIGNAL(triggered(bool)), this, SLOT(actUndo_onTriggered()));
	connect(ui->actRedo, SIGNAL(triggered(bool)), this, SLOT(actRedo_onTriggered()));
	//connect(MainTimer, SIGNAL(timeout()), this, SLOT(MainTimer_onTimeout()));
	//MainTimer->start();
	
	Browser->GetBrowserWidget()->LoadScene();

	return true;
}

void ZEDMainEditor::DeinitializeSelf()
{
	MainViewPort->Deinitialize();
	Core->Deinitialize();
}

void ZEDMainEditor::actNew_onTriggered()
{

}

void ZEDMainEditor::actOpen_onTriggered()
{

}

void ZEDMainEditor::actClose_onTriggered()
{

}

void ZEDMainEditor::actSave_onTriggered()
{

}

void ZEDMainEditor::actSaveAs_onTriggered()
{

}

void ZEDMainEditor::actExit_onTriggered()
{
	qApp->quit();
}

void ZEDMainEditor::actUndo_onTriggered()
{
	/*if (Core->GetEditorModule()->GetViewPort()->hasFocus())
		ZEDCore::GetInstance()->GetOperationManager()->Undo();*/
}

void ZEDMainEditor::actRedo_onTriggered()
{
	/*if (Core->GetEditorModule()->GetViewPort()->hasFocus())
		ZEDCore::GetInstance()->GetOperationManager()->Redo();*/
}

void ZEDMainEditor::actClone_onTriggered()
{

}

void ZEDMainEditor::actDelete_onTriggered()
{

}

void ZEDMainEditor::actSelect_onTriggered()
{
	ui->actMove->setChecked(false);
	ui->actRotate->setChecked(false);
	ui->actScale->setChecked(false);

	if(!ui->actSelect->isChecked())
		ui->actSelect->setChecked(true);

	ZEDCore::GetInstance()->GetTransformationManager()->GetGizmo()->SetMode(ZED_GM_HELPER);
}

void ZEDMainEditor::actMove_onTriggered()
{
	ui->actSelect->setChecked(false);
	ui->actRotate->setChecked(false);
	ui->actScale->setChecked(false);

	if(!ui->actMove->isChecked())
		ui->actMove->setChecked(true);

	ZEDCore::GetInstance()->GetTransformationManager()->GetGizmo()->SetMode(ZED_GM_MOVE);
}

void ZEDMainEditor::actRotate_onTriggered()
{
	ui->actSelect->setChecked(false);
	ui->actMove->setChecked(false);
	ui->actScale->setChecked(false);

	if(!ui->actRotate->isChecked())
		ui->actRotate->setChecked(true);

	ZEDCore::GetInstance()->GetTransformationManager()->GetGizmo()->SetMode(ZED_GM_ROTATE);
}

void ZEDMainEditor::actScale_onTriggered()
{
	ui->actSelect->setChecked(false);
	ui->actMove->setChecked(false);
	ui->actRotate->setChecked(false);

	if(!ui->actScale->isChecked())
		ui->actScale->setChecked(true);

	ZEDCore::GetInstance()->GetTransformationManager()->GetGizmo()->SetMode(ZED_GM_SCALE);
}

void ZEDMainEditor::MainTimer_onTimeout()
{
	Core->ProcessEngine();

	ui->actUndo->setEnabled(Core->GetOperationManager()->CanUndo());
	ui->actRedo->setEnabled(Core->GetOperationManager()->CanRedo());

	ui->lblCameraPositionX->setText(QString::number(MainViewPort->GetPosition().x));
	ui->lblCameraPositionY->setText(QString::number(MainViewPort->GetPosition().y));
	ui->lblCameraPositionZ->setText(QString::number(MainViewPort->GetPosition().z));

	float Rx, Ry, Rz;
	ZEQuaternion::ConvertToEulerAngles(Rx, Ry, Rz, MainViewPort->GetRotation());
	ui->lblCameraRotationX->setText(QString::number((ZEInt)ZEAngle::ToDegree(Rx)));
	ui->lblCameraRotationY->setText(QString::number((ZEInt)ZEAngle::ToDegree(Ry)));
	ui->lblCameraRotationZ->setText(QString::number((ZEInt)ZEAngle::ToDegree(Rz)));
}

ZEDMainBrowser* ZEDMainEditor::GetBrowser()
{
	return Browser;
}

ZEDMainEditor::ZEDMainEditor(QWidget* Parent, Qt::WindowFlags Flags) : QMainWindow(Parent, Flags)
{
	ui = new Ui::MainEditor();
	ui->setupUi(this);
	showMaximized();
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	/*MainTimer = new QTimer(this);
	MainTimer->setInterval(0);*/

	Core = ZEDCore::GetInstance();
	MainViewPort = new ZEDViewport(ui->CentralWidget);
	MainViewPort->show();
	ui->ViewPort = MainViewPort;
	ui->gridLayout->addWidget(ui->ViewPort, 0, 0, 1, 1);
	
//	Core->GetEditorModule()->SetViewPort(MainViewPort);
	
	Browser = new ZEDMainBrowser(this);
	addDockWidget(Qt::RightDockWidgetArea, Browser);

	InitializeSelf();
}

ZEDMainEditor::~ZEDMainEditor()
{
	DeinitializeSelf();
}
