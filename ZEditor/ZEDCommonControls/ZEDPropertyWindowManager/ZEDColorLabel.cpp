//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDColorLabel.cpp
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

#include "ZEDColorLabel.h"
#include "ZEDColorPickerRGB.h"
#include "ZEDColorPickerRGBA.h"

void ZEDColorLabel::mousePressEvent(QMouseEvent* Event)
{
	if (ExternalEventCall == true)
	{
		if (this->Color.isValid())
		{
			QPalette Palette = palette();
			Palette.setColor(QPalette::Base, Color);		
			setPalette(Palette);
			this->setAutoFillBackground(true);
			update();
			repaint();
		}

	}

	else
	{
		QColor TempColor;

		if(Type == ZED_CLT_RGBA)
			TempColor = QColorDialog::getColor(Color, this, QString("Select Color"), QColorDialog::ShowAlphaChannel);

		if(Type == ZED_CLT_RGB)
			TempColor = QColorDialog::getColor(Color, this, QString("Select Color"));

		if(TempColor.isValid())
		{
			Color = TempColor;
			QPalette Palette = palette();
			Palette.setColor(QPalette::Base, Color);		
			setPalette(Palette);
			this->setAutoFillBackground(true);
			update();
			repaint();
		}

		if(Type == ZED_CLT_RGB)
			((ZEDColorPickerRGB*)(ParentItem))->ChangeByDialog();

		if(Type == ZED_CLT_RGBA)
			((ZEDColorPickerRGBA*)(ParentItem))->ChangeByDialog();
	}
}

void ZEDColorLabel::SetColor(QColor Color)
{
	this->Color = Color;
	//ExternalEventCall = true;

	QPalette Palette = palette();
	Palette.setColor(QPalette::Base, Color);		
	setPalette(Palette);
	this->setAutoFillBackground(true);
	update();
	repaint();

	//mousePressEvent(NULL);
}

QColor ZEDColorLabel::GetColor()
{
	return Color;
}

ZEDColorLabel::ZEDColorLabel(QWidget* Parent, QTreeWidgetItem* ParentItem, ZEDColorLabelType Type) : QLabel(Parent)
{
	this->Type = Type;
	this->ParentItem = ParentItem;
	setFrameShape(QFrame::Box);
	this->setAutoFillBackground(true);
	setMaximumHeight(17);
	ExternalEventCall = false;
}

ZEDColorLabel::~ZEDColorLabel()
{
	
}
