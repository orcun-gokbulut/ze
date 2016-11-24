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

#include "ZEDMenuManager.h"
#include "ZEDMenuItem.h"
#include "ZEDCommand.h"
#include "ZEDUIUtils.h"

#include "ZEError.h"
#include "ZEFile/ZEPathInfo.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEML/ZEMLReader.h"

#include <QAction>
#include <QMenu>


void ZEDMenu::Setup()
{
	if (DeferredSetup)
		return;

	CleanUp();
	for (ZESize I = 0; I < Items.GetCount(); I++)
		Items[I]->Setup();

	OnUpdated(this);
}

void ZEDMenu::CleanUp()
{
	for (ZESize I = 0; I < Items.GetCount(); I++)
		Items[I]->CleanUp();
}

ZEDMenu::ZEDMenu()
{
	Manager = NULL;
	Menu = new QMenu();
	Protected = false;
	DeferredSetup = false;
}

ZEDMenu::~ZEDMenu()
{
	if (GetManager() != NULL)
		GetManager()->RemoveMenu(this);

	ClearItems();
	delete Menu;
}

ZEDMenuManager* ZEDMenu::GetManager()
{
	return Manager;
}

void ZEDMenu::SetName(const ZEString& Name)
{
	if (this->Name == Name)
		return;

	this->Name = Name;

	OnUpdated(this);
}

const ZEString& ZEDMenu::GetName() const
{
	return Name;
}

void ZEDMenu::SetText(const ZEString& Text)
{
	if (this->Text == Text)
		return;

	this->Text = Text;
	
	Menu->setTitle(Text.ToCString());
}

const ZEString& ZEDMenu::GetText() const
{
	return Text;
}

void ZEDMenu::SetIcon(const ZEString& Icon)
{
	if (ZEPathInfo::Compare(this->Icon, Icon))
		return;

	this->Icon = Icon;

	Menu->setIcon(ZEDUIUtils::GetIcon(GetIcon()));
}

const ZEString& ZEDMenu::GetIcon() const
{
	return Icon;
}

void ZEDMenu::SetProtected(bool Protected)
{
	this->Protected = Protected;
}

bool ZEDMenu::GetProtected() const
{
	return Protected;
}

const ZEArray<ZEDMenuItem*>& ZEDMenu::GetItems() const
{
	return Items;
}

QMenu* ZEDMenu::GetNativeMenu()
{
	return Menu;
}

void ZEDMenu::AddItem(ZEDMenuItem* Item)
{
	zeDebugCheck(Item == NULL, "Cannot insert menu item. Item is NULL");
	zeDebugCheck(Item->Menu != NULL, "Cannot insert menu item. Item is already added.");

	Item->Menu = this;
	Items.Add(Item);

	Setup();
}

void ZEDMenu::InsertItem(ZESize Index, ZEDMenuItem* Item)
{
	zeDebugCheck(Item == NULL, "Cannot insert menu item. Item is NULL");
	zeDebugCheck(Item->Menu != NULL, "Cannot insert menu item. Item is already added.");


	Item->Menu = this;

	if (Index >= Items.GetCount())
		Items.Add(Item);
	else
		Items.Insert(Index, Item);

	Setup();
}

void ZEDMenu::RemoveItem(ZEDMenuItem* Item)
{
	zeDebugCheck(Item == NULL, "Cannot delete menu item. Item is NULL");
	zeDebugCheck(Item->Menu != this, "Cannot delete menu item. Item is belong to this menu.");

	Item->CleanUp();
	Item->Menu = NULL;
	Items.RemoveValue(Item);
}

void ZEDMenu::ClearItems()
{
	while(Items.GetCount() != 0)
		Items.GetFirstItem()->Destroy();
	
	CleanUp();
}

bool ZEDMenu::Load(ZEMLReaderNode* MenuNode)
{
	zeCheckError(MenuNode == NULL, false, "Cannot load Menu. MenuNode is NULL.");
	zeCheckError(!MenuNode->IsValid(), false, "Cannot load Menu. MenuNode is not valid.");

	SetName(MenuNode->ReadString("Name"));
	SetIcon(MenuNode->ReadString("Icon"));
	SetText(MenuNode->ReadString("Text"));
	SetProtected(MenuNode->ReadBoolean("Protected", false));

	DeferredSetup = true;
	ClearItems();
	ZEMLReaderNode ItemsNode = MenuNode->GetNode("Items");
	if (ItemsNode.IsValid())
	{
		ZESize ItemCount = ItemsNode.GetNodeCount("Item");
		for (ZESize I = 0; I < ItemCount; I++)
		{
			ZEDMenuItem* Item = new ZEDMenuItem();
			ZEMLReaderNode ItemNode = ItemsNode.GetNode("Item", I);
			if (!Item->Load(&ItemNode))
			{
				delete Item;
				return false;
			}
			AddItem(Item);
		}
	}
	DeferredSetup = false;

	Setup();

	return true;
}

bool ZEDMenu::Save(ZEMLWriterNode* MenusNode)
{
	zeCheckError(MenusNode == NULL, false, "Cannot save Menu Item. ItemNode is NULL.");
	
	ZEMLWriterNode MenuNode;
	MenusNode->OpenNode("Menu", MenuNode);

	MenuNode.WriteString("Name", GetName());
	MenuNode.WriteString("Icon", GetIcon());
	MenuNode.WriteString("Text", GetText());
	MenuNode.WriteBool("Protected", GetProtected());
	
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

void ZEDMenu::Destroy()
{
	delete this;
}

ZEDMenu* ZEDMenu::CreateInstance()
{
	return new ZEDMenu;
}
