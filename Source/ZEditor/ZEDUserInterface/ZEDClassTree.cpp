//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDClassTree.cpp
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

#include "ZEDClassTree.h"

#include "ZEMeta\ZEProvider.h"

#include <QHeaderView>
#include "ZEGame\ZEEntity.h"


bool ZEDClassTree::Filter(ZEClass* Class)
{
	if (!ZEClass::IsDerivedFrom(BaseClass, Class))
		return false;

	if (!SearchPattern.isEmpty() && QString(Class->GetName()).indexOf(SearchPattern) == -1)
		return false;

	return true;
}

void ZEDClassTree::UpdateAlphabeticList()
{
	const ZEArray<ZEClass*>& Classes = ZEProvider::GetInstance()->GetClasses();
	for (ZESize I = 0; I < Classes.GetCount(); I++)
	{
		if (ExcludeAbstract && Classes[I]->IsAbstract())
			continue;

		if (!Filter(Classes[I]))
			continue;

		QTreeWidgetItem* Item = new QTreeWidgetItem();
		Item->setText(0, Classes[I]->GetName());
		Item->setExpanded(true);
		Item->setData(0, Qt::UserRole, QVariant((unsigned long long)Classes[I]));
	
		addTopLevelItem(Item);
	}
}

void ZEDClassTree::UpdateInheritanceTree(ZEClass* CurrentBaseClass, QTreeWidgetItem* ParentItem)
{
	const ZEArray<ZEClass*>& Classes = ZEProvider::GetInstance()->GetClasses();
	for (ZESize I = 0; I < Classes.GetCount(); I++)
	{
		if (ExcludeAbstract && Classes[I]->IsAbstract())
			continue;

		if (!Filter(Classes[I]))
			continue;

		if (Classes[I]->GetParentClass() != CurrentBaseClass)
			continue;

		QTreeWidgetItem* Item = new QTreeWidgetItem();
		Item->setText(0, Classes[I]->GetName());
		Item->setExpanded(true);
		Item->setData(0, Qt::UserRole, QVariant((unsigned long long)Classes[I]));

		if (ParentItem == NULL)
		{
			addTopLevelItem(Item);
		}
		else
		{
			ParentItem->addChild(Item);
			ParentItem->setExpanded(true);
		}

		UpdateInheritanceTree(Classes[I], Item);
	}
}

void ZEDClassTree::UpdateCatagoryTree(QTreeWidgetItem* Item)
{

}

bool ZEDClassTree::InitializeSelf()
{
	if (!ZEDComponent::InitializeSelf())
		return false;

	Update();

	return true;
}

void ZEDClassTree::SetBaseClass(ZEClass* Class)
{
	if (BaseClass == Class)
		return;

	BaseClass = Class;

	if (IsInitialized())
		Update();
}

ZEClass* ZEDClassTree::GetBaseClass() const
{
	return BaseClass;
}

void ZEDClassTree::SetMode(ZEDClassTreeMode Mode)
{
	if (this->Mode == Mode)
		return;

	this->Mode = Mode;

	if (IsInitialized())
		Update();
}

ZEDClassTreeMode ZEDClassTree::GetMode() const
{
	return Mode;
}

void ZEDClassTree::SetExcludeAbstract(bool Enabled)
{
	if (ExcludeAbstract == Enabled)
		return;

	ExcludeAbstract = Enabled;

	if (IsInitialized())
		Update();
}

bool ZEDClassTree::GetExcludeAbstract() const
{
	return false;
}

void ZEDClassTree::SetSearchPattern(const QString& Pattern)
{
	if (SearchPattern.pattern() == Pattern)
		return;

	SearchPattern.setPattern(Pattern);

	if (IsInitialized())
		Update();
}

QString ZEDClassTree::GetSearchPattern() const
{
	return SearchPattern.pattern();
}

ZEClass* ZEDClassTree::GetSelectedClass() const
{
	if (selectedItems().count() != 1)
		return NULL;

	return (ZEClass*)selectedItems()[0]->data(0, Qt::UserRole).toULongLong();
}

void ZEDClassTree::Update()
{
	setSortingEnabled(false);

	clear();
	if (!SearchPattern.isEmpty())
	{
		UpdateAlphabeticList();
	}
	else
	{
		switch (Mode)
		{

			default:
			case ZED_CTM_NONE:
			case ZED_CTM_LIST:
				UpdateAlphabeticList();
				break;
			
			case ZED_CTM_INHERITANCE_TREE:
				UpdateInheritanceTree(BaseClass, NULL);
				break;

			case ZED_CTM_CATEGORY_TREE:
				UpdateCatagoryTree(NULL);
				break;
		}
	}
	
	setSortingEnabled(true);
	sortByColumn(0, Qt::AscendingOrder);
}

ZEDClassTree::ZEDClassTree(QWidget* Parent) : QTreeWidget(Parent)
{
	BaseClass = ZEEntity::Class();
	Mode = ZED_CTM_INHERITANCE_TREE;
	ExcludeAbstract = false;

	setWindowTitle("Class Browser");

	SearchPattern.setCaseSensitivity(Qt::CaseInsensitive);
	SearchPattern.setPatternSyntax(QRegExp::Wildcard);

	QTreeWidgetItem* Item = new QTreeWidgetItem();
	Item->setText(0, "Name");
	Item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
	setHeaderItem(Item);

	header()->setStretchLastSection(false);
	header()->setSectionResizeMode(0, QHeaderView::Stretch);

	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setAlternatingRowColors(true);
}
