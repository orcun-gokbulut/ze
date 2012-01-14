//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDNodeEditorNodeScaleGizmoPoint.cpp
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

#include <QCursor>
#include <QBrush>
#include <QDebug>

#include "ZEDNodeEditorNodeScaleGizmoPoint.h"
#include "ZEDNodeEditorNodeScaleGizmo.h"
#include "ZEDNodeEditorNode.h"

#define SCALE_GIZMO_DIMENSION 8

void ZEDNodeEditorNodeScaleGizmoPoint::mouseMoveEvent(QGraphicsSceneMouseEvent* Event)
{
	if ((Event->buttons() & Qt::LeftButton) == Qt::LeftButton)
		QPointF DeltaMousePos = OldMousePositionScaleRect - Event->scenePos();

	OldMousePositionScaleRect = Event->scenePos();
	QGraphicsPolygonItem::mouseMoveEvent(Event);
}

void ZEDNodeEditorNodeScaleGizmoPoint::mousePressEvent(QGraphicsSceneMouseEvent *Event)
{
	if (Event->button() == Qt::LeftButton)
	{		
		OldMousePosition = Event->scenePos();	
		OldMousePositionScaleRect = Event->scenePos();
	}
	QGraphicsPolygonItem::mousePressEvent(Event);
}
void ZEDNodeEditorNodeScaleGizmoPoint::mouseReleaseEvent(QGraphicsSceneMouseEvent *Event)
{
	OldMousePosition = Event->scenePos();
	QGraphicsPolygonItem::mouseReleaseEvent(Event);
}
ZEDNodeEditorNodeScaleGizmoPointPosition ZEDNodeEditorNodeScaleGizmoPoint::GetGizmoPointPosition()
{
	return GizmoPointPosition;
}
ZEDNodeEditorNodeScaleGizmoPoint::ZEDNodeEditorNodeScaleGizmoPoint(ZEDNodeEditorNode* Parent, ZEDNodeEditorNodeScaleGizmo* ParentGizmo, ZEDNodeEditorNodeScaleGizmoPointPosition Position) : QGraphicsPolygonItem(ParentGizmo)
{
	this->ParentGizmo = ParentGizmo;
	ParentNode = Parent;
	GizmoPointPosition = Position;

	QPolygonF TempPolygon;
	QBrush brush(Qt::white);

	setBrush(brush);

	if(Position == ZED_NENSGPP_TOP_LEFT)
	{	
		TempPolygon.append(QPointF(-4.0f, -1.0f));
		TempPolygon.append(QPointF(-3.0f, -3.0f));
		TempPolygon.append(QPointF(-1.0f, -4.0f));
		TempPolygon.append(QPointF(1.0f, -4.0f));
		TempPolygon.append(QPointF(3.0f, -3.0f));
		TempPolygon.append(QPointF(4.0f, -1.0f));
		TempPolygon.append(QPointF(4.0f, 1.0f));
		TempPolygon.append(QPointF(3.0f, 3.0f));
		TempPolygon.append(QPointF(2.0f, 4.0f));
		TempPolygon.append(QPointF(-1.0f, 4.0f));
		TempPolygon.append(QPointF(-3.0f, 3.0f));
		TempPolygon.append(QPointF(-4.0f, 1.0f));
		TempPolygon.append(QPointF(-4.0f, -1.0f));
		setPolygon(TempPolygon);
		setCursor(Qt::SizeFDiagCursor);
	}

	else if(Position == ZED_NENSGPP_TOP)
	{
		setPolygon(QRectF( - SCALE_GIZMO_DIMENSION / 2, - SCALE_GIZMO_DIMENSION / 2, SCALE_GIZMO_DIMENSION, SCALE_GIZMO_DIMENSION));		
		setCursor(Qt::SizeVerCursor);
	}

	else if(Position == ZED_NENSGPP_TOP_RIGHT)
	{	
		TempPolygon.append(QPointF(-4.0f, -1.0f));
		TempPolygon.append(QPointF(-3.0f, -3.0f));
		TempPolygon.append(QPointF(-1.0f, -4.0f));
		TempPolygon.append(QPointF(1.0f, -4.0f));
		TempPolygon.append(QPointF(3.0f, -3.0f));
		TempPolygon.append(QPointF(4.0f, -1.0f));
		TempPolygon.append(QPointF(4.0f, 1.0f));
		TempPolygon.append(QPointF(3.0f, 3.0f));
		TempPolygon.append(QPointF(2.0f, 4.0f));
		TempPolygon.append(QPointF(-1.0f, 4.0f));
		TempPolygon.append(QPointF(-3.0f, 3.0f));
		TempPolygon.append(QPointF(-4.0f, 1.0f));
		TempPolygon.append(QPointF(-4.0f, -1.0f));
		setPolygon(TempPolygon);
		setCursor(Qt::SizeBDiagCursor);
	}

	else if(Position == ZED_NENSGPP_BOTTOM_LEFT)
	{	
		TempPolygon.append(QPointF(-4.0f, -1.0f));
		TempPolygon.append(QPointF(-3.0f, -3.0f));
		TempPolygon.append(QPointF(-1.0f, -4.0f));
		TempPolygon.append(QPointF(1.0f, -4.0f));
		TempPolygon.append(QPointF(3.0f, -3.0f));
		TempPolygon.append(QPointF(4.0f, -1.0f));
		TempPolygon.append(QPointF(4.0f, 1.0f));
		TempPolygon.append(QPointF(3.0f, 3.0f));
		TempPolygon.append(QPointF(2.0f, 4.0f));
		TempPolygon.append(QPointF(-1.0f, 4.0f));
		TempPolygon.append(QPointF(-3.0f, 3.0f));
		TempPolygon.append(QPointF(-4.0f, 1.0f));
		TempPolygon.append(QPointF(-4.0f, -1.0f));
		setPolygon(TempPolygon);
		setCursor(Qt::SizeBDiagCursor);
	}

	else if(Position == ZED_NENSGPP_BOTTOM)
	{
		setPolygon(QRectF( - SCALE_GIZMO_DIMENSION / 2, - SCALE_GIZMO_DIMENSION / 2, SCALE_GIZMO_DIMENSION, SCALE_GIZMO_DIMENSION));
		setCursor(Qt::SizeVerCursor);		
	}

	else if(Position == ZED_NENSGPP_BOTTOM_RIGHT)
	{	
		TempPolygon.append(QPointF(-4.0f, -1.0f));
		TempPolygon.append(QPointF(-3.0f, -3.0f));
		TempPolygon.append(QPointF(-1.0f, -4.0f));
		TempPolygon.append(QPointF(1.0f, -4.0f));
		TempPolygon.append(QPointF(3.0f, -3.0f));
		TempPolygon.append(QPointF(4.0f, -1.0f));
		TempPolygon.append(QPointF(4.0f, 1.0f));
		TempPolygon.append(QPointF(3.0f, 3.0f));
		TempPolygon.append(QPointF(2.0f, 4.0f));
		TempPolygon.append(QPointF(-1.0f, 4.0f));
		TempPolygon.append(QPointF(-3.0f, 3.0f));
		TempPolygon.append(QPointF(-4.0f, 1.0f));
		TempPolygon.append(QPointF(-4.0f, -1.0f));
		setPolygon(TempPolygon);
		setCursor(Qt::SizeFDiagCursor);
	}

	else if(Position == ZED_NENSGPP_RIGHT)
	{
		setPolygon(QRectF( - SCALE_GIZMO_DIMENSION / 2, - SCALE_GIZMO_DIMENSION / 2, SCALE_GIZMO_DIMENSION, SCALE_GIZMO_DIMENSION));	
		setCursor(Qt::SizeHorCursor);
	}

	else if(Position == ZED_NENSGPP_LEFT)
	{
		setPolygon(QRectF( - SCALE_GIZMO_DIMENSION / 2, - SCALE_GIZMO_DIMENSION / 2, SCALE_GIZMO_DIMENSION, SCALE_GIZMO_DIMENSION));
		setCursor(Qt::SizeHorCursor);	
	}

	setFlags(flags() | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIgnoresTransformations);

}
