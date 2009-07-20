//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - GUIComponent.cpp
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

#include "ZEGUIComponent.h"

using namespace std;

ZEGUIComponent::ZEGUIComponent()
{
	FocusState = 0;
	Parent = NULL;
	Engine = NULL;

	Positions[0] = 0;
	Positions[1] = 0;
	Positions[2] = 0;
	Positions[3] = 0;
}

ZEGUIComponent::~ZEGUIComponent()
{}

void ZEGUIComponent::SetParent(ZEGUIWindow *parentName,ZEGUIEngine *engineName)
{
	Parent = parentName;
	Engine = engineName;
}

ZEGUIWindow *ZEGUIComponent::GetParent()
{
	return Parent;
}

float *ZEGUIComponent::GetPositions()
{
	return Positions;
}

void ZEGUIComponent::SetPositions(float newX0,float newX1,float newY0,float newY1,unsigned char relative)
{
	if (relative)
	{
		Positions[0] += newX0;
		Positions[1] += newX1;
		Positions[2] += newY0;
		Positions[3] += newY1;
	}
	//else
	//{
	//	float *parentSizes = parent->getPositions();
	//	float parentX = parentSizes[1] - parentSizes[0];
	//	float parentY = parentSizes[3] - parentSizes[2];

	//	positions[0] = parentSizes[0] + parentX * newX0;
	//	positions[1] = parentSizes[0] + parentX * newX1;
	//	positions[2] = parentSizes[2] + parentY * newY0;
	//	positions[3] = parentSizes[2] + parentY * newY1;
	//}
}

void ZEGUIComponent::SetVisible(bool state)
{
	VisibleState = state;
}

bool ZEGUIComponent::IsVisible()
{
	return VisibleState;
}

void ZEGUIComponent::Draw()
{
//	drawFunction(positions,0);
}

void ZEGUIComponent::SetDrawFunctionLink(void (*linkedFunction)(float *,unsigned int))
{
//	drawFunction = linkedFunction;
}

bool ZEGUIComponent::IsMousePositionInside(float mx,float my)
{
	return (( mx >= Positions[0] ) && ( mx <= Positions[1] ) && ( my >= Positions[2] ) && (my <= Positions[3]));
}



//---------------------------------EVENTS-----------------------

bool ZEGUIComponent::MouseMotionEvent(float mx,float my)
{
	if (!IsMousePositionInside(mx,my))
	{
		return 0;
	}
	HoverState = 1;

	return 1;
}

bool ZEGUIComponent::MouseActionEvent(unsigned char actionType)
{
	switch (actionType)
	{
	case ZE_GUI_MOUSE_LEFT_DOWN:
	case ZE_GUI_MOUSE_RIGHT_DOWN:
	case ZE_GUI_MOUSE_MIDDLE_DOWN:
		FocusState = 1;
		return 1;
	default:
		return 0;

	}

	//if ( actionType == GUI_BUTTON_PRESSED ) 
	//{
	//	focusState = 1;

	//	return 1;
	//}

	//return 0;
}

bool ZEGUIComponent::KeyboardEvent(unsigned char key,bool actionType)
{
	return 1;
}

