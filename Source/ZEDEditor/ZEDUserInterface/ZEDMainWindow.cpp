//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDMainWindow.cpp
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

#include "ZEDMainWindow.h"
#include "ui_ZEDMainWindow.h"

#include "ZEError.h"
#include "ZEDS/ZEFormat.h"
#include "ZERegEx/ZERegEx.h"
#include "ZEMeta/ZEEventDelegate.h"
#include "ZEFile/ZEPathTokenizer.h"
#include "ZEFile/ZEPathInfo.h"

#include "ZEDMenu.h"
#include "ZEDToolbar.h"
#include "ZEDWindow.h"
#include "ZEDToolbarManager.h"
#include "ZEDCommandManager.h"
#include "ZEDCore/ZEDEditor.h"
#include "ZEDCore/ZEDViewPort.h"
#include "ZEDCustomizeUIWindow.h"

#include <QDockWidget>
#include <QToolBar>
#include <QMessageBox>

ZEInt ToolbarSortOperator(ZEDToolbar*const& A, ZEDToolbar*const& B)
{
	if (A->GetLocation() < B->GetLocation())
	{
		return -1;
	}
	else if (A->GetLocation() > B->GetLocation())
	{
		return 1;
	}
	else
	{
		if (A->GetRow() < B->GetRow())
		{
			return -1;
		}
		else if (A->GetRow() > B->GetRow())
		{
			return 1;
		}
		else
		{
			if (A->GetOrder() < B->GetOrder())
				return -1;
			else if (A->GetOrder() > B->GetOrder())
				return 1;
			else
				return 0;
		}
	}
}

bool ZEDMainWindow::eventFilter(QObject* Object, QEvent* Event)
{
	if (Object != GetMainWindow())
		return false;

	if (Event->type() == QEvent::Close)
	{
		int Result = QMessageBox::question(static_cast<QWidget*>(Object), "ZEDEditor", "Are you sure that you want to quit ?", QMessageBox::Yes, QMessageBox::No);
		if (Result == QMessageBox::Yes)
		{
			Event->accept();
			GetEditor()->Exit();
			return false;
		}
		else
		{
			Event->ignore();
			return true;
		}
	}

	return false;
}

void ZEDMainWindow::UpdateMainMenu()
{
	ZEDMenu* MainMenu = MenuManager->GetMenu("MainMenu");
	if (MainMenu == NULL)
		return;

	MainMenu->OnUpdated += ZEEventDelegate<void (const ZEDMenu* Menu)>::Create<ZEDMainWindow, &ZEDMainWindow::MainMenu_OnUpdated>(this);
	MainMenu_OnUpdated(MainMenu);
}

void ZEDMainWindow::UpdateToolbars()
{
	QList<QToolBar *> CurrentToolbars = MainWindowNative->findChildren<QToolBar *>();
	for (int I = 0; I < CurrentToolbars.count(); I++)
		MainWindowNative->removeEventFilter(CurrentToolbars[I]);

	ZEArray<ZEDToolbar*> Toolbars = ToolbarManager->GetToolbars();
	Toolbars.Sort2<ToolbarSortOperator>();
	
	ZEDToolbar* PrevToolbar = NULL;
	for (ZESize I = 0; I < Toolbars.GetCount(); I++)
	{
		ZEDToolbar* CurrentToolbar = Toolbars[I];

		Qt::ToolBarArea Area;
		switch (CurrentToolbar->GetLocation())
		{
			case ZED_TDL_LEFT:
				Area = Qt::LeftToolBarArea;
				break;

			case ZED_TDL_RIGHT:
				Area = Qt::RightToolBarArea;
				break;

			case ZED_TDL_BOTTOM:
				Area = Qt::BottomToolBarArea;
				break;

			default:
			case ZED_TDL_TOP:
				Area = Qt::TopToolBarArea;
				break;
		}

		if (PrevToolbar != NULL && 
			PrevToolbar->GetLocation() == CurrentToolbar->GetLocation() &&
			PrevToolbar->GetRow() != CurrentToolbar->GetRow())
		{
			MainWindowNative->addToolBarBreak(Area);
		}

		MainWindowNative->addToolBar(Area, Toolbars[I]->GetNativeToolbar());
		
		PrevToolbar = CurrentToolbar;
	}
}

bool ZEDMainWindow::InitializeInternal()
{
	if (!ZEDComponent::InitializeInternal())
		return false;

	RegisterCommands();

	if (ZEPathInfo("#S:/Configurations/ZEDEditor/Menu.ZEDConfig").IsExists())
	{
		if (!MenuManager->Load("#S:/Configurations/ZEDEditor/Menu.ZEDConfig"))
			MenuManager->Load("#R:/ZEDEditor/Configurations/MenuDefault.ZEDConfig");
	}
	else
	{
		MenuManager->Load("#R:/ZEDEditor/Configurations/MenuDefault.ZEDConfig");
	}

	UpdateMainMenu();


	if (ZEPathInfo("#S:/Configurations/ZEDEditor/Toolbar.ZEDConfig").IsExists())
	{
		if (!ToolbarManager->Load("#S:/Configurations/ZEDEditor/Toolbar.ZEDConfig"))
			ToolbarManager->Load("#R:/ZEDEditor/Configurations/ToolbarDefault.ZEDConfig");
	}
	else
	{
		ToolbarManager->Load("#R:/ZEDEditor/Configurations/ToolbarDefault.ZEDConfig");
	}

	UpdateToolbars();

	return true;
}

ZEDMainWindow::ZEDMainWindow()
{
	MenuManager = ZEDMenuManager::CreateInstance();
	ToolbarManager = ZEDToolbarManager::CreateInstance();

	MainWindowNative = new QMainWindow();
	Form = new Ui_ZEDMainWindow();
	Form->setupUi(MainWindowNative);

	MainWindowNative->installEventFilter(this);

	MainWindowNative->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	MainWindowNative->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
}

ZEDMainWindow::~ZEDMainWindow()
{
	MenuManager->Destroy();
	ToolbarManager->Destroy();
}

void ZEDMainWindow::RegisterCommands()
{
	CustomizeUICommand.SetCategory("User Interface");
	CustomizeUICommand.SetName("ZEDEditor::CustomizeUICommand");
	CustomizeUICommand.SetText("Customize UI");
	CustomizeUICommand.OnAction += ZEDCommandDelegate::Create<ZEDMainWindow, &ZEDMainWindow::CustomizeUICommand_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&CustomizeUICommand);
}


void ZEDMainWindow::MainMenu_OnUpdated(const ZEDMenu* Menu)
{
	QMenu* NativeMenu = const_cast<ZEDMenu*>(Menu)->GetNativeMenu();
	QList<QAction*> Actions = NativeMenu->actions();

	MainWindowNative->menuBar()->clear();
	for (ZESize I = 0; I < Actions.count(); I++)
	{
		if (Actions[I]->isSeparator())
			continue;

		MainWindowNative->menuBar()->addAction(Actions[I]);
	}
}

void ZEDMainWindow::CustomizeUICommand_OnAction(const ZEDCommand* Command)
{
	Configure();
}

QMainWindow* ZEDMainWindow::GetMainWindow()
{
	return MainWindowNative;
}

ZEDMenuManager* ZEDMainWindow::GetMenuManager()
{
	return MenuManager;
}

ZEDToolbarManager* ZEDMainWindow::GetToolbarManager()
{
	return ToolbarManager;
}

const ZEArray<ZEDWindow*>& ZEDMainWindow::GetWindows()
{
	return Windows;
}

void ZEDMainWindow::AddWindow(ZEDWindow* Window, ZEDWindowDefaults Default)
{
	zeCheckError(Window == NULL, ZE_VOID, "Window is NULL.");
	zeCheckError(Window->GetMainWindow() == this, ZE_VOID, "Window is already added to Main Window. Window Name: \"%s\".", Window->GetName().ToCString());
	
	Windows.Add(Window);
	GetEditor()->AddComponent(Window);

	switch (Default & 0xF0)
	{
		case ZED_WD_DOCK_LEFT:
			MainWindowNative->addDockWidget(Qt::LeftDockWidgetArea, Window->GetDockWidget());
			break;

		case ZED_WD_DOCK_RIGHT:
			MainWindowNative->addDockWidget(Qt::RightDockWidgetArea, Window->GetDockWidget());
			break;

		case ZED_WD_DOCK_TOP:
			MainWindowNative->addDockWidget(Qt::TopDockWidgetArea, Window->GetDockWidget());
			break;

		case ZED_WD_DOCK_BOTTOM:
			MainWindowNative->addDockWidget(Qt::BottomDockWidgetArea, Window->GetDockWidget());
			break;

		case ZED_WD_STACK_LEFT:
			MainWindowNative->addDockWidget(Qt::LeftDockWidgetArea, Window->GetDockWidget());
			break;

		case ZED_WD_STACK_RIGHT:
			MainWindowNative->addDockWidget(Qt::RightDockWidgetArea, Window->GetDockWidget());
			break;

		case ZED_WD_STACK_TOP:
			MainWindowNative->addDockWidget(Qt::TopDockWidgetArea, Window->GetDockWidget());
			break;

		case ZED_WD_STACK_BOTTOM:
			MainWindowNative->addDockWidget(Qt::BottomDockWidgetArea, Window->GetDockWidget());
			break;

		default:
			MainWindowNative->addDockWidget(Qt::NoDockWidgetArea, Window->GetDockWidget());
			break;
	}

	Window->SetVisible(Default & ZED_WD_VISIBLE);
}

void ZEDMainWindow::RemoveWindow(ZEDWindow* Window)
{
	zeCheckError(Window == NULL, ZE_VOID, "Window is NULL.");
	zeCheckError(Window->GetMainWindow() != this, ZE_VOID, "Window does not belong to this Main Window. Window Name: \"%s\".", Window->GetName().ToCString());

	Window->Deinitialize();
	Windows.RemoveValue(Window);
}

void ZEDMainWindow::SetViewport(ZEDViewport* Viewport)
{
	this->Viewport = Viewport;
	MainWindowNative->setCentralWidget(Viewport);
}

ZEDViewport* ZEDMainWindow::GetViewport()
{
	return Viewport;
}

void ZEDMainWindow::Configure()
{
	ZEDCustomizeUIWindow* Customize = new ZEDCustomizeUIWindow(this);
	Customize->show();
}

ZEDMainWindow* ZEDMainWindow::CreateInstance()
{
	return new ZEDMainWindow();
}
