//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDNodeEditorGraphicsView.h
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
#ifndef _H_ZED_NODE_EDITOR_GRAPHICS_VIEW_H_
#define _H_ZED_NODE_EDITOR_GRAPHICS_VIEW_H_

#include <QtGui/QGraphicsView>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/qgraphicsitem.h>
#include "ZETypes.h"

class ZEDNodeEditorNode;
class ZEDIOPortConnection;
class ZEDNodeIOPort;
class ZEDNodeEditorItem;
class ZEDUndoRedoOperation;
class ZEDNodeEditorNodeScaleGizmoPoint;
class ZEDUndoRedoManager;

enum ZEDNodeEditorGraphicsViewRubberBandSelectionMode
{
	ZED_NEGV_RBSM_INTERSECTS,
	ZED_NEGV_RBSM_INSIDE
};

class ZEDNodeEditorGraphicsView : public QGraphicsView
{
	Q_OBJECT

	friend class ZEDNodeEditorNode;
	friend class ZEDNodeEditorDeleteNodeOperation;
	friend class ZEDNodeEditorMoveNodeOperation;
	friend class ZEDNodeEditorDeleteConnectionOperation;
	friend class ZEDNodeEditorAddConnectionOperation;
	friend class ZEDNodeEditorResizeNodeOperation;
	friend class ZEDNodeEditorItemSelectionOperation;
	friend class ZEDNodeEditorItemDeselectionOperation;
	friend class ZEDNodeEditorBringForwardOperation;
	friend class ZEDNodeEditorSendBackwardOperation;

	private:

		QList<ZEDNodeEditorNode*>							Nodes;
		QList<ZEDIOPortConnection*>							Connections;
		QList<ZEDNodeEditorNode*>							SelectedNodes;
		QList<ZEDIOPortConnection*>							SelectedConnections;

		QList<ZEDNodeEditorItem*>							SelectedItems;

		QGraphicsScene*										MainScene;

		QGraphicsRectItem*									RubberBandSelectionRectItem;
		QGraphicsRectItem*									TempRect;

		ZEDIOPortConnection*                                TempConnection; 
		ZEDNodeEditorNodeScaleGizmoPoint*					SelectedGizmo;
		ZEDNodeEditorNode*									SelectedNode;
		ZEDNodeIOPort*										SelectedIOPort;
		ZEDUndoRedoManager*									UndoRedoManager;

		QMenu*												ViewContextMenu;
		QMenu*												MultipleSelectionContextMenu;		

		QList<QAction*>										ViewContextMenuItems;
		QList<QAction*>										MultipleSelectionContextMenuItems;

		QPointF												InitialMousePos;
		QPointF												LastMousePos;

		float 												ConnectionPointFromX;
		float 												ConnectionPointFromY;
		float												ScaleCoefficient;

		bool												IsMultipleSelectionEnabled;
		bool												IsDeselectionEnabled;	
		bool												SelectedItemsIncludeIOPort;
		bool												SelectedItemsIncludeNodeConnectionPoint;
		bool												IsMousePositionContainsGizmo;
		bool												IsMouseMoving;
		bool												IsRubberBandSelectionOnGoing;

		ZEInt												PanStartX;
		ZEInt												PanStartY;
		ZEInt												NodeZValue;
		ZEInt												LastSelectedConnectionIndex;
		ZEInt												ConnectionLoopControlInteger;
		ZEInt												GridSpacing;
		
		ZEDNodeEditorGraphicsViewRubberBandSelectionMode	RubberBandSelectionMode;		

		void												RubberBandSelection();
		void												SelectItem(ZEDNodeEditorItem* Item, ZEDUndoRedoOperation* UndoRedoOperation);
		void												DeselectItem(ZEDNodeEditorItem* Item, ZEDUndoRedoOperation* DeselectionUndoRedoOperation);
		void												MutualDeselection(ZEDUndoRedoOperation* DeselectionUndoRedoOperation);
		void												SetNewConnection(ZEDNodeEditorNode* FromNode, ZEDNodeIOPort* FromPort);
		ZEInt													SelectTopMostNodeZValue(QPointF MousePosition);
		void												DeleteConnection(ZEDIOPortConnection* Connection);
		void												DeleteNode(ZEDNodeEditorNode* Node);	

	protected:

		virtual void										mousePressEvent(QMouseEvent *Event);
		virtual void										mouseReleaseEvent(QMouseEvent *Event);
		virtual void										mouseDoubleClickEvent(QMouseEvent *Event);
		virtual void										mouseMoveEvent(QMouseEvent *Event);

		virtual void										wheelEvent(QWheelEvent *Event);

		virtual void										keyPressEvent(QKeyEvent *Event);
		virtual void										keyReleaseEvent(QKeyEvent *Event);

		virtual void										resizeEvent(QResizeEvent *Event);
		virtual void										drawBackground (QPainter* Painter, const QRectF &rect);

	public slots:

		void												SendBackward();
		void												BringForward();
		void												DeleteSelectedItems();
		void												SelectAllItems();

	signals:

		void												OnIOPortSelection(ZEDNodeIOPort* SelectedIOPort);

		void												OnNodeSelection(QList<ZEDNodeEditorNode*>* SelectedNodes);
		void												OnNodeDeleting(ZEDNodeEditorNode* DeletingNode, bool& DeletingNodeValidation);
		void												OnNodeDeleted(ZEDNodeEditorNode* NodeDeleted);

		void												OnConnectionDeleting(ZEDIOPortConnection* DeletingConnection, bool& DeletingConnectionValidation);
		void												OnConnectionDeleted(ZEDIOPortConnection* ConnectionDeleted);
		void												OnConnectionSelection(QList<ZEDIOPortConnection*>* SelectedConnections);
		void												OnConnectionAdding(ZEDIOPortConnection* ConnectionAdding, bool& ConnectionAddingValidation);
		void												OnConnectionAdded(ZEDIOPortConnection* ConnectionAdded);


	public:

		QList<QAction*>*									GetCustomViewContextMenuItems();
		void												AddCustomViewContextMenuItems(QList<QAction*> ContextMenuItems);
		void												DisplayViewContextMenu(QPointF MousePos);
		
		QList<QAction*>*									GetMultipleSelectionCustomContextMenuItems();
		void												AddMultipleSelectionCustomContextMenuItems(QList<QAction*> ContextMenuItems);
		void												DisplayContextMenuForMultipleSelection(QPointF MousePos);
	
		QGraphicsScene*										GetScene();

		const QList<ZEDNodeEditorNode*>*					GetNodes();
		const QList<ZEDIOPortConnection*>*					GetConnections();

		QList<ZEDNodeEditorNode*>*							GetSelectedNodes();
		QList<ZEDIOPortConnection*>*						GetSelectedConnections();
		QList<ZEDNodeEditorItem*>*							GetSelectedItems();

		bool												GetIsMultipleSelectionEnabled();
		bool												GetIsDeselectionEnabled();

		void												SetRubberBandSelectionMode(ZEDNodeEditorGraphicsViewRubberBandSelectionMode Mode);
		ZEDNodeEditorGraphicsViewRubberBandSelectionMode	GetRubberBandSelectionMode() const;

		void												AddNode(ZEDNodeEditorNode* NewNode, QPointF Position);

		void												AddNodeConnection(ZEDNodeEditorNode* FromNode, ZEDNodeEditorNode* ToNode, ZEDNodeIOPort* To = NULL, bool IsDirected = true);
		void												AddConnection(ZEDIOPortConnection* NewConnection);
		void												AddConnection(ZEDNodeIOPort* From, ZEDNodeIOPort* To, bool IsDirected = true);

															ZEDNodeEditorGraphicsView(QWidget* Parent);
};

#endif
