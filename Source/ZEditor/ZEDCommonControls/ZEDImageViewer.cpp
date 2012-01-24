//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDImageViewer.cpp
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

#include "ZEDImageViewer.h"

#include <FreeImage.h>
#include <QtGui/QMouseEvent>
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEMath.h"

void ZEDImageViewerViewPort::mousePressEvent(QMouseEvent* Event)
{
	QGraphicsView::mousePressEvent(Event);

	if(Event->button() == MidButton)
		ResetZoom();
}

void ZEDImageViewerViewPort::ResetZoom()
{
	if(ZoomCount < 0)
	{
		while (ZoomCount != 0)
		{
			scale(1.1f, 1.1f);
			ZoomCount++;
		}
	}
	else if(ZoomCount > 0)
	{
		while (ZoomCount != 0)
		{
			scale(1.0f / 1.1f, 1.0f / 1.1f);
			ZoomCount--;
		}
	}
	else
		return;
}

void ZEDImageViewerViewPort::mouseReleaseEvent(QMouseEvent* Event)
{
	QGraphicsView::mouseReleaseEvent(Event);
	setCursor(Qt::ArrowCursor);
}

void ZEDImageViewerViewPort::mouseMoveEvent(QMouseEvent* Event)
{
	QGraphicsView::mouseMoveEvent(Event);
	QPointF UVCoordinates = mapToScene(Event->pos());

	float ImageWidth;
	float ImageHeight;

	ImageWidth = ((QGraphicsPixmapItem*)(items().at(0)))->boundingRect().width() - 1;
	ImageHeight = ((QGraphicsPixmapItem*)(items().at(0)))->boundingRect().height() - 1;

	ZEInt XCoordinate = ZEMath::Floor(UVCoordinates.x());
	ZEInt YCoordinate = ZEMath::Floor(UVCoordinates.y());

	QPixmap ImagePixmap = ((QGraphicsPixmapItem*)(items().at(0)))->pixmap();
	QImage Image;
	Image = ImagePixmap.toImage();
	QRgb PixelColor = Image.pixel(XCoordinate, YCoordinate);

	if(ParentViewer != NULL)
		ParentViewer->SetInfoNotifications(UVCoordinates.x() / ImageWidth, UVCoordinates.y() / ImageHeight, XCoordinate, YCoordinate, QColor(PixelColor), ImageWidth, ImageHeight);
}

void ZEDImageViewerViewPort::wheelEvent(QWheelEvent* Event)
{
	if (Event->delta() < 0)
	{
		scale(1.0f / 1.1f, 1.0f / 1.1f);
		ZoomCount--;
	}
	else
	{
		scale(1.1f, 1.1f);
		ZoomCount++;
	}
}

ZEDImageViewerViewPort::ZEDImageViewerViewPort(ZEDImageViewer* ParentViewer, QWidget* Parent) : QGraphicsView(Parent)
{
	setScene(&ViewportScene);
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	setDragMode(ScrollHandDrag);
	this->ParentViewer = ParentViewer;
	setCursor(Qt::ArrowCursor);

	QPixmap TilePixmap(16, 16);
	TilePixmap.fill(Qt::white);
	QPainter TilePainter(&TilePixmap);
	QColor color(220, 220, 220);
	TilePainter.fillRect(0, 0, 8, 8, color);
	TilePainter.fillRect(8, 8, 8, 8, color);
	TilePainter.end();
	ZoomCount = 0;

	setBackgroundBrush(TilePixmap);
}

ZEDImageViewerViewPort::~ZEDImageViewerViewPort()
{
	ParentViewer = NULL;
}

/************************************************************************/
/*                         ZEDImageViewer                               */
/************************************************************************/

ZEDImageViewer::ZEDImageViewer()
{	
	FreeImage_Initialise();
	ViewerLayout.setContentsMargins(0, 0, 0, 0);
	setCentralWidget(new QWidget(this));
	ViewerViewport = new ZEDImageViewerViewPort(this, centralWidget());
	centralWidget()->setLayout(&ViewerLayout);
	ViewerLayout.addWidget(ViewerViewport);	
	ViewerViewport->scene()->addItem(&PixmapItem);
	ViewerLayout.addLayout(&InfoLayout);

	ULabel = new QLabel(" U : \n X : ", this);
	VLabel = new QLabel("V : \nY : ", this);
	ColorInfoLabel = new QLabel("ARGB : \nRGBF : ", this);
	ColorLabel = new QLabel("\n", this);
	ResolutionLabel = new QLabel("Resolution : \n", this);
	ULabel->setFixedWidth(85);
	VLabel->setFixedWidth(85);
	ColorInfoLabel->setFixedWidth(250);
	ColorLabel->setFixedWidth(25);
	ColorLabel->setFixedHeight(25);
	ColorLabel->setFrameShape(QFrame::Box);
	ResolutionLabel->setFixedWidth(180);
	ULabel->setWordWrap(true);
	VLabel->setWordWrap(true);
	ColorInfoLabel->setWordWrap(true);
	ColorLabel->setWordWrap(true);
	ResolutionLabel->setWordWrap(true);
	InfoLayout.addWidget(ULabel);
	InfoLayout.addWidget(VLabel);
	InfoLayout.addWidget(ResolutionLabel);
	InfoLayout.addWidget(ColorLabel);
	InfoLayout.addWidget(ColorInfoLabel);
	InfoLayout.addSpacerItem(new QSpacerItem(20,20, QSizePolicy::Expanding, QSizePolicy::Minimum));
}

ZEDImageViewer::~ZEDImageViewer()
{
	ULabel->close();
	delete ULabel;
	ULabel = NULL;

	VLabel->close();
	delete VLabel;
	VLabel = NULL;

	ColorInfoLabel->close();
	delete ColorInfoLabel;
	ColorInfoLabel = NULL;

	ColorLabel->close();
	delete ColorLabel;
	ColorLabel = NULL;

	ResolutionLabel->close();
	delete ResolutionLabel;
	ResolutionLabel = NULL;
}

void ZEDImageViewer::SetImage(QString FileName)
{
	QPixmap Pixmap;

	FIBITMAP*	BitMap = NULL;
	FREE_IMAGE_FORMAT ImageFormat = FreeImage_GetFileType((const char*)FileName.toLatin1());

	if (ImageFormat == FIF_UNKNOWN)
		return;

	BitMap = FreeImage_Load(ImageFormat,(const char*)FileName.toLatin1(), 0);

	if (BitMap == NULL)
		return;

	FIMEMORY* Stream = FreeImage_OpenMemory();

	if (!FreeImage_SaveToMemory(FIF_BMP, BitMap, Stream))
	{
		FreeImage_Unload(BitMap);
		return;
	}

	DWORD BufferSize = 0;
	BYTE* Buffer = NULL;

	if(!FreeImage_AcquireMemory(Stream, &Buffer, &BufferSize))
	{
		FreeImage_Unload(BitMap);
		return;
	}

	Pixmap.loadFromData(Buffer, BufferSize, "bitmap");

	FreeImage_CloseMemory(Stream);
	FreeImage_Unload(BitMap);

	PixmapItem.setPixmap(Pixmap);
	ViewerViewport->ensureVisible(&PixmapItem);
	resize(1024, 768);

	setWindowTitle(FileName);
}

void ZEDImageViewer::SetInfoNotifications(float U, float V, ZEInt X, ZEInt Y, QColor PixelColor, ZEInt Width, ZEInt Height)
{ 
	if(U < 0.0f || U > 1.0f || V < 0.0f || V > 1.0f)
	{
		ULabel->setText(" U : Undefined\n X : Undefined");
		VLabel->setText("V : Undefined\nY : Undefined");
		ColorInfoLabel->setText("ARGB : Undefined\nRGBF : Undefined");
	}
	else
	{
		ULabel->setText(" U : " + QString::number(U) + "\n X : " + QString::number(X));
		VLabel->setText("V : " + QString::number(V) + "\nY : " + QString::number(Y));
		ColorInfoLabel->setText("ARGB : " + QString::number(PixelColor.alpha()) + "  " + QString::number(PixelColor.red()) + "  " +QString::number(PixelColor.green()) + "  " + QString::number(PixelColor.blue()) + "\n" + 
			"RGBF : " + QString::number(PixelColor.red() / 255.0f) + "  " + QString::number(PixelColor.green() / 255.0f) + "  " +QString::number(PixelColor.blue() / 255.0f) + "  " + QString::number(PixelColor.alpha() / 255.0f));

		QPalette Palette = ColorLabel->palette();
		Palette.setColor(QPalette::Window, PixelColor);		
		ColorLabel->setPalette(Palette);
		ColorLabel->setAutoFillBackground(true);
		ColorLabel->update();
		ColorLabel->repaint();
	}

	ResolutionLabel->setText("Resolution : " + QString::number(Width) + "(w) x " + QString::number(Height) + "(h)\n");
}
