//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEAIMainWindow.cpp
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

#include "ZEAIMainWindow.h"
#include "ui_ZEAIMainWindow.h"

#include "ZEAIActor.h"
#include "ZEAISteering.h"
#include "ZEDNodeEditorMinimap.h"
#include "ZEMath/ZEAngle.h"

#include <QGraphicsItem>
#include <QTimer>

float frand(float Range)
{
	return ((float)rand()/(float)RAND_MAX) * Range -
		((float)rand()/(float)RAND_MAX) * Range;
}

void ZEAIMainWindow::Update()
{
	Frame++;
	Time += Timer->interval();
	
	Form->lblFrame->setText(QString().setNum(Frame));
	Form->lblTime->setText(QString().setNum(Time));
	for (size_t I = 0; I < Actors.GetCount(); I++)
		Actors[I]->Tick(Timer->interval() / 1000.0f);

	UpdateActorProperties(true);
}

ZEAIMainWindow::ZEAIMainWindow(QWidget *Parent, Qt::WFlags Flags) : QMainWindow(Parent, Flags)
{
	Form = new Ui::ZEAIMainWindow();
	Form->setupUi(this);
	Frame = 0;
	Time = 0;
	Timer = new QTimer();
	Timer->setInterval(30);

	Form->World->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

	connect(Form->btnStart, SIGNAL(clicked()), this, SLOT(btnStart_Clicked()));
	connect(Form->btnStop, SIGNAL(clicked()), this, SLOT(btnStop_Clicked()));
	connect(Form->btnStep, SIGNAL(clicked()), this, SLOT(btnStep_Clicked()));
	connect(Form->btnReset, SIGNAL(clicked()), this, SLOT(btnReset_Clicked()));
	connect(Form->btnRandomize, SIGNAL(clicked()), this, SLOT(btnRandomize_Clicked()));
	connect(Form->btnAddActor, SIGNAL(clicked()), this, SLOT(btnAddActor_Clicked()));
	connect(Form->btnDeleteActor, SIGNAL(clicked()), this, SLOT(btnDeleteActor_Clicked()));
	connect(Form->lstActors, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(lstActors_ItemClicked(QListWidgetItem*)));

	connect(Form->txtActorName, SIGNAL(TextChanged(QString)), this, SLOT(txtActorName_TextChanged(QString)));
	connect(Form->txtRadius, SIGNAL(valueChanged(double)), this, SLOT(txtRadius_ValueChanged(double)));
	connect(Form->txtMaxLinearAcceleration, SIGNAL(valueChanged(double)), this, SLOT(txtMaxLinearAcceleration_ValueChanged(double)));
	connect(Form->txtMaxAngularAcceleration, SIGNAL(valueChanged(double)), this, SLOT(txtMaxAngularAcceleration_ValueChanged(double)));
	connect(Form->txtMaxLinearVelocity, SIGNAL(valueChanged(double)), this, SLOT(txtMaxLinearVelocity_ValueChanged(double)));
	connect(Form->txtMaxAngularVelocity, SIGNAL(valueChanged(double)), this, SLOT(txtMaxAngularVelocity_ValueChanged(double)));
	connect(Timer, SIGNAL(timeout()), this, SLOT(Timer_TimeOut()));
	Scene = new QGraphicsScene();

	Form->World->setScene(Scene);
	Form->World->scale(1.0f, -1.0f);
	Form->World->setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing | QPainter::HighQualityAntialiasing);

	connect(Scene, SIGNAL(changed(const QList<QRectF>&)), this, SLOT(World_Changed(const QList<QRectF>&)));
	connect(Scene, SIGNAL(selectionChanged()), this, SLOT(World_SelectionChanged()));

	Form->grpMinimap->setLayout(new QHBoxLayout());
	
	ZEDNodeEditorMinimap* Minimap = new ZEDNodeEditorMinimap(Form->World, Scene, Form->grpMinimap);
	Form->grpMinimap->layout()->addWidget(Minimap);
	Form->grpActorProperties->setVisible(false);
	Minimap->setTransform(Form->World->transform());
	Minimap->setBackgroundBrush(QBrush(Qt::white));
}

void ZEAIMainWindow::UpdateActorProperties(bool Partial)
{
	Form->grpActorProperties->setVisible(Scene->selectedItems().count() != 0);
	
	if (Scene->selectedItems().count() == 0)
		return;

	QGraphicsActorItem* Item = (QGraphicsActorItem*)Scene->selectedItems()[0];

	if (!Partial)
	{
		if (Scene->selectedItems().count() == 1)
			Form->txtActorName->setText(Item->Actor->GetName().GetValue());
		else
			Form->txtActorName->setText("");

		Form->txtRadius->setValue(Item->Actor->GetRadius());
		Form->txtMaxLinearVelocity->setValue(Item->Actor->GetMaxLinearSpeed());
		Form->txtMaxLinearAcceleration->setValue(Item->Actor->GetMaxLinearAcceleration());
		Form->txtMaxAngularVelocity->setValue(ZEAngle::ConvertToDegree(Item->Actor->GetMaxAngularVelocity()));
		Form->txtMaxAngularAcceleration->setValue(ZEAngle::ConvertToDegree(Item->Actor->GetMaxAngularAcceleration()));
	}

	if (Scene->selectedItems().count() == 1)
	{
		Form->txtActorLinearVelocity->setText(
			QString().sprintf("<%.2f, %.2f, %.2f>", 
			Item->Actor->GetLinearVelocity().x, Item->Actor->GetLinearVelocity().y, Item->Actor->GetLinearVelocity().z));

		Form->txtActorPosition->setText(
			QString().sprintf("<%.2f, %.2f, %.2f>", 
			Item->Actor->GetPosition().x, Item->Actor->GetPosition().y, Item->Actor->GetPosition().z));
		Form->txtActorRotation->setText(QString().setNum(Item->Actor->GetRotation()));

		Form->txtActorLinearAcceleration->setText(
			QString().sprintf("<%.2f, %.2f, %.2f>", 
			Item->Actor->GetLinearAcceleration().x, Item->Actor->GetLinearAcceleration().y, Item->Actor->GetLinearAcceleration().z));

		Form->txtActorRotation->setText(QString().setNum(ZEAngle::ConvertToDegree(Item->Actor->GetRotation())));
		Form->txtActorAngularVelocity->setText(QString().setNum(ZEAngle::ConvertToDegree(Item->Actor->GetAngularVelocity())));
		Form->txtActorAngularAcceleration->setText(QString().setNum(ZEAngle::ConvertToDegree(Item->Actor->GetAngularAcceleration())));

	}
	else
	{
		Form->txtActorAngularVelocity->setText("");
		Form->txtActorLinearVelocity->setText("");
		Form->txtActorPosition->setText("");
		Form->txtActorRotation->setText("");
	}
}

ZEAIMainWindow::~ZEAIMainWindow()
{
	delete Form;
}

void ZEAIMainWindow::Timer_TimeOut()
{
	Update();
}

void ZEAIMainWindow::btnStart_Clicked()
{
	Timer->start();
	Form->btnStart->setEnabled(false);
	Form->btnStop->setEnabled(true);
	Form->btnStep->setEnabled(true);
}

void ZEAIMainWindow::btnStop_Clicked()
{
	Timer->stop();
	Form->btnStart->setEnabled(true);
	Form->btnStop->setEnabled(false);
	Form->btnStep->setEnabled(true);
}

void ZEAIMainWindow::btnStep_Clicked()
{
	Update();
}

void ZEAIMainWindow::btnReset_Clicked()
{
	Actors[0]->SetRotation(frand(ZE_PI));
	Actors[0]->UpdateVisual();

	/*
	Frame = 0;
	Time = 0;
	Form->lblFrame->setText(QString(Frame));
	for (size_t I = 0; I < Actors.GetCount(); I++)
	{
		Actors[I]->SetLinearVelocity(ZEVector3::Zero);
		Actors[I]->SetAngularVelocity(0.0f);
	}*/
}

void ZEAIMainWindow::btnRandomize_Clicked()
{
	for (int I = 0; I < Actors.GetCount(); I++)
	{
		Actors[I]->SetPosition(ZEVector3(frand(50), frand(50), 0.0f));
		Actors[I]->UpdateVisual();
	}
}

void ZEAIMainWindow::btnAddActor_Clicked()
{
	ZEAIActor* New = new ZEAIActor(this);
	Actors.Add(New);
	Form->lstActors->addItem(new QListWidgetItem(New->GetName().ToCString()));
	Form->btnDeleteActor->setEnabled(true);

	if (Actors.GetCount() != 1)
	{
		Actors[Actors.GetCount() - 1]->AddSteering(new ZEAIAlignSteering());
		Actors[Actors.GetCount() - 1]->GetSteerings()[0]->SetTarget(Actors[0]);
	}
}

void ZEAIMainWindow::btnDeleteActor_Clicked()
{
	for (int I = 0; I < Scene->selectedItems().count(); I++)
	{
		QGraphicsActorItem* Item = (QGraphicsActorItem*)Scene->selectedItems()[I];
		Form->lstActors->takeItem(Actors.FindIndex(Item->Actor));
		Actors.DeleteValue(Item->Actor);
		delete Item;
	}

	if (Actors.GetCount() == NULL)
		Form->btnDeleteActor->setEnabled(false);
}

void ZEAIMainWindow::lstActors_ItemClicked(QListWidgetItem* Item)
{
	Scene->blockSignals(true);
	Scene->clearSelection();
	for (int I = 0; I < Actors.GetCount(); I++)
	{
		if (Form->lstActors->item(I)->isSelected())
			Actors[I]->VisualActor->setSelected(true);
	}
	UpdateActorProperties();
	Scene->blockSignals(false);
}

void ZEAIMainWindow::World_Changed(const QList<QRectF> & region)
{
	for (int I = 0; I < Scene->selectedItems().count(); I++)
	{
		QGraphicsActorItem* Item = (QGraphicsActorItem*)Scene->selectedItems()[I];
		Item->Actor->SetPosition(ZEVector3(0.1f * Item->pos().x(), 0.1f * Item->pos().y(), 0.0f));
	}

	UpdateActorProperties(true);
}

void ZEAIMainWindow::World_SelectionChanged()
{
	Form->lstActors->blockSignals(true);
	Form->lstActors->clearSelection();
	for (int I = 0; I < Scene->selectedItems().count(); I++)
	{
		QGraphicsActorItem* Item = (QGraphicsActorItem*)Scene->selectedItems()[I];		
		Form->lstActors->setCurrentRow(Actors.FindIndex(Item->Actor), QItemSelectionModel::Select);
	}
	UpdateActorProperties();
	Form->lstActors->blockSignals(false);
}

void ZEAIMainWindow::txtActorName_TextChanged(QString Text)
{
	if (Scene->selectedItems().count() != 1)
		return;

	QGraphicsActorItem* Item = (QGraphicsActorItem*)Scene->selectedItems()[0];
	Item->Actor->SetName(Text.toUtf8().constData());
	Item->Actor->UpdateVisual();
}

void ZEAIMainWindow::txtTickInterval_ValueChanged(int Value)
{
	Timer->setInterval(Value);
}

void ZEAIMainWindow::txtRadius_ValueChanged(double Value)
{
	for (int I = 0; I < Scene->selectedItems().count(); I++)
	{
		((QGraphicsActorItem*)Scene->selectedItems()[I])->Actor->SetRadius(Value);
		((QGraphicsActorItem*)Scene->selectedItems()[I])->Actor->UpdateVisual();
	}
}

void ZEAIMainWindow::txtMaxLinearAcceleration_ValueChanged(double Value)
{
	for (int I = 0; I < Scene->selectedItems().count(); I++)
		((QGraphicsActorItem*)Scene->selectedItems()[I])->Actor->SetMaxLinearAcceleration(Value);
}

void ZEAIMainWindow::txtMaxAngularAcceleration_ValueChanged(double Value)
{
	for (int I = 0; I < Scene->selectedItems().count(); I++)
		((QGraphicsActorItem*)Scene->selectedItems()[I])->Actor->SetMaxAngularAcceleration(ZEAngle::ConvertToRadian(Value));
}

void ZEAIMainWindow::txtMaxLinearVelocity_ValueChanged(double Value)
{
	for (int I = 0; I < Scene->selectedItems().count(); I++)
		((QGraphicsActorItem*)Scene->selectedItems()[I])->Actor->SetMaxLinearVelocity(Value);
}
void ZEAIMainWindow::txtMaxAngularVelocity_ValueChanged(double Value)
{
	for (int I = 0; I < Scene->selectedItems().count(); I++)
		((QGraphicsActorItem*)Scene->selectedItems()[I])->Actor->SetMaxAngularVelocity(ZEAngle::ConvertToRadian(Value));
}
