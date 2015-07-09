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
#include "ZEDCore/ZEDModule.h"
#include "ZEDCore/ZEDViewPort.h"
#include "ui_ZEDMainEditor.h"

bool ZEDMainEditor::InitializeSelf()
{
	QObject::connect(ui->actNew, SIGNAL(triggered(bool)), this, SLOT(actNew_onTriggered()));
	QObject::connect(ui->actOpen, SIGNAL(triggered(bool)), this, SLOT(actOpen_onTriggered()));
	QObject::connect(ui->actClose, SIGNAL(triggered(bool)), this, SLOT(actClose_onTriggered()));
	QObject::connect(ui->actSave, SIGNAL(triggered(bool)), this, SLOT(actSave_onTriggered()));
	QObject::connect(ui->actSaveAs, SIGNAL(triggered(bool)), this, SLOT(actSaveAs_onTriggered()));
	QObject::connect(ui->actExit, SIGNAL(triggered(bool)), this, SLOT(actExit_onTriggered()));

	return true;
}

bool ZEDMainEditor::DeinitializeSelf()
{
	return true;
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

}


bool ZEDMainEditor::Initialize()
{
	return InitializeSelf();
}

bool ZEDMainEditor::Deinitalize()
{
	return DeinitializeSelf();
}

ZEDMainEditor::ZEDMainEditor(QWidget* Parent, Qt::WindowFlags Flags) : QMainWindow(Parent, Flags)
{
	ui = new Ui::MainEditor();
	ui->setupUi(this);
	showMaximized();

	Core = ZEDCore::GetInstance();
	ZEDViewPort* ViewPort = Core->GetEditorModule()->GetViewPort();
	ViewPort->setParent(ui->CentralWidget);
	ui->ViewPort = ViewPort;
	ui->gridLayout->addWidget(ui->ViewPort, 0, 0, 1, 1);

	Core->InitializeEngine();
}
