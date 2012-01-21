//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDNodeEditorGraphicsView.cpp
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

#include <QGraphicsView>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QtAlgorithms>
#include <QTransform>
#include <QScrollBar>
#include <QPen>
#include <QCursor>
#include <QPixmap>
#include <QPainter>
#include <QMenu>
#include <QAction>
#include <math.h>

#include "ZEDNodeEditorGraphicsView.h"
#include "ZEDIOPortConnection.h"
#include "ZEDNodeConnectionPoint.h"
#include "ZEDNodeEditorNode.h"
#include "ZEDNodeIOPort.h"
#include "ZEDNodeEditorNodeScaleGizmo.h"
#include "ZEDNodeEditorNodeScaleGizmoPoint.h"
#include "ZEDNodeEditorUndoRedo.h"
#include "../ZEDUndoRedo/ZEDUndoRedoOperation.h"
#include "../ZEDUndoRedo/ZEDUndoRedoManager.h"
#include "ZEDDefinitions.h"

#include "ZEDNodeEditorMiniMap.h"

#define MAX_Z_VALUE 4000000

void ZEDNodeEditorGraphicsView::mousePressEvent(QMouseEvent *Event)
{
	ZEDNodeEditorItem* PressedItem = NULL;

	if (Event->button() == Qt::LeftButton)
	{
		SelectedNode = NULL;
		bool LookAtConnections = true;
		IsMousePositionContainsGizmo = false;
		IsMouseMoving = false;
		ZEInt PressedConnectionCount = 0;
		QPointF MouseScenePos(0.0f, 0.0f);
		MouseScenePos = mapToScene(Event->pos());
		InitialMousePos = MouseScenePos;

		ZEInt TopMostNodeZValue = SelectTopMostNodeZValue(MouseScenePos);

		for (ZEInt I = 0; I < Nodes.count(); I++)
		{	
			for (ZEInt J = 0; J < Nodes[I]->GetScaleGizmo()->GetGizmoPointList()->count(); J++) // Controlling if any gizmo containing the mousePos
			{
				if (Nodes[I]->GetScaleGizmo()->GetGizmoPointList()->value(J)->boundingRect().contains(Nodes[I]->GetScaleGizmo()->GetGizmoPointList()->value(J)->deviceTransform(viewportTransform()).inverted().map(Event->pos())) && Nodes[I]->GetScaleGizmo()->GetGizmoPointList()->value(J)->isVisible())
				{
					IsMousePositionContainsGizmo = true;
					SelectedGizmo = Nodes[I]->GetScaleGizmo()->GetGizmoPointList()->value(J);
					SelectedNode = Nodes[I];
				}
			}
		}
		qDebug() << "*****************************";

		bool ItemFound = false;
		for (ZEInt I = 0; I < Nodes.count(); I++)
		{	
			if (Nodes[I]->GetScaleGizmo()->isVisible() && IsMousePositionContainsGizmo == true && ItemFound == false) // if any gizmo selected set pressed item as a Node
			{
				PressedItem = (ZEDNodeEditorItem*)(Nodes[I]);
				LookAtConnections = false;
				LastSelectedConnectionIndex = 0;
				PressedConnectionCount = 0;
				ConnectionLoopControlInteger = 0;
				ItemFound = true;
			}
			else if (Nodes[I]->sceneBoundingRect().contains(MouseScenePos) && ItemFound == false) //If mouse pos in a node
			{
				if (Nodes[I]->zValue() == TopMostNodeZValue) //select topmost when mouse clicked
				{
					if (Nodes[I]->GetIOPorts()->count() == 0) // if there is no IO port
					{
						PressedItem = (ZEDNodeEditorItem*)(Nodes[I]);
					}
					for (ZEInt J = 0; J < Nodes[I]->GetIOPorts()->count(); J++)
					{
						if ((Nodes[I]->GetIOPorts()->value(J)->sceneBoundingRect().contains(MouseScenePos) == false && Nodes[I]->GetConnectionPoint()->sceneBoundingRect().contains(MouseScenePos) == false) || transform().m11() < 0.30f ) // If none of IO ports selected select Node
						{
							PressedItem = (ZEDNodeEditorItem*)(Nodes[I]);

						}
						else if (Nodes[I]->GetConnectionPoint()->sceneBoundingRect().contains(MouseScenePos) == true) // If any Node Connection Point Clicked select That connection Point
						{
							if (IsMultipleSelectionEnabled == false)
							{
								SelectedNode = Nodes[I];
								PressedItem = (ZEDNodeEditorItem*)(Nodes[I]->GetConnectionPoint());
							}
						}
						else if (Nodes[I]->GetIOPorts()->value(J)->sceneBoundingRect().contains(MouseScenePos) == true && transform().m11() > 0.30f) // If any IO port Selected select IOPort
						{
							if (IsMultipleSelectionEnabled == false)
							{
								emit OnIOPortSelection(Nodes[I]->GetIOPorts()->value(J));
								SelectedNode = Nodes[I];
								SelectedIOPort = Nodes[I]->GetIOPorts()->value(J);
								PressedItem = (ZEDNodeEditorItem*)(Nodes[I]->GetIOPorts()->value(J));
							}
							break;
						}				
					}	
				}				
				LookAtConnections = false;
				LastSelectedConnectionIndex = 0;
				PressedConnectionCount = 0;
				ConnectionLoopControlInteger = 0;
			}

			if (PressedItem != NULL)
			{
				if (PressedItem->GetItemType() == ZED_NEIT_NODE)
				{
					if (IsMultipleSelectionEnabled == true && IsDeselectionEnabled == false)
					{
						SelectedNodes.append(Nodes[I]);
					}
					else if (IsMultipleSelectionEnabled == false && IsDeselectionEnabled == true)
					{
						SelectedNodes.removeOne(Nodes[I]);
					}
					else if (IsMultipleSelectionEnabled == false && IsDeselectionEnabled == false)
					{
						if (!SelectedItems.contains(PressedItem))
						{
							SelectedNodes.clear();
							SelectedNodes.append(Nodes[I]);
						}
					}

					for (ZEInt I = 0; I < SelectedNodes.count(); I++)
					{
						SelectedNodes[I]->SetNodeOldPosition(SelectedNodes[I]->scenePos());
						SelectedNodes[I]->SetNodeOldRectangle(SelectedNodes[I]->boundingRect());
					}

					emit OnNodeSelection(&SelectedNodes);
					SelectedConnections.clear();
				}
			}
			if (SelectedItems.contains(PressedItem) == false && PressedItem != NULL) // if no item selected
			{
				break;
			}
		}
		
		if (LookAtConnections) // If none of Node | Gizmo | IOPort selected, look if any connection is pressed
		{
			for (ZEInt I = 0 ; I < Connections.count(); I++) //Look how many connections under the mouse when LMB pressed
			{
				QPolygonF TempConnectionBodyBoundingRect	= Connections[I]->ConnectionBodyBoundingRect->mapToScene(Connections[I]->ConnectionBodyBoundingRect->polygon()); // Controlling three virtual bounding polygons that if any of them clicked 
				QPolygonF TempConnectionToBoundingRect		= Connections[I]->ConnectionToBoundingRect->mapToScene(Connections[I]->ConnectionToBoundingRect->polygon());
				QPolygonF TempConnectionFromBoundingRect	= Connections[I]->ConnectionFromBoundingRect->mapToScene(Connections[I]->ConnectionFromBoundingRect->polygon());

				if(TempConnectionBodyBoundingRect.containsPoint(MouseScenePos, Qt::OddEvenFill) || TempConnectionToBoundingRect.containsPoint(MouseScenePos, Qt::OddEvenFill) || TempConnectionFromBoundingRect.containsPoint(MouseScenePos, Qt::OddEvenFill))
				{
					PressedConnectionCount++;
					SelectedNodes.clear();
				}
			}
			if (PressedConnectionCount == 1) // if Connection under the mouse is one
			{
				for (ZEInt I = 0 ; I < Connections.count(); I++)
				{
					QPolygonF TempConnectionBodyBoundingRect	= Connections[I]->ConnectionBodyBoundingRect->mapToScene(Connections[I]->ConnectionBodyBoundingRect->polygon());
					QPolygonF TempConnectionToBoundingRect		= Connections[I]->ConnectionToBoundingRect->mapToScene(Connections[I]->ConnectionToBoundingRect->polygon());
					QPolygonF TempConnectionFromBoundingRect	= Connections[I]->ConnectionFromBoundingRect->mapToScene(Connections[I]->ConnectionFromBoundingRect->polygon());

					if(TempConnectionBodyBoundingRect.containsPoint(MouseScenePos, Qt::OddEvenFill) || TempConnectionToBoundingRect.containsPoint(MouseScenePos, Qt::OddEvenFill) || TempConnectionFromBoundingRect.containsPoint(MouseScenePos, Qt::OddEvenFill))
					{
						if (IsDeselectionEnabled == true && IsMultipleSelectionEnabled == false) // is any connection deselected set z value as -1
							Connections[I]->setZValue(-1);
						else // else make the selected ones z value 0 the others -1
						{
							for (ZEInt J = 0; J < Connections.count(); J++)
							{
								if (J != I)
								{
									Connections[J]->setZValue(-1);
								}
							}
							Connections[I]->setZValue(0);
						}

						PressedItem = (ZEDNodeEditorItem*)(Connections[I]);
						PressedConnectionCount = 0;

						if (IsMultipleSelectionEnabled == true && IsDeselectionEnabled == false)
						{
							SelectedConnections.append(Connections[I]);
						}
						else if (IsMultipleSelectionEnabled == false && IsDeselectionEnabled == true)
						{
							SelectedConnections.removeOne(Connections[I]);
						}
						else if (IsMultipleSelectionEnabled == false && IsDeselectionEnabled == false)
						{
							SelectedConnections.clear();
							SelectedConnections.append(Connections[I]);
						}
						emit OnConnectionSelection(&SelectedConnections); // emit a signal that contains all selected Connections under the limits of above if controls

						break;
					}
				}
			}
			else if (PressedConnectionCount > 1) // if more than 1 connection is under the mouse
			{				
				for (ZEInt I = LastSelectedConnectionIndex ; I < Connections.count(); I++) // selecting the connections one by one
				{
					QPolygonF TempConnectionBodyBoundingRect	= Connections[I]->ConnectionBodyBoundingRect->mapToScene(Connections[I]->ConnectionBodyBoundingRect->polygon());
					QPolygonF TempConnectionToBoundingRect		= Connections[I]->ConnectionToBoundingRect->mapToScene(Connections[I]->ConnectionToBoundingRect->polygon());
					QPolygonF TempConnectionFromBoundingRect	= Connections[I]->ConnectionFromBoundingRect->mapToScene(Connections[I]->ConnectionFromBoundingRect->polygon());

					if(TempConnectionBodyBoundingRect.containsPoint(MouseScenePos, Qt::OddEvenFill) || TempConnectionToBoundingRect.containsPoint(MouseScenePos, Qt::OddEvenFill) || TempConnectionFromBoundingRect.containsPoint(MouseScenePos, Qt::OddEvenFill))
					{
						if (Connections[I]->GetSelected() != true && IsDeselectionEnabled == false) // do this for non selected Connections
						{
							Connections[I]->setZValue(0);
							PressedItem = (ZEDNodeEditorItem*)(Connections[I]);
							LastSelectedConnectionIndex = I + 1;
							ConnectionLoopControlInteger ++;

							if (ConnectionLoopControlInteger == PressedConnectionCount)
							{
								LastSelectedConnectionIndex = 0;
								ConnectionLoopControlInteger = 0;
							}
							PressedConnectionCount = 0;

							if (IsMultipleSelectionEnabled == true && IsDeselectionEnabled == false)
							{
								SelectedConnections.append(Connections[I]);
							}
							else if (IsMultipleSelectionEnabled == false && IsDeselectionEnabled == false)
							{
								SelectedConnections.clear();
								SelectedConnections.append(Connections[I]);

								for (ZEInt J = 0; J < Connections.count(); J++)
								{
									if (J != I)
									{
										Connections[J]->setZValue(-1);
									}
								}
							}
							emit OnConnectionSelection(&SelectedConnections);

							break;
						}
					}
				}	
				for (ZEInt I = Connections.count() - 1 ; I > 0; I--) // to deselect connections one by one
				{
					QPolygonF TempConnectionBodyBoundingRect	= Connections[I]->ConnectionBodyBoundingRect->mapToScene(Connections[I]->ConnectionBodyBoundingRect->polygon());
					QPolygonF TempConnectionToBoundingRect		= Connections[I]->ConnectionToBoundingRect->mapToScene(Connections[I]->ConnectionToBoundingRect->polygon());
					QPolygonF TempConnectionFromBoundingRect	= Connections[I]->ConnectionFromBoundingRect->mapToScene(Connections[I]->ConnectionFromBoundingRect->polygon());

					if(TempConnectionBodyBoundingRect.containsPoint(MouseScenePos, Qt::OddEvenFill) || TempConnectionToBoundingRect.containsPoint(MouseScenePos, Qt::OddEvenFill) || TempConnectionFromBoundingRect.containsPoint(MouseScenePos, Qt::OddEvenFill))
					{
						if (IsDeselectionEnabled == true && Connections[I]->GetSelected() == true && IsMultipleSelectionEnabled == false) // if deselection enabled and connection selected than do this loop
						{
							Connections[I]->setZValue(-1);
							PressedItem = (ZEDNodeEditorItem*)(Connections[I]);
							if (I >= 1)
							{
								LastSelectedConnectionIndex = I - 1;
								ConnectionLoopControlInteger ++;
							}

							if (ConnectionLoopControlInteger == PressedConnectionCount)
							{
								LastSelectedConnectionIndex = 0;
								ConnectionLoopControlInteger = 0;
							}
							PressedConnectionCount = 0;


							if (IsMultipleSelectionEnabled == false && IsDeselectionEnabled == true)
							{
								SelectedConnections.removeOne(Connections[I]);
							}
								emit OnConnectionSelection(&SelectedConnections); // emit the list of selected connections
								qDebug() << SelectedConnections;

							break;
						}
					}
				}					
			}			
		}
		ZEDUndoRedoOperation* UndoRedoOperation = ZEDUndoRedoOperation::CreateInstance();

		if(PressedItem != NULL) // if any item pressed
		{			
			if (IsMultipleSelectionEnabled == true && IsDeselectionEnabled == false)  // if Mult. selection enabled
			{
				for (ZEInt I = 0; I < SelectedItems.count(); I++)
				{
					if (SelectedItems[I]->GetItemType() != ZED_NEIT_IOPORT) // if selected items include IOPort set the flag false
						SelectedItemsIncludeIOPort = false;

					else // else vice versa
					{
						SelectedItemsIncludeIOPort = true;
						for (ZEInt I = 0; I < SelectedItems.count(); I++)
							SelectedItems[I]->SetSelected(false);
					}

					if (SelectedItems[I]->GetItemType() != ZED_NEIT_NODE_CONNECTION_POINT) // if selected items include node connection point set the flag false
						SelectedItemsIncludeNodeConnectionPoint = false;

					else // else vice versa
					{
						SelectedItemsIncludeNodeConnectionPoint = true;
						for (ZEInt I = 0; I < SelectedItems.count(); I++)
							SelectedItems[I]->SetSelected(false);
					}

					if (SelectedItemsIncludeNodeConnectionPoint || SelectedItemsIncludeIOPort)
					{
						MutualDeselection(UndoRedoOperation);
					}					
				}				

				if (SelectedItemsIncludeIOPort == false || SelectedItemsIncludeNodeConnectionPoint == false) // if selected item not includes IOPort or Node connection Point
				{
					if(SelectedItems.contains(PressedItem) == false) // and Newly pressed items not in the selected items
					{
						SelectItem(PressedItem, UndoRedoOperation);
						
					}				
				}
			}

			else if (IsMultipleSelectionEnabled == false && IsDeselectionEnabled == false) // if none of mult. sel. or desel. enabled
			{
				if (!SelectedItems.contains(PressedItem) || SelectedItems.count() == 0) // if there is no selected item or pressed item is not in the selected item list
				{
					for (ZEInt I = 0; I < SelectedItems.count(); I++) // clear and deselect all the elements on the view
						SelectedItems[I]->SetSelected(false);

					MutualDeselection(UndoRedoOperation);

					if(IsMousePositionContainsGizmo == true) // if any gizmo clicked
					{
						SelectItem((ZEDNodeEditorItem*)(SelectedNode), UndoRedoOperation);
						
					}
					else // none of gizmo clicked
					{
						SelectItem(PressedItem, UndoRedoOperation);
						

						if (PressedItem->GetItemType() != ZED_NEIT_IOPORT) // if not IOPort
							SelectedItemsIncludeIOPort = false;

						else // set connection if an io port pressed
						{
							SelectedItemsIncludeIOPort = true;

							SetNewConnection(SelectedNode, SelectedIOPort);
						}

						if (PressedItem->GetItemType() != ZED_NEIT_NODE_CONNECTION_POINT) // if not Node Connection Point
							SelectedItemsIncludeNodeConnectionPoint = false;

						else // set connection if an Node connection point pressed
						{
							SelectedItemsIncludeNodeConnectionPoint = true;
							SetNewConnection(SelectedNode, NULL);

						}

						if (SelectedItemsIncludeIOPort || SelectedItemsIncludeNodeConnectionPoint)
						{
							SelectedNodes.clear();
						}
					}					
				}
				else if(SelectedItems.contains(PressedItem))
				{
					if (PressedItem->GetItemType() != ZED_NEIT_IOPORT)
						SelectedItemsIncludeIOPort = false;

					else // set connection if an io port pressed
					{
						SelectedItemsIncludeIOPort = true;

						SetNewConnection(SelectedNode, SelectedIOPort);

					}
				}
			}
			
			if(SelectedItems.contains(PressedItem) == true && IsDeselectionEnabled == true)
			{
				//PressedItem->SetSelected(false);
				DeselectItem(PressedItem, UndoRedoOperation);
			}
			IsRubberBandSelectionOnGoing = false;

			
		}

		else if(PressedItem == NULL)
		{
			if(IsMultipleSelectionEnabled == false && IsDeselectionEnabled == false)
			{
				for (ZEInt I = 0; I < Connections.count(); I++)
				{				
					Connections[I]->SetSelected(false);
					Connections[I]->setZValue(-1);
				}

				for (ZEInt I = 0; I < SelectedItems.count(); I++)
					SelectedItems[I]->SetSelected(false);

				MutualDeselection(UndoRedoOperation);
				SelectedNodes.clear();
				SelectedConnections.clear();
			}
			LastSelectedConnectionIndex = 0;		
			PressedConnectionCount = 0;
			SelectedItemsIncludeIOPort = false;
			SelectedItemsIncludeNodeConnectionPoint = false;
			IsRubberBandSelectionOnGoing = true;
			RubberBandSelectionRectItem = new QGraphicsRectItem(NULL,MainScene);
			MainScene->addItem(RubberBandSelectionRectItem);
		}	

		if (!UndoRedoOperation->ChildOperations.isEmpty())
			UndoRedoManager->RegisterUndoRedoOperation(UndoRedoOperation);
		else
			UndoRedoOperation->Destroy();
	}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////      RIGHT BUTTON	///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
	else if (Event->button() == Qt::RightButton)
	{
		bool AnyItemPressedFlag = false;
		PressedItem = NULL;
		QPointF MouseScenePos(0.0f, 0.0f);
		MouseScenePos = mapToScene(Event->pos());

		ZEInt TopMostNodeZValue = SelectTopMostNodeZValue(MouseScenePos);

		ZEDUndoRedoOperation* UndoRedoOperation = ZEDUndoRedoOperation::CreateInstance();

		if (SelectedItems.count() > 1)
		{
			for (ZEInt I = 0; I < Nodes.count(); I++)
			{	
				if (Nodes[I]->sceneBoundingRect().contains(MouseScenePos) ) //If mouse pos in a node
				{
						DisplayContextMenuForMultipleSelection(QCursor::pos());
						AnyItemPressedFlag = true;
						IsMultipleSelectionEnabled = false;
						IsDeselectionEnabled = false;
						break;	
				}
			}
			if (!AnyItemPressedFlag) //If mouse pos NOT in a node
			{
				for (ZEInt I = 0; I < SelectedItems.count(); I++)
				{
					SelectedItems[I]->SetSelected(false);
				}
				MutualDeselection(UndoRedoOperation);
				SelectedNodes.clear();
				DisplayViewContextMenu(QCursor::pos());
			}
		}
		else
		{
			for (ZEInt I = 0; I < Nodes.count(); I++)
			{	
				if (Nodes[I]->sceneBoundingRect().contains(MouseScenePos) ) //If mouse pos in a node
				{
					if (Nodes[I]->zValue() == TopMostNodeZValue) //select topmost when mouse clicked
					{
						if (Nodes[I]->GetIOPorts()->count() == 0) // if there is no IO port
						{
							PressedItem = (ZEDNodeEditorItem*)(Nodes[I]);

							SelectedNodes.clear();
							SelectedNodes.append(Nodes[I]);

							emit OnNodeSelection(&SelectedNodes);
							SelectedConnections.clear();

						}
						for (ZEInt J = 0; J < Nodes[I]->GetIOPorts()->count(); J++)
						{
							if (Nodes[I]->GetIOPorts()->value(J)->sceneBoundingRect().contains(MouseScenePos) == false || transform().m11() < 0.30f ) // If none of IO ports selected select Node
							{
								PressedItem = (ZEDNodeEditorItem*)(Nodes[I]);

								SelectedNodes.clear();
								SelectedNodes.append(Nodes[I]);


								emit OnNodeSelection(&SelectedNodes);
								SelectedConnections.clear();
							}
							else if (Nodes[I]->GetIOPorts()->value(J)->sceneBoundingRect().contains(MouseScenePos) == true && transform().m11() > 0.30f) // If any IO port Selected select IOPort
							{
								emit OnIOPortSelection(Nodes[I]->GetIOPorts()->value(J));
								SelectedNode = Nodes[I];
								SelectedIOPort = Nodes[I]->GetIOPorts()->value(J);
								PressedItem = (ZEDNodeEditorItem*)(Nodes[I]->GetIOPorts()->value(J));

								break;
							}				
						}	
					}
					LastSelectedConnectionIndex = 0;
					ConnectionLoopControlInteger = 0;
				}
				if (SelectedItems.contains(PressedItem) == false && PressedItem != NULL) // if no item selected
				{
					break;
				}
			}
					
			if(PressedItem != NULL) // if any item pressed
			{
				if (IsMultipleSelectionEnabled == true && IsDeselectionEnabled == false)  // if Mult. selection enabled
				{
					TempConnection = new ZEDIOPortConnection(NULL);	
					for (ZEInt I = 0; I < SelectedItems.count(); I++)
					{
						if (SelectedItems[I]->GetItemType() != ZED_NEIT_IOPORT) // if selected items include IOPort set the flag false
							SelectedItemsIncludeIOPort = false;

						else // else vice versa
						{
							SelectedItemsIncludeIOPort = true;
							for (ZEInt I = 0; I < SelectedItems.count(); I++)
								SelectedItems[I]->SetSelected(false);
						}
						if (SelectedItemsIncludeNodeConnectionPoint || SelectedItemsIncludeIOPort)
						{
							MutualDeselection(UndoRedoOperation);
						}					
					}				

					if (SelectedItemsIncludeIOPort == false || SelectedItemsIncludeNodeConnectionPoint == false) // if selected item not includes IOPort or Node connection Point
					{
						if(SelectedItems.contains(PressedItem) == false) // and Newly pressed items not in the selected items
						{							
							SelectItem(PressedItem, UndoRedoOperation);						
						}				
					}
				}

				else if (IsMultipleSelectionEnabled == false && IsDeselectionEnabled == false) // if none of mult. sel. or desel. enabled
				{
					if (!SelectedItems.contains(PressedItem) || SelectedItems.count() == 0) // if there is no selected item or pressed item is not in the selected item list
					{
						for (ZEInt I = 0; I < SelectedItems.count(); I++) // clear and deselect all the elements on the view
							SelectedItems[I]->SetSelected(false);

						MutualDeselection(UndoRedoOperation);

						if(IsMousePositionContainsGizmo == true) // if any gizmo clicked
						{
							SelectItem((ZEDNodeEditorItem*)(SelectedNode), UndoRedoOperation);
							
						}
						else // none of gizmo clicked
						{
							SelectItem(PressedItem, UndoRedoOperation);
							

							if (PressedItem->GetItemType() != ZED_NEIT_IOPORT) // if not IOPort
								SelectedItemsIncludeIOPort = false;

							else // set connection if an io port pressed
							{
								SelectedConnections.clear();
								for (ZEInt I = 0; I < Connections.count(); I++) // set selected connections for selected IOPort
								{
									if (Connections[I]->GetFrom() == SelectedIOPort || Connections[I]->GetTo() == SelectedIOPort)
									{
										SelectedConnections.append(Connections[I]);
									}
								}
								SelectedItemsIncludeIOPort = true;

							}

							if (SelectedItemsIncludeIOPort || SelectedItemsIncludeNodeConnectionPoint)
							{
								SelectedNodes.clear();
							}

						}

						
					}
					else if(SelectedItems.contains(PressedItem))
					{
						if (PressedItem->GetItemType() != ZED_NEIT_IOPORT)
							SelectedItemsIncludeIOPort = false;

						else // set connection if an io port pressed
						{
							SelectedItemsIncludeIOPort = true;

							SelectedNodes.clear();
						}
					}
				}
				
				if(SelectedItems.contains(PressedItem) == true && IsDeselectionEnabled == true)
				{
					DeselectItem(PressedItem, UndoRedoOperation);
					PressedItem->SetSelected(false);
				}
				IsRubberBandSelectionOnGoing = false;
			}

			else if(PressedItem == NULL)
			{
				if(IsMultipleSelectionEnabled == false && IsDeselectionEnabled == false)
				{
					for (ZEInt I = 0; I < Connections.count(); I++)
					{				
						Connections[I]->SetSelected(false);
						Connections[I]->setZValue(-1);
					}

					for (ZEInt I = 0; I < SelectedItems.count(); I++)
						SelectedItems[I]->SetSelected(false);

					MutualDeselection(UndoRedoOperation);
					SelectedNodes.clear();
					SelectedConnections.clear();
				}
				LastSelectedConnectionIndex = 0;		
				IsRubberBandSelectionOnGoing = true;
				RubberBandSelectionRectItem = new QGraphicsRectItem(NULL,MainScene);
				MainScene->addItem(RubberBandSelectionRectItem);
			}		
			for (ZEInt I = 0; I < Nodes.count(); I++)
			{	
				if (Nodes[I]->sceneBoundingRect().contains(MouseScenePos) ) //If mouse pos in a node
				{
					if (Nodes[I]->zValue() == TopMostNodeZValue) //select topmost when mouse clicked
					{
						if (Nodes[I]->GetIOPorts()->count() == 0) // if there is no IO port
						{
							Nodes[I]->DisplayNodeContextMenu(QCursor::pos());
							AnyItemPressedFlag = true;
							break;
						}
						for (ZEInt J = 0; J < Nodes[I]->GetIOPorts()->count(); J++)
						{
							if (Nodes[I]->GetIOPorts()->value(J)->sceneBoundingRect().contains(MouseScenePos) == true && transform().m11() > 0.30f) // If any IO port Selected select IOPort
							{
								Nodes[I]->GetIOPorts()->value(J)->DisplayIOPortContextMenu(QCursor::pos());
								AnyItemPressedFlag = true;
								break;
							}										
						}	
					}
				}
			}
			if (!AnyItemPressedFlag)
			{
				for (ZEInt I = 0; I < Nodes.count(); I++)
				{	
					if (Nodes[I]->sceneBoundingRect().contains(MouseScenePos) ) //If mouse pos in a node
					{
						if (Nodes[I]->zValue() == TopMostNodeZValue) //select topmost when mouse clicked
						{
							if (Nodes[I]->GetIOPorts()->count() == 0) // if there is no IO port
							{
								Nodes[I]->DisplayNodeContextMenu(QCursor::pos());
								AnyItemPressedFlag = true;
								break;
							}
							for (ZEInt J = 0; J < Nodes[I]->GetIOPorts()->count(); J++)
							{
								if (Nodes[I]->GetIOPorts()->value(J)->sceneBoundingRect().contains(MouseScenePos) == false || transform().m11() < 0.30f) // If none of IO ports selected select Node
								{
									Nodes[I]->DisplayNodeContextMenu(QCursor::pos());
									AnyItemPressedFlag = true;
									break;
								}
							}	
						}
					}
				}
			}	
			if (!AnyItemPressedFlag) //If mouse pos NOT in a node
			{
				for (ZEInt I = 0; I < SelectedItems.count(); I++)
				{
					SelectedItems[I]->SetSelected(false);
				}
				MutualDeselection(UndoRedoOperation);
				SelectedNodes.clear();
				DisplayViewContextMenu(QCursor::pos());
			}
		}

		if (!UndoRedoOperation->ChildOperations.isEmpty())
			UndoRedoManager->RegisterUndoRedoOperation(UndoRedoOperation);
		else
			UndoRedoOperation->Destroy();	
	}

	else if (Event->button() == Qt::MidButton)
	{
		setCursor(Qt::ClosedHandCursor); 
		PanStartX = Event->x(); 
		PanStartY = Event->y(); 
	}

	QGraphicsView::mousePressEvent(Event);
}
void ZEDNodeEditorGraphicsView::mouseReleaseEvent(QMouseEvent *Event)
{
	if (Event->button() == Qt::LeftButton)
	{
		IsMouseMoving = false;

		for (ZEInt I = 0; I < Nodes.count(); I++)
		{
			if(Nodes[I]->GetSelected() == true)
			{
				Nodes[I]->ScaleByGizmo();

				for (ZEInt J = 0; J < Connections.count(); J++)
				{
					if (Connections[J]->GetFromNode() == Nodes[I] || Connections[J]->GetToNode() == Nodes[I])
					{
						Connections[J]->Update();
						Connections[J]->UpdateBoundingRect();
					}
				}
			}
		}

		if (SelectedNodes.count() != 0 && !IsRubberBandSelectionOnGoing)
		{
			if ((fabs(SelectedNodes[0]->GetNodeOldPosition().x() - SelectedNodes[0]->scenePos().x()) > ZE_ZERO_THRESHOLD) || fabs(SelectedNodes[0]->GetNodeOldPosition().y() - SelectedNodes[0]->scenePos().y()) > ZE_ZERO_THRESHOLD)
			{
				ZEDUndoRedoOperation* UndoRedoOperation = ZEDUndoRedoOperation::CreateInstance();
				
				for (ZEInt I = 0; I < SelectedNodes.count(); I++)
				{
					ZEDNodeEditorMoveNodeOperation* NodeMove = ZEDNodeEditorMoveNodeOperation::CreateInstance();
					NodeMove->SetView(this);
					NodeMove->SetMovedNode(SelectedNodes[I]);
					UndoRedoOperation->AddChildOperation(NodeMove);
				}

				if (!UndoRedoOperation->ChildOperations.isEmpty())
					UndoRedoManager->RegisterUndoRedoOperation(UndoRedoOperation);
				else
					UndoRedoOperation->Destroy();		
			}
		}
		if (IsDeselectionEnabled == false &&  IsMousePositionContainsGizmo == true)
		{
			if (SelectedNodes.count() != 0)
			{
				if ((fabs(SelectedNodes[0]->GetNodeOldRectangle().width() - SelectedNodes[0]->sceneBoundingRect().width()) > ZE_ZERO_THRESHOLD) || fabs(SelectedNodes[0]->GetNodeOldRectangle().height() - SelectedNodes[0]->sceneBoundingRect().height()) > ZE_ZERO_THRESHOLD)
				{
					SelectedNode->ScaleByGizmo();

					ZEDUndoRedoOperation* UndoRedoOperation = ZEDUndoRedoOperation::CreateInstance();

					for (ZEInt I = 0; I < SelectedNodes.count(); I++)
					{
						ZEDNodeEditorResizeNodeOperation* NodeResize = ZEDNodeEditorResizeNodeOperation::CreateInstance();
						NodeResize->SetView(this);
						NodeResize->SetResizedNode(SelectedNodes[I]);
						UndoRedoOperation->AddChildOperation(NodeResize);
					}

					if (!UndoRedoOperation->ChildOperations.isEmpty())
						UndoRedoManager->RegisterUndoRedoOperation(UndoRedoOperation);
					else
						UndoRedoOperation->Destroy();
				}
			}
		}
		if (IsRubberBandSelectionOnGoing) // RubberBandSelection
		{
			RubberBandSelection();
		}

		if (SelectedItemsIncludeIOPort == true && IsDeselectionEnabled == false && IsMultipleSelectionEnabled == false) // after mouse released control if it includes port or not if available draw if not dont draw
		{
			MainScene->removeItem(TempConnection);
			QPointF MousePosNow = mapToScene(Event->pos());
			bool SelectedFlag = false;
			QList<ZEInt> SelectTopMost;
			ZEInt TopMost = 0;

			for (ZEInt I = 0; I < Nodes.count(); I++) //create qlist to find topmost item
			{
				if (Nodes[I]->sceneBoundingRect().contains(MousePosNow))
					SelectTopMost.append(Nodes[I]->zValue());
			}

			if (SelectTopMost.count() != 0)	//take mosttop from qlist
			{
				qSort(SelectTopMost.begin(), SelectTopMost.end());
				TopMost = SelectTopMost.takeLast();
			}

			ZEDUndoRedoOperation* UndoRedoOperation = ZEDUndoRedoOperation::CreateInstance();
			for (ZEInt I = 0; I < Nodes.count(); I++)
			{
				for (ZEInt J = 0; J < Nodes[I]->GetIOPorts()->count(); J++)
				{
					if (Nodes[I]->GetIOPorts()->value(J)->sceneBoundingRect().contains(MousePosNow) && SelectedFlag == false)
					{
						if (TempConnection->GetFromNode() != Nodes[I] && Nodes[I]->zValue() == TopMost)
						{
							TempConnection->SetTo(Nodes[I]->GetIOPorts()->value(J));
							TempConnection->SetToNode(Nodes[I]);
							
							if (TempConnection->GetFrom() != NULL)
							{
								AddConnection(TempConnection->GetFrom(), TempConnection->GetTo(), TempConnection->GetDirected());
							}

							for (ZEInt I = 0; I < SelectedItems.count(); I++)
								SelectedItems[I]->SetSelected(false);
							
							MutualDeselection(UndoRedoOperation);							
							
							SelectedFlag = true;
							break;
						}						
					}
				}
			}

			if (!UndoRedoOperation->ChildOperations.isEmpty())
				UndoRedoManager->RegisterUndoRedoOperation(UndoRedoOperation);
			else
				UndoRedoOperation->Destroy();
			

			MainScene->removeItem(TempConnection);
			delete(TempConnection);
			SelectedItemsIncludeIOPort = false;
		}
		else if (SelectedItemsIncludeNodeConnectionPoint == true && IsDeselectionEnabled == false && IsMultipleSelectionEnabled == false) // after mouse released control if it includes Node connection point / IOPort or not. if available draw if not dont draw
		{
			MainScene->removeItem(TempConnection);
			QPointF MousePosNow = mapToScene(Event->pos());
			bool IOPortSelectedFlag = false;
			bool NodeConnectionPointSelectedFlag = false;
			QList<ZEInt> SelectTopMost;
			ZEInt TopMost = 0;

			for (ZEInt I = 0; I < Nodes.count(); I++) //create qlist to find topmost item
			{
				if (Nodes[I]->sceneBoundingRect().contains(MousePosNow))
					SelectTopMost.append(Nodes[I]->zValue());
			}

			if (SelectTopMost.count() != 0)	//take mosttop from qlist
			{
				qSort(SelectTopMost.begin(), SelectTopMost.end());
				TopMost = SelectTopMost.takeLast();
			}

			ZEDUndoRedoOperation* UndoRedoOperation = ZEDUndoRedoOperation::CreateInstance();
			
			for (ZEInt I = 0; I < Nodes.count(); I++)
			{
				if (Nodes[I]->GetConnectionPoint()->sceneBoundingRect().contains(MousePosNow) && NodeConnectionPointSelectedFlag == false)
				{
					if (TempConnection->GetFromNode() != Nodes[I] && Nodes[I]->zValue() == TopMost)
					{
						TempConnection->SetToNode(Nodes[I]);
						TempConnection->SetTo(NULL);

						if (TempConnection->GetFrom() == NULL)
							AddNodeConnection(TempConnection->GetFromNode(), TempConnection->GetToNode(), TempConnection->GetTo(), TempConnection->GetDirected());

						for (ZEInt I = 0; I < SelectedItems.count(); I++)
							SelectedItems[I]->SetSelected(false);

						MutualDeselection(UndoRedoOperation);

						NodeConnectionPointSelectedFlag = true;
						break;
					}
				}
				else
				{
					for (ZEInt J = 0; J < Nodes[I]->GetIOPorts()->count(); J++)
					{
						if (Nodes[I]->GetIOPorts()->value(J)->sceneBoundingRect().contains(MousePosNow) && IOPortSelectedFlag == false)
						{
							if (TempConnection->GetFromNode() != Nodes[I] && Nodes[I]->zValue() == TopMost)
 							{
								TempConnection->SetTo(Nodes[I]->GetIOPorts()->value(J));
								TempConnection->SetToNode(Nodes[I]);

								if (TempConnection->GetFrom() == NULL)
									AddNodeConnection(TempConnection->GetFromNode(), TempConnection->GetToNode(), TempConnection->GetTo(), TempConnection->GetDirected());

								for (ZEInt I = 0; I < SelectedItems.count(); I++)
									SelectedItems[I]->SetSelected(false);

								MutualDeselection(UndoRedoOperation);

								IOPortSelectedFlag = true;
								break;
							}						
						}
					}
				}
				if (IOPortSelectedFlag == false && NodeConnectionPointSelectedFlag == false)
				{
					for (ZEInt I = 0; I < SelectedItems.count(); I++)
						SelectedItems[I]->SetSelected(false);

					MutualDeselection(UndoRedoOperation);
				}
			
			}
			if (!UndoRedoOperation->ChildOperations.isEmpty())
				UndoRedoManager->RegisterUndoRedoOperation(UndoRedoOperation);
			else
				UndoRedoOperation->Destroy();
			
			MainScene->removeItem(TempConnection);
			delete(TempConnection);
			SelectedItemsIncludeIOPort = false;
			SelectedItemsIncludeNodeConnectionPoint = false;
		}

	}
	else if (Event->button() == Qt::MidButton)
	{
		if (cursor().shape() == Qt::ClosedHandCursor)
		{
			setCursor(Qt::ArrowCursor); 
		}
		Event->accept(); 
		return; 
	}
	QGraphicsView::mouseReleaseEvent(Event);
}
void ZEDNodeEditorGraphicsView::mouseDoubleClickEvent(QMouseEvent *Event)
{
	QGraphicsView::mouseDoubleClickEvent(Event);
}
void ZEDNodeEditorGraphicsView::mouseMoveEvent(QMouseEvent *Event)
{
	QPointF MouseNowPos = mapToScene(Event->pos());

	if ((Event->buttons() & Qt::LeftButton) == Qt::LeftButton && (Event->buttons() & Qt::XButton2) != Qt::XButton2)
	{
		IsMouseMoving = true;
		if (IsRubberBandSelectionOnGoing == false)
		{
			if (IsDeselectionEnabled == false &&  IsMousePositionContainsGizmo == false) 
			{
				LastMousePos = mapToScene(Event->pos());

				for (ZEInt I = 0; I < Nodes.count(); I++)
				{
					if(Nodes[I]->GetSelected() == true)
					{
						Nodes[I]->translate((LastMousePos.x() - InitialMousePos.x()), (LastMousePos.y() - InitialMousePos.y()));
						Nodes[I]->GetScaleGizmo()->translate((LastMousePos.x() - InitialMousePos.x()), (LastMousePos.y() - InitialMousePos.y()));

						for (ZEInt J = 0; J < Connections.count(); J++)
						{
							if (Connections[J]->GetFromNode() == Nodes[I] || Connections[J]->GetToNode() == Nodes[I])
							{
								Connections[J]->Update();
							}
						}
					}
				}
				
				InitialMousePos = LastMousePos;
			}
			else if (IsDeselectionEnabled == false &&  IsMousePositionContainsGizmo == true )
			{
				LastMousePos = mapToScene(Event->pos());
				for (ZEInt I = 0; I < Nodes.count(); I++)
				{
					if(Nodes[I]->GetSelected() == true)
					{
						Nodes[I]->ScaleRect(InitialMousePos - LastMousePos, SelectedGizmo->GetGizmoPointPosition());
					}
				}
				InitialMousePos = LastMousePos;
			}
			else if (IsDeselectionEnabled == false &&  IsMousePositionContainsGizmo == true && IsMultipleSelectionEnabled == false)
			{
				LastMousePos = mapToScene(Event->pos());
				
				SelectedNode->ScaleRect(InitialMousePos - LastMousePos, SelectedGizmo->GetGizmoPointPosition());

				InitialMousePos = LastMousePos;
			}
			
			if (SelectedItemsIncludeIOPort == true && IsDeselectionEnabled == false && IsMultipleSelectionEnabled == false ) // �f item is IO port then start drawing a line
			{
				LastMousePos = mapToScene(Event->pos());

				if (LastMousePos.x() >=  (TempConnection->GetFrom()->sceneBoundingRect().topLeft().x() +  TempConnection->GetFrom()->sceneBoundingRect().width() / 2)) //determine where will the line start drawing
				{
					ConnectionPointFromX = TempConnection->GetFrom()->sceneBoundingRect().bottomRight().x() - TempConnection->GetFrom()->sceneBoundingRect().height() / 4;
				}
				else if (LastMousePos.x() <  (TempConnection->GetFrom()->sceneBoundingRect().topLeft().x() +  TempConnection->GetFrom()->sceneBoundingRect().width() / 2))
				{
					ConnectionPointFromX = TempConnection->GetFrom()->sceneBoundingRect().topLeft().x() + TempConnection->GetFrom()->sceneBoundingRect().height() / 4;
				}

				QPolygonF TempPolygon;
								
				TempPolygon << QPoint(ConnectionPointFromX , ConnectionPointFromY) << QPoint(LastMousePos.x() , LastMousePos.y());

				TempConnection->setPolygon(TempPolygon);
				TempConnection->setZValue(MAX_Z_VALUE);

				QPen BlackPen(Qt::black);
				BlackPen.setWidth(0.5);	

				TempConnection->setPen(BlackPen);

				if (TempConnection->GetDirected() == true)
				{
					TempConnection->GetDirectionArrow(LastMousePos);
				}

				bool SelectedFlag = false;
				QList<ZEInt> SelectTopMost;
				ZEInt TopMost = 0;

				for (ZEInt I = 0; I < Nodes.count(); I++) //create qlist to find topmost item
				{
					if (Nodes[I]->sceneBoundingRect().contains(LastMousePos))
						SelectTopMost.append(Nodes[I]->zValue());
				}

				if (SelectTopMost.count() != 0)	//take mosttop from qlist
				{
					qSort(SelectTopMost.begin(), SelectTopMost.end());
					TopMost = SelectTopMost.takeLast();
				}
				
				for (ZEInt I = 0; I < Nodes.count(); I++)
				{
					if (Nodes[I]->sceneBoundingRect().contains(LastMousePos))
					{
						for (ZEInt J = 0; J < Nodes[I]->GetIOPorts()->count(); J++)
						{
							if (Nodes[I]->GetIOPorts()->value(J)->sceneBoundingRect().contains(LastMousePos) && Nodes[I]->zValue() == TopMost && SelectedFlag == false)
							{
								if (TempConnection->GetFromNode() != Nodes[I])
								{
									SelectedItems.append((ZEDNodeEditorItem*)(Nodes[I]->GetIOPorts()->value(J)));
									Nodes[I]->GetIOPorts()->value(J)->SetSelected(true);

									SelectedFlag = true;
								}
							}
							else if (TempConnection->GetFrom() != Nodes[I]->GetIOPorts()->value(J))
								Nodes[I]->GetIOPorts()->value(J)->SetSelected(false);
						}
					}
					else 
					{
						for (ZEInt J = 0; J < Nodes[I]->GetIOPorts()->count(); J++)
							if (TempConnection->GetFrom() != Nodes[I]->GetIOPorts()->value(J))
								Nodes[I]->GetIOPorts()->value(J)->SetSelected(false);
					}
				}

				InitialMousePos = LastMousePos;
			}
			else if (SelectedItemsIncludeNodeConnectionPoint == true && IsDeselectionEnabled == false && IsMultipleSelectionEnabled == false) // when building a connection from Node connection Point
			{
				LastMousePos = mapToScene(Event->pos());

				QPolygonF TempPolygon;

				TempPolygon << QPoint(ConnectionPointFromX , ConnectionPointFromY) << QPoint(LastMousePos.x() , LastMousePos.y());

				TempConnection->setPolygon(TempPolygon);
				TempConnection->setZValue(MAX_Z_VALUE);

				QPen BlackPen(Qt::black);
				BlackPen.setWidth(0.5);	

				TempConnection->setPen(BlackPen);

				if (TempConnection->GetDirected() == true)
				{
					TempConnection->GetDirectionArrow(LastMousePos);
				}

				bool SelectedIOPortFlag = false;
				bool SelectedNodeConnectionPointFlag = false;

				QList<ZEInt> SelectTopMost;
				ZEInt TopMost = 0;

				for (ZEInt I = 0; I < Nodes.count(); I++) //create qlist to find topmost item
				{
					if (Nodes[I]->sceneBoundingRect().contains(LastMousePos))
						SelectTopMost.append(Nodes[I]->zValue());
				}

				if (SelectTopMost.count() != 0)	//take mosttop from qlist
				{
					qSort(SelectTopMost.begin(), SelectTopMost.end());
					TopMost = SelectTopMost.takeLast();
				}

				for (ZEInt I = 0; I < Nodes.count(); I++)
				{
					if (Nodes[I]->sceneBoundingRect().contains(LastMousePos))
					{
						if (Nodes[I]->GetConnectionPoint()->sceneBoundingRect().contains(LastMousePos) && Nodes[I]->zValue() == TopMost && SelectedNodeConnectionPointFlag == false)
						{
							if (TempConnection->GetFromNode() != Nodes[I])
							{
								SelectedItems.append((ZEDNodeEditorItem*)(Nodes[I]->GetConnectionPoint()));
								Nodes[I]->GetConnectionPoint()->SetSelected(true);

								SelectedNodeConnectionPointFlag = true;
							}
						}
						else if (!Nodes[I]->GetConnectionPoint()->sceneBoundingRect().contains(LastMousePos) )
						{
							for (ZEInt J = 0; J < Nodes[I]->GetIOPorts()->count(); J++)
							{
								if (Nodes[I]->GetIOPorts()->value(J)->sceneBoundingRect().contains(LastMousePos)  && Nodes[I]->zValue() == TopMost && SelectedIOPortFlag == false)
								{
									if (TempConnection->GetFromNode() != Nodes[I])
									{
										SelectedItems.append((ZEDNodeEditorItem*)(Nodes[I]->GetIOPorts()->value(J)));
										Nodes[I]->GetIOPorts()->value(J)->SetSelected(true);

										SelectedIOPortFlag = true;
									}
								}
								else if (Nodes[I] != TempConnection->GetFromNode() )
								{
									Nodes[I]->GetConnectionPoint()->SetSelected(false);
									Nodes[I]->GetIOPorts()->value(J)->SetSelected(false);
								}
							}
						}
						else if (Nodes[I]->GetConnectionPoint() != TempConnection->GetFromNode()->GetConnectionPoint() )
						{
							Nodes[I]->GetConnectionPoint()->SetSelected(false);
						}
						
					}
					else 
					{
						for (ZEInt J = 0; J < Nodes[I]->GetIOPorts()->count(); J++)
							Nodes[I]->GetIOPorts()->value(J)->SetSelected(false);

						if (Nodes[I]->GetConnectionPoint() != TempConnection->GetFromNode()->GetConnectionPoint())
						{
							Nodes[I]->GetConnectionPoint()->SetSelected(false);
						}
					}
				}

				InitialMousePos = LastMousePos;

			}
			else if (SelectedItemsIncludeNodeConnectionPoint == true && (IsDeselectionEnabled == true || IsMultipleSelectionEnabled == true) )
			{
				for (ZEInt I = 0; I < Nodes.count(); I++)
				{
					for (ZEInt J = 0; J < Nodes[I]->GetIOPorts()->count(); J++)
					{
						Nodes[I]->GetIOPorts()->value(J)->SetSelected(false);
					}

					Nodes[I]->GetConnectionPoint()->SetSelected(false);
				}				
				SelectedItemsIncludeNodeConnectionPoint = false;
				SelectedItems.clear();
				delete(TempConnection);
			}
			else if (SelectedItemsIncludeIOPort == true && (IsDeselectionEnabled == true || IsMultipleSelectionEnabled == true) )
			{
				for (ZEInt I = 0; I < Nodes.count(); I++)
				{
					for (ZEInt J = 0; J < Nodes[I]->GetIOPorts()->count(); J++)
					{
						Nodes[I]->GetIOPorts()->value(J)->SetSelected(false);
					}
				}
				SelectedItemsIncludeIOPort = false;
				SelectedItems.clear();
				delete(TempConnection);
			}

		}
		else if (IsRubberBandSelectionOnGoing == true)
		{
			LastMousePos = mapToScene(Event->pos());			
			RubberBandSelectionRectItem->setRect(QRectF(InitialMousePos,LastMousePos));
			RubberBandSelectionRectItem->setPen(Qt::DashLine);
			RubberBandSelectionRectItem->setZValue(MAX_Z_VALUE);			
		}
	}

	else if((Event->buttons() & Qt::MidButton) == Qt::MidButton && (Event->buttons() & Qt::XButton2) != Qt::XButton2&& (Event->buttons() & Qt::XButton1) != Qt::XButton1)
	{
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (Event->x() - PanStartX)); 
		verticalScrollBar()->setValue(verticalScrollBar()->value() - (Event->y() - PanStartY)); 
		PanStartX = Event->x(); 
		PanStartY = Event->y(); 
		Event->accept(); 
		return; 
	}
	QGraphicsView::mouseMoveEvent(Event);
}

void ZEDNodeEditorGraphicsView::wheelEvent(QWheelEvent *Event)
{
	bool DoScaling;
	TempRect->setRect(sceneRect());
	TempRect->scale(pow((double)2, Event->delta() / 120.0), pow((double)2, Event->delta() / 120.0));

	if (TempRect->transform().m11() > 4)
	{
		TempRect->scale(pow((double)2, Event->delta() / -120.0), pow((double)2, Event->delta() / -120.0));
		DoScaling = false;
	}
	else if (TempRect->transform().m11() < 0.10)
	{
		TempRect->scale(pow((double)2, Event->delta() / -120.0), pow((double)2, Event->delta() / -120.0));
		DoScaling = false;
	}
	else
		DoScaling = true;

	if (TempRect->transform().m11() <= 0.25)
	{
		for (ZEInt I = 0; I < Nodes.count(); I++)// set names visibility after a scaling treshold
		{	
			for (ZEInt J = 0; J < Nodes[I]->GetIOPorts()->count(); J++)
			{
				Nodes[I]->GetIOPorts()->value(J)->GetIOPortName()->setVisible(false);
			}
		}
	}
	else if (TempRect->transform().m11() > 0.25)
	{
		for (ZEInt I = 0; I < Nodes.count(); I++)// set names visibility after a scaling treshold
		{	
			for (ZEInt J = 0; J < Nodes[I]->GetIOPorts()->count(); J++)
			{
				Nodes[I]->GetIOPorts()->value(J)->GetIOPortName()->setVisible(true);
			}
		}
	}

	if (DoScaling)
	{
		scale(pow((double)2, Event->delta() / 120.0), pow((double)2, Event->delta() / 120.0));	
		GridSpacing = GridSpacing / pow((double)2, Event->delta() / 120);

		for (ZEInt I = 0; I < Nodes.count(); I++)// resize all the gizmo points
		{	
			for (ZEInt J = 0; J < Nodes[I]->GetScaleGizmo()->GetGizmoPointList()->count(); J++)
			{
				//Nodes[I]->GetScaleGizmo()->GetGizmoPointList()->value(J)->scale(pow((double)2, Event->delta() / -120.0), pow((double)2, Event->delta() / -120.0));
				Nodes[I]->GetScaleGizmo()->Update();
				ScaleCoefficient = Nodes[I]->GetScaleGizmo()->GetGizmoPointList()->value(J)->transform().m11();
			}
		}
	}	
	//QGraphicsView::wheelEvent(Event);
}

void ZEDNodeEditorGraphicsView::keyPressEvent(QKeyEvent *Event)
{
	if(Event->key() == Qt::Key_Control ) // multi selection
		IsMultipleSelectionEnabled = true;

	else if(Event->key() == Qt::Key_Alt) // deselection
		IsDeselectionEnabled = true;
	
	if (Event->key() == Qt::Key_PageUp) // bring to front
		BringForward();

	else if (Event->key() == Qt::Key_PageDown) //send to back
		SendBackward();

	else if (Event->key() == Qt::Key_Delete) 
	{
		DeleteSelectedItems();
	}
	else if (Event->matches(QKeySequence::Undo))
	{
		if (IsMouseMoving == false)
			UndoRedoManager->Undo();
	}
	else if (Event->matches(QKeySequence::Redo))
	{
		if (IsMouseMoving == false)
			UndoRedoManager->Redo();
	}
	else if (Event->matches(QKeySequence::SelectAll))
	{
		SelectAllItems();
	}

	//QGraphicsView::keyPressEvent(Event);
}
void ZEDNodeEditorGraphicsView::keyReleaseEvent(QKeyEvent *Event)
{
	if(Event->key() == Qt::Key_Control)
		IsMultipleSelectionEnabled = false;

	if(Event->key() == Qt::Key_Alt)
		IsDeselectionEnabled = false;

	QGraphicsView::keyReleaseEvent(Event);
}

void ZEDNodeEditorGraphicsView::resizeEvent(QResizeEvent *Event)
{
	QGraphicsView::resizeEvent(Event);
}
void ZEDNodeEditorGraphicsView::drawBackground (QPainter* Painter, const QRectF &rect)
{
	QGraphicsView::drawBackground(Painter,rect);

	ZEInt topLeftGridX = rect.topLeft().toPoint().x()- GridSpacing - (rect.topLeft().toPoint().x() % GridSpacing);
	ZEInt topLeftGridY = rect.topLeft().toPoint().y()- GridSpacing - (rect.topLeft().toPoint().y() % GridSpacing);	
	ZEInt bottomRightGridX = rect.bottomRight().toPoint().x() + GridSpacing - (rect.bottomRight().toPoint().x() % GridSpacing);
	ZEInt bottomRightGridY = rect.bottomRight().toPoint().y() + GridSpacing - (rect.bottomRight().toPoint().y() % GridSpacing);	
	Painter->setPen(Qt::darkGray);

	for (ZEInt I = topLeftGridX; I < bottomRightGridX; I = I + GridSpacing)
	{
		Painter->drawLine(I , rect.topLeft().y() , I , rect.bottomRight().y());
	}
	for (ZEInt I = topLeftGridY; I < bottomRightGridY; I = I + GridSpacing)
	{
		Painter->drawLine(rect.topLeft().x() , I , rect.bottomRight().x(), I);
	}	

	setSceneRect(MainScene->itemsBoundingRect());
}
void ZEDNodeEditorGraphicsView::RubberBandSelection()
{
	if (RubberBandSelectionMode == ZED_NEGV_RBSM_INTERSECTS)
	{
		ZEDUndoRedoOperation* UndoRedoOperation = ZEDUndoRedoOperation::CreateInstance();

		if (IsMultipleSelectionEnabled == true && IsDeselectionEnabled == false)
		{					
			for (ZEInt I = 0; I < Nodes.count(); I++)
			{
				if (Nodes[I]->sceneBoundingRect().intersects(RubberBandSelectionRectItem->rect()))
				{
					Nodes[I]->SetSelected(true);

					SelectItem((ZEDNodeEditorItem*)(Nodes[I]), UndoRedoOperation);						

					SelectedNodes.append(Nodes[I]);
					emit OnNodeSelection(&SelectedNodes);
				}
			}

			for (ZEInt I = 0; I < Connections.count(); I++)
			{
				if(Connections[I]->collidesWithItem(RubberBandSelectionRectItem, Qt::IntersectsItemShape) || Connections[I]->ArrowPolygon->collidesWithItem(RubberBandSelectionRectItem, Qt::IntersectsItemShape))
				{
					Connections[I]->SetSelected(true);
					Connections[I]->setZValue(0);
					SelectItem((ZEDNodeEditorItem*)(Connections[I]), UndoRedoOperation);

					SelectedConnections.append(Connections[I]);
					emit OnConnectionSelection(&SelectedConnections);
				}
			}
		}

		else if (IsMultipleSelectionEnabled == false && IsDeselectionEnabled == true)
		{
			for (ZEInt I = 0; I < Nodes.count(); I++)
			{
				if (Nodes[I]->sceneBoundingRect().intersects(RubberBandSelectionRectItem->rect()))
				{
					Nodes[I]->SetSelected(false);
					DeselectItem((ZEDNodeEditorItem*)(Nodes[I]), UndoRedoOperation);

					SelectedNodes.removeOne(Nodes[I]);
					emit OnNodeSelection(&SelectedNodes);
				}
			}
			for (ZEInt I = 0; I < Connections.count(); I++)
			{
				if(Connections[I]->collidesWithItem(RubberBandSelectionRectItem, Qt::IntersectsItemShape) || Connections[I]->ArrowPolygon->collidesWithItem(RubberBandSelectionRectItem, Qt::IntersectsItemShape))
				{
					Connections[I]->SetSelected(false);
					Connections[I]->setZValue(-1);
					DeselectItem((ZEDNodeEditorItem*)(Connections[I]), UndoRedoOperation);

					SelectedConnections.removeOne(Connections[I]);
					emit OnConnectionSelection(&SelectedConnections);
				}
			}
		}
		else if (IsMultipleSelectionEnabled == false && IsDeselectionEnabled == false)
		{
			MutualDeselection(UndoRedoOperation);
			for (ZEInt I = 0; I < Nodes.count(); I++)
			{
				if (Nodes[I]->sceneBoundingRect().intersects(RubberBandSelectionRectItem->rect()))
				{
					Nodes[I]->SetSelected(true);
					SelectItem((ZEDNodeEditorItem*)(Nodes[I]), UndoRedoOperation);

					SelectedNodes.append(Nodes[I]);
					emit OnNodeSelection(&SelectedNodes);
				}
			}	
			for (ZEInt I = 0; I < Connections.count(); I++)
			{
				if(Connections[I]->collidesWithItem(RubberBandSelectionRectItem, Qt::IntersectsItemShape) || Connections[I]->ArrowPolygon->collidesWithItem(RubberBandSelectionRectItem, Qt::IntersectsItemShape))
				{
					Connections[I]->SetSelected(true);
					Connections[I]->setZValue(0);
					SelectItem((ZEDNodeEditorItem*)(Connections[I]), UndoRedoOperation);

					SelectedConnections.append(Connections[I]);
					emit OnConnectionSelection(&SelectedConnections);
				}
			}
		}

		if (!UndoRedoOperation->ChildOperations.isEmpty())
			UndoRedoManager->RegisterUndoRedoOperation(UndoRedoOperation);
		else
			UndoRedoOperation->Destroy();		
	}
	else if (RubberBandSelectionMode == ZED_NEGV_RBSM_INSIDE)
	{
		ZEDUndoRedoOperation* UndoRedoOperation = ZEDUndoRedoOperation::CreateInstance();
		if (IsMultipleSelectionEnabled == true && IsDeselectionEnabled == false)
		{
			for (ZEInt I = 0; I < Nodes.count(); I++)
			{
				if (RubberBandSelectionRectItem->rect().contains(Nodes[I]->sceneBoundingRect()))
				{
					Nodes[I]->SetSelected(true);
					SelectItem((ZEDNodeEditorItem*)(Nodes[I]), UndoRedoOperation);

					SelectedNodes.append(Nodes[I]);
					emit OnNodeSelection(&SelectedNodes);
				}
			}	
			for (ZEInt I = 0; I < Connections.count(); I++)
			{
				if(Connections[I]->collidesWithItem(RubberBandSelectionRectItem, Qt::ContainsItemShape) || Connections[I]->ArrowPolygon->collidesWithItem(RubberBandSelectionRectItem, Qt::ContainsItemShape))
				{
					Connections[I]->SetSelected(true);
					Connections[I]->setZValue(0);
					SelectItem((ZEDNodeEditorItem*)(Connections[I]), UndoRedoOperation);

					SelectedConnections.append(Connections[I]);
					emit OnConnectionSelection(&SelectedConnections);
				}
			}
		}

		else if (IsMultipleSelectionEnabled == false && IsDeselectionEnabled == true)
		{
			for (ZEInt I = 0; I < Nodes.count(); I++)
			{
				if (RubberBandSelectionRectItem->rect().contains(Nodes[I]->sceneBoundingRect()))
				{
					Nodes[I]->SetSelected(false);
					DeselectItem((ZEDNodeEditorItem*)(Nodes[I]), UndoRedoOperation);

					SelectedNodes.removeOne(Nodes[I]);
					emit OnNodeSelection(&SelectedNodes);
				}
			}
			for (ZEInt I = 0; I < Connections.count(); I++)
			{
				if(Connections[I]->collidesWithItem(RubberBandSelectionRectItem, Qt::ContainsItemShape) || Connections[I]->ArrowPolygon->collidesWithItem(RubberBandSelectionRectItem, Qt::ContainsItemShape))
				{
					Connections[I]->SetSelected(false);
					Connections[I]->setZValue(-1);
					DeselectItem((ZEDNodeEditorItem*)(Connections[I]), UndoRedoOperation);

					SelectedConnections.removeOne(Connections[I]);
					emit OnConnectionSelection(&SelectedConnections);
				}
			}
		}
		else if (IsMultipleSelectionEnabled == false && IsDeselectionEnabled == false)
		{
			MutualDeselection(UndoRedoOperation);
			for (ZEInt I = 0; I < Nodes.count(); I++)
			{
				if (RubberBandSelectionRectItem->rect().contains(Nodes[I]->sceneBoundingRect()))
				{
					Nodes[I]->SetSelected(true);
					SelectItem((ZEDNodeEditorItem*)(Nodes[I]), UndoRedoOperation);

					SelectedNodes.append(Nodes[I]);
					emit OnNodeSelection(&SelectedNodes);
				}
			}	
			for (ZEInt I = 0; I < Connections.count(); I++)
			{
				if(Connections[I]->collidesWithItem(RubberBandSelectionRectItem, Qt::ContainsItemShape) || Connections[I]->ArrowPolygon->collidesWithItem(RubberBandSelectionRectItem, Qt::ContainsItemShape))
				{
					Connections[I]->SetSelected(true);
					Connections[I]->setZValue(0);
					SelectItem((ZEDNodeEditorItem*)(Connections[I]), UndoRedoOperation);

					SelectedConnections.append(Connections[I]);
					emit OnConnectionSelection(&SelectedConnections);
				}
			}
		}
		if (!UndoRedoOperation->ChildOperations.isEmpty())
			UndoRedoManager->RegisterUndoRedoOperation(UndoRedoOperation);
		else
			UndoRedoOperation->Destroy();
	}	

	MainScene->removeItem(RubberBandSelectionRectItem);
	IsRubberBandSelectionOnGoing = false;
}
void ZEDNodeEditorGraphicsView::SelectItem(ZEDNodeEditorItem* Item, ZEDUndoRedoOperation* UndoRedoOperation )
{
	if (!SelectedItems.contains(Item) && Item->GetItemType() != ZED_NEIT_IOPORT && Item->GetItemType() != ZED_NEIT_NODE_CONNECTION_POINT) // for selection undo/redo
	{
		ZEDNodeEditorItemSelectionOperation* ItemSelection = ZEDNodeEditorItemSelectionOperation::CreateInstance();
		ItemSelection->SetView(this);
		ItemSelection->SetSelectedItem(Item);
		UndoRedoOperation->AddChildOperation(ItemSelection);
	}

	if (!SelectedItems.contains(Item))
	{
		SelectedItems.append(Item);
		Item->SetSelected(true);
	}
}
void ZEDNodeEditorGraphicsView::DeselectItem(ZEDNodeEditorItem* Item, ZEDUndoRedoOperation* DeselectionUndoRedoOperation)
{
	if (!SelectedItems.isEmpty() && Item->GetItemType() != ZED_NEIT_IOPORT && Item->GetItemType() != ZED_NEIT_NODE_CONNECTION_POINT) // for deselection undo/redo
	{
		ZEDNodeEditorItemDeselectionOperation* ItemDeselection = ZEDNodeEditorItemDeselectionOperation::CreateInstance();
		ItemDeselection->SetView(this);
		ItemDeselection->SetDeselectedItem(Item);
		DeselectionUndoRedoOperation->AddChildOperation(ItemDeselection);
	}

	if (!SelectedItems.isEmpty())
	{
		Item->SetSelected(false);
		SelectedItems.removeOne(Item);
	}
}
void ZEDNodeEditorGraphicsView::MutualDeselection(ZEDUndoRedoOperation* DeselectionUndoRedoOperation)
{
	if (!SelectedItems.isEmpty()) // for mutual deselection undo/redo
	{
		for (ZEInt I = 0; I < SelectedItems.count(); I++)
		{
			if (SelectedItems[I]->GetItemType() != ZED_NEIT_IOPORT && SelectedItems[I]->GetItemType() != ZED_NEIT_NODE_CONNECTION_POINT)
			{
				ZEDNodeEditorItemDeselectionOperation* ItemDeselection = ZEDNodeEditorItemDeselectionOperation::CreateInstance();
				ItemDeselection->SetView(this);
				ItemDeselection->SetDeselectedItem(SelectedItems[I]);
				DeselectionUndoRedoOperation->AddChildOperation(ItemDeselection);
			}
		}		
	}
	SelectedItems.clear();
	
}
void ZEDNodeEditorGraphicsView::SetNewConnection(ZEDNodeEditorNode* FromNode, ZEDNodeIOPort* FromPort)
{
	TempConnection = new ZEDIOPortConnection(NULL);							
	TempConnection->SetFrom(FromPort);
	TempConnection->SetFromNode(FromNode);
	TempConnection->SetDirected(true);
	MainScene->addItem(TempConnection);

	if (FromPort == NULL)
	{
		ConnectionPointFromX = SelectedNode->GetConnectionPoint()->sceneBoundingRect().topLeft().x() + SelectedNode->GetConnectionPoint()->sceneBoundingRect().width() / 2;
		ConnectionPointFromY = SelectedNode->GetConnectionPoint()->sceneBoundingRect().topLeft().y();
	}
	else
	{
		ConnectionPointFromX = TempConnection->GetFrom()->sceneBoundingRect().topLeft().x() + TempConnection->GetFrom()->sceneBoundingRect().height() / 4;
		ConnectionPointFromY = TempConnection->GetFrom()->sceneBoundingRect().topLeft().y() + TempConnection->GetFrom()->sceneBoundingRect().height() / 2;
	}

}

ZEInt ZEDNodeEditorGraphicsView::SelectTopMostNodeZValue(QPointF MousePosition)
{
	QList<ZEInt> SelectTopMostZvalue;
	ZEInt TopMostNodeZValue = 0;
	SelectTopMostZvalue.clear();
	
	if (!Nodes.isEmpty())
	{
		for (ZEInt I = 0; I < Nodes.count(); I++) //create qlist to find topmost item
		{	
			if (Nodes[I]->sceneBoundingRect().contains(MousePosition)) // append all nodes to List that includes MousePos to find topmost
				SelectTopMostZvalue.append(Nodes[I]->zValue());
		}

		if (SelectTopMostZvalue.count() != 0)	//take mosttop from qlist
		{
			qSort(SelectTopMostZvalue.begin(), SelectTopMostZvalue.end());
			TopMostNodeZValue = SelectTopMostZvalue.takeLast();
		}
	}

	return TopMostNodeZValue;
}
void ZEDNodeEditorGraphicsView::SendBackward()
{
	bool SendBackwardAvailable = true;

	for (ZEInt I = 0; I < SelectedNodes.count(); I++)
	{
		if (SelectedNodes[I]->zValue() < 2)
		{
			SendBackwardAvailable = false;
		}		
	}

	if (SendBackwardAvailable)
	{
		ZEDUndoRedoOperation* UndoRedoOperation = ZEDUndoRedoOperation::CreateInstance();

		for (ZEInt I = 0; I < SelectedNodes.count(); I++)
		{
			for (ZEInt J = 0; J < Nodes.count(); J++)
			{
				if (Nodes[J]->zValue() == SelectedNodes[I]->zValue() - 1)
					Nodes[J]->setZValue(SelectedNodes[I]->zValue());
			}
			ZEDNodeEditorSendBackwardOperation* SendingBackward = ZEDNodeEditorSendBackwardOperation::CreateInstance();

			SendingBackward->SetView(this);
			SendingBackward->SetNodeSendBackward(SelectedNodes[I]);
			UndoRedoOperation->AddChildOperation(SendingBackward);

			SelectedNodes[I]->setZValue(SelectedNodes[I]->zValue() - 1);	
		}

		UndoRedoManager->RegisterUndoRedoOperation(UndoRedoOperation);
	}
}
void ZEDNodeEditorGraphicsView::BringForward()
{
	bool BringForwardAvailable = true;

	for (ZEInt I = 0; I < SelectedNodes.count(); I++)
	{
		if (SelectedNodes[I]->zValue() > Nodes.count() - 1)
		{
			BringForwardAvailable = false;
		}		
	}

	if (BringForwardAvailable)
	{
		ZEDUndoRedoOperation* UndoRedoOperation = ZEDUndoRedoOperation::CreateInstance();

		for (ZEInt I = 0; I < SelectedNodes.count(); I++)
		{
			for (ZEInt J = 0; J < Nodes.count(); J++)
			{
				if (Nodes[J]->zValue() == SelectedNodes[I]->zValue() + 1)
					Nodes[J]->setZValue(SelectedNodes[I]->zValue());
			}
			ZEDNodeEditorBringForwardOperation* BringingForward = ZEDNodeEditorBringForwardOperation::CreateInstance();

			BringingForward->SetView(this);
			BringingForward->SetNodeBringForward(SelectedNodes[I]);
			UndoRedoOperation->AddChildOperation(BringingForward);

			SelectedNodes[I]->setZValue(SelectedNodes[I]->zValue() + 1);	
		}

		UndoRedoManager->RegisterUndoRedoOperation(UndoRedoOperation);
	}
}
void ZEDNodeEditorGraphicsView::DeleteSelectedItems()
{
	bool IsDeletePerformed = false;

	if (!IsMouseMoving && !SelectedItems.isEmpty())
	{
		for (ZEInt I = 0; I < SelectedItems.count(); I++) // rearrange selected items which are nodes, zvalues descenting order
		{
			if (SelectedItems[I]->GetItemType() == ZED_NEIT_NODE)
			{		
				for (ZEInt J = I; J < SelectedItems.count(); J++)
				{
					if (SelectedItems[J]->GetItemType() == ZED_NEIT_NODE)
					{			
						if (((ZEDNodeEditorNode*)(SelectedItems[I]))->zValue() < ((ZEDNodeEditorNode*)(SelectedItems[J]))->zValue())
						{
							SelectedItems.swap(I,J);
						}
					}
				}
			}
		}

		ZEDUndoRedoOperation* UndoRedoOperation = ZEDUndoRedoOperation::CreateInstance();

		for (ZEInt I = 0; I < SelectedItems.count(); I++)
		{
			if (SelectedItems[I]->GetItemType() == ZED_NEIT_IOPORT)
			{			
				QList<ZEDIOPortConnection*> TempConnections = Connections;

				for (ZEInt J = 0; J < TempConnections.count(); J++)
				{
					if (TempConnections[J]->GetFrom() == (ZEDNodeIOPort*)(SelectedItems[I]) || TempConnections[J]->GetTo() == (ZEDNodeIOPort*)(SelectedItems[I]))
					{
						bool DeletingConnectionsValidation = true;

						emit OnConnectionDeleting(TempConnections[J], DeletingConnectionsValidation);

						if (DeletingConnectionsValidation)
						{
							DeleteConnection(TempConnections[J]);

							ZEDNodeEditorDeleteConnectionOperation* ConnectionDelete = ZEDNodeEditorDeleteConnectionOperation::CreateInstance();
							ConnectionDelete->SetView(this);
							ConnectionDelete->SetDeletedConnection(TempConnections[J]);
							UndoRedoOperation->AddChildOperation(ConnectionDelete);

							IsDeletePerformed = true;
							emit OnConnectionDeleted(TempConnections[J]);
						}
					}
				}
			}
			else if (SelectedItems[I]->GetItemType() == ZED_NEIT_NODE)
			{
				bool DeletingNodesValidation = true;

				emit OnNodeDeleting((ZEDNodeEditorNode*)(SelectedItems[I]), DeletingNodesValidation);

				if (DeletingNodesValidation)
				{
					QList<ZEDIOPortConnection*> TempConnections = Connections;

					for (ZEInt J = 0; J < TempConnections.count(); J++)
					{
						if (TempConnections[J]->GetFromNode() == (ZEDNodeEditorNode*)(SelectedItems[I]) || TempConnections[J]->GetToNode() == (ZEDNodeEditorNode*)(SelectedItems[I]))
						{
							for (ZEInt K = SelectedItems.count() - 1; K > I; K--)
							{
								if (SelectedItems[K] == ((ZEDNodeEditorItem*)(TempConnections[J])))
								{
									SelectedItems[K]->SetSelected(false);
									SelectedItems[K] = NULL;
									SelectedItems.removeAt(K);
								}
							}
							DeleteConnection(TempConnections[J]);

							ZEDNodeEditorDeleteConnectionOperation* ConnectionDelete = ZEDNodeEditorDeleteConnectionOperation::CreateInstance();
							ConnectionDelete->SetView(this);
							ConnectionDelete->SetDeletedConnection(TempConnections[J]);
							UndoRedoOperation->AddChildOperation(ConnectionDelete);

							IsDeletePerformed = true;
							emit OnConnectionDeleted(TempConnections[J]);
						}
					}

					DeleteNode((ZEDNodeEditorNode*)(SelectedItems[I]));	

					ZEDNodeEditorDeleteNodeOperation* NodeDelete = ZEDNodeEditorDeleteNodeOperation::CreateInstance();
					NodeDelete->SetView(this);
					NodeDelete->SetDeletedNode((ZEDNodeEditorNode*)(SelectedItems[I]));
					UndoRedoOperation->AddChildOperation(NodeDelete);

					IsDeletePerformed = true;
					emit OnNodeDeleted((ZEDNodeEditorNode*)(SelectedItems[I]));
				}		
			}
			else if (SelectedItems[I]->GetItemType() == ZED_NEIT_CONNECTION )
			{
				bool DeletingConnectionsValidation = true;

				emit OnConnectionDeleting((ZEDIOPortConnection*)(SelectedItems[I]), DeletingConnectionsValidation);

				if (DeletingConnectionsValidation)
				{
					DeleteConnection((ZEDIOPortConnection*)(SelectedItems[I]));

					ZEDNodeEditorDeleteConnectionOperation* ConnectionDelete = ZEDNodeEditorDeleteConnectionOperation::CreateInstance();
					ConnectionDelete->SetView(this);
					ConnectionDelete->SetDeletedConnection((ZEDIOPortConnection*)(SelectedItems[I]));
					UndoRedoOperation->AddChildOperation(ConnectionDelete);

					IsDeletePerformed = true;
					emit OnConnectionDeleted((ZEDIOPortConnection*)(SelectedItems[I]));
				}
			}
		}

		if(IsDeletePerformed)
			UndoRedoManager->RegisterUndoRedoOperation(UndoRedoOperation);
		else
			UndoRedoOperation->Destroy();

		for (ZEInt I = 0; I < SelectedItems.count(); I++)
		{
			SelectedItems[I]->SetSelected(false);
		}

		SelectedItems.clear();
		SelectedNodes.clear();
		SelectedConnections.clear();
	}
}

void ZEDNodeEditorGraphicsView::DeleteNode(ZEDNodeEditorNode* Node)
{	
	MainScene->removeItem(Node->GetScaleGizmo());
	MainScene->removeItem(Node);
	NodeZValue--;
		
	ZEInt RemovedNodeZValue = Node->zValue();

	Nodes.removeOne(Node);

	for (ZEInt I = 0; I < Nodes.count(); I++)
	{				
		if (Nodes[I]->zValue() > RemovedNodeZValue)
		{
			Nodes[I]->setZValue(Nodes[I]->zValue() - 1);
		}
	}
}
void ZEDNodeEditorGraphicsView::DeleteConnection(ZEDIOPortConnection* Connection)
{
	MainScene->removeItem(Connection);
	Connections.removeOne(Connection);
}
void ZEDNodeEditorGraphicsView::SelectAllItems()
{
	SelectedNodes.clear();
	SelectedItems.clear();
	SelectedConnections.clear();

	if (SelectedIOPort != NULL)
	{
		SelectedIOPort->SetSelected(false);
		SelectedIOPort = NULL;
	}

	ZEDUndoRedoOperation* UndoRedoOperation = ZEDUndoRedoOperation::CreateInstance();
	
	for (ZEInt I = 0; I < Nodes.count(); I++)
	{
		SelectedNodes.append(Nodes[I]);
		SelectItem((ZEDNodeEditorItem*)(Nodes[I]), UndoRedoOperation);
		Nodes[I]->SetSelected(true);		
	}
	for (ZEInt I = 0; I < Connections.count(); I++)
	{
		SelectedConnections.append(Connections[I]);
		SelectItem((ZEDNodeEditorItem*)(Connections[I]), UndoRedoOperation);
		Connections[I]->SetSelected(true);
	}

	if (!UndoRedoOperation->ChildOperations.isEmpty())
		UndoRedoManager->RegisterUndoRedoOperation(UndoRedoOperation);
	else
		UndoRedoOperation->Destroy();

	emit OnNodeSelection(&SelectedNodes);
	emit OnConnectionSelection(&SelectedConnections);
}

QList<QAction*>* ZEDNodeEditorGraphicsView::GetCustomViewContextMenuItems()
{
	return &ViewContextMenuItems;
}
void ZEDNodeEditorGraphicsView::AddCustomViewContextMenuItems(QList<QAction*> ContextMenuItems)
{
	for (ZEInt I = 0; I < ContextMenuItems.count(); I++)
	{
		ContextMenuItems[I] = new QAction(ContextMenuItems[I]->text(), ViewContextMenu);
		ViewContextMenu->addAction(ContextMenuItems[I]);
		ViewContextMenuItems.append(ContextMenuItems[I]);
	}
}
void ZEDNodeEditorGraphicsView::DisplayViewContextMenu(QPointF MousePos)
{
	QAction *selectedAction = ViewContextMenu->exec(MousePos.toPoint());
}
void ZEDNodeEditorGraphicsView::AddMultipleSelectionCustomContextMenuItems(QList<QAction*> ContextMenuItems)
{
	for (ZEInt I = 0; I < ContextMenuItems.count(); I++)
	{
		ContextMenuItems[I] = new QAction(ContextMenuItems[I]->text(), MultipleSelectionContextMenu);
		MultipleSelectionContextMenu->addAction(ContextMenuItems[I]);
		MultipleSelectionContextMenuItems.append(ContextMenuItems[I]);
	}
}
QList<QAction*>* ZEDNodeEditorGraphicsView::GetMultipleSelectionCustomContextMenuItems()
{
	return &MultipleSelectionContextMenuItems;
}
void ZEDNodeEditorGraphicsView::DisplayContextMenuForMultipleSelection(QPointF MousePos)
{
	QAction *SelectedAction = MultipleSelectionContextMenu->exec(MousePos.toPoint());
}
QGraphicsScene* ZEDNodeEditorGraphicsView::GetScene()
{
	return MainScene;
}
const QList<ZEDNodeEditorNode*>* ZEDNodeEditorGraphicsView::GetNodes()
{
	return &Nodes;
}
const QList<ZEDIOPortConnection*>* ZEDNodeEditorGraphicsView::GetConnections()
{
	return &Connections;
}

QList<ZEDNodeEditorNode*>* ZEDNodeEditorGraphicsView::GetSelectedNodes()
{
	return &SelectedNodes;
}
QList<ZEDIOPortConnection*>* ZEDNodeEditorGraphicsView::GetSelectedConnections()
{
	return &SelectedConnections;
}
QList<ZEDNodeEditorItem*>* ZEDNodeEditorGraphicsView::GetSelectedItems()
{
	return &SelectedItems;
}
bool ZEDNodeEditorGraphicsView::GetIsMultipleSelectionEnabled()
{
	return IsMultipleSelectionEnabled;
}
bool ZEDNodeEditorGraphicsView::GetIsDeselectionEnabled()
{
	return IsDeselectionEnabled;
}
void ZEDNodeEditorGraphicsView::SetRubberBandSelectionMode(ZEDNodeEditorGraphicsViewRubberBandSelectionMode Mode)
{
	RubberBandSelectionMode = Mode;
}
ZEDNodeEditorGraphicsViewRubberBandSelectionMode ZEDNodeEditorGraphicsView::GetRubberBandSelectionMode() const
{
	return RubberBandSelectionMode;
}

void ZEDNodeEditorGraphicsView::AddNode(ZEDNodeEditorNode* NewNode, QPointF Position)
{
	if(NewNode == NULL)
		return;

	ZEDNodeEditorNodeScaleGizmo* Gizmo = new ZEDNodeEditorNodeScaleGizmo(NewNode);
	NewNode->SetScaleGizmo(Gizmo);
	MainScene->addItem(Gizmo);
	Gizmo->setZValue(MAX_Z_VALUE);
	NewNode->setZValue(NodeZValue);
	MainScene->addItem(NewNode);
	NewNode->setPos(Position);
	Gizmo->setVisible(false);

	QTransform Matrix;
	Matrix.setMatrix(ScaleCoefficient,0,0,0,ScaleCoefficient,0,0,0,1);
	for (ZEInt J = 0; J < Gizmo->GetGizmoPointList()->count(); J++)
	{
		Gizmo->GetGizmoPointList()->value(J)->setTransform(Matrix);		
	}	

	Gizmo->Update();
	Gizmo->setPos(NewNode->scenePos());

	Nodes.append(NewNode);
	NodeZValue++;

	if (ScaleCoefficient > 2.01f)
	{
		for (ZEInt I = 0; I < Nodes.count(); I++)// set names visibility after a scaling treshold
		{	
			for (ZEInt J = 0; J < Nodes[I]->GetIOPorts()->count(); J++)
			{
				Nodes[I]->GetIOPorts()->value(J)->GetIOPortName()->setVisible(false);
			}
		}
	}

}

void ZEDNodeEditorGraphicsView::AddConnection(ZEDIOPortConnection* NewConnection)
{

}
void ZEDNodeEditorGraphicsView::AddNodeConnection(ZEDNodeEditorNode* FromNode, ZEDNodeEditorNode* ToNode, ZEDNodeIOPort* To, bool IsDirected)
{
	ZEDIOPortConnection* Connection = new ZEDIOPortConnection(NULL);
	QPen BlackPen(Qt::black);
	BlackPen.setWidth(0.5);	

	Connection->SetFrom(NULL);
	Connection->SetTo(To);
	Connection->SetToNode(ToNode);
	Connection->SetFromNode(FromNode);
	Connection->SetDirected(IsDirected);
	Connection->Update();
	Connection->UpdateBoundingRect();
	Connection->setZValue(-1);
	Connection->setPen(BlackPen);

	bool ConnectionAddingValidation = true;
	emit OnConnectionAdding(Connection, ConnectionAddingValidation);

	if(!ConnectionAddingValidation)
	{
		delete Connection;
		Connection = NULL;
		return;
	}
	else
	{
		MainScene->addItem(Connection);
		Connections.append(Connection);

		bool IsValid = true;

		for (ZEInt I = 0; I < Connections.count(); I++)
		{
			if(Connections[I] == Connection)
				continue;

			if((Connections[I]->GetFromNode() == FromNode && Connections[I]->GetTo() == To && Connections[I]->GetTo() != NULL) || (Connections[I]->GetFromNode() == FromNode && Connections[I]->GetToNode() == ToNode && To == NULL && Connections[I]->GetTo() == NULL))
				IsValid = false;
		}

		if(!IsValid)
		{
			Connection->setVisible(false);
			MainScene->removeItem(Connection);
			Connections.removeOne(Connection);
			delete Connection;
			Connection = NULL;
		}
		if(IsValid)
		{
			SelectedConnections.clear();

			ZEDUndoRedoOperation* UndoRedoOperation = ZEDUndoRedoOperation::CreateInstance();

			ZEDNodeEditorAddConnectionOperation* ConnectionAdd = ZEDNodeEditorAddConnectionOperation::CreateInstance();
			ConnectionAdd->SetView(this);
			ConnectionAdd->SetAddedConnection(Connection);
			UndoRedoOperation->AddChildOperation(ConnectionAdd);

			if (!UndoRedoOperation->ChildOperations.isEmpty())
				UndoRedoManager->RegisterUndoRedoOperation(UndoRedoOperation);
			else
				UndoRedoOperation->Destroy();
		}

		emit OnConnectionAdded(Connection);
	}
}

void ZEDNodeEditorGraphicsView::AddConnection(ZEDNodeIOPort* From, ZEDNodeIOPort* To, bool IsDirected)
{
	ZEDIOPortConnection* Connection = new ZEDIOPortConnection(NULL);
	QPen BlackPen(Qt::black);
	BlackPen.setWidth(0.5);	

	Connection->SetFrom(From);
	Connection->SetTo(To);
	Connection->SetToNode(To->GetParentNode());
	Connection->SetFromNode(From->GetParentNode());
	Connection->SetDirected(IsDirected);
	Connection->Update();
	Connection->UpdateBoundingRect();
	Connection->setZValue(-1);
	Connection->setPen(BlackPen);

	bool ConnectionAddingValidation = true;
	emit OnConnectionAdding(Connection, ConnectionAddingValidation);

	if(!ConnectionAddingValidation)
	{
		delete Connection;
		Connection = NULL;
		return;
	}
	else
	{
		MainScene->addItem(Connection);
		Connections.append(Connection);

		bool IsValid = true;

		for (ZEInt I = 0; I < Connections.count(); I++)
		{
			if(Connections[I] == Connection)
				continue;

			if((Connections[I]->GetTo() == To &&  Connections[I]->GetFrom() == From) || (Connections[I]->GetTo() == From &&  Connections[I]->GetFrom() == To))
				IsValid = false;
		}

		if(!IsValid)
		{
			Connection->setVisible(false);
			MainScene->removeItem(Connection);
			Connections.removeOne(Connection);
			delete Connection;
			Connection = NULL;
		}
		if(IsValid)
		{
			SelectedConnections.clear();

			ZEDUndoRedoOperation* UndoRedoOperation = ZEDUndoRedoOperation::CreateInstance();

			ZEDNodeEditorAddConnectionOperation* ConnectionAdd = ZEDNodeEditorAddConnectionOperation::CreateInstance();
			ConnectionAdd->SetView(this);
			ConnectionAdd->SetAddedConnection(Connection);
			UndoRedoOperation->AddChildOperation(ConnectionAdd);

			if (!UndoRedoOperation->ChildOperations.isEmpty())
				UndoRedoManager->RegisterUndoRedoOperation(UndoRedoOperation);
			else
				UndoRedoOperation->Destroy();
		}

		emit OnConnectionAdded(Connection);
	}

}



ZEDNodeEditorGraphicsView::ZEDNodeEditorGraphicsView(QWidget* Parent) : QGraphicsView(Parent)
{
	TempRect			= new QGraphicsRectItem();
	MainScene			= new QGraphicsScene(this);
	UndoRedoManager		= new ZEDUndoRedoManager();

	setScene(MainScene);
	MainScene->setBackgroundBrush(Qt::gray);
	horizontalScrollBar()->setFocusPolicy( Qt::NoFocus ); 
	verticalScrollBar()->setFocusPolicy(Qt::NoFocus);

	GridSpacing = 30;
	LastSelectedConnectionIndex = 0;
	ConnectionLoopControlInteger = 0;
	ScaleCoefficient = 1;
	NodeZValue = 1;

	IsMultipleSelectionEnabled = false;
	IsDeselectionEnabled = false;	
	SelectedItemsIncludeIOPort = false;
	SelectedItemsIncludeNodeConnectionPoint = false;
	IsMousePositionContainsGizmo = false;
	IsMouseMoving = false;
	IsRubberBandSelectionOnGoing = false;


	TempConnection = NULL; 
	SelectedGizmo = NULL;
	SelectedNode = NULL;
	SelectedIOPort = NULL;

	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	SetRubberBandSelectionMode(ZED_NEGV_RBSM_INTERSECTS);
	setViewportUpdateMode(ViewportUpdateMode::FullViewportUpdate);


	ViewContextMenu = new QMenu();
	QAction *SelectAll = new QAction("Select All", ViewContextMenu);

	ViewContextMenu->addAction(SelectAll);

	QObject::connect(SelectAll, SIGNAL(triggered()), this, SLOT(SelectAllItems()));

	MultipleSelectionContextMenu = new QMenu();
	QAction *RemoveNodes = new QAction("Remove Nodes", MultipleSelectionContextMenu);
	QAction *SendBackwardNodes = new QAction("Send Backward", MultipleSelectionContextMenu);
	QAction *BringForwardNodes = new QAction("Bring Forward", MultipleSelectionContextMenu);

	MultipleSelectionContextMenu->addAction(RemoveNodes);
	MultipleSelectionContextMenu->addSeparator();
	MultipleSelectionContextMenu->addAction(SendBackwardNodes);
	MultipleSelectionContextMenu->addAction(BringForwardNodes);

	QObject::connect(RemoveNodes, SIGNAL(triggered()), this, SLOT(DeleteNode()));
	QObject::connect(SendBackwardNodes, SIGNAL(triggered()), this, SLOT(SendBackward()));
	QObject::connect(BringForwardNodes, SIGNAL(triggered()), this, SLOT(BringForward()));

}
