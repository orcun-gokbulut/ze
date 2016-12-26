//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDObjectModel.cpp
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

#include "ZEDObjectModel.h"

#include "ZEDS/ZEFormat.h"
#include "ZEFile/ZEPathInfo.h"
#include "ZEDCore/ZEDEditor.h"
#include "ZEDCore/ZEDSelectionEvent.h"
#include "ZEDCore/ZEDObjectWrapper.h"
#include "ZEDCore/ZEDObjectEvent.h"
#include "ZEDCore/ZEDObjectManager.h"

#include <QIcon>
#include <QMimeData>
#include "ZEDCore/ZEDAsset.h"
#include "ZEDCore/ZEDAssetType.h"

bool ZEDObjectModel::Filter(ZEDObjectWrapper* Wrapper) const
{
	// Search by keyword
	bool FilterSearchResult = false;
	if (!FilterSearch.GetPattern().IsEmpty())
	{
		// Search Name
		if (FilterSearch.Match(Wrapper->GetName()))
			FilterSearchResult = true;

		// Search Id
		if (!FilterSearchResult)
		{
			bool ConversionResult = false;
			ZEInt ObjectId = FilterSearch.GetPattern().ToInt32();
			if (ConversionResult && ObjectId == Wrapper->GetId())
				FilterSearchResult = true;
		}

		// Search Class & Parent Classes
		if (!FilterSearchResult)
		{
			ZEClass* Class = Wrapper->GetObject()->GetClass();
			while (Class != NULL)
			{
				if (FilterSearch.Match(Class->GetName()))
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

bool ZEDObjectModel::FilterForward(ZEDObjectWrapper* Wrapper) const
{
	const ZEArray<ZEDObjectWrapper*>& ChildWrappers = Wrapper->GetChildWrappers();

	for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
	{
		if (Filter(ChildWrappers[I]))
			return true;

		if (FilterForward(ChildWrappers[I]))
			return true;
	}

	return false;
}

bool ZEDObjectModel::FilterBackward(ZEDObjectWrapper* Wrapper) const
{
	if (Wrapper == NULL)
		return false;

	if (Filter(Wrapper))
		return true;

	if (FilterBackward(Wrapper->GetParent()))
		return true;

	return false;
}

bool ZEDObjectModel::FilterHierarchy(ZEDObjectWrapper* Wrapper) const
{
	if (Filter(Wrapper))
		return true;
	else if (FilterBackward(Wrapper->GetParent()))
		return true;
	else if (FilterForward(Wrapper))
		return true;

	return false;
}

void ZEDObjectModel::SetRootWrapper(ZEDObjectWrapper* Wrapper)
{
	if (RootWrapper == Wrapper)
		return;

	beginResetModel();
	RootWrapper = Wrapper;
	endResetModel();
}

ZEDObjectWrapper* ZEDObjectModel::GetRootWrapper() const
{
	return RootWrapper;
}

void ZEDObjectModel::SetMode(ZEDObjectModelMode Mode)
{
	if (this->Mode == Mode)
		return;

	beginResetModel();
	this->Mode = Mode;
	endResetModel();

}

ZEDObjectModelMode ZEDObjectModel::GetMode() const
{
	return Mode;
}

void ZEDObjectModel::SetFilterPattern(const ZEString& Text)
{
	if (FilterSearch.GetPattern() == Text)
		return;
	
	beginResetModel();
	FilterSearch.Compile(ZEFormat::Format("*{0}*", Text));
	endResetModel();
}

const ZEString& ZEDObjectModel::GetFilterPattern() const
{
	return FilterSearch.GetPattern();
}

void ZEDObjectModel::SetFilterIncludedObjects(const ZEArray<ZEObject*>& Filter)
{
	if (FilterIncludedObjects.GetSize() == Filter.GetSize())
	{
		bool Found = false;
		for (ZESize I = 0; I < Filter.GetCount(); I++)
		{
			if (!FilterIncludedObjects.Exists(Filter[I]))
				Found = true;
		}

		if (!Found)
			return;
	}

	beginResetModel();
	FilterIncludedObjects = Filter;
	endResetModel();
}

const ZEArray<ZEObject*>& ZEDObjectModel::GetFilterIncludedObjects() const
{
	return FilterIncludedObjects;
}

void ZEDObjectModel::SetFilterExcludedObjects(const ZEArray<ZEObject*>& Filter)
{
	if (FilterExcludedObjects.GetSize() == Filter.GetSize())
	{
		bool Found = false;
		for (ZESize I = 0; I < Filter.GetCount(); I++)
		{
			if (!FilterExcludedObjects.Exists(Filter[I]))
				Found = true;
		}

		if (!Found)
			return;
	}

	beginResetModel();
	FilterExcludedObjects = Filter;
	endResetModel();
}

const ZEArray<ZEObject*>& ZEDObjectModel::GetFilterExcludedObjects() const
{
	return FilterExcludedObjects;
}

void ZEDObjectModel::SetFilterIncludedClasses(const ZEArray<ZEClass*>& Filter)
{
	if (FilterIncludedClasses.GetSize() == Filter.GetSize())
	{
		bool Found = false;
		for (ZESize I = 0; I < Filter.GetCount(); I++)
		{
			if (!FilterIncludedClasses.Exists(Filter[I]))
				Found = true;
		}

		if (!Found)
			return;
	}

	beginResetModel();
	FilterIncludedClasses = Filter;
	endResetModel();
}

const ZEArray<ZEClass*>& ZEDObjectModel::GetFilterIncludedClasses() const
{
	return FilterIncludedClasses;
}

void ZEDObjectModel::SetFilterExclucedClasses(const ZEArray<ZEClass*>& Filter)
{
	if (FilterExcludedClasses.GetSize() == Filter.GetSize())
	{
		bool Found = false;
		for (ZESize I = 0; I < Filter.GetCount(); I++)
		{
			if (!FilterExcludedClasses.Exists(Filter[I]))
				Found = true;
		}

		if (!Found)
			return;
	}

	beginResetModel();
	FilterExcludedClasses = Filter;
	endResetModel();
}

const ZEArray<ZEClass*>& ZEDObjectModel::GetFilterExcludedClasses() const
{
	return FilterExcludedClasses;
}

ZEDObjectWrapper* ZEDObjectModel::ConvertToWrapper(const QModelIndex& Index) const
{
	return static_cast<ZEDObjectWrapper*>(Index.internalPointer());
}

void ZEDObjectModel::ObjectEvent(const ZEDObjectEvent* Event)
{
	if (RootWrapper == NULL)
		return;

	if (Event->GetType() == ZED_OET_ADDING)
	{
		if (Mode == ZED_OMM_TREE)
		{
			ZEDObjectWrapper* Wrapper = Event->GetWrapper();
			if (!FilterHierarchy(Wrapper))
				return;

			int Index = (int)Wrapper->GetParent()->GetChildWrappers().GetCount();
			beginInsertRows(parent(createIndex(0, 0, Wrapper)), Index, Index);
		}
		else if (Mode == ZED_OMM_LIST)
		{
			beginResetModel();
		}

	}
	else if (Event->GetType() == ZED_OET_ADDED)
	{
		if (Mode == ZED_OMM_TREE)
			endInsertRows();
		else if (Mode == ZED_OMM_LIST)
			endResetModel();
	}
	else if (Event->GetType() == ZED_OET_REMOVING)
	{
		if (Mode == ZED_OMM_TREE)
		{
			ZEDObjectWrapper* Wrapper = Event->GetWrapper();
			if (!FilterHierarchy(Wrapper))
				return;

			ZESSize Index = Wrapper->GetParent()->GetChildWrappers().FindIndex(Wrapper);
			beginRemoveRows(parent(createIndex(0, 0, Wrapper)), Index, Index);
		}
		else if (Mode == ZED_OMM_LIST)
		{
			beginResetModel();
		}
	}
	else if (Event->GetType() == ZED_OET_REMOVED)
	{
		if (Mode == ZED_OMM_TREE)
			endRemoveRows();
		else if (Mode == ZED_OMM_LIST)
			endResetModel();
	}
	else if (Event->GetType() == ZED_OET_CHANGED)
	{
		if (Mode == ZED_OMM_TREE)
		{
			ZEDObjectWrapper* Wrapper = Event->GetWrapper();
			if (!FilterHierarchy(Wrapper))
				return;

			ZESSize Index = Wrapper->GetParent()->GetChildWrappers().FindIndex(Wrapper);
			dataChanged(createIndex(Index, 0, Wrapper->GetParent()), createIndex(Index, columnCount(), Wrapper->GetParent()));
		}
		else if (Mode == ZED_OMM_LIST)
		{
			beginResetModel();
			endResetModel();
		}
	}
}

ZEDObjectWrapper* ZEDObjectModel::indexList(ZEDObjectWrapper* Wrapper, int Row, int& Index) const
{
	if (Filter(Wrapper))
	{
		if (Index == Row)
			return Wrapper;

		Index++;
	}

	const ZEArray<ZEDObjectWrapper*>& ChildWrappers =  Wrapper->GetChildWrappers();
	for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
	{
		ZEDObjectWrapper* Result = indexList(ChildWrappers[I], Row, Index);
		if (Result != NULL)
			return Result;
	}

	return NULL;
}

int ZEDObjectModel::rowCountList(ZEDObjectWrapper* Wrapper) const
{
	int Count = (Filter(Wrapper) ? 1 : 0);

	const ZEArray<ZEDObjectWrapper*>& ChildWrappers =  Wrapper->GetChildWrappers();
	for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
	{
		Count += rowCountList(ChildWrappers[I]);
	}

	return Count;
}

bool ZEDObjectModel::dropMimeDataInternal(const QMimeData* Data, Qt::DropAction Action, int Row, int Column, const QModelIndex& Parent, bool Operation) const
{
	ZEDObjectWrapper* Destination = NULL;
	if (Row == -1 || Column == -1)
		Destination = ConvertToWrapper(Parent);
	else
		Destination = ConvertToWrapper(index(Row, Column, Parent));
	if (Destination == NULL)
		return false;

	if (Data->hasFormat("application/x.zinek.zeobjectwrapper"))
	{
		QByteArray Array = Data->data("application/x.zinek.zeobjectwrapper");
		ZEDObjectWrapper** Wrappers = reinterpret_cast<ZEDObjectWrapper**>(Array.data());
		ZESize WrapperCount = Data->data("application/x.zinek.zeobjectwrapper").size() / sizeof(ZEDObjectWrapper*);

		if (WrapperCount == 0)
			return false;

		if (Action == Qt::MoveAction)
		{
			for (ZESize I = 0; I < WrapperCount; I++)
			{
				if (Destination == Wrappers[I])
					return false;

				if (!Destination->CheckChildrenClass(Wrappers[I]->GetObjectClass()))
					return false;
			}

			if (Operation)
			{
				ZEArray<ZEDObjectWrapper*> SourceWrappers;
				SourceWrappers.AddMultiple(Wrappers, WrapperCount);
				GetEditor()->GetObjectManager()->RelocateObjects(Destination, SourceWrappers);
			}

			return true;
		}
		else if (Action == Qt::CopyAction)
		{
			for (ZESize I = 0; I < WrapperCount; I++)
			{
				if (!Destination->CheckChildrenClass(Wrappers[I]->GetObjectClass()))
					return false;
			}


			if (Operation)
			{
				ZEArray<ZEDObjectWrapper*> SourceWrappers;
				SourceWrappers.AddMultiple(Wrappers, WrapperCount);
				GetEditor()->GetObjectManager()->CloneObjects(SourceWrappers);
				GetEditor()->GetObjectManager()->RelocateObjects(Destination, SourceWrappers);
			}

			return true;
		}
	}
	else if (Data->hasFormat("application/x.zinek.zeclass"))
	{
		if (Action != Qt::LinkAction)
			return false;

		ZEClass* Class = *reinterpret_cast<ZEClass**>(Data->data("application/x.zinek.zeclass").data());
		if (Class->IsAbstract())
			return false;

		if (!Destination->CheckChildrenClass(Class))
			return false;

		if (Operation)
			GetEditor()->GetObjectManager()->CreateObject(Destination, Class);

		return true;
	}
	else if (Data->hasFormat("application/x.zinek.zedasset"))
	{

		ZEDAsset* Asset = *reinterpret_cast<ZEDAsset**>(Data->data("application/x.zinek.zedasset").data());
		if (!Asset->GetType()->GetCapabilities().GetFlags(ZED_ATC_WRAPPER))
			return false;

		ZEClass* ObjectClass = Asset->GetWrapperObjectClass();
		if (!Destination->CheckChildrenClass(ObjectClass))
			return false;

		if (Operation)
		{
			ZEDObjectWrapper* Wrapper = Asset->CreateWrapper(GetEditor());
			if (Wrapper == NULL)
				return false;

			GetEditor()->GetObjectManager()->RelocateObject(Destination, Wrapper);
		}

		return true;
	}

	return false;
}

QModelIndex ZEDObjectModel::index(int Row, int Column, const QModelIndex& Parent) const
{
	if (Mode == ZED_OMM_TREE)
	{
		if (!Parent.isValid())
		{
			if (Row != 0)
				return QModelIndex();

			return (RootWrapper != NULL ? createIndex(0, Column, RootWrapper) : QModelIndex());
		}

		ZEDObjectWrapper* ParentWrapper = ConvertToWrapper(Parent);
		if (ParentWrapper == NULL)
			return QModelIndex();

		ZESize Index = 0;
		const ZEArray<ZEDObjectWrapper*>& ChildWrappers =  ParentWrapper->GetChildWrappers();
		for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
		{
			if (!FilterHierarchy(ChildWrappers[I]))
				continue;

			if (Row == Index)
				return createIndex(Row, Column, ChildWrappers[I]);

			Index++;
		}
	}
	else if (Mode == ZED_OMM_LIST)
	{		
		int Index = 0;
		ZEDObjectWrapper* Wrapper = indexList(RootWrapper, Row, Index);
		return  createIndex(Row, Column, Wrapper);
	}

	return QModelIndex();
}

QModelIndex ZEDObjectModel::parent(const QModelIndex& Child) const
{
	if (Mode == ZED_OMM_TREE)
	{
		if (!Child.isValid())
			return QModelIndex();

		ZEDObjectWrapper* ChildWrapper = ConvertToWrapper(Child);
		if (ChildWrapper == RootWrapper)
			return QModelIndex();

		ZEDObjectWrapper* ParentWrapper = ChildWrapper->GetParent();
		if (ParentWrapper == RootWrapper)
			return createIndex(0, 0, RootWrapper);

		int Index = 0;
		ZEDObjectWrapper* GrandParentWrapper = ParentWrapper->GetParent();
		const ZEArray<ZEDObjectWrapper*>& ChildWrappers = GrandParentWrapper->GetChildWrappers();
		for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
		{
			if (!FilterHierarchy(ChildWrappers[I]))
				continue;

			if (ChildWrappers[I] == ParentWrapper)
				return createIndex(Index, 0, ParentWrapper);

			Index++;
		}
	}

	return QModelIndex();
}

bool ZEDObjectModel::hasChildren(const QModelIndex& Parent) const
{
	if (Mode == ZED_OMM_TREE)
	{
		if (!Parent.isValid())
			return (RootWrapper != NULL);

		if (Parent.column() != 0)
			return false;

		ZEDObjectWrapper* ParentWrapper = ConvertToWrapper(Parent);
		if (ParentWrapper == NULL)
			return false;

		const ZEArray<ZEDObjectWrapper*>& ChildWrappers =  ParentWrapper->GetChildWrappers();
		for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
		{
			if (!FilterHierarchy(ChildWrappers[I]))
				continue;

			return true;
		}
	}
	else if (Mode == ZED_OMM_LIST)
	{
		return (!Parent.isValid());
	}
	
	return false;
}

int ZEDObjectModel::rowCount(const QModelIndex& Parent) const
{
	if (Mode == ZED_OMM_TREE)
	{
		if (!Parent.isValid())
			return (RootWrapper == NULL ? 0 : 1);

		if (Parent.column() != 0)
			return 0;

		ZEDObjectWrapper* ParentWrapper = ConvertToWrapper(Parent);
		if (ParentWrapper == NULL)
			return 0;

		int Count = 0;
		const ZEArray<ZEDObjectWrapper*>& ChildWrappers =  ParentWrapper->GetChildWrappers();
		for (ZESize I = 0; I < ChildWrappers.GetCount(); I++)
		{
			if (!FilterHierarchy(ChildWrappers[I]))
				continue;

			Count++;
		}

		return Count;
	}
	else if (Mode == ZED_OMM_LIST)
	{
		if (Parent.isValid())
			return 0;
		else
			return rowCountList(RootWrapper);
	}

	return 0;
}

int ZEDObjectModel::columnCount(const QModelIndex& Parent) const
{
	return 2;
}

QVariant ZEDObjectModel::data(const QModelIndex& Index, int Role) const
{
	ZEDObjectWrapper* Wrapper = ConvertToWrapper(Index);
	if (Wrapper == NULL)
		return QVariant();

	switch (Role)
	{
		case Qt::DisplayRole:
			if (Index.column() == 0)
				return Wrapper->GetName().ToCString();
			else if (Index.column() == 1)
				return Wrapper->GetObjectClass()->GetName();
			break;

		case Qt::TextAlignmentRole:
			if (Index.column() == 0)
				return (int)(Qt::AlignLeft | Qt::AlignVCenter);
			else if (Index.column() == 1)
				return (int)(Qt::AlignCenter | Qt::AlignVCenter);
			break;

		case Qt::DecorationRole:
			if (Index.column() == 0)
			{
				QString IconFile = ZEPathInfo(Wrapper->GetIconFileName()).GetRealPath().Path;
				return QIcon(IconFile);
			}
			break;
	}

	return QVariant();
}

QVariant ZEDObjectModel::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
	if (Orientation == Qt::Vertical)
		return QVariant();

	switch (Role)
	{
		case Qt::DisplayRole:
			if (Section == 0)
				return "Name";
			else if (Section == 1)
				return "Class";
			break;

		case Qt::TextAlignmentRole:
			if (Section == 0)
				return (int)(Qt::AlignLeft | Qt::AlignVCenter);
			else
				return (int)(Qt::AlignCenter | Qt::AlignVCenter);
			break;
	}

	return QVariant();
}

Qt::ItemFlags ZEDObjectModel::flags(const QModelIndex& Index) const
{
	ZEDObjectWrapper* Wrapper = ConvertToWrapper(Index);
	if (Wrapper == NULL)
		return QAbstractItemModel::flags(Index);

	Qt::ItemFlags Flags = QAbstractItemModel::flags(Index) | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled; 
	Flags &= (Wrapper->GetSelectable() && Wrapper->GetFrozen() ? ~Qt::ItemIsSelectable : ~0);
	return Flags;
}

QStringList ZEDObjectModel::mimeTypes() const
{
	QStringList List;
	List << "application/x.zinek.zeclass";
	List << "application/x.zinek.zeobjectwrapper";
	List << "application/x.zinek.zeasset";
	return List;
}

QMimeData* ZEDObjectModel::mimeData(const QModelIndexList& Indexes) const
{
	QByteArray Array;
	for (int I = 0; I < Indexes.count(); I++)
	{
		if (Indexes[I].column() != 0)
			continue;

		ZEDObjectWrapper* Wrapper = ConvertToWrapper(Indexes[I]);
		if (Wrapper == NULL)
			continue;
		
		Array.append((const char*)(&Wrapper), sizeof(ZEDObjectWrapper*));
	}

	QMimeData* Data = new QMimeData();
	Data->setData("application/x.zinek.zeobjectwrapper", Array);
	return Data;
}

bool ZEDObjectModel::canDropMimeData(const QMimeData* Data, Qt::DropAction Action, int Row, int Column, const QModelIndex& Parent) const
{
	return dropMimeDataInternal(Data, Action, Row, Column, Parent, false);
}

bool ZEDObjectModel::dropMimeData(const QMimeData* Data, Qt::DropAction Action,	int Row, int Column, const QModelIndex& Parent)
{
	return dropMimeDataInternal(Data, Action, Row, Column, Parent, true);
}

Qt::DropActions ZEDObjectModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

Qt::DropActions ZEDObjectModel::supportedDragActions() const
{
	return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

ZEDObjectModel::ZEDObjectModel()
{
	RootWrapper = NULL;
	Mode = ZED_OMM_TREE;
}
