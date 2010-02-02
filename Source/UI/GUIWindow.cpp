//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - GUIWindow.cpp
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

#include "ZEGUIWindow.h"

//-----------------------Button&DragButton Actions--------------------------

void DragButtonFunction(ZEGUIComponent *ButtonName,float DifferenceX,float DifferenceY)
{
	ButtonName->GetParent()->SetPositions(DifferenceX,DifferenceX,DifferenceY,DifferenceY);
}

void ExitButtonFunction(ZEGUIComponent *ButtonName)
{
	ButtonName->GetParent()->GetParent()->DeleteChild(ButtonName->GetParent());
}

//-----------------------Button&DragButton Actions--------------------------

ZEGUIWindow::ZEGUIWindow(float newX0,float newX1,float newY0,float newY1)
{
	HoveredChild = NULL;

	InKeyboardAction = NULL;
	InMouseAction = NULL;
	InMouseMotion = NULL;

	Positions[0] = newX0;
	Positions[1] = newX1;
	Positions[2] = newY0;
	Positions[3] = newY1;

	if ((Positions[0] != 0) || (Positions[1] != 1) || (Positions[2] != 0) || (Positions[3] != 1))
	{
		ZEGUIDragButton *Drag = new ZEGUIDragButton();
		Drag->SetPositions(Positions[0] + 0.01,Positions[1] - 0.06,Positions[3] - 0.05,Positions[3] - 0.01);
		Drag->SetDragActionLink(DragButtonFunction);
		AddChild(Drag);

		ZEGUIButton *ExitButton = new ZEGUIButton();
		ExitButton->SetPositions(Positions[1] - 0.05,Positions[1] - 0.01,Positions[3] - 0.05,Positions[3] - 0.01);
		ExitButton->SetClickActionLink(ExitButtonFunction);
		AddChild(ExitButton);
	}
}

ZEGUIWindow::~ZEGUIWindow()
{}

ZEGUIComponent *ZEGUIWindow::GetHoveredChild()
{
	return HoveredChild;
}

ZEGUIComponent *ZEGUIWindow::GetFocusedChild()
{
	ZELinkedList<ZEGUIComponent>::Iterator z = ChildrenDepths.GenerateHeadIterator();

	return z.GetPointed();
}

void ZEGUIWindow::AddChild(ZEGUIComponent *Child)
{
	ZELinkedList<ZEGUIComponent>::Iterator zIterator = ChildrenDepths.GenerateHeadIterator();

	zIterator.AddElementBackward(Child);
	Child->SetParent(this,Engine);
}

void ZEGUIWindow::DeleteChild(ZEGUIComponent *Child)
{
	ZELinkedList<ZEGUIComponent>::Iterator Z = ChildrenDepths.GenerateHeadIterator();

	while (1)
	{
		if (Z.GetPointed() == Child) break;
		Z++;
	}

	Z.DeletePointed();
}

void ZEGUIWindow::FocusChild(ZEGUIComponent *Child)
{
	DeleteChild(Child);

	ZELinkedList<ZEGUIComponent>::Iterator Z = ChildrenDepths.GenerateHeadIterator();
	Z.AddElementBackward(Child);
}

void ZEGUIWindow::Draw()
{
	DrawSelf();

	ZELinkedList<ZEGUIComponent>::Iterator ZIterator = ChildrenDepths.GenerateTailIterator();

	while (1)
	{
		if (ZIterator.IsEnd()) break;
		(ZIterator.GetPointed())->Draw();
		ZIterator--;
	}
}

void ZEGUIWindow::DrawSelf()
{
	if (Parent == NULL) return;

	if (FocusState == 1) glColor4fv(Color);
	else glColor4f(Color[0]/2,Color[1]/2,Color[2]/2,Color[3]/2);

	glBegin(GL_POLYGON);
	glVertex2d(Positions[0],Positions[2]);
	glVertex2d(Positions[1],Positions[2]);
	glVertex2d(Positions[1],Positions[3]);
	glVertex2d(Positions[0],Positions[3]);
	glEnd();

	if (HoverState == 1)glColor4f(0,1,0,1);
	else glColor4f(0,0.2,0,1);

	glBegin(GL_LINE_LOOP);
	glVertex2d(Positions[0],Positions[2]);
	glVertex2d(Positions[1],Positions[2]);
	glVertex2d(Positions[1],Positions[3]);
	glVertex2d(Positions[0],Positions[3]);
	glEnd();
}

void ZEGUIWindow::setMouseMotionLink(void (*LinkedFunction)(float,float))
{
	InMouseMotion = LinkedFunction;
}

void ZEGUIWindow::setMouseActionLink(void (*LinkedFunction)(unsigned char,bool))
{
	InMouseAction = LinkedFunction;
}

void ZEGUIWindow::setKeyboardActionLink(void (*LinkedFunction)(unsigned char,bool))
{
	InKeyboardAction = LinkedFunction;
}

void ZEGUIWindow::setHoverExitedLink(void (*LinkedFunction)(void))
{
	InHoverExited = LinkedFunction;
}

void ZEGUIWindow::setFocusExitedLink(void (*LinkedFunction)(void))
{
	InFocusExited = LinkedFunction;
}

void ZEGUIWindow::SetColor(float r,float g,float b,float a)
{
	Color[0] = r;
	Color[1] = g;
	Color[2] = b;
	Color[3] = a;
}

//-------------------------------------EVENTS-----------------------

void ZEGUIWindow::SetPositions(float x0,float x1,float y0,float y1,unsigned char state)
{
	/*setPositions(x,x,y,y);*/
	if (state)
	{
		Positions[0] += x0;
		Positions[1] += x1;
		Positions[2] += y0;
		Positions[3] += y1;
	}

	ZELinkedList<ZEGUIComponent>::Iterator Z = ChildrenDepths.GenerateHeadIterator();

	int k = 1;
	while (1)
	{
		if(Z.IsEnd()) break;
		Z.GetPointed()->SetPositions(x0,x1,y0,y1);
		Z++;
	}


}

bool ZEGUIWindow::MouseMotionEvent(float mx,float my)
{
	if ( !ZEGUIComponent::MouseMotionEvent(mx,my) ) return 0;
	
	ZEGUIComponent *OldHoveredChild = HoveredChild;

	ZELinkedList<ZEGUIComponent>::Iterator Z = ChildrenDepths.GenerateHeadIterator();

	while (1)
	{
		if (Z.IsEnd()) 
		{
			HoveredChild = NULL;
			break;
		}
		else if ( Z.GetPointed()->MouseMotionEvent(mx,my)) 
		{
			HoveredChild = Z.GetPointed();
			break;
		}
		Z++;
	}

	if (( OldHoveredChild != NULL ) && ( OldHoveredChild != HoveredChild )) OldHoveredChild->HoverExited();

	if ( InMouseMotion != NULL ) (*InMouseMotion)(mx,my);

	return 1;
}


bool ZEGUIWindow::MouseActionEvent(unsigned char  ActionType)
{
	//if ( hoveredChild != NULL )
	//{
	//	if (actionType == GUI_BUTTON_PRESSED)
	//	{
	//		if (hoveredChild->mouseActionEvent(buttonName,actionType))
	//		{
	//			if (childrenDepths.getSize() > 1)
	//			{
	//				ZELinkedList<ZEGUIComponent>::Iterator z = childrenDepths.generateHeadIterator();
	//				while (1)
	//				{
	//					if ( z.getPointed() == hoveredChild ) break;
	//					z++;
	//				}

	//				z.deletePointed();
	//				z = childrenDepths.generateHeadIterator();
	//				z.getPointed()->focusExited();
	//				z.addElementBackward(hoveredChild);
	//			}
	//		}
	//	}
	//	else
	//	{
	//		ZELinkedList<ZEGUIComponent>::Iterator z = childrenDepths.generateHeadIterator();
	//		z.getPointed()->mouseActionEvent(buttonName,actionType);
	//	}
	//}

	//if ( inMouseAction != NULL ) (*inMouseAction)(buttonName,actionType);

	//return ZEGUIComponent::mouseActionEvent(buttonName,actionType);
	switch (ActionType)
	{
	case ZE_GUI_MOUSE_LEFT_DOWN:
	case ZE_GUI_MOUSE_RIGHT_DOWN:
	case ZE_GUI_MOUSE_MIDDLE_DOWN:
		FocusState = 1;
		if ((HoveredChild != NULL)&&(HoveredChild->MouseActionEvent(ActionType)))
		{
			if (ChildrenDepths.GetSize() > 1)
			{
				ZELinkedList<ZEGUIComponent>::Iterator Z = ChildrenDepths.GenerateHeadIterator();
				while (1)
				{
					if ( Z.GetPointed() == HoveredChild ) break;
					Z++;
				}

				Z.DeletePointed();
				Z = ChildrenDepths.GenerateHeadIterator();
				Z.GetPointed()->FocusExited();
				Z.AddElementBackward(HoveredChild);
			}
		}
		return 1;
	default:
		ZELinkedList<ZEGUIComponent>::Iterator Z = ChildrenDepths.GenerateHeadIterator();
		if(!Z.IsEnd()) Z.GetPointed()->MouseActionEvent(ActionType);
		return 1;

	}
}

bool ZEGUIWindow::KeyboardEvent(unsigned char Key,bool ActionType)
{
	return 1;
}

void ZEGUIWindow::HoverExited()
{
	HoverState = 0;

	if ( HoveredChild != NULL ) HoveredChild->HoverExited();
}

void ZEGUIWindow::FocusExited()
{
	FocusState = 0;

	ZELinkedList<ZEGUIComponent>::Iterator Z = ChildrenDepths.GenerateHeadIterator();

	if (!Z.IsEnd()) Z.GetPointed()->FocusExited();
}
