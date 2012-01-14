//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDLoadingScreen.cpp
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

#include <QFont>
#include <QColor>
#include <QBrush>
#include "ZEDLoadingScreen.h"


void ZEDLoadingScreen::SetNotificationText(const char* Text)
{
	this->NotificationLabel->clear();
	this->NotificationLabel->setText(QString(" ") + QString(Text));
	this->repaint();
}
						
 ZEDLoadingScreen::ZEDLoadingScreen()
{
	this->setFixedSize(500,320);
	
	QFont Font; 

	this->PictureLabel = new QLabel(this);
	this->NotificationLabel = new QLabel(QString(" Loading lots of thing..."),this);

	this->Layout = new QHBoxLayout(this);
	this->Layout->setDirection(QBoxLayout::BottomToTop);

	this->PMap.load(":/SplashScreen/ZEditor Splash Screen Flat.png");

	this->PictureLabel->setPixmap(this->PMap);

	this->NotificationLabel->setMaximumHeight(18);
	this->NotificationLabel->setMinimumHeight(18);

	this->setLayout(this->Layout);

	this->Layout->addWidget(this->NotificationLabel);
	this->Layout->addWidget(this->PictureLabel);

	this->Layout->setMargin(0);

	this->setWindowFlags(Qt::SplashScreen);


	this->hide();

}
