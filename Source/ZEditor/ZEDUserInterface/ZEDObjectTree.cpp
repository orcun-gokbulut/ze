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
#include "ZEDCore/ZEDSelectionEvent.h"
#include "ZEDCore/ZEDObjectWrapper.h"
#include "ZEDCore/ZEDObjectEvent.h"

#include <QHeaderView>


void ZEDObjectTree::ObjectEvent(const ZEDObjectEvent* Event)
{
	if (Event->GetType() != ZED_OCET_REMOVED)
	{
		bool ParentFound = false;
		ZEDObjectWrapper* Current = Event->GetWrapper();
		while (Current != NULL)
		{
			if (Current == RootWrapper)
			{
				ParentFound = true;
				break;
			}

			Current = Current->GetParent();
		}

		if (!ParentFound)
			return;
	}

	switch (Event->GetType())
	{
		case ZED_OCET_ADDED:
			AddWrapper(Event->GetWrapper());
			break;

		case ZED_OCET_REMOVED:
			RemoveWrapper(Event->GetWrapper());
			break;

		case ZED_OCET_CHANGED:
			UpdateItem(GetTreeItem(Event->GetWrapper()), Event->GetWrapper());
			break;

		default:
			break;
	}
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
	// Search by keyword
	bool FilterSearchResult = false;
	if (!FilterSearch.pattern().isEmpty())
	{
		// Search Name
		if (QString(Wrapper->GetName()).indexOf(FilterSearch) != -1)
			FilterSearchResult = true;

		// Search Id
		if (!FilterSearchResult)
		{
			bool ConversionResult = false;
			ZEInt ObjectId = FilterSearch.pattern().toInt(&ConversionResult);
			if (ConversionResult && ObjectId == Wrapper->GetId())
				FilterSearchResult = true;
		}

		// Search Class & Parent Classes
		if (!FilterSearchResult)
		{
			ZEClass* Class = Wrapper->GetObject()->GetClass();
			while (Class != NULL)
			{
				if (QString(Class->GetName()).indexOf(FilterSearch) != -1)
				{
					FilterSearchResult = true;
					break;
				}

				Class = Class->GetParentClass();
			}
		}
	}
	else
	{
		FilterSearchResult = true;
	}

	if (!FilterSearchResult)
		return false;

	// Filter Included Classes
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

	// Filter Excluded Classes
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

bool ZEDObjectTree::CheckWrapper(ZEDObjectWrapper* Wrapper)
{
	const ZEArray<ZEDObjectWrapper*>& ChildWrappers = Wrapper->GetChildWrappers();
	for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
	{
		if (CheckWrapper(ChildWrappers[I]))
			return true;
	}

	if (FilterWrapper(Wrapper))
		return true;

	return false;
}

void ZEDObjectTree::AddWrapper(ZEDObjectWrapper* Wrapper)
{
	if (Wrapper == NULL)
		return;

	if (!CheckWrapper(Wrapper))
		return;

	// Missing Parent Item
	QTreeWidgetItem* ParentItem = GetTreeItem(Wrapper->GetParent());
	if (Mode == ZED_OTM_TREE && Wrapper != RootWrapper)
	{
		if (ParentItem == NULL)
		{
			AddWrapper(Wrapper->GetParent());
			return;
		}
	}

	QTreeWidgetItem* Item = new QTreeWidgetItem();

	UpdateItem(Item, Wrapper);

	if (Mode == ZED_OTM_LIST || Wrapper == RootWrapper)
		addTopLevelItem(Item);
	else if (Mode == ZED_OTM_TREE)
		ParentItem->addChild(Item);

	if (ParentItem != NULL)
		ParentItem->setExpanded(true);

	const ZEArray<ZEDObjectWrapper*>& ChildWrappers = Wrapper->GetChildWrappers();
	for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
		AddWrapper(ChildWrappers[I]);
}


void ZEDObjectTree::RemoveWrapper(ZEDObjectWrapper* Wrapper)
{
	if (Wrapper == NULL)
		return;

	QTreeWidgetItem* Item = GetTreeItem(Wrapper);
	if (Item == NULL)
		return;

	if (Mode == ZED_OTM_LIST)
	{
		for (ZESize I = 0; I < topLevelItemCount(); I++)
		{
			ZEDObjectWrapper* CurrentWrapper = GetWrapper(topLevelItem(I));
			if (CurrentWrapper->GetParent() == Wrapper)
				RemoveWrapper(CurrentWrapper);
		}
	}

	delete Item;
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
	if (TreeItem == NULL)
		return;

	if (Wrapper == NULL)
		return;

	TreeItem->setData(0, Qt::UserRole, QVariant((ZEUInt64)Wrapper));
	TreeItem->setText(0, Wrapper->GetName().ToCString());
	TreeItem->setText(1, Wrapper->GetObject()->GetClass()->GetName());
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

void ZEDObjectTree::SetMode(ZEDObjectTreeMode Mode)
{
	this->Mode = Mode;

}
ZEDObjectTreeMode ZEDObjectTree::GetMode() const
{
	return Mode;
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
	clear();

	if (RootWrapper == NULL)
		return;

	AddWrapper(RootWrapper);
}

ZEDObjectTree::ZEDObjectTree(QWidget* Parent) : QTreeWidget(Parent)
{
	setWindowTitle("Object Browser");

	RootWrapper = NULL;
	Mode = ZED_OTM_TREE;

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
