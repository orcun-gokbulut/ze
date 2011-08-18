//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDSoundFilePreviewWidget.cpp
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

#include "ZEDSoundFilePreviewWidget.h"
#include <QPixmap>

ZEDSoundFilePreviewWidget::ZEDSoundFilePreviewWidget(QWidget* Parent, QString FileName, WindowFlags F) : ZEDPreviewWidget(Parent, FileName, F)
{
	setFixedSize(256, 256);
	setPixmap(QPixmap("sound.png"));

	setLayout(&VerticalLayout);
	VerticalLayout.setDirection(QBoxLayout.BottomToTop);
	VerticalLayout.addLayout(&HorizontalLayout);
	PlayButton.setText("Play");
	StopButton.setText("Stop");
	LoopCheckBox.setText("Loop");
	LoopCheckBox.setChecked(false);

	VerticalLayout.addSpacerItem(new QSpacerItem(20,20, QSizePolicy.Minimum, QSizePolicy.Expanding));
	HorizontalLayout.addWidget(&PlayButton);
	HorizontalLayout.addWidget(&StopButton);
	HorizontalLayout.addWidget(&LoopCheckBox);

	QObject::connect(&PlayButton, SIGNAL(pressed()), this, SLOT(PlaySound()));
	QObject::connect(&StopButton, SIGNAL(pressed()), this, SLOT(StopSound()));
	QObject::connect(&LoopCheckBox, SIGNAL(stateChanged(int)), this, SLOT(EnableLoop(int)));

	Player = new QSound(FileName, this);
}

ZEDSoundFilePreviewWidget::~ZEDSoundFilePreviewWidget()
{
	Player->stop();
	delete Player;
	Player = NULL;
}

QList<QAction*> ZEDSoundFilePreviewWidget::GetContextMenuItems()
{
	return QList<QAction*>();
}

void ZEDSoundFilePreviewWidget::PlaySound()
{
	Player->play();
}

void ZEDSoundFilePreviewWidget::StopSound()
{
	Player->stop();
}

void ZEDSoundFilePreviewWidget::EnableLoop(int State)
{	
	Player->stop();

	if(State == Unchecked)
		Player->setLoops(1);
	else if(State == Checked)
		Player->setLoops(-1);
}
