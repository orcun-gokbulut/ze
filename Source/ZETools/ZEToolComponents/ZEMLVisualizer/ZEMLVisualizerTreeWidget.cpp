//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLVisualizerTreeWidget.cpp
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

#include "ZEMLVisualizerTreeWidget.h"
#include "ZETypes.h"
#include "QtGui/QHeaderView"
#include "QtGui/QScrollBar"
#include "QtCore/qmimedata.h"
#include "ZEMLVisualizerWidget.h"
#include "ZEFile/ZEFileUtils.h"
#include "QtGui/Qevent.h"
#include "QtCore/QUrl"
#include "QtCore/QString"

ZEMLVisualizerTreeWidget::ZEMLVisualizerTreeWidget(QWidget* Parent) : QTreeWidget(Parent)
{
	QTreeWidgetItem* Header = headerItem();
	Header->setText(0, "Name");
	Header->setText(1, "Type");
	Header->setText(2, "Value");
	ParentWidget = Parent;
	setAcceptDrops(true);
	setDragDropMode(DragDrop);
}

void ZEMLVisualizerTreeWidget::drawRow(QPainter* Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const
{
	QTreeWidget::drawRow(Painter, Option, Index);

	for(ZEInt I = 0; I < columnCount(); I++)
	{
		QModelIndex CurrentIndex = Index.sibling(Index.row(), I);
		if(CurrentIndex.isValid())
		{
			if(I == 0)
			{
				QRect GridRect = visualRect(CurrentIndex);
				GridRect.setTopLeft(QPoint(0, GridRect.topLeft().y()));
				Painter->setPen(QColor(200,200,200));
				Painter->drawRect(GridRect);
			}

			else
			{
				QRect GridRect = visualRect(CurrentIndex);
				Painter->setPen(QColor(200,200,200));
				Painter->drawRect(GridRect);
			}
		} 
	}
}

void ZEMLVisualizerTreeWidget::dragEnterEvent(QDragEnterEvent* Event)
{
	if(Event->mimeData()->hasUrls())
		Event->acceptProposedAction();
}

void ZEMLVisualizerTreeWidget::dropEvent(QDropEvent* Event)
{
	if(!ZEFileUtils::IsFile((const char*)Event->mimeData()->urls()[0].path().toLatin1()))
		return;

	((ZEMLVisualizerWidget*)ParentWidget)->SetZEMLFile((const char*)Event->mimeData()->urls()[0].path().toLatin1());
}

bool ZEMLVisualizerTreeWidget::dropMimeData(QTreeWidgetItem* Parent, int Index, const QMimeData* Data, Qt::DropAction Action)
{
	return true;
}