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

class ZEDMenuWrapper : public QMenu
{
	public:
		ZEString Section;
};

bool ZEDMainWindow::eventFilter(QObject* Object, QEvent* Event)
{
	if (Object != GetMainWindow())
		return false;

	if (Event->type() == QEvent::Close)
	{
		int Result = QMessageBox::question(static_cast<QWidget*>(Object), "ZEDEditor", "Are you sure that you want to quit ?", QMessageBox::Yes, QMessageBox::No);
		if (Result)
		{
			GetEditor()->Exit();
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}

QMenu* ZEDMainWindow::GetOrCreateMenu(QMenu* Parent, const ZEString& Target)
{
	ZERegEx RegExp("(.*)(::)+(.*)");
	ZERegExMatch Match;
	RegExp.Match(Target, Match, ZE_REF_NO_MATCH_STRING);

	ZEString& Section = Match.SubMatches[0].String;
	ZEString& Name = Match.SubMatches[2].String;
	
	if (Match.SubMatches[1].String != "::")
	{
		Name = Section;
		Section = ZEString::Empty;
	}

	QWidget* ParentWidget = Parent;
	if (ParentWidget == NULL)
		ParentWidget = MainWindow->menuBar();

	QList<QMenu*> ChildMenus = Parent->findChildren<QMenu*>();
	for (ZESize I = 0; I < ChildMenus.count(); I++)
	{
		ZEDMenuWrapper* ChildMenu = static_cast<ZEDMenuWrapper*>(ChildMenus[I]);
		if (ChildMenu->title() == Name && ChildMenu->Section == Section)
			return ChildMenu;
	}

	return NULL;
}

ZEDMainWindow::ZEDMainWindow()
{
	MainWindow = new QMainWindow();
	Form = new Ui_ZEDMainWindow();
	Form->setupUi(MainWindow);

	MainWindow->installEventFilter(this);

	MainWindow->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	MainWindow->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
}

ZEDMainWindow::~ZEDMainWindow()
{

}

QMainWindow* ZEDMainWindow::GetMainWindow()
{
	return MainWindow;
}

const ZEArray<ZEDMenu*>& ZEDMainWindow::GetMenus()
{
	return Menus;
}

const ZEArray<ZEDToolbar*>& ZEDMainWindow::GetToolbars()
{
	return Toolbars;
}

const ZEArray<ZEDWindow*>& ZEDMainWindow::GetWindows()
{
	return Windows;
}

void ZEDMainWindow::AddMenu(ZEDMenu* Menu)
{
	zeCheckError(Menu == NULL, ZE_VOID, "Menu is NULL.");
	zeCheckError(Menu->GetMainWindow() != NULL, ZE_VOID, "Menu is already added to a Main Window. Menu Path: \"%s\".", Menu->GetPath().ToCString());

	ZEPathTokenizer Tokenizer;
	Tokenizer.Tokenize(Menu->GetPath());

	QMenu* Parent = NULL;
	for (ZESize I = 0; I < Tokenizer.GetTokenCount() - 1; I++)
	{
		QMenu* Menu = GetOrCreateMenu(Parent, Tokenizer.GetToken(I));
		Parent = Menu;
	}

	QMenu* SectionStart = NULL;
	QMenu* SectionEnd = NULL;
	QList<QMenu*> List = findChildren<QMenu*>();
	for (ZESize I = 0; I < List.count(); I++)
	{
		if (static_cast<ZEDMenuWrapper*>(List[I])->Section == Menu->GetSection())
		{
			SectionStart = List[I];
			continue;
		}
		else if (SectionStart != NULL && static_cast<ZEDMenuWrapper*>(List[I])->Section != Menu->GetSection())
		{
			SectionEnd = List[I];
			break;
		}
	}

	if (SectionEnd == NULL && List.count() != 0)
		Parent->insertSeparator(SectionEnd->menuAction());

	if (Menu->GetAction() != NULL)
		Parent->insertAction(SectionEnd->menuAction(), Menu->GetAction());

	if (Menu->GetMenu() != NULL)
		Parent->insertMenu(SectionEnd->menuAction(), Menu->GetMenu());
}

void ZEDMainWindow::RemoveMenu(ZEDMenu* Menu)
{

}

void ZEDMainWindow::AddToolbar(ZEDToolbar* Toolbar)
{
	zeCheckError(Toolbar == NULL, ZE_VOID, "Toolbar is NULL.");
	zeCheckError(Toolbar->GetMainWindow() != NULL, ZE_VOID, "Toolbar already added to a Main Window. Toolbar Name: \"%s\".", Toolbar->GetName().ToCString());

	Toolbars.Add(Toolbar);
	GetEditor()->AddComponent(Toolbar);
	GetMainWindow()->addToolBar(Toolbar->GetToolbar());

	QAction* Action = new QAction(this);
	Action->setText(Toolbar->GetName().ToCString());
	Action->setCheckable(true);
	Action->setChecked(Toolbar->GetToolbar()->isVisible());
	QObject::connect(Toolbar->GetToolbar(), SIGNAL(visibilityChanged(bool)), Toolbar->Action, SLOT(setChecked(bool)));
	QObject::connect(Toolbar->Action, SIGNAL(toggled(bool)), Toolbar->GetToolbar(), SLOT(setVisible(bool)));

	Toolbar->Menu = new ZEDMenu();
	if (Toolbar->GetCategory().IsEmpty())
		Toolbar->Menu->SetPath(ZEFormat::Format("Windows/Toolbars/{0}", Toolbar->GetName()));
	else
		Toolbar->Menu->SetPath(ZEFormat::Format("Windows/Toolbars/{0}::{1}", Toolbar->GetCategory(), Toolbar->GetName()));
	Toolbar->Menu->SetAction(Action);
	//AddMenu(Toolbar->Menu);
}

void ZEDMainWindow::RemoveToolbar(ZEDToolbar* Toolbar)
{
	zeCheckError(Toolbar == NULL, ZE_VOID, "Toolbar is NULL.");
	zeCheckError(Toolbar->GetMainWindow() != this, ZE_VOID, "Toolbar does not belong to this Main Window. Toolbar Name: \"%s\".", Toolbar->GetName().ToCString());

	GetMainWindow()->removeToolBar(Toolbar->GetToolbar());
	RemoveMenu(Toolbar->Menu);

	GetEditor()->RemoveComponent(Toolbar);
	Toolbars.RemoveValue(Toolbar);
}

void ZEDMainWindow::AddWindow(ZEDWindow* Window)
{
	zeCheckError(Window == NULL, ZE_VOID, "Window is NULL.");
	zeCheckError(Window->GetMainWindow() == this, ZE_VOID, "Window is already added to Main Window. Window Name: \"%s\".", Window->GetName().ToCString());
	
	Windows.Add(Window);

	QAction* Action = new QAction(this);
	Action->setText(Window->GetName().ToCString());
	Action->setCheckable(true);
	Action->setChecked(Window->GetWidget()->isVisible());
	connect(Window->GetWidget(), SIGNAL(visibilityChanged(bool)), Window->Action, SLOT(setChecked(bool)));
	connect(Window->Action, SIGNAL(toggled(bool)), Window->GetWidget(), SLOT(setVisible(bool)));

	Window->Menu = new ZEDMenu();
	if (Window->GetCategory().IsEmpty())
		Window->Menu->SetPath(ZEFormat::Format("Windows/{0}", Window->GetName()));
	else
		Window->Menu->SetPath(ZEFormat::Format("Windows/{0}::{1}", Window->GetCategory(), Window->GetName()));
	Window->Menu->SetAction(Action);
	AddMenu(Window->Menu);
	
	if (IsInitialized())
		Window->InitializeSelf();
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
