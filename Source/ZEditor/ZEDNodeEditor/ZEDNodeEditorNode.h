//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDNodeEditorNode.h
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
#ifndef _H_ZED_NODE_EDITOR_NODE_H_
#define _H_ZED_NODE_EDITOR_NODE_H_

#include <QGraphicsPolygonItem>
#include <QAction>
#include "ZEDNodeEditorItem.h"

class ZEDNodeEditorGraphicsView;
class ZEDNodeIOPort;
class ZEDNodeEditorNodeScaleGizmo;
enum ZEDNodeEditorNodeScaleGizmoPointPosition;

enum ZEDIOPortOrder
{
	ZED_IOPA_IO_TYPE_ORDER,
	ZED_IOPA_ALPABETICAL_ORDER
};

class ZEDNodeEditorNode : public QGraphicsPolygonItem, public ZEDNodeEditorItem
{
		friend class ZEDNodeConnectionPoint;
		friend class ZEDNodeEditorNodeScaleGizmoPoint;
		friend class ZEDNodeEditorNodeScaleGizmo;
		friend class ZEDNodeEditorGraphicsView;
		friend class ZEDNodeIOPort;
		friend class ZEDNodeEditorDeleteNodeOperation;
		friend class ZEDNodeEditorResizeNodeOperation;

	private:

		ZEDIOPortOrder					IOPortOrder;
		QList<ZEDNodeIOPort*>			IOPorts;
		ZEDNodeEditorGraphicsView*		ParentView;
		void*							UserData;		

		QMenu*							NodeContextMenu;

		QGraphicsTextItem*				TypeTextItem;
		QGraphicsTextItem*				NameTextItem;
		QGraphicsPixmapItem*			PreviewItem;
		QGraphicsPolygonItem*			ScaleRectItem;

		ZEDNodeConnectionPoint*			NodeConnectionPoint;
		ZEDNodeEditorNodeScaleGizmo*	NodeScaleGizmo;

		QList<QAction*>					NodeContextMenuItems;
		
		bool							PreviewPixmapVisibility;
		bool							IsSelected;

		QString							NodeNameText;
		QString							NodeTypeText;
		
		QPointF							LastPortPosition;
		QPointF							InitialMouseScenePos;
		QPointF							LastMouseScenePos;
		QPointF							OldNodePosition;

		QRectF							OldNodePolygon;

		void							ScaleByGizmo();
		void							ScaleRect(QPointF Amount, ZEDNodeEditorNodeScaleGizmoPointPosition GizmoNodePosition);

		void							UpdateIOPortsOnPixmapVisibilityChange();

	protected:

		void							UpdatePreview();

		void							hoverEnterEvent(QGraphicsSceneHoverEvent *Event);
		void							hoverLeaveEvent(QGraphicsSceneHoverEvent *Event);

	public slots:

		void							ReArrangeIOPorts(QList<ZEDNodeIOPort*> IO);

	public:


		void							SetNodeOldRectangle(QRectF OldPolygon = QRectF(0,0,0,0));
		QRectF							GetNodeOldRectangle();

		void							SetNodeOldPosition(QPointF OldPosition = QPointF(0,0));
		QPointF							GetNodeOldPosition();

		void							AddCustomNodeContextMenuItems(QList<QAction*> ContextMenuItems);
		QList<QAction*>*				GetCustomNodeContextMenuItems();

		void							DisplayNodeContextMenu(QPointF MousePos);

		ZEDNodeConnectionPoint*			GetConnectionPoint();

		QGraphicsPolygonItem*			GetScaleRect();

		ZEDIOPortOrder					GetIOPortOrder();
		
		void							SetScaleGizmo(ZEDNodeEditorNodeScaleGizmo* Gizmo);
		ZEDNodeEditorNodeScaleGizmo*	GetScaleGizmo();

		virtual void					SetSelected(bool IsSelected);

		virtual	ZEDNodeEditorItemType	GetItemType();
		void							Update();

		void							SetUserData(void* UserData);
		void*							GetUserData() const;

		void							SetNameText(QString NameText);
		QString							GetNameText() const;

		void							SetTypeText(QString TypeText);
		QString							GetTypeText() const;

		void							SetPreviewPixmap(QByteArray BitmapData);
		QPixmap							GetPreviewPixmap();

		void							SetPreviewPixmapVisibility(bool IsVisible);
		bool							GetPreviewPixmapVisibility();

		void							AddIOPort(ZEDNodeIOPort* IOPort);
		QList<ZEDNodeIOPort*>*			GetIOPorts();

										ZEDNodeEditorNode(QGraphicsItem* Parent, ZEDNodeEditorGraphicsView* ParentView, ZEDIOPortOrder Alignment);
										~ZEDNodeEditorNode();

};

#endif
