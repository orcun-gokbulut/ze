//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDMapFilePlugIn.cpp
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

#include "ZEDMapFilePlugIn.h"

QString* ZEDMapFilePlugIn::GetSupportedFileExtension(int &Count)
{
	Count = 1;
	QString* ExtensionArray = new QString[1];
	ExtensionArray[0] = ZEMAP;
	return ExtensionArray;
}

QWidget* ZEDMapFilePlugIn::GetEditor(QString FileName, QString WorkingDirectory, QWidget* Parent, QLayout* Layout)
{
	return NULL;
}

QPixmap ZEDMapFilePlugIn::GetPreview(QString FileName, int Size)
{
	QPen ShadowPen = QPen(Qt::black);
	QPen PlainTextPen = QPen(Qt::white);
	QFileInfo FileInfo(FileName);

	QPixmap Pixmap = QPixmap(":/Previews/ZEMapFilePreview.png").scaledToHeight(Size);
	QPixmap TempPixmap(Pixmap.size());
	QPainter Painter(&TempPixmap);
	Painter.drawPixmap(0,0,Pixmap);
	Painter.setPen(ShadowPen);
	Painter.drawText(QPointF(4,14), FileInfo.fileName());
	Painter.setPen(PlainTextPen);
	Painter.drawText(QPointF(5,15), FileInfo.fileName());
	Painter.setPen(ShadowPen);
	Painter.drawText(QPointF(4,29), QString().setNum(FileInfo.size() / 1024) + QString(" Kb"));
	Painter.setPen(PlainTextPen);
	Painter.drawText(QPointF(5,30), QString().setNum(FileInfo.size() / 1024) + QString(" Kb"));
	return TempPixmap;
}

const char* ZEDMapFilePlugIn::GetVersion()
{
	return "0.5";
}

const char*	ZEDMapFilePlugIn::GetName()
{
	return "MapFilePlugIn";
}

const char*	ZEDMapFilePlugIn::GetAuthor()
{
	return "Zinek Code House & Game Studio";
}

ZEEntity* ZEDMapFilePlugIn::GetEntity()
{
	return NULL;
}

ZEDPlugInType ZEDMapFilePlugIn::GetType()
{
	return ZED_FILE_TYPE_PLUG_IN;
}

ZEDMapFilePlugIn::ZEDMapFilePlugIn()
{

}
