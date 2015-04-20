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
#include "ZEFile/ZEPathUtils.h"

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
	ZEString DefaultCompanyName = "Zinek";
	ZEString DefaultApplicationName = "Engine";
	ZEString DefaultResourcesDirectoryName = "Resources";
	ZEPathManager::CustomizePaths(&DefaultCompanyName, &DefaultApplicationName, &DefaultResourcesDirectoryName);

	connect(&EngineMainLoopTimer, SIGNAL(timeout()), this, SLOT(EngineMainLoop()));
	connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(SaveMaterials()));
	connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(CloseFile()));
	EngineMainLoopTimer.start(0);

	Material = NULL;
	ModelResource = NULL;

	DirectLight1PropertyWindowManager = new ZEDPropertyWindowManager(ui->LightPropertiesTabWidget, ViewPort->GetDirectLight1(), ZEPathManager::GetWorkingDirectory().ToCString());
	ui->DirectLight1TabLayout->addWidget(DirectLight1PropertyWindowManager);

	DirectLight2PropertyWindowManager = new ZEDPropertyWindowManager(ui->LightPropertiesTabWidget, ViewPort->GetDirectLight2(), ZEPathManager::GetWorkingDirectory().ToCString());
	ui->DirectLight2TabLayout->addWidget(DirectLight2PropertyWindowManager);

	DirectLight3PropertyWindowManager = new ZEDPropertyWindowManager(ui->LightPropertiesTabWidget, ViewPort->GetDirectLight3(), ZEPathManager::GetWorkingDirectory().ToCString());
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

	if(!File.Open(fileName, ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
		zeError("Can not open given file. File : %s", fileName);

	ZEMLNode* MaterialNode = new ZEMLNode("Material");
	MaterialNode->AddProperty("Name", Material->GetName());
	ZEMLNode* ConfigurationNode = MaterialNode->AddSubNode("Configuration");

	ConfigurationNode->AddProperty("Name", "Default"); //Will be changed when configuration is implemented.
	ConfigurationNode->AddProperty("Wireframe", Material->GetWireframe());
	ConfigurationNode->AddProperty("TwoSided", Material->GetTwoSided());
	ConfigurationNode->AddProperty("TransparencyMode", (ZEInt32)Material->GetTransparancyMode());
	ConfigurationNode->AddProperty("LightningEnabled", Material->GetLightningEnabled());
	ConfigurationNode->AddProperty("ShadowReceiver", Material->GetShadowReceiver());
	ConfigurationNode->AddProperty("ShadowCaster", Material->GetShadowCaster());

	ConfigurationNode->AddProperty("AmbientEnabled", Material->GetAmbientEnabled());
	ConfigurationNode->AddProperty("GlobalAmbientEnabled", Material->GetGlobalAmbientEnabled());
	ConfigurationNode->AddProperty("DiffuseEnabled", Material->GetDiffuseEnabled());
	ConfigurationNode->AddProperty("NormalMapEnabled", Material->GetNormalMapEnabled());
	ConfigurationNode->AddProperty("ParallaxMapEnabled", Material->GetParallaxMapEnabled());
	ConfigurationNode->AddProperty("SpecularEnabled", Material->GetSpecularEnabled());
	ConfigurationNode->AddProperty("EmmisiveEnabled", Material->GetEmmisiveEnabled());
	ConfigurationNode->AddProperty("OpacityEnabled", Material->GetOpacityEnabled());
	ConfigurationNode->AddProperty("DetailBaseMapEnabled", Material->GetDetailBaseMapEnabled());
	ConfigurationNode->AddProperty("DetailNormalMapEnabled", Material->GetDetailNormalMapEnabled());
	ConfigurationNode->AddProperty("ReflectionEnabled", Material->GetReflectionEnabled());
	ConfigurationNode->AddProperty("RefractionEnabled", Material->GetRefractionEnabled());
	ConfigurationNode->AddProperty("LightMapEnabled", Material->GetLightMapEnabled());
	ConfigurationNode->AddProperty("AlphaCullEnabled", Material->GetAlphaCullEnabled());
	ConfigurationNode->AddProperty("VertexColorEnabled", Material->GetVertexColorEnabled());

	ConfigurationNode->AddProperty("Opacity", Material->GetOpacity());
	ConfigurationNode->AddProperty("OpacityComponent", (ZEInt32)Material->GetOpacityComponent());
	ConfigurationNode->AddProperty("AlphaCullLimit", Material->GetAlphaCullLimit());
	ConfigurationNode->AddProperty("DetailMapTiling", Material->GetDetailMapTiling());
	ConfigurationNode->AddProperty("SpecularShininess", Material->GetSpecularShininess());
	ConfigurationNode->AddProperty("SubSurfaceScatteringFactor", Material->GetDiffuseSubSurfaceScatteringFactor());

	ConfigurationNode->AddProperty("AmbientColor", Material->GetAmbientColor());
	ConfigurationNode->AddProperty("AmbientFactor", Material->GetAmbientFactor());
	ConfigurationNode->AddProperty("SpecularColor", Material->GetSpecularColor());
	ConfigurationNode->AddProperty("SpecularFactor", Material->GetSpecularFactor());
	ConfigurationNode->AddProperty("DiffuseColor", Material->GetDiffuseColor());
	ConfigurationNode->AddProperty("DiffuseFactor", Material->GetDiffuseFactor());
	ConfigurationNode->AddProperty("EmmisiveColor", Material->GetEmmisiveColor());
	ConfigurationNode->AddProperty("EmmisiveFactor", Material->GetEmmisiveFactor());
	ConfigurationNode->AddProperty("RefractionIndex", Material->GetRefractionIndex());
	ConfigurationNode->AddProperty("RefractionFactor", Material->GetRefractionFactor());
	ConfigurationNode->AddProperty("ReflectionFactor", Material->GetReflectionFactor());

	if(strlen(Material->GetBaseMapFile()) != 0)
		ConfigurationNode->AddProperty("BaseMap", ZEPathUtils::GetRelativePath(ZEString((const char*)CurrentFileName.toLatin1()), ZEPathManager::GetWorkingDirectory() + "\\" + Material->GetBaseMapFile()));
	else
		ConfigurationNode->AddProperty("BaseMap", "");

	ConfigurationNode->AddProperty("BaseMapAddressModeU", (ZEInt32)Material->GetBaseMapAddressModeU());
	ConfigurationNode->AddProperty("BaseMapAddressModeV", (ZEInt32)Material->GetBaseMapAddressModeV());
	
	if(strlen(Material->GetNormalMapFile()) != 0)
		ConfigurationNode->AddProperty("NormalMap", ZEPathUtils::GetRelativePath(ZEString((const char*)CurrentFileName.toLatin1()), ZEPathManager::GetWorkingDirectory() + "\\" + Material->GetNormalMapFile()));
	else
		ConfigurationNode->AddProperty("NormalMap", ""); 
	
	ConfigurationNode->AddProperty("NormalMapAddressModeU", Material->GetNormalMapAddressModeU());
	ConfigurationNode->AddProperty("NormalMapAddressModeV", Material->GetNormalMapAddressModeV());
	
	if(strlen(Material->GetParallaxMapFile()) != 0)
		ConfigurationNode->AddProperty("ParallaxMap", ZEPathUtils::GetRelativePath(ZEString((const char*)CurrentFileName.toLatin1()), ZEPathManager::GetWorkingDirectory() + "\\" + Material->GetParallaxMapFile()));
	else
		ConfigurationNode->AddProperty("ParallaxMap", "");

	ConfigurationNode->AddProperty("ParallaxMapAddressModeU", Material->GetParallaxMapAddressModeU());
	ConfigurationNode->AddProperty("ParallaxMapAddressModeV", Material->GetParallaxMapAddressModeV());
	
	if(strlen(Material->GetSpecularMapFile()) != 0)
		ConfigurationNode->AddProperty("SpecularMap", ZEPathUtils::GetRelativePath(ZEString((const char*)CurrentFileName.toLatin1()), ZEPathManager::GetWorkingDirectory() + "\\" + Material->GetSpecularMapFile()));
	else
		ConfigurationNode->AddProperty("SpecularMap", "");

	ConfigurationNode->AddProperty("SpecularMapAddressModeU", Material->GetSpecularMapAddressModeU());
	ConfigurationNode->AddProperty("SpecularMapAddressModeV", Material->GetSpecularMapAddressModeV());
	
	if(strlen(Material->GetEmmisiveMapFile()) != 0)
		ConfigurationNode->AddProperty("EmmisiveMap", ZEPathUtils::GetRelativePath(ZEString((const char*)CurrentFileName.toLatin1()), ZEPathManager::GetWorkingDirectory() + "\\" + Material->GetEmmisiveMapFile()));
	else
		ConfigurationNode->AddProperty("EmmisiveMap", "");

	ConfigurationNode->AddProperty("EmmisiveMapAddressModeU", Material->GetEmmisiveMapAddressModeU());
	ConfigurationNode->AddProperty("EmmisiveMapAddressModeV", Material->GetEmmisiveMapAddressModeV());
	
	if(strlen(Material->GetOpacityMapFile()) != 0)
		ConfigurationNode->AddProperty("OpacityMap", ZEPathUtils::GetRelativePath(ZEString((const char*)CurrentFileName.toLatin1()), ZEPathManager::GetWorkingDirectory() + "\\" + Material->GetOpacityMapFile()));
	else
		ConfigurationNode->AddProperty("OpacityMap", "");

	ConfigurationNode->AddProperty("OpacityMapAddressModeU", Material->GetOpacityMapAddressModeU());
	ConfigurationNode->AddProperty("OpacityMapAddressModeV", Material->GetOpacityMapAddressModeV());
	
	if(strlen(Material->GetDetailBaseMapFile()) != 0)
		ConfigurationNode->AddProperty("DetailBaseMap", ZEPathUtils::GetRelativePath(ZEString((const char*)CurrentFileName.toLatin1()), ZEPathManager::GetWorkingDirectory() + "\\" + Material->GetDetailBaseMapFile()));
	else
		ConfigurationNode->AddProperty("DetailBaseMap", "");

	ConfigurationNode->AddProperty("DetailBaseMapAddressModeU", Material->GetDetailBaseMapAddressModeU());
	ConfigurationNode->AddProperty("DetailBaseMapAddressModeV", Material->GetDetailBaseMapAddressModeV());
	
	if(strlen(Material->GetDetailNormalMapFile()) != 0)
		ConfigurationNode->AddProperty("DetailNormalMap", ZEPathUtils::GetRelativePath(ZEString((const char*)CurrentFileName.toLatin1()), ZEPathManager::GetWorkingDirectory() + "\\" + Material->GetDetailNormalMapFile()));
	else
		ConfigurationNode->AddProperty("DetailNormalMap", "");

	ConfigurationNode->AddProperty("DetailNormalMapAddressModeU", Material->GetDetailNormalMapAddressModeU());
	ConfigurationNode->AddProperty("DetailNormalMapAddressModeV", Material->GetDetailNormalMapAddressModeV());

	if(strlen(Material->GetEnvironmentMapFile()) != 0)
		ConfigurationNode->AddProperty("EnvironmentMap", ZEPathUtils::GetRelativePath(ZEString((const char*)CurrentFileName.toLatin1()), ZEPathManager::GetWorkingDirectory() + "\\" + Material->GetEnvironmentMapFile()));
	else
		ConfigurationNode->AddProperty("EnvironmentMap", "");

	ConfigurationNode->AddProperty("EnvironmentMapAddressModeU", Material->GetEnvironmentMapAddressModeU());
	ConfigurationNode->AddProperty("EnvironmentMapAddressModeV", Material->GetEnvironmentMapAddressModeV());
	ConfigurationNode->AddProperty("EnvironmentMapAddressModeW", Material->GetEnvironmentMapAddressModeW());
	
	if(strlen(Material->GetLightMapFile()) != 0)
		ConfigurationNode->AddProperty("LightMap", ZEPathUtils::GetRelativePath(ZEString((const char*)CurrentFileName.toLatin1()), ZEPathManager::GetWorkingDirectory() + "\\" + Material->GetLightMapFile()));
	else
		ConfigurationNode->AddProperty("LightMap", "");

	ConfigurationNode->AddProperty("LightMapAddressModeU", Material->GetLightMapAddressModeU());
	ConfigurationNode->AddProperty("LightMapAddressModeV", Material->GetLightMapAddressModeV());

	MaterialNode->Write(&File);

	if(File.IsOpen())
		File.Close();

	delete MaterialNode;	
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
	QString SelectedFilePath = QFileDialog::getOpenFileName(0, QString("Load Material"), QString(ZEPathManager::GetResourcesPath().ToCString()), QString("*.ZEMATERIAL"), 0, 0);

	if(SelectedFilePath.count() != 0)
	{
		CloseFile();
		SelectedFilePath = SelectedFilePath.replace("/", "\\");
		CurrentFileName = SelectedFilePath;
		SelectedFilePath.remove(QString(ZEPathManager::GetWorkingDirectory().ToCString()) + "\\", Qt::CaseInsensitive);

		ViewPort->SetMaterial((const char*)SelectedFilePath.toLatin1());
		ZEDPropertyWindowManager* PropertyWindowManager = new ZEDPropertyWindowManager(ui->PropertyTabWidget, ((ZEFixedMaterial*)(ViewPort->GetModel()->GetModelResource()->GetMaterials()[0])), (ZEPathManager::GetResourcesPath() + "\\").ToCString());
		ui->PropertyTabWidget->addTab(PropertyWindowManager, ((ZEFixedMaterial*)(ViewPort->GetModel()->GetModelResource()->GetMaterials()[0]))->GetName().ToCString());
		PropertyWindows.Add(PropertyWindowManager);
		Material = ((ZEFixedMaterial*)ViewPort->GetModel()->GetModelResource()->GetMaterials()[0]);
	}

	setWindowTitle("ZEDMaterialEditor - " + SelectedFilePath);
}

void ZEDMaterialEditor::OpenModel()
{
	QString SelectedFilePath = QFileDialog::getOpenFileName(0, QString("Load Model"), QString(ZEPathManager::GetResourcesPath().ToCString()), QString("*.ZEMODEL"), 0, 0);

	if(SelectedFilePath.count() != 0)
	{
		CloseFile();
		SelectedFilePath = SelectedFilePath.replace("/", "\\");
		CurrentFileName = SelectedFilePath;
		SelectedFilePath.remove(QString(ZEPathManager::GetWorkingDirectory().ToCString()) + "\\", Qt::CaseInsensitive);

		ModelResource = ZEModelResource::LoadResource((const char*)SelectedFilePath.toLatin1());
		ViewPort->SetModelResource(ModelResource);

		for(int I = 0; I < ViewPort->GetModelMaterials().GetCount(); I++)
		{
			ZEDPropertyWindowManager* PropertyWindowManager = new ZEDPropertyWindowManager(ui->PropertyTabWidget, ((ZEFixedMaterial*)(ViewPort->GetModel()->GetModelResource()->GetMaterials()[I])), (ZEPathManager::GetResourcesPath() + "\\").ToCString());
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
