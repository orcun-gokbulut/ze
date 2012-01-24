//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDNodeEditorUndoRedo.h
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
#ifndef _H_ZED_NODE_EDITOR_UNDO_REDO_H_
#define _H_ZED_NODE_EDITOR_UNDO_REDO_H_

#include <QtGui/QUndoCommand>
#include <QtGui/QGraphicsScene>

#include "../ZEDUndoRedo/ZEDUndoRedoOperation.h"
#include "ZETypes.h"

class ZEDIOPortConnectionPoint;
class ZEDNodeConnectionPoint;
class ZEDNodeEditorNodeScaleGizmo;
class ZEDNodeEditorNodeScaleGizmoPoint;
class ZEDNodeIOPort;
class ZEDNodeEditorNode;
class ZEDIOPortConnection;
class ZEDNodeEditorGraphicsView;
class ZEDNodeEditorItem;
class ZEDUndoRedoOperation;

class ZEDNodeEditorAddConnectionOperation : public ZEDUndoRedoOperation
{
	private:

		ZEDNodeEditorGraphicsView* ParentView;
		ZEDIOPortConnection* AddConnectionUndoRedo;

	protected:

													ZEDNodeEditorAddConnectionOperation();
		virtual										~ZEDNodeEditorAddConnectionOperation();

	public:

		static ZEDNodeEditorAddConnectionOperation*	CreateInstance();
		virtual void								Destroy();

		void										SetAddedConnection(ZEDIOPortConnection* Connection);
		void										SetView(ZEDNodeEditorGraphicsView* View);

		virtual void 								Undo();
		virtual void 								Redo();

													ZEDNodeEditorAddConnectionOperation(ZEDIOPortConnection* Connection, ZEDNodeEditorGraphicsView* View);
};

class ZEDNodeEditorDeleteNodeOperation : public ZEDUndoRedoOperation
{
	private:

		ZEDNodeEditorGraphicsView*					ParentView;
		QPointF										NodePos;
		ZEInt										NodeZValue;
		ZEDNodeEditorNode*		 					DeletedNode;

	protected:

													ZEDNodeEditorDeleteNodeOperation();
		virtual										~ZEDNodeEditorDeleteNodeOperation();

	public:

		static ZEDNodeEditorDeleteNodeOperation*	CreateInstance();
		virtual void								Destroy();

		void										SetDeletedNode(ZEDNodeEditorNode* Node);
		void										SetView(ZEDNodeEditorGraphicsView* View);

		virtual void 								Undo();
		virtual void 								Redo();

													
};

class ZEDNodeEditorMoveNodeOperation :  public ZEDUndoRedoOperation
{
	private:

		ZEDNodeEditorGraphicsView*					ParentView;
		QPointF		 								NodeOldPosition;
		QPointF										NodeNewPosition;
		ZEDNodeEditorNode*							MoveNode;

	protected:

													ZEDNodeEditorMoveNodeOperation();
		virtual										~ZEDNodeEditorMoveNodeOperation();

	public:

		static ZEDNodeEditorMoveNodeOperation*		CreateInstance();
		virtual void								Destroy();

		void										SetMovedNode(ZEDNodeEditorNode* Node);
		void										SetView(ZEDNodeEditorGraphicsView* View);

		virtual void 								Undo();
		virtual void 								Redo();

};

class ZEDNodeEditorDeleteConnectionOperation :  public ZEDUndoRedoOperation
{
	private:

		ZEDNodeEditorGraphicsView*					ParentView;
		ZEDIOPortConnection*						DeletedConnection;
		QList<ZEDIOPortConnection*> 				DeletedConnectionsRedoList;

	protected:

													ZEDNodeEditorDeleteConnectionOperation();
		virtual										~ZEDNodeEditorDeleteConnectionOperation();

	public:

		static ZEDNodeEditorDeleteConnectionOperation*	CreateInstance();
		virtual void									Destroy();

		void										SetDeletedConnection(ZEDIOPortConnection* Connection);
		void										SetView(ZEDNodeEditorGraphicsView* View);

		virtual void 								Undo();
		virtual void 								Redo();

};
class ZEDNodeEditorResizeNodeOperation : public ZEDUndoRedoOperation
{
	private:

		ZEDNodeEditorGraphicsView*					ParentView;
		QRectF 										NodeOldRectangle;
		QRectF 										NodeNewRectangle;
		ZEDNodeEditorNode*							ResizeNode;

	protected:

													ZEDNodeEditorResizeNodeOperation();
		virtual										~ZEDNodeEditorResizeNodeOperation();

	public:

		static ZEDNodeEditorResizeNodeOperation*	CreateInstance();
		virtual void								Destroy();

		void										SetResizedNode(ZEDNodeEditorNode* Node);
		void										SetView(ZEDNodeEditorGraphicsView* View);

		virtual void 								Undo();
		virtual void 								Redo();
};

class ZEDNodeEditorItemSelectionOperation : public ZEDUndoRedoOperation
{
private:

	ZEDNodeEditorGraphicsView*						ParentView;
	ZEDNodeEditorItem*								SelectedItem;

protected:

												ZEDNodeEditorItemSelectionOperation();
	virtual										~ZEDNodeEditorItemSelectionOperation();

public:

	static ZEDNodeEditorItemSelectionOperation*	CreateInstance();
	virtual void								Destroy();

	void										SetSelectedItem(ZEDNodeEditorItem*	Item);
	void										SetView(ZEDNodeEditorGraphicsView* View);

	virtual void 								Undo();
	virtual void 								Redo();
};

class ZEDNodeEditorItemDeselectionOperation : public ZEDUndoRedoOperation
{
private:

	ZEDNodeEditorGraphicsView*					ParentView;
	ZEDNodeEditorItem*							DeselectedItem;

protected:

												ZEDNodeEditorItemDeselectionOperation();
	virtual										~ZEDNodeEditorItemDeselectionOperation();

public:

	static ZEDNodeEditorItemDeselectionOperation*CreateInstance();
	virtual void								Destroy();

	void										SetDeselectedItem(ZEDNodeEditorItem* Item);
	void										SetView(ZEDNodeEditorGraphicsView* View);

	virtual void 								Undo();
	virtual void 								Redo();
};

class ZEDNodeEditorSendBackwardOperation : public ZEDUndoRedoOperation
{
private:

	ZEDNodeEditorGraphicsView*					ParentView;
	ZEDNodeEditorNode*							SendBackwardNode;
	ZEInt											SendBackwardNodeZValue;

protected:

												ZEDNodeEditorSendBackwardOperation();
	virtual										~ZEDNodeEditorSendBackwardOperation();

public:

	static ZEDNodeEditorSendBackwardOperation*	CreateInstance();
	virtual void								Destroy();

	void										SetNodeSendBackward(ZEDNodeEditorNode* Node);
	void										SetView(ZEDNodeEditorGraphicsView* View);

	virtual void 								Undo();
	virtual void 								Redo();
};

class ZEDNodeEditorBringForwardOperation : public ZEDUndoRedoOperation
{
private:

	ZEDNodeEditorGraphicsView*					ParentView;
	ZEDNodeEditorNode*							BringForwardNode;
	ZEInt											BringForwardNodeZValue;

protected:

												ZEDNodeEditorBringForwardOperation();
	virtual										~ZEDNodeEditorBringForwardOperation();

public:

	static ZEDNodeEditorBringForwardOperation*	CreateInstance();
	virtual void								Destroy();

	void										SetNodeBringForward(ZEDNodeEditorNode* Node);
	void										SetView(ZEDNodeEditorGraphicsView* View);

	virtual void 								Undo();
	virtual void 								Redo();
};


#endif
