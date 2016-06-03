//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDModule.cpp
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

#include "ZEDModule.h"

#include "ZEDCore.h"
#include "ZEDSceneWrapper.h"
#include "ZEDSelectionManager.h"
#include "ZEDViewport.h"
#include "ZEDViewportManager.h"
#include "ZEDViewportEvent.h"
#include "ZEDGizmo.h"
#include "ZEDEntityWrapper.h"
#include "ZEDObjectBrowser.h"
#include "ZEDMainWindow.h"
#include "ZEDViewportController.h"


#include "ZEGame/ZEGrid.h"
#include "ZEGame/ZEScene.h"
#include "ZEModel/ZEModel.h"
#include "ZEAtmosphere/ZEATAtmosphere.h"
#include "ZEAtmosphere/ZEATSkyBox.h"
#include "ZERenderer/ZELightDirectional.h"
#include "ZERenderer/ZERNRenderParameters.h"

#include "ZEDPropertyEditor.h"
#include "ZEDSelectionToolbar.h"
#include "ZEDTransformationToolbar.h"
#include "ZEDTransformationManager.h"

void ZEDModule::DistributeEvent(const ZEDEvent* Event)
{
	for (ZESize I = 0; I < Components.GetCount(); I++)
	{
		if (Event->IsAcquired())
			break;

		Components[I]->EventReceived(Event);
	}
}

bool ZEDModule::InitializeSelf()
{
	for (ZESize I = 0; I < Components.GetCount(); I++)
	{
		if (!Components[I]->Initialize())
		{
			zeError("Cannot initialize component.");
			return false;
		}
	}

	return true;
}

bool ZEDModule::DeinitializeSelf()
{
	for (ZESize I = 0; I < Components.GetCount(); I++)
		Components[I]->Deinitialize();

	return true;
}

ZEDOperationManager* ZEDModule::GetOperationManager()
{
	return OperationManager;
}

ZEDSelectionManager* ZEDModule::GetSelectionManager()
{
	return SelectionManager;
}

ZEDTransformationManager* ZEDModule::GetTransformManager()
{
	return TransformManager;
}

ZEDViewportManager* ZEDModule::GetViewportManager()
{
	return ViewportManager;
}

ZEDObjectWrapper* ZEDModule::GetRootWrapper()
{
	return RootWrapper;
}

void ZEDModule::AddComponent(ZEDComponent* Component)
{
	zeCheckError(Component->Module != NULL, ZE_VOID, "Component is already registered to a module.");

	Components.Add(Component);
	
	if (IsInitialized())
		Component->Initialize();

	Component->Module = this;
}

void ZEDModule::RemoveComponent(ZEDComponent* Component)
{
	zeCheckError(Component->Module != this, ZE_VOID, "Component doesn't belong to this module.");

	Component->Deinitialize();
	Components.RemoveValue(Component);
	Component->Module = NULL;
}

void ZEDModule::Process(float ElapsedTime)
{
	ZEDTickEvent Event;
	Event.SetElapsedTime(ElapsedTime);
	DistributeEvent(&Event);
}

void ZEDModule::PostProcess(float ElapsedTime)
{
	ZEArray<ZEDViewport*> Viewports = ViewportManager->GetViewports();
	for (ZESize I = 0; I < Viewports.GetCount(); I++)
	{
		if (!Viewports[I]->PreRender())
			continue;

		ZERNRenderer* Renderer = Viewports[I]->GetRenderer();
		ZERNPreRenderParameters PreRenderParameters;
		PreRenderParameters.Renderer = Viewports[I]->GetRenderer();
		PreRenderParameters.View = &Viewports[I]->GetView();
		RootWrapper->PreRender(&PreRenderParameters);
	}

	ViewportManager->Render();
}

void ZEDModule::StartUp()
{
	ZEDMainWindow* MainWindow = new ZEDMainWindow();
	AddComponent(MainWindow);
	MainWindow->show();

	ZEDTransformationToolbar* TransformManagerToolbar = new ZEDTransformationToolbar();
	TransformManagerToolbar->SetTransformManager(TransformManager);
	AddComponent(TransformManagerToolbar);
	TransformManagerToolbar->show();

	ZEDSelectionToolbar* SelectionToolbar = new ZEDSelectionToolbar();
	AddComponent(SelectionToolbar);
	SelectionToolbar->SetSelectionManager(GetSelectionManager());
	SelectionToolbar->show();

	ZEDViewportController* Controller = new ZEDViewportController();
	AddComponent(Controller);

	ZEDObjectBrowser* Browser = new ZEDObjectBrowser();
	AddComponent(Browser);
	Browser->show();

	ZEDPropertyEditor* Property = new ZEDPropertyEditor();
	AddComponent(Property);
	Property->show();

	ZEDViewport* Viewport = new ZEDViewport();
	GetViewportManager()->RegisterViewport(Viewport);
	MainWindow->SetViewport(Viewport);

	ZEScene* Scene = new ZEScene();
	Scene->Initialize();

	RootWrapper = ZEDSceneWrapper::CreateInstance();
	RootWrapper->SetObject(Scene);

	ZEGrid* Grid = ZEGrid::CreateInstance();
	ZEDEntityWrapper* GridWrapper = ZEDEntityWrapper::CreateInstance();
	GridWrapper->SetObject(Grid);
	GridWrapper->SetSelectable(false);
	RootWrapper->AddChildWrapper(GridWrapper);

	ZEModel* Trial = ZEModel::CreateInstance();
	Trial->SetModelResource(ZEModelResource::LoadSharedResource("#R:/GraphicsTest/Sponza_Model/Sponza.ZEMODEL"));
	ZEDEntityWrapper* Trial1Wrapper = ZEDEntityWrapper::CreateInstance();
	Trial1Wrapper->SetObject(Trial);
	RootWrapper->AddChildWrapper(Trial1Wrapper);

	ZEModel* Trial2 = ZEModel::CreateInstance();
	//Trial2->SetBoundingBox(ZEAABBox(ZEVector3(-1.0f, -1.0f, -1.0f),ZEVector3(1.0f, 1.0f, 1.0f)));
	//Trial2->SetUserDefinedBoundingBoxEnabled(true);
	Trial2->SetPosition(ZEVector3(5.0f, 0.0f, 5.0f));
	Trial2->SetModelResource(ZEModelResource::LoadSharedResource("#R:/GraphicsTest/Sponza_Model/Sponza.ZEMODEL"));
	ZEDEntityWrapper* Trial2Wrapper = ZEDEntityWrapper::CreateInstance();
	Trial2Wrapper->SetObject(Trial2);
	RootWrapper->AddChildWrapper(Trial2Wrapper);

	ZELightDirectional* Light1 = ZELightDirectional::CreateInstance();
	ZEDEntityWrapper* Light1Wrapper = ZEDEntityWrapper::CreateInstance();
	Light1->SetIntensity(1.0f);
	Light1->SetColor(ZEVector3::One);
	Light1Wrapper->SetObject(Light1);
	RootWrapper->AddChildWrapper(Light1Wrapper);

	Scene->SetAmbientColor(ZEVector3::One);
	Scene->SetAmbientFactor(0.2f);
	
	/*ZEATAtmosphere* Atmosphere = ZEATAtmosphere::CreateInstance();
	ZEDEntityWrapper* AtmosphereWrapper = ZEDEntityWrapper::CreateInstance();
	AtmosphereWrapper->SetObject(Atmosphere);
	AtmosphereWrapper->SetSelectable(false);
	SceneWrapper->AddChildWrapper(AtmosphereWrapper);*/

	ZEATSkyBox* SkyBox = ZEATSkyBox::CreateInstance();
	SkyBox->SetTextureFile("#R:/ZEEngine/ZEAtmosphere/Textures/StarMap.png");
	ZEDEntityWrapper* SkyBoxWrapper = ZEDEntityWrapper::CreateInstance();
	SkyBoxWrapper->SetObject(SkyBox);
	SkyBoxWrapper->SetSelectable(false);
	RootWrapper->AddChildWrapper(SkyBoxWrapper);

}

void ZEDModule::ShutDown()
{

}

ZEDModule::ZEDModule()
{
	RootWrapper = NULL;

	OperationManager = ZEDOperationManager::CreateInstance();
	AddComponent(OperationManager);

	TransformManager = ZEDTransformationManager::CreateInstance();
	AddComponent(TransformManager);

	SelectionManager = ZEDSelectionManager::CreateInstance();
	AddComponent(SelectionManager);

	ViewportManager = ZEDViewportManager::CreateInstance();
	AddComponent(ViewportManager);
}

ZEDModule::~ZEDModule()
{
	for (ZESize I = 0; I < Components.GetCount(); I++)
		Components[I]->Destroy();

	Components.Clear();
}
