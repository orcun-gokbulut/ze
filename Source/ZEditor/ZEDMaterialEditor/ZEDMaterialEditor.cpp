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
#include "ZEGraphics/ZEFixedMaterial.h"
#include "ZEFile/ZEPathManager.h"
#include "ZEDMaterialEditorViewPort.h"
#include "ZEModel/ZEModel.h"
#include "ZEModel/ZEModelMesh.h"
#include "ZEModel/ZEModelMeshLOD.h"
#include "ZEMeta/ZEObject.h"
#include "ZEMeta/ZEProperty.h"
#include "ZEML/ZEMLWriter.h"

ZEDMaterialEditor::ZEDMaterialEditor(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags)
{
	ui = new Ui::ZEDMaterialEditorClass();
	ui->setupUi(this);
	showMaximized();

	QObject::connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(SaveAs()));
	QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(Open()));
	QObject::connect(ui->actionOpenModel, SIGNAL(triggered()), this, SLOT(OpenModel()));

	ViewPort = new ZEDMaterialEditorViewPort(this ,ui->centralWidget);
	ui->CentralLayout->addWidget(ViewPort);
	InitializeEngine();
	ViewPort->Initialize();
// 	ZEString DefaultCompanyName = "Zinek";
// 	ZEString DefaultApplicationName = "Engine";
// 	ZEString DefaultResourcesDirectoryName = "Resources";
// 	ZEPathManager::CustomizePaths(&DefaultCompanyName, &DefaultApplicationName, &DefaultResourcesDirectoryName);

	connect(&EngineMainLoopTimer, SIGNAL(timeout()), this, SLOT(EngineMainLoop()));
	connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(SaveMaterials()));
	connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(CloseFile()));
	EngineMainLoopTimer.start(0);

	Material = NULL;
	ModelResource = NULL;

	DirectLight1PropertyWindowManager = new ZEDPropertyWindowManager(ui->LightPropertiesTabWidget, ViewPort->GetDirectLight1(), ZEPathManager::GetInstance()->GetEnginePath().ToCString());
	ui->DirectLight1TabLayout->addWidget(DirectLight1PropertyWindowManager);

	DirectLight2PropertyWindowManager = new ZEDPropertyWindowManager(ui->LightPropertiesTabWidget, ViewPort->GetDirectLight2(), ZEPathManager::GetInstance()->GetEnginePath().ToCString());
	ui->DirectLight2TabLayout->addWidget(DirectLight2PropertyWindowManager);

	DirectLight3PropertyWindowManager = new ZEDPropertyWindowManager(ui->LightPropertiesTabWidget, ViewPort->GetDirectLight3(), ZEPathManager::GetInstance()->GetEnginePath().ToCString());
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


void ZEDMaterialEditor::UpdateLightPropertyWidgets()
{
	DirectLight1PropertyWindowManager->UpdatePropertyWidgetValues();
	DirectLight2PropertyWindowManager->UpdatePropertyWidgetValues();
	DirectLight3PropertyWindowManager->UpdatePropertyWidgetValues();
}

void ZEDMaterialEditor::SaveAs()
{
// 	QString SelectedFilePath = QFileDialog::getSaveFileName(0,QString("Save Scene As"),QString(WorkingDir),QString("*.ZEMATERIAL"),0,0);
// 	SelectedFilePath = QString("resources\\") + SelectedFilePath.remove(WorkingDir);
// 
// 	ZEFile* WriteFile = new ZEFile();
// 	WriteFile->Open((const char*)SelectedFilePath.toLatin1(), ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
// 	ViewPort->GetModelMaterials()[0]->WriteToFile(WriteFile);
// 	WriteFile->Close();
// 	delete WriteFile;
}

void ZEDMaterialEditor::Close()
{

}

void ZEDMaterialEditor::WriteMaterialToFile(ZEFixedMaterial* Material, ZEString fileName)
{
	ZEFile File;
	ZEMLWriter MaterialWriter;
	MaterialWriter.Open(fileName);

	ZEMLWriterNode MaterialNode = MaterialWriter.WriteRootNode("Material");
	MaterialNode.WriteUInt8("MajorVersion", 1);
	MaterialNode.WriteUInt8("MinorVersion", 0);
	MaterialNode.WriteString("Name", Material->GetName());

	ZEMLWriterNode MaterialConfigNode = MaterialNode.OpenSubNode("Configuration");
	MaterialConfigNode.WriteString("Name", "Default"); //Will be changed when configuration is implemented.
	MaterialConfigNode.WriteBool("Wireframe", Material->GetWireframe());
	MaterialConfigNode.WriteBool("TwoSided", Material->GetTwoSided());
	MaterialConfigNode.WriteInt32("TransparencyMode", (ZEInt32)Material->GetTransparancyMode());
	MaterialConfigNode.WriteBool("LightningEnabled", Material->GetLightningEnabled());
	MaterialConfigNode.WriteBool("ShadowReceiver", Material->GetShadowReceiver());
	MaterialConfigNode.WriteBool("ShadowCaster", Material->GetShadowCaster());

	MaterialConfigNode.WriteBool("AmbientEnabled", Material->GetAmbientEnabled());
	MaterialConfigNode.WriteBool("GlobalAmbientEnabled", Material->GetGlobalAmbientEnabled());
	MaterialConfigNode.WriteBool("DiffuseEnabled", Material->GetDiffuseEnabled());
	MaterialConfigNode.WriteBool("NormalMapEnabled", Material->GetNormalMapEnabled());
	MaterialConfigNode.WriteBool("ParallaxMapEnabled", Material->GetParallaxMapEnabled());
	MaterialConfigNode.WriteBool("SpecularEnabled", Material->GetSpecularEnabled());
	MaterialConfigNode.WriteBool("EmmisiveEnabled", Material->GetEmmisiveEnabled());
	MaterialConfigNode.WriteBool("OpacityEnabled", Material->GetOpacityEnabled());
	MaterialConfigNode.WriteBool("DetailBaseMapEnabled", Material->GetDetailBaseMapEnabled());
	MaterialConfigNode.WriteBool("DetailNormalMapEnabled", Material->GetDetailNormalMapEnabled());
	MaterialConfigNode.WriteBool("ReflectionEnabled", Material->GetReflectionEnabled());
	MaterialConfigNode.WriteBool("RefractionEnabled", Material->GetRefractionEnabled());
	MaterialConfigNode.WriteBool("LightMapEnabled", Material->GetLightMapEnabled());
	MaterialConfigNode.WriteBool("AlphaCullEnabled", Material->GetAlphaCullEnabled());
	MaterialConfigNode.WriteBool("VertexColorEnabled", Material->GetVertexColorEnabled());

	MaterialConfigNode.WriteFloat("Opacity", Material->GetOpacity());
	MaterialConfigNode.WriteInt32("OpacityComponent", (ZEInt32)Material->GetOpacityComponent());
	MaterialConfigNode.WriteFloat("AlphaCullLimit", Material->GetAlphaCullLimit());
	MaterialConfigNode.WriteVector2("DetailMapTiling", Material->GetDetailMapTiling());
	MaterialConfigNode.WriteFloat("SpecularShininess", Material->GetSpecularShininess());
	MaterialConfigNode.WriteFloat("SubSurfaceScatteringFactor", Material->GetDiffuseSubSurfaceScatteringFactor());

	MaterialConfigNode.WriteVector3("AmbientColor", Material->GetAmbientColor());
	MaterialConfigNode.WriteFloat("AmbientFactor", Material->GetAmbientFactor());
	MaterialConfigNode.WriteVector3("SpecularColor", Material->GetSpecularColor());
	MaterialConfigNode.WriteFloat("SpecularFactor", Material->GetSpecularFactor());
	MaterialConfigNode.WriteVector3("DiffuseColor", Material->GetDiffuseColor());
	MaterialConfigNode.WriteFloat("DiffuseFactor", Material->GetDiffuseFactor());
	MaterialConfigNode.WriteVector3("EmmisiveColor", Material->GetEmmisiveColor());
	MaterialConfigNode.WriteFloat("EmmisiveFactor", Material->GetEmmisiveFactor());
	MaterialConfigNode.WriteFloat("RefractionIndex", Material->GetRefractionIndex());
	MaterialConfigNode.WriteFloat("RefractionFactor", Material->GetRefractionFactor());
	MaterialConfigNode.WriteFloat("ReflectionFactor", Material->GetReflectionFactor());

	MaterialConfigNode.WriteString("BaseMap", Material->GetBaseMapFile());
	MaterialConfigNode.WriteInt32("BaseMapAddressModeU", (ZEInt32)Material->GetBaseMapAddressModeU());
	MaterialConfigNode.WriteInt32("BaseMapAddressModeV", (ZEInt32)Material->GetBaseMapAddressModeV());
	MaterialConfigNode.WriteString("NormalMap", Material->GetNormalMapFile());
	MaterialConfigNode.WriteInt32("NormalMapAddressModeU", Material->GetNormalMapAddressModeU());
	MaterialConfigNode.WriteInt32("NormalMapAddressModeV", Material->GetNormalMapAddressModeV());
	MaterialConfigNode.WriteString("ParallaxMap", Material->GetParallaxMapFile());
	MaterialConfigNode.WriteInt32("ParallaxMapAddressModeU", Material->GetParallaxMapAddressModeU());
	MaterialConfigNode.WriteInt32("ParallaxMapAddressModeV", Material->GetParallaxMapAddressModeV());
	MaterialConfigNode.WriteString("SpecularMap", Material->GetSpecularMapFile());
	MaterialConfigNode.WriteInt32("SpecularMapAddressModeU", Material->GetSpecularMapAddressModeU());
	MaterialConfigNode.WriteInt32("SpecularMapAddressModeV", Material->GetSpecularMapAddressModeV());
	MaterialConfigNode.WriteString("EmmisiveMap", Material->GetEmmisiveMapFile());
	MaterialConfigNode.WriteInt32("EmmisiveMapAddressModeU", Material->GetEmmisiveMapAddressModeU());
	MaterialConfigNode.WriteInt32("EmmisiveMapAddressModeV", Material->GetEmmisiveMapAddressModeV());
	MaterialConfigNode.WriteString("OpacityMap", Material->GetOpacityMapFile());
	MaterialConfigNode.WriteInt32("OpacityMapAddressModeU", Material->GetOpacityMapAddressModeU());
	MaterialConfigNode.WriteInt32("OpacityMapAddressModeV", Material->GetOpacityMapAddressModeV());
	MaterialConfigNode.WriteString("DetailBaseMap", Material->GetDetailBaseMapFile());
	MaterialConfigNode.WriteInt32("DetailBaseMapAddressModeU", Material->GetDetailBaseMapAddressModeU());
	MaterialConfigNode.WriteInt32("DetailBaseMapAddressModeV", Material->GetDetailBaseMapAddressModeV());
	MaterialConfigNode.WriteString("DetailNormalMap", Material->GetDetailNormalMapFile());
	MaterialConfigNode.WriteInt32("DetailNormalMapAddressModeU", Material->GetDetailNormalMapAddressModeU());
	MaterialConfigNode.WriteInt32("DetailNormalMapAddressModeV", Material->GetDetailNormalMapAddressModeV());
	MaterialConfigNode.WriteString("EnvironmentMap", Material->GetEnvironmentMapFile());
	MaterialConfigNode.WriteInt32("EnvironmentMapAddressModeU", Material->GetEnvironmentMapAddressModeU());
	MaterialConfigNode.WriteInt32("EnvironmentMapAddressModeV", Material->GetEnvironmentMapAddressModeV());
	MaterialConfigNode.WriteInt32("EnvironmentMapAddressModeW", Material->GetEnvironmentMapAddressModeW());
	MaterialConfigNode.WriteString("LightMap", Material->GetLightMapFile());
	MaterialConfigNode.WriteInt32("LightMapAddressModeU", Material->GetLightMapAddressModeU());
	MaterialConfigNode.WriteInt32("LightMapAddressModeV", Material->GetLightMapAddressModeV());

	MaterialConfigNode.CloseNode();
	MaterialNode.CloseNode();
	MaterialWriter.Close();

	if(File.IsOpen())
		File.Close();
}

void ZEDMaterialEditor::SaveMaterials()
{
	ZEArray<ZEFixedMaterial*> Materials = ViewPort->GetModelMaterials();

	for(int I = 0; I < Materials.GetCount(); I++)
	{
		WriteMaterialToFile(Materials[I], Materials[I]->GetFileName());
	}
}

void ZEDMaterialEditor::Open()
{
	QString SelectedFilePath = QFileDialog::getOpenFileName(0, QString("Load Material"), QString(ZEPathManager::GetInstance()->GetResourcePath().ToCString()), QString("*.ZEMATERIAL"), 0, 0);

	if(SelectedFilePath.count() != 0)
	{
		CloseFile();
		SelectedFilePath = SelectedFilePath.replace("/", "\\");
		CurrentFileName = SelectedFilePath;
		/*SelectedFilePath.remove(QString(ZEPathManager::GetWorkingDirectory().ToCString()) + "\\", Qt::CaseInsensitive);*/

		ViewPort->SetMaterial((const char*)SelectedFilePath.toLatin1());
		ZEDPropertyWindowManager* PropertyWindowManager = new ZEDPropertyWindowManager(ui->PropertyTabWidget, ((ZEFixedMaterial*)(ViewPort->GetModel()->GetModelResource()->GetMaterials()[0])), (ZEPathManager::GetInstance()->GetResourcePath() + "\\").ToCString());
		ui->PropertyTabWidget->addTab(PropertyWindowManager, ((ZEFixedMaterial*)(ViewPort->GetModel()->GetModelResource()->GetMaterials()[0]))->GetName().ToCString());
		PropertyWindows.Add(PropertyWindowManager);
		Material = ((ZEFixedMaterial*)ViewPort->GetModel()->GetModelResource()->GetMaterials()[0]);
	}

	setWindowTitle("ZEDMaterialEditor - " + SelectedFilePath);
}

void ZEDMaterialEditor::OpenModel()
{
	QString SelectedFilePath = QFileDialog::getOpenFileName(0, QString("Load Model"), QString(ZEPathManager::GetInstance()->GetResourcePath().ToCString()), QString("*.ZEMODEL"), 0, 0);

	if(SelectedFilePath.count() != 0)
	{
		CloseFile();
		SelectedFilePath = SelectedFilePath.replace("/", "\\");
		CurrentFileName = SelectedFilePath;
		//SelectedFilePath.remove(QString(ZEPathManager::GetWorkingDirectory().ToCString()) + "\\", Qt::CaseInsensitive);

		ModelResource = ZEModelResource::LoadResource((const char*)SelectedFilePath.toLatin1());
		ViewPort->SetModelResource(ModelResource);

		for(int I = 0; I < ViewPort->GetModelMaterials().GetCount(); I++)
		{
			ZEDPropertyWindowManager* PropertyWindowManager = new ZEDPropertyWindowManager(ui->PropertyTabWidget, ((ZEFixedMaterial*)(ViewPort->GetModel()->GetModelResource()->GetMaterials()[I])), (ZEPathManager::GetInstance()->GetResourcePath() + "\\").ToCString());
			ui->PropertyTabWidget->addTab(PropertyWindowManager, ((ZEFixedMaterial*)(ModelResource->GetMaterials()[I]))->GetName().ToCString());
			PropertyWindows.Add(PropertyWindowManager);
		}
	}

	setWindowTitle(QString("ZEDMaterialEditor - %1").arg(SelectedFilePath));
}

void ZEDMaterialEditor::CloseFile()
{
	Material = NULL;
	ModelResource = NULL;
	CleanPropertyWindows();
}

void ZEDMaterialEditor::CleanPropertyWindows()
{
	for(int I = 0; I < PropertyWindows.GetCount(); I++)
	{
		if(PropertyWindows[I] != NULL)

		PropertyWindows[I]->close();
		delete PropertyWindows[I];
		PropertyWindows[I] = NULL;
	}
	
	PropertyWindows.Clear();
	ui->PropertyTabWidget->clear();
}
