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
#include "ZEDComponent.h"
#include "ZEDOperationManager.h"
#include "ZEDSelectionManager.h"
#include "ZEDTransformationManager.h"
#include "ZEDObjectManager.h"
#include "ZEDObjectWrapper.h"
#include "ZEDViewPort.h"
#include "ZEDViewportManager.h"
#include "ZEDEditorCore.h"
#include "ZEDAssetManager.h"
#include "ZEDObjectWrapper3D.h"
#include "ZEDUserInterface/ZEDMainWindow.h"
#include "ZEDUserInterface/ZEDCommandManager.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZEUI/ZEUIManager.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>



void ZEDEditor::RegisterCommands()
{
	NewCommand.SetName("ZEDEditor::NewCommand");
	NewCommand.SetText("New");
	NewCommand.SetCategory("File");
	NewCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/New.png");
	NewCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_N));
	NewCommand.OnAction += ZEDCommandDelegate::Create<ZEDEditor, &ZEDEditor::NewCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&NewCommand);

	OpenCommand.SetName("ZEDEditor::OpenCommand");
	OpenCommand.SetText("Open");
	OpenCommand.SetCategory("File");
	OpenCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_O));
	OpenCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/Open.png");
	OpenCommand.OnAction += ZEDCommandDelegate::Create<ZEDEditor, &ZEDEditor::OpenCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&OpenCommand);

	RecentFilesCommand.SetName("ZEDEditor::RecentFilesCommand");
	RecentFilesCommand.SetText("Recent Files");
	RecentFilesCommand.SetCategory("File");
	RecentFilesCommand.SetType(ZED_CT_LIST_COMMAND);
	RecentFilesCommand.OnAction += ZEDCommandDelegate::Create<ZEDEditor, &ZEDEditor::RecentFilesCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&RecentFilesCommand);

	SaveCommand.SetName("ZEDEditor::SaveCommand");
	SaveCommand.SetText("Save");
	SaveCommand.SetCategory("File");
	SaveCommand.SetIcon("#R:/ZEEngine/ZEGUI/Textures/SemiChecked.png");
	SaveCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL, ZED_VKK_S));
	SaveCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/Save.png");
	SaveCommand.OnAction += ZEDCommandDelegate::Create<ZEDEditor, &ZEDEditor::SaveCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&SaveCommand);

	SaveAsCommand.SetName("ZEDEditor::SaveAsCommand");
	SaveAsCommand.SetText("SaveAs");
	SaveAsCommand.SetCategory("File");
	SaveAsCommand.SetShortcut(ZEDCommandShortcut(ZED_VKM_CTRL | ZED_VKM_SHIFT, ZED_VKK_S));
	SaveAsCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/SaveAs.png");
	SaveAsCommand.OnAction += ZEDCommandDelegate::Create<ZEDEditor, &ZEDEditor::SaveAsCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&SaveAsCommand);

	CloseCommand.SetName("ZEDEditor::CloseCommand");
	CloseCommand.SetText("Close");
	CloseCommand.SetCategory("File");
	CloseCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/Close.png");
	CloseCommand.OnAction += ZEDCommandDelegate::Create<ZEDEditor, &ZEDEditor::CloseCommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&CloseCommand);

	ExitCommand.SetName("ZEDEditor::ExitCommand");
	ExitCommand.SetText("Exit");
	ExitCommand.SetCategory("Application");
	ExitCommand.OnAction += ZEDCommandDelegate::Create<ZEDEditor, &ZEDEditor::ExitCommand_OnAction>(this);
	ExitCommand.SetIcon("#R:/ZEDEditor/Icons/ZEDCommand/Exit.png");
	ZEDCommandManager::GetInstance()->RegisterCommand(&ExitCommand);

	UpdateCommands();
}

void ZEDEditor::UpdateCommands()
{
	SaveCommand.SetEnabled(State != ZED_ES_NONE);
	SaveAsCommand.SetEnabled(State != ZED_ES_NONE);
	CloseCommand.SetEnabled(State != ZED_ES_NONE);
	RecentFilesCommand.SetEnabled(RecentFilesCommand.GetListItems().GetCount() != 0);
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
	if (State == ZED_ES_NONE)
		return;

	if (FileName.IsEmpty())
	{
		QString Result = QFileDialog::getSaveFileName(GetMainWindow()->GetMainWindow(), "Save", QString(), GetExtensions().ToCString(), NULL, QFileDialog::DontResolveSymlinks);
		if (Result.isEmpty())
			return;

		FileName = Result.toStdString();
	}

	if (!Save(FileName))
	{
		QMessageBox::critical(GetMainWindow()->GetMainWindow(), "Cannot save file.", "Error", QMessageBox::Ok);
		return;
	}
}

void ZEDEditor::SaveAsCommand_OnAction(const ZEDCommand* Command)
{
	if (State == ZED_ES_NONE)
		return;

	QString Result = QFileDialog::getSaveFileName(GetMainWindow()->GetMainWindow(), "Save As", GetFileName().ToCString(), GetExtensions().ToCString(), NULL, QFileDialog::DontResolveSymlinks);
	if (Result.isEmpty())
		return;

	if (!Save(FileName))
		QMessageBox::critical(GetMainWindow()->GetMainWindow(), "Cannot save file.", "Error", QMessageBox::Ok);
}

void ZEDEditor::CloseCommand_OnAction(const ZEDCommand* Command)
{
	if (State == ZED_ES_NONE)
		return;

	if (Modified)
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
	if (Modified)
		Result = QMessageBox::question(GetMainWindow()->GetMainWindow(), "ZEDEditor", "Are you sure that you want to exit ? You unsaved changes in this file. All of your changes will be gone.", QMessageBox::Yes, QMessageBox::No);
	else
		Result = QMessageBox::question(GetMainWindow()->GetMainWindow(), "ZEDEditor", "Are you sure that you want to exit ?", QMessageBox::Yes, QMessageBox::No);

	if (Result == QMessageBox::No)
		return;

	Exit();
}

void ZEDEditor::RecentFilesCommand_OnAction(const ZEDCommand* Command)
{
	if (!Load(Command->GetListItems()[Command->GetValueIndex()]))
		QMessageBox::critical(GetMainWindow()->GetMainWindow(), "Cannot load file.", "Error", QMessageBox::Ok);
}

void ZEDEditor::PopulateRecentFiles()
{
	QSettings Settings("Zinek", GetClass()->GetName());
	QStringList RecentFiles = Settings.value("RecentFiles", QStringList()).toStringList();

	ZEArray<ZEString> Items;
	Items.SetCount(RecentFiles.count());
	for (int I = 0; I < RecentFiles.size(); I++)
		Items[I] = RecentFiles[I].toStdString();

	RecentFilesCommand.SetListItems(Items);

	UpdateCommands();
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

	AssetManager = ZEDAssetManager::CreateInstance();
	ViewportManager = ZEDViewportManager::CreateInstance();
	ObjectManager = ZEDObjectManager::CreateInstance();
	SelectionManager = ZEDSelectionManager::CreateInstance();
	TransformationManager = ZEDTransformationManager::CreateInstance();
	OperationManager = ZEDOperationManager::CreateInstance();
	UIManager = ZEUIManager::CreateInstance();

	AddComponent(AssetManager);
	AddComponent(ViewportManager);
	UIManager->Initialize();
	AddComponent(ObjectManager);
	AddComponent(TransformationManager);
	AddComponent(SelectionManager);
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

ZEDAssetManager* ZEDEditor::GetAssetManager()
{
	return AssetManager;
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

ZEDEditorState ZEDEditor::GetEditorState()
{
	return State;
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
	for (ZESize I = 0; I < Components.GetCount(); I++)
		Components[I]->Process(ElapsedTime);

	OnTick(this, ElapsedTime);
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
		
		ZEDObjectWrapper3D* RootWrapper = ZEClass::Cast<ZEDObjectWrapper3D>(GetObjectManager()->GetRootWrapper());
		if (RootWrapper != NULL)
			RootWrapper->PreRender(&PreRenderParameters);

		UIManager->Render(Renderer);
	}

	ViewportManager->Render();
}

void ZEDEditor::New()
{
	Close();

	State = ZED_ES_NEW;
	Modified = false;
	FileName = "";

	if (GetObjectManager()->GetRootWrapper() != NULL)
		GetObjectManager()->GetRootWrapper()->Clean();

	UpdateCommands();

	OnNew(this);
	OnStateChanged(this, State);
}

bool ZEDEditor::Save(const ZEString& FileName)
{
	OnSaving(this);

	bool Result = GetObjectManager()->GetRootWrapper()->Save(FileName);
	if (!Result)
		return false;

	State = ZED_ES_OPENED;
	Modified = false;
	this->FileName = FileName;

	UpdateCommands();

	RegisterRecentFile(FileName);

	OnSaved(this);
	OnStateChanged(this, State);

	return Result;
}

bool ZEDEditor::Load(const ZEString& FileName)
{
	Close();

	this->FileName = FileName;

	OnLoading(this);

	bool Result = GetObjectManager()->GetRootWrapper()->Load(FileName);
	if (!Result)
	{
		this->FileName = "";
		return false;
	}

	State = ZED_ES_OPENED;
	Modified = false;
	RegisterRecentFile(FileName);
	UpdateCommands();

	OnLoaded(this);
	OnStateChanged(this, State);

	return Result;
}

void ZEDEditor::Close()
{
	if (State == ZED_ES_NONE)
		return;

	OnClosing(this);

	GetObjectManager()->GetRootWrapper()->Clean();

	State = ZED_ES_NONE;
	Modified = false;
	FileName = "";
	
	UpdateCommands();

	OnClosed(this);
	OnStateChanged(this, State);
}

void ZEDEditor::Exit()
{
	OnExiting(this);
	if (GetCore() != NULL)
		GetCore()->Exit();
}

void ZEDEditor::MarkDocumentModified()
{
	if (State == ZED_ES_NONE)
		return;

	if (Modified)
		return;

	Modified = false;

	UpdateCommands();

	OnModified(this);
	OnStateChanged(this, State);
}

void ZEDEditor::UnmarkDocumentModified()
{
	if (State == ZED_ES_NONE)
		return;

	if (!Modified)
		return;

	Modified = false;

	UpdateCommands();

	OnUnmodified(this);
	OnStateChanged(this, State);
}

ZEDEditor::ZEDEditor()
{
	Core = NULL;
	State = ZED_ES_NONE;

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
	Deinitialize();

	while (Components.GetCount() != 0)
		Components.GetFirstItem()->Destroy();
}
