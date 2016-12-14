//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDObjectSelectionModel.cpp
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

#include "ZEDObjectSelectionModel.h"

#include "ZEDCore/ZEDEditor.h"
#include "ZEDCore/ZEDSelectionManager.h"
#include "ZEDCore/ZEDSelectionEvent.h"
#include "QAbstractItemView"

QModelIndex ZEDObjectSelectionModel::FindIndex(const QModelIndex& Parent, ZEDObjectWrapper* Wrapper, int Column)
{
	int RowCount = model()->rowCount(Parent);
	for (ZESize I = 0; I < RowCount; I++)
	{
		QModelIndex Index = model()->index(I, Column, Parent);
		ZEDObjectWrapper* IndexWrapper = static_cast<ZEDObjectWrapper*>(Index.internalPointer());
		if (IndexWrapper == Wrapper)
			return Index;

		Index = FindIndex(Index, Wrapper, Column);
		if (Index.isValid())
			return Index;
	}

	return QModelIndex();
}

void ZEDObjectSelectionModel::SelectionEvent(const ZEDSelectionEvent* Event)
{
	if (model() == NULL)
		return;

	HandlingEvent = true;
	switch (Event->GetType())
	{
		case ZED_SET_SELECTED:
		{
			const ZEArray<ZEDObjectWrapper*>& SelectedList = Event->GetAddedlist();
			for (ZESize I = 0; I < SelectedList.GetCount(); I++)
			{
				if (SelectionBehavior == QAbstractItemView::SelectItems)
				{
					QModelIndex Index = FindIndex(QModelIndex(), SelectedList[I], 0);
					if (Index.isValid())
						select(Index, QItemSelectionModel::Deselect);
				}
				else if (SelectionBehavior == QAbstractItemView::SelectRows)
				{
					QModelIndex Parent = model()->parent(FindIndex(QModelIndex(), SelectedList[I], 0));
					int ColumnCount = model()->columnCount(Parent);
					for (int N = 0; N < ColumnCount; N++)
					{
						QModelIndex Index = FindIndex(Parent, SelectedList[I], N);
						if (Index.isValid())
							select(Index, QItemSelectionModel::Select);
					}
				}
			}
			break;
		}

		case ZED_SET_DESELECTED:
		{
			const ZEArray<ZEDObjectWrapper*>& DeselectedList = Event->GetRemovedlist();
			for (ZESize I = 0; I < DeselectedList.GetCount(); I++)
			{
				if (SelectionBehavior == QAbstractItemView::SelectItems)
				{
					QModelIndex Index = FindIndex(QModelIndex(), DeselectedList[I], 0);
					if (Index.isValid())
						select(Index, QItemSelectionModel::Deselect);
				}
				else if (SelectionBehavior == QAbstractItemView::SelectRows)
				{
					QModelIndex Parent = model()->parent(FindIndex(QModelIndex(), DeselectedList[I], 0));
					int ColumnCount = model()->columnCount(Parent);
					for (int N = 0; N < ColumnCount; N++)
					{
						QModelIndex Index = FindIndex(Parent, DeselectedList[I], N);
						if (Index.isValid())
							select(Index, QItemSelectionModel::Deselect);
					}
				}
			}
			break;
		}
	}
	HandlingEvent = false;
}

void ZEDObjectSelectionModel::UpdateSelection()
{
	if (GetEditor() == NULL || GetEditor()->GetSelectionManager() == NULL)
		return;

	clearSelection();
	const ZEArray<ZEDObjectWrapper*>& SelectedList = GetEditor()->GetSelectionManager()->GetSelection();
	for (ZESize I = 0; I < SelectedList.GetCount(); I++)
	{
		if (SelectionBehavior == QAbstractItemView::SelectItems)
		{
			QModelIndex Index = FindIndex(QModelIndex(), SelectedList[I], 0);
			if (Index.isValid())
				select(Index, QItemSelectionModel::Deselect);
		}
		else if (SelectionBehavior == QAbstractItemView::SelectRows)
		{
			QModelIndex Parent = model()->parent(FindIndex(QModelIndex(), SelectedList[I], 0));
			int ColumnCount = model()->columnCount(Parent);
			for (int N = 0; N < ColumnCount; N++)
			{
				QModelIndex Index = FindIndex(Parent, SelectedList[I], N);
				if (Index.isValid())
					select(Index, QItemSelectionModel::Select);
			}
		}
	}
}

void ZEDObjectSelectionModel::this_selectionChanged(const QItemSelection& Selected, const QItemSelection& Deselected)
{
	if (model() == NULL)
		return;

	if (HandlingEvent)
		return;

	ZEArray<ZEDObjectWrapper*> DeselectedWrappers;
	for (ZESize I = 0; I < Deselected.count(); I++)
	{
		QModelIndexList DeselectedIndexes = Deselected[I].indexes();
		for (ZESize N = 0; N < DeselectedIndexes.count(); N++)
		{
			ZEDObjectWrapper* Wrapper = static_cast<ZEDObjectWrapper*>(DeselectedIndexes[N].internalPointer());
			if (Wrapper == NULL)
				continue;

			if (DeselectedWrappers.Exists(Wrapper))
				continue;

			DeselectedWrappers.Add(Wrapper);
		}
	}

	GetEditor()->GetSelectionManager()->DeselectObjects(DeselectedWrappers);


	ZEArray<ZEDObjectWrapper*> SelectedWrappers;
	for (ZESize I = 0; I < Selected.count(); I++)
	{
		QModelIndexList SelectedIndexes = Selected[I].indexes();
		for (ZESize N = 0; N < SelectedIndexes.count(); N++)
		{
			ZEDObjectWrapper* Wrapper = static_cast<ZEDObjectWrapper*>(SelectedIndexes[N].internalPointer());
			if (Wrapper == NULL)
				continue;

			if (SelectedWrappers.Exists(Wrapper))
				continue;

			SelectedWrappers.Add(Wrapper);
		}
	}

	GetEditor()->GetSelectionManager()->SelectObjects(SelectedWrappers);
}

void ZEDObjectSelectionModel::setSelectionBehavior(QAbstractItemView::SelectionBehavior Behavior)
{
	SelectionBehavior = Behavior;
}

QAbstractItemView::SelectionBehavior ZEDObjectSelectionModel::selectionBehavior() const
{
	return SelectionBehavior;
}

ZEDObjectSelectionModel::ZEDObjectSelectionModel(QAbstractItemModel* Model) : QItemSelectionModel(Model)
{
	HandlingEvent = false;

	SelectionBehavior = QAbstractItemView::SelectItems;
	if (Model != NULL)
		UpdateSelection();

	connect(this, &QItemSelectionModel::selectionChanged, this, &ZEDObjectSelectionModel::this_selectionChanged);
}

ZEDObjectSelectionModel::ZEDObjectSelectionModel(QAbstractItemModel* Model, QObject* Parent) : QItemSelectionModel(Model, Parent)
{
	HandlingEvent = false;

	if (Model != NULL)
		UpdateSelection();

	connect(this, &QItemSelectionModel::selectionChanged, this, &ZEDObjectSelectionModel::this_selectionChanged);
}
