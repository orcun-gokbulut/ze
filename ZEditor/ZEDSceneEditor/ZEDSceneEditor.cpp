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

#define WINDOWS_LEAN_AND_MEAN
#include "ui_ZEDSceneEditor.h"
#include "ZEGraphics/ZEDirect3D9/ZED3D9FrameRenderer.h"
#include <QFileDialog.h>
#include <QWidget>
#include <QAction>
#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QTabWidget>
#include <windows.h>
#include "ZESDK.h"
#include "ZEDSceneEditor.h"
#include "ZEGame\ZEPlayer.h"
#include "ZEGame\ZEGame.h"
#include "ZEGame\ZEScene.h"
#include "ZECore\ZEModuleManager.h"
#include "ZECore\ZEWindow.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEMath/ZEMathDefinitions.h"

#include "ZEDCommonEntities/ZEDScreenAxis.h"


#include <ZEGame\ZEEntityProvider.h>
#include <ZEGraphics\ZEDirectionalLight.h>
#include <ZECore\ZEOptionManager.h>
#include <ZEModel\ZEModel.h>
#include <ZEGame\ZESkyBrush.h>
#include "ZEDCommonControls\CSS.h"




MapEditor::MapEditor(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	SplashScreen = new ZEDLoadingScreen();
	SplashScreen->show();

	SplashScreen->SetNotificationText("Setting Up UI...");

	ui = new Ui::MapEditorClass();
	ui->setupUi(this);

	SplashScreen->SetNotificationText("Reading Working Directory...");
	this->WorkingDirectory = QDir::currentPath() + QString("/resources");

	SplashScreen->SetNotificationText("Setting Up Assert Browser...");
	//AssertBrowser = new NewZEDAssertBrowser(QDir::currentPath());	

	SelectionDirtyFlag = false;
	
	this->MainLoopTimer = new QTimer(this);
	MainLoopTimer->start(0);

	BackupSaveTimer = new QTimer();
	BackupSaveTimer->start(120000);

	SplashScreen->SetNotificationText("Making Connections...");
	MakeConnections();
	SplashScreen->SetNotificationText("Loading Selection Item Plugnins...");

	SplashScreen->SetNotificationText("Loading Plugins...");
	this->InitializePlugIns();

	SplashScreen->SetNotificationText("Setting Up Viewport...");
	ViewPort = new ZEDViewPort(this ,&(this->SelectedItems), this->WorkingDirectory, ui->centralWidget);
	ui->ViewPort = ViewPort;
	ui->gridLayout->addWidget(ui->ViewPort, 0, 0, 1, 1);

	PropertyWindowManager = NULL;
	SplashScreen->SetNotificationText("Setting Up Console...");
	this->Console = new ZEDConsole(ui->ConsoleInput,ui->ConsoleOutput);

	SplashScreen->SetNotificationText("Initializing Engine...");
	StartEngine();

	this->Game = zeCore->GetGame();
	this->Scene = zeCore->GetGame()->GetScene();
	this->Player = ZEPlayer::CreateInstance();
	this->Scene->SetActiveCamera(this->Player->GetCamera());
	this->Scene->AddEntity(this->Player);
	Player->SetPosition(ZEVector3(0.0f, 2.0f, 0.0f));
	this->Player->GetCamera()->SetFOV(ZE_PI / 3);

	this->GenerateAssertList();	
	EntitySelector = new ZEDEntitySelector();
	this->InitializeAdditionalToolBarItems();
	this->SaveFlag = false;	
	ViewPort->SetPlayerHandle(Player);
	ViewPort->SetSceneHandle(this->Scene);

	SplashScreen->SetNotificationText("Initializing Gizmos...");
	ZEDGizmo::BaseInitialize();
	GizmoMode = ZED_GM_NONE;

	SceneList = new ZEDSceneList(Scene, this);
	addDockWidget(Qt::LeftDockWidgetArea, SceneList);

	QObject::connect(this->SceneList->SceneTree, SIGNAL(itemSelectionChanged()), this, SLOT(UpdateSelectedEntitiesBySceneList()));
	QObject::connect(this->SceneList, SIGNAL(visibilityChanged(bool)), ui->SceneListOpenAction, SLOT(setChecked(bool)));

	SplashScreen->hide();
	showMaximized();
	this->statusBar()->showMessage("Ready");
	//Grid = new ZEDGrid(Scene);

	//ZEDScreenAxisHelper* Helper = new ZEDScreenAxisHelper(Player);
	//Scene->AddEntity(Helper);

	ZEDirectionalLight* SceneLight = ZEDirectionalLight::CreateInstance();
	SceneLight->Destroy();

	ZEModel* Model = ZEModel::CreateInstance();
	Model->Destroy();

	ZESkyBrush* Sky = ZESkyBrush::CreateInstance();
	Sky->Destroy();

	//setStyleSheet(GetCSSFromFile(":/CSS/MapEditor.qss"));

	ZED3D9FrameRenderer* Renderer = ((ZED3D9FrameRenderer*)(Scene->GetRenderer()));

	new ZEDPropertyWindowManager(0, &Renderer->HDRProcessor, QString());

	LoadTrees();
	LoadBushes();
	LoadFlowers();

}

void MapEditor::MakeConnections()
{
	QObject::connect(MainLoopTimer,					SIGNAL(timeout()),			this,					SLOT(EngineMainLoop()));
	QObject::connect(MainLoopTimer,					SIGNAL(timeout()),			this,					SLOT(EventsLoop()));
	QObject::connect(MainLoopTimer,					SIGNAL(timeout()),			this,					SLOT(SelectionsLoop()));
	QObject::connect(MainLoopTimer,					SIGNAL(timeout()),			this,					SLOT(UpdateCamPos()));

	QObject::connect(BackupSaveTimer,				SIGNAL(timeout()),			this,					SLOT(BackupSave()));

	QObject::connect(ui->NewAction,					SIGNAL(triggered(bool)),	this,					SLOT(NewMapActionTriggered()));
	QObject::connect(ui->LoadAction,				SIGNAL(triggered(bool)),	this,					SLOT(LoadMapActionTriggered()));
	QObject::connect(ui->SaveAction,				SIGNAL(triggered(bool)),	this,					SLOT(SaveSceneActionTriggered()));
	QObject::connect(ui->LoadSceneAction,			SIGNAL(triggered(bool)),	this,					SLOT(LoadSceneActionTriggered()));
	QObject::connect(ui->ConsoleInput,				SIGNAL(returnPressed()),	this,					SLOT(ConsoleInput()));
	QObject::connect(ui->EntityBrowserOpenAction,	SIGNAL(triggered(bool)),	this,					SLOT(ShowEntitySelector()));
	QObject::connect(ui->AddButton,					SIGNAL(clicked()),			this,					SLOT(NewEntityToScene()));
	QObject::connect(ui->MoveAction,				SIGNAL(triggered(bool)),	this,					SLOT(MoveActionTriggered()));
	QObject::connect(ui->RotateAction,				SIGNAL(triggered(bool)),	this,					SLOT(RotateActionTriggered()));
	QObject::connect(ui->ScaleAction,				SIGNAL(triggered(bool)),	this,					SLOT(ScaleActionTriggered()));
	QObject::connect(ui->SelectAction,				SIGNAL(triggered(bool)),	this,					SLOT(SelectActionTriggered()));
	QObject::connect(ui->StepSizeSpinBox,			SIGNAL(valueChanged(double)),	this,					SLOT(StepSizeChanged()));
	QObject::connect(ui->DeleteAction,				SIGNAL(triggered(bool)),	this,					SLOT(DeleteActionTriggered()));
	QObject::connect(ui->CopyAction,				SIGNAL(triggered(bool)),	this,					SLOT(CopyActionTriggered()));
	QObject::connect(ui->GoToEntityAction,			SIGNAL(triggered(bool)),	this,					SLOT(GoToEntityActionTriggered()));
	QObject::connect(ui->CloseAction,				SIGNAL(triggered(bool)),	this,					SLOT(CloseMapActionTriggered()));
	QObject::connect(ui->SaveSceneAsAction,			SIGNAL(triggered(bool)),	this,					SLOT(SaveSceneAsActionTriggered()));
	//QObject::connect(ui->AssertsWindowOpenAction,	SIGNAL(triggered(bool)),	this->AssertBrowser,	SLOT(show()));
	QObject::connect(ui->actionUndo,				SIGNAL(triggered(bool)),	this,					SLOT(UndoActionTriggered()));
	QObject::connect(ui->actionRedo,				SIGNAL(triggered(bool)),	this,					SLOT(RedoActionTriggered()));
	QObject::connect(ui->HideAction,				SIGNAL(triggered(bool)),	this,					SLOT(HideActionTriggered()));
	QObject::connect(ui->UnhideAction,				SIGNAL(triggered(bool)),	this,					SLOT(UnHideActionTriggered()));
	QObject::connect(ui->SceneListOpenAction,		SIGNAL(triggered(bool)),	this,					SLOT(SceneListOpenActionTriggered(bool)));
	QObject::connect(this->ui->GridToogleAction,	SIGNAL(toggled(bool)),		this,					SLOT(ChangeGridVisibility(bool)));

	QObject::connect(ui->actionGenerate_Random_Tree, SIGNAL(triggered(bool)), this, SLOT(GenerateRandomTree()));
	QObject::connect(ui->actionGenerate_Random_Bush, SIGNAL(triggered(bool)), this, SLOT(GenerateRandomBush()));
	QObject::connect(ui->actionGenerate_Random_Flower, SIGNAL(triggered(bool)), this, SLOT(GenerateRandomFlowers()));
}

//Vegetation

void MapEditor::LoadTrees()
{
	Trees.SetCount(0);
	Trees.Add(ZEModelResource::LoadSharedResource("Tree11.zemodel"));
	Trees.Add(ZEModelResource::LoadSharedResource("Tree12.zemodel"));
	Trees.Add(ZEModelResource::LoadSharedResource("Tree13.zemodel"));
	Trees.Add(ZEModelResource::LoadSharedResource("Tree14.zemodel"));
	Trees.Add(ZEModelResource::LoadSharedResource("Tree15.zemodel"));
	Trees.Add(ZEModelResource::LoadSharedResource("Tree21.zemodel"));
	Trees.Add(ZEModelResource::LoadSharedResource("Tree22.zemodel"));
	Trees.Add(ZEModelResource::LoadSharedResource("Tree23.zemodel"));
	Trees.Add(ZEModelResource::LoadSharedResource("Tree24.zemodel"));
	Trees.Add(ZEModelResource::LoadSharedResource("Tree25.zemodel"));
	Trees.Add(ZEModelResource::LoadSharedResource("Tree31.zemodel"));
	Trees.Add(ZEModelResource::LoadSharedResource("Tree32.zemodel"));
	Trees.Add(ZEModelResource::LoadSharedResource("Tree33.zemodel"));
	Trees.Add(ZEModelResource::LoadSharedResource("Tree34.zemodel"));
	Trees.Add(ZEModelResource::LoadSharedResource("Tree35.zemodel"));
}

void MapEditor::LoadBushes()
{
	Bushes.SetCount(0);
	Bushes.Add(ZEModelResource::LoadSharedResource("Bush11.zemodel"));
	Bushes.Add(ZEModelResource::LoadSharedResource("Bush21.zemodel"));
	Bushes.Add(ZEModelResource::LoadSharedResource("Bush31.zemodel"));
}

void MapEditor::LoadFlowers()
{
	Flowers.SetCount(0);
	Flowers.Add(ZEModelResource::LoadSharedResource("Flower11.zemodel"));
	Flowers.Add(ZEModelResource::LoadSharedResource("Flower21.zemodel"));
}

void MapEditor::GenerateRandomFlowers()
{
	for (int I = 0; I < 150; I++)
	{
		ZEModel* Flower = ZEModel::CreateInstance();
		Flower->SetModelResource(Flowers[rand() % 2]);

		ZEVector3 TempScale;
		float RandomFloat;
		RandomFloat = (float(rand() % 10));

		if (RandomFloat < 3.0f)
			RandomFloat = 5.0f;

		TempScale.x = RandomFloat;
		TempScale.y = RandomFloat;
		TempScale.z = RandomFloat;
		Flower->SetScale(TempScale);

		ZEVector3 RandomPosition;
		RandomPosition.x = -50 + rand() % 100;
		RandomPosition.z = -100 + rand() % 200;
		RandomPosition.y = 3;
		Flower->SetPosition(RandomPosition);
		Scene->AddEntity(Flower);
	}

	SceneList->Update();
}

void MapEditor::GenerateRandomTree()
{
	for (int I = 0; I < 150; I++)
	{
		ZEModel* Tree = ZEModel::CreateInstance();
		Tree->SetModelResource(Trees[rand() % 15]);
		/*ZEQuaternion Temp;
		ZEQuaternion::CreateFromEuler(Temp, 0, rand() % 360, 0);
		Tree->SetRotation(Temp);*/
		ZEVector3 TempScale;
		float RandomFloat;
		RandomFloat = float(rand() % 10) / 25.0;

		if (RandomFloat < 0.2f)
			RandomFloat = 0.2f;

		TempScale.x = RandomFloat;
		TempScale.y = RandomFloat;
		TempScale.z = RandomFloat;
		Tree->SetScale(TempScale);

		ZEVector3 RandomPosition;
		RandomPosition.x = -50 + rand() % 100;
		RandomPosition.z = -100 + rand() % 200;
		RandomPosition.y = 2;
		Tree->SetPosition(RandomPosition);
		Scene->AddEntity(Tree);
	}

	SceneList->Update();
}

void MapEditor::GenerateRandomBush()
{
	for (int I = 0; I < 150; I++)
	{
		ZEModel* Bush = ZEModel::CreateInstance();
		Bush->SetModelResource(Bushes[rand() % 3]);
		/*ZEQuaternion Temp;
		ZEQuaternion::CreateFromEuler(Temp, 0, rand() % 360, 0);
		Tree->SetRotation(Temp);*/
		ZEVector3 TempScale;
		float RandomFloat;
		RandomFloat = (float(rand() % 4) / 2.0f);

		if (RandomFloat < 1.0f)
			RandomFloat = 1.2f;

		TempScale.x = RandomFloat;
		TempScale.y = RandomFloat;
		TempScale.z = RandomFloat;
		Bush->SetScale(TempScale);

		ZEVector3 RandomPosition;
		RandomPosition.x = -50 + rand() % 100;
		RandomPosition.z = -100 + rand() % 200;
		RandomPosition.y = 1.8f;
		Bush->SetPosition(RandomPosition);
		Scene->AddEntity(Bush);
	}

	SceneList->Update();
}

void MapEditor::StartEngine()
{
	zeCore->GetOptions()->Load("options.ini");
	zeCore->GetOptions()->ResetChanges();
	zeCore->GetWindow()->SetWindowType(ZE_WT_COMPONENT);
	zeCore->GetWindow()->SetComponentWindowHandle(ui->ViewPort->winId());
	zeInitialize(GetModuleHandle(NULL), ui->ViewPort->winId());
	//zeCore->GetGame()->GetScene()->SetVisualDebugElements(ZE_VDE_NONE);
}

MapEditor::~MapEditor()
{
	ZEConsole::GetInstance()->SetConsoleInterface(NULL);
}

void MapEditor::BackupSave()
{	
	for (int I = 0; I < SelectedItems.GetCount(); I++)
	{
		delete SelectedItems[I];
	}

	SelectedItems.Clear();
	SelectionDirtyFlag = true;

	zeCore->GetGame()->GetScene()->Save("Backup.ZESCENE");
}

void MapEditor::NewMapActionTriggered()
{
	QString SelectedFilePath = QFileDialog::getOpenFileName(0,QString("New Map"),QString(this->WorkingDirectory),QString("*.ZEMAP"),0,0);
	
	if(SelectedFilePath.count() != 0)
	{
		SelectedFilePath = SelectedFilePath.remove(this->WorkingDirectory);
		//zeCore->GetGame()->GetScene()->LoadMap((const char*)SelectedFilePath.toLatin1());
	}
}


void MapEditor::LoadMapActionTriggered()
{
	QString SelectedFilePath = QFileDialog::getOpenFileName(0,QString("Load Map"),QString(this->WorkingDirectory),QString("*.ZEMAP"),0,0);

	if(SelectedFilePath.count() != 0)
	{
		SelectedFilePath = SelectedFilePath.remove(this->WorkingDirectory);
		//zeCore->GetGame()->GetScene()->LoadMap((const char*)SelectedFilePath.toLatin1());
	}
}

void MapEditor::LoadSceneActionTriggered()
{
	QString SelectedFilePath = QFileDialog::getOpenFileName(0,QString("Load Scene"),QString(this->WorkingDirectory),QString("*.ZESCENE"),0,0);

	if(SelectedFilePath.count() != 0)
	{
		SelectedFilePath = SelectedFilePath.remove(this->WorkingDirectory);
		zeCore->GetGame()->GetScene()->Load((const char*)SelectedFilePath.toLatin1());
	}

	SceneList->Update();

	for (int I = 0; I < Scene->GetEntities().GetCount(); I++)
	{
		if(QString(Scene->GetEntities()[I]->GetClassDescription()->GetName()) == QString("ZEPlayer"))
			ViewPort->SetPlayerHandle((ZEPlayer*)(Scene->GetEntities()[I]));
	}
}

void MapEditor::SaveSceneActionTriggered()
{

	if(SaveFlag == false)
	{
		QString SelectedFilePath = QFileDialog::getSaveFileName(0,QString("Save Scene"),QString(this->WorkingDirectory),QString("*.ZESCENE"),0,0);
	
		if(SelectedFilePath.count() != 0)
		{
			SelectedFilePath = SelectedFilePath.remove(this->WorkingDirectory);
			SelectedFilePath = SelectedFilePath.remove(0,1);
			zeCore->GetGame()->GetScene()->Save((const char*)SelectedFilePath.toLatin1());
			this->CurrentFileName = SelectedFilePath;
			SaveFlag = true;
		}
	}

	if(SaveFlag == true)
	{
		zeCore->GetGame()->GetScene()->Save((const char*)this->CurrentFileName.toLatin1());
	}
}

void MapEditor::SaveSceneAsActionTriggered()
{
	QString SelectedFilePath = QFileDialog::getSaveFileName(0,QString("Save Scene As"),QString(this->WorkingDirectory),QString("*.ZESCENE"),0,0);

	if(SelectedFilePath.count() != 0)
	{
		SelectedFilePath = SelectedFilePath.remove(this->WorkingDirectory);
		SelectedFilePath = SelectedFilePath.remove(0,1);
		zeCore->GetGame()->GetScene()->Save((const char*)SelectedFilePath.toLatin1());
		this->CurrentFileName = SelectedFilePath;
		SaveFlag = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MapEditor::EngineMainLoop()
{
	zeMainLoop();
}

void MapEditor::EventsLoop()
{
	if(SelectionDirtyFlag == true)
		SelectionChangedEvent();

	SelectionDirtyFlag = false;
}

void MapEditor::SelectionChangedEvent()
{
	if(PropertyWindowManager != NULL)
	{
		PropertyWindowManager->close();
		delete PropertyWindowManager;
		PropertyWindowManager = NULL;

		for (int I = 0; I < CustomPropertyWidgets.GetCount(); I++)
		{
			CustomPropertyWidgets[I]->close();
			delete CustomPropertyWidgets[I];
		}
	}

	CustomPropertyWidgets.Clear();

	ui->SelectedEntityCounterLabel->setText(QString().setNum(SelectedItems.GetCount()) + QString(" Item(s) Selected"));

	if(SelectedItems.GetCount() == 0)
		SceneList->ClearAllSelections();

	this->ViewPort->setFocus(Qt::OtherFocusReason);

	if(SelectedItems.GetCount() != 1)
		return;

	if(SelectedItems.GetCount() == 1)
		DisplaySelectedEntityProperties();
}

void MapEditor::SelectionsLoop()
{
	for (int I = 0; I < SelectedItems.GetCount(); I++)
	{
		SelectedItems[I]->Update();
	}
}

void MapEditor::ConsoleInput()
{
	this->Console->Input((const char*)ui->ConsoleInput->text().toLatin1());
	ui->ConsoleInput->clear();
}

void MapEditor::ShowEntitySelector()
{
	ZEArray<ZEEntity*>	SelectedEntites;
	EntitySelector->GetSelectedEntities(&SelectedEntites);
	
	ZEEntity* CurrentEntity;

	for(int I = 0; I < SelectedItems.GetCount(); I++)
		delete SelectedItems[I];

	SelectedItems.Clear();

	for(int I = 0;  I < SelectedEntites.GetCount(); I++)
	{
		CurrentEntity = SelectedEntites[I];
		SelectedItems.Add(CreateSelectionItem(CurrentEntity));
	}

	SelectionDirtyFlag = true;
}

void MapEditor::SceneListOpenActionTriggered(bool Checked)
{
	if (Checked)
		SceneList->show();
	else
		SceneList->hide();
}

void MapEditor::DisplaySelectedEntityProperties()
{
	ZEArray<QWidget*> CPropertyWidgets = SelectedItems[0]->GetCustomPropertyWidgets(ui->PropertiesTabWidget);
	if (CPropertyWidgets.GetCount() != 0)
	{
		for(int I = 0; I < CPropertyWidgets.GetCount(); I++)
		{
			ui->PropertiesTabWidget->addTab(CPropertyWidgets[I], CPropertyWidgets[I]->objectName());
			CustomPropertyWidgets.Add(CPropertyWidgets[I]);
		}
	}

	PropertyWindowManager = new ZEDPropertyWindowManager(ui->PropertiesTabWidget, SelectedItems[0]->GetClass(), WorkingDirectory);
	ui->PropertiesTabLayout->addWidget(PropertyWindowManager);
}

void MapEditor::UpdatePropertyWidgetValues()
{
	if (PropertyWindowManager != NULL)
		PropertyWindowManager->UpdatePropertyWidgetValues();
}


void MapEditor::GenerateAssertList()
{	
	for(int I = 0; I < ZEEntityProvider::GetInstance()->GetClasses().GetCount(); I++)
	{
		QString Temp = ZEEntityProvider::GetInstance()->GetClasses().GetItem(I)->GetName();
		ui->AssertsList->addItem(QString(ZEEntityProvider::GetInstance()->GetClasses().GetItem(I)->GetName()));
	}
}


void MapEditor::NewEntityToScene()
{
	QString TempString = ui->AssertsList->currentItem()->text().toLatin1();
	ZEEntity* Entity = (ZEEntity*)(ZEEntityProvider::GetInstance()->CreateInstance(((const char*)TempString.toLatin1())));
	Scene->AddEntity(Entity);

	TempString = TempString.remove(QString("ZE"));
	QString EntityName = TempString + QString(" ") + QString().setNum(Entity->GetEntityId());
	Entity->SetName((const char*)EntityName.toLatin1());

	for(int I = 0; I < SelectedItems.GetCount(); I++)
		delete SelectedItems[I];

	SelectedItems.Clear();
	SelectedItems.Add(CreateSelectionItem(Entity));
	SelectionDirtyFlag = true;

	SceneList->AddItem(Entity);
}

void MapEditor::SelectActionTriggered()
{
	ui->MoveAction->setChecked(false);
	ui->ScaleAction->setChecked(false);
	ui->RotateAction->setChecked(false);
	
	if(!ui->SelectAction->isChecked())
		ui->SelectAction->setChecked(true);
	
	GizmoMode = ZED_GM_NONE;

	for(int I = 0; I < SelectedItems.GetCount(); I++)
		SelectedItems[I]->SetGizmoMode(GizmoMode);
}

void MapEditor::MoveActionTriggered()
{
	ui->SelectAction->setChecked(false);
	ui->ScaleAction->setChecked(false);
	ui->RotateAction->setChecked(false);
	
	if(!ui->MoveAction->isChecked())
		ui->MoveAction->setChecked(true);
	
	GizmoMode = ZED_GM_MOVE;

	for(int I = 0; I < SelectedItems.GetCount(); I++)
		SelectedItems[I]->SetGizmoMode(GizmoMode);
}
void MapEditor::RotateActionTriggered()
{
	ui->SelectAction->setChecked(false);
	ui->ScaleAction->setChecked(false);
	ui->MoveAction->setChecked(false);
	
	if(!ui->RotateAction->isChecked())
		ui->RotateAction->setChecked(true);
	
	GizmoMode = ZED_GM_ROTATE;

	for(int I = 0; I < SelectedItems.GetCount(); I++)
		SelectedItems[I]->SetGizmoMode(GizmoMode);
}

void MapEditor::ScaleActionTriggered()
{
	ui->SelectAction->setChecked(false);
	ui->MoveAction->setChecked(false);
	ui->RotateAction->setChecked(false);
	
	if(!ui->ScaleAction->isChecked())
		ui->ScaleAction->setChecked(true);
	
	GizmoMode = ZED_GM_SCALE;

	for(int I = 0; I < SelectedItems.GetCount(); I++)
		SelectedItems[I]->SetGizmoMode(GizmoMode);
}

void MapEditor::StepSizeChanged()
{
	this->ViewPort->UpdateStepSize(ui->StepSizeSpinBox->value());
}

void MapEditor::DeleteActionTriggered()
{
	for (int I = 0; I < SelectedItems.GetCount(); I++)
	{
		QObject::disconnect(this->SceneList->SceneTree, SIGNAL(itemSelectionChanged()), this, SLOT(UpdateSelectedEntitiesBySceneList()));
		SceneList->RemoveItem(((ZEEntity*)SelectedItems[I]->GetClass()));
		QObject::connect(this->SceneList->SceneTree, SIGNAL(itemSelectionChanged()), this, SLOT(UpdateSelectedEntitiesBySceneList()));
		Scene->RemoveEntity((ZEEntity*)SelectedItems[I]->GetClass());
	}

	for (int I = 0; I < SelectedItems.GetCount(); I++)
	{
		delete SelectedItems[I];
	}

	SelectedItems.Clear();
	SelectionDirtyFlag = true;
}

void MapEditor::CopyActionTriggered()
{
	ZEEntity* EntityToCopy;
	ZEClassDescription* ClassDescription;
	const ZEPropertyDescription* PropertyDescriptions;
	int	PropertyCount = 0;
	ZEEntity* NewEntity = NULL;
	ZEVariant TempVariant;

	QString EntityName;

	ZEArray<ZEEntity*> CreatedEntities;

	for (int I = 0; I < SelectedItems.GetCount(); I++)
	{
		EntityToCopy = ((ZEEntity*)(SelectedItems[I]->GetClass()));
		ClassDescription = EntityToCopy->GetClassDescription();
		NewEntity = (ZEEntity*)(ZEEntityProvider::GetInstance()->CreateInstance(ClassDescription->GetName()));
		Scene->AddEntity(NewEntity);

		int TrueId = NewEntity->GetEntityId();
		EntityName = QString(ClassDescription->GetName()) + QString(" ") + QString().setNum(TrueId);
		EntityName = EntityName.remove(QString("ZE"));
				
		while (ClassDescription != NULL)
		{
			PropertyCount = ClassDescription->GetPropertyCount();
			PropertyDescriptions = ClassDescription->GetProperties();

			for(int J = 0; J < PropertyCount; J++)
			{
				EntityToCopy->GetProperty(PropertyDescriptions[J].Name, TempVariant);
				NewEntity->SetProperty(PropertyDescriptions[J].Name, TempVariant);			
			}			

			ClassDescription = ClassDescription->GetParent();
		}
		
		NewEntity->SetEntityId(TrueId);
		NewEntity->SetName((const char*)EntityName.toLatin1());
		CreatedEntities.Add(NewEntity);
	}

	for(int I = 0; I < SelectedItems.GetCount(); I++)
		delete SelectedItems[I];

	SelectedItems.Clear();

	for (int I = 0; I < CreatedEntities.GetCount(); I++)
	{
		SelectedItems.Add(CreateSelectionItem(CreatedEntities[I]));
		SceneList->AddItem(CreatedEntities[I]);
	}

	SelectionDirtyFlag = true;
}

void MapEditor::GoToEntityActionTriggered()
{
	ZEVector3	FocusPosition;
	size_t		SelectedItemCount = SelectedItems.GetCount();

	for (int I = 0; I < SelectedItemCount; I++)
		FocusPosition = FocusPosition + SelectedItems[I]->GetPosition();

	FocusPosition.x = FocusPosition.x / SelectedItemCount;
	FocusPosition.y = FocusPosition.y / SelectedItemCount;
	FocusPosition.z = FocusPosition.z / SelectedItemCount;

	ZEVector3 NewPosition, NewDirection;
	ZEQuaternion Direction;
	Direction = zeCore->GetGame()->GetScene()->GetActiveCamera()->GetWorldRotation();
	ZEQuaternion::VectorProduct(NewDirection,Direction,ZEVector3(0.0f,0.0f,1.0f));
	ZEVector3::Scale(NewDirection,NewDirection, (zeCore->GetGame()->GetScene()->GetActiveCamera()->GetNearZ() + 10.0f));
	ZEVector3::Sub(NewPosition, FocusPosition, NewDirection);
	zeCore->GetGame()->GetScene()->GetActiveCamera()->GetOwner()->SetPosition(NewPosition);
}

void MapEditor::CloseMapActionTriggered()
{

}

void MapEditor::UpdateSelectedEntitiesByRayCast()
{

}

void MapEditor::UpdateSelectedEntitiesBySceneList()
{
	const QList<QTreeWidgetItem*> SelectedListItems = SceneList->SceneTree->selectedItems();
	ZEArray<ZEEntity*>			SelectedListEntities;

	for(int I = 0; I < SelectedListItems.count(); I++)
	{
		if(SelectedListItems[I]->childCount() != 0)
		{
			int ChildCount = SelectedListItems[I]->childCount();

			for (int J = 0; J < ChildCount; J++)
			{
				SelectedListEntities.Add(((ZEDSceneListItem*)(SelectedListItems[I]->child(J)))->Entity);
			}
		}
		else
		{
			 SelectedListEntities.Add(((ZEDSceneListItem*)(SelectedListItems[I]))->Entity);
		}
	}

	for (int I = 0; I < SelectedItems.GetCount(); I++)
		delete SelectedItems[I];

	SelectedItems.Clear();

	for (int I = 0; I < SelectedListEntities.GetCount(); I++)
		SelectedItems.Add(CreateSelectionItem(SelectedListEntities[I]));

	SelectionDirtyFlag = true;
}

void MapEditor::InitializeAdditionalToolBarItems()
{
	this->ui->OperationsToolBar->addSeparator();
	this->OperationSpaceLabel = new QLabel(QString(" Working Space  "), this->ui->OperationsToolBar);
	this->OperationSpaceComboBox = new QComboBox(this->ui->OperationsToolBar);
	this->OperationSpaceComboBox->addItem(QString(" World	"));
	this->OperationSpaceComboBox->addItem(QString(" Local	"));
	this->OperationSpaceComboBox->addItem(QString(" View	"));
	this->OperationSpaceComboBox->setMinimumWidth(50);
	this->OperationSpaceLabel->setMinimumWidth(50);
	this->ui->OperationsToolBar->addWidget(this->OperationSpaceLabel);
	this->ui->OperationsToolBar->addWidget(this->OperationSpaceComboBox);
	QObject::connect(this->OperationSpaceComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(ChangeGizmoWorkingSpace(QString)));	
}

void MapEditor::InitializePlugIns()
{

}

void MapEditor::ChangeGizmoWorkingSpace(QString Text)
{
	if (Text == QString(" Local   "))
	{
		for (int I = 0; I < SelectedItems.GetCount(); I++)
			SelectedItems[I]->SetGizmoSpace(ZED_GS_LOCAL);
	}
	else if (Text == QString(" World   "))
	{
		for (int I = 0; I < SelectedItems.GetCount(); I++)
			SelectedItems[I]->SetGizmoSpace(ZED_GS_WORLD);
	}
	else if (Text == QString(" View   "))
	{
		for (int I = 0; I < SelectedItems.GetCount(); I++)
			SelectedItems[I]->SetGizmoSpace(ZED_GS_VIEW);
	}
}

void MapEditor::ChangeGridVisibility(bool Visibility)
{
	Grid->SetVisible(Visibility);	
}
		
void MapEditor::UndoActionTriggered()
{
	UndoRedoManager.PerformUndo();
}
		
void MapEditor::RedoActionTriggered()
{
	UndoRedoManager.PerformRedo();
}

ZEDSelectionItem* MapEditor::CreateSelectionItem(ZEEntity* Entity)
{
	if(Entity == NULL)
		return NULL;

	for (int I = 0; I < SelectionItemPlugIns.GetCount(); I++)
	{
		if (QString(Entity->GetClassDescription()->GetName()) == SelectionItemPlugIns[I]->GetSupportedClassName())
			return SelectionItemPlugIns[I]->CreateSelectionItem(((ZEClass*)(Entity)), GizmoMode, Scene);
	}

	return SelectionItemPlugIns[3]->CreateSelectionItem(((ZEClass*)Entity), GizmoMode, Scene); //Fix harcoded index

	//return NULL;
}

void MapEditor::HideActionTriggered()
{
	for (int I = 0; I < SelectedItems.GetCount(); I++)
	{
		SelectedItems[I]->SetVisiblity(false);
	}
}

void MapEditor::UnHideActionTriggered()
{
	for (int I = 0; I < SelectedItems.GetCount(); I++)
	{
		SelectedItems[I]->SetVisiblity(true);
	}
}

void MapEditor::ReadOptions()
{

}

void MapEditor::WriteOptions()
{

}

void MapEditor::UpdateCamPos()
{
	ZEVector3 PlayerPosition = this->Player->GetPosition();

	ui->CamPosXLabel->setText(QString("X:") + QString().setNum(PlayerPosition.x));
	ui->CamPosYLabel->setText(QString("Y:") + QString().setNum(PlayerPosition.y));
	ui->CamPosZLabel->setText(QString("Z:") + QString().setNum(PlayerPosition.z));
}
