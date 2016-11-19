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
#include "ZEDUserInterface/ZEDCommandManager.h"


void ZEDEditor::DistributeEvent(const ZEDEvent* Event)
{
	for (ZESize I = 0; I < Components.GetCount(); I++)
	{
		if (Event->IsAcquired())
			break;

		Components[I]->EventReceived(Event);
	}
}

void ZEDEditor::RegisterCommands()
{
	NewCommand.SetName("ZEDEditor::NewCommand");
	NewCommand.SetText("New");
	NewCommand.SetCategory("File");
	NewCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_N));
	NewCommand.OnAction += ZEDCommandDelegate::Create<ZEDEditor, &ZEDEditor::NewCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&NewCommand);

	OpenCommand.SetName("ZEDEditor::OpenCommand");
	OpenCommand.SetText("Open");
	OpenCommand.SetCategory("File");
	OpenCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_O));
	OpenCommand.OnAction += ZEDCommandDelegate::Create<ZEDEditor, &ZEDEditor::OpenCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&OpenCommand);

	RecentFilesCommand.SetName("ZEDEditor::RecentFilesCommand");
	RecentFilesCommand.SetText("Recent Files");
	RecentFilesCommand.SetCategory("File");
	RecentFilesCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_N));
	RecentFilesCommand.OnAction += ZEDCommandDelegate::Create<ZEDEditor, &ZEDEditor::RecentFilesCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&RecentFilesCommand);

	SaveCommand.SetName("ZEDEditor::SaveCommand");
	SaveCommand.SetText("Save");
	SaveCommand.SetCategory("File");
	SaveCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_S));
	SaveCommand.OnAction += ZEDCommandDelegate::Create<ZEDEditor, &ZEDEditor::SaveCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&SaveCommand);

	SaveAsCommand.SetName("ZEDEditor::SaveAsCommand");
	SaveAsCommand.SetText("SaveAs");
	SaveAsCommand.SetCategory("File");
	SaveAsCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL | ZED_VKM_SHIFT, ZED_VKK_S));
	SaveAsCommand.OnAction += ZEDCommandDelegate::Create<ZEDEditor, &ZEDEditor::SaveAsCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&SaveAsCommand);

	CloseCommand.SetName("ZEDEditor::CloseCommand");
	CloseCommand.SetText("Close");
	CloseCommand.SetCategory("File");
	CloseCommand.OnAction += ZEDCommandDelegate::Create<ZEDEditor, &ZEDEditor::CloseCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&CloseCommand);

	ExitCommand.SetName("ZEDEditor::ExitCommand");
	ExitCommand.SetText("Exit");
	ExitCommand.SetCategory("Application");
	ExitCommand.OnAction += ZEDCommandDelegate::Create<ZEDEditor, &ZEDEditor::ExitCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&ExitCommand);
}

void ZEDEditor::NewCommand_OnAction(const ZEDCommand* Command)
{
	New();
}

void ZEDEditor::OpenCommand_OnAction(const ZEDCommand* Command)
{
	QString Result = QFileDialog::getOpenFileName(GetMainWindow()->GetMainWindow(), "Open", QString(), GetExtensions().ToCString());
	if (Result.isEmpty())
		return;

	if (!Load(Result.toUtf8().begin()))
		QMessageBox::critical(GetMainWindow()->GetMainWindow(), "Cannot load file.", "Error", QMessageBox::Ok);
}

void ZEDEditor::SaveCommand_OnAction(const ZEDCommand* Command)
{
	if (FileState == ZED_ES_NONE)
		return;

	if (!Save(FileName))
		QMessageBox::critical(GetMainWindow()->GetMainWindow(), "Cannot save file.", "Error", QMessageBox::Ok);
}

void ZEDEditor::SaveAsCommand_OnAction(const ZEDCommand* Command)
{
	if (FileState == ZED_ES_NONE)
		return;

	QString Result = QFileDialog::getOpenFileName(GetMainWindow()->GetMainWindow(), "Open", QString(), GetExtensions().ToCString());
	if (Result.isEmpty())
		return;

	if (!Save(FileName))
		QMessageBox::critical(GetMainWindow()->GetMainWindow(), "Cannot save file.", "Error", QMessageBox::Ok);
}

void ZEDEditor::CloseCommand_OnAction(const ZEDCommand* Command)
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

void ZEDEditor::ExitCommand_OnAction(const ZEDCommand* Command)
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

void ZEDEditor::RecentFilesCommand_OnAction(const ZEDCommand* Command)
{
	Load(Command->GetValue().GetString());
}

void ZEDEditor::UpdateCommands()
{
	SaveCommand.SetEnabled(FileState != ZED_ES_NONE);
	SaveAsCommand.SetEnabled(FileState != ZED_ES_NONE);
	CloseCommand.SetEnabled(FileState != ZED_ES_NONE);
}

void ZEDEditor::PopulateRecentFiles()
{
	QSettings Settings("Zinek", GetClass()->GetName());
	QStringList RecentFiles = Settings.value("RecentFiles", QStringList()).toStringList();

	ZEArray<ZEString> Items;
	for (int I = 0; I < RecentFiles.size(); I++)
		Items[I] = RecentFiles[I].toStdString();

	RecentFilesCommand.SetListItems(Items);
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
	Settings.setValue("RecentFiles", RecentFiles);

	PopulateRecentFiles();
}

bool ZEDEditor::InitializeInternal()
{
	if (!ZEInitializable::InitializeInternal())
		return false;

	RegisterCommands();

	MainWindow = ZEDMainWindow::CreateInstance();

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

	AddComponent(MainWindow);

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

ZEUIManager* ZEDEditor::GetUIManager()
{
	return UIManager;
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

	if (GetObjectManager() != NULL)
		GetObjectManager()->Tick(ElapsedTime);
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
		
		UIManager->Render(Renderer);
	}

	ViewportManager->Render();
}

void ZEDEditor::New()
{
	FileState = ZED_ES_UNMODIFIED;
	FileName = "";

	UpdateCommands();

	ZEDEditorEvent Event;
	Event.FileName = FileName;
	Event.Type = ZED_EET_NEW_FILE;
	DistributeEvent(&Event);
}

bool ZEDEditor::Save(const ZEString& FileName)
{
	FileState = ZED_ES_UNMODIFIED;
	this->FileName = FileName;

	UpdateCommands();

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

	UpdateCommands();

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

	UpdateCommands();

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

	UpdateCommands();

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

	UpdateCommands();

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
}

ZEDEditor::~ZEDEditor()
{
	while (Components.GetCount() != 0)
		Components.GetFirstItem()->Destroy();
}
