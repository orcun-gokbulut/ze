//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDIOPortConnection.cpp
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
#include <QDebug>
#include <QGraphicsPolygonItem>
#include <QCursor>
#include <math.h>

#include "ZEDIOPortConnection.h"
#include "ZEDNodeIOPort.h"
#include "ZEDNodeEditorNode.h"
#include "ZEDDefinitions.h"

#define ARROW_DIMENSION 12.0f
#define PI 3.14159265358979323846f

void ZEDIOPortConnection::hoverEnterEvent(QGraphicsSceneHoverEvent *Event)
{
	QGraphicsPolygonItem::hoverEnterEvent(Event);
}

void ZEDIOPortConnection::hoverLeaveEvent(QGraphicsSceneHoverEvent *Event)
{
	QGraphicsPolygonItem::hoverLeaveEvent(Event);
}

void ZEDIOPortConnection::UpdateBoundingRect()
{
	if (From != NULL && To != NULL)
	{
		QPolygonF TempPolygon;

		float ConnectionPointFromX = From->sceneBoundingRect().topLeft().x();
		float ConnectionPointFromY = From->sceneBoundingRect().topLeft().y() + From->sceneBoundingRect().height() / 2;
		float ConnectionPointToX = To->sceneBoundingRect().topLeft().x();
		float ConnectionPointToY = To->sceneBoundingRect().topLeft().y() + To->sceneBoundingRect().height() / 2;

		if (From->sceneBoundingRect().topLeft().x() <  (To->sceneBoundingRect().topLeft().x() +  To->sceneBoundingRect().width() / 2))
		{
			ConnectionPointFromX = From->sceneBoundingRect().bottomRight().x();
		}
		if (To->sceneBoundingRect().topLeft().x() <  (From->sceneBoundingRect().topLeft().x() +  From->sceneBoundingRect().width() / 2))
		{
			ConnectionPointToX = To->sceneBoundingRect().bottomRight().x();
		}

		///////////////////////////////Drawing the bounding polygon///////////////////////////////
		if ((ConnectionPointToX - ARROW_DIMENSION) - (ConnectionPointFromX + ARROW_DIMENSION) >= 0 && fabs(ConnectionPointFromX - From->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD  && fabs(ConnectionPointToX - To->sceneBoundingRect().topLeft().x()) < ZE_ZERO_THRESHOLD)
		{
			float Angle = atan((ConnectionPointToY - ConnectionPointFromY) / (ConnectionPointToX - ConnectionPointFromX - 2 * ARROW_DIMENSION)) * 180 / PI;
			float M = (ConnectionPointToY - ConnectionPointFromY) / (ConnectionPointToX - ConnectionPointFromX - 2 * ARROW_DIMENSION);
			float DistanceX = (ConnectionPointToX - ARROW_DIMENSION) - (ConnectionPointFromX + ARROW_DIMENSION);
			float DistanceY = (ConnectionPointToY - ARROW_DIMENSION) - (ConnectionPointFromY - ARROW_DIMENSION) ;

			if(sqrt(DistanceX * DistanceX + DistanceY * DistanceY) > 0)
			{
				TempPolygon.append(QPointF(0, 0));
				TempPolygon.append(QPointF(sqrt(DistanceX * DistanceX + DistanceY * DistanceY) , 0));
				TempPolygon.append(QPointF(sqrt(DistanceX * DistanceX + DistanceY * DistanceY), ARROW_DIMENSION / 2));
				TempPolygon.append(QPointF(0 , ARROW_DIMENSION / 2));
				ConnectionBodyBoundingRect->setPolygon(TempPolygon);
				ConnectionBodyBoundingRect->rotate(Angle - OldAngle);

				if (Angle >= 0)
					ConnectionBodyBoundingRect->setPos(ConnectionPointFromX + ARROW_DIMENSION + ARROW_DIMENSION / 4 , ConnectionPointFromY - ARROW_DIMENSION / 4);
				else if (Angle <= 0)
					ConnectionBodyBoundingRect->setPos(ConnectionPointFromX + ARROW_DIMENSION - ARROW_DIMENSION / 4 , ConnectionPointFromY - ARROW_DIMENSION / 4);

				qDebug() << OldAngle;
				qDebug() << "----------";
				qDebug() << Angle;
				qDebug() << "**********";
				OldAngle = Angle;
			}

		}
		else if((ConnectionPointFromX - ARROW_DIMENSION) >= (ConnectionPointToX + ARROW_DIMENSION) && fabs(ConnectionPointFromX - From->sceneBoundingRect().topLeft().x()) < ZE_ZERO_THRESHOLD && fabs(ConnectionPointToX - To->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD)
		{
			float Angle = atan((ConnectionPointToY - ConnectionPointFromY) / (ConnectionPointToX - ConnectionPointFromX + 2 * ARROW_DIMENSION)) * 180 / PI;
			float M = (ConnectionPointToY - ConnectionPointFromY) / (ConnectionPointToX - ConnectionPointFromX + 2 * ARROW_DIMENSION);
			float DistanceX =  (ConnectionPointFromX - ARROW_DIMENSION) - (ConnectionPointToX + ARROW_DIMENSION);
			float DistanceY = (ConnectionPointToY - ARROW_DIMENSION) - (ConnectionPointFromY - ARROW_DIMENSION) ;

			if(sqrt(DistanceX * DistanceX + DistanceY * DistanceY) > 0)
			{
				TempPolygon.append(QPointF(0, 0));
				TempPolygon.append(QPointF(sqrt(DistanceX * DistanceX + DistanceY * DistanceY) , 0));
				TempPolygon.append(QPointF(sqrt(DistanceX * DistanceX + DistanceY * DistanceY), ARROW_DIMENSION / 2));
				TempPolygon.append(QPointF(0 , ARROW_DIMENSION / 2));
				ConnectionBodyBoundingRect->setPolygon(TempPolygon);
				ConnectionBodyBoundingRect->rotate(Angle - OldAngle);

				if (Angle >= 0 && Angle != 90 && Angle != -90 )
					ConnectionBodyBoundingRect->setPos(ConnectionPointToX + ARROW_DIMENSION + ARROW_DIMENSION / 4 , ConnectionPointToY - ARROW_DIMENSION / 4);
				else if (Angle <= 0 && Angle != 90 && Angle != -90 )
					ConnectionBodyBoundingRect->setPos(ConnectionPointToX + ARROW_DIMENSION - ARROW_DIMENSION / 4 , ConnectionPointToY - ARROW_DIMENSION / 4);
				else if (Angle == 90)
					ConnectionBodyBoundingRect->setPos(ConnectionPointFromX - ARROW_DIMENSION + ARROW_DIMENSION / 4 , ConnectionPointFromY - ARROW_DIMENSION / 4);
				else if (Angle == -90)
					ConnectionBodyBoundingRect->setPos(ConnectionPointFromX - ARROW_DIMENSION - ARROW_DIMENSION / 4 , ConnectionPointFromY - ARROW_DIMENSION / 4);

				OldAngle = Angle;	
			}
		}
		else if (fabs(ConnectionPointFromX - From->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD && fabs(ConnectionPointToX - To->sceneBoundingRect().topLeft().x()) < ZE_ZERO_THRESHOLD)
		{
			float Angle = - atan((ConnectionPointToY - ConnectionPointFromY) / ( ConnectionPointFromX - ConnectionPointToX + 2 * ARROW_DIMENSION)) * 180 / PI;
			float M = (ConnectionPointToY - ConnectionPointFromY) / (ConnectionPointToX - ConnectionPointFromX + 2 * ARROW_DIMENSION);
			float DistanceX = (ConnectionPointFromX - ARROW_DIMENSION) - (ConnectionPointToX + ARROW_DIMENSION);
			float DistanceY = (ConnectionPointToY - ARROW_DIMENSION) - (ConnectionPointFromY - ARROW_DIMENSION) ;

			if(sqrt(DistanceX * DistanceX + DistanceY * DistanceY) > 0)
			{
				TempPolygon.append(QPointF(0, 0));
				TempPolygon.append(QPointF(sqrt(DistanceX * DistanceX + DistanceY * DistanceY) , 0));
				TempPolygon.append(QPointF(sqrt(DistanceX * DistanceX + DistanceY * DistanceY), ARROW_DIMENSION / 2));
				TempPolygon.append(QPointF(0 , ARROW_DIMENSION / 2));
				ConnectionBodyBoundingRect->setPolygon(TempPolygon);
				ConnectionBodyBoundingRect->rotate((Angle - OldAngle));

				if (Angle >= 0)
					ConnectionBodyBoundingRect->setPos(ConnectionPointToX - ARROW_DIMENSION + ARROW_DIMENSION / 4 , ConnectionPointToY - ARROW_DIMENSION / 4);
				else if (Angle <= 0)
					ConnectionBodyBoundingRect->setPos(ConnectionPointToX - ARROW_DIMENSION - ARROW_DIMENSION / 4 , ConnectionPointToY - ARROW_DIMENSION / 4);


				OldAngle = Angle;
			}
		}

		else if (fabs(ConnectionPointFromX - From->sceneBoundingRect().topLeft().x()) < ZE_ZERO_THRESHOLD && fabs(ConnectionPointToX - To->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD)
		{
			float Angle =  -atan((ConnectionPointToY - ConnectionPointFromY) / ( ConnectionPointFromX - ConnectionPointToX - 2 * ARROW_DIMENSION)) * 180 / PI;
			float M = (ConnectionPointToY - ConnectionPointFromY) / (ConnectionPointToX - ConnectionPointFromX - 2 * ARROW_DIMENSION);
			float DistanceX = (ConnectionPointFromX - ARROW_DIMENSION) - (ConnectionPointToX + ARROW_DIMENSION);
			float DistanceY = (ConnectionPointToY - ARROW_DIMENSION) - (ConnectionPointFromY - ARROW_DIMENSION) ;

			if(sqrt(DistanceX * DistanceX + DistanceY * DistanceY) > 0)
			{
				TempPolygon.append(QPointF(0, 0));
				TempPolygon.append(QPointF(sqrt(DistanceX * DistanceX + DistanceY * DistanceY) , 0));
				TempPolygon.append(QPointF(sqrt(DistanceX * DistanceX + DistanceY * DistanceY), ARROW_DIMENSION / 2));
				TempPolygon.append(QPointF(0 , ARROW_DIMENSION / 2));
				ConnectionBodyBoundingRect->setPolygon(TempPolygon);
				ConnectionBodyBoundingRect->rotate((Angle - OldAngle));

				if (Angle >= 0)
					ConnectionBodyBoundingRect->setPos(ConnectionPointFromX - ARROW_DIMENSION + ARROW_DIMENSION / 4 , ConnectionPointFromY - ARROW_DIMENSION / 4);
				else if (Angle <= 0)
					ConnectionBodyBoundingRect->setPos(ConnectionPointFromX - ARROW_DIMENSION - ARROW_DIMENSION / 4 , ConnectionPointFromY - ARROW_DIMENSION / 4);

				OldAngle = Angle;
			}
		}
		else if (ConnectionPointFromX > ConnectionPointToX && fabs(ConnectionPointFromX - From->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD && fabs(ConnectionPointToX - To->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD)
		{
			float Angle = atan((ConnectionPointToY - ConnectionPointFromY) / (ConnectionPointToX - ConnectionPointFromX )) * 180 / PI;
			float M = (ConnectionPointToY - ConnectionPointFromY) / (ConnectionPointToX - ConnectionPointFromX );
			float DistanceX =  (ConnectionPointFromX - ARROW_DIMENSION) - (ConnectionPointToX + ARROW_DIMENSION);
			float DistanceY = (ConnectionPointToY - ARROW_DIMENSION) - (ConnectionPointFromY - ARROW_DIMENSION) ;

			if(sqrt(DistanceX * DistanceX + DistanceY * DistanceY) > 0)
			{
				TempPolygon.append(QPointF(0, 0));
				TempPolygon.append(QPointF(sqrt(DistanceX * DistanceX + DistanceY * DistanceY) , 0));
				TempPolygon.append(QPointF(sqrt(DistanceX * DistanceX + DistanceY * DistanceY), ARROW_DIMENSION / 2));
				TempPolygon.append(QPointF(0 , ARROW_DIMENSION / 2));
				ConnectionBodyBoundingRect->setPolygon(TempPolygon);
				ConnectionBodyBoundingRect->rotate(Angle - OldAngle);

				if (Angle >= 0 )
					ConnectionBodyBoundingRect->setPos(ConnectionPointToX + ARROW_DIMENSION + ARROW_DIMENSION / 4 , ConnectionPointToY - ARROW_DIMENSION / 4);
				else if (Angle <= 0  )
					ConnectionBodyBoundingRect->setPos(ConnectionPointToX + ARROW_DIMENSION - ARROW_DIMENSION / 4 , ConnectionPointToY - ARROW_DIMENSION / 4);


				OldAngle = Angle;
			}
		}
		else if (ConnectionPointFromX < ConnectionPointToX && fabs(ConnectionPointFromX - From->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD && fabs(ConnectionPointToX - To->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD)
		{
			float Angle = atan((ConnectionPointToY - ConnectionPointFromY) / (ConnectionPointToX - ConnectionPointFromX )) * 180 / PI;
			float M = (ConnectionPointToY - ConnectionPointFromY) / (ConnectionPointToX - ConnectionPointFromX );
			float DistanceX =  (ConnectionPointFromX - ARROW_DIMENSION) - (ConnectionPointToX + ARROW_DIMENSION);
			float DistanceY = (ConnectionPointToY - ARROW_DIMENSION) - (ConnectionPointFromY - ARROW_DIMENSION) ;

			if(sqrt(DistanceX * DistanceX + DistanceY * DistanceY) > 0)
			{
				TempPolygon.append(QPointF(0, 0));
				TempPolygon.append(QPointF(sqrt(DistanceX * DistanceX + DistanceY * DistanceY) , 0));
				TempPolygon.append(QPointF(sqrt(DistanceX * DistanceX + DistanceY * DistanceY), ARROW_DIMENSION / 2));
				TempPolygon.append(QPointF(0 , ARROW_DIMENSION / 2));
				ConnectionBodyBoundingRect->setPolygon(TempPolygon);
				ConnectionBodyBoundingRect->rotate(Angle - OldAngle);

				if (Angle >= 0 )
					ConnectionBodyBoundingRect->setPos(ConnectionPointFromX + ARROW_DIMENSION + ARROW_DIMENSION / 4 , ConnectionPointFromY - ARROW_DIMENSION / 4);
				else if (Angle <= 0  )
					ConnectionBodyBoundingRect->setPos(ConnectionPointFromX + ARROW_DIMENSION - ARROW_DIMENSION / 4 , ConnectionPointFromY - ARROW_DIMENSION / 4);

				OldAngle = Angle;	
			}
		}

		TempPolygon.clear();

		if (IsDirected == true)
		{
			if (fabs(ConnectionPointToX - To->sceneBoundingRect().topLeft().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(0, 0));
				TempPolygon.append(QPointF(0, ARROW_DIMENSION));
				TempPolygon.append(QPointF(-ARROW_DIMENSION - 2, ARROW_DIMENSION));
				TempPolygon.append(QPointF(-ARROW_DIMENSION - 2, 0));
				ConnectionToBoundingRect->setPolygon(TempPolygon);	
				ConnectionToBoundingRect->setPos(ConnectionPointToX, (To->sceneBoundingRect().height() - ARROW_DIMENSION) / 2 + To->sceneBoundingRect().topLeft().y());
			}
			else if (fabs(ConnectionPointToX - To->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(0, 0));
				TempPolygon.append(QPointF(0, ARROW_DIMENSION));
				TempPolygon.append(QPointF(ARROW_DIMENSION + 2, ARROW_DIMENSION));
				TempPolygon.append(QPointF(ARROW_DIMENSION + 2, 0));
				ConnectionToBoundingRect->setPolygon(TempPolygon);	
				ConnectionToBoundingRect->setPos(ConnectionPointToX, (From->sceneBoundingRect().height() - ARROW_DIMENSION) / 2 + To->sceneBoundingRect().topLeft().y());
			}
		}
		else if (IsDirected == false)
		{
			if (fabs(ConnectionPointToX - To->sceneBoundingRect().topLeft().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(0, 0));
				TempPolygon.append(QPointF(0, ARROW_DIMENSION / 2));
				TempPolygon.append(QPointF(-ARROW_DIMENSION - 2, ARROW_DIMENSION / 2));
				TempPolygon.append(QPointF(-ARROW_DIMENSION - 2, 0));
				ConnectionToBoundingRect->setPolygon(TempPolygon);	
				ConnectionToBoundingRect->setPos(ConnectionPointToX, (To->sceneBoundingRect().height() - ARROW_DIMENSION / 2) / 2 + To->sceneBoundingRect().topLeft().y());
			}
			else if (fabs(ConnectionPointToX - To->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(0, 0));
				TempPolygon.append(QPointF(0, ARROW_DIMENSION / 2));
				TempPolygon.append(QPointF(ARROW_DIMENSION + 2, ARROW_DIMENSION / 2));
				TempPolygon.append(QPointF(ARROW_DIMENSION + 2, 0));
				ConnectionToBoundingRect->setPolygon(TempPolygon);	
				ConnectionToBoundingRect->setPos(ConnectionPointToX, (To->sceneBoundingRect().height() - ARROW_DIMENSION / 2) / 2 + To->sceneBoundingRect().topLeft().y());
			}
		}

		TempPolygon.clear();
		if (fabs(ConnectionPointFromX - From->sceneBoundingRect().topLeft().x()) < ZE_ZERO_THRESHOLD)
		{
			TempPolygon.append(QPointF(0, 0));
			TempPolygon.append(QPointF(0, ARROW_DIMENSION / 2));
			TempPolygon.append(QPointF(-ARROW_DIMENSION - 2, ARROW_DIMENSION / 2));
			TempPolygon.append(QPointF(-ARROW_DIMENSION - 2, 0));
			ConnectionFromBoundingRect->setPolygon(TempPolygon);	
			ConnectionFromBoundingRect->setPos(ConnectionPointFromX, (From->sceneBoundingRect().height() - ARROW_DIMENSION / 2) / 2 + From->sceneBoundingRect().topLeft().y());
		}
		else if (fabs(ConnectionPointFromX - From->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD)
		{
			TempPolygon.append(QPointF(0, 0));
			TempPolygon.append(QPointF(0, ARROW_DIMENSION / 2));
			TempPolygon.append(QPointF(ARROW_DIMENSION + 2, ARROW_DIMENSION / 2));
			TempPolygon.append(QPointF(ARROW_DIMENSION + 2, 0));
			ConnectionFromBoundingRect->setPolygon(TempPolygon);	
			ConnectionFromBoundingRect->setPos(ConnectionPointFromX, (From->sceneBoundingRect().height() - ARROW_DIMENSION / 2) / 2 + From->sceneBoundingRect().topLeft().y());
		}
	}
	else if (From == NULL && To == NULL)
	{
		QPolygonF TempPolygon;

		float ConnectionPointFromX = FromNode->sceneBoundingRect().topLeft().x() + FromNode->sceneBoundingRect().width() / 2;
		float ConnectionPointFromY = FromNode->sceneBoundingRect().topLeft().y();
		float ConnectionPointToX = ToNode->sceneBoundingRect().topLeft().x() + ToNode->sceneBoundingRect().width() / 2;
		float ConnectionPointToY = ToNode->sceneBoundingRect().topLeft().y();

		if (ConnectionPointToX <= ConnectionPointFromX)
		{
			if (ConnectionPointFromY <= ConnectionPointToY)
			{
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION / 4, ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION / 4, ConnectionPointFromY - ARROW_DIMENSION * 2 + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION / 4, ConnectionPointFromY - ARROW_DIMENSION * 2 + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION / 4, ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION / 4, ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION / 4, ConnectionPointFromY - ARROW_DIMENSION * 2 - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION / 4, ConnectionPointFromY - ARROW_DIMENSION * 2 - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION / 4, ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION / 4, ConnectionPointFromY));
			}
			else if (ConnectionPointFromY > ConnectionPointToY)
			{
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION / 4, ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION / 4, ConnectionPointToY - ARROW_DIMENSION * 2 - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION / 4, ConnectionPointToY - ARROW_DIMENSION * 2 - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION / 4, ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION / 4, ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION / 4, ConnectionPointToY - ARROW_DIMENSION * 2 + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION / 4, ConnectionPointToY - ARROW_DIMENSION * 2 + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION / 4, ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION / 4, ConnectionPointToY));
			}
		}
		else if (ConnectionPointToX > ConnectionPointFromX)
		{
			if (ConnectionPointFromY <= ConnectionPointToY)
			{
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION / 4, ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION / 4, ConnectionPointFromY - ARROW_DIMENSION * 2 - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION / 4, ConnectionPointFromY - ARROW_DIMENSION * 2 - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION / 4, ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION / 4, ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION / 4, ConnectionPointFromY - ARROW_DIMENSION * 2 + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION / 4, ConnectionPointFromY - ARROW_DIMENSION * 2 + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION / 4, ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION / 4, ConnectionPointFromY));

			}
			else if (ConnectionPointFromY > ConnectionPointToY)
			{
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION / 4, ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION / 4, ConnectionPointToY - ARROW_DIMENSION * 2 + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION / 4, ConnectionPointToY - ARROW_DIMENSION * 2 + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION / 4, ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION / 4, ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION / 4, ConnectionPointToY - ARROW_DIMENSION * 2 - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION / 4, ConnectionPointToY - ARROW_DIMENSION * 2 - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION / 4, ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION / 4, ConnectionPointToY));
			}
		}
		ConnectionBodyBoundingRect->setPolygon(TempPolygon);

		TempPolygon.clear();
		if (IsDirected == true)
		{
			TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION / 2, ConnectionPointToY));
			TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION / 2, ConnectionPointToY - ARROW_DIMENSION));
			TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION / 2, ConnectionPointToY - ARROW_DIMENSION));
			TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION / 2, ConnectionPointToY));

			ConnectionToBoundingRect->setPolygon(TempPolygon);
		}
	}
	else if (From == NULL && To != NULL)
	{
		QPolygonF TempPolygon;

		float ConnectionPointFromX = FromNode->sceneBoundingRect().topLeft().x() + FromNode->sceneBoundingRect().width() / 2;
		float ConnectionPointFromY = FromNode->sceneBoundingRect().topLeft().y();
		float ConnectionPointToX = To->sceneBoundingRect().topLeft().x();
		float ConnectionPointToY = To->sceneBoundingRect().topLeft().y() + To->sceneBoundingRect().height() / 2;

		if (ConnectionPointToX + To->sceneBoundingRect().width() / 2 <  ConnectionPointFromX)
		{
			ConnectionPointToX = To->sceneBoundingRect().bottomRight().x();
		}

		if (ConnectionPointFromY - ARROW_DIMENSION * 2 <= ConnectionPointToY)
		{
			if (fabs(ConnectionPointToX - To->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION / 4, ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION / 4, ConnectionPointFromY - ARROW_DIMENSION * 2 + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION * 2 + ARROW_DIMENSION / 4, ConnectionPointFromY - ARROW_DIMENSION * 2 + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION * 2 + ARROW_DIMENSION / 4, ConnectionPointToY + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointToY + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointToY - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION * 2 - ARROW_DIMENSION / 4, ConnectionPointToY - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION * 2 - ARROW_DIMENSION / 4, ConnectionPointFromY - ARROW_DIMENSION * 2 - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION / 4, ConnectionPointFromY - ARROW_DIMENSION * 2 - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION / 4, ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION / 4, ConnectionPointFromY));
			}
			else if (fabs(ConnectionPointToX - To->sceneBoundingRect().topLeft().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION / 4, ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION / 4, ConnectionPointFromY - ARROW_DIMENSION * 2 + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION * 2 - ARROW_DIMENSION / 4, ConnectionPointFromY - ARROW_DIMENSION * 2 + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION * 2 - ARROW_DIMENSION / 4, ConnectionPointToY + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointToY + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointToY - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION * 2 + ARROW_DIMENSION / 4, ConnectionPointToY - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION * 2 + ARROW_DIMENSION / 4, ConnectionPointFromY - ARROW_DIMENSION * 2 - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION / 4, ConnectionPointFromY - ARROW_DIMENSION * 2 - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION / 4, ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION / 4, ConnectionPointFromY));
			}
		}
		else if (ConnectionPointFromY - ARROW_DIMENSION * 2 > ConnectionPointToY)
		{
			if (fabs(ConnectionPointToX - To->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION / 4, ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION / 4, ConnectionPointToY + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX , ConnectionPointToY  + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX , ConnectionPointToY  - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION / 4, ConnectionPointToY - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION / 4, ConnectionPointFromY));
			}
			else if (fabs(ConnectionPointToX - To->sceneBoundingRect().topLeft().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION / 4, ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION / 4, ConnectionPointToY - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX , ConnectionPointToY  - ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointToX , ConnectionPointToY  + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION / 4, ConnectionPointToY + ARROW_DIMENSION / 4));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION / 4, ConnectionPointFromY));

			}
		}

		ConnectionBodyBoundingRect->setPolygon(TempPolygon);

		TempPolygon.clear();

		if (IsDirected == true)
		{
			if (fabs(ConnectionPointToX - To->sceneBoundingRect().topLeft().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointToY - ARROW_DIMENSION / 2));
				TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointToY + ARROW_DIMENSION / 2));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION , ConnectionPointToY + ARROW_DIMENSION / 2));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION , ConnectionPointToY - ARROW_DIMENSION / 2));
				ConnectionToBoundingRect->setPolygon(TempPolygon);	
			}
			else if (fabs(ConnectionPointToX - To->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointToY - ARROW_DIMENSION / 2));
				TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointToY + ARROW_DIMENSION / 2));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION , ConnectionPointToY + ARROW_DIMENSION / 2));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION , ConnectionPointToY - ARROW_DIMENSION / 2));
				ConnectionToBoundingRect->setPolygon(TempPolygon);	
			}
		}

	}
}

void ZEDIOPortConnection::Update()
{
	if (From != NULL && To != NULL)
	{
		QPolygonF TempPolygon;

		float ConnectionPointFromX = From->sceneBoundingRect().topLeft().x();
		float ConnectionPointFromY = From->sceneBoundingRect().topLeft().y() + From->sceneBoundingRect().height() / 2;
		float ConnectionPointToX = To->sceneBoundingRect().topLeft().x();
		float ConnectionPointToY = To->sceneBoundingRect().topLeft().y() + To->sceneBoundingRect().height() / 2;

		if (From->sceneBoundingRect().topLeft().x() <  (To->sceneBoundingRect().topLeft().x() +  To->sceneBoundingRect().width() / 2))
		{
			ConnectionPointFromX = From->sceneBoundingRect().bottomRight().x();
		}
		if (To->sceneBoundingRect().topLeft().x() <  (From->sceneBoundingRect().topLeft().x() +  From->sceneBoundingRect().width() / 2))
		{
			ConnectionPointToX = To->sceneBoundingRect().bottomRight().x();
		}

		TempPolygon.clear();

		///////////////////////////////////////////Drawing the line ////////////////////////////////////////////////

		if (fabs(ConnectionPointFromX - From->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD)
		{
			if (fabs(ConnectionPointToX - To->sceneBoundingRect().topLeft().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(ConnectionPointFromX , ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION , ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION , ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX , ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION , ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION , ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX , ConnectionPointFromY));
				setPolygon(TempPolygon);
			}
			else if (fabs(ConnectionPointToX - To->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(ConnectionPointFromX , ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION , ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION , ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX , ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION , ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointFromX + ARROW_DIMENSION , ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX , ConnectionPointFromY));
				setPolygon(TempPolygon);
			}


		}
		else if (fabs(ConnectionPointFromX - From->sceneBoundingRect().topLeft().x()) < ZE_ZERO_THRESHOLD)
		{
			if (fabs(ConnectionPointToX - To->sceneBoundingRect().topLeft().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(ConnectionPointFromX , ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION , ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION , ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX , ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION , ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION , ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX , ConnectionPointFromY));
				setPolygon(TempPolygon);
			}
			else if (fabs(ConnectionPointToX - To->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(ConnectionPointFromX , ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION , ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION , ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX , ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION , ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointFromX - ARROW_DIMENSION , ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX , ConnectionPointFromY));
				setPolygon(TempPolygon);
			}
		}


		TempPolygon.clear();



		////////////////////////////// Drawing the arrow ///////////////////////////////////////
		TempPolygon.clear();

		if (IsDirected == true)
		{
			if (fabs(ConnectionPointToX - To->sceneBoundingRect().topLeft().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(0, 0));
				TempPolygon.append(QPointF((0 - ARROW_DIMENSION), - (0 - ARROW_DIMENSION / 2)));
				TempPolygon.append(QPointF( (0 - ARROW_DIMENSION),- (0 + ARROW_DIMENSION / 2)));
				TempPolygon.append(QPointF(0, 0));
			}
			else if (fabs(ConnectionPointToX - To->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(0, 0));
				TempPolygon.append(QPointF((0 + ARROW_DIMENSION), - (0 + ARROW_DIMENSION / 2)));
				TempPolygon.append(QPointF( (0 + ARROW_DIMENSION),- (0 - ARROW_DIMENSION / 2)));
				TempPolygon.append(QPointF(0, 0));
			}
			ArrowPolygon->setPolygon(TempPolygon);	

			ArrowPolygon->setPos(ConnectionPointToX, ConnectionPointToY);

		}
		else if (IsDirected == false)
		{

		}
	}
	else if (From == NULL && To == NULL)
	{
		QPolygonF TempPolygon;

		float ConnectionPointFromX = FromNode->sceneBoundingRect().topLeft().x() + FromNode->sceneBoundingRect().width() / 2;
		float ConnectionPointFromY = FromNode->sceneBoundingRect().topLeft().y();
		float ConnectionPointToX = ToNode->sceneBoundingRect().topLeft().x() + ToNode->sceneBoundingRect().width() / 2;
		float ConnectionPointToY = ToNode->sceneBoundingRect().topLeft().y();

		if (ConnectionPointFromY <= ConnectionPointToY)
		{
			TempPolygon.append(QPointF(ConnectionPointFromX, ConnectionPointFromY));
			TempPolygon.append(QPointF(ConnectionPointFromX, ConnectionPointFromY - ARROW_DIMENSION * 2));
			TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointFromY - ARROW_DIMENSION * 2));
			TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointToY));

			TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointFromY - ARROW_DIMENSION * 2));
			TempPolygon.append(QPointF(ConnectionPointFromX, ConnectionPointFromY - ARROW_DIMENSION * 2));
			TempPolygon.append(QPointF(ConnectionPointFromX, ConnectionPointFromY));
		}
		else if (ConnectionPointFromY > ConnectionPointToY)
		{
			TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointToY));	
			TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointToY - ARROW_DIMENSION * 2));
			TempPolygon.append(QPointF(ConnectionPointFromX, ConnectionPointToY - ARROW_DIMENSION * 2));
			TempPolygon.append(QPointF(ConnectionPointFromX, ConnectionPointFromY));

			TempPolygon.append(QPointF(ConnectionPointFromX, ConnectionPointToY - ARROW_DIMENSION * 2));
			TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointToY - ARROW_DIMENSION * 2));
			TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointToY));
		}

		setPolygon(TempPolygon);

		TempPolygon.clear();

		if (IsDirected == true)
		{
			TempPolygon.append(QPointF(0, 0));
			TempPolygon.append(QPointF(-ARROW_DIMENSION / 2,- ARROW_DIMENSION));
			TempPolygon.append(QPointF(ARROW_DIMENSION / 2, - ARROW_DIMENSION));
			TempPolygon.append(QPointF(0, 0));

			ArrowPolygon->setPolygon(TempPolygon);	

			ArrowPolygon->setPos(ConnectionPointToX, ConnectionPointToY);

		}


	}
	else if (From == NULL && To != NULL)
	{
		QPolygonF TempPolygon;

		float ConnectionPointFromX = FromNode->sceneBoundingRect().topLeft().x() + FromNode->sceneBoundingRect().width() / 2;
		float ConnectionPointFromY = FromNode->sceneBoundingRect().topLeft().y();
		float ConnectionPointToX = To->sceneBoundingRect().topLeft().x();
		float ConnectionPointToY = To->sceneBoundingRect().topLeft().y() + To->sceneBoundingRect().height() / 2;

		if (ConnectionPointToX + To->sceneBoundingRect().width() / 2 <  ConnectionPointFromX)
		{
			ConnectionPointToX = To->sceneBoundingRect().bottomRight().x();
		}

		if (ConnectionPointFromY - ARROW_DIMENSION * 2 <= ConnectionPointToY)
		{
			if (fabs(ConnectionPointToX - To->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(ConnectionPointFromX, ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX, ConnectionPointFromY - ARROW_DIMENSION * 2));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION * 2, ConnectionPointFromY - ARROW_DIMENSION * 2));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION * 2, ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointToY));

				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION * 2, ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX + ARROW_DIMENSION * 2, ConnectionPointFromY - ARROW_DIMENSION * 2));
				TempPolygon.append(QPointF(ConnectionPointFromX , ConnectionPointFromY - ARROW_DIMENSION * 2));
				TempPolygon.append(QPointF(ConnectionPointFromX, ConnectionPointFromY));
			}
			else if (fabs(ConnectionPointToX - To->sceneBoundingRect().topLeft().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(ConnectionPointFromX, ConnectionPointFromY));
				TempPolygon.append(QPointF(ConnectionPointFromX, ConnectionPointFromY - ARROW_DIMENSION * 2));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION * 2, ConnectionPointFromY - ARROW_DIMENSION * 2));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION * 2, ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointToY));

				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION * 2, ConnectionPointToY));
				TempPolygon.append(QPointF(ConnectionPointToX - ARROW_DIMENSION * 2, ConnectionPointFromY - ARROW_DIMENSION * 2));
				TempPolygon.append(QPointF(ConnectionPointFromX , ConnectionPointFromY - ARROW_DIMENSION * 2));
				TempPolygon.append(QPointF(ConnectionPointFromX, ConnectionPointFromY));
			}
		}
		else if (ConnectionPointFromY - ARROW_DIMENSION * 2 > ConnectionPointToY)
		{
			TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointToY));	
			TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointToY));
			TempPolygon.append(QPointF(ConnectionPointFromX, ConnectionPointToY));
			TempPolygon.append(QPointF(ConnectionPointFromX, ConnectionPointFromY));

			TempPolygon.append(QPointF(ConnectionPointFromX, ConnectionPointToY));
			TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointToY));
			TempPolygon.append(QPointF(ConnectionPointToX, ConnectionPointToY));
		}

		setPolygon(TempPolygon);

		TempPolygon.clear();

		if (IsDirected == true)
		{
			if (fabs(ConnectionPointToX - To->sceneBoundingRect().topLeft().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(0, 0));
				TempPolygon.append(QPointF((0 - ARROW_DIMENSION), - (0 - ARROW_DIMENSION / 2)));
				TempPolygon.append(QPointF( (0 - ARROW_DIMENSION),- (0 + ARROW_DIMENSION / 2)));
				TempPolygon.append(QPointF(0, 0));
			}
			else if (fabs(ConnectionPointToX - To->sceneBoundingRect().bottomRight().x()) < ZE_ZERO_THRESHOLD)
			{
				TempPolygon.append(QPointF(0, 0));
				TempPolygon.append(QPointF((0 + ARROW_DIMENSION), - (0 + ARROW_DIMENSION / 2)));
				TempPolygon.append(QPointF( (0 + ARROW_DIMENSION),- (0 - ARROW_DIMENSION / 2)));
				TempPolygon.append(QPointF(0, 0));
			}
			ArrowPolygon->setPolygon(TempPolygon);	

			ArrowPolygon->setPos(ConnectionPointToX, ConnectionPointToY);

		}
	}
}

QGraphicsPolygonItem* ZEDIOPortConnection::GetDirectionArrow(QPointF MousePosPoint)
{
	if (From == NULL)
	{
		QPolygonF TempPolygon;
		float ConnectionPointFromNodeX = FromNode->sceneBoundingRect().topLeft().x() + FromNode->sceneBoundingRect().width() / 2;
		float ConnectionPointFromNodeY = FromNode->sceneBoundingRect().topLeft().y();
		ZEInt Angle;
		float M = (MousePosPoint.y() - ConnectionPointFromNodeY) / (MousePosPoint.x() - ConnectionPointFromNodeX);

		if (MousePosPoint.x() < ConnectionPointFromNodeX)
		{
			TempPolygon.append(QPointF(0, 0));
			TempPolygon.append(QPointF((0 + ARROW_DIMENSION), - (0 + ARROW_DIMENSION / 2)));
			TempPolygon.append(QPointF((0 + ARROW_DIMENSION),- (0 - ARROW_DIMENSION / 2)));
			TempPolygon.append(QPointF(0, 0));
			Angle = atan((MousePosPoint.y() - ConnectionPointFromNodeY) / (MousePosPoint.x() - ConnectionPointFromNodeX)) * 180 / PI;
		}
		else if (MousePosPoint.x() >= ConnectionPointFromNodeX)
		{
			TempPolygon.append(QPointF(0, 0));
			TempPolygon.append(QPointF((0 - ARROW_DIMENSION), - (0 - ARROW_DIMENSION / 2)));
			TempPolygon.append(QPointF((0 - ARROW_DIMENSION),- (0 + ARROW_DIMENSION / 2)));
			TempPolygon.append(QPointF(0, 0));
			Angle = atan((MousePosPoint.y() - ConnectionPointFromNodeY) / (MousePosPoint.x() - ConnectionPointFromNodeX)) * 180 / PI;
		}


		ArrowPolygon->setBrush(Qt::black);
		ArrowPolygon->setPolygon(TempPolygon);	

		ArrowPolygon->rotate(Angle - OldAngle);

		ArrowPolygon->setPos(MousePosPoint.x(), MousePosPoint.y());
		OldAngle = Angle;
	}
	else
	{
		QPolygonF TempPolygon;
		float ConnectionPointFromXLeft = From->sceneBoundingRect().topLeft().x() - From->sceneBoundingRect().height() / 4;
		float ConnectionPointFromYLeft = From->sceneBoundingRect().topLeft().y() + From->sceneBoundingRect().height() / 2;
		float ConnectionPointFromXRight = From->sceneBoundingRect().bottomRight().x() + From->sceneBoundingRect().height() / 4;
		float ConnectionPointFromYRight = From->sceneBoundingRect().bottomRight().y() - From->sceneBoundingRect().height() / 2;
		ZEInt Angle;
		float M = (MousePosPoint.y() - ConnectionPointFromYLeft) / (MousePosPoint.x() - ConnectionPointFromXLeft);

		if (MousePosPoint.x() >=  (ConnectionPointFromXLeft)  && MousePosPoint.x() < ConnectionPointFromXLeft + From->sceneBoundingRect().width() / 2)
		{
			TempPolygon.append(QPointF(0, 0));
			TempPolygon.append(QPointF((0 - ARROW_DIMENSION), - (0 - ARROW_DIMENSION / 2)));
			TempPolygon.append(QPointF((0 - ARROW_DIMENSION),- (0 + ARROW_DIMENSION / 2)));
			TempPolygon.append(QPointF(0, 0));
			Angle = atan((MousePosPoint.y() - ConnectionPointFromYLeft) / (MousePosPoint.x() - ConnectionPointFromXLeft)) * 180 / PI;
		}
		else if (MousePosPoint.x() <  (ConnectionPointFromXLeft))
		{
			TempPolygon.append(QPointF(0, 0));
			TempPolygon.append(QPointF((0 + ARROW_DIMENSION), - (0 + ARROW_DIMENSION / 2)));
			TempPolygon.append(QPointF((0 + ARROW_DIMENSION),- (0 - ARROW_DIMENSION / 2)));
			TempPolygon.append(QPointF(0, 0));
			Angle = atan((MousePosPoint.y() - ConnectionPointFromYLeft) / (MousePosPoint.x() - ConnectionPointFromXLeft)) * 180 / PI;
		}
		else if (MousePosPoint.x() >=  (ConnectionPointFromXRight))
		{
			TempPolygon.append(QPointF(0, 0));
			TempPolygon.append(QPointF((0 - ARROW_DIMENSION), - (0 - ARROW_DIMENSION / 2)));
			TempPolygon.append(QPointF((0 - ARROW_DIMENSION),- (0 + ARROW_DIMENSION / 2)));
			TempPolygon.append(QPointF(0, 0));
			Angle = atan((MousePosPoint.y() - ConnectionPointFromYRight) / (MousePosPoint.x() - ConnectionPointFromXRight)) * 180 / PI;
		}
		else if (MousePosPoint.x() <  (ConnectionPointFromXRight))
		{
			TempPolygon.append(QPointF(0, 0));
			TempPolygon.append(QPointF((0 + ARROW_DIMENSION), - (0 + ARROW_DIMENSION / 2)));
			TempPolygon.append(QPointF((0 + ARROW_DIMENSION),- (0 - ARROW_DIMENSION / 2)));
			TempPolygon.append(QPointF(0, 0));
			Angle = atan((MousePosPoint.y() - ConnectionPointFromYRight) / (MousePosPoint.x() - ConnectionPointFromXRight)) * 180 / PI;
		}

		ArrowPolygon->setBrush(Qt::black);
		ArrowPolygon->setPolygon(TempPolygon);	

		ArrowPolygon->rotate(Angle - OldAngle);

		ArrowPolygon->setPos(MousePosPoint.x(), MousePosPoint.y());
		OldAngle = Angle;
	}
	return ArrowPolygon;
}

void ZEDIOPortConnection::SetSelected(bool IsSelected)
{
	if(IsSelected)
	{
		QPen MagentaPen(Qt::magenta);
		MagentaPen.setWidth(0.5);
		setPen(MagentaPen);
		ArrowPolygon->setBrush(Qt::magenta);
		ArrowPolygon->setPen(MagentaPen);
	}
	else
	{
		QPen BlackPen(Qt::black);
		BlackPen.setWidth(0.5);
		setPen(BlackPen);	
		ArrowPolygon->setBrush(Qt::black);
		ArrowPolygon->setPen(BlackPen);
	}

	ZEDNodeEditorItem::SetSelected(IsSelected);
}

ZEDNodeEditorItemType ZEDIOPortConnection::GetItemType()
{
	return ZED_NEIT_CONNECTION;
}

void ZEDIOPortConnection::SetFrom(ZEDNodeIOPort* From)
{
	 this->From = From;
} 
ZEDNodeIOPort* ZEDIOPortConnection::GetFrom() const
{
	return From;
}

void ZEDIOPortConnection::SetTo(ZEDNodeIOPort* To)
{
	this->To = To;
}

ZEDNodeIOPort* ZEDIOPortConnection::GetTo() const
{
	return To;
}

void ZEDIOPortConnection::SetFromNode(ZEDNodeEditorNode* FromNode)
{
	this->FromNode = FromNode;
}

ZEDNodeEditorNode* ZEDIOPortConnection::GetFromNode() const
{
	return FromNode;
}

void ZEDIOPortConnection::SetToNode(ZEDNodeEditorNode* ToNode)
{
	this->ToNode = ToNode;
}

ZEDNodeEditorNode* ZEDIOPortConnection::GetToNode() const
{
	return ToNode;
}

void ZEDIOPortConnection::SetDirected(bool IsDirected)
{
	this->IsDirected = IsDirected;
}

bool ZEDIOPortConnection::GetDirected() const
{
	return IsDirected;
}

void ZEDIOPortConnection::SetUserData(void* UserData)
{
	this->UserData = UserData;
}

void* ZEDIOPortConnection::GetUserData() const
{
	return UserData;
}

ZEDIOPortConnection::ZEDIOPortConnection(QGraphicsItem* Parent) : QGraphicsPolygonItem (Parent)
{
	From = NULL;
	To = NULL;
	FromNode = NULL;
	ToNode = NULL;

	OldAngle = 0.0f;
	OldAngle2 = 0.0f;

	IsDirected = true;

	ArrowPolygon				= new QGraphicsPolygonItem(this);
	ConnectionBodyBoundingRect	= new QGraphicsPolygonItem(this);
	ConnectionToBoundingRect	= new QGraphicsPolygonItem(this);
	ConnectionFromBoundingRect	= new QGraphicsPolygonItem(this);

	ConnectionBodyBoundingRect->setOpacity(0.0f);
	ConnectionToBoundingRect->setOpacity(0.0f);
	ConnectionFromBoundingRect->setOpacity(0.0f);

	ConnectionBodyBoundingRect->setCursor(Qt::CrossCursor);
	ConnectionToBoundingRect->setCursor(Qt::CrossCursor);
	ConnectionFromBoundingRect->setCursor(Qt::CrossCursor);

	ArrowPolygon->setBrush(Qt::black);
	QPen BlackPen(Qt::black);
	BlackPen.setWidth(0.5);
	setPen(BlackPen);	
}
