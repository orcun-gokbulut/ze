//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDClassModel.cpp
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

#include "ZEDClassModel.h"

#include "ZEDS/ZEArray.h"
#include "ZEMeta/ZEClass.h"
#include "ZEMeta/ZEProvider.h"
#include "ZEMeta/ZEObject.h"
#include "ZEFile/ZEPathInfo.h"

#include <QtGui/QIcon>
#include <QtCore/QMimeData>


bool ZEDClassModel::Filter(ZEClass* Class) const
{
	if (RootClass != NULL && !ZEClass::IsDerivedFrom(RootClass, Class))
		return false;

	if (!SearchPattern.GetPattern().IsEmpty() && !SearchPattern.Match(Class->GetName()))
		return false;

	if (IncludeFilter.GetCount() != 0)
	{
		for (ZESize I = 0; I < IncludeFilter.GetCount(); I++)
		{
			if (!ZEClass::IsDerivedFrom(IncludeFilter[I], Class))
				return false;
		}
	}

	if (ExcludeFilter.GetCount() != 0)
	{
		for (ZESize I = 0; I < IncludeFilter.GetCount(); I++)
		{
			if (ZEClass::IsDerivedFrom(IncludeFilter[I], Class))
				return false;
		}
	}

	return true;
}

void ZEDClassModel::SetMode(ZEDClassModelMode Mode)
{
	if (this->Mode == Mode)
		return;

	beginResetModel();
	this->Mode = Mode;
	endResetModel();
}

ZEDClassModelMode ZEDClassModel::GetMode() const
{
	return Mode;
}

void ZEDClassModel::SetRootClass(ZEClass* Class)
{
	if (RootClass == Class)
		return;

	beginResetModel();
	RootClass = Class;
	endResetModel();
}

ZEClass* ZEDClassModel::GetRootClass() const
{
	return RootClass;
}

void ZEDClassModel::SetSearchPattern(const ZEString& Pattern)
{
	if (SearchPattern.GetPattern() == Pattern)
		return;

	beginResetModel();
	SearchPattern.Compile(Pattern);
	endResetModel();
}

const ZEString& ZEDClassModel::GetSearchPattern() const
{
	return SearchPattern.GetPattern();
}

void ZEDClassModel::SetIncludeFilter(const ZEArray<ZEClass*>& Filter)
{
	if (IncludeFilter.GetSize() == Filter.GetSize())
	{
		bool Found = false;
		for (ZESize I = 0; I < Filter.GetCount(); I++)
		{
			if (!IncludeFilter.Exists(Filter[I]))
				Found = true;
		}

		if (!Found)
			return;
	}

	beginResetModel();
	IncludeFilter = Filter;
	endResetModel();
}

const ZEArray<ZEClass*>& ZEDClassModel::GetIncludeFilter() const
{
	return IncludeFilter;
}

void ZEDClassModel::SetExcludeFilter(const ZEArray<ZEClass*>& Filter)
{
	if (ExcludeFilter.GetSize() == Filter.GetSize())
	{
		bool Found = false;
		for (ZESize I = 0; I < Filter.GetCount(); I++)
		{
			if (!ExcludeFilter.Exists(Filter[I]))
				Found = true;
		}

		if (!Found)
			return;
	}

	beginResetModel();
	ExcludeFilter = Filter;
	endResetModel();
}

const ZEArray<ZEClass*>& ZEDClassModel::GetExcludeFilter() const
{
	return ExcludeFilter;
}

void ZEDClassModel::SetExcludeAbstractClasses(bool Enabled)
{
	if (ExcludeAbstractClasses == Enabled)
		return;

	beginResetModel();
	ExcludeAbstractClasses = Enabled;
	endResetModel();
}

bool ZEDClassModel::GetExcludeAbstractClasses() const
{
	return ExcludeAbstractClasses;
}

ZEClass* ZEDClassModel::ConvertToClass(const QModelIndex& Index) const
{
	return static_cast<ZEClass*>(Index.internalPointer());
}

QModelIndex ZEDClassModel::index(int Row, int Column, const QModelIndex& Parent) const
{
	if (Mode == ZED_CMM_INHERITENCE_TREE)
	{
		ZEClass* ParentClass = ConvertToClass(Parent);
		if (ParentClass == NULL)
			ParentClass = RootClass;

		const ZEArray<ZEClass*> Classes = ZEProvider::GetInstance()->GetClasses();
		Classes.LockRead();
		{
			ZESize Index = 0;
			for (ZESize I = 0; I < Classes.GetCount(); I++)
			{
				if (Classes[I]->GetParentClass() != ParentClass)
					continue;

				if (!Filter(Classes[I]->GetParentClass()))
					continue;

				if (Index == Row)
				{
					ZEClass* Class = Classes[I];
					Classes.UnlockRead();
					return createIndex(Row, Column, Class);
				}

				Index++;
			}
		}
		Classes.UnlockRead();
	}
	else if (Mode == ZED_CMM_LIST)
	{
		if (Parent != QModelIndex())
			return QModelIndex();

		const ZEArray<ZEClass*> Classes = ZEProvider::GetInstance()->GetClasses();
		Classes.LockRead();
		{
			if (Row >= Classes.GetCount())
			{
				Classes.UnlockRead();
				return QModelIndex();
			}

			ZESize Index = 0;
			for (ZESize I = 0; I < Classes.GetCount(); I++)
			{
				if (!Filter(Classes[I]))
					continue;

				if (Index == Row)
				{
					Classes.UnlockRead();
					return createIndex(Row, Column, Classes[I]);
				}

				Index++;
			}
		}
		Classes.UnlockRead();
		return QModelIndex();
	}
	
	return QModelIndex();
}

QModelIndex ZEDClassModel::parent(const QModelIndex& Child) const
{
	if (Mode == ZED_CMM_INHERITENCE_TREE)
	{
		ZEClass* ChildClass = ConvertToClass(Child);
		if (ChildClass == NULL)
			return QModelIndex();

		ZEClass* ParentClass = ChildClass->GetParentClass();
		if (ParentClass == NULL)
			return QModelIndex();

		ZEClass* ParentParentClass = ParentClass->GetParentClass();
		if (ParentParentClass == NULL)
			return createIndex(0, 0, ZEObject::Class());
		else if (ParentParentClass == RootClass)
			return createIndex(0, 0, RootClass);

		const ZEArray<ZEClass*> Classes = ZEProvider::GetInstance()->GetClasses();
		Classes.LockRead();
		{
			ZESize Index = 0;
			for (ZESize I = 0; I < Classes.GetCount(); I++)
			{
				if (Classes[I]->GetParentClass() != ParentParentClass)
					continue;

				if (!Filter(Classes[I]->GetParentClass()))
					continue;

				if (Classes[I] == ParentClass)
				{
					Classes.UnlockRead();
					return createIndex(0, 0, Classes[I]);
				}
				Index++;
			}
		}
		Classes.UnlockRead();
	}

	return QModelIndex();
}

bool ZEDClassModel::hasChildren(const QModelIndex& Parent) const
{
	if (Mode == ZED_CMM_LIST)
		return (Parent == QModelIndex());
	else
		return QAbstractItemModel::hasChildren(Parent);
}

int ZEDClassModel::rowCount(const QModelIndex& Parent) const
{
	if (Mode == ZED_CMM_INHERITENCE_TREE)
	{
		ZESize Count = 0;
		ZEClass* ParentClass = ConvertToClass(Parent);
		if (ParentClass == NULL)
			ParentClass = RootClass;

		const ZEArray<ZEClass*> Classes = ZEProvider::GetInstance()->GetClasses();
		Classes.LockRead();
		{
			for (ZESize I = 0; I < Classes.GetCount(); I++)
			{
				if (Classes[I]->GetParentClass() != ParentClass)
					continue;

				if (!Filter(Classes[I]))
					continue;

				Count++;
			}
		}
		Classes.UnlockRead();

		return Count;
	}
	else if (Mode == ZED_CMM_LIST)
	{
		if (Parent != QModelIndex())
			return 0;

		ZESize Count = 0;
		const ZEArray<ZEClass*> Classes = ZEProvider::GetInstance()->GetClasses();
		Classes.LockRead();
		{
			for (ZESize I = 0; I < Classes.GetCount(); I++)
			{
				if (!Filter(Classes[I]))
					continue;

				Count++;
			}
		}
		Classes.UnlockRead();
		return Count;
	}
}

int ZEDClassModel::columnCount(const QModelIndex& Parent) const
{
	return 3;
}

QVariant ZEDClassModel::data(const QModelIndex& Index, int Role) const
{
	ZEClass* Class = static_cast<ZEClass*>(Index.internalPointer());
	if (Class == NULL)
		return QVariant();

	switch (Role)
	{
		case Qt::DisplayRole:
		{
			if (Index.column() == 0)
			{
				return Class->GetName();
			}
			else if (Index.column() == 1)
			{
				if (Class->GetParentClass() == NULL)
					return "";

				return Class->GetParentClass()->GetName();
			}
			else if (Index.column() == 2)
			{
				return Class->IsAbstract();
			}
			break;
		}

		case Qt::DecorationRole:
			if (Index.column() == 0)
			{
				QString IconFile = ZEPathInfo(Class->GetAttributeValue("ZEDEditor.ObjectWrapper.Icon")).GetRealPath().Path;
				return QIcon(IconFile);
			}
			break;

		case Qt::TextAlignmentRole:
			if (Index.column() == 0)
				return (Qt::AlignLeft | Qt::AlignVCenter);
			else
				return (Qt::AlignCenter | Qt::AlignVCenter);
			break;
	}

	return QVariant();
}

QVariant ZEDClassModel::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
	if (Orientation == Qt::Vertical)
		return QVariant();

	switch (Role)
	{
		case Qt::DisplayRole:
			if (Section == 0)
				return "Name";
			else if (Section == 1)
				return "Parent";
			else if (Section = 2)
				return "Abstract";
			break;

		case Qt::TextAlignmentRole:
			if (Section == 0)
				return (Qt::AlignLeft | Qt::AlignVCenter);
			else
				return (Qt::AlignCenter | Qt::AlignVCenter);
			break;
	}

	return QVariant();
}

Qt::ItemFlags ZEDClassModel::flags(const QModelIndex& Index) const
{
	if (Index.column() == 0)
		return QAbstractItemModel::flags(Index) | Qt::ItemIsDragEnabled;
	else
		return  QAbstractItemModel::flags(Index);
}

QStringList ZEDClassModel::mimeTypes() const
{
	QStringList List;
	List << "application/x.zinek.zeclass";
	return List;
}

QMimeData* ZEDClassModel::mimeData(const QModelIndexList& Indexes) const
{
	if (Indexes.count() != 1)
		return NULL;

	ZEClass* Class = ConvertToClass(Indexes[0]);
	QMimeData* Data = new QMimeData();
	Data->setText(Class->GetName());
	Data->setData("application/x.zinek.zeclass", QByteArray((const char*)&Class, sizeof(ZEClass*)));
	return Data;
}

Qt::DropActions ZEDClassModel::supportedDragActions() const
{
	return Qt::DropAction::CopyAction;
}

ZEDClassModel::ZEDClassModel()
{
	RootClass = NULL;
	Mode = ZED_CMM_INHERITENCE_TREE;
	ExcludeAbstractClasses = false;
}
