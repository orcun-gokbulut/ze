//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDNodeEditorNode.cpp
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

#include <QPen>
#include <QBrush>
#include <QString>
#include <QDebug>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QFont>
#include <QMenu>
#include <QAction>
#include <QKeyEvent>
#include <QStringList>

#include "ZEDNodeEditorNode.h"
#include "ZEDNodeIOPort.h"
#include "ZEDNodeEditorNodeScaleGizmo.h"
#include "ZEDNodeEditorNodeScaleGizmoPoint.h"
#include "ZEDNodeConnectionPoint.h"
#include "ZEDNodeEditorGraphicsView.h"

#define SPACE_BETWEEN_NODE_ITEMS		20
#define PIXMAP_DIMENSION				128
#define SCALE_GIZMO_DIMENSION			6
#define NODE_CONNECTION_POINT_DIMENSION 30


void ZEDNodeEditorNode::UpdatePreview()
{

}

void ZEDNodeEditorNode::hoverEnterEvent(QGraphicsSceneHoverEvent *Event)
{
	QGraphicsItem::hoverEnterEvent(Event);
}
void ZEDNodeEditorNode::hoverLeaveEvent(QGraphicsSceneHoverEvent *Event)
{
	QGraphicsItem::hoverLeaveEvent(Event);
}

void ZEDNodeEditorNode::SetNodeOldRectangle(QRectF OldRectangle)
{
	OldNodePolygon = OldRectangle;
}

QRectF ZEDNodeEditorNode::GetNodeOldRectangle()
{
	return OldNodePolygon;
}

void ZEDNodeEditorNode::SetNodeOldPosition(QPointF OldPosition)
{
	OldNodePosition = OldPosition;
}

QPointF ZEDNodeEditorNode::GetNodeOldPosition()
{
	return OldNodePosition;
}
void ZEDNodeEditorNode::AddCustomNodeContextMenuItems(QList<QAction*> ContextMenuItems)
{
	for (int I = 0; I < ContextMenuItems.count(); I++)
	{
		ContextMenuItems[I] = new QAction(ContextMenuItems[I]->text(), NodeContextMenu);
		NodeContextMenu->addAction(ContextMenuItems[I]);
		NodeContextMenuItems.append(ContextMenuItems[I]);
	}
}

QList<QAction*>* ZEDNodeEditorNode::GetCustomNodeContextMenuItems()
{
	return &NodeContextMenuItems;
}
void ZEDNodeEditorNode::DisplayNodeContextMenu(QPointF MousePos)
{
	ParentView->IsMultipleSelectionEnabled = false;
	ParentView->IsDeselectionEnabled = false;
	QAction *selectedAction = NodeContextMenu->exec(MousePos.toPoint());
}
ZEDNodeEditorItemType ZEDNodeEditorNode::GetItemType()
{
	return ZED_NEIT_NODE;
}

void ZEDNodeEditorNode::SetSelected(bool IsSelected)
{
	this->IsSelected = IsSelected;

	if(IsSelected)
	{
		setBrush(QBrush(QColor(200,200,200,255)));
		NodeScaleGizmo->setVisible(true);
	}
	else
	{
		setBrush(QBrush(QColor(200,200,200,255)));
		NodeScaleGizmo->setVisible(false);		
	}

	ZEDNodeEditorItem::SetSelected(IsSelected);
}

ZEDIOPortOrder ZEDNodeEditorNode::GetIOPortOrder()
{
	return IOPortOrder;
}
void ZEDNodeEditorNode::SetScaleGizmo(ZEDNodeEditorNodeScaleGizmo* Gizmo)
{
	NodeScaleGizmo = Gizmo;
}
ZEDNodeEditorNodeScaleGizmo* ZEDNodeEditorNode::GetScaleGizmo()
{
	return NodeScaleGizmo;
}
ZEDNodeConnectionPoint* ZEDNodeEditorNode::GetConnectionPoint()
{
	return NodeConnectionPoint;
}
QGraphicsPolygonItem* ZEDNodeEditorNode::GetScaleRect()
{
	return ScaleRectItem;
}
void ZEDNodeEditorNode::ScaleByGizmo()
{
	setPolygon(ScaleRectItem->polygon());
		
	SetNameText(NodeNameText);
	SetTypeText(NodeTypeText);
	
	NodeConnectionPoint->setPos(polygon().boundingRect().topLeft().x() + (polygon().boundingRect().width() - NODE_CONNECTION_POINT_DIMENSION) / 2 , polygon().boundingRect().topLeft().y());
	PreviewItem->setPos(polygon().boundingRect().topLeft().x() + (polygon().boundingRect().width() - PIXMAP_DIMENSION) / 2,ScaleRectItem->boundingRect().y() + SPACE_BETWEEN_NODE_ITEMS * 2 + NODE_CONNECTION_POINT_DIMENSION / 2);

	float NewY = polygon().boundingRect().bottomRight().y() - SPACE_BETWEEN_NODE_ITEMS / 2 ;
	
	for (int I = IOPorts.count() - 1; I >= 0; I--)
	{
		NewY = NewY - SPACE_BETWEEN_NODE_ITEMS;
		
		IOPorts[I]->setPos(polygon().boundingRect().topLeft().x(), NewY);
		IOPorts[I]->Update();
	}
}
void ZEDNodeEditorNode::ScaleRect(QPointF Amount, ZEDNodeEditorNodeScaleGizmoPointPosition GizmoNodePosition)
{
	if (ParentView->GetIsDeselectionEnabled() == false)
	{
		QRectF TempPolygon;
		int MinHeight;
		if (GizmoNodePosition == ZED_NENSGPP_BOTTOM_RIGHT)
		{
			TempPolygon.setRect(ScaleRectItem->boundingRect().topLeft().x(), ScaleRectItem->boundingRect().topLeft().y(), ScaleRectItem->boundingRect().width() - Amount.x(), ScaleRectItem->boundingRect().height() - Amount.y());

			if(TempPolygon.width() >= PIXMAP_DIMENSION + 10)
			{
				ScaleRectItem->setPolygon(QRectF(ScaleRectItem->boundingRect().topLeft().x(), ScaleRectItem->boundingRect().topLeft().y(), ScaleRectItem->boundingRect().width() - Amount.x(), ScaleRectItem->boundingRect().height()));
				NodeScaleGizmo->Update();
			}
			if (IOPorts.count() > 0 && PreviewItem->isVisible() == true)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + PIXMAP_DIMENSION + 20 + (IOPorts.count()) * SPACE_BETWEEN_NODE_ITEMS + NODE_CONNECTION_POINT_DIMENSION / 2;

			else if (IOPorts.count() > 0 && PreviewItem->isVisible() == false)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + 10 + (IOPorts.count()) * SPACE_BETWEEN_NODE_ITEMS + NODE_CONNECTION_POINT_DIMENSION / 2;

			else if (IOPorts.count() == 0 && PreviewItem->isVisible() == true)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + PIXMAP_DIMENSION + 10 + NODE_CONNECTION_POINT_DIMENSION / 2;

			else if (IOPorts.count() == 0 && PreviewItem->isVisible() == false)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2  + NODE_CONNECTION_POINT_DIMENSION / 2;

			if (TempPolygon.height() >= MinHeight)
			{
				ScaleRectItem->setPolygon(QRectF(ScaleRectItem->boundingRect().topLeft().x(), ScaleRectItem->boundingRect().topLeft().y(), ScaleRectItem->boundingRect().width(), ScaleRectItem->boundingRect().height() - Amount.y()));
				NodeScaleGizmo->Update();
			}
		}
		else if (GizmoNodePosition == ZED_NENSGPP_TOP_LEFT)
		{
			TempPolygon.setRect(ScaleRectItem->boundingRect().topLeft().x() - Amount.x(), ScaleRectItem->boundingRect().topLeft().y() - Amount.y(), ScaleRectItem->boundingRect().width() + Amount.x(), ScaleRectItem->boundingRect().height() + Amount.y());

			if(TempPolygon.width() >= PIXMAP_DIMENSION + 10)
			{
				ScaleRectItem->setPolygon(QRectF(ScaleRectItem->boundingRect().topLeft().x() - Amount.x(), ScaleRectItem->boundingRect().topLeft().y(), ScaleRectItem->boundingRect().width() + Amount.x(), ScaleRectItem->boundingRect().height()));
				NodeScaleGizmo->Update();
			}
			if (IOPorts.count() > 0 && PreviewItem->isVisible() == true)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + PIXMAP_DIMENSION + 20 + (IOPorts.count()) * SPACE_BETWEEN_NODE_ITEMS + NODE_CONNECTION_POINT_DIMENSION / 2;

			else if (IOPorts.count() > 0 && PreviewItem->isVisible() == false)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + 10 + (IOPorts.count()) * SPACE_BETWEEN_NODE_ITEMS + NODE_CONNECTION_POINT_DIMENSION / 2;

			else if (IOPorts.count() == 0 && PreviewItem->isVisible() == true)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + PIXMAP_DIMENSION + 10 + NODE_CONNECTION_POINT_DIMENSION / 2;

			else if (IOPorts.count() == 0 && PreviewItem->isVisible() == false)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + NODE_CONNECTION_POINT_DIMENSION / 2;

			if (TempPolygon.height() >= MinHeight)
			{
				ScaleRectItem->setPolygon(QRectF(ScaleRectItem->boundingRect().topLeft().x(), ScaleRectItem->boundingRect().topLeft().y() - Amount.y(), ScaleRectItem->boundingRect().width(), ScaleRectItem->boundingRect().height() + Amount.y()));
				NodeScaleGizmo->Update();
			}
		}
		else if (GizmoNodePosition == ZED_NENSGPP_TOP)
		{
			TempPolygon.setRect(ScaleRectItem->boundingRect().topLeft().x(), ScaleRectItem->boundingRect().topLeft().y() - Amount.y(), ScaleRectItem->boundingRect().width(), ScaleRectItem->boundingRect().height() + Amount.y());

			if (IOPorts.count() > 0 && PreviewItem->isVisible() == true)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + PIXMAP_DIMENSION + 20 + (IOPorts.count()) * SPACE_BETWEEN_NODE_ITEMS + NODE_CONNECTION_POINT_DIMENSION / 2;

			else if (IOPorts.count() > 0 && PreviewItem->isVisible() == false)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + 10 + (IOPorts.count()) * SPACE_BETWEEN_NODE_ITEMS + NODE_CONNECTION_POINT_DIMENSION / 2;

			else if (IOPorts.count() == 0 && PreviewItem->isVisible() == true)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + PIXMAP_DIMENSION + 10 + NODE_CONNECTION_POINT_DIMENSION / 2;

			else if (IOPorts.count() == 0 && PreviewItem->isVisible() == false)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + NODE_CONNECTION_POINT_DIMENSION / 2;

			if (TempPolygon.height() >= MinHeight)
			{
				ScaleRectItem->setPolygon(QRectF(ScaleRectItem->boundingRect().topLeft().x(), ScaleRectItem->boundingRect().topLeft().y() - Amount.y(), ScaleRectItem->boundingRect().width(), ScaleRectItem->boundingRect().height() + Amount.y()));
				NodeScaleGizmo->Update();
			}

		}
		else if (GizmoNodePosition == ZED_NENSGPP_TOP_RIGHT)
		{
			TempPolygon.setRect(ScaleRectItem->boundingRect().topLeft().x() , ScaleRectItem->boundingRect().topLeft().y() - Amount.y(), ScaleRectItem->boundingRect().width() - Amount.x(), ScaleRectItem->boundingRect().height() + Amount.y());

			if(TempPolygon.width() >= PIXMAP_DIMENSION + 10)
			{
				ScaleRectItem->setPolygon(QRectF(ScaleRectItem->boundingRect().topLeft().x(), ScaleRectItem->boundingRect().topLeft().y(), ScaleRectItem->boundingRect().width() - Amount.x(), ScaleRectItem->boundingRect().height()));
				NodeScaleGizmo->Update();
			}
			if (IOPorts.count() > 0 && PreviewItem->isVisible() == true)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + PIXMAP_DIMENSION + 20 + (IOPorts.count()) * SPACE_BETWEEN_NODE_ITEMS + NODE_CONNECTION_POINT_DIMENSION / 2;

			else if (IOPorts.count() > 0 && PreviewItem->isVisible() == false)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + 10 + (IOPorts.count()) * SPACE_BETWEEN_NODE_ITEMS + NODE_CONNECTION_POINT_DIMENSION / 2;

			else if (IOPorts.count() == 0 && PreviewItem->isVisible() == true)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + PIXMAP_DIMENSION + 10 + NODE_CONNECTION_POINT_DIMENSION / 2;

			else if (IOPorts.count() == 0 && PreviewItem->isVisible() == false)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + NODE_CONNECTION_POINT_DIMENSION / 2;

			if (TempPolygon.height() >= MinHeight)
			{
				ScaleRectItem->setPolygon(QRectF(ScaleRectItem->boundingRect().topLeft().x(), ScaleRectItem->boundingRect().topLeft().y() - Amount.y(), ScaleRectItem->boundingRect().width(), ScaleRectItem->boundingRect().height() + Amount.y()));
				NodeScaleGizmo->Update();
			}

		}
		else if (GizmoNodePosition == ZED_NENSGPP_BOTTOM_LEFT)
		{
			TempPolygon.setRect(ScaleRectItem->boundingRect().topLeft().x() - Amount.x(), ScaleRectItem->boundingRect().topLeft().y(), ScaleRectItem->boundingRect().width() + Amount.x(), ScaleRectItem->boundingRect().height() - Amount.y());

			if(TempPolygon.width() >= PIXMAP_DIMENSION + 10)
			{
				ScaleRectItem->setPolygon(QRectF(ScaleRectItem->boundingRect().topLeft().x() - Amount.x(), ScaleRectItem->boundingRect().topLeft().y(), ScaleRectItem->boundingRect().width() + Amount.x(), ScaleRectItem->boundingRect().height()));
				NodeScaleGizmo->Update();
			}
			if (IOPorts.count() > 0 && PreviewItem->isVisible() == true)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + PIXMAP_DIMENSION + 20 + (IOPorts.count()) * SPACE_BETWEEN_NODE_ITEMS + NODE_CONNECTION_POINT_DIMENSION / 2;

			else if (IOPorts.count() > 0 && PreviewItem->isVisible() == false)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + 10 + (IOPorts.count()) * SPACE_BETWEEN_NODE_ITEMS + NODE_CONNECTION_POINT_DIMENSION / 2;

			else if (IOPorts.count() == 0 && PreviewItem->isVisible() == true)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + PIXMAP_DIMENSION + 10 + NODE_CONNECTION_POINT_DIMENSION / 2;

			else if (IOPorts.count() == 0 && PreviewItem->isVisible() == false)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + NODE_CONNECTION_POINT_DIMENSION / 2;

			if (TempPolygon.height() >= MinHeight)
			{
				ScaleRectItem->setPolygon(QRectF(ScaleRectItem->boundingRect().topLeft().x(), ScaleRectItem->boundingRect().topLeft().y(), ScaleRectItem->boundingRect().width(), ScaleRectItem->boundingRect().height() - Amount.y()));
				NodeScaleGizmo->Update();
			}

		}
		else if (GizmoNodePosition == ZED_NENSGPP_BOTTOM)
		{
			TempPolygon.setRect(ScaleRectItem->boundingRect().topLeft().x(), ScaleRectItem->boundingRect().topLeft().y(), ScaleRectItem->boundingRect().width(), ScaleRectItem->boundingRect().height() - Amount.y());

			if (IOPorts.count() > 0 && PreviewItem->isVisible() == true)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + PIXMAP_DIMENSION + 20 + (IOPorts.count()) * SPACE_BETWEEN_NODE_ITEMS + NODE_CONNECTION_POINT_DIMENSION / 2;

			else if (IOPorts.count() > 0 && PreviewItem->isVisible() == false)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + 10 + (IOPorts.count()) * SPACE_BETWEEN_NODE_ITEMS + NODE_CONNECTION_POINT_DIMENSION / 2;

			else if (IOPorts.count() == 0 && PreviewItem->isVisible() == true)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + PIXMAP_DIMENSION + 10 + NODE_CONNECTION_POINT_DIMENSION / 2;

			else if (IOPorts.count() == 0 && PreviewItem->isVisible() == false)
				MinHeight = SPACE_BETWEEN_NODE_ITEMS * 2 + NODE_CONNECTION_POINT_DIMENSION / 2;

			if (TempPolygon.height() >= MinHeight)
			{
				ScaleRectItem->setPolygon(QRectF(ScaleRectItem->boundingRect().topLeft().x(), ScaleRectItem->boundingRect().topLeft().y(), ScaleRectItem->boundingRect().width(), ScaleRectItem->boundingRect().height() - Amount.y()));
				NodeScaleGizmo->Update();
			}


		}
		else if (GizmoNodePosition == ZED_NENSGPP_RIGHT)
		{
			TempPolygon.setRect(ScaleRectItem->boundingRect().topLeft().x() , ScaleRectItem->boundingRect().topLeft().y(), ScaleRectItem->boundingRect().width() - Amount.x(), ScaleRectItem->boundingRect().height());

			if(TempPolygon.width() >= PIXMAP_DIMENSION + 10)
			{
				ScaleRectItem->setPolygon(QRectF(ScaleRectItem->boundingRect().topLeft().x(), ScaleRectItem->boundingRect().topLeft().y(), ScaleRectItem->boundingRect().width() - Amount.x(), ScaleRectItem->boundingRect().height()));
				NodeScaleGizmo->Update();
			}
		}
		else if (GizmoNodePosition == ZED_NENSGPP_LEFT)
		{
			TempPolygon.setRect(ScaleRectItem->boundingRect().topLeft().x() - Amount.x(), ScaleRectItem->boundingRect().topLeft().y(), ScaleRectItem->boundingRect().width() + Amount.x(), ScaleRectItem->boundingRect().height());

			if(TempPolygon.width() >= PIXMAP_DIMENSION + 10)
			{
				ScaleRectItem->setPolygon(QRectF(ScaleRectItem->boundingRect().topLeft().x() - Amount.x(), ScaleRectItem->boundingRect().topLeft().y(), ScaleRectItem->boundingRect().width() + Amount.x(), ScaleRectItem->boundingRect().height()));
				NodeScaleGizmo->Update();
			}
		}
	}
}
void ZEDNodeEditorNode::Update()
{
	int a = LastPortPosition.y() + (SPACE_BETWEEN_NODE_ITEMS * IOPorts.count());
	setPolygon(QRectF(0, 0, PIXMAP_DIMENSION + 10, a + SPACE_BETWEEN_NODE_ITEMS / 2));
	ScaleRectItem->setPolygon(QRectF(polygon().boundingRect().topLeft().x(), polygon().boundingRect().topLeft().y(), polygon().boundingRect().width(), polygon().boundingRect().height()));

}

void ZEDNodeEditorNode::SetUserData(void* UserData)
{

}
void* ZEDNodeEditorNode::GetUserData() const
{
	return UserData;
}

void ZEDNodeEditorNode::SetNameText(QString NameText)
{
	NodeNameText = NameText;
	float NameLenghtTreshold = polygon().boundingRect().width() / 8;

	if (NameText.length() > NameLenghtTreshold)
	{
		NameText.truncate(NameLenghtTreshold - 3);
		NameText.insert(NameLenghtTreshold - 3, QString("..."));
	}
	NameTextItem->setPlainText(NameText);

	float TextItemPosX = ((polygon().boundingRect().width() - NameTextItem->boundingRect().width()) / 2) + polygon().boundingRect().topLeft().x();
	NameTextItem->setPos(TextItemPosX, polygon().boundingRect().topLeft().y() + + NODE_CONNECTION_POINT_DIMENSION / 2);
}
QString ZEDNodeEditorNode::GetNameText() const
{
	if(NameTextItem != NULL)
		return NodeNameText;
	else
		return QString();
}

void ZEDNodeEditorNode::SetTypeText(QString TypeText)
{
	NodeTypeText = TypeText;
	float NameLenghtTreshold = polygon().boundingRect().width() / 8;

	if (TypeText.length() > NameLenghtTreshold)
	{
		TypeText.truncate(NameLenghtTreshold - 3);
		TypeText.insert(NameLenghtTreshold - 3, QString("..."));
	}

	TypeTextItem->setPlainText(TypeText);

	float TextItemPosX = ((polygon().boundingRect().width() - TypeTextItem->boundingRect().width()) / 2) + polygon().boundingRect().topLeft().x();
	TypeTextItem->setPos(TextItemPosX,( polygon().boundingRect().topLeft().y() + SPACE_BETWEEN_NODE_ITEMS + NODE_CONNECTION_POINT_DIMENSION / 2));
}
QString	ZEDNodeEditorNode::GetTypeText() const
{
	if(TypeTextItem != NULL)
		return NodeTypeText;
	else
		return QString();
}

void ZEDNodeEditorNode::SetPreviewPixmap(QByteArray BitmapData)
{
	QPixmap Pixmap;	
	Pixmap.loadFromData(BitmapData);

	if (PreviewPixmapVisibility)
	{
		PreviewItem->setPixmap(Pixmap.scaled(PIXMAP_DIMENSION, PIXMAP_DIMENSION, Qt::KeepAspectRatio, Qt::FastTransformation));
	}
}
QPixmap	ZEDNodeEditorNode::GetPreviewPixmap()
{
	if(PreviewItem != NULL)
		return PreviewItem->pixmap();
	else
		return QPixmap();
}

void ZEDNodeEditorNode::SetPreviewPixmapVisibility(bool IsVisible)
{	
	if (this->PreviewPixmapVisibility != IsVisible)
	{
		this->PreviewPixmapVisibility = IsVisible;
		UpdateIOPortsOnPixmapVisibilityChange();
		PreviewItem->setVisible(IsVisible);
	}
}

bool ZEDNodeEditorNode::GetPreviewPixmapVisibility()
{
	return PreviewPixmapVisibility;
}
void ZEDNodeEditorNode::UpdateIOPortsOnPixmapVisibilityChange()
{
	if (PreviewPixmapVisibility == true)
	{
		for (int I = 0; I < IOPorts.count(); I++ )
		{
			IOPorts[I]->setPos(IOPorts[I]->pos().x(), (IOPorts[I]->pos().y() + (PIXMAP_DIMENSION + 10)));
		}
		LastPortPosition.setY(LastPortPosition.y() + (PIXMAP_DIMENSION + 10));
	}
	else if(PreviewPixmapVisibility == false)
	{
		for (int I = 0; I < IOPorts.count(); I++ )
		{
			IOPorts[I]->setPos(IOPorts[I]->pos().x(), (IOPorts[I]->pos().y() - (PIXMAP_DIMENSION )));
		}
		LastPortPosition.setY(LastPortPosition.y() - (PIXMAP_DIMENSION));
	}

	Update();
}

QList<ZEDNodeIOPort*>*	ZEDNodeEditorNode::GetIOPorts()
{
	return &IOPorts;
}
void ZEDNodeEditorNode::ReArrangeIOPorts(QList<ZEDNodeIOPort*> IO)
{
	IOPorts = IO;

	if (IOPortOrder == ZED_IOPA_IO_TYPE_ORDER)
	{
		QList<ZEDNodeIOPort*> IOInputs;
		QList<ZEDNodeIOPort*> IOOutputs;
		QList<ZEDNodeIOPort*> IOInputOutputs;

		for(int I=0; I < IOPorts.count(); I++)
		{
			if(IOPorts[I]->GetType() == ZED_NIOPT_INPUT)
			{
				IOInputs.append(IOPorts[I]);
			}
			else if(IOPorts[I]->GetType() == ZED_NIOPT_OUTPUT)
			{
				IOOutputs.append(IOPorts[I]);
			}
			else if(IOPorts[I]->GetType() == ZED_NIOPT_INPUTOUTPUT)
			{
				IOInputOutputs.append(IOPorts[I]);
			}
		}	
		for(int K = 0; K < IOInputs.count(); K++)
		{
			for ( int I = 0; I < IOInputs.count() - 1 - K; I++ )
				if ( QString::compare(IOInputs[I]->GetName(), IOInputs[I+1]->GetName(), Qt::CaseInsensitive) > 0)
				{
					IOInputs.swap(I, I+1);
				}
		}
		for(int K = 0; K < IOOutputs.count(); K++)
		{
			for ( int I = 0; I < IOOutputs.count() - 1 - K; I++ )
				if ( QString::compare(IOOutputs[I]->GetName(), IOOutputs[I+1]->GetName(), Qt::CaseInsensitive) > 0)
				{
					IOOutputs.swap(I, I+1);
				}
		}
		for(int K = 0; K < IOInputOutputs.count(); K++)
		{
			for ( int I = 0; I < IOInputOutputs.count() - 1 - K; I++ )
				if ( QString::compare(IOInputOutputs[I]->GetName(), IOInputOutputs[I+1]->GetName(), Qt::CaseInsensitive) > 0)
				{
					IOInputOutputs.swap(I, I+1);
				}
		}
		IOPorts.clear();
		IOPorts.append(IOInputs);
		IOPorts.append(IOOutputs);
		IOPorts.append(IOInputOutputs);

	}	

	else if (IOPortOrder == ZED_IOPA_ALPABETICAL_ORDER)
	{
		for(int K = 0; K < IOPorts.count(); K++)
		{
			for ( int I = 0; I < IOPorts.count() - 1 - K; I++ )
				if ( QString::compare(IOPorts[I]->GetName(), IOPorts[I+1]->GetName(), Qt::CaseInsensitive) > 0)
				{
					IOPorts.swap(I, I+1);
				}
		}
	}


	for (int I = 0; I < IOPorts.count(); I++)
		IOPorts[I]->setPos(LastPortPosition.x() , (LastPortPosition.y() + (SPACE_BETWEEN_NODE_ITEMS * I)));	

	Update();
}
void ZEDNodeEditorNode::AddIOPort(ZEDNodeIOPort* IOPort)
{
	IOPorts.append(IOPort);		
	ReArrangeIOPorts(IOPorts);
}

ZEDNodeEditorNode::ZEDNodeEditorNode(QGraphicsItem* Parent, ZEDNodeEditorGraphicsView* ParentView, ZEDIOPortOrder Order) : QGraphicsPolygonItem(Parent)
{
	IOPortOrder = Order;
	setPolygon(QRectF(0,0,PIXMAP_DIMENSION +10 ,SPACE_BETWEEN_NODE_ITEMS * 2 + NODE_CONNECTION_POINT_DIMENSION / 2));

	PreviewPixmapVisibility = false;
	TypeTextItem = new QGraphicsTextItem("", this);
	NameTextItem = new QGraphicsTextItem("", this);
	NodeScaleGizmo = new ZEDNodeEditorNodeScaleGizmo(this);
	ScaleRectItem = new QGraphicsPolygonItem(this);
	NodeConnectionPoint = new ZEDNodeConnectionPoint(this);	

	OldNodePosition = QPointF(0,0);
	OldNodePolygon = QRectF(0,0,0,0);


	NodeContextMenu = new QMenu();
	QAction *RemoveNode = new QAction("Remove Node", NodeContextMenu);
	QAction *SendBackwardNode = new QAction("Send Backward", NodeContextMenu);
	QAction *BringForwardNode = new QAction("Bring Forward", NodeContextMenu);

	NodeContextMenu->addAction(RemoveNode);
	NodeContextMenu->addSeparator();
	NodeContextMenu->addAction(SendBackwardNode);
	NodeContextMenu->addAction(BringForwardNode);

	QObject::connect(RemoveNode, SIGNAL(triggered()), ParentView, SLOT(DeleteNodes()));
	QObject::connect(SendBackwardNode, SIGNAL(triggered()), ParentView, SLOT(SendBackward()));
	QObject::connect(BringForwardNode, SIGNAL(triggered()), ParentView, SLOT(BringForward()));

	NodeScaleGizmo->setVisible(false);

	QFont CourierFont;
	CourierFont.setFamily("Courier");

	NameTextItem->setFont(CourierFont);
	TypeTextItem->setFont(CourierFont);

	this->ParentView = ParentView;	

	PreviewItem = new QGraphicsPixmapItem(QPixmap(PIXMAP_DIMENSION,PIXMAP_DIMENSION),this);
	PreviewItem->setVisible(PreviewPixmapVisibility);

	PreviewItem->setPos(SPACE_BETWEEN_NODE_ITEMS / 4,SPACE_BETWEEN_NODE_ITEMS * 2 + NODE_CONNECTION_POINT_DIMENSION / 2);

	QPen BlackPen(Qt::black);
	QBrush GrayBrush(QColor(200,200,200,255));


	setPen(BlackPen);
	setBrush(GrayBrush);
	NodeNameText = "";
	NodeTypeText = "";
	setToolTip(NodeNameText + " :\n" + NodeTypeText);

	LastPortPosition.setX(polygon().boundingRect().topLeft().x());
	LastPortPosition.setY(polygon().boundingRect().topLeft().y() + SPACE_BETWEEN_NODE_ITEMS * 2 + NODE_CONNECTION_POINT_DIMENSION / 2);

	ScaleRectItem->setPen(BlackPen);
	ScaleRectItem->setPolygon(QRectF(polygon().boundingRect().topLeft().x(), polygon().boundingRect().topLeft().y(), polygon().boundingRect().width(), polygon().boundingRect().height()));


	NodeConnectionPoint->setPos(polygon().boundingRect().topLeft().x() + (polygon().boundingRect().width() - NODE_CONNECTION_POINT_DIMENSION) / 2 , polygon().boundingRect().topLeft().y());

	IsSelected = false;

	setCursor(Qt::ArrowCursor);
}
ZEDNodeEditorNode::~ZEDNodeEditorNode()
{
	
}
