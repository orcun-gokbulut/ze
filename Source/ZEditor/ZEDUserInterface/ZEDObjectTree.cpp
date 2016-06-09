//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDObjectTree.cpp
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

#include "ZEDObjectTree.h"

#include "ZEDCore/ZEDModule.h"
#include "ZEDCore/ZEDObjectWrapper.h"
#include "ZEDCore/ZEDSelectionEvent.h"

#include <QHeaderView>


void ZEDObjectTree::ObjectEvent(const ZEDObjectEvent* Event)
{
	UpdateWrapper(Event->GetWrapper());
}

void ZEDObjectTree::SelectionEvent(const ZEDSelectionEvent* Event)
{
	QSignalBlocker Blocker(this);

	if (Event->GetType() == ZED_SET_MANAGER_STATE_CHANGED || Event->GetType() == ZED_SET_FOCUS_CHANGED)
		return;

	if (Event->GetType() == ZED_SET_SELECTED)
	{
		const ZEArray<ZEDObjectWrapper*>& SelectedObjects = Event->GetSelectedObjects();
		for (ZESize I = 0; I < SelectedObjects.GetCount(); I++)
		{
			QTreeWidgetItem* Item = GetTreeItem(SelectedObjects[I]);
			if (Item == NULL)
				continue;

			Item->setSelected(true);
		}

	}
	else if (Event->GetType() == ZED_SET_DESELECTED)
	{
		const ZEArray<ZEDObjectWrapper*>& DeselectedObjects = Event->GetUnselectedObjects();
		for (ZESize I = 0; I < DeselectedObjects.GetCount(); I++)
		{
			QTreeWidgetItem* Item = GetTreeItem(DeselectedObjects[I]);
			if (Item == NULL)
				continue;

			Item->setSelected(false);
		}
	}	
}

bool ZEDObjectTree::InitializeSelf()
{
	if (!ZEDComponent::InitializeSelf())
		return false;

	Update();

	return true;
}

QTreeWidgetItem* ZEDObjectTree::FindItem(QTreeWidgetItem* TreeItem, ZEDObjectWrapper* Target) const
{
	ZEDObjectWrapper* Wrapper = GetWrapper(TreeItem);
	if (Wrapper == NULL)
		return false;

	if (Wrapper == Target)
		return TreeItem;

	for (ZESize I = 0; I < TreeItem->childCount(); I++)
	{
		QTreeWidgetItem* Result = FindItem(TreeItem->child((int)I), Target);
		if (Result != NULL)
			return Result;
	}

	return NULL;
}

bool ZEDObjectTree::FilterWrapper(ZEDObjectWrapper* Wrapper)
{
	if (!FilterSearch.pattern().isEmpty() && QString(Wrapper->GetName()).indexOf(FilterSearch) == -1)
		return false;

	if (FilterIncludedClasses.GetCount() != 0)
	{
		bool Found = false;
		for (ZESize I = 0; I < FilterIncludedClasses.GetCount(); I++)
		{
			if (ZEClass::IsDerivedFrom(FilterIncludedClasses[I], Wrapper->GetObject()->GetClass()))
			{
				Found = true;
				break;
			}
		}

		if (!Found)
			return false;
	}


	if (FilterExcludedClasses.GetCount() != 0)
	{
		bool Found = false;
		for (ZESize I = 0; I < FilterExcludedClasses.GetCount(); I++)
		{
			if (ZEClass::IsDerivedFrom(FilterExcludedClasses[I], Wrapper->GetObject()->GetClass()))
				return false;
		}
	}

	return true;
}

void ZEDObjectTree::OnSelectionChanged()
{
	QList<QTreeWidgetItem*> SelectedItems = selectedItems();
	ZEArray<ZEDObjectWrapper*> Wrappers;
	for (ZESize I = 0; I < SelectedItems.count(); I++)
	{
		ZEDObjectWrapper* Wrapper = GetWrapper(SelectedItems[I]);
		if (Wrapper == NULL)
			continue;

		Wrappers.Add(Wrapper);
	}

	GetModule()->GetSelectionManager()->SetSelection(Wrappers);
}

void ZEDObjectTree::UpdateItem(QTreeWidgetItem* TreeItem, ZEDObjectWrapper* Wrapper)
{
	TreeItem->setText(0, Wrapper->GetName().ToCString());
	TreeItem->setText(1, Wrapper->GetObject()->GetClass()->GetName());
}

void ZEDObjectTree::UpdateWrapper(QTreeWidgetItem* TreeItem)
{
	ZEDObjectWrapper* Wrapper = GetWrapper(TreeItem);
	if (Wrapper == NULL)
		return;

	if (!FilterWrapper(Wrapper))
		return;

	UpdateItem(TreeItem, Wrapper);

	// Remove Old
	const ZEArray<ZEDObjectWrapper*>& ChildWrappers = Wrapper->GetChildWrappers();
	for (ZESize I = 0; I < TreeItem->childCount(); I++)
	{
		QTreeWidgetItem* ChildTreeItem = TreeItem->child((int)I);
		ZEDObjectWrapper* ChildWrapper = GetWrapper(TreeItem);

		if (ChildWrapper == NULL)
		{
			delete ChildTreeItem;
			I--;
		}
		else if (!ChildWrappers.Exists(Wrapper))
		{
			delete ChildTreeItem;
			I--;
		}
	}

	// Add New
	for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
	{
		if (I < TreeItem->childCount())
		{
			QTreeWidgetItem* ChildTreeItem = TreeItem->child((int)I);
			ZEDObjectWrapper* ChildTreeItemWrapper = GetWrapper(ChildTreeItem);
			if (ChildWrappers[I] == ChildTreeItemWrapper)
			{
				UpdateWrapper(ChildTreeItem);
				continue;
			}
		}

		QTreeWidgetItem* NewTreeItem = new QTreeWidgetItem();
		NewTreeItem->setData(0, Qt::UserRole, QVariant((qlonglong)ChildWrappers[I]));
		UpdateWrapper(NewTreeItem);
		TreeItem->insertChild((int)I, NewTreeItem);
	}
}

void ZEDObjectTree::UpdateWrapper(ZEDObjectWrapper* Wrapper)
{
	QTreeWidgetItem* Item = GetTreeItem(Wrapper);
	if (Item == NULL)
		return;

	UpdateWrapper(Item);
}

void ZEDObjectTree::SetRootWrapper(ZEDObjectWrapper* Wrapper)
{
	RootWrapper = Wrapper;

	if (IsInitialized())
		Update();
}

ZEDObjectWrapper* ZEDObjectTree::GetRootWrapper() const
{
	return RootWrapper;
}

ZEDObjectWrapper* ZEDObjectTree::GetWrapper(QTreeWidgetItem* Item) const
{
	if (Item == NULL)
		return NULL;

	bool Result;
	ZEDObjectWrapper* Wrapper = (ZEDObjectWrapper*)Item->data(0, Qt::UserRole).toULongLong(&Result);
	if (!Result)
		return NULL;

	return Wrapper;
}

QTreeWidgetItem* ZEDObjectTree::GetTreeItem(ZEDObjectWrapper* Wrapper) const
{
	return FindItem(topLevelItem(0), Wrapper);
}

void ZEDObjectTree::SetFilterPattern(const QString& Text)
{
	FilterSearch.setPattern(Text);
}

QString ZEDObjectTree::GetFilterPattern() const
{
	return FilterSearch.pattern();
}

const ZEArray<ZEClass*> ZEDObjectTree::GetFilterIncludedClasses() const
{
	return FilterIncludedClasses;
}

void ZEDObjectTree::AddFilterIncludedClass(ZEClass* Class)
{
	if (FilterExcludedClasses.Exists(Class))
		return;

	FilterIncludedClasses.Add(Class);
}

void ZEDObjectTree::RemoveFilterIncludedClass(ZEClass* Class)
{
	FilterIncludedClasses.RemoveValue(Class);
}

const ZEArray<ZEClass*> ZEDObjectTree::GetFilterExcludedClasses() const
{
	return FilterExcludedClasses;
}

void ZEDObjectTree::RemoveFilterExcludedClass(ZEClass* Class)
{
	if (FilterExcludedClasses.Exists(Class))
		return;

	FilterExcludedClasses.Add(Class);
}

void ZEDObjectTree::AddFilterExcludedClass(ZEClass* Class)
{
	FilterExcludedClasses.RemoveValue(Class);
}

void ZEDObjectTree::Update()
{
	if (RootWrapper == NULL)
	{
		clear();
		return;
	}

	if (topLevelItem(0) == NULL)
	{
		QTreeWidgetItem* NewTreeItem = new QTreeWidgetItem();
		NewTreeItem->setExpanded(true);
		addTopLevelItem(NewTreeItem);
	}
	
	topLevelItem(0)->setData(0, Qt::UserRole, QVariant((qlonglong)RootWrapper));
	UpdateWrapper(topLevelItem(0));
}

ZEDObjectTree::ZEDObjectTree(QWidget* Parent) : QTreeWidget(Parent)
{
	setWindowTitle("Object Browser");

	RootWrapper = NULL;

	QTreeWidgetItem* Item = new QTreeWidgetItem();
	Item->setText(0, "Name");
	Item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
	Item->setText(1, "Class");
	Item->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);
	setHeaderItem(Item);

	header()->setStretchLastSection(false);
	header()->setSectionResizeMode(0, QHeaderView::Stretch);
	header()->setSectionResizeMode(1, QHeaderView::Interactive);

	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setAlternatingRowColors(true);
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(OnSelectionChanged()));
}
