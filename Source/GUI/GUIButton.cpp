//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - GUIButton.cpp
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

#include "ZEGUIButton.h"

ZEGUIButton::ZEGUIButton()
{
	ClickState = 0;
	InClickAction = NULL;
}

ZEGUIButton::~ZEGUIButton()
{}

void ZEGUIButton::SetClickActionLink(void (*linkedFunction)(ZEGUIComponent *))
{
	InClickAction = linkedFunction;
}

void ZEGUIButton::Draw()
{
	if (ClickState != 1) glColor4f(1,1,0,0);
	else glColor4f(0.5,0.5,0,0);

	glBegin(GL_POLYGON);
	glVertex2d(Positions[0],Positions[2]);
	glVertex2d(Positions[1],Positions[2]);
	glVertex2d(Positions[1],Positions[3]);
	glVertex2d(Positions[0],Positions[3]);
	glEnd();

	if (HoverState == 1)glColor4f(0,1,1,0);
	else glColor4f(0,0.5,0.5,0);

	glBegin(GL_LINE_LOOP);
	glVertex2d(Positions[0],Positions[2]);
	glVertex2d(Positions[1],Positions[2]);
	glVertex2d(Positions[1],Positions[3]);
	glVertex2d(Positions[0],Positions[3]);
	glEnd();
}

bool ZEGUIButton::MouseActionEvent(unsigned char ActionType)
{
	//if (buttonName == GUI_MOUSE_LEFT_BUTTON)
	//{
	//	if (actionType == GUI_BUTTON_PRESSED)
	//	{
	//		clickState = 1;
	//		return 1;
	//	}

	//	clickState = 0;
	//	
	//	//parent->repositionEvent(0.01,0);
	//	return 1;
	//}
	switch (ActionType)
	{
	case ZE_GUI_MOUSE_LEFT_DOWN:
		ClickState = 1;
	case ZE_GUI_MOUSE_RIGHT_DOWN:
	case ZE_GUI_MOUSE_MIDDLE_DOWN:
		FocusState = 1;
		return 1;
	case ZE_GUI_MOUSE_LEFT_UP:
		if (ClickState == 1 ) 
		{
			ClickState = 0;
			if (InClickAction != NULL) InClickAction(this);
		}
	default:
		return 0;

	}
}
