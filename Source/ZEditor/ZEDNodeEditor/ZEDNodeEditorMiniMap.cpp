//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDNodeEditorMiniMap.cpp
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

#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsItem>
#include <QtGui/QWidget>
#include <QtGui/QPolygonF>
#include <QtGui/QPen>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtCore/QPointF>
#include <QtGui/QCursor>
#include <QtGui/QScrollBar>
#include <QtCore/QDebug>
#include <QtGui/QTransform>

#include "ZEDNodeEditorMiniMap.h"

void ZEDNodeEditorMinimap::mousePressEvent(QMouseEvent* Event)
{
	if((Event->buttons() & Qt::LeftButton) == Qt::LeftButton)
	{
		MouseInitialPosition = mapToScene(Event->pos());
		SeenRectangleOnParentView = ParentView->mapToScene(ParentView->rect());

		QRect SeenRectangleOnViewRectangle = QRect(MouseInitialPosition.x() - SeenRectangleOnParentView.boundingRect().width() / 2, MouseInitialPosition.y() - SeenRectangleOnParentView.boundingRect().height() / 2, SeenRectangleOnParentView.boundingRect().width(), SeenRectangleOnParentView.boundingRect().height());

		ParentView->centerOn(SeenRectangleOnViewRectangle.center());
		SeenRectangleOnParentView = ParentView->mapToScene(ParentView->rect());
		
		MouseIsMoving = true;
	}

	QGraphicsView::mousePressEvent(Event);
}

void ZEDNodeEditorMinimap::mouseMoveEvent(QMouseEvent* Event)
{
	if((Event->buttons() & Qt::LeftButton) == Qt::LeftButton)
	{
		MouseLastPosition = mapToScene(Event->pos());
	
		SeenRectangleOnParentView.translate((MouseLastPosition.x() - MouseInitialPosition.x()), (MouseLastPosition.y() - MouseInitialPosition.y()));	

		QRectF SeenRectangleOnViewRectangle = (SeenRectangleOnParentView.boundingRect());
		ParentView->centerOn(SeenRectangleOnViewRectangle.center());

		MouseInitialPosition = MouseLastPosition;
	}

	QGraphicsView::mouseMoveEvent(Event);
}

void ZEDNodeEditorMinimap::mouseReleaseEvent(QMouseEvent* Event)
{
	MouseIsMoving = false;

	QGraphicsView::mouseReleaseEvent(Event);
}

void ZEDNodeEditorMinimap::wheelEvent(QWheelEvent* Event)
{
	
}

void ZEDNodeEditorMinimap::drawForeground (QPainter* Painter, const QRectF &Rect)
{
	if (!MouseIsMoving)
		SeenRectangleOnParentView = ParentView->mapToScene(ParentView->rect());

	Painter->setPen(Qt::green);
	Painter->drawPolygon(SeenRectangleOnParentView);
	
	fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);

	this->viewport()->update();

	//QGraphicsView::drawForeground(Painter, Rect);
}

ZEDNodeEditorMinimap::ZEDNodeEditorMinimap(QGraphicsView* View, QGraphicsScene* Scene, QWidget* Parent) : QGraphicsView(Parent)
{
	ParentScene = Scene;
	ParentView = View;
	this->Parent = Parent;

	SeenRectangleOnParentView = QPolygonF(0);
	MouseInitialPosition = QPointF(0.0f, 0.0f);
	MouseLastPosition = QPointF(0.0f, 0.0f);
	MouseIsMoving = false;

	horizontalScrollBar()->setFocusPolicy(Qt::NoFocus); 
	verticalScrollBar()->setFocusPolicy(Qt::NoFocus);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	setScene(ParentScene);

 	scene()->setBackgroundBrush(Qt::gray);

	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	setInteractive(false);
	
	if (Parent == NULL)
		setVisible(true);

	setGeometry(0, 0, 256, 256);

	setOptimizationFlags(OptimizationFlags() | QGraphicsView::DontAdjustForAntialiasing | QGraphicsView::DontClipPainter);	
}
