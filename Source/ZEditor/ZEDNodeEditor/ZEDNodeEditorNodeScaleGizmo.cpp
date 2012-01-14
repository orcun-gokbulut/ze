//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDNodeEditorNodeScaleGizmo.cpp
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

#include <QBrush>
#include <QPen>
#include <QCursor>
#include <QRectF>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>

#include "ZEDNodeEditorNodeScaleGizmo.h"
#include "ZEDNodeEditorNodeScaleGizmoPoint.h"
#include "ZEDNodeEditorNode.h"

#define SCALE_GIZMO_DIMENSION 8.0f

void ZEDNodeEditorNodeScaleGizmo::Update()
{
	QRectF BoundingRect = ParentNode->ScaleRectItem->boundingRect();

	BlackDashPen.setWidthF(TopLeftPoint->sceneBoundingRect().width() / SCALE_GIZMO_DIMENSION);
	setPen(BlackDashPen);
	setPolygon(QRectF(BoundingRect.topLeft().x(), BoundingRect.topLeft().y(), ParentNode->ScaleRectItem->sceneBoundingRect().width(), ParentNode->ScaleRectItem->sceneBoundingRect().height()));

	TopLeftPoint->setPos(BoundingRect.topLeft().x(), BoundingRect.topLeft().y());
	TopRightPoint->setPos(BoundingRect.topRight().x(), BoundingRect.topRight().y());
	BottomLeftPoint->setPos(BoundingRect.bottomLeft().x(), BoundingRect.bottomLeft().y());
	BottomRightPoint->setPos(BoundingRect.bottomRight().x(),BoundingRect.bottomRight().y());
	TopPoint->setPos((BoundingRect.width()) / 2 + BoundingRect.topLeft().x(), BoundingRect.topLeft().y());
	BottomPoint->setPos((BoundingRect.width()) / 2 + BoundingRect.topLeft().x(), BoundingRect.bottomRight().y());
	RightPoint->setPos(BoundingRect.bottomRight().x(), (BoundingRect.height()) / 2 + BoundingRect.topLeft().y());
	LeftPoint->setPos(BoundingRect.bottomLeft().x(), (BoundingRect.height()) / 2 + BoundingRect.topLeft().y());

}
QList<ZEDNodeEditorNodeScaleGizmoPoint*>* ZEDNodeEditorNodeScaleGizmo::GetGizmoPointList()
{
	return &GizmoPointList;
}

ZEDNodeEditorItemType ZEDNodeEditorNodeScaleGizmo::GetItemType()
{
	return ZED_NEIT_SCALEGIZMO;
}
ZEDNodeEditorNodeScaleGizmo::ZEDNodeEditorNodeScaleGizmo(ZEDNodeEditorNode* Parent) : QGraphicsPolygonItem(NULL)
{
	ParentNode = Parent;

	TopLeftPoint		= new ZEDNodeEditorNodeScaleGizmoPoint(ParentNode, this, ZED_NENSGPP_TOP_LEFT);
	TopRightPoint		= new ZEDNodeEditorNodeScaleGizmoPoint(ParentNode, this, ZED_NENSGPP_TOP_RIGHT);
	BottomLeftPoint 	= new ZEDNodeEditorNodeScaleGizmoPoint(ParentNode, this, ZED_NENSGPP_BOTTOM_LEFT);
	BottomRightPoint	= new ZEDNodeEditorNodeScaleGizmoPoint(ParentNode, this, ZED_NENSGPP_BOTTOM_RIGHT);
	TopPoint			= new ZEDNodeEditorNodeScaleGizmoPoint(ParentNode, this, ZED_NENSGPP_TOP);
	BottomPoint			= new ZEDNodeEditorNodeScaleGizmoPoint(ParentNode, this, ZED_NENSGPP_BOTTOM);
	RightPoint			= new ZEDNodeEditorNodeScaleGizmoPoint(ParentNode, this, ZED_NENSGPP_RIGHT);
	LeftPoint			= new ZEDNodeEditorNodeScaleGizmoPoint(ParentNode, this, ZED_NENSGPP_LEFT);

	BlackDashPen.setColor(Qt::black);
	BlackDashPen.setStyle(Qt::DashLine);
	BlackDashPen.setWidthF(TopLeftPoint->sceneBoundingRect().width() / 10);
	setPen(BlackDashPen);
	setPolygon(QRectF(0, 0, ParentNode->sceneBoundingRect().width(), ParentNode->sceneBoundingRect().height()));

	QRectF BoundingRect = polygon().boundingRect();

	TopLeftPoint->setPos(BoundingRect.topLeft().x(),BoundingRect.topLeft().y());
	TopRightPoint->setPos(BoundingRect.topRight().x(),BoundingRect.topRight().y());
	BottomLeftPoint->setPos(BoundingRect.bottomLeft().x(), BoundingRect.bottomLeft().y());
	BottomRightPoint->setPos(BoundingRect.bottomRight().x(),BoundingRect.bottomRight().y());
	TopPoint->setPos((BoundingRect.width()) / 2 + BoundingRect.topLeft().x(), 0.0f);
	BottomPoint->setPos((BoundingRect.width()) / 2 + BoundingRect.topLeft().x(), BoundingRect.bottomRight().y());
	RightPoint->setPos(BoundingRect.bottomRight().x(), (BoundingRect.height()) / 2 + BoundingRect.topLeft().y());
	LeftPoint->setPos(BoundingRect.bottomLeft().x() , (BoundingRect.height()) / 2 + BoundingRect.topLeft().y());
	
	GizmoPointList.append(TopLeftPoint);
	GizmoPointList.append(TopRightPoint);
	GizmoPointList.append(BottomLeftPoint);
	GizmoPointList.append(BottomRightPoint);
	GizmoPointList.append(TopPoint);
	GizmoPointList.append(BottomPoint);
	GizmoPointList.append(RightPoint);
	GizmoPointList.append(LeftPoint);

}
