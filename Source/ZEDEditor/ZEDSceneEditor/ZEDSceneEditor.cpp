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
#include "ZEDUserInterface/ZEDPropertyEditor.h"
#include "ZEDUserInterface/ZEDClassBrowser.h"
#include "ZEDUserInterface/ZEDObjectTree.h"
#include "ZEGame/ZEScene.h"
#include "ZEGame/ZESector.h"
#include "ZEModel/ZEModel.h"
#include "ZEAtmosphere/ZEATAtmosphere.h"
#include "ZEAtmosphere/ZEATSkyBox.h"
#include "ZERenderer/ZELightDirectional.h"

#include <QDockWidget>


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
	QDockWidget* ObjectBrowserDockWidget = new QDockWidget();
	ObjectBrowserDockWidget->setWindowTitle(ObjectBrowser->windowTitle());
	ObjectBrowserDockWidget->setWidget(ObjectBrowser);
	AddComponent(ObjectBrowser);
	GetMainWindow()->GetMainWindow()->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, ObjectBrowserDockWidget);

	AssetBrowser = new ZEDAssetBrowser();
	QDockWidget* AssetBrowserDockWidget = new QDockWidget();
	AssetBrowserDockWidget->setWindowTitle(AssetBrowser->windowTitle());
	AssetBrowserDockWidget->setWidget(AssetBrowser);
	GetMainWindow()->GetMainWindow()->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, AssetBrowserDockWidget);

	PropertyEditor = new ZEDPropertyEditor();
	QDockWidget* PropertyEditorDockWidget = new QDockWidget();
	PropertyEditorDockWidget->setWindowTitle(PropertyEditor->windowTitle());
	PropertyEditorDockWidget->setWidget(PropertyEditor);
	GetMainWindow()->GetMainWindow()->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, PropertyEditorDockWidget);
	AddComponent(PropertyEditor);

	ClassBrowser = new ZEDClassBrowser();
	QDockWidget* ClassBrowserDockWidget = new QDockWidget();
	ClassBrowserDockWidget->setWindowTitle(ClassBrowser->windowTitle());
	ClassBrowserDockWidget->setWidget(ClassBrowser);
	GetMainWindow()->GetMainWindow()->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, ClassBrowserDockWidget);
	AddComponent(ClassBrowser);

	GetMainWindow()->GetMainWindow()->tabifyDockWidget(ObjectBrowserDockWidget, AssetBrowserDockWidget);
	GetMainWindow()->GetMainWindow()->tabifyDockWidget(PropertyEditorDockWidget, ClassBrowserDockWidget);

	Viewport = new ZEDViewport();
	GetViewportManager()->RegisterViewport(Viewport);
	GetMainWindow()->SetViewport(Viewport);

	Scene = ZEScene::CreateInstance();
	Scene->Initialize();

	GetObjectManager()->SetRootWrapper(GetObjectManager()->WrapObject(Scene));
	ObjectBrowser->GetObjectTree()->SetRootWrapper(GetObjectManager()->GetRootWrapper());

	ZEDGrid* Grid = ZEDGrid::CreateInstance();
	Scene->AddEntity(Grid);

// 	ZEModel* Trial = ZEModel::CreateInstance();
// 	Trial->SetModelResource(ZEModelResource::LoadSharedResource("#R:/GraphicsTest/Sponza_Model/Sponza.ZEMODEL"));
// 	Scene->AddEntity(Trial);
// 
// 	ZEModel* Trial2 = ZEModel::CreateInstance();
// 	Trial2->SetPosition(ZEVector3(5.0f, 0.0f, 5.0f));
// 	Trial2->SetModelResource(ZEModelResource::LoadSharedResource("#R:/GraphicsTest/Sponza_Model/Sponza.ZEMODEL"));
// 	Scene->AddEntity(Trial2);
// 
	ZELightDirectional* Light1 = ZELightDirectional::CreateInstance();
	Light1->SetIntensity(3.0f);
	Light1->SetColor(ZEVector3::One);
	Light1->SetUseSunLight(true);
	Scene->AddEntity(Light1);

	ZESector* Sector = ZESector::CreateInstance();
	Sector->SetSectorFile("#R:/ZETrainSimulator/Sectors/Sector003/Sector003.ZESector");
	Scene->AddEntity(Sector);

	Scene->SetAmbientColor(ZEVector3::One);
	Scene->SetAmbientFactor(0.2f);

	ZEATAtmosphere* Atmosphere = ZEATAtmosphere::CreateInstance();
	Atmosphere->SetSunLight(Light1);
	Scene->AddEntity(Atmosphere);

	ZEATSkyBox* SkyBox = ZEATSkyBox::CreateInstance();
	SkyBox->SetTextureFile("#R:/ZEEngine/ZEAtmosphere/Textures/StarMap.png");
	Scene->AddEntity(SkyBox);

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
	PropertyEditor = NULL;
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
