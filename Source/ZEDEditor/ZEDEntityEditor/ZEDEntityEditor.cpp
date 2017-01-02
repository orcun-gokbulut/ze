//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDEntityEditor.cpp
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

#include "ZEDEntityEditor.h"

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
#include "ZEDTextureAssetType.h"
#include "ZEDCore/ZEDAssetManager.h"

#include "ZEDEntityWrapper.h"
#include "ZEDSceneWrapper.h"

#include "ZEDMaterialAssetType.h"
#include "ZEDModelAssetType.h"
#include "ZEDSceneAssetType.h"
#include "ZEDSoundAssetType.h"
#include "ZEDPrefabAssetType.h"
#include "ZEDCore/ZEDViewportSelectionController.h"

bool ZEDEntityEditor::InitializeInternal()
{
	if (!ZEDEditor::InitializeInternal())
		return false;

	GetObjectManager()->RegisterWrapperClass(ZEDEntityWrapper::Class());
	GetObjectManager()->RegisterWrapperClass(ZEDSceneWrapper::Class());

	GetAssetManager()->RegisterAssetType(new ZEDTextureAssetType());
	GetAssetManager()->RegisterAssetType(new ZEDMaterialAssetType());
	GetAssetManager()->RegisterAssetType(new ZEDModelAssetType());
	GetAssetManager()->RegisterAssetType(new ZEDSceneAssetType());
	GetAssetManager()->RegisterAssetType(new ZEDSoundAssetType());
	GetAssetManager()->RegisterAssetType(new ZEDPrefabAssetType());
	
	ViewportController = ZEDViewportController::CreateInstance();
	AddComponent(ViewportController);

	ViewportSelectionController = ZEDViewportSelectionController::CreateInstance();
	AddComponent(ViewportSelectionController);

	ObjectBrowser = new ZEDObjectBrowser();
	GetMainWindow()->AddWindow(ObjectBrowser, ZED_WD_VISIBLE | ZED_WD_STACK_LEFT);

	AssetBrowser = new ZEDAssetBrowser();
	GetMainWindow()->AddWindow(AssetBrowser, ZED_WD_VISIBLE | ZED_WD_STACK_RIGHT);

	PropertyWindow = new ZEDPropertyWindow();
	GetMainWindow()->AddWindow(PropertyWindow, ZED_WD_VISIBLE | ZED_WD_STACK_RIGHT);

	ClassBrowser = new ZEDClassBrowser();
	GetMainWindow()->AddWindow(ClassBrowser, ZED_WD_VISIBLE | ZED_WD_STACK_RIGHT);

	Viewport = new ZEDViewport();
	GetViewportManager()->RegisterViewport(Viewport);
	GetMainWindow()->SetViewport(Viewport);

	Scene = ZEScene::CreateInstance();
	Scene->Initialize();

	GetObjectManager()->SetRootWrapper(GetObjectManager()->WrapObject(Scene));
	ObjectBrowser->SetRootWrapper(GetObjectManager()->GetRootWrapper());

	New();

 	ZEModel* Trial = ZEModel::CreateInstance();
	Trial->SetModelFile("#R:/ZETrainSimulator/Actors/Vehicles/DE24000/DE24000.ZEMODEL");
	//Trial->SetModelFile("#R:/ZETrainSimulator/Sectors/Sector003/Sector003.ZEMODEL");
	//Trial->SetModelFile("#R:/GraphicsTest/Sponza_Model/Sponza.new.ZEMODEL");
	Scene->AddEntity(Trial);

	//ZEInterior* Sponza = ZEInterior::CreateInstance();
	//Sponza->SetInteriorFile("#R:/GraphicsTest/Sponza/Sponza.ZEINTERIOR");
	//Scene->AddEntity(Sponza);

	ZELightDirectional* Light1 = ZELightDirectional::CreateInstance();
	Light1->SetIntensity(3.0f);
	Light1->SetColor(ZEVector3::One);
	Light1->SetUseSunLight(true);
	Scene->AddEntity(Light1);

	Scene->SetAmbientColor(ZEVector3::One);
	Scene->SetAmbientFactor(0.2f);

	ZEATAtmosphere* Atmosphere = ZEATAtmosphere::CreateInstance();
	Atmosphere->SetSunLight(Light1);
	Scene->AddEntity(Atmosphere);

	if (GetObjectManager()->GetRootWrapper() != NULL)
		GetObjectManager()->GetRootWrapper()->Update();
	GetMainWindow()->GetMainWindow()->show();

	return true;
}

ZEDEntityEditor::ZEDEntityEditor()
{
	Viewport = NULL;
	ViewportController = NULL;
	ObjectBrowser = NULL;
	ClassBrowser = NULL;
	AssetBrowser = NULL;
	PropertyWindow = NULL;
	Scene = NULL;
}

ZEDEntityEditor::~ZEDEntityEditor()
{

}

ZEDEntityEditor* ZEDEntityEditor::CreateInstance()
{
	return new ZEDEntityEditor();
}
