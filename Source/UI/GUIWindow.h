//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - GUIWindow.h
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

#ifndef __GUI_WINDOW__
#define __GUI_WINDOW__

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <gl/glut.h>
#include <iostream>
#include "ZELinkedList.h"
#include "ZEGUIComponent.h"
#include "ZEGUIEngine.h"
#include "ZEGUIConstantDefinitions.h"
#include "ZEGUIDragButton.h"
#include "ZEGUIButton.h"

class ZEGUIComponent;
class ZEGUIEngine;

class ZEGUIWindow : public ZEGUIComponent
{
private:
	float Color[4];

protected:

	ZEGUIComponent *HoveredChild;

	void (*InMouseMotion) (float x,float y);
	void (*InMouseAction) (unsigned char button,bool state);
	void (*InKeyboardAction) (unsigned char key,bool state);
	void (*InHoverExited) (void);
	void (*InFocusExited) (void);

	ZELinkedList <ZEGUIComponent> ChildrenDepths;

public:
	ZEGUIWindow();
	ZEGUIWindow(float newX0,float newX1,float newY0,float newY1);
	~ZEGUIWindow();

	virtual void SetPositions(float newX0,float newX1,float newY0,float newY1,unsigned char relative = 1);

	virtual void DrawSelf();
	virtual void Draw();

	void AddChild(ZEGUIComponent *);
	void DeleteChild(ZEGUIComponent *);
	void FocusChild(ZEGUIComponent *);
	ZEGUIComponent *GetHoveredChild();
	ZEGUIComponent *GetFocusedChild();
	void AddDragButton();

	virtual bool MouseMotionEvent(float mx,float my);
	virtual bool MouseActionEvent(unsigned char  actionType);
	virtual bool KeyboardEvent(unsigned char key,bool actionType);
	virtual void HoverExited();
	virtual void FocusExited();

	void setMouseMotionLink(void (*linkedFunction)(float,float));
	void setMouseActionLink(void (*linkedFunction)(unsigned char,bool));
	void setKeyboardActionLink(void (*linkedFunction)(unsigned char,bool));
	void setHoverExitedLink(void (linkedFunction)(void));
	void setFocusExitedLink(void (linkedFunction)(void));

	void SetColor(float,float,float,float);
};

#endif
