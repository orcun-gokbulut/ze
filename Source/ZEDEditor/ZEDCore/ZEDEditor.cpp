//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDEditor.cpp
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

#include "ZEDEditor.h"

#include "ZEDEvent.h"
#include "ZEDComponent.h"
#include "ZEDOperationManager.h"
#include "ZEDSelectionManager.h"
#include "ZEDTransformationManager.h"
#include "ZEDObjectManager.h"
#include "ZEDObjectWrapper.h"
#include "ZEDViewPort.h"
#include "ZEDViewportManager.h"
#include "ZERenderer\ZERNRenderParameters.h"
#include "ZEUI\ZEUIManager.h"
#include "ZEDUserInterface\ZEDMainWindow.h"
#include "ZEDEditorCore.h"


void ZEDEditor::DistributeEvent(const ZEDEvent* Event)
{
	for (ZESize I = 0; I < Components.GetCount(); I++)
	{
		if (Event->IsAcquired())
			break;

		Components[I]->EventReceived(Event);
	}
}

bool ZEDEditor::InitializeSelf()
{
	if (!ZEInitializable::InitializeSelf())
		return false;

	MainWindow = ZEDMainWindow::CreateInstance();
	AddComponent(MainWindow);

	ViewportManager = ZEDViewportManager::CreateInstance();
	AddComponent(ViewportManager);

	UIManager = ZEUIManager::CreateInstance();
	UIManager->Initialize();

	ObjectManager = ZEDObjectManager::CreateInstance();
	AddComponent(ObjectManager);

	TransformationManager = ZEDTransformationManager::CreateInstance();
	AddComponent(TransformationManager);

	SelectionManager = ZEDSelectionManager::CreateInstance();
	AddComponent(SelectionManager);

	OperationManager = ZEDOperationManager::CreateInstance();
	AddComponent(OperationManager);

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

void ZEDEditor::DeinitializeSelf()
{
	for (ZESize I = 0; I < Components.GetCount(); I++)
		Components[I]->Deinitialize();

	UIManager->Deinitialize();

	ZEInitializable::DeinitializeSelf();
}

ZEDEditorCore* ZEDEditor::GetCore()
{
	return Core;
}

ZEDObjectManager* ZEDEditor::GetObjectManager()
{
	return ObjectManager;
}

ZEDOperationManager* ZEDEditor::GetOperationManager()
{
	return OperationManager;
}

ZEDSelectionManager* ZEDEditor::GetSelectionManager()
{
	return SelectionManager;
}

ZEDTransformationManager* ZEDEditor::GetTransformationManager()
{
	return TransformationManager;
}

ZEDViewportManager* ZEDEditor::GetViewportManager()
{
	return ViewportManager;
}

ZEDMainWindow* ZEDEditor::GetMainWindow()
{
	return MainWindow;
}

void ZEDEditor::AddComponent(ZEDComponent* Component)
{
	zeCheckError(Component->Editor != NULL, ZE_VOID, "Component is already registered to a module.");

	Components.Add(Component);
	Component->Editor = this;

	if (IsInitializedOrInitializing())
		Component->Initialize();
}

void ZEDEditor::RemoveComponent(ZEDComponent* Component)
{
	zeCheckError(Component->Editor != this, ZE_VOID, "Component doesn't belong to this module.");

	Component->Deinitialize();
	Components.RemoveValue(Component);
	Component->Editor = NULL;
}

void ZEDEditor::Process(float ElapsedTime)
{
	ZEDTickEvent Event;
	Event.SetElapsedTime(ElapsedTime);
	DistributeEvent(&Event);
}

void ZEDEditor::PostProcess(float ElapsedTime)
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
		
		if (GetObjectManager()->GetRootWrapper() != NULL)
			GetObjectManager()->GetRootWrapper()->PreRender(&PreRenderParameters);
	}

	ViewportManager->Render();
}

void ZEDEditor::New()
{

}

bool ZEDEditor::Save(const ZEString& FileName)
{
	return true;
}

bool ZEDEditor::Load(const ZEString& FileName)
{
	return true;
}

void ZEDEditor::Close()
{
	
}

void ZEDEditor::Exit()
{
	if (GetCore() != NULL)
		GetCore()->Exit();
}

void ZEDEditor::Destroy()
{
	delete this;
}

ZEDEditor::ZEDEditor()
{
	Core = NULL;
}

ZEDEditor::~ZEDEditor()
{
	while (Components.GetCount() != 0)
		Components.GetFirstItem()->Destroy();
}
