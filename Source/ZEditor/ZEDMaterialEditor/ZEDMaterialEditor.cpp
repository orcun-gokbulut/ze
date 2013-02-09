//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDMaterialEditor.cpp
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

#define WIN32_LEAN_AND_MEAN
#include "ui_ZEDMaterialEditor.h"
#include "ZEDMaterialEditor.h"
#include <Windows.h>
#include <ZESDK.h>
#include <ZECore/ZECore.h>
#include <ZECore/ZEWindow.h>
#include <ZECore/ZEConsole.h>
#include <ZECore/ZEOptionManager.h>
#include <ZECore/ZEModuleManager.h>

#include "ZEDCommonControls/ZEDPropertyWindowManager/ZEDPropertyWindowManager.h"
#include "ZERenderer/ZEFixedMaterial.h"

#define WorkingDir "C:/Users/Can/Desktop/ZE/trunk/RunDir/resources/"

ZEDMaterialEditor::ZEDMaterialEditor(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags)
{
	ui = new Ui::ZEDMaterialEditorClass();
	ui->setupUi(this);
	showMaximized();

	QObject::connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));
	QObject::connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(SaveAs()));
	QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(Open()));

	ViewPort = new ZEDMaterialEditorViewPort(this ,ui->centralWidget);
	ui->CentralLayout->addWidget(ViewPort);
	InitializeEngine();
	ViewPort->Initialize();
	QObject::connect(&EngineMainLoopTimer, SIGNAL(timeout()), this, SLOT(EngineMainLoop()));
	QObject::connect(ui->actionCube, SIGNAL(triggered()), this, SLOT(CubeSelected()));
	QObject::connect(ui->actionPlane, SIGNAL(triggered()), this, SLOT(PlaneSelected()));
	QObject::connect(ui->actionSphere, SIGNAL(triggered()), this, SLOT(SphereSelected()));
	QObject::connect(ui->actionCylinder, SIGNAL(triggered()), this, SLOT(CylinderSelected()));
	EngineMainLoopTimer.start(0);

	for(ZESize I = 0; I < ViewPort->GetModelMaterials().GetCount(); I++)
	{
		MaterialPropertyWindowManager = new ZEDPropertyWindowManager(ui->PropertyTabWidget, ViewPort->GetModelMaterials().GetItem(I), WorkingDir);
		ui->PropertyTabWidget->addTab(MaterialPropertyWindowManager, QString().setNum(I));
	}

// 	MaterialPropertyWindowManager = new ZEDPropertyWindowManager(ui->PropertyTabWidget, ViewPort->GetModelMaterial(), WorkingDir);
// 	ui->MaterialTabLayout->addWidget(MaterialPropertyWindowManager);

	DirectLight1PropertyWindowManager = new ZEDPropertyWindowManager(ui->PropertyTabWidget, ViewPort->GetDirectLight1(), WorkingDir);
	ui->DirectLight1TabLayout->addWidget(DirectLight1PropertyWindowManager);

	DirectLight2PropertyWindowManager = new ZEDPropertyWindowManager(ui->PropertyTabWidget, ViewPort->GetDirectLight2(), WorkingDir);
	ui->DirectLight2TabLayout->addWidget(DirectLight2PropertyWindowManager);

	DirectLight3PropertyWindowManager = new ZEDPropertyWindowManager(ui->PropertyTabWidget, ViewPort->GetDirectLight3(), WorkingDir);
	ui->DirectLight3TabLayout->addWidget(DirectLight3PropertyWindowManager);
}

ZEDMaterialEditor::~ZEDMaterialEditor()
{

}

void ZEDMaterialEditor::InitializeEngine()
{
	zeCore->GetOptions()->Load("options.ini");
	zeCore->GetOptions()->ResetChanges();
	zeCore->GetWindow()->SetWindowType(ZE_WT_COMPONENT);
	zeCore->GetWindow()->SetComponentWindowHandle(ViewPort->winId());
	
	zeInitialize(GetModuleHandle(NULL), ViewPort->winId());
	zeCore->GetConsole()->SetConsoleInterface(NULL);
	//zeCore->GetGame()->GetScene()->SetVisualDebugElements(ZE_VDE_NONE);
}

void ZEDMaterialEditor::EngineMainLoop()
{
	zeMainLoop();
}

void ZEDMaterialEditor::SphereSelected()
{
	MaterialPropertyWindowManager->close();
	delete MaterialPropertyWindowManager;
	MaterialPropertyWindowManager = NULL;
	ViewPort->SetModelFile("Box.ZEMODEL");
//	MaterialPropertyWindowManager = new ZEDPropertyWindowManager(ui->PropertyTabWidget, ViewPort->GetModelMaterial(), "C:/Users/Can/Desktop/ZE/trunk/RunDir/resources/");
}

void ZEDMaterialEditor::CubeSelected()
{
	MaterialPropertyWindowManager->close();
	delete MaterialPropertyWindowManager;
	MaterialPropertyWindowManager = NULL;
	ViewPort->SetModelFile("Box.ZEMODEL");
	//MaterialPropertyWindowManager = new ZEDPropertyWindowManager(ui->PropertyTabWidget, ViewPort->GetModelMaterial(), "C:/Users/Can/Desktop/ZE/trunk/RunDir/resources/");
}

void ZEDMaterialEditor::PlaneSelected()
{
	MaterialPropertyWindowManager->close();
	delete MaterialPropertyWindowManager;
	MaterialPropertyWindowManager = NULL;
	ViewPort->SetModelFile("Box.ZEMODEL");
	//MaterialPropertyWindowManager = new ZEDPropertyWindowManager(ui->PropertyTabWidget, ViewPort->GetModelMaterial(), "C:/Users/Can/Desktop/ZE/trunk/RunDir/resources/");
}

void ZEDMaterialEditor::CylinderSelected()
{
	MaterialPropertyWindowManager->close();
	delete MaterialPropertyWindowManager;
	MaterialPropertyWindowManager = NULL;
	ViewPort->SetModelFile("Box.ZEMODEL");
	//MaterialPropertyWindowManager = new ZEDPropertyWindowManager(ui->PropertyTabWidget, ViewPort->GetModelMaterial(), "C:/Users/Can/Desktop/ZE/trunk/RunDir/resources/");
}

void ZEDMaterialEditor::UpdateLightPropertyWidgets()
{
	DirectLight1PropertyWindowManager->UpdatePropertyWidgetValues();
	DirectLight2PropertyWindowManager->UpdatePropertyWidgetValues();
	DirectLight3PropertyWindowManager->UpdatePropertyWidgetValues();
}

void ZEDMaterialEditor::SaveAs()
{
	QString SelectedFilePath = QFileDialog::getSaveFileName(0,QString("Save Scene As"),QString(WorkingDir),QString("*.ZEMATERIAL"),0,0);
	SelectedFilePath = QString("resources\\") + SelectedFilePath.remove(WorkingDir);

	ZEFile* WriteFile = new ZEFile();
	WriteFile->Open((const char*)SelectedFilePath.toLatin1(), ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
	ViewPort->GetModelMaterials()[0]->WriteToFile(WriteFile);
	WriteFile->Close();
	delete WriteFile;
}

void ZEDMaterialEditor::Open()
{
	QString SelectedFilePath = QFileDialog::getOpenFileName(0,QString("Load Material"),QString(WorkingDir),QString("*.ZEMATERIAL"),0,0);

	if(SelectedFilePath.count() != 0)
	{
		SelectedFilePath = QString("resources\\") + SelectedFilePath.remove(WorkingDir);
		ZEFile* ReadFile = new ZEFile();
		ReadFile->Open((const char*)SelectedFilePath.toLatin1(), ZE_FOM_READ, ZE_FCM_NONE);
		ViewPort->GetModelMaterials()[0]->ReadFromFile(ReadFile);
		ReadFile->Close();
		delete ReadFile;
		MaterialPropertyWindowManager->close();
		delete MaterialPropertyWindowManager;

		for(ZESize I = 0; I < ViewPort->GetModelMaterials().GetCount(); I++)
		{
			MaterialPropertyWindowManager = new ZEDPropertyWindowManager(ui->PropertyTabWidget, ViewPort->GetModelMaterials().GetItem(I), WorkingDir);
			ui->PropertyTabWidget->addTab(MaterialPropertyWindowManager, QString().setNum(I));
		}
	}
}
