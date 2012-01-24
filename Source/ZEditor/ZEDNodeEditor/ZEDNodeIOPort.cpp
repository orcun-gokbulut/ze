//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDNodeIOPort.cpp
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

#include <QtGui/QPen>
#include <QtGui/QFont>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include "ZEDNodeEditorGraphicsView.h"
#include "ZEDNodeIOPort.h"
#include "ZEDNodeEditorNode.h"
#include "ZEDIOPortConnectionPoint.h"

#define POLYGON_HEIGHT 20.0f

void ZEDNodeIOPort::hoverEnterEvent(QGraphicsSceneHoverEvent *Event)
{	
	QGraphicsItem::hoverEnterEvent(Event);
}

void ZEDNodeIOPort::hoverLeaveEvent(QGraphicsSceneHoverEvent *Event)
{
	QGraphicsItem::hoverLeaveEvent(Event);
}

void ZEDNodeIOPort::DisplayIOPortContextMenu(QPointF MousePos)
{
	QAction* SelectedAction = IOPortContextMenu->exec(MousePos.toPoint());
}

void ZEDNodeIOPort::Update()
{
	QString Name = PortName;
	float NameLenghtTreshold = ParentNode->boundingRect().width() / 9;

	if (Name.length() > NameLenghtTreshold)
	{
		Name.truncate(NameLenghtTreshold - 3);
		Name.insert(NameLenghtTreshold - 3, QString("..."));
	}
	IOPortName->setPlainText(Name);	

	float IOPortNamePosX = ((ParentNode->polygon().boundingRect().width() - IOPortName->boundingRect().width()) / 2);
	IOPortName->setPos(IOPortNamePosX, POLYGON_HEIGHT - 20);

	setPolygon(QRectF(0,0,ParentNode->boundingRect().width(), POLYGON_HEIGHT));

	IOPortConnectionPointLeft->setPos(0, 5);
	IOPortConnectionPointRight->setPos(ParentNode->boundingRect().width() - 10, 5);
}

ZEDNodeEditorItemType ZEDNodeIOPort::GetItemType()
{
	return ZED_NEIT_IOPORT;
}

ZEDNodeIOPortType ZEDNodeIOPort::GetType() const
{
	return Type;
}

QString ZEDNodeIOPort::GetTypeName()
{
	return TypeName;
}

void ZEDNodeIOPort::SetParentNode(ZEDNodeEditorNode* ParentNode)
{
	this->ParentNode = ParentNode;
}

ZEDNodeEditorNode* ZEDNodeIOPort::GetParentNode() const
{
	return ParentNode;
}

QString	ZEDNodeIOPort::GetName()
{
	return PortName;
}

QGraphicsTextItem*	ZEDNodeIOPort::GetIOPortName()
{
	return IOPortName;
}

void ZEDNodeIOPort::SetSelected(bool IsSelected)
{
	if(IsSelected)
		setBrush(QBrush(Qt::yellow));
	else
		setBrush(QBrush(NULL));

	ZEDNodeEditorItem::SetSelected(IsSelected);
}

ZEDNodeIOPort::ZEDNodeIOPort(ZEDNodeEditorNode* ParentNode, ZEDNodeIOPortType Type, QString Name, QString TypeName) : QGraphicsPolygonItem (ParentNode) //Variable type will be added
{
	this->ParentView = ParentNode->ParentView;
	this->ParentNode = ParentNode;
	this->Type = Type;
	PortName = Name;
	this->TypeName = TypeName;

	IOPortContextMenu = new QMenu();
	QAction* RemoveIOPort = new QAction("Remove Connections", IOPortContextMenu);
	IOPortContextMenu->addAction(RemoveIOPort);

	QObject::connect(RemoveIOPort, SIGNAL(triggered()), ParentView, SLOT(DeleteConnections()));

	setToolTip(PortName + " :\n" + TypeName);

	float NameLenghtTreshold = ParentNode->boundingRect().width() / 9;

	if (Name.length() > NameLenghtTreshold)
	{
		Name.truncate(NameLenghtTreshold - 3);
		Name.insert(NameLenghtTreshold - 3, QString("..."));
	}

	QFont CourierFont;
	CourierFont.setFamily("Courier");
	IOPortName = new QGraphicsTextItem(this);
	IOPortName->setPlainText(Name);	
	IOPortName->setFont(CourierFont);

	setPolygon(QRectF(0,0,ParentNode->boundingRect().width(), POLYGON_HEIGHT));

	IOPortConnectionPointLeft = new ZEDIOPortConnectionPoint(this, ZED_IOPCPA_LEFT);	
	IOPortConnectionPointRight = new ZEDIOPortConnectionPoint(this, ZED_IOPCPA_RIGHT);	

	float IOPortNamePosX = ((polygon().boundingRect().width() - IOPortName->boundingRect().width()) / 2) + ParentNode->boundingRect().topLeft().x();
	IOPortName->setPos(IOPortNamePosX,pos().y());

	IOPortConnectionPointLeft->setPos(ParentNode->boundingRect().topLeft().x(), 5);
	IOPortConnectionPointRight->setPos(ParentNode->boundingRect().bottomRight().x()-10, 5);
}
