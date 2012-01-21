//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDImageFilePreviewWidget.cpp
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

#include "ZEDImageFilePreviewWidget.h"

#include <FreeImage.h>
#include <QPen>
#include <QFileInfo>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QImage>
#include <math.h>



/************************************************************************/
/*                       ZEDImageFilePreviewWidget                      */
/************************************************************************/

ZEDImageFilePreviewWidget::ZEDImageFilePreviewWidget(QWidget* Parent, QString FileName, WindowFlags F) : ZEDPreviewWidget(Parent, FileName, F)
{
	setLayout(&VerticalLayout);
	VerticalLayout.setDirection(QBoxLayout::TopToBottom);
	VerticalLayout.addSpacerItem(new QSpacerItem(20,20, QSizePolicy::Minimum, QSizePolicy::Expanding));
	VerticalLayout.addLayout(&HorizontalLayout);
	ViewButton.setText("View");
	HorizontalLayout.addWidget(&ViewButton);	
	setPixmap(CreatePixmap(FileName));
	ImageViewer = NULL;

	ViewAction = new QAction("View", this);
	Actions.append(ViewAction);

	QObject::connect(&ViewButton, SIGNAL(pressed()), this, SLOT(ShowViewer()));
	QObject::connect(ViewAction, SIGNAL(triggered(bool)), this, SLOT(ShowViewer()));
}

ZEDImageFilePreviewWidget::~ZEDImageFilePreviewWidget()
{
	if (ImageViewer != NULL)
	{
		ImageViewer->close();
		delete ImageViewer;
		ImageViewer = NULL;
	}
}

QPixmap ZEDImageFilePreviewWidget::CreatePixmap(QString FileName)
{
	FreeImage_Initialise();

	QPen ShadowPen = QPen(Qt::black);
	QPen PlainTextPen = QPen(Qt::white);
	QFileInfo FileInfo(FileName);

	FIBITMAP*	BitMap = NULL;
	FREE_IMAGE_FORMAT ImageFormat = FreeImage_GetFileType((const char*)FileName.toLatin1());

	if (ImageFormat == FIF_UNKNOWN)
		return QPixmap();

	BitMap = FreeImage_Load(ImageFormat,(const char*)FileName.toLatin1(), 0);

	if (BitMap == NULL)
		return QPixmap();

	FIMEMORY* Stream = FreeImage_OpenMemory();

	if (!FreeImage_SaveToMemory(FIF_BMP, BitMap, Stream))
	{
		FreeImage_Unload(BitMap);
		return QPixmap();
	}

	DWORD BufferSize = 0;
	BYTE* Buffer = NULL;


	if(!FreeImage_AcquireMemory(Stream, &Buffer, &BufferSize))
	{
		FreeImage_Unload(BitMap);
		return QPixmap();
	}

	QPixmap Pixmap;

	if(Pixmap.loadFromData(Buffer, BufferSize, "bitmap") == true)
	{
		if (Pixmap.height() < Pixmap.width())
		{
			Pixmap = Pixmap.scaledToWidth(256);
		}
		else
			Pixmap = Pixmap.scaledToHeight(256);
	}
	else
	{
		FreeImage_Unload(BitMap);
		return QPixmap();
	}

	ZEInt Height = FreeImage_GetHeight(BitMap);
	ZEInt	Width = FreeImage_GetWidth(BitMap);

	QString Resolution = QString().setNum(Height) + QString(" x ") + QString().setNum(Width);

	FreeImage_CloseMemory(Stream);
	FreeImage_Unload(BitMap);

	if (Pixmap.height() < Pixmap.width())
	{
		Pixmap = Pixmap.scaledToWidth(256);
	}
	else
		Pixmap = Pixmap.scaledToHeight(256);

	QPixmap TempPixmap(Pixmap.size());
	QPainter Painter(&TempPixmap);
	Painter.drawPixmap(0,0,Pixmap);
	Painter.setPen(ShadowPen);
	Painter.drawText(QPointF(4,29), Resolution);
	Painter.setPen(PlainTextPen);
	Painter.drawText(QPointF(5,30), Resolution);
	return TempPixmap;
}

void ZEDImageFilePreviewWidget::ShowViewer()
{
	if (ImageViewer == NULL)
		ImageViewer = new ZEDImageViewer();
	
	ImageViewer->show();
	ImageViewer->SetImage(GetFileName());
}

QList<QAction*> ZEDImageFilePreviewWidget::GetContextMenuItems()
{
	return Actions;
}
