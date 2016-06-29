//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDAssetTree.cpp
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

#include "ZEDAssetTree.h"

#include <QHeaderView>
#include "ZEFile\ZEDirectoryInfo.h"


bool ZEDAssetTree::Filter(const QString& Item)
{
	if (!ExtensionPattern.isEmpty() && Item.indexOf(ExtensionPattern) == -1)
		return false;

	if (!SearchPattern.isEmpty() && Item.indexOf(SearchPattern) == -1)
		return false;

	return true;
}

void ZEDAssetTree::UpdateDirectoryTree(const QString& Path, QTreeWidgetItem* PathItem)
{
	ZEDirectoryInfo DirectoryInfo(Path.toUtf8().begin());

	ZEArray<ZEString> Directories = DirectoryInfo.GetSubDirectories();
	for (ZESize I = 0; I < Directories.GetCount(); I++)
	{
		QTreeWidgetItem* Item = NULL;
		if (Mode == ZED_ATM_TREE && SearchPattern.isEmpty())
		{
			Item = new QTreeWidgetItem();
			Item->setText(0, Directories[I].ToCString());
			Item->setData(0, Qt::UserRole, QString("%1/%2").arg(Path).arg(Directories[I].ToCString()));
			
			if (PathItem == NULL)
				addTopLevelItem(Item);
			else
				PathItem->addChild(Item);
		}

		UpdateDirectoryTree(QString("%1/%2").arg(Path).arg(Directories[I].ToCString()), Item);
	}

	ZEArray<ZEString> Files = DirectoryInfo.GetFiles();
	for (ZESize I = 0; I < Files.GetCount(); I++)
	{
		if (!Filter(Files[I].ToCString()))
			continue;

		QTreeWidgetItem* Item = new QTreeWidgetItem();
		Item->setText(0, Files[I].ToCString());
		Item->setData(0, Qt::UserRole, QString("%1/%2").arg(Path).arg(Files[I].ToCString()));

		if (PathItem == NULL)
			addTopLevelItem(Item);
		else
			PathItem->addChild(Item);
	}
}

void ZEDAssetTree::SetPath(const QString& Path)
{
	if (this->Path == Path)
		return;

	this->Path = Path;

	Update();
}

const QString& ZEDAssetTree::GetPath() const
{
	return Path;
}

void ZEDAssetTree::SetMode(ZEDAssetTreeMode Mode)
{
	if (this->Mode == Mode)
		return;

	this->Mode = Mode;

	Update();
}

void ZEDAssetTree::SetExtensions(const QString& Extensions)
{
	ExtensionPattern.setPattern(Extensions);
}

QString ZEDAssetTree::GetExtensions() const
{
	return ExtensionPattern.pattern();
}

ZEDAssetTreeMode ZEDAssetTree::GetMode() const
{
	return Mode;
}

void ZEDAssetTree::SetSearchPattern(const QString& Pattern)
{
	if (SearchPattern.pattern() == Pattern)
		return;

	SearchPattern.setPattern(Pattern);

	Update();
}

QString ZEDAssetTree::GetSearchPattern() const
{
	return SearchPattern.pattern();
}

QString ZEDAssetTree::GetSelectedPath() const
{
	if (selectedItems().count() != 1)
		return "";

	return selectedItems()[0]->data(0, Qt::UserRole).toString();
}

void ZEDAssetTree::Update()
{
	setSortingEnabled(false);

	clear();
	
	UpdateDirectoryTree(Path, NULL);
	
	setSortingEnabled(true);
	sortByColumn(0, Qt::AscendingOrder);
}

ZEDAssetTree::ZEDAssetTree(QWidget* Parent) : QTreeWidget(Parent)
{
	Path = "#R:/";
	Mode = ZED_ATM_TREE;

	setWindowTitle("Class Browser");

	ExtensionPattern.setPattern("*.*");
	ExtensionPattern.setCaseSensitivity(Qt::CaseInsensitive);
	ExtensionPattern.setPatternSyntax(QRegExp::Wildcard);

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
