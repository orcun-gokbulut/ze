//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEAIMainWindow.h
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

#pragma once
#ifndef __ZE_AI_MAIN_WINDOW_H__
#define __ZE_AI_MAIN_WINDOW_H__

#include <QMainWindow>

#include "ZEDS\ZEArray.h"

class QTimer;
class ZEAIActor;

namespace Ui
{
	class ZEAIMainWindow;
}

class QGraphicsScene;
class QListWidgetItem;

class ZEAIMainWindow : public QMainWindow
{
	Q_OBJECT
	
	public:
		ZEArray<ZEAIActor*>		Actors;
		Ui::ZEAIMainWindow*		Form;
		QGraphicsScene*			Scene;
	
		QTimer*					Timer;
		int						Frame;
		int						Time;

		void					UpdateActorProperties(bool Partial = false);
		void					Update();

								ZEAIMainWindow(QWidget *Parent = 0, Qt::WFlags Flags = 0);
								~ZEAIMainWindow();

	public slots:
		void					Timer_TimeOut();
		void					btnStart_Clicked();
		void					btnStop_Clicked();
		void					btnStep_Clicked();
		void					btnReset_Clicked();
		void					btnRandomize_Clicked();
		void					btnAddActor_Clicked();
		void					btnDeleteActor_Clicked();
		void					lstActors_ItemClicked(QListWidgetItem* Item);
		void					World_Changed(const QList<QRectF> & region);
		void					World_SelectionChanged();

		void					txtTickInterval_ValueChanged(int Value);
		void					txtMaxLinearAcceleration_ValueChanged(double Value);
		void					txtRadius_ValueChanged(double Value);
		void					txtMaxAngularAcceleration_ValueChanged(double Value);
		void					txtMaxLinearVelocity_ValueChanged(double Value);
		void					txtMaxAngularVelocity_ValueChanged(double Value);
		void					txtActorName_TextChanged(QString Text);
};

#endif
