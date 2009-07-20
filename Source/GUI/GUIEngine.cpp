//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - GUIEngine.cpp
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

#include "ZEGUIEngine.h"

//-------------------------------------------------------------------------
//-------------------------ZEGUIEngine functions-----------------------------
//-------------------------------------------------------------------------

ZEGUIEngine::ZEGUIEngine()
{

}

ZEGUIEngine::~ZEGUIEngine()
{
}

void ZEGUIEngine::AdjustSreenSize(int x,int y)
{
	ScreenSizeX = x;
	ScreenSizeY = y;
	glLineWidth(2);
}

void ZEGUIEngine::Draw()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glLoadIdentity();
	glOrtho(0,1,0,1,-5,5);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	MainWindow->Draw();
	
	glMatrixMode(GL_PROJECTION);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}

ZEGUIWindow *ZEGUIEngine::GetMainWindow()
{
	return MainWindow;
}

void ZEGUIEngine::SetMainWindow(ZEGUIWindow *main)
{
	MainWindow = main;
	MainWindow->SetParent(NULL,this);
}

void ZEGUIEngine::InputEventHandler(unsigned int eventType,float data0,float data1)
{
	if (eventType == ZE_GUI_MOUSE_MOTION)
	{
		data0 = data0 / ScreenSizeX;
		data1 = (ScreenSizeY - data1) / ScreenSizeY;

		MainWindow->MouseMotionEvent(data0,data1);
			
	}
	else if (eventType == ZE_GUI_MOUSE_ACTION)
	{
		data0 = data0 * 3 + !data1;
		MainWindow->MouseActionEvent(data0);
	}
	else
	{
		MainWindow->KeyboardEvent(data0,data1);
	}
}
