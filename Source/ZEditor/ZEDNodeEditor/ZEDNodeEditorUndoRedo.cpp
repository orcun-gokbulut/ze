//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDNodeEditorUndoRedo.cpp
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

#include <QtCore/QDebug>
#include <QtCore/QtAlgorithms>

#include "ZEDNodeEditorUndoRedo.h"
#include "../ZEDUndoRedo/ZEDUndoRedoOperation.h"
#include "ZEDIOPortConnectionPoint.h"
#include "ZEDNodeConnectionPoint.h"
#include "ZEDNodeEditorNodeScaleGizmo.h"
#include "ZEDNodeEditorNodeScaleGizmoPoint.h"
#include "ZEDNodeIOPort.h"
#include "ZEDNodeEditorNode.h"
#include "ZEDIOPortConnection.h"
#include "ZEDNodeEditorGraphicsView.h"
#include "ZEDNodeEditorItem.h"


ZEDNodeEditorAddConnectionOperation::ZEDNodeEditorAddConnectionOperation() 
{
	ParentView = NULL;
	AddConnectionUndoRedo = NULL;
}
ZEDNodeEditorAddConnectionOperation::~ZEDNodeEditorAddConnectionOperation() 
{

}

ZEDNodeEditorAddConnectionOperation* ZEDNodeEditorAddConnectionOperation::CreateInstance()
{
	return new ZEDNodeEditorAddConnectionOperation();
}

void ZEDNodeEditorAddConnectionOperation::Destroy()
{
	delete this;
}
void ZEDNodeEditorAddConnectionOperation::SetAddedConnection(ZEDIOPortConnection* Connection)
{
	AddConnectionUndoRedo = Connection;
}
void ZEDNodeEditorAddConnectionOperation::SetView(ZEDNodeEditorGraphicsView* View)
{
	ParentView = View;
}
void ZEDNodeEditorAddConnectionOperation::Undo()
{
	if (AddConnectionUndoRedo != NULL)
	{
		ZEInt Index = ParentView->Connections.indexOf(AddConnectionUndoRedo);
		ParentView->MainScene->removeItem(ParentView->Connections[Index]);				
		ParentView->Connections.removeAt(Index);
		
	}	

	ZEDUndoRedoOperation::Undo();
}

void ZEDNodeEditorAddConnectionOperation::Redo()
{
	if (AddConnectionUndoRedo != NULL)
	{
		ParentView->MainScene->addItem(AddConnectionUndoRedo);
		ParentView->Connections.append(AddConnectionUndoRedo);
	}	

	ZEDUndoRedoOperation::Redo();
}

ZEDNodeEditorDeleteNodeOperation::ZEDNodeEditorDeleteNodeOperation() 
{
	ParentView	= NULL;
	DeletedNode = NULL;
	NodePos		= QPointF(0.0f,0.0f);
	NodeZValue	= 0;
}
ZEDNodeEditorDeleteNodeOperation::~ZEDNodeEditorDeleteNodeOperation()
{
	if(DeletedNode != NULL)
		delete DeletedNode;

	DeletedNode = NULL;
}
ZEDNodeEditorDeleteNodeOperation* ZEDNodeEditorDeleteNodeOperation::CreateInstance()
{
	return new ZEDNodeEditorDeleteNodeOperation();
}
void ZEDNodeEditorDeleteNodeOperation::Destroy()
{
	delete this;
}

void ZEDNodeEditorDeleteNodeOperation::SetDeletedNode(ZEDNodeEditorNode* Node)
{
	DeletedNode = Node;
	NodePos		= Node->pos();
	NodeZValue	= Node->zValue();
}
void ZEDNodeEditorDeleteNodeOperation::SetView(ZEDNodeEditorGraphicsView* View)
{
	ParentView = View;
}

void ZEDNodeEditorDeleteNodeOperation::Undo()
{
	for (ZEInt J = 0; J < ParentView->Nodes.count() ; J++)
	{
		if (ParentView->Nodes[J]->zValue() >= NodeZValue )
		{
			ParentView->Nodes[J]->setZValue(ParentView->Nodes[J]->zValue() + 1);
		}
	}

	ParentView->AddNode(DeletedNode, NodePos);
	DeletedNode->setZValue(NodeZValue);

	ZEDUndoRedoOperation::Undo();
}

void ZEDNodeEditorDeleteNodeOperation::Redo()
{
	ParentView->MainScene->removeItem(DeletedNode);	
	ParentView->MainScene->removeItem(DeletedNode->GetScaleGizmo());	

	ZEInt Index = ParentView->Nodes.indexOf(DeletedNode);
	ParentView->Nodes.removeAt(Index);

	ZEInt RemovedNodeZValue = DeletedNode->zValue();

	for (ZEInt I = 0; I < ParentView->Nodes.count(); I++)
	{				
		if (ParentView->Nodes[I]->zValue() > RemovedNodeZValue)
		{
			ParentView->Nodes[I]->setZValue(ParentView->Nodes[I]->zValue() - 1);
		}
	}
	ParentView->NodeZValue--;

	ZEDUndoRedoOperation::Redo();
}

ZEDNodeEditorMoveNodeOperation::ZEDNodeEditorMoveNodeOperation()
{
	ParentView = NULL;
	NodeOldPosition = QPointF(0.0f,0.0f);
	MoveNode = NULL;
}
ZEDNodeEditorMoveNodeOperation::~ZEDNodeEditorMoveNodeOperation()
{

}
ZEDNodeEditorMoveNodeOperation* ZEDNodeEditorMoveNodeOperation::CreateInstance()
{
	return new ZEDNodeEditorMoveNodeOperation();
}
void ZEDNodeEditorMoveNodeOperation::Destroy()
{
	delete this;
}

void ZEDNodeEditorMoveNodeOperation::SetMovedNode(ZEDNodeEditorNode* Node)
{
	NodeOldPosition = Node->GetNodeOldPosition();
	MoveNode = Node;
}

void ZEDNodeEditorMoveNodeOperation::SetView(ZEDNodeEditorGraphicsView* View)
{
	ParentView = View;
}

void ZEDNodeEditorMoveNodeOperation::Undo()
{
	ZEInt Index = ParentView->Nodes.indexOf(MoveNode);
	NodeNewPosition = ParentView->Nodes[Index]->scenePos();

	ParentView->Nodes[Index]->translate(NodeOldPosition.x() - NodeNewPosition.x(), NodeOldPosition.y() - NodeNewPosition.y());
	ParentView->Nodes[Index]->GetScaleGizmo()->translate(NodeOldPosition.x() - NodeNewPosition.x(), NodeOldPosition.y() - NodeNewPosition.y());

	for (ZEInt J = 0; J < ParentView->Connections.count(); J++)
	{
		if (ParentView->Connections[J]->GetFromNode() == MoveNode || ParentView->Connections[J]->GetToNode() == MoveNode)
		{
			ParentView->Connections[J]->Update();
			ParentView->Connections[J]->UpdateBoundingRect();
		}
	}

	ZEDUndoRedoOperation::Undo();
}

void ZEDNodeEditorMoveNodeOperation::Redo()
{
	NodeOldPosition = MoveNode->scenePos();

	MoveNode->translate(NodeNewPosition.x() - NodeOldPosition.x(), NodeNewPosition.y() - NodeOldPosition.y());
	MoveNode->GetScaleGizmo()->translate(NodeNewPosition.x() - NodeOldPosition.x(), NodeNewPosition.y() - NodeOldPosition.y());

	for (ZEInt J = 0; J < ParentView->Connections.count(); J++)
	{
		if (ParentView->Connections[J]->GetFromNode() == MoveNode || ParentView->Connections[J]->GetToNode() == MoveNode)
		{
			ParentView->Connections[J]->Update();
			ParentView->Connections[J]->UpdateBoundingRect();
		}
	}

	ZEDUndoRedoOperation::Redo();
}

ZEDNodeEditorDeleteConnectionOperation::ZEDNodeEditorDeleteConnectionOperation()
{
	ParentView = NULL;	
	DeletedConnection = NULL;
}
ZEDNodeEditorDeleteConnectionOperation::~ZEDNodeEditorDeleteConnectionOperation()
{
	if(DeletedConnection != NULL)
		delete DeletedConnection;

	DeletedConnection = NULL;
}

ZEDNodeEditorDeleteConnectionOperation* ZEDNodeEditorDeleteConnectionOperation::CreateInstance()
{
	return new ZEDNodeEditorDeleteConnectionOperation();
}

void ZEDNodeEditorDeleteConnectionOperation::Destroy()
{
	delete this;
}

void ZEDNodeEditorDeleteConnectionOperation::SetDeletedConnection(ZEDIOPortConnection* Connection)
{
	DeletedConnection = Connection;
}

void ZEDNodeEditorDeleteConnectionOperation::SetView(ZEDNodeEditorGraphicsView* View)
{
	ParentView = View;
}

void ZEDNodeEditorDeleteConnectionOperation::Undo()
{
	ParentView->MainScene->addItem(DeletedConnection);
	ParentView->Connections.append(DeletedConnection);

	ZEDUndoRedoOperation::Undo();
}

void ZEDNodeEditorDeleteConnectionOperation::Redo()
{
	ZEInt Index = ParentView->Connections.indexOf(DeletedConnection);
	ParentView->MainScene->removeItem(ParentView->Connections[Index]);				
	ParentView->Connections.removeAt(Index);

	ZEDUndoRedoOperation::Redo();
}

ZEDNodeEditorResizeNodeOperation::ZEDNodeEditorResizeNodeOperation()
{
	ParentView = NULL;
	NodeOldRectangle = QRectF(0.0f,0.0f,0.0f,0.0f);
	ResizeNode = NULL;
}
ZEDNodeEditorResizeNodeOperation::~ZEDNodeEditorResizeNodeOperation()
{

}
ZEDNodeEditorResizeNodeOperation* ZEDNodeEditorResizeNodeOperation::CreateInstance()
{
	return new ZEDNodeEditorResizeNodeOperation();
}

void ZEDNodeEditorResizeNodeOperation::Destroy()
{
	delete this;
}

void ZEDNodeEditorResizeNodeOperation::SetResizedNode(ZEDNodeEditorNode* Node)
{
	NodeOldRectangle = Node->GetNodeOldRectangle();
	ResizeNode = Node;
}

void ZEDNodeEditorResizeNodeOperation::SetView(ZEDNodeEditorGraphicsView* View)
{
	ParentView = View;
}

void ZEDNodeEditorResizeNodeOperation::Undo()
{
	ZEInt Index = ParentView->Nodes.indexOf(ResizeNode);
	NodeNewRectangle = QRectF(ParentView->Nodes[Index]->boundingRect());
	
	ResizeNode->setPolygon(QRectF(NodeOldRectangle));
	ResizeNode->GetScaleRect()->setPolygon(NodeOldRectangle);
	ResizeNode->ScaleByGizmo();
	ResizeNode->GetScaleGizmo()->Update();

	for (ZEInt J = 0; J < ParentView->GetConnections()->count(); J++)
	{
		if (ParentView->Connections[J]->GetFromNode() == ResizeNode || ParentView->Connections[J]->GetToNode() == ResizeNode)
		{
			ParentView->Connections[J]->Update();
			ParentView->Connections[J]->UpdateBoundingRect();
		}
	}

	ZEDUndoRedoOperation::Undo();
}

void ZEDNodeEditorResizeNodeOperation::Redo()
{
	ZEInt Index = ParentView->Nodes.indexOf(ResizeNode);
	NodeOldRectangle = QRectF(ParentView->Nodes[Index]->boundingRect());

	ResizeNode->setPolygon(QRectF(NodeNewRectangle));
	ResizeNode->GetScaleRect()->setPolygon(NodeNewRectangle);
	ResizeNode->ScaleByGizmo();
	ResizeNode->GetScaleGizmo()->Update();

	for (ZEInt J = 0; J < ParentView->GetConnections()->count(); J++)
	{
		if (ParentView->Connections[J]->GetFromNode() == ResizeNode || ParentView->Connections[J]->GetToNode() == ResizeNode)
		{
			ParentView->Connections[J]->Update();
			ParentView->Connections[J]->UpdateBoundingRect();
		}
	}

	ZEDUndoRedoOperation::Redo();
}



ZEDNodeEditorItemSelectionOperation::ZEDNodeEditorItemSelectionOperation()
{
	ParentView = NULL;
	SelectedItem = NULL;
}
ZEDNodeEditorItemSelectionOperation::~ZEDNodeEditorItemSelectionOperation()
{

}
ZEDNodeEditorItemSelectionOperation* ZEDNodeEditorItemSelectionOperation::CreateInstance()
{
	return new ZEDNodeEditorItemSelectionOperation();
}

void ZEDNodeEditorItemSelectionOperation::Destroy()
{
	delete this;
}

void ZEDNodeEditorItemSelectionOperation::SetSelectedItem(ZEDNodeEditorItem* Item)
{
	SelectedItem = Item;
}

void ZEDNodeEditorItemSelectionOperation::SetView(ZEDNodeEditorGraphicsView* View)
{
	ParentView = View;
}

void ZEDNodeEditorItemSelectionOperation::Undo()
{
	if (SelectedItem != NULL)
	{
		SelectedItem->SetSelected(false);
		
		ZEInt Index = ParentView->SelectedItems.indexOf(SelectedItem);
		ParentView->SelectedItems.removeAt(Index);
	}

	ZEDUndoRedoOperation::Undo();
}

void ZEDNodeEditorItemSelectionOperation::Redo()
{
	if (SelectedItem != NULL)
	{
		SelectedItem->SetSelected(true);
		ParentView->SelectedItems.append(SelectedItem);
	}

	ZEDUndoRedoOperation::Redo();
}

ZEDNodeEditorItemDeselectionOperation::ZEDNodeEditorItemDeselectionOperation()
{
	ParentView = NULL;
	DeselectedItem = NULL;
}
ZEDNodeEditorItemDeselectionOperation::~ZEDNodeEditorItemDeselectionOperation()
{

}
ZEDNodeEditorItemDeselectionOperation* ZEDNodeEditorItemDeselectionOperation::CreateInstance()
{
	return new ZEDNodeEditorItemDeselectionOperation();
}

void ZEDNodeEditorItemDeselectionOperation::Destroy()
{
	delete this;
}

void ZEDNodeEditorItemDeselectionOperation::SetDeselectedItem(ZEDNodeEditorItem* Item)
{
	DeselectedItem = Item;
}

void ZEDNodeEditorItemDeselectionOperation::SetView(ZEDNodeEditorGraphicsView* View)
{
	ParentView = View;
}

void ZEDNodeEditorItemDeselectionOperation::Undo()
{
	if (DeselectedItem != NULL)
	{
		DeselectedItem->SetSelected(true);
		ParentView->SelectedItems.append(DeselectedItem);
	}

	ZEDUndoRedoOperation::Undo();
}

void ZEDNodeEditorItemDeselectionOperation::Redo()
{
	if (DeselectedItem != NULL)
	{
		DeselectedItem->SetSelected(false);

		ZEInt Index = ParentView->SelectedItems.indexOf(DeselectedItem);
		ParentView->SelectedItems.removeAt(Index);
	}

	ZEDUndoRedoOperation::Redo();
}

ZEDNodeEditorSendBackwardOperation::ZEDNodeEditorSendBackwardOperation()
{
	ParentView = NULL;
	SendBackwardNode = NULL;
	SendBackwardNodeZValue = 0;
}

ZEDNodeEditorSendBackwardOperation::~ZEDNodeEditorSendBackwardOperation()
{

}

ZEDNodeEditorSendBackwardOperation* ZEDNodeEditorSendBackwardOperation::CreateInstance()
{
	return new ZEDNodeEditorSendBackwardOperation();
}

void ZEDNodeEditorSendBackwardOperation::Destroy()
{
	delete this;
}

void ZEDNodeEditorSendBackwardOperation::SetNodeSendBackward(ZEDNodeEditorNode* Node)
{
	SendBackwardNode = Node;
	SendBackwardNodeZValue = Node->zValue();
}

void ZEDNodeEditorSendBackwardOperation::SetView(ZEDNodeEditorGraphicsView* View)
{
	ParentView = View;
}

void ZEDNodeEditorSendBackwardOperation::Undo()
{
	for (ZEInt I = 0; I < ParentView->Nodes.count(); I++)
	{
		if (ParentView->Nodes[I]->zValue() == SendBackwardNodeZValue)
		{
			ParentView->Nodes[I]->setZValue(ParentView->Nodes[I]->zValue() - 1);
		}
	}
	
	SendBackwardNode->setZValue(SendBackwardNodeZValue);

	for (ZEInt I = 0; I < ParentView->Nodes.count(); I++)
	{
		qDebug() << ParentView->Nodes[I]->zValue();
	}
}

void ZEDNodeEditorSendBackwardOperation::Redo()
{
	for (ZEInt I = 0; I < ParentView->Nodes.count(); I++)
	{
		if (ParentView->Nodes[I]->zValue() == SendBackwardNodeZValue - 1)
		{
			ParentView->Nodes[I]->setZValue(SendBackwardNodeZValue);
		}
	}

	SendBackwardNode->setZValue(SendBackwardNodeZValue - 1);
}

ZEDNodeEditorBringForwardOperation::ZEDNodeEditorBringForwardOperation()
{
	ParentView = NULL;
	BringForwardNode = NULL;
	BringForwardNodeZValue = 0;
}

ZEDNodeEditorBringForwardOperation::~ZEDNodeEditorBringForwardOperation()
{

}

ZEDNodeEditorBringForwardOperation* ZEDNodeEditorBringForwardOperation::CreateInstance()
{
	return new ZEDNodeEditorBringForwardOperation();
}

void ZEDNodeEditorBringForwardOperation::Destroy()
{
	delete this;
}

void ZEDNodeEditorBringForwardOperation::SetNodeBringForward(ZEDNodeEditorNode* Node)
{
	BringForwardNode = Node;
	BringForwardNodeZValue = Node->zValue();
}

void ZEDNodeEditorBringForwardOperation::SetView(ZEDNodeEditorGraphicsView* View)
{
	ParentView = View;
}

void ZEDNodeEditorBringForwardOperation::Undo()
{
	for (ZEInt I = 0; I < ParentView->Nodes.count(); I++)
	{
		if (ParentView->Nodes[I]->zValue() == BringForwardNodeZValue)
		{
			ParentView->Nodes[I]->setZValue(BringForwardNodeZValue + 1);
		}
	}

	BringForwardNode->setZValue(BringForwardNodeZValue);
}

void ZEDNodeEditorBringForwardOperation::Redo()
{
	for (ZEInt I = 0; I < ParentView->Nodes.count(); I++)
	{
		if (ParentView->Nodes[I]->zValue() == BringForwardNodeZValue + 1)
		{
			ParentView->Nodes[I]->setZValue(BringForwardNodeZValue);
		}
	}

	BringForwardNode->setZValue(BringForwardNodeZValue + 1);
}
