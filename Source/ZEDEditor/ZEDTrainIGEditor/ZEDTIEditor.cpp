//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDTIEditor.cpp
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

#include "ZEDTIEditor.h"

#include "ZEDCore/ZEDGrid.h"
#include "ZEDCore/ZEDObjectWrapper.h"
#include "ZEDCore/ZEDObjectManager.h"
#include "ZEDCore/ZEDViewPort.h"
#include "ZEDCore/ZEDViewportManager.h"
#include "ZEDCore/ZEDViewportController.h"
#include "ZEDUserInterface/ZEDMainWindow.h"
#include "ZEDUserInterface/ZEDObjectBrowser.h"
#include "ZEDUserInterface/ZEDAssetBrowser.h"
#include "ZEDUserInterface/ZEDPropertyWindow.h"
#include "ZEDUserInterface/ZEDClassBrowser.h"
#include "ZEGame/ZEScene.h"
#include "ZEGame/ZESector.h"
#include "ZEModel/ZEModel.h"
#include "ZEAtmosphere/ZEATAtmosphere.h"
#include "ZEAtmosphere/ZEATSkyBox.h"
#include "ZERenderer/ZELightDirectional.h"
#include "ZEGame/ZEStateScreen.h"
#include "ZEInterior/ZEInterior.h"
#include "ZEDCore/ZEDAssetManager.h"

#include "ZEDEntityWrapper.h"
#include "ZEDSceneWrapper.h"

#include "ZEDCore/ZEDViewportSelectionController.h"
#include "ZEApplications/ZETrainIG/ZETISector.h"
#include "ZEApplications/ZETrainIG/ZETIVegetation.h"
#include "ZEApplications/ZETrainIG/ZETIVehicle.h"
#include "ZEApplications/ZETrainIG/ZETIPerson.h"
#include "ZEApplications/ZETrainIG/ZETIAnimal.h"
#include "ZEDUserInterface/ZEDClassModel.h"


bool ZEDTIEditor::InitializeInternal()
{
	if (!ZEDEditor::InitializeInternal())
		return false;
	
	ViewportController = ZEDViewportController::CreateInstance();
	AddComponent(ViewportController);

	ViewportSelectionController = ZEDViewportSelectionController::CreateInstance();
	AddComponent(ViewportSelectionController);

	ObjectBrowser = new ZEDObjectBrowser();
	GetMainWindow()->AddWindow(ObjectBrowser, ZED_WD_VISIBLE | ZED_WD_STACK_LEFT);

// 	AssetBrowser = new ZEDAssetBrowser();
// 	GetMainWindow()->AddWindow(AssetBrowser, ZED_WD_VISIBLE | ZED_WD_STACK_RIGHT);

	PropertyWindow = new ZEDPropertyWindow();
	GetMainWindow()->AddWindow(PropertyWindow, ZED_WD_VISIBLE | ZED_WD_STACK_RIGHT);

 	ClassBrowser = new ZEDClassBrowser();
	ZEDClassModel* ClassModel = new ZEDClassModel();
	ClassModel->SetRootClass(ZEEntity::Class());
	ClassModel->SetMode(ZED_CMM_LIST);
	ClassModel->SetHierarchy(ZED_CMH_INHERITANCE);

 	ZEArray<ZEClass*> ClassIncFilter;
	ClassIncFilter.Add(ZEModel::Class());
	ClassIncFilter.Add(ZELightPoint::Class());
 	ClassIncFilter.Add(ZETIVegetation::Class());
  	ClassIncFilter.Add(ZETIVehicle::Class());
	ClassIncFilter.Add(ZETIPerson::Class());
	ClassIncFilter.Add(ZETIAnimal::Class());
 	ClassModel->SetIncludeFilter(ClassIncFilter);

	ClassBrowser->SetClassModel(ClassModel); 

	GetMainWindow()->AddWindow(ClassBrowser, ZED_WD_VISIBLE | ZED_WD_STACK_RIGHT);

	Viewport = new ZEDViewport();
	GetViewportManager()->RegisterViewport(Viewport);
	GetMainWindow()->GetMainWindow()->setCentralWidget(Viewport);

	OnNew.AddDelegate<ZEDTIEditor, &ZEDTIEditor::Editor_OnNew>(this);

	New();

	Scene->SetAmbientColor(ZEVector3::One);
	Scene->SetAmbientFactor(0.5f);

	if (GetObjectManager()->GetRootWrapper() != NULL)
		GetObjectManager()->GetRootWrapper()->Update();

	GetMainWindow()->GetMainWindow()->show();

	return true;
}

bool ZEDTIEditor::DeinitializeInternal()
{
	OnNew.DisconnectObject(this);

	return ZEDEditor::DeinitializeInternal();
}

void ZEDTIEditor::Editor_OnNew(ZEDEditor* Editor)
{
	Scene = ZEScene::CreateInstance();
	Scene->Initialize();

	ZETISector* Sector = ZETISector::CreateInstance();
	GetObjectManager()->SetRootWrapper(GetObjectManager()->WrapObject(Sector));
	ObjectBrowser->SetRootWrapper(GetObjectManager()->GetRootWrapper());

	Scene->AddEntity(Sector);
	Scene->LoadEntities();
}

ZEString ZEDTIEditor::GetExtensions()
{
	return "*.ZESector";
}

ZEDTIEditor::ZEDTIEditor()
{
	Viewport = NULL;
	ViewportController = NULL;
	ObjectBrowser = NULL;
	ClassBrowser = NULL;
	AssetBrowser = NULL;
	PropertyWindow = NULL;
	Scene = NULL;
	SectorManager = NULL;
}

ZEDTIEditor::~ZEDTIEditor()
{

}

ZEDTIEditor* ZEDTIEditor::CreateInstance()
{
	return new ZEDTIEditor();
}


