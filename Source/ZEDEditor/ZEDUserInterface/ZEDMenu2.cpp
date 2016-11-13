//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDMenu2.cpp
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

#include "ZEDMenu2.h"

#include "ZEDMenuItem.h"
#include "ZEDCommand.h"
#include "ZEDUIUtils.h"

#include "ZEError.h"
#include "ZEFile/ZEPathInfo.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEML/ZEMLReader.h"

#include <QAction>
#include <QMenu>

ZEDMenu2::ZEDMenu2()
{
	Manager = NULL;
	Menu = new QMenu();
	SystemMenu = false;
}

ZEDMenu2::~ZEDMenu2()
{
	ClearItems();
	delete Menu;
}

ZEDMenuManager* ZEDMenu2::GetManager()
{
	return Manager;
}

void ZEDMenu2::SetName(const ZEString&	Name)
{
	if (this->Name == Name)
		return;

	this->Name = Name;

	OnUpdated(this);
}

const ZEString& ZEDMenu2::GetName() const
{
	return Name;
}

void ZEDMenu2::SetText(const ZEString& Text)
{
	if (this->Text == Text)
		return;

	this->Text = Text;
	
	Menu->setTitle(Text.ToCString());

	OnUpdated(this);
}

const ZEString& ZEDMenu2::GetText() const
{
	return Text;
}

void ZEDMenu2::SetIcon(const ZEString& Icon)
{
	if (ZEPathInfo::Compare(this->Icon, Icon))
		return;

	this->Icon = Icon;

	Menu->setIcon(ZEDUIUtils::GetIcon(GetIcon()));

	OnUpdated(this);
}

const ZEString& ZEDMenu2::GetIcon() const
{
	return Icon;
}

void ZEDMenu2::SetSystemMenu(bool SystemMenu)
{
	this->SystemMenu = SystemMenu;
}

bool ZEDMenu2::GetSystemMenu() const
{
	return SystemMenu;
}

const ZEArray<ZEDMenuItem*>& ZEDMenu2::GetItems() const
{
	return Items;
}

QMenu* ZEDMenu2::GetNativeMenu()
{
	return Menu;
}

void ZEDMenu2::AddItem(ZEDMenuItem* Item)
{
	zeDebugCheck(Item == NULL, "Cannot insert menu item. Item is NULL");
	zeDebugCheck(Item == NULL, "Cannot insert menu item. Item is already added.");

	Items.Add(Item);

	Item->Menu = this;
	Item->Action = new ZEDMenuAction(Item);
	Menu->addAction(Item->Action);
	Item->Update();
}

void ZEDMenu2::InsertItem(ZESize Index, ZEDMenuItem* Item)
{
	zeDebugCheck(Item == NULL, "Cannot insert menu item. Item is NULL");
	zeDebugCheck(Item == NULL, "Cannot insert menu item. Item is already added.");

	if (Items.GetCount() >= Index)
		Items.Add(Item);
	else
		Items.Insert(Index, Item);

	Item->Menu = this;
	Item->Action = new ZEDMenuAction(Item);
	Menu->addAction(Item->Action);
	Item->Update();
}

void ZEDMenu2::RemoveItem(ZEDMenuItem* Item)
{
	zeDebugCheck(Item == NULL, "Cannot delete menu item. Item is NULL");
	zeDebugCheck(Item->Menu != this, "Cannot delete menu item. Item is belong to this menu.");

	delete Item->Action;
	Items.RemoveValue(Item);
}

void ZEDMenu2::ClearItems()
{
	for (ZESize I = 0; I < Items.GetCount(); I++)
		delete Items[I];

	Items.Clear();
}

bool ZEDMenu2::Load(ZEMLReaderNode* MenuNode)
{
	zeCheckError(MenuNode == NULL, false, "Cannot load Menu. MenuNode is NULL.");
	zeCheckError(MenuNode->IsValid(), false, "Cannot load Menu. MenuNode is not valid.");

	SetName(MenuNode->ReadString("Name"));
	SetIcon(MenuNode->ReadString("Icon"));

	ClearItems();
	ZEMLReaderNode ItemsNode = MenuNode->GetNode("Items");
	if (ItemsNode.IsValid())
	{
		ZESize ItemCount = ItemsNode.GetNodeCount("Items");
		Items.SetCount(ItemCount);
		for (ZESize I = 0; I < ItemCount; I++)
		{
			ZEMLReaderNode ItemNode = ItemsNode.GetNode("Item");
			if (!Items[I]->Load(&ItemNode))
				return false;
		}
	}

	return true;
}

bool ZEDMenu2::Save(ZEMLWriterNode* MenusNode)
{
	zeCheckError(MenusNode == NULL, false, "Cannot save Menu Item. ItemNode is NULL.");

	if (GetSystemMenu())
		return true;

	ZEMLWriterNode MenuNode;
	MenuNode.OpenNode("Menu", MenuNode);

	MenuNode.WriteString("Name", GetName());
	MenuNode.WriteString("Icon", GetIcon());
	
	ZEMLWriterNode ItemsNode;
	MenuNode.OpenNode("Items", ItemsNode);
	
	for (ZESize I = 0; I < Items.GetCount(); I++)
	{
		if (!Items[I]->Save(&ItemsNode))
			return false;
	}

	ItemsNode.CloseNode();
	MenuNode.CloseNode();

	return true;
}

void ZEDMenu2::Update()
{
	for (ZESize I = 0; I < Items.GetCount(); I++)
		Items[I]->Update();
}

void ZEDMenu2::Destroy()
{
	delete this;
}

ZEDMenu2* ZEDMenu2::CreateInstance()
{
	return new ZEDMenu2;
}
