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
#include "ZEFile/ZEPathTokenizer.h"
#include "ZERegEx/ZERegEx.h"

#include "ZEDWindow.h"
#include "ZEDToolbar.h"
#include "ZEDMenu.h"
#include "ZEDCore/ZEDEditor.h"
#include "ZEDCore/ZEDViewPort.h"
#include "ZEDUserInterfaceComponent.h"

#include <QDockWidget>
#include <QToolBar>
#include <QMessageBox>
#include "ZEDCommandManager.h"
#include "ZEDMenu2.h"
#include "ZEDMenuItem.h"
#include "ZEMeta/ZEEventDelegate.h"

class ZEDMenuWrapper : public QMenu
{
	public:
		ZEString Section;
};

void ZEDMainWindow::QuitCommand_OnAction(const ZEDCommand* Command)
{
	int Result = QMessageBox::question(MainWindow, "ZEDEditor", "Are you sure that you want to quit ?", QMessageBox::Yes, QMessageBox::No);
	if (Result == QMessageBox::Yes)
		GetEditor()->Exit();
}

void ZEDMainWindow::B_OnAction(const ZEDCommand* Command)
{
	QuitCommand.SetEnabled(Command->GetValue().GetBoolean());
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

void ZEDMainWindow::WindowMenuCallback(ZEDMenu* Menu)
{
	ZEDWindow* Window = static_cast<ZEDWindow*>(Menu->GetUserData());
	if (Window == NULL)
		return;

	Window->SetVisible(Menu->GetChecked());
}

void ZEDMainWindow::ToolbarMenuCallback(ZEDMenu* Menu)
{
	ZEDToolbar* Toolbar = static_cast<ZEDToolbar*>(Menu->GetUserData());
	if (Toolbar == NULL)
		return;

	Toolbar->SetVisible(Menu->GetChecked());
}

ZEDMainWindow::ZEDMainWindow()
{
	QuitCommand.SetName("Quit");
	QuitCommand.SetCategory("Main Window");
	QuitCommand.SetText("Quit");
	QuitCommand.SetIcon("#R:/ZEEngine/ZEGUI/Textures/Close.png");
	QuitCommand.SetTooltip("Exit from the editor");
	QuitCommand.SetEnabled(false);
	QuitCommand.OnAction += ZEEventDelegate<void (const ZEDCommand*)>::Create<ZEDMainWindow, &ZEDMainWindow::QuitCommand_OnAction>(this);

	ZEDCommandManager::GetInstance()->RegisterCommand(&QuitCommand);

	B.SetName("CommandB");
	B.SetCategory("Test1");
	B.SetText("B");
	B.SetTooltip("Tool tip o a");
	B.SetType(ZED_CT_CHECK);
	B.OnAction += ZEEventDelegate<void (const ZEDCommand*)>::Create<ZEDMainWindow, &ZEDMainWindow::B_OnAction>(this);
	ZEDCommandManager::GetInstance()->RegisterCommand(&B);

	MenuManager = ZEDMenuManager::CreateInstance();

	ZEDMenu2* MainMenu = ZEDMenu2::CreateInstance();
	MainMenu->SetName("MainMenu");
	MainMenu->SetText("Menu1");
	MenuManager->AddMenu(MainMenu);

	ZEDMenu2* SubMenu2 = ZEDMenu2::CreateInstance();
	SubMenu2->SetName("MainMenu2");
	SubMenu2->SetText("Menu12");
	MenuManager->AddMenu(SubMenu2);

	ZEDMenuItem* Item = new ZEDMenuItem();
	Item->SetType(ZED_MIT_COMMAND);
	Item->SetTargetName("CommandB");
	SubMenu2->AddItem(Item);

	ZEDMenuItem* Item1 = new ZEDMenuItem();
	Item1->SetType(ZED_MIT_COMMAND);
	Item1->SetTargetName("Quit");
	MainMenu->AddItem(Item1);

	ZEDMenuItem* Item2 = new ZEDMenuItem();;
	Item2->SetType(ZED_MIT_SEPERATOR);
	MainMenu->AddItem(Item2);

	ZEDMenuItem* Item3 = new ZEDMenuItem();;
	Item3->SetType(ZED_MIT_MENU_POINTER);
	Item3->SetTargetName("MainMenu2");
	MainMenu->AddItem(Item3);


	MenuManager->Update();

	MainWindow = new QMainWindow();
	Form = new Ui_ZEDMainWindow();
	Form->setupUi(MainWindow);

	MainWindow->menuBar()->addMenu(MenuManager->GetMenu("MainMenu")->GetNativeMenu());

	MainWindow->installEventFilter(this);

	MainWindow->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	MainWindow->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	RootMenu.MenuBar = MainWindow->menuBar();
}

ZEDMainWindow::~ZEDMainWindow()
{
	MenuManager->Destroy();
}

QMainWindow* ZEDMainWindow::GetMainWindow()
{
	return MainWindow;
}

ZEDMenu* ZEDMainWindow::GetRootMenu()
{
	return &RootMenu;
}

void ZEDMainWindow::AddMenu(const ZEString& Path, ZEDMenu* Menu)
{
	return;
	zeCheckError(Menu == NULL, ZE_VOID, "Menu is NULL.");
	RootMenu.AddMenu(Path, Menu);
}

void ZEDMainWindow::RemoveMenu(ZEDMenu* Menu)
{
	zeCheckError(Menu == NULL, ZE_VOID, "Menu is NULL.");
	RootMenu.RemoveMenu(Menu);
}


const ZEArray<ZEDToolbar*>& ZEDMainWindow::GetToolbars()
{
	return Toolbars;
}

void ZEDMainWindow::AddToolbar(ZEDToolbar* Toolbar, ZEDToolbarDefaults Default)
{
	zeCheckError(Toolbar == NULL, ZE_VOID, "Toolbar is NULL.");
	zeCheckError(Toolbar->GetMainWindow() != NULL, ZE_VOID, "Toolbar already added to a Main Window. Toolbar Name: \"%s\".", Toolbar->GetName().ToCString());

	Toolbars.Add(Toolbar);
	GetEditor()->AddComponent(Toolbar);

	switch (Default & 0xF0)
	{
		case ZED_WD_DOCK_LEFT:
			MainWindow->addToolBar(Qt::LeftToolBarArea, Toolbar->GetToolbar());
			break;

		case ZED_WD_DOCK_RIGHT:
			MainWindow->addToolBar(Qt::RightToolBarArea, Toolbar->GetToolbar());
			break;

		case ZED_WD_DOCK_TOP:
			MainWindow->addToolBar(Qt::TopToolBarArea, Toolbar->GetToolbar());
			break;

		case ZED_WD_DOCK_BOTTOM:
			MainWindow->addToolBar(Qt::BottomToolBarArea, Toolbar->GetToolbar());
			break;

		default:
			MainWindow->addToolBar(Qt::NoToolBarArea, Toolbar->GetToolbar());
			break;
	}
	
	Toolbar->SetVisible(Default & ZED_WD_VISIBLE);

	Toolbar->Menu = new ZEDMenu();
	Toolbar->Menu->SetName(Toolbar->GetName());
	Toolbar->Menu->SetSection(Toolbar->GetCategory());
	Toolbar->Menu->SetCheckable(true);
	Toolbar->Menu->SetChecked(Default & ZED_WD_VISIBLE);
	Toolbar->Menu->SetUserData(Toolbar);
	Toolbar->Menu->SetCallback(ZEDelegateMethod(ZEDMenuCallback, ZEDMainWindow, ToolbarMenuCallback, this));
	AddMenu("User Interface/Toolbars", Toolbar->Menu);
}

void ZEDMainWindow::RemoveToolbar(ZEDToolbar* Toolbar)
{
	zeCheckError(Toolbar == NULL, ZE_VOID, "Toolbar is NULL.");
	zeCheckError(Toolbar->GetMainWindow() != this, ZE_VOID, "Toolbar does not belong to this Main Window. Toolbar Name: \"%s\".", Toolbar->GetName().ToCString());

	GetMainWindow()->removeToolBar(Toolbar->GetToolbar());
	RemoveMenu(Toolbar->Menu);

	GetEditor()->RemoveComponent(Toolbar);
	Toolbars.RemoveValue(Toolbar);

	RemoveMenu(Toolbar->Menu);
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
			MainWindow->addDockWidget(Qt::LeftDockWidgetArea, Window->GetDockWidget());
			break;

		case ZED_WD_DOCK_RIGHT:
			MainWindow->addDockWidget(Qt::RightDockWidgetArea, Window->GetDockWidget());
			break;

		case ZED_WD_DOCK_TOP:
			MainWindow->addDockWidget(Qt::TopDockWidgetArea, Window->GetDockWidget());
			break;

		case ZED_WD_DOCK_BOTTOM:
			MainWindow->addDockWidget(Qt::BottomDockWidgetArea, Window->GetDockWidget());
			break;

		case ZED_WD_STACK_LEFT:
			MainWindow->addDockWidget(Qt::LeftDockWidgetArea, Window->GetDockWidget());
			break;

		case ZED_WD_STACK_RIGHT:
			MainWindow->addDockWidget(Qt::RightDockWidgetArea, Window->GetDockWidget());
			break;

		case ZED_WD_STACK_TOP:
			MainWindow->addDockWidget(Qt::TopDockWidgetArea, Window->GetDockWidget());
			break;

		case ZED_WD_STACK_BOTTOM:
			MainWindow->addDockWidget(Qt::BottomDockWidgetArea, Window->GetDockWidget());
			break;

		default:
			MainWindow->addDockWidget(Qt::NoDockWidgetArea, Window->GetDockWidget());
			break;
	}

	Window->SetVisible(Default & ZED_WD_VISIBLE);

	Window->Menu = new ZEDMenu();
	Window->Menu->SetName(Window->GetName());
	Window->Menu->SetSection(Window->GetCategory());
	Window->Menu->SetCheckable(true);
	Window->Menu->SetChecked(Default & ZED_WD_VISIBLE);
	Window->Menu->SetUserData(Window);
	Window->Menu->SetCallback(ZEDelegateMethod(ZEDMenuCallback, ZEDMainWindow, WindowMenuCallback, this));

	if (Window->GetCategory().IsEmpty())
		AddMenu("User Interface/Windows", Window->Menu);
	else
		AddMenu(ZEFormat::Format("User Interface/Windows/{0}", Window->GetCategory()), Window->Menu);

}

void ZEDMainWindow::RemoveWindow(ZEDWindow* Window)
{
	zeCheckError(Window == NULL, ZE_VOID, "Window is NULL.");
	zeCheckError(Window->GetMainWindow() != this, ZE_VOID, "Window does not belong to this Main Window. Window Name: \"%s\".", Window->GetName().ToCString());

	Window->Deinitialize();
	Windows.RemoveValue(Window);

	RemoveMenu(Window->Menu);
}

void ZEDMainWindow::SetViewport(ZEDViewport* Viewport)
{
	this->Viewport = Viewport;
	MainWindow->setCentralWidget(Viewport);
}

ZEDViewport* ZEDMainWindow::GetViewport()
{
	return Viewport;
}

ZEDMainWindow* ZEDMainWindow::CreateInstance()
{
	return new ZEDMainWindow();
}
