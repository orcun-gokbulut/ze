//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDObjectManager.cpp
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

#include "ZEDObjectManager.h"

#include "ZEMeta\ZEMTProvider.h"

#include "ZEDEditor.h"
#include "ZEDObjectWrapper.h"
#include "ZEDOperationManager.h"
#include "ZEDCreateOperation.h"
#include "ZEDDeleteOperation.h"
#include "ZEDRelocateOperation.h"
#include "ZEDEditorEvent.h"
#include "ZEDSelectionManager.h"
#include "ZEDUserInterface\ZEDCommandManager.h"
#include "ZEDCloneOperation.h"

void ZEDObjectManager::LoadWrapperClasses()
{
	ZEArray<ZEClass*> WrapperClasses = ZEMTProvider::GetInstance()->GetClasses(ZEDObjectWrapper::Class());
	for (ZESize I = 0; I < WrapperClasses.GetCount(); I++)
	{
		ZEClass* WrapperClass = WrapperClasses[I];
		RegisterWrapperClass(WrapperClass);
	}
}

bool ZEDObjectManager::InitializeInternal()
{
	if (!ZEDComponent::InitializeInternal())
		return false;
	
	RegisterCommands();
	LoadWrapperClasses();
	
	GetEditor()->GetSelectionManager()->OnSelectionChanged.AddDelegate<ZEDObjectManager, &ZEDObjectManager::SelectionManager_OnSelectionChanged>(this);

	if (RootWrapper != NULL)
	{
		RootWrapper->Initialize();
		RootWrapper->Update();
	}

	return true;
}

bool ZEDObjectManager::DeinitializeInternal()
{
	GetEditor()->GetSelectionManager()->OnSelectionChanged.DisconnectObject(this);

	if (RootWrapper != NULL)
		RootWrapper->Deinitialize();

	return ZEDComponent::DeinitializeInternal();
}

void ZEDObjectManager::EditorEvent(const ZEDEditorEvent* Event)
{
	UpdateCommands();
}

void ZEDObjectManager::SelectionManager_OnSelectionChanged(ZEDSelectionManager* Manager, const ZEArray<ZEDObjectWrapper*>& Selection)
{
	UpdateCommands();
}

ZEDObjectManager::ZEDObjectManager()
{
	RootWrapper = NULL;
}

ZEDObjectManager::~ZEDObjectManager()
{
	Deinitialize();

	if (RootWrapper != NULL)
		RootWrapper->Destroy();
}

void ZEDObjectManager::RegisterCommands()
{
	CloneCommand.SetName("ZEDObjects::CloneCommand");
	CloneCommand.SetCategory("Objects");
	CloneCommand.SetText("Clone");
	CloneCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/Copy.png");
	CloneCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_C));
	CloneCommand.OnAction += ZEDCommandDelegate::Create<ZEDObjectManager, &ZEDObjectManager::CloneCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&CloneCommand);

	DeleteCommand.SetName("ZEDObjects::DeleteCommand");
	DeleteCommand.SetCategory("Objects");
	DeleteCommand.SetText("Delete");
	DeleteCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/Delete.png");
	DeleteCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_DELETE));
	DeleteCommand.OnAction += ZEDCommandDelegate::Create<ZEDObjectManager, &ZEDObjectManager::DeleteCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&DeleteCommand);

	UpdateCommands();
}

void ZEDObjectManager::UpdateCommands()
{
	ZEDSelectionManager* SelectionManager = GetEditor()->GetSelectionManager();
	if (SelectionManager == NULL)
		return;

	bool HasSelection = (SelectionManager->GetSelection().GetCount() != 0);
	CloneCommand.SetEnabled(HasSelection);
	DeleteCommand.SetEnabled(HasSelection);
}

void ZEDObjectManager::CloneCommand_OnAction(const ZEDCommand* Command)
{
	CloneObjects(GetEditor()->GetSelectionManager()->GetSelection());
}

void ZEDObjectManager::DeleteCommand_OnAction(const ZEDCommand* Command)
{
	this->DeleteObjects(GetEditor()->GetSelectionManager()->GetSelection());
}

const ZEArray<ZEDWrapperRegistration>& ZEDObjectManager::GetWrapperClasses()
{
	return WrapperClasses;
}

void ZEDObjectManager::RegisterWrapperClass(ZEClass* WrapperClass)
{
	zeCheckError(WrapperClass == NULL, ZE_VOID, "Class is NULL.");

	for (ZESize I = 0; I < WrapperClasses.GetCount(); I++)
	{
		if (WrapperClasses[I].WrapperClass == WrapperClass)
		{
			zeError("Wrapper class is already registered. Class Name: \"%s\".", WrapperClass->GetName());
			return;
		}
	}

	const char* TargetClassName = WrapperClass->GetAttributeValue("ZEDObjectWrapper.TargetClass");
	if (strcmp(TargetClassName, "") == 0)
		return;

	ZEClass* TargetClass = ZEMTProvider::GetInstance()->GetClass(TargetClassName);
	if (TargetClass == NULL)
	{
		zeError("Target class of Wrapper class does not exists. Wrapper Class Name: \"%s\", Target Class Name: \"%s\".", 
			WrapperClass->GetName(),
			TargetClassName);
		return;
	}

	ZEDWrapperRegistration Registration;
	Registration.WrapperClass = WrapperClass;
	Registration.TargetClass = TargetClass;
	WrapperClasses.Add(Registration);
}

void ZEDObjectManager::UnregisterWrapperClass(ZEClass* WrapperClass)
{
	zeCheckError(WrapperClass == NULL, ZE_VOID, "Class is NULL.");

	for (ZESize I = 0; I < WrapperClasses.GetCount(); I++)
	{
		if (WrapperClasses[I].WrapperClass == WrapperClass)
		{
			WrapperClasses.Remove(I);
			return;
		}
	}

	zeError("Wrapper class is not registered. Class Name: \"%s\".", WrapperClass->GetName());
}

void ZEDObjectManager::SetRootWrapper(ZEDObjectWrapper* Wrapper)
{
	if (RootWrapper == Wrapper)
		return;

	if (RootWrapper != NULL)
	{
		RootWrapper->Deinitialize();
		RootWrapper->SetManager(NULL);
	}

	RootWrapper = Wrapper;

	if (RootWrapper == NULL)
		return;

	RootWrapper->SetManager(this);

	if (IsInitialized())
	{
		RootWrapper->Initialize();
		RootWrapper->Update();
	}
}

ZEDObjectWrapper* ZEDObjectManager::GetRootWrapper()
{
	return RootWrapper;
}

ZEClass* ZEDObjectManager::FindWrapperClass(ZEClass* ObjectClass)
{
	ZEClass* BestWrapperClass = NULL;
	ZESize BestWrapperClassDistance = 1000;

	for (ZESize I = 0; I < WrapperClasses.GetCount(); I++)
	{
		ZESize LocalDistance = 0;
		ZEClass* CurrentClass = ObjectClass;
		while (CurrentClass != NULL)
		{
			if (LocalDistance > BestWrapperClassDistance)
				break;

			if (CurrentClass->IsAbstract())
			{
				CurrentClass = CurrentClass->GetParentClass();
				continue;
			}

			if (CurrentClass == WrapperClasses[I].TargetClass)
			{
				BestWrapperClass = WrapperClasses[I].WrapperClass;
				BestWrapperClassDistance = LocalDistance;
			}

			LocalDistance++;
			CurrentClass = CurrentClass->GetParentClass();
		}
	}

	return BestWrapperClass;
}

ZEDObjectWrapper* ZEDObjectManager::FindWrapper(ZEObject* Object, ZEDObjectWrapper* ParentWrapper)
{
	if (ParentWrapper == NULL)
		ParentWrapper = RootWrapper;

	if (ParentWrapper == NULL)
		return NULL;

	const ZEArray<ZEDObjectWrapper*>& ChildWrappers = ParentWrapper->GetChildWrappers();
	for (ZESize I = 0 ; I < ChildWrappers.GetCount(); I++)
	{
		if (ChildWrappers[I]->GetObject() == Object)
			return ChildWrappers[I];

		FindWrapper(Object, ChildWrappers[I]);
	}

	return NULL;
}

ZEDObjectWrapper* ZEDObjectManager::WrapObject(ZEObject* Object)
{
	if (Object == NULL)
		return NULL;

	ZEDObjectWrapper* Wrapper = NULL;
	ZEClass* WrapperClass = FindWrapperClass(Object->GetClass());
	if (WrapperClass != NULL)
		Wrapper = static_cast<ZEDObjectWrapper*>(WrapperClass->CreateInstance());

	if (Wrapper != NULL)
	{
		Wrapper->SetManager(this);
		Wrapper->SetObject(Object);
	}

	return Wrapper;
}

void ZEDObjectManager::CreateObject(ZEDObjectWrapper* Parent, ZEClass* Class)
{
	ZEDCreateOperation* CreateOperation = ZEDCreateOperation::Create(Class, Parent);
	GetEditor()->GetOperationManager()->DoOperation(CreateOperation);
}

void ZEDObjectManager::DeleteObject(ZEDObjectWrapper* Wrapper)
{
	ZEArray<ZEDObjectWrapper*> Wrappers;
	Wrappers.Add(Wrapper);

	ZEDDeleteOperation* DeleteOperation = ZEDDeleteOperation::Create(Wrappers);
	GetEditor()->GetOperationManager()->DoOperation(DeleteOperation);
}

void ZEDObjectManager::DeleteObjects(const ZEArray<ZEDObjectWrapper*> Wrappers)
{
	ZEDDeleteOperation* DeleteOperation = ZEDDeleteOperation::Create(Wrappers);
	GetEditor()->GetOperationManager()->DoOperation(DeleteOperation);
}

void ZEDObjectManager::RelocateObject(ZEDObjectWrapper* Destination, ZEDObjectWrapper* Wrapper)
{
	ZEArray<ZEDObjectWrapper*> Wrappers;
	Wrappers.Add(Wrapper);

	ZEDRelocateOperation* RelocateOperation = ZEDRelocateOperation::Create(Destination, Wrappers);
	GetEditor()->GetOperationManager()->DoOperation(RelocateOperation);
}

void ZEDObjectManager::RelocateObjects(ZEDObjectWrapper* Destination, const ZEArray<ZEDObjectWrapper*> Wrappers)
{
	ZEDRelocateOperation* RelocateOperation = ZEDRelocateOperation::Create(Destination, Wrappers);
	GetEditor()->GetOperationManager()->DoOperation(RelocateOperation);
}

void ZEDObjectManager::CloneObject(ZEDObjectWrapper* Wrapper)
{
	ZEArray<ZEDObjectWrapper*> Wrappers;
	Wrappers.Add(Wrapper);

	ZEDCloneOperation* CloneOperation = ZEDCloneOperation::Create(Wrappers);
	GetEditor()->GetOperationManager()->DoOperation(CloneOperation);
}

void ZEDObjectManager::CloneObjects(const ZEArray<ZEDObjectWrapper*> Wrappers)
{
	ZEDCloneOperation* CloneOperation = ZEDCloneOperation::Create(Wrappers);
	GetEditor()->GetOperationManager()->DoOperation(CloneOperation);
}

void ZEDObjectManager::Tick(float ElapsedTime)
{
	if (RootWrapper != NULL)
		RootWrapper->Tick(ElapsedTime);
}

ZEDObjectManager* ZEDObjectManager::CreateInstance()
{
	return new ZEDObjectManager();
}
