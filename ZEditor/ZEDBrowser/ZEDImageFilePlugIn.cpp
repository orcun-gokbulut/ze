//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDImageFilePlugIn.cpp
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

#include <QFileInfo>
#include <QPen>
#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <FreeImage.h>

#include "ZEDImageFilePlugIn.h"

QString* ZEDImageFilePlugIn::GetSupportedFileExtension(int &Count)
{
	Count = 3;
	QString* ExtensionArray = new QString[3];
	ExtensionArray[0] = QString("DDS");
	ExtensionArray[1] = QString("TGA");
	ExtensionArray[2] = QString("BMP");
	return ExtensionArray;
}

QWidget* ZEDImageFilePlugIn::GetEditor(QString FileName, QString WorkingDirectory, QWidget* Parent, QLayout* Layout)
{
	return NULL;
}

QPixmap ZEDImageFilePlugIn::GetPreview(QString FileName, int Size)
{
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

	int Height = FreeImage_GetHeight(BitMap);
	int	Width = FreeImage_GetWidth(BitMap);


	QString Name = FileInfo.fileName();
	QString Resolution = QString().setNum(Height) + QString(" x ") + QString().setNum(Width);

	FreeImage_CloseMemory(Stream);
	FreeImage_Unload(BitMap);

	if (Pixmap.height() < Pixmap.width())
	{
		Pixmap = Pixmap.scaledToWidth(Size);
	}
	else
		Pixmap = Pixmap.scaledToHeight(Size);

	QPixmap TempPixmap(Pixmap.size());
	QPainter Painter(&TempPixmap);
	Painter.drawPixmap(0,0,Pixmap);
	Painter.setPen(ShadowPen);
	Painter.drawText(QPointF(4,14), FileInfo.fileName());
	Painter.setPen(PlainTextPen);
	Painter.drawText(QPointF(5,15), FileInfo.fileName());
	Painter.setPen(ShadowPen);
	Painter.drawText(QPointF(4,29), Resolution);
	Painter.setPen(PlainTextPen);
	Painter.drawText(QPointF(5,30), Resolution);
	Painter.setPen(ShadowPen);
	Painter.drawText(QPointF(4,44), QString().setNum(FileInfo.size() / 1024) + QString(" Kb"));
	Painter.setPen(PlainTextPen);
	Painter.drawText(QPointF(5,45), QString().setNum(FileInfo.size() / 1024) + QString(" Kb"));
	return TempPixmap;
}

const char* ZEDImageFilePlugIn::GetVersion()
{
	return "0.5";
}

const char*	ZEDImageFilePlugIn::GetName()
{
	return "ImageFilePlugIn";
}

const char*	ZEDImageFilePlugIn::GetAuthor()
{
	return "Zinek Code House & Game Studio";
}

ZEEntity* ZEDImageFilePlugIn::GetEntity()
{
	return NULL;
}

ZEDPlugInType ZEDImageFilePlugIn::GetType()
{
	return ZED_FILE_TYPE_PLUG_IN;
}

ZEDImageFilePlugIn::ZEDImageFilePlugIn()
{

}
