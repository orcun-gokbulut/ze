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
#include "ZEAISteering.h"
#include "ZEAIMainWindow.h"
#include "ui_ZEAIMainWindow.h"

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
	float Radius = 10 * this->Radius;
	
	VisualActor->setTransform(QTransform().rotateRadians(-Rotation));
	VisualActor->setPos(Position.x * 10.0f, Position.y * 10.0f);

	if (fabs(GetLinearVelocity().LengthSquare()) > ZE_ZERO_TRESHOLD)
	{
		ZEVector3 Offset = GetLinearVelocity().Normalize() * Radius + Position * 10.0f;
		VisualLinearVelocity->setLine(Offset.x, Offset.y, Offset.x + LinearVelocity.x * 10.0f, Offset.y + LinearVelocity.y * 10.0f);
	}

	if (fabs(LinearAcceleration.LengthSquare()) > ZE_ZERO_TRESHOLD)
	{
		ZEVector3 Offset = GetLinearAcceleration().Normalize() * Radius + Position * 10.0f;
		VisualLinearAcceleration->setLine(Offset.x, Offset.y, Offset.x + LinearAcceleration.x * 10.0f, Offset.y + LinearAcceleration.y * 10.0f);
	}

	VisualActor->setRect(-Radius, -Radius, 2.0f * Radius, 2.0f * Radius);
	
	QGraphicsLineItem* Item;
	Item = (QGraphicsLineItem*)VisualActor->childItems().at(0);
	Item->setLine(0.0f, 0.0f, Radius * sinf(ZE_PI_8), Radius * cosf(ZE_PI_8));
	Item = (QGraphicsLineItem*)VisualActor->childItems().at(1);
	Item->setLine(0.0f, 0.0f, Radius * sinf(-ZE_PI_8), Radius * cosf(-ZE_PI_8));
	Item = (QGraphicsLineItem*)VisualActor->childItems().at(2);
	Item->setLine(0.0f, Radius, 2 * ZE_PI * Radius * AngularVelocity, Radius);
	Item = (QGraphicsLineItem*)VisualActor->childItems().at(3);
	Item->setLine(0.0f, Radius, 2 * ZE_PI * Radius * AngularAcceleration, Radius);

	QGraphicsTextItem* TextItem = (QGraphicsTextItem*)VisualActor->childItems().at(4);
	TextItem->setPlainText(GetName().GetValue());
	TextItem->setPos(0, -Radius);
	TextItem->setTransform(QTransform::fromScale(1.0f, -1.0f));
}

void ZEAIActor::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString&  ZEAIActor::GetName()
{
	return Name;
}

void ZEAIActor::SetPosition(const ZEVector3& Position)
{
	this->Position = Position;
}

const ZEVector3& ZEAIActor::GetPosition()
{
	return Position;
}

void ZEAIActor::SetRotation(float Rotation)
{
	this->Rotation = ZEAngle::RangeRadian(Rotation);
}

float ZEAIActor::GetRotation()
{
	return Rotation;
}

void ZEAIActor::SetLinearVelocity(const ZEVector3& Velocity)
{
	LinearVelocity = Velocity;
}

const ZEVector3& ZEAIActor::GetLinearVelocity()
{
	return LinearVelocity;
}

void ZEAIActor::SetAngularVelocity(float Velocity)
{
	AngularVelocity = Velocity;
}

float ZEAIActor::GetAngularVelocity()
{
	return AngularVelocity;
}

const ZEVector3& ZEAIActor::GetLinearAcceleration()
{
	return LinearAcceleration;
}

float ZEAIActor::GetAngularAcceleration()
{
	return AngularAcceleration;
}

void ZEAIActor::SetMaxAngularVelocity(float Velocity)
{
	MaxAngularVelocity = Velocity;
}

float ZEAIActor::GetMaxAngularVelocity()
{
	return MaxLinearVelocity;
}

void ZEAIActor::SetMaxAngularAcceleration(float Acceleration)
{
	MaxAngularAcceleration = Acceleration;
}

float ZEAIActor::GetMaxAngularAcceleration()
{
	return MaxAngularAcceleration;
}

void ZEAIActor::SetMaxLinearVelocity(float Velocity)
{
	MaxLinearVelocity = Velocity;
}

float ZEAIActor::GetMaxLinearSpeed()
{
	return MaxLinearVelocity;
}

void ZEAIActor::SetMaxLinearAcceleration(float Acceleration)
{
	MaxLinearAcceleration = Acceleration;
}

float ZEAIActor::GetMaxLinearAcceleration()
{
	return MaxLinearAcceleration;
}

void ZEAIActor::SetRadius(float Radius)
{
	this->Radius = Radius;
}

float ZEAIActor::GetRadius()
{
	return Radius;
}

const ZEArray<ZEAISteering*>& ZEAIActor::GetSteerings()
{
	return Steerings;
}

void ZEAIActor::AddSteering(ZEAISteering* Steering)
{
	Steering->Owner = this;
	Steerings.Add(Steering);
}

void ZEAIActor::RemoveSteering(ZEAISteering* Steering)
{
	Steerings.DeleteValue(Steering);
}

void ZEAIActor::Tick(float ElapsedTime)
{
	ZEVector3 Position = GetPosition();
	float Rotation = GetRotation();
	ZEVector3 LinearVelocity = GetLinearVelocity();
	float AngularVelocity = GetAngularVelocity();

	Position += LinearVelocity * ElapsedTime;
	Rotation += AngularVelocity * ElapsedTime;
	
	Rotation = ZEAngle::RangeRadian(Rotation);

	LinearAcceleration = ZEVector3::Zero;
	AngularAcceleration = 0.0f;
	for (size_t I = 0; I < Steerings.GetCount(); I++)
	{
		ZEAISteeringOutput Output = Steerings[I]->Process(ElapsedTime);
		LinearVelocity += Output.LinearAcceleration * ElapsedTime;
		AngularVelocity += Output.AngularAcceleration * ElapsedTime;
		
		LinearAcceleration += Output.LinearAcceleration;
		AngularAcceleration += Output.AngularAcceleration;
	}

	if (LinearVelocity.LengthSquare() > MaxLinearVelocity * MaxLinearVelocity)
		LinearVelocity = LinearVelocity.Normalize() * MaxLinearVelocity;
	
	if (AngularVelocity > MaxAngularVelocity)
		AngularVelocity = MaxAngularVelocity;
	else if (AngularVelocity < -MaxAngularVelocity)
		AngularVelocity = -MaxAngularVelocity;


	SetPosition(Position);
	SetRotation(Rotation);
	SetLinearVelocity(LinearVelocity);
	SetAngularVelocity(AngularVelocity);

	UpdateVisual();
}

ZEAIActor::ZEAIActor(::ZEAIMainWindow* Window)
{
	static int Index = 0;
	this->Window = Window;
	Position = ZEVector3::Zero;
	Rotation = 0.0f;
	MaxAngularVelocity = ZE_PI_2;
	MaxLinearVelocity = 1.0f;
	LinearVelocity = ZEVector3::Zero;
	AngularVelocity = 0.0f;
	LinearAcceleration = ZEVector3::Zero;
	AngularAcceleration = 0.0f;

	MaxAngularAcceleration = 1.0f;
	MaxLinearAcceleration = 1.0f;
	Radius = 1.0f;

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
	Name = ZEString::Format("Actor%2d", Index);
	UpdateVisual();
}

ZEAIActor::~ZEAIActor()
{
	Window->Form->World->scene()->removeItem(VisualActor);
	delete VisualActor;
	Window->Form->World->scene()->removeItem(VisualLinearVelocity);
	delete VisualLinearVelocity;
}
