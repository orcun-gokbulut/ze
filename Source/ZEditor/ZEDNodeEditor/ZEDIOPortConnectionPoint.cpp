//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDIOPortConnectionPoint.cpp
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

#include <QPen>
#include <QBrush>
#include "ZEDIOPortConnectionPoint.h"
#include "ZEDNodeIOPort.h"

#define INPUT_COLOR			QColor(0, 200, 0)
#define OUTPUT_COLOR		QColor(200, 0, 0)
#define INPUT_OUTPUT_COLOR	QColor(200, 200, 0)

ZEDIOPortConnectionPoint::ZEDIOPortConnectionPoint(ZEDNodeIOPort* ParentIOPort, ZEDIOPortConnectionPointAlignment Alignment) : QGraphicsPolygonItem(ParentIOPort)
{
	this->ParentIOPort = ParentIOPort;
	QPolygonF TempPolygon;

	QPen BlackPen(Qt::black);
	QBrush InputBrush(INPUT_COLOR);
	QBrush OutputBrush(OUTPUT_COLOR);
	QBrush InputOutputBrush(INPUT_OUTPUT_COLOR);
	setPen(BlackPen);

	if (Alignment == ZED_IOPCPA_LEFT)
	{
		if(ParentIOPort->GetType() == ZED_NIOPT_INPUT)
		{
			TempPolygon.append(QPointF(0.0f, 10.0f));
			TempPolygon.append(QPointF(10.0f, 10.0f));
			TempPolygon.append(QPointF(10.0f, 0.0f));
			TempPolygon.append(QPointF(0.0f, 0.0f));
			setBrush(InputBrush);
			setPolygon(TempPolygon);
		}
		else if (ParentIOPort->GetType() == ZED_NIOPT_OUTPUT)
		{
			TempPolygon.append(QPointF(10.0f, 5.0f));
			TempPolygon.append(QPointF(0.0f, 10.0f));
			TempPolygon.append(QPointF(0.0f, 0.0f));
			TempPolygon.append(QPointF(10.0f, 5.0f));
			setBrush(OutputBrush);
			setPolygon(TempPolygon);		
		}
		else if (ParentIOPort->GetType() == ZED_NIOPT_INPUTOUTPUT)
		{
			TempPolygon.append(QPointF(0.0f, 10.0f));
			TempPolygon.append(QPointF(10.0f, 10.0f));
			TempPolygon.append(QPointF(10.0f, 0.0f));
			TempPolygon.append(QPointF(0.0f, 0.0f));
			TempPolygon.append(QPointF(10.0f, 5.0f));
			TempPolygon.append(QPointF(0.0f, 10.0f));
			TempPolygon.append(QPointF(0.0f, 0.0f));
			TempPolygon.append(QPointF(10.0f, 5.0f));
			setBrush(InputOutputBrush);
			setPolygon(TempPolygon);
		}
	}
	else if (Alignment == ZED_IOPCPA_RIGHT)
	{
		if(ParentIOPort->GetType() == ZED_NIOPT_INPUT)
		{
			TempPolygon.append(QPointF(0.0f, 10.0f));
			TempPolygon.append(QPointF(10.0f, 10.0f));
			TempPolygon.append(QPointF(10.0f, 0.0f));
			TempPolygon.append(QPointF(0.0f, 0.0f));
			setBrush(InputBrush);
			setPolygon(TempPolygon);
		}
		else if (ParentIOPort->GetType() == ZED_NIOPT_OUTPUT)
		{
			TempPolygon.append(QPointF(0.0f, 5.0f));
			TempPolygon.append(QPointF(10.0f, 0.0f));
			TempPolygon.append(QPointF(10.0f, 10.0f));
			TempPolygon.append(QPointF(0.0f, 5.0f));
			setBrush(OutputBrush);
			setPolygon(TempPolygon);		
		}
		else if (ParentIOPort->GetType() == ZED_NIOPT_INPUTOUTPUT)
		{
			TempPolygon.append(QPointF(10.0f, 0.0f));
			TempPolygon.append(QPointF(0.0f, 0.0f));
			TempPolygon.append(QPointF(0.0f, 10.0f));
			TempPolygon.append(QPointF(10.0f, 10.0f));
			TempPolygon.append(QPointF(0.0f, 5.0f));
			TempPolygon.append(QPointF(10.0f, 10.0f));
			TempPolygon.append(QPointF(10.0f, 0.0f));
			TempPolygon.append(QPointF(0.0f, 5.0f));
			setBrush(InputOutputBrush);
			setPolygon(TempPolygon);
		}
	}
}
