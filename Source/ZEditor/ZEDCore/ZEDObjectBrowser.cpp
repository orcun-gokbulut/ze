//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDObjectBrowser.cpp
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

#include "ZEDObjectBrowser.h"

#include "ZEDCore/ZEDObjectWrapper.h"
#include "ZEDCore/ZEDCore.h"
#include "ZEDCore/ZEDModule.h"

#include <QHeaderView>

QTreeWidgetItem* ZEDObjectBrowser::FindTreeItem(QTreeWidgetItem* TreeItem, ZEDObjectWrapper* Target)
{
	bool Ok = false;
	ZEDObjectWrapper* Wrapper = (ZEDObjectWrapper*)TreeItem->data(0, Qt::UserRole).toULongLong(&Ok);

	if (!Ok || Wrapper == NULL)
		return false;

	if (Wrapper == Target)
		return TreeItem;

	for (ZESize I = 0; I < TreeItem->childCount(); I++)
	{
		QTreeWidgetItem* Result = FindTreeItem(TreeItem->child((int)I), Wrapper);
		if (Result != NULL)
			return Result;
	}

	return NULL;
}

void ZEDObjectBrowser::OnSelectionChanged()
{

}

void ZEDObjectBrowser::UpdateItem(QTreeWidgetItem* TreeItem, ZEDObjectWrapper* Wrapper)
{
	TreeItem->setText(0, Wrapper->GetName().ToCString());
	TreeItem->setText(1, Wrapper->GetObject()->GetClass()->GetName());
}

void ZEDObjectBrowser::SetRootWrapper(ZEDObjectWrapper* Wrapper)
{
	RootWrapper = Wrapper;
}

ZEDObjectWrapper* ZEDObjectBrowser::GetRootWrapper()
{
	return RootWrapper;
}

QTreeWidgetItem* ZEDObjectBrowser::FindTreeItem(ZEDObjectWrapper* Wrapper)
{
	return FindTreeItem(topLevelItem(0), Wrapper);
}

void ZEDObjectBrowser::UpdateWrapper(QTreeWidgetItem* TreeItem)
{
	bool Ok = false;
	ZEDObjectWrapper* Wrapper = (ZEDObjectWrapper*)TreeItem->data(0, Qt::UserRole).toULongLong(&Ok);

	if (!Ok || Wrapper == NULL)
		return;

	UpdateItem(TreeItem, Wrapper);

	// Remove Old
	const ZEArray<ZEDObjectWrapper*>& ChildWrappers = Wrapper->GetChildWrappers();
	for (ZESize I = 0; I < TreeItem->childCount(); I++)
	{
		QTreeWidgetItem* ChildTreeItem = TreeItem->child((int)I);
		ZEDObjectWrapper* ChildWrapper = (ZEDObjectWrapper*)TreeItem->data(0, Qt::UserRole).toULongLong(&Ok);

		if (!Ok || Wrapper == NULL)
		{
			delete ChildTreeItem;
			I--;
		}
		else if (!ChildWrappers.Exists(Wrapper))
		{
			delete ChildTreeItem;
		}
	}

	// Add New
	for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
	{
		if (I < TreeItem->childCount())
		{
			QTreeWidgetItem* ChildTreeItem = TreeItem->child((int)I);
			ZEDObjectWrapper* ChildTreeItemWrapper = (ZEDObjectWrapper*)ChildTreeItem->data(0, Qt::UserRole).toULongLong(&Ok);
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

void ZEDObjectBrowser::Update()
{
	if (RootWrapper == NULL)
	{
		clear();
		return;
	}

	if (topLevelItem(0) == NULL)
	{
		QTreeWidgetItem* NewTreeItem = new QTreeWidgetItem();
		addTopLevelItem(NewTreeItem);
	}
	
	topLevelItem(0)->setData(0, Qt::UserRole, QVariant((qlonglong)RootWrapper));
	UpdateWrapper(topLevelItem(0));
}

ZEDObjectBrowser::ZEDObjectBrowser(QWidget* Parent) : QTreeWidget(Parent)
{
	RootWrapper = NULL;

	QTreeWidgetItem* Item = new QTreeWidgetItem();
	Item->setText(0, "Name");
	Item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
	Item->setText(1, "Class");
	Item->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);
	setHeaderItem(Item);

	header()->setStretchLastSection(false);
	header()->setSectionResizeMode(0, QHeaderView::Stretch);

	connect(this, SIGNAL(selectionChanged()), this, SLOT(OnSelectionChanged()));
}
