//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSceneEditor.h
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

#pragma once
#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include <QtGui/QMainWindow>
#include <QtCore/QPoint.h>
#include <QtCore/QTimer.h>
#include <QtCore/QString.h>
#include "ZEDCommonControls\ZEDViewPort.h"
#include "ZEGame\ZEEntity.h"
#include "ZEDEntitySelector.h"
#include "ZEDCommonControls/ZEDPropertyWindowManager/ZEDPropertyWindowManager.h"
#include "ZEDCommonControls\ZEDLoadingScreen.h"
#include "ZEDCommonControls\ZEDConsoleInterface.h"
#include "ZEDCore\ZEDPlugIn.h"
#include "..\ZEDUndoRedo\ZEDUndoRedo.h"
#include "ZEDSelectionItem.h"
#include "ZEDSelectionItemPlugin.h"
#include "ZEDSceneList.h"
#include "ZEDCommonEntities/ZEDGrid.h"
#include <ZEModel/ZEModel.h>

using namespace Qt;

namespace Ui
{
	class MapEditorClass;
}

class MapEditor : public QMainWindow
{
	Q_OBJECT
	friend class ZEDViewPort;

	private:

		bool						SaveFlag;
		bool						SelectionDirtyFlag;

		ZEScene*					Scene;
		ZEGame*						Game;
		ZEPlayer*					Player;
		ZEDGizmoMode				GizmoMode;
		ZEDSceneList*				SceneList;
		ZEDUndoRedoManagerOld			UndoRedoManager;
		ZEDLoadingScreen*			SplashScreen;
		ZEDGrid*					Grid;

		ZEDSelectionItem*			CreateSelectionItem(ZEEntity* Entity);
		void						MakeConnections();
		void						SelectionChangedEvent();
		void						ReadOptions();
		void						WriteOptions();

		void						StartEngine();
		void						DisplaySelectedEntityProperties();
		void						UpdatePropertyWidgetValues();
		void						GenerateAssertList();
		void						InitializeAdditionalToolBarItems();
		void						InitializePlugIns();

		//Test
		//NewZEDAssertBrowser*		AssertBrowser;
		//test



	private slots:

		void						LoadMapActionTriggered();
		void						LoadSceneActionTriggered();
		void						NewMapActionTriggered();
		void						MoveActionTriggered();
		void						RotateActionTriggered();
		void						ScaleActionTriggered();
		void						SelectActionTriggered();
		void						StepSizeChanged();
		void						DeleteActionTriggered();
		void						CopyActionTriggered();
		void						GoToEntityActionTriggered();
		void						HideActionTriggered();
		void						UnHideActionTriggered();
		void						CloseMapActionTriggered();
		void						SaveSceneActionTriggered();
		void						SaveSceneAsActionTriggered();
		void						ChangeGizmoWorkingSpace(QString Text);
		void						SceneListOpenActionTriggered(bool Checked);
		void						ChangeGridVisibility(bool Visibility);

		void						EngineMainLoop();
		void						EventsLoop();
		void						SelectionsLoop();
		void						UpdateCamPos();

		//Vegetation

		void						GenerateRandomTree();
		void						GenerateRandomBush();
		void						GenerateRandomFlowers();

	public slots:

		void						ConsoleInput();
		void						ShowEntitySelector();
		void						UpdateSelectedEntitiesByRayCast();
		void						UpdateSelectedEntitiesBySceneList();
		void						NewEntityToScene();
		void						UndoActionTriggered();
		void						RedoActionTriggered();

		void						BackupSave();

	public:

		ZEArray<ZEModelResource*>			Trees;
		ZEArray<ZEModelResource*>			Bushes;
		ZEArray<ZEModelResource*>			Flowers;

		QString								CurrentFileName;
		QString								WorkingDirectory;
		QTimer*								MainLoopTimer;
		QTimer*								BackupSaveTimer;
		ZEDEntitySelector*					EntitySelector;
		ZEDViewPort*						ViewPort;

		ZEDPropertyWindowManager*			PropertyWindowManager;
		ZEArray<QWidget*>					CustomPropertyWidgets;

		ZEDConsole*							Console;
		QComboBox*							OperationSpaceComboBox;
		QLabel*								OperationSpaceLabel;

		ZEArray<ZEDSelectionItemPlugin*>	SelectionItemPlugIns;
//		ZEArray<ZEDFilePlugIn*>				FileTypePlugIns;

		ZEArray<ZEDSelectionItem*>			SelectedItems;
//		ZEArray<ZEDPlugin*>					PlugIns;
		Ui::MapEditorClass*					ui;

		void								LoadTrees();
		void								LoadBushes();
		void								LoadFlowers();

											MapEditor(QWidget *parent = 0, WFlags flags = 0);
											~MapEditor();
};

#endif // MAPEDITOR_H
