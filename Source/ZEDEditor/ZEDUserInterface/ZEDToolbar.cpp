//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDToolbar.cpp
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

#include "ZEDToolbar.h"

#include "ZEDToolbarItem.h"
#include "ZEDCommand.h"
#include "ZEDUIUtils.h"
#include "ZEDToolbarManager.h"

#include "ZEError.h"
#include "ZEFile/ZEPathInfo.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEML/ZEMLReader.h"

#include <QAction>
#include <QToolBar>
#include "QMainWindow"

void ZEDToolbar::Toolbar_topLevelChanged(bool Changed)
{
	QMainWindow* MainWindow = static_cast<QMainWindow*>(Toolbar->parent());
	Qt::ToolBarArea Area = MainWindow->toolBarArea(Toolbar);
	switch (Area)
	{
		case Qt::LeftToolBarArea:
			Location = ZED_TDL_LEFT;
			break;

		case Qt::RightToolBarArea:
			Location = ZED_TDL_RIGHT;
			break;

		case Qt::BottomToolBarArea:
			Location = ZED_TDL_BOTTOM;
			break;

		default:
		case Qt::TopToolBarArea:
			Location = ZED_TDL_TOP;
			break;
	}

	Order = Toolbar->pos().x();
	Row = Toolbar->pos().y();

	zeLog("%s, L: %s", GetName().ToCString(), ZEDToolbarLocation_Declaration()->ToText(Location, "Unknown").ToCString());
}

void ZEDToolbar::Setup()
{
	if (DeferredSetup)
		return;

	CleanUp();
	for (ZESize I = 0; I < Items.GetCount(); I++)
		Items[I]->Setup();
}

void ZEDToolbar::CleanUp()
{
	for (ZESize I = 0; I < Items.GetCount(); I++)
		Items[I]->CleanUp();
}

ZEDToolbar::ZEDToolbar()
{
	Manager = NULL;
	Toolbar = new QToolBar();
	Location = ZED_TDL_TOP;
	Order = 0;
	Row = 0;
	Visible = true;
	DeferredSetup = false;

	connect(Toolbar, &QToolBar::topLevelChanged, this, &ZEDToolbar::Toolbar_topLevelChanged);
}

ZEDToolbar::~ZEDToolbar()
{
	if (GetManager() != NULL)
		GetManager()->RemoveToolbar(this);

	ClearItems();
	delete Toolbar;
}

ZEDToolbarManager* ZEDToolbar::GetManager()
{
	return Manager;
}

void ZEDToolbar::SetName(const ZEString&	Name)
{
	if (this->Name == Name)
		return;

	this->Name = Name;
}

const ZEString& ZEDToolbar::GetName() const
{
	return Name;
}

void ZEDToolbar::SetText(const ZEString& Text)
{
	if (this->Text == Text)
		return;

	this->Text = Text;
	
	Toolbar->setWindowTitle(Text.ToCString());
}

const ZEString& ZEDToolbar::GetText() const
{
	return Text;
}

void ZEDToolbar::SetIcon(const ZEString& Icon)
{
	if (ZEPathInfo::Compare(this->Icon, Icon))
		return;

	this->Icon = Icon;

	Toolbar->setWindowIcon(ZEDUIUtils::GetIcon(GetIcon()));
}

const ZEString& ZEDToolbar::GetIcon() const
{
	return Icon;
}

void ZEDToolbar::SetVisible(bool Visible)
{
	if (this->Visible == Visible)
		return;

	this->Visible = Visible;

	Toolbar->setVisible(Visible);
}

bool ZEDToolbar::GetVisible() const
{
	return Visible;
}

void ZEDToolbar::SetLocation(ZEDToolbarLocation Location)
{
	if (this->Location == Location)
		return;

	this->Location = Location;

	OnUpdated(this);
}

ZEDToolbarLocation ZEDToolbar::GetLocation() const
{
	return Location;
}

void ZEDToolbar::SetOrder(ZEUInt Order)
{
	if (this->Order == Order)
		return;

	this->Order = Order;

	OnUpdated(this);
}

ZEUInt ZEDToolbar::GetOrder() const
{
	return Order;
}

void ZEDToolbar::SetRow(ZEUInt Row)
{
	if (this->Row == Row)
		return;

	this->Row = Row;

	OnUpdated(this);
}

ZEUInt ZEDToolbar::GetRow() const
{
	return Row;
}

const ZEArray<ZEDToolbarItem*>& ZEDToolbar::GetItems() const
{
	return Items;
}

QToolBar* ZEDToolbar::GetNativeToolbar()
{
	return Toolbar;
}

void ZEDToolbar::AddItem(ZEDToolbarItem* Item)
{
	zeDebugCheck(Item == NULL, "Cannot insert toolbar item. Item is NULL");
	zeDebugCheck(Item->Toolbar != NULL, "Cannot insert toolbar item. Item is already added.");

	Item->Toolbar = this;
	Items.Add(Item);

	Setup();
}

void ZEDToolbar::InsertItem(ZESize Index, ZEDToolbarItem* Item)
{
	zeDebugCheck(Item == NULL, "Cannot insert toolbar item. Item is NULL");
	zeDebugCheck(Item->Toolbar != NULL, "Cannot insert toolbar item. Item is already added.");

	Item->Toolbar = this;

	if (Index >= Items.GetCount())
		Items.Add(Item);
	else
		Items.Insert(Index, Item);

	Setup();
}

void ZEDToolbar::RemoveItem(ZEDToolbarItem* Item)
{
	zeDebugCheck(Item == NULL, "Cannot delete toolbar item. Item is NULL");
	zeDebugCheck(Item->Toolbar != this, "Cannot delete toolbar item. Item is belong to this toolbar.");

	Item->Toolbar = NULL;
	Items.RemoveValue(Item);

	Setup();
}

void ZEDToolbar::ClearItems()
{
	while(Items.GetCount() != 0)
		Items.GetFirstItem()->Destroy();

	Setup();
}

bool ZEDToolbar::Load(ZEMLReaderNode* ToolbarNode)
{
	zeCheckError(ToolbarNode == NULL, false, "Cannot load Toolbar. ToolbarNode is NULL.");
	zeCheckError(!ToolbarNode->IsValid(), false, "Cannot load Toolbar. ToolbarNode is not valid.");

	SetName(ToolbarNode->ReadString("Name"));
	SetIcon(ToolbarNode->ReadString("Icon"));
	SetText(ToolbarNode->ReadString("Text"));
	SetVisible(ToolbarNode->ReadBoolean("Visible", true));
	SetLocation((ZEDToolbarLocation)ToolbarNode->ReadUInt8("Location", ZED_TDL_TOP));
	SetOrder(ToolbarNode->ReadUInt32("Order"));
	SetRow(ToolbarNode->ReadUInt32("Row"));

	DeferredSetup = true;
	ClearItems();
	ZEMLReaderNode ItemsNode = ToolbarNode->GetNode("Items");
	if (ItemsNode.IsValid())
	{
		ZESize ItemCount = ItemsNode.GetNodeCount("Item");
		for (ZESize I = 0; I < ItemCount; I++)
		{
			ZEDToolbarItem* Item = new ZEDToolbarItem();
			ZEMLReaderNode ItemNode = ItemsNode.GetNode("Item", I);
			if (!Item->Load(&ItemNode))
			{
				delete Item;
				continue;
			}

			AddItem(Item);
		}
	}
	DeferredSetup = false;

	Setup();

	return true;
}

bool ZEDToolbar::Save(ZEMLWriterNode* ToolbarsNode)
{
	zeCheckError(ToolbarsNode == NULL, false, "Cannot save Toolbar Item. ItemNode is NULL.");

	ZEMLWriterNode ToolbarNode;
	ToolbarsNode->OpenNode("Toolbar", ToolbarNode);

	ToolbarNode.WriteString("Name", GetName());
	ToolbarNode.WriteString("Icon", GetIcon());
	ToolbarNode.WriteString("Text", GetText());
	ToolbarNode.WriteBool("Visible", GetVisible());
	
	ToolbarNode.WriteUInt8("Location", Location);
	ToolbarNode.WriteUInt32("Order", Order);
	ToolbarNode.WriteUInt32("Row", Row);
	
	ZEMLWriterNode ItemsNode;
	ToolbarNode.OpenNode("Items", ItemsNode);
	
	for (ZESize I = 0; I < Items.GetCount(); I++)
	{
		if (!Items[I]->Save(&ItemsNode))
			return false;
	}

	ItemsNode.CloseNode();
	ToolbarNode.CloseNode();

	return true;
}

void ZEDToolbar::Destroy()
{
	delete this;
}

ZEDToolbar* ZEDToolbar::CreateInstance()
{
	return new ZEDToolbar;
}
