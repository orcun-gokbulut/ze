//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDNodeIOPort.h
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

#pragma once
#ifndef _H_ZED_NODE_IO_PORT_H_
#define _H_ZED_NODE_IO_PORT_H_

#include <QtGui/QGraphicsPolygonItem>
#include "ZEDNodeEditorItem.h"

class ZEDNodeEditorGraphicsView;
class ZEDNodeEditorNode;
class ZEDIOPortConnection;
class ZEDIOPortConnectionPoint;

enum ZEDNodeIOPortType
{
	ZED_NIOPT_INPUT,
	ZED_NIOPT_OUTPUT,
	ZED_NIOPT_INPUTOUTPUT
};

class ZEDNodeIOPort : public QObject, public QGraphicsPolygonItem, public ZEDNodeEditorItem
{
	Q_OBJECT

	private:

		ZEDNodeEditorGraphicsView*		ParentView;
		ZEDNodeEditorNode*				ParentNode;
		ZEDIOPortConnectionPoint*		IOPortConnectionPointLeft;
		ZEDIOPortConnectionPoint*		IOPortConnectionPointRight;
		QMenu*							IOPortContextMenu;
		ZEDNodeIOPortType				Type;
		QList<ZEDIOPortConnection*>		Connections;	
		QGraphicsTextItem*				IOPortName;
		QString							PortName;
		QString							TypeName;	
		
	protected :

		void							hoverEnterEvent(QGraphicsSceneHoverEvent *Event);
		void							hoverLeaveEvent(QGraphicsSceneHoverEvent *Event);

	public:

		void							DisplayIOPortContextMenu(QPointF MousePos);

		void							SetSelected(bool IsSelected);

		void							Update();

		virtual	ZEDNodeEditorItemType	GetItemType();

		ZEDNodeIOPortType				GetType() const;
		QString							GetTypeName();

		void							SetParentNode(ZEDNodeEditorNode* ParentNode);
		ZEDNodeEditorNode*				GetParentNode() const;

		QGraphicsTextItem*				GetIOPortName();
		QString							GetName();
		
										ZEDNodeIOPort(ZEDNodeEditorNode* ParentNode, ZEDNodeIOPortType Type, QString Name, QString TypeName);

	signals:

		void							RemoveConnections();
};

#endif
