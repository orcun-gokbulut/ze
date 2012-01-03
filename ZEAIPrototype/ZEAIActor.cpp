//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEAIActor.cpp
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

#include "ZEAIActor.h"
#include "ZEAI\ZESteering.h"
#include "ZEAIMainWindow.h"
#include "ui_ZEAIMainWindow.h"

#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMathDefinitions.h"
#include "ZEMath/ZEAngle.h"

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsItemGroup>

using namespace Ui;

void ZEAIActor::UpdateVisual()
{
	float Radius = 10 * this->GetRadius();
	
	VisualActor->setTransform(QTransform().rotateRadians(-GetRotation2D()));
	VisualActor->setPos(GetPosition().x * 10.0f, GetPosition().y * 10.0f);

	if (fabs(GetLinearVelocity().LengthSquare()) > ZE_ZERO_TRESHOLD)
	{
		ZEVector3 Offset = GetLinearVelocity().Normalize() * GetRadius() + GetPosition() * 10.0f;
		VisualLinearVelocity->setLine(Offset.x, Offset.y, Offset.x + GetLinearVelocity().x * 10.0f, Offset.y + GetLinearVelocity().y * 10.0f);
	}

	if (fabs(GetLinearAcceleration().LengthSquare()) > ZE_ZERO_TRESHOLD)
	{
		ZEVector3 Offset = GetLinearAcceleration().Normalize() * GetRadius() + GetPosition() * 10.0f;
		VisualLinearAcceleration->setLine(Offset.x, Offset.y, Offset.x + GetLinearAcceleration().x * 10.0f, Offset.y + GetLinearAcceleration().y * 10.0f);
	}

	VisualActor->setRect(-GetRadius(), -GetRadius(), 2.0f * GetRadius(), 2.0f * GetRadius());
	
	QGraphicsLineItem* Item;
	Item = (QGraphicsLineItem*)VisualActor->childItems().at(0);
	Item->setLine(0.0f, 0.0f, GetRadius() * sinf(ZE_PI_8), GetRadius() * cosf(ZE_PI_8));
	Item = (QGraphicsLineItem*)VisualActor->childItems().at(1);
	Item->setLine(0.0f, 0.0f, GetRadius() * sinf(-ZE_PI_8), GetRadius() * cosf(-ZE_PI_8));
	Item = (QGraphicsLineItem*)VisualActor->childItems().at(2);
	Item->setLine(0.0f, Radius, ZE_PI * GetRadius() * GetAngularVelocity(), GetRadius());
	Item = (QGraphicsLineItem*)VisualActor->childItems().at(3);
	Item->setLine(0.0f, Radius, ZE_PI * GetRadius() * GetAngularAcceleration(), GetRadius());

	QGraphicsTextItem* TextItem = (QGraphicsTextItem*)VisualActor->childItems().at(4);
	TextItem->setPlainText(GetName());
	TextItem->setPos(0, -Radius);
	TextItem->setTransform(QTransform::fromScale(1.0f, -1.0f));
}

void ZEAIActor::Tick(float ElapsedTime)
{
	ZEActor::Tick(ElapsedTime);

	UpdateVisual();
}

ZEAIActor::ZEAIActor(::ZEAIMainWindow* Window)
{
	this->Window = Window;

	VisualLinearAcceleration = new QGraphicsLineItem(0.0, 0.0, 0.0f, 0.0);
	VisualLinearAcceleration->setPen(QPen(QBrush(Qt::red), 1.5f));
	Window->Form->World->scene()->addItem(VisualLinearAcceleration);

	VisualLinearVelocity = new QGraphicsLineItem(0.0, 0.0, 0.0f, 0.0);
	VisualLinearVelocity->setPen(QPen(QBrush(Qt::green), 1.5f));
	Window->Form->World->scene()->addItem(VisualLinearVelocity);

	VisualLinearAcceleration = new QGraphicsLineItem(0.0, 0.0, 0.0f, 0.0);
	VisualLinearAcceleration->setPen(QPen(QBrush(Qt::red), 1.5f));
	Window->Form->World->scene()->addItem(VisualLinearAcceleration);

	VisualLinearVelocity = new QGraphicsLineItem(0.0, 0.0, 0.0f, 0.0);
	VisualLinearVelocity->setPen(QPen(QBrush(Qt::green), 1.5f));
	Window->Form->World->scene()->addItem(VisualLinearVelocity);

	VisualActor = new QGraphicsActorItem();
	VisualActor->Actor = this;
	VisualActor->setPen(QPen(QBrush(Qt::GlobalColor::darkBlue), 2.5f));
	VisualActor->setBrush(QBrush(QColor(230, 255, 230)));
	VisualActor->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
	VisualActor->childItems().append(new QGraphicsLineItem(VisualActor));
	VisualActor->childItems().append(new QGraphicsLineItem(VisualActor));

	VisualAngularVelocity = new QGraphicsLineItem(VisualActor);
	VisualAngularVelocity->setPen(QPen(QBrush(Qt::darkGreen), 1.5f));

	VisualAngularAcceleration = new QGraphicsLineItem(VisualActor);
	VisualAngularAcceleration->setPen(QPen(QBrush(Qt::darkRed), 1.5f));

	VisualActor->childItems().append(VisualAngularVelocity);
	VisualActor->childItems().append(VisualAngularAcceleration);
	VisualActor->childItems().append(new QGraphicsTextItem(VisualActor));
	Window->Form->World->scene()->addItem(VisualActor);

	Index++;
	SetName(ZEString::Format("Actor%2d", Index));
	UpdateVisual();
}

ZEAIActor::~ZEAIActor()
{
	Window->Form->World->scene()->removeItem(VisualActor);
	delete VisualActor;
	Window->Form->World->scene()->removeItem(VisualLinearVelocity);
	delete VisualLinearVelocity;
}
