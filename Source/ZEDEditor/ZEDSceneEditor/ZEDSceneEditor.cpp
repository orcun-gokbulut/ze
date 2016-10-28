//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSceneEditor.cpp
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

#include "ZEDSceneEditor.h"

#include "ZEDCore/ZEDGrid.h"
#include "ZEDCore/ZEDObjectWrapper.h"
#include "ZEDCore/ZEDObjectManager.h"
#include "ZEDCore/ZEDViewPort.h"
#include "ZEDCore/ZEDViewportManager.h"
#include "ZEDCore/ZEDViewportController.h"
#include "ZEDUserInterface/ZEDMainWindow.h"
#include "ZEDUserInterface/ZEDTransformationToolbar.h"
#include "ZEDUserInterface/ZEDSelectionToolbar.h"
#include "ZEDUserInterface/ZEDObjectBrowser.h"
#include "ZEDUserInterface/ZEDAssetBrowser.h"
#include "ZEDUserInterface/ZEDPropertyWindow.h"
#include "ZEDUserInterface/ZEDClassBrowser.h"
#include "ZEDUserInterface/ZEDObjectTree.h"
#include "ZEGame/ZEScene.h"
#include "ZEGame/ZESector.h"
#include "ZEModel/ZEModel.h"
#include "ZEAtmosphere/ZEATAtmosphere.h"
#include "ZEAtmosphere/ZEATSkyBox.h"
#include "ZERenderer/ZELightDirectional.h"
#include "ZEGame/ZEStateScreen.h"

bool ZEDSceneEditor::InitializeInternal()
{
	if (!ZEDEditor::InitializeInternal())
		return false;

	TransformationToolbar = ZEDTransformationToolbar::CreateInstance();
	GetMainWindow()->AddToolbar(TransformationToolbar);
	
	SelectionToolbar = ZEDSelectionToolbar::CreateInstance();
	GetMainWindow()->AddToolbar(SelectionToolbar);

	ZEDViewportController* Controller = ZEDViewportController::CreateInstance();
	AddComponent(Controller);

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
	ObjectBrowser->GetObjectTree()->SetRootWrapper(GetObjectManager()->GetRootWrapper());

	/*ZEStateScreen* StateScreen = ZEStateScreen::CreateInstance();
	StateScreen->SetManager(GetUIManager());
	Scene->AddEntity(StateScreen);*/

 	ZEModel* Trial = ZEModel::CreateInstance();
	//Trial->SetModelFile("#R:/ZETrainSimulator/Procedurals/Railway/Tracks/Track-100.new.ZEMODEL");
	Trial->SetModelFile("#R:/ZETrainSimulator/Sectors/Sector003/Sector003.ZEMODEL");
	//Trial->SetModelFile("#R:/GraphicsTest/Sponza_Model/Sponza.new.ZEMODEL");
	Scene->AddEntity(Trial);

	ZELightDirectional* Light1 = ZELightDirectional::CreateInstance();
	Light1->SetIntensity(3.0f);
	Light1->SetColor(ZEVector3::One);
	Light1->SetUseSunLight(true);
	Scene->AddEntity(Light1);

	/*ZESector* Sector = ZESector::CreateInstance();
	Sector->SetSectorFile("#R:/ZETrainSimulator/Sectors/Sector001/Sector001.ZESector");
	Scene->AddEntity(Sector);

	Sector = ZESector::CreateInstance();
	Sector->SetSectorFile("#R:/ZETrainSimulator/Sectors/Sector002/Sector002.ZESector");
	Sector->SetWorldPosition(2.0f * ZEVector3::UnitX * 1000.0f);
	Scene->AddEntity(Sector);

	Sector = ZESector::CreateInstance();
	Sector->SetSectorFile("#R:/ZETrainSimulator/Sectors/Sector003/Sector003.ZESector");
	Sector->SetWorldPosition(3.0f * ZEVector3::UnitX * 1000.0f);
	Scene->AddEntity(Sector);

	Sector = ZESector::CreateInstance();
	Sector->SetSectorFile("#R:/ZETrainSimulator/Sectors/Sector004/Sector004.ZESector");
	Sector->SetWorldPosition(4.0f * ZEVector3::UnitX * 1000.0f);
	Scene->AddEntity(Sector);

	Sector = ZESector::CreateInstance();
	Sector->SetSectorFile("#R:/ZETrainSimulator/Sectors/Sector005/Sector005.ZESector");
	Sector->SetWorldPosition(5.0f * ZEVector3::UnitX * 1000.0f);
	Scene->AddEntity(Sector);*/

	Scene->SetAmbientColor(ZEVector3::One);
	Scene->SetAmbientFactor(0.2f);

	ZEATAtmosphere* Atmosphere = ZEATAtmosphere::CreateInstance();
	Atmosphere->SetSunLight(Light1);
	Scene->AddEntity(Atmosphere);

	GetObjectManager()->GetRootWrapper()->Update();
	GetMainWindow()->GetMainWindow()->show();

	return true;
}

ZEDSceneEditor::ZEDSceneEditor()
{
	Viewport = NULL;
	Controller = NULL;
	ObjectBrowser = NULL;
	ClassBrowser = NULL;
	AssetBrowser = NULL;
	PropertyWindow = NULL;
	SelectionToolbar = NULL;
	TransformationToolbar = NULL;
	Scene = NULL;
}

ZEDSceneEditor::~ZEDSceneEditor()
{

}

void ZEDSceneEditor::New()
{
	Close();

	ZEDEditor::New();
}

bool ZEDSceneEditor::Save(const ZEString& FileName)
{
	if (!Scene->Serialize(FileName))
		return false;

	return ZEDEditor::Save(FileName);
}

bool ZEDSceneEditor::Load(const ZEString& FileName)
{
	Close();

	if (!Scene->Unserialize(FileName))
		return false;

	return ZEDEditor::Load(FileName);
}

void ZEDSceneEditor::Close()
{
	Scene->ClearEntities();
	ZEDEditor::Close();
}

ZEDSceneEditor* ZEDSceneEditor::CreateInstance()
{
	return new ZEDSceneEditor();
}
