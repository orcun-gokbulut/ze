//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - GUIDragButton.cpp
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

#include "ZEGUIDragButton.h"

ZEGUIDragButton::ZEGUIDragButton()
{}

ZEGUIDragButton::~ZEGUIDragButton()
{}

void ZEGUIDragButton::SetDragActionLink(void (*LinkedFunction)(ZEGUIComponent *,float,float))
{
	InDragAction = LinkedFunction;
}

bool ZEGUIDragButton::MouseActionEvent(unsigned char ActionType)
{
	//if (actionType == GUI_BUTTON_PRESSED)
	//{
	//	clickState = 1;
	//}
	//else 
	//{
	//	clickState = 0;
	//}

	//return 1;

	if (ZEGUIComponent::MouseActionEvent(ActionType))
	{
		ClickState = 1;
		return 1;
	}

	ClickState = 0;
	return 1;
}

bool ZEGUIDragButton::MouseMotionEvent(float Mx,float My)
{
	if (ClickState == 1)
	{
		//Parent->SetPositions(Mx - OldX,Mx - OldX, My - OldY,My - OldY);
		if (InDragAction != NULL) InDragAction(this,Mx - OldX,My - OldY);
		OldX = Mx;
		OldY = My;

		return 1;
	}
	else
	{
		OldX = Mx;
		OldY = My;

		if (!ZEGUIComponent::MouseMotionEvent(Mx,My)) return 0;
		return 1;
	}
}

void ZEGUIDragButton::Draw()
{
	if (ClickState == 1)glColor4f(1,0,1,0);
	else glColor4f(0.5,0,0.5,0);

	glBegin(GL_LINE_LOOP);
	glVertex2d(Positions[0],Positions[2]);
	glVertex2d(Positions[1],Positions[2]);
	glVertex2d(Positions[1],Positions[3]);
	glVertex2d(Positions[0],Positions[3]);
	glEnd();
}
