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

#include "ZEDS/ZEDelegate.h"
#include "ZEDEditorEvent.h"
#include "ZEDEvent.h"
#include "ZEDComponent.h"
#include "ZEDOperationManager.h"
#include "ZEDSelectionManager.h"
#include "ZEDTransformationManager.h"
#include "ZEDObjectManager.h"
#include "ZEDObjectWrapper.h"
#include "ZEDViewPort.h"
#include "ZEDViewportManager.h"
#include "ZEDEditorCore.h"
#include "ZEDUserInterface/ZEDMainWindow.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZEUI/ZEUIManager.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>


void ZEDEditor::DistributeEvent(const ZEDEvent* Event)
{
	for (ZESize I = 0; I < Components.GetCount(); I++)
	{
		if (Event->IsAcquired())
			break;

		Components[I]->EventReceived(Event);
	}
}

void ZEDEditor::NewMenu_activated(ZEDMenu* Menu)
{
	New();
}

void ZEDEditor::OpenMenu_activated(ZEDMenu* Menu)
{
	QString Result = QFileDialog::getOpenFileName(GetMainWindow()->GetMainWindow(), "Open", QString(), GetExtensions().ToCString());
	if (Result.isEmpty())
		return;

	if (!Load(Result.toUtf8().begin()))
		QMessageBox::critical(GetMainWindow()->GetMainWindow(), "Cannot load file.", "Error", QMessageBox::Ok);
}

void ZEDEditor::SaveMenu_activated(ZEDMenu* Menu)
{
	if (FileState == ZED_ES_NONE)
		return;

	if (!Save(FileName))
		QMessageBox::critical(GetMainWindow()->GetMainWindow(), "Cannot save file.", "Error", QMessageBox::Ok);
}

void ZEDEditor::SaveAsMenu_activated(ZEDMenu* Menu)
{
	if (FileState == ZED_ES_NONE)
		return;

	QString Result = QFileDialog::getOpenFileName(GetMainWindow()->GetMainWindow(), "Open", QString(), GetExtensions().ToCString());
	if (Result.isEmpty())
		return;

	if (!Save(FileName))
		QMessageBox::critical(GetMainWindow()->GetMainWindow(), "Cannot save file.", "Error", QMessageBox::Ok);
}

void ZEDEditor::CloseMenu_activated(ZEDMenu* Menu)
{
	if (FileState == ZED_ES_NONE)
		return;

	if (FileState == ZED_ES_MODIFIED)
	{
		int Result = QMessageBox::question(GetMainWindow()->GetMainWindow(), "ZEDEditor", "Are you sure that you want to close this file ? All of your changes will be gone.", QMessageBox::Yes, QMessageBox::No);
		if (Result == QMessageBox::No)
			return;
	}

	Close();
}

void ZEDEditor::ExitMenu_activated(ZEDMenu* Menu)
{
	int Result;
	if (FileState == ZED_ES_MODIFIED)
		Result = QMessageBox::question(GetMainWindow()->GetMainWindow(), "ZEDEditor", "Are you sure that you want to exit ? You unsaved changes in this file. All of your changes will be gone.", QMessageBox::Yes, QMessageBox::No);
	else
		Result = QMessageBox::question(GetMainWindow()->GetMainWindow(), "ZEDEditor", "Are you sure that you want to exit ?", QMessageBox::Yes, QMessageBox::No);

	if (Result == QMessageBox::No)
		return;

	Exit();
}

void ZEDEditor::RecentFilesMenu_activated(ZEDMenu* Menu)
{
	Load(Menu->GetName());
}

void ZEDEditor::UpdateMenu()
{
	SaveMenu->SetEnabled(FileState != ZED_ES_NONE);
	SaveAsMenu->SetEnabled(FileState != ZED_ES_NONE);
	CloseMenu->SetEnabled(FileState != ZED_ES_NONE);
}

void ZEDEditor::InitializeMenu()
{
	NewMenu = new ZEDMenu();
	NewMenu->SetName("New");
	NewMenu->SetSection("Open");
	NewMenu->SetCallback(ZEDelegateMethod(ZEDMenuCallback, ZEDEditor, NewMenu_activated, this));
	GetMainWindow()->AddMenu("File", NewMenu);
	
	OpenMenu = new ZEDMenu();
	OpenMenu->SetName("Open");
	OpenMenu->SetSection("Open");
	OpenMenu->SetCallback(ZEDelegateMethod(ZEDMenuCallback, ZEDEditor, OpenMenu_activated, this));
	GetMainWindow()->AddMenu("File", OpenMenu);
	
	RecentFilesMenu = new ZEDMenu();
	RecentFilesMenu->SetName("Recent Files");
	RecentFilesMenu->SetSection("Open");
	GetMainWindow()->AddMenu("File", RecentFilesMenu);

	SaveMenu = new ZEDMenu();
	SaveMenu->SetName("Save");
	SaveMenu->SetSection("Save");
	SaveMenu->SetCallback(ZEDelegateMethod(ZEDMenuCallback, ZEDEditor, SaveMenu_activated, this));
	GetMainWindow()->AddMenu("File", SaveMenu);
	
	SaveAsMenu = new ZEDMenu();
	SaveAsMenu->SetName("Save As");
	SaveAsMenu->SetSection("Save");
	SaveAsMenu->SetCallback(ZEDelegateMethod(ZEDMenuCallback, ZEDEditor, SaveAsMenu_activated, this));
	GetMainWindow()->AddMenu("File", SaveAsMenu);

	CloseMenu = new ZEDMenu();
	CloseMenu->SetName("Close");
	CloseMenu->SetSection("Close");
	CloseMenu->SetCallback(ZEDelegateMethod(ZEDMenuCallback, ZEDEditor, CloseMenu_activated, this));
	GetMainWindow()->AddMenu("File", CloseMenu);

	ExitMenu = new ZEDMenu();
	ExitMenu->SetName("Exit");
	ExitMenu->SetSection("Exit");
	ExitMenu->SetCallback(ZEDelegateMethod(ZEDMenuCallback, ZEDEditor, ExitMenu_activated, this));
	GetMainWindow()->AddMenu("File", ExitMenu);

	UpdateMenu();
}

void ZEDEditor::DeinitializeMenu()
{
	delete NewMenu;
	NewMenu = NULL;

	delete OpenMenu;
	OpenMenu = NULL;

	delete SaveMenu;
	SaveMenu = NULL;

	delete SaveAsMenu;
	SaveAsMenu = NULL;

	delete CloseMenu;
	CloseMenu = NULL;

	delete ExitMenu;
	ExitMenu = NULL;

	delete RecentFilesMenu;
	RecentFilesMenu = NULL;
}

void ZEDEditor::PopulateRecentFiles()
{
	QSettings Settings("Zinek", GetClass()->GetName());
	QStringList RecentFiles = Settings.value("RecentFiles", QStringList()).toStringList();

	while (RecentFilesMenu->GetChildMenus().GetCount() != 0)
		delete RecentFilesMenu->GetChildMenus().GetFirstItem();

	for (int I = 0; I < RecentFiles.size(); I++)
	{
		ZEDMenu* Menu = new ZEDMenu();
		Menu->SetName(RecentFiles[I].toUtf8().begin());
		Menu->SetCallback(ZEDelegateMethod(ZEDMenuCallback, ZEDEditor, RecentFilesMenu_activated, this));
		GetMainWindow()->AddMenu("File/Open::Recent Files", Menu);
	}
}

void ZEDEditor::RegisterRecentFile(const ZEString& FileName)
{
	QSettings Settings("Zinek", GetClass()->GetName());
	QStringList RecentFiles = Settings.value("RecentFiles", QStringList()).toStringList();

	int Index = RecentFiles.indexOf(FileName.ToCString());
	if (Index != -1)
		RecentFiles.removeAt(Index);

	if (RecentFiles.size() > 10)
		RecentFiles.removeLast();

	RecentFiles.insert(0, FileName.ToCString());

	Settings.setValue("File/Open::RecentFiles", RecentFiles);

	PopulateRecentFiles();
}

bool ZEDEditor::InitializeInternal()
{
	if (!ZEInitializable::InitializeInternal())
		return false;

	MainWindow = ZEDMainWindow::CreateInstance();
	AddComponent(MainWindow);

	InitializeMenu();

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

	PopulateRecentFiles();

	return true;
}

bool ZEDEditor::DeinitializeInternal()
{
	for (ZESize I = 0; I < Components.GetCount(); I++)
		Components[I]->Deinitialize();

	UIManager->Deinitialize();
	
	DeinitializeMenu();

	return ZEInitializable::DeinitializeInternal();
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

ZEDFileState ZEDEditor::GetFileState()
{
	return FileState;
}

const ZEString& ZEDEditor::GetFileName()
{
	return FileName;
}

ZEString ZEDEditor::GetExtensions()
{
	return "*.*";
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
	FileState = ZED_ES_UNMODIFIED;
	FileName = "";

	UpdateMenu();

	ZEDEditorEvent Event;
	Event.FileName = FileName;
	Event.Type = ZED_EET_NEW_FILE;
	DistributeEvent(&Event);
}

bool ZEDEditor::Save(const ZEString& FileName)
{
	FileState = ZED_ES_UNMODIFIED;
	this->FileName = FileName;

	UpdateMenu();

	ZEDEditorEvent Event;
	Event.FileName = FileName;
	Event.Type = ZED_EET_FILE_SAVED;
	DistributeEvent(&Event);

	return true;
}

bool ZEDEditor::Load(const ZEString& FileName)
{
	FileState = ZED_ES_UNMODIFIED;
	this->FileName = FileName;

	UpdateMenu();

	ZEDEditorEvent Event;
	Event.FileName = FileName;
	Event.Type = ZED_EET_FILE_LOADED;
	DistributeEvent(&Event);

	return true;
}

void ZEDEditor::Close()
{
	FileState = ZED_ES_NONE;
	FileName = NULL;

	UpdateMenu();

	ZEDEditorEvent Event;
	Event.FileName = FileName;
	Event.Type = ZED_EET_FILE_CLOSED;
	DistributeEvent(&Event);
}

void ZEDEditor::Exit()
{
	if (GetCore() != NULL)
		GetCore()->Exit();
}

void ZEDEditor::MarkDocumentModified()
{
	if (FileState == ZED_ES_NONE)
		return;

	FileState = ZED_ES_MODIFIED;

	UpdateMenu();

	ZEDEditorEvent Event;
	Event.FileName = FileName;
	Event.Type = ZED_EET_FILE_MODIFIED;
	DistributeEvent(&Event);
}

void ZEDEditor::UnmarkDocumentModified()
{
	if (FileState == ZED_ES_NONE)
		return;

	FileState = ZED_ES_UNMODIFIED;

	UpdateMenu();

	ZEDEditorEvent Event;
	Event.FileName = FileName;
	Event.Type = ZED_EET_FILE_UNMODIFIED;
	DistributeEvent(&Event);
}

ZEDEditor::ZEDEditor()
{
	Core = NULL;
	FileState = ZED_ES_NONE;

	ObjectManager = NULL;
	OperationManager = NULL;
	SelectionManager = NULL;
	TransformationManager = NULL;
	ViewportManager = NULL;
	MainWindow = NULL;
	UIManager = NULL;

	NewMenu = NULL;
	OpenMenu = NULL;
	SaveMenu = NULL;
	SaveAsMenu = NULL;
	CloseMenu = NULL;
	ExitMenu = NULL;
	RecentFilesMenu = NULL;
}

ZEDEditor::~ZEDEditor()
{
	while (Components.GetCount() != 0)
		Components.GetFirstItem()->Destroy();
}
