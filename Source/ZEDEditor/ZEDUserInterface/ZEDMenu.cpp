//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDMenu.cpp
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

#include "ZEDMenu.h"

#include "ZERegEx\ZERegEx.h"

#include <QMenu>
#include <QMenuBar>

void ZEDMenu::Update()
{
	if (MenuBar == NULL && Menu == NULL && ChildMenus.GetCount() != 0)
	{
		Menu = new QMenu();
		Action->setMenu(Menu);
	}

	for (ZESize I = 0; I < ChildMenus.GetCount(); I++)
	{
		if (MenuBar != NULL)
			MenuBar->removeAction(ChildMenus[I]->Action);
		else
			Menu->removeAction(ChildMenus[I]->Action);
	}

	QObjectList Children;
	if (MenuBar != NULL)
		Children = MenuBar->children();
	else
		Children = Menu->children();

	for (int I = 0; I < Children.count(); I++)
	{
		if (qobject_cast<QAction*>(Children[I]) != NULL)
			delete qobject_cast<QAction*>(Children[I]);
		else if (qobject_cast<QMenu*>(Children[I]) != NULL)
			delete qobject_cast<QMenu*>(Children[I]);
	}

	const ZEString CurrentSection = ChildMenus[0]->GetSection();
	for (ZESize I = 0; I < ChildMenus.GetCount(); I++)
	{
		if (CurrentSection != ChildMenus[I]->GetSection() &&  (I != (ZESSize)ChildMenus.GetCount() - 1))
		{
			if (MenuBar != NULL)
				MenuBar->addSeparator();
			else
				Menu->addSeparator();
		}

		if (MenuBar != NULL)
			MenuBar->addAction(ChildMenus[I]->Action);
		else
			Menu->addAction(ChildMenus[I]->Action);

	}
}

void ZEDMenu::Action_triggered()
{
	Callback.CheckAndCall(this);
}

ZEDMenu* ZEDMenu::GetParent()
{
	return Parent;
}

ZEDMainWindow* ZEDMenu::GetMainWindow()
{
	return MainWindow;
}

void ZEDMenu::SetName(const ZEString& Name)
{
	this->Name = Name;
	Action->setText(Name.ToCString());
}

ZEString ZEDMenu::GetName() const
{
	return Name;
}

void ZEDMenu::SetSection(const ZEString& Name)
{
	if (this->Section == Section)
		return;

	this->Section = Section;

	Parent->Update();
}

const ZEString& ZEDMenu::GetSection() const
{
	return Section;
}

void ZEDMenu::SetIconPath(const ZEString& Path)
{
	IconPath = Path;
	Action->setIcon(QIcon(Path.ToCString()));
}

const ZEString& ZEDMenu::GetIconPath() const
{
	return IconPath;
}

void ZEDMenu::SetEnabled(bool Enabled)
{
	Action->setEnabled(Enabled);
}

bool ZEDMenu::GetEnabled() const
{
	return Action->isEnabled();
}

void ZEDMenu::SetVisible(bool Visible)
{
	Action->setVisible(Visible);
}

bool ZEDMenu::GetVisible() const
{
	return Action->isVisible();
}

void ZEDMenu::SetCheckable(bool Checkable)
{
	this->Checkable = Checkable;
	Action->setCheckable(Checkable);
}

bool ZEDMenu::GetCheckable()
{
	return Checked;
}

void ZEDMenu::SetChecked(bool Checked)
{
	this->Checked = Checked;
	Action->setChecked(Checked);
}

bool ZEDMenu::GetChecked()
{
	return Checked;
}

void ZEDMenu::SetUserData(void* Data)
{
	this->UserData = Data;
}

void* ZEDMenu::GetUserData()
{
	return UserData;
}

void ZEDMenu::SetCallback(const ZEDMenuCallback& Callback)
{
	this->Callback = Callback;
}

const ZEDMenuCallback& ZEDMenu::GetCallback()
{
	return Callback;
}

const ZEArray<ZEDMenu*>& ZEDMenu::GetChildMenus() const
{
	return ChildMenus;
}

void ZEDMenu::AddChildMenu(ZEDMenu* Menu)
{
	zeCheckError(Menu == NULL, ZE_VOID, "SubMenu is NULL.");
	zeCheckError(Menu->GetParent() != NULL, ZE_VOID, "Item is already added to a Menu. Menu Name: \"%s\", Item Name: \"%s\".", 
		GetName().ToCString(), Menu->GetName().ToCString());

	ZESize I;
	bool Found = false;
	for (I = 0; I < ChildMenus.GetCount(); I++)
	{
		if (ChildMenus[I]->GetSection() == Menu->GetSection())
		{
			Found = true;
			continue;
		}

		if (Found && ChildMenus[I]->GetSection() != Menu->GetSection())
		{
			ChildMenus.Insert(I, Menu);
			break;
		}
	}
	
	ChildMenus.Insert(I, Menu);

	Update();
}

bool ZEDMenu::RemoveChildMenu(ZEDMenu* Menu)
{
	zeCheckError(Menu == NULL, false, "Section is NULL.");

	if (!ChildMenus.Exists(Menu))
		return false;

	ChildMenus.RemoveValue(Menu);
	Menu->Parent = NULL;

	if (ChildMenus.GetCount() == 0 && AutoGenerated)
	{
		if (Parent != NULL)
			Parent->RemoveChildMenu(this);
		delete this;
	}
	else
	{
		Update();
	}

	return true;
}

void ZEDMenu::AddMenu(const ZEString& Path, ZEDMenu* Menu)
{
	ZERegEx PathRegEx("(.*?)[/\\\\](.*)");
	ZERegEx SectionRegEx("(.*)::(.*)");

	ZEString Current;
	ZEString Remaining;
	ZERegExMatch PathMatch;
	if (PathRegEx.Match(Path, PathMatch))
	{
		Current = PathMatch.SubMatches[0].String;
		Remaining = PathMatch.SubMatches[1].String;
	}
	else
	{
		Current = Path;
	}

	ZEString Name;
	ZEString Section;
	ZERegExMatch SectionMatch;
	if (SectionRegEx.Match(Current, SectionMatch))
	{
		Name = SectionMatch.SubMatches[1].String;
		Section = SectionMatch.SubMatches[0].String;
	}
	else
	{
		Name = Current;
	}

	if (!Current.IsEmpty())
	{
		for(ZESize I = 0; I < ChildMenus.GetCount(); I++)
		{
			if (ChildMenus[I]->GetSection() == Menu->GetSection() && 
				ChildMenus[I]->GetName() == Name)
			{
				ChildMenus[I]->AddMenu(Remaining, Menu);
				return;
			}
		}

		ZEDMenu* NewMenu = new ZEDMenu();
		NewMenu->SetName(Name);
		NewMenu->SetSection(Section);
		NewMenu->AutoGenerated = true;
		AddChildMenu(NewMenu);

		NewMenu->AddMenu(Remaining, Menu);
	}
	else
	{
		AddChildMenu(Menu);
	}
}

bool ZEDMenu::RemoveMenu(ZEDMenu* Menu)
{
	zeCheckError(Menu == NULL, false, "Section is NULL.");

	if (RemoveChildMenu(Menu))
		return true;

	for (ZESize I = 0; I < ChildMenus.GetCount(); I++)
	{
		if (ChildMenus[I]->RemoveMenu(Menu))
			return true;
	}

	return false;
}

ZEDMenu::ZEDMenu()
{
	Parent = NULL;
	AutoGenerated = false;
	Checkable = false;
	Checked = false;
	Menu = NULL;
	MenuBar = NULL;
	UserData = NULL;

	Action = new QAction(this);
	connect(Action, SIGNAL(triggered()), this, SLOT(Action_triggered()));
}

ZEDMenu::~ZEDMenu()
{
	if (GetParent() != NULL)
		GetParent()->RemoveChildMenu(this);

	while(ChildMenus.GetCount() != 0)
		delete ChildMenus.GetFirstItem();

	delete Action;
}
